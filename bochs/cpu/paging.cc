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



#if 0
  // - what should the reserved bits in the error code be ?
  // - move CR0.wp bit in lookup table to cache.  Then dump
  //   cache whenever it is changed.  This eliminates the
  //   extra calculation and shifting.
  // - change BX_READ and BX_WRITE to 0,1 ???
#endif



#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif




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
//   31..12: page base address
//   11.. 9: available
//        8: G (Pentium Pro+), 0=reserved otherwise
//        7: 0=reserved
//        6: D   (386+)
//        5: A   (386+)
//        4: PCD (486+), 0=reserved otherwise
//        3: PWT (486+), 0=reserved otherwise
//        2: U/S (386+)
//        1: R/W (386+)
//        0: P=1 (386+)


// Page Directory/Table Entry Fields Defined:
// ==========================================
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
//   addressing
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
//
// The TLB cache holds the following info, from which the above
// fields can efficiently be extracted to facilitate a privilege check:
//
//   |4 |3 |2 |1 |0 |
//   |  |  |us|rw|D |
//          |  |  |
//          |  |  +---> Dirty bit from PTE (not used for privilege check)
//          +--+------> u/s,r/w combined of page dir & table
//
//
// The rest of the fields are taken from current access parameters
// and the write-protect field:
//
//   |4 |3 |2 |1 |0 |
//   |wp|us|  |  |rw|
//    |  |        |
//    |  |        +---> r/w of current access
//    |  |
//    |  +------------> u/s of current access
//    +---------------> Current CR0.wp value


static unsigned priv_check[BX_PRIV_CHECK_SIZE];



  void
BX_CPU_C::enable_paging(void)
{
  TLB_flush();
  if (bx_dbg.paging) BX_INFO(("enable_paging():\n"));
//BX_DEBUG(( "enable_paging():-------------------------\n" ));
}

  void
BX_CPU_C::disable_paging(void)
{
  TLB_flush();
  if (bx_dbg.paging) BX_INFO(("disable_paging():\n"));
}

  void
BX_CPU_C::CR3_change(Bit32u value32)
{
  if (bx_dbg.paging) {
    BX_INFO(("CR3_change(): flush TLB cache\n"));
    BX_INFO(("Page Directory Base %08x\n", (unsigned) value32));
    }

  // flush TLB even if value does not change
  TLB_flush();
  BX_CPU_THIS_PTR cr3 = value32;
}

  void
BX_CPU_C::TLB_init(void)
{
  // Called to initialize the TLB upon startup.
  // Unconditional initialization of all TLB entries.

#if BX_USE_TLB
  unsigned i;
  unsigned wp, us_combined, rw_combined, us_current, rw_current;

  for (i=0; i<BX_TLB_SIZE; i++) {
    BX_CPU_THIS_PTR TLB.entry[i].lpf = BX_INVALID_TLB_ENTRY;
    }

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

#endif  // #if BX_USE_TLB
}

  void
BX_CPU_C::TLB_flush(void)
{
#if BX_USE_TLB
  for (unsigned i=0; i<BX_TLB_SIZE; i++) {
    BX_CPU_THIS_PTR TLB.entry[i].lpf = BX_INVALID_TLB_ENTRY;
    }
#endif  // #if BX_USE_TLB

  invalidate_prefetch_q();
}

  void
BX_CPU_C::TLB_clear(void)
{
#if BX_USE_TLB
  for (unsigned i=0; i<BX_TLB_SIZE; i++) {
    BX_CPU_THIS_PTR TLB.entry[i].lpf = BX_INVALID_TLB_ENTRY;
    }
#endif  // #if BX_USE_TLB
}

  void
BX_CPU_C::INVLPG(BxInstruction_t* i)
{
#if BX_CPU_LEVEL >= 4
  invalidate_prefetch_q();

  // Operand must not be a register
  if (i->mod == 0xc0) {
    BX_INFO(("INVLPG: op is a register"));
    UndefinedOpcode(i);
    }
  // Can not be executed in v8086 mode
  if (v8086_mode())
    exception(BX_GP_EXCEPTION, 0, 0);

  // Protected instruction: CPL0 only
  if (BX_CPU_THIS_PTR cr0.pe) {
    if (CPL!=0) {
      BX_INFO(("INVLPG: CPL!=0\n"));
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }

#if BX_USE_TLB
  // Just clear the entire TLB, ugh!
  TLB_clear();
#endif // BX_USE_TLB
  BX_INSTR_TLB_CNTRL(BX_INSTR_INVLPG, 0);

#else
  // not supported on < 486
  UndefinedOpcode(i);
#endif
}


// Translate a linear address to a physical address, for
// a data access (D)

  Bit32u
BX_CPU_C::dtranslate_linear(Bit32u laddress, unsigned pl, unsigned rw)
{
  Bit32u   lpf, ppf, poffset, TLB_index, error_code, paddress;
  Bit32u   pde, pde_addr;
  Bit32u   pte, pte_addr;
  unsigned priv_index;
  Boolean  is_rw;
  Bit32u   combined_access, new_combined_access;

  lpf       = laddress & 0xfffff000; // linear page frame
  poffset   = laddress & 0x00000fff; // physical offset
  TLB_index = BX_TLB_INDEX_OF(lpf);


  is_rw = (rw>=BX_WRITE); // write or r-m-w

  if (BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf == lpf) {
    paddress        = BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf | poffset;
    combined_access = BX_CPU_THIS_PTR TLB.entry[TLB_index].combined_access;
priv_check:
    priv_index =
#if BX_CPU_LEVEL >= 4
      (BX_CPU_THIS_PTR cr0.wp<<4) |  // bit 4
#endif
      (pl<<3) |                      // bit 3
      (combined_access & 0x06) |     // bit 2,1
      is_rw;                         // bit 0

    if (priv_check[priv_index]) {
      // Operation has proper privilege.
      // See if A/D bits need updating.
      //BW !! a read access does not do any updates, patched load
      new_combined_access = combined_access | is_rw;
      if (new_combined_access == combined_access) {
        // A/D bits already up-to-date
        return(paddress);
        }

      // A/D bits need updating first
      BX_CPU_THIS_PTR TLB.entry[TLB_index].combined_access = new_combined_access;
      pte_addr = BX_CPU_THIS_PTR TLB.entry[TLB_index].pte_addr;
      BX_CPU_THIS_PTR mem->read_physical(this, pte_addr, 4, &pte); // get old PTE
      pte |= 0x20 | (is_rw << 6);
      BX_CPU_THIS_PTR mem->write_physical(this, pte_addr, 4, &pte); // write updated PTE
      return(paddress);
      }

    // Protection violation
    error_code = 0xfffffff9; // RSVD=1, P=1
    goto page_fault_check;
    }

  // Get page dir entry
  pde_addr = (BX_CPU_THIS_PTR cr3 & 0xfffff000) |
             ((laddress & 0xffc00000) >> 20);
  BX_CPU_THIS_PTR mem->read_physical(this, pde_addr, 4, &pde);
  if ( !(pde & 0x01) ) {
    // Page Directory Entry NOT present
    error_code = 0xfffffff8; // RSVD=1, P=0
    goto page_fault_not_present;
    }

  // Get page table entry
  pte_addr = (pde & 0xfffff000) |
             ((laddress & 0x003ff000) >> 10);
  BX_CPU_THIS_PTR mem->read_physical(this, pte_addr, 4, &pte);

  // update PDE if A bit was not set before
  if ( !(pde & 0x20) ) {
    pde |= 0x20;
    BX_CPU_THIS_PTR mem->write_physical(this, pde_addr, 4, &pde);
    }

  if ( !(pte & 0x01) ) {
    // Page Table Entry NOT present
    error_code = 0xfffffff8; // RSVD=1, P=0
    goto page_fault_not_present;
    }

  //BW added: update PTE if A bit was not set before
  if ( !(pte & 0x20) ) {
    pte |= 0x20;
    BX_CPU_THIS_PTR mem->write_physical(this, pte_addr, 4, &pte);
    }

  // 386 and 486+ have different bahaviour for combining
  // privilege from PDE and PTE.
#if BX_CPU_LEVEL == 3
  combined_access  = (pde | pte) & 0x04; // U/S
  combined_access |= (pde & pte) & 0x02; // R/W
#else // 486+
  combined_access  = (pde & pte) & 0x06; // U/S and R/W
#endif

  ppf = pte & 0xfffff000;
  paddress = ppf | poffset;

  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = lpf;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf = ppf;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].pte_addr = pte_addr;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].combined_access = combined_access;
  goto priv_check;


page_fault_check:
// (mch) Define RMW_WRITES for old behavior
#if !defined(RMW_WRITES)
  /* (mch) Ok, so we know it's a page fault. It the access is a
     read-modify-write access we check if the read faults, if it
     does then we (optionally) do not set the write bit */
  if (rw == BX_RW) {
          priv_index =
#if BX_CPU_LEVEL >= 4
                  (BX_CPU_THIS_PTR cr0.wp<<4) |               // bit 4
#endif
                  (pl<<3) |                   // bit 3
                  (combined_access & 0x06) |  // bit 2,1
                  0;                      // bit 0 (read)
          if (!priv_check[priv_index]) {
                  // Fault on read
                  is_rw = 0;
          }
  }
#endif /* RMW_WRITES */
  goto page_fault_proper;

page_fault_not_present:
#if !defined(RMW_WRITES)
  if (rw == BX_RW)
          is_rw = 0;
#endif /* RMW_WRITES */
  goto page_fault_proper;

  page_fault_proper:
  error_code |= (pl << 2) | (is_rw << 1);
  BX_CPU_THIS_PTR cr2 = laddress;
  // invalidate entry - we can get away without maintaining A bit in PTE
  // if we don't maintain TLB entries without it set.
  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = BX_INVALID_TLB_ENTRY;
  exception(BX_PF_EXCEPTION, error_code, 0);
  return(0); // keep compiler happy
}


// Translate a linear address to a physical address, for
// an instruction fetch access (I)

  Bit32u
BX_CPU_C::itranslate_linear(Bit32u laddress, unsigned pl)
{
  Bit32u   lpf, ppf, poffset, TLB_index, error_code, paddress;
  Bit32u   pde, pde_addr;
  Bit32u   pte, pte_addr;
  unsigned priv_index;
  Bit32u   combined_access;

  lpf       = laddress & 0xfffff000; // linear page frame
  poffset   = laddress & 0x00000fff; // physical offset
  TLB_index = BX_TLB_INDEX_OF(lpf);


  if (BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf == lpf) {
    paddress        = BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf | poffset;
    combined_access = BX_CPU_THIS_PTR TLB.entry[TLB_index].combined_access;
priv_check:
    priv_index =
#if BX_CPU_LEVEL >= 4
      (BX_CPU_THIS_PTR cr0.wp<<4) |   // bit 4
#endif
      (pl<<3) |                       // bit 3
      (combined_access & 0x06);       // bit 2,1
                                      // bit 0 == 0

    if (priv_check[priv_index]) {
      // Operation has proper privilege.
      return(paddress);
      }

    // Protection violation
    error_code = 0xfffffff9; // RSVD=1, P=1
    goto page_fault;
    }

  // Get page dir entry
  pde_addr = (BX_CPU_THIS_PTR cr3 & 0xfffff000) |
             ((laddress & 0xffc00000) >> 20);
  BX_CPU_THIS_PTR mem->read_physical(this, pde_addr, 4, &pde);
  if ( !(pde & 0x01) ) {
    // Page Directory Entry NOT present
    error_code = 0xfffffff8; // RSVD=1, P=0
    goto page_fault;
    }

  // Get page table entry
  pte_addr = (pde & 0xfffff000) |
             ((laddress & 0x003ff000) >> 10);
  BX_CPU_THIS_PTR mem->read_physical(this, pte_addr, 4, &pte);

  // update PDE if A bit was not set before
  if ( !(pde & 0x20) ) {
    pde |= 0x20;
    BX_CPU_THIS_PTR mem->write_physical(this, pde_addr, 4, &pde);
    }

  if ( !(pte & 0x01) ) {
    // Page Table Entry NOT present
    error_code = 0xfffffff8; // RSVD=1, P=0
    goto page_fault;
    }

  //BW added: update PTE if A bit was not set before
  if ( !(pte & 0x20) ) {
    pte |= 0x20;
    BX_CPU_THIS_PTR mem->write_physical(this, pte_addr, 4, &pte);
    }

  // 386 and 486+ have different bahaviour for combining
  // privilege from PDE and PTE.
#if BX_CPU_LEVEL == 3
  combined_access  = (pde | pte) & 0x04; // U/S
  combined_access |= (pde & pte) & 0x02; // R/W
#else // 486+
  combined_access  = (pde & pte) & 0x06; // U/S and R/W
#endif

  ppf = pte & 0xfffff000;
  paddress = ppf | poffset;

  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = lpf;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf = ppf;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].pte_addr = pte_addr;
  BX_CPU_THIS_PTR TLB.entry[TLB_index].combined_access = combined_access;
  goto priv_check;


page_fault:
  error_code |= (pl << 2);
  BX_CPU_THIS_PTR cr2 = laddress;
  // invalidate entry - we can get away without maintaining A bit in PTE
  // if we don't maintain TLB entries without it set.
  BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf = BX_INVALID_TLB_ENTRY;
  exception(BX_PF_EXCEPTION, error_code, 0);
  return(0); // keep compiler happy
}


#if BX_DEBUGGER || BX_DISASM || BX_INSTRUMENTATION

  void
BX_CPU_C::dbg_xlate_linear2phy(Bit32u laddress, Bit32u *phy, Boolean *valid)
{
  Bit32u   lpf, ppf, poffset, TLB_index, paddress;
  Bit32u   pde, pde_addr;
  Bit32u   pte, pte_addr;

  if (BX_CPU_THIS_PTR cr0.pg == 0) {
    *phy = laddress;
    *valid = 1;
    return;
    }

  lpf       = laddress & 0xfffff000; // linear page frame
  poffset   = laddress & 0x00000fff; // physical offset
  TLB_index = BX_TLB_INDEX_OF(lpf);

  // see if page is in the TLB first
  if (BX_CPU_THIS_PTR TLB.entry[TLB_index].lpf == lpf) {
    paddress        = BX_CPU_THIS_PTR TLB.entry[TLB_index].ppf | poffset;
    *phy = paddress;
    *valid = 1;
    return;
    }

  // Get page dir entry
  pde_addr = (BX_CPU_THIS_PTR cr3 & 0xfffff000) |
             ((laddress & 0xffc00000) >> 20);
  BX_CPU_THIS_PTR mem->read_physical(this, pde_addr, 4, &pde);
  if ( !(pde & 0x01) ) {
    // Page Directory Entry NOT present
    goto page_fault;
    }

  // Get page table entry
  pte_addr = (pde & 0xfffff000) |
             ((laddress & 0x003ff000) >> 10);
  BX_CPU_THIS_PTR mem->read_physical(this, pte_addr, 4, &pte);
  if ( !(pte & 0x01) ) {
    // Page Table Entry NOT present
    goto page_fault;
    }

  ppf = pte & 0xfffff000;
  paddress = ppf | poffset;

  *phy = paddress;
  *valid = 1;
  return;

page_fault:
  *phy = 0;
  *valid = 0;
  return;
}
#endif



  void
BX_CPU_C::access_linear(Bit32u laddress, unsigned length, unsigned pl,
    unsigned rw, void *data)
{
  Bit32u mod4096;
  unsigned xlate_rw;


#if BX_X86_DEBUGGER
  if ( BX_CPU_THIS_PTR dr7 & 0x000000ff ) {
    // Only compare debug registers if any breakpoints are enabled
    Bit32u dr6_bits;
    unsigned opa, opb;
    opa = BX_HWDebugMemRW; // Read or Write always compares vs 11b
    if (rw==BX_READ) // only compares vs 11b
      opb = opa;
    else // BX_WRITE or BX_RW; also compare vs 01b
      opb = BX_HWDebugMemW;
    dr6_bits = hwdebug_compare(laddress, length, opa, opb);
    if (dr6_bits) {
      BX_CPU_THIS_PTR debug_trap |= dr6_bits;
      BX_CPU_THIS_PTR async_event = 1;
      }
    }
#endif

  if (rw==BX_RW) {
    xlate_rw = BX_RW;
    rw = BX_READ;
    }
  else {
    xlate_rw = rw;
    }


  // perhaps put this check before all code which calls this function,
  // so we don't have to here

  if (BX_CPU_THIS_PTR cr0.pg) {
    /* check for reference across multiple pages */
    mod4096 = laddress & 0x00000fff;
    if ( (mod4096 + length) <= 4096 ) {
      // Bit32u paddress1;

      /* access within single page */
      BX_CPU_THIS_PTR address_xlation.paddress1 = dtranslate_linear(laddress, pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.pages     = 1;

      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(laddress, BX_CPU_THIS_PTR address_xlation.paddress1, length);
        BX_CPU_THIS_PTR mem->read_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1, length, data);
        }
      else {
        BX_INSTR_LIN_WRITE(laddress, BX_CPU_THIS_PTR address_xlation.paddress1, length);
        BX_CPU_THIS_PTR mem->write_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1, length, data);
        }
      return;
      }
    else {
      // access across 2 pages
      BX_CPU_THIS_PTR address_xlation.paddress1 = dtranslate_linear(laddress, pl, xlate_rw);
      BX_CPU_THIS_PTR address_xlation.len1      = 4096 - mod4096;
      BX_CPU_THIS_PTR address_xlation.len2      = length - BX_CPU_THIS_PTR address_xlation.len1;
      BX_CPU_THIS_PTR address_xlation.pages     = 2;

      BX_CPU_THIS_PTR address_xlation.paddress2 = dtranslate_linear(laddress + BX_CPU_THIS_PTR address_xlation.len1, pl, xlate_rw);

#ifdef BX_LITTLE_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(laddress,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->read_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1,
                             BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_READ(laddress + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->read_physical(this, BX_CPU_THIS_PTR address_xlation.paddress2,
                             BX_CPU_THIS_PTR address_xlation.len2,
                             ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
        }
      else {
        BX_INSTR_LIN_WRITE(laddress,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->write_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1,
                              BX_CPU_THIS_PTR address_xlation.len1, data);
        BX_INSTR_LIN_WRITE(laddress + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->write_physical(this, BX_CPU_THIS_PTR address_xlation.paddress2,
                              BX_CPU_THIS_PTR address_xlation.len2,
                              ((Bit8u*)data) + BX_CPU_THIS_PTR address_xlation.len1);
        }

#else // BX_BIG_ENDIAN
      if (rw == BX_READ) {
        BX_INSTR_LIN_READ(laddress,
                          BX_CPU_THIS_PTR address_xlation.paddress1,
                          BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->read_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1,
                             BX_CPU_THIS_PTR address_xlation.len1,
                             ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_READ(laddress + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->read_physical(this, BX_CPU_THIS_PTR address_xlation.paddress2,
                             BX_CPU_THIS_PTR address_xlation.len2, data);
        }
      else {
        BX_INSTR_LIN_WRITE(laddress,
                           BX_CPU_THIS_PTR address_xlation.paddress1,
                           BX_CPU_THIS_PTR address_xlation.len1);
        BX_CPU_THIS_PTR mem->write_physical(this, BX_CPU_THIS_PTR address_xlation.paddress1,
                              BX_CPU_THIS_PTR address_xlation.len1,
                              ((Bit8u*)data) + (length - BX_CPU_THIS_PTR address_xlation.len1));
        BX_INSTR_LIN_WRITE(laddress + BX_CPU_THIS_PTR address_xlation.len1,
                          BX_CPU_THIS_PTR address_xlation.paddress2,
                          BX_CPU_THIS_PTR address_xlation.len2);
        BX_CPU_THIS_PTR mem->write_physical(this, BX_CPU_THIS_PTR address_xlation.paddress2,
                              BX_CPU_THIS_PTR address_xlation.len2, data);
        }
#endif

      return;
      }
    }
  else {
    // paging off, pass linear address thru to physical
    if (rw == BX_READ) {
      BX_INSTR_LIN_READ(laddress, laddress, length);
      BX_CPU_THIS_PTR mem->read_physical(this, laddress, length, data);
      }
    else {
      BX_INSTR_LIN_WRITE(laddress, laddress, length);
      BX_CPU_THIS_PTR mem->write_physical(this, laddress, length, data);
      }
    return;
    }
}








#else   // BX_SUPPORT_PAGING

// stub functions for non-support of paging
  void
BX_CPU_C::enable_paging(void)
{
  BX_PANIC(("enable_paging(): not implemented\n"));
}

  void
BX_CPU_C::disable_paging(void)
{
  BX_PANIC(("disable_paging() called\n"));
}

  void
BX_CPU_C::CR3_change(Bit32u value32)
{
  BX_INFO(("CR3_change(): flush TLB cache\n"));
  BX_INFO(("Page Directory Base %08x\n", (unsigned) value32));
}


  void
BX_CPU_C::access_linear(Bit32u laddress, unsigned length, unsigned pl,
    unsigned rw, void *data)
{
  /* perhaps put this check before all code which calls this function,
   * so we don't have to here
   */
  if (BX_CPU_THIS_PTR cr0.pg == 0) {
    if (rw == BX_READ)
      BX_CPU_THIS_PTR mem->read_physical(this, laddress, length, data);
    else
      BX_CPU_THIS_PTR mem->write_physical(this, laddress, length, data);
    return;
    }

  BX_PANIC(("access_linear: paging not supported\n"));
}



#endif  // BX_SUPPORT_PAGING
