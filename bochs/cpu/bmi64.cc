/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2018 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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

#if BX_SUPPORT_X86_64 

#if BX_SUPPORT_AVX

#include "scalar_arith.h"
#include "wide_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ANDN_GqBqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());

  op1_64 = ~op1_64 & op2_64;

  SET_FLAGS_OSZAxC_LOGIC_64(op1_64); // keep PF unchanged

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULX_GqBqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = RDX;
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2());

  Bit128u product_128;

  // product_128 = ((Bit128u) op1_64) * ((Bit128u) op2_64);
  // product_64l = (Bit64u) (product_128 & 0xFFFFFFFFFFFFFFFF);
  // product_64h = (Bit64u) (product_128 >> 64);

  long_mul(&product_128,op1_64,op2_64);

  BX_WRITE_64BIT_REG(i->src1(), product_128.lo);
  BX_WRITE_64BIT_REG(i->dst(),  product_128.hi);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSI_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  bool tmpCF = (op1_64 != 0);

  op1_64 = (-op1_64) & op1_64;

  SET_FLAGS_OSZAxC_LOGIC_64(op1_64); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSMSK_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  bool tmpCF = (op1_64 == 0);

  op1_64 = (op1_64-1) ^ op1_64;

  SET_FLAGS_OSZAxC_LOGIC_64(op1_64); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSR_BqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  bool tmpCF = (op1_64 == 0);

  op1_64 = (op1_64-1) & op1_64;

  SET_FLAGS_OSZAxC_LOGIC_64(op1_64); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RORX_GqEqIbR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());

  unsigned count = i->Ib() & 0x3f;
  if (count) {
    op1_64 = (op1_64 >> count) | (op1_64 << (64 - count));
  }

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRX_GqEqBqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x3f;
  if (count)
    op1_64 >>= count;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SARX_GqEqBqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x3f;
  if (count) {
    /* count < 64, since only lower 6 bits used */
    op1_64 = ((Bit64s) op1_64) >> count;
  }

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLX_GqEqBqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x3f;
  if (count)
    op1_64 <<= count;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BEXTR_GqEqBqR(bxInstruction_c *i)
{
  Bit16u control = BX_READ_16BIT_REG(i->src2());
  unsigned start = control & 0xff;
  unsigned len   = control >> 8;

  Bit64u op1_64 = bextrq(BX_READ_64BIT_REG(i->src1()), start, len);
  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BZHI_GqEqBqR(bxInstruction_c *i)
{
  unsigned control = BX_READ_8BIT_REGL(i->src2());
  bool tmpCF = 0;
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  
  if (control < 64) {
    Bit64u mask = (BX_CONST64(1) << control) - 1;
    op1_64 &= mask;
  }
  else {
    tmpCF = 1;
  }

  SET_FLAGS_OSZAxC_LOGIC_64(op1_64); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXT_GqBqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2()), result_64 = 0;

  Bit64u wr_mask = 0x1;

  for (; op2_64 != 0; op2_64 >>= 1)
  {
    if (op2_64 & 0x1) {
      if (op1_64 & 0x1) result_64 |= wr_mask;
      wr_mask <<= 1;
    }
    op1_64 >>= 1;
  }

  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PDEP_GqBqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src1());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src2()), result_64 = 0;

  Bit64u wr_mask = 0x1;

  for (; op2_64 != 0; op2_64 >>= 1)
  {
    if (op2_64 & 0x1) {
      if (op1_64 & 0x1) result_64 |= wr_mask;
      op1_64 >>= 1;
    }
    wr_mask <<= 1;
  }
  
  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_AVX

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADCX_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());
  Bit64u sum_64 = op1_64 + op2_64 + getB_CF();

  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  Bit64u carry_out = ADD_COUT_VEC(op1_64, op2_64, sum_64);
  set_CF(carry_out >> 63);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADOX_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());
  Bit64u sum_64 = op1_64 + op2_64 + getB_OF();

  BX_WRITE_64BIT_REG(i->dst(), sum_64);

  Bit64u overflow = GET_ADD_OVERFLOW(op1_64, op2_64, sum_64, BX_CONST64(0x8000000000000000));
  set_OF(!!overflow);

  BX_NEXT_INSTR(i);
}

#endif
