/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl.cc,v 1.29 2007-11-13 21:07:07 sshwarts Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::SAHF(bxInstruction_c *i)
{
  setB_SF((AH & 0x80) >> 7);
  setB_ZF((AH & 0x40) >> 6);
  setB_AF((AH & 0x10) >> 4);
  setB_CF (AH & 0x01);
  setB_PF((AH & 0x04) >> 2);
}

void BX_CPU_C::LAHF(bxInstruction_c *i)
{
  AH = read_flags() & 0xFF;
}

void BX_CPU_C::CLC(bxInstruction_c *i)
{
  clear_CF();
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
        BX_DEBUG(("CLI: IOPL < CPL in protected mode"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
  }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_SUPPORT_VME
      if (CR4_VME_ENABLED) {
        BX_CPU_THIS_PTR clear_VIF();
        return;
      }
#endif
      BX_DEBUG(("CLI: IOPL != 3 in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }
#endif
#endif

  BX_CPU_THIS_PTR clear_IF();
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
      if (cpl == 3 && IOPL < 3) {
        if (! BX_CPU_THIS_PTR get_VIP())
        {
          BX_CPU_THIS_PTR assert_VIF();
          return;
        }

        BX_DEBUG(("STI: #GP(0) in VME mode"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }
    }
#endif
    if (cpl > IOPL) {
      BX_DEBUG(("STI: CPL > IOPL in protected mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }
#if BX_CPU_LEVEL >= 3
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_SUPPORT_VME
      if (CR4_VME_ENABLED && BX_CPU_THIS_PTR get_VIP() == 0)
      {
        BX_CPU_THIS_PTR assert_VIF();
        return;
      }
#endif
      BX_DEBUG(("STI: IOPL != 3 in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }
#endif
#endif

  if (!BX_CPU_THIS_PTR get_IF()) {
    BX_CPU_THIS_PTR assert_IF();
    BX_CPU_THIS_PTR inhibit_mask |= BX_INHIBIT_INTERRUPTS;
    BX_CPU_THIS_PTR async_event = 1;
  }
}

void BX_CPU_C::CLD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR clear_DF();
}

void BX_CPU_C::STD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR assert_DF();
}

void BX_CPU_C::CMC(bxInstruction_c *i)
{
  set_CF(! get_CF());
}

void BX_CPU_C::PUSHF_Fw(bxInstruction_c *i)
{
  Bit16u flags = read_flags();

  if (v8086_mode()) {
    if ((BX_CPU_THIS_PTR get_IOPL() < 3) && (CR4_VME_ENABLED == 0)) {
      BX_DEBUG(("PUSHFW: #GP(0) in v8086 (no VME) mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
      return;
    }
#if BX_SUPPORT_VME
    if (CR4_VME_ENABLED && BX_CPU_THIS_PTR get_IOPL() < 3) {
      flags |= EFlagsIOPLMask;
      if (BX_CPU_THIS_PTR get_VIF())
        flags |=  EFlagsIFMask;
      else
        flags &= ~EFlagsIFMask;
    }
#endif
  }

  push_16(flags);
}

void BX_CPU_C::POPF_Fw(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask;
#if BX_CPU_LEVEL >= 3
  changeMask |= EFlagsNTMask;     // NT could be modified
#endif
  Bit16u flags16;

  if (protected_mode()) {
    pop_16(&flags16);
    if (CPL==0)
      changeMask |= EFlagsIOPLMask;
    if (CPL <= BX_CPU_THIS_PTR get_IOPL())
      changeMask |= EFlagsIFMask;
  }
  else if (v8086_mode()) {
    if ((BX_CPU_THIS_PTR get_IOPL() < 3) && (CR4_VME_ENABLED == 0)) {
      BX_DEBUG(("POPFW: #GP(0) in v8086 (no VME) mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    pop_16(&flags16);
#if BX_SUPPORT_VME
    if (CR4_VME_ENABLED && BX_CPU_THIS_PTR get_IOPL() < 3) {
      if (((flags16 & EFlagsIFMask) && BX_CPU_THIS_PTR get_VIP()) || 
           (flags16 & EFlagsTFMask))
      {
        BX_DEBUG(("POPFW: #GP(0) in VME mode"));
        exception(BX_GP_EXCEPTION, 0, 0);
      }

      // IF, IOPL unchanged, EFLAGS.VIF = TMP_FLAGS.IF
      changeMask |= EFlagsVIFMask;
      Bit32u flags32 = (Bit32u) flags16;
      if (BX_CPU_THIS_PTR get_IF()) flags32 |= EFlagsVIFMask;
      writeEFlags(flags32, changeMask);
      return;
    }
#endif
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

void BX_CPU_C::PUSHF_Fd(bxInstruction_c *i)
{
  if (v8086_mode() && (BX_CPU_THIS_PTR get_IOPL()<3)) {
    BX_DEBUG(("PUSHFD: #GP(0) in v8086 mode"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  // VM & RF flags cleared in image stored on the stack
  push_32(read_eflags() & 0x00fcffff);
}

void BX_CPU_C::POPF_Fd(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | 
                          EFlagsDFMask | EFlagsNTMask | EFlagsRFMask;
#if BX_CPU_LEVEL >= 4
  changeMask |= (EFlagsIDMask | EFlagsACMask);  // ID/AC
#endif
  Bit32u flags32;

  if (protected_mode()) {
    pop_32(&flags32);
    // IOPL changed only if (CPL == 0),
    // IF changed only if (CPL <= EFLAGS.IOPL),
    // VIF, VIP, VM are unaffected
    if (CPL==0)
      changeMask |= EFlagsIOPLMask;
    if (CPL <= BX_CPU_THIS_PTR get_IOPL())
      changeMask |= EFlagsIFMask;
  }
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
      BX_DEBUG(("POPFD: #GP(0) in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    pop_32(&flags32);
    // v8086-mode: VM, IOPL, VIP, VIF are unaffected
    changeMask |= EFlagsIFMask;
  }
  else { // Real-mode
    pop_32(&flags32);
    // VIF, VIP, VM are unaffected
    changeMask |= (EFlagsIOPLMask | EFlagsIFMask);
  }

  writeEFlags(flags32, changeMask);
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::PUSHF_Fq(bxInstruction_c *i)
{
  // VM & RF flags cleared in image stored on the stack
  push_64(read_eflags() & 0x00fcffff);
}

void BX_CPU_C::POPF_Fq(bxInstruction_c *i)
{
  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask
                        | EFlagsNTMask | EFlagsRFMask | EFlagsACMask
                        | EFlagsIDMask;
  Bit64u flags64;

  BX_ASSERT (protected_mode());

  pop_64(&flags64);
  Bit32u flags32 = (Bit32u) flags64;
  if (CPL==0)
    changeMask |= EFlagsIOPLMask;
  if (CPL <= BX_CPU_THIS_PTR get_IOPL())
    changeMask |= EFlagsIFMask;

  // VIF, VIP, VM are unaffected
  writeEFlags(flags32, changeMask);
}
#endif

#endif  // BX_CPU_LEVEL >= 3

void BX_CPU_C::SALC(bxInstruction_c *i)
{
  if (get_CF()) {
    AL = 0xff;
  }
  else {
    AL = 0x00;
  }
}
