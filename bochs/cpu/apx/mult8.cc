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

// EVEX map4 one-operand MUL/IMUL with NF=1: same as legacy, but flags are not updated

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_MUL_ALEbR(bxInstruction_c *i)
{
  Bit8u op1 = AL;
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* now write product back to destination */
  AX = ((Bit16u) op1) * ((Bit16u) op2);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::APX_NF_IMUL_ALEbR(bxInstruction_c *i)
{
  Bit8s op1 = AL;
  Bit8s op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* now write product back to destination */
  AX = op1 * op2;

  BX_NEXT_INSTR(i);
}

#endif
