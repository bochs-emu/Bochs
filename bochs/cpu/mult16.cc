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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MUL_AXEwR(bxInstruction_c *i)
{
  Bit16u op1_16 = AX;
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  Bit32u product_32  = ((Bit32u) op1_16) * ((Bit32u) op2_16);
  Bit16u product_16l = (product_32 & 0xFFFF);
  Bit16u product_16h =  product_32 >> 16;

  /* now write product back to destination */
  AX = product_16l;
  DX = product_16h;

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_LOGIC_16(product_16l);
  if(product_16h != 0)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_AXEwR(bxInstruction_c *i)
{
  Bit16s op1_16 = AX;
  Bit16s op2_16 = BX_READ_16BIT_REG(i->src());

  Bit32s product_32  = ((Bit32s) op1_16) * ((Bit32s) op2_16);
  Bit16u product_16l = (product_32 & 0xFFFF);
  Bit16u product_16h = product_32 >> 16;

  /* now write product back to destination */
  AX = product_16l;
  DX = product_16h;

  /* set eflags:
   * IMUL r/m16: condition for clearing CF & OF:
   *   DX:AX = sign-extend of AX
   */
  SET_FLAGS_OSZAPC_LOGIC_16(product_16l);
  if(product_32 != (Bit16s)product_32)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DIV_AXEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());
  if (op2_16 == 0)
    exception(BX_DE_EXCEPTION, 0);

  Bit32u op1_32 = (((Bit32u) DX) << 16) | ((Bit32u) AX);

  Bit32u quotient_32  = op1_32 / op2_16;
  Bit16u remainder_16 = op1_32 % op2_16;
  Bit16u quotient_16l = quotient_32 & 0xFFFF;

  if (quotient_32 != quotient_16l)
    exception(BX_DE_EXCEPTION, 0);

  /* now write quotient back to destination */
  AX = quotient_16l;
  DX = remainder_16;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IDIV_AXEwR(bxInstruction_c *i)
{
  Bit32s op1_32 = ((((Bit32u) DX) << 16) | ((Bit32u) AX));

  /* check MIN_INT case */
  if (op1_32 == ((Bit32s)0x80000000))
    exception(BX_DE_EXCEPTION, 0);

  Bit16s op2_16 = BX_READ_16BIT_REG(i->src());

  if (op2_16 == 0)
    exception(BX_DE_EXCEPTION, 0);

  Bit32s quotient_32  = op1_32 / op2_16;
  Bit16s remainder_16 = op1_32 % op2_16;
  Bit16s quotient_16l = quotient_32 & 0xFFFF;

  if (quotient_32 != quotient_16l)
    exception(BX_DE_EXCEPTION, 0);

  /* now write quotient back to destination */
  AX = quotient_16l;
  DX = remainder_16;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GwEwIwR(bxInstruction_c *i)
{
  Bit16s op2_16 = BX_READ_16BIT_REG(i->src());
  Bit16s op3_16 = i->Iw();

  Bit32s product_32  = op2_16 * op3_16;
  Bit16u product_16 = (product_32 & 0xFFFF);

  /* now write product back to destination */
  BX_WRITE_16BIT_REG(i->dst(), product_16);

  /* set eflags:
   * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
   *   result exactly fits within r16
   */
  SET_FLAGS_OSZAPC_LOGIC_16(product_16);
  if(product_32 != (Bit16s) product_32)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GwEwR(bxInstruction_c *i)
{
  Bit16s op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit16s op2_16 = BX_READ_16BIT_REG(i->src());

  Bit32s product_32 = op1_16 * op2_16;
  Bit16u product_16 = (product_32 & 0xFFFF);

  /* now write product back to destination */
  BX_WRITE_16BIT_REG(i->dst(), product_16);

  /* set eflags:
   * IMUL r16,r/m16: condition for clearing CF & OF:
   *   result exactly fits within r16
   */
  SET_FLAGS_OSZAPC_LOGIC_16(product_16);
  if(product_32 != (Bit16s) product_32)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}
