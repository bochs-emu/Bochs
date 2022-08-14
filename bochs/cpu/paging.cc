/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2021  The Bochs Project
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "memory/memory-bochs.h"
#include "pc_system.h"

// X86 Registers Which Affect Paging:
// ==================================
//
// CR0:
//   bit 31: PG, Paging (386+)
//   bit 16: WP, Write Protect (486+)
//     0: allow   supervisor level writes into user level RO pages
//     1: inhibit supervisor level writes into user level RO pages
//
// CR3:
//   bit 31..12: PDBR, Page Directory Base Register (386+)
//   bit      4: PCD, Page level Cache Disable (486+)
//     Controls caching of current page directory.  Affects only the processor's
//     internal caches (L1 and L2).
//     This flag ignored if paging disabled (PG=0) or cache disabled (CD=1).
//     Values:
//       0: Page Directory can be cached
//       1: Page Directory not cached
//   bit      3: PWT, Page level Writes Transparent (486+)
//     Controls write-through or write-back caching policy of current page
//     directory.  Affects only the processor's internal caches (L1 and L2).
//     This flag ignored if paging disabled (PG=0) or cache disabled (CD=1).
//     Values:
//       0: write-back caching enabled
//       1: write-through caching enabled
//
// CR4:
//   bit 4: PSE, Page Size Extension (Pentium+)
//     0: 4KByte pages (typical)
//     1: 4MByte or 2MByte pages
//   bit 5: PAE, Physical Address Extension (Pentium Pro+)
//     0: 32bit physical addresses
//     1: 36bit physical addresses
//   bit 7: PGE, Page Global Enable (Pentium Pro+)
//     The global page feature allows frequently used or shared pages
//     to be marked as global (PDE or PTE bit 8).  Global pages are
//     not flushed from TLB on a task switch or write to CR3.
//     Values:
//       0: disables global page feature
//       1: enables global page feature
//
//    page size extention and physical address size extention matrix (legacy mode)
//   ==============================================================================
//   CR0.PG  CR4.PAE  CR4.PSE  PDPE.PS  PDE.PS | page size   physical address size
//   ==============================================================================
//      0       X        X       R         X   |   --          paging disabled
//      1       0        0       R         X   |   4K              32bits
//      1       0        1       R         0   |   4K              32bits
//      1       0        1       R         1   |   4M              32bits
//      1       1        X       R         0   |   4K              36bits
//      1       1        X       R         1   |   2M              36bits

//     page size extention and physical address size extention matrix (long mode)
//   ==============================================================================
//   CR0.PG  CR4.PAE  CR4.PSE  PDPE.PS  PDE.PS | page size   physical address size
//   ==============================================================================
//      1       1        X       0         0   |   4K              52bits
//      1       1        X       0         1   |   2M              52bits
//      1       1        X       1         -   |   1G              52bits


// Page Directory/Table Entry Fields Defined:
// ==========================================
// NX: No Execute
//   This bit controls the ability to execute code from all physical
//   pages mapped by the table entry.
//     0: Code can be executed from the mapped physical pages
//     1: Code cannot be executed
//   The NX bit can only be set when the no-execute page-protection
//   feature is enabled by setting EFER.NXE=1, If EFER.NXE=0, the
//   NX bit is treated as reserved. In this case, #PF occurs if the
//   NX bit is not cleared to zero.
//
// G: Global flag
//   Indiciates a global page when set.  When a page is marked
//   global and the PGE flag in CR4 is set, the page table or
//   directory entry for the page is not invalidated in the TLB
//   when CR3 is loaded or a task switch occurs.  Only software
//   clears and sets this flag.  For page directory entries that
//   point to page tables, this flag is ignored and the global
//   characteristics of a page are set in the page table entries.
//
// PS: Page Size flag
//   Only used in page directory entries.  When PS=0, the page
//   size is 4KBytes and the page directory entry points to a
//   page table.  When PS=1, the page size is 4MBytes for
//   normal 32-bit addressing and 2MBytes if extended physical
//   addressing.
//
// PAT: Page-Attribute Table
//   This bit is only present in the lowest level of the page
//   translation hierarchy. The PAT bit is the high-order bit
//   of a 3-bit index into the PAT register. The other two
//   bits involved in forming the index are the PCD and PWT
//   bits.
//
// D: Dirty bit:
//   Processor sets the Dirty bit in the 2nd-level page table before a
//   write operation to an address mapped by that page table entry.
//   Dirty bit in directory entries is undefined.
//
// A: Accessed bit:
//   Processor sets the Accessed bits in both levels of page tables before
//   a read/write operation to a page.
//
// PCD: Page level Cache Disable
//   Controls caching of individual pages or page tables.
//   This allows a per-page based mechanism to disable caching, for
//   those pages which contained memory mapped IO, or otherwise
//   should not be cached.  Processor ignores this flag if paging
//   is not used (CR0.PG=0) or the cache disable bit is set (CR0.CD=1).
//   Values:
//     0: page or page table can be cached
//     1: page or page table is not cached (prevented)
//
// PWT: Page level Write Through
//   Controls the write-through or write-back caching policy of individual
//   pages or page tables.  Processor ignores this flag if paging
//   is not used (CR0.PG=0) or the cache disable bit is set (CR0.CD=1).
//   Values:
//     0: write-back caching
//     1: write-through caching
//
// U/S: User/Supervisor level
//   0: Supervisor level - for the OS, drivers, etc.
//   1: User level - application code and data
//
// R/W: Read/Write access
//   0: read-only access
//   1: read/write access
//
// P: Present
//   0: Not present
//   1: Present
// ==========================================

// Combined page directory/page table protection:
// ==============================================
// There is one column for the combined effect on a 386
// and one column for the combined effect on a 486+ CPU.
// The 386 CPU behavior is not supported by Bochs.
//
// +----------------+-----------------+----------------+----------------+
// |  Page Directory|     Page Table  |   Combined 386 |  Combined 486+ |
// |Privilege  Type | Privilege  Type | Privilege  Type| Privilege  Type|
// |----------------+-----------------+----------------+----------------|
// |User       R    | User       R    | User       R   | User       R   |
// |User       R    | User       RW   | User       R   | User       R   |
// |User       RW   | User       R    | User       R   | User       R   |
// |User       RW   | User       RW   | User       RW  | User       RW  |
// |User       R    | Supervisor R    | User       R   | Supervisor RW  |
// |User       R    | Supervisor RW   | User       R   | Supervisor RW  |
// |User       RW   | Supervisor R    | User       R   | Supervisor RW  |
// |User       RW   | Supervisor RW   | User       RW  | Supervisor RW  |
// |Supervisor R    | User       R    | User       R   | Supervisor RW  |
// |Supervisor R    | User       RW   | User       R   | Supervisor RW  |
// |Supervisor RW   | User       R    | User       R   | Supervisor RW  |
// |Supervisor RW   | User       RW   | User       RW  | Supervisor RW  |
// |Supervisor R    | Supervisor R    | Supervisor RW  | Supervisor RW  |
// |Supervisor R    | Supervisor RW   | Supervisor RW  | Supervisor RW  |
// |Supervisor RW   | Supervisor R    | Supervisor RW  | Supervisor RW  |
// |Supervisor RW   | Supervisor RW   | Supervisor RW  | Supervisor RW  |
// +----------------+-----------------+----------------+----------------+

// Page Fault Error Code Format:
// =============================
//
// bits 31..4: Reserved
// bit  3: RSVD (Pentium Pro+)
//   0: fault caused by reserved bits set to 1 in a page directory
//      when the PSE or PAE flags in CR4 are set to 1
//   1: fault was not caused by reserved bit violation
// bit  2: U/S (386+)
//   0: fault originated when in supervior mode
//   1: fault originated when in user mode
// bit  1: R/W (386+)
//   0: access causing the fault was a read
//   1: access causing the fault was a write
// bit  0: P (386+)
//   0: fault caused by a nonpresent page
//   1: fault caused by a page level protection violation

// Some paging related notes:
// ==========================
//
// - When the processor is running in supervisor level, all pages are both
//   readable and writable (write-protect ignored).  When running at user
//   level, only pages which belong to the user level are accessible;
//   read/write & read-only are readable, read/write are writable.
//
// - If the Present bit is 0 in either level of page table, an
//   access which uses these entries will generate a page fault.
//
// - (A)ccess bit is used to report read or write access to a page
//   or 2nd level page table.
//
// - (D)irty bit is used to report write access to a page.
//
// - Processor running at CPL=0,1,2 maps to U/S=0
//   Processor running at CPL=3     maps to U/S=1

// bit [11] of the TLB lpf used for TLB_NoHostPtr valid indication
#define TLB_LPFOf(laddr) AlignedAccessLPFOf(laddr, 0x7ff)

#if BX_CPU_LEVEL >= 4
#  define BX_PRIV_CHECK_SIZE 32
#else
#  define BX_PRIV_CHECK_SIZE 16
#endif

// The 'priv_check' array is used to decide if the current access
// has the proper paging permissions.  An index is formed, based
// on parameters such as the access type and level, the write protect
// flag and values cached in the TLB.  The format of the index into this
// array is:
//
//   |4 |3 |2 |1 |0 |
//   |wp|us|us|rw|rw|
//    |  |  |  |  |
//    |  |  |  |  +---> r/w of current access
//    |  |  +--+------> u/s,r/w combined of page dir & table (cached)
//    |  +------------> u/s of current access
//    +---------------> Current CR0.WP value
//
//                                                                  CR0.WP = 0     CR0.WP = 1
//    -----------------------------------------------------------------------------------------
//       0  0  0  0 | sys read from supervisor page             | Allowed       | Allowed
//       0  0  0  1 | sys write to read only supervisor page    | Allowed       | Not Allowed
//       0  0  1  0 | sys read from supervisor page             | Allowed       | Allowed
//       0  0  1  1 | sys write to supervisor page              | Allowed       | Allowed
//       0  1  0  0 | sys read from read only user page         | Allowed       | Allowed
//       0  1  0  1 | sys write to read only user page          | Allowed       | Not Allowed
//       0  1  1  0 | sys read from user page                   | Allowed       | Allowed
//       0  1  1  1 | sys write to user page                    | Allowed       | Allowed
//       1  0  0  0 | user read from read only supervisor page  | Not Allowed   | Not Allowed
//       1  0  0  1 | user write to read only supervisor page   | Not Allowed   | Not Allowed
//       1  0  1  0 | user read from supervisor page            | Not Allowed   | Not Allowed
//       1  0  1  1 | user write to supervisor page             | Not Allowed   | Not Allowed
//       1  1  0  0 | user read from read only user page        | Allowed       | Allowed
//       1  1  0  1 | user write to read only user page         | Not Allowed   | Not Allowed
//       1  1  1  0 | user read from user page                  | Allowed       | Allowed
//       1  1  1  1 | user write to user page                   | Allowed       | Allowed
//

/* 0xff0bbb0b */
static const Bit8u priv_check[BX_PRIV_CHECK_SIZE] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1,
#if BX_CPU_LEVEL >= 4
  1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1
#endif
};

// The 'priv_check' array for shadow stack accesses
//
//      |3 |2 |1 |0 |
//      |us|us|rw|rw|
//       |  |  |  |
//       |  |  |  +---> r/w of current access
//       |  +--+------> u/s,r/w combined of page dir & table (cached)
//       +------------> u/s of current access
//
//    -------------------------------------------------------------------
//       0  0  0  0 | sys read from supervisor page             | Allowed
//       0  0  0  1 | sys write to read only supervisor page    | Allowed : shadow stack page looks like read only page
//       0  0  1  0 | sys read from supervisor page             | Allowed
//       0  0  1  1 | sys write to supervisor page              | Allowed
//       0  1  0  0 | sys read from read only user page         | Not Allowed   : supervisor-mode shadow-stack access is not allowed to a user-mode page
//       0  1  0  1 | sys write to read only user page          | Not Allowed   : supervisor-mode shadow-stack access is not allowed to a user-mode page
//       0  1  1  0 | sys read from user page                   | Not Allowed   : supervisor-mode shadow-stack access is not allowed to a user-mode page
//       0  1  1  1 | sys write to user page                    | Not Allowed   : supervisor-mode shadow-stack access is not allowed to a user-mode page
//       1  0  0  0 | user read from read only supervisor page  | Not Allowed   : user-mode shadow-stack access is not allowed to a supervisor-mode page
//       1  0  0  1 | user write to read only supervisor page   | Not Allowed   : user-mode shadow-stack access is not allowed to a supervisor-mode page
//       1  0  1  0 | user read from supervisor page            | Not Allowed   : user-mode shadow-stack access is not allowed to a supervisor-mode page
//       1  0  1  1 | user write to supervisor page             | Not Allowed   : user-mode shadow-stack access is not allowed to a supervisor-mode page
//       1  1  0  0 | user read from read only user page        | Allowed
//       1  1  0  1 | user write to read only user page         | Allowed : shadow stack page looks like read only page
//       1  1  1  0 | user read from user page                  | Allowed
//       1  1  1  1 | user write to user page                   | Allowed
//

const Bit64u BX_PAGING_PHY_ADDRESS_RESERVED_BITS = BX_PHY_ADDRESS_RESERVED_BITS & BX_CONST64(0xfffffffffffff);

const Bit64u PAGE_DIRECTORY_NX_BIT = BX_CONST64(0x8000000000000000);

const Bit64u BX_CR3_PAGING_MASK = BX_CONST64(0x000ffffffffff000);

// Each entry in the TLB cache has 3 entries:
//
//   lpf:         Linear Page Frame (page aligned linear address of page)
//     bits 32..12  Linear page frame
//     bit  11      0: TLB HostPtr access allowed, 1: not allowed
//     bit  10...0  Invalidate index
//
//   ppf:         Physical Page Frame (page aligned phy address of page)
//
//   hostPageAddr:
//                Host Page Frame address used for direct access to
//                the mem.vector[] space allocated for the guest physical
//                memory.  If this is zero, it means that a pointer
//                to the host space could not be generated, likely because
//                that page of memory is not standard memory (it might
//                be memory mapped IO, ROM, etc).
//
//   accessBits:
//
//     bit  31:     Page is a global page.
//
//       The following bits are used for a very efficient permissions
//       check.  The goal is to be able, using only the current privilege
//       level and access type, to determine if the page tables allow the
//       access to occur or at least should rewalk the page tables.  On
//       the first read access, permissions are set to only read, so a
//       rewalk is necessary when a subsequent write fails the tests.
//       This allows for the dirty bit to be set properly, but for the
//       test to be efficient.  Note that the CR0.WP flag is not present.
//       The values in the following flags is based on the current CR0.WP
//       value, necessitating a TLB flush when CR0.WP changes.
//
//       The test bit:
//         OK = 1 << ((S<<2) | (W<<1) | U)
//
//       where S:1=Shadow Stack (CET)
//             W:1=Write, 0=Read;
//             U:1=CPL3, 0=CPL0-2
//       
//       Thus for reads, it is:
//         OK = 0x01 << (          U )
//       for writes:
//         OK = 0x04 << (          U )
//       for shadow stack reads:
//         OK = 0x10 << (          U )
//       for shadow stack writes:
//         OK = 0x40 << (          U )
//
//     bit 3: Write   from User   privilege is OK
//     bit 2: Write   from System privilege is OK
//     bit 1: Read    from User   privilege is OK
//     bit 0: Read    from System privilege is OK
//
//       Note, that the TLB should have TLB_NoHostPtr bit set in the lpf when
//       direct access through host pointer is NOT allowed for the page.
//       A memory operation asking for a direct access through host pointer
//       will not set TLB_NoHostPtr bit in its lpf and thus get TLB miss 
//       result when the direct access is not allowed.
//

const Bit32u TLB_NoHostPtr = 0x800; /* set this bit when direct access is NOT allowed */

#include "cpustats.h"

// ==============================================================

void BX_CPU_C::TLB_flush(void)
{
  INC_TLBFLUSH_STAT(tlbGlobalFlushes);

  invalidate_prefetch_q();
  invalidate_stack_cache();

  BX_CPU_THIS_PTR DTLB.flush();
  BX_CPU_THIS_PTR ITLB.flush();

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB might change translation for monitored page
  // and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR wakeup_monitor();
#endif

  // break all links bewteen traces
  BX_CPU_THIS_PTR iCache.breakLinks();
}

#if BX_CPU_LEVEL >= 6
void BX_CPU_C::TLB_flushNonGlobal(void)
{
  INC_TLBFLUSH_STAT(tlbNonGlobalFlushes);

  invalidate_prefetch_q();
  invalidate_stack_cache();

  BX_CPU_THIS_PTR DTLB.flushNonGlobal();
  BX_CPU_THIS_PTR ITLB.flushNonGlobal();

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB might change translation for monitored page
  // and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR wakeup_monitor();
#endif

  // break all links bewteen traces
  BX_CPU_THIS_PTR iCache.breakLinks();
}
#endif

void BX_CPU_C::TLB_invlpg(bx_address laddr)
{
  invalidate_prefetch_q();
  invalidate_stack_cache();

  BX_DEBUG(("TLB_invlpg(0x" FMT_ADDRX "): invalidate TLB entry", laddr));
  BX_CPU_THIS_PTR DTLB.invlpg(laddr);
  BX_CPU_THIS_PTR ITLB.invlpg(laddr);

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB entry might change translation for monitored
  // page and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR wakeup_monitor();
#endif

  // break all links bewteen traces
  BX_CPU_THIS_PTR iCache.breakLinks();
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVLPG(bxInstruction_c* i)
{
  // CPL is always 0 in real mode
  if (/* !real_mode() && */ CPL!=0) {
    BX_ERROR(("%s: priveledge check failed, generate #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (VMEXIT(VMX_VM_EXEC_CTRL2_INVLPG_VMEXIT)) VMexit(VMX_VMEXIT_INVLPG, laddr);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_INVLPG))
      Svm_Vmexit(SVM_VMEXIT_INVLPG, BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_DECODE_ASSIST) ? laddr : 0);
  }
#endif

#if BX_SUPPORT_X86_64
  if (IsCanonical(laddr))
#endif
  {
    BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVLPG, laddr);
    TLB_invlpg(laddr);
  }

  BX_NEXT_TRACE(i);
}

// error checking order - page not present, reserved bits, protection
enum {
  ERROR_NOT_PRESENT  = 0x00,
  ERROR_PROTECTION   = 0x01,
  ERROR_WRITE_ACCESS = 0x02,
  ERROR_USER_ACCESS  = 0x04,
  ERROR_RESERVED     = 0x08,
  ERROR_CODE_ACCESS  = 0x10,
  ERROR_PKEY         = 0x20,
  ERROR_SHADOW_STACK = 0x40,
};

void BX_CPU_C::page_fault(unsigned fault, bx_address laddr, unsigned user, unsigned rw)
{
  unsigned isWrite = rw & 1;

  Bit32u error_code = fault | (user << 2) | (isWrite << 1);
#if BX_CPU_LEVEL >= 6
  if (rw == BX_EXECUTE) {
    if (BX_CPU_THIS_PTR cr4.get_SMEP())
      error_code |= ERROR_CODE_ACCESS; // I/D = 1
    if (BX_CPU_THIS_PTR cr4.get_PAE() && BX_CPU_THIS_PTR efer.get_NXE())
      error_code |= ERROR_CODE_ACCESS;
  }
#endif
#if BX_SUPPORT_CET
  bool is_shadow_stack = (rw & 4) != 0;
  if (is_shadow_stack)
    error_code |= ERROR_SHADOW_STACK;
#endif

#if BX_SUPPORT_SVM
  SvmInterceptException(BX_HARDWARE_EXCEPTION, BX_PF_EXCEPTION, error_code, 1, laddr); // before the CR2 was modified
#endif

#if BX_SUPPORT_VMX
  VMexit_Event(BX_HARDWARE_EXCEPTION, BX_PF_EXCEPTION, error_code, 1, laddr); // before the CR2 was modified
#endif

  BX_CPU_THIS_PTR cr2 = laddr;

#if BX_SUPPORT_X86_64
  BX_DEBUG(("page fault for address %08x%08x @ %08x%08x",
             GET32H(laddr), GET32L(laddr), GET32H(RIP), GET32L(RIP)));
#else
  BX_DEBUG(("page fault for address %08x @ %08x", laddr, EIP));
#endif

  exception(BX_PF_EXCEPTION, error_code);
}

enum {
  BX_LEVEL_PML4 = 3,
  BX_LEVEL_PDPTE = 2,
  BX_LEVEL_PDE = 1,
  BX_LEVEL_PTE = 0
};

static const char *bx_paging_level[4] = { "PTE", "PDE", "PDPE", "PML4" }; // keep it 4 letters

// combined_access legend:
// -----------------------
// 00    |
// 01    | R/W
// 02    | U/S
// 03    |
// 07    | Shadow Stack
// 08    | Global
// 11-09 | memtype (3 bits)

enum {
  BX_COMBINED_ACCESS_WRITE = 0x2,
  BX_COMBINED_ACCESS_USER  = 0x4,
  BX_COMBINED_SHADOW_STACK = 0x80,
  BX_COMBINED_GLOBAL_PAGE  = 0x100,
};

#define IS_USER_PAGE(combined_access) !!((combined_access) & BX_COMBINED_ACCESS_USER)

#if BX_CPU_LEVEL >= 6

//                Format of a Long Mode Non-Leaf Entry
// -----------------------------------------------------------
// 00    | Present (P)
// 01    | R/W
// 02    | U/S
// 03    | Page-Level Write-Through (PWT)
// 04    | Page-Level Cache-Disable (PCD)
// 05    | Accessed (A)
// 06    | (ignored)
// 07    | Page Size (PS), must be 0 if no Large Page on the level
// 11-08 | (ignored)
// PA-12 | Physical address of 4-KByte aligned page-directory-pointer table
// 51-PA | Reserved (must be zero)
// 62-52 | (ignored)
// 63    | Execute-Disable (XD) (if EFER.NXE=1, reserved otherwise)
// -----------------------------------------------------------

const Bit64u PAGING_PAE_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS;

// in legacy PAE mode bits [62:52] are reserved. bit 63 is NXE
const Bit64u PAGING_LEGACY_PAE_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0x7ff0000000000000);

//       Format of a PDPTE that References a 1-GByte Page
// -----------------------------------------------------------
// 00    | Present (P)
// 01    | R/W
// 02    | U/S
// 03    | Page-Level Write-Through (PWT)
// 04    | Page-Level Cache-Disable (PCD)
// 05    | Accessed (A)
// 06    | (ignored)
// 07    | Page Size, must be 1 to indicate a 1-GByte Page
// 08    | Global (G) (if CR4.PGE=1, ignored otherwise)
// 11-09 | (ignored)
// 12    | PAT (if PAT is supported, reserved otherwise)
// 29-13 | Reserved (must be zero)
// PA-30 | Physical address of the 1-Gbyte Page
// 51-PA | Reserved (must be zero)
// 62-52 | (ignored)
// 63    | Execute-Disable (XD) (if EFER.NXE=1, reserved otherwise)
// -----------------------------------------------------------

const Bit64u PAGING_PAE_PDPTE1G_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0x3FFFE000);

//        Format of a PAE PDE that Maps a 2-MByte Page
// -----------------------------------------------------------
// 00    | Present (P)
// 01    | R/W
// 02    | U/S
// 03    | Page-Level Write-Through (PWT)
// 04    | Page-Level Cache-Disable (PCD)
// 05    | Accessed (A)
// 06    | Dirty (D)
// 07    | Page Size (PS), must be 1 to indicate a 2-MByte Page
// 08    | Global (G) (if CR4.PGE=1, ignored otherwise)
// 11-09 | (ignored)
// 12    | PAT (if PAT is supported, reserved otherwise)
// 20-13 | Reserved (must be zero)
// PA-21 | Physical address of the 2-MByte page
// 51-PA | Reserved (must be zero)
// 62-52 | ignored in long mode, reserved (must be 0) in legacy PAE mode
// 63    | Execute-Disable (XD) (if EFER.NXE=1, reserved otherwise)
// -----------------------------------------------------------

const Bit64u PAGING_PAE_PDE2M_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0x001FE000);

//        Format of a PAE PTE that Maps a 4-KByte Page
// -----------------------------------------------------------
// 00    | Present (P)
// 01    | R/W
// 02    | U/S
// 03    | Page-Level Write-Through (PWT)
// 04    | Page-Level Cache-Disable (PCD)
// 05    | Accessed (A)
// 06    | Dirty (D)
// 07    | PAT (if PAT is supported, reserved otherwise)
// 08    | Global (G) (if CR4.PGE=1, ignored otherwise)
// 11-09 | (ignored)
// PA-12 | Physical address of the 4-KByte page
// 51-PA | Reserved (must be zero)
// 62-52 | ignored in long mode, reserved (must be 0) in legacy PAE mode
// 63    | Execute-Disable (XD) (if EFER.NXE=1, reserved otherwise)
// -----------------------------------------------------------

int BX_CPU_C::check_entry_PAE(const char *s, Bit64u entry, Bit64u reserved, unsigned rw, bool *nx_fault)
{
  if (!(entry & 0x1)) {
    BX_DEBUG(("PAE %s: entry not present", s));
    return ERROR_NOT_PRESENT;
  }

  if (entry & reserved) {
    BX_DEBUG(("PAE %s: reserved bit is set 0x" FMT_ADDRX64 "(reserved: " FMT_ADDRX64 ")", s, entry, entry & reserved));
    return ERROR_RESERVED | ERROR_PROTECTION;
  }

  if (entry & PAGE_DIRECTORY_NX_BIT) {
    if (rw == BX_EXECUTE) {
      BX_DEBUG(("PAE %s: non-executable page fault occurred", s));
      *nx_fault = true;
    }
  }

  return -1;
}

#if BX_SUPPORT_MEMTYPE
BX_CPP_INLINE Bit32u calculate_pcd_pwt(Bit32u entry)
{
  Bit32u pcd_pwt = (entry >> 3) & 0x3; // PCD, PWT are stored in bits 3 and 4
  return pcd_pwt;
}

// extract PCD, PWT and PAT pat bits from page table entry
BX_CPP_INLINE Bit32u calculate_pat(Bit32u entry, Bit32u lpf_mask)
{
  Bit32u pcd_pwt = calculate_pcd_pwt(entry);
  // PAT is stored in bit 12 for large pages and in bit 7 for small pages
  Bit32u pat = ((lpf_mask < 0x1000) ? (entry >> 7) : (entry >> 12)) & 0x1;
  return pcd_pwt | (pat << 2);
}
#endif

#if BX_SUPPORT_X86_64

// Translate a linear address to a physical address in long mode
bx_phy_address BX_CPU_C::translate_linear_long_mode(bx_address laddr, Bit32u &lpf_mask, Bit32u &pkey, unsigned user, unsigned rw)
{
  bx_phy_address ppf = BX_CPU_THIS_PTR cr3 & BX_CR3_PAGING_MASK;

  bx_phy_address entry_addr[4];
  Bit64u entry[4];
  BxMemtype entry_memtype[4] = { 0 };

  bool nx_fault = false;
  int leaf;

  Bit64u offset_mask = BX_CONST64(0x0000ffffffffffff);
  lpf_mask = 0xfff;
  Bit32u combined_access = (BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER);
  Bit64u curr_entry = BX_CPU_THIS_PTR cr3;

  Bit64u reserved = PAGING_PAE_RESERVED_BITS;
  if (! BX_CPU_THIS_PTR efer.get_NXE())
    reserved |= PAGE_DIRECTORY_NX_BIT;

  for (leaf = BX_LEVEL_PML4;; --leaf) {
    entry_addr[leaf] = ppf + ((laddr >> (9 + 9*leaf)) & 0xff8);
#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE))
        entry_addr[leaf] = translate_guest_physical(entry_addr[leaf], laddr, true /* laddr_valid */, true /* page walk */, IS_USER_PAGE(combined_access), BX_READ);
    }
#endif
#if BX_SUPPORT_SVM
    if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED) {
      entry_addr[leaf] = nested_walk(entry_addr[leaf], BX_RW, 1);
    }
#endif

#if BX_SUPPORT_MEMTYPE
    entry_memtype[leaf] = resolve_memtype(memtype_by_mtrr(entry_addr[leaf]), memtype_by_pat(calculate_pcd_pwt((Bit32u) curr_entry)));
#endif
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, entry_memtype[leaf], BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    offset_mask >>= 9;

    curr_entry = entry[leaf];
    int fault = check_entry_PAE(bx_paging_level[leaf], curr_entry, reserved, rw, &nx_fault);
    if (fault >= 0)
      page_fault(fault, laddr, user, rw);

    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);

    if (leaf == BX_LEVEL_PTE) break;

    if (curr_entry & 0x80) {
      if (leaf > (BX_LEVEL_PDE + !!is_cpu_extension_supported(BX_ISA_1G_PAGES))) {
        BX_DEBUG(("long mode %s: PS bit set !", bx_paging_level[leaf]));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, user, rw);
      }

      ppf &= BX_CONST64(0x000fffffffffe000);
      if (ppf & offset_mask) {
         BX_DEBUG(("long mode %s: reserved bit is set: 0x" FMT_ADDRX64, bx_paging_level[leaf], curr_entry));
         page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, user, rw);
      }

      lpf_mask = (Bit32u) offset_mask;
      break;
    }

    combined_access &= curr_entry; // U/S and R/W
  }

  bool isWrite = (rw & 1); // write or r-m-w

#if BX_SUPPORT_PKEYS
  if (rw != BX_EXECUTE) {
    if (BX_CPU_THIS_PTR cr4.get_PKE()) {
      pkey = (entry[leaf] >> 59) & 0xf;

      // check of accessDisable bit set
      if (user) {
        if (BX_CPU_THIS_PTR pkru & (1<<(pkey*2))) {
          BX_ERROR(("protection key access not allowed PKRU=%x pkey=%d", BX_CPU_THIS_PTR pkru, pkey));
          page_fault(ERROR_PROTECTION | ERROR_PKEY, laddr, user, rw);
        }
      }

      // check of writeDisable bit set
      if (BX_CPU_THIS_PTR pkru & (1<<(pkey*2+1))) {
        if (isWrite && (user || BX_CPU_THIS_PTR cr0.get_WP())) {
          BX_ERROR(("protection key write not allowed PKRU=%x pkey=%d", BX_CPU_THIS_PTR pkru, pkey));
          page_fault(ERROR_PROTECTION | ERROR_PKEY, laddr, user, rw);
        }
      }
    }

    if (BX_CPU_THIS_PTR cr4.get_PKS() && !user) {
      pkey = (entry[leaf] >> 59) & 0xf;

      // check of accessDisable bit set
      if (BX_CPU_THIS_PTR pkrs & (1<<(pkey*2))) {
        BX_ERROR(("protection key access not allowed PKRS=%x pkey=%d", BX_CPU_THIS_PTR pkrs, pkey));
        page_fault(ERROR_PROTECTION | ERROR_PKEY, laddr, user, rw);
      }

      // check of writeDisable bit set
      if (BX_CPU_THIS_PTR pkrs & (1<<(pkey*2+1))) {
        if (isWrite && BX_CPU_THIS_PTR cr0.get_WP()) {
          BX_ERROR(("protection key write not allowed PKRS=%x pkey=%d", BX_CPU_THIS_PTR pkrs, pkey));
          page_fault(ERROR_PROTECTION | ERROR_PKEY, laddr, user, rw);
        }
      }
    }
  }
#endif

#if BX_SUPPORT_CET
  bool shadow_stack = (rw & 4) != 0;
  if (shadow_stack) {
    // shadow stack pages:
    //  - R/W bit=1 in every paging structure entry except the leaf
    //  - R/W bit=0 and Dirty=1 for leaf entry
    bool shadow_stack_page = ((combined_access & BX_COMBINED_ACCESS_WRITE) != 0) && ((entry[leaf] & 0x40) != 0) && ((entry[leaf] & 0x02) == 0);
    if (!shadow_stack_page) {
      BX_DEBUG(("shadow stack access to not shadow stack page CA=%x entry=%lx\n", combined_access, Bit32u(entry[leaf] & 0xfff)));
      page_fault(ERROR_PROTECTION, laddr, user, rw);
    }

    combined_access &= entry[leaf]; // U/S and R/W

    // must be to shadow stack page, check that U/S match
    if ((combined_access & BX_COMBINED_ACCESS_USER) ^ (user << 2)) {
      BX_DEBUG(("shadow stack U/S access mismatch"));
      page_fault(ERROR_PROTECTION, laddr, user, rw);
    }
    combined_access |= BX_COMBINED_SHADOW_STACK;
  }
  else
#endif
  {
    combined_access &= entry[leaf]; // U/S and R/W

    unsigned priv_index = (BX_CPU_THIS_PTR cr0.get_WP() << 4) | // bit 4
                          (user<<3) |                           // bit 3
                          (combined_access | (unsigned)isWrite);// bit 2,1,0

    if (!priv_check[priv_index] || nx_fault)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  if (BX_CPU_THIS_PTR cr4.get_SMEP() && rw == BX_EXECUTE && !user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  // SMAP protections are disabled if EFLAGS.AC=1
  if (BX_CPU_THIS_PTR cr4.get_SMAP() && ! BX_CPU_THIS_PTR get_AC() && rw != BX_EXECUTE && ! user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  if (BX_CPU_THIS_PTR cr4.get_PGE())
    combined_access |= (entry[leaf] & BX_COMBINED_GLOBAL_PAGE);

#if BX_SUPPORT_MEMTYPE
  combined_access |= (memtype_by_pat(calculate_pat((Bit32u) entry[leaf], lpf_mask)) << 9);
#endif

  // Update A/D bits if needed
  update_access_dirty_PAE(entry_addr, entry, entry_memtype, BX_LEVEL_PML4, leaf, isWrite);

  return (ppf | combined_access);
}

#endif

void BX_CPU_C::update_access_dirty_PAE(bx_phy_address *entry_addr, Bit64u *entry, BxMemtype *entry_memtype, unsigned max_level, unsigned leaf, unsigned write)
{
  // Update A bit if needed
  for (unsigned level=max_level; level > leaf; level--) {
    if (!(entry[level] & 0x20)) {
      entry[level] |= 0x20;
      access_write_physical(entry_addr[level], 8, &entry[level]);
      BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[level], 8, entry_memtype[level], BX_WRITE,
            (BX_PTE_ACCESS + level), (Bit8u*)(&entry[level]));
    }
  }

  // Update A/D bits if needed
  if (!(entry[leaf] & 0x20) || (write && !(entry[leaf] & 0x40))) {
    entry[leaf] |= (0x20 | (write<<6)); // Update A and possibly D bits
    access_write_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, entry_memtype[leaf], BX_WRITE,
            (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));
  }
}

//          Format of Legacy PAE PDPTR entry (PDPTE)
// -----------------------------------------------------------
// 00    | Present (P)
// 02-01 | Reserved (must be zero)
// 03    | Page-Level Write-Through (PWT) (486+), 0=reserved otherwise
// 04    | Page-Level Cache-Disable (PCD) (486+), 0=reserved otherwise
// 08-05 | Reserved (must be zero)
// 11-09 | (ignored)
// PA-12 | Physical address of 4-KByte aligned page directory
// 63-PA | Reserved (must be zero)
// -----------------------------------------------------------

const Bit64u PAGING_PAE_PDPTE_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0xFFF00000000001E6);

bool BX_CPP_AttrRegparmN(1) BX_CPU_C::CheckPDPTR(bx_phy_address cr3_val)
{
  // with Nested Paging PDPTRs are not loaded for guest page tables but
  // accessed on demand as part of the guest page walk
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED)
    return 1;
#endif

  cr3_val &= 0xffffffe0;
#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE))
      cr3_val = translate_guest_physical(cr3_val, 0, false /* laddr_valid */, true /* page walk */, 0, BX_READ);
  }
#endif

  Bit64u pdptr[4];
  unsigned n;

  for (n=0; n<4; n++) {
    // read and check PDPTE entries
    bx_phy_address pdpe_entry_addr = (bx_phy_address) (cr3_val | (n << 3));
    access_read_physical(pdpe_entry_addr, 8, &(pdptr[n]));
    BX_NOTIFY_PHY_MEMORY_ACCESS(pdpe_entry_addr, 8, BX_MEMTYPE_INVALID, BX_READ, (BX_PDPTR0_ACCESS + n), (Bit8u*) &(pdptr[n]));

    if (pdptr[n] & 0x1) {
       if (pdptr[n] & PAGING_PAE_PDPTE_RESERVED_BITS) return 0;
    }
  }

  // load new PDPTRs
  for (n=0; n<4; n++)
    BX_CPU_THIS_PTR PDPTR_CACHE.entry[n] = pdptr[n];

  return 1; /* PDPTRs are fine */
}

#if BX_SUPPORT_VMX >= 2
bool BX_CPP_AttrRegparmN(1) BX_CPU_C::CheckPDPTR(Bit64u *pdptr)
{
  for (unsigned n=0; n<4; n++) {
     if (pdptr[n] & 0x1) {
        if (pdptr[n] & PAGING_PAE_PDPTE_RESERVED_BITS) return 0;
     }
  }

  return 1; /* PDPTRs are fine */
}
#endif

bx_phy_address BX_CPU_C::translate_linear_load_PDPTR(bx_address laddr, unsigned user, unsigned rw)
{
  unsigned index = (laddr >> 30) & 0x3;
  Bit64u pdptr;

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED) 
  {
    bx_phy_address cr3_val = BX_CPU_THIS_PTR cr3 & 0xffffffe0;
    cr3_val = nested_walk(cr3_val, BX_RW, 1);

    bx_phy_address pdpe_entry_addr = (bx_phy_address) (cr3_val | (index << 3));
    access_read_physical(pdpe_entry_addr, 8, &pdptr);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pdpe_entry_addr, 8, BX_MEMTYPE_INVALID, BX_READ, (BX_PDPTR0_ACCESS + index), (Bit8u*) &pdptr);

    if (pdptr & 0x1) {
      if (pdptr & PAGING_PAE_PDPTE_RESERVED_BITS) {
        BX_DEBUG(("PAE PDPTE%d entry reserved bits set: 0x" FMT_ADDRX64, index, pdptr));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, user, rw);
      }
    }
  }
  else
#endif
  {
    pdptr = BX_CPU_THIS_PTR PDPTR_CACHE.entry[index];
  }

  if (! (pdptr & 0x1)) {
    BX_DEBUG(("PAE PDPTE entry not present !"));
    page_fault(ERROR_NOT_PRESENT, laddr, user, rw);
  }

  return pdptr;
}

// Translate a linear address to a physical address in PAE paging mode
bx_phy_address BX_CPU_C::translate_linear_PAE(bx_address laddr, Bit32u &lpf_mask, unsigned user, unsigned rw)
{
  bx_phy_address entry_addr[2];
  Bit64u entry[2];
  BxMemtype entry_memtype[2] = { 0 };
  bool nx_fault = false;
  int leaf;

  lpf_mask = 0xfff;
  Bit32u combined_access = (BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER);

  Bit64u reserved = PAGING_LEGACY_PAE_RESERVED_BITS;
  if (! BX_CPU_THIS_PTR efer.get_NXE())
    reserved |= PAGE_DIRECTORY_NX_BIT;

  Bit64u pdpte = translate_linear_load_PDPTR(laddr, user, rw);
  bx_phy_address ppf = pdpte & BX_CONST64(0x000ffffffffff000);
  Bit64u curr_entry = pdpte;

  for (leaf = BX_LEVEL_PDE;; --leaf) {
    entry_addr[leaf] = ppf + ((laddr >> (9 + 9*leaf)) & 0xff8);
#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE))
        entry_addr[leaf] = translate_guest_physical(entry_addr[leaf], laddr, true /* laddr_valid */, true /* page walk */, IS_USER_PAGE(combined_access), BX_READ);
    }
#endif
#if BX_SUPPORT_SVM
    if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED) {
      entry_addr[leaf] = nested_walk(entry_addr[leaf], BX_RW, 1);
    }
#endif

#if BX_SUPPORT_MEMTYPE
    entry_memtype[leaf] = resolve_memtype(memtype_by_mtrr(entry_addr[leaf]), memtype_by_pat(calculate_pcd_pwt((Bit32u) curr_entry)));
#endif
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, entry_memtype[leaf], BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    curr_entry = entry[leaf];
    int fault = check_entry_PAE(bx_paging_level[leaf], curr_entry, reserved, rw, &nx_fault);
    if (fault >= 0)
      page_fault(fault, laddr, user, rw);

    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);

    if (leaf == BX_LEVEL_PTE) break;

    // Ignore CR4.PSE in PAE mode
    if (curr_entry & 0x80) {
      if (curr_entry & PAGING_PAE_PDE2M_RESERVED_BITS) {
        BX_DEBUG(("PAE PDE2M: reserved bit is set PDE=0x" FMT_ADDRX64, curr_entry));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, user, rw);
      }

      // Make up the physical page frame address
      ppf = (bx_phy_address)(curr_entry & BX_CONST64(0x000fffffffe00000));
      lpf_mask = 0x1fffff;
      break;
    }

    combined_access &= curr_entry; // U/S and R/W
  }

  bool isWrite = (rw & 1); // write or r-m-w

#if BX_SUPPORT_CET
  bool shadow_stack = (rw & 4) != 0;
  if (shadow_stack) {
    // shadow stack pages:
    //  - R/W bit=1 in every paging structure entry except the leaf
    //  - R/W bit=0 and Dirty=1 for leaf entry
    bool shadow_stack_page = ((combined_access & BX_COMBINED_ACCESS_WRITE) != 0) && ((entry[leaf] & 0x40) != 0) && ((entry[leaf] & 0x02) == 0);
    if (!shadow_stack_page)
      page_fault(ERROR_PROTECTION, laddr, user, rw);

    combined_access &= entry[leaf]; // U/S and R/W

    // must be to shadow stack page, check that U/S match
    if ((combined_access & BX_COMBINED_ACCESS_USER) ^ (user << 2)) {
      BX_DEBUG(("shadow stack U/S access mismatch"));
      page_fault(ERROR_PROTECTION, laddr, user, rw);
    }
    combined_access |= BX_COMBINED_SHADOW_STACK;
  }
  else
#endif
  {
    combined_access &= entry[leaf]; // U/S and R/W

    unsigned priv_index = (BX_CPU_THIS_PTR cr0.get_WP() << 4) | // bit 4
                          (user<<3) |                           // bit 3
                          (combined_access | (unsigned)isWrite);// bit 2,1,0

    if (!priv_check[priv_index] || nx_fault)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  if (BX_CPU_THIS_PTR cr4.get_SMEP() && rw == BX_EXECUTE && !user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  // SMAP protections are disabled if EFLAGS.AC=1
  if (BX_CPU_THIS_PTR cr4.get_SMAP() && ! BX_CPU_THIS_PTR get_AC() && rw != BX_EXECUTE && ! user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  if (BX_CPU_THIS_PTR cr4.get_PGE())
    combined_access |= (entry[leaf] & BX_COMBINED_GLOBAL_PAGE); // G

#if BX_SUPPORT_MEMTYPE
  combined_access |= (memtype_by_pat(calculate_pat((Bit32u) entry[leaf], lpf_mask)) << 9);
#endif

  // Update A/D bits if needed
  update_access_dirty_PAE(entry_addr, entry, entry_memtype, BX_LEVEL_PDE, leaf, isWrite);

  return (ppf | combined_access);
}

#endif

//           Format of a PDE that Maps a 4-MByte Page
// -----------------------------------------------------------
// 00    | Present (P)
// 01    | R/W
// 02    | U/S
// 03    | Page-Level Write-Through (PWT)
// 04    | Page-Level Cache-Disable (PCD)
// 05    | Accessed (A)
// 06    | Dirty (D)
// 07    | Page size, must be 1 to indicate 4-Mbyte page
// 08    | Global (G) (if CR4.PGE=1, ignored otherwise)
// 11-09 | (ignored)
// 12    | PAT (if PAT is supported, reserved otherwise)
// PA-13 | Bits PA-32 of physical address of the 4-MByte page
// 21-PA | Reserved (must be zero)
// 31-22 | Bits 31-22 of physical address of the 4-MByte page
// -----------------------------------------------------------

#if BX_PHY_ADDRESS_WIDTH > 40
const Bit32u PAGING_PDE4M_RESERVED_BITS = 0; // there are no reserved bits in PDE4M when physical address is wider than 40 bit
#else
const Bit32u PAGING_PDE4M_RESERVED_BITS = ((1 << (41-BX_PHY_ADDRESS_WIDTH))-1) << (13 + BX_PHY_ADDRESS_WIDTH - 32);
#endif

// Translate a linear address to a physical address in legacy paging mode
bx_phy_address BX_CPU_C::translate_linear_legacy(bx_address laddr, Bit32u &lpf_mask, unsigned user, unsigned rw)
{
  bx_phy_address entry_addr[2], ppf = (Bit32u) BX_CPU_THIS_PTR cr3 & BX_CR3_PAGING_MASK;
  Bit32u entry[2];
  BxMemtype entry_memtype[2] = { 0 };
  int leaf;

  lpf_mask = 0xfff;
  Bit32u combined_access = (BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER);
  Bit32u curr_entry = (Bit32u) BX_CPU_THIS_PTR cr3;

  for (leaf = BX_LEVEL_PDE;; --leaf) {
    entry_addr[leaf] = ppf + ((laddr >> (10 + 10*leaf)) & 0xffc);
#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE))
        entry_addr[leaf] = translate_guest_physical(entry_addr[leaf], laddr, true /* laddr_valid */, true /* page walk */, IS_USER_PAGE(combined_access), BX_READ);
    }
#endif
#if BX_SUPPORT_SVM
    if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED) {
      entry_addr[leaf] = nested_walk(entry_addr[leaf], BX_RW, 1);
    }
#endif

#if BX_SUPPORT_MEMTYPE
    entry_memtype[leaf] = resolve_memtype(memtype_by_mtrr(entry_addr[leaf]), memtype_by_pat(calculate_pcd_pwt(curr_entry)));
#endif
    access_read_physical(entry_addr[leaf], 4, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 4, entry_memtype[leaf], BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    curr_entry = entry[leaf];
    if (!(curr_entry & 0x1)) {
      BX_DEBUG(("%s: entry not present", bx_paging_level[leaf]));
      page_fault(ERROR_NOT_PRESENT, laddr, user, rw);
    }

    ppf = curr_entry & 0xfffff000;

    if (leaf == BX_LEVEL_PTE) break;

#if BX_CPU_LEVEL >= 5
    if ((curr_entry & 0x80) != 0 && BX_CPU_THIS_PTR cr4.get_PSE()) {
      // 4M paging, only if CR4.PSE enabled, ignore PDE.PS otherwise
      if (curr_entry & PAGING_PDE4M_RESERVED_BITS) {
        BX_DEBUG(("PSE PDE4M: reserved bit is set: PDE=0x%08x", entry[BX_LEVEL_PDE]));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, user, rw);
      }

      // make up the physical frame number
      ppf = (curr_entry & 0xffc00000);
#if BX_PHY_ADDRESS_WIDTH > 32
      ppf |= ((bx_phy_address)(curr_entry & 0x003fe000)) << 19;
#endif
      lpf_mask = 0x3fffff;
      break;
    }
#endif

    combined_access &= curr_entry; // U/S and R/W
  }

  bool isWrite = (rw & 1); // write or r-m-w

#if BX_SUPPORT_CET
  bool shadow_stack = (rw & 4) != 0;
  if (shadow_stack) {
    // shadow stack pages:
    //  - R/W bit=1 in every paging structure entry except the leaf
    //  - R/W bit=0 and Dirty=1 for leaf entry
    bool shadow_stack_page = ((combined_access & BX_COMBINED_ACCESS_WRITE) != 0) && ((entry[leaf] & 0x40) != 0) && ((entry[leaf] & 0x02) == 0);
    if (!shadow_stack_page)
      page_fault(ERROR_PROTECTION, laddr, user, rw);

    combined_access &= entry[leaf]; // U/S and R/W

    // must be to shadow stack page, check that U/S match
    if ((combined_access & BX_COMBINED_ACCESS_USER) ^ (user << 2)) {
      BX_DEBUG(("shadow stack U/S access mismatch"));
      page_fault(ERROR_PROTECTION, laddr, user, rw);
    }
    combined_access |= BX_COMBINED_SHADOW_STACK;
  }
  else
#endif
  {
    combined_access &= entry[leaf]; // U/S and R/W

    unsigned priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.get_WP() << 4) |   // bit 4
#endif
        (user<<3) |                             // bit 3
        (combined_access | (unsigned)isWrite);  // bit 2,1,0

    if (!priv_check[priv_index])
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

#if BX_CPU_LEVEL >= 6
  if (BX_CPU_THIS_PTR cr4.get_SMEP() && rw == BX_EXECUTE && !user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  // SMAP protections are disabled if EFLAGS.AC=1
  if (BX_CPU_THIS_PTR cr4.get_SMAP() && ! BX_CPU_THIS_PTR get_AC() && rw != BX_EXECUTE && ! user) {
    if (combined_access & BX_COMBINED_ACCESS_USER)
      page_fault(ERROR_PROTECTION, laddr, user, rw);
  }

  if (BX_CPU_THIS_PTR cr4.get_PGE())
    combined_access |= (entry[leaf] & BX_COMBINED_GLOBAL_PAGE);

#if BX_SUPPORT_MEMTYPE
  combined_access |= (memtype_by_pat(calculate_pat(entry[leaf], lpf_mask)) << 9);
#endif

#endif

  update_access_dirty(entry_addr, entry, entry_memtype, leaf, isWrite);

  return (ppf | combined_access);
}

void BX_CPU_C::update_access_dirty(bx_phy_address *entry_addr, Bit32u *entry, BxMemtype *entry_memtype, unsigned leaf, unsigned write)
{
  if (leaf == BX_LEVEL_PTE) {
    // Update PDE A bit if needed
    if (!(entry[BX_LEVEL_PDE] & 0x20)) {
      entry[BX_LEVEL_PDE] |= 0x20;
      access_write_physical(entry_addr[BX_LEVEL_PDE], 4, &entry[BX_LEVEL_PDE]);
      BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[BX_LEVEL_PDE], 4, entry_memtype[BX_LEVEL_PDE], BX_WRITE, BX_PDE_ACCESS, (Bit8u*)(&entry[BX_LEVEL_PDE]));
    }
  }

  // Update A/D bits if needed
  if (!(entry[leaf] & 0x20) || (write && !(entry[leaf] & 0x40))) {
    entry[leaf] |= (0x20 | (write<<6)); // Update A and possibly D bits
    access_write_physical(entry_addr[leaf], 4, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 4, entry_memtype[leaf], BX_WRITE, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));
  }
}

// Translate a linear address to a physical address
bx_phy_address BX_CPU_C::translate_linear(bx_TLB_entry *tlbEntry, bx_address laddr, unsigned user, unsigned rw)
{
#if BX_SUPPORT_X86_64
  if (! long_mode()) laddr &= 0xffffffff;
#endif

  bx_phy_address paddress, ppf, poffset = PAGE_OFFSET(laddr);
  unsigned isWrite = rw & 1; // write or r-m-w
  unsigned isExecute = (rw == BX_EXECUTE);
  unsigned isShadowStack = (rw & 4); // 4 if shadowstack and 0 otherwise
  bx_address lpf = LPFOf(laddr);

  INC_TLB_STAT(tlbLookups);
  if (isExecute)
    INC_TLB_STAT(tlbExecuteLookups);
  if (isWrite)
    INC_TLB_STAT(tlbWriteLookups);

  // already looked up TLB for code access
  if (! isExecute && TLB_LPFOf(tlbEntry->lpf) == lpf)
  {
    paddress = tlbEntry->ppf | poffset;

#if BX_SUPPORT_PKEYS
    if (isWrite) {
      if (tlbEntry->accessBits & (1 << (isShadowStack | (isWrite<<1) | user)) & BX_CPU_THIS_PTR wr_pkey[tlbEntry->pkey])
        return paddress;
    }
    else {
      if (tlbEntry->accessBits & (1 << (isShadowStack | user)) & BX_CPU_THIS_PTR rd_pkey[tlbEntry->pkey])
        return paddress;
    }
#else
    if (tlbEntry->accessBits & (1 << (isShadowStack | (isWrite<<1) | user)))
      return paddress;
#endif

    // The current access does not have permission according to the info
    // in our TLB cache entry.  Re-walk the page tables, in case there is
    // updated information in the memory image, and let the long path code
    // generate an exception if one is warranted.

    // Invalidate the TLB entry before re-walk as re-walk may end with paging fault.
    // The entry will be reinitialized later if page walk succeeds.
    tlbEntry->invalidate();
  }

  INC_TLB_STAT(tlbMisses);
  if (isExecute)
    INC_TLB_STAT(tlbExecuteMisses);
  if (isWrite)
    INC_TLB_STAT(tlbWriteMisses);

  Bit32u lpf_mask = 0xfff; // 4K pages
  Bit32u combined_access = BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER;
#if BX_SUPPORT_X86_64
  Bit32u pkey = 0;
#endif

  if(BX_CPU_THIS_PTR cr0.get_PG())
  {
    BX_DEBUG(("page walk for%s address 0x" FMT_LIN_ADDRX, isShadowStack ? " shadow stack" : "", laddr));

#if BX_CPU_LEVEL >= 6
#if BX_SUPPORT_X86_64
    if (long_mode())
      paddress = translate_linear_long_mode(laddr, lpf_mask, pkey, user, rw);
    else
#endif
      if (BX_CPU_THIS_PTR cr4.get_PAE())
        paddress = translate_linear_PAE(laddr, lpf_mask, user, rw);
      else
#endif 
        paddress = translate_linear_legacy(laddr, lpf_mask, user, rw);

    // translate_linear functions return combined U/S, R/W bits, Global Page bit
    // and also effective page tables memory type in lower 12 bits of the physical address.
    // Bit 1 - R/W bit
    // Bit 2 - U/S bit
    // Bit 9,10,11 - Effective Memory Table from page tables
    combined_access = paddress & lpf_mask;
    paddress = (paddress & ~((Bit64u) lpf_mask)) | (laddr & lpf_mask);

#if BX_CPU_LEVEL >= 5
    if (lpf_mask > 0xfff) {
      if (isExecute)
        BX_CPU_THIS_PTR ITLB.split_large = true;
      else
        BX_CPU_THIS_PTR DTLB.split_large = true;
    }
#endif
  }
  else {
    // no paging
    paddress = (bx_phy_address) laddr;
    combined_access |= (BX_MEMTYPE_WB << 9); // act as memory type by paging is WB
  }

  // Calculate physical memory address and fill in TLB cache entry
#if BX_SUPPORT_VMX >= 2
  bool spp_page = false;
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE)) {
      paddress = translate_guest_physical(paddress, laddr, true /* laddr_valid */, false /* page walk */, IS_USER_PAGE(combined_access), rw, isShadowStack & !user, &spp_page);
    }
  }
#endif
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED) {
    // hack: ignore isExecute attribute in SMM mode under SVM virtualization
    if (BX_CPU_THIS_PTR in_smm && rw == BX_EXECUTE) rw = BX_READ;

    paddress = nested_walk(paddress, rw, 0);
  }
#endif
  paddress = A20ADDR(paddress);
  ppf = PPFOf(paddress);

  // direct memory access is NOT allowed by default
  tlbEntry->lpf = lpf | TLB_NoHostPtr;
  tlbEntry->lpf_mask = lpf_mask;
#if BX_SUPPORT_PKEYS
  tlbEntry->pkey = pkey;
#endif
  tlbEntry->ppf = ppf;
  tlbEntry->accessBits = 0;

  if (isExecute) {
    tlbEntry->accessBits |= TLB_SysExecuteOK;
  }
  else {
#if BX_SUPPORT_CET
    if (isShadowStack) {
      tlbEntry->accessBits |= TLB_SysReadOK | TLB_SysReadShadowStackOK;
      if (isWrite)
        tlbEntry->accessBits |= TLB_SysWriteShadowStackOK;
    }
    else
#endif
    {
      tlbEntry->accessBits |= TLB_SysReadOK;
      if (isWrite)
        tlbEntry->accessBits |= TLB_SysWriteOK;
    }
  }

  if (! BX_CPU_THIS_PTR cr0.get_PG()
#if BX_SUPPORT_VMX >= 2
        && ! (BX_CPU_THIS_PTR in_vmx_guest && SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE))
#endif
#if BX_SUPPORT_SVM
        && ! (BX_CPU_THIS_PTR in_svm_guest && SVM_NESTED_PAGING_ENABLED)
#endif
    ) {
    if (isExecute)
      tlbEntry->accessBits |= TLB_UserExecuteOK;
    else
      tlbEntry->accessBits |= TLB_UserReadOK | TLB_UserWriteOK;
  }
  else {
    if ((combined_access & BX_COMBINED_ACCESS_USER) != 0) {

      if (user) {
        if (isExecute) {
          tlbEntry->accessBits |= TLB_UserExecuteOK;
        }
        else {
#if BX_SUPPORT_CET
          if (isShadowStack) {
            tlbEntry->accessBits |= TLB_UserReadOK | TLB_UserReadShadowStackOK;
            if (isWrite)
              tlbEntry->accessBits |= TLB_UserWriteShadowStackOK;
          }
          else
#endif
          {
            tlbEntry->accessBits |= TLB_UserReadOK;
            if (isWrite)
              tlbEntry->accessBits |= TLB_UserWriteOK;
          }
        }
      }

#if BX_CPU_LEVEL >= 6
      if (isExecute) {
        if (BX_CPU_THIS_PTR cr4.get_SMEP())
          tlbEntry->accessBits &= ~TLB_SysExecuteOK;
      }
      else {
        if (BX_CPU_THIS_PTR cr4.get_SMAP())
          tlbEntry->accessBits &= ~(TLB_SysReadOK | TLB_SysWriteOK);
      }
#endif

#if BX_SUPPORT_CET
      // system shadow stack accesses cannot access user pages
      tlbEntry->accessBits &= ~(TLB_SysReadShadowStackOK | TLB_SysWriteShadowStackOK);
#endif
    }
  }

#if BX_SUPPORT_VMX >= 2
  if (spp_page) {
    // the page was write-allowed only due to SPP, such pages cannot be cached as WriteOK
    tlbEntry->accessBits &= ~(TLB_SysWriteShadowStackOK | TLB_UserWriteShadowStackOK | TLB_SysWriteOK | TLB_UserWriteOK);
  }
#endif

#if BX_CPU_LEVEL >= 6
  if (combined_access & BX_COMBINED_GLOBAL_PAGE) // Global bit
    tlbEntry->accessBits |= TLB_GlobalPage;
#endif

  // Attempt to get a host pointer to this physical page. Put that
  // pointer in the TLB cache. Note if the request is vetoed, NULL
  // will be returned, and it's OK to OR zero in anyways.
  tlbEntry->hostPageAddr = BX_CPU_THIS_PTR getHostMemAddr(ppf, rw);
  if (tlbEntry->hostPageAddr) {
    // All access allowed also via direct pointer
#if BX_X86_DEBUGGER
    if (! hwbreakpoint_check(laddr, BX_HWDebugMemW, BX_HWDebugMemRW))
#endif
       tlbEntry->lpf = lpf; // allow direct access with HostPtr
  }

#if BX_SUPPORT_MEMTYPE
  tlbEntry->memtype = resolve_memtype(memtype_by_mtrr(tlbEntry->ppf), combined_access >> 9 /* effective page tables memory type */);
#endif

  return paddress;
}

const char *get_memtype_name(BxMemtype memtype)
{
  static const char *mem_type_string[9] = { "UC", "WC", "RESERVED2", "RESERVED3", "WT", "WP", "WB", "UC-", "INVALID" };
  if (memtype > BX_MEMTYPE_INVALID) memtype = BX_MEMTYPE_INVALID;
  return mem_type_string[memtype];
}

#if BX_SUPPORT_MEMTYPE
BxMemtype BX_CPP_AttrRegparmN(1) BX_CPU_C::memtype_by_mtrr(bx_phy_address pAddr)
{
#if BX_CPU_LEVEL >= 6
  if (is_cpu_extension_supported(BX_ISA_MTRR)) {
    const Bit32u BX_MTRR_DEFTYPE_FIXED_MTRR_ENABLE_MASK = (1 << 10);
    const Bit32u BX_MTRR_ENABLE_MASK = (1 << 11);

    if (BX_CPU_THIS_PTR msr.mtrr_deftype & BX_MTRR_ENABLE_MASK) {
      // fixed range MTRR take priority over variable range MTRR when enabled
      if (pAddr < 0x100000 && (BX_CPU_THIS_PTR msr.mtrr_deftype & BX_MTRR_DEFTYPE_FIXED_MTRR_ENABLE_MASK)) {
        if (pAddr < 0x80000) {
          unsigned index = (pAddr >> 16) & 0x7;
          return (BxMemtype) BX_CPU_THIS_PTR msr.mtrrfix64k.ubyte(index);
        }
        if (pAddr < 0xc0000) {
          unsigned index = ((pAddr - 0x80000) >> 14) & 0xf;
          return (BxMemtype) BX_CPU_THIS_PTR msr.mtrrfix16k[index >> 3].ubyte(index & 0x7);
        }
        else {
          unsigned index =  (pAddr - 0xc0000) >> 12;
          return (BxMemtype) BX_CPU_THIS_PTR msr.mtrrfix4k [index >> 3].ubyte(index & 0x7);
        }
      }

      int memtype = -1;

      for (unsigned i=0; i < BX_NUM_VARIABLE_RANGE_MTRRS; i++) {
        Bit64u base = BX_CPU_THIS_PTR msr.mtrrphys[i*2];
        Bit64u mask = BX_CPU_THIS_PTR msr.mtrrphys[i*2 + 1];
        if ((mask & BX_MTRR_ENABLE_MASK) == 0) continue;
        mask = PPFOf(mask);
        if ((pAddr & mask) == (base & mask)) {
          //
          // Matched variable MTRR, check overlap rules:
          // - if two or more variable memory ranges match and the memory types are identical,
          //   then that memory type is used.
          // - if two or more variable memory ranges match and one of the memory types is UC,
          //   the UC memory type used.
          // - if two or more variable memory ranges match and the memory types are WT and WB,
          //   the WT memory type is used.
          // - For overlaps not defined by the above rules, processor behavior is undefined.
          //
          BxMemtype curr_memtype = BxMemtype(base & 0xff);
          if (curr_memtype == BX_MEMTYPE_UC)
            return BX_MEMTYPE_UC;

          if (memtype == -1) {
            memtype = curr_memtype; // first match
          }
          else if (memtype != (int) curr_memtype) {
            if (curr_memtype == BX_MEMTYPE_WT && memtype == BX_MEMTYPE_WB)
              memtype = BX_MEMTYPE_WT;
            else if (curr_memtype == BX_MEMTYPE_WB && memtype == BX_MEMTYPE_WT)
              memtype = BX_MEMTYPE_WT;
            else
              memtype = BX_MEMTYPE_INVALID;
          }
        }
      }

      if (memtype != -1)
        return BxMemtype(memtype);

      // didn't match any variable range MTRR, return default memory type
      return BxMemtype(BX_CPU_THIS_PTR msr.mtrr_deftype & 0xff);
    }

    // return UC memory type when MTRRs are not enabled
    return BX_MEMTYPE_UC;
  }
#endif

  // return INVALID memory type when MTRRs are not supported
  return BX_MEMTYPE_INVALID;
}

BxMemtype BX_CPP_AttrRegparmN(1) BX_CPU_C::memtype_by_pat(unsigned pat)
{
  return (BxMemtype) BX_CPU_THIS_PTR msr.pat.ubyte(pat);
}

BxMemtype BX_CPP_AttrRegparmN(2) BX_CPU_C::resolve_memtype(BxMemtype mtrr_memtype, BxMemtype pat_memtype)
{
  if (BX_CPU_THIS_PTR cr0.get_CD())
    return BX_MEMTYPE_UC;

  if (mtrr_memtype == BX_MEMTYPE_INVALID) // will result in ignore of MTRR memory type
    mtrr_memtype = BX_MEMTYPE_WB;

  switch(pat_memtype) {
    case BX_MEMTYPE_UC:
    case BX_MEMTYPE_WC:
      return pat_memtype;

    case BX_MEMTYPE_WT:
    case BX_MEMTYPE_WP:
      if (mtrr_memtype == BX_MEMTYPE_WC) return BX_MEMTYPE_UC;
      return (mtrr_memtype < pat_memtype) ? mtrr_memtype : pat_memtype;

    case BX_MEMTYPE_WB:
      return mtrr_memtype;

    case BX_MEMTYPE_UC_WEAK:
      return (mtrr_memtype == BX_MEMTYPE_WC) ? BX_MEMTYPE_WC : BX_MEMTYPE_UC;

    default:
      BX_PANIC(("unexpected PAT memory type: %u", (unsigned) pat_memtype));
  }

  return BX_MEMTYPE_INVALID; // keep compiler happy
}
#endif

#if BX_SUPPORT_SVM

void BX_CPU_C::nested_page_fault(unsigned fault, bx_phy_address guest_paddr, unsigned rw, unsigned is_page_walk)
{
  unsigned isWrite = rw & 1;

  Bit64u error_code = fault | (1 << 2) | (isWrite << 1);
  if (rw == BX_EXECUTE)
    error_code |= ERROR_CODE_ACCESS; // I/D = 1

  if (is_page_walk)
    error_code |= BX_CONST64(1) << 33;
  else
    error_code |= BX_CONST64(1) << 32;

  Svm_Vmexit(SVM_VMEXIT_NPF, error_code, guest_paddr);
}

bx_phy_address BX_CPU_C::nested_walk_long_mode(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk)
{
  bx_phy_address entry_addr[4];
  Bit64u entry[4];
  BxMemtype entry_memtype[4] = { BX_MEMTYPE_INVALID };
  bool nx_fault = false;
  int leaf;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;
  SVM_HOST_STATE *host_state = &BX_CPU_THIS_PTR vmcb.host_state;
  bx_phy_address ppf = ctrls->ncr3 & BX_CR3_PAGING_MASK;
  Bit64u offset_mask = BX_CONST64(0x0000ffffffffffff);
  unsigned combined_access = BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER;

  Bit64u reserved = PAGING_PAE_RESERVED_BITS;
  if (! host_state->efer.get_NXE())
    reserved |= PAGE_DIRECTORY_NX_BIT;

  for (leaf = BX_LEVEL_PML4;; --leaf) {
    entry_addr[leaf] = ppf + ((guest_paddr >> (9 + 9*leaf)) & 0xff8);
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, BX_MEMTYPE_INVALID, BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));
    offset_mask >>= 9;

    Bit64u curr_entry = entry[leaf];
    int fault = check_entry_PAE(bx_paging_level[leaf], curr_entry, reserved, rw, &nx_fault);
    if (fault >= 0)
      nested_page_fault(fault, guest_paddr, rw, is_page_walk);

    combined_access &= curr_entry; // U/S and R/W
    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);

    if (leaf == BX_LEVEL_PTE) break;

    if (curr_entry & 0x80) {
      if (leaf > (BX_LEVEL_PDE + !!is_cpu_extension_supported(BX_ISA_1G_PAGES))) {
        BX_DEBUG(("Nested PAE Walk %s: PS bit set !", bx_paging_level[leaf]));
        nested_page_fault(ERROR_RESERVED | ERROR_PROTECTION, guest_paddr, rw, is_page_walk);
      }

      ppf &= BX_CONST64(0x000fffffffffe000);
      if (ppf & offset_mask) {
        BX_DEBUG(("Nested PAE Walk %s: reserved bit is set: 0x" FMT_ADDRX64, bx_paging_level[leaf], curr_entry));
        nested_page_fault(ERROR_RESERVED | ERROR_PROTECTION, guest_paddr, rw, is_page_walk);
      }

      break;
    }
  }

  bool isWrite = (rw & 1); // write or r-m-w

  unsigned priv_index = (1<<3) /* user */ | (combined_access | isWrite);

  if (!priv_check[priv_index] || nx_fault)
    nested_page_fault(ERROR_PROTECTION, guest_paddr, rw, is_page_walk);

  // Update A/D bits if needed
  update_access_dirty_PAE(entry_addr, entry, entry_memtype, BX_LEVEL_PML4, leaf, isWrite);

  // Make up the physical page frame address
  return ppf | (bx_phy_address)(guest_paddr & offset_mask);	
}

bx_phy_address BX_CPU_C::nested_walk_PAE(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk)
{
  bx_phy_address entry_addr[2];
  Bit64u entry[2];
  BxMemtype entry_memtype[2] = { BX_MEMTYPE_INVALID };
  bool nx_fault = false;
  int leaf;

  unsigned combined_access = BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;
  SVM_HOST_STATE *host_state = &BX_CPU_THIS_PTR vmcb.host_state;
  bx_phy_address ncr3 = ctrls->ncr3 & 0xffffffe0;
  unsigned index = (guest_paddr >> 30) & 0x3;
  Bit64u pdptr;

  bx_phy_address pdpe_entry_addr = (bx_phy_address) (ncr3 | (index << 3));
  access_read_physical(pdpe_entry_addr, 8, &pdptr);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pdpe_entry_addr, 8, BX_MEMTYPE_INVALID, BX_READ, (BX_PDPTR0_ACCESS + index), (Bit8u*) &pdptr);

  if (! (pdptr & 0x1)) {
    BX_DEBUG(("Nested PAE Walk PDPTE%d entry not present !", index));
    nested_page_fault(ERROR_NOT_PRESENT, guest_paddr, rw, is_page_walk);
  }

  if (pdptr & PAGING_PAE_PDPTE_RESERVED_BITS) {
    BX_DEBUG(("Nested PAE Walk PDPTE%d entry reserved bits set: 0x" FMT_ADDRX64, index, pdptr));
    nested_page_fault(ERROR_RESERVED | ERROR_PROTECTION, guest_paddr, rw, is_page_walk);
  }

  Bit64u reserved = PAGING_LEGACY_PAE_RESERVED_BITS;
  if (! host_state->efer.get_NXE())
    reserved |= PAGE_DIRECTORY_NX_BIT;

  bx_phy_address ppf = pdptr & BX_CONST64(0x000ffffffffff000);

  for (leaf = BX_LEVEL_PDE;; --leaf) {
    entry_addr[leaf] = ppf + ((guest_paddr >> (9 + 9*leaf)) & 0xff8);
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, BX_MEMTYPE_INVALID, BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    Bit64u curr_entry = entry[leaf];
    int fault = check_entry_PAE(bx_paging_level[leaf], curr_entry, reserved, rw, &nx_fault);
    if (fault >= 0)
      nested_page_fault(fault, guest_paddr, rw, is_page_walk);

    combined_access &= curr_entry; // U/S and R/W
    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);

    if (leaf == BX_LEVEL_PTE) break;

    // Ignore CR4.PSE in PAE mode
    if (curr_entry & 0x80) {
      if (curr_entry & PAGING_PAE_PDE2M_RESERVED_BITS) {
        BX_DEBUG(("PAE PDE2M: reserved bit is set PDE=0x" FMT_ADDRX64, curr_entry));
        nested_page_fault(ERROR_RESERVED | ERROR_PROTECTION, guest_paddr, rw, is_page_walk);
      }

      // Make up the physical page frame address
      ppf = (bx_phy_address)((curr_entry & BX_CONST64(0x000fffffffe00000)) | (guest_paddr & 0x001ff000));
      break;
    }
  }

  bool isWrite = (rw & 1); // write or r-m-w

  unsigned priv_index = (1<<3) /* user */ | (combined_access | isWrite);

  if (!priv_check[priv_index] || nx_fault)
    nested_page_fault(ERROR_PROTECTION, guest_paddr, rw, is_page_walk);

  // Update A/D bits if needed
  update_access_dirty_PAE(entry_addr, entry, entry_memtype, BX_LEVEL_PDE, leaf, isWrite);

  Bit32u page_offset = PAGE_OFFSET(guest_paddr);
  return ppf | page_offset;
}

bx_phy_address BX_CPU_C::nested_walk_legacy(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk)
{
  bx_phy_address entry_addr[2];
  Bit32u entry[2];
  BxMemtype entry_memtype[2] = { BX_MEMTYPE_INVALID };
  int leaf;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;
  SVM_HOST_STATE *host_state = &BX_CPU_THIS_PTR vmcb.host_state;
  bx_phy_address ppf = ctrls->ncr3 & BX_CR3_PAGING_MASK;
  unsigned combined_access = BX_COMBINED_ACCESS_WRITE | BX_COMBINED_ACCESS_USER;

  for (leaf = BX_LEVEL_PDE;; --leaf) {
    entry_addr[leaf] = ppf + ((guest_paddr >> (10 + 10*leaf)) & 0xffc);
    access_read_physical(entry_addr[leaf], 4, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 4, BX_MEMTYPE_INVALID, BX_READ, (BX_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    Bit32u curr_entry = entry[leaf];
    if (!(curr_entry & 0x1)) {
      BX_DEBUG(("Nested %s Walk: entry not present", bx_paging_level[leaf]));
      nested_page_fault(ERROR_NOT_PRESENT, guest_paddr, rw, is_page_walk);
    }

    combined_access &= curr_entry; // U/S and R/W
    ppf = curr_entry & 0xfffff000;

    if (leaf == BX_LEVEL_PTE) break;

    if ((curr_entry & 0x80) != 0 && host_state->cr4.get_PSE()) {
      // 4M paging, only if CR4.PSE enabled, ignore PDE.PS otherwise
      if (curr_entry & PAGING_PDE4M_RESERVED_BITS) {
        BX_DEBUG(("Nested PSE Walk PDE4M: reserved bit is set: PDE=0x%08x", entry[BX_LEVEL_PDE]));
        nested_page_fault(ERROR_RESERVED | ERROR_PROTECTION, guest_paddr, rw, is_page_walk);
      }

      // make up the physical frame number
      ppf = (curr_entry & 0xffc00000) | (guest_paddr & 0x003ff000);
#if BX_PHY_ADDRESS_WIDTH > 32
      ppf |= ((bx_phy_address)(curr_entry & 0x003fe000)) << 19;
#endif
      break;
    }
  }

  bool isWrite = (rw & 1); // write or r-m-w

  unsigned priv_index = (1<<3) /* user */ | (combined_access | isWrite);

  if (!priv_check[priv_index])
    nested_page_fault(ERROR_PROTECTION, guest_paddr, rw, is_page_walk);

  update_access_dirty(entry_addr, entry, entry_memtype, leaf, isWrite);

  Bit32u page_offset = PAGE_OFFSET(guest_paddr);
  return ppf | page_offset;
}

bx_phy_address BX_CPU_C::nested_walk(bx_phy_address guest_paddr, unsigned rw, bool is_page_walk)
{
  SVM_HOST_STATE *host_state = &BX_CPU_THIS_PTR vmcb.host_state;

  BX_DEBUG(("Nested walk for guest paddr 0x" FMT_PHY_ADDRX, guest_paddr));

  if (host_state->efer.get_LMA())
    return nested_walk_long_mode(guest_paddr, rw, is_page_walk);
  else if (host_state->cr4.get_PAE())
    return nested_walk_PAE(guest_paddr, rw, is_page_walk);
  else
    return nested_walk_legacy(guest_paddr, rw, is_page_walk);
}

#endif

#if BX_SUPPORT_VMX >= 2

/* EPT access type */
enum {
  BX_EPT_READ    = 0x01,
  BX_EPT_WRITE   = 0x02,
  BX_EPT_EXECUTE = 0x04,

  BX_EPT_MBE_SUPERVISOR_EXECUTE = BX_EPT_EXECUTE,
  BX_EPT_MBE_USER_EXECUTE = 0x400
};

/* EPT access mask */
enum {
  BX_EPT_ENTRY_NOT_PRESENT        = 0x00,
  BX_EPT_ENTRY_READ_ONLY          = 0x01,
  BX_EPT_ENTRY_WRITE_ONLY         = 0x02,
  BX_EPT_ENTRY_READ_WRITE         = 0x03,
  BX_EPT_ENTRY_EXECUTE_ONLY       = 0x04,
  BX_EPT_ENTRY_READ_EXECUTE       = 0x05,
  BX_EPT_ENTRY_WRITE_EXECUTE      = 0x06,
  BX_EPT_ENTRY_READ_WRITE_EXECUTE = 0x07
};

#define BX_VMX_EPT_ACCESS_DIRTY_ENABLED                 (BX_CPU_THIS_PTR vmcs.eptptr & 0x40)
#define BX_VMX_EPT_SUPERVISOR_SHADOW_STACK_CTRL_ENABLED (BX_CPU_THIS_PTR vmcs.eptptr & 0x80)

//                   Format of a EPT Entry
// -----------------------------------------------------------
// 00    | Read access
// 01    | Write access
// 02    | Execute Access
// 05-03 | EPT Memory type (for leaf entries, reserved otherwise)
// 06    | Ignore PAT memory type (for leaf entries, reserved otherwise)
// 07    | Page Size, must be 1 to indicate a Large Page
// 08    | Accessed bit (if supported, ignored otherwise)
// 09    | Dirty bit (for leaf entries, if supported, ignored otherwise)
// 11-10 | (ignored)
// PA-12 | Physical address
// 51-PA | Reserved (must be zero)
// 61-52 | (ignored)
// 60    | Supervisor Shadow Stack Page (CET)
// 61    | Sub Page Protected (SPP)
// 63    | Suppress #VE
// -----------------------------------------------------------

const Bit64u BX_SUPPRESS_EPT_VIOLATION_EXCEPTION = (BX_CONST64(1) << 63);
const Bit64u BX_SUB_PAGE_PROTECTED               = (BX_CONST64(1) << 61);
const Bit64u BX_SUPERVISOR_SHADOW_STACK_PAGE     = (BX_CONST64(1) << 60);

const Bit64u PAGING_EPT_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS;

// entries which were allowed to write only because of SPP cannot be cached in DTLB as writeable
bx_phy_address BX_CPU_C::translate_guest_physical(bx_phy_address guest_paddr, bx_address guest_laddr, bool guest_laddr_valid, bool is_page_walk, unsigned user_page, unsigned rw, bool supervisor_shadow_stack, bool *spp_page)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address entry_addr[4], ppf = LPFOf(vm->eptptr);
  Bit64u entry[4];
  int leaf;

#if BX_SUPPORT_MEMTYPE
  // The MTRRs have no effect on the memory type used for an access to an EPT paging structures.
  BxMemtype eptptr_memtype = BX_CPU_THIS_PTR cr0.get_CD() ? (BX_MEMTYPE_UC) : BxMemtype(vm->eptptr & 0x7);
#endif

  Bit64u offset_mask = BX_CONST64(0x0000ffffffffffff);
  Bit32u combined_access = 0x7, access_mask = 0;
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_MBE_CTRL))
    combined_access |= BX_EPT_MBE_USER_EXECUTE;

  BX_DEBUG(("EPT walk for guest paddr 0x" FMT_PHY_ADDRX, guest_paddr));

  // when EPT A/D enabled treat guest page table accesses as writes
  if (BX_VMX_EPT_ACCESS_DIRTY_ENABLED && is_page_walk && guest_laddr_valid)
    rw = BX_WRITE;

  if (rw == BX_EXECUTE) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_MBE_CTRL)) {
      access_mask |= user_page ? BX_EPT_MBE_USER_EXECUTE : BX_EPT_MBE_SUPERVISOR_EXECUTE;
    }
    else {
      access_mask |= BX_EPT_EXECUTE;
    }
  }
  if (rw & 1) access_mask |= BX_EPT_WRITE; // write or r-m-w
  if ((rw & 3) == BX_READ) access_mask |= BX_EPT_READ;  // handle correctly shadow stack reads

  Bit32u vmexit_reason = 0;

  for (leaf = BX_LEVEL_PML4;; --leaf) {
    entry_addr[leaf] = ppf + ((guest_paddr >> (9 + 9*leaf)) & 0xff8);
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, MEMTYPE(eptptr_memtype), BX_READ, (BX_EPT_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    offset_mask >>= 9;
    Bit64u curr_entry = entry[leaf];
    Bit32u curr_access_mask = curr_entry & 0x7;
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_MBE_CTRL)) {
      curr_access_mask |= (curr_entry & BX_EPT_MBE_USER_EXECUTE);
    }

    if (curr_access_mask == BX_EPT_ENTRY_NOT_PRESENT) {
      BX_DEBUG(("EPT %s: not present", bx_paging_level[leaf]));
      vmexit_reason = VMX_VMEXIT_EPT_VIOLATION;
      break;
    }

    if ((curr_access_mask & (BX_EPT_READ | BX_EPT_WRITE)) == BX_EPT_ENTRY_WRITE_ONLY) {
      BX_DEBUG(("EPT %s: EPT misconfiguration access_mask=%x", bx_paging_level[leaf], curr_access_mask));
      vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
      break;
    }

    extern bool isMemTypeValidMTRR(unsigned memtype);
    if (! isMemTypeValidMTRR((curr_entry >> 3) & 7)) {
      BX_DEBUG(("EPT %s: EPT misconfiguration memtype=%d",
        bx_paging_level[leaf], (unsigned)((curr_entry >> 3) & 7)));
      vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
      break;
    }

    if (curr_entry & PAGING_EPT_RESERVED_BITS) {
      BX_DEBUG(("EPT %s: reserved bit is set 0x" FMT_ADDRX64 "(reserved: " FMT_ADDRX64 ")", bx_paging_level[leaf], curr_entry, curr_entry & PAGING_EPT_RESERVED_BITS));
      vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
      break;
    }

    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);

    if (leaf == BX_LEVEL_PTE) break;

    if (curr_entry & 0x80) {
      if (leaf > (BX_LEVEL_PDE + !!is_cpu_extension_supported(BX_ISA_1G_PAGES))) {
        BX_DEBUG(("EPT %s: PS bit set !", bx_paging_level[leaf]));
        vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
        break;
      }

      ppf &= BX_CONST64(0x000fffffffffe000);
      if (ppf & offset_mask) {
         BX_DEBUG(("EPT %s: reserved bit is set: 0x" FMT_ADDRX64, bx_paging_level[leaf], curr_entry));
         vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
         break;
      }

      // Make up the physical page frame address
      ppf += (bx_phy_address)(guest_paddr & offset_mask);
      break;
    }

    // EPT non leaf entry, check for reserved bits
    if ((curr_entry >> 3) & 0xf) {
      BX_DEBUG(("EPT %s: EPT misconfiguration, reserved bits set for non-leaf entry", bx_paging_level[leaf]));
      vmexit_reason = VMX_VMEXIT_EPT_MISCONFIGURATION;
      break;
    }

    combined_access &= curr_access_mask;
  }

  // defer final combined_access calculation (with leaf entry) until CET is handled

  if (!vmexit_reason) {
#if BX_SUPPORT_CET
    if (BX_VMX_EPT_SUPERVISOR_SHADOW_STACK_CTRL_ENABLED && supervisor_shadow_stack) {
      // The EPT.R bit is set in all EPT paging-structure entry controlling the translation
      // The EPT.W bit is set in all EPT paging-structure entry controlling the translation except the leaf entry (allowed for shadow stack write access)
      // The SSS bit (bit 60) is 1 in the EPT paging-structure entry maps the page
      bool supervisor_shadow_stack_page = ((combined_access & BX_EPT_ENTRY_READ_WRITE) == BX_EPT_ENTRY_READ_WRITE) && 
                                             ((entry[leaf] & BX_EPT_READ) != 0) &&
                                             (((entry[leaf] & BX_EPT_WRITE) == 0) || !(access_mask & BX_EPT_WRITE)) &&
                                             ((entry[leaf] & BX_SUPERVISOR_SHADOW_STACK_PAGE) != 0);
      if (!supervisor_shadow_stack_page) {
        BX_ERROR(("VMEXIT: supervisor shadow stack access to non supervisor shadow stack page"));
        vmexit_reason = VMX_VMEXIT_EPT_VIOLATION;
      }
    }
    else
#endif
    {
      combined_access &= entry[leaf];

      if ((access_mask & combined_access) != access_mask) {
        vmexit_reason = VMX_VMEXIT_EPT_VIOLATION;
        if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_SUBPAGE_WR_PROTECT_CTRL) && (entry[leaf] & BX_SUB_PAGE_PROTECTED) != 0 && leaf == BX_LEVEL_PTE) {
          // if cumulative read-access bit is 0, the write access is not eligible for SPP
          if ((access_mask & BX_EPT_WRITE) != 0 && (combined_access & BX_EPT_ENTRY_READ_WRITE) == BX_EPT_ENTRY_READ_ONLY && guest_laddr_valid && ! is_page_walk) {
            if (spp_walk(guest_paddr, guest_laddr, BX_MEMTYPE_WB)) { // memory type indicated in IA32_VMX_BASIC MSR
              if (spp_page) *spp_page = true;
              vmexit_reason = 0;
            }
          }
        }
      }
    }
  }

  if (vmexit_reason) {
    BX_ERROR(("VMEXIT: EPT %s for guest paddr 0x" FMT_PHY_ADDRX " laddr 0x" FMT_ADDRX,
       (vmexit_reason == VMX_VMEXIT_EPT_VIOLATION) ? "violation" : "misconfig", guest_paddr, guest_laddr));

    Bit32u vmexit_qualification = 0;

    // no VMExit qualification for EPT Misconfiguration VMExit
    if (vmexit_reason == VMX_VMEXIT_EPT_VIOLATION) {
      combined_access &= entry[leaf];
      vmexit_qualification = access_mask | (combined_access << 3);
      if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_MBE_CTRL) && (rw == BX_EXECUTE)) {
        vmexit_qualification &= (0x3f); // reset all bit bits beyond [5:0]
        vmexit_qualification |= (1<<2); // bit2 indicate the operation was instruction fetch
        if (combined_access & BX_EPT_MBE_USER_EXECUTE)
          vmexit_qualification |= (1<<6);
      }
      if (guest_laddr_valid) {
        vmexit_qualification |= (1<<7);
        if (! is_page_walk) vmexit_qualification |= (1<<8);
      }
      if (BX_CPU_THIS_PTR nmi_unblocking_iret)
        vmexit_qualification |= (1 << 12);
#if BX_SUPPORT_CET
      if (rw & 4) // shadow stack access
        vmexit_qualification |= (1 << 13);

      if (BX_VMX_EPT_SUPERVISOR_SHADOW_STACK_CTRL_ENABLED && (entry[leaf] & BX_SUPERVISOR_SHADOW_STACK_PAGE) != 0)
        vmexit_qualification |= (1 << 14);
#endif
      if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_VIOLATION_EXCEPTION)) {
        if ((entry[leaf] & BX_SUPPRESS_EPT_VIOLATION_EXCEPTION) == 0)
          Virtualization_Exception(vmexit_qualification, guest_paddr, guest_laddr);
      }
    }

    VMwrite64(VMCS_64BIT_GUEST_PHYSICAL_ADDR, guest_paddr);
    VMwrite_natural(VMCS_GUEST_LINEAR_ADDR, guest_laddr);
    VMexit(vmexit_reason, vmexit_qualification);
  }

  if (BX_VMX_EPT_ACCESS_DIRTY_ENABLED) {
    // write access and Dirty-bit is not set in the leaf entry
    unsigned dirty_update = (rw & 1) && !(entry[leaf] & 0x200);
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_PML_ENABLE))
      vmx_page_modification_logging(guest_paddr, dirty_update);

    update_ept_access_dirty(entry_addr, entry, MEMTYPE(eptptr_memtype), leaf, rw & 1);
  }

  Bit32u page_offset = PAGE_OFFSET(guest_paddr);
  return ppf | page_offset;
}

// Access bit 8, Dirty bit 9
void BX_CPU_C::update_ept_access_dirty(bx_phy_address *entry_addr, Bit64u *entry, BxMemtype eptptr_memtype, unsigned leaf, unsigned write)
{
  // Update A bit if needed
  for (unsigned level=BX_LEVEL_PML4; level > leaf; level--) {
    if (!(entry[level] & 0x100)) {
      entry[level] |= 0x100;
      access_write_physical(entry_addr[level], 8, &entry[level]);
      BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[level], 8, MEMTYPE(eptptr_memtype), BX_WRITE, (BX_EPT_PTE_ACCESS + level), (Bit8u*)(&entry[level]));
    }
  }

  // Update A/D bits if needed
  if (!(entry[leaf] & 0x100) || (write && !(entry[leaf] & 0x200))) {
    entry[leaf] |= (0x100 | (write<<9)); // Update A and possibly D bits
    access_write_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, MEMTYPE(eptptr_memtype), BX_WRITE, (BX_EPT_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));
  }
}

const Bit64u PAGING_SPP_RESERVED_BITS = BX_PAGING_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0xFFF0000000000FFE);

const Bit32u VMX_SPP_NOT_PRESENT_QUALIFICATION = (1<<11);

bool BX_CPU_C::spp_walk(bx_phy_address guest_paddr, bx_address guest_laddr, BxMemtype memtype)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address entry_addr[4], ppf = LPFOf(vm->spptp);
  Bit64u entry[4];
  int leaf;

  BX_DEBUG(("SPP walk for guest paddr 0x" FMT_PHY_ADDRX, guest_paddr));

  Bit32u vmexit_reason = 0;
  Bit32u vmexit_qualification = 0;

  for (leaf = BX_LEVEL_PML4;; --leaf) {
    entry_addr[leaf] = ppf + ((guest_paddr >> (9 + 9*leaf)) & 0xff8);
    access_read_physical(entry_addr[leaf], 8, &entry[leaf]);
    BX_NOTIFY_PHY_MEMORY_ACCESS(entry_addr[leaf], 8, MEMTYPE(memtype), BX_READ, (BX_EPT_SPP_PTE_ACCESS + leaf), (Bit8u*)(&entry[leaf]));

    if (leaf == BX_LEVEL_PTE) break;

    Bit64u curr_entry = entry[leaf];

    if (!(curr_entry & 1)) {
      BX_DEBUG(("SPP %s: not present", bx_paging_level[leaf]));
      vmexit_reason = VMX_VMEXIT_SPP;
      vmexit_qualification = VMX_SPP_NOT_PRESENT_QUALIFICATION;
      break;
    }

    if (curr_entry & PAGING_SPP_RESERVED_BITS) {
      BX_DEBUG(("SPP %s: reserved bit is set 0x" FMT_ADDRX64, bx_paging_level[leaf], curr_entry));
      vmexit_reason = VMX_VMEXIT_SPP;
      break;
    }

    ppf = curr_entry & BX_CONST64(0x000ffffffffff000);
  }

  if (! vmexit_reason) {
    const Bit64u leaf_reserved_bits = BX_CONST64(0xAAAAAAAAAAAAAAAA);
    if (entry[BX_LEVEL_PTE] & leaf_reserved_bits) {
      BX_DEBUG(("SPP PTE: reserved (odd) bits are set"));
      vmexit_reason = VMX_VMEXIT_SPP;
    }
  }

  if (vmexit_reason) {
    BX_ERROR(("VMEXIT: SPP %s for guest paddr 0x" FMT_PHY_ADDRX " laddr 0x" FMT_ADDRX,
       (vmexit_qualification == VMX_SPP_NOT_PRESENT_QUALIFICATION) ? "not present" : "misconfig", guest_paddr, guest_laddr));

    if (BX_CPU_THIS_PTR nmi_unblocking_iret)
      vmexit_qualification |= (1 << 12);

    VMwrite64(VMCS_64BIT_GUEST_PHYSICAL_ADDR, guest_paddr);
    VMwrite_natural(VMCS_GUEST_LINEAR_ADDR, guest_laddr);
    VMexit(vmexit_reason, vmexit_qualification);
  }

  Bit32u spp_bit = 2 * ((guest_paddr & 0xFFF) >> 7);
  return (entry[BX_LEVEL_PTE] >> spp_bit) & 1;
}

#endif

#if BX_DEBUGGER

void dbg_print_paging_pte(int level, Bit64u entry)
{
  dbg_printf("%4s: 0x%08x%08x", bx_paging_level[level], GET32H(entry), GET32L(entry));

  if (entry & BX_CONST64(0x8000000000000000))
    dbg_printf(" XD");
  else
    dbg_printf("   ");

  if (level == BX_LEVEL_PTE) {
    dbg_printf("    %s %s %s",
      (entry & 0x0100) ? "G" : "g",
      (entry & 0x0080) ? "PAT" : "pat",
      (entry & 0x0040) ? "D" : "d");
  }
  else {
    if (entry & 0x80) {
      dbg_printf(" PS %s %s %s",
        (entry & 0x0100) ? "G" : "g",
        (entry & 0x1000) ? "PAT" : "pat",
        (entry & 0x0040) ? "D" : "d");
    }
    else {
      dbg_printf(" ps        ");
    }
  }

  dbg_printf(" %s %s %s %s %s %s\n",
    (entry & 0x20) ? "A" : "a",
    (entry & 0x10) ? "PCD" : "pcd",
    (entry & 0x08) ? "PWT" : "pwt",
    (entry & 0x04) ? "U" : "S",
    (entry & 0x02) ? "W" : "R",
    (entry & 0x01) ? "P" : "p");
}

#if BX_SUPPORT_VMX >= 2
void dbg_print_ept_paging_pte(int level, Bit64u entry, bool mbe)
{
  dbg_printf("EPT %4s: 0x%08x%08x", bx_paging_level[level], GET32H(entry), GET32L(entry));

  if (level != BX_LEVEL_PTE && (entry & 0x80))
    dbg_printf(" PS");
  else
    dbg_printf("   ");

  if (mbe)
    dbg_printf(" %s", (entry & 0x400) ? "XU" : "xu");

  dbg_printf(" %s %s %s",
    (entry & 0x04) ? "X" : "x",
    (entry & 0x02) ? "W" : "w",
    (entry & 0x01) ? "R" : "r");

  if (level == BX_LEVEL_PTE || (entry & 0x80)) {
    dbg_printf(" %s %s\n",
      (entry & 0x40) ? "IGNORE_PAT" : "ignore_pat",
      get_memtype_name(BxMemtype((entry >> 3) & 0x7)));
  }
  else {
    dbg_printf("\n");
  }
}
#endif

#endif // BX_DEBUGGER

#if BX_SUPPORT_VMX >= 2
bool BX_CPU_C::dbg_translate_guest_physical(bx_phy_address guest_paddr, bx_phy_address *phy, bool verbose)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address pt_address = LPFOf(vm->eptptr);
  Bit64u offset_mask = BX_CONST64(0x0000ffffffffffff);

  for (int level = 3; level >= 0; --level) {
    Bit64u pte;
    pt_address += ((guest_paddr >> (9 + 9*level)) & 0xff8);
    offset_mask >>= 9;
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pt_address, 8, &pte);
#if BX_DEBUGGER
    if (verbose)
      dbg_print_ept_paging_pte(level, pte, SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_MBE_CTRL));
#endif
    switch(pte & 7) {
    case BX_EPT_ENTRY_NOT_PRESENT:
    case BX_EPT_ENTRY_WRITE_ONLY:
    case BX_EPT_ENTRY_WRITE_EXECUTE:
      return 0;
    }
    if (pte & BX_PAGING_PHY_ADDRESS_RESERVED_BITS)
      return 0;

    pt_address = bx_phy_address(pte & BX_CONST64(0x000ffffffffff000));

    if (level == BX_LEVEL_PTE) break;

    if (pte & 0x80) {
       if (level > (BX_LEVEL_PDE + !!is_cpu_extension_supported(BX_ISA_1G_PAGES)))
         return 0;

        pt_address &= BX_CONST64(0x000fffffffffe000);
        if (pt_address & offset_mask) return 0;
        break;
      }
  }

  *phy = pt_address + (bx_phy_address)(guest_paddr & offset_mask);
  return 1;
}
#endif

bool BX_CPU_C::dbg_xlate_linear2phy(bx_address laddr, bx_phy_address *phy, bx_address *lpf_mask, bool verbose)
{
  bx_phy_address paddress;
  bx_address offset_mask = 0xfff;

#if BX_SUPPORT_X86_64
  if (! long_mode()) laddr &= 0xffffffff;
#endif

  if (! BX_CPU_THIS_PTR cr0.get_PG()) {
    paddress = (bx_phy_address) laddr;
  }
  else {
    bx_phy_address pt_address = BX_CPU_THIS_PTR cr3 & BX_CR3_PAGING_MASK;

#if BX_CPU_LEVEL >= 6
    if (BX_CPU_THIS_PTR cr4.get_PAE()) {
      offset_mask = BX_CONST64(0x0000ffffffffffff);

      int level = 3;
      if (! long_mode()) {
        pt_address = BX_CPU_THIS_PTR PDPTR_CACHE.entry[(laddr >> 30) & 3];
        if (! (pt_address & 0x1)) {
           offset_mask = 0x3fffffff;
           goto page_fault;
	}
        offset_mask >>= 18;
        pt_address &= BX_CONST64(0x000ffffffffff000);
        level = 1;
      }

      for (; level >= 0; --level) {
        Bit64u pte;
        pt_address += ((laddr >> (9 + 9*level)) & 0xff8);
        offset_mask >>= 9;
#if BX_SUPPORT_VMX >= 2
        if (BX_CPU_THIS_PTR in_vmx_guest) {
          if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE)) {
            if (! dbg_translate_guest_physical(pt_address, &pt_address, verbose))
              goto page_fault;
          }
        }
#endif
        BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pt_address, 8, &pte);
#if BX_DEBUGGER
        if (verbose)
          dbg_print_paging_pte(level, pte);
#endif
        if(!(pte & 1))
          goto page_fault;
        if (pte & BX_PAGING_PHY_ADDRESS_RESERVED_BITS)
          goto page_fault;
        pt_address = bx_phy_address(pte & BX_CONST64(0x000ffffffffff000));
        if (level == BX_LEVEL_PTE) break;
        if (pte & 0x80) {
          // large page
          pt_address &= BX_CONST64(0x000fffffffffe000);
          if (pt_address & offset_mask)
            goto page_fault;
          if (is_cpu_extension_supported(BX_ISA_1G_PAGES) && level == BX_LEVEL_PDPTE) break;
          if (level == BX_LEVEL_PDE) break;
          goto page_fault;
        }
      }
      paddress = pt_address + (bx_phy_address)(laddr & offset_mask);
    }
    else   // not PAE
#endif
    {
      offset_mask = 0xfff;
      for (int level = 1; level >= 0; --level) {
        Bit32u pte;
        pt_address += ((laddr >> (10 + 10*level)) & 0xffc);
#if BX_SUPPORT_VMX >= 2
        if (BX_CPU_THIS_PTR in_vmx_guest) {
          if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE)) {
            if (! dbg_translate_guest_physical(pt_address, &pt_address, verbose))
              goto page_fault;
          }
        }
#endif
        BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pt_address, 4, &pte);
#if BX_DEBUGGER
        if (verbose)
          dbg_print_paging_pte(level, pte);
#endif
        if (!(pte & 1)) 
          goto page_fault;
        pt_address = pte & 0xfffff000;
#if BX_CPU_LEVEL >= 6
        if (level == BX_LEVEL_PDE && (pte & 0x80) != 0 && BX_CPU_THIS_PTR cr4.get_PSE()) {
          offset_mask = 0x3fffff;
          pt_address = pte & 0xffc00000;
#if BX_PHY_ADDRESS_WIDTH > 32
          pt_address += ((bx_phy_address)(pte & 0x003fe000)) << 19;
#endif
          break;
        }
#endif
      }
      paddress = pt_address + (bx_phy_address)(laddr & offset_mask);
    }
  }
#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_EPT_ENABLE)) {
      if (! dbg_translate_guest_physical(paddress, &paddress, verbose))
        goto page_fault;
    }
  }
#endif

  if (lpf_mask)
    *lpf_mask = offset_mask;
  *phy = A20ADDR(paddress);
  return 1;

page_fault:
  if (lpf_mask)
    *lpf_mask = offset_mask;
  *phy = 0;
  return 0;
}

int BX_CPU_C::access_write_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned xlate_rw, Bit32u ac_mask, void *data)
{
#if BX_SUPPORT_CET
  BX_ASSERT(xlate_rw == BX_WRITE || xlate_rw == BX_SHADOW_STACK_WRITE);
#else
  BX_ASSERT(xlate_rw == BX_WRITE);
#endif

  Bit32u pageOffset = PAGE_OFFSET(laddr);

  bool user = (curr_pl == 3);

  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);

#if BX_SUPPORT_X86_64
  if (! IsCanonical(laddr)) {
    BX_ERROR(("access_write_linear(): canonical failure"));
    return -1;
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  if (BX_CPU_THIS_PTR alignment_check() && user) {
    if (pageOffset & ac_mask) {
      BX_ERROR(("access_write_linear(): #AC misaligned access"));
      exception(BX_AC_EXCEPTION, 0);
    }
  }
#endif

  /* check for reference across multiple pages */
  if ((pageOffset + len) <= 4096) {
    // Access within single page.
    BX_CPU_THIS_PTR address_xlation.paddress1 = translate_linear(tlbEntry, laddr, user, xlate_rw);
    BX_CPU_THIS_PTR address_xlation.pages     = 1;
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR address_xlation.memtype1  = tlbEntry->get_memtype();
#endif

    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
                          len, tlbEntry->get_memtype(), xlate_rw, (Bit8u*) data);

    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1, len, data);

#if BX_X86_DEBUGGER
    hwbreakpoint_match(laddr, len, xlate_rw);
#endif
  }
  else {
    // access across 2 pages
    BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
    BX_CPU_THIS_PTR address_xlation.len2 = len - BX_CPU_THIS_PTR address_xlation.len1;
    BX_CPU_THIS_PTR address_xlation.pages = 2;
    bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
#if BX_SUPPORT_X86_64
    if (! long64_mode()) laddr2 &= 0xffffffff; /* handle linear address wrap in legacy mode */
    else {
      if (! IsCanonical(laddr2)) {
        BX_ERROR(("access_write_linear(): canonical failure for second half of page split access"));
        return -1;
      }
    }
#endif

    bx_TLB_entry *tlbEntry2 = BX_DTLB_ENTRY_OF(laddr2, 0);

    BX_CPU_THIS_PTR address_xlation.paddress1 = translate_linear(tlbEntry, laddr, user, xlate_rw);
    BX_CPU_THIS_PTR address_xlation.paddress2 = translate_linear(tlbEntry2, laddr2, user, xlate_rw);
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
    BX_CPU_THIS_PTR address_xlation.memtype2 = tlbEntry2->get_memtype();
#endif

#ifdef BX_LITTLE_ENDIAN
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, tlbEntry->get_memtype(),
        xlate_rw, (Bit8u*) data);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, tlbEntry2->get_memtype(),
        xlate_rw, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, tlbEntry->get_memtype(),
        xlate_rw, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, tlbEntry2->get_memtype(),
        xlate_rw, (Bit8u*) data);
    access_write_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, data);
#endif

#if BX_X86_DEBUGGER
    hwbreakpoint_match(laddr,  BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
    hwbreakpoint_match(laddr2, BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
#endif
  }

  return 0;
}

int BX_CPU_C::access_read_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned xlate_rw, Bit32u ac_mask, void *data)
{
#if BX_SUPPORT_CET
  BX_ASSERT(xlate_rw == BX_READ || xlate_rw == BX_RW || xlate_rw == BX_SHADOW_STACK_READ || xlate_rw == BX_SHADOW_STACK_RW);
#else
  BX_ASSERT(xlate_rw == BX_READ || xlate_rw == BX_RW);
#endif

  Bit32u pageOffset = PAGE_OFFSET(laddr);

  bool user = (curr_pl == 3);

#if BX_SUPPORT_X86_64
  if (! IsCanonical(laddr)) {
    BX_ERROR(("access_read_linear(): canonical failure"));
    return -1;
  }
#endif

#if BX_CPU_LEVEL >= 4 && BX_SUPPORT_ALIGNMENT_CHECK
  if (BX_CPU_THIS_PTR alignment_check() && user) {
    if (pageOffset & ac_mask) {
      BX_ERROR(("access_read_linear(): #AC misaligned access"));
      exception(BX_AC_EXCEPTION, 0);
    }
  }
#endif

  bx_TLB_entry *tlbEntry = BX_DTLB_ENTRY_OF(laddr, 0);

  /* check for reference across multiple pages */
  if ((pageOffset + len) <= 4096) {
    // Access within single page.
    BX_CPU_THIS_PTR address_xlation.paddress1 = translate_linear(tlbEntry, laddr, user, xlate_rw);
    BX_CPU_THIS_PTR address_xlation.pages     = 1;
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR address_xlation.memtype1  = tlbEntry->get_memtype();
#endif
    access_read_physical(BX_CPU_THIS_PTR address_xlation.paddress1, len, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1, len, tlbEntry->get_memtype(), xlate_rw, (Bit8u*) data);

#if BX_X86_DEBUGGER
    hwbreakpoint_match(laddr, len, xlate_rw);
#endif
  }
  else {
    // access across 2 pages
    BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
    BX_CPU_THIS_PTR address_xlation.len2 = len - BX_CPU_THIS_PTR address_xlation.len1;
    BX_CPU_THIS_PTR address_xlation.pages = 2;
    bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
#if BX_SUPPORT_X86_64
    if (! long64_mode()) laddr2 &= 0xffffffff; /* handle linear address wrap in legacy mode */
    else {
      if (! IsCanonical(laddr2)) {
        BX_ERROR(("access_read_linear(): canonical failure for second half of page split access"));
        return -1;
      }
    }
#endif

    bx_TLB_entry *tlbEntry2 = BX_DTLB_ENTRY_OF(laddr2, 0);

    BX_CPU_THIS_PTR address_xlation.paddress1 = translate_linear(tlbEntry, laddr, user, xlate_rw);
    BX_CPU_THIS_PTR address_xlation.paddress2 = translate_linear(tlbEntry2, laddr2, user, xlate_rw);
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR address_xlation.memtype1 = tlbEntry->get_memtype();
    BX_CPU_THIS_PTR address_xlation.memtype2 = tlbEntry2->get_memtype();
#endif

#ifdef BX_LITTLE_ENDIAN
    access_read_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, tlbEntry->get_memtype(),
        xlate_rw, (Bit8u*) data);
    access_read_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2,
        ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, tlbEntry2->get_memtype(),
        xlate_rw, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
    access_read_physical(BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1,
        ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
        BX_CPU_THIS_PTR address_xlation.len1, tlbEntry->get_memtype(),
        xlate_rw, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
    access_read_physical(BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, data);
    BX_NOTIFY_LIN_MEMORY_ACCESS(laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
        BX_CPU_THIS_PTR address_xlation.len2, tlbEntry2->get_memtype(),
        xlate_rw, (Bit8u*) data);
#endif

#if BX_X86_DEBUGGER
    hwbreakpoint_match(laddr,  BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
    hwbreakpoint_match(laddr2, BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
#endif
  }

  return 0;
}

void BX_CPU_C::access_write_physical(bx_phy_address paddr, unsigned len, void *data)
{
#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64
  if (is_virtual_apic_page(paddr)) {
    VMX_Virtual_Apic_Write(paddr, len, data);
    return;
  }
#endif

#if BX_SUPPORT_APIC
  if (BX_CPU_THIS_PTR lapic.is_selected(paddr)) {
    BX_CPU_THIS_PTR lapic.write(paddr, data, len);
    return;
  }
#endif

  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, paddr, len, data);
}

void BX_CPU_C::access_read_physical(bx_phy_address paddr, unsigned len, void *data)
{
#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64
  if (is_virtual_apic_page(paddr)) {
    paddr = VMX_Virtual_Apic_Read(paddr, len, data);
  }
#endif

#if BX_SUPPORT_APIC
  if (BX_CPU_THIS_PTR lapic.is_selected(paddr)) {
    BX_CPU_THIS_PTR lapic.read(paddr, data, len);
    return;
  }
#endif

  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, paddr, len, data);
}

bx_hostpageaddr_t BX_CPU_C::getHostMemAddr(bx_phy_address paddr, unsigned rw)
{
#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64
  if (is_virtual_apic_page(paddr))
    return 0; // Do not allow direct access to virtual apic page
#endif

#if BX_SUPPORT_APIC
  if (BX_CPU_THIS_PTR lapic.is_selected(paddr))
    return 0; // Vetoed!  APIC address space
#endif

  return (bx_hostpageaddr_t) BX_MEM(0)->getHostMemAddr(BX_CPU_THIS, paddr, rw);
}

#if BX_LARGE_RAMFILE
bool BX_CPU_C::check_addr_in_tlb_buffers(const Bit8u *addr, const Bit8u *end)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR vmcshostptr) {
    if ((BX_CPU_THIS_PTR vmcshostptr >= (const bx_hostpageaddr_t)addr) &&
        (BX_CPU_THIS_PTR vmcshostptr  < (const bx_hostpageaddr_t)end)) return true;
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR vmcbhostptr) {
    if ((BX_CPU_THIS_PTR vmcbhostptr >= (const bx_hostpageaddr_t)addr) &&
        (BX_CPU_THIS_PTR vmcbhostptr  < (const bx_hostpageaddr_t)end)) return true;
  }
#endif

  for (unsigned tlb_entry_num=0; tlb_entry_num < BX_DTLB_SIZE; tlb_entry_num++) {
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR DTLB.entry[tlb_entry_num];
    if (tlbEntry->valid()) {
      if ((tlbEntry->hostPageAddr >= (const bx_hostpageaddr_t)addr) &&
          (tlbEntry->hostPageAddr  < (const bx_hostpageaddr_t)end))
        return true;
    }
  }

  for (unsigned tlb_entry_num=0; tlb_entry_num < BX_ITLB_SIZE; tlb_entry_num++) {
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR ITLB.entry[tlb_entry_num];
    if (tlbEntry->valid()) {
      if ((tlbEntry->hostPageAddr >= (const bx_hostpageaddr_t)addr) &&
          (tlbEntry->hostPageAddr  < (const bx_hostpageaddr_t)end))
        return true;
    }
  }

  return false;
}
#endif
