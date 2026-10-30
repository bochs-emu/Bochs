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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_MUL_AXEwR(bxInstruction_c *i)
{
  Bit16u op1_16 = AX;
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  Bit32u product_32 = ((Bit32u) op1_16) * ((Bit32u) op2_16);

  /* now write product back to destination */
  AX = product_32 & 0xFFFF;
  DX = product_32 >> 16;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_IMUL_AXEwR(bxInstruction_c *i)
{
  Bit16s op1_16 = AX;
  Bit16s op2_16 = BX_READ_16BIT_REG(i->src());

  Bit32s product_32 = ((Bit32s) op1_16) * ((Bit32s) op2_16);

  /* now write product back to destination */
  AX = product_32 & 0xFFFF;
  DX = product_32 >> 16;

  BX_NEXT_INSTR(i);
}

// EVEX map4 IMUL AF and 69/6B:
//   IMUL_BwGwEwR : AF ND=1 (dst = vvvv) or ND=0 NF=1 (dst = reg), dst = src1 * src2
//                  69/6B except ND=0 NF=0, dst = src1 * imm
//                  flags updated unless NF=1, upper part of the destination cleared only when ND=1

void BX_CPP_AttrRegparmN(1) BX_CPU_C::IMUL_BwGwEwR(bxInstruction_c *i)
{
  Bit16s op1_16 = BX_READ_16BIT_REG(i->src1());
  Bit16s op2_16;

  unsigned ia_opcode = i->getIaOpcode();
  if (ia_opcode == BX_IA_APX_IMUL_GwEwIw || ia_opcode == BX_IA_APX_IMUL_GwEwsIb)
    op2_16 = (Bit16s) i->Iw();
  else
    op2_16 = BX_READ_16BIT_REG(i->src2());

  Bit32s product_32 = op1_16 * op2_16;
  Bit16u product_16 = (product_32 & 0xFFFF);

  /* now write product back to destination */
  if (i->getApxEvexND()) {
    BX_WRITE_32BIT_REGZ(i->dst(), product_16);
  }
  else {
    BX_WRITE_16BIT_REG(i->dst(), product_16);
  }

  if (! i->getApxEvexNF()) {
    SET_FLAGS_OSZAPC_LOGIC_16(product_16);
    if(product_32 != (Bit16s) product_32)
    {
      BX_CPU_THIS_PTR oszapc.assert_flags_OxxxxC();
    }
  }

  BX_NEXT_INSTR(i);
}

#endif
