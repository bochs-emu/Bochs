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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CCMPF(bxInstruction_c *i)
{
  setDFV_OSZAPC(i->dfv());
  BX_NEXT_INSTR(i);
}

// CCMPxx Gb, Eb, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_GbEbR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_GbEbR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_GbEbR(CCMPO_GbEbR, get_OF())
APX_CCMP_GbEbR(CCMPNO_GbEbR, ! get_OF())
APX_CCMP_GbEbR(CCMPB_GbEbR, get_CF())
APX_CCMP_GbEbR(CCMPNB_GbEbR, ! get_CF())
APX_CCMP_GbEbR(CCMPZ_GbEbR, get_ZF())
APX_CCMP_GbEbR(CCMPNZ_GbEbR, ! get_ZF())
APX_CCMP_GbEbR(CCMPBE_GbEbR, get_CF() || get_ZF())
APX_CCMP_GbEbR(CCMPNBE_GbEbR, ! (get_CF() || get_ZF()))
APX_CCMP_GbEbR(CCMPS_GbEbR, get_SF())
APX_CCMP_GbEbR(CCMPNS_GbEbR, ! get_SF())
APX_CCMP_GbEbR(CCMPL_GbEbR, getB_SF() != getB_OF())
APX_CCMP_GbEbR(CCMPNL_GbEbR, getB_SF() == getB_OF())
APX_CCMP_GbEbR(CCMPLE_GbEbR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_GbEbR(CCMPNLE_GbEbR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Eb, Ib, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_EbIbR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_EbIbR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_EbIbR(CCMPO_EbIbR, get_OF())
APX_CCMP_EbIbR(CCMPNO_EbIbR, ! get_OF())
APX_CCMP_EbIbR(CCMPB_EbIbR, get_CF())
APX_CCMP_EbIbR(CCMPNB_EbIbR, ! get_CF())
APX_CCMP_EbIbR(CCMPZ_EbIbR, get_ZF())
APX_CCMP_EbIbR(CCMPNZ_EbIbR, ! get_ZF())
APX_CCMP_EbIbR(CCMPBE_EbIbR, get_CF() || get_ZF())
APX_CCMP_EbIbR(CCMPNBE_EbIbR, ! (get_CF() || get_ZF()))
APX_CCMP_EbIbR(CCMPS_EbIbR, get_SF())
APX_CCMP_EbIbR(CCMPNS_EbIbR, ! get_SF())
APX_CCMP_EbIbR(CCMPL_EbIbR, getB_SF() != getB_OF())
APX_CCMP_EbIbR(CCMPNL_EbIbR, getB_SF() == getB_OF())
APX_CCMP_EbIbR(CCMPLE_EbIbR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_EbIbR(CCMPNLE_EbIbR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Gw, Ew, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_GwEwR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_GwEwR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_GwEwR(CCMPO_GwEwR, get_OF())
APX_CCMP_GwEwR(CCMPNO_GwEwR, ! get_OF())
APX_CCMP_GwEwR(CCMPB_GwEwR, get_CF())
APX_CCMP_GwEwR(CCMPNB_GwEwR, ! get_CF())
APX_CCMP_GwEwR(CCMPZ_GwEwR, get_ZF())
APX_CCMP_GwEwR(CCMPNZ_GwEwR, ! get_ZF())
APX_CCMP_GwEwR(CCMPBE_GwEwR, get_CF() || get_ZF())
APX_CCMP_GwEwR(CCMPNBE_GwEwR, ! (get_CF() || get_ZF()))
APX_CCMP_GwEwR(CCMPS_GwEwR, get_SF())
APX_CCMP_GwEwR(CCMPNS_GwEwR, ! get_SF())
APX_CCMP_GwEwR(CCMPL_GwEwR, getB_SF() != getB_OF())
APX_CCMP_GwEwR(CCMPNL_GwEwR, getB_SF() == getB_OF())
APX_CCMP_GwEwR(CCMPLE_GwEwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_GwEwR(CCMPNLE_GwEwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Ew, Iw, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_EwIwR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_EwIwR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_EwIwR(CCMPO_EwIwR, get_OF())
APX_CCMP_EwIwR(CCMPNO_EwIwR, ! get_OF())
APX_CCMP_EwIwR(CCMPB_EwIwR, get_CF())
APX_CCMP_EwIwR(CCMPNB_EwIwR, ! get_CF())
APX_CCMP_EwIwR(CCMPZ_EwIwR, get_ZF())
APX_CCMP_EwIwR(CCMPNZ_EwIwR, ! get_ZF())
APX_CCMP_EwIwR(CCMPBE_EwIwR, get_CF() || get_ZF())
APX_CCMP_EwIwR(CCMPNBE_EwIwR, ! (get_CF() || get_ZF()))
APX_CCMP_EwIwR(CCMPS_EwIwR, get_SF())
APX_CCMP_EwIwR(CCMPNS_EwIwR, ! get_SF())
APX_CCMP_EwIwR(CCMPL_EwIwR, getB_SF() != getB_OF())
APX_CCMP_EwIwR(CCMPNL_EwIwR, getB_SF() == getB_OF())
APX_CCMP_EwIwR(CCMPLE_EwIwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_EwIwR(CCMPNLE_EwIwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Gd, Ed, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_GdEdR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_GdEdR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_GdEdR(CCMPO_GdEdR, get_OF())
APX_CCMP_GdEdR(CCMPNO_GdEdR, ! get_OF())
APX_CCMP_GdEdR(CCMPB_GdEdR, get_CF())
APX_CCMP_GdEdR(CCMPNB_GdEdR, ! get_CF())
APX_CCMP_GdEdR(CCMPZ_GdEdR, get_ZF())
APX_CCMP_GdEdR(CCMPNZ_GdEdR, ! get_ZF())
APX_CCMP_GdEdR(CCMPBE_GdEdR, get_CF() || get_ZF())
APX_CCMP_GdEdR(CCMPNBE_GdEdR, ! (get_CF() || get_ZF()))
APX_CCMP_GdEdR(CCMPS_GdEdR, get_SF())
APX_CCMP_GdEdR(CCMPNS_GdEdR, ! get_SF())
APX_CCMP_GdEdR(CCMPL_GdEdR, getB_SF() != getB_OF())
APX_CCMP_GdEdR(CCMPNL_GdEdR, getB_SF() == getB_OF())
APX_CCMP_GdEdR(CCMPLE_GdEdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_GdEdR(CCMPNLE_GdEdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Ed, Id, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_EdIdR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_EdIdR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_EdIdR(CCMPO_EdIdR, get_OF())
APX_CCMP_EdIdR(CCMPNO_EdIdR, ! get_OF())
APX_CCMP_EdIdR(CCMPB_EdIdR, get_CF())
APX_CCMP_EdIdR(CCMPNB_EdIdR, ! get_CF())
APX_CCMP_EdIdR(CCMPZ_EdIdR, get_ZF())
APX_CCMP_EdIdR(CCMPNZ_EdIdR, ! get_ZF())
APX_CCMP_EdIdR(CCMPBE_EdIdR, get_CF() || get_ZF())
APX_CCMP_EdIdR(CCMPNBE_EdIdR, ! (get_CF() || get_ZF()))
APX_CCMP_EdIdR(CCMPS_EdIdR, get_SF())
APX_CCMP_EdIdR(CCMPNS_EdIdR, ! get_SF())
APX_CCMP_EdIdR(CCMPL_EdIdR, getB_SF() != getB_OF())
APX_CCMP_EdIdR(CCMPNL_EdIdR, getB_SF() == getB_OF())
APX_CCMP_EdIdR(CCMPLE_EdIdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_EdIdR(CCMPNLE_EdIdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Gq, Eq, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_GqEqR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_GqEqR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_GqEqR(CCMPO_GqEqR, get_OF())
APX_CCMP_GqEqR(CCMPNO_GqEqR, ! get_OF())
APX_CCMP_GqEqR(CCMPB_GqEqR, get_CF())
APX_CCMP_GqEqR(CCMPNB_GqEqR, ! get_CF())
APX_CCMP_GqEqR(CCMPZ_GqEqR, get_ZF())
APX_CCMP_GqEqR(CCMPNZ_GqEqR, ! get_ZF())
APX_CCMP_GqEqR(CCMPBE_GqEqR, get_CF() || get_ZF())
APX_CCMP_GqEqR(CCMPNBE_GqEqR, ! (get_CF() || get_ZF()))
APX_CCMP_GqEqR(CCMPS_GqEqR, get_SF())
APX_CCMP_GqEqR(CCMPNS_GqEqR, ! get_SF())
APX_CCMP_GqEqR(CCMPL_GqEqR, getB_SF() != getB_OF())
APX_CCMP_GqEqR(CCMPNL_GqEqR, getB_SF() == getB_OF())
APX_CCMP_GqEqR(CCMPLE_GqEqR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_GqEqR(CCMPNLE_GqEqR, ! get_ZF() && (getB_SF() == getB_OF()))

// CCMPxx Eq, Id, dfv: compare when the source condition is true, otherwise flags = dfv
#define APX_CCMP_EqIdR(HANDLER, cond)                                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return CMP_EqIdR(i);                                                                \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CCMP_EqIdR(CCMPO_EqIdR, get_OF())
APX_CCMP_EqIdR(CCMPNO_EqIdR, ! get_OF())
APX_CCMP_EqIdR(CCMPB_EqIdR, get_CF())
APX_CCMP_EqIdR(CCMPNB_EqIdR, ! get_CF())
APX_CCMP_EqIdR(CCMPZ_EqIdR, get_ZF())
APX_CCMP_EqIdR(CCMPNZ_EqIdR, ! get_ZF())
APX_CCMP_EqIdR(CCMPBE_EqIdR, get_CF() || get_ZF())
APX_CCMP_EqIdR(CCMPNBE_EqIdR, ! (get_CF() || get_ZF()))
APX_CCMP_EqIdR(CCMPS_EqIdR, get_SF())
APX_CCMP_EqIdR(CCMPNS_EqIdR, ! get_SF())
APX_CCMP_EqIdR(CCMPL_EqIdR, getB_SF() != getB_OF())
APX_CCMP_EqIdR(CCMPNL_EqIdR, getB_SF() == getB_OF())
APX_CCMP_EqIdR(CCMPLE_EqIdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CCMP_EqIdR(CCMPNLE_EqIdR, ! get_ZF() && (getB_SF() == getB_OF()))

#endif
