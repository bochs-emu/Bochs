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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->dst(), pop_32());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP_EdM(bxInstruction_c *i)
{
  RSP_SPECULATIVE;

  Bit32u val32 = pop_32();

  // Note: there is one little weirdism here.  It is possible to use
  // ESP in the modrm addressing. If used, the value of ESP after the
  // pop is used to calculate the address.
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_dword_32(i->seg(), eaddr, val32);

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EdR(bxInstruction_c *i)
{
  push_32(BX_READ_32BIT_REG(i->dst()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_EdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_virtual_dword_32(i->seg(), eaddr);

  push_32(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH32_Sw(bxInstruction_c *i)
{
  Bit16u val_16 = BX_CPU_THIS_PTR sregs[i->src()].selector.value;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    stack_write_word((Bit32u) (ESP-4), val_16);
    ESP -= 4;
  }
  else
  {
    stack_write_word((Bit16u) (SP-4), val_16);
    SP -= 4;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POP32_Sw(bxInstruction_c *i)
{
  Bit16u selector;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    selector = stack_read_word(ESP);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[i->dst()], selector);
    ESP += 4;
  }
  else {
    selector = stack_read_word(SP);
    load_seg_reg(&BX_CPU_THIS_PTR sregs[i->dst()], selector);
    SP += 4;
  }

  if (i->dst() == BX_SEG_REG_SS) {
    // POP SS inhibits interrupts, debug exceptions and single-step
    // trap exceptions until the execution boundary following the
    // next instruction is reached.
    // Same code as MOV_SwEw()
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS_BY_MOVSS);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSH_Id(bxInstruction_c *i)
{
  push_32(i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHA32(bxInstruction_c *i)
{
  Bit32u temp_ESP = ESP;
  Bit16u temp_SP  = SP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    stack_write_dword((Bit32u) (temp_ESP -  4), EAX);
    stack_write_dword((Bit32u) (temp_ESP -  8), ECX);
    stack_write_dword((Bit32u) (temp_ESP - 12), EDX);
    stack_write_dword((Bit32u) (temp_ESP - 16), EBX);
    stack_write_dword((Bit32u) (temp_ESP - 20), temp_ESP);
    stack_write_dword((Bit32u) (temp_ESP - 24), EBP);
    stack_write_dword((Bit32u) (temp_ESP - 28), ESI);
    stack_write_dword((Bit32u) (temp_ESP - 32), EDI);
    ESP -= 32;
  }
  else
  {
    stack_write_dword((Bit16u) (temp_SP -  4), EAX);
    stack_write_dword((Bit16u) (temp_SP -  8), ECX);
    stack_write_dword((Bit16u) (temp_SP - 12), EDX);
    stack_write_dword((Bit16u) (temp_SP - 16), EBX);
    stack_write_dword((Bit16u) (temp_SP - 20), temp_ESP);
    stack_write_dword((Bit16u) (temp_SP - 24), EBP);
    stack_write_dword((Bit16u) (temp_SP - 28), ESI);
    stack_write_dword((Bit16u) (temp_SP - 32), EDI);
    SP -= 32;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPA32(bxInstruction_c *i)
{
  Bit32u edi, esi, ebp, ebx, edx, ecx, eax;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b)
  {
    Bit32u temp_ESP = ESP;
    edi = stack_read_dword((Bit32u) (temp_ESP +  0));
    esi = stack_read_dword((Bit32u) (temp_ESP +  4));
    ebp = stack_read_dword((Bit32u) (temp_ESP +  8));
          stack_read_dword((Bit32u) (temp_ESP + 12));
    ebx = stack_read_dword((Bit32u) (temp_ESP + 16));
    edx = stack_read_dword((Bit32u) (temp_ESP + 20));
    ecx = stack_read_dword((Bit32u) (temp_ESP + 24));
    eax = stack_read_dword((Bit32u) (temp_ESP + 28));
    ESP += 32;
  }
  else
  {
    Bit16u temp_SP = SP;
    edi = stack_read_dword((Bit16u) (temp_SP +  0));
    esi = stack_read_dword((Bit16u) (temp_SP +  4));
    ebp = stack_read_dword((Bit16u) (temp_SP +  8));
          stack_read_dword((Bit16u) (temp_SP + 12));
    ebx = stack_read_dword((Bit16u) (temp_SP + 16));
    edx = stack_read_dword((Bit16u) (temp_SP + 20));
    ecx = stack_read_dword((Bit16u) (temp_SP + 24));
    eax = stack_read_dword((Bit16u) (temp_SP + 28));
    SP += 32;
  }

  EDI = edi;
  ESI = esi;
  EBP = ebp;
  EBX = ebx;
  EDX = edx;
  ECX = ecx;
  EAX = eax;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::ENTER32_IwIb(bxInstruction_c *i)
{
  Bit16u imm16 = i->Iw();
  Bit8u level = i->Ib2();
  level &= 0x1F;

  RSP_SPECULATIVE;

  push_32(EBP);
  Bit32u frame_ptr32 = ESP;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    Bit32u ebp = EBP;  // Use temp copy for case of exception.

    if (level > 0) {
      /* do level-1 times */
      while (--level) {
        ebp -= 4;
        Bit32u temp32 = stack_read_dword(ebp);
        push_32(temp32);
      }

      /* push(frame pointer) */
      push_32(frame_ptr32);
    }

    ESP -= imm16;

    // ENTER finishes with memory write check on the final stack pointer
    // the memory is touched but no write actually occurs
    // emulate it by doing RMW read access from SS:ESP
    read_RMW_virtual_dword_32(BX_SEG_REG_SS, ESP);
  }
  else {
    Bit16u bp = BP;

    if (level > 0) {
      /* do level-1 times */
      while (--level) {
        bp -= 4;
        Bit32u temp32 = stack_read_dword(bp);
        push_32(temp32);
      }

      /* push(frame pointer) */
      push_32(frame_ptr32);
    }

    SP -= imm16;

    // ENTER finishes with memory write check on the final stack pointer
    // the memory is touched but no write actually occurs
    // emulate it by doing RMW read access from SS:SP
    read_RMW_virtual_dword_32(BX_SEG_REG_SS, SP);
  }

  EBP = frame_ptr32;

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEAVE32(bxInstruction_c *i)
{
  BX_ASSERT(BX_CPU_THIS_PTR cpu_mode != BX_MODE_LONG_64);

  Bit32u value32;

  if (BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].cache.u.segment.d_b) {
    value32 = stack_read_dword(EBP);
    ESP = EBP + 4;
  }
  else {
    value32 = stack_read_dword(BP);
    SP = BP + 4;
  }

  EBP = value32;

  BX_NEXT_INSTR(i);
}
