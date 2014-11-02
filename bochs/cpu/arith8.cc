/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GbEbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = read_virtual_byte(i->seg(), eaddr);
  Bit32u sum = op1 + op2;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2 + getB_CF();

  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GbEbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2 + getB_CF();

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = read_virtual_byte(i->seg(), eaddr);
  Bit32u sum = op1 + op2 + getB_CF();

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());

  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = read_virtual_byte(i->seg(), eaddr);
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());
  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - (op2_8 + getB_CF());
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = read_virtual_byte(i->seg(), eaddr);
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GbEbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = read_virtual_byte(i->seg(), eaddr);
  Bit32u diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EbGbM(bxInstruction_c *i)
{
  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  write_RMW_virtual_byte(sum);

  /* and write destination into source */
  BX_WRITE_8BIT_REGx(i->src(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EbGbR(bxInstruction_c *i)
{
  /* XADD dst(r/m8), src(r8)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  Bit32u sum = op1 + op2;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_8BIT_REGx(i->src(), i->extend8bitL(), op1);
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2;

  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EbIbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2 + getB_CF();

  write_RMW_virtual_byte(sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EbIbR(bxInstruction_c *i)
{
  Bit32u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2 = i->Ib();
  Bit32u sum = op1 + op2 + getB_CF();

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), sum);

  SET_FLAGS_OSZAPC_ADD_8(op1, op2, sum);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  write_RMW_virtual_byte(diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), diff_8);

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_virtual_byte(i->seg(), eaddr);
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EbIbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u op2_8 = i->Ib();
  Bit32u diff_8 = op1_8 - op2_8;

  SET_FLAGS_OSZAPC_SUB_8(op1_8, op2_8, diff_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1_8 = - (Bit8s)(op1_8);
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAPC_SUB_8(0, 0 - op1_8, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8 = - (Bit8s)(op1_8);
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAPC_SUB_8(0, 0 - op1_8, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1_8++;
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAP_ADD_8(op1_8 - 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8++;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAP_ADD_8(op1_8 - 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  op1_8--;
  write_RMW_virtual_byte(op1_8);

  SET_FLAGS_OSZAP_SUB_8(op1_8 + 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  op1_8--;
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op1_8);

  SET_FLAGS_OSZAP_SUB_8(op1_8 + 1, 0, op1_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_8 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit32u diff_8 = AL - op1_8;

  SET_FLAGS_OSZAPC_SUB_8(AL, op1_8, diff_8);

  if (diff_8 == 0) {  // if accumulator == dest
    // dest <-- src
    Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
    write_RMW_virtual_byte(op2_8);
  }
  else {
    // accumulator <-- dest
    write_RMW_virtual_byte(op1_8);
    AL = op1_8;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EbGbR(bxInstruction_c *i)
{
  Bit32u op1_8 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit32u diff_8 = AL - op1_8;

  SET_FLAGS_OSZAPC_SUB_8(AL, op1_8, diff_8);

  if (diff_8 == 0) {  // if accumulator == dest
    // dest <-- src
    Bit32u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op2_8);
  }
  else {
    // accumulator <-- dest
    AL = op1_8;
  }

  BX_NEXT_INSTR(i);
}
