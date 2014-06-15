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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EwR(bxInstruction_c *i)
{
  Bit32u rx = ++BX_READ_16BIT_REG(i->dst());
  SET_FLAGS_OSZAP_ADD_16(rx - 1, 0, rx);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EwR(bxInstruction_c *i)
{
  Bit32u rx = --BX_READ_16BIT_REG(i->dst());
  SET_FLAGS_OSZAP_SUB_16(rx + 1, 0, rx);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16;

  write_RMW_virtual_word(sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16;

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u sum_16 = op1_16 + op2_16;

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  write_RMW_virtual_word(sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  write_RMW_virtual_word(diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  write_RMW_virtual_word(diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - (op2_16 + getB_CF());

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - op2_16;

  write_RMW_virtual_word(diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GwEwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CBW(bxInstruction_c *i)
{
  /* CBW: no flags are effected */
  AX = (Bit8s) AL;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CWD(bxInstruction_c *i)
{
  /* CWD: no flags are affected */
  if (AX & 0x8000) {
    DX = 0xFFFF;
  }
  else {
    DX = 0x0000;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EwGwM(bxInstruction_c *i)
{
  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16;

  write_RMW_virtual_word(sum_16);

  /* and write destination into source */
  BX_WRITE_16BIT_REG(i->src(), op1_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EwGwR(bxInstruction_c *i)
{
  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = BX_READ_16BIT_REG(i->src());
  Bit32u sum_16 = op1_16 + op2_16;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_16BIT_REG(i->src(), op1_16);
  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16;

  write_RMW_virtual_word(sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16;
  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  write_RMW_virtual_word(sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u sum_16 = op1_16 + op2_16 + getB_CF();

  BX_WRITE_16BIT_REG(i->dst(), sum_16);

  SET_FLAGS_OSZAPC_ADD_16(op1_16, op2_16, sum_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  write_RMW_virtual_word(diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  BX_WRITE_16BIT_REG(i->dst(), diff_16);

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_virtual_word(i->seg(), eaddr);
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EwIwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit32u op2_16 = i->Iw();
  Bit32u diff_16 = op1_16 - op2_16;

  SET_FLAGS_OSZAPC_SUB_16(op1_16, op2_16, diff_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  op1_16 = 0 - (Bit32s)(Bit16s)(op1_16);
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_SUB_16(0, 0 - op1_16, op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EwR(bxInstruction_c *i)
{
  Bit32u op1_16 = BX_READ_16BIT_REG(i->dst());
  op1_16 = 0 - (Bit32s)(Bit16s)(op1_16);
  BX_WRITE_16BIT_REG(i->dst(), op1_16);

  SET_FLAGS_OSZAPC_SUB_16(0, 0 - op1_16, op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  op1_16++;
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAP_ADD_16(op1_16 - 1, 0, op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  op1_16--;
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAP_SUB_16(op1_16 + 1, 0, op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  Bit16u diff_16 = AX - op1_16;

  SET_FLAGS_OSZAPC_SUB_16(AX, op1_16, diff_16);

  if (diff_16 == 0) {  // if accumulator == dest
    // dest <-- src
    write_RMW_virtual_word(BX_READ_16BIT_REG(i->src()));
  }
  else {
    // accumulator <-- dest
    write_RMW_virtual_word(op1_16);
    AX = op1_16;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->dst());
  Bit16u diff_16 = AX - op1_16;

  SET_FLAGS_OSZAPC_SUB_16(AX, op1_16, diff_16);

  if (diff_16 == 0) {  // if accumulator == dest
    // dest <-- src
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));
  }
  else {
    // accumulator <-- dest
    AX = op1_16;
  }

  BX_NEXT_INSTR(i);
}
