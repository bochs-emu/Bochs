/////////////////////////////////////////////////////////////////////////
// $Id: io.cc,v 1.36 2006-08-01 17:09:05 vruppert Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RDI EDI
#define RSI ESI
#define RAX EAX
#endif

#if BX_SupportRepeatSpeedups
Bit32u BX_CPU_C::FastRepINSW(bxInstruction_c *i, bx_address dstOff, Bit16u port, Bit32u wordCount)
{
  Bit32u paddrDst, wordsFitDst;
  signed int pointerDelta;

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES];

  // Do segment checks for the 1st word.  We do not want to
  // trip an exception beyond this, because the address would
  // be incorrect.  After we know how many bytes we will directly
  // transfer, we can do the full segment limit check ourselves
  // without generating an exception.
  write_virtual_checks(dstSegPtr, dstOff, 2);

  bx_address laddrDst = BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_ES) + dstOff;
  if (BX_CPU_THIS_PTR cr0.pg)
    paddrDst = dtranslate_linear(laddrDst, CPL==3, BX_WRITE);
  else
    paddrDst = laddrDst;
  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrDst = A20ADDR(paddrDst);

  Bit8u *hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
            paddrDst, BX_WRITE, DATA_ACCESS);

  // Check that native host access was not vetoed for that page, and
  // that the address is word aligned.
  if (!hostAddrDst || (paddrDst & 1)) return 0;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF()) {
    // Counting downward
    // Note: 1st word must not cross page boundary.
    if ((paddrDst & 0xfff) > 0xffe) return 0;
    wordsFitDst = (2 + (paddrDst & 0xfff)) >> 1;
    pointerDelta = -2;
  }
  else {
    // Counting upward
    wordsFitDst = (0x1000 - (paddrDst & 0xfff)) >> 1;
    pointerDelta =  2;
  }

  // Restrict word count to the number that will fit in this page.
  if (wordCount > wordsFitDst)
      wordCount = wordsFitDst;

  // If after all the restrictions, there is anything left to do...
  if (wordCount) {
    Bit32u dstSegLimit = dstSegPtr->cache.u.segment.limit_scaled;
    unsigned count;

    // For 16-bit addressing mode, clamp the segment limits to 16bits
    // so we don't have to worry about computations using si/di
    // rolling over 16-bit boundaries.
    if (!i->as32L()) {
      if (dstSegLimit > 0xffff)
          dstSegLimit = 0xffff;
    }
    // Before we copy memory, we need to make sure that the segments
    // allow the accesses up to the given source and dest offset.  If
    // the cache.valid bits have SegAccessWOK and ROK, we know that
    // the cache is valid for those operations, and that the segments
    // are non-expand down (thus we can make a simple limit check).
    if (!(dstSegPtr->cache.valid & SegAccessWOK)) return 0;

    if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64)
    {
      // Now make sure transfer will fit within the constraints of the
      // segment boundaries, 0..limit for non expand-down.  We know
      // wordCount >= 1 here.
      if (BX_CPU_THIS_PTR get_DF()) {
        // Counting downward
        Bit32u minOffset = (wordCount-1) << 1;
        if (dstOff < minOffset) return 0;
      }
      else {
        // Counting upward
        Bit32u dstMaxOffset = (dstSegLimit - (wordCount<<1)) + 1;
        if (dstOff > dstMaxOffset) return 0;
      }
    }

    for (count=0; count<wordCount; ) {
      bx_devices.bulkIOQuantumsTransferred = 0;
      if (BX_CPU_THIS_PTR get_DF()==0) { // Only do accel for DF=0
        bx_devices.bulkIOHostAddr = hostAddrDst;
        bx_devices.bulkIOQuantumsRequested = (wordCount - count);
      }
      else
        bx_devices.bulkIOQuantumsRequested = 0;
      Bit16u temp16 = BX_INP(port, 2);
      if (bx_devices.bulkIOQuantumsTransferred) {
        hostAddrDst =  bx_devices.bulkIOHostAddr;
        count += bx_devices.bulkIOQuantumsTransferred;
      }
      else {
#ifdef BX_LITTLE_ENDIAN
        * (Bit16u *) hostAddrDst = temp16;
#else
        * (Bit16u *) hostAddrDst = ((temp16 >> 8) | (temp16 << 8));
#endif
        hostAddrDst += pointerDelta;
        count++;
      }
      // Terminate early if there was an event.
      if (BX_CPU_THIS_PTR async_event) break;
    }

    // Reset for next non-bulk IO
    bx_devices.bulkIOQuantumsRequested = 0;

    return count;
  }

  return 0;
}

Bit32u BX_CPU_C::FastRepOUTSW(bxInstruction_c *i, unsigned srcSeg, bx_address srcOff, Bit16u port, Bit32u wordCount)
{
  Bit32u paddrSrc, wordsFitSrc;
  signed int pointerDelta;

  bx_segment_reg_t *srcSegPtr = &BX_CPU_THIS_PTR sregs[srcSeg];

  // Do segment checks for the 1st word.  We do not want to
  // trip an exception beyond this, because the address would
  // be incorrect.  After we know how many bytes we will directly
  // transfer, we can do the full segment limit check ourselves
  // without generating an exception.
  read_virtual_checks(srcSegPtr, srcOff, 2);

  bx_address laddrSrc = BX_CPU_THIS_PTR get_segment_base(srcSeg) + srcOff;
  if (BX_CPU_THIS_PTR cr0.pg)
    paddrSrc = dtranslate_linear(laddrSrc, CPL==3, BX_READ);
  else
    paddrSrc = laddrSrc;
  // If we want to write directly into the physical memory array,
  // we need the A20 address.
  paddrSrc = A20ADDR(paddrSrc);

  Bit8u *hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS,
          paddrSrc, BX_READ, DATA_ACCESS);

  // Check that native host access was not vetoed for that page, and
  // that the address is word aligned.
  if (hostAddrSrc && ! (paddrSrc & 1)) {
    // See how many words can fit in the rest of this page.
    if (BX_CPU_THIS_PTR get_DF()) {
      // Counting downward
      // Note: 1st word must not cross page boundary.
      if ((paddrSrc & 0xfff) > 0xffe) return 0;
      wordsFitSrc = (2 + (paddrSrc & 0xfff)) >> 1;
      pointerDelta = (unsigned) -2;
    }
    else {
      // Counting upward
      wordsFitSrc = (0x1000 - (paddrSrc & 0xfff)) >> 1;
      pointerDelta =  2;
    }

    // Restrict word count to the number that will fit in this page.
    if (wordCount > wordsFitSrc)
        wordCount = wordsFitSrc;

    // If after all the restrictions, there is anything left to do...
    if (wordCount) {
      Bit32u srcSegLimit = srcSegPtr->cache.u.segment.limit_scaled;
      unsigned count;

      // For 16-bit addressing mode, clamp the segment limits to 16bits
      // so we don't have to worry about computations using si/di
      // rolling over 16-bit boundaries.
      if (!i->as32L()) {
        if (srcSegLimit > 0xffff)
            srcSegLimit = 0xffff;
      }

      // Before we copy memory, we need to make sure that the segments
      // allow the accesses up to the given source and dest offset.  If
      // the cache.valid bits have SegAccessWOK and ROK, we know that
      // the cache is valid for those operations, and that the segments
      // are non-expand down (thus we can make a simple limit check).
      if ( !(srcSegPtr->cache.valid & SegAccessROK) ) return 0;

      if (BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64)
      {
        // Now make sure transfer will fit within the constraints of the
        // segment boundaries, 0..limit for non expand-down.  We know
        // wordCount >= 1 here.
        if (BX_CPU_THIS_PTR get_DF()) {
          // Counting downward
          Bit32u minOffset = (wordCount-1) << 1;
          if (srcOff < minOffset) return 0;
        }
        else {
          // Counting upward
          Bit32u srcMaxOffset = (srcSegLimit - (wordCount<<1)) + 1;
          if (srcOff > srcMaxOffset) return 0;
        }
      }

      for (count=0; count<wordCount; ) {
        bx_devices.bulkIOQuantumsTransferred = 0;
        if (BX_CPU_THIS_PTR get_DF()==0) { // Only do accel for DF=0
          bx_devices.bulkIOHostAddr = hostAddrSrc;
          bx_devices.bulkIOQuantumsRequested = (wordCount - count);
        }
        else
          bx_devices.bulkIOQuantumsRequested = 0;
        Bit16u temp16 = * (Bit16u *) hostAddrSrc;
#ifdef BX_LITTLE_ENDIAN
        BX_OUTP(port, temp16, 2);
#else
        BX_OUTP(port, ((temp16 >> 8) | (temp16 << 8)), 2);
#endif
        if (bx_devices.bulkIOQuantumsTransferred) {
          hostAddrSrc =  bx_devices.bulkIOHostAddr;
          count += bx_devices.bulkIOQuantumsTransferred;
        }
        else {
          hostAddrSrc += pointerDelta;
          count++;
        }
        // Terminate early if there was an event.
        if (BX_CPU_THIS_PTR async_event) break;
      }

      // Reset for next non-bulk IO
      bx_devices.bulkIOQuantumsRequested = 0;

      return count;
    }
  }

  return 0;
}

#endif

void BX_CPU_C::INSB_YbDX(bxInstruction_c *i)
{
  Bit8u value8=0;

  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL()))) {
    if (! BX_CPU_THIS_PTR allow_io(DX, 1)) {
      BX_DEBUG(("INSB_YbDX: I/O access not allowed !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_byte(BX_SEG_REG_ES, RDI, &value8);

    value8 = BX_INP(DX, 1);

    /* no seg override possible */
    write_virtual_byte(BX_SEG_REG_ES, RDI, &value8);

    if (BX_CPU_THIS_PTR get_DF())
      RDI--;
    else
      RDI++;
  }
  else
#endif
   if (i->as32L()) {
    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_byte(BX_SEG_REG_ES, EDI, &value8);

    value8 = BX_INP(DX, 1);

    /* no seg override possible */
    write_virtual_byte(BX_SEG_REG_ES, EDI, &value8);

    if (BX_CPU_THIS_PTR get_DF()) {
      RDI = EDI - 1;
    }
    else {
      RDI = EDI + 1;
    }
  }
  else {
    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_byte(BX_SEG_REG_ES, DI, &value8);

    value8 = BX_INP(DX, 1);

    /* no seg override possible */
    write_virtual_byte(BX_SEG_REG_ES, DI, &value8);

    if (BX_CPU_THIS_PTR get_DF())
      DI--;
    else
      DI++;
  }
}

// input word from port to string
void BX_CPU_C::INSW_YwDX(bxInstruction_c *i)
{
  bx_address edi;
  unsigned int incr = 2;

#if BX_SUPPORT_X86_64
  if (i->as64L())
    edi = RDI;
  else
#endif
   if (i->as32L())
    edi = EDI;
  else
    edi = DI;

  Bit16u value16=0;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
  /* If conditions are right, we can transfer IO to physical memory
   * in a batch, rather than one instruction at a time.
   */
  if (i->repUsedL() && !BX_CPU_THIS_PTR async_event) {
    Bit32u wordCount;

#if BX_SUPPORT_X86_64
    if (i->as64L())
      wordCount = RCX; // Truncated to 32bits. (we're only doing 1 page)
    else
#endif
    if (i->as32L())
      wordCount = ECX;
    else
      wordCount =  CX;

    BX_ASSERT(wordCount > 0);
    wordCount = FastRepINSW(i, edi, DX, wordCount);
    if (wordCount)
    {
      // Decrement the ticks count by the number of iterations, minus
      // one, since the main cpu loop will decrement one.  Also,
      // the count is predecremented before examined, so defintely
      // don't roll it under zero.
      BX_TICKN(wordCount-1);

#if BX_SUPPORT_X86_64
      if (i->as64L())
        RCX -= (wordCount-1);
      else
#endif
      if (i->as32L())
        ECX -= (wordCount-1);
      else
        CX  -= (wordCount-1);

      incr = wordCount << 1; // count * 2.
      goto doIncr;
    }
  }
#endif  // (BX_DEBUGGER == 0)
#endif  // #if BX_SupportRepeatSpeedups

  // Write a zero to memory, to trigger any segment or page
  // faults before reading from IO port.
  write_virtual_word(BX_SEG_REG_ES, edi, &value16);

  value16 = BX_INP(DX, 2);

  /* no seg override allowed */
  write_virtual_word(BX_SEG_REG_ES, edi, &value16);
  incr = 2;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr:
#endif
#endif

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RDI = RDI - incr;
    else
      RDI = RDI + incr;
  }
  else
#endif
  if (i->as32L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RDI = EDI - incr;
    else
      RDI = EDI + incr;
  }
  else {
    if (BX_CPU_THIS_PTR get_DF())
      DI = DI - incr;
    else
      DI = DI + incr;
  }
}

// input doubleword from port to string
void BX_CPU_C::INSD_YdDX(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL()))) {
    if (! BX_CPU_THIS_PTR allow_io(DX, 4)) {
      BX_DEBUG(("INSD_YdDX: I/O access not allowed !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

  bx_address edi;

#if BX_SUPPORT_X86_64
  if (i->as64L())
    edi = RDI;
  else
#endif
   if (i->as32L())
    edi = EDI;
  else
    edi = DI;

  Bit32u value32=0;

  // Write a zero to memory, to trigger any segment or page
  // faults before reading from IO port.
  write_virtual_dword(BX_SEG_REG_ES, edi, &value32);

  value32 = BX_INP(DX, 4);

  /* no seg override allowed */
  write_virtual_dword(BX_SEG_REG_ES, edi, &value32);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RDI = RDI - 4;
    else
      RDI = RDI + 4;
  }
  else
#endif
  if (i->as32L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RDI = EDI - 4;
    else
      RDI = EDI + 4;
  }
  else {
    if (BX_CPU_THIS_PTR get_DF())
      DI = DI - 4;
    else
      DI = DI + 4;
  }
}

void BX_CPU_C::OUTSB_DXXb(bxInstruction_c *i)
{
  Bit8u value8;
  bx_address esi;

  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL()))) {
    if (! BX_CPU_THIS_PTR allow_io(DX, 1)) {
      BX_DEBUG(("OUTSB_DXXb: I/O access not allowed !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

#if BX_SUPPORT_X86_64
  if (i->as64L())
    esi = RSI;
  else
#endif
  if (i->as32L())
    esi = ESI;
  else
    esi = SI;

  read_virtual_byte(i->seg(), esi, &value8);

  BX_OUTP(DX, value8, 1);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI--;
    else
      RSI++;
  }
  else
#endif
  if (i->as32L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI--;
    else
      RSI++;
  }
  else {
    if (BX_CPU_THIS_PTR get_DF())
      SI--;
    else
      SI++;
  }
}

// output word string to port
void BX_CPU_C::OUTSW_DXXw(bxInstruction_c *i)
{
  bx_address esi;
  unsigned incr = 2;

  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL()))) {
    if (! BX_CPU_THIS_PTR allow_io(DX, 2)) {
      BX_DEBUG(("OUTSW_DXXw: I/O access not allowed !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

#if BX_SUPPORT_X86_64
  if (i->as64L())
    esi = RSI;
  else
#endif
  if (i->as32L())
    esi = ESI;
  else
    esi = SI;

  Bit16u value16=0;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
  /* If conditions are right, we can transfer IO to physical memory
   * in a batch, rather than one instruction at a time.
   */
  if (i->repUsedL() && !BX_CPU_THIS_PTR async_event) {
    Bit32u wordCount;

#if BX_SUPPORT_X86_64
    if (i->as64L())
      wordCount = RCX; // Truncated to 32bits. (we're only doing 1 page)
    else
#endif
    if (i->as32L())
      wordCount = ECX;
    else
      wordCount = CX;

    BX_ASSERT(wordCount > 0);
    wordCount = FastRepOUTSW(i, i->seg(), esi, DX, wordCount);
    if (wordCount)
    {
      // Decrement eCX.  Note, the main loop will decrement 1 also, so
      // decrement by one less than expected, like the case above.
      BX_TICKN(wordCount-1); // Main cpu loop also decrements one more.

#if BX_SUPPORT_X86_64
      if (i->as64L())
        RCX -= (wordCount-1);
      else
#endif
      if (i->as32L())
        ECX -= (wordCount-1);
      else
        CX  -= (wordCount-1);
      incr = wordCount << 1; // count * 2.
      goto doIncr;
    }
  }

#endif  // (BX_DEBUGGER == 0)
#endif  // #if BX_SupportRepeatSpeedups

  read_virtual_word(i->seg(), esi, &value16);
  BX_OUTP(DX, value16, 2);
  incr = 2;

#if BX_SupportRepeatSpeedups
#if (BX_DEBUGGER == 0)
doIncr:
#endif
#endif

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI = RSI - incr;
    else
      RSI = RSI + incr;
  }
  else
#endif
  if (i->as32L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI = ESI - incr;
    else
      RSI = ESI + incr;
  }
  else {
    if (BX_CPU_THIS_PTR get_DF())
      SI = SI - incr;
    else
      SI = SI + incr;
  }
}

// output doubleword string to port
void BX_CPU_C::OUTSD_DXXd(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR get_VM() || (CPL>BX_CPU_THIS_PTR get_IOPL()))) {
    if (! BX_CPU_THIS_PTR allow_io(DX, 4)) {
      BX_DEBUG(("OUTSD_DXXd: I/O access not allowed !"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

  bx_address esi;

#if BX_SUPPORT_X86_64
  if (i->as64L())
    esi = RSI;
  else
#endif
  if (i->as32L())
    esi = ESI;
  else
    esi =  SI;

  Bit32u value32=0;
  read_virtual_dword(i->seg(), esi, &value32);
  BX_OUTP(DX, value32, 4);

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI = RSI - 4;
    else
      RSI = RSI + 4;
  }
  else
#endif
  if (i->as32L()) {
    if (BX_CPU_THIS_PTR get_DF())
      RSI = ESI - 4;
    else
      RSI = ESI + 4;
  }
  else {
    if (BX_CPU_THIS_PTR get_DF())
      SI = SI - 4;
    else
      SI = SI + 4;
  }
}

void BX_CPU_C::IN_ALIb(bxInstruction_c *i)
{
  AL = BX_CPU_THIS_PTR inp8(i->Ib());
}

void BX_CPU_C::IN_AXIb(bxInstruction_c *i)
{
  AX = BX_CPU_THIS_PTR inp16(i->Ib());
}

void BX_CPU_C::IN_EAXIb(bxInstruction_c *i)
{
  RAX = BX_CPU_THIS_PTR inp32(i->Ib());
}

void BX_CPU_C::OUT_IbAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp8(i->Ib(), AL);
}

void BX_CPU_C::OUT_IbAX(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp16(i->Ib(), AX);
}

void BX_CPU_C::OUT_IbEAX(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp32(i->Ib(), EAX);
}

void BX_CPU_C::IN_ALDX(bxInstruction_c *i)
{
  AL = BX_CPU_THIS_PTR inp8(DX);
}

void BX_CPU_C::IN_AXDX(bxInstruction_c *i)
{
  AX = BX_CPU_THIS_PTR inp16(DX);
}

void BX_CPU_C::IN_EAXDX(bxInstruction_c *i)
{
  RAX = BX_CPU_THIS_PTR inp32(DX);
}

void BX_CPU_C::OUT_DXAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp8(DX, AL);
}

void BX_CPU_C::OUT_DXAX(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp16(DX, AX);
}

void BX_CPU_C::OUT_DXEAX(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR outp32(DX, EAX);
}
