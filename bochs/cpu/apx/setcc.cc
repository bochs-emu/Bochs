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

// SETcc.ZU reg (ND=1): the whole destination register is written with the condition (zero upper)
#define APX_SETCC_ZU_EbR(HANDLER, cond)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    BX_WRITE_32BIT_REGZ(i->dst(), (cond));                                                \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

APX_SETCC_ZU_EbR(SETO_ZU_EbR, get_OF() != 0)
APX_SETCC_ZU_EbR(SETNO_ZU_EbR, ! get_OF())
APX_SETCC_ZU_EbR(SETB_ZU_EbR, get_CF() != 0)
APX_SETCC_ZU_EbR(SETNB_ZU_EbR, ! get_CF())
APX_SETCC_ZU_EbR(SETZ_ZU_EbR, get_ZF() != 0)
APX_SETCC_ZU_EbR(SETNZ_ZU_EbR, ! get_ZF())
APX_SETCC_ZU_EbR(SETBE_ZU_EbR, (get_CF() || get_ZF()))
APX_SETCC_ZU_EbR(SETNBE_ZU_EbR, ! (get_CF() || get_ZF()))
APX_SETCC_ZU_EbR(SETS_ZU_EbR, get_SF() != 0)
APX_SETCC_ZU_EbR(SETNS_ZU_EbR, ! get_SF())
APX_SETCC_ZU_EbR(SETP_ZU_EbR, get_PF() != 0)
APX_SETCC_ZU_EbR(SETNP_ZU_EbR, ! get_PF())
APX_SETCC_ZU_EbR(SETL_ZU_EbR, getB_SF() != getB_OF())
APX_SETCC_ZU_EbR(SETNL_ZU_EbR, getB_SF() == getB_OF())
APX_SETCC_ZU_EbR(SETLE_ZU_EbR, (get_ZF() || (getB_SF() != getB_OF())))
APX_SETCC_ZU_EbR(SETNLE_ZU_EbR, (! get_ZF() && (getB_SF() == getB_OF())))

#endif
