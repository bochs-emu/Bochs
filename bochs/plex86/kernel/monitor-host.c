/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  monitor-host.c:  This file contains the top-level monitor code,
 *    accessible from the host space. (kernel independent code)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
#define IN_HOST_SPACE
#include "monitor.h"


/* =====================================================================
 * Plex86 module global variables.  This should be the _only_ place
 * where globals are declared.  Since plex86 supports multiple VMs, almost
 * all data is stored per-VM.  For the few variables which are global
 * to all VMs, we have to be careful to access them in SMP friendly ways.
 * The ones which are written upon kernel module initialization are fine,
 * since they are only written once.
 * =====================================================================
 */

/* Info regarding the physical pages that comprise the kernel module,
 * including physical page information.  This is written (once) at
 * kernel module initialization time.  Thus there are no SMP access issues.
 */
kernelModulePages_t kernelModulePages;

/* Information of the host processor as returned by the CPUID
 * instruction.  This is written (once) at kernel module initialization time.
 * Thus there no are SMP access issues.
 */
cpuid_info_t hostCpuIDInfo;


/* Some constants used by the VM logic.  Since they're "const", there are
 * no problems with SMP access.
 */
static const selector_t nullSelector = { raw: 0 };
static const descriptor_t nullDescriptor = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  };




static int  hostInitIDTSlot(vm_t *vm, unsigned vec, int type);
static void hostMapMonPages(vm_t *vm, Bit32u *, unsigned, Bit32u *, page_t *,
                            unsigned user, unsigned writable, char *name);
#if ANAL_CHECKS
static void hostMapBlankPage(vm_t *vm, Bit32u *laddr_p, page_t *pageTable);
#endif

#define RW0 0
#define RW1 1
#define US0 0
#define US1 1

#define IDT_INTERRUPT          0
#define IDT_EXCEPTION_ERROR    1
#define IDT_EXCEPTION_NOERROR  2





  unsigned
hostModuleInit(void)
{
  /* Kernel independent stuff to do at kernel module load time. */

  if (!hostGetCpuCapabilities()) {
    hostOSPrint("getCpuCapabilities returned error\n");
    return(0); /* Fail. */
    }
  else {
#if 0
    hostOSPrint("ptype:%u, family:%u, model:%u stepping:%u\n",
        hostCpuIDInfo.procSignature.fields.procType,
        hostCpuIDInfo.procSignature.fields.family,
        hostCpuIDInfo.procSignature.fields.model,
        hostCpuIDInfo.procSignature.fields.stepping);
#endif
    }

  /* xxx Should check that host CS.base is page aligned here. */

#if 1
  {
  Bit32u cr0;

  asm volatile ( "movl %%cr0, %0" : "=r" (cr0) );
  hostOSPrint("host CR0=0x%x\n", cr0);
  }
#endif

  return(1); /* Pass. */
}

  void
hostDeviceOpen(vm_t *vm)
{
  /* Kernel independent stuff to do at device open time. */

  /* Zero out entire VM structure. */
  mon_memzero( vm, sizeof(vm_t) );

  vm->vmState = VMStateFDOpened;
}

  int
hostInitMonitor(vm_t *vm)
{
  unsigned pdi, pti;
  unsigned int i;
  Bit32u nexus_size;
  page_t  *pageTable;
  Bit32u laddr, base;
  int r;

  vm->kernel_offset = hostOSKernelOffset();

  vm->system.a20Enable = 1; /* Start with A20 line enabled. */
  vm->system.a20AddrMask  = 0xffffffff; /* All address lines contribute. */
  vm->system.a20IndexMask = 0x000fffff; /* All address lines contribute. */

  /* Initialize nexus */
  mon_memzero(vm->host.addr.nexus, 4096);

  /* Copy transition code (nexus) into code page allocated for this VM. */
  nexus_size = ((Bit32u) &__nexus_end) - ((Bit32u) &__nexus_start);
  if (nexus_size > 4096)
    goto error;
  mon_memcpy(vm->host.addr.nexus, &__nexus_start, nexus_size);


  /* Init the convenience pointers. */

  /* Pointer to host2mon routine inside nexus page */
  vm->host.__host2mon = (void (*)(void)) HOST_NEXUS_OFFSET(vm, __host2mon);

  /* Pointer to guest context on monitor stack */
  vm->host.addr.guest_context = (guest_context_t *)
    ( (Bit32u)vm->host.addr.nexus + PAGESIZE - sizeof(guest_context_t) );

  /* Zero out various monitor data structures */
  mon_memzero(vm->host.addr.log_buffer, 4096*LOG_BUFF_PAGES);
  mon_memzero(&vm->log_buffer_info,
              sizeof(vm->log_buffer_info));
  mon_memzero(vm->host.addr.page_dir, 4096);
  mon_memzero(vm->host.addr.guest_cpu, 4096);
  mon_memzero(vm->host.addr.idt, MON_IDT_PAGES*4096);
  mon_memzero(vm->host.addr.gdt, MON_GDT_PAGES*4096);
  mon_memzero(vm->host.addr.ldt, MON_LDT_PAGES*4096);
  mon_memzero(vm->host.addr.tss, MON_TSS_PAGES*4096);
  mon_memzero(vm->host.addr.idt_stubs, MON_IDT_STUBS_PAGES*4096);

  vm->guestPhyPagePinQueue.nEntries = 0;
  vm->guestPhyPagePinQueue.tail = 0;

  /*
   *  ================
   *  Nexus Page Table
   *  ================
   *
   *  All structures needed by the monitor inside the guest environment
   *  (code to perform the transition between host<-->guest, fault handler
   *  code, various processor data structures like page directory, GDT,
   *  IDT, TSS etc.) are mapped into a single Page Table.
   *
   *  This allows us to migrate the complete nexus to anywhere in the
   *  guest address space by just updating a single (unused) page directory
   *  entry in the monitor/guest page directory to point to this nexus
   *  page table.
   *
   *  To simplify nexus migration, we try to avoid storing guest linear
   *  addresses to nexus structures as far as possible.  Instead, we use
   *  offsets relative to the monitor code/data segments.  As we update
   *  the base of these segments whenever the monitor migrates, the net
   *  effect is that those *offsets* remain valid across nexus migration. 
   */

  /* Fill in the PDE flags.  The US bit is set to 1 (user access).
   * All of the US bits in the monitor PTEs are set to 0 (system access).
   */
  vm->host.nexus_pde.fields.base = vm->pages.nexus_page_tbl;
  vm->host.nexus_pde.fields.avail = 0;
  vm->host.nexus_pde.fields.G = 0;      /* not global */
  vm->host.nexus_pde.fields.PS = 0;     /* 4K pages */
  vm->host.nexus_pde.fields.D = 0;      /* (unused in pde) */
  vm->host.nexus_pde.fields.A = 0;      /* not accessed */
  vm->host.nexus_pde.fields.PCD = 0;    /* normal caching */
  vm->host.nexus_pde.fields.PWT = 0;    /* normal write-back */
  vm->host.nexus_pde.fields.US = 1;     /* user access (see above) */
  vm->host.nexus_pde.fields.RW = 1;     /* read or write */
  vm->host.nexus_pde.fields.P = 1;      /* present in memory */

  /* Clear Page Table. */
  pageTable = vm->host.addr.nexus_page_tbl;
  mon_memzero(pageTable, 4096);

  /* xxx Comment here */
  laddr = 0;
  base = MON_BASE_FROM_LADDR(laddr);

  hostMapMonPages(vm, kernelModulePages.ppi, kernelModulePages.nPages, &laddr,
                  pageTable, US0, RW1, "Monitor code/data pages");


#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif

  vm->guest.addr.nexus = (nexus_t *) (laddr - base);
  hostMapMonPages(vm, &vm->pages.nexus, 1, &laddr, pageTable, US0, RW1, "Nexus");
  vm->guest.addr.guest_context = (guest_context_t *)
    ( (Bit32u)vm->guest.addr.nexus + PAGESIZE - sizeof(guest_context_t) );

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->host.addr.nexus->vm = (void *) (laddr - base);
  hostMapMonPages(vm, vm->pages.vm, BytesToPages(sizeof(*vm)),
                  &laddr, pageTable, US0, RW1, "VM structure");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->guest.addr.idt = (gate_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.idt, MON_IDT_PAGES, &laddr, pageTable, US0, RW1,
                  "IDT");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->guest.addr.gdt = (descriptor_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.gdt, MON_GDT_PAGES, &laddr, pageTable, US0, RW1,
                  "GDT");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->guest.addr.ldt = (descriptor_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.ldt, MON_LDT_PAGES, &laddr, pageTable, US0, RW1,
                  "LDT");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->guest.addr.tss = (tss_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.tss, MON_TSS_PAGES, &laddr, pageTable, US0, RW1,
                  "TSS");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  vm->guest.addr.idt_stubs = (idt_stub_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.idt_stubs, MON_IDT_STUBS_PAGES, &laddr,
                  pageTable, US0, RW1, "IDT stubs");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Monitor Page Directory */
  vm->guest.addr.page_dir = (pageEntry_t *) (laddr - base);
  hostMapMonPages(vm, &vm->pages.page_dir, 1, &laddr, pageTable, US0, RW1,
                  "Monitor Page Directory");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Nexus Page Table */
  vm->guest.addr.nexus_page_tbl = (page_t *) (laddr - base);
  hostMapMonPages(vm, &vm->pages.nexus_page_tbl, 1, &laddr, pageTable, US0, RW1,
                  "Nexus Page Table");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Map virtualized guest page tables into monitor. */
  vm->guest.addr.page_tbl = (page_t *) (laddr - base);
  hostMapMonPages(vm, vm->pages.page_tbl, MON_PAGE_TABLES,
                  &laddr, pageTable, US0, RW1, "Guest Page Tables");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Map of linear addresses of page tables mapped into monitor */
  vm->guest.addr.page_tbl_laddr_map = (unsigned *) (laddr - base);
  hostMapMonPages(vm, &vm->pages.page_tbl_laddr_map, 1, &laddr, pageTable,
                  US0, RW1, "Page Table Laddr Map");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Guest CPU state (mapped RW into user space also). */
  vm->guest.addr.guest_cpu = (guest_cpu_t *) (laddr - base);
  hostMapMonPages(vm, &vm->pages.guest_cpu, 1, &laddr,
                  pageTable, US0, RW1, "Guest CPU State");


#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /*
   *  We need a buffer to implement a debug print facility which
   *  can work in either host or monitor space.  Map the buffer
   *  into monitor/guest space.
   */
  vm->guest.addr.log_buffer = (unsigned char *) (laddr - base);
  hostMapMonPages(vm, vm->pages.log_buffer, LOG_BUFF_PAGES, &laddr,
                  pageTable, US0, RW1, "Log Buffer");

  {
  /* The physical addresses of the following pages are not */
  /* yet established.  Pass dummy info until they are mapped. */
  Bit32u tmp[1];
  tmp[0] = 0;

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Window into the guest's current physical code page */
  vm->guest.addr.code_phy_page = (unsigned char *) (laddr - base);
  hostMapMonPages(vm, tmp, 1, &laddr, pageTable, US0, RW1, "Code Phy Page");

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif
  /* Temporary window into a guest physical page, for accessing */
  /* guest GDT, IDT, etc info. */
  vm->guest.addr.tmp_phy_page0 = (unsigned char *) (laddr - base);
  hostMapMonPages(vm, tmp, 1, &laddr, pageTable, US0, RW1, "Tmp Phy Page0");

  vm->guest.addr.tmp_phy_page1 = (unsigned char *) (laddr - base);
  hostMapMonPages(vm, tmp, 1, &laddr, pageTable, US0, RW1, "Tmp Phy Page1");
  }

#if ANAL_CHECKS
  hostMapBlankPage(vm, &laddr, pageTable);
#endif

  hostOSPrint("Using %u/1024 PTE slots in 4Meg monitor range.\n",
              (laddr >> 12) & 0x3ff);

  /* Pointer to mon2host routine inside nexus page */
  vm->guest.__mon2host = (void (*)(void)) MON_NEXUS_OFFSET(vm, __mon2host);


  /*
   *  =====================
   *  Transition Page Table
   *  =====================
   *
   *  To aid in the transition between host<-->monitor/guest spaces,
   *  we need to have an address identity map situation for at least
   *  one page; the page containing the transition code.   As we do
   *  not know in advance whether this linear address range is in use
   *  by the guest as well, we set aside a complete additional Page
   *  Table, which contains only a single PTE pointing to the nexus page.
   *
   *  To create the identity map, we simply change the corresponding
   *  monitor page directory entry to point to this transition Page Table.
   *  This happens transparently inside the host<-->guest transition code; 
   *  both the guest/monitor code and the host side code never see this 
   *  transition page table entered into the page directory!
   *
   *  NOTE: We need to ensure that the nexus page table never spans the
   *        same 4Meg linear address space region as this page table!
   *        As we are free to choose the nexus linear address, this is
   *        not a problem.
   */

  /* Get full linear address of nexus code page, as seen in host space. */
  laddr = (Bit32u)vm->host.addr.nexus + vm->kernel_offset;
  pdi = laddr >> 22;
  pti = (laddr >> 12) & 0x3ff;

  /*
   *  We need to be able to access the PDE in the monitor page directory
   *  that corresponds to this linear address from both host and monitor 
   *  address spaces.
   */
  vm->host.addr.nexus->transition_pde_p_host = vm->host.addr.page_dir + pdi;
  vm->host.addr.nexus->transition_pde_p_mon  = (pageEntry_t *)
                        (((Bit32u)vm->guest.addr.page_dir) + (pdi << 2));
  vm->host.addr.nexus->transition_laddr = laddr;

  /* Fill in the PDE flags */
  vm->host.addr.nexus->transition_pde.fields.base = vm->pages.transition_PT;
  vm->host.addr.nexus->transition_pde.fields.avail = 0;
  vm->host.addr.nexus->transition_pde.fields.G = 0;   /* not global */
  vm->host.addr.nexus->transition_pde.fields.PS = 0;  /* 4K pages */
  vm->host.addr.nexus->transition_pde.fields.D = 0;   /* (unused in pde) */
  vm->host.addr.nexus->transition_pde.fields.A = 0;   /* not accessed */
  vm->host.addr.nexus->transition_pde.fields.PCD = 0; /* normal caching */
  vm->host.addr.nexus->transition_pde.fields.PWT = 0; /* normal write-back*/
  vm->host.addr.nexus->transition_pde.fields.US = 0;  /* no user access  */
  vm->host.addr.nexus->transition_pde.fields.RW = 1;  /* read or write */
  vm->host.addr.nexus->transition_pde.fields.P = 1;   /* present in memory*/

  /* Clear Page Table; only one PTE is used. */
  pageTable = vm->host.addr.transition_PT;
  mon_memzero(pageTable, 4096);

  /* Fill in the PTE for identity mapping the code page */
  pageTable->pte[pti].fields.base = vm->pages.nexus;
  pageTable->pte[pti].fields.avail = 0;
  pageTable->pte[pti].fields.G = 0;      /* not global          */
  pageTable->pte[pti].fields.PS = 0;     /* (unused in pte)     */
  pageTable->pte[pti].fields.D = 0;      /* clean               */
  pageTable->pte[pti].fields.A = 0;      /* not accessed        */
  pageTable->pte[pti].fields.PCD = 0;    /* normal caching      */
  pageTable->pte[pti].fields.PWT = 0;    /* normal write-back   */
  pageTable->pte[pti].fields.US = 0;     /* user can not access */
  pageTable->pte[pti].fields.RW = 1;     /* read or write       */
  pageTable->pte[pti].fields.P = 1;      /* present in memory   */


  /* 
   *  Setup the TSS for the monitor/guest environment.
   *
   *  We don't need to set the pagedir in the TSS, because we don't 
   *  actually jump to it anyway.  The TSS is just used to set the kernel 
   *  stack and in a later stage, perhaps the I/O permission bitmap.
   */

  /* No task chain. */
  vm->host.addr.tss->back = 0;

  /* No debugging or I/O, for now. */
  vm->host.addr.tss->trap = 0;
  vm->host.addr.tss->io = sizeof(tss_t);

  /* Monitor stack offset. */
  vm->host.addr.tss->esp0 =
    ((Bit32u)vm->guest.addr.nexus) + PAGESIZE;


  /*
   * Set up initial monitor code and stack offset.
   */

  vm->host.addr.nexus->mon_jmp_info.offset   = MON_NEXUS_OFFSET(vm, __mon_cs);
  vm->host.addr.nexus->mon_stack_info.offset =
      vm->host.addr.tss->esp0 - (sizeof(guest_context_t) + 48);
/* xxx 48 above should be calculated from code below which winds
 * xxx up monitor stack.
 */


  /*
   *  Setup the IDT for the monitor/guest environment
   */

  r = 0;
  r |= hostInitIDTSlot(vm,  0, IDT_EXCEPTION_NOERROR); /* Divide error        */
  r |= hostInitIDTSlot(vm,  1, IDT_EXCEPTION_NOERROR); /* Debug exceptions    */
  r |= hostInitIDTSlot(vm,  2, IDT_INTERRUPT);         /* NMI                 */
  r |= hostInitIDTSlot(vm,  3, IDT_EXCEPTION_NOERROR); /* Breakpoint          */
  r |= hostInitIDTSlot(vm,  4, IDT_EXCEPTION_NOERROR); /* Overflow            */
  r |= hostInitIDTSlot(vm,  5, IDT_EXCEPTION_NOERROR); /* Bounds check        */
  r |= hostInitIDTSlot(vm,  6, IDT_EXCEPTION_NOERROR); /* Invalid opcode      */
  r |= hostInitIDTSlot(vm,  7, IDT_EXCEPTION_NOERROR); /* FPU not available   */
  r |= hostInitIDTSlot(vm,  8, IDT_EXCEPTION_ERROR);   /* Double fault        */
  r |= hostInitIDTSlot(vm,  9, IDT_EXCEPTION_NOERROR); /* FPU segment overrun */
  r |= hostInitIDTSlot(vm, 10, IDT_EXCEPTION_ERROR);   /* Invalid TSS         */
  r |= hostInitIDTSlot(vm, 11, IDT_EXCEPTION_ERROR);   /* Segment not present */
  r |= hostInitIDTSlot(vm, 12, IDT_EXCEPTION_ERROR);   /* Stack exception     */
  r |= hostInitIDTSlot(vm, 13, IDT_EXCEPTION_ERROR);   /* GP fault            */
  r |= hostInitIDTSlot(vm, 14, IDT_EXCEPTION_ERROR);   /* Page fault          */
  r |= hostInitIDTSlot(vm, 15, IDT_EXCEPTION_NOERROR); /* reserved            */
  r |= hostInitIDTSlot(vm, 16, IDT_EXCEPTION_NOERROR); /* Coprocessor error   */
  r |= hostInitIDTSlot(vm, 17, IDT_EXCEPTION_ERROR);   /* Alignment check     */
  r |= hostInitIDTSlot(vm, 18, IDT_EXCEPTION_NOERROR); /* Machine check       */

  /* Reserved exceptions */
  for (i = 19; i < 32; i++)
      r |= hostInitIDTSlot(vm, i, IDT_EXCEPTION_NOERROR);

  /* Hardware interrupts */
  for (i = 32; i < 256; i++)
      r |= hostInitIDTSlot(vm, i, IDT_INTERRUPT);
  if (r!=0) 
      goto error;


  /*
   *  Setup the initial guest context
   */

  mon_memzero(vm->host.addr.guest_context, sizeof(guest_context_t));

  /* Wind up the monitor stack for the initial transition via
   * __host2mon.  At the tail end, monitor state is popped from the
   * stack and a RET is executed.
   */
  {
  Bit32u *ptr;

  ptr = (Bit32u *) (((unsigned char *) vm->host.addr.guest_context) - 4);
  *ptr-- = (Bit32u) &__ret_to_guest;
  *ptr-- = 0x02; /* eflags: only reserved bit on */
  *ptr-- = 0; /* eax */
  *ptr-- = 0; /* ecx */
  *ptr-- = 0; /* edx */
  *ptr-- = 0; /* ebx */
  *ptr-- = 0; /* esp dummy */
  *ptr-- = 0; /* ebp */
  *ptr-- = 0; /* esi */
  *ptr-- = 0; /* edi */
  *ptr-- = 0; /* FS; start with null value. */
  *ptr-- = 0; /* GS; start with null value. */
  }

  vm->vmState |= VMStateInitMonitor;
  vm->mon_request = MonReqNone;

  return(1); /* all OK */

error:
  return(0); /* error */
}



  unsigned
hostInitGuestPhyMem(vm_t *vm)
{
  unsigned i;
  mon_memzero(vm->pageInfo, sizeof(vm->pageInfo));
  for (i=0; i<vm->pages.guest_n_pages; i++) {
    /* For now, we start out by preallocating physical pages */
    /* for the guest, though not necessarily mapped into linear */
    /* space. */
    vm->pageInfo[i].attr.raw = 0;
    vm->pageInfo[i].tsc = 0;
    vm->pageInfo[i].attr.fields.allocated = 1;
    }
 
  {
  Bit32u rom_page;
  unsigned npages;
 
  /* Mark BIOS ROM area as ReadOnly */
  rom_page = 0xf0000 >> 12;
  npages = (1 + 0xfffff - 0xf0000) / 4096;
  for (i=0; i<npages; i++)
    vm->pageInfo[rom_page + i].attr.fields.RO = 1;

  /* Mark VGA BIOS ROM area as ReadOnly */
  rom_page = 0xc0000 >> 12;
  npages = (1 + 0xc7fff - 0xc0000) / 4096;
  for (i=0; i<npages; i++)
    vm->pageInfo[rom_page + i].attr.fields.RO = 1;
  }
 
#if 1
  /* Mark VGA framebuffer area as Memory Mapped IO */
  {
  Bit32u vga_page;
  unsigned npages;
 
  vga_page = 0xa0000 >> 12;
  npages = (1 + 0xbffff - 0xa0000) / 4096;
  for (i=0; i<npages; i++)
    vm->pageInfo[vga_page + i].attr.fields.memMapIO = 1;
  }
#endif

  return(0);
}


  int
hostInitIDTSlot(vm_t *vm, unsigned vec, int type)
/*
 *  initIDTSlot():  Initialize a monitor IDT slot.
 */
{
  /* IDT slot stubs */

  idt_stub_t *stub = &vm->host.addr.idt_stubs[vec];
  Bit32u stub_mon = ((Bit32u) vm->guest.addr.idt_stubs) +
                    vec*sizeof(idt_stub_t);

  if (sizeof(idt_stub_t) != IDT_STUB_SIZE)
    return( -1 );

  switch (type) {
    case IDT_INTERRUPT:
      stub->m2.pushla = 0x68;
      stub->m2.dummy  = 0;
      stub->m2.pushlb = 0x68;
      stub->m2.vector = vec;
      stub->m2.jmp    = 0xe9;
      stub->m2.reloc  = ((Bit32u) &__handle_int) -
        (stub_mon + sizeof(idt_method2_t));
      break;

    case IDT_EXCEPTION_ERROR:
      stub->m1.pushl  = 0x68;
      stub->m1.vector = vec;
      stub->m1.jmp    = 0xe9;
      stub->m1.reloc  = ((Bit32u) &__handle_fault) -
        (stub_mon + sizeof(idt_method1_t));
      break;

    case IDT_EXCEPTION_NOERROR:
      stub->m2.pushla = 0x68;
      stub->m2.dummy  = 0;
      stub->m2.pushlb = 0x68;
      stub->m2.vector = vec;
      stub->m2.jmp    = 0xe9;
      stub->m2.reloc  = ((Bit32u) &__handle_fault) -
        (stub_mon + sizeof(idt_method2_t));
      break;

    default:
      return -1;
    }

  /* Set the interrupt gate */
  SET_INT_GATE(vm->host.addr.idt[vec],
               nullSelector, stub_mon, D_PRESENT, D_DPL0, D_D32);
  return 0;
}


/*
 *  Map pages allocated by host, into the linear address space of
 *  the monitor/guest, given the Page Table supplied.
 */

  void
hostMapMonPages(vm_t *vm, Bit32u *pages, unsigned n, Bit32u *laddr_p,
                page_t *pageTable, unsigned user, unsigned writable, char *name)
{
  unsigned i, pti;


#if 0
hostOSPrint("hostMapMonPages: '%s' mapped at 0x%x .. 0x%x.\n",
            name,
            (*laddr_p) - MON_BASE_FROM_LADDR(0),
            ((*laddr_p) + (n*4096)) - MON_BASE_FROM_LADDR(0) );
#endif

  pti = (*laddr_p >> 12) & 0x3ff;
  for (i = 0; i < n; i++, pti++) {
    if (pti > 1024)
        break;  /* This should not happen! */

    /* Fill in the PTE flags */
    pageTable->pte[pti].fields.base = pages[i];
    pageTable->pte[pti].fields.avail = 0;
    pageTable->pte[pti].fields.G = 0;         /* not global */
    pageTable->pte[pti].fields.PS = 0;        /* (unused in pte) */
    pageTable->pte[pti].fields.D = 0;         /* clean */
    pageTable->pte[pti].fields.A = 0;         /* not accessed */
    pageTable->pte[pti].fields.PCD = 0;       /* normal caching */
    pageTable->pte[pti].fields.PWT = 0;       /* normal write-back */
    pageTable->pte[pti].fields.US = user;     /* 0=system, 1=user */
    pageTable->pte[pti].fields.RW = writable; /* 0=RO, 1=RW */
    pageTable->pte[pti].fields.P = 1;         /* present in memory */
    }

  /*
   *  Advance linear address pointer, for the next set of pages
   *  to be mapped.
   */
  *laddr_p += 4096 * n;
}

#if ANAL_CHECKS
  void
hostMapBlankPage(vm_t *vm, Bit32u *laddr_p, page_t *pageTable)
{
  unsigned pti;
 
  pti = (*laddr_p >> 12) & 0x3ff;
  if (pti > 1024)
    return;  /* This should not happen! */
 
  /* Fill in the PTE flags */
  pageTable->pte[pti].fields.base = 0;
  pageTable->pte[pti].fields.avail = 0;
  pageTable->pte[pti].fields.G = 0;      /* not global */
  pageTable->pte[pti].fields.PS = 0;     /* (unused in pte) */
  pageTable->pte[pti].fields.D = 0;      /* clean */
  pageTable->pte[pti].fields.A = 0;      /* not accessed */
  pageTable->pte[pti].fields.PCD = 0;    /* normal caching */
  pageTable->pte[pti].fields.PWT = 0;    /* normal write-back */
  pageTable->pte[pti].fields.US = 0;
  pageTable->pte[pti].fields.RW = 0;
  pageTable->pte[pti].fields.P = 0;
 
  /*
   *  Advance linear address pointer, for the next set of pages
   *  to be mapped.
   */
  *laddr_p += 4096;
}
#endif

  int
hostIoctlGeneric(vm_t *vm, void *inode, void *filp,
                 unsigned int cmd, unsigned long arg)
{
  switch (cmd) {

    /*
     * Set the guest CPUID info.
     */
    case PLEX86_CPUID:
      {
      if ( vm->vmState & VMStateGuestCPUID ) {
        /* Can't change guest CPUID. */
        return -Plex86ErrnoEINVAL;
        }
      if ( hostOSCopyFromUser(&vm->guestCPUIDInfo, (void *)arg,
                              sizeof(vm->guestCPUIDInfo)) )
        return -Plex86ErrnoEFAULT;
/* xxx Value checks here. */
      vm->vmState |= VMStateGuestCPUID;
      return 0;
      }

    case PLEX86_REGISTER_MEMORY:
      {
      plex86IoctlRegisterMem_t registerMemMsg;
      if ( hostOSCopyFromUser(&registerMemMsg, (void *)arg,
                              sizeof(registerMemMsg)) )
        return -Plex86ErrnoEFAULT;
      return( hostIoctlRegisterMem(vm, &registerMemMsg) );
      }

    /*
     * Tear down the VM environment.
     */
    case PLEX86_TEARDOWN:
      if ( vm->vmState & VMStateRegisteredAll ) {
        hostOSPrint("plex86: guest memory is still registered!\n");
        /* Could effect the unpinning here and then do:
         *   vm->vmState &= ~VMStateRegisteredAll;
         */
        return -Plex86ErrnoEBUSY;
        }

      hostUnallocVmPages(vm);
      /* Fixme: deal with state better here. */

      /* Reset state to only FD opened. */
      vm->vmState = VMStateFDOpened;
      return 0;


    /*
     * Execute the guest in the VM for a while.  The guest CPU state
     * is specified in a memory window mmap()'d to user space.
     */
    case PLEX86_EXECUTE:
      {
      plex86IoctlExecute_t executeMsg;
      int ret;

      if ( hostOSCopyFromUser(&executeMsg, (void *)arg, sizeof(executeMsg)) )
        return -Plex86ErrnoEFAULT;
      ret = hostIoctlExecute(vm, &executeMsg);
      if ( hostOSCopyToUser((void *)arg, &executeMsg, sizeof(executeMsg)) )
        return -Plex86ErrnoEFAULT;
      return ret;
      }

#warning "PLEX86_RESET should only conditionally compiled for debugging."
    /*
     * For debugging, when the module gets hosed, this is a way
     * to reset the in-use count, so we can rmmod it.
     */
    case PLEX86_RESET:
      hostOSModuleCountReset(vm, inode, filp);
      return 0;


    default:
      hostOSPrint("plex86: unknown ioctl(%d) called\n", cmd);
      return -Plex86ErrnoEINVAL;
    }
}

  int
hostIoctlExecute(vm_t *vm, plex86IoctlExecute_t *executeMsg)
{
  guest_cpu_t     *guest_cpu;
  guest_context_t *guest_stack_context;
  nexus_t *nexus;
  unsigned s;
  int retval;

  if ( (vm->vmState != VMStateReady) ||
       (vm->mon_request != MonReqNone) ) {
    retval = Plex86NoExecute_VMState; /* Fail. */
    goto handlePanic;
    }

  /* Only (virtualized) native execution is supported currently.
   * Later, it will be interesting to breakpoint one instruction
   * at-a-time using Plex86ExecuteMethodBreakpoint, for
   * cosimulation.
   */
  if (executeMsg->executeMethod != Plex86ExecuteMethodNative) {
    retval = Plex86NoExecute_Method; /* Fail. */
    goto handleFail;
    }

  /* A pointer to the guest CPU state as passed from host-user space.
   * This structure is memory mapped between user and kernel/monitor space.
   */
  guest_cpu = vm->host.addr.guest_cpu;

  /* A pointer to the guest CPU state saved on the monitor stack. */
  guest_stack_context =  vm->host.addr.guest_context;

  /* =================================================================
   * Before executing the guest in the VM, we must check that
   * the guest conditions meet the requirements of the user-level-only
   * VM.
   * =================================================================
   */

  /* CR0:
   *   PG(31)==1
   *   CD(30)==? (look into this later)
   *   NW(29)==? (look into this later)
   *   AM(18)==pass-thru from guest
   *   WP(16)==Don't care.  Monitor always sets this to 1.
   *   NE( 5)==? (look into this later)
   *   ET( 4)==? (look into this later)
   *   TS( 3)==? (look into this later)
   *   EM( 2)==? (look into this later)
   *   MP( 1)==? (look into this later)
   *   PE( 0)==1
   */
  /* 0x8005003b */
  if ( (guest_cpu->cr0.raw & 0xe0000037) != 0x80000033 ) {
    hostOSPrint("plex86: guest CR0=0x%x\n", guest_cpu->cr0.raw);
    retval = Plex86NoExecute_CR0; /* Fail. */
    goto handleFail;
    }

  /* CR4:
   *   OSXMMEXCPT(10)==? (look into this later)
   *   OSFXSR(9)==? (look into this later)
   *   PCE(8)==? (look into this later)
   *   PGE(7)==? (look into this later)
   *   MCE(6)==? (look into this later)
   *   PAE(5)==? (look into this later)
   *   PSE(4)==? (look into this later)
   *   DE(3)==? (look into this later)
   *   TSD(2)==? (look into this later)
   *   PVI(1)==? (look into this later)
   *   VME(0)==? (look into this later)
   */
  if ( (guest_cpu->cr4.raw & 0x000007ff) != 0x00000000 ) {
    hostOSPrint("plex86: guest CR4=0x%x\n", guest_cpu->cr4.raw);
    retval = Plex86NoExecute_CR4; /* Fail. */
    goto handleFail;
    }

  /* Guest CPL must be 3 (user-level).
   * CS selector must not be NULL.
   */
  if ( (guest_cpu->sreg[SRegCS].sel.fields.rpl != 3) ||
       (guest_cpu->sreg[SRegCS].sel.fields.index == 0) ||
       (guest_cpu->sreg[SRegCS].des.dpl != 3) ) {
    retval = Plex86NoExecute_CS; /* Fail. */
    goto handleFail;
    }

  /* A20 line must be enabled. */
  if ( guest_cpu->a20Enable != 1 ) {
    retval = Plex86NoExecute_A20; /* Fail. */
    goto handleFail;
    }

  /* Some code not really used now, since we only support A20 being enabled. */
  {
  unsigned newA20Enable;
  newA20Enable = guest_cpu->a20Enable > 0; /* Make 0 or 1. */
  if ( newA20Enable != vm->system.a20Enable ) {
    if ( (!newA20Enable) && guest_cpu->cr0.fields.pg ) {
      /* A20 disabled, paging on not supported.  Well, really I have to
       * see if it matters.  This check was in old plex86 code.
       */
      retval = Plex86NoExecute_A20; /* Fail. */
      goto handleFail;
      }
    vm->system.a20Enable = newA20Enable;
    vm->system.a20AddrMask  = 0xffefffff | (newA20Enable << 20);
    vm->system.a20IndexMask = 0x000ffeff | (newA20Enable << 8);
    }
  }

  /* LDT not supported.
   * Monitor uses GDT slots 1,2,3, so guest segments can not.
   * Segment descriptor cache DPL should equal 3.
   */
  for (s=0; s<6; s++) {
    unsigned selector = guest_cpu->sreg[s].sel.raw;
    unsigned index;
    /* Only care if selector is not NULL. */
    if ( selector & 0xfffc ) {
      if ( (selector & 0x0007) != 3 ) {
        /* Either TI=1 (LDT usage) or RPL!=3. */
        retval = Plex86NoExecute_Selector; /* Fail. */
        goto handleFail;
        }
      index = selector >> 3;
      if ( index <= 3 ) {
        /* Selector index field uses one of the monitor GDT slots. */
        retval = Plex86NoExecute_Selector; /* Fail. */
        goto handleFail;
        }
      if ( index >= (MON_GDT_SIZE/8) ) {
        /* Selector index field uses a slot beyond the monitor GDT size. */
        retval = Plex86NoExecute_Selector; /* Fail. */
        goto handleFail;
        }
      if ( guest_cpu->sreg[s].des.dpl != 3 ) {
        retval = Plex86NoExecute_DPL; /* Fail. */
        goto handleFail;
        }
      }
    }

  /* EFlags constraints:
   *   VIP/VIF==0
   *   VM==0
   *   RF==0
   *   NT==0
   *   IOPL==0 (We may be able to allow this to be 0..2)
   *   IF==1
   *   TF==0
   *   bit1==1
   */
  if ( (guest_cpu->eflags & (0x001b7302)) !=
       (0x00000202) ) {
    retval = Plex86NoExecute_EFlags; /* Fail. */
    goto handleFail;
    }

  /* Notes on other stuff:
   *  - CPUID emulation vs virtualization match.
   */

  /* NOTE: We should commit to executing the guest at this point.
   *   We must not leave stray entries in the GDT.
   */

/* Install virtualized guest descriptors in GDT.
 * Either use descriptor caches from guest space, or we have
 * to chase down the GDT entries using the guest's paging
 * system.  Might be a cheaper/safe bet to just use the
 * descriptor caches.  If the guest reloads a descriptor,
 * just let the user space deal with it.
 */
  for (s=0; s<6; s++) {
    if ( (guest_cpu->sreg[s].sel.raw & 0xfffc) != 0) {
      vm->host.addr.gdt[ guest_cpu->sreg[s].sel.fields.index ] =
          guest_cpu->sreg[s].des;
      }
    }
#warning "Have to clear out GDT"

  guest_stack_context->gs = guest_cpu->sreg[SRegGS].sel.raw;
  guest_stack_context->fs = guest_cpu->sreg[SRegFS].sel.raw;
  guest_stack_context->ds = guest_cpu->sreg[SRegDS].sel.raw;
  guest_stack_context->es = guest_cpu->sreg[SRegES].sel.raw;

  /* Could use memcpy(); both are in order.  Pack both structs. */
  guest_stack_context->edi = guest_cpu->edi;
  guest_stack_context->esi = guest_cpu->esi;
  guest_stack_context->ebp = guest_cpu->ebp;
  guest_stack_context->dummy_esp = 0; /* Not needed. */
  guest_stack_context->ebx = guest_cpu->ebx;
  guest_stack_context->edx = guest_cpu->edx;
  guest_stack_context->ecx = guest_cpu->ecx;
  guest_stack_context->eax = guest_cpu->eax;

  /* Fields vector/error are ignored for return to guest. */

  /* CS:EIP */
  guest_stack_context->eip = guest_cpu->eip;
  guest_stack_context->cs  = guest_cpu->sreg[SRegCS].sel.raw;

  guest_stack_context->eflags.raw = guest_cpu->eflags;
  vm->veflags.raw = 0; /* Virtualized EFLAGS - implement later. */

  guest_stack_context->esp = guest_cpu->esp;
  guest_stack_context->ss  = guest_cpu->sreg[SRegSS].sel.raw;

  /* Pointer to the fields in the nexus.S assembly code. */
  nexus = vm->host.addr.nexus;

#warning "Monitor CRx hacks"
  nexus->mon_cr0 = 0x8001003b | /* PG/WP/NE/ET/TS/MP/PE */
    (guest_cpu->cr0.raw & 0x00040000); /* Pass-thru AM from guest. */
  /* Could move mon_cr3 load to mapMonitor. */
  nexus->mon_cr3 = vm->pages.page_dir << 12;
  nexus->mon_cr4 = 0x00000004; /* TSD=1 */

/* vm->guest_cpu.cr0.raw = guest_cpu->cr0 | 0x32; */ /* +++ hack for now */

// Notes:
//   - Implement some of monPagingRemap from old code, since that
//   was intended to be run/triggered by an initial mode change.
//   - After execution of 1st timeslice, need to copy dynamic state
//   from VM to guest_cpu area.
//   - Deal with cycle counts etc.

  hostInitShadowPaging(vm);

  for (;;) {
    unsigned long eflags;

#if 0
    /* If print buffer has contents, return to user space to print. */
    if (vm->log_buffer_info.offset) {
      vm->mon_msgs.header.msg_type = VMMessagePrintBuf;
      vm->mon_msgs.header.msg_len  = 0;
      vm->mon_request = MonReqNone; /* Request satisfied */
      resetPrintBuf(vm); /* xxx Fix print mess */
      retval = 100;
      goto handleFail;
      }
#endif

    vm_save_flags(eflags);
    vm_restore_flags(eflags & ~0x00004300); /* clear NT/IF/TF */
#if ANAL_CHECKS
    if (!(eflags & 0x200)) {
      vm_restore_flags(eflags);
      hostOSPrint("ioctlExecute: EFLAGS.IF==0\n");
      retval = 101; /* Fail. */
      goto handlePanic;
      }
#endif

    /* Call assembly routine to effect transition. */
    vm->host.__host2mon();

    /* First check for an asynchronous event (interrupt redirection) */
    if ( vm->mon_request == MonReqRedirect ) {
      vm_restore_flags(eflags & ~0x00000200); /* restore all but IF */
      soft_int(vm->redirect_vector); /* sets IF to 1 */
      hostOSInstrumentIntRedirCount(vm->redirect_vector);
      vm->mon_request = MonReqNone; /* Request satisfied */
      }

    /* Event was synchronous; monitor requested a switch back to host. */
    else {
      vm_restore_flags(eflags);

      /* Perform action requested by monitor. */
      switch ( vm->mon_request ) {
        case MonReqRemapMonitor:
#if 0
          if ( mapMonitor(vm) ) {
            vm->mon_request = MonReqNone; /* Request satisfied */
            break;
            }
          else {
            hostOSPrint("mapMonitor failed.\n");
            hostOSPrint("Panic w/ abort_code=%u\n", vm->abort_code);
            retval = 102;
            goto handlePanic;
            }
#endif
          hostOSPrint("ioctlExecute: case MonReqRemapMonitor.\n");
          retval = 103;
          goto handlePanic;

        case MonReqFlushPrintBuf:
          hostOSPrint("ioctlExecute: case MonReqFlushPrintBuf.\n");
          retval = 104;
          goto handlePanic;

        case MonReqGuestFault:
          /* Encountered a guest fault. */
          hostCopyGuestStateToUserSpace(vm);
          executeMsg->cyclesExecuted       = 0; /* Handle later. */
          executeMsg->instructionsExecuted = 0; /* Handle later. */
          executeMsg->monitorState.state   = vm->vmState;
          executeMsg->monitorState.request = vm->mon_request;
          executeMsg->monitorState.guestFaultNo = vm->guestFaultNo;
          vm->mon_request = MonReqNone;
          return 0;

        case MonReqPanic:
          if (vm->abort_code)
            hostOSPrint("Panic w/ abort_code=%u\n", vm->abort_code);
          hostOSPrint("ioctlExecute: case MonReqPanic.\n");
          retval = 106;
          goto handlePanic;

        case MonReqPinUserPage:
          if ( !hostHandlePagePinRequest(vm, vm->pinReqPPI) ) {
            retval = 108;
            goto handlePanic;
            }
          continue; /* Back to VM monitor. */

        default:
          hostOSPrint("ioctlExecute: default case (%u).\n", vm->mon_request);
          retval = 107;
          goto handlePanic;
        }
      }

    /* Let host decide whether we are allowed another timeslice */
    if ( !hostOSIdle() ) {
      /* We are returning only because the host wants to
       * schedule other work.
       */
      executeMsg->monitorState.state   = vm->vmState;
      executeMsg->monitorState.request = MonReqNone;
      return 0;
      }
    }

  /* Should not get here. */
  retval = 109;
  goto handlePanic;

handleFail:
  /* Handle inabilitiy to execute the guest due to certain state. */
  executeMsg->monitorState.state   = vm->vmState;
  executeMsg->monitorState.request = vm->mon_request;
  return(retval);

handlePanic:
  vm->vmState  |= VMStatePanic;
  vm->mon_request = MonReqPanic;
  executeMsg->monitorState.state   = vm->vmState;
  executeMsg->monitorState.request = vm->mon_request;
  return(retval);
}

  void
hostCopyGuestStateToUserSpace(vm_t *vm)
{
  guest_cpu_t     *guest_cpu;
  guest_context_t *guest_stack_context;

  /* A pointer to the guest CPU state as passed from host-user space.
   * This structure is memory mapped between user and kernel/monitor space.
   */
  guest_cpu = vm->host.addr.guest_cpu;

  /* A pointer to the guest CPU state saved on the monitor stack. */
  guest_stack_context =  vm->host.addr.guest_context;

  guest_cpu->sreg[SRegES].sel.raw = guest_stack_context->es;
  if ( (guest_stack_context->es & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegES].des = nullDescriptor;
    guest_cpu->sreg[SRegES].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegES].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegES].sel.fields.index ];
    guest_cpu->sreg[SRegES].valid = 1;
    }

  guest_cpu->sreg[SRegCS].sel.raw = guest_stack_context->cs;
  if ( (guest_stack_context->cs & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegCS].des = nullDescriptor;
    guest_cpu->sreg[SRegCS].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegCS].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegCS].sel.fields.index ];
    guest_cpu->sreg[SRegCS].valid = 1;
    }

  guest_cpu->sreg[SRegSS].sel.raw = guest_stack_context->ss;
  if ( (guest_stack_context->ss & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegSS].des = nullDescriptor;
    guest_cpu->sreg[SRegSS].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegSS].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegSS].sel.fields.index ];
    guest_cpu->sreg[SRegSS].valid = 1;
    }

  guest_cpu->sreg[SRegDS].sel.raw = guest_stack_context->ds;
  if ( (guest_stack_context->ds & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegDS].des = nullDescriptor;
    guest_cpu->sreg[SRegDS].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegDS].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegDS].sel.fields.index ];
    guest_cpu->sreg[SRegDS].valid = 1;
    }

  guest_cpu->sreg[SRegFS].sel.raw = guest_stack_context->fs;
  if ( (guest_stack_context->fs & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegFS].des = nullDescriptor;
    guest_cpu->sreg[SRegFS].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegFS].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegFS].sel.fields.index ];
    guest_cpu->sreg[SRegFS].valid = 1;
    }

  guest_cpu->sreg[SRegGS].sel.raw = guest_stack_context->gs;
  if ( (guest_stack_context->gs & 0xfffc) == 0 ) {
    guest_cpu->sreg[SRegGS].des = nullDescriptor;
    guest_cpu->sreg[SRegGS].valid = 0;
    }
  else {
    guest_cpu->sreg[SRegGS].des =
      vm->host.addr.gdt[ guest_cpu->sreg[SRegGS].sel.fields.index ];
    guest_cpu->sreg[SRegGS].valid = 1;
    }

  /* Could use memcpy(); both are in order.  Pack both structs. */
  guest_cpu->edi = guest_stack_context->edi;
  guest_cpu->esi = guest_stack_context->esi;
  guest_cpu->ebp = guest_stack_context->ebp;
  guest_cpu->esp = guest_stack_context->esp;
  guest_cpu->ebx = guest_stack_context->ebx;
  guest_cpu->edx = guest_stack_context->edx;
  guest_cpu->ecx = guest_stack_context->ecx;
  guest_cpu->eax = guest_stack_context->eax;

  /* CS:EIP */
  guest_cpu->eip = guest_stack_context->eip;

  guest_cpu->eflags = guest_stack_context->eflags.raw;
  /* vm->veflags.raw = 0; */ /* Virtualized EFLAGS - implement later. */
}


  int
hostIoctlRegisterMem(vm_t *vm, plex86IoctlRegisterMem_t *registerMemMsg)
{
  unsigned error;

  /* Do not allow duplicate allocation.  The file descriptor must be
   * opened.  The guest CPUID info can be filled in later.
   */
  if ( (vm->vmState & ~VMStateGuestCPUID) != VMStateFDOpened )
    return -Plex86ErrnoEBUSY;

  if (vm->pages.guest_n_megs != 0)
    return -Plex86ErrnoEBUSY;

  /* Check that the amount of memory is reasonable. */
  if ( (registerMemMsg->nMegs > PLEX86_MAX_PHY_MEGS)  ||
       (registerMemMsg->nMegs < 4) ||
       (registerMemMsg->nMegs & 0x3) )
    return -Plex86ErrnoEINVAL;

  /* Check that the guest memory vector is page aligned. */
  if ( registerMemMsg->guestPhyMemVector & 0xfff )
    return -Plex86ErrnoEINVAL;

  /* Check that the log buffer area is page aligned. */
  if ( registerMemMsg->logBufferWindow & 0xfff )
    return -Plex86ErrnoEINVAL;

  /* Check that the guest CPU area is page aligned. */
  if ( registerMemMsg->guestCPUWindow & 0xfff )
    return -Plex86ErrnoEINVAL;

  /* Check that none of the user areas overlap.  In case we have a
   * number of regions, use some generic code to handle N regions.
   */
  {
#define NumUserRegions 3
  struct {
    Bit32u min, max;
    } userRegion[NumUserRegions];
  unsigned i,j;

  userRegion[0].min = registerMemMsg->guestPhyMemVector;
  userRegion[0].max = userRegion[0].min + (registerMemMsg->nMegs<<20) - 1;
  userRegion[1].min = registerMemMsg->logBufferWindow;
  userRegion[1].max = userRegion[1].min + LOG_BUFF_SIZE - 1;
  userRegion[2].min = registerMemMsg->guestCPUWindow;
  userRegion[2].max = userRegion[2].min + (4096) - 1;

  for (i=1; i<NumUserRegions; i++) {
    for (j=1; j<NumUserRegions; j++) {
      if (j == i)
        continue; /* Don't compare at the same region. */
      /* Check for min(j) contained in region(i). */
      if ( (userRegion[j].min >= userRegion[i].min) &&
           (userRegion[j].min <= userRegion[i].max) )
        return -Plex86ErrnoEINVAL;
      /* Check for max(j) contained in region(i). */
      if ( (userRegion[j].max >= userRegion[i].min) &&
           (userRegion[j].max <= userRegion[i].max) )
        return -Plex86ErrnoEINVAL;
      }
    }
  }



  /* Allocate memory */
  if ( (error = hostAllocVmPages(vm, registerMemMsg)) != 0 ) {
    hostOSPrint("plex86: allocVmPages failed at %u\n",
                error);
    return -Plex86ErrnoENOMEM;
    }

  /* Initialize the guests physical memory. */
  if ( hostInitGuestPhyMem(vm) ) {
    hostUnallocVmPages(vm);
    return -Plex86ErrnoEFAULT;
    }

  /* Initialize the monitor. */
  if ( !hostInitMonitor(vm) ||
       !hostMapMonitor(vm) ) {
    hostUnallocVmPages(vm);
    return -Plex86ErrnoEFAULT;
    }
  return 0;
}



/*
 * Allocate various pages/memory needed by monitor.
 */

  int
hostAllocVmPages(vm_t *vm, plex86IoctlRegisterMem_t *registerMemMsg)
{
  vm_pages_t *pg = &vm->pages;
  vm_addr_t  *ad = &vm->host.addr;
#warning "Fix these shortcuts"
  unsigned where = 1;

  /* clear out allocated pages lists */
  mon_memzero(pg, sizeof(*pg));
  mon_memzero(ad, sizeof(*ad));

  /* Guest physical memory pages */
  pg->guest_n_megs  = registerMemMsg->nMegs;
  pg->guest_n_pages = registerMemMsg->nMegs * 256;
  pg->guest_n_bytes = registerMemMsg->nMegs * 1024 * 1024;
  if ( pg->guest_n_pages > MAX_MON_GUEST_PAGES) {
    /* The size of the user-space allocated guest physical memory must
     * fit within the maximum number of guest pages that the VM monitor
     * supports.
     */
    goto error;
    }
  where++;

  vm->guestPhyMemAddr = registerMemMsg->guestPhyMemVector;
  vm->vmState |= VMStateRegisteredPhyMem; /* Bogus for now. */
  where++;

  {
  Bit32u hostPPI, kernelAddr;

  /* Guest CPU state (malloc()'d in user space). */
  if ( !hostOSGetAndPinUserPage(vm, registerMemMsg->guestCPUWindow,
            &pg->guest_cpu_hostOSPtr, &hostPPI, &kernelAddr) ) {
    goto error;
    }
  ad->guest_cpu = (guest_cpu_t *) kernelAddr;
  pg->guest_cpu = hostPPI;
vm->vmState |= VMStateRegisteredGuestCPU; /* For now. */
  where++;

  /* Log buffer area (malloc()'d in user space). */
  /* LOG_BUFF_PAGES */
  if ( !hostOSGetAndPinUserPage(vm, registerMemMsg->logBufferWindow,
            &pg->log_buffer_hostOSPtr[0], &hostPPI, &kernelAddr) ) {
    goto error;
    }
  ad->log_buffer = (Bit8u *) kernelAddr;
  pg->log_buffer[0] = hostPPI;
  where++;
vm->vmState |= VMStateRegisteredPrintBuffer; /* For now. */
  }


  /* Monitor page directory */
  if ( !(ad->page_dir = (pageEntry_t *) hostOSAllocZeroedPage()) ) {
    goto error;
    }
  where++;
  if (!(pg->page_dir = hostOSGetAllocedPagePhyPage(ad->page_dir))) {
    goto error;
    }
  where++;

  /* Monitor page tables */
  if ( !(ad->page_tbl = hostOSAllocZeroedMem(4096 * MON_PAGE_TABLES)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->page_tbl, MON_PAGE_TABLES, 
           ad->page_tbl, 4096 * MON_PAGE_TABLES)) {
    goto error;
    }
  where++;

  /* Map of the linear addresses of page tables currently */
  /* mapped into the monitor space. */
  if ( !(ad->page_tbl_laddr_map = (unsigned *) hostOSAllocZeroedPage()) ) {
    goto error;
    }
  where++;
  if ( !(pg->page_tbl_laddr_map =
         hostOSGetAllocedPagePhyPage(ad->page_tbl_laddr_map)) ) {
    goto error;
    }
  where++;

  /* Nexus page table */
  if ( !(ad->nexus_page_tbl = (page_t *) hostOSAllocZeroedPage()) ) {
    goto error;
    }
  where++;
  if ( !(pg->nexus_page_tbl = hostOSGetAllocedPagePhyPage(ad->nexus_page_tbl)) ) {
    goto error;
    }
  where++;

  /* Transition page table */
  if ( !(ad->transition_PT = (page_t *) hostOSAllocZeroedPage()) ) {
    goto error;
    }
  where++;
  if ( !(pg->transition_PT = hostOSGetAllocedPagePhyPage(ad->transition_PT)) ) {
    goto error;
    }
  where++;

  /* Nexus page */
  if ( !(ad->nexus = (nexus_t *) hostOSAllocZeroedPage()) ) {
    goto error;
    }
  where++;
  if ( !(pg->nexus = hostOSGetAllocedPagePhyPage(ad->nexus)) ) {
    goto error;
    }
  where++;

  /* Monitor IDT */
  if ( !(ad->idt = hostOSAllocZeroedMem(MON_IDT_PAGES*4096)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->idt, MON_IDT_PAGES, ad->idt, MON_IDT_SIZE)) {
    goto error;
    }
  where++;

  /* Monitor GDT */
  if ( !(ad->gdt = hostOSAllocZeroedMem(MON_GDT_PAGES*4096)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->gdt, MON_GDT_PAGES, ad->gdt, MON_GDT_SIZE)) {
    goto error;
    }
  where++;

  /* Monitor LDT */
  if ( !(ad->ldt = hostOSAllocZeroedMem(MON_LDT_PAGES*4096)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->ldt, MON_LDT_PAGES, ad->ldt, MON_LDT_SIZE)) {
    goto error;
    }
  where++;

  /* Monitor TSS */
  if ( !(ad->tss = hostOSAllocZeroedMem(MON_TSS_PAGES*4096)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->tss, MON_TSS_PAGES, ad->tss, MON_TSS_SIZE)) {
    goto error;
    }
  where++;

  /* Monitor IDT stubs */
  if ( !(ad->idt_stubs = hostOSAllocZeroedMem(MON_IDT_STUBS_PAGES*4096)) ) {
    goto error;
    }
  where++;
  if (!hostOSGetAllocedMemPhyPages(pg->idt_stubs, MON_IDT_STUBS_PAGES, 
           ad->idt_stubs, MON_IDT_STUBS_SIZE)) {
    goto error;
    }
  where++;

  /* Get the physical pages associated with the vm_t structure. */
  if (!hostOSGetAllocedMemPhyPages(pg->vm, MAX_VM_STRUCT_PAGES, vm, sizeof(*vm))) {
    goto error;
    }
  where++;

  vm->vmState |= VMStateMemAllocated;
  return 0; /* OK. */

 error:
    hostUnallocVmPages( vm );
    return( where );
}


/* */
/* Unallocate pages/memory used by monitor */
/* */

  void
hostUnallocVmPages( vm_t *vm )
{
  vm_pages_t *pg = &vm->pages;
  vm_addr_t  *ad = &vm->host.addr;

  /* Guest physical memory pages */
  if (vm->guestPhyMemAddr) {
    hostReleasePinnedUserPages(vm);
    vm->guestPhyMemAddr = 0;
    }
  vm->vmState &= ~VMStateRegisteredPhyMem; /* Bogus for now. */

  /* Monitor page directory */
  if (ad->page_dir) hostOSFreePage(ad->page_dir);

  /* Monitor page tables */
  if (ad->page_tbl) hostOSFreeMem(ad->page_tbl);

  /* Map of linear addresses of page tables mapped into monitor. */
  if (ad->page_tbl_laddr_map) hostOSFreePage(ad->page_tbl_laddr_map);

  /* Nexus page table */
  if (ad->nexus_page_tbl) hostOSFreePage(ad->nexus_page_tbl);

  /* Guest CPU state. */
  if (ad->guest_cpu) hostOSFreePage(ad->guest_cpu);

  /* Transition page table */
  if (ad->transition_PT) hostOSFreePage(ad->transition_PT);

  if (ad->log_buffer) hostOSFreeMem(ad->log_buffer);

  /* Nexus page */
  if (ad->nexus) hostOSFreePage(ad->nexus);

  /* Monitor IDT */
  if (ad->idt) hostOSFreeMem(ad->idt);

  /* Monitor GDT */
  if (ad->gdt) hostOSFreeMem(ad->gdt);

  /* Monitor LDT */
  if (ad->ldt) hostOSFreeMem(ad->ldt);

  /* Monitor TSS */
  if (ad->tss) hostOSFreeMem(ad->tss);

  /* Monitor IDT stubs */
  if (ad->idt_stubs) hostOSFreeMem(ad->idt_stubs);


  /* clear out allocated pages lists */
  mon_memzero(pg, sizeof(*pg));
  mon_memzero(ad, sizeof(*ad));
}

  unsigned
hostGetCpuCapabilities(void)
{
  Bit32u eax, ebx, ecx, edx;

  /* Get the highest allowed cpuid level */
  asm volatile (
    "xorl %%eax,%%eax\n\t"
    "cpuid"
    : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
    :
    : "cc"
    );
  if (eax < 1)
    return(0); /* not enough capabilities */

  /* Copy vendor string. */
  hostCpuIDInfo.vendorDWord0 = ebx;
  hostCpuIDInfo.vendorDWord1 = edx;
  hostCpuIDInfo.vendorDWord2 = ecx;

  /* CPUID w/ EAX==1: Processor Signature & Feature Flags */
  asm volatile (
    "movl $1,%%eax\n\t"
    "cpuid"
    : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
    :
    : "cc"
    );
  hostCpuIDInfo.procSignature.raw = eax;
  hostCpuIDInfo.featureFlags.raw = edx;
  /* Plex86 needs TSC */
  if (hostCpuIDInfo.featureFlags.fields.tsc==0)
    return(0);

  return(1);
}


/* Map the monitor and guest into the VM. */

  unsigned
hostMapMonitor(vm_t *vm)
{
  selector_t monCsSel, monSsSel, monTssSel;
  Bit32u laddr, base;
  unsigned slot;
  guest_context_t *guestContext;
  nexus_t *nexus;
  descriptor_t *gdt;

  /* For convenience, some pointers. */
  guestContext = vm->host.addr.guest_context;
  nexus        = vm->host.addr.nexus;
  gdt          = vm->host.addr.gdt;

#warning "Is the GDT being cleared of old values?"
/* +++ should zero out GDT, so prev entries do not remain */

  /* =========================
   * Map in Monitor structures
   * =========================
   */

  /* CS/SS/TSS selectors:
   * For now, hardcode in monitor descriptors at slots 1,2,3.  As we
   * are only running user code in the VM, these are likely safe slots
   * as they are often used guest OSes for kernel descriptors.
   */
  monCsSel.raw     = Selector(1, 0, RPL0);
  monSsSel.raw     = Selector(2, 0, RPL0);
  monTssSel.raw    = Selector(3, 0, RPL0);

  /* Search for unused PDE for nexus PT  (fixed for now) */
  laddr = 0x70000000;
  vm->mon_pde_mask = laddr & 0xffc00000;
  vm->mon_pdi      = laddr >> 22;
  base = MON_BASE_FROM_LADDR(laddr);

  /* Map nexus into monitor/guest address space */
  vm->host.addr.page_dir[laddr >> 22] = vm->host.nexus_pde;

  /* CS/SS/TSS descriptors: Put at fixed GDT location for now. */
  SET_DESCRIPTOR(gdt[monCsSel.fields.index], base, 0xfffff, 
                 D_PG, D_D32, D_AVL0, D_PRESENT, D_DPL0, D_CODE | D_READ)
  SET_DESCRIPTOR(gdt[monSsSel.fields.index], base, 0xfffff, 
                 D_PG, D_D32, D_AVL0, D_PRESENT, D_DPL0, D_DATA | D_WRITE)
  SET_DESCRIPTOR(gdt[monTssSel.fields.index],
                 base + (Bit32u) vm->guest.addr.tss,
                 sizeof(tss_t)-1,
                 D_BG, 0, D_AVL0, D_PRESENT, D_DPL0, D_TSS)


  /* Fix up the selectors of all IDT entries. */
  for ( slot = 0; slot < 256; slot++ )
      vm->host.addr.idt[slot].selector = monCsSel;

  /* The monitor GDT/IDT loading info. */
  nexus->mon_gdt_info.base  = base + (Bit32u) vm->guest.addr.gdt;
  nexus->mon_gdt_info.limit = MON_GDT_SIZE;
  nexus->mon_idt_info.base  = base + (Bit32u) vm->guest.addr.idt;
  nexus->mon_idt_info.limit = MON_IDT_SIZE;

  /* We don't have a monitor LDT for now. */
  nexus->mon_ldt_sel = 0;

  /* The monitor TSS. */
  nexus->mon_tss_sel = monTssSel.raw;
  vm->host.addr.tss->esp0 = ((Bit32u)vm->guest.addr.nexus) + PAGESIZE;
  vm->host.addr.tss->ss0  = monSsSel.raw;

  /* Monitor code and stack segments. */
  nexus->mon_jmp_info.selector   = monCsSel.raw;
  nexus->mon_stack_info.selector = monSsSel.raw;

  /* Monitor code/data segment base. */
  nexus->mon_base = base;

  vm->vmState |= VMStateMapMonitor;
  return(1);
}

  void
hostInitShadowPaging(vm_t *vm)
{
  pageEntry_t *monPDir;
  Bit32u pdi;
/*Bit32u cr3_page_index;*/
/*phy_page_usage_t *pusage;*/

#if 0
  cr3_page_index = A20Addr(vm, vm->guest_cpu.cr3) >> 12;
  if ( cr3_page_index >= vm->pages.guest_n_pages)
    xxxpanic(vm, "monPagingRemap: CR3 conflicts with monitor space\n");
#endif

  /* Reset page table heap */
  vm->ptbl_laddr_map_i = 0;

  /* Clear monitor PD except 4Meg range used by monitor */
  monPDir = vm->host.addr.page_dir;
  for (pdi=0; pdi<1024; pdi++) {
#if ANAL_CHECKS
    vm->host.addr.page_tbl_laddr_map[pdi] = -1; /* max unsigned */
#endif
    if (pdi != vm->mon_pdi)
      monPDir[pdi].raw = 0;
    }

  /* Update vpaging timestamp. */
  vm->vpaging_tsc = vm_rdtsc();

#if 0
  /* When we remap the monitor page tables, IF guest paging is
   * enabled, then mark the page containing the guest page directory
   * as such.  In non-paged mode, there is no page directory.
   */
  if (vm->guest_cpu.cr0.fields.pg) {
    pusage = &vm->pageInfo[cr3_page_index];
    pusage->tsc = vm->vpaging_tsc;
    pusage->attr.raw &= PageUsageSticky;
    pusage->attr.raw |= PageUsagePDir;
    pusage->attr.fields.access_perm = PagePermNA;
    if (pusage->attr.raw & PageBadUsage4PDir)
      xxxpanic(vm, "monPagingRemap: BadUsage4PDir\n");
    }
#endif
}


  void
hostReleasePinnedUserPages(vm_t *vm)
{
  unsigned ppi;
  unsigned dirty;
  unsigned nPages;
  Bit32u kernelAddr;

  /* Unpin the pages associate with the guest physical memory. */
  nPages = vm->pages.guest_n_pages;
  for (ppi=0; ppi<nPages; ppi++) {
    if ( vm->pageInfo[ppi].attr.fields.pinned ) {
      void *osSpecificPtr;

      osSpecificPtr = (void *) vm->hostStructPagePtr[ppi];
#warning "Conditionalize page dirtying before page release."
      dirty = 1; /* FIXME: 1 for now. */
      hostOSUnpinUserPage(vm,
          vm->guestPhyMemAddr + (ppi<<12),
          osSpecificPtr,
          ppi,
          0 /* There was no host kernel addr mapped for this page. */,
          dirty);
      vm->pageInfo[ppi].attr.fields.pinned = 0;
      }
    }

  /* Unpin the pages associated with the guest_cpu area. */
  kernelAddr = (Bit32u) vm->host.addr.guest_cpu;
  hostOSUnpinUserPage(vm,
      0, /* User space address. */
      vm->pages.guest_cpu_hostOSPtr,
      vm->pages.guest_cpu,
      &kernelAddr,
      1 /* Dirty. */);

  /* Unpin the pages associated with the log buffer area. */
  kernelAddr = (Bit32u) vm->host.addr.log_buffer;
  hostOSUnpinUserPage(vm,
      0, /* User space address. */
      vm->pages.log_buffer_hostOSPtr[0],
      vm->pages.log_buffer[0],
      &kernelAddr,
      1 /* Dirty. */);
#warning "User space address is passed as 0 for now..."
}

  unsigned
hostHandlePagePinRequest(vm_t *vm, Bit32u reqGuestPPI)
{
  Bit32u hostPPI;
  unsigned qIndex;

#warning "We must not unpin open pages (for page walking) here."
  if (vm->guestPhyPagePinQueue.nEntries < MaxPhyPagesPinned) {
    /* There is room in the Q for another entry - we have not reached
     * the upper limit of allowable number of pinned pages.
     */
    qIndex = vm->guestPhyPagePinQueue.nEntries;
    }
  else {
    unsigned dirty;
    Bit32u unpinGuestPPI;
    /* There is no room in the Q for another entry - we have reached
     * the upper limit of allowable number of pinned pages.  We must
     * first unpin a page to free up the limit, then we can pin the
     * requested page.  This keeps plex86 from pinning an unconstrained
     * number of pages at one time.
     */
    qIndex = vm->guestPhyPagePinQueue.tail;
    dirty = 1; /* FIXME: 1 for now. */
    unpinGuestPPI = vm->guestPhyPagePinQueue.ppi[qIndex];
    hostOSUnpinUserPage(vm,
        vm->guestPhyMemAddr + (unpinGuestPPI<<12),
        vm->hostStructPagePtr[unpinGuestPPI],
        unpinGuestPPI,
        0 /* There was no host kernel addr mapped for this page. */,
        dirty);
    vm->pageInfo[unpinGuestPPI].attr.fields.pinned = 0;
    }

  /* Pin the requested guest physical page in the host OS. */
  if ( !hostOSGetAndPinUserPage(vm,
            vm->guestPhyMemAddr + (reqGuestPPI<<12),
            &vm->hostStructPagePtr[reqGuestPPI],
            &hostPPI,
            0 /* Don't need a host kernel address. */
            ) ) {
    hostOSPrint("handlePagePinReq: request to pin failed.\n");
    return(0); /* Fail. */
    }

  /* Pinning activities have succeeded.  Mark this physical page as being
   * pinnned, and store it's physical address.
   */
  vm->pageInfo[reqGuestPPI].attr.fields.pinned = 1;
  vm->pageInfo[reqGuestPPI].hostPPI = hostPPI;

  /* Now add this entry to the Q. */
  vm->guestPhyPagePinQueue.ppi[qIndex] = reqGuestPPI;

  if (vm->guestPhyPagePinQueue.nEntries < MaxPhyPagesPinned) {
    vm->guestPhyPagePinQueue.nEntries++;
    vm->guestPhyPagePinQueue.tail =
        vm->guestPhyPagePinQueue.nEntries % MaxPhyPagesPinned;
    }
  else {
    /* Leave .nEntries at the maximum value - Q is full. */
    vm->guestPhyPagePinQueue.tail =
        (vm->guestPhyPagePinQueue.tail + 1) % MaxPhyPagesPinned;
    }

  return(1); /* OK. */
}
