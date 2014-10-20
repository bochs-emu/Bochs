/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

#if BX_SUPPORT_X86_64

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 ^= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 ^= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 ^= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 ^= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XOR_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64 ^= op2_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 |= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64 |= op2_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 = ~op1_64;
  write_RMW_virtual_qword(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::NOT_EqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64 = ~op1_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), eaddr);
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 |= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 |= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::OR_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 |= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 &= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 &= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_GqEqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 &= op2_64;

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  op1_64 &= op2_64;
  write_RMW_virtual_qword(op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::AND_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64 = (Bit32s) i->Id();

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op1_64 &= op2_64;
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 &= op2_64;

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EqGqM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op2_64 = BX_READ_64BIT_REG(i->src());
  op1_64 &= op2_64;

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EqIdR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = (Bit32s) i->Id();
  op1_64 &= op2_64;

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_EqIdM(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  op2_64 = (Bit32s) i->Id();
  op1_64 &= op2_64;

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);

  BX_NEXT_INSTR(i);
}

#endif /* if BX_SUPPORT_X86_64 */
