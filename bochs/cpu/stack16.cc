/////////////////////////////////////////////////////////////////////////
// $Id: stack16.cc,v 1.6.2.1 2002-03-17 08:57:02 bdenney Exp $
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
BX_CPU_C::PUSH_RX(BxInstruction_t *i)
{
  push_16( BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].word.rx );
}

  void
BX_CPU_C::POP_RX(BxInstruction_t *i)
{
  Bit16u rx;

  pop_16(&rx);
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].word.rx = rx;
}

  void
BX_CPU_C::POP_Ew(BxInstruction_t *i)
{
  Bit16u val16;

  pop_16(&val16);

  if (i->mod == 0xc0) {
    BX_WRITE_16BIT_REG(i->rm, val16);
    }
  else {
    // Note: there is one little weirdism here.  When 32bit addressing
    // is used, it is possible to use ESP in the modrm addressing.
    // If used, the value of ESP after the pop is used to calculate
    // the address.
    if (i->as_32 && (i->mod!=0xc0) && (i->rm==4) && (i->base==4)) {
      BX_CPU_CALL_METHOD (i->ResolveModrm, (i));
      }
    write_virtual_word(i->seg, i->rm_addr, &val16);
    }
}

  void
BX_CPU_C::PUSHAD16(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("PUSHAD: not supported on an 8086"));
#else
  Bit32u temp_ESP;
  Bit16u sp;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
    temp_ESP = ESP;
  else
    temp_ESP = SP;


#if BX_CPU_LEVEL >= 2
    if (protected_mode()) {
      if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_ESP, 16) ) {
        BX_PANIC(("PUSHA(): stack doesn't have enough room!"));
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }
    else
#endif
      {
      if (temp_ESP < 16)
        BX_PANIC(("pushad: eSP < 16"));
      }

    sp = SP;

    /* ??? optimize this by using virtual write, all checks passed */
    push_16(AX);
    push_16(CX);
    push_16(DX);
    push_16(BX);
    push_16(sp);
    push_16(BP);
    push_16(SI);
    push_16(DI);
#endif
}

  void
BX_CPU_C::POPAD16(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("POPAD not supported on an 8086"));
#else /* 286+ */

    Bit16u di, si, bp, tmp, bx, dx, cx, ax;

    if (protected_mode()) {
      if ( !can_pop(16) ) {
        BX_PANIC(("pop_a: not enough bytes on stack"));
        exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }
      }

    /* ??? optimize this */
    pop_16(&di);
    pop_16(&si);
    pop_16(&bp);
    pop_16(&tmp); /* value for SP discarded */
    pop_16(&bx);
    pop_16(&dx);
    pop_16(&cx);
    pop_16(&ax);

    DI = di;
    SI = si;
    BP = bp;
    BX = bx;
    DX = dx;
    CX = cx;
    AX = ax;
#endif
}

  void
BX_CPU_C::PUSH_Iw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("PUSH_Iv: not supported on 8086!"));
#else

    Bit16u imm16;

    imm16 = i->Iw;

    push_16(imm16);
#endif
}

  void
BX_CPU_C::PUSH_Ew(BxInstruction_t *i)
{
    Bit16u op1_16;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    push_16(op1_16);
}
