/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  monitor.h: main VM monitor defines
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

#ifndef __MONITOR_H__
#define __MONITOR_H__

#if defined(__NetBSD__) || defined(__FreeBSD__)
#include <machine/stdarg.h>
#else
#include <stdarg.h>
#endif


#include "descriptor.h"
#include "descriptor2.h"
#include "tss.h"
#include "paging.h"
#include "eflags.h"
#include "guest_context.h"

#ifndef UNUSED
#  define UNUSED(x) ((void)(x))
#endif


/* Method1: push event info (CPU pushes error code before) */
typedef struct 
{
  Bit8u  pushl;    /* Always 0x68 == pushl            */
  Bit32u vector;   /* Interrupt vector number         */
  Bit8u  jmp;      /* Always 0xe9 == jmp              */
  Bit32u reloc;    /* Relative offset of destination  */
} __attribute__ ((packed)) idt_method1_t;

/* Method2: push a dummy error first, then event info */
typedef struct 
{
  Bit8u  pushla;   /* Always 0x68 == pushl            */
  Bit32u dummy;    /* Dummy error code                */
  Bit8u  pushlb;   /* Always 0x68 == pushl            */
  Bit32u vector;   /* Interrupt vector number         */
  Bit8u  jmp;      /* Always 0xe9 == jmp              */
  Bit32u reloc;    /* Relative offset of destination  */
} __attribute__ ((packed)) idt_method2_t;

typedef union 
{
  idt_method1_t m1;
  idt_method2_t m2;
} idt_stub_t;


/* Nexus fields.  This C structure maps to identical assembly */
/* fields in nexus.S.  Make sure to update both!  These fields */
/* are accessible to the nexus code during the transition from */
/* host<->guest and are stored in a single page. */

typedef struct {
  /* guest pointer to vm_t structure. */
  void          *vm;

  /* These fields are only used by the transition code. */
  /* They hold all info necessary to switch back to the host. */
  gdt_info_t     host_gdt_info;
  gdt_info_t     host_idt_info;
  far_jmp_info_t host_jmp_info;
  far_jmp_info_t host_stack_info;
  Bit16u         host_ldt_sel;
  Bit16u         host_tss_sel;
  Bit32u         host_cr0;
  Bit32u         host_cr2;
  Bit32u         host_cr3;
  Bit32u         host_cr4;

  /* These fields are filled by the host-side code, and used  */
  /* by the transition code.  They contain all info necessary */
  /* to switch to the monitor/guest address space. */
  /* This info changes whenever the monitor migrates. */
  gdt_info_t     mon_gdt_info;
  gdt_info_t     mon_idt_info;
  far_jmp_info_t mon_jmp_info;
  far_jmp_info_t mon_stack_info;
  Bit16u         mon_ldt_sel;
  Bit16u         mon_tss_sel;
  Bit32u         mon_base;
  Bit32u         mon_cr0;
  Bit32u         mon_cr3;
  Bit32u         mon_cr4;
  Bit32u         mon_eflags;

  /* These fields contain info used by the transition code to */
  /* create the temporary identity mapping.  They never change. */
  pageEntry_t    transition_pde;
  pageEntry_t   *transition_pde_p_host;
  pageEntry_t   *transition_pde_p_mon;
  Bit32u         transition_laddr;
} __attribute__ ((packed)) nexus_t;


/* For reference, the following describes where bits from the guest */
/* eflags register are stored/managed. */
/* */
/* Key: */
/*  g: Flag value as requested by guest */
/*  V: Virtualized flag value, as loaded in eflags when guest is executing */
/*  ?: Unhandled yet, request of set bit causes panic for now */
/* */
/*   === ======= ====== ======= ======= ======= */
/*  |I|V|V|A|V|R|0|N|IO|O|D|I|T|S|Z|0|A|0|P|1|C| flag */
/*  |D|I|I|C|M|F| |T|PL|F|F|F|F|F|F| |F| |F| |F| */
/*  | |P|F| | | | | |  | | | | | | | | | | | | | */
/*  |g|?|?|g|V|g|g|g|VV|g|g|V|g|g|g|g|g|g|g|g|g| context->eflags */
/*  | |?|?| |g| | | |gg| | |g| | | | | | | | | | veflags */

/* #define VirtualizedEflags 0x001a3200 */
#define VirtualizedEflags 0x001a3300


/* I define the 'nexus' as the set of data structures which */
/* must exist in the current linear guest address space.  The */
/* host linear address space is not available while the current */
/* guest code is running, since we are using a completely */
/* different set of page mappings for the guest.  However, */
/* at some point an exception/interrupt will occur.  The */
/* interrupt mechanisms require that several structures exist in */
/* the current linear address space in order to service such */
/* an event.  These data structures make up part of our VM, */
/* a thin layer which exists in the guest.  Following is a */
/* list of what data structures compose this 'nexus': */
/* */
/*     - IDT (max  2048 bytes) */
/*     - GDT (max 65536 bytes) */
/*     - LDT (max 65536 bytes) */
/*     - TSS (max  8328 = 104 + 32 int redir + 8192 I/O permissions) */
/*     - kernel stack page */
/*     - transition code (host <--> guest) */
/*     - interrupt handler stubs */
/*     - Page Tables; PDE & PTE pages. */

/*
 * Sizes of various nexus data structures used by the monitor
 */

#define PLEX86_MAX_PHY_MEGS 32
#define PAGESIZE 4096

#define IDT_STUB_SIZE 15
#define BytesToPages(b) ( ((b)+4095) >> 12 )

#define MON_IDT_SIZE       (8*256)
#define MON_GDT_SIZE       (8*512)
#define MON_LDT_SIZE       (8*1)
#define MON_IDT_STUBS_SIZE (IDT_STUB_SIZE*256)
#define MON_TSS_SIZE       (104)

#define MON_IDT_PAGES       BytesToPages(MON_IDT_SIZE)
#define MON_GDT_PAGES       BytesToPages(MON_GDT_SIZE)
#define MON_LDT_PAGES       BytesToPages(MON_LDT_SIZE)
#define MON_IDT_STUBS_PAGES BytesToPages(MON_IDT_STUBS_SIZE)
#define MON_TSS_PAGES       BytesToPages(MON_TSS_SIZE)

#define MAX_MON_GUEST_PAGES    (PLEX86_MAX_PHY_MEGS * 256)
/* +++ MON_PAGE_TABLES is kind of random */
#define MON_PAGE_TABLES    (10*((PLEX86_MAX_PHY_MEGS+3) >> 2))

#define MAX_VM_STRUCT_PAGES (68)

#define LOG_BUFF_PAGES 1
#define LOG_BUFF_SIZE  ((LOG_BUFF_PAGES)*4096)

/*
 *  Pages allocated for the VM by the host kernel driver.
 *  N Megs of physical memory are allocated, per the user's
 *  request, for the guest OS/application code.
 *  Additionally, some other overhead pages are allocated
 *  for structures such as the page directory, page tables,
 *  and other virtualized facilities.
 */

typedef struct {
  /* requested size of the guest[] array in megs and pages */
  unsigned guest_n_megs;
  unsigned guest_n_pages;
  unsigned guest_n_bytes;

  /* pages comprising the vm_t struct itself. */
  Bit32u vm[MAX_VM_STRUCT_PAGES];

  /* for the monitor's page directory */
  Bit32u page_dir;

  /* for the monitor's page table */
  Bit32u page_tbl[MON_PAGE_TABLES];

  /* Map of the linear addresses of page tables currently */
  /* mapped into the monitor space. */
  Bit32u page_tbl_laddr_map;

  /* for the extra page table that maps our nexus code and structures */
  Bit32u nexus_page_tbl;

  /* For the CPU state passed between user and kernel/monitor space. */
  Bit32u guest_cpu;
  void  *guest_cpu_hostOSPtr;

  /* We need a Page Table for identity mapping the transition code */
  /* between host and monitor spaces. */
  Bit32u transition_PT;

  Bit32u log_buffer[LOG_BUFF_PAGES];
  void  *log_buffer_hostOSPtr[LOG_BUFF_PAGES];

  /* Physical addresses of host pages which comprise the actual */
  /* monitor structures.  These will be mapped into the current */
  /* guest task's linear address space as well. */
  Bit32u nexus;
  Bit32u idt[MON_IDT_PAGES];
  Bit32u gdt[MON_GDT_PAGES];
  Bit32u ldt[MON_LDT_PAGES];
  Bit32u tss[MON_TSS_PAGES];
  Bit32u idt_stubs[MON_IDT_STUBS_PAGES];
  } vm_pages_t;


typedef struct {
  pageEntry_t  *page_dir;
  page_t       *page_tbl;
  unsigned     *page_tbl_laddr_map;
  page_t       *nexus_page_tbl;
  guest_cpu_t  *guest_cpu;
  page_t       *transition_PT;
  unsigned char *log_buffer;
  Bit8u        *code_phy_page; /* only use in mon space */
  Bit8u        *tmp_phy_page0; /* only use in mon space */
  Bit8u        *tmp_phy_page1; /* only use in mon space */

  nexus_t      *nexus;
  /* Pointer into the monitor stack, so we can easily retrieve the */
  /* stack snapshot upon interrupt/exception. */
  guest_context_t *guest_context;
  gate_t       *idt;
  descriptor_t *gdt;
  descriptor_t *ldt;
  tss_t        *tss;
  idt_stub_t   *idt_stubs;
  } vm_addr_t;


/* These bits define the possible usage and attributes assigned */
/* to a particular guest physical page.  These are useful for keeping */
/* track of what kinds of system structures are contained in a page */
/* at a given time, and if the page has associated cached code */
/* information in the prescan logic.  We can also tag particular */
/* pages with other more static attributes. */

typedef union {
  struct {
    Bit32u access_perm:2;     /* */
    Bit32u lmap_count:2;      /* */
    Bit32u ptbl:1;            /* page table */
    Bit32u pdir:1;            /* page directory */
    Bit32u spare0:1;          /* (spare) */
    Bit32u memMapIO:1;        /* MemMapIO */
    Bit32u RO:1;              /* RO */
    Bit32u allocated:1;       /* Allocated */
    Bit32u pinned:1;          /* Pinned by host OS. */
    Bit32u spare1:1;          /* (spare) */
    Bit32u laddr_backlink:20; /* 1st unvirtualized laddr backlink */
    } __attribute__ ((packed)) fields;
  Bit32u raw;
  } __attribute__ ((packed)) phy_page_attr_t;

typedef struct {
  phy_page_attr_t attr;
  Bit64u tsc; /* for comparing to CR3 timestamp counter */

  Bit32u hostPPI;
  } __attribute__ ((packed)) phyPageInfo_t;

/* Possible values of the access_perm field above. */
#define PagePermRW      0
#define PagePermRO      1
#define PagePermEmulate 2
#define PagePermNA PagePermEmulate  /* No Access is synomym */


/* Bitmasks to access fields in structure above. */
#define PageUsagePTbl             0x010
#define PageUsagePDir             0x020

#define PageUsageMemMapIO         0x080
#define PageUsageRO               0x100
#define PageUsageAllocated        0x200
#define PageUsageSwappable        0x400

/* Group of attributes which retain their value, even when CR3 */
/* is reloaded and the page mappings are flushed. */
#define PageUsageSticky \
  ( PageUsageMemMapIO | PageUsageRO | \
    PageUsageAllocated | PageUsageSwappable )

/* Group of attributes which are not compatible with a Page Table */
/* occupying a physical page. */
#define PageBadUsage4PTbl \
  ( PageUsagePDir | PageUsageMemMapIO | PageUsageRO )

/* Group of attributes which are not compatible with a Page Directory */
/* occupying a physical page.  Keep in mind, when the PDir is marked, */
/* no other dynamic bits will be set. */
#define PageBadUsage4PDir \
  ( PageUsageMemMapIO | PageUsageRO )

#define PageUsageCausesNA \
  ( PageUsagePTbl | PageUsagePDir | PageUsageMemMapIO )
#define PageUsageCausesRO \
  ( PageUsageRO )

#define PDEUnhandled 0x000001d8
#define PTEUnhandled 0x00000198


#define ExceptionDE   0 /* Divide Error (fault) */
#define ExceptionDB   1 /* Debug (fault/trap) */
#define ExceptionBP   3 /* Breakpoint (trap) */
#define ExceptionOF   4 /* Overflow (trap) */
#define ExceptionBR   5 /* BOUND (fault) */
#define ExceptionUD   6
#define ExceptionNM   7
#define ExceptionDF   8
#define ExceptionTS  10
#define ExceptionNP  11
#define ExceptionSS  12
#define ExceptionGP  13
#define ExceptionPF  14
#define ExceptionMF  16
#define ExceptionAC  17


#define CR0_PE  (1<<0)
#define CR0_MP  (1<<1)
#define CR0_EM  (1<<2)
#define CR0_TS  (1<<3)
#define CR0_ET  (1<<4)
#define CR0_NE  (1<<5)
#define CR0_WP  (1<<16)
#define CR0_AM  (1<<18)
#define CR0_NW  (1<<29)
#define CR0_CD  (1<<30)
#define CR0_PG  (1<<31)

/*
 * Complete state of the VM (Virtual Machine).
 */
typedef struct {
  Bit32u guestPhyMemAddr; /* Ptr to malloced memory from user space. */

  /* Store eflags values of the guest which are virtualized to
   * run in the monitor
   */
  eflags_t veflags;

  unsigned executeMethod;
  unsigned vmState;

  unsigned mon_request;
  unsigned guestFaultNo;
  Bit32u   pinReqPPI;

  unsigned redirect_vector;

  Bit32u   kernel_offset;

#define MonitorSpace 0
#define UserSpace    1
#define HostSpace    2

  volatile unsigned inMonFault;

  /* Extra info on aborts, especially when a message can't
   * be printed out
   */
  unsigned abort_code;

  struct {
    Bit64u     t0; /* TSC before excecution of guest code */
    Bit64u     cyclesElapsed; /* Cycles of guest execution */
    unsigned   a20Enable;    /* A20 line enabled? */
    Bit32u     a20AddrMask;  /* mask to apply to phy address */
    Bit32u     a20IndexMask; /* mask to apply to phy address */
    } system;

  cpuid_info_t guestCPUIDInfo;

/* This macro yields a physical address after applying the A20 line
 * enable mask to the original physical address.
 */
#define A20Addr(vm, paddr) ( (paddr) & ((vm)->system.a20AddrMask) )
#define A20PageIndex(vm, pi) ( (pi) & ((vm)->system.a20IndexMask) )

  /* Keep an index of the next available Page Table */
  unsigned  ptbl_laddr_map_i;

  Bit32u mon_pde_mask; /* Upper 10 bits of monitor lin addr space */
  Bit32u mon_pdi;      /* Same value shifted down 22 bits. */
  Bit64u vpaging_tsc; /* time stamp of last page mappings flush */

  /* We need to keep track of what each of the guest's physical */
  /* pages contains, and maintain some additional attributes. */
  /* We determine which kinds of information reside in the page, */
  /* dynamically. */
  phyPageInfo_t pageInfo[MAX_MON_GUEST_PAGES];

  /* This is a hack for now.  I need to store the "struct page *"
   * information returned by get_user_pages() in the Linux kernel.
   * Should clean this up.
   */
  void  *hostStructPagePtr[MAX_MON_GUEST_PAGES];

  /* A revolving queue, which stores information on guest physical memory
   * pages which are currently pinned.  Only a certain number of pages
   * may be pinned at any one time.  This is a really simplistic
   * strategy - when the Q is full, the page which was pinned the
   * longest time ago is unpinned to make room.  It's a
   * "least recently pinned" strategy.
   */
#define MaxPhyPagesPinned 1024  /* 4Megs of pinned pages max per VM. */
  struct {
    unsigned nEntries; /* Number of entries in table. */
    unsigned tail;
    Bit32u ppi[MaxPhyPagesPinned]; /* Physical Page Index of pinned guest page. */
    } guestPhyPagePinQueue;

  struct {
    volatile unsigned event; /* Any log event occurred. */
    /* Inactive, OK to dump to host and change */
    volatile unsigned locked;
    /* Number of times buffer wrapped since last print to kernel */
    /*   debug facility */
    volatile unsigned offset; /* Current index within buffer */
    volatile unsigned error;  /* Error printing. (ex. string too long) */
    } log_buffer_info;

  vm_pages_t         pages;  /* memory pages allocated by the host */

  /* Host specific fields.  These fields should NOT be accessed */
  /* from code which may execute in either host or monitor/guest */
  /* spaces, unless you need to _specifically_ manipulate a */
  /* host-specific field. */
  struct {
    vm_addr_t    addr;   /* addresses of data structures in host space */
    void       (*__host2mon)(void);   /* Host to guest nexus entry point */
    pageEntry_t  nexus_pde;           /* PDE pointing to nexus page table */
    } host;

  /* Guest specific fields.  These fields should NOT be accessed */
  /* from code which may execute in either host or monitor/guest */
  /* spaces, unless you need to _specifically_ manipulate a */
  /* guest-specific field. */
  struct {
    vm_addr_t    addr;   /* addresses of data structures in guest space */
    void       (*__mon2host)(void);  /* monitor to host entry point */
    } guest;
  } vm_t;


extern char __nexus_start, __nexus_end, __mon_cs;
extern char __host2mon, __mon2host, __handle_fault, __handle_int;
extern char __ret_to_guest;



/*
 * This structure describes the pages containing the code/data
 * of the monitor itself (inside the kernel module)
 */

#define Plex86MaxKernelModulePages 128

typedef struct {
  /* Virtual address space occupied by the kernel module. */
  Bit32u startOffset;
  Bit32u startOffsetPageAligned;
  unsigned nPages; /* Number of pages. */
    
  /* A list of the Physical Page Indeces of the pages comprising the
   * kernel module.  A PPI is just the physical page address >> 12.
   */
  Bit32u ppi[Plex86MaxKernelModulePages];
  } kernelModulePages_t;

extern kernelModulePages_t kernelModulePages;
extern cpuid_info_t        hostCpuIDInfo;



#if !defined(IN_HOST_SPACE) && !defined(IN_MONITOR_SPACE)
#error "No space defined for this file"
#endif


#if defined(IN_HOST_SPACE) || defined(IN_MONITOR_SPACE)

void  mon_memzero(void *ptr, int size);
void  mon_memcpy(void *dst, void *src, int size);
void *mon_memset(void *s, unsigned c, unsigned n);

/*
 *  We need to set the monitor CS/DS base address so that the module pages,
 *  which are mapped starting at linear address 'laddr' into the guest address
 *  space, reside at the same offset relative to the monitor CS base as they
 *  reside relative to the kernel CS base in the host address space.  This way,
 *  we can execute the (non-relocatable) module code within the guest address
 *  space ...
 */
#define MON_BASE_FROM_LADDR(laddr) \
    ((laddr) - kernelModulePages.startOffsetPageAligned)


/* ============================================================
 * These are the functions which are available in either of the
 * host or monitor/guest spaces.
 */


/* Access to label offsets in nexus.S... From the host address perspective */
#define HOST_NEXUS_OFFSET(vm, field) \
    ( ((Bit32u)vm->host.addr.nexus) + \
      (((Bit32u) &field) - ((Bit32u) &__nexus_start)) )

/* From the monitor/guest address perspective. */
#define MON_NEXUS_OFFSET(vm, field) \
    ( ((Bit32u)vm->guest.addr.nexus) + \
      (((Bit32u) &field) - ((Bit32u) &__nexus_start)) )

  static __inline__ Bit64u
vm_rdtsc(void) {
  Bit64u ret;
  asm volatile (
    "rdtsc"
    : "=A" (ret)
    );
  return ret;
  }
#endif  /* {HOST, MONITOR} */




#ifdef IN_HOST_SPACE
/* ==========================================================
 * These are the functions which are available to the monitor
 * running in the host space.
 */


/*
 * Generate a software interrupt
 */

#define soft_int(n)                             \
    asm volatile (                              \
        "    movb %b0, __soft_int_vector \n\t"  \
        "    jmp __soft_int_n            \n\t"  \
        "__soft_int_n:                   \n\t"  \
        "    sti                         \n\t"  \
        "    .byte 0xcd                  \n\t"  \
        "__soft_int_vector:              \n\t"  \
        "    .byte 0x00                  \n\t"  \
        :                                       \
        : "r" ((Bit8u) (n) )                    \
        : "memory"                              \
    )

#define Plex86ErrnoEBUSY      1
#define Plex86ErrnoENOMEM     2
#define Plex86ErrnoEFAULT     3
#define Plex86ErrnoEINVAL     4
#define Plex86ErrnoEACCES     5
#define Plex86ErrnoEAGAIN     6

#define vm_save_flags(x) \
  asm volatile("pushfl ; popl %0": "=g" (x): :"memory")

#define vm_restore_flags(x) \
  asm volatile("pushl %0 ; popfl": :"g" (x): "memory", "cc")


int      hostInitMonitor(vm_t *);
unsigned hostMapMonitor(vm_t *);
unsigned hostInitGuestPhyMem(vm_t *);
void     hostUnallocVmPages(vm_t *);
int      hostAllocVmPages(vm_t *, plex86IoctlRegisterMem_t *registerMsg);
void     hostInitShadowPaging(vm_t *vm);
void     hostDeviceOpen(vm_t *);
unsigned hostModuleInit(void);
unsigned hostGetCpuCapabilities(void);
int      hostIoctlGeneric(vm_t *vm, void *inode, void *filp,
                          unsigned int cmd, unsigned long arg);
int      hostIoctlExecute(vm_t *vm, plex86IoctlExecute_t *executeMsg);
int      hostIoctlRegisterMem(vm_t *vm, plex86IoctlRegisterMem_t *registerMsg);
void     hostCopyGuestStateToUserSpace(vm_t *vm);
void     hostReleasePinnedUserPages(vm_t *vm);
unsigned hostHandlePagePinRequest(vm_t *vm, Bit32u reqPPI);

/* These are the functions that the host-OS-specific file of the
 * plex86 device driver must define.
 */
unsigned hostOSIdle(void);
void    *hostOSAllocZeroedMem(unsigned long size);
void     hostOSFreeMem(void *ptr);
void    *hostOSAllocZeroedPage(void);
void     hostOSFreePage(void *ptr);
unsigned hostOSGetAllocedMemPhyPages(Bit32u *page, int max_pages, void *ptr,
                                   unsigned size);
Bit32u   hostOSGetAndPinUserPage(vm_t *vm, Bit32u userAddr, void **osSpecificPtr,
             Bit32u *ppi, Bit32u *kernelAddr);
void     hostOSUnpinUserPage(vm_t *vm, Bit32u userAddr, void *osSpecificPtr,
             Bit32u ppi, Bit32u *kernelAddr, unsigned dirty);
Bit32u   hostOSGetAllocedPagePhyPage(void *ptr);
void     hostOSPrint(char *fmt, ...);
Bit32u   hostOSKernelOffset(void);
int      hostOSConvertPlex86Errno(unsigned ret);
void     hostOSModuleCountReset(vm_t *vm, void *inode, void *filp);
void     hostOSInstrumentIntRedirCount(unsigned interruptVector);
unsigned long hostOSCopyFromUser(void *to, void *from, unsigned long len);
unsigned long hostOSCopyToUser(void *to, void *from, unsigned long len);

#endif  /* HOST Space */




#ifdef IN_MONITOR_SPACE
/* ==========================================================
 * These are the functions which are available to the monitor
 * running in the monitor/guest space.
 */

void sysFlushPrintBuf(vm_t *);
void sysRemapMonitor(vm_t *);
int  monprint(vm_t *, char *fmt, ...);
int  mon_vsnprintf(char *str, unsigned size, const char *fmt,
                   va_list args);
void resetPrintBuf(vm_t *);

/* Translate from guest laddr to monitor laddr. */
#define Guest2Monitor(vm, laddr) ( ((Bit32u) (laddr)) - \
                                   vm->guest.addr.nexus->mon_base )

void monpanic(vm_t *, char *fmt, ...) __attribute__ ((noreturn));
void monpanic_nomess(vm_t *);

void toHostGuestFault(vm_t *, unsigned fault);
void toHostPinUserPage(vm_t *, Bit32u ppi);

void guestPageFault(vm_t *, guest_context_t *context, Bit32u cr2);
void *open_guest_phy_page(vm_t *, Bit32u ppage_index, Bit8u *mon_offset);
void close_guest_phy_page(vm_t *, Bit32u ppage_index);

#define MapLinOK              0
#define MapLinMonConflict     1
#define MapLinAlreadyMapped   2
#define MapLinPPageOOB        3
#define MapLinException       4
#define MapLinEmulate         5
unsigned mapGuestLinAddr(vm_t *, Bit32u guest_laddr,
                         Bit32u *guest_ppage_index, unsigned us,
                         unsigned rw, Bit32u attr, Bit32u *error);
unsigned addPageAttributes(vm_t *, Bit32u ppi, Bit32u attr);
phyPageInfo_t *getPageUsage(vm_t *, Bit32u ppage_index);
void virtualize_lconstruct(vm_t *, Bit32u l0, Bit32u l1, unsigned perm);

unsigned getMonPTi(vm_t *, unsigned pdi, unsigned source);
#define invlpg_mon_offset(mon_offset) \
  asm volatile ("invlpg (%0)": :"r" (mon_offset): "memory")

/* For now nothing, but we should conditionally compile in code
 * to panic when the expression is not true.
 */

#define VM_ASSERT(vm, expression) \
  if ( !(expression) ) \
    monpanic(vm, "Assertion (%s) failed at %s:%u", \
             #expression, __FILE__, __LINE__)

#define CLI() asm volatile ("cli": : : "memory")
#define STI() asm volatile ("sti": : : "memory")

#endif  /* MONITOR Space. */

#endif  /* __MONITOR_H__ */
