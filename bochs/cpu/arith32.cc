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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EdR(bxInstruction_c *i)
{
  Bit32u erx = ++BX_READ_32BIT_REG(i->dst());
  SET_FLAGS_OSZAP_ADD_32(erx - 1, 0, erx);
  BX_CLEAR_64BIT_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EdR(bxInstruction_c *i)
{
  Bit32u erx = --BX_READ_32BIT_REG(i->dst());
  SET_FLAGS_OSZAP_SUB_32(erx + 1, 0, erx);
  BX_CLEAR_64BIT_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EdGdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32 + temp_CF;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GdEdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_GdEdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EdGdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GdEdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_GdEdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EdIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  diff_32 = op1_32 - (op2_32 + temp_CF);
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SBB_EdIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - op2_32;
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CWDE(bxInstruction_c *i)
{
  /* CWDE: no flags are effected */
  Bit32u tmp = (Bit16s) AX;
  RAX = tmp;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CDQ(bxInstruction_c *i)
{
  /* CDQ: no flags are affected */
  if (EAX & 0x80000000) {
    RDX = 0xFFFFFFFF;
  }
  else {
    RDX = 0x00000000;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  /* and write destination into source */
  BX_WRITE_32BIT_REGZ(i->src(), op1_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XADD_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  sum_32 = op1_32 + op2_32;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_32BIT_REGZ(i->src(), op1_32);
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = i->Id();
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADD_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = i->Id();
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EdIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  sum_32 = op1_32 + op2_32 + temp_CF;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ADC_EdIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->dst(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  diff_32 = op1_32 - op2_32;
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SUB_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_virtual_dword(i->seg(), eaddr);
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMP_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 = - (Bit32s)(op1_32);
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_SUB_32(0, 0 - op1_32, op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NEG_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 = - (Bit32s)(op1_32);
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_SUB_32(0, 0 - op1_32, op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INC_EdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32++;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAP_ADD_32(op1_32 - 1, 0, op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::DEC_EdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32--;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAP_SUB_32(op1_32 + 1, 0, op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u diff_32 = EAX - op1_32;
  SET_FLAGS_OSZAPC_SUB_32(EAX, op1_32, diff_32);

  if (diff_32 == 0) {  // if accumulator == dest
    // dest <-- src
    write_RMW_virtual_dword(BX_READ_32BIT_REG(i->src()));
  }
  else {
    // accumulator <-- dest
    write_RMW_virtual_dword(op1_32);
    RAX = op1_32;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u diff_32 = EAX - op1_32;
  SET_FLAGS_OSZAPC_SUB_32(EAX, op1_32, diff_32);

  if (diff_32 == 0) {  // if accumulator == dest
    // dest <-- src
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));
  }
  else {
    // accumulator <-- dest
    RAX = op1_32;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPXCHG8B(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  // check write permission for following write
  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), eaddr);
  Bit64u op2_64 = ((Bit64u) EDX << 32) | EAX;

  if (op1_64 == op2_64) {  // if accumulator == dest
    // dest <-- src (ECX:EBX)
    op2_64 = ((Bit64u) ECX << 32) | EBX;
    write_RMW_virtual_qword(op2_64);
    assert_ZF();
  }
  else {
    // accumulator <-- dest
    write_RMW_virtual_qword(op1_64);
    RAX = GET32L(op1_64);
    RDX = GET32H(op1_64);
    clear_ZF();
  }

  BX_NEXT_INSTR(i);
}
