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






#include "bochs.h"




  void
BX_CPU_C::XOR_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
}


  void
BX_CPU_C::XOR_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, result_16;

    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(i->nnn, result_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
}


  void
BX_CPU_C::XOR_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    sum_16 = op1_16 ^ op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_XOR16);
}

  void
BX_CPU_C::XOR_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 ^ op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_XOR16);
}


  void
BX_CPU_C::OR_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
}


  void
BX_CPU_C::NOT_Ew(BxInstruction_t *i)
{
    Bit16u op1_16, result_16;

    /* op1 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = ~op1_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }
}


  void
BX_CPU_C::OR_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
}


  void
BX_CPU_C::OR_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, result_16;


    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    result_16 = op1_16 | op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(i->nnn, result_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_OR16);
}


  void
BX_CPU_C::OR_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    sum_16 = op1_16 | op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_OR16);
}



  void
BX_CPU_C::AND_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;



    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
}


  void
BX_CPU_C::AND_GwEw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, result_16;


    op1_16 = BX_READ_16BIT_REG(i->nnn);

    /* op2_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op2_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op2_16);
      }

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(i->nnn, result_16);

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
}


  void
BX_CPU_C::AND_AXIw(BxInstruction_t *i)
{
    Bit16u op1_16, op2_16, sum_16;

    op1_16 = AX;

    op2_16 = i->Iw;

    sum_16 = op1_16 & op2_16;

    /* now write sum back to destination */
    AX = sum_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, sum_16, BX_INSTR_AND16);
}

  void
BX_CPU_C::AND_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;

    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 & op2_16;

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_16BIT_REG(i->rm, result_16);
      }
    else {
      write_RMW_virtual_word(result_16);
      }

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_AND16);
}


  void
BX_CPU_C::TEST_EwGw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn);

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 & op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
}



  void
BX_CPU_C::TEST_AXIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;

    op1_16 = AX;

    /* op2_16 is imm16 */
    op2_16 = i->Iw;

    result_16 = op1_16 & op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
}


  void
BX_CPU_C::TEST_EwIw(BxInstruction_t *i)
{
    Bit16u op2_16, op1_16, result_16;


    /* op2_16 is imm16 */
    op2_16 = i->Iw;

    /* op1_16 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_16 = BX_READ_16BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg, i->rm_addr, &op1_16);
      }

    result_16 = op1_16 & op2_16;

    SET_FLAGS_OSZAPC_16(op1_16, op2_16, result_16, BX_INSTR_TEST16);
}
