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
BX_CPU_C::XOR_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 ^ op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}

  void
BX_CPU_C::XOR_GbEb(BxInstruction_t *i)
{
  Bit8u op1, op2, result;

  op1 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  result = op1 ^ op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, result);

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}


  void
BX_CPU_C::XOR_ALIb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;

  op1 = AL;

  op2 = i->Ib;

  sum = op1 ^ op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_XOR8);
}


  void
BX_CPU_C::XOR_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 ^ op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}



  void
BX_CPU_C::OR_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 | op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  void
BX_CPU_C::NOT_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;


  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  result_8 = ~op1_8;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result_8);
    }
  else {
    write_RMW_virtual_byte(result_8);
    }
}


  void
BX_CPU_C::OR_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;


  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 | op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  void
BX_CPU_C::OR_GbEb(BxInstruction_t *i)
{
  Bit8u op1, op2, result;


  op1 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  result = op1 | op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, result);


  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  void
BX_CPU_C::OR_ALIb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = i->Ib;

  sum = op1 | op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_OR8);
}



  void
BX_CPU_C::AND_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 & op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}


  void
BX_CPU_C::AND_GbEb(BxInstruction_t *i)
{
  Bit8u op1, op2, result;

  op1 = BX_READ_8BIT_REG(i->nnn);

  /* op2 is a register or memory reference */
  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  result = op1 & op2;

  /* now write result back to destination, which is a register */
  BX_WRITE_8BIT_REG(i->nnn, result);

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}


  void
BX_CPU_C::AND_ALIb(BxInstruction_t *i)
{
  Bit8u op1, op2, sum;


  op1 = AL;

  op2 = i->Ib;

  sum = op1 & op2;

  /* now write sum back to destination, which is a register */
  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_AND8);
}




  void
BX_CPU_C::AND_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;


  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 & op2;

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result);
    }
  else {
    write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
}


  void
BX_CPU_C::TEST_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}


  void
BX_CPU_C::TEST_ALIb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  /* op1 is the AL register */
  op1 = AL;

  /* op2 is imm8 */
  op2 = i->Ib;

  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}



  void
BX_CPU_C::TEST_EbIb(BxInstruction_t *i)
{
  Bit8u op2, op1, result;

  op2 = i->Ib;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op1);
    }

  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
}
