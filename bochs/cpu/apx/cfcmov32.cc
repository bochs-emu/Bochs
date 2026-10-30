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
#define APX_CFCMOV_GdEdR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit32u op_32 = 0;                                                                     \
    if (cond)                                                                             \
      op_32 = BX_READ_32BIT_REG(i->src());                                                \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_32);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GdEdR(CFCMOVO_GdEdR, get_OF())
APX_CFCMOV_GdEdR(CFCMOVNO_GdEdR, ! get_OF())
APX_CFCMOV_GdEdR(CFCMOVB_GdEdR, get_CF())
APX_CFCMOV_GdEdR(CFCMOVNB_GdEdR, ! get_CF())
APX_CFCMOV_GdEdR(CFCMOVZ_GdEdR, get_ZF())
APX_CFCMOV_GdEdR(CFCMOVNZ_GdEdR, ! get_ZF())
APX_CFCMOV_GdEdR(CFCMOVBE_GdEdR, get_CF() || get_ZF())
APX_CFCMOV_GdEdR(CFCMOVNBE_GdEdR, ! (get_CF() || get_ZF()))
APX_CFCMOV_GdEdR(CFCMOVS_GdEdR, get_SF())
APX_CFCMOV_GdEdR(CFCMOVNS_GdEdR, ! get_SF())
APX_CFCMOV_GdEdR(CFCMOVP_GdEdR, get_PF())
APX_CFCMOV_GdEdR(CFCMOVNP_GdEdR, ! get_PF())
APX_CFCMOV_GdEdR(CFCMOVL_GdEdR, getB_SF() != getB_OF())
APX_CFCMOV_GdEdR(CFCMOVNL_GdEdR, getB_SF() == getB_OF())
APX_CFCMOV_GdEdR(CFCMOVLE_GdEdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GdEdR(CFCMOVNLE_GdEdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc reg, mem (ND=0, NF=0): memory faults are suppressed when the condition is false
#define APX_CFCMOV_GdEdM(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit32u op_32 = 0;                                                                     \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      op_32 = read_virtual_dword(i->seg(), eaddr);                                        \
    }                                                                                     \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_32);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_GdEdM(CFCMOVO_GdEdM, get_OF())
APX_CFCMOV_GdEdM(CFCMOVNO_GdEdM, ! get_OF())
APX_CFCMOV_GdEdM(CFCMOVB_GdEdM, get_CF())
APX_CFCMOV_GdEdM(CFCMOVNB_GdEdM, ! get_CF())
APX_CFCMOV_GdEdM(CFCMOVZ_GdEdM, get_ZF())
APX_CFCMOV_GdEdM(CFCMOVNZ_GdEdM, ! get_ZF())
APX_CFCMOV_GdEdM(CFCMOVBE_GdEdM, get_CF() || get_ZF())
APX_CFCMOV_GdEdM(CFCMOVNBE_GdEdM, ! (get_CF() || get_ZF()))
APX_CFCMOV_GdEdM(CFCMOVS_GdEdM, get_SF())
APX_CFCMOV_GdEdM(CFCMOVNS_GdEdM, ! get_SF())
APX_CFCMOV_GdEdM(CFCMOVP_GdEdM, get_PF())
APX_CFCMOV_GdEdM(CFCMOVNP_GdEdM, ! get_PF())
APX_CFCMOV_GdEdM(CFCMOVL_GdEdM, getB_SF() != getB_OF())
APX_CFCMOV_GdEdM(CFCMOVNL_GdEdM, getB_SF() == getB_OF())
APX_CFCMOV_GdEdM(CFCMOVLE_GdEdM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_GdEdM(CFCMOVNLE_GdEdM, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc mem, reg (ND=0, NF=1): no memory write and no memory faults when the condition is false
#define APX_CFCMOV_EdGdM(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      write_virtual_dword(i->seg(), eaddr, BX_READ_32BIT_REG(i->src()));                  \
    }                                                                                     \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_EdGdM(CFCMOVO_EdGdM, get_OF())
APX_CFCMOV_EdGdM(CFCMOVNO_EdGdM, ! get_OF())
APX_CFCMOV_EdGdM(CFCMOVB_EdGdM, get_CF())
APX_CFCMOV_EdGdM(CFCMOVNB_EdGdM, ! get_CF())
APX_CFCMOV_EdGdM(CFCMOVZ_EdGdM, get_ZF())
APX_CFCMOV_EdGdM(CFCMOVNZ_EdGdM, ! get_ZF())
APX_CFCMOV_EdGdM(CFCMOVBE_EdGdM, get_CF() || get_ZF())
APX_CFCMOV_EdGdM(CFCMOVNBE_EdGdM, ! (get_CF() || get_ZF()))
APX_CFCMOV_EdGdM(CFCMOVS_EdGdM, get_SF())
APX_CFCMOV_EdGdM(CFCMOVNS_EdGdM, ! get_SF())
APX_CFCMOV_EdGdM(CFCMOVP_EdGdM, get_PF())
APX_CFCMOV_EdGdM(CFCMOVNP_EdGdM, ! get_PF())
APX_CFCMOV_EdGdM(CFCMOVL_EdGdM, getB_SF() != getB_OF())
APX_CFCMOV_EdGdM(CFCMOVNL_EdGdM, getB_SF() == getB_OF())
APX_CFCMOV_EdGdM(CFCMOVLE_EdGdM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_EdGdM(CFCMOVNLE_EdGdM, ! get_ZF() && (getB_SF() == getB_OF()))

// CMOVcc / CFCMOVcc ndd, reg, reg (ND=1), also CMOVcc ndd, reg, mem after the memory operand is loaded
#define APX_CMOV_BdGdEdR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit32u op_32 = BX_READ_32BIT_REG(i->src1());                                          \
    if (cond)                                                                             \
      op_32 = BX_READ_32BIT_REG(i->src2());                                               \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_32);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CMOV_BdGdEdR(CMOVO_BdGdEdR, get_OF())
APX_CMOV_BdGdEdR(CMOVNO_BdGdEdR, ! get_OF())
APX_CMOV_BdGdEdR(CMOVB_BdGdEdR, get_CF())
APX_CMOV_BdGdEdR(CMOVNB_BdGdEdR, ! get_CF())
APX_CMOV_BdGdEdR(CMOVZ_BdGdEdR, get_ZF())
APX_CMOV_BdGdEdR(CMOVNZ_BdGdEdR, ! get_ZF())
APX_CMOV_BdGdEdR(CMOVBE_BdGdEdR, get_CF() || get_ZF())
APX_CMOV_BdGdEdR(CMOVNBE_BdGdEdR, ! (get_CF() || get_ZF()))
APX_CMOV_BdGdEdR(CMOVS_BdGdEdR, get_SF())
APX_CMOV_BdGdEdR(CMOVNS_BdGdEdR, ! get_SF())
APX_CMOV_BdGdEdR(CMOVP_BdGdEdR, get_PF())
APX_CMOV_BdGdEdR(CMOVNP_BdGdEdR, ! get_PF())
APX_CMOV_BdGdEdR(CMOVL_BdGdEdR, getB_SF() != getB_OF())
APX_CMOV_BdGdEdR(CMOVNL_BdGdEdR, getB_SF() == getB_OF())
APX_CMOV_BdGdEdR(CMOVLE_BdGdEdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CMOV_BdGdEdR(CMOVNLE_BdGdEdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CFCMOVcc ndd, reg, mem (ND=1, NF=1): memory faults are suppressed when the condition is false
#define APX_CFCMOV_BdGdEdM(HANDLER, cond)                                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    Bit32u op_32 = BX_READ_32BIT_REG(i->src1());                                          \
    if (cond) {                                                                           \
      bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                          \
      op_32 = read_virtual_dword(i->seg(), eaddr);                                        \
    }                                                                                     \
                                                                                          \
    BX_WRITE_32BIT_REGZ(i->dst(), op_32);                                                 \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CFCMOV_BdGdEdM(CFCMOVO_BdGdEdM, get_OF())
APX_CFCMOV_BdGdEdM(CFCMOVNO_BdGdEdM, ! get_OF())
APX_CFCMOV_BdGdEdM(CFCMOVB_BdGdEdM, get_CF())
APX_CFCMOV_BdGdEdM(CFCMOVNB_BdGdEdM, ! get_CF())
APX_CFCMOV_BdGdEdM(CFCMOVZ_BdGdEdM, get_ZF())
APX_CFCMOV_BdGdEdM(CFCMOVNZ_BdGdEdM, ! get_ZF())
APX_CFCMOV_BdGdEdM(CFCMOVBE_BdGdEdM, get_CF() || get_ZF())
APX_CFCMOV_BdGdEdM(CFCMOVNBE_BdGdEdM, ! (get_CF() || get_ZF()))
APX_CFCMOV_BdGdEdM(CFCMOVS_BdGdEdM, get_SF())
APX_CFCMOV_BdGdEdM(CFCMOVNS_BdGdEdM, ! get_SF())
APX_CFCMOV_BdGdEdM(CFCMOVP_BdGdEdM, get_PF())
APX_CFCMOV_BdGdEdM(CFCMOVNP_BdGdEdM, ! get_PF())
APX_CFCMOV_BdGdEdM(CFCMOVL_BdGdEdM, getB_SF() != getB_OF())
APX_CFCMOV_BdGdEdM(CFCMOVNL_BdGdEdM, getB_SF() == getB_OF())
APX_CFCMOV_BdGdEdM(CFCMOVLE_BdGdEdM, get_ZF() || (getB_SF() != getB_OF()))
APX_CFCMOV_BdGdEdM(CFCMOVNLE_BdGdEdM, ! get_ZF() && (getB_SF() == getB_OF()))

#endif
