/////////////////////////////////////////////////////////////////////////
// $Id: soft_int.cc,v 1.47 2009-01-20 21:28:43 sshwarts Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RSP ESP
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GwMa(bxInstruction_c *i)
{
  Bit16s bound_min, bound_max;
  Bit16s op1_16 = BX_READ_16BIT_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  bound_min = (Bit16s) read_virtual_word(i->seg(), eaddr);
  bound_max = (Bit16s) read_virtual_word(i->seg(), eaddr+2);

  if (op1_16 < bound_min || op1_16 > bound_max) {
    BX_INFO(("BOUND_GdMa: fails bounds test"));
    exception(BX_BR_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GdMa(bxInstruction_c *i)
{
  Bit32s bound_min, bound_max;
  Bit32s op1_32 = BX_READ_32BIT_REG(i->nnn());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  bound_min = (Bit32s) read_virtual_dword(i->seg(), eaddr);
  bound_max = (Bit32s) read_virtual_dword(i->seg(), eaddr+4);

  if (op1_32 < bound_min || op1_32 > bound_max) {
    BX_INFO(("BOUND_GdMa: fails bounds test"));
    exception(BX_BR_EXCEPTION, 0, 0);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT1(bxInstruction_c *i)
{
  // This is an undocumented instrucion (opcode 0xf1)
  // which is useful for an ICE system.

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // interrupt is not RSP safe
  interrupt(1, BX_HARDWARE_EXCEPTION, 0, 0);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT3(bxInstruction_c *i)
{
  // INT 3 is not IOPL sensitive

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  // interrupt is not RSP safe
  interrupt(3, BX_SOFTWARE_EXCEPTION, 0, 0);

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}


void BX_CPP_AttrRegparmN(1) BX_CPU_C::INT_Ib(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  Bit8u vector = i->Ib();

  BX_CPU_THIS_PTR speculative_rsp = 1;
  BX_CPU_THIS_PTR prev_rsp = RSP;

  if (v8086_mode()) {
#if BX_SUPPORT_VME
    if (BX_CPU_THIS_PTR cr4.get_VME())
    {
      bx_address tr_base = BX_CPU_THIS_PTR tr.cache.u.system.base;

      Bit16u io_base = system_read_word(tr_base + 102);
      Bit8u vme_redirection_bitmap = system_read_byte(tr_base + io_base - 32 + (vector >> 3));

      if (! (vme_redirection_bitmap & (1 << (vector & 7))))
      {
        // redirect interrupt through virtual-mode idt
        v86_redirect_interrupt(vector);
        goto done;
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
  if ((vector == 0x21) && (AH == 0x4c)) {
    BX_INFO(("INT 21/4C called AL=0x%02x, BX=0x%04x", (unsigned) AL, (unsigned) BX));
  }
#endif

  interrupt(vector, BX_SOFTWARE_INTERRUPT, 0, 0);

done:

  BX_CPU_THIS_PTR speculative_rsp = 0;

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INTO(bxInstruction_c *i)
{
#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  if (get_OF()) {
    BX_CPU_THIS_PTR speculative_rsp = 1;
    BX_CPU_THIS_PTR prev_rsp = RSP;

    // interrupt is not RSP safe
    interrupt(4, BX_SOFTWARE_EXCEPTION, 0, 0);

    BX_CPU_THIS_PTR speculative_rsp = 0;

    BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                        EIP);
  }
}
