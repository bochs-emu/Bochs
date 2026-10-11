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

// JMPABS target64: direct jump to 64-bit absolute address
void BX_CPP_AttrRegparmN(1) BX_CPU_C::JMPABS(bxInstruction_c *i)
{
  Bit64u new_RIP = i->Iq();

  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("%s: canonical RIP violation", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  RIP = new_RIP;

  BX_INSTR_NEAR_BRANCH_TAKEN(BX_CPU_ID, BX_INSTR_IS_JMP, PREV_RIP, RIP);

  BX_LINK_TRACE(i);
}

#endif
