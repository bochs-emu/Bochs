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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BbGbEbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) sum);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

// ADD Eb, Gb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ADD_EbGbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BbGbEbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  Bit32u sum = op1 + op2 + getB_CF();

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BbGbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BbEbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BbGbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src1(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src2(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) diff_8);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

// SUB Eb, Gb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SUB_EbGbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BbEbIbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) sum);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

// ADD Eb, Ib with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_ADD_EbIbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BbEbIbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2 + getB_CF();

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BbEbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) diff_8);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

// SUB Eb, Ib with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_SUB_EbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_BbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1_8 = -op1_8;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) op1_8);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_8(0, 0 - op1_8, op1_8);

  BX_NEXT_INSTR(i);
}

// NEG Eb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_NEG_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8 = -op1_8;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_BbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1_8++;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) op1_8);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_ADD_8(op1_8 - 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

// INC Eb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_INC_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8++;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_BbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  op1_8--;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8u) op1_8);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_SUB_8(op1_8 + 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

// DEC Eb with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_DEC_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8--;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  BX_NEXT_INSTR(i);
}

#endif
