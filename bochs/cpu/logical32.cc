/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 ^= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 ^= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  op1_32 ^= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 ^= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 ^= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 |= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 |= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 = ~op1_32;
  write_RMW_virtual_dword(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 = ~op1_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 |= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 |= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  op1_32 |= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 &= op2_32;
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 &= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GdEdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = read_virtual_dword(i->seg(), eaddr);
  op1_32 &= op2_32;
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdIdM(bxInstruction_c *i)
{
  Bit32u op1_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  op1_32 &= i->Id();
  write_RMW_virtual_dword(op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 &= i->Id();
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdGdM(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_32 = read_virtual_dword(i->seg(), eaddr);
  op2_32 = BX_READ_32BIT_REG(i->src());
  op1_32 &= op2_32;

  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdIdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  op1_32 &= i->Id();
  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EdIdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_virtual_dword(i->seg(), eaddr);
  op1_32 &= i->Id();
  SET_FLAGS_OSZAPC_LOGIC_32(op1_32);

  BX_NEXT_INSTR(i);
}
