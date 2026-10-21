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


void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BbGbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// XOR Eb, Gb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_XOR_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_BbEbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// XOR Eb, Ib with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_XOR_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1 ^= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BbEbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// OR Eb, Ib with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_OR_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_BbEbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1_8 = ~op1_8;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_BbGbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// OR Eb, Gb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_OR_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 |= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BbGbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// AND Eb, Gb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_AND_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_BbEbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_32BIT_REGZ(i->dst(), op1);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_LOGIC_8(op1);

  BX_NEXT_INSTR(i);
}

// AND Eb, Ib with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_AND_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib();

  op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1 &= op2;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

#endif
