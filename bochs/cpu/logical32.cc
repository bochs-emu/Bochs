//  Copyright (C) 2000  MandrakeSoft S.A.
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






#define BX_IN_CPU_METHOD 1
#include "bochs.h"




  void
BX_CPU_C::XOR_EdGd(BxInstruction_t *i)
{
    /* for 32 bit operand size mode */
    Bit32u op2_32, op1_32, result_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
}


  void
BX_CPU_C::XOR_GdEd(BxInstruction_t *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, result_32;

    op1_32 = BX_READ_32BIT_REG(i->nnn);

    /* op2_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(i->nnn, result_32);

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
}


  void
BX_CPU_C::XOR_EAXId(BxInstruction_t *i)
{
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = i->Id;

    sum_32 = op1_32 ^ op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_XOR32);
}

  void
BX_CPU_C::XOR_EdId(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    op2_32 = i->Id;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 ^ op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_XOR32);
}


  void
BX_CPU_C::OR_EdId(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    op2_32 = i->Id;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
}

  void
BX_CPU_C::NOT_Ed(BxInstruction_t *i)
{
    Bit32u op1_32, result_32;

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = ~op1_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }
}


  void
BX_CPU_C::OR_EdGd(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
}


  void
BX_CPU_C::OR_GdEd(BxInstruction_t *i)
{
    Bit32u op1_32, op2_32, result_32;

    op1_32 = BX_READ_32BIT_REG(i->nnn);

    /* op2_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }

    result_32 = op1_32 | op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(i->nnn, result_32);

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_OR32);
}


  void
BX_CPU_C::OR_EAXId(BxInstruction_t *i)
{
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = i->Id;

    sum_32 = op1_32 | op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_OR32);
}



  void
BX_CPU_C::AND_EdGd(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
}


  void
BX_CPU_C::AND_GdEd(BxInstruction_t *i)
{
    Bit32u op1_32, op2_32, result_32;

    op1_32 = BX_READ_32BIT_REG(i->nnn);

    /* op2_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(i->nnn, result_32);

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
}


  void
BX_CPU_C::AND_EAXId(BxInstruction_t *i)
{
    Bit32u op1_32, op2_32, sum_32;

    op1_32 = EAX;

    op2_32 = i->Id;

    sum_32 = op1_32 & op2_32;

    /* now write sum back to destination */
    EAX = sum_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, sum_32, BX_INSTR_AND32);
}

  void
BX_CPU_C::AND_EdId(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    op2_32 = i->Id;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 & op2_32;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, result_32);
      }
    else {
      write_RMW_virtual_dword(result_32);
      }

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_AND32);
}


  void
BX_CPU_C::TEST_EdGd(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 & op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
}



  void
BX_CPU_C::TEST_EAXId(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    /* op1 is EAX register */
    op1_32 = EAX;

    /* op2 is imm32 */
    op2_32 = i->Id;

    result_32 = op1_32 & op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
}


  void
BX_CPU_C::TEST_EdId(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32, result_32;

    /* op2 is imm32 */
    op2_32 = i->Id;

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op1_32);
      }

    result_32 = op1_32 & op2_32;

    SET_FLAGS_OSZAPC_32(op1_32, op2_32, result_32, BX_INSTR_TEST32);
}
