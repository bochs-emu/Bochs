/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GwMa(bxInstruction_c *i)
{
  Bit16s op1_16 = BX_READ_16BIT_REG(i->dst());

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16s bound_min = (Bit16s) read_virtual_word_32(i->seg(), eaddr);
  Bit16s bound_max = (Bit16s) read_virtual_word_32(i->seg(), (eaddr+2) & i->asize_mask());

  if (op1_16 < bound_min || op1_16 > bound_max) {
    BX_DEBUG(("%s: fails bounds test", i->getIaOpcodeNameShort()));
    exception(BX_BR_EXCEPTION, 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BOUND_GdMa(bxInstruction_c *i)
{
  Bit32s op1_32 = BX_READ_32BIT_REG(i->dst());

  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32s bound_min = (Bit32s) read_virtual_dword_32(i->seg(), eaddr);
  Bit32s bound_max = (Bit32s) read_virtual_dword_32(i->seg(), (eaddr+4) & i->asize_mask());

  if (op1_32 < bound_min || op1_32 > bound_max) {
    BX_DEBUG(("%s: fails bounds test", i->getIaOpcodeNameShort()));
    exception(BX_BR_EXCEPTION, 0);
  }

  BX_NEXT_INSTR(i);
}

// This is an undocumented instrucion (opcode 0xf1) which
// is useful for an ICE system
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INT1(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  VMexit_Event(BX_PRIVILEGED_SOFTWARE_INTERRUPT, 1, 0, 0);
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_ICEBP)) Svm_Vmexit(SVM_VMEXIT_ICEBP);
  }
#endif

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  BX_CPU_THIS_PTR EXT = 1;

  // interrupt is not RSP safe
  interrupt(1, BX_PRIVILEGED_SOFTWARE_INTERRUPT, 0, 0);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INT3(bxInstruction_c *i)
{
  // INT 3 is not IOPL sensitive

#if BX_SUPPORT_VMX
  VMexit_Event(BX_SOFTWARE_EXCEPTION, 3, 0, 0);
#endif

#if BX_SUPPORT_SVM
  SvmInterceptException(BX_SOFTWARE_EXCEPTION, 3, 0, 0);
#endif

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  // interrupt is not RSP safe
  interrupt(3, BX_SOFTWARE_EXCEPTION, 0, 0);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);

  BX_NEXT_TRACE(i);
}


BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INT_Ib(bxInstruction_c *i)
{
  Bit8u vector = i->Ib();

#if BX_SUPPORT_VMX
  VMexit_Event(BX_SOFTWARE_INTERRUPT, vector, 0, 0);
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_SOFTINT))
      Svm_Vmexit(SVM_VMEXIT_SOFTWARE_INTERRUPT, BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_DECODE_ASSIST) ? vector : 0);
  }
#endif

#ifdef SHOW_EXIT_STATUS
  if ((vector == 0x21) && (AH == 0x4c)) {
    BX_INFO(("INT 21/4C called AL=0x%02x, BX=0x%04x", (unsigned) AL, (unsigned) BX));
  }
#endif

#if BX_DEBUGGER
  BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

  interrupt(vector, BX_SOFTWARE_INTERRUPT, 0, 0);

  BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                      EIP);

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INTO(bxInstruction_c *i)
{
  if (get_OF()) {

#if BX_SUPPORT_VMX
    VMexit_Event(BX_SOFTWARE_EXCEPTION, 4, 0, 0);
#endif

#if BX_SUPPORT_SVM
    SvmInterceptException(BX_SOFTWARE_EXCEPTION, 4, 0, 0);
#endif

#if BX_DEBUGGER
    BX_CPU_THIS_PTR show_flag |= Flag_softint;
#endif

    // interrupt is not RSP safe
    interrupt(4, BX_SOFTWARE_EXCEPTION, 0, 0);

    BX_INSTR_FAR_BRANCH(BX_CPU_ID, BX_INSTR_IS_INT,
                        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
                        EIP);
  }

  BX_NEXT_TRACE(i);
}
