/////////////////////////////////////////////////////////////////////////
// $Id: arith64.cc,v 1.44 2007-12-20 18:29:38 sshwarts Exp $
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


#if BX_SUPPORT_X86_64

void BX_CPU_C::ADD_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64;
  write_RMW_virtual_qword(sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->rm(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 = BX_READ_64BIT_REG(i->rm());
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_RAXId(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64;

  /* now write sum back to destination */
  RAX = sum_64;

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADC_EqGqM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64 + temp_CF;
  write_RMW_virtual_qword(sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_EqGqR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64 + temp_CF;
  BX_WRITE_64BIT_REG(i->rm(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_GqEqM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  sum_64 = op1_64 + op2_64 + temp_CF;

  /* now write sum back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_GqEqR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 = BX_READ_64BIT_REG(i->rm());
  sum_64 = op1_64 + op2_64 + temp_CF;

  /* now write sum back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64 + temp_CF;

  /* now write sum back to destination */
  RAX = sum_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::SBB_EqGqM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - (op2_64 + temp_CF);
  write_RMW_virtual_qword(diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_EqGqR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - (op2_64 + temp_CF);
  BX_WRITE_64BIT_REG(i->rm(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_GqEqM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  diff_64 = op1_64 - (op2_64 + temp_CF);

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_GqEqR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 = BX_READ_64BIT_REG(i->rm());
  diff_64 = op1_64 - (op2_64 + temp_CF);

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - (op2_64 + temp_CF);

  /* now write diff back to destination */
  RAX = diff_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_EqIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - (op2_64 + temp_CF);
  write_RMW_virtual_qword(diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_EqIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - (op2_64 + temp_CF);
  BX_WRITE_64BIT_REG(i->rm(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SUB_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - op2_64;
  write_RMW_virtual_qword(diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::SUB_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - op2_64;
  BX_WRITE_64BIT_REG(i->rm(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::SUB_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  diff_64 = op1_64 - op2_64;

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::SUB_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 = BX_READ_64BIT_REG(i->rm());
  diff_64 = op1_64 - op2_64;

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::SUB_RAXId(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  /* now write diff back to destination */
  RAX = diff_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 = BX_READ_64BIT_REG(i->rm());
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_RAXId(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CDQE(bxInstruction_c *i)
{
  /* CWDE: no flags are affected */
  RAX = (Bit32s) EAX;
}

void BX_CPU_C::CQO(bxInstruction_c *i)
{
  /* CQO: no flags are affected */

  if (RAX & BX_CONST64(0x8000000000000000))
      RDX = BX_CONST64(0xffffffffffffffff);
  else
      RDX = 0;
}

void BX_CPU_C::XADD_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64;
  write_RMW_virtual_qword(sum_64);

  /* and write destination into source */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::XADD_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  sum_64 = op1_64 + op2_64;

  // and write destination into source
  // Note: if both op1 & op2 are registers, the last one written
  //       should be the sum, as op1 & op2 may be the same register.
  //       For example:  XADD AL, AL
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
  BX_WRITE_64BIT_REG(i->rm(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64;
  write_RMW_virtual_qword(sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADD_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64;
  BX_WRITE_64BIT_REG(i->rm(), sum_64);

  SET_FLAGS_OSZAPC_ADD_64(op1_64, op2_64, sum_64);
}

void BX_CPU_C::ADC_EqIdM(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64 + temp_CF;
  write_RMW_virtual_qword(sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_EqIdR(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64 + temp_CF;
  BX_WRITE_64BIT_REG(i->rm(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::SUB_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;
  write_RMW_virtual_qword(diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::SUB_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;
  BX_WRITE_64BIT_REG(i->rm(), diff_64);

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::CMP_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);
}

void BX_CPU_C::NEG_EqM(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op1_64 = -op1_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_RESULT_64(op1_64, BX_INSTR_NEG64);
}

void BX_CPU_C::NEG_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  op1_64 = -op1_64;
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  SET_FLAGS_OSZAPC_RESULT_64(op1_64, BX_INSTR_NEG64);
}

void BX_CPU_C::INC_EqM(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op1_64++;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_INC_64(op1_64);
}

void BX_CPU_C::INC_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  op1_64++;
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  SET_FLAGS_OSZAPC_INC_64(op1_64);
}

void BX_CPU_C::DEC_EqM(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  op1_64--;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_DEC_64(op1_64);
}

void BX_CPU_C::DEC_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  op1_64--;
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  SET_FLAGS_OSZAPC_DEC_64(op1_64);
}

void BX_CPU_C::CMPXCHG_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  /* pointer, segment address pair */
  read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  diff_64 = RAX - op1_64;
  SET_FLAGS_OSZAPC_SUB_64(RAX, op1_64, diff_64);

  if (diff_64 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_64 = BX_READ_64BIT_REG(i->nnn());
    write_RMW_virtual_qword(op2_64);
  }
  else {
    // accumulator <-- dest
    RAX = op1_64;
  }
}

void BX_CPU_C::CMPXCHG_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  diff_64 = RAX - op1_64;
  SET_FLAGS_OSZAPC_SUB_64(RAX, op1_64, diff_64);

  if (diff_64 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_64 = BX_READ_64BIT_REG(i->nnn());
    BX_WRITE_64BIT_REG(i->rm(), op2_64);
  }
  else {
    // accumulator <-- dest
    RAX = op1_64;
  }
}

void BX_CPU_C::CMPXCHG16B(bxInstruction_c *i)
{
  Bit64u op1_64_lo, op1_64_hi, diff;

  if (RMAddr(i) & 0xf) {
    BX_ERROR(("CMPXCHG16B: not aligned memory location (#GP)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  read_RMW_virtual_qword(i->seg(), RMAddr(i),     &op1_64_lo);
  read_RMW_virtual_qword(i->seg(), RMAddr(i) + 8, &op1_64_hi);

  diff  = RAX - op1_64_lo;
  diff |= RDX - op1_64_hi;

  if (diff == 0) {  // if accumulator == dest
    // dest <-- src
    write_RMW_virtual_qword(RCX);
    // write permissions already checked by read_RMW_virtual_qword
    write_virtual_qword(i->seg(), RMAddr(i), RBX);
    assert_ZF();
  }
  else {
    clear_ZF();
    // accumulator <-- dest
    RAX = op1_64_lo;
    RDX = op1_64_hi;
  }
}

#endif /* if BX_SUPPORT_X86_64 */
