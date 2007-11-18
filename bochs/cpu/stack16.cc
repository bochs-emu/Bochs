/////////////////////////////////////////////////////////////////////////
// $Id: stack16.cc,v 1.24 2007-11-18 18:49:19 sshwarts Exp $
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


void BX_CPU_C::PUSH_RX(bxInstruction_c *i)
{
  push_16(BX_READ_16BIT_REG(i->opcodeReg()));
}

void BX_CPU_C::PUSH16_CS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value);
}

void BX_CPU_C::PUSH16_DS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS].selector.value);
}

void BX_CPU_C::PUSH16_ES(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES].selector.value);
}

void BX_CPU_C::PUSH16_FS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS].selector.value);
}

void BX_CPU_C::PUSH16_GS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS].selector.value);
}

void BX_CPU_C::PUSH16_SS(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value);
}

void BX_CPU_C::POP16_DS(bxInstruction_c *i)
{
  Bit16u ds;
  pop_16(&ds);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_DS], ds);
}

void BX_CPU_C::POP16_ES(bxInstruction_c *i)
{
  Bit16u es;
  pop_16(&es);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_ES], es);
}

void BX_CPU_C::POP16_FS(bxInstruction_c *i)
{
  Bit16u fs;
  pop_16(&fs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], fs);
}

void BX_CPU_C::POP16_GS(bxInstruction_c *i)
{
  Bit16u gs;
  pop_16(&gs);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], gs);
}

void BX_CPU_C::POP16_SS(bxInstruction_c *i)
{
  Bit16u ss;
  pop_16(&ss);
  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], ss);

  // POP SS inhibits interrupts, debug exceptions and single-step
  // trap exceptions until the execution boundary following the
  // next instruction is reached.
  // Same code as MOV_SwEw()
  BX_CPU_THIS_PTR inhibit_mask |=
    BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
  BX_CPU_THIS_PTR async_event = 1;
}

void BX_CPU_C::POP_RX(bxInstruction_c *i)
{
  Bit16u rx;
  pop_16(&rx);
  BX_WRITE_16BIT_REG(i->opcodeReg(), rx)
}

void BX_CPU_C::POP_EwM(bxInstruction_c *i)
{
  Bit16u val16;

  pop_16(&val16);

  // Note: there is one little weirdism here.  When 32bit addressing
  // is used, it is possible to use ESP in the modrm addressing.
  // If used, the value of ESP after the pop is used to calculate
  // the address.
  if (i->as32L() && (i->rm()==4) && (i->sibBase()==4)) {
    BX_CPU_CALL_METHODR (i->ResolveModrm, (i));
  }
  write_virtual_word(i->seg(), RMAddr(i), &val16);
}

void BX_CPU_C::POP_EwR(bxInstruction_c *i)
{
  Bit16u val16;
  pop_16(&val16);
  BX_WRITE_16BIT_REG(i->rm(), val16);
}

#if BX_CPU_LEVEL >= 3
void BX_CPU_C::PUSHAD16(bxInstruction_c *i)
{
  Bit32u temp_ESP = ESP;
  Bit16u temp_SP  = SP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  2), &AX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  4), &CX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  6), &DX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP -  8), &BX);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 10), &temp_SP);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 12), &BP);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 14), &SI);
    write_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP - 16), &DI);
    ESP -= 16;
  }
  else
  {
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  2), &AX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  4), &CX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  6), &DX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP -  8), &BX);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 10), &temp_SP);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 12), &BP);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 14), &SI);
    write_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP - 16), &DI);
    SP -= 16;
  }
}

void BX_CPU_C::POPAD16(bxInstruction_c *i)
{
  Bit16u di, si, bp, bx, dx, cx, ax;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    Bit32u temp_ESP = ESP;
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  0), &di);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  2), &si);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  4), &bp);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP +  8), &bx);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 10), &dx);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 12), &cx);
    read_virtual_word(BX_SEG_REG_SS, (Bit32u) (temp_ESP + 14), &ax);
    ESP += 16;
  }
  else
  {
    Bit16u temp_SP = SP;
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  0), &di);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  2), &si);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  4), &bp);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP +  8), &bx);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 10), &dx);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 12), &cx);
    read_virtual_word(BX_SEG_REG_SS, (Bit16u) (temp_SP + 14), &ax);
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

void BX_CPU_C::PUSH_Iw(bxInstruction_c *i)
{
  push_16(i->Iw());
}

void BX_CPU_C::PUSH_EwM(bxInstruction_c *i)
{
  Bit16u op1_16;

  /* pointer, segment address pair */
  read_virtual_word(i->seg(), RMAddr(i), &op1_16);

  push_16(op1_16);
}

void BX_CPU_C::PUSH_EwR(bxInstruction_c *i)
{
  push_16(BX_READ_16BIT_REG(i->rm()));
}
