/////////////////////////////////////////////////////////////////////////
// $Id: mult16.cc,v 1.5.2.1 2002-03-17 08:57:01 bdenney Exp $
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
BX_CPU_C::MUL_AXEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, product_16h, product_16l;
    Bit32u product_32;
    Boolean temp_flag;

    op1_16 = AX;

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    product_32 = ((Bit32u) op1_16) * ((Bit32u) op2_16);

    product_16l = (product_32 & 0xFFFF);
    product_16h = product_32 >> 16;

    /* now write product back to destination */

    AX = product_16l;
    DX = product_16h;

    /* set eflags:
     * MUL affects the following flags: C,O
     */

    temp_flag = (product_16h != 0);
    SET_FLAGS_OxxxxC(temp_flag, temp_flag);
}



  void
BX_CPU_C::IMUL_AXEw(BxInstruction_t *i)
{
    Bit16s op1_16, op2_16;
    Bit32s product_32;
    Bit16u product_16h, product_16l;

    op1_16 = AX;

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, (Bit16u *) &op2_16);
      }

    product_32 = ((Bit32s) op1_16) * ((Bit32s) op2_16);

    product_16l = (product_32 & 0xFFFF);
    product_16h = product_32 >> 16;

    /* now write product back to destination */

    AX = product_16l;
    DX = product_16h;

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r/m16: condition for clearing CF & OF:
     *   DX:AX = sign-extend of AX
     */

    if ( (DX==0xffff) && (AX & 0x8000) ) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else if ( (DX==0x0000) && (AX < 0x8000) ) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
}


  void
BX_CPU_C::DIV_AXEw(BxInstruction_t *i)
{
    Bit16u op2_16, remainder_16, quotient_16l;
    Bit32u op1_32, quotient_32;

    op1_32 = (((Bit32u) DX) << 16) | ((Bit32u) AX);

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    if (op2_16 == 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }
    quotient_32 = op1_32 / op2_16;
    remainder_16 = op1_32 % op2_16;
    quotient_16l = quotient_32 & 0xFFFF;

    if (quotient_32 != quotient_16l) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }

    /* set EFLAGS:
     * DIV affects the following flags: O,S,Z,A,P,C are undefined
     */

#if INTEL_DIV_FLAG_BUG == 1
    set_CF(1);
#endif

    /* now write quotient back to destination */

    AX = quotient_16l;
    DX = remainder_16;
}


  void
BX_CPU_C::IDIV_AXEw(BxInstruction_t *i)
{
    Bit16s op2_16, remainder_16, quotient_16l;
    Bit32s op1_32, quotient_32;

    op1_32 = ((((Bit32u) DX) << 16) | ((Bit32u) AX));

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, (Bit16u *) &op2_16);
      }

    if (op2_16 == 0) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }
    quotient_32 = op1_32 / op2_16;
    remainder_16 = op1_32 % op2_16;
    quotient_16l = quotient_32 & 0xFFFF;

    if (quotient_32 != quotient_16l) {
      exception(BX_DE_EXCEPTION, 0, 0);
      }

    /* set EFLAGS:
     * IDIV affects the following flags: O,S,Z,A,P,C are undefined
     */

#if INTEL_DIV_FLAG_BUG == 1
    set_CF(1);
#endif

    /* now write quotient back to destination */

    AX = quotient_16l;
    DX = remainder_16;
}


  void
BX_CPU_C::IMUL_GwEwIw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("IMUL_GvEvIv() unsupported on 8086!"));
#else


    Bit16u product_16l;
    Bit16s op2_16, op3_16;
    Bit32s product_32;

    op3_16 = i->Iw;

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, (Bit16u *) &op2_16);
      }

    product_32 = op2_16 * op3_16;

    product_16l = (product_32 & 0xFFFF);

    /* now write product back to destination */
    BX_WRITE_16BIT_REG(i->nnn, product_16l);

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */

    if (product_32 > -32768  && product_32 < 32767) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
#endif
}

  void
BX_CPU_C::IMUL_GwEw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("IMUL_GvEv() unsupported on 8086!"));
#else

    Bit16u product_16l;
    Bit16s op1_16, op2_16;
    Bit32s product_32;

    /* op2 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, (Bit16u *) &op2_16);
      }

    op1_16 = BX_READ_16BIT_REG(i->nnn);

    product_32 = op1_16 * op2_16;

    product_16l = (product_32 & 0xFFFF);

    /* now write product back to destination */
    BX_WRITE_16BIT_REG(i->nnn, product_16l);

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */

    if (product_32 > -32768  && product_32 < 32767) {
      SET_FLAGS_OxxxxC(0, 0);
      }
    else {
      SET_FLAGS_OxxxxC(1, 1);
      }
#endif
}
