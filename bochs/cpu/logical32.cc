/////////////////////////////////////////////////////////////////////////
// $Id: logical32.cc,v 1.35 2007-11-20 17:15:33 sshwarts Exp $
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RAX EAX
#endif

void BX_CPU_C::XOR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 ^= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::XOR_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 ^= op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::XOR_GdEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  op1_32 ^= op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::XOR_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32;

  op1_32 = EAX;
  op1_32 ^= i->Id();
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::XOR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 ^= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::XOR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 ^= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::OR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 |= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::OR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 |= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::NOT_EdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 = ~op1_32;
  write_RMW_virtual_dword(op1_32);
}

void BX_CPU_C::NOT_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 = ~op1_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

void BX_CPU_C::OR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 |= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::OR_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 |= op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::OR_GdEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  op1_32 |= op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::OR_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 |= op2_32;
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_GdEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  op1_32 &= op2_32;

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 &= op2_32;
  RAX = op1_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 &= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::AND_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 &= i->Id();
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::TEST_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::TEST_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::TEST_EAXId(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = EAX;
  op2_32 = i->Id();
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::TEST_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  read_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  op1_32 &= i->Id();

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}

void BX_CPU_C::TEST_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  op1_32 &= i->Id();
  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
}
