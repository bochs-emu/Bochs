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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CTESTF(bxInstruction_c *i)
{
  setDFV_OSZAPC(i->dfv());
  BX_NEXT_INSTR(i);
}

// CTESTxx Gb, Eb, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_GbEbR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EbGbR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_GbEbR(CTESTO_GbEbR, get_OF())
APX_CTEST_GbEbR(CTESTNO_GbEbR, ! get_OF())
APX_CTEST_GbEbR(CTESTB_GbEbR, get_CF())
APX_CTEST_GbEbR(CTESTNB_GbEbR, ! get_CF())
APX_CTEST_GbEbR(CTESTZ_GbEbR, get_ZF())
APX_CTEST_GbEbR(CTESTNZ_GbEbR, ! get_ZF())
APX_CTEST_GbEbR(CTESTBE_GbEbR, get_CF() || get_ZF())
APX_CTEST_GbEbR(CTESTNBE_GbEbR, ! (get_CF() || get_ZF()))
APX_CTEST_GbEbR(CTESTS_GbEbR, get_SF())
APX_CTEST_GbEbR(CTESTNS_GbEbR, ! get_SF())
APX_CTEST_GbEbR(CTESTL_GbEbR, getB_SF() != getB_OF())
APX_CTEST_GbEbR(CTESTNL_GbEbR, getB_SF() == getB_OF())
APX_CTEST_GbEbR(CTESTLE_GbEbR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_GbEbR(CTESTNLE_GbEbR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Eb, Ib, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_EbIbR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EbIbR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_EbIbR(CTESTO_EbIbR, get_OF())
APX_CTEST_EbIbR(CTESTNO_EbIbR, ! get_OF())
APX_CTEST_EbIbR(CTESTB_EbIbR, get_CF())
APX_CTEST_EbIbR(CTESTNB_EbIbR, ! get_CF())
APX_CTEST_EbIbR(CTESTZ_EbIbR, get_ZF())
APX_CTEST_EbIbR(CTESTNZ_EbIbR, ! get_ZF())
APX_CTEST_EbIbR(CTESTBE_EbIbR, get_CF() || get_ZF())
APX_CTEST_EbIbR(CTESTNBE_EbIbR, ! (get_CF() || get_ZF()))
APX_CTEST_EbIbR(CTESTS_EbIbR, get_SF())
APX_CTEST_EbIbR(CTESTNS_EbIbR, ! get_SF())
APX_CTEST_EbIbR(CTESTL_EbIbR, getB_SF() != getB_OF())
APX_CTEST_EbIbR(CTESTNL_EbIbR, getB_SF() == getB_OF())
APX_CTEST_EbIbR(CTESTLE_EbIbR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_EbIbR(CTESTNLE_EbIbR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Gw, Ew, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_GwEwR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EwGwR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_GwEwR(CTESTO_GwEwR, get_OF())
APX_CTEST_GwEwR(CTESTNO_GwEwR, ! get_OF())
APX_CTEST_GwEwR(CTESTB_GwEwR, get_CF())
APX_CTEST_GwEwR(CTESTNB_GwEwR, ! get_CF())
APX_CTEST_GwEwR(CTESTZ_GwEwR, get_ZF())
APX_CTEST_GwEwR(CTESTNZ_GwEwR, ! get_ZF())
APX_CTEST_GwEwR(CTESTBE_GwEwR, get_CF() || get_ZF())
APX_CTEST_GwEwR(CTESTNBE_GwEwR, ! (get_CF() || get_ZF()))
APX_CTEST_GwEwR(CTESTS_GwEwR, get_SF())
APX_CTEST_GwEwR(CTESTNS_GwEwR, ! get_SF())
APX_CTEST_GwEwR(CTESTL_GwEwR, getB_SF() != getB_OF())
APX_CTEST_GwEwR(CTESTNL_GwEwR, getB_SF() == getB_OF())
APX_CTEST_GwEwR(CTESTLE_GwEwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_GwEwR(CTESTNLE_GwEwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Ew, Iw, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_EwIwR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EwIwR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_EwIwR(CTESTO_EwIwR, get_OF())
APX_CTEST_EwIwR(CTESTNO_EwIwR, ! get_OF())
APX_CTEST_EwIwR(CTESTB_EwIwR, get_CF())
APX_CTEST_EwIwR(CTESTNB_EwIwR, ! get_CF())
APX_CTEST_EwIwR(CTESTZ_EwIwR, get_ZF())
APX_CTEST_EwIwR(CTESTNZ_EwIwR, ! get_ZF())
APX_CTEST_EwIwR(CTESTBE_EwIwR, get_CF() || get_ZF())
APX_CTEST_EwIwR(CTESTNBE_EwIwR, ! (get_CF() || get_ZF()))
APX_CTEST_EwIwR(CTESTS_EwIwR, get_SF())
APX_CTEST_EwIwR(CTESTNS_EwIwR, ! get_SF())
APX_CTEST_EwIwR(CTESTL_EwIwR, getB_SF() != getB_OF())
APX_CTEST_EwIwR(CTESTNL_EwIwR, getB_SF() == getB_OF())
APX_CTEST_EwIwR(CTESTLE_EwIwR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_EwIwR(CTESTNLE_EwIwR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Gd, Ed, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_GdEdR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EdGdR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_GdEdR(CTESTO_GdEdR, get_OF())
APX_CTEST_GdEdR(CTESTNO_GdEdR, ! get_OF())
APX_CTEST_GdEdR(CTESTB_GdEdR, get_CF())
APX_CTEST_GdEdR(CTESTNB_GdEdR, ! get_CF())
APX_CTEST_GdEdR(CTESTZ_GdEdR, get_ZF())
APX_CTEST_GdEdR(CTESTNZ_GdEdR, ! get_ZF())
APX_CTEST_GdEdR(CTESTBE_GdEdR, get_CF() || get_ZF())
APX_CTEST_GdEdR(CTESTNBE_GdEdR, ! (get_CF() || get_ZF()))
APX_CTEST_GdEdR(CTESTS_GdEdR, get_SF())
APX_CTEST_GdEdR(CTESTNS_GdEdR, ! get_SF())
APX_CTEST_GdEdR(CTESTL_GdEdR, getB_SF() != getB_OF())
APX_CTEST_GdEdR(CTESTNL_GdEdR, getB_SF() == getB_OF())
APX_CTEST_GdEdR(CTESTLE_GdEdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_GdEdR(CTESTNLE_GdEdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Ed, Id, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_EdIdR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EdIdR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_EdIdR(CTESTO_EdIdR, get_OF())
APX_CTEST_EdIdR(CTESTNO_EdIdR, ! get_OF())
APX_CTEST_EdIdR(CTESTB_EdIdR, get_CF())
APX_CTEST_EdIdR(CTESTNB_EdIdR, ! get_CF())
APX_CTEST_EdIdR(CTESTZ_EdIdR, get_ZF())
APX_CTEST_EdIdR(CTESTNZ_EdIdR, ! get_ZF())
APX_CTEST_EdIdR(CTESTBE_EdIdR, get_CF() || get_ZF())
APX_CTEST_EdIdR(CTESTNBE_EdIdR, ! (get_CF() || get_ZF()))
APX_CTEST_EdIdR(CTESTS_EdIdR, get_SF())
APX_CTEST_EdIdR(CTESTNS_EdIdR, ! get_SF())
APX_CTEST_EdIdR(CTESTL_EdIdR, getB_SF() != getB_OF())
APX_CTEST_EdIdR(CTESTNL_EdIdR, getB_SF() == getB_OF())
APX_CTEST_EdIdR(CTESTLE_EdIdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_EdIdR(CTESTNLE_EdIdR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Gq, Eq, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_GqEqR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EqGqR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_GqEqR(CTESTO_GqEqR, get_OF())
APX_CTEST_GqEqR(CTESTNO_GqEqR, ! get_OF())
APX_CTEST_GqEqR(CTESTB_GqEqR, get_CF())
APX_CTEST_GqEqR(CTESTNB_GqEqR, ! get_CF())
APX_CTEST_GqEqR(CTESTZ_GqEqR, get_ZF())
APX_CTEST_GqEqR(CTESTNZ_GqEqR, ! get_ZF())
APX_CTEST_GqEqR(CTESTBE_GqEqR, get_CF() || get_ZF())
APX_CTEST_GqEqR(CTESTNBE_GqEqR, ! (get_CF() || get_ZF()))
APX_CTEST_GqEqR(CTESTS_GqEqR, get_SF())
APX_CTEST_GqEqR(CTESTNS_GqEqR, ! get_SF())
APX_CTEST_GqEqR(CTESTL_GqEqR, getB_SF() != getB_OF())
APX_CTEST_GqEqR(CTESTNL_GqEqR, getB_SF() == getB_OF())
APX_CTEST_GqEqR(CTESTLE_GqEqR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_GqEqR(CTESTNLE_GqEqR, ! get_ZF() && (getB_SF() == getB_OF()))

// CTESTxx Eq, Id, dfv: test when the source condition is true, otherwise flags = dfv
#define APX_CTEST_EqIdR(HANDLER, cond)                                                    \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      return TEST_EqIdR(i);                                                               \
                                                                                          \
    setDFV_OSZAPC(i->dfv());                                                              \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_CTEST_EqIdR(CTESTO_EqIdR, get_OF())
APX_CTEST_EqIdR(CTESTNO_EqIdR, ! get_OF())
APX_CTEST_EqIdR(CTESTB_EqIdR, get_CF())
APX_CTEST_EqIdR(CTESTNB_EqIdR, ! get_CF())
APX_CTEST_EqIdR(CTESTZ_EqIdR, get_ZF())
APX_CTEST_EqIdR(CTESTNZ_EqIdR, ! get_ZF())
APX_CTEST_EqIdR(CTESTBE_EqIdR, get_CF() || get_ZF())
APX_CTEST_EqIdR(CTESTNBE_EqIdR, ! (get_CF() || get_ZF()))
APX_CTEST_EqIdR(CTESTS_EqIdR, get_SF())
APX_CTEST_EqIdR(CTESTNS_EqIdR, ! get_SF())
APX_CTEST_EqIdR(CTESTL_EqIdR, getB_SF() != getB_OF())
APX_CTEST_EqIdR(CTESTNL_EqIdR, getB_SF() == getB_OF())
APX_CTEST_EqIdR(CTESTLE_EqIdR, get_ZF() || (getB_SF() != getB_OF()))
APX_CTEST_EqIdR(CTESTNLE_EqIdR, ! get_ZF() && (getB_SF() == getB_OF()))

#endif
