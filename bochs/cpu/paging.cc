/////////////////////////////////////////////////////////////////////////
// $Id: paging.cc,v 1.156 2008-08-23 13:55:37 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

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


// Page Directory/Table Entry format when P=0:
// ===========================================
//
//   31.. 1: available
//        0: P=0

// Page Directory Entry format when P=1 (4-Kbyte Page Table):
// ==========================================================
//
//   31..12: page table base address
//   11.. 9: available
//        8: G (Pentium Pro+), 0=reserved otherwise
//        7: PS (Pentium+), 0=reserved otherwise
//        6: 0=reserved
//        5: A   (386+)
//        4: PCD (486+), 0=reserved otherwise
//        3: PWT (486+), 0=reserved otherwise
//        2: U/S (386+)
//        1: R/W (386+)
//        0: P=1 (386+)

// Page Table Entry format when P=1 (4-Kbyte Page):
// ================================================
//
//   63..63: NX                  |
//   62..52: available           | Long mode
//   51..32: page base address   |
//   31..12: page base address
//   11.. 9: available
//        8: G (Pentium Pro+), 0=reserved otherwise
//        7: PAT
//        6: D   (386+)
//        5: A   (386+)
//        4: PCD (486+), 0=reserved otherwise
//        3: PWT (486+), 0=reserved otherwise
//        2: U/S (386+)
//        1: R/W (386+)
//        0: P=1 (386+)

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

#if BX_SUPPORT_X86_64
  #define BX_INVALID_TLB_ENTRY BX_CONST64(0xffffffffffffffff)
#else
  #define BX_INVALID_TLB_ENTRY 0xffffffff
#endif

// bit [11] of the TLB lpf used for TLB_HostPtr valid indication
#define TLB_LPFOf(laddr) AlignedAccessLPFOf(laddr, 0x7ff)

#if BX_CPU_LEVEL >= 4
#  define BX_PRIV_CHECK_SIZE 32
#else
#  define BX_PRIV_CHECK_SIZE 16
#endif

static unsigned priv_check[BX_PRIV_CHECK_SIZE];

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
//       The test is:
//         OK = (accessBits & ((W<<1) | U)) <> 0  [W:1=Write, 0=Read, U:1=CPL3,0=CPL0-2]
//
//       Thus for reads, it is:
//         OK =       (          U )
//       And for writes:
//         OK = 0x2 | (          U )
//
//       Note, that the TLB should have TLB_HostPtr bit set when direct
//       access through host pointer is NOT allowed for the page. A memory
//       operation asking for a direct access through host pointer will
//       set TLB_HostPtr bit in its lpf field and thus get TLB miss result 
//       when the direct access is not allowed.
//

#define TLB_SysOnly     (0x1)
#define TLB_ReadOnly    (0x2)

#define TLB_HostPtr     (0x800) /* set this bit when direct access is NOT allowed */

#define TLB_GlobalPage  (0x80000000)

// === TLB Instrumentation section ==============================

// Note: this is an approximation of what Peter Tattam had.

#define InstrumentTLB 0

#if InstrumentTLB
static unsigned tlbLookups=0;
static unsigned tlbMisses=0;
static unsigned tlbGlobalFlushes=0;
static unsigned tlbNonGlobalFlushes=0;

#define InstrTLB_StatsMask 0xfffff

#define InstrTLB_Stats() {\
  if ((tlbLookups & InstrTLB_StatsMask) == 0) { \
    BX_INFO(("TLB lookup:%8d miss:%8d %6.2f%% flush:%8d %6.2f%%", \
          tlbLookups, \
          tlbMisses, \
          tlbMisses * 100.0 / tlbLookups, \
          (tlbGlobalFlushes+tlbNonGlobalFlushes), \
          (tlbGlobalFlushes+tlbNonGlobalFlushes) * 100.0 / tlbLookups \
          )); \
    tlbLookups = tlbMisses = tlbGlobalFlushes = tlbNonGlobalFlushes = 0; \
    } \
  }
#define InstrTLB_Increment(v) (v)++

#else
#define InstrTLB_Stats()
#define InstrTLB_Increment(v)
#endif

#define BX_PHY_ADDRESS_MASK ((((Bit64u)(1)) << BX_PHY_ADDRESS_WIDTH) - 1)

#define BX_PHY_ADDRESS_RESERVED_BITS \
      (~BX_PHY_ADDRESS_MASK & BX_CONST64(0xfffffffffffff))

// ==============================================================

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::pagingCR0Changed(Bit32u oldCR0, Bit32u newCR0)
{
  // Modification of PG,PE flushes TLB cache according to docs.
  // Additionally, the TLB strategy is based on the current value of
  // WP, so if that changes we must also flush the TLB.
  if ((oldCR0 & 0x80010001) != (newCR0 & 0x80010001))
    TLB_flush(); // Flush Global entries also.
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::pagingCR4Changed(Bit32u oldCR4, Bit32u newCR4)
{
  // Modification of PGE,PAE,PSE flushes TLB cache according to docs.
  if ((oldCR4 & 0x000000b0) != (newCR4 & 0x000000b0))
    TLB_flush(); // Flush Global entries also.

#if BX_SUPPORT_PAE
  if ((oldCR4 & 0x00000020) != (newCR4 & 0x00000020)) {
    if (BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode())
      BX_CPU_THIS_PTR cr3_masked = BX_CPU_THIS_PTR cr3 & 0xffffffe0;
    else
      BX_CPU_THIS_PTR cr3_masked = BX_CPU_THIS_PTR cr3 & BX_CONST64(0x000ffffffffff000);
  }
#endif
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::SetCR3(bx_address val)
{
  // flush TLB even if value does not change
#if BX_SUPPORT_GLOBAL_PAGES
  if (BX_CPU_THIS_PTR cr4.get_PGE())
    TLB_flushNonGlobal(); // Don't flush Global entries.
  else
#endif
    TLB_flush();          // Flush Global entries also.

#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
    // when not in long mode cr3 could be only 32-bit
    BX_CPU_THIS_PTR cr3_masked = val & 0xffffffe0;
  }
  else
#endif
  {
#if BX_PHY_ADDRESS_WIDTH == 32
    if (val & BX_CONST64(0x000fffff00000000)) {
      BX_PANIC(("SetCR3() 0x%08x%08x: Only 32 bit physical address space is emulated !", GET32H(val), GET32L(val)));
    }
#endif
    if (val & BX_PHY_ADDRESS_RESERVED_BITS) {
      BX_ERROR(("SetCR3(): Attempt to write to reserved bits of CR3"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    BX_CPU_THIS_PTR cr3_masked = val & BX_CONST64(0x000ffffffffff000);
  }

  BX_CPU_THIS_PTR cr3 = val;
}

// Called to initialize the TLB upon startup.
// Unconditional initialization of all TLB entries.
void BX_CPU_C::TLB_init(void)
{
  unsigned n, wp, us_combined, rw_combined, us_current, rw_current;

  //
  // Setup privilege check matrix.
  //
  for (n=0; n<BX_PRIV_CHECK_SIZE; n++) {
    wp          = (n & 0x10) >> 4;
    us_current  = (n & 0x08) >> 3;
    us_combined = (n & 0x04) >> 2;
    rw_combined = (n & 0x02) >> 1;
    rw_current  = (n & 0x01) >> 0;
    if (wp) { // when write protect on
      if (us_current > us_combined) // user access, supervisor page
        priv_check[n] = 0;
      else if (rw_current > rw_combined) // RW access, RO page
        priv_check[n] = 0;
      else
        priv_check[n] = 1;
    }
    else { // when write protect off
      if (us_current == 0) // Supervisor mode access, anything goes
        priv_check[n] = 1;
      else {
        // user mode access
        if (us_combined == 0) // user access, supervisor Page
          priv_check[n] = 0;
        else if (rw_current > rw_combined) // RW access, RO page
          priv_check[n] = 0;
        else
          priv_check[n] = 1;
      }
    }
  }

  TLB_flush();
}

void BX_CPU_C::TLB_flush(void)
{
#if InstrumentTLB
  InstrTLB_Increment(tlbGlobalFlushes);
#endif

  invalidate_prefetch_q();

  for (unsigned n=0; n<BX_TLB_SIZE; n++) {
    BX_CPU_THIS_PTR TLB.entry[n].lpf = BX_INVALID_TLB_ENTRY;
  }

#if BX_SUPPORT_PAE
  BX_CPU_THIS_PTR PDPE_CACHE.valid = 0;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB might change translation for monitored page
  // and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif
}

#if BX_SUPPORT_GLOBAL_PAGES

void BX_CPU_C::TLB_flushNonGlobal(void)
{
#if InstrumentTLB
  InstrTLB_Increment(tlbNonGlobalFlushes);
#endif

  invalidate_prefetch_q();

  for (unsigned n=0; n<BX_TLB_SIZE; n++) {
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[n];
    if (!(tlbEntry->accessBits & TLB_GlobalPage))
      tlbEntry->lpf = BX_INVALID_TLB_ENTRY;
  }

#if BX_SUPPORT_PAE
  BX_CPU_THIS_PTR PDPE_CACHE.valid = 0;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB might change translation for monitored page
  // and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif
}

#endif

void BX_CPU_C::TLB_invlpg(bx_address laddr)
{
  invalidate_prefetch_q();

  BX_DEBUG(("TLB_invlpg(0x"FMT_ADDRX"): invalidate TLB entry", laddr));

  unsigned TLB_index = BX_TLB_INDEX_OF(laddr, 0);
  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[TLB_index];
  if (TLB_LPFOf(tlbEntry->lpf) == lpf) {
    tlbEntry->lpf = BX_INVALID_TLB_ENTRY;
  }

#if BX_SUPPORT_PAE
  BX_CPU_THIS_PTR PDPE_CACHE.valid = 0;
#endif

#if BX_SUPPORT_MONITOR_MWAIT
  // invalidating of the TLB entry might change translation for monitored
  // page and cause subsequent MWAIT instruction to wait forever
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVLPG(bxInstruction_c* i)
{
#if BX_CPU_LEVEL >= 4
  if (!real_mode() && CPL!=0) {
    BX_ERROR(("INVLPG: priveledge check failed, generate #GP(0)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  bx_address laddr = get_laddr(i->seg(), eaddr);
  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVLPG, laddr);
  TLB_invlpg(laddr);
#else
  BX_INFO(("INVLPG: required i486, use --enable-cpu=4 option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

// error checking order - page not present, reserved bits, protection
#define ERROR_NOT_PRESENT       0x00
#define ERROR_PROTECTION        0x01
#define ERROR_RESERVED          0x08
#define ERROR_CODE_ACCESS       0x10

void BX_CPU_C::page_fault(unsigned fault, bx_address laddr, unsigned user, unsigned rw, unsigned access_type)
{
  unsigned error_code = fault;

  error_code |= (user << 2) | (rw << 1);
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_NXE() && (access_type == CODE_ACCESS))
    error_code |= ERROR_CODE_ACCESS; // I/D = 1
#endif
  BX_CPU_THIS_PTR cr2 = laddr;

#if BX_SUPPORT_X86_64
  BX_DEBUG(("page fault for address %08x%08x @ %08x%08x",
             GET32H(laddr), GET32L(laddr), GET32H(RIP), GET32L(RIP)));
#else
  BX_DEBUG(("page fault for address %08x @ %08x", laddr, EIP));
#endif

  exception(BX_PF_EXCEPTION, error_code, 0);
}

/* PAE PML4: bits [51 .. physical address width], [7] - support 1G paging */
#define PAGING_PAE_PML4_RESERVED_BITS \
    (BX_PHY_ADDRESS_RESERVED_BITS/* | BX_CONST64(0x80)*/)

/* PAE PDPE: bits [51 .. physical address width], [7] - support 1G paging */
#define PAGING_PAE_PDPE_RESERVED_BITS \
    (BX_PHY_ADDRESS_RESERVED_BITS/* | BX_CONST64(0x80)*/)

/* PAE PDE: bits [51 .. physical address width] */
#define PAGING_PAE_PDE_RESERVED_BITS (BX_PHY_ADDRESS_RESERVED_BITS)

/* PAE PDE4M: bits [51 .. physical address width], [20:13] */
#define PAGING_PAE_PDE4M_RESERVED_BITS \
    (PAGING_PAE_PDE_RESERVED_BITS | BX_CONST64(0x001FE000))

/* PAE PTE: bits [51 .. physical address width] */
#define PAGING_PAE_PTE_RESERVED_BITS (BX_PHY_ADDRESS_RESERVED_BITS)

#define PAGE_DIRECTORY_NX_BIT (BX_CONST64(0x8000000000000000))

#if BX_SUPPORT_PAE

// Translate a linear address to a physical address in PAE paging mode
bx_phy_address BX_CPU_C::translate_linear_PAE(bx_address laddr, Bit32u &combined_access, unsigned curr_pl, unsigned rw, unsigned access_type)
{
  bx_phy_address pdpe_addr, ppf;
  Bit64u pdpe, pde, pte;
#if BX_SUPPORT_X86_64
  Bit64u pml4, pml4_addr = 0;
#endif
  unsigned priv_index, nx_fault = 0;
  bx_bool isWrite = (rw >= BX_WRITE); // write or r-m-w
  unsigned pl = (curr_pl == 3);

  combined_access = 0;

#if BX_SUPPORT_X86_64
  if (long_mode()) {
    // Get PML4 entry
    pml4_addr = (bx_phy_address)(BX_CPU_THIS_PTR cr3_masked |
               ((laddr & BX_CONST64(0x0000ff8000000000)) >> 36));
    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pml4_addr, 8, &pml4);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pml4_addr, 8, BX_READ, (Bit8u*)(&pml4));

    if (!(pml4 & 0x1)) {
      BX_DEBUG(("PML4: entry not present"));
      page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
    }
#if BX_PHY_ADDRESS_WIDTH == 32
    if (pml4 & BX_CONST64(0x000fffff00000000)) {
      BX_PANIC(("PML4 0x%08x%08x: Only 32 bit physical address space is emulated !", GET32H(pml4), GET32L(pml4)));
    }
#endif
    if (pml4 & PAGING_PAE_PML4_RESERVED_BITS) {
      BX_DEBUG(("PML4: reserved bit is set PML4=%08x:%08x", GET32H(pml4), GET32L(pml4)));
      page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
    }
    if (pml4 & PAGE_DIRECTORY_NX_BIT) {
      if (! BX_CPU_THIS_PTR efer.get_NXE()) {
        BX_DEBUG(("PML4: NX bit set when EFER.NXE is disabled"));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
      }
      if (access_type == CODE_ACCESS) {
        BX_DEBUG(("PML4: non-executable page fault occured"));
        nx_fault = 1;
      }
    }

    pdpe_addr = (bx_phy_address)((pml4 & BX_CONST64(0x000ffffffffff000)) |
                               ((laddr & BX_CONST64(0x0000007fc0000000)) >> 27));

    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pdpe_addr, 8, &pdpe);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pdpe_addr, 8, BX_READ, (Bit8u*)(&pdpe));
  }
  else
#endif
  {
    pdpe_addr = (bx_phy_address) (BX_CPU_THIS_PTR cr3_masked | ((laddr & 0xc0000000) >> 27));

    if (! BX_CPU_THIS_PTR PDPE_CACHE.valid) {
      for (int n=0; n<4; n++) {
        // read PDPE cache entry
        bx_phy_address entry_pdpe_addr = (bx_phy_address) (BX_CPU_THIS_PTR cr3_masked | (n << 3));
        BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, entry_pdpe_addr, 8, &(BX_CPU_THIS_PTR PDPE_CACHE.entry[n]));
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, entry_pdpe_addr, 8, 
             BX_READ, (Bit8u*)(&BX_CPU_THIS_PTR PDPE_CACHE.entry[n]));
      }
      BX_CPU_THIS_PTR PDPE_CACHE.valid = 1;
    }

    pdpe = BX_CPU_THIS_PTR PDPE_CACHE.entry[(laddr >> 30) & 3];
  }

  if (!(pdpe & 0x1)) {
    BX_DEBUG(("PAE PDPE: entry not present"));
    page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
  }
#if BX_PHY_ADDRESS_WIDTH == 32
  if (pdpe & BX_CONST64(0x000fffff00000000)) {
    BX_PANIC(("PAE PDPE 0x%08x%08x: Only 32 bit physical address space is emulated !", GET32H(pdpe), GET32L(pdpe)));
  }
#endif
  if (pdpe & PAGING_PAE_PDPE_RESERVED_BITS) {
    BX_DEBUG(("PAE PDPE: reserved bit is set: PDPE=%08x:%08x", GET32H(pdpe), GET32L(pdpe)));
    page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
  }
#if BX_SUPPORT_X86_64
  if (pdpe & PAGE_DIRECTORY_NX_BIT) {
    if (! BX_CPU_THIS_PTR efer.get_NXE()) {
      BX_DEBUG(("PDPE: NX bit set when EFER.NXE is disabled"));
      page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
    }
    if (access_type == CODE_ACCESS) {
      BX_DEBUG(("PDPE: non-executable page fault occured"));
      nx_fault = 1;
    }
  }
#endif
  bx_phy_address pde_addr = (bx_phy_address)((pdpe & BX_CONST64(0x000ffffffffff000))
                         | ((laddr & 0x3fe00000) >> 18));

  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pde_addr, 8, &pde);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 8, BX_READ, (Bit8u*)(&pde));

  if (!(pde & 0x1)) {
    BX_DEBUG(("PAE PDE: entry not present"));
    page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
  }
#if BX_PHY_ADDRESS_WIDTH == 32
  if (pde & BX_CONST64(0x000fffff00000000)) {
    BX_PANIC(("PAE PDE 0x%08x%08x: Only 32 bit physical address space is emulated !", GET32H(pde), GET32L(pde)));
  }
#endif
  if (pde & PAGING_PAE_PDE_RESERVED_BITS) {
    BX_DEBUG(("PAE PDE: reserved bit is set PDE=%08x:%08x", GET32H(pde), GET32L(pde)));
    page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
  }
#if BX_SUPPORT_X86_64
  if (pde & PAGE_DIRECTORY_NX_BIT) {
    if (! BX_CPU_THIS_PTR efer.get_NXE()) {
      BX_DEBUG(("PDE: NX bit set when EFER.NXE is disabled"));
      page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
    }
    if (access_type == CODE_ACCESS) {
      BX_DEBUG(("PDE: non-executable page fault occured"));
      nx_fault = 1;
    }
  }
#endif

  // Ignore CR4.PSE in PAE mode
  if (pde & 0x80) {
    if (pde & PAGING_PAE_PDE4M_RESERVED_BITS) {
      BX_DEBUG(("PAE PDE4M: reserved bit is set PDE=%08x:%08x", GET32H(pde), GET32L(pde)));
      page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
    }

    // Combined access is just access from the pde (no pte involved).
    combined_access = (pde) & 0x06; // U/S and R/W
#if BX_SUPPORT_X86_64
    if (long_mode()) {
      combined_access &= (pml4 & pdpe) & 0x06;
    }
#endif

#if BX_SUPPORT_GLOBAL_PAGES
    if (BX_CPU_THIS_PTR cr4.get_PGE())
      combined_access |= (pde & 0x100); // G
#endif

    priv_index =
#if BX_CPU_LEVEL >= 4
      (BX_CPU_THIS_PTR cr0.get_WP() << 4) |  // bit 4
#endif
      (pl<<3) |                              // bit 3
      (combined_access & 0x06) |             // bit 2,1
      (isWrite);                             // bit 0

    if (!priv_check[priv_index] || nx_fault)
      page_fault(ERROR_PROTECTION, laddr, pl, isWrite, access_type);

#if BX_SUPPORT_X86_64
    if (long_mode()) {
      // Update PML4 A bit if needed.
      if (!(pml4 & 0x20)) {
        pml4 |= 0x20;
        BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pml4_addr, 8, &pml4);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pml4_addr, 8, BX_WRITE, (Bit8u*)(&pml4));
      }
    }
#endif

    // Update PDPE A bit if needed.
    if (!(pdpe & 0x20)) {
      pdpe |= 0x20;
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pdpe_addr, 8, &pdpe);
      BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pdpe_addr, 8, BX_WRITE, (Bit8u*)(&pdpe));
    }

    // Update PDE A/D bits if needed.
    if (((pde & 0x20)==0) || (isWrite && ((pde & 0x40)==0))) {
      pde |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pde_addr, 8, &pde);
      BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 8, BX_WRITE, (Bit8u*)(&pde));
    }

    // Make up the physical page frame address.
    ppf = (bx_phy_address)((pde & BX_CONST64(0x000fffffffe00000)) | (laddr & 0x001ff000));

    return ppf;
  }

  // 4k pages, Get page table entry
  bx_phy_address pte_addr = (bx_phy_address)((pde & BX_CONST64(0x000ffffffffff000)) |
                           ((laddr & 0x001ff000) >> 9));
  
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pte_addr, 8, &pte);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pte_addr, 8, BX_READ, (Bit8u*)(&pte));

  if (!(pte & 0x1)) {
    BX_DEBUG(("PAE PTE: entry not present"));
    page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
  }
#if BX_PHY_ADDRESS_WIDTH == 32
  if (pte & BX_CONST64(0x000fffff00000000)) {
    BX_PANIC(("PAE PTE 0x%08x%08x: Only 32 bit physical address space is emulated !", GET32H(pte), GET32L(pte)));
  }
#endif
  if (pte & PAGING_PAE_PTE_RESERVED_BITS) {
    BX_DEBUG(("PAE PTE: reserved bit is set PTE=%08x:%08x", GET32H(pte), GET32L(pte)));
    page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
  }
#if BX_SUPPORT_X86_64
  if (pte & PAGE_DIRECTORY_NX_BIT) {
    if (! BX_CPU_THIS_PTR efer.get_NXE()) {
      BX_DEBUG(("PTE: NX bit set when EFER.NXE is disabled"));
      page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
    }
    if (access_type == CODE_ACCESS) {
      BX_DEBUG(("PTE: non-executable page fault occured"));
      nx_fault = 1;
    }
  }
#endif

  combined_access = (pde & pte) & 0x06; // U/S and R/W
#if BX_SUPPORT_X86_64
  if (long_mode()) {
    combined_access &= (pml4 & pdpe) & 0x06;
  }
#endif

#if BX_SUPPORT_GLOBAL_PAGES
  if (BX_CPU_THIS_PTR cr4.get_PGE())
    combined_access |= (pte & 0x100); // G
#endif

  priv_index =
#if BX_CPU_LEVEL >= 4
   (BX_CPU_THIS_PTR cr0.get_WP() << 4) |  // bit 4
#endif
   (pl<<3) |                              // bit 3
   (combined_access & 0x06) |             // bit 2,1
   (isWrite);                             // bit 0

  if (!priv_check[priv_index] || nx_fault)
    page_fault(ERROR_PROTECTION, laddr, pl, isWrite, access_type);

#if BX_SUPPORT_X86_64
    if (long_mode()) {
      // Update PML4 A bit if needed.
      if (!(pml4 & 0x20)) {
        pml4 |= 0x20;
        BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pml4_addr, 8, &pml4);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pml4_addr, 8, BX_WRITE, (Bit8u*)(&pml4));
      }
    }
#endif

  // Update PDPE A bit if needed.
  if (!(pdpe & 0x20)) {
    pdpe |= 0x20;
    BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pdpe_addr, 8, &pdpe);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pdpe_addr, 8, BX_WRITE, (Bit8u*)(&pdpe));
  }

  // Update PDE A bit if needed.
  if (!(pde & 0x20)) {
    pde |= 0x20;
    BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pde_addr, 8, &pde);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 8, BX_WRITE, (Bit8u*)(&pde));
  }

  // Update PTE A/D bits if needed.
  if (((pte & 0x20)==0) || (isWrite && ((pte & 0x40)==0))) {
    pte |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
    BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pte_addr, 8, &pte);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pte_addr, 8, BX_WRITE, (Bit8u*)(&pte));
  }

  // Make up the physical page frame address.
  ppf = (bx_phy_address)(pte & BX_CONST64(0x000ffffffffff000));

  return ppf;
}

#endif // BX_SUPPORT_PAE

/* PSE PDE4M: bits [21:17] */
#define PAGING_PSE_PDE4M_RESERVED_BITS \
    (BX_PHY_ADDRESS_RESERVED_BITS | BX_CONST64(0x003E0000))

// Translate a linear address to a physical address
bx_phy_address BX_CPU_C::translate_linear(bx_address laddr, unsigned curr_pl, unsigned rw, unsigned access_type)
{
  Bit32u   combined_access = 0;
  unsigned priv_index;

  // note - we assume physical memory < 4gig so for brevity & speed, we'll use
  // 32 bit entries although cr3 is expanded to 64 bits.
  bx_phy_address paddress, ppf, poffset = PAGE_OFFSET(laddr);
  bx_bool isWrite = (rw >= BX_WRITE); // write or r-m-w
  unsigned pl = (curr_pl == 3);

  BX_ASSERT(BX_CPU_THIS_PTR cr0.get_PG());

  InstrTLB_Increment(tlbLookups);
  InstrTLB_Stats();

  bx_address lpf = LPFOf(laddr);
  unsigned TLB_index = BX_TLB_INDEX_OF(lpf, 0);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[TLB_index];

  // already looked up TLB for code access
  if (TLB_LPFOf(tlbEntry->lpf) == lpf)
  {
    paddress = tlbEntry->ppf | poffset;

    if (! (tlbEntry->accessBits & ((isWrite<<1) | pl)))
      return paddress;

    // The current access does not have permission according to the info
    // in our TLB cache entry.  Re-walk the page tables, in case there is
    // updated information in the memory image, and let the long path code
    // generate an exception if one is warranted.
  }

  BX_DEBUG(("page walk for address 0x" FMT_LIN_ADDRX, laddr));

  InstrTLB_Increment(tlbMisses);

#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE())
  {
    ppf = translate_linear_PAE(laddr, combined_access, curr_pl, rw, access_type);
  }
  else
#endif  // #if BX_SUPPORT_PAE
  {
    // CR4.PAE==0 (and EFER.LMA==0)
    Bit32u pde, pte;

    bx_phy_address pde_addr = (bx_phy_address) (BX_CPU_THIS_PTR cr3_masked |
                             ((laddr & 0xffc00000) >> 20));

    BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 4, BX_READ, (Bit8u*)(&pde));

    if (!(pde & 0x1)) {
      BX_DEBUG(("PDE: entry not present"));
      page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
    }

#if BX_SUPPORT_LARGE_PAGES
    if ((pde & 0x80) && BX_CPU_THIS_PTR cr4.get_PSE())
    {
      // Note: when the PSE and PAE flags in CR4 are set, the
      //       processor generates a PF if the reserved bits are not zero.
      if (pde & PAGING_PSE_PDE4M_RESERVED_BITS) {
        BX_DEBUG(("PSE PDE4M: reserved bit is set: PDE=0x%08x", pde));
        page_fault(ERROR_RESERVED | ERROR_PROTECTION, laddr, pl, isWrite, access_type);
      }

#if BX_PHY_ADDRESS_WIDTH == 32
      if (pde & 0x0001e000) {
        BX_PANIC(("PSE PDE4M 0x%08x: Only 32 bit physical address space is emulated !", pde));
      }
#endif
      // Combined access is just access from the pde (no pte involved).
      combined_access = pde & 0x06; // U/S and R/W

#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE())
        combined_access |= pde & 0x100; // {G}
#endif

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.get_WP() << 4) |  // bit 4
#endif
        (pl<<3) |                              // bit 3
        (combined_access & 0x06) |             // bit 2,1
        (isWrite);                             // bit 0

      if (!priv_check[priv_index])
        page_fault(ERROR_PROTECTION, laddr, pl, isWrite, access_type);

      // Update PDE A/D bits if needed.
      if (((pde & 0x20)==0) || (isWrite && ((pde & 0x40)==0))) {
        pde |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 4, BX_WRITE, (Bit8u*)(&pde));
      }

      // make up the physical frame number
      ppf = (pde & 0xffc00000) | (laddr & 0x003ff000);
    }
    else // else normal 4K page...
#endif
    {
      // Get page table entry
      bx_phy_address pte_addr = (bx_phy_address)((pde & 0xfffff000) | ((laddr & 0x003ff000) >> 10));

      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pte_addr, 4, &pte);
      BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pte_addr, 4, BX_READ, (Bit8u*)(&pte));

      if (!(pte & 0x1)) {
        BX_DEBUG(("PTE: entry not present"));
        page_fault(ERROR_NOT_PRESENT, laddr, pl, isWrite, access_type);
      }

      // 386 and 486+ have different behaviour for combining
      // privilege from PDE and PTE.
#if BX_CPU_LEVEL == 3
      combined_access  = (pde | pte) & 0x04; // U/S
      combined_access |= (pde & pte) & 0x02; // R/W
#else // 486+
      combined_access  = (pde & pte) & 0x06; // U/S and R/W
#endif

#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE())
        combined_access |= (pte & 0x100); // G
#endif

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.get_WP() << 4) |  // bit 4
#endif
        (pl<<3) |                              // bit 3
        (combined_access & 0x06) |             // bit 2,1
        (isWrite);                             // bit 0

      if (!priv_check[priv_index])
        page_fault(ERROR_PROTECTION, laddr, pl, isWrite, access_type);

      // Update PDE A bit if needed.
      if (!(pde & 0x20)) {
        pde |= 0x20;
        BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pde_addr, 4, BX_WRITE, (Bit8u*)(&pde));
      }

      // Update PTE A/D bits if needed.
      if (((pte & 0x20)==0) || (isWrite && ((pte & 0x40)==0))) {
        pte |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pte_addr, 4, &pte);
        BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pte_addr, 4, BX_WRITE, (Bit8u*)(&pte));
      }

      // Make up the physical page frame address.
      ppf = pte & 0xfffff000;
    }
  }

  // Calculate physical memory address and fill in TLB cache entry
  paddress = ppf | poffset;

  // direct memory access is NOT allowed by default
  tlbEntry->lpf = lpf | TLB_HostPtr;
  tlbEntry->ppf = ppf;
  tlbEntry->accessBits = 0;

  if ((combined_access & 4) == 0) { // System
    tlbEntry->accessBits |= TLB_SysOnly;
    if (! isWrite)
      tlbEntry->accessBits |= TLB_ReadOnly;
  }
  else {
    // Current operation is a read or a page is read only
    // Not efficient handling of system write to user read only page:
    // hopefully it is very rare case, optimize later
    if (! isWrite || (combined_access & 2) == 0) {
       tlbEntry->accessBits |= TLB_ReadOnly;
    }
  }

#if BX_SUPPORT_GLOBAL_PAGES
  if (combined_access & 0x100) // Global bit
    tlbEntry->accessBits |= TLB_GlobalPage;
#endif

#if BX_SupportGuest2HostTLB
  // Attempt to get a host pointer to this physical page. Put that
  // pointer in the TLB cache. Note if the request is vetoed, NULL
  // will be returned, and it's OK to OR zero in anyways.
  tlbEntry->hostPageAddr = (bx_hostpageaddr_t) BX_MEM(0)->getHostMemAddr(BX_CPU_THIS,
       A20ADDR(ppf), rw, access_type);

  if (tlbEntry->hostPageAddr) {
    // All access allowed also via direct pointer
#if BX_X86_DEBUGGER
    if (! hwbreakpoint_check(laddr))
#endif
       tlbEntry->lpf = lpf; // allow direct access with HostPtr
  }
#endif

  return paddress;
}

#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION || BX_GDBSTUB

bx_bool BX_CPU_C::dbg_xlate_linear2phy(bx_address laddr, bx_phy_address *phy)
{
  if (BX_CPU_THIS_PTR cr0.get_PG() == 0) {
    *phy = (bx_phy_address) laddr;
    return 1;
  }

  bx_phy_address paddress;

  // see if page is in the TLB first
  bx_address lpf = LPFOf(laddr);
  unsigned TLB_index = BX_TLB_INDEX_OF(lpf, 0);
  bx_TLB_entry *tlbEntry  = &BX_CPU_THIS_PTR TLB.entry[TLB_index];

  if (TLB_LPFOf(tlbEntry->lpf) == lpf) {
    paddress = tlbEntry->ppf | PAGE_OFFSET(laddr);
    *phy = paddress;
    return 1;
  }

  bx_phy_address pt_address = BX_CPU_THIS_PTR cr3_masked;
  bx_address offset_mask = 0xfff;

#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE()) {
    int levels = 3;
#if BX_SUPPORT_X86_64
    if (long_mode())
      levels = 4;
#endif
    for (int level = levels - 1; level >= 0; --level) {
      Bit64u pte;
      pt_address += 8 * ((laddr >> (12 + 9*level)) & 511);
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pt_address, 8, &pte);
      if(!(pte & 1))
	goto page_fault;
      if (pte & BX_PHY_ADDRESS_RESERVED_BITS)
	goto page_fault;
      pt_address = bx_phy_address(pte & BX_CONST64(0x000ffffffffff000));
      if (level == 1 && (pte & 0x80)) { // PSE page
	offset_mask = 0x1fffff;
	break;
      }
    }
    paddress = pt_address + (bx_phy_address)(laddr & offset_mask);
  }
  else   // not PAE
#endif
  {
    for (int level = 1; level >= 0; --level) {
      Bit32u pte;
      pt_address += 4 * ((laddr >> (12 + 10*level)) & 1023);
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pt_address, 4, &pte);
      if (!(pte & 1))
	goto page_fault;
      pt_address = pte & 0xfffff000;
      if (level == 1 && (pte & 0x80)) { // PSE page
	offset_mask = 0x3fffff;
	break;
      }
    }
    paddress = pt_address + (bx_phy_address)(laddr & offset_mask);
  }

  *phy = paddress;
  return 1;

page_fault:
  *phy = 0;
  return 0;
}
#endif

void BX_CPU_C::access_write_linear(bx_address laddr, unsigned len, unsigned curr_pl, void *data)
{
#if BX_X86_DEBUGGER
  hwbreakpoint_match(laddr, len, BX_WRITE);
#endif

  Bit32u pageOffset = PAGE_OFFSET(laddr);

  if (BX_CPU_THIS_PTR cr0.get_PG()) {
    /* check for reference across multiple pages */
    if ((pageOffset + len) <= 4096) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, curr_pl, BX_WRITE);
      BX_CPU_THIS_PTR address_xlation.pages     = 1;

      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, BX_CPU_THIS_PTR address_xlation.paddress1, len, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr, BX_CPU_THIS_PTR address_xlation.paddress1,
                          len, curr_pl, BX_WRITE, (Bit8u*) data);

      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1, len, data);
    }
    else {
      // access across 2 pages
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, curr_pl, BX_WRITE);
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = len -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.paddress2 =
          dtranslate_linear(laddr2, curr_pl, BX_WRITE);

#ifdef BX_LITTLE_ENDIAN
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl, 
          BX_WRITE, (Bit8u*) data);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl, 
          BX_WRITE, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2,
          ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl, 
          BX_WRITE, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1,
          ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl, 
          BX_WRITE, (Bit8u*) data);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, data);
#endif
    }
  }
  else {
    // Paging off.
    if ((pageOffset + len) <= 4096) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 = (bx_phy_address) laddr;
      BX_CPU_THIS_PTR address_xlation.pages     = 1;
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, (bx_phy_address) laddr, len, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr, (bx_phy_address) laddr, len,
                          curr_pl, BX_WRITE, (Bit8u*) data);

#if BX_SupportGuest2HostTLB
      // do not replace to the TLB if there is a breakpoint defined
      // in the same page
#if BX_X86_DEBUGGER
      if (! hwbreakpoint_check(laddr))
#endif
      {
        unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
        bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
        bx_address lpf = LPFOf(laddr);
      
        if (TLB_LPFOf(tlbEntry->lpf) != lpf) {
          // We haven't seen this page, or it's been bumped before.

          // Request a direct write pointer so we can do either R or W.
          bx_hostpageaddr_t hostPageAddr = (bx_hostpageaddr_t)
            BX_MEM(0)->getHostMemAddr(BX_CPU_THIS, A20ADDR(lpf), BX_WRITE, DATA_ACCESS);

          if (hostPageAddr) {
            tlbEntry->lpf = lpf; // Got direct write pointer OK
            tlbEntry->ppf = (bx_phy_address) lpf;
            tlbEntry->hostPageAddr = hostPageAddr;
            // Mark for any operation to succeed.
            tlbEntry->accessBits = 0;
          }
        }
      }
#endif

      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, (bx_phy_address) laddr, len, data);
    }
    else {
      // Access spans two pages.
      BX_CPU_THIS_PTR address_xlation.paddress1 = (bx_phy_address) laddr;
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = len -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.paddress2 = (bx_phy_address) laddr2;

#ifdef BX_LITTLE_ENDIAN
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_WRITE, (Bit8u*) data);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_WRITE, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2,
          ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_WRITE, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1,
          ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, BX_WRITE);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_WRITE, (Bit8u*) data);
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, data);
#endif
    }
  }
}

void BX_CPU_C::access_read_linear(bx_address laddr, unsigned len, unsigned curr_pl, unsigned xlate_rw, void *data)
{
  BX_ASSERT(xlate_rw == BX_READ || xlate_rw == BX_RW);

#if BX_X86_DEBUGGER
  hwbreakpoint_match(laddr, len, xlate_rw);
#endif

  Bit32u pageOffset = PAGE_OFFSET(laddr);

  if (BX_CPU_THIS_PTR cr0.get_PG()) {
    /* check for reference across multiple pages */
    if ((pageOffset + len) <= 4096) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, curr_pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.pages     = 1;
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1, len, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1, len, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1, len, curr_pl,
          BX_READ, (Bit8u*) data);
    }
    else {
      // access across 2 pages
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, curr_pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = len -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.paddress2 =
          dtranslate_linear(laddr2, curr_pl, xlate_rw);

#ifdef BX_LITTLE_ENDIAN
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_READ, (Bit8u*) data);
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2,
          ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_READ, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1,
          ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_READ, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_READ, (Bit8u*) data);
#endif
    }
  }
  else {
    // Paging off.
    if ((pageOffset + len) <= 4096) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 = (bx_phy_address) laddr;
      BX_CPU_THIS_PTR address_xlation.pages     = 1;
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr, (bx_phy_address) laddr, len, xlate_rw);

#if BX_SupportGuest2HostTLB
      // do not replace to the TLB if there is a breakpoint defined
      // in the same page
#if BX_X86_DEBUGGER
      if (! hwbreakpoint_check(laddr))
#endif
      {
        unsigned tlbIndex = BX_TLB_INDEX_OF(laddr, 0);
        bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
        bx_address lpf = LPFOf(laddr);

        if (TLB_LPFOf(tlbEntry->lpf) != lpf) {
          // We haven't seen this page, or it's been bumped before.

          // Request a direct write pointer so we can do either R or W.
          bx_hostpageaddr_t hostPageAddr = (bx_hostpageaddr_t)
              BX_MEM(0)->getHostMemAddr(BX_CPU_THIS, A20ADDR(lpf), BX_READ, DATA_ACCESS);

          if (hostPageAddr) {
            tlbEntry->lpf = lpf; // Got direct read pointer OK.
            tlbEntry->ppf = (bx_phy_address) lpf;
            tlbEntry->hostPageAddr = hostPageAddr;
            // Mark for any following read to succeed.
            tlbEntry->accessBits = TLB_ReadOnly;
          }
        }
      }
#endif

      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, (bx_phy_address) laddr, len, data);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr, (bx_phy_address) laddr, len,
                          curr_pl, BX_READ, (Bit8u*) data);
    }
    else {
      // Access spans two pages.
      BX_CPU_THIS_PTR address_xlation.paddress1 = (bx_phy_address) laddr;
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = len -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      bx_address laddr2 = laddr + BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.paddress2 = (bx_phy_address) laddr2;

#ifdef BX_LITTLE_ENDIAN
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_READ, (Bit8u*) data);
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2,
          ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_READ, ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
#else // BX_BIG_ENDIAN
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1,
          ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr,
          BX_CPU_THIS_PTR address_xlation.paddress1,
          BX_CPU_THIS_PTR address_xlation.len1, curr_pl,
          BX_READ, ((Bit8u*)data) + (len - BX_CPU_THIS_PTR address_xlation.len1));
      BX_MEM(0)->readPhysicalPage(BX_CPU_THIS,
          BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, data);
      BX_INSTR_LIN_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, xlate_rw);
      BX_DBG_LIN_MEMORY_ACCESS(BX_CPU_ID, laddr2, BX_CPU_THIS_PTR address_xlation.paddress2,
          BX_CPU_THIS_PTR address_xlation.len2, curr_pl,
          BX_READ, (Bit8u*) data);
#endif
    }
  }
}
