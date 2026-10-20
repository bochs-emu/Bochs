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
#define APX_CFCMOV_GwEwR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit16u op_16 = 0;                                                                     \
    if (cond)                                                                             \
      op_16 = BX_READ_16BIT_REG(i->src());                                                \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_16);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GwEwR(CFCMOVO_GwEwR, get_OF())
APX_CFCMOV_GwEwR(CFCMOVNO_GwEwR, ! get_OF())
APX_CFCMOV_GwEwR(CFCMOVB_GwEwR, get_CF())
APX_CFCMOV_GwEwR(CFCMOVNB_GwEwR, ! get_CF())
APX_CFCMOV_GwEwR(CFCMOVZ_GwEwR, get_ZF())
APX_CFCMOV_GwEwR(CFCMOVNZ_GwEwR, ! get_ZF())
APX_CFCMOV_GwEwR(CFCMOVBE_GwEwR, get_CF() || get_ZF())
APX_CFCMOV_GwEwR(CFCMOVNBE_GwEwR, ! (get_CF() || get_ZF()))
APX_CFCMOV_GwEwR(CFCMOVS_GwEwR, get_SF())
APX_CFCMOV_GwEwR(CFCMOVNS_GwEwR, ! get_SF())
APX_CFCMOV_GwEwR(CFCMOVP_GwEwR, get_PF())
APX_CFCMOV_GwEwR(CFCMOVNP_GwEwR, ! get_PF())
APX_CFCMOV_GwEwR(CFCMOVL_GwEwR, getB_SF() != getB_OF())
APX_CFCMOV_GwEwR(CFCMOVNL_GwEwR, getB_SF() == getB_OF())
APX_CFCMOV_GwEwR(CFCMOVLE_GwEwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GwEwR(CFCMOVNLE_GwEwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc reg, mem (ND=0, NF=0): memory faults are suppressed when the condition is false
#define APX_CFCMOV_GwEwM(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit16u op_16 = 0;                                                                     \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      op_16 = read_virtual_word(i->seg(), eaddr);                                         \
    }                                                                                     \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_16);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GwEwM(CFCMOVO_GwEwM, get_OF())
APX_CFCMOV_GwEwM(CFCMOVNO_GwEwM, ! get_OF())
APX_CFCMOV_GwEwM(CFCMOVB_GwEwM, get_CF())
APX_CFCMOV_GwEwM(CFCMOVNB_GwEwM, ! get_CF())
APX_CFCMOV_GwEwM(CFCMOVZ_GwEwM, get_ZF())
APX_CFCMOV_GwEwM(CFCMOVNZ_GwEwM, ! get_ZF())
APX_CFCMOV_GwEwM(CFCMOVBE_GwEwM, get_CF() || get_ZF())
APX_CFCMOV_GwEwM(CFCMOVNBE_GwEwM, ! (get_CF() || get_ZF()))
APX_CFCMOV_GwEwM(CFCMOVS_GwEwM, get_SF())
APX_CFCMOV_GwEwM(CFCMOVNS_GwEwM, ! get_SF())
APX_CFCMOV_GwEwM(CFCMOVP_GwEwM, get_PF())
APX_CFCMOV_GwEwM(CFCMOVNP_GwEwM, ! get_PF())
APX_CFCMOV_GwEwM(CFCMOVL_GwEwM, getB_SF() != getB_OF())
APX_CFCMOV_GwEwM(CFCMOVNL_GwEwM, getB_SF() == getB_OF())
APX_CFCMOV_GwEwM(CFCMOVLE_GwEwM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GwEwM(CFCMOVNLE_GwEwM, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc mem, reg (ND=0, NF=1): no memory write and no memory faults when the condition is false
#define APX_CFCMOV_EwGwM(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      write_virtual_word(i->seg(), eaddr, BX_READ_16BIT_REG(i->src()));                   \
    }                                                                                     \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_EwGwM(CFCMOVO_EwGwM, get_OF())
APX_CFCMOV_EwGwM(CFCMOVNO_EwGwM, ! get_OF())
APX_CFCMOV_EwGwM(CFCMOVB_EwGwM, get_CF())
APX_CFCMOV_EwGwM(CFCMOVNB_EwGwM, ! get_CF())
APX_CFCMOV_EwGwM(CFCMOVZ_EwGwM, get_ZF())
APX_CFCMOV_EwGwM(CFCMOVNZ_EwGwM, ! get_ZF())
APX_CFCMOV_EwGwM(CFCMOVBE_EwGwM, get_CF() || get_ZF())
APX_CFCMOV_EwGwM(CFCMOVNBE_EwGwM, ! (get_CF() || get_ZF()))
APX_CFCMOV_EwGwM(CFCMOVS_EwGwM, get_SF())
APX_CFCMOV_EwGwM(CFCMOVNS_EwGwM, ! get_SF())
APX_CFCMOV_EwGwM(CFCMOVP_EwGwM, get_PF())
APX_CFCMOV_EwGwM(CFCMOVNP_EwGwM, ! get_PF())
APX_CFCMOV_EwGwM(CFCMOVL_EwGwM, getB_SF() != getB_OF())
APX_CFCMOV_EwGwM(CFCMOVNL_EwGwM, getB_SF() == getB_OF())
APX_CFCMOV_EwGwM(CFCMOVLE_EwGwM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_EwGwM(CFCMOVNLE_EwGwM, ! get_ZF() && (getB_SF() == getB_OF()))

// CMOVcc / CFCMOVcc ndd, reg, reg (ND=1), also CMOVcc ndd, reg, mem after the memory operand is loaded
#define APX_CMOV_BwGwEwR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit16u op_16 = BX_READ_16BIT_REG(i->src1());                                          \
    if (cond)                                                                             \
      op_16 = BX_READ_16BIT_REG(i->src2());                                               \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_16);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CMOV_BwGwEwR(CMOVO_BwGwEwR, get_OF())
APX_CMOV_BwGwEwR(CMOVNO_BwGwEwR, ! get_OF())
APX_CMOV_BwGwEwR(CMOVB_BwGwEwR, get_CF())
APX_CMOV_BwGwEwR(CMOVNB_BwGwEwR, ! get_CF())
APX_CMOV_BwGwEwR(CMOVZ_BwGwEwR, get_ZF())
APX_CMOV_BwGwEwR(CMOVNZ_BwGwEwR, ! get_ZF())
APX_CMOV_BwGwEwR(CMOVBE_BwGwEwR, get_CF() || get_ZF())
APX_CMOV_BwGwEwR(CMOVNBE_BwGwEwR, ! (get_CF() || get_ZF()))
APX_CMOV_BwGwEwR(CMOVS_BwGwEwR, get_SF())
APX_CMOV_BwGwEwR(CMOVNS_BwGwEwR, ! get_SF())
APX_CMOV_BwGwEwR(CMOVP_BwGwEwR, get_PF())
APX_CMOV_BwGwEwR(CMOVNP_BwGwEwR, ! get_PF())
APX_CMOV_BwGwEwR(CMOVL_BwGwEwR, getB_SF() != getB_OF())
APX_CMOV_BwGwEwR(CMOVNL_BwGwEwR, getB_SF() == getB_OF())
APX_CMOV_BwGwEwR(CMOVLE_BwGwEwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CMOV_BwGwEwR(CMOVNLE_BwGwEwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc ndd, reg, mem (ND=1, NF=1): memory faults are suppressed when the condition is false
#define APX_CFCMOV_BwGwEwM(HANDLER, cond)                                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit16u op_16 = BX_READ_16BIT_REG(i->src1());                                          \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      op_16 = read_virtual_word(i->seg(), eaddr);                                         \
    }                                                                                     \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_16);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_BwGwEwM(CFCMOVO_BwGwEwM, get_OF())
APX_CFCMOV_BwGwEwM(CFCMOVNO_BwGwEwM, ! get_OF())
APX_CFCMOV_BwGwEwM(CFCMOVB_BwGwEwM, get_CF())
APX_CFCMOV_BwGwEwM(CFCMOVNB_BwGwEwM, ! get_CF())
APX_CFCMOV_BwGwEwM(CFCMOVZ_BwGwEwM, get_ZF())
APX_CFCMOV_BwGwEwM(CFCMOVNZ_BwGwEwM, ! get_ZF())
APX_CFCMOV_BwGwEwM(CFCMOVBE_BwGwEwM, get_CF() || get_ZF())
APX_CFCMOV_BwGwEwM(CFCMOVNBE_BwGwEwM, ! (get_CF() || get_ZF()))
APX_CFCMOV_BwGwEwM(CFCMOVS_BwGwEwM, get_SF())
APX_CFCMOV_BwGwEwM(CFCMOVNS_BwGwEwM, ! get_SF())
APX_CFCMOV_BwGwEwM(CFCMOVP_BwGwEwM, get_PF())
APX_CFCMOV_BwGwEwM(CFCMOVNP_BwGwEwM, ! get_PF())
APX_CFCMOV_BwGwEwM(CFCMOVL_BwGwEwM, getB_SF() != getB_OF())
APX_CFCMOV_BwGwEwM(CFCMOVNL_BwGwEwM, getB_SF() == getB_OF())
APX_CFCMOV_BwGwEwM(CFCMOVLE_BwGwEwM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_BwGwEwM(CFCMOVNLE_BwGwEwM, ! get_ZF() && (getB_SF() == getB_OF()))

#endif
