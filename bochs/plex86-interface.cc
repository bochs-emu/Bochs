/////////////////////////////////////////////////////////////////////////
//// $Id: plex86-interface.cc,v 1.1 2003-01-01 17:34:09 kevinlawton Exp $
///////////////////////////////////////////////////////////////////////////
////
////  Copyright (C) 2002  Kevin P. Lawton
////
////  This library is free software; you can redistribute it and/or
////  modify it under the terms of the GNU Lesser General Public
////  License as published by the Free Software Foundation; either
////  version 2 of the License, or (at your option) any later version.
////
////  This library is distributed in the hope that it will be useful,
////  but WITHOUT ANY WARRANTY; without even the implied warranty of
////  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
////  Lesser General Public License for more details.
////
////  You should have received a copy of the GNU Lesser General Public
////  License along with this library; if not, write to the Free Software
////  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#include "bochs.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "plex86-interface.h"

#define LOG_THIS genlog->


unsigned     plex86State = 0;
int          plex86FD = -1;
Bit8u       *plex86MemPtr = 0;
size_t       plex86MemSize = 0;
Bit8u       *plex86PrintBuffer = 0;
guest_cpu_t *plex86GuestCPU = 0;

static void copyPlex86StateToBochs(BX_CPU_C *cpu);
static void copyBochsDescriptorToPlex86(descriptor_t *, bx_descriptor_t *);
static void copyPlex86DescriptorToBochs(BX_CPU_C *,
                                        bx_descriptor_t *, descriptor_t *);

static unsigned faultCount[32];


  int
plex86Open(void)
{
  cpuid_info_t bochsCPUID;

  if (plex86State) {
    // This should be the first operation; no state should be set yet.
    fprintf(stderr, "plex86Open: plex86State = 0x%x\n", plex86State);
    return(-1); // Error.
    }

  // Open a new VM.
  fprintf(stderr, "Opening VM.\n");
  fprintf(stderr, "Trying /dev/misc/plex86...");
  plex86FD = open("/dev/misc/plex86", O_RDWR);
  if (plex86FD >= 0) {
    fprintf(stderr, "OK.\n");
    }
  else {
    fprintf(stderr, "failed.\n");
    // Try the old name.
    fprintf(stderr, "Trying /dev/plex86...");
    plex86FD = open("/dev/plex86", O_RDWR);
    if (plex86FD >= 0) {
      fprintf(stderr, "OK.\n");
      }
    else {
      fprintf(stderr, "failed.\n");
      fprintf(stderr, "Did you load the kernel module?"
              "  Read the toplevel README file!\n");
      perror ("open");
      return(-1); // Error.
      }
    }

  fprintf(stderr, "Zeroing guest CPUID info (complete this).\n");
  memset(&bochsCPUID, 0, sizeof(bochsCPUID));
// xxx Fill in CPUID info here.

  fprintf(stderr, "Passing guest CPUID to plex86.\n");
  if ( ioctl(plex86FD, PLEX86_CPUID, &bochsCPUID) ) {
    perror("ioctl CPUID: ");
    return(-1); // Error.
    }

  return(plex86FD); // File descriptor is return val.
}

  Bit8u *
plex86AllocateMemory(unsigned nMegs)
{
  Bit8u *ptr;

  plex86MemSize = nMegs * 1024 * 1024;

  if (plex86FD < 0) {
    fprintf(stderr, "plex86AllocateMemory: FD not open.\n");
    return(0);
    }

  // Allocate memory from the host OS for the virtual physical memory.
  fprintf(stderr, "Allocating %dMB of physical memory in VM.\n", nMegs);
  if (ioctl(plex86FD, PLEX86_ALLOCVPHYS, nMegs) == -1) {
    perror("ioctl ALLOCVPHYS: ");
    plex86TearDown();
    return(0);
    }
  plex86State |= Plex86StateMemAllocated;

  // Map guest virtual physical memory into user address space and zero it.
  fprintf(stderr, "Mapping virtualized physical memory into monitor.\n");
  ptr = (Bit8u*) mmap(NULL, plex86MemSize, PROT_READ | PROT_WRITE,
      MAP_SHARED, plex86FD, 0);
  if (ptr == (void *) -1) {
    perror("mmap of physical pages");
    plex86TearDown();
    return(0);
    }
  plex86State |= Plex86StateMMapPhyMem;

  plex86MemPtr = ptr;
  // No need to zero physical memory.  It's zeroed by the plex86 module.

  // Create a memory mapping of the monitor's print buffer into
  // user memory.  This is used for efficient printing of info that
  // the monitor prints out.
  fprintf(stderr, "Mapping monitor print buffer into user mem.\n");
  plex86PrintBuffer = (Bit8u*) mmap(NULL, 4096, PROT_READ,
      MAP_SHARED, plex86FD, plex86MemSize + 0*4096);
  if (plex86PrintBuffer == (void *) -1) {
    perror("mmap of monitor print buffer");
    plex86TearDown();
    return(0);
    }
  plex86State |= Plex86StateMMapPrintBuffer;

  // Create a memory mapping of the monitor's guest_cpu structure into
  // user memory.  This is used for passing the guest_cpu state between
  // user and kernel/monitor space.
  fprintf(stderr, "Mapping guest_cpu structure into user mem.\n");
  plex86GuestCPU = (guest_cpu_t *) mmap(NULL, 4096, PROT_READ | PROT_WRITE,
      MAP_SHARED, plex86FD, plex86MemSize + 1*4096);
  if (plex86GuestCPU == (void *) -1) {
    perror("mmap of guest_cpu structure");
    plex86TearDown();
    return(0);
    }
  plex86State |= Plex86StateMMapGuestCPU;
  // No need to zero guest_cpu structure.  Kernel module does that.

  return(ptr);
}

  unsigned
plex86TearDown(void)
{
  fprintf(stderr, "plex86TearDown called.\n");

  fprintf(stderr, "Guest Fault Count (FYI):\n");
  for (unsigned f=0; f<32; f++) {
    if (faultCount[f])
      fprintf(stderr, "FC[%u] = %u\n", f, faultCount[f]);
    }

  if ( plex86FD < 0 ) {
    fprintf(stderr, "plex86TearDown: FD not open.\n");
    return(0);
    }

  if ( plex86State & Plex86StateMMapPhyMem ) {
    fprintf(stderr, "Unmapping guest physical memory.\n");
    if (munmap(plex86MemPtr, plex86MemSize) != 0) {
      perror ("munmap of guest physical memory");
      return(0); // Failed.
      }
    }
  plex86State &= ~Plex86StateMMapPhyMem;

  if ( plex86State & Plex86StateMMapPrintBuffer ) {
    fprintf(stderr, "Unmapping print buffer.\n");
    if (munmap(plex86PrintBuffer, 4096) != 0) {
      perror("munmap of print buffer.");
      return(0); // Failed.
      }
    }
  plex86State &= ~Plex86StateMMapPrintBuffer;

  if ( plex86State & Plex86StateMMapGuestCPU ) {
    fprintf(stderr, "Unmapping guest_cpu structure.\n");
    if (munmap(plex86GuestCPU, 4096) != 0) {
      perror("munmap of guest_cpu structure.");
      return(0); // Failed.
      }
    }
  plex86State &= ~Plex86StateMMapGuestCPU;

  fprintf(stderr, "Tearing down VM.\n");
  if (ioctl(plex86FD, PLEX86_TEARDOWN, 0) == -1) {
    perror("ioctl TEARDOWN: ");
    return(0); // Failed.
    }
  plex86State &= ~Plex86StateMemAllocated;

  // Close the connection to the kernel module.
  fprintf(stderr, "Closing VM device.\n");
  if (close(plex86FD) == -1) {
    perror("close of VM device\n");
    return(0); // Failed.
    }

  plex86State = 0; // For good measure.

  return(1); // OK.
}

  unsigned
plex86ExecuteInVM(BX_CPU_C *cpu)
{
  plex86IoctlExecute_t executeMsg;
  int ret;

  if ( plex86State != Plex86StateReady ) {
    fprintf(stderr, "plex86ExecuteInVM: not in ready state (0x%x)\n",
            plex86State);
    BX_PANIC(("plex86ExecuteInVM: bailing"));
    return(0);
    }

  executeMsg.executeMethod = Plex86ExecuteMethodNative;
  plex86GuestCPU->edi = cpu->gen_reg[BX_32BIT_REG_EDI].dword.erx;
  plex86GuestCPU->esi = cpu->gen_reg[BX_32BIT_REG_ESI].dword.erx;
  plex86GuestCPU->ebp = cpu->gen_reg[BX_32BIT_REG_EBP].dword.erx;
  plex86GuestCPU->esp = cpu->gen_reg[BX_32BIT_REG_ESP].dword.erx;
  plex86GuestCPU->ebx = cpu->gen_reg[BX_32BIT_REG_EBX].dword.erx;
  plex86GuestCPU->edx = cpu->gen_reg[BX_32BIT_REG_EDX].dword.erx;
  plex86GuestCPU->ecx = cpu->gen_reg[BX_32BIT_REG_ECX].dword.erx;
  plex86GuestCPU->eax = cpu->gen_reg[BX_32BIT_REG_EAX].dword.erx;

  plex86GuestCPU->eflags = cpu->eflags.val32;
  plex86GuestCPU->eip = cpu->dword.eip;

  // ES/CS/SS/DS/FS/GS
  for (unsigned s=0; s<6; s++) {
    plex86GuestCPU->sreg[s].sel.raw = cpu->sregs[s].selector.value;
    copyBochsDescriptorToPlex86(&plex86GuestCPU->sreg[s].des,
                                &cpu->sregs[s].cache);
    plex86GuestCPU->sreg[s].valid = cpu->sregs[s].cache.valid;
    }

  // LDTR
  plex86GuestCPU->ldtr.sel.raw = cpu->ldtr.selector.value;
  copyBochsDescriptorToPlex86(&plex86GuestCPU->ldtr.des, &cpu->ldtr.cache);
  plex86GuestCPU->ldtr.valid = cpu->ldtr.cache.valid;

  // TR
  plex86GuestCPU->tr.sel.raw = cpu->tr.selector.value;
  copyBochsDescriptorToPlex86(&plex86GuestCPU->tr.des, &cpu->tr.cache);
  plex86GuestCPU->tr.valid = cpu->tr.cache.valid;

  // GDTR/IDTR
  plex86GuestCPU->gdtr.base  = cpu->gdtr.base;
  plex86GuestCPU->gdtr.limit = cpu->gdtr.limit;
  plex86GuestCPU->idtr.base  = cpu->idtr.base;
  plex86GuestCPU->idtr.limit = cpu->idtr.limit;

  plex86GuestCPU->dr0 = cpu->dr0;
  plex86GuestCPU->dr1 = cpu->dr1;
  plex86GuestCPU->dr2 = cpu->dr2;
  plex86GuestCPU->dr3 = cpu->dr3;
  plex86GuestCPU->dr6 = cpu->dr6;
  plex86GuestCPU->dr7 = cpu->dr7;

  plex86GuestCPU->tr3 = 0; // Unimplemented in bochs.
  plex86GuestCPU->tr4 = 0; // Unimplemented in bochs.
  plex86GuestCPU->tr5 = 0; // Unimplemented in bochs.
  plex86GuestCPU->tr6 = 0; // Unimplemented in bochs.
  plex86GuestCPU->tr7 = 0; // Unimplemented in bochs.

  plex86GuestCPU->cr0.raw = cpu->cr0.val32;
  plex86GuestCPU->cr1     = cpu->cr1;
  plex86GuestCPU->cr2     = cpu->cr2;
  plex86GuestCPU->cr3     = cpu->cr3;
  plex86GuestCPU->cr4.raw = cpu->cr4.registerValue;

  plex86GuestCPU->a20Enable = BX_GET_ENABLE_A20();

  ret = ioctl(plex86FD, PLEX86_EXECUTE, &executeMsg);
  if (ret < 0) {
    fprintf(stderr, "ioctl(PLEX86_EXECUTE) returns < 0\n");
    }
  else

  switch ( executeMsg.monitorState.request ) {
    case MonReqFlushPrintBuf:
      fprintf(stderr, "MonReqFlushPrintBuf:\n");
      fprintf(stderr, "::%s\n", plex86PrintBuffer);
      break;
    case MonReqPanic:
      fprintf(stderr, "MonReqPanic:\n");
      fprintf(stderr, "::%s\n", plex86PrintBuffer);
      break;
    //case MonReqNone:
    //  copyPlex86StateToBochs(cpu);
    //  return(0); /* All OK. */
    case MonReqGuestFault:
      faultCount[ executeMsg.monitorState.guestFaultNo ]++;
      copyPlex86StateToBochs(cpu);
      return(0); /* All OK. */
    default:
      fprintf(stderr, "executeMsg.request = %u\n",
              executeMsg.monitorState.request);
      break;
    }

  plex86TearDown();
  BX_PANIC(("plex86ExecuteInVM: bailing"));

  return(0);
}

  void
copyPlex86StateToBochs(BX_CPU_C *cpu)
{
  cpu->gen_reg[BX_32BIT_REG_EDI].dword.erx = plex86GuestCPU->edi;
  cpu->gen_reg[BX_32BIT_REG_ESI].dword.erx = plex86GuestCPU->esi;
  cpu->gen_reg[BX_32BIT_REG_EBP].dword.erx = plex86GuestCPU->ebp;
  cpu->gen_reg[BX_32BIT_REG_ESP].dword.erx = plex86GuestCPU->esp;
  cpu->gen_reg[BX_32BIT_REG_EBX].dword.erx = plex86GuestCPU->ebx;
  cpu->gen_reg[BX_32BIT_REG_EDX].dword.erx = plex86GuestCPU->edx;
  cpu->gen_reg[BX_32BIT_REG_ECX].dword.erx = plex86GuestCPU->ecx;
  cpu->gen_reg[BX_32BIT_REG_EAX].dword.erx = plex86GuestCPU->eax;

  cpu->eflags.val32 = plex86GuestCPU->eflags;
  cpu->dword.eip    = plex86GuestCPU->eip;

  // Set fields used for exception processing.
  cpu->prev_eip = plex86GuestCPU->eip;
  cpu->prev_esp = plex86GuestCPU->esp;

  // ES/CS/SS/DS/FS/GS
  for (unsigned s=0; s<6; s++) {
    cpu->sregs[s].selector.value = plex86GuestCPU->sreg[s].sel.raw;
    cpu->sregs[s].cache.valid    = plex86GuestCPU->sreg[s].valid;
    if ( (cpu->sregs[s].selector.value & 0xfffc) == 0 ) {
      /* Null selector. */
      if ( cpu->sregs[s].cache.valid ) {
        plex86TearDown();
        BX_PANIC(("copyPlex86StateToBochs: null descriptor [%u] "
                  "with descriptor cache valid bit set.", s));
        }
      /* valid bit == 0, invalidates a bochs descriptor cache. */
      }
    else {
      /* Non-null selector. */
      if ( cpu->sregs[s].cache.valid==0 ) {
        plex86TearDown();
        BX_PANIC(("copyPlex86StateToBochs: non-null descriptor [%u] "
                  "with descriptor cache valid bit clear.", s));
        }
      copyPlex86DescriptorToBochs(cpu, &cpu->sregs[s].cache,
          &plex86GuestCPU->sreg[s].des);
      }
    }
}

  void
copyBochsDescriptorToPlex86(descriptor_t *plex86Desc, bx_descriptor_t *bochsDesc)
{
  // For now this function is a hack to convert from bochs descriptor
  // cache fields which are parsed out into separate fields, to
  // a packed descriptor format as stored in a real segment descriptor.
  // This is user only for code/data segments and the LDTR/TR.
  // Ideally, bochs would store the 64-bit segment descriptor when
  // it loads segment registers.

  if (bochsDesc->valid == 0) {
    memset(plex86Desc, 0, sizeof(*plex86Desc));
    return;
    }
  plex86Desc->p = bochsDesc->p;
  plex86Desc->dpl = bochsDesc->dpl;
  plex86Desc->type = (bochsDesc->segment<<4) | bochsDesc->type;
  if (bochsDesc->segment) {
    // Code/Data segment type.
    Bit32u limit = bochsDesc->u.segment.limit;
    plex86Desc->limit_low  = limit; // Only lower 16-bits.
    plex86Desc->limit_high = limit >> 16;
    Bit32u base = bochsDesc->u.segment.base;
    plex86Desc->base_low  = base;
    plex86Desc->base_med  = base >> 16;
    plex86Desc->base_high = base >> 24;
    plex86Desc->avl = bochsDesc->u.segment.avl;
    plex86Desc->reserved = 0;
    plex86Desc->d_b = bochsDesc->u.segment.d_b;
    plex86Desc->g   = bochsDesc->u.segment.g;
    }
  else if (bochsDesc->type == 2) {
    // LDT descriptor.
    plex86Desc->limit_low  = bochsDesc->u.ldt.limit;
    plex86Desc->limit_high = 0;
    Bit32u base = bochsDesc->u.ldt.base;
    plex86Desc->base_low  = base;
    plex86Desc->base_med  = base >> 16;
    plex86Desc->base_high = base >> 24;
    plex86Desc->avl = 0;
    plex86Desc->reserved = 0;
    plex86Desc->d_b = 0;
    plex86Desc->g   = 0;
    }
  else if ( (bochsDesc->type == 9) || (bochsDesc->type==1) ) {
    // TSS
    Bit32u limit = bochsDesc->u.tss386.limit;
    plex86Desc->limit_low  = limit; // Only lower 16-bits.
    plex86Desc->limit_high = limit >> 16;
    Bit32u base = bochsDesc->u.tss386.base;
    plex86Desc->base_low  = base;
    plex86Desc->base_med  = base >> 16;
    plex86Desc->base_high = base >> 24;
    plex86Desc->avl = bochsDesc->u.tss386.avl;
    plex86Desc->reserved = 0;
    plex86Desc->d_b = 0;
    plex86Desc->g   = bochsDesc->u.tss386.g;
    }
  else {
    BX_PANIC(("copyBochsDescriptorToPlex86: desc type = %u.",
              bochsDesc->type));
    }
}

  void
copyPlex86DescriptorToBochs(BX_CPU_C *cpu,
                            bx_descriptor_t *bochsDesc, descriptor_t *plex86Desc)
{
  Bit32u dword1, dword2, *dwordPtr;
  dwordPtr = (Bit32u *) plex86Desc;

  /* We can assume little endian, since we're running an x86 VM. */
  dword1 = dwordPtr[0];
  dword2 = dwordPtr[1];
  cpu->parse_descriptor(dword1, dword2, bochsDesc);
}
