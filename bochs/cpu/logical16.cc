/////////////////////////////////////////////////////////////////////////
// $Id: logical16.cc,v 1.34 2007-11-20 17:15:33 sshwarts Exp $
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

void BX_CPU_C::XOR_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 ^= op2_16;
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::XOR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  Bit16u op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 ^= op2_16;
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::XOR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  op1_16 ^= op2_16;

  BX_WRITE_16BIT_REG(i->nnn(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::XOR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16;

  op1_16 = AX;
  op1_16 ^= i->Iw();
  AX = op1_16;

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::XOR_EwIwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op1_16 ^= i->Iw();
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::XOR_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  op1_16 ^= i->Iw();
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::OR_EwIwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op1_16 |= i->Iw();
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::OR_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  op1_16 |= i->Iw();
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::NOT_EwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op1_16 = ~op1_16;
  write_RMW_virtual_word(op1_16);
}

void BX_CPU_C::NOT_EwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  op1_16 = ~op1_16;
  BX_WRITE_16BIT_REG(i->rm(), op1_16);
}

void BX_CPU_C::OR_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 |= op2_16;
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::OR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 |= op2_16;
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::OR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  op1_16 |= op2_16;

  BX_WRITE_16BIT_REG(i->nnn(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::OR_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = AX;
  op2_16 = i->Iw();
  op1_16 |= op2_16;
  AX = op1_16;

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 &= op2_16;
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 &= op2_16;
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  op1_16 &= op2_16;

  BX_WRITE_16BIT_REG(i->nnn(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = AX;
  op2_16 = i->Iw();
  op1_16 &= op2_16;
  AX = op1_16;

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_EwIwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op1_16 &= i->Iw();
  write_RMW_virtual_word(op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::AND_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  op1_16 &= i->Iw();
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::TEST_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  read_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 &= op2_16;
  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::TEST_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op1_16 &= op2_16;
  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::TEST_AXIw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = AX;
  op2_16 = i->Iw();
  op1_16 &= op2_16;

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::TEST_EwIwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  read_virtual_word(i->seg(), RMAddr(i), &op1_16);
  op1_16 &= i->Iw();

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}

void BX_CPU_C::TEST_EwIwR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  op1_16 &= i->Iw();
  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
}
