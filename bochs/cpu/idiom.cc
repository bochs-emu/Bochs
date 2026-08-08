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

// Integer ZERO IDIOM:
//    XOR reg, reg
//    SUB reg, reg

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ZERO_IDIOM_GwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->dst(), 0);
  SET_FLAGS_OSZAPC_LOGIC_16(0);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ZERO_IDIOM_GdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->dst(), 0);
  SET_FLAGS_OSZAPC_LOGIC_32(0);
  BX_NEXT_INSTR(i);
}

// Integer TEST FOR ZERO IDIOM:
//    TEST reg, reg

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_GwR_ZERO_IDIOM(bxInstruction_c *i)
{
  SET_FLAGS_OSZAPC_LOGIC_16(BX_READ_16BIT_REG(i->dst()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_GdR_ZERO_IDIOM(bxInstruction_c *i)
{
  SET_FLAGS_OSZAPC_LOGIC_32(BX_READ_32BIT_REG(i->dst()));
  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TEST_GqR_ZERO_IDIOM(bxInstruction_c *i)
{
  SET_FLAGS_OSZAPC_LOGIC_64(BX_READ_64BIT_REG(i->dst()));
  BX_NEXT_INSTR(i);
}
#endif

// SSE ZERO IDIOM:
//    PXOR xmm, xmm
//    XORPS xmm, xmm
//    XORPD xmm, xmm

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ZERO_IDIOM_SSE_VdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_XMM_REG(i->dst()).clear();
#endif
  BX_NEXT_INSTR(i);
}
