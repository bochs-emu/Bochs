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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BwGwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src2());
  Bit32u sum_16 = op1_16 + op2_16;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) sum_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EwGwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16;

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BwGwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src2());
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BwGwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src2());
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BwEwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BwGwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src1());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src2());
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) diff_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EwGwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BwEwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) sum_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_ND0_ADD_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16;
  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BwEwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BwEwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) diff_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_ND0_SUB_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_BwEwR(bxInstruction_c *i)
{
  Bit16u rx = BX_READ_16BIT_REG(i->src());
  rx++;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) rx);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_ADD_16(rx - 1, 0, rx);

  BX_NEXT_INSTR(i);
}

// INC Ew with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_INC_EwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16++;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_BwEwR(bxInstruction_c *i)
{
  Bit16u rx = BX_READ_16BIT_REG(i->src());
  rx--;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) rx);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_SUB_16(rx + 1, 0, rx);

  BX_NEXT_INSTR(i);
}

// DEC Ew with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_DEC_EwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16--;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_BwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->src());
  op1_16 = -op1_16;
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16u) op1_16);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_16(0, 0 - op1_16, op1_16);

  BX_NEXT_INSTR(i);
}

// NEG Ew with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_NEG_EwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16 = -op1_16;
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  BX_NEXT_INSTR(i);
}

#endif
