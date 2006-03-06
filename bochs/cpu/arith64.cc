/////////////////////////////////////////////////////////////////////////
// $Id: arith64.cc,v 1.28 2006-03-06 22:02:51 sshwarts Exp $
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



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_X86_64

void BX_CPU_C::ADD_EqGq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, sum_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    sum_64 = op1_64 + op2_64;
    BX_WRITE_64BIT_REG(i->rm(), sum_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    sum_64 = op1_64 + op2_64;
    Write_RMW_virtual_qword(sum_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

void BX_CPU_C::ADD_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  sum_64 = op1_64 + op2_64;

  /* now write sum back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

void BX_CPU_C::ADD_RAXId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64;

  /* now write sum back to destination */
  RAX = sum_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

void BX_CPU_C::ADC_EqGq(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, sum_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    sum_64 = op1_64 + op2_64 + temp_CF;
    BX_WRITE_64BIT_REG(i->rm(), sum_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    sum_64 = op1_64 + op2_64 + temp_CF;
    Write_RMW_virtual_qword(sum_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_GqEq(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  sum_64 = op1_64 + op2_64 + temp_CF;

  /* now write sum back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), sum_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::ADC_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, sum_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  sum_64 = op1_64 + op2_64 + temp_CF;

  /* now write sum back to destination */
  RAX = sum_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::SBB_EqGq(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    diff_64 = op1_64 - (op2_64 + temp_CF);
    BX_WRITE_64BIT_REG(i->rm(), diff_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    diff_64 = op1_64 - (op2_64 + temp_CF);
    Write_RMW_virtual_qword(diff_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_GqEq(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  diff_64 = op1_64 - (op2_64 + temp_CF);

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - (op2_64 + temp_CF);

  /* now write diff back to destination */
  RAX = diff_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SBB_EqId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = (Bit32s) i->Id();

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    diff_64 = op1_64 - (op2_64 + temp_CF);
    BX_WRITE_64BIT_REG(i->rm(), diff_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    diff_64 = op1_64 - (op2_64 + temp_CF);
    Write_RMW_virtual_qword(diff_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB_SBB64(temp_CF));
}

void BX_CPU_C::SUB_EqGq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    diff_64 = op1_64 - op2_64;
    BX_WRITE_64BIT_REG(i->rm(), diff_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    diff_64 = op1_64 - op2_64;
    Write_RMW_virtual_qword(diff_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}

void BX_CPU_C::SUB_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  diff_64 = op1_64 - op2_64;

  /* now write diff back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), diff_64);

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}

void BX_CPU_C::SUB_RAXId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  /* now write diff back to destination */
  RAX = diff_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}

void BX_CPU_C::CMP_EqGq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);
}

void BX_CPU_C::CMP_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = BX_READ_64BIT_REG(i->nnn());

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);
}

void BX_CPU_C::CMP_RAXId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64, diff_64;

  op1_64 = RAX;
  op2_64 = (Bit32s) i->Id();
  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);
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

void BX_CPU_C::XADD_EqGq(bxInstruction_c *i)
{
  Bit64u op2_64, op1_64, sum_64;

  /* XADD dst(r/m), src(r)
   * temp <-- src + dst         | sum = op2 + op1
   * src  <-- dst               | op2 = op1
   * dst  <-- tmp               | op1 = sum
   */

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  sum_64 = op1_64 + op2_64;

  /* now write sum back to destination */
  if (i->modC0()) {
    // and write destination into source
    // Note: if both op1 & op2 are registers, the last one written
    //       should be the sum, as op1 & op2 may be the same register.
    //       For example:  XADD AL, AL
    BX_WRITE_64BIT_REG(i->nnn(), op1_64);
    BX_WRITE_64BIT_REG(i->rm(), sum_64);
  }
  else {
    Write_RMW_virtual_qword(sum_64);
    /* and write destination into source */
    BX_WRITE_64BIT_REG(i->nnn(), op1_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

void BX_CPU_C::ADD_EqId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, sum_64;

  op2_64 = (Bit32s) i->Id();

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    sum_64 = op1_64 + op2_64;
    BX_WRITE_64BIT_REG(i->rm(), sum_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    sum_64 = op1_64 + op2_64;
    Write_RMW_virtual_qword(sum_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

void BX_CPU_C::ADC_EqId(bxInstruction_c *i)
{
  bx_bool temp_CF = getB_CF();

  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, sum_64;

  op2_64 = (Bit32s) i->Id();

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    sum_64 = op1_64 + op2_64 + temp_CF;
    BX_WRITE_64BIT_REG(i->rm(), sum_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    sum_64 = op1_64 + op2_64 + temp_CF;
    Write_RMW_virtual_qword(sum_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD_ADC64(temp_CF));
}

void BX_CPU_C::SUB_EqId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = (Bit32s) i->Id();

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    diff_64 = op1_64 - op2_64;
    BX_WRITE_64BIT_REG(i->rm(), diff_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    diff_64 = op1_64 - op2_64;
    Write_RMW_virtual_qword(diff_64);
  }

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}

void BX_CPU_C::CMP_EqId(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op2_64, op1_64, diff_64;

  op2_64 = (Bit32s) i->Id();

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  diff_64 = op1_64 - op2_64;

  SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_COMPARE64);
}

void BX_CPU_C::NEG_Eq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, diff_64;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    diff_64 = -op1_64;
    BX_WRITE_64BIT_REG(i->rm(), diff_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    diff_64 = -op1_64;
    Write_RMW_virtual_qword(diff_64);
  }

  SET_FLAGS_OSZAPC_RESULT_64(diff_64, BX_INSTR_NEG64);
}

void BX_CPU_C::INC_Eq(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op1_64++;
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    op1_64++;
    Write_RMW_virtual_qword(op1_64);
  }

  SET_FLAGS_OSZAP_RESULT_64(op1_64, BX_INSTR_INC64);
}

void BX_CPU_C::DEC_Eq(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    op1_64--;
    BX_WRITE_64BIT_REG(i->rm(), op1_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    op1_64--;
    Write_RMW_virtual_qword(op1_64);
  }

  SET_FLAGS_OSZAP_RESULT_64(op1_64, BX_INSTR_DEC64);
}

void BX_CPU_C::CMPXCHG_EqGq(bxInstruction_c *i)
{
  Bit64u op2_64, op1_64, diff_64;

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
  }

  diff_64 = RAX - op1_64;

  SET_FLAGS_OSZAPC_64(RAX, op1_64, diff_64, BX_INSTR_COMPARE64);

  if (diff_64 == 0) {  // if accumulator == dest
    // dest <-- src
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op2_64);
    }
    else {
      Write_RMW_virtual_qword(op2_64);
    }
  }
  else {
    // accumulator <-- dest
    RAX = op1_64;
  }
}

void BX_CPU_C::CMPXCHG16B(bxInstruction_c *i)
{
  Bit64u op1_64_lo, op1_64_hi, diff;

  if (i->modC0()) {
    BX_INFO(("CMPXCHG16B: dest is not memory location (#UD)"));
    UndefinedOpcode(i);
  }

  if (RMAddr(i) & 0xf) {
    BX_INFO(("CMPXCHG16B: not aligned memory location (#GP)"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  read_virtual_qword(i->seg(), RMAddr(i), &op1_64_lo);
  read_RMW_virtual_qword(i->seg(), RMAddr(i) + 8, &op1_64_hi);

  diff  = RAX - op1_64_lo;
  diff |= RDX - op1_64_hi;

  if (diff == 0) {  // if accumulator == dest
    // ZF = 1
    set_ZF(1);
    // dest <-- src
    Write_RMW_virtual_qword(RCX);
    write_virtual_qword(i->seg(), RMAddr(i), &RBX);
  }
  else {
    // ZF = 0
    set_ZF(0);
    // accumulator <-- dest
    RAX = op1_64_lo;
    RDX = op1_64_hi;
  }
}

#endif /* if BX_SUPPORT_X86_64 */
