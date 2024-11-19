/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2024  The Bochs Project
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AADD_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);

  if (laddr & 0x3) {
    BX_ERROR(("%s: access not aligned to 4-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  write_RMW_linear_dword(op1_32 + op2_32);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AADD_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr64(i->seg(), eaddr);

  if (laddr & 0x7) {
    BX_ERROR(("%s: access not aligned to 8-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), eaddr);
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  write_RMW_linear_qword(op1_64 + op2_64);
  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AAND_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);

  if (laddr & 0x3) {
    BX_ERROR(("%s: access not aligned to 4-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  write_RMW_linear_dword(op1_32 & op2_32);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AAND_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr64(i->seg(), eaddr);

  if (laddr & 0x7) {
    BX_ERROR(("%s: access not aligned to 8-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), eaddr);
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  write_RMW_linear_qword(op1_64 & op2_64);
  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AOR_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);

  if (laddr & 0x3) {
    BX_ERROR(("%s: access not aligned to 4-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  write_RMW_linear_dword(op1_32 | op2_32);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AOR_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr64(i->seg(), eaddr);

  if (laddr & 0x7) {
    BX_ERROR(("%s: access not aligned to 8-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), eaddr);
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  write_RMW_linear_qword(op1_64 | op2_64);
  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::AXOR_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = agen_read_aligned(i->seg(), eaddr, 4);

  if (laddr & 0x3) {
    BX_ERROR(("%s: access not aligned to 4-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  write_RMW_linear_dword(op1_32 ^ op2_32);
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::AXOR_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr64(i->seg(), eaddr);

  if (laddr & 0x7) {
    BX_ERROR(("%s: access not aligned to 8-byte cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // #GP(0) if memory type if not #WB not implemented yet

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), eaddr);
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  write_RMW_linear_qword(op1_64 ^ op2_64);
  BX_NEXT_INSTR(i);
}
#endif
