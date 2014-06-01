/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MUL_ALEbR(bxInstruction_c *i)
{
  Bit8u op1 = AL;
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

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

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_ALEbR(bxInstruction_c *i)
{
  Bit8s op1 = AL;
  Bit8s op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

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

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DIV_ALEbR(bxInstruction_c *i)
{
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  if (op2 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  Bit16u op1 = AX;

  Bit16u quotient_16 = op1 / op2;
  Bit8u remainder_8 = op1 % op2;
  Bit8u quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l)
  {
    exception(BX_DE_EXCEPTION, 0);
  }

  /* now write quotient back to destination */
  AL = quotient_8l;
  AH = remainder_8;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IDIV_ALEbR(bxInstruction_c *i)
{
  Bit16s op1 = AX;

  /* check MIN_INT case */
  if (op1 == ((Bit16s)0x8000))
    exception(BX_DE_EXCEPTION, 0);

  Bit8s op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  if (op2 == 0)
    exception(BX_DE_EXCEPTION, 0);

  Bit16s quotient_16 = op1 / op2;
  Bit8s remainder_8 = op1 % op2;
  Bit8s quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l)
    exception(BX_DE_EXCEPTION, 0);

  /* now write quotient back to destination */
  AL = quotient_8l;
  AH = remainder_8;

  BX_NEXT_INSTR(i);
}
