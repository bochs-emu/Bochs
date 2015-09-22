/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2015 Stanislav Shwartsman
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

// BX_TLB_SIZE: Number of entries in TLB
// BX_TLB_INDEX_OF(lpf): This macro is passed the linear page frame
//   (top 20 bits of the linear address.  It must map these bits to
//   one of the TLB cache slots, given the size of BX_TLB_SIZE.
//   There will be a many-to-one mapping to each TLB cache slot.
//   When there are collisions, the old entry is overwritten with
//   one for the newest access.

#define BX_TLB_SIZE 1024
#define BX_TLB_MASK ((BX_TLB_SIZE-1) << 12)
#define BX_TLB_INDEX_OF(lpf, len) ((((unsigned)(lpf) + (len)) & BX_TLB_MASK) >> 12)

typedef bx_ptr_equiv_t bx_hostpageaddr_t;

#if BX_SUPPORT_X86_64
  #define BX_INVALID_TLB_ENTRY BX_CONST64(0xffffffffffffffff)
#else
  #define BX_INVALID_TLB_ENTRY 0xffffffff
#endif

typedef struct {
  bx_address lpf;       // linear page frame
  bx_phy_address ppf;   // physical page frame
  bx_hostpageaddr_t hostPageAddr;
  Bit32u accessBits;
  Bit32u lpf_mask;      // linear address mask of the page size

#if BX_SUPPORT_MEMTYPE
  Bit32u memtype;      // keep it Bit32u for alignment
#endif

  BX_CPP_INLINE void invalidate() {
    lpf = BX_INVALID_TLB_ENTRY;
    accessBits = 0;
  }

  BX_CPP_INLINE Bit32u get_memtype() const
  {
#if BX_SUPPORT_MEMTYPE
    return memtype;
#else
    return BX_MEMTYPE_UC;
#endif
  }

} bx_TLB_entry;

#if BX_SUPPORT_X86_64
  #define LPF_MASK BX_CONST64(0xfffffffffffff000)
#else
  #define LPF_MASK (0xfffff000)
#endif

#if BX_PHY_ADDRESS_LONG
  #define PPF_MASK BX_CONST64(0xfffffffffffff000)
#else
  #define PPF_MASK (0xfffff000)
#endif

#define PAGE_OFFSET(laddr) ((Bit32u)(laddr) & 0xfff)

BX_CPP_INLINE bx_address LPFOf(bx_address laddr) { return laddr & LPF_MASK; }
BX_CPP_INLINE bx_address PPFOf(bx_phy_address paddr) { return paddr & PPF_MASK; }

BX_CPP_INLINE bx_address AlignedAccessLPFOf(bx_address laddr, unsigned alignment_mask)
{
  return laddr & (LPF_MASK | alignment_mask);
}

#define BX_TLB_ENTRY_OF(lpf, len) (&BX_CPU_THIS_PTR TLB.entry[BX_TLB_INDEX_OF((lpf), (len))])

#endif
