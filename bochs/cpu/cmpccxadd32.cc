/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2022-2026 Stanislav Shwartsman
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

// CMPccXADD Ed, Gd, Bd: compare, add Bd to the memory operand when the condition is true
#define CMPCCXADD_EdGdBd(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit32u op2_32 = BX_READ_32BIT_REG(i->src1());                                         \
    Bit32u op3_32 = BX_READ_32BIT_REG(i->src2());                                         \
                                                                                          \
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i), laddr = get_laddr64(i->seg(), eaddr);      \
    if (laddr & 3) {                                                                      \
      BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));                 \
      exception(BX_GP_EXCEPTION, 0);                                                      \
    }                                                                                     \
                                                                                          \
    Bit32u op1_32 = read_RMW_linear_dword(i->seg(), laddr); /* implicit lock */           \
    Bit32u diff_32 = op1_32 - op2_32;                                                     \
    SET_FLAGS_OSZAPC_SUB_32(op1_32, op2_32, diff_32);                                     \
    write_RMW_linear_dword((cond) ? op1_32 + op3_32 : op1_32);                            \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->src1(), op1_32);                                               \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

CMPCCXADD_EdGdBd(CMPBEXADD_EdGdBd, get_CF() || get_ZF())
CMPCCXADD_EdGdBd(CMPBXADD_EdGdBd, get_CF())
CMPCCXADD_EdGdBd(CMPLEXADD_EdGdBd, get_ZF() || getB_SF() != getB_OF())
CMPCCXADD_EdGdBd(CMPLXADD_EdGdBd, getB_SF() != getB_OF())
CMPCCXADD_EdGdBd(CMPNBEXADD_EdGdBd, !get_CF() && !get_ZF())
CMPCCXADD_EdGdBd(CMPNBXADD_EdGdBd, !get_CF())
CMPCCXADD_EdGdBd(CMPNLEXADD_EdGdBd, !get_ZF() && getB_SF() == getB_OF())
CMPCCXADD_EdGdBd(CMPNLXADD_EdGdBd, getB_SF() == getB_OF())
CMPCCXADD_EdGdBd(CMPNOXADD_EdGdBd, !get_OF())
CMPCCXADD_EdGdBd(CMPNPXADD_EdGdBd, !get_PF())
CMPCCXADD_EdGdBd(CMPNSXADD_EdGdBd, !get_SF())
CMPCCXADD_EdGdBd(CMPNZXADD_EdGdBd, !get_ZF())
CMPCCXADD_EdGdBd(CMPOXADD_EdGdBd, get_OF())
CMPCCXADD_EdGdBd(CMPPXADD_EdGdBd, get_PF())
CMPCCXADD_EdGdBd(CMPSXADD_EdGdBd, get_SF())
CMPCCXADD_EdGdBd(CMPZXADD_EdGdBd, get_ZF())

#endif
