/////////////////////////////////////////////////////////////////////////
// $Id: io.cc,v 1.6 2002-09-01 20:12:09 kevinlawton Exp $
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



  void
BX_CPU_C::INSB_YbDX(BxInstruction_t *i)
{
  Bit8u value8=0;

  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
    if ( !BX_CPU_THIS_PTR allow_io(DX, 1) ) {
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }

  if (i->as_32) {
    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_byte(BX_SEG_REG_ES, EDI, &value8);

    value8 = BX_INP(DX, 1);

    /* no seg override possible */
    write_virtual_byte(BX_SEG_REG_ES, EDI, &value8);

    if (BX_CPU_THIS_PTR eflags.df) {
      EDI = EDI - 1;
      }
    else {
      EDI = EDI + 1;
      }
    }
  else {
    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_byte(BX_SEG_REG_ES, DI, &value8);

    value8 = BX_INP(DX, 1);

    /* no seg override possible */
    write_virtual_byte(BX_SEG_REG_ES, DI, &value8);

    if (BX_CPU_THIS_PTR eflags.df) {
      DI = DI - 1;
      }
    else {
      DI = DI + 1;
      }
    }
}

  void
BX_CPU_C::INSW_YvDX(BxInstruction_t *i)
  // input word/doubleword from port to string
{
  Bit32u edi;
  unsigned int incr;

  if (i->as_32)
    edi = EDI;
  else
    edi = DI;

  if (i->os_32) {
    Bit32u value32=0;

    if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
      if ( !BX_CPU_THIS_PTR allow_io(DX, 4) ) {
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      }

    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_dword(BX_SEG_REG_ES, edi, &value32);

    value32 = BX_INP(DX, 4);

    /* no seg override allowed */
    write_virtual_dword(BX_SEG_REG_ES, edi, &value32);
    incr = 4;
    }
  else {
    Bit16u value16=0;

    if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
      if ( !BX_CPU_THIS_PTR allow_io(DX, 2) ) {
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      }

#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u wordCount;
      bx_segment_reg_t *dstSegPtr;

      if (i->as_32)
        wordCount = ECX;
      else
        wordCount = CX;
      dstSegPtr = &BX_CPU_THIS_PTR sregs[BX_SREG_ES];
      // Do segment checks for the 1st word.  We do not want to
      // trip an exception beyond this, because the address would
      // be incorrect.  After we know how many bytes we will directly
      // transfer, we can do the full segment limit check ourselves
      // without generating an exception.
      write_virtual_checks(dstSegPtr, edi, 2);
      if (wordCount) {
        Bit32u laddr, paddr, wordsCanFit;
        Bit8u *hostAddrDst;

        laddr = dstSegPtr->cache.u.segment.base + edi;
        if (BX_CPU_THIS_PTR cr0.pg)
          paddr = dtranslate_linear(laddr, CPL==3, BX_WRITE);
        else
          paddr = laddr;
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddr = A20ADDR(paddr);

        hostAddrDst = BX_CPU_THIS_PTR mem->getHostMemAddr(paddr, BX_WRITE);

        // Check that native host access was not vetoed for that page, and
        // that the address is word aligned.
        if ( hostAddrDst && ! (paddr & 1) ) {
          // See how many words can fit in the rest of this page.
          wordsCanFit = (0x1000 - (paddr & 0xfff)) >> 1;
          // Restrict word count to the number that will fit in this page.
          if (wordCount > wordsCanFit)
            wordCount = wordsCanFit;

          // If after all the restrictions, there is anything left to do...
          if (wordCount) {
            unsigned transferLen;
            Bit32u roomDst;
            unsigned j;
            unsigned pointerDelta;

            transferLen = wordCount<<1; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(dstSegPtr->cache.valid & SegAccessWOK) ) {
              goto noAcceleration;
              }
            roomDst = (dstSegPtr->cache.u.segment.limit_scaled - edi) + 1; 
            if ( roomDst < transferLen ) {
              goto noAcceleration;
              }

            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -2;
            else
              pointerDelta =  2;
            for (j=0; j<wordCount; ) {
              Bit16u temp16;
              temp16 = BX_INP(DX, 2);
              * (Bit16u *) hostAddrDst = temp16;
              hostAddrDst += pointerDelta;
              j++;
              BX_TICK1();
              if ( BX_CPU_THIS_PTR async_event )
                break;
              }
            wordCount = j;
            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (wordCount-1);
            else
              CX  -= (wordCount-1);
            incr = wordCount << 1; // count * 2.
            goto doIncr;
            }
          }
        }
      }
#endif // __i386__
#endif

noAcceleration:

    // Write a zero to memory, to trigger any segment or page
    // faults before reading from IO port.
    write_virtual_word(BX_SEG_REG_ES, edi, &value16);

    value16 = BX_INP(DX, 2);

    /* no seg override allowed */
    write_virtual_word(BX_SEG_REG_ES, edi, &value16);
    incr = 2;
    }

#if (BX_DEBUGGER == 0)
doIncr:
#endif

  if (i->as_32) {
    if (BX_CPU_THIS_PTR eflags.df)
      EDI = EDI - incr;
    else
      EDI = EDI + incr;
    }
  else {
    if (BX_CPU_THIS_PTR eflags.df)
      DI = DI - incr;
    else
      DI = DI + incr;
    }
}

  void
BX_CPU_C::OUTSB_DXXb(BxInstruction_t *i)
{
  unsigned seg;
  Bit8u value8;
  Bit32u esi;

  if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
    if ( !BX_CPU_THIS_PTR allow_io(DX, 1) ) {
      exception(BX_GP_EXCEPTION, 0, 0);
      }
    }

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

  if (i->as_32)
    esi = ESI;
  else
    esi = SI;

  read_virtual_byte(seg, esi, &value8);

  BX_OUTP(DX, value8, 1);

  if (i->as_32) {
    if (BX_CPU_THIS_PTR eflags.df)
      ESI -= 1;
    else
      ESI += 1;
    }
  else {
    if (BX_CPU_THIS_PTR eflags.df)
      SI -= 1;
    else
      SI += 1;
    }
}

  void
BX_CPU_C::OUTSW_DXXv(BxInstruction_t *i)
  // output word/doubleword string to port
{
  unsigned seg;
  Bit32u esi;
  unsigned int incr;

  if (!BX_NULL_SEG_REG(i->seg)) {
    seg = i->seg;
    }
  else {
    seg = BX_SEG_REG_DS;
    }

  if (i->as_32)
    esi = ESI;
  else
    esi = SI;

  if (i->os_32) {
    Bit32u value32;

    if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
      if ( !BX_CPU_THIS_PTR allow_io(DX, 4) ) {
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      }

    read_virtual_dword(seg, esi, &value32);

    BX_OUTP(DX, value32, 4);
    incr = 4;
    }
  else {
    Bit16u value16;

    if (BX_CPU_THIS_PTR cr0.pe && (BX_CPU_THIS_PTR eflags.vm || (CPL>IOPL))) {
      if ( !BX_CPU_THIS_PTR allow_io(DX, 2) ) {
        exception(BX_GP_EXCEPTION, 0, 0);
        }
      }

#if (BX_DEBUGGER == 0)
#if (defined(__i386__) && __i386__)
    /* If conditions are right, we can transfer IO to physical memory
     * in a batch, rather than one instruction at a time.
     */
    if (i->rep_used && !BX_CPU_THIS_PTR async_event) {
      Bit32u wordCount;
      bx_segment_reg_t *srcSegPtr;

      if (i->as_32)
        wordCount = ECX;
      else
        wordCount = CX;
      srcSegPtr = &BX_CPU_THIS_PTR sregs[seg];
      // Do segment checks for the 1st word.  We do not want to
      // trip an exception beyond this, because the address would
      // be incorrect.  After we know how many bytes we will directly
      // transfer, we can do the full segment limit check ourselves
      // without generating an exception.
      read_virtual_checks(srcSegPtr, esi, 2);
      if (wordCount) {
        Bit32u laddr, paddr, wordsCanFit;
        Bit8u *hostAddrSrc;

        laddr = srcSegPtr->cache.u.segment.base + esi;
        if (BX_CPU_THIS_PTR cr0.pg)
          paddr = dtranslate_linear(laddr, CPL==3, BX_READ);
        else
          paddr = laddr;
        // If we want to write directly into the physical memory array,
        // we need the A20 address.
        paddr = A20ADDR(paddr);

        hostAddrSrc = BX_CPU_THIS_PTR mem->getHostMemAddr(paddr, BX_READ);

        // Check that native host access was not vetoed for that page, and
        // that the address is word aligned.
        if ( hostAddrSrc && ! (paddr & 1) ) {
          // See how many words can fit in the rest of this page.
          wordsCanFit = (0x1000 - (paddr & 0xfff)) >> 1;
          // Restrict word count to the number that will fit in this page.
          if (wordCount > wordsCanFit)
            wordCount = wordsCanFit;

          // If after all the restrictions, there is anything left to do...
          if (wordCount) {
            unsigned transferLen;
            Bit32u roomSrc;
            unsigned j;
            unsigned pointerDelta;

            transferLen = wordCount<<1; // Number bytes to transfer.

            // Before we copy memory, we need to make sure that the segments
            // allow the accesses up to the given source and dest offset.  If
            // the cache.valid bits have SegAccessWOK and ROK, we know that
            // the cache is valid for those operations, and that the segments
            // are non-expand down (thus we can make a simple limit check).
            if ( !(srcSegPtr->cache.valid & SegAccessROK) ) {
              goto noAcceleration;
              }
            roomSrc = (srcSegPtr->cache.u.segment.limit_scaled - esi) + 1; 
            if ( roomSrc < transferLen ) {
              goto noAcceleration;
              }

            if (BX_CPU_THIS_PTR eflags.df)
              pointerDelta = (unsigned) -2;
            else
              pointerDelta =  2;
            for (j=0; j<wordCount; ) {
              Bit16u temp16;
              temp16 = * (Bit16u *) hostAddrSrc;
              hostAddrSrc += pointerDelta;
              BX_OUTP(DX, temp16, 2);
              j++;
              BX_TICK1();
              if ( BX_CPU_THIS_PTR async_event )
                break;
              }
            wordCount = j;
            // Decrement eCX.  Note, the main loop will decrement 1 also, so
            // decrement by one less than expected, like the case above.
            if (i->as_32)
              ECX -= (wordCount-1);
            else
              CX  -= (wordCount-1);
            incr = wordCount << 1; // count * 2.
            goto doIncr;
            }
          }
        }
      }
#endif // __i386__
#endif

noAcceleration:

    read_virtual_word(seg, esi, &value16);

    BX_OUTP(DX, value16, 2);
    incr = 2;
    }

#if (BX_DEBUGGER == 0)
doIncr:
#endif

  if (i->as_32) {
    if (BX_CPU_THIS_PTR eflags.df)
      ESI = ESI - incr;
    else
      ESI = ESI + incr;
    }
  else {
    if (BX_CPU_THIS_PTR eflags.df)
      SI = SI - incr;
    else
      SI = SI + incr;
    }
}


  void
BX_CPU_C::IN_ALIb(BxInstruction_t *i)
{
  Bit8u al, imm8;

  imm8 = i->Ib;

  al = BX_CPU_THIS_PTR inp8(imm8);

  AL = al;
}

  void
BX_CPU_C::IN_eAXIb(BxInstruction_t *i)
{
  Bit8u imm8;


  imm8 = i->Ib;

#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    Bit32u eax;

    eax = BX_CPU_THIS_PTR inp32(imm8);
    EAX = eax;
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    {
    Bit16u ax;

    ax = BX_CPU_THIS_PTR inp16(imm8);
    AX = ax;
    }
}

  void
BX_CPU_C::OUT_IbAL(BxInstruction_t *i)
{
  Bit8u al, imm8;

  imm8 = i->Ib;

  al = AL;

  BX_CPU_THIS_PTR outp8(imm8, al);
}

  void
BX_CPU_C::OUT_IbeAX(BxInstruction_t *i)
{
  Bit8u imm8;

  imm8 = i->Ib;

#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    BX_CPU_THIS_PTR outp32(imm8, EAX);
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    {
    BX_CPU_THIS_PTR outp16(imm8, AX);
    }
}

  void
BX_CPU_C::IN_ALDX(BxInstruction_t *i)
{
  Bit8u al;

  al = BX_CPU_THIS_PTR inp8(DX);

  AL = al;
}

  void
BX_CPU_C::IN_eAXDX(BxInstruction_t *i)
{
#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    Bit32u eax;

    eax = BX_CPU_THIS_PTR inp32(DX);
    EAX = eax;
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    {
    Bit16u ax;

    ax = BX_CPU_THIS_PTR inp16(DX);
    AX = ax;
    }
}

  void
BX_CPU_C::OUT_DXAL(BxInstruction_t *i)
{
  Bit16u dx;
  Bit8u al;

  dx = DX;
  al = AL;

  BX_CPU_THIS_PTR outp8(dx, al);
}

  void
BX_CPU_C::OUT_DXeAX(BxInstruction_t *i)
{
  Bit16u dx;

  dx = DX;

#if BX_CPU_LEVEL > 2
  if (i->os_32) {
    BX_CPU_THIS_PTR outp32(dx, EAX);
    }
  else
#endif /* BX_CPU_LEVEL > 2 */
    {
    BX_CPU_THIS_PTR outp16(dx, AX);
    }
}
