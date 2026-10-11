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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = BX_READ_64BIT_REG(i->src2());
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());
  Bit64u sum_64 = op1_64 + op2_64 + getB_CF();
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADCX_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());
  Bit64u sum_64 = op1_64 + op2_64 + getB_CF();
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  Bit64u carry_out = ADD_COUT_VEC(op1_64, op2_64, sum_64);
  set_CF(carry_out >> 63);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADOX_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());
  Bit64u sum_64 = op1_64 + op2_64 + getB_OF();
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  Bit64u carry_out = ADD_COUT_VEC(op1_64, op2_64, sum_64);
  set_OF(carry_out >> 63);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = BX_READ_64BIT_REG(i->src2());
  diff_64 = op1_64 - (op2_64 + getB_CF());
  BX_WRITE_64BIT_REG(i->dst(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BqEqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - (op2_64 + getB_CF());
  BX_WRITE_64BIT_REG(i->dst(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BqGqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = BX_READ_64BIT_REG(i->src2());
  diff_64 = op1_64 - op2_64;
  BX_WRITE_64BIT_REG(i->dst(), diff_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BqEqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BqEqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64 + getB_CF();
  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BqEqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->src1());
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;
  BX_WRITE_64BIT_REG(i->dst(), diff_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  op1_64 = -op1_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_64(0, -op1_64, op1_64);

  BX_NEXT_INSTR(i);
}

// NEG Eq with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_NEG_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64 = -op1_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  op1_64++;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_ADD_64(op1_64 - 1, 0, op1_64);

  BX_NEXT_INSTR(i);
}

// INC Eq with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_INC_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64++;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  op1_64--;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_SUB_64(op1_64 + 1, 0, op1_64);

  BX_NEXT_INSTR(i);
}

// DEC Eq with NF=1 (ND=0 register form): flags are not updated
void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_DEC_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64--;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

#endif
