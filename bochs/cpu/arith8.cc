/////////////////////////////////////////////////////////////////////////
// $Id: arith8.cc,v 1.50 2007-12-04 19:27:22 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::ADD_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2;
  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADD_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADD_GbEbM(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  read_virtual_byte(i->seg(), RMAddr(i), &op2);
  sum = op1 + op2;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADD_GbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  sum = op1 + op2;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADD_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  op1 = AL;
  op2 = i->Ib();
  sum = op1 + op2;
  AL = sum;

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADC_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;
  bx_bool temp_CF = getB_CF();

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2 + temp_CF;
  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::ADC_EbGbR(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;
  bx_bool temp_CF = getB_CF();

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2 + temp_CF;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::ADC_GbEbM(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;
  bx_bool temp_CF = getB_CF();

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  read_virtual_byte(i->seg(), RMAddr(i), &op2);
  sum = op1 + op2 + temp_CF;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::ADC_GbEbR(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;
  bx_bool temp_CF = getB_CF();

  op1 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  sum = op1 + op2 + temp_CF;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::ADC_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;
  bx_bool temp_CF = getB_CF();

  op1 = AL;
  op2 = i->Ib();
  sum = op1 + op2 + temp_CF;
  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::SBB_EbGbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;
  bx_bool temp_CF = getB_CF();

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - (op2_8 + temp_CF);
  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_EbGbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;
  bx_bool temp_CF = getB_CF();

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - (op2_8 + temp_CF);
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_GbEbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;
  bx_bool temp_CF = getB_CF();

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  diff_8 = op1_8 - (op2_8 + temp_CF);
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_GbEbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;
  bx_bool temp_CF = getB_CF();

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - (op2_8 + temp_CF);
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_ALIb(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;
  bx_bool temp_CF = getB_CF();

  op1_8 = AL;
  op2_8 = i->Ib();
  diff_8 = op1_8 - (op2_8 + temp_CF);
  AL = diff_8;

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_EbIbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;
  bx_bool temp_CF = getB_CF();

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  diff_8 = op1_8 - (op2_8 + temp_CF);
  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SBB_EbIbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;
  bx_bool temp_CF = getB_CF();

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - (op2_8 + temp_CF);
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_8(op1_8, op2_8, diff_8, BX_INSTR_SUB_SBB8(temp_CF));
}

void BX_CPU_C::SUB_EbGbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;
  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::SUB_EbGbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::SUB_GbEbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  diff_8 = op1_8 - op2_8;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::SUB_GbEbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::SUB_ALIb(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = AL;
  op2_8 = i->Ib();
  diff_8 = op1_8 - op2_8;
  AL = diff_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_EbGbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  read_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_EbGbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_GbEbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_GbEbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_ALIb(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = AL;
  op2_8 = i->Ib();
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::XADD_EbGbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit8u op1, op2, sum;

  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2;
  write_RMW_virtual_byte(sum);

  /* and write destination into source */
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
#else
  BX_INFO(("XADD_EbGb: not supported on < 80486"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XADD_EbGbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit8u op1, op2, sum;

  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  sum = op1 + op2;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
#else
  BX_INFO(("XADD_EbGb: not supported on < 80486"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADD_EbIbM(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib(), sum;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
  sum = op1 + op2;
  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADD_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib(), sum;

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  sum = op1 + op2;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);
}

void BX_CPU_C::ADC_EbIbM(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib(), sum;
  bx_bool temp_CF = getB_CF();

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
  sum = op1 + op2 + temp_CF;
  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::ADC_EbIbR(bxInstruction_c *i)
{
  Bit8u op1, op2 = i->Ib(), sum;
  bx_bool temp_CF = getB_CF();

  op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  sum = op1 + op2 + temp_CF;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_ADD_ADC8(temp_CF));
}

void BX_CPU_C::SUB_EbIbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  diff_8 = op1_8 - op2_8;
  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::SUB_EbIbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_EbIbM(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;

  read_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::CMP_EbIbR(bxInstruction_c *i)
{
  Bit8u op1_8, op2_8 = i->Ib(), diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = op1_8 - op2_8;
  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);
}

void BX_CPU_C::NEG_EbM(bxInstruction_c *i)
{
  Bit8u op1_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op1_8 = -op1_8;
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAPC_RESULT_8(op1_8, BX_INSTR_NEG8);
}

void BX_CPU_C::NEG_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1_8 = -op1_8;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAPC_RESULT_8(op1_8, BX_INSTR_NEG8);
}

void BX_CPU_C::INC_EbM(bxInstruction_c *i)
{
  Bit8u op1_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op1_8++;
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAPC_INC_8(op1_8);
}

void BX_CPU_C::INC_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1_8++;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAPC_INC_8(op1_8);
}

void BX_CPU_C::DEC_EbM(bxInstruction_c *i)
{
  Bit8u op1_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  op1_8--;
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAPC_DEC_8(op1_8);
}

void BX_CPU_C::DEC_EbR(bxInstruction_c *i)
{
  Bit8u op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  op1_8--;
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAPC_DEC_8(op1_8);
}

void BX_CPU_C::CMPXCHG_EbGbM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit8u op1_8, op2_8, diff_8;

  read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
  diff_8 = AL - op1_8;
  SET_FLAGS_OSZAPC_SUB_8(AL, op1_8, diff_8);

  if (diff_8 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
    write_RMW_virtual_byte(op2_8);
  }
  else {
    // accumulator <-- dest
    AL = op1_8;
  }

#else
  BX_INFO(("CMPXCHG_EbGb: not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPXCHG_EbGbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit8u op1_8, op2_8, diff_8;

  op1_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  diff_8 = AL - op1_8;
  SET_FLAGS_OSZAPC_SUB_8(AL, op1_8, diff_8);

  if (diff_8 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_8 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op2_8);
  }
  else {
    // accumulator <-- dest
    AL = op1_8;
  }

#else
  BX_INFO(("CMPXCHG_EbGb: not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}
