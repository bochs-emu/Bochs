/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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

#if BX_SUPPORT_APX

#include "wide_int.h"
#include "decoder/ia_opcodes.h"

// EVEX map4 one-operand MUL/IMUL with NF=1: same as legacy, but flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_MUL_RAXEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = RAX;
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  Bit128u product_128 = (Bit128u) op1_64 * op2_64;

  /* now write product back to destination */
  RAX = GET128L(product_128);
  RDX = GET128H(product_128);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_IMUL_RAXEqR(bxInstruction_c *i)
{
  Bit64s op1_64 = RAX;
  Bit64s op2_64 = BX_READ_64BIT_REG(i->src());

  Bit128s product_128 = (Bit128s) op1_64 * op2_64;

  /* now write product back to destination */
  RAX = GET128L(product_128);
  RDX = GET128H(product_128);

  BX_NEXT_INSTR(i);
}

// EVEX map4 IMUL AF and 69/6B:
//   IMUL_BqGqEqR : AF ND=1 (dst = vvvv) or ND=0 NF=1 (dst = reg), dst = src1 * src2
//                  69/6B except ND=0 NF=0, dst = src1 * imm
//                  flags updated unless NF=1

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_BqGqEqR(bxInstruction_c *i)
{
  Bit64s op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64s op2_64;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_IMUL_GqEqId || ia_opcode == BX_IA_APX_IMUL_GqEqsIb)
    op2_64 = (Bit32s) i->Id();
  else
    op2_64 = BX_READ_64BIT_REG(i->src2());

  Bit128s product_128 = (Bit128s) op1_64 * op2_64;

  /* now write product back to destination */
  BX_WRITE_64BIT_REG(i->dst(), GET128L(product_128));

  if (! i->getApxEvexNF()) {
    SET_FLAGS_OSZAPC_LOGIC_64(GET128L(product_128));
    if (((Bit64u) GET128H(product_128) + (GET128L(product_128) >> 63)) != 0) {
      BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
    }
  }

  BX_NEXT_INSTR(i);
}

#endif
