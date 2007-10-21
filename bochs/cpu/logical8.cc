/////////////////////////////////////////////////////////////////////////
// $Id: logical8.cc,v 1.31 2007-10-21 23:35:11 sshwarts Exp $
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


void BX_CPU_C::XOR_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 ^= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 ^= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::XOR_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
  }

  op1 ^= op2;

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::XOR_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 ^= op2;
   AL = op1;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::XOR_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 ^= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 ^= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::OR_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 |= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 |= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::NOT_Eb(bxInstruction_c *i)
{
  Bit8u op1_8;

  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1_8 = ~op1_8;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1_8);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
    op1_8 = ~op1_8;
    write_RMW_virtual_byte(op1_8);
  }
}

void BX_CPU_C::OR_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 |= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 |= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::OR_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
  }

  op1 |= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
}

void BX_CPU_C::OR_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 |= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);

  AL = op1;
}

void BX_CPU_C::AND_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 &= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 &= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::AND_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
  }

  op1 &= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
}

void BX_CPU_C::AND_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2;

  op1 = AL;
  op2 = i->Ib();
  op1 &= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);

  AL = op1;
}

void BX_CPU_C::AND_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    op1 &= op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op1);
  }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    op1 &= op2;
    write_RMW_virtual_byte(op1);
  }

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::TEST_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
  }

  op1 &= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::TEST_ALIb(bxInstruction_c *i)
{
  Bit8u op1 = AL;
  Bit8u op2 = i->Ib();
  op1 &= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}

void BX_CPU_C::TEST_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
  }

  op1 &= op2;

  SET_FLAGS_OSZAPC_RESULT_8(op1, BX_INSTR_LOGIC8);
}
