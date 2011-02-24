/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EqM(bxInstruction_c *i)
{
  RSP_SPECULATIVE;

  Bit64u val64 = pop_64();

  // Note: there is one little weirdism here.  It is possible to use
  // RSP in the modrm addressing. If used, the value of RSP after the
  // pop is used to calculate the address.
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_qword_64(i->seg(), eaddr, val64);

  RSP_COMMIT;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_RRX(bxInstruction_c *i)
{
  push_64(BX_READ_64BIT_REG(i->rm()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_RRX(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->rm(), pop_64());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH64_FS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH64_GS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP64_FS(bxInstruction_c *i)
{
  Bit64u fs = read_virtual_word_64(BX_SEG_REG_SS, RSP);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], (Bit16u) fs);
  RSP += 8;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP64_GS(bxInstruction_c *i)
{
  Bit64u gs = read_virtual_word_64(BX_SEG_REG_SS, RSP);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], (Bit16u) gs);
  RSP += 8;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH64_Id(bxInstruction_c *i)
{
  Bit64u imm64 = (Bit32s) i->Id();
  push_64(imm64);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_virtual_qword_64(i->seg(), eaddr);

  push_64(op1_64);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ENTER64_IwIb(bxInstruction_c *i)
{
  Bit8u level = i->Ib2();
  level &= 0x1F;

  Bit64u temp_RSP = RSP, temp_RBP = RBP;

  temp_RSP -= 8;
  write_virtual_qword_64(BX_SEG_REG_SS, temp_RSP, temp_RBP);

  Bit64u frame_ptr64 = temp_RSP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      temp_RBP -= 8;
      Bit64u temp64 = read_virtual_qword_64(BX_SEG_REG_SS, temp_RBP);
      temp_RSP -= 8;
      write_virtual_qword_64(BX_SEG_REG_SS, temp_RSP, temp64);
    } /* while (--level) */

    /* push(frame pointer) */
    temp_RSP -= 8;
    write_virtual_qword_64(BX_SEG_REG_SS, temp_RSP, frame_ptr64);
  } /* if (level > 0) ... */

  temp_RSP -= i->Iw();

  // ENTER finishes with memory write check on the final stack pointer
  // the memory is touched but no write actually occurs
  // emulate it by doing RMW read access from SS:RSP
  read_RMW_virtual_qword_64(BX_SEG_REG_SS, temp_RSP);

  RBP = frame_ptr64;
  RSP = temp_RSP;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LEAVE64(bxInstruction_c *i)
{
  // restore frame pointer
  Bit64u temp64 = read_virtual_qword_64(BX_SEG_REG_SS, RBP);
  RSP = RBP + 8;
  RBP = temp64;
}

#endif /* if BX_SUPPORT_X86_64 */
