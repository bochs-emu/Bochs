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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MUL_EAXEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = EAX;
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  Bit64u product_64  = ((Bit64u) op1_32) * ((Bit64u) op2_32);
  Bit32u product_32l = GET32L(product_64);
  Bit32u product_32h = GET32H(product_64);

  /* now write product back to destination */
  RAX = product_32l;
  RDX = product_32h;

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_LOGIC_32(product_32l);
  if(product_32h != 0)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_EAXEdR(bxInstruction_c *i)
{
  Bit32s op1_32 = EAX;
  Bit32s op2_32 = BX_READ_32BIT_REG(i->src());

  Bit64s product_64  = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  Bit32u product_32l = GET32L(product_64);
  Bit32u product_32h = GET32H(product_64);

  /* now write product back to destination */
  RAX = product_32l;
  RDX = product_32h;

  /* set eflags:
   * IMUL r/m32: condition for clearing CF & OF:
   *   EDX:EAX = sign-extend of EAX
   */
  SET_FLAGS_OSZAPC_LOGIC_32(product_32l);
  if(product_64 != (Bit32s)product_64)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DIV_EAXEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
  if (op2_32 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  Bit64u op1_64 = (((Bit64u) EDX) << 32) + ((Bit64u) EAX);

  Bit64u quotient_64  = op1_64 / op2_32;
  Bit32u remainder_32 = (Bit32u) (op1_64 % op2_32);
  Bit32u quotient_32l = (Bit32u) (quotient_64 & 0xFFFFFFFF);

  if (quotient_64 != quotient_32l)
  {
    exception(BX_DE_EXCEPTION, 0);
  }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_32l;
  RDX = remainder_32;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IDIV_EAXEdR(bxInstruction_c *i)
{
  Bit64s op1_64 = (((Bit64u) EDX) << 32) | ((Bit64u) EAX);

  /* check MIN_INT case */
  if (op1_64 == ((Bit64s)BX_CONST64(0x8000000000000000)))
    exception(BX_DE_EXCEPTION, 0);

  Bit32s op2_32 = BX_READ_32BIT_REG(i->src());

  if (op2_32 == 0)
    exception(BX_DE_EXCEPTION, 0);

  Bit64s quotient_64  = op1_64 / op2_32;
  Bit32s remainder_32 = (Bit32s) (op1_64 % op2_32);
  Bit32s quotient_32l = (Bit32s) (quotient_64 & 0xFFFFFFFF);

  if (quotient_64 != quotient_32l)
  {
    exception(BX_DE_EXCEPTION, 0);
  }

  /* set EFLAGS:
   * IDIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = (Bit32u) quotient_32l;
  RDX = (Bit32u) remainder_32;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GdEdIdR(bxInstruction_c *i)
{
  Bit32s op2_32 = BX_READ_32BIT_REG(i->src());
  Bit32s op3_32 = i->Id();

  Bit64s product_64 = ((Bit64s) op2_32) * ((Bit64s) op3_32);
  Bit32u product_32 = (Bit32u)(product_64 & 0xFFFFFFFF);

  /* now write product back to destination */
  BX_WRITE_32BIT_REGZ(i->dst(), product_32);

  /* set eflags:
   * IMUL r32,r/m32,imm32: condition for clearing CF & OF:
   *   result exactly fits within r32
   */
  SET_FLAGS_OSZAPC_LOGIC_32(product_32);
  if(product_64 != (Bit32s) product_64)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GdEdR(bxInstruction_c *i)
{
  Bit32s op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32s op2_32 = BX_READ_32BIT_REG(i->src());

  Bit64s product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  Bit32u product_32 = (Bit32u)(product_64 & 0xFFFFFFFF);

  /* now write product back to destination */
  BX_WRITE_32BIT_REGZ(i->dst(), product_32);

  /* set eflags:
   * IMUL r32,r/m32: condition for clearing CF & OF:
   *   result exactly fits within r32
   */
  SET_FLAGS_OSZAPC_LOGIC_32(product_32);
  if(product_64 != (Bit32s) product_64)
  {
    ASSERT_FLAGS_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}
