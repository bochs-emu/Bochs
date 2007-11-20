/////////////////////////////////////////////////////////////////////////
// $Id: stack64.cc,v 1.26 2007-11-20 17:15:33 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_X86_64

void BX_CPU_C::POP_EqM(bxInstruction_c *i)
{
  Bit64u val64;

  pop_64(&val64);

  // Note: there is one little weirdism here.  It is possible to use 
  // RSP in the modrm addressing. If used, the value of RSP after the 
  // pop is used to calculate the address.
  if (i->rm()==4 && i->sibBase()==4) {
    // call method on BX_CPU_C object
    BX_CPU_CALL_METHODR (i->ResolveModrm, (i));
  }
  write_virtual_qword(i->seg(), RMAddr(i), &val64);
}

void BX_CPU_C::POP_EqR(bxInstruction_c *i)
{
  Bit64u val64;
  pop_64(&val64);
  BX_WRITE_64BIT_REG(i->rm(), val64);
}

void BX_CPU_C::PUSH_RRX(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].rrx);
}

void BX_CPU_C::POP_RRX(bxInstruction_c *i)
{
  Bit64u rrx;
  pop_64(&rrx);
  BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].rrx = rrx;
}

void BX_CPU_C::PUSH64_FS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}

void BX_CPU_C::PUSH64_GS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}

void BX_CPU_C::POP64_FS(bxInstruction_c *i)
{
  Bit64u fs;
  pop_64(&fs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], (Bit16u) fs);
}

void BX_CPU_C::POP64_GS(bxInstruction_c *i)
{
  Bit64u gs;
  pop_64(&gs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], (Bit16u) gs);
}

void BX_CPU_C::PUSH64_Id(bxInstruction_c *i)
{
  Bit64u imm64 = (Bit32s) i->Id();
  push_64(imm64);
}

void BX_CPU_C::PUSH_EqM(bxInstruction_c *i)
{
  Bit64u op1_64;

  /* pointer, segment address pair */
  read_virtual_qword(i->seg(), RMAddr(i), &op1_64);

  push_64(op1_64);
}

void BX_CPU_C::PUSH_EqR(bxInstruction_c *i)
{
  push_64(BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::ENTER64_IwIb(bxInstruction_c *i)
{
  Bit8u level = i->Ib2();
  level &= 0x1F;
  Bit64u bytes_to_push = 8 + level*8 + i->Iw();

  if (! can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, RSP, bytes_to_push))
  {
    BX_ERROR(("ENTER: not enough room on stack!"));
    exception(BX_SS_EXCEPTION, 0, 0);
  }

  push_64(RBP);

  Bit64u frame_ptr64 = RSP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      Bit64u temp64;

      RBP -= 8;
      read_virtual_qword(BX_SEG_REG_SS, RBP, &temp64);
      RSP -= 8;
      write_virtual_qword(BX_SEG_REG_SS, RSP, &temp64);
    } /* while (--level) */

    /* push(frame pointer) */
    RSP -= 8;
    write_virtual_qword(BX_SEG_REG_SS, RSP, &frame_ptr64);
  } /* if (level > 0) ... */

  RBP = frame_ptr64;

  RSP -= i->Iw();
}

void BX_CPU_C::LEAVE64(bxInstruction_c *i)
{
  // delete frame
  RSP = RBP;

  // restore frame pointer
  Bit64u temp64;
  pop_64(&temp64);
  RBP = temp64;
}

#endif /* if BX_SUPPORT_X86_64 */
