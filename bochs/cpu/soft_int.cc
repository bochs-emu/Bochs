/////////////////////////////////////////////////////////////////////////
// $Id: soft_int.cc,v 1.30 2006-06-09 22:29:07 sshwarts Exp $
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

void BX_CPU_C::BOUND_GwMa(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_INFO(("BOUND_GwMa: op2 must be memory reference"));
    UndefinedOpcode(i);
  }

  Bit16s bound_min, bound_max;
  Bit16s op1_16 = BX_READ_16BIT_REG(i->nnn());

  read_virtual_word(i->seg(), RMAddr(i),   (Bit16u *) &bound_min);
  read_virtual_word(i->seg(), RMAddr(i)+2, (Bit16u *) &bound_max);

  if (op1_16 < bound_min || op1_16 > bound_max) {
    BX_INFO(("BOUND_GdMa: fails bounds test"));
    exception(BX_BR_EXCEPTION, 0, 0);
  }
}

void BX_CPU_C::BOUND_GdMa(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_INFO(("BOUND_GdMa: op2 must be memory reference"));
    UndefinedOpcode(i);
  }

  Bit32s bound_min, bound_max;
  Bit32s op1_32 = BX_READ_32BIT_REG(i->nnn());

  read_virtual_dword(i->seg(), RMAddr(i),   (Bit32u *) &bound_min);
  read_virtual_dword(i->seg(), RMAddr(i)+4, (Bit32u *) &bound_max);

  if (op1_32 < bound_min || op1_32 > bound_max) {
    BX_INFO(("BOUND_GdMa: fails bounds test"));
    exception(BX_BR_EXCEPTION, 0, 0);
  }
}

void BX_CPU_C::INT1(bxInstruction_c *i)
{
  // This is an undocumented instrucion (opcode 0xf1)
  // which is useful for an ICE system.

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

#if BX_EXTERNAL_DEBUGGER
  trap_debugger(0);
#endif

  interrupt(1, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}

void BX_CPU_C::INT3(bxInstruction_c *i)
{
  // INT 3 is not IOPL sensitive

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  interrupt(3, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}


void BX_CPU_C::INT_Ib(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  Bit8u vector = i->Ib();

  if (v8086_mode()) {
#if BX_SUPPORT_VME
    if (BX_CPU_THIS_PTR cr4.get_VME())
    {
      Bit8u vme_redirection_bitmap;
      Bit16u io_base;

      access_linear(BX_CPU_THIS_PTR tr.cache.u.tss.base + 102, 
            2, 0, BX_READ, &io_base);
      access_linear(BX_CPU_THIS_PTR tr.cache.u.tss.base + io_base - 32 + (vector >> 3),
            1, 0, BX_READ, &vme_redirection_bitmap);

      if (! (vme_redirection_bitmap & (1 << (vector & 7))))
      {
        // redirect interrupt through virtual-mode idt
        v86_redirect_interrupt(vector);
        return;
      }
    }
#endif
    // interrupt is not redirected or VME is OFF
    if (BX_CPU_THIS_PTR get_IOPL() < 3)
    {
      BX_DEBUG(("INT_Ib(): Interrupt cannot be redirected, generate #GP(0)"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }

#ifdef SHOW_EXIT_STATUS
  if ( (vector == 0x21) && (AH == 0x4c) ) {
    BX_INFO(("INT 21/4C called AL=0x%02x, BX=0x%04x", (unsigned) AL, (unsigned) BX));
  }
#endif

  interrupt(vector, 1, 0, 0);
  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}

void BX_CPU_C::INTO(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  if (get_OF()) {
    interrupt(4, 1, 0, 0);
    BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                        EIP);
  }
}
