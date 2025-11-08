/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2025 Stanislav Shwartsman
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

// FIXME: long_mode64 requirement must be handled through opcode tables attributes

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ERETS(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FRED()) {
    BX_ERROR(("%s: FRED in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL must be 0", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  speculative_RSP();
  RSP += 8;                // skip error code so that RSP references the return state

  Bit64u new_RIP = pop_64();
  Bit64u temp_CS = pop_64();
  Bit64u new_RFLAGS = pop_64();
  Bit64u new_RSP = pop_64();
  Bit64u temp_SS = pop_64();

  if (! isCanonical(new_RIP) /* || tempCS & 0xFFFFFFFF_FFF8FFFF != current CS */ /* newRFLAGS & 0xFFFFFFFF_FFC2802A != 2 */ /* tempSS & 0xFFF8FFFF != currentSS */ ) {
    BX_ERROR(("%s: #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  BX_NEXT_INSTR(i);
}

#endif
