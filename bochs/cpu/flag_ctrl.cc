/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl.cc,v 1.22 2005-09-29 17:32:32 sshwarts Exp $
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


void BX_CPU_C::SAHF(bxInstruction_c *i)
{
  set_SF((AH & 0x80) >> 7);
  set_ZF((AH & 0x40) >> 6);
  set_AF((AH & 0x10) >> 4);
  set_CF(AH & 0x01);
  set_PF((AH & 0x04) >> 2);
}

void BX_CPU_C::LAHF(bxInstruction_c *i)
{
  AH = (get_SF() ? 0x80 : 0) |
       (get_ZF() ? 0x40 : 0) |
       (get_AF() ? 0x10 : 0) |
       (get_PF() ? 0x04 : 0) |
       (0x02) |
       (get_CF() ? 0x01 : 0);
}

void BX_CPU_C::CLC(bxInstruction_c *i)
{
  clear_CF ();
}

void BX_CPU_C::STC(bxInstruction_c *i)
{
  assert_CF();
}

void BX_CPU_C::CLI(bxInstruction_c *i)
{
  Bit32u IOPL = BX_CPU_THIS_PTR get_IOPL();
  Bit32u  cpl = CPL;

#if BX_CPU_LEVEL >= 2
  if (protected_mode())
  {
#if BX_SUPPORT_VME
    if (BX_CPU_THIS_PTR cr4.get_PVI() && (cpl == 3))
    {
      if (IOPL < 3) {
        BX_CPU_THIS_PTR clear_VIF();
        return;
      }
    }
    else 
#endif
    {
      if (IOPL < cpl) {
        exception(BX_GP_EXCEPTION, 0, 0);
        return;
      }
    }
  }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_SUPPORT_VME
      if (BX_CPU_THIS_PTR cr4.get_VME())
      {
        BX_CPU_THIS_PTR clear_VIF();
        return;
      }
#endif
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
    }
  }
#endif
#endif

  BX_CPU_THIS_PTR clear_IF ();
}

void BX_CPU_C::STI(bxInstruction_c *i)
{
  Bit32u IOPL = BX_CPU_THIS_PTR get_IOPL();
  Bit32u  cpl = CPL;

#if BX_CPU_LEVEL >= 2
  if (protected_mode())
  {
#if BX_SUPPORT_VME
    if (BX_CPU_THIS_PTR cr4.get_PVI())
    {
      if (cpl == 3) {
        if (IOPL < 3) 
        {
          if (! BX_CPU_THIS_PTR get_VIP ())
          {
            BX_CPU_THIS_PTR assert_VIF();
            return;
          }

          exception(BX_GP_EXCEPTION, 0, 0);
        }
      }
    }
#endif
    if (cpl > IOPL)
      exception(BX_GP_EXCEPTION, 0, 0);
  }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_SUPPORT_VME
      if (! BX_CPU_THIS_PTR get_VIP () && BX_CPU_THIS_PTR cr4.get_VME())
      {
        BX_CPU_THIS_PTR assert_VIF();
        return;
      }
#endif
      exception(BX_GP_EXCEPTION, 0, 0);
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

void BX_CPU_C::CLD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR clear_DF ();
}

void BX_CPU_C::STD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR assert_DF ();
}

void BX_CPU_C::CMC(bxInstruction_c *i)
{
  set_CF( !get_CF() );
}

void BX_CPU_C::PUSHF_Fw(bxInstruction_c *i)
{
  if (v8086_mode() && (BX_CPU_THIS_PTR get_IOPL ()<3)) {
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
  }

  push_16(read_flags());
}

#if BX_CPU_LEVEL >= 3

void BX_CPU_C::PUSHF_Fd(bxInstruction_c *i)
{
  if (v8086_mode() && (BX_CPU_THIS_PTR get_IOPL ()<3)) {
    exception(BX_GP_EXCEPTION, 0, 0);
    return;
  }

  // VM & RF flags cleared in image stored on the stack
  push_32(read_eflags() & 0x00fcffff);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::PUSHF_Fq(bxInstruction_c *i)
{
  // VM & RF flags cleared in image stored on the stack
  push_64(read_eflags() & 0x00fcffff);
}
#endif

#endif  // BX_CPU_LEVEL >= 3

void BX_CPU_C::POPF_Fw(bxInstruction_c *i)
{
  Bit32u changeMask = 0x004dd5;
  Bit16u flags16;

  if (protected_mode()) {
    pop_16(&flags16);
    if (CPL==0)
      changeMask |= EFlagsIOPLMask;
    if (CPL <= BX_CPU_THIS_PTR get_IOPL())
      changeMask |= EFlagsIFMask;
  }
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
    }
    pop_16(&flags16);
    // All non-reserved flags except IOPL can be modified
    changeMask |= EFlagsIFMask;
  }
  else {
    pop_16(&flags16);
    // All non-reserved flags can be modified
    changeMask |= (EFlagsIOPLMask | EFlagsIFMask);
  }

  writeEFlags((Bit32u) flags16, changeMask);
}

#if BX_CPU_LEVEL >= 3

void BX_CPU_C::POPF_Fd(bxInstruction_c *i)
{
  Bit32u changeMask = 0x244dd5;	// AC, ID
  Bit32u flags32;

  if (protected_mode()) {
    pop_32(&flags32);
    if (CPL==0)
      changeMask |= EFlagsIOPLMask;
    if (CPL <= BX_CPU_THIS_PTR get_IOPL())
      changeMask |= EFlagsIFMask;

    // All non-reserved flags except VIP/VIF and VM can be modified
    // VIP/VIF are cleared, VM is unaffected
    changeMask |= (EFlagsRFMask | EFlagsVIPMask | EFlagsVIFMask);

    flags32 &= ~(EFlagsVIFMask | EFlagsVIPMask); // Clear VIP/VIF
  }
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
    }
    pop_32(&flags32);
    // v8086-mode: VM,RF,IOPL,VIP,VIF are unaffected.
    changeMask |= EFlagsIFMask;
  }
  else { // Real-mode
    pop_32(&flags32);

    // All non-reserved flags except VIP/VIF and VM can be modified
    // VIP/VIF are cleared, VM is unaffected
    changeMask |= (EFlagsIOPLMask | EFlagsIFMask | EFlagsRFMask |
                    EFlagsVIPMask | EFlagsVIFMask);

    flags32 &= ~(EFlagsVIPMask | EFlagsVIFMask); // Clear VIP/VIF
  }

  writeEFlags(flags32, changeMask);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::POPF_Fq(bxInstruction_c *i)
{
  Bit32u changeMask = 0x3d4dd5; // AC, ID, RF, VIP/VIF
  Bit64u flags64;

  BX_ASSERT (protected_mode());

  pop_64(&flags64);
  Bit32u flags32 = flags64 & 0xffffffff;
  if (CPL==0)
    changeMask |= EFlagsIOPLMask;
  if (CPL <= BX_CPU_THIS_PTR get_IOPL())
    changeMask |= EFlagsIFMask;

  // All non-reserved flags except VIP/VIF and VM can be modified
  // VIP/VIF are cleared, VM is unaffected
  flags32 &= ~(EFlagsVIPMask | EFlagsVIFMask); // Clear VIP/VIF

  writeEFlags(flags32, changeMask);
}
#endif

#endif  // BX_CPU_LEVEL >= 3

void BX_CPU_C::SALC(bxInstruction_c *i)
{
  if ( get_CF() ) {
    AL = 0xff;
  }
  else {
    AL = 0x00;
  }
}
