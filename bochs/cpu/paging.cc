/////////////////////////////////////////////////////////////////////////
// $Id: paging.cc,v 1.66 2006-03-02 23:16:12 sshwarts Exp $
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


// Notes from merge of x86-64 enhancements:
//   Looks like for x86-64/PAE=1/PTE with PSE=1, the
//     CR4.PSE field is not consulted by the processor?
//   Fix the PAE case to not update the page table tree entries
//     until the final protection check?  This is how it is on
//     P6 for non-PAE anyways...



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if 0
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
//     Page size extention and physical address size extention matrix
//   ====================================================================
//   CR0.PG  CR4.PAE  CR4.PSE  PDE.PS | page size   physical address size
//   ====================================================================
//      0       X        X        X   |    -          paging disabled
//      1       0        0        X   |   4K              32bits
//      1       0        1        0   |   4K              32bits
//      1       0        1        1   |   4M              32bits
//      1       1        X        0   |   4K              36bits
//      1       1        X        1   |   2M              36bits


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
//
// - Pentium+ processors have separate TLB's for data and instruction caches
// - Pentium Pro+ processors maintain separate 4K and 4M TLBs.
#endif

#if BX_SUPPORT_PAGING

#define BX_INVALID_TLB_ENTRY 0xffffffff

#if BX_USE_QUICK_TLB_INVALIDATE
#define BX_MAX_TLB_INVALIDATE 0xffe
#endif

#define BX_USE_TLB_GENERATION 1

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
//    +---------------> Current CR0.wp value


// Each entry in the TLB cache has 3 entries:
//   lpf:         Linear Page Frame (page aligned linear address of page)
//     bits 32..12 Linear page frame.
//     bits 11..0  Invalidate index.
//   ppf:         Physical Page Frame (page aligned phy address of page)
//   accessBits:
//     bits 32..11: Host Page Frame address used for direct access to
//                  the mem.vector[] space allocated for the guest physical
//                  memory.  If this is zero, it means that a pointer
//                  to the host space could not be generated, likely because
//                  that page of memory is not standard memory (it might
//                  be memory mapped IO, ROM, etc).
//     bits  9..10: (currently unused)
//
//     bit   8:     Page is a global page.
//
//
//       The following 4 bits are used for a very efficient permissions
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
//         OK = 1 << ( (W<<1) | U )   [W:1=write, 0=read, U:1=CPL3,0=CPL0-2]
//       
//       Thus for reads, it is:
//         OK = 0x10 << (          U )
//       And for writes:
//         OK = 0x40 << (          U )
//
//     bit 7:       a Write from User   privilege is OK
//     bit 6:       a Write from System privilege is OK
//     bit 5:       a Read  from User   privilege is OK
//     bit 4:       a Read  from System privilege is OK
//
//       And the lowest 4 bits are as above, except that they also indicate
//       that hostPageAddr is valid, so we do not separately need to test 
//       that pointer against NULL.  These have smaller constants for us
//       to be able to use smaller encodings in the trace generators.  Note
//       that whenever bit n (n=0,1,2,3) is set, then also n+4 is set.
//       (The opposite is of course not true)
//
//     bit 3:       a Write from User   privilege is OK, hostPageAddr is valid
//     bit 2:       a Write from System privilege is OK, hostPageAddr is valid
//     bit 1:       a Read  from User   privilege is OK, hostPageAddr is valid
//     bit 0:       a Read  from System privilege is OK, hostPageAddr is valid
//

#define TLB_GlobalPage       0x100

#define TLB_WriteUserOK       0x80
#define TLB_WriteSysOK        0x40
#define TLB_ReadUserOK        0x20
#define TLB_ReadSysOK         0x10
#define TLB_WriteUserPtrOK    0x08
#define TLB_WriteSysPtrOK     0x04
#define TLB_ReadUserPtrOK     0x02
#define TLB_ReadSysPtrOK      0x01



#ifdef __GNUC__
#warning "Move priv_check to CPU fields, or init.cc"
#endif

static unsigned priv_check[BX_PRIV_CHECK_SIZE];


#define PAGE_DIRECTORY_NX_BIT (BX_CONST64(0x8000000000000000))


// === TLB Instrumentation section ==============================

// Note: this is an approximation of what Peter Tattam had.

#define InstrumentTLB 0

#if InstrumentTLB
static unsigned tlbLookups=0;
static unsigned tlbMisses=0;
static unsigned tlbGlobalFlushes=0;
static unsigned tlbNonGlobalFlushes=0;
static unsigned tlbEntryFlushes=0;
static unsigned tlbEntryInvlpg=0;

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

// ==============================================================


  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::pagingCR0Changed(Bit32u oldCR0, Bit32u newCR0)
{
  // Modification of PG,PE flushes TLB cache according to docs.
  // Additionally, the TLB strategy is based on the current value of
  // WP, so if that changes we must also flush the TLB.
  if ( (oldCR0 & 0x80010001) != (newCR0 & 0x80010001) )
    TLB_flush(1); // 1 = Flush Global entries also.

  if (bx_dbg.paging)
    BX_INFO(("pagingCR0Changed: (0x%x -> 0x%x)", oldCR0, newCR0));
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::pagingCR4Changed(Bit32u oldCR4, Bit32u newCR4)
{
  // Modification of PGE,PAE,PSE flushes TLB cache according to docs.
  if ( (oldCR4 & 0x000000b0) != (newCR4 & 0x000000b0) )
    TLB_flush(1); // 1 = Flush Global entries also.

  if (bx_dbg.paging)
    BX_INFO(("pagingCR4Changed: (0x%x -> 0x%x)", oldCR4, newCR4));

#if BX_SUPPORT_PAE
  if ( (oldCR4 & 0x00000020) != (newCR4 & 0x00000020) ) {
    if (BX_CPU_THIS_PTR cr4.get_PAE())
      BX_CPU_THIS_PTR cr3_masked = BX_CPU_THIS_PTR cr3 & 0xffffffe0;
    else
      BX_CPU_THIS_PTR cr3_masked = BX_CPU_THIS_PTR cr3 & 0xfffff000;
  }
#endif
}

  void BX_CPP_AttrRegparmN(1)
BX_CPU_C::CR3_change(bx_address value)
{
  if (bx_dbg.paging) {
    BX_INFO(("CR3_change(): flush TLB cache"));
    BX_INFO(("Page Directory Base %08x", (unsigned) value));
  }

  // flush TLB even if value does not change
  TLB_flush(0); // 0 = Don't flush Global entries.
  BX_CPU_THIS_PTR cr3 = value;
#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE())
    BX_CPU_THIS_PTR cr3_masked = value & 0xffffffe0;
  else
#endif
    BX_CPU_THIS_PTR cr3_masked = value & 0xfffff000;
}

  void
BX_CPU_C::pagingA20Changed(void)
{
  TLB_flush(1); // 1 = Flush Global entries too.
}

  void
BX_CPU_C::TLB_init(void)
{
  // Called to initialize the TLB upon startup.
  // Unconditional initialization of all TLB entries.

#if BX_USE_TLB
  unsigned i;
  unsigned wp, us_combined, rw_combined, us_current, rw_current;

  for (i=0; i<BX_TLB_SIZE; i++)
    BX_CPU_THIS_PTR TLB.entry[i].lpf = BX_INVALID_TLB_ENTRY;

  //
  // Setup privilege check matrix.
  //

  for (i=0; i<BX_PRIV_CHECK_SIZE; i++) {
    wp          = (i & 0x10) >> 4;
    us_current  = (i & 0x08) >> 3;
    us_combined = (i & 0x04) >> 2;
    rw_combined = (i & 0x02) >> 1;
    rw_current  = (i & 0x01) >> 0;
    if (wp) { // when write protect on
      if (us_current > us_combined) // user access, supervisor page
        priv_check[i] = 0;
      else if (rw_current > rw_combined) // RW access, RO page
        priv_check[i] = 0;
      else
        priv_check[i] = 1;
    }
    else { // when write protect off
      if (us_current == 0) // Supervisor mode access, anything goes
        priv_check[i] = 1;
      else {
        // user mode access
        if (us_combined == 0) // user access, supervisor Page
          priv_check[i] = 0;
        else if (rw_current > rw_combined) // RW access, RO page
          priv_check[i] = 0;
        else
          priv_check[i] = 1;
      }
    }
  }

#if BX_USE_QUICK_TLB_INVALIDATE
  BX_CPU_THIS_PTR TLB.tlb_invalidate = BX_MAX_TLB_INVALIDATE;
#endif

#endif  // #if BX_USE_TLB
}

void BX_CPU_C::TLB_flush(bx_bool invalidateGlobal)
{
#if InstrumentTLB
  if (invalidateGlobal)
    InstrTLB_Increment(tlbGlobalFlushes);
  else
    InstrTLB_Increment(tlbNonGlobalFlushes);
#endif

#if BX_USE_TLB
  for (unsigned i=0; i<BX_TLB_SIZE; i++) {
    // To be conscious of the native cache line usage, only
    // write to (invalidate) entries which need it.
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[i];
    if (tlbEntry->lpf != BX_INVALID_TLB_ENTRY) {
#if BX_SUPPORT_GLOBAL_PAGES
      if (invalidateGlobal || !(tlbEntry->accessBits & TLB_GlobalPage))
#endif
      {
        tlbEntry->lpf = BX_INVALID_TLB_ENTRY;
        InstrTLB_Increment(tlbEntryFlushes); // A TLB entry flush occurred.
      }
    }
  }
#endif  // #if BX_USE_TLB
}

void BX_CPU_C::TLB_invlpg(bx_address laddr)
{
  Bit32u TLB_index = BX_TLB_INDEX_OF(laddr);
  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = BX_INVALID_TLB_ENTRY;
  InstrTLB_Increment(tlbEntryFlushes); // A TLB entry flush occurred.
}

void BX_CPU_C::INVLPG(bxInstruction_c* i)
{
#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  // Operand must not be a register
  if (i->modC0()) {

#if BX_SUPPORT_X86_64

    //
    // Opcode 0F 01:
    //

    // ----------------------------------------------------
    //     MOD    REG  RM  | non 64 bit mode | 64 bit mode
    // ----------------------------------------------------
    //  MOD <> 11  7   --- |     INVLPG      |   INVLPG
    //  MOD == 11  7    0  |      #UD        |   SWAPGS
    //  MOD == 11  7    1  |      #UD        |   RDTSCP
    //  MOD == 11  7   2-7 |      #UD        |    #UD

    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      if (i->nnn() == 7) {
        switch(i->rm()) {
        case 0:
          BX_CPU_THIS_PTR SWAPGS(i);
          return;
        case 1:
          BX_CPU_THIS_PTR RDTSCP(i);
          return;
        default:
          BX_INFO(("INVLPG: 0F 01 /7 RM=%d opcode is undefined !", i->rm()));
          UndefinedOpcode(i);
        }
      }
    }

#endif

    BX_INFO(("INVLPG: op is a register"));
    UndefinedOpcode(i);
  }

  // Can not be executed in v8086 mode
  if (v8086_mode())
    exception(BX_GP_EXCEPTION, 0, 0);

  // Protected instruction: CPL0 only
  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

#if BX_USE_TLB
  bx_address laddr = BX_CPU_THIS_PTR get_segment_base(i->seg()) + RMAddr(i);
  TLB_invlpg(laddr);
  InstrTLB_Increment(tlbEntryInvlpg);
#endif // BX_USE_TLB

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_INVLPG, 0);

#else
  // not supported on < 486
  BX_INFO(("INVLPG: required i486, use --enable-cpu=4 option"));
  UndefinedOpcode(i);
#endif
}

// Translate a linear address to a physical address, for
// a data access (D)

  Bit32u BX_CPP_AttrRegparmN(3)
BX_CPU_C::translate_linear(bx_address laddr, unsigned pl, unsigned rw, unsigned access_type)
{
  bx_address lpf;
  Bit32u   accessBits, combined_access = 0, error_code = 0;
  unsigned priv_index;
#if BX_USE_TLB
  Bit32u TLB_index;
#endif

  InstrTLB_Increment(tlbLookups);
  InstrTLB_Stats();

  // note - we assume physical memory < 4gig so for brevity & speed, we'll use
  // 32 bit entries although cr3 is expanded to 64 bits.
  Bit32u ppf, poffset, paddress;

  bx_bool isWrite = (rw >= BX_WRITE); // write or r-m-w

#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE())
  {
    bx_address pde, pdp;
    Bit32u pde_addr;
    Bit32u pdp_addr;

    lpf     = laddr & BX_CONST64(0xfffffffffffff000); // linear page frame
    poffset = laddr & 0x00000fff; // physical offset

#if BX_USE_TLB
    TLB_index = BX_TLB_INDEX_OF(lpf);
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[TLB_index];

    if (tlbEntry->lpf == BX_TLB_LPF_VALUE(lpf)) 
    {
      paddress   = tlbEntry->ppf | poffset;
      accessBits = tlbEntry->accessBits;

      if (accessBits & (0x10 << ((isWrite<<1) | pl)))
        return(paddress);

      // The current access does not have permission according to the info
      // in our TLB cache entry.  Re-walk the page tables, in case there is
      // updated information in the memory image, and let the long path code
      // generate an exception if one is warranted.
    }
#endif

    InstrTLB_Increment(tlbMisses);

#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR msr.lma)
    {
      bx_address pml4;

      // Get PML4 entry
      Bit32u pml4_addr = BX_CPU_THIS_PTR cr3_masked |
                  ((laddr & BX_CONST64(0x0000ff8000000000)) >> 36);
      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pml4_addr, 8, &pml4);

      if ( !(pml4 & 0x01) ) {
        goto page_fault_not_present; // PML4 Entry NOT present
      }
      if (pml4 & PAGE_DIRECTORY_NX_BIT) {
        if (! BX_CPU_THIS_PTR msr.nxe)
          goto page_fault_reserved;
        else if (access_type == CODE_ACCESS)
          goto page_fault_access;
      }
      if ( !(pml4 & 0x20) )
      {
        pml4 |= 0x20;
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pml4_addr, 8, &pml4);
      }

      // Get PDP entry
      pdp_addr = (pml4 & 0xfffff000) |
                 ((laddr & BX_CONST64(0x0000007fc0000000)) >> 27);
    }
    else
#endif
    {
      pdp_addr = BX_CPU_THIS_PTR cr3_masked | ((laddr & 0xc0000000) >> 27);
    }

    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pdp_addr, sizeof(bx_address), &pdp);

    if ( !(pdp & 0x01) ) {
      goto page_fault_not_present; // PDP Entry NOT present
    }
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR msr.lma)
    {
      if (pdp & PAGE_DIRECTORY_NX_BIT) {
        if (! BX_CPU_THIS_PTR msr.nxe)
          goto page_fault_reserved;
        else if (access_type == CODE_ACCESS)
          goto page_fault_access;
      }
    }
#endif
    if ( !(pdp & 0x20) ) {
      pdp |= 0x20;
      BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pdp_addr, sizeof(bx_address), &pdp);
    }

    // Get page dir entry
    pde_addr = (pdp & 0xfffff000) | ((laddr & 0x3fe00000) >> 18);

    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pde_addr, sizeof(bx_address), &pde);

    if ( !(pde & 0x01) ) {
      goto page_fault_not_present; // Page Directory Entry NOT present
    }

#if BX_SUPPORT_X86_64
    if (pde & PAGE_DIRECTORY_NX_BIT) {
      if (! BX_CPU_THIS_PTR msr.nxe)
        goto page_fault_reserved;
      else if (access_type == CODE_ACCESS)
        goto page_fault_access;
    }
#endif

#if BX_SUPPORT_4MEG_PAGES
    // (KPL) Weird.  I would think the processor would consult CR.PSE?
    // if ((pde & 0x80) && (BX_CPU_THIS_PTR cr4.get_PSE())) {}
    if (pde & 0x80) {
      // 4M pages are enabled, and this is a 4Meg page.

      // Combined access is just access from the pde (no pte involved).
      combined_access = pde & 0x06; // U/S and R/W
      // Make up the physical page frame address.
      ppf = (pde & 0xffe00000) | (laddr & 0x001ff000);

#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE()) { // PGE==1
        combined_access |= (pde & TLB_GlobalPage);  // G
      }
#endif

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.wp<<4) |  // bit 4
#endif
        (pl<<3) |                      // bit 3
        (combined_access & 0x06) |     // bit 2,1
        isWrite;                       // bit 0

      if (!priv_check[priv_index]) goto page_fault_access;

      // Update PDE if A/D bits if needed.
      if ( ((pde & 0x20)==0) || (isWrite && ((pde&0x40)==0)) )
      {
        pde |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pde_addr, sizeof(bx_address), &pde);
      }
    }
    else
#endif
    { // 4k pages.
      bx_address pte;

      // Get page table entry
      Bit32u pte_addr = (pde & 0xfffff000) | ((laddr & 0x001ff000) >> 9);

      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pte_addr, sizeof(bx_address), &pte);

      if ( !(pte & 0x01) ) {
        goto page_fault_not_present; 
      }

#if BX_SUPPORT_X86_64
      if (pte & PAGE_DIRECTORY_NX_BIT) {
        if (! BX_CPU_THIS_PTR msr.nxe)
          goto page_fault_reserved;
        else if (access_type == CODE_ACCESS)
          goto page_fault_access;
      }
#endif

      combined_access = (pde & pte) & 0x06; // U/S and R/W

      // Make up the physical page frame address.
      ppf = pte & 0xfffff000;

#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE()) { // PGE==1
        combined_access |= (pte & TLB_GlobalPage);  // G
      }
#endif

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.wp<<4) |  // bit 4
#endif
        (pl<<3) |                      // bit 3
        (combined_access & 0x06) |     // bit 2,1
        isWrite;                       // bit 0

      if (!priv_check[priv_index]) goto page_fault_access;

      // Update PDE A bit if needed.
      if ( (pde & 0x20)==0 ) {
        pde |= 0x20; // Update A bit.
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pde_addr, sizeof(bx_address), &pde);
      }

      // Update PTE A/D bits if needed.
      if (((pte & 0x20)==0) || (isWrite && ((pte&0x40)==0))) 
      {
        pte |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pte_addr, sizeof(bx_address), &pte);
      }
    }
  }
  else
#endif  // #if BX_SUPPORT_PAE
  {
    // CR4.PAE==0 (and MSR.LMA==0)

    lpf       = laddr & 0xfffff000; // linear page frame
    poffset   = laddr & 0x00000fff; // physical offset

#if BX_USE_TLB
    TLB_index = BX_TLB_INDEX_OF(lpf);
    bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[TLB_index];

    if (tlbEntry->lpf == BX_TLB_LPF_VALUE(lpf))
    {
      paddress   = tlbEntry->ppf | poffset;
      accessBits = tlbEntry->accessBits;

      if (accessBits & (0x10 << ((isWrite<<1) | pl)))
        return(paddress);

      // The current access does not have permission according to the info
      // in our TLB cache entry.  Re-walk the page tables, in case there is
      // updated information in the memory image, and let the long path code
      // generate an exception if one is warranted.
    }
#endif

    InstrTLB_Increment(tlbMisses);

    Bit32u pde, pde_addr;

    // Get page dir entry
    pde_addr = BX_CPU_THIS_PTR cr3_masked | ((laddr & 0xffc00000) >> 20);

    BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);

    if ( !(pde & 0x01) ) {
      goto page_fault_not_present; // Page Directory Entry NOT present
    }

#if BX_SUPPORT_4MEG_PAGES
    if ((pde & 0x80) && (BX_CPU_THIS_PTR cr4.get_PSE()))
    {
      // 4M pages are enabled, and this is a 4Meg page.
      // Note: when the PSE and PAE flags in CR4 are set,
      // the processor generates a PF if the reserved bits are not
      // set to 0.  (We don't handle PAE yet, just a note for
      // the future).

      // Combined access is just access from the pde (no pte involved).
      combined_access = pde & 0x006; // {US,RW}
      // make up the physical frame number
      ppf = (pde & 0xFFC00000) | (laddr & 0x003FF000);

#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE()) { // PGE==1
        combined_access |= pde & TLB_GlobalPage;    // {G}
      }
#endif

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.wp<<4) |  // bit 4
#endif
        (pl<<3) |                      // bit 3
        (combined_access & 0x06) |     // bit 2,1
        isWrite;                       // bit 0

      if (!priv_check[priv_index]) goto page_fault_access;

      // Update PDE if A/D bits if needed.
      if (((pde & 0x20)==0) || (isWrite && ((pde&0x40)==0)))
      {
        pde |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
      }
    }
    else // Else normal 4Kbyte page...
#endif
    {
      Bit32u pte, pte_addr;

#if (BX_CPU_LEVEL < 6)
      // update PDE if A bit was not set before
      if ( !(pde & 0x20) ) {
        pde |= 0x20;
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
      }
#endif

      // Get page table entry
      pte_addr = (pde & 0xfffff000) | ((laddr & 0x003ff000) >> 10);

      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pte_addr, 4, &pte);

      if ( !(pte & 0x01) ) {
        goto page_fault_not_present; // Page Table Entry NOT present
      }

      // 386 and 486+ have different bahaviour for combining
      // privilege from PDE and PTE.
#if BX_CPU_LEVEL == 3
      combined_access  = (pde | pte) & 0x04; // U/S
      combined_access |= (pde & pte) & 0x02; // R/W
#else // 486+
      combined_access  = (pde & pte) & 0x06; // U/S and R/W
#if BX_SUPPORT_GLOBAL_PAGES
      if (BX_CPU_THIS_PTR cr4.get_PGE()) {
        combined_access |= (pte & TLB_GlobalPage); // G
      }
#endif
#endif

      // Make up the physical page frame address.
      ppf = pte & 0xfffff000;

      priv_index =
#if BX_CPU_LEVEL >= 4
        (BX_CPU_THIS_PTR cr0.wp<<4) |  // bit 4
#endif
        (pl<<3) |                      // bit 3
        (combined_access & 0x06) |     // bit 2,1
        isWrite;                       // bit 0

      if (!priv_check[priv_index]) goto page_fault_access;

#if (BX_CPU_LEVEL >= 6)
      // update PDE if A bit was not set before
      if ( !(pde & 0x20) ) {
        pde |= 0x20;
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pde_addr, 4, &pde);
      }
#endif

      // Update PTE if A/D bits if needed.
      if (((pte & 0x20)==0) || (isWrite && ((pte&0x40)==0))) 
      {
        pte |= (0x20 | (isWrite<<6)); // Update A and possibly D bits
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, pte_addr, 4, &pte);
      }
    }
  }

  // Calculate physical memory address and fill in TLB cache entry
  paddress = ppf | poffset;
#if BX_USE_TLB
  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = BX_TLB_LPF_VALUE(lpf);
  BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf = ppf;
#endif

// b3: Write User  OK
// b2: Write Sys   OK
// b1: Read  User  OK
// b0: Read  Sys   OK
  if ( combined_access & 4 ) { // User
    // User priv; read from {user,sys} OK.
    accessBits = (TLB_ReadUserOK | TLB_ReadSysOK);
    if ( isWrite )  // Current operation is a write (Dirty bit updated)
    {
      if (combined_access & 2) {
         // R/W access from {user,sys} OK.
        accessBits |= (TLB_WriteUserOK | TLB_WriteSysOK);
      }
      else {
        accessBits |= TLB_WriteSysOK; // read only page, only {sys} write allowed
      }
    }
  }
  else { // System
    accessBits = TLB_ReadSysOK;     // System priv; read from {sys} OK.
    if ( isWrite ) {     // Current operation is a write (Dirty bit updated)
      accessBits |= TLB_WriteSysOK; // write from {sys} OK.
    }
  }
#if BX_SUPPORT_GLOBAL_PAGES
  accessBits |= combined_access & TLB_GlobalPage; // Global bit
#endif
#if BX_USE_TLB
#if BX_SupportGuest2HostTLB
  // Attempt to get a host pointer to this physical page. Put that
  // pointer in the TLB cache. Note if the request is vetoed, NULL
  // will be returned, and it's OK to OR zero in anyways.
  BX_CPU_THIS_PTR TLB.entry[TLB_index].hostPageAddr =
    (bx_hostpageaddr_t) BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS, A20ADDR(ppf), rw);

  if (BX_CPU_THIS_PTR TLB.entry[TLB_index].hostPageAddr) {
    // All access allowed also via direct pointer
    accessBits |= (accessBits & 0xf0) >> 4; 
  }
#endif
  BX_CPU_THIS_PTR TLB.entry[TLB_index].accessBits = accessBits;
#endif

  return(paddress);

// error checking order - page not present, reserved bits, protection
#define ERROR_NOT_PRESENT       0x00
#define ERROR_PROTECTION        0x01
#define ERROR_RESERVED          0x08
#define ERROR_CODE_ACCESS       0x10

#if BX_SUPPORT_X86_64 // keep compiler happy
page_fault_reserved:
  error_code |= ERROR_RESERVED;   // RSVD = 1
#endif

page_fault_access:
  error_code |= ERROR_PROTECTION; // P = 1

page_fault_not_present:
  error_code |= (pl << 2) | (isWrite << 1);
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR msr.nxe && (access_type == CODE_ACCESS))
    error_code |= ERROR_CODE_ACCESS; // I/D = 1
#endif
  BX_CPU_THIS_PTR cr2 = laddr;
  // Invalidate TLB entry.
#if BX_USE_TLB
  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = BX_INVALID_TLB_ENTRY;
#endif
#if BX_EXTERNAL_DEBUGGER
#if BX_SUPPORT_X86_64
  printf("page fault for address %08x%08x @ %08x%08x\n",
               (Bit32u)(laddr >> 32),(Bit32u)(laddr & 0xffffffff),
               (Bit32u)(RIP >> 32),(Bit32u)(RIP & 0xffffffff));
#else
  printf("page fault for address %08x:%08x\n",
               (Bit32u)(laddr >> 32),(Bit32u)(laddr & 0xffffffff));
#endif
#endif
  exception(BX_PF_EXCEPTION, error_code, 0);
  return(0); // keep compiler happy
}

  Bit32u BX_CPP_AttrRegparmN(3)
BX_CPU_C::dtranslate_linear(bx_address laddr, unsigned pl, unsigned rw)
{
  return translate_linear(laddr, pl, rw, DATA_ACCESS);
}

  Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::itranslate_linear(bx_address laddr, unsigned pl)
{
  return translate_linear(laddr, pl, BX_READ, CODE_ACCESS);
}

#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION || BX_GDBSTUB

void BX_CPU_C::dbg_xlate_linear2phy(bx_address laddr, Bit32u *phy, bx_bool *valid)
{
  bx_address lpf, poffset, paddress;

  if (BX_CPU_THIS_PTR cr0.pg == 0) {
    *phy = laddr;
    *valid = 1;
    return;
  }

  lpf       = laddr & BX_CONST64(0xfffffffffffff000); // linear page frame
  poffset   = laddr & 0x00000fff; // physical offset

  // see if page is in the TLB first
#if BX_USE_TLB
  Bit32u TLB_index = BX_TLB_INDEX_OF(lpf);
  bx_TLB_entry *tlbEntry  = &BX_CPU_THIS_PTR TLB.entry[TLB_index];

  if (tlbEntry->lpf == BX_TLB_LPF_VALUE(lpf)) {
    paddress = tlbEntry->ppf | poffset;
    *phy = paddress;
    *valid = 1;
    return;
  }
#endif

#if BX_SUPPORT_PAE
  if (BX_CPU_THIS_PTR cr4.get_PAE()) {
    Bit64u pt_address;
    int levels = 3;
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR msr.lme)
      levels = 4;
#endif
    pt_address = BX_CPU_THIS_PTR cr3_masked;
    Bit64u offset_mask = 0xfff;
    for (int level = levels - 1; level >= 0; --level) {
      Bit64u pte;
      pt_address += 8 * ((laddr >> (12 + 9*level)) & 511);
      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pt_address, 8, &pte);
      if (!(pte & 1))
	goto page_fault;
      pt_address = pte & BX_CONST64(0x000ffffffffff000);
      if (level == 1 && (pte & 0x80)) { // PSE page
	offset_mask = 0x1fffff;
	break;
      }
    }
    paddress = pt_address + (laddr & offset_mask);
  } 
  else   // not PAE
#endif
  {
    Bit32u pt_address = BX_CPU_THIS_PTR cr3_masked;
    Bit32u offset_mask = 0xfff;
    for (int level = 1; level >= 0; --level) {
      Bit32u pte;
      pt_address += 4 * ((laddr >> (12 + 10*level)) & 1023);
      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, pt_address, 4, &pte);
      if (!(pte & 1))
	goto page_fault;
      pt_address = pte & 0xfffff000;
      if (level == 1 && (pte & 0x80)) { // PSE page
	offset_mask = 0x3fffff;
	break;
      }
    }
    paddress = pt_address + (laddr & offset_mask);
  }

  *phy = paddress;
  *valid = 1;
  return;

page_fault:
  *phy = 0;
  *valid = 0;
  return;
}
#endif

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::access_linear(bx_address laddr, unsigned length, unsigned pl,
    unsigned rw, void *data)
{

#if BX_X86_DEBUGGER
  if (BX_CPU_THIS_PTR dr7 & 0x000000ff) {
    // Only compare debug registers if any breakpoints are enabled
    Bit32u dr6_bits;
    unsigned opa, opb;
    opa = BX_HWDebugMemRW; // Read or Write always compares vs 11b
    if (rw==BX_READ) // only compares vs 11b
      opb = opa;
    else // BX_WRITE or BX_RW; also compare vs 01b
      opb = BX_HWDebugMemW;
    dr6_bits = hwdebug_compare(laddr, length, opa, opb);
    if (dr6_bits) {
      BX_CPU_THIS_PTR debug_trap |= dr6_bits;
      BX_CPU_THIS_PTR async_event = 1;
    }
  }
#endif

  Bit32u pageOffset = laddr & 0x00000fff;
  unsigned xlate_rw = rw;
  if (rw==BX_RW) rw = BX_READ;

  if (BX_CPU_THIS_PTR cr0.pg) {
    /* check for reference across multiple pages */
    if ( (pageOffset + length) <= 4096 ) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.pages     = 1;

      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr, BX_CPU_THIS_PTR address_xlation.paddress1, length);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1, length, data);
      }
      else {
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr, BX_CPU_THIS_PTR address_xlation.paddress1, length);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1, length, data);
      }
      return;
    }
    else {
      // access across 2 pages, no need to check for Local APIC here,
      // correct Local APIC access always 16-byte aligned and 
      // maximum 4-byte length so it cannot page split.
      BX_CPU_THIS_PTR address_xlation.paddress1 =
          dtranslate_linear(laddr, pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = length -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      BX_CPU_THIS_PTR address_xlation.paddress2 =
          dtranslate_linear(laddr + BX_CPU_THIS_PTR address_xlation.len1,
                            pl, xlate_rw);

#ifdef BX_LITTLE_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
                             BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
                             BX_CPU_THIS_PTR address_xlation.len2,
                             ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      }
      else {
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
                              BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
                              BX_CPU_THIS_PTR address_xlation.len2,
                              ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      }

#else // BX_BIG_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
                             BX_CPU_THIS_PTR address_xlation.len1,
                             ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
                             BX_CPU_THIS_PTR address_xlation.len2, data);
      }
      else {
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress1,
                              BX_CPU_THIS_PTR address_xlation.len1,
                              ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, BX_CPU_THIS_PTR address_xlation.paddress2,
                              BX_CPU_THIS_PTR address_xlation.len2, data);
      }
#endif
      return;
    }
  }
  else {
    // Paging off.
    if ( (pageOffset + length) <= 4096 ) {
      // Access within single page.
      BX_CPU_THIS_PTR address_xlation.paddress1 = laddr;
      BX_CPU_THIS_PTR address_xlation.pages     = 1;
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr, laddr, length);
#if BX_SupportGuest2HostTLB
        Bit32u tlbIndex = BX_TLB_INDEX_OF(laddr);
        bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
        Bit32u lpf = laddr & 0xfffff000;

        if (tlbEntry->lpf == BX_TLB_LPF_VALUE(lpf)) {
          BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, laddr, length, data);
          return;
        }
        // We haven't seen this page, or it's been bumped before.

        tlbEntry->lpf = BX_TLB_LPF_VALUE(lpf);
        tlbEntry->ppf = lpf;
        // Request a direct write pointer so we can do either R or W.
        tlbEntry->hostPageAddr = (bx_hostpageaddr_t)
            BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS, A20ADDR(lpf), BX_WRITE);

        if (! tlbEntry->hostPageAddr) {
          // Direct write vetoed.  Try requesting only direct reads.
          tlbEntry->hostPageAddr = (bx_hostpageaddr_t)
              BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS, A20ADDR(lpf), BX_READ);
          if (tlbEntry->hostPageAddr) {
            // Got direct read pointer OK.
            tlbEntry->accessBits =
              (TLB_ReadSysOK | TLB_ReadUserOK | TLB_ReadSysPtrOK | TLB_ReadUserPtrOK);
          }
          else
            tlbEntry->accessBits = 0;
        }
        else {
          // Got direct write pointer OK.  Mark for any operation to succeed.
          tlbEntry->accessBits =(TLB_ReadSysOK | TLB_ReadUserOK | TLB_WriteSysOK | TLB_WriteUserOK |
            TLB_ReadSysPtrOK | TLB_ReadUserPtrOK | TLB_WriteSysPtrOK | TLB_WriteUserPtrOK);
        }
#endif  // BX_SupportGuest2HostTLB

        // Let access fall through to the following for this iteration.
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, laddr, length, data);
      }
      else { // Write
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr, laddr, length);
#if BX_SupportGuest2HostTLB
        Bit32u tlbIndex = BX_TLB_INDEX_OF(laddr);
        bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[tlbIndex];
        Bit32u lpf = laddr & 0xfffff000;

        if (tlbEntry->lpf == BX_TLB_LPF_VALUE(lpf)) {
          BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, laddr, length, data);
          return;
        }
        // We haven't seen this page, or it's been bumped before.

        tlbEntry->lpf = BX_TLB_LPF_VALUE(lpf);
        tlbEntry->ppf = lpf;
        // TLB.entry[tlbIndex].ppf field not used for PG==0.
        // Request a direct write pointer so we can do either R or W.
        tlbEntry->hostPageAddr = (bx_hostpageaddr_t)
            BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS, A20ADDR(lpf), BX_WRITE);

        if (tlbEntry->hostPageAddr) {
          // Got direct write pointer OK.  Mark for any operation to succeed.
          tlbEntry->accessBits = (TLB_ReadSysOK | TLB_ReadUserOK | TLB_WriteSysOK | TLB_WriteUserOK |
            TLB_ReadSysPtrOK | TLB_ReadUserPtrOK | TLB_WriteSysPtrOK | TLB_WriteUserPtrOK);
        }
        else
          tlbEntry->accessBits = 0;
#endif  // BX_SupportGuest2HostTLB

        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, laddr, length, data);
      }
    }
    else {
      // access across 2 pages, no need to check for Local APIC here,
      // correct Local APIC access always 16-byte aligned and 
      // maximum 4-byte length so it cannot page split.
      BX_CPU_THIS_PTR address_xlation.paddress1 = laddr;
      BX_CPU_THIS_PTR address_xlation.len1 = 4096 - pageOffset;
      BX_CPU_THIS_PTR address_xlation.len2 = length -
          BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;
      BX_CPU_THIS_PTR address_xlation.paddress2 = laddr +
          BX_CPU_THIS_PTR address_xlation.len1;

#ifdef BX_LITTLE_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1,
            BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress2,
            BX_CPU_THIS_PTR address_xlation.len2,
            ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      }
      else {
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1,
            BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
            BX_CPU_THIS_PTR address_xlation.paddress2,
            BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress2,
            BX_CPU_THIS_PTR address_xlation.len2,
            ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
      }

#else // BX_BIG_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1,
            BX_CPU_THIS_PTR address_xlation.len1,
            ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_READ(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress2,
            BX_CPU_THIS_PTR address_xlation.len2, data);
      }
      else {
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress1,
            BX_CPU_THIS_PTR address_xlation.len1,
            ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_WRITE(BX_CPU_ID, laddr + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS,
            BX_CPU_THIS_PTR address_xlation.paddress2,
            BX_CPU_THIS_PTR address_xlation.len2, data);
      }
#endif
    }
  }
}

#else   // BX_SUPPORT_PAGING

// stub functions for non-support of paging

void BX_CPU_C::CR3_change(bx_address value32)
{
  BX_INFO(("CR3_change(): flush TLB cache"));
  BX_INFO(("Page Directory Base %08x", (unsigned) value32));
}

void BX_CPU_C::access_linear(Bit32u laddr, unsigned length, unsigned pl,
    unsigned rw, void *data)
{
  /* perhaps put this check before all code which calls this function,
   * so we don't have to here
   */
  if (BX_CPU_THIS_PTR cr0.pg == 0) {
    if (rw == BX_READ)
      BX_CPU_THIS_PTR mem->readPhysicalPage(BX_CPU_THIS, laddr, length, data);
    else
      BX_CPU_THIS_PTR mem->writePhysicalPage(BX_CPU_THIS, laddr, length, data);
    return;
  }

  BX_PANIC(("access_linear: paging not supported"));
}

void BX_CPU_C::INVLPG(bxInstruction_c* i)
{}

#endif  // BX_SUPPORT_PAGING
