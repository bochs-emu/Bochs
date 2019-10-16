/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2019  The Bochs Project
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
#define LOG_THIS BX_CPU_THIS_PTR

//
// Repeat Speedups methods
//

#if BX_SUPPORT_REPEAT_SPEEDUPS
Bit32u BX_CPU_C::FastRepMOVSB(unsigned srcSeg, Bit32u srcOff, unsigned dstSeg, Bit32u dstOff, Bit32u byteCount, Bit32u granularity)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  bx_address laddrSrc, laddrDst;

  bx_segment_reg_t *srcSegPtr = &BX_CPU_THIS_PTR sregs[srcSeg];
  if (srcSegPtr->cache.valid & SegAccessROK4G) {
    laddrSrc = srcOff;
  }
  else {
    if (!(srcSegPtr->cache.valid & SegAccessROK))
      return 0;
    if ((srcOff | 0xfff) > srcSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrSrc = get_laddr32(srcSeg, srcOff);
  }

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];
  if (dstSegPtr->cache.valid & SegAccessWOK4G) {
    laddrDst = dstOff;
  }
  else {
    if (!(dstSegPtr->cache.valid & SegAccessWOK))
      return 0;
    if ((dstOff | 0xfff) > dstSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrDst = get_laddr32(dstSeg, dstOff);
  }

  return FastRepMOVSB(laddrSrc, laddrDst, byteCount, granularity);
}

Bit32u BX_CPU_C::FastRepMOVSB(bx_address laddrSrc, bx_address laddrDst, Bit64u byteCount, Bit32u granularity)
{
  Bit8u *hostAddrSrc = v2h_read_byte(laddrSrc, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrSrc) return 0;

  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  assert(! BX_CPU_THIS_PTR get_DF());

  // See how many bytes can fit in the rest of this page.
  Bit32u bytesFitSrc = 0x1000 - PAGE_OFFSET(laddrSrc);
  Bit32u bytesFitDst = 0x1000 - PAGE_OFFSET(laddrDst);

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (byteCount > bytesFitSrc)
    byteCount = bytesFitSrc;
  if (byteCount > bytesFitDst)
    byteCount = bytesFitDst;
  if (byteCount > bx_pc_system.getNumCpuTicksLeftNextEvent())
    byteCount = bx_pc_system.getNumCpuTicksLeftNextEvent();

  byteCount &= ~(granularity-1);

  // If after all the restrictions, there is anything left to do...
  if (byteCount) {
    // Transfer data directly using host addresses
    for (unsigned j=0; j<byteCount; j++) {
      * (Bit8u *) hostAddrDst = * (Bit8u *) hostAddrSrc;
      hostAddrDst++;
      hostAddrSrc++;
    }
  }

  return byteCount;
}

Bit32u BX_CPU_C::FastRepSTOSB(unsigned dstSeg, Bit32u dstOff, Bit8u val, Bit32u count)
{
  bx_address laddrDst;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];
  if (dstSegPtr->cache.valid & SegAccessWOK4G) {
    laddrDst = dstOff;
  }
  else {
    if (!(dstSegPtr->cache.valid & SegAccessWOK))
      return 0;
    if ((dstOff | 0xfff) > dstSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrDst = get_laddr32(dstSeg, dstOff);
  }

  return FastRepSTOSB(laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSB(bx_address laddrDst, Bit8u val, Bit32u count)
{
  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  assert(! BX_CPU_THIS_PTR get_DF());

  // See how many bytes can fit in the rest of this page.
  Bit32u bytesFitDst = 0x1000 - PAGE_OFFSET(laddrDst);

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > bytesFitDst)
    count = bytesFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit8u *) hostAddrDst = val;
      hostAddrDst++;
    }
  }

  return count;
}

Bit32u BX_CPU_C::FastRepSTOSW(unsigned dstSeg, Bit32u dstOff, Bit16u val, Bit32u count)
{
  bx_address laddrDst;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];
  if (dstSegPtr->cache.valid & SegAccessWOK4G) {
    laddrDst = dstOff;
  }
  else {
    if (!(dstSegPtr->cache.valid & SegAccessWOK))
      return 0;
    if ((dstOff | 0xfff) > dstSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrDst = get_laddr32(dstSeg, dstOff);
  }

  return FastRepSTOSW(laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSW(bx_address laddrDst, Bit16u val, Bit32u count)
{
  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  assert(! BX_CPU_THIS_PTR get_DF());

  // See how many words can fit in the rest of this page.
  Bit32u wordsFitDst = (0x1000 - PAGE_OFFSET(laddrDst)) >> 1;

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > wordsFitDst)
    count = wordsFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      WriteHostWordToLittleEndian((Bit16u*)hostAddrDst, val);
      hostAddrDst += 2;
    }
  }

  return count;
}

Bit32u BX_CPU_C::FastRepSTOSD(unsigned dstSeg, Bit32u dstOff, Bit32u val, Bit32u count)
{
  bx_address laddrDst;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];
  if (dstSegPtr->cache.valid & SegAccessWOK4G) {
    laddrDst = dstOff;
  }
  else {
    if (!(dstSegPtr->cache.valid & SegAccessWOK))
      return 0;
    if ((dstOff | 0xfff) > dstSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrDst = get_laddr32(dstSeg, dstOff);
  }

  return FastRepSTOSD(laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSD(bx_address laddrDst, Bit32u val, Bit32u count)
{
  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  assert(! BX_CPU_THIS_PTR get_DF());

  // See how many dwords can fit in the rest of this page.
  Bit32u dwordsFitDst = (0x1000 - PAGE_OFFSET(laddrDst)) >> 2;

  // Restrict dword count to the number that will fit in either
  // source or dest pages.
  if (count > dwordsFitDst)
    count = dwordsFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      WriteHostDWordToLittleEndian((Bit32u*)hostAddrDst, val);
      hostAddrDst += 4;
    }
  }

  return count;
}
#endif
