/////////////////////////////////////////////////////////////////////////
// $Id: stack64.cc,v 1.3 2002-09-18 05:36:48 kevinlawton Exp $
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
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif


  void
BX_CPU_C::POP_Eq(bxInstruction_c *i)
{
  Bit64u val64;

  pop_64(&val64);

  if (i->mod() == 0xc0) {
    BX_WRITE_64BIT_REG(i->rm(), val64);
    }
  else {
    // Note: there is one little weirdism here.  When 64bit addressing
    // is used, it is possible to use RSP in the modrm addressing.
    // If used, the value of RSP after the pop is used to calculate
    // the address.
    if (i->as64L() && (i->mod()!=0xc0) && (i->rm()==4) && (i->sibBase()==4)) {
      // call method on BX_CPU_C object
      BX_CPU_CALL_METHOD (i->ResolveModrm, (i));
      }
    write_virtual_qword(i->seg(), RMAddr(i), &val64);
    }
}

  void
BX_CPU_C::PUSH_RRX(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR gen_reg[(i->b1 & 7) + i->rex_b()].rrx);
  //push_64(BX_CPU_THIS_PTR gen_reg[i->nnn()].rrx);
}

  void
BX_CPU_C::POP_RRX(bxInstruction_c *i)
{
  Bit64u rrx;

  pop_64(&rrx);
  BX_CPU_THIS_PTR gen_reg[(i->b1 & 7) + i->rex_b()].rrx = rrx;
  //BX_CPU_THIS_PTR gen_reg[i->nnn()].rrx = rrx;
}


  void
BX_CPU_C::PUSH64_CS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
}
  void
BX_CPU_C::PUSH64_DS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
}
  void
BX_CPU_C::PUSH64_ES(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
}
  void
BX_CPU_C::PUSH64_FS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}
  void
BX_CPU_C::PUSH64_GS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}
  void
BX_CPU_C::PUSH64_SS(bxInstruction_c *i)
{
  push_64(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
}


  void
BX_CPU_C::POP64_DS(bxInstruction_c *i)
{
  Bit64u ds;
  pop_64(&ds);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], (Bit16u) ds);
}
  void
BX_CPU_C::POP64_ES(bxInstruction_c *i)
{
  Bit64u es;
  pop_64(&es);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], (Bit16u) es);
}
  void
BX_CPU_C::POP64_FS(bxInstruction_c *i)
{
  Bit64u fs;
  pop_64(&fs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], (Bit16u) fs);
}
  void
BX_CPU_C::POP64_GS(bxInstruction_c *i)
{
  Bit64u gs;
  pop_64(&gs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], (Bit16u) gs);
}
  void
BX_CPU_C::POP64_SS(bxInstruction_c *i)
{
  Bit64u ss;
  pop_64(&ss);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], (Bit16u) ss);

  // POP SS inhibits interrupts, debug exceptions and single-step
  // trap exceptions until the execution boundary following the
  // next instruction is reached.
  // Same code as MOV_SwEw()
  BX_CPU_THIS_PTR inhibit_mask |=
    BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
  BX_CPU_THIS_PTR async_event = 1;
}


  void
BX_CPU_C::PUSHAD64(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("PUSHAD: not supported on an 8086"));
#else
  Bit64u temp_RSP;
  Bit64u rsp;

  temp_RSP = RSP;
  if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_RSP, 64) ) {
    BX_PANIC(("PUSHAD(): stack doesn't have enough room!"));
    exception(BX_SS_EXCEPTION, 0, 0);
    return;
    }

  rsp = RSP;

  /* ??? optimize this by using virtual write, all checks passed */
  push_64(RAX);
  push_64(RCX);
  push_64(RDX);
  push_64(RBX);
  push_64(rsp);
  push_64(RBP);
  push_64(RSI);
  push_64(RDI);
#endif
}

  void
BX_CPU_C::POPAD64(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("POPAD not supported on an 8086"));
#else /* 286+ */
    Bit64u rdi, rsi, rbp, rtmp, rbx, rdx, rcx, rax;

    if ( !can_pop(64) ) {
      BX_PANIC(("pop_ad: not enough bytes on stack"));
      exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }

    /* ??? optimize this */
    pop_64(&rdi);
    pop_64(&rsi);
    pop_64(&rbp);
    pop_64(&rtmp); /* value for ESP discarded */
    pop_64(&rbx);
    pop_64(&rdx);
    pop_64(&rcx);
    pop_64(&rax);

    RDI = rdi;
    RSI = rsi;
    RBP = rbp;
    RBX = rbx;
    RDX = rdx;
    RCX = rcx;
    RAX = rax;
#endif
}

  void
BX_CPU_C::PUSH64_Id(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("PUSH_Id: not supported on 8086!"));
#else

    Bit64u imm64;

    imm64 = (Bit32s) i->Id();

    push_64(imm64);
#endif
}

  void
BX_CPU_C::PUSH_Eq(bxInstruction_c *i)
{
    Bit64u op1_64;

    /* op1_64 is a register or memory reference */
    if (i->mod() == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op1_64);
      }

    push_64(op1_64);
}


  void
BX_CPU_C::ENTER64_IwIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("ENTER_IwIb: not supported by 8086!"));
#else
  Bit64u frame_ptr64;
  Bit16u frame_ptr16;
  Bit8u level;
  static Bit8u first_time = 1;

  level = i->Ib2();

  invalidate_prefetch_q();

  level %= 32;
/* ??? */
  if (first_time && level>0) {
    BX_ERROR(("enter() with level > 0. The emulation of this instruction may not be complete.  This warning will be printed only once per bochs run."));
    first_time = 0;
  }
//if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b && i->os64L()==0) {
//  BX_INFO(("enter(): stacksize!=opsize: I'm unsure of the code for this"));
//  BX_PANIC(("         The Intel manuals are a mess on this one!"));
//  }

    {
    Bit64u bytes_to_push, temp_RSP;

    if (level == 0) {
      bytes_to_push = 8 + i->Iw();
      }
    else { /* level > 0 */
      bytes_to_push = 8 + (level-1)*8 + 8 + i->Iw();
      }
    temp_RSP = RSP;
    if ( !can_push(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache, temp_RSP, bytes_to_push) ) {
      BX_PANIC(("ENTER: not enough room on stack!"));
      exception(BX_SS_EXCEPTION, 0, 0);
      }
    }

  push_64(RBP);

  frame_ptr64 = RSP;

  if (level > 0) {
    /* do level-1 times */
    while (--level) {
      Bit64u temp64;

      RBP -= 4;
      read_virtual_qword(BX_SEG_REG_SS, RBP, &temp64);
      ESP -= 4;
      write_virtual_qword(BX_SEG_REG_SS, RSP, &temp64);
      } /* while (--level) */

    /* push(frame pointer) */
    RSP -= 4;
    write_virtual_qword(BX_SEG_REG_SS, RSP, &frame_ptr64);
    } /* if (level > 0) ... */

  RBP = frame_ptr64;

  RSP = RSP - i->Iw();
#endif
}

  void
BX_CPU_C::LEAVE64(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 2
  BX_PANIC(("LEAVE: not supported by 8086!"));
#else
  // delete frame
  RSP = RBP;

  // restore frame pointer
  {
  Bit64u temp64;

  pop_64(&temp64);
  RBP = temp64;
  }
#endif
}
