/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl.cc,v 1.14 2002-10-24 21:05:42 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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





  void
BX_CPU_C::SAHF(bxInstruction_c *i)
{
  set_SF((AH & 0x80) >> 7);
  set_ZF((AH & 0x40) >> 6);
  set_AF((AH & 0x10) >> 4);
  set_CF(AH & 0x01);
  set_PF((AH & 0x04) >> 2);
}

  void
BX_CPU_C::LAHF(bxInstruction_c *i)
{
  AH = (get_SF() ? 0x80 : 0) |
       (get_ZF() ? 0x40 : 0) |
       (get_AF() ? 0x10 : 0) |
       (get_PF() ? 0x04 : 0) |
       (0x02) |
       (get_CF() ? 0x01 : 0);
}

  void
BX_CPU_C::CLC(bxInstruction_c *i)
{
  set_CF(0);
}

  void
BX_CPU_C::STC(bxInstruction_c *i)
{
  set_CF(1);
}

  void
BX_CPU_C::CLI(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    if (CPL > BX_CPU_THIS_PTR get_IOPL ()) {
      //BX_INFO(("CLI: CPL > IOPL")); /* ??? */
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL () != 3) {
      //BX_INFO(("CLI: IOPL != 3")); /* ??? */
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif
#endif

  BX_CPU_THIS_PTR clear_IF ();
}

  void
BX_CPU_C::STI(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 2
  if (protected_mode()) {
    if (CPL > BX_CPU_THIS_PTR get_IOPL ()) {
      //BX_INFO(("STI: CPL > IOPL")); /* ??? */
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL () != 3) {
      //BX_INFO(("STI: IOPL != 3")); /* ??? */
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }
#endif
#endif

  if (!BX_CPU_THIS_PTR get_IF ()) {
    BX_CPU_THIS_PTR assert_IF ();
    BX_CPU_THIS_PTR inhibit_mask |= BX_INHIBIT_INTERRUPTS;
    BX_CPU_THIS_PTR async_event = 1;
    }
}

  void
BX_CPU_C::CLD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR clear_DF ();
}

  void
BX_CPU_C::STD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR assert_DF ();
}

  void
BX_CPU_C::CMC(bxInstruction_c *i)
{
  set_CF( !get_CF() );
}

  void
BX_CPU_C::PUSHF_Fv(bxInstruction_c *i)
{
  if (v8086_mode() && (BX_CPU_THIS_PTR get_IOPL ()<3)) {
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

#if BX_CPU_LEVEL >= 3
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    push_64(read_eflags() & 0x00fcffff);
    }
  else
#endif
  if (i->os32L()) {
    push_32(read_eflags() & 0x00fcffff);
    }
  else
#endif
    {
    push_16(read_flags());
    }
}


  void
BX_CPU_C::POPF_Fv(bxInstruction_c *i)
{
  Bit32u changeMask = 0x004dd5;
  Bit32u flags32;

#if BX_CPU_LEVEL >= 3
  if (protected_mode()) {
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
      Bit64u flags64;

      pop_64(&flags64);
      flags32 = flags64;
      changeMask |= 0x240000; // ID,AC
      if (CPL==0)
        changeMask |= (3<<12); // IOPL
      if (CPL <= BX_CPU_THIS_PTR get_IOPL())
        changeMask |= (1<<9); // IF
      }
    else
#endif  // #if BX_SUPPORT_X86_64
    if (i->os32L()) {

      pop_32(&flags32);
      changeMask |= 0x240000; // ID,AC
      if (CPL==0)
        changeMask |= (3<<12); // IOPL
      if (CPL <= BX_CPU_THIS_PTR get_IOPL())
        changeMask |= (1<<9); // IF
      }
    else
#endif  // BX_CPU_LEVEL >= 3
      {
      Bit16u flags16;

      pop_16(&flags16);
      flags32 = flags16;
      if (CPL==0)
        changeMask |= (3<<12); // IOPL
      if (CPL <= BX_CPU_THIS_PTR get_IOPL())
        changeMask |= (1<<9); // IF
      }

    // Protected-mode: VIP/VIF cleared, VM unaffected.
    // Does this happen for 16 bit case?  fixme!
    flags32 &= ~( (1<<20) | (1<<19) ); // Clear VIP/VIF
    }

  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    if (i->os32L()) {
      pop_32(&flags32);
      changeMask |= 0x240000; // ID,AC
      }
    else {
      Bit16u flags16;
      pop_16(&flags16);
      flags32 = flags16;
      }
    // v8086-mode: VM,RF,IOPL,VIP,VIF are unaffected.
    changeMask |= (1<<9); // IF
    }

  else { // Real-mode
    if (i->os32L()) {
      pop_32(&flags32);
      changeMask |= 0x243200; // ID,AC,IOPL,IF
      }
    else { /* 16 bit opsize */
      Bit16u flags16;

      pop_16(&flags16);
      flags32 = flags16;
      changeMask |= 0x3200; // IOPL,IF
      }
    // Real-mode: VIP/VIF cleared, VM unaffected.
    flags32 &= ~( (1<<20) | (1<<19) ); // Clear VIP/VIF
    }

  writeEFlags(flags32, changeMask);
}


  void
BX_CPU_C::SALC(bxInstruction_c *i)
{
  if ( get_CF() ) {
    AL = 0xff;
    }
  else {
    AL = 0x00;
    }
}
