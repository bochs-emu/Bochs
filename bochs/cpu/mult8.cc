/////////////////////////////////////////////////////////////////////////
// $Id: mult8.cc,v 1.25 2007-12-20 20:58:37 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::MUL_ALEb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op1 = AL;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    op2 = read_virtual_byte(i->seg(), RMAddr(i));
  }

  Bit32u product_16 = ((Bit16u) op1) * ((Bit16u) op2);

  Bit8u product_8l = (product_16 & 0xFF);
  Bit8u product_8h =  product_16 >> 8;

  /* now write product back to destination */
  AX = product_16;

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_LOGIC_8(product_8l);
  if(product_8h != 0)
  {
    ASSERT_FLAGS_OxxxxC();
  }
}

void BX_CPU_C::IMUL_ALEb(bxInstruction_c *i)
{
  Bit8s op2, op1;

  op1 = AL;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    op2 = (Bit8s) read_virtual_byte(i->seg(), RMAddr(i));
  }

  Bit16s product_16 = op1 * op2;
  Bit8u  product_8 = (product_16 & 0xFF);

  /* now write product back to destination */
  AX = product_16;

  /* set EFLAGS:
   * IMUL r/m8: condition for clearing CF & OF:
   *   AX = sign-extend of AL to 16 bits
   */

  SET_FLAGS_OSZAPC_LOGIC_8(product_8);
  if(product_16 != (Bit8s) product_16)
  {
    ASSERT_FLAGS_OxxxxC();
  }
}

void BX_CPU_C::DIV_ALEb(bxInstruction_c *i)
{
  Bit8u op2, quotient_8l, remainder_8;
  Bit16u quotient_16, op1;

  op1 = AX;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    op2 = read_virtual_byte(i->seg(), RMAddr(i));
  }

  if (op2 == 0) {
    exception(BX_DE_EXCEPTION, 0, 0);
  }

  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l)
  {
    exception(BX_DE_EXCEPTION, 0, 0);
  }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

#if INTEL_DIV_FLAG_BUG == 1
  assert_CF();
#endif

  /* now write quotient back to destination */
  AL = quotient_8l;
  AH = remainder_8;
}

void BX_CPU_C::IDIV_ALEb(bxInstruction_c *i)
{
  Bit8s op2, quotient_8l, remainder_8;
  Bit16s quotient_16, op1;

  op1 = AX;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    op2 = (Bit8s) read_virtual_byte(i->seg(), RMAddr(i));
  }

  if (op2 == 0)
    exception(BX_DE_EXCEPTION, 0, 0);

  /* check MIN_INT divided by -1 case */
  if ((op1 == ((Bit16s)0x8000)) && (op2 == -1))
    exception(BX_DE_EXCEPTION, 0, 0);

  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l)
    exception(BX_DE_EXCEPTION, 0, 0);

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

#if INTEL_DIV_FLAG_BUG == 1
  assert_CF();
#endif

  /* now write quotient back to destination */
  AL = quotient_8l;
  AH = remainder_8;
}
