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

#if BX_SUPPORT_AVX

#include "scalar_arith.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ANDN_GdBdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());

  op1_32 = ~op1_32 & op2_32;

  SET_FLAGS_OSZAxC_LOGIC_32(op1_32); // keep PF unchanged

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULX_GdBdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = EDX;
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2());
  Bit64u product_64  = ((Bit64u) op1_32) * ((Bit64u) op2_32);

  BX_WRITE_32BIT_REGZ(i->src1(), GET32L(product_64));
  BX_WRITE_32BIT_REGZ(i->dst(), GET32H(product_64));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSI_BdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  bool tmpCF = (op1_32 != 0);

  op1_32 = (-op1_32) & op1_32;

  SET_FLAGS_OSZAxC_LOGIC_32(op1_32); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSMSK_BdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  bool tmpCF = (op1_32 == 0);

  op1_32 = (op1_32-1) ^ op1_32;

  SET_FLAGS_OSZAxC_LOGIC_32(op1_32); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BLSR_BdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  bool tmpCF = (op1_32 == 0);

  op1_32 = (op1_32-1) & op1_32;

  SET_FLAGS_OSZAxC_LOGIC_32(op1_32); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RORX_GdEdIbR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());

  unsigned count = i->Ib() & 0x1f;
  if (count) {
    op1_32 = (op1_32 >> count) | (op1_32 << (32 - count));
  }

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHRX_GdEdBdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x1f;
  if (count)
    op1_32 >>= count;

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SARX_GdEdBdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x1f;
  if (count) {
    /* count < 32, since only lower 5 bits used */
    op1_32 = ((Bit32s) op1_32) >> count;
  }

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SHLX_GdEdBdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());

  unsigned count = BX_READ_32BIT_REG(i->src2()) & 0x1f;
  if (count)
    op1_32 <<= count;

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BEXTR_GdEdBdR(bxInstruction_c *i)
{
  Bit16u control = BX_READ_16BIT_REG(i->src2());
  unsigned start = control & 0xff;
  unsigned len   = control >> 8;
  
  Bit32u op1_32 = bextrd(BX_READ_32BIT_REG(i->src1()), start, len);
  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BZHI_GdEdBdR(bxInstruction_c *i)
{
  unsigned control = BX_READ_8BIT_REGL(i->src2());
  bool tmpCF = 0;
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  
  if (control < 32) {
    Bit32u mask = (1 << control) - 1;
    op1_32 &= mask;
  }
  else {
    tmpCF = 1;
  }

  SET_FLAGS_OSZAxC_LOGIC_32(op1_32); // keep PF unchanged
  set_CF(tmpCF);

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}


void BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXT_GdBdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2()), result_32 = 0;

  Bit32u wr_mask = 0x1;

  for (; op2_32 != 0; op2_32 >>= 1)
  {
    if (op2_32 & 0x1) {
      if (op1_32 & 0x1) result_32 |= wr_mask;
      wr_mask <<= 1;
    }
    op1_32 >>= 1;
  }
  
  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PDEP_GdBdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src2()), result_32 = 0;

  Bit32u wr_mask = 0x1;

  for (; op2_32 != 0; op2_32 >>= 1)
  {
    if (op2_32 & 0x1) {
      if (op1_32 & 0x1) result_32 |= wr_mask;
      op1_32 >>= 1;
    }
    wr_mask <<= 1;
  }
  
  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADCX_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
  Bit32u sum_32 = op1_32 + op2_32 + getB_CF();

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  Bit32u carry_out = ADD_COUT_VEC(op1_32, op2_32, sum_32);
  set_CF(carry_out >> 31);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADOX_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
  Bit32u sum_32 = op1_32 + op2_32 + getB_OF();

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  Bit32u overflow = GET_ADD_OVERFLOW(op1_32, op2_32, sum_32, 0x80000000);
  set_OF(!!overflow);

  BX_NEXT_INSTR(i);
}
