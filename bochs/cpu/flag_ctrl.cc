/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SAHF(bxInstruction_c *i)
{
  set_SF((AH & 0x80) >> 7);
  set_ZF((AH & 0x40) >> 6);
  set_AF((AH & 0x10) >> 4);
  set_CF (AH & 0x01);
  set_PF((AH & 0x04) >> 2);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LAHF(bxInstruction_c *i)
{
  AH = read_eflags() & 0xFF;

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CLC(bxInstruction_c *i)
{
  clear_CF();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STC(bxInstruction_c *i)
{
  assert_CF();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CLI(bxInstruction_c *i)
{
  Bit32u IOPL = BX_CPU_THIS_PTR get_IOPL();

  if (protected_mode())
  {
#if BX_CPU_LEVEL >= 5
    if (BX_CPU_THIS_PTR cr4.get_PVI() && (CPL == 3))
    {
      if (IOPL < 3) {
        BX_CPU_THIS_PTR clear_VIF();
        BX_NEXT_INSTR(i);
      }
    }
    else
#endif
    {
      if (IOPL < CPL) {
        BX_DEBUG(("CLI: IOPL < CPL in protected mode"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
  }
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_CPU_LEVEL >= 5
      if (BX_CPU_THIS_PTR cr4.get_VME()) {
        BX_CPU_THIS_PTR clear_VIF();
        BX_NEXT_INSTR(i);
      }
#endif
      BX_DEBUG(("CLI: IOPL != 3 in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

  BX_CPU_THIS_PTR clear_IF();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STI(bxInstruction_c *i)
{
  Bit32u IOPL = BX_CPU_THIS_PTR get_IOPL();

  if (protected_mode())
  {
#if BX_CPU_LEVEL >= 5
    if (BX_CPU_THIS_PTR cr4.get_PVI())
    {
      if (CPL == 3 && IOPL < 3) {
        if (! BX_CPU_THIS_PTR get_VIP())
        {
          BX_CPU_THIS_PTR assert_VIF();
          BX_NEXT_INSTR(i);
        }

        BX_DEBUG(("STI: #GP(0) in VME mode"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
#endif
    if (CPL > IOPL) {
      BX_DEBUG(("STI: CPL > IOPL in protected mode"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
  else if (v8086_mode())
  {
    if (IOPL != 3) {
#if BX_CPU_LEVEL >= 5
      if (BX_CPU_THIS_PTR cr4.get_VME() && BX_CPU_THIS_PTR get_VIP() == 0)
      {
        BX_CPU_THIS_PTR assert_VIF();
        BX_NEXT_INSTR(i);
      }
#endif
      BX_DEBUG(("STI: IOPL != 3 in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

  if (! BX_CPU_THIS_PTR get_IF()) {
    BX_CPU_THIS_PTR assert_IF();
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CLD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR clear_DF();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR assert_DF();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMC(bxInstruction_c *i)
{
  set_CF(! get_CF());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fw(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_PUSHF)) Svm_Vmexit(SVM_VMEXIT_PUSHF);
  }
#endif

  Bit16u flags = (Bit16u) read_eflags();

  if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
#if BX_CPU_LEVEL >= 5
      if (BX_CPU_THIS_PTR cr4.get_VME()) {
        flags |= EFlagsIOPLMask;
        if (BX_CPU_THIS_PTR get_VIF())
          flags |=  EFlagsIFMask;
        else
          flags &= ~EFlagsIFMask;
      }
      else
#endif
      {
        BX_DEBUG(("PUSHFW: #GP(0) in v8086 (no VME) mode"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
  }

  push_16(flags);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fw(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_POPF)) Svm_Vmexit(SVM_VMEXIT_POPF);
  }
#endif

  // Build a mask of the following bits:
  // x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask | EFlagsNTMask;

  RSP_SPECULATIVE;

  Bit16u flags16 = pop_16();

  if (protected_mode()) {
    if (CPL==0)
      changeMask |= EFlagsIOPLMask;
    if (CPL <= BX_CPU_THIS_PTR get_IOPL())
      changeMask |= EFlagsIFMask;
  }
  else if (v8086_mode()) {
    if (BX_CPU_THIS_PTR get_IOPL() < 3) {
#if BX_CPU_LEVEL >= 5
      if (BX_CPU_THIS_PTR cr4.get_VME()) {

        if (((flags16 & EFlagsIFMask) && BX_CPU_THIS_PTR get_VIP()) ||
             (flags16 & EFlagsTFMask))
        {
          BX_ERROR(("POPFW: #GP(0) in VME mode"));
          exception(BX_GP_EXCEPTION, 0);
        }

        // IF, IOPL unchanged, EFLAGS.VIF = TMP_FLAGS.IF
        changeMask |= EFlagsVIFMask;
        Bit32u flags32 = (Bit32u) flags16;
        if (flags32 & EFlagsIFMask) flags32 |= EFlagsVIFMask;
        writeEFlags(flags32, changeMask);
        RSP_COMMIT;

        BX_NEXT_INSTR(i);
      }
#endif
      BX_DEBUG(("POPFW: #GP(0) in v8086 (no VME) mode"));
      exception(BX_GP_EXCEPTION, 0);
    }

    changeMask |= EFlagsIFMask;
  }
  else {
    // All non-reserved flags can be modified
    changeMask |= (EFlagsIOPLMask | EFlagsIFMask);
  }

  writeEFlags((Bit32u) flags16, changeMask);

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 3

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fd(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_PUSHF)) Svm_Vmexit(SVM_VMEXIT_PUSHF);
  }
#endif

  if (v8086_mode() && (BX_CPU_THIS_PTR get_IOPL()<3)) {
    BX_DEBUG(("PUSHFD: #GP(0) in v8086 mode"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // VM & RF flags cleared in image stored on the stack
  push_32(read_eflags() & 0x00fcffff);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fd(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_POPF)) Svm_Vmexit(SVM_VMEXIT_POPF);
  }
#endif

  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask |
                          EFlagsDFMask | EFlagsNTMask | EFlagsRFMask;
#if BX_CPU_LEVEL >= 4
  changeMask |= (EFlagsIDMask | EFlagsACMask);  // ID/AC
#endif

  RSP_SPECULATIVE;

  Bit32u flags32 = pop_32();

  if (protected_mode()) {
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
      BX_ERROR(("POPFD: #GP(0) in v8086 mode"));
      exception(BX_GP_EXCEPTION, 0);
    }
    // v8086-mode: VM, IOPL, VIP, VIF are unaffected
    changeMask |= EFlagsIFMask;
  }
  else { // Real-mode
    // VIF, VIP, VM are unaffected
    changeMask |= (EFlagsIOPLMask | EFlagsIFMask);
  }

  writeEFlags(flags32, changeMask);

  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PUSHF_Fq(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_PUSHF)) Svm_Vmexit(SVM_VMEXIT_PUSHF);
  }
#endif

  // VM & RF flags cleared in image stored on the stack
  push_64(read_eflags() & 0x00fcffff);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPF_Fq(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_POPF)) Svm_Vmexit(SVM_VMEXIT_POPF);
  }
#endif

  // Build a mask of the following bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = EFlagsOSZAPCMask | EFlagsTFMask | EFlagsDFMask
                        | EFlagsNTMask | EFlagsRFMask | EFlagsACMask
                        | EFlagsIDMask;

  BX_ASSERT (protected_mode());

  Bit32u eflags32 = (Bit32u) pop_64();

  if (CPL==0)
    changeMask |= EFlagsIOPLMask;
  if (CPL <= BX_CPU_THIS_PTR get_IOPL())
    changeMask |= EFlagsIFMask;

  // VIF, VIP, VM are unaffected
  writeEFlags(eflags32, changeMask);

  BX_NEXT_INSTR(i);
}
#endif

#endif  // BX_CPU_LEVEL >= 3

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SALC(bxInstruction_c *i)
{
  if (get_CF()) {
    AL = 0xff;
  }
  else {
    AL = 0x00;
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STAC(bxInstruction_c *i)
{
  if (CPL != 0) {
    BX_ERROR(("STAC is not recognized when CPL != 0"));
    exception(BX_UD_EXCEPTION, 0);
  }

  assert_AC();

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CLAC(bxInstruction_c *i)
{
  if (CPL != 0) {
    BX_ERROR(("CLAC is not recognized when CPL != 0"));
    exception(BX_UD_EXCEPTION, 0);
  }

  clear_AC();

  BX_NEXT_INSTR(i);
}
