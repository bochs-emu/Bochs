/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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

#if BX_SUPPORT_AMX

#include "amx.h"
#include "cpu/decoder/ia_opcodes.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSRINIT(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_CPU_THIS_PTR amx->bsr_clear();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSRMOVF_BsrVdqWdq(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_CPU_THIS_PTR amx->scaledata.scale[0] = BX_READ_AVX_REG(i->src2());
  BX_CPU_THIS_PTR amx->scaledata.scale[1] = BX_READ_AVX_REG(i->src1());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSRMOV_BsrWdq(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_CPU_THIS_PTR amx->scaledata.scale[i->getIaOpcode() == BX_IA_EVEX_BSRMOVH_BsrWdq] = BX_READ_AVX_REG(i->src1());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSRMOV_WdqBsrR(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_WRITE_AVX_REG(i->dst(), BX_CPU_THIS_PTR amx->scaledata.scale[i->getIaOpcode() == BX_IA_EVEX_BSRMOVH_BsrWdq]);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSRMOV_WdqBsrM(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit64u eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  write_virtual_zmmword(i->seg(), get_laddr64(i->seg(), eaddr), &BX_CPU_THIS_PTR amx->scaledata.scale[i->getIaOpcode() == BX_IA_EVEX_BSRMOVH_BsrWdq]);

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_AMX
