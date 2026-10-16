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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit32u sum_32 = op1_32 + op2_32 + getB_CF();
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADCX_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit32u sum_32 = op1_32 + op2_32 + getB_CF();
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  Bit32u carry_out = ADD_COUT_VEC(op1_32, op2_32, sum_32);
  set_CF(carry_out >> 31);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADOX_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit32u sum_32 = op1_32 + op2_32 + getB_OF();
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  Bit32u carry_out = ADD_COUT_VEC(op1_32, op2_32, sum_32);
  set_OF(carry_out >> 31);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32, temp_CF = getB_CF();

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32, temp_CF = getB_CF();

  op1_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BdGdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->src1());
  op2_32 = BX_READ_32BIT_REG(i->src2());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->src());
  op2_32 = i->Id();
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), sum_32, temp_CF = getB_CF();

  op1_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_BdEdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  op1_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_BdEdR(bxInstruction_c *i)
{
  Bit32u erx = BX_READ_32BIT_REG(i->src());
  erx++;
  BX_WRITE_32BIT_REGZ(i->dst(), erx);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_ADD_32(erx - 1, 0, erx);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_BdEdR(bxInstruction_c *i)
{
  Bit32u erx = BX_READ_32BIT_REG(i->src());
  erx--;
  BX_WRITE_32BIT_REGZ(i->dst(), erx);


  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAP_SUB_32(erx + 1, 0, erx);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_BdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  op1_32 = -op1_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  if (! i->getApxEvexNF())
    SET_FLAGS_OSZAPC_SUB_32(0, 0 - op1_32, op1_32);

  BX_NEXT_INSTR(i);
}

#endif
