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

#include "decoder/ia_opcodes.h"

// EVEX map4 one-operand MUL/IMUL with NF=1: same as legacy, but flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_MUL_EAXEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = EAX;
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  Bit64u product_64 = ((Bit64u) op1_32) * ((Bit64u) op2_32);

  /* now write product back to destination */
  RAX = GET32L(product_64);
  RDX = GET32H(product_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_IMUL_EAXEdR(bxInstruction_c *i)
{
  Bit32s op1_32 = EAX;
  Bit32s op2_32 = BX_READ_32BIT_REG(i->src());

  Bit64s product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);

  /* now write product back to destination */
  RAX = GET32L(product_64);
  RDX = GET32H(product_64);

  BX_NEXT_INSTR(i);
}

// EVEX map4 IMUL AF and 69/6B:
//   IMUL_BdGdEdR : AF ND=1 (dst = vvvv) or ND=0 NF=1 (dst = reg), dst = src1 * src2
//                  69/6B except ND=0 NF=0, dst = src1 * imm
//                  flags updated unless NF=1

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_BdGdEdR(bxInstruction_c *i)
{
  Bit32s op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32s op2_32;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_IMUL_GdEdId || ia_opcode == BX_IA_APX_IMUL_GdEdsIb)
    op2_32 = (Bit32s) i->Id();
  else
    op2_32 = BX_READ_32BIT_REG(i->src2());

  Bit64s product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  Bit32u product_32 = (Bit32u)(product_64 & 0xFFFFFFFF);

  /* now write product back to destination */
  BX_WRITE_32BIT_REGZ(i->dst(), product_32);

  if (! i->getApxEvexNF()) {
    SET_FLAGS_OSZAPC_LOGIC_32(product_32);
    if(product_64 != (Bit32s) product_64)
    {
      BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
    }
  }

  BX_NEXT_INSTR(i);
}

#endif
