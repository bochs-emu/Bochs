/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2019  The Bochs Project
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM
#include "svm.h"
#endif

#include "iodev/iodev.h"

//
// Repeat Speedups methods
//

#if BX_SUPPORT_REPEAT_SPEEDUPS
Bit32u BX_CPU_C::FastRepINSW(Bit32u dstOff, Bit16u port, Bit32u wordCount)
{
  Bit32u wordsFitDst;
  signed int pointerDelta;
  Bit8u *hostAddrDst;
  unsigned count;
  bx_address laddrDst;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  bx_segment_reg_t *dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES];
  if (dstSegPtr->cache.valid & SegAccessWOK4G) {
    laddrDst = dstOff;
  }
  else {
    if (!(dstSegPtr->cache.valid & SegAccessWOK))
      return 0;
    if ((dstOff | 0xfff) > dstSegPtr->cache.u.segment.limit_scaled)
      return 0;

    laddrDst = get_laddr32(BX_SEG_REG_ES, dstOff);
  }

  // check that the address is word aligned
  if (laddrDst & 1) return 0;

  hostAddrDst = v2h_write_byte(laddrDst, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrDst) return 0;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF()) {
    // Counting downward
    // 1st word must cannot cross page boundary because it is word aligned
    wordsFitDst = (2 + (PAGE_OFFSET(laddrDst))) >> 1;
    pointerDelta = -2;
  }
  else {
    // Counting upward
    wordsFitDst = (0x1000 - PAGE_OFFSET(laddrDst)) >> 1;
    pointerDelta =  2;
  }

  // Restrict word count to the number that will fit in this page.
  if (wordCount > wordsFitDst)
      wordCount = wordsFitDst;

  // If after all the restrictions, there is anything left to do...
  if (wordCount) {
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
        hostAddrDst = bx_devices.bulkIOHostAddr;
        count += bx_devices.bulkIOQuantumsTransferred;
      }
      else {
        WriteHostWordToLittleEndian((Bit16u*)hostAddrDst, temp16);
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

Bit32u BX_CPU_C::FastRepOUTSW(unsigned srcSeg, Bit32u srcOff, Bit16u port, Bit32u wordCount)
{
  Bit32u wordsFitSrc;
  signed int pointerDelta;
  Bit8u *hostAddrSrc;
  unsigned count;
  bx_address laddrSrc;

  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

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

  // check that the address is word aligned
  if (laddrSrc & 1) return 0;

  hostAddrSrc = v2h_read_byte(laddrSrc, USER_PL);
  // Check that native host access was not vetoed for that page
  if (!hostAddrSrc) return 0;

  // See how many words can fit in the rest of this page.
  if (BX_CPU_THIS_PTR get_DF()) {
    // Counting downward
    // 1st word must cannot cross page boundary because it is word aligned
    wordsFitSrc = (2 + (PAGE_OFFSET(laddrSrc))) >> 1;
    pointerDelta = (unsigned) -2;
  }
  else {
    // Counting upward
    wordsFitSrc = (0x1000 - PAGE_OFFSET(laddrSrc)) >> 1;
    pointerDelta =  2;
  }

  // Restrict word count to the number that will fit in this page.
  if (wordCount > wordsFitSrc)
      wordCount = wordsFitSrc;

  // If after all the restrictions, there is anything left to do...
  if (wordCount) {
    for (count=0; count<wordCount; ) {
      bx_devices.bulkIOQuantumsTransferred = 0;
      if (BX_CPU_THIS_PTR get_DF()==0) { // Only do accel for DF=0
        bx_devices.bulkIOHostAddr = hostAddrSrc;
        bx_devices.bulkIOQuantumsRequested = (wordCount - count);
      }
      else
        bx_devices.bulkIOQuantumsRequested = 0;
      Bit16u temp16 = ReadHostWordFromLittleEndian((Bit16u*)hostAddrSrc);
      BX_OUTP(port, temp16, 2);
      if (bx_devices.bulkIOQuantumsTransferred) {
        hostAddrSrc = bx_devices.bulkIOHostAddr;
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

  return 0;
}

#endif

//
// REP INS methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSB_YbDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 1)) {
    BX_DEBUG(("INSB_YbDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB64_YbDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB32_YbDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSB16_YbDX);
  }

  BX_NEXT_TRACE(i);
}

// 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB16_YbDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_virtual_byte_32(BX_SEG_REG_ES, DI); // no lock

  value8 = BX_INP(DX, 1);

  write_RMW_linear_byte(value8);

  if (BX_CPU_THIS_PTR get_DF())
    DI--;
  else
    DI++;
}

// 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB32_YbDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_virtual_byte(BX_SEG_REG_ES, EDI); // no lock

  value8 = BX_INP(DX, 1);

  /* no seg override possible */
  write_RMW_linear_byte(value8);

  if (BX_CPU_THIS_PTR get_DF()) {
    RDI = EDI - 1;
  }
  else {
    RDI = EDI + 1;
  }
}

#if BX_SUPPORT_X86_64

// 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSB64_YbDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit8u value8 = read_RMW_linear_byte(BX_SEG_REG_ES, RDI); // no lock

  value8 = BX_INP(DX, 1);

  write_RMW_linear_byte(value8);

  if (BX_CPU_THIS_PTR get_DF())
    RDI--;
  else
    RDI++;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSW_YwDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 2)) {
    BX_DEBUG(("INSW_YwDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW64_YwDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW32_YwDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSW16_YwDX);
  }

  BX_NEXT_TRACE(i);
}

// 16-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW16_YwDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit16u value16 = read_RMW_virtual_word_32(BX_SEG_REG_ES, DI); // no lock

  value16 = BX_INP(DX, 2);

  write_RMW_linear_word(value16);

  if (BX_CPU_THIS_PTR get_DF())
    DI -= 2;
  else
    DI += 2;
}

// 16-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW32_YwDX(bxInstruction_c *i)
{
  Bit16u value16=0;
  Bit32u edi = EDI;
  unsigned increment = 2;

#if BX_SUPPORT_REPEAT_SPEEDUPS
  /* If conditions are right, we can transfer IO to physical memory
   * in a batch, rather than one instruction at a time.
   */
  if (i->repUsedL() && !BX_CPU_THIS_PTR async_event && !bx_dbg.debugger_active)
  {
    Bit32u wordCount = ECX;
    BX_ASSERT(wordCount > 0);
    wordCount = FastRepINSW(edi, DX, wordCount);
    if (wordCount) {
      // Decrement the ticks count by the number of iterations, minus
      // one, since the main cpu loop will decrement one.  Also,
      // the count is predecremented before examined, so defintely
      // don't roll it under zero.
      BX_TICKN(wordCount-1);
      RCX = ECX - (wordCount-1);
      increment = wordCount << 1; // count * 2.
    }
    else {
      // trigger any segment or page faults before reading from IO port
      value16 = read_RMW_virtual_word(BX_SEG_REG_ES, edi); // no lock

      value16 = BX_INP(DX, 2);

      write_RMW_linear_word(value16);
    }
  }
  else
#endif
  {
    // trigger any segment or page faults before reading from IO port
    value16 = read_RMW_virtual_word_32(BX_SEG_REG_ES, edi); // no lock

    value16 = BX_INP(DX, 2);

    write_RMW_linear_word(value16);
  }

  if (BX_CPU_THIS_PTR get_DF())
    RDI = EDI - increment;
  else
    RDI = EDI + increment;
}

#if BX_SUPPORT_X86_64

// 16-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSW64_YwDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit16u value16 = read_RMW_linear_word(BX_SEG_REG_ES, RDI); // no lock

  value16 = BX_INP(DX, 2);

  write_RMW_linear_word(value16);

  if (BX_CPU_THIS_PTR get_DF())
    RDI -= 2;
  else
    RDI += 2;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_INSD_YdDX(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 4)) {
    BX_DEBUG(("INSD_YdDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD64_YdDX);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD32_YdDX);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RDI); // always clear upper part of RDI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::INSD16_YdDX);
  }

  BX_NEXT_TRACE(i);
}

// 32-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD16_YdDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_virtual_dword_32(BX_SEG_REG_ES, DI); // no lock

  value32 = BX_INP(DX, 4);

  write_RMW_linear_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    DI -= 4;
  else
    DI += 4;
}

// 32-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD32_YdDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_virtual_dword(BX_SEG_REG_ES, EDI); // no lock

  value32 = BX_INP(DX, 4);

  write_RMW_linear_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    RDI = EDI - 4;
  else
    RDI = EDI + 4;
}

#if BX_SUPPORT_X86_64

// 32-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::INSD64_YdDX(bxInstruction_c *i)
{
  // trigger any segment or page faults before reading from IO port
  Bit32u value32 = read_RMW_linear_dword(BX_SEG_REG_ES, RDI); // no lock

  value32 = BX_INP(DX, 4);

  write_RMW_linear_dword(value32);

  if (BX_CPU_THIS_PTR get_DF())
    RDI -= 4;
  else
    RDI += 4;
}

#endif

//
// REP OUTS methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSB_DXXb(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 1)) {
    BX_DEBUG(("OUTSB_DXXb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB64_DXXb);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB32_DXXb);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSB16_DXXb);
  }

  BX_NEXT_TRACE(i);
}

// 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB16_DXXb(bxInstruction_c *i)
{
  Bit8u value8 = read_virtual_byte_32(i->seg(), SI);
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    SI--;
  else
    SI++;
}

// 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB32_DXXb(bxInstruction_c *i)
{
  Bit8u value8 = read_virtual_byte(i->seg(), ESI);
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - 1;
  else
    RSI = ESI + 1;
}

#if BX_SUPPORT_X86_64

// 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSB64_DXXb(bxInstruction_c *i)
{
  Bit8u value8 = read_linear_byte(i->seg(), get_laddr64(i->seg(), RSI));
  BX_OUTP(DX, value8, 1);

  if (BX_CPU_THIS_PTR get_DF())
    RSI--;
  else
    RSI++;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSW_DXXw(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 2)) {
    BX_DEBUG(("OUTSW_DXXw: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW64_DXXw);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW32_DXXw);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSW16_DXXw);
  }

  BX_NEXT_TRACE(i);
}

// 16-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW16_DXXw(bxInstruction_c *i)
{
  Bit16u value16 = read_virtual_word_32(i->seg(), SI);
  BX_OUTP(DX, value16, 2);

  if (BX_CPU_THIS_PTR get_DF())
    SI -= 2;
  else
    SI += 2;
}

// 16-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW32_DXXw(bxInstruction_c *i)
{
  Bit16u value16;
  Bit32u esi = ESI;
  unsigned increment = 2;

#if BX_SUPPORT_REPEAT_SPEEDUPS
  /* If conditions are right, we can transfer IO to physical memory
   * in a batch, rather than one instruction at a time.
   */
  if (i->repUsedL() && !BX_CPU_THIS_PTR async_event && !bx_dbg.debugger_active) {
    Bit32u wordCount = ECX;
    wordCount = FastRepOUTSW(i->seg(), esi, DX, wordCount);
    if (wordCount) {
      // Decrement eCX.  Note, the main loop will decrement 1 also, so
      // decrement by one less than expected, like the case above.
      BX_TICKN(wordCount-1); // Main cpu loop also decrements one more.
      RCX = ECX - (wordCount-1);
      increment = wordCount << 1; // count * 2.
    }
    else {
      value16 = read_virtual_word(i->seg(), esi);
      BX_OUTP(DX, value16, 2);
    }
  }
  else
#endif
  {
    value16 = read_virtual_word(i->seg(), esi);
    BX_OUTP(DX, value16, 2);
  }

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - increment;
  else
    RSI = ESI + increment;
}

#if BX_SUPPORT_X86_64

// 16-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSW64_DXXw(bxInstruction_c *i)
{
  Bit16u value16 = read_linear_word(i->seg(), get_laddr64(i->seg(), RSI));
  BX_OUTP(DX, value16, 2);

  if (BX_CPU_THIS_PTR get_DF())
    RSI -= 2;
  else
    RSI += 2;
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::REP_OUTSD_DXXd(bxInstruction_c *i)
{
  if (! allow_io(i, DX, 4)) {
    BX_DEBUG(("OUTSD_DXXd: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD64_DXXd);
  }
  else
#endif
  if (i->as32L()) {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD32_DXXd);
    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RSI); // always clear upper part of RSI
  }
  else {
    BX_CPU_THIS_PTR repeat(i, &BX_CPU_C::OUTSD16_DXXd);
  }

  BX_NEXT_TRACE(i);
}

// 32-bit operand size, 16-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD16_DXXd(bxInstruction_c *i)
{
  Bit32u value32 = read_virtual_dword_32(i->seg(), SI);
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    SI -= 4;
  else
    SI += 4;
}

// 32-bit operand size, 32-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD32_DXXd(bxInstruction_c *i)
{
  Bit32u value32 = read_virtual_dword(i->seg(), ESI);
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    RSI = ESI - 4;
  else
    RSI = ESI + 4;
}

#if BX_SUPPORT_X86_64

// 32-bit operand size, 64-bit address size
void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUTSD64_DXXd(bxInstruction_c *i)
{
  Bit32u value32 = read_linear_dword(i->seg(), get_laddr64(i->seg(), RSI));
  BX_OUTP(DX, value32, 4);

  if (BX_CPU_THIS_PTR get_DF())
    RSI -= 4;
  else
    RSI += 4;
}

#endif

//
// non repeatable IN/OUT methods
//

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_ALIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("IN_ALIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  AL = BX_INP(port, 1);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_AXIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("IN_AXIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  AX = BX_INP(port, 2);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_EAXIb(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("IN_EAXIb: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  RAX = BX_INP(port, 4);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbAL(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("OUT_IbAL: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, AL, 1);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbAX(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("OUT_IbAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, AX, 2);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_IbEAX(bxInstruction_c *i)
{
  unsigned port = i->Ib();

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("OUT_IbEAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, EAX, 4);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_ALDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("IN_ALDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  AL = BX_INP(port, 1);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_AXDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("IN_AXDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  AX = BX_INP(port, 2);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IN_EAXDX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("IN_EAXDX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  RAX = BX_INP(port, 4);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXAL(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 1)) {
    BX_DEBUG(("OUT_DXAL: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, AL, 1);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXAX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 2)) {
    BX_DEBUG(("OUT_DXAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, AX, 2);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OUT_DXEAX(bxInstruction_c *i)
{
  unsigned port = DX;

  if (! allow_io(i, port, 4)) {
    BX_DEBUG(("OUT_DXEAX: I/O access not allowed !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_OUTP(port, EAX, 4);

  BX_NEXT_TRACE(i);
}

bool BX_CPP_AttrRegparmN(3) BX_CPU_C::allow_io(bxInstruction_c *i, Bit16u port, unsigned len)
{
  /* If CPL <= IOPL, then all IO portesses are accessible.
   * Otherwise, must check the IO permission map on >286.
   * On the 286, there is no IO permissions map */
  
  static bool port_e9_hack_all_rings = SIM->get_param_bool(BXPN_PORT_E9_HACK)->get() && SIM->get_param_bool(BXPN_PORT_E9_HACK_ALL_RINGS)->get();
  if (0xe9 == port && port_e9_hack_all_rings)
    return(1); // port e9 hack can be used by unprivileged code

#if BX_SUPPORT_IODEBUG
  static bool iodebug_all_rings = SIM->get_param_bool(BXPN_IODEBUG_ALL_RINGS)->get();
  if (0x8A00 == (port & 0xfffe) && iodebug_all_rings)
    return(1); // iodebug ports (0x8A00 & 0x8A01) can be used by unprivileged code
#endif /* if BX_SUPPORT_IODEBUG */

  if (BX_CPU_THIS_PTR cr0.get_PE() && (BX_CPU_THIS_PTR get_VM() || (CPL > BX_CPU_THIS_PTR get_IOPL())))
  {
    if (BX_CPU_THIS_PTR tr.cache.valid==0 ||
       (BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_AVAIL_386_TSS &&
        BX_CPU_THIS_PTR tr.cache.type != BX_SYS_SEGMENT_BUSY_386_TSS))
    {
      BX_ERROR(("allow_io(): TR doesn't point to a valid 32bit TSS, TR.TYPE=%u", BX_CPU_THIS_PTR tr.cache.type));
      return(0);
    }

    if (BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled < 103) {
      BX_ERROR(("allow_io(): TR.limit < 103"));
      return(0);
    }

    Bit32u io_base = system_read_word(BX_CPU_THIS_PTR tr.cache.u.segment.base + 102);

    if ((io_base + port/8) >= BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled) {
      BX_DEBUG(("allow_io(): IO port %x (len %d) outside TSS IO permission map (base=%x, limit=%x) #GP(0)",
        port, len, io_base, BX_CPU_THIS_PTR tr.cache.u.segment.limit_scaled));
      return(0);
    }

    Bit16u permission16 = system_read_word(BX_CPU_THIS_PTR tr.cache.u.segment.base + io_base + port/8);

    unsigned bit_index = port & 0x7;
    unsigned mask = (1 << len) - 1;
    if ((permission16 >> bit_index) & mask)
      return(0);
  }

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_IO)) SvmInterceptIO(i, port, len);
  }
#endif

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest)
    VMexit_IO(i, port, len);
#endif

#if BX_X86_DEBUGGER && BX_CPU_LEVEL >= 5
  iobreakpoint_match(port, len);
#endif

  return(1);
}
