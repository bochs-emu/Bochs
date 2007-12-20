/////////////////////////////////////////////////////////////////////////
// $Id: arith32.cc,v 1.68 2007-12-20 18:29:38 sshwarts Exp $
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


#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu code.
#define RAX EAX
#define RDX EDX
#endif


void BX_CPU_C::INC_ERX(bxInstruction_c *i)
{
  Bit32u erx = ++BX_READ_32BIT_REG(i->opcodeReg());
  SET_FLAGS_OSZAPC_INC_32(erx);
  BX_CLEAR_64BIT_HIGH(i->opcodeReg());
}

void BX_CPU_C::DEC_ERX(bxInstruction_c *i)
{
  Bit32u erx = --BX_READ_32BIT_REG(i->opcodeReg());
  SET_FLAGS_OSZAPC_DEC_32(erx);
  BX_CLEAR_64BIT_HIGH(i->opcodeReg());
}

void BX_CPU_C::ADD_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADD_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADD_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADD_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADD_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  op1_32 = EAX;
  sum_32 = op1_32 + op2_32;
  RAX = sum_32;

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADC_EdGdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32 + temp_CF;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::ADC_EdGdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::ADC_GdEdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::ADC_GdEdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->nnn(), sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::ADC_EAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  op1_32 = EAX;
  sum_32 = op1_32 + op2_32 + temp_CF;
  RAX = sum_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::SBB_EdGdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_EdGdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->rm(), diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_GdEdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_GdEdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_EAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  diff_32 = op1_32 - (op2_32 + temp_CF);
  RAX = diff_32;

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_EdIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  diff_32 = op1_32 - (op2_32 + temp_CF);
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SBB_EdIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = op1_32 - (op2_32 + temp_CF);
  BX_WRITE_32BIT_REGZ(i->rm(), diff_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, diff_32, BX_INSTR_SUB_SBB32(temp_CF));
}

void BX_CPU_C::SUB_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - op2_32;
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::SUB_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::SUB_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::SUB_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->nnn(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::SUB_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;
  RAX = diff_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CWDE(bxInstruction_c *i)
{
  /* CWDE: no flags are effected */
  Bit32u tmp = (Bit16s) AX;
  RAX = tmp;
}

void BX_CPU_C::CDQ(bxInstruction_c *i)
{
  /* CDQ: no flags are affected */
  if (EAX & 0x80000000) {
    RDX = 0xFFFFFFFF;
  }
  else {
    RDX = 0x00000000;
  }
}

// Some info on the opcodes at {0F,A6} and {0F,A7}
// On 386 steps A0-B0:
//   {OF,A6} = XBTS
//   {OF,A7} = IBTS
// On 486 steps A0-B0:
//   {OF,A6} = CMPXCHG 8
//   {OF,A7} = CMPXCHG 16|32
//
// On 486 >= B steps, and further processors, the
// CMPXCHG instructions were moved to opcodes:
//   {OF,B0} = CMPXCHG 8
//   {OF,B1} = CMPXCHG 16|32

void BX_CPU_C::CMPXCHG_XBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_XBTS: Generate #UD exception"));
  UndefinedOpcode(i);
}

void BX_CPU_C::CMPXCHG_IBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_IBTS: Generate #UD exception"));
  UndefinedOpcode(i);
}

void BX_CPU_C::XADD_EdGdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u op1_32, op2_32, sum_32;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  /* and write destination into source */
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
#else
  BX_INFO (("XADD_EdGd not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XADD_EdGdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u op1_32, op2_32, sum_32;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  sum_32 = op1_32 + op2_32;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
#else
  BX_INFO (("XADD_EdGd not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADD_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = i->Id();
  sum_32 = op1_32 + op2_32;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADD_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, sum_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = i->Id();
  sum_32 = op1_32 + op2_32;

  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

  SET_FLAGS_OSZAPC_ADD_32(op1_32, op2_32, sum_32);
}

void BX_CPU_C::ADC_EdIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  sum_32 = op1_32 + op2_32 + temp_CF;
  write_RMW_virtual_dword(sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::ADC_EdIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit32u op1_32, op2_32 = i->Id(), sum_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  sum_32 = op1_32 + op2_32 + temp_CF;
  BX_WRITE_32BIT_REGZ(i->rm(), sum_32);

  SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_ADD_ADC32(temp_CF));
}

void BX_CPU_C::SUB_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  diff_32 = op1_32 - op2_32;
  write_RMW_virtual_dword(diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::SUB_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32 = i->Id(), diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = op1_32 - op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), diff_32);

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::CMP_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = i->Id();
  diff_32 = op1_32 - op2_32;

  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
}

void BX_CPU_C::NEG_EdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 = -op1_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_RESULT_32(op1_32, BX_INSTR_NEG32);
}

void BX_CPU_C::NEG_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 = -op1_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_RESULT_32(op1_32, BX_INSTR_NEG32);
}

void BX_CPU_C::INC_EdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32++;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_INC_32(op1_32);
}

void BX_CPU_C::INC_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32++;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_INC_32(op1_32);
}

void BX_CPU_C::DEC_EdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32--;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_DEC_32(op1_32);
}

void BX_CPU_C::DEC_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32--;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_DEC_32(op1_32);
}

void BX_CPU_C::CMPXCHG_EdGdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u op1_32, op2_32, diff_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  diff_32 = EAX - op1_32;
  SET_FLAGS_OSZAPC_SUB_32(EAX, op1_32, diff_32);

  if (diff_32 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_32 = BX_READ_32BIT_REG(i->nnn());
    write_RMW_virtual_dword(op2_32);
  }
  else {
    // accumulator <-- dest
    RAX = op1_32;
  }
#else
  BX_INFO(("CMPXCHG_EdGd: not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPXCHG_EdGdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u op1_32, op2_32, diff_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  diff_32 = EAX - op1_32;
  SET_FLAGS_OSZAPC_SUB_32(EAX, op1_32, diff_32);

  if (diff_32 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_32 = BX_READ_32BIT_REG(i->nnn());
    BX_WRITE_32BIT_REGZ(i->rm(), op2_32);
  }
  else {
    // accumulator <-- dest
    RAX = op1_32;
  }
#else
  BX_INFO(("CMPXCHG_EdGd: not supported for cpulevel <= 3"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPXCHG8B(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  Bit32u op1_64_lo, op1_64_hi, diff;

  read_RMW_virtual_dword(i->seg(), RMAddr(i),     &op1_64_lo);
  read_RMW_virtual_dword(i->seg(), RMAddr(i) + 4, &op1_64_hi);

  diff  = EAX - op1_64_lo;
  diff |= EDX - op1_64_hi;

  if (diff == 0) {  // if accumulator == dest
    // dest <-- src
    write_RMW_virtual_dword(ECX);
    // write permissions already checked by read_RMW_virtual_dword
    write_virtual_dword(i->seg(), RMAddr(i), EBX);
    assert_ZF();
  }
  else {
    clear_ZF();
    // accumulator <-- dest
    RAX = op1_64_lo;
    RDX = op1_64_hi;
  }

#else
  BX_INFO(("CMPXCHG8B: not supported for cpulevel <= 4"));
  UndefinedOpcode(i);
#endif
}
