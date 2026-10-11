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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  op1_32 ^= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  op1_32 ^= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  op1_32 |= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_BdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  op1_32 = ~op1_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  op1_32 |= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  op1_32 &= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  op1_32 &= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

#endif
