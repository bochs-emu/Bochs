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
BX_CPU_C::MUL_ALEb(BxInstruction_t *i)
{
  Bit8u op2, op1;
  Bit16u product_16;
  Boolean temp_flag;

  op1 = AL;

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  product_16 = op1 * op2;

  /* set EFLAGS:
   * MUL affects the following flags: C,O
   */

  temp_flag = ((product_16 & 0xFF00) != 0);
  SET_FLAGS_OxxxxC(temp_flag, temp_flag);

  /* now write product back to destination */

  AX = product_16;
}


  void
BX_CPU_C::IMUL_ALEb(BxInstruction_t *i)
{
  Bit8s op2, op1;
  Bit16s product_16;
  Bit16u upper_bits;


  op1 = AL;

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, (Bit8u *) &op2);
    }

  product_16 = op1 * op2;

  /* now write product back to destination */

  AX = product_16;

  /* set EFLAGS:
   * IMUL affects the following flags: C,O
   * IMUL r/m8: condition for clearing CF & OF:
   *   AL = sign-extend of AL to 16 bits
   */
  upper_bits = AX & 0xff80;
  if (upper_bits==0xff80  ||  upper_bits==0x0000) {
    SET_FLAGS_OxxxxC(0, 0);
    }
  else {
    SET_FLAGS_OxxxxC(1, 1);
    }
}



  void
BX_CPU_C::DIV_ALEb(BxInstruction_t *i)
{
  Bit8u op2, quotient_8l, remainder_8;
  Bit16u quotient_16, op1;


  op1 = AX;

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  if (op2 == 0) {
    exception(BX_DE_EXCEPTION, 0, 0);
    }
  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l) {
    exception(BX_DE_EXCEPTION, 0, 0);
    }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

#if INTEL_DIV_FLAG_BUG == 1
    set_CF(1);
#endif

  /* now write quotient back to destination */

  AL = quotient_8l;
  AH = remainder_8;
}


  void
BX_CPU_C::IDIV_ALEb(BxInstruction_t *i)
{
  Bit8s op2, quotient_8l, remainder_8;
  Bit16s quotient_16, op1;


  op1 = AX;


  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, (Bit8u *) &op2);
    }

  if (op2 == 0) {
    exception(BX_DE_EXCEPTION, 0, 0);
    }

  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l) {
BX_INFO(("quotient_16: %04x, remainder_8: %02x, quotient_8l: %02x",
  (unsigned) quotient_16, (unsigned) remainder_8, (unsigned) quotient_8l));
AL = quotient_8l;
AH = remainder_8;
BX_INFO(("AH: %02x, AL: %02x", (unsigned) AH, (unsigned) AL));
    exception(BX_DE_EXCEPTION, 0, 0);
    }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

#if INTEL_DIV_FLAG_BUG == 1
    set_CF(1);
#endif

  /* now write quotient back to destination */

  AL = quotient_8l;
  AH = remainder_8;
}
