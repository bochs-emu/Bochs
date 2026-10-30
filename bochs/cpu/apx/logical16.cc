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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BwGwEwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->src1());
  op2_16 = BX_READ_16BIT_REG(i->src2());
  op1_16 ^= op2_16;
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// XOR Ew, Gw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_XOR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->dst());
  op2_16 = BX_READ_16BIT_REG(i->src());
  op1_16 ^= op2_16;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BwEwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  op1_16 ^= i->Iw();
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// XOR Ew, Iw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_XOR_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16 ^= i->Iw();
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BwEwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  op1_16 |= i->Iw();
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// OR Ew, Iw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_OR_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16 |= i->Iw();
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_BwEwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  op1_16 = ~op1_16;
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BwGwEwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->src1());
  op2_16 = BX_READ_16BIT_REG(i->src2());
  op1_16 |= op2_16;
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// OR Ew, Gw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_OR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->dst());
  op2_16 = BX_READ_16BIT_REG(i->src());
  op1_16 |= op2_16;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BwGwEwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->src1());
  op2_16 = BX_READ_16BIT_REG(i->src2());
  op1_16 &= op2_16;
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// AND Ew, Gw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_AND_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->dst());
  op2_16 = BX_READ_16BIT_REG(i->src());
  op1_16 &= op2_16;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BwEwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->src());
  op1_16 &= i->Iw();
  // ND=1: 16-bit result is zero extended to the full destination register (zero upper)
  BX_WRITE_32BIT_REGZ(i->dst(), op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_16(op1_16);

  BX_NEXT_INSTR(i);
}

// AND Ew, Iw with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_AND_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16 &= i->Iw();
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

#endif
