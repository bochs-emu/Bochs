/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2022 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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

#if BX_SUPPORT_AVX

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPBEXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((get_CF() || get_ZF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPBXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(get_CF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPLEXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((get_ZF() || getB_SF() != getB_OF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPLXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((getB_SF() != getB_OF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNBEXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((!get_CF() && !get_ZF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNBXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(!get_CF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNLEXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((!get_ZF() && getB_SF() == getB_OF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNLXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((getB_SF() == getB_OF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNOXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(!get_OF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNPXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(!get_PF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNSXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(!get_SF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPNZXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword((!get_ZF()) ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPOXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(get_OF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPPXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(get_PF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPSXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(get_SF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPZXADD_EdGdBd(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());
  Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr(i->seg(), eaddr);
  if (laddr & 3) {
    BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); // implicit lock
  Bit32u diff_32 = op1_32 - op2_32;
  SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);
  write_RMW_linear_dword(get_ZF() ? op1_32 + op3_32 : op1_32);

  BX_WRITE_32BIT_REGZ(i->src1(), op1_32);

  BX_NEXT_INSTR(i);
}

#endif
