/////////////////////////////////////////////////////////////////////////
// $Id: arith64.cc,v 1.11 2002-10-25 17:23:33 sshwarts Exp $
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
#define LOG_THIS BX_CPU_THIS_PTR


/* I don't think these versions accessible in 64 bit mode
  void
BX_CPU_C::INC_RRX(bxInstruction_c *i)
{
  Bit32u rrx;

  rrx = ++ BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].rrx;
  SET_FLAGS_OSZAP_64(0, 0, rrx, BX_INSTR_INC64);
}

  void
BX_CPU_C::DEC_RRX(bxInstruction_c *i)
{
  Bit32u rrx;

  rrx = -- BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].rrx;
  SET_FLAGS_OSZAP_64(0, 0, rrx, BX_INSTR_DEC64);
}

*/



  void
BX_CPU_C::ADD_EqGq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, sum_64;

    /* op2_64 is a register, RMAddr(i) is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
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
      BX_WRITE_64BIT_REG(i->rm(), sum_64);
      }
    else {
      write_RMW_virtual_qword(sum_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}


  void
BX_CPU_C::ADD_GqEq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, sum_64;

    /* op1_64 is a register, RMAddr(i) is an index of a register */
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


  void
BX_CPU_C::ADD_RAXId(bxInstruction_c *i)
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

  void
BX_CPU_C::ADC_EqGq(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();


    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, sum_64;

    /* op2_64 is a register, RMAddr(i) is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    sum_64 = op1_64 + op2_64 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), sum_64);
      }
    else {
      write_RMW_virtual_qword(sum_64);
      }

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, sum_64, BX_INSTR_ADC64,
                              temp_CF);
}


  void
BX_CPU_C::ADC_GqEq(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();


    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, sum_64;

    /* op1_64 is a register, RMAddr(i) is an index of a register */
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

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, sum_64, BX_INSTR_ADC64,
                             temp_CF);
}


  void
BX_CPU_C::ADC_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();

    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, sum_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id();

    sum_64 = op1_64 + op2_64 + temp_CF;

    /* now write sum back to destination */
    RAX = sum_64;

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, sum_64, BX_INSTR_ADC64,
                           temp_CF);
}




  void
BX_CPU_C::SBB_EqGq(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();


    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, diff_64;

    /* op2_64 is a register, RMAddr(i) is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    diff_64 = op1_64 - (op2_64 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), diff_64);
      }
    else {
      write_RMW_virtual_qword(diff_64);
      }

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, diff_64, BX_INSTR_SBB64,
                              temp_CF);
}


  void
BX_CPU_C::SBB_GqEq(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();


    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, diff_64;

    /* op1_64 is a register, RMAddr(i) is an index of a register */
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

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, diff_64, BX_INSTR_SBB64,
                              temp_CF);
}


  void
BX_CPU_C::SBB_RAXId(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();

    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, diff_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id();

    diff_64 = op1_64 - (op2_64 + temp_CF);

    /* now write diff back to destination */
    RAX = diff_64;

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, diff_64, BX_INSTR_SBB64,
                              temp_CF);
}



  void
BX_CPU_C::SBB_EqId(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();


    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, diff_64;

    op2_64 = (Bit32s) i->Id();

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    diff_64 = op1_64 - (op2_64 + temp_CF);

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), diff_64);
      }
    else {
      write_RMW_virtual_qword(diff_64);
      }

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, diff_64, BX_INSTR_SBB64,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EqGq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, diff_64;

    /* op2_64 is a register, RMAddr(i) is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn());

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    diff_64 = op1_64 - op2_64;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), diff_64);
      }
    else {
      write_RMW_virtual_qword(diff_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}


  void
BX_CPU_C::SUB_GqEq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, diff_64;

    /* op1_64 is a register, RMAddr(i) is an index of a register */
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

  void
BX_CPU_C::SUB_RAXId(bxInstruction_c *i)
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


  void
BX_CPU_C::CMP_EqGq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, diff_64;

    /* op2_64 is a register, RMAddr(i) is an index of a register */
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

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMP64);
}


  void
BX_CPU_C::CMP_GqEq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, diff_64;

    /* op1_64 is a register, RMAddr(i) is an index of a register */
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

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMP64);
}


  void
BX_CPU_C::CMP_RAXId(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, diff_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id();

    diff_64 = op1_64 - op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMP64);
}


#ifdef ignore
  void
BX_CPU_C::CWDE64(bxInstruction_c *i)
{
  /* CBW: no flags are effected */

    RAX = (Bit16s) AX;
}
#endif

  void
BX_CPU_C::CDQE(bxInstruction_c *i)
{
  /* CWDE: no flags are affected */

    RAX = (Bit32s) EAX;
}

  void
BX_CPU_C::CQO(bxInstruction_c *i)
{
  /* CQO: no flags are affected */

    if (RAX & BX_CONST64(0x8000000000000000))
      RDX = BX_CONST64(0xffffffffffffffff);
    else
      RDX = 0;
}

#ifdef ignore_this
// not sure about these....

// Some info on the opcodes at {0F,A6} and {0F,A7}
// On 386 steps A0-B0:
//   {OF,A6} = XBTS
//   {OF,A7} = IBTS
// On 486 steps A0-B0:
//   {OF,A6} = CMPXCHG 8
//   {OF,A7} = CMPXCHG 16|64
//
// On 486 >= B steps, and further processors, the
// CMPXCHG instructions were moved to opcodes:
//   {OF,B0} = CMPXCHG 8
//   {OF,B1} = CMPXCHG 16|64

  void
BX_CPU_C::CMPXCHG_XBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_XBTS:"));
  UndefinedOpcode(i);
}

  void
BX_CPU_C::CMPXCHG_IBTS(bxInstruction_c *i)
{
  BX_INFO(("CMPXCHG_IBTS:"));
  UndefinedOpcode(i);
}

#endif


  void
BX_CPU_C::XADD_EqGq(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

    Bit64u op2_64, op1_64, sum_64;

    /* XADD dst(r/m), src(r)
     * temp <-- src + dst         | sum = op2 + op1
     * src  <-- dst               | op2 = op1
     * dst  <-- tmp               | op1 = sum
     */

    /* op2 is a register, RMAddr(i) is an index of a register */
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
      write_RMW_virtual_qword(sum_64);
      /* and write destination into source */
      BX_WRITE_64BIT_REG(i->nnn(), op1_64);
      }


    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_XADD64);
#else

#endif
}



  void
BX_CPU_C::ADD_EqId(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, sum_64;

    op2_64 = (Bit32s) i->Id();

    /* op1_64 is a register or memory reference */
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
      BX_WRITE_64BIT_REG(i->rm(), sum_64);
      }
    else {
      write_RMW_virtual_qword(sum_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_ADD64);
}

  void
BX_CPU_C::ADC_EqId(bxInstruction_c *i)
{
  bx_bool temp_CF;

  temp_CF = getB_CF();

    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, sum_64;

    op2_64 = (Bit32s) i->Id();

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    sum_64 = op1_64 + op2_64 + temp_CF;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), sum_64);
      }
    else {
      write_RMW_virtual_qword(sum_64);
      }

    SET_FLAGS_OSZAPC_64_CF(op1_64, op2_64, sum_64, BX_INSTR_ADC64,
                              temp_CF);
}


  void
BX_CPU_C::SUB_EqId(bxInstruction_c *i)
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
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    diff_64 = op1_64 - op2_64;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), diff_64);
      }
    else {
      write_RMW_virtual_qword(diff_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_SUB64);
}

  void
BX_CPU_C::CMP_EqId(bxInstruction_c *i)
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

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, diff_64, BX_INSTR_CMP64);
}




  void
BX_CPU_C::NEG_Eq(bxInstruction_c *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, diff_64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    diff_64 = 0 - op1_64;

    /* now write diff back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), diff_64);
      }
    else {
      write_RMW_virtual_qword(diff_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, 0, diff_64, BX_INSTR_NEG64);
}


  void
BX_CPU_C::INC_Eq(bxInstruction_c *i)
{
    Bit64u op1_64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    op1_64++;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }

    SET_FLAGS_OSZAP_64(0, 0, op1_64, BX_INSTR_INC64);
}


  void
BX_CPU_C::DEC_Eq(bxInstruction_c *i)
{
    Bit64u op1_64;

    /* op1_64 is a register or memory reference */
    if (i->modC0()) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    op1_64--;

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_64BIT_REG(i->rm(), op1_64);
      }
    else {
      write_RMW_virtual_qword(op1_64);
      }

    SET_FLAGS_OSZAP_64(0, 0, op1_64, BX_INSTR_DEC64);
}


  void
BX_CPU_C::CMPXCHG_EqGq(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 4) || (BX_CPU_LEVEL_HACKED >= 4)

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

    SET_FLAGS_OSZAPC_64(RAX, op1_64, diff_64, BX_INSTR_CMP64);

    if (diff_64 == 0) {  // if accumulator == dest
      // ZF = 1
      set_ZF(1);
      // dest <-- src
      op2_64 = BX_READ_64BIT_REG(i->nnn());

      if (i->modC0()) {
        BX_WRITE_64BIT_REG(i->rm(), op2_64);
        }
      else {
        write_RMW_virtual_qword(op2_64);
        }
      }
    else {
      // ZF = 0
      set_ZF(0);
      // accumulator <-- dest
      RAX = op1_64;
      }
#else
  BX_PANIC(("CMPXCHG_EqGq:"));
#endif
}


