/////////////////////////////////////////////////////////////////////////
// $Id: logical64.cc,v 1.1 2002-09-13 15:53:22 kevinlawton Exp $
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




  void
BX_CPU_C::XOR_EqGq(BxInstruction_t *i)
{
    /* for 64 bit operand size mode */
    Bit64u op2_64, op1_64, result_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 ^ op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_XOR64);
}


  void
BX_CPU_C::XOR_GqEq(BxInstruction_t *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, result_64;

    op1_64 = BX_READ_64BIT_REG(i->nnn);

    /* op2_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg, i->rm_addr, &op2_64);
      }

    result_64 = op1_64 ^ op2_64;

    /* now write result back to destination */
    BX_WRITE_64BIT_REG(i->nnn, result_64);

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_XOR64);
}


  void
BX_CPU_C::XOR_RAXId(BxInstruction_t *i)
{
    /* for 64 bit operand size mode */
    Bit64u op1_64, op2_64, sum_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id;

    sum_64 = op1_64 ^ op2_64;

    /* now write sum back to destination */
    RAX = sum_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_XOR64);
}

  void
BX_CPU_C::XOR_EqId(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    op2_64 = (Bit32s) i->Id;

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 ^ op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_XOR64);
}


  void
BX_CPU_C::OR_EqId(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    op2_64 = (Bit32s) i->Id;

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 | op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_OR64);
}

  void
BX_CPU_C::NOT_Eq(BxInstruction_t *i)
{
    Bit64u op1_64, result_64;

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = ~op1_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }
}


  void
BX_CPU_C::OR_EqGq(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 | op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_OR64);
}


  void
BX_CPU_C::OR_GqEq(BxInstruction_t *i)
{
    Bit64u op1_64, op2_64, result_64;

    op1_64 = BX_READ_64BIT_REG(i->nnn);

    /* op2_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg, i->rm_addr, &op2_64);
      }

    result_64 = op1_64 | op2_64;

    /* now write result back to destination */
    BX_WRITE_64BIT_REG(i->nnn, result_64);

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_OR64);
}


  void
BX_CPU_C::OR_RAXId(BxInstruction_t *i)
{
    Bit64u op1_64, op2_64, sum_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id;

    sum_64 = op1_64 | op2_64;

    /* now write sum back to destination */
    RAX = sum_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_OR64);
}



  void
BX_CPU_C::AND_EqGq(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 & op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_AND64);
}


  void
BX_CPU_C::AND_GqEq(BxInstruction_t *i)
{
    Bit64u op1_64, op2_64, result_64;

    op1_64 = BX_READ_64BIT_REG(i->nnn);

    /* op2_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg, i->rm_addr, &op2_64);
      }

    result_64 = op1_64 & op2_64;

    /* now write result back to destination */
    BX_WRITE_64BIT_REG(i->nnn, result_64);

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_AND64);
}


  void
BX_CPU_C::AND_RAXId(BxInstruction_t *i)
{
    Bit64u op1_64, op2_64, sum_64;

    op1_64 = RAX;

    op2_64 = (Bit32s) i->Id;

    sum_64 = op1_64 & op2_64;

    /* now write sum back to destination */
    RAX = sum_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, sum_64, BX_INSTR_AND64);
}

  void
BX_CPU_C::AND_EqId(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    op2_64 = (Bit32s) i->Id;

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 & op2_64;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, result_64);
      }
    else {
      write_RMW_virtual_qword(result_64);
      }

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_AND64);
}


  void
BX_CPU_C::TEST_EqGq(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 & op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_TEST64);
}



  void
BX_CPU_C::TEST_RAXId(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    /* op1 is RAX register */
    op1_64 = RAX;

    /* op2 is imm64 */
    op2_64 = (Bit32s) i->Id;

    result_64 = op1_64 & op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_TEST64);
}


  void
BX_CPU_C::TEST_EqId(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64, result_64;

    /* op2 is imm64 */
    op2_64 = (Bit32s) i->Id;

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg, i->rm_addr, &op1_64);
      }

    result_64 = op1_64 & op2_64;

    SET_FLAGS_OSZAPC_64(op1_64, op2_64, result_64, BX_INSTR_TEST64);
}
