/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EqM(bxInstruction_c *i)
{
  RSP_SPECULATIVE;

  Bit64u val64 = pop_64();

  // Note: there is one little weirdism here.  It is possible to use
  // RSP in the modrm addressing. If used, the value of RSP after the
  // pop is used to calculate the address.
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), val64);

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EqR(bxInstruction_c *i)
{
  push_64(BX_READ_64BIT_REG(i->dst()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EqR(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), pop_64());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH64_Sw(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[i->src()].selector.value);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP64_Sw(bxInstruction_c *i)
{
  Bit16u selector = stack_read_word(RSP);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[i->dst()], selector);
  RSP += 8;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH64_Id(bxInstruction_c *i)
{
  Bit64u imm64 = (Bit32s) i->Id();
  push_64(imm64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  push_64(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ENTER64_IwIb(bxInstruction_c *i)
{
  Bit8u level = i->Ib2();
  level &= 0x1F;

  Bit64u temp_RSP = RSP, temp_RBP = RBP;

  temp_RSP -= 8;
  stack_write_qword(temp_RSP, temp_RBP);

  Bit64u frame_ptr64 = temp_RSP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      temp_RBP -= 8;
      Bit64u temp64 = stack_read_qword(temp_RBP);
      temp_RSP -= 8;
      stack_write_qword(temp_RSP, temp64);
    } /* while (--level) */

    /* push(frame pointer) */
    temp_RSP -= 8;
    stack_write_qword(temp_RSP, frame_ptr64);
  } /* if (level > 0) ... */

  temp_RSP -= i->Iw();

  // ENTER finishes with memory write check on the final stack pointer
  // the memory is touched but no write actually occurs
  // emulate it by doing RMW read access from SS:RSP
  read_RMW_virtual_qword_64(BX_SEG_REG_SS, temp_RSP);

  RBP = frame_ptr64;
  RSP = temp_RSP;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEAVE64(bxInstruction_c *i)
{
  // restore frame pointer
  Bit64u temp64 = stack_read_qword(RBP);
  RSP = RBP + 8;
  RBP = temp64;

  BX_NEXT_INSTR(i);
}

#endif /* if BX_SUPPORT_X86_64 */
