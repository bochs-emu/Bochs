/////////////////////////////////////////////////////////////////////////
// $Id: logical16.cc,v 1.27 2007-10-21 22:07:33 sshwarts Exp $
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


void BX_CPU_C::XOR_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());

    result_16 = op1_16 ^ op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 ^ op2_16;
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::XOR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;
  unsigned nnn = i->nnn();

  op1_16 = BX_READ_16BIT_REG(nnn);

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  result_16 = op1_16 ^ op2_16;

  BX_WRITE_16BIT_REG(nnn, result_16);

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::XOR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = AX;
  op2_16 = i->Iw();
  result_16 = op1_16 ^ op2_16;
  AX = result_16;

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::XOR_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 ^ op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 ^ op2_16;
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::OR_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 | op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 | op2_16;
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::NOT_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = ~op1_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = ~op1_16;
    write_RMW_virtual_word(result_16);
  }
}

void BX_CPU_C::OR_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 | op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 | op2_16;
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::OR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  result_16 = op1_16 | op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);

  BX_WRITE_16BIT_REG(i->nnn(), result_16);
}

void BX_CPU_C::OR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = AX;
  op2_16 = i->Iw();
  result_16 = op1_16 | op2_16;
  AX = result_16;

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::AND_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
    result_16 = op1_16 & op2_16;
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
    result_16 = op1_16 & op2_16;
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::AND_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  result_16 = op1_16 & op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);

  BX_WRITE_16BIT_REG(i->nnn(), result_16);
}

void BX_CPU_C::AND_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;

  op1_16 = AX;
  op2_16 = i->Iw();

  result_16 = op1_16 & op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);

  AX = result_16;
}

void BX_CPU_C::AND_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16, result_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());

    result_16 = op1_16 & op2_16;

    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);

    result_16 = op1_16 & op2_16;

    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::TEST_EwGw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  Bit16u result_16 = op1_16 & op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::TEST_AXIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op1_16 = AX;
  op2_16 = i->Iw();

  Bit16u result_16 = op1_16 & op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}

void BX_CPU_C::TEST_EwIw(bxInstruction_c *i)
{
  Bit16u op2_16, op1_16;

  op2_16 = i->Iw();

  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  Bit16u result_16 = op1_16 & op2_16;
  SET_FLAGS_OSZAPC_RESULT_16(result_16, BX_INSTR_LOGIC16);
}
