/////////////////////////////////////////////////////////////////////////
// $Id: string.cc,v 1.30 2005-07-04 17:44:08 sshwarts Exp $
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_X86_64==0
#define RSI ESI
#define RDI EDI
#define RAX EAX
#endif


#if BX_SupportRepeatSpeedups
Bit32u BX_CPU_C::FastRepMOVSB(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff, unsigned dstSeg, bx_address dstOff, Bit32u count)
{
  Bit32u bytesFitSrc, bytesFitDst;
  signed int pointerDelta;
  bx_address laddrDst, laddrSrc;
  Bit32u     paddrDst, paddrSrc;

  bx_segment_reg_t *srcSegPtr = &BX_CPU_THIS_PTR sregs[srcSeg];
  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  // Do segment checks for the 1st byte. We do not want to
  // trip an exception beyond this, because the address would
  // be incorrect.  After we know how many bytes we will directly
  // transfer, we can do the full segment limit check ourselves
  // without generating an exception.
  read_virtual_checks(srcSegPtr, srcOff, 1);
  laddrSrc = BX_CPU_THIS_PTR get_segment_base(srcSeg) + srcOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
  }
  else {
    paddrSrc = laddrSrc;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrSrc = A20ADDR(paddrSrc);
  Bit8u *hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrSrc, BX_READ);

  if (! hostAddrSrc) return 0;

  write_virtual_checks(dstSegPtr, dstOff, 1);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many bytes can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    bytesFitSrc = 1 + (paddrSrc & 0xfff);
    bytesFitDst = 1 + (paddrDst & 0xfff);
    pointerDelta = (signed int) -1;
  }
  else {
    // Counting upward.
    bytesFitSrc = (0x1000 - (paddrSrc & 0xfff));
    bytesFitDst = (0x1000 - (paddrDst & 0xfff));
    pointerDelta = (signed int)  1;
  }

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > bytesFitSrc)
    count = bytesFitSrc;
  if (count > bytesFitDst)
    count = bytesFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
         !(dstSegPtr->cache.valid & SegAccessWOK) )
    {
      return 0;
    }

    if ( !IsLongMode() )
    {
      Bit32u srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using si/di
        // rolling over 16-bit boundaries.
        if (srcSegLimit > 0xffff)
          srcSegLimit = 0xffff;
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        Bit32u minOffset = (count-1);
        if ( srcOff < minOffset )
          return 0;
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u srcMaxOffset = (srcSegLimit - count) + 1;
        Bit32u dstMaxOffset = (dstSegLimit - count) + 1;
        if ( srcOff > srcMaxOffset )
          return 0;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit8u *) hostAddrDst = * (Bit8u *) hostAddrSrc;
      hostAddrDst += pointerDelta;
      hostAddrSrc += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepMOVSW(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff, unsigned dstSeg, bx_address dstOff, Bit32u count)
{
  Bit32u wordsFitSrc, wordsFitDst;
  signed int pointerDelta;
  bx_address laddrDst, laddrSrc;
  Bit32u     paddrDst, paddrSrc;

  bx_segment_reg_t *srcSegPtr = &BX_CPU_THIS_PTR sregs[srcSeg];
  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  // Do segment checks for the 1st word.  We do not want to
  // trip an exception beyond this, because the address would
  // be incorrect.  After we know how many bytes we will directly
  // transfer, we can do the full segment limit check ourselves
  // without generating an exception.
  read_virtual_checks(srcSegPtr, srcOff, 2);
  laddrSrc = BX_CPU_THIS_PTR get_segment_base(srcSeg) + srcOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
  }
  else {
    paddrSrc = laddrSrc;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrSrc = A20ADDR(paddrSrc);
  Bit8u *hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrSrc, BX_READ);

  if (! hostAddrSrc) return 0;

  write_virtual_checks(dstSegPtr, dstOff, 2);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    // Note: 1st word must not cross page boundary.
    if ( ((paddrSrc & 0xfff) > 0xffe) || ((paddrDst & 0xfff) > 0xffe) )
       return 0;
    wordsFitSrc = (2 + (paddrSrc & 0xfff)) >> 1;
    wordsFitDst = (2 + (paddrDst & 0xfff)) >> 1;
    pointerDelta = (signed int) -2;
  }
  else {
    // Counting upward.
    wordsFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 1;
    wordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 1;
    pointerDelta = (signed int)  2;
  }

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > wordsFitSrc)
    count = wordsFitSrc;
  if (count > wordsFitDst)
    count = wordsFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
         !(dstSegPtr->cache.valid & SegAccessWOK) )
    {
      return 0;
    }

    if ( !IsLongMode() )
    {
      Bit32u srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using si/di
        // rolling over 16-bit boundaries.
        if (srcSegLimit > 0xffff)
          srcSegLimit = 0xffff;
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        // Counting downward.
        Bit32u minOffset = (count-1) << 1;
        if ( srcOff < minOffset )
          return 0;
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u srcMaxOffset = (srcSegLimit - (count<<1)) + 1;
        Bit32u dstMaxOffset = (dstSegLimit - (count<<1)) + 1;
        if ( srcOff > srcMaxOffset )
          return 0;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit16u *) hostAddrDst = * (Bit16u *) hostAddrSrc;
      hostAddrDst += pointerDelta;
      hostAddrSrc += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepMOVSD(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff, unsigned dstSeg, bx_address dstOff, Bit32u count)
{
  Bit32u dwordsFitSrc, dwordsFitDst;
  signed int pointerDelta;
  bx_address laddrDst, laddrSrc;
  Bit32u     paddrDst, paddrSrc;

  bx_segment_reg_t *srcSegPtr = &BX_CPU_THIS_PTR sregs[srcSeg];
  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  // Do segment checks for the 1st dword.  We do not want to
  // trip an exception beyond this, because the address would
  // be incorrect.  After we know how many bytes we will directly
  // transfer, we can do the full segment limit check ourselves
  // without generating an exception.
  read_virtual_checks(srcSegPtr, srcOff, 4);
  laddrSrc = BX_CPU_THIS_PTR get_segment_base(srcSeg) + srcOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
  }
  else {
    paddrSrc = laddrSrc;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrSrc = A20ADDR(paddrSrc);
  Bit8u *hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrSrc, BX_READ);

  if (! hostAddrSrc) return 0;

  write_virtual_checks(dstSegPtr, dstOff, 4);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many dwords can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    // Note: 1st dword must not cross page boundary.
    if ( ((paddrSrc & 0xfff) > 0xffc) || ((paddrDst & 0xfff) > 0xffc) )
      return 0;    
    dwordsFitSrc = (4 + (paddrSrc & 0xfff)) >> 2;
    dwordsFitDst = (4 + (paddrDst & 0xfff)) >> 2;
    pointerDelta = (signed int) -4;
  }
  else {
    // Counting upward.
    dwordsFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 2;
    dwordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 2;
    pointerDelta = (signed int)  4;
  }

  // Restrict dword count to the number that will fit in either
  // source or dest pages.
  if (count > dwordsFitSrc)
    count = dwordsFitSrc;
  if (count > dwordsFitDst)
    count = dwordsFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(srcSegPtr->cache.valid & SegAccessROK) ||
         !(dstSegPtr->cache.valid & SegAccessWOK) )
    {
      return 0;
    }

    if ( !IsLongMode() )
    {
      Bit32u srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using si/di
        // rolling over 16-bit boundaries.
        if (srcSegLimit > 0xffff)
          srcSegLimit = 0xffff;
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        // Counting downward.
        Bit32u minOffset = (count-1) << 2;
        if ( srcOff < minOffset )
          return 0;
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u srcMaxOffset = (srcSegLimit - (count<<2)) + 1;
        Bit32u dstMaxOffset = (dstSegLimit - (count<<2)) + 1;
        if ( srcOff > srcMaxOffset )
          return 0;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit32u *) hostAddrDst = * (Bit32u *) hostAddrSrc;
      hostAddrDst += pointerDelta;
      hostAddrSrc += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSB(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff, Bit8u val, Bit32u count)
{
  Bit32u bytesFitDst;
  signed int pointerDelta;
  bx_address laddrDst;
  Bit32u     paddrDst;

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  write_virtual_checks(dstSegPtr, dstOff, 1);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many bytes can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    bytesFitDst = 1 + (paddrDst & 0xfff);
    pointerDelta = (signed int) -1;
  }
  else {
    // Counting upward.
    bytesFitDst = (0x1000 - (paddrDst & 0xfff));
    pointerDelta = (signed int)  1;
  }

  // Restrict word count to the number that will fit in either
  // source or dest pages.
  if (count > bytesFitDst)
    count = bytesFitDst;
  if (count > bx_pc_system.getNumCpuTicksLeftNextEvent())
    count = bx_pc_system.getNumCpuTicksLeftNextEvent();

  // If after all the restrictions, there is anything left to do...
  if (count) {
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) return 0;

    if ( !IsLongMode() )
    {
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using di
        // rolling over 16-bit boundaries.
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        Bit32u minOffset = (count-1);
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u dstMaxOffset = (dstSegLimit - count) + 1;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit8u *) hostAddrDst = val;
      hostAddrDst += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSW(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff, Bit16u val, Bit32u count)
{
  Bit32u wordsFitDst;
  signed int pointerDelta;
  bx_address laddrDst;
  Bit32u     paddrDst;

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  write_virtual_checks(dstSegPtr, dstOff, 2);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    // Note: 1st word must not cross page boundary.
    if ((paddrDst & 0xfff) > 0xffe) return 0;
    wordsFitDst = (2 + (paddrDst & 0xfff)) >> 1;
    pointerDelta = (signed int) -2;
  }
  else {
    // Counting upward.
    wordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 1;
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
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) return 0;

    if ( !IsLongMode() )
    {
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using di
        // rolling over 16-bit boundaries.
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        // Counting downward.
        Bit32u minOffset = (count-1) << 1;
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u dstMaxOffset = (dstSegLimit - (count<<1)) + 1;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit16u *) hostAddrDst = val;
      hostAddrDst += pointerDelta;
    }

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepSTOSD(bxInstruction_c *i, unsigned dstSeg, bx_address dstOff, Bit32u val, Bit32u count)
{
  Bit32u dwordsFitDst;
  signed int pointerDelta;
  bx_address laddrDst;
  Bit32u     paddrDst;

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[dstSeg];

  write_virtual_checks(dstSegPtr, dstOff, 4);
  laddrDst = BX_CPU_THIS_PTR get_segment_base(dstSeg) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg) {
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  }
  else {
    paddrDst = laddrDst;
  }

  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);
  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE);

  if (! hostAddrDst) return 0;

  // See how many dwords can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF ()) {
    // Counting downward.
    // Note: 1st dword must not cross page boundary.
    if ((paddrDst & 0xfff) > 0xffc) return 0;    
    dwordsFitDst = (4 + (paddrDst & 0xfff)) >> 2;
    pointerDelta = (signed int) -4;
  }
  else {
    // Counting upward.
    dwordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 2;
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
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non expand-down (thus we can make a simple limit check).
    if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) return 0;

    if ( !IsLongMode() )
    {
      Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;

      if (! i->as32L()) {
        // For 16-bit addressing mode, clamp the segment limits to 16bits
        // so we don't have to worry about computations using di
        // rolling over 16-bit boundaries.
        if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
      }

      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // count >= 1 here.
      if (BX_CPU_THIS_PTR get_DF ()) {
        // Counting downward.
        Bit32u minOffset = (count-1) << 2;
        if ( dstOff < minOffset )
          return 0;
      }
      else {
        // Counting upward.
        Bit32u dstMaxOffset = (dstSegLimit - (count<<2)) + 1;
        if ( dstOff > dstMaxOffset )
          return 0;
      }
    }

    // Transfer data directly using host addresses
    for (unsigned j=0; j<count; j++) {
      * (Bit32u *) hostAddrDst = val;
      hostAddrDst += pointerDelta;
    }

    return count;
  }

  return 0;
}
#endif


/* MOVSB ES:[EDI], DS:[ESI]   DS may be overridden
 *   mov string from DS:[ESI] into ES:[EDI]
 */
void BX_CPU_C::MOVSB_XbYb(bxInstruction_c *i)
{
  unsigned seg;
  Bit8u temp8;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;

    read_virtual_byte(seg, rsi, &temp8);
    write_virtual_byte(BX_SEG_REG_ES, rdi, &temp8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement RSI, RDI */
      rsi--;
      rdi--;
    }
    else {
      /* increment RSI, RDI */
      rsi++;
      rdi++;
    }

    RSI = rsi;
    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    Bit32u esi, edi;

    esi = ESI;
    edi = EDI;

    read_virtual_byte(seg, esi, &temp8);
    write_virtual_byte(BX_SEG_REG_ES, edi, &temp8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement ESI, EDI */
      esi--;
      edi--;
    }
    else {
      /* increment ESI, EDI */
      esi++;
      edi++;
    }

    // zero extension of RSI/RDI
    RSI = esi;
    RDI = edi;
  }
  else
  { /* 16 bit address mode */
    unsigned incr = 1;

    Bit16u si = SI;
    Bit16u di = DI;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time */
    if (i->repUsedL() && !BX_CPU_THIS_PTR async_event)
    {
      Bit32u byteCount = CX;
      BX_ASSERT(byteCount > 0);
      byteCount = FastRepMOVSB(i, seg, si, BX_SEG_REG_ES, di, byteCount);
      if (byteCount)
      {
        // Decrement the ticks count by the number of iterations, minus
        // one, since the main cpu loop will decrement one.  Also,
        // the count is predecremented before examined, so defintely
        // don't roll it under zero.
        BX_TICKN(byteCount-1);

        // Decrement eCX. Note, the main loop will decrement 1 also, so
        // decrement by one less than expected, like the case above.
        CX -= (byteCount-1);

        incr = byteCount;
        goto doIncr16;
      }
    }
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    read_virtual_byte(seg, si, &temp8);
    write_virtual_byte(BX_SEG_REG_ES, di, &temp8);

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr16:
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement SI, DI */
      si -= incr;
      di -= incr;
    }
    else {
      /* increment SI, DI */
      si += incr;
      di += incr;
    }

    SI = si;
    DI = di;
  }
}

/* 16 bit opsize mode */
void BX_CPU_C::MOVSW_XwYw(bxInstruction_c *i)
{
  Bit16u temp16;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {

    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    read_virtual_word(seg, rsi, &temp16);
    write_virtual_word(BX_SEG_REG_ES, rdi, &temp16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 2;
      rdi -= 2;
    }
    else {
      rsi += 2;
      rdi += 2;
    }

    RSI = rsi;
    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {

    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_word(seg, esi, &temp16);
    write_virtual_word(BX_SEG_REG_ES, edi, &temp16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 2;
      edi -= 2;
    }
    else {
      esi += 2;
      edi += 2;
    }

    // zero extension of RSI/RDI
    RSI = esi;
    RDI = edi;
  }
  else
  { /* 16bit address mode */
    unsigned incr = 2;

    Bit16u si = SI;
    Bit16u di = DI;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->repUsedL() && !BX_CPU_THIS_PTR async_event)
    {
      Bit32u wordCount = CX;
      BX_ASSERT(wordCount > 0);
      wordCount = FastRepMOVSW(i, seg, si, BX_SEG_REG_ES, di, wordCount);
      if (wordCount)
      {
        // Decrement the ticks count by the number of iterations, minus
        // one, since the main cpu loop will decrement one.  Also,
        // the count is predecremented before examined, so defintely
        // don't roll it under zero.
        BX_TICKN(wordCount-1);

        // Decrement eCX. Note, the main loop will decrement 1 also, so
        // decrement by one less than expected, like the case above.
        CX -= (wordCount-1);

        incr = wordCount << 1; // count * 2
        goto doIncr16;
      }
    }
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    read_virtual_word(seg, si, &temp16);
    write_virtual_word(BX_SEG_REG_ES, di, &temp16);

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr16:
#endif
#endif

    if (BX_CPU_THIS_PTR get_DF ()) {
      /* decrement SI, DI */
      si -= incr;
      di -= incr;
    }
    else {
      /* increment SI, DI */
      si += incr;
      di += incr;
    }

    SI = si;
    DI = di;
  }
}

/* 32 bit opsize mode */
void BX_CPU_C::MOVSD_XdYd(bxInstruction_c *i)
{
  Bit32u temp32;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    read_virtual_dword(seg, rsi, &temp32);
    write_virtual_dword(BX_SEG_REG_ES, rdi, &temp32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 4;
      rdi -= 4;
    }
    else {
      rsi += 4;
      rdi += 4;
    }

    RSI = rsi;
    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    unsigned incr = 4;

    Bit32u esi = ESI;
    Bit32u edi = EDI;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->repUsedL() && !BX_CPU_THIS_PTR async_event)
    {
      Bit32u dwordCount = ECX;
      BX_ASSERT(dwordCount > 0);
      dwordCount = FastRepMOVSD(i, seg, esi, BX_SEG_REG_ES, edi, dwordCount);
      if (dwordCount)
      {
        // Decrement the ticks count by the number of iterations, minus
        // one, since the main cpu loop will decrement one.  Also,
        // the count is predecremented before examined, so defintely
        // don't roll it under zero.
        BX_TICKN(dwordCount-1);

        // Decrement eCX. Note, the main loop will decrement 1 also, so
        // decrement by one less than expected, like the case above.
        ECX -= (dwordCount-1);

        incr = dwordCount << 2; // count * 4
        goto doIncr32;
      }
    }
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    read_virtual_dword(seg, esi, &temp32);
    write_virtual_dword(BX_SEG_REG_ES, edi, &temp32);

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr32:
#endif
#endif

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= incr;
      edi -= incr;
    }
    else {
      esi += incr;
      edi += incr;
    }

    // zero extension of RSI/RDI
    RSI = esi;
    RDI = edi;
  }
  else
  { /* 16bit address mode */
    Bit16u si = SI;
    Bit16u di = DI;

    read_virtual_dword(seg, si, &temp32);
    write_virtual_dword(BX_SEG_REG_ES, di, &temp32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      si -= 4;
      di -= 4;
    }
    else {
      si += 4;
      di += 4;
    }

    SI = si;
    DI = di;
  }
}

#if BX_SUPPORT_X86_64

/* 64 bit opsize mode */
void BX_CPU_C::MOVSQ_XqYq(bxInstruction_c *i)
{
  Bit64u temp64;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

  if (i->as64L()) {
    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    read_virtual_qword(seg, rsi, &temp64);
    write_virtual_qword(BX_SEG_REG_ES, rdi, &temp64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 8;
      rdi -= 8;
    }
    else {
      rsi += 8;
      rdi += 8;
    }

    RSI = rsi;
    RDI = rdi;
  }
  else /* 32-bit address size mode */
  {
    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_qword(seg, esi, &temp64);
    write_virtual_qword(BX_SEG_REG_ES, edi, &temp64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 8;
      edi -= 8;
    }
    else {
      esi += 8;
      edi += 8;
    }

    // zero extension of RSI/RDI
    RSI = esi;
    RDI = edi;
  }
}

#endif

void BX_CPU_C::CMPSB_XbYb(bxInstruction_c *i)
{
  unsigned seg;
  Bit8u op1_8, op2_8, diff_8;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    read_virtual_byte(seg, rsi, &op1_8);
    read_virtual_byte(BX_SEG_REG_ES, rdi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi--;
      rdi--;
    }
    else {
      rsi++;
      rdi++;
    }

    RDI = rdi;
    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_byte(seg, esi, &op1_8);
    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi--;
      edi--;
    }
    else {
      esi++;
      edi++;
    }

    // zero extension of RSI/RDI
    RDI = edi;
    RSI = esi;
  }
  else
  { /* 16bit address mode */
    Bit16u si = SI;
    Bit16u di = DI;

    read_virtual_byte(seg, si, &op1_8);
    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      si--;
      di--;
    }
    else {
      si++;
      di++;
    }

    DI = di;
    SI = si;
  }
}

/* 16 bit opsize mode */
void BX_CPU_C::CMPSW_XwYw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, diff_16;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi, rdi;

    rsi = RSI;
    rdi = RDI;

    read_virtual_word(seg, rsi, &op1_16);
    read_virtual_word(BX_SEG_REG_ES, rdi, &op2_16);

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 2;
      rdi -= 2;
    }
    else {
      rsi += 2;
      rdi += 2;
    }

    RDI = rdi;
    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_word(seg, esi, &op1_16);
    read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);

    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 2;
      edi -= 2;
    }
    else {
      esi += 2;
      edi += 2;
    }

    // zero extension of RSI/RDI
    RDI = edi;
    RSI = esi;
  }
  else
  { /* 16 bit address mode */
    Bit16u si = SI;
    Bit16u di = DI;

    read_virtual_word(seg, si, &op1_16);
    read_virtual_word(BX_SEG_REG_ES, di, &op2_16);

    diff_16 = op1_16 - op2_16;
    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      si -= 2;
      di -= 2;
    }
    else {
      si += 2;
      di += 2;
    }

    DI = di;
    SI = si;
  }
}

/* 32 bit opsize mode */
void BX_CPU_C::CMPSD_XdYd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    Bit32u op1_32, op2_32, diff_32;

    read_virtual_dword(seg, rsi, &op1_32);
    read_virtual_dword(BX_SEG_REG_ES, rdi, &op2_32);

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 4;
      rdi -= 4;
    }
    else {
      rsi += 4;
      rdi += 4;
    }

    RDI = rdi;
    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_dword(seg, esi, &op1_32);
    read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 4;
      edi -= 4;
    }
    else {
      esi += 4;
      edi += 4;
    }

    // zero extension of RSI/RDI
    RDI = edi;
    RSI = esi;
  }
  else
  { /* 16 bit address mode */
    Bit16u si = SI;
    Bit16u di = DI;

    read_virtual_dword(seg, si, &op1_32);
    read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);

    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      si -= 4;
      di -= 4;
    }
    else {
      si += 4;
      di += 4;
    }

    DI = di;
    SI = si;
  }
}

#if BX_SUPPORT_X86_64

/* 64 bit opsize mode */
void BX_CPU_C::CMPSQ_XqYq(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;
  unsigned seg;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

  if (i->as64L()) {
    Bit64u rsi = RSI;
    Bit64u rdi = RDI;

    read_virtual_qword(seg, rsi, &op1_64);
    read_virtual_qword(BX_SEG_REG_ES, rdi, &op2_64);

    diff_64 = op1_64 - op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 8;
      rdi -= 8;
    }
    else {
      rsi += 8;
      rdi += 8;
    }

    RDI = rdi;
    RSI = rsi;
  }
  else /* 32 bit address size */
  {
    Bit32u esi = ESI;
    Bit32u edi = EDI;

    read_virtual_qword(seg, esi, &op1_64);
    read_virtual_qword(BX_SEG_REG_ES, edi, &op2_64);

    diff_64 = op1_64 - op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 8;
      edi -= 8;
    }
    else {
      esi += 8;
      edi += 8;
    }

    // zero extension of RSI/RDI
    RDI = edi;
    RSI = esi;
  }
}

#endif

void BX_CPU_C::SCASB_ALXb(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = AL;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    read_virtual_byte(BX_SEG_REG_ES, rdi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);
 
    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi--;
    }
    else {
      rdi++;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u edi = EDI;

    read_virtual_byte(BX_SEG_REG_ES, edi, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi--;
    }
    else {
      edi++;
    }

    // zero extension of RDI
    RDI = edi;
  }
  else
  { /* 16bit address mode */
    Bit16u di = DI;

    read_virtual_byte(BX_SEG_REG_ES, di, &op2_8);

    diff_8 = op1_8 - op2_8;

    SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_COMPARE8);

    if (BX_CPU_THIS_PTR get_DF ()) {
      di--;
    }
    else {
      di++;
    }

    DI = di;
  }
}

/* 16 bit opsize mode */
void BX_CPU_C::SCASW_AXXw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, diff_16;

  op1_16 = AX;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    read_virtual_word(BX_SEG_REG_ES, rdi, &op2_16);
    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 2;
    }
    else {
      rdi += 2;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u edi = EDI;

    read_virtual_word(BX_SEG_REG_ES, edi, &op2_16);
    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 2;
    }
    else {
      edi += 2;
    }

    // zero extension of RDI
    RDI = edi;
  }
  else
  { /* 16bit address mode */
    Bit16u di = DI;

    read_virtual_word(BX_SEG_REG_ES, di, &op2_16);
    diff_16 = op1_16 - op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, diff_16, BX_INSTR_COMPARE16);

    if (BX_CPU_THIS_PTR get_DF ()) {
      di -= 2;
    }
    else {
      di += 2;
    }

    DI = di;
  }
}

/* 32 bit opsize mode */
void BX_CPU_C::SCASD_EAXXd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    read_virtual_dword(BX_SEG_REG_ES, rdi, &op2_32);
    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 4;
    }
    else {
      rdi += 4;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) {
    Bit32u edi = EDI;

    read_virtual_dword(BX_SEG_REG_ES, edi, &op2_32);
    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 4;
    }
    else {
      edi += 4;
    }

    // zero extension of RDI
    RDI = edi;
  }
  else
  { /* 16bit address mode */
    Bit16u di = DI;

    read_virtual_dword(BX_SEG_REG_ES, di, &op2_32);
    diff_32 = op1_32 - op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_COMPARE32);

    if (BX_CPU_THIS_PTR get_DF ()) {
      di -= 4;
    }
    else {
      di += 4;
    }

    DI = di;
  }
}

#if BX_SUPPORT_X86_64

/* 64 bit opsize mode */
void BX_CPU_C::SCASQ_RAXXq(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;

  if (i->as64L()) {
    Bit64u rdi = RDI;

    read_virtual_qword(BX_SEG_REG_ES, rdi, &op2_64);
    diff_64 = op1_64 - op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 8;
    }
    else {
      rdi += 8;
    }

    RDI = rdi;
  }
  else
  {
    Bit32u edi = EDI;

    read_virtual_qword(BX_SEG_REG_ES, edi, &op2_64);
    diff_64 = op1_64 - op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 8;
    }
    else {
      edi += 8;
    }

    // zero extension of RDI
    RDI = edi;
  }
}

#endif

void BX_CPU_C::STOSB_YbAL(bxInstruction_c *i)
{
  Bit8u al = AL;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    write_virtual_byte(BX_SEG_REG_ES, rdi, &al);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi--;
    }
    else {
      rdi++;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  {
    unsigned incr = 1;
    Bit32u edi;

    if (i->as32L()) {
      edi = EDI;
    }
    else
    { /* 16bit address size */
      edi = DI;
    }

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->repUsedL() && !BX_CPU_THIS_PTR async_event)
    {
      Bit32u byteCount;

      if (i->as32L())
        byteCount = ECX;
      else
        byteCount = CX;

      BX_ASSERT(byteCount);
      byteCount = FastRepSTOSB(i, BX_SEG_REG_ES, edi, al, byteCount);
      if (byteCount)
      {
        // Decrement the ticks count by the number of iterations, minus
        // one, since the main cpu loop will decrement one.  Also,
        // the count is predecremented before examined, so defintely
        // don't roll it under zero.
        BX_TICKN(byteCount-1);

        // Decrement eCX.  Note, the main loop will decrement 1 also, so
        // decrement by one less than expected, like the case above.
        if (i->as32L())
          ECX -= (byteCount-1);
        else
          CX  -= (byteCount-1);

        incr = byteCount;
        goto doIncr16;
      }
    }
#endif  // (BX_DEBUGGER == 0)
#endif  // BX_SupportRepeatSpeedups

    write_virtual_byte(BX_SEG_REG_ES, edi, &al);

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr16:
#endif
#endif

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= incr;
    }
    else {
      edi += incr;
    }

    if (i->as32L())
      // zero extension of RDI
      RDI = edi;
    else
       DI = edi;
  }
}

/* 16 bit opsize mode */
void BX_CPU_C::STOSW_YwAX(bxInstruction_c *i)
{
  Bit16u ax = AX;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    write_virtual_word(BX_SEG_REG_ES, rdi, &ax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 2;
    }
    else {
      rdi += 2;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L()) 
  {
    Bit32u edi = EDI;

    write_virtual_word(BX_SEG_REG_ES, edi, &ax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 2;
    }
    else {
      edi += 2;
    }

    // zero extension of RDI
    RDI = edi;
  }
  else
  { /* 16bit address size */
    Bit16u di = DI;

    write_virtual_word(BX_SEG_REG_ES, di, &ax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      di -= 2;
    }
    else {
      di += 2;
    }

    DI = di;
  }
}

/* 32 bit opsize mode */
void BX_CPU_C::STOSD_YdEAX(bxInstruction_c *i)
{
  Bit32u eax = EAX;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rdi = RDI;

    write_virtual_dword(BX_SEG_REG_ES, rdi, &eax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 4;
    }
    else {
      rdi += 4;
    }

    RDI = rdi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    Bit32u edi = EDI;

    write_virtual_dword(BX_SEG_REG_ES, edi, &eax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 4;
    }
    else {
      edi += 4;
    }

    // zero extension of RDI
    RDI = edi;
  }
  else
  { /* 16bit address size */
    Bit16u di = DI;

    write_virtual_dword(BX_SEG_REG_ES, di, &eax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      di -= 4;
    }
    else {
      di += 4;
    }

    DI = di;
  }
}

#if BX_SUPPORT_X86_64

/* 64 bit opsize mode */
void BX_CPU_C::STOSQ_YqRAX(bxInstruction_c *i)
{
  Bit64u rax = RAX;

  if (i->as64L()) {
    Bit64u rdi = RDI;

    write_virtual_qword(BX_SEG_REG_ES, rdi, &rax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      rdi -= 8;
    }
    else {
      rdi += 8;
    }

    RDI = rdi;
  }
  else /* 32 bit address size */
  {
    Bit32u edi = EDI;

    write_virtual_qword(BX_SEG_REG_ES, edi, &rax);

    if (BX_CPU_THIS_PTR get_DF ()) {
      edi -= 8;
    }
    else {
      edi += 8;
    }

    // zero extension of RDI
    RDI = edi;
  }
}

#endif

void BX_CPU_C::LODSB_ALXb(bxInstruction_c *i)
{
  unsigned seg;
  Bit8u al;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;

    read_virtual_byte(seg, rsi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi--;
    }
    else {
      rsi++;
    }

    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    Bit32u esi = ESI;

    read_virtual_byte(seg, esi, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      esi--;
    }
    else {
      esi++;
    }

    // zero extension of RSI
    RSI = esi;
  }
  else
  { /* 16bit address mode */
    Bit16u si = SI;

    read_virtual_byte(seg, si, &al);

    AL = al;
    if (BX_CPU_THIS_PTR get_DF ()) {
      si--;
    }
    else {
      si++;
    }

    SI = si;
  }
}

/* 16 bit opsize mode */
void BX_CPU_C::LODSW_AXXw(bxInstruction_c *i)
{
  unsigned seg;
  Bit16u ax;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;

    read_virtual_word(seg, rsi, &ax);
    AX = ax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 2;
    }
    else {
      rsi += 2;
    }

    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    Bit32u esi = ESI;

    read_virtual_word(seg, esi, &ax);
    AX = ax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 2;
    }
    else {
      esi += 2;
    }

    // zero extension of RSI
    RSI = esi;
  }
  else
  { /* 16bit address mode */
    Bit16u si = SI;

    read_virtual_word(seg, si, &ax);
    AX = ax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      si -= 2;
    }
    else {
      si += 2;
    }

    SI = si;
  }
}

/* 32 bit opsize mode */
void BX_CPU_C::LODSD_EAXXd(bxInstruction_c *i)
{
  unsigned seg;
  Bit32u eax;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    Bit64u rsi = RSI;

    read_virtual_dword(seg, rsi, &eax);
    RAX = eax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 4;
    }
    else {
      rsi += 4;
    }

    RSI = rsi;
  }
  else
#endif  // #if BX_SUPPORT_X86_64
  if (i->as32L())
  {
    Bit32u esi = ESI;

    read_virtual_dword(seg, esi, &eax);
    RAX = eax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 4;
    }
    else {
      esi += 4;
    }

    // zero extension of RSI
    RSI = esi;
  }
  else
  { /* 16bit address mode */
    Bit16u si = SI;

    read_virtual_dword(seg, si, &eax);
    RAX = eax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      si -= 4;
    }
    else {
      si += 4;
    }

    SI = si;
  }
}

#if BX_SUPPORT_X86_64

/* 64 bit opsize mode */
void BX_CPU_C::LODSQ_RAXXq(bxInstruction_c *i)
{
  unsigned seg;
  Bit64u rax;

  if (!BX_NULL_SEG_REG(i->seg())) {
    seg = i->seg();
  }
  else {
    seg = BX_SEG_REG_DS;
  }

  if (i->as64L()) {
    Bit64u rsi = RSI;

    read_virtual_qword(seg, rsi, &rax);
    RAX = rax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      rsi -= 8;
    }
    else {
      rsi += 8;
    }

    RSI = rsi;
  }
  else /* 32 bit address size */
  {
    Bit32u esi = ESI;

    read_virtual_qword(seg, esi, &rax);
    RAX = rax;

    if (BX_CPU_THIS_PTR get_DF ()) {
      esi -= 8;
    }
    else {
      esi += 8;
    }

    // zero extension of RSI
    RSI = esi;
  }
}

#endif
