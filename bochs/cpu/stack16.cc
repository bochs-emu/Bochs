/////////////////////////////////////////////////////////////////////////
// $Id: stack16.cc,v 1.37 2008-04-05 17:51:55 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RSP ESP
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_RX(bxInstruction_c *i)
{
  push_16(BX_READ_16BIT_REG(i->opcodeReg()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_CS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_DS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_ES(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_FS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_GS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_SS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_DS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u ds = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);

  BX_CPU_THIS_PTR speculative_rsp = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_ES(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u es = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);

  BX_CPU_THIS_PTR speculative_rsp = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_FS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u fs = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);

  BX_CPU_THIS_PTR speculative_rsp = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_GS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u gs = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);

  BX_CPU_THIS_PTR speculative_rsp = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_SS(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u ss = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  // POP SS inhibits interrupts, debug exceptions and single-step
  // trap exceptions until the execution boundary following the
  // next instruction is reached.
  // Same code as MOV_SwEw()
  BX_CPU_THIS_PTR inhibit_mask |=
    BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
  BX_CPU_THIS_PTR async_event = 1;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_RX(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->opcodeReg(), pop_16());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EwM(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  Bit16u val16 = pop_16();

  // Note: there is one little weirdism here.  It is possible to use
  // SP in the modrm addressing. If used, the value of SP after the
  // pop is used to calculate the address.
  BX_CPU_CALL_METHODR (i->ResolveModrm, (i));

  write_virtual_word(i->seg(), RMAddr(i), val16);

  BX_CPU_THIS_PTR speculative_rsp = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_Iw(bxInstruction_c *i)
{
  push_16(i->Iw());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EwM(bxInstruction_c *i)
{
  BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op1_16 = read_virtual_word(i->seg(), RMAddr(i));

  push_16(op1_16);
}

#if BX_CPU_LEVEL >= 3
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHAD16(bxInstruction_c *i)
{
  Bit32u temp_ESP = ESP;
  Bit16u temp_SP  = SP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  2), AX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  4), CX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  6), DX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  8), BX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 10), temp_SP);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 12), BP);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 14), SI);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 16), DI);
    ESP -= 16;
  }
  else
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  2), AX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  4), CX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  6), DX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  8), BX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 10), temp_SP);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 12), BP);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 14), SI);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 16), DI);
    SP -= 16;
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::POPAD16(bxInstruction_c *i)
{
  Bit16u di, si, bp, bx, dx, cx, ax;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    Bit32u temp_ESP = ESP;
    di = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  0));
    si = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  2));
    bp = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  4));
    bx = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  8));
    dx = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 10));
    cx = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 12));
    ax = read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 14));
    ESP += 16;
  }
  else
  {
    Bit16u temp_SP = SP;
    di = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  0));
    si = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  2));
    bp = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  4));
    bx = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  8));
    dx = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 10));
    cx = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 12));
    ax = read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 14));
    SP += 16;
  }

  DI = di;
  SI = si;
  BP = bp;
  BX = bx;
  DX = dx;
  CX = cx;
  AX = ax;
}
#endif

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::can_pop(Bit32u bytes)
{
  Bit32u temp_ESP, expand_down_limit;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    return(1);
  }
#endif

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) { /* Big bit set: use ESP */
    temp_ESP = ESP;
    expand_down_limit = 0xFFFFFFFF;
  }
  else { /* Big bit clear: use SP */
    temp_ESP = SP;
    expand_down_limit = 0xFFFF;
  }

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.valid==0) {
    BX_ERROR(("can_pop(): SS invalidated"));
    return(0); /* never gets here */
  }

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.p==0) {
    BX_ERROR(("can_pop(): SS.p = 0"));
    return(0);
  }

  if (IS_DATA_SEGMENT_EXPAND_DOWN(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.type)) { /* expand down */
    if (temp_ESP == expand_down_limit) {
      BX_PANIC(("can_pop(): found SP=ffff"));
      return(0);
    }
    if (((expand_down_limit - temp_ESP) + 1) >= bytes)
      return(1);
    return(0);
  }
  else { /* normal (expand-up) segment */
    if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled==0) {
      BX_ERROR(("can_pop(): SS.limit = 0"));
      return(0);
    }
    if (temp_ESP == expand_down_limit) {
      BX_ERROR(("can_pop(): found SP=ffff"));
      return(0);
    }
    if (temp_ESP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled) {
      BX_ERROR(("can_pop(): eSP > SS.limit"));
      return(0);
    }
    if (((BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.limit_scaled - temp_ESP) + 1) >= bytes)
      return(1);
    return(0);
  }
}
