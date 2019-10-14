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
Bit32u BX_CPU_C::FastRepMOVSB(bxInstruction_c *i, unsigned srcSeg, Bit32u srcOff, unsigned dstSeg, Bit32u dstOff, Bit32u count, Bit32u granularity)
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

  return FastRepMOVSB(i, laddrSrc, laddrDst, count, granularity);
}

Bit32u BX_CPU_C::FastRepMOVSB(bxInstruction_c *i, bx_address laddrSrc, bx_address laddrDst, Bit32u count, Bit32u granularity)
{
  Bit8u *hostAddrSrc = v2h_read_byte(laddrSrc, USER_PL);
  // Check that native host access was not vetoed for that page
  if (! hostAddrSrc) return 0;

  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  assert(! BX_CPU_THIS_PTR get_DF());

  // See how many bytes can fit in the rest of this page.
  Bit32u bytesFitSrc = 0x1000 - PAGE_OFFSET(laddrSrc);
  Bit32u bytesFitDst = 0x1000 - PAGE_OFFSET(laddrDst);

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > bytesFitSrc)
    count = bytesFitSrc;
  if (count > bytesFitDst)
    count = bytesFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  count &= ~(granularity-1);

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit8u *) hostAddrDst = * (Bit8u *) hostAddrSrc;
      hostAddrDst++;
      hostAddrSrc++;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSB(bxInstruction_c *i, unsigned dstSeg, Bit32u dstOff, Bit8u val, Bit32u count)
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

  return FastRepSTOSB(i, laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSB(bxInstruction_c *i, bx_address laddrDst, Bit8u val, Bit32u count)
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

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSW(bxInstruction_c *i, unsigned dstSeg, Bit32u dstOff, Bit16u val, Bit32u count)
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

  return FastRepSTOSW(i, laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSW(bxInstruction_c *i, bx_address laddrDst, Bit16u val, Bit32u count)
{
  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  Bit32u wordsFitDst;
  signed int pointerDelta;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF()) {
    // Counting downward.
    // Note: 1st word must not cross page boundary.
    if ((laddrDst & 0xfff) > 0xffe) return 0;
    wordsFitDst = (2 + PAGE_OFFSET(laddrDst)) >> 1;
    pointerDelta = (signed int) -2;
  }
  else {
    // Counting upward.
    wordsFitDst = (0x1000 - PAGE_OFFSET(laddrDst)) >> 1;
    pointerDelta = (signed int)  2;
  }

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
      WriteHostWordToLittleEndian(hostAddrDst, val);
      hostAddrDst += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSD(bxInstruction_c *i, unsigned dstSeg, Bit32u dstOff, Bit32u val, Bit32u count)
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

  return FastRepSTOSD(i, laddrDst, val, count);
}

Bit32u BX_CPU_C::FastRepSTOSD(bxInstruction_c *i, bx_address laddrDst, Bit32u val, Bit32u count)
{
  Bit8u *hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  Bit32u dwordsFitDst;
  signed int pointerDelta;

  // See how many dwords can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF()) {
    // Counting downward.
    // Note: 1st dword must not cross page boundary.
    if ((laddrDst & 0xfff) > 0xffc) return 0;
    dwordsFitDst = (4 + PAGE_OFFSET(laddrDst)) >> 2;
    pointerDelta = (signed int) -4;
  }
  else {
    // Counting upward.
    dwordsFitDst = (0x1000 - PAGE_OFFSET(laddrDst)) >> 2;
    pointerDelta = (signed int)  4;
  }

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
      WriteHostDWordToLittleEndian(hostAddrDst, val);
      hostAddrDst += pointerDelta;
    }

    return count;
  }

  return 0;
}
#endif
