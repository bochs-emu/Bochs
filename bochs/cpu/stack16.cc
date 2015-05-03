/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EwR(bxInstruction_c *i)
{
  push_16(BX_READ_16BIT_REG(i->dst()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit16u op1_16 = read_virtual_word(i->seg(), eaddr);

  push_16(op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH16_Sw(bxInstruction_c *i)
{
  push_16(BX_CPU_THIS_PTR sregs[i->src()].selector.value);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP16_Sw(bxInstruction_c *i)
{
  RSP_SPECULATIVE;

  Bit16u selector = pop_16();
  load_seg_reg(&BX_CPU_THIS_PTR sregs[i->dst()], selector);

  RSP_COMMIT;

  if (i->dst() == BX_SEG_REG_SS) {
    // POP SS inhibits interrupts, debug exceptions and single-step
    // trap exceptions until the execution boundary following the
    // next instruction is reached.
    // Same code as MOV_SwEw()
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS_BY_MOVSS);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->dst(), pop_16());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EwM(bxInstruction_c *i)
{
  RSP_SPECULATIVE;

  Bit16u val16 = pop_16();

  // Note: there is one little weirdism here.  It is possible to use
  // SP in the modrm addressing. If used, the value of SP after the
  // pop is used to calculate the address.
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_word(i->seg(), eaddr, val16);

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_Iw(bxInstruction_c *i)
{
  push_16(i->Iw());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHA16(bxInstruction_c *i)
{
  Bit32u temp_ESP = ESP;
  Bit16u temp_SP  = SP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    stack_write_word((Bit32u)(temp_ESP -  2), AX);
    stack_write_word((Bit32u)(temp_ESP -  4), CX);
    stack_write_word((Bit32u)(temp_ESP -  6), DX);
    stack_write_word((Bit32u)(temp_ESP -  8), BX);
    stack_write_word((Bit32u)(temp_ESP - 10), temp_SP);
    stack_write_word((Bit32u)(temp_ESP - 12), BP);
    stack_write_word((Bit32u)(temp_ESP - 14), SI);
    stack_write_word((Bit32u)(temp_ESP - 16), DI);
    ESP -= 16;
  }
  else
  {
    stack_write_word((Bit16u)(temp_SP -  2), AX);
    stack_write_word((Bit16u)(temp_SP -  4), CX);
    stack_write_word((Bit16u)(temp_SP -  6), DX);
    stack_write_word((Bit16u)(temp_SP -  8), BX);
    stack_write_word((Bit16u)(temp_SP - 10), temp_SP);
    stack_write_word((Bit16u)(temp_SP - 12), BP);
    stack_write_word((Bit16u)(temp_SP - 14), SI);
    stack_write_word((Bit16u)(temp_SP - 16), DI);
    SP -= 16;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPA16(bxInstruction_c *i)
{
  Bit16u di, si, bp, bx, dx, cx, ax;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    Bit32u temp_ESP = ESP;
    di = stack_read_word((Bit32u)(temp_ESP +  0));
    si = stack_read_word((Bit32u)(temp_ESP +  2));
    bp = stack_read_word((Bit32u)(temp_ESP +  4));
         stack_read_word((Bit32u)(temp_ESP +  6));
    bx = stack_read_word((Bit32u)(temp_ESP +  8));
    dx = stack_read_word((Bit32u)(temp_ESP + 10));
    cx = stack_read_word((Bit32u)(temp_ESP + 12));
    ax = stack_read_word((Bit32u)(temp_ESP + 14));
    ESP += 16;
  }
  else
  {
    Bit16u temp_SP = SP;
    di = stack_read_word((Bit16u)(temp_SP +  0));
    si = stack_read_word((Bit16u)(temp_SP +  2));
    bp = stack_read_word((Bit16u)(temp_SP +  4));
         stack_read_word((Bit16u)(temp_SP +  6));
    bx = stack_read_word((Bit16u)(temp_SP +  8));
    dx = stack_read_word((Bit16u)(temp_SP + 10));
    cx = stack_read_word((Bit16u)(temp_SP + 12));
    ax = stack_read_word((Bit16u)(temp_SP + 14));
    SP += 16;
  }

  DI = di;
  SI = si;
  BP = bp;
  BX = bx;
  DX = dx;
  CX = cx;
  AX = ax;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ENTER16_IwIb(bxInstruction_c *i)
{
  Bit16u imm16 = i->Iw();
  Bit8u level = i->Ib2();
  level &= 0x1F;

  RSP_SPECULATIVE;

  push_16(BP);
  Bit16u frame_ptr16 = SP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    Bit32u ebp = EBP; // Use temp copy for case of exception.

    if (level > 0) {
      /* do level-1 times */
      while (--level) {
        ebp -= 2;
        Bit16u temp16 = stack_read_word(ebp);
        push_16(temp16);
      }

      /* push(frame pointer) */
      push_16(frame_ptr16);
    }

    ESP -= imm16;

    // ENTER finishes with memory write check on the final stack pointer
    // the memory is touched but no write actually occurs
    // emulate it by doing RMW read access from SS:ESP
    read_RMW_virtual_word_32(BX_SEG_REG_SS, ESP);

    BP = frame_ptr16;
  }
  else {
    Bit16u bp = BP;

    if (level > 0) {
      /* do level-1 times */
      while (--level) {
        bp -= 2;
        Bit16u temp16 = stack_read_word(bp);
        push_16(temp16);
      }

      /* push(frame pointer) */
      push_16(frame_ptr16);
    }

    SP -= imm16;

    // ENTER finishes with memory write check on the final stack pointer
    // the memory is touched but no write actually occurs
    // emulate it by doing RMW read access from SS:SP
    read_RMW_virtual_word_32(BX_SEG_REG_SS, SP);
  }

  BP = frame_ptr16;

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEAVE16(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit16u value16;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    value16 = stack_read_word(EBP);
    ESP = EBP + 2;
  }
  else {
    value16 = stack_read_word(BP);
    SP = BP + 2;
  }

  BP = value16;

  BX_NEXT_INSTR(i);
}
