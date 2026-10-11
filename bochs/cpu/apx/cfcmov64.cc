/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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

#if BX_SUPPORT_APX

// CFCMOVcc reg, reg (ND=0, NF=0|1): destination is zeroed when the condition is false
#define APX_CFCMOV_GqEqR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit64u op_64 = 0;                                                                     \
    if (cond)                                                                             \
      op_64 = BX_READ_64BIT_REG(i->src());                                                \
                                                                                          \
    BX_WRITE_64BIT_REG(i->dst(), op_64);                                                  \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GqEqR(CFCMOVO_GqEqR, get_OF())
APX_CFCMOV_GqEqR(CFCMOVNO_GqEqR, ! get_OF())
APX_CFCMOV_GqEqR(CFCMOVB_GqEqR, get_CF())
APX_CFCMOV_GqEqR(CFCMOVNB_GqEqR, ! get_CF())
APX_CFCMOV_GqEqR(CFCMOVZ_GqEqR, get_ZF())
APX_CFCMOV_GqEqR(CFCMOVNZ_GqEqR, ! get_ZF())
APX_CFCMOV_GqEqR(CFCMOVBE_GqEqR, get_CF() || get_ZF())
APX_CFCMOV_GqEqR(CFCMOVNBE_GqEqR, ! (get_CF() || get_ZF()))
APX_CFCMOV_GqEqR(CFCMOVS_GqEqR, get_SF())
APX_CFCMOV_GqEqR(CFCMOVNS_GqEqR, ! get_SF())
APX_CFCMOV_GqEqR(CFCMOVP_GqEqR, get_PF())
APX_CFCMOV_GqEqR(CFCMOVNP_GqEqR, ! get_PF())
APX_CFCMOV_GqEqR(CFCMOVL_GqEqR, getB_SF() != getB_OF())
APX_CFCMOV_GqEqR(CFCMOVNL_GqEqR, getB_SF() == getB_OF())
APX_CFCMOV_GqEqR(CFCMOVLE_GqEqR, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GqEqR(CFCMOVNLE_GqEqR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc reg, mem (ND=0, NF=0): memory faults are suppressed when the condition is false
#define APX_CFCMOV_GqEqM(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit64u op_64 = 0;                                                                     \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);                                       \
      op_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));                  \
    }                                                                                     \
                                                                                          \
    BX_WRITE_64BIT_REG(i->dst(), op_64);                                                  \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GqEqM(CFCMOVO_GqEqM, get_OF())
APX_CFCMOV_GqEqM(CFCMOVNO_GqEqM, ! get_OF())
APX_CFCMOV_GqEqM(CFCMOVB_GqEqM, get_CF())
APX_CFCMOV_GqEqM(CFCMOVNB_GqEqM, ! get_CF())
APX_CFCMOV_GqEqM(CFCMOVZ_GqEqM, get_ZF())
APX_CFCMOV_GqEqM(CFCMOVNZ_GqEqM, ! get_ZF())
APX_CFCMOV_GqEqM(CFCMOVBE_GqEqM, get_CF() || get_ZF())
APX_CFCMOV_GqEqM(CFCMOVNBE_GqEqM, ! (get_CF() || get_ZF()))
APX_CFCMOV_GqEqM(CFCMOVS_GqEqM, get_SF())
APX_CFCMOV_GqEqM(CFCMOVNS_GqEqM, ! get_SF())
APX_CFCMOV_GqEqM(CFCMOVP_GqEqM, get_PF())
APX_CFCMOV_GqEqM(CFCMOVNP_GqEqM, ! get_PF())
APX_CFCMOV_GqEqM(CFCMOVL_GqEqM, getB_SF() != getB_OF())
APX_CFCMOV_GqEqM(CFCMOVNL_GqEqM, getB_SF() == getB_OF())
APX_CFCMOV_GqEqM(CFCMOVLE_GqEqM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GqEqM(CFCMOVNLE_GqEqM, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc mem, reg (ND=0, NF=1): no memory write and no memory faults when the condition is false
#define APX_CFCMOV_EqGqM(HANDLER, cond)                                                         \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                           \
  {                                                                                             \
    if (cond) {                                                                                 \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);                                             \
      write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), BX_READ_64BIT_REG(i->src()));  \
    }                                                                                           \
                                                                                                \
    BX_NEXT_INSTR(i);                                                                           \
  }

APX_CFCMOV_EqGqM(CFCMOVO_EqGqM, get_OF())
APX_CFCMOV_EqGqM(CFCMOVNO_EqGqM, ! get_OF())
APX_CFCMOV_EqGqM(CFCMOVB_EqGqM, get_CF())
APX_CFCMOV_EqGqM(CFCMOVNB_EqGqM, ! get_CF())
APX_CFCMOV_EqGqM(CFCMOVZ_EqGqM, get_ZF())
APX_CFCMOV_EqGqM(CFCMOVNZ_EqGqM, ! get_ZF())
APX_CFCMOV_EqGqM(CFCMOVBE_EqGqM, get_CF() || get_ZF())
APX_CFCMOV_EqGqM(CFCMOVNBE_EqGqM, ! (get_CF() || get_ZF()))
APX_CFCMOV_EqGqM(CFCMOVS_EqGqM, get_SF())
APX_CFCMOV_EqGqM(CFCMOVNS_EqGqM, ! get_SF())
APX_CFCMOV_EqGqM(CFCMOVP_EqGqM, get_PF())
APX_CFCMOV_EqGqM(CFCMOVNP_EqGqM, ! get_PF())
APX_CFCMOV_EqGqM(CFCMOVL_EqGqM, getB_SF() != getB_OF())
APX_CFCMOV_EqGqM(CFCMOVNL_EqGqM, getB_SF() == getB_OF())
APX_CFCMOV_EqGqM(CFCMOVLE_EqGqM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_EqGqM(CFCMOVNLE_EqGqM, ! get_ZF() && (getB_SF() == getB_OF()))

// CMOVcc / CFCMOVcc ndd, reg, reg (ND=1), also CMOVcc ndd, reg, mem after the memory operand is loaded
#define APX_CMOV_BqGqEqR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit64u op_64 = BX_READ_64BIT_REG(i->src1());                                          \
    if (cond)                                                                             \
      op_64 = BX_READ_64BIT_REG(i->src2());                                               \
                                                                                          \
    BX_WRITE_64BIT_REG(i->dst(), op_64);                                                  \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CMOV_BqGqEqR(CMOVO_BqGqEqR, get_OF())
APX_CMOV_BqGqEqR(CMOVNO_BqGqEqR, ! get_OF())
APX_CMOV_BqGqEqR(CMOVB_BqGqEqR, get_CF())
APX_CMOV_BqGqEqR(CMOVNB_BqGqEqR, ! get_CF())
APX_CMOV_BqGqEqR(CMOVZ_BqGqEqR, get_ZF())
APX_CMOV_BqGqEqR(CMOVNZ_BqGqEqR, ! get_ZF())
APX_CMOV_BqGqEqR(CMOVBE_BqGqEqR, get_CF() || get_ZF())
APX_CMOV_BqGqEqR(CMOVNBE_BqGqEqR, ! (get_CF() || get_ZF()))
APX_CMOV_BqGqEqR(CMOVS_BqGqEqR, get_SF())
APX_CMOV_BqGqEqR(CMOVNS_BqGqEqR, ! get_SF())
APX_CMOV_BqGqEqR(CMOVP_BqGqEqR, get_PF())
APX_CMOV_BqGqEqR(CMOVNP_BqGqEqR, ! get_PF())
APX_CMOV_BqGqEqR(CMOVL_BqGqEqR, getB_SF() != getB_OF())
APX_CMOV_BqGqEqR(CMOVNL_BqGqEqR, getB_SF() == getB_OF())
APX_CMOV_BqGqEqR(CMOVLE_BqGqEqR, get_ZF() || (getB_SF() != getB_OF()))
APX_CMOV_BqGqEqR(CMOVNLE_BqGqEqR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc ndd, reg, mem (ND=1, NF=1): memory faults are suppressed when the condition is false
#define APX_CFCMOV_BqGqEqM(HANDLER, cond)                                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit64u op_64 = BX_READ_64BIT_REG(i->src1());                                          \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);                                       \
      op_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));                  \
    }                                                                                     \
                                                                                          \
    BX_WRITE_64BIT_REG(i->dst(), op_64);                                                  \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_BqGqEqM(CFCMOVO_BqGqEqM, get_OF())
APX_CFCMOV_BqGqEqM(CFCMOVNO_BqGqEqM, ! get_OF())
APX_CFCMOV_BqGqEqM(CFCMOVB_BqGqEqM, get_CF())
APX_CFCMOV_BqGqEqM(CFCMOVNB_BqGqEqM, ! get_CF())
APX_CFCMOV_BqGqEqM(CFCMOVZ_BqGqEqM, get_ZF())
APX_CFCMOV_BqGqEqM(CFCMOVNZ_BqGqEqM, ! get_ZF())
APX_CFCMOV_BqGqEqM(CFCMOVBE_BqGqEqM, get_CF() || get_ZF())
APX_CFCMOV_BqGqEqM(CFCMOVNBE_BqGqEqM, ! (get_CF() || get_ZF()))
APX_CFCMOV_BqGqEqM(CFCMOVS_BqGqEqM, get_SF())
APX_CFCMOV_BqGqEqM(CFCMOVNS_BqGqEqM, ! get_SF())
APX_CFCMOV_BqGqEqM(CFCMOVP_BqGqEqM, get_PF())
APX_CFCMOV_BqGqEqM(CFCMOVNP_BqGqEqM, ! get_PF())
APX_CFCMOV_BqGqEqM(CFCMOVL_BqGqEqM, getB_SF() != getB_OF())
APX_CFCMOV_BqGqEqM(CFCMOVNL_BqGqEqM, getB_SF() == getB_OF())
APX_CFCMOV_BqGqEqM(CFCMOVLE_BqGqEqM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_BqGqEqM(CFCMOVNLE_BqGqEqM, ! get_ZF() && (getB_SF() == getB_OF()))

#endif
