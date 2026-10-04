/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2026  The Bochs Project
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

#include "wide_int.h"

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MUL_RAXEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = RAX;
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  Bit128u product_128 = (Bit128u) op1_64 * op2_64;

  /* now write product back to destination */
  RAX = GET128L(product_128);
  RDX = GET128H(product_128);

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_LOGIC_64(GET128L(product_128));
  if(GET128H(product_128) != 0)
  {
    BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_RAXEqR(bxInstruction_c *i)
{
  Bit64s op1_64 = RAX;
  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  Bit128s product_128 = (Bit128s) op1_64 * op2_64;

  /* now write product back to destination */
  RAX = GET128L(product_128);
  RDX = GET128H(product_128);

  /* set eflags:
   * IMUL r/m64: condition for clearing CF & OF:
   *   RDX:RAX = sign-extend of RAX
   */

  SET_FLAGS_OSZAPC_LOGIC_64(GET128L(product_128));

  /* magic compare between RDX:RAX and sign extended RAX */
  if (((Bit64u) GET128H(product_128) + (GET128L(product_128) >> 63)) != 0) {
    BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::DIV_RAXEqR(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());
  if (op2_64 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  Bit128u op1_128 = MAKE128U(RDX, RAX);

  Bit128u quotient_128 = op1_128 / op2_64;

  if (GET128H(quotient_128) != 0)
    exception(BX_DE_EXCEPTION, 0);

  Bit64u quotient_64l = GET128L(quotient_128);
  Bit64u remainder_64 = RAX - quotient_64l * op2_64;

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_64l;
  RDX = remainder_64;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IDIV_RAXEqR(bxInstruction_c *i)
{
  Bit128s op1_128 = MAKE128S(RDX, RAX);

  /* check MIN_INT case */
  if (op1_128 == BIT128S_MIN)
    exception(BX_DE_EXCEPTION, 0);

  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  if (op2_64 == 0) {
    exception(BX_DE_EXCEPTION, 0);
  }

  Bit128s quotient_128 = op1_128 / op2_64;
  Bit64s quotient_64l = (Bit64s) GET128L(quotient_128);

  /* the quotient must fit into 64-bit signed integer */
  if (quotient_128 != quotient_64l)
    exception(BX_DE_EXCEPTION, 0);

  Bit64s remainder_64 = (Bit64s)(RAX - (Bit64u) quotient_64l * (Bit64u) op2_64);

  /* set EFLAGS:
   * IDIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_64l;
  RDX = remainder_64;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GqEqIdR(bxInstruction_c *i)
{
  Bit64s op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64s op2_64 = (Bit32s) i->Id();

  Bit128s product_128 = (Bit128s) op1_64 * op2_64;

  /* now write product back to destination */
  BX_WRITE_64BIT_REG(i->dst(), GET128L(product_128));

  SET_FLAGS_OSZAPC_LOGIC_64(GET128L(product_128));

  if (((Bit64u) GET128H(product_128) + (GET128L(product_128) >> 63)) != 0) {
    BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_GqEqR(bxInstruction_c *i)
{
  Bit64s op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  Bit128s product_128 = (Bit128s) op1_64 * op2_64;

  /* now write product back to destination */
  BX_WRITE_64BIT_REG(i->dst(), GET128L(product_128));

  SET_FLAGS_OSZAPC_LOGIC_64(GET128L(product_128));

  if (((Bit64u) GET128H(product_128) + (GET128L(product_128) >> 63)) != 0) {
    BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
  }

  BX_NEXT_INSTR(i);
}

#endif /* if BX_SUPPORT_X86_64 */
