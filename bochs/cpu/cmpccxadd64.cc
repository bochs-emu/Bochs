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

// CMPccXADD Eq, Gq, Bq: compare, add Bq to the memory operand when the condition is true
#define CMPCCXADD_EqGqBq(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit64u op2_64 = BX_READ_64BIT_REG(i->src1());                                         \
    Bit64u op3_64 = BX_READ_64BIT_REG(i->src2());                                         \
                                                                                          \
    bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i), laddr = get_laddr64(i->seg(), eaddr);   \
    if (laddr & 7) {                                                                      \
      BX_ERROR(("%s: #GP misaligned access", i->getIaOpcodeNameShort()));                 \
      exception(BX_GP_EXCEPTION, 0);                                                      \
    }                                                                                     \
                                                                                          \
    Bit64u op1_64 = read_RMW_linear_qword(i->seg(), laddr); /* implicit lock */           \
    Bit64u diff_64 = op1_64 - op2_64;                                                     \
    SET_FLAGS_OSZAPC_SUB_64(op1_64, op2_64, diff_64);                                     \
    write_RMW_linear_qword((cond) ? op1_64 + op3_64 : op1_64);                            \
                                                                                          \
    BX_WRITE_64BIT_REG(i->src1(), op1_64);                                                \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

CMPCCXADD_EqGqBq(CMPBEXADD_EqGqBq, get_CF() || get_ZF())
CMPCCXADD_EqGqBq(CMPBXADD_EqGqBq, get_CF())
CMPCCXADD_EqGqBq(CMPLEXADD_EqGqBq, get_ZF() || getB_SF() != getB_OF())
CMPCCXADD_EqGqBq(CMPLXADD_EqGqBq, getB_SF() != getB_OF())
CMPCCXADD_EqGqBq(CMPNBEXADD_EqGqBq, !get_CF() && !get_ZF())
CMPCCXADD_EqGqBq(CMPNBXADD_EqGqBq, !get_CF())
CMPCCXADD_EqGqBq(CMPNLEXADD_EqGqBq, !get_ZF() && getB_SF() == getB_OF())
CMPCCXADD_EqGqBq(CMPNLXADD_EqGqBq, getB_SF() == getB_OF())
CMPCCXADD_EqGqBq(CMPNOXADD_EqGqBq, !get_OF())
CMPCCXADD_EqGqBq(CMPNPXADD_EqGqBq, !get_PF())
CMPCCXADD_EqGqBq(CMPNSXADD_EqGqBq, !get_SF())
CMPCCXADD_EqGqBq(CMPNZXADD_EqGqBq, !get_ZF())
CMPCCXADD_EqGqBq(CMPOXADD_EqGqBq, get_OF())
CMPCCXADD_EqGqBq(CMPPXADD_EqGqBq, get_PF())
CMPCCXADD_EqGqBq(CMPSXADD_EqGqBq, get_SF())
CMPCCXADD_EqGqBq(CMPZXADD_EqGqBq, get_ZF())

#endif
