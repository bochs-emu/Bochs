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

// PUSH2 v64, b64 (also PUSH2P - the PPX hint doesn't change semantics)
//   [RSP-8] = v64, [RSP-16] = b64, both or none of the two qwords are written
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH2_BqEqR(bxInstruction_c *i)
{
  if (i->dst() == BX_64BIT_REG_RSP || i->src() == BX_64BIT_REG_RSP) {
    BX_ERROR(("%s: RSP is not allowed as operand", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // #GP(0) if RSP is not 16-byte aligned
  BxPackedXmmRegister data;
  data.xmm64u(0) = BX_READ_64BIT_REG(i->src());
  data.xmm64u(1) = BX_READ_64BIT_REG(i->dst());

  write_linear_xmmword_aligned(BX_SEG_REG_SS, get_laddr64(BX_SEG_REG_SS, RSP - 16), &data);

  RSP -= 16;

  BX_NEXT_INSTR(i);
}

// POP2 v64, b64 (also POP2P - the PPX hint doesn't change semantics)
//   v64 = [RSP], b64 = [RSP+8]
void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP2_BqEqR(bxInstruction_c *i)
{
  if (i->dst() == BX_64BIT_REG_RSP || i->src() == BX_64BIT_REG_RSP || i->dst() == i->src()) {
    BX_ERROR(("%s: RSP or same register is not allowed as operand", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  // #GP(0) if RSP is not 16-byte aligned
  BxPackedXmmRegister data;
  read_linear_xmmword_aligned(BX_SEG_REG_SS, get_laddr64(BX_SEG_REG_SS, RSP), &data);

  RSP += 16;

  BX_WRITE_64BIT_REG(i->dst(), data.xmm64u(0));
  BX_WRITE_64BIT_REG(i->src(), data.xmm64u(1));

  BX_NEXT_INSTR(i);
}

#endif
