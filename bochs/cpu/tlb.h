/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2015-2019 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_TLB_H
#define BX_TLB_H

#if BX_SUPPORT_X86_64
const bx_address LPF_MASK = BX_CONST64(0xfffffffffffff000);
#else
const bx_address LPF_MASK = 0xfffff000;
#endif

#if BX_PHY_ADDRESS_LONG
const bx_phy_address PPF_MASK = BX_CONST64(0xfffffffffffff000);
#else
const bx_phy_address PPF_MASK = 0xfffff000;
#endif

BX_CPP_INLINE Bit32u PAGE_OFFSET(bx_address laddr)
{
  return Bit32u(laddr) & 0xfff;
}

BX_CPP_INLINE bx_address LPFOf(bx_address laddr) { return laddr & LPF_MASK; }
BX_CPP_INLINE bx_address PPFOf(bx_phy_address paddr) { return paddr & PPF_MASK; }

BX_CPP_INLINE bx_address AlignedAccessLPFOf(bx_address laddr, unsigned alignment_mask)
{
  return laddr & (LPF_MASK | alignment_mask);
}

// BX_TLB_INDEX_OF(lpf): This macro is passed the linear page frame
//   (top bits of the linear address).  It must map these bits to
//   one of the TLB cache slots, given the size of BX_TLB_SIZE.
//   There will be a many-to-one mapping to each TLB cache slot.
//   When there are collisions, the old entry is overwritten with
//   one for the newest access.
#define BX_DTLB_ENTRY_OF(lpf, len) (BX_CPU_THIS_PTR DTLB.get_entry_of((lpf), (len)))
#define BX_DTLB_INDEX_OF(lpf, len) (BX_CPU_THIS_PTR DTLB.get_index_of((lpf), (len)))

#define BX_ITLB_ENTRY_OF(lpf) (BX_CPU_THIS_PTR ITLB.get_entry_of(lpf))
#define BX_ITLB_INDEX_OF(lpf) (BX_CPU_THIS_PTR ITLB.get_index_of(lpf))

typedef bx_ptr_equiv_t bx_hostpageaddr_t;

#if BX_SUPPORT_X86_64
const bx_address BX_INVALID_TLB_ENTRY = BX_CONST64(0xffffffffffffffff);
#else
const bx_address BX_INVALID_TLB_ENTRY = 0xffffffff;
#endif

// accessBits in DTLB
const Bit32u TLB_SysReadOK   = 0x01;
const Bit32u TLB_UserReadOK  = 0x02;
const Bit32u TLB_SysWriteOK  = 0x04;
const Bit32u TLB_UserWriteOK = 0x08;

const Bit32u TLB_SysReadShadowStackOK   = 0x10;
const Bit32u TLB_UserReadShadowStackOK  = 0x20;
const Bit32u TLB_SysWriteShadowStackOK  = 0x40;
const Bit32u TLB_UserWriteShadowStackOK = 0x80;

// accessBits in ITLB
const Bit32u TLB_SysExecuteOK  = 0x01;
const Bit32u TLB_UserExecuteOK = 0x02;
// global
const Bit32u TLB_GlobalPage    = 0x80000000;

#if BX_SUPPORT_PKEYS

// check if page from a TLB entry can be written
#define isWriteOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x04 << (user)) & BX_CPU_THIS_PTR wr_pkey[tlbEntry->pkey])

// check if page from a TLB entry can be read
#define isReadOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x01 << (user)) & BX_CPU_THIS_PTR rd_pkey[tlbEntry->pkey])

#if BX_SUPPORT_CET
// check if page from a TLB entry can be written for shadow stack access
#define isShadowStackWriteOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x40 << (user)) & BX_CPU_THIS_PTR wr_pkey[tlbEntry->pkey])

// check if page from a TLB entry can be read
#define isShadowStackReadOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x10 << (user)) & BX_CPU_THIS_PTR rd_pkey[tlbEntry->pkey])
#endif

#else // ! BX_SUPPORT_PKEYS

// check if page from a TLB entry can be written
#define isWriteOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x04 << (user)))

// check if page from a TLB entry can be read
#define isReadOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x01 << (user)))

#if BX_SUPPORT_CET
// check if page from a TLB entry can be written for shadow stack access
#define isShadowStackWriteOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x40 << (user)))

// check if page from a TLB entry can be read
#define isShadowStackReadOK(tlbEntry, user) \
  (tlbEntry->accessBits & (0x10 << (user)))
#endif

#endif

enum {
  BX_MEMTYPE_UC = 0,
  BX_MEMTYPE_WC = 1,
  BX_MEMTYPE_RESERVED2 = 2,
  BX_MEMTYPE_RESERVED3 = 3,
  BX_MEMTYPE_WT = 4,
  BX_MEMTYPE_WP = 5,
  BX_MEMTYPE_WB = 6,
  BX_MEMTYPE_UC_WEAK = 7, // PAT only
  BX_MEMTYPE_INVALID = 8
};

typedef unsigned BxMemtype;

// avoid wasting cycles to determine memory type if not required
#if BX_SUPPORT_MEMTYPE
  #define MEMTYPE(memtype) (memtype)
#else
  #define MEMTYPE(memtype) (BX_MEMTYPE_UC)
#endif

struct bx_TLB_entry
{
  bx_address lpf;       // linear page frame
  bx_phy_address ppf;   // physical page frame
  bx_hostpageaddr_t hostPageAddr;
  Bit32u accessBits;
#if BX_SUPPORT_PKEYS
  Bit32u pkey;
#endif
  Bit32u lpf_mask;      // linear address mask of the page size
#if BX_SUPPORT_MEMTYPE
  Bit32u memtype;       // keep it Bit32u for alignment
#endif

  bx_TLB_entry() { invalidate(); }

  BX_CPP_INLINE bx_bool valid() const { return lpf != BX_INVALID_TLB_ENTRY; }

  BX_CPP_INLINE void invalidate() {
    lpf = BX_INVALID_TLB_ENTRY;
    accessBits = 0;
  }

  BX_CPP_INLINE Bit32u get_memtype() const { return MEMTYPE(memtype); }
};

template <unsigned size>
struct TLB {
  bx_TLB_entry entry[size];
#if BX_CPU_LEVEL >= 5
  bx_bool split_large;
#endif

public:
  TLB() { flush(); }

  BX_CPP_INLINE unsigned get_index_of(bx_address lpf, unsigned len = 0)
  {
    const Bit32u tlb_mask = ((size-1) << 12);
    return (((unsigned(lpf) + len) & tlb_mask) >> 12);
  }

  BX_CPP_INLINE bx_TLB_entry *get_entry_of(bx_address lpf, unsigned len = 0)
  {
    return &entry[get_index_of(lpf, len)];
  }

  BX_CPP_INLINE void flush(void)
  {
    for (unsigned n=0; n < size; n++)
      entry[n].invalidate();

#if BX_CPU_LEVEL >= 5
    split_large = false;  // flushing whole TLB
#endif
  }

#if BX_CPU_LEVEL >= 6
  BX_CPP_INLINE void flushNonGlobal(void)
  {
    Bit32u lpf_mask = 0;

    for (unsigned n=0; n<size; n++) {
      bx_TLB_entry *tlbEntry = &entry[n];
      if (tlbEntry->valid()) {
        if (!(tlbEntry->accessBits & TLB_GlobalPage))
          tlbEntry->invalidate();
        else
          lpf_mask |= tlbEntry->lpf_mask;
      }
    }

    split_large = (lpf_mask > 0xfff);
  }
#endif

  BX_CPP_INLINE void invlpg(bx_address laddr)
  {
#if BX_CPU_LEVEL >= 5
    if (split_large) {
      Bit32u lpf_mask = 0;

      // make sure INVLPG handles correctly large pages
      for (unsigned n=0; n<size; n++) {
        bx_TLB_entry *tlbEntry = &entry[n];
        if (tlbEntry->valid()) {
          bx_address entry_lpf_mask = tlbEntry->lpf_mask;
          if ((laddr & ~entry_lpf_mask) == (tlbEntry->lpf & ~entry_lpf_mask)) {
            tlbEntry->invalidate();
          }
          else {
            lpf_mask |= entry_lpf_mask;
          }
        }
      }

      split_large = (lpf_mask > 0xfff);
    }
    else
#endif
    {
      bx_TLB_entry *tlbEntry = get_entry_of(laddr);
      if (LPFOf(tlbEntry->lpf) == LPFOf(laddr))
        tlbEntry->invalidate();
    }
  }
};

#endif
