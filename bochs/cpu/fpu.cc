/////////////////////////////////////////////////////////////////////////
// $Id: fpu.cc,v 1.10 2004-02-11 20:04:34 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
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
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_FPU
void BX_CPU_C::prepareFPU(void)
{
  if (BX_CPU_THIS_PTR cr0.em || BX_CPU_THIS_PTR cr0.ts) {
    exception(BX_NM_EXCEPTION, 0, 0);
  }
}

void BX_CPU_C::FPU_check_pending_exceptions(void)
{
  if(FPU_PARTIAL_STATUS & FPU_SW_SUMMARY)
  {
    if (BX_CPU_THIS_PTR cr0.ne == 0)
    {
      // MSDOS compatibility external interrupt (IRQ13)
      BX_INFO (("math_abort: MSDOS compatibility FPU exception"));
      DEV_pic_raise_irq(13);
    }
    else
    {
      exception(BX_MF_EXCEPTION, 0, 0);
    }
  }
}
#endif

void BX_CPU_C::FWAIT(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  // WAIT doesn't generate single steps on 8086.
  // The same goes for prefix instructions, and instructions which
  // modify segment registers. (pg4-16)
  BX_PANIC(("FWAIT: not implemented for < 386"));
#else // BX_CPU_LEVEL >= 3

#if BX_SUPPORT_FPU
  if (BX_CPU_THIS_PTR cr0.ts && BX_CPU_THIS_PTR cr0.mp) {
    exception(BX_NM_EXCEPTION, 0, 0);
  }

  if (FPU_PARTIAL_STATUS & FPU_SW_SUMMARY)
      exception(BX_MF_EXCEPTION, 0, 0);
#else
  BX_INFO(("FWAIT: requred FPU, use --enable-fpu"));
#endif

#endif // BX_CPU_LEVEL >= 3
}

void BX_CPU_C::FLD_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLD_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLD_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLD_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD_EXTENDED_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLD_EXTENDED_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FILD_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FILD_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FILD_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FILD_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FILD_QWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FILD_QWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDENV: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDCW: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLD1(bxInstruction_c *i) 
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLD1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDL2T(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDL2T: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDL2E(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDL2E: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDPI(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDPI: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDLG2(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDLG2: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDLN2(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDLN2: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FLDZ(bxInstruction_c *i)                      
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FLDZ: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FBLD_PACKED_BCD(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FBLD_PACKED_BCD: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FST_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSTP_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSTP_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FST_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSTP_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSTP_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FST_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FST_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSTP_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSTP_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSTP_EXTENDED_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSTP_EXTENDED_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIST_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIST_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISTP_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISTP_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIST_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIST_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISTP_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISTP_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISTP_QWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISTP_QWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNSTENV(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNSTENV: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNSTCW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNSTCW: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNSTSW(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNSTSW: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNSTSW_AX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNSTSW_AX: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FBSTP_PACKED_BCD(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FBSTP_PACKED_BCD: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISTTP16(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_PANIC(("FISTTP16: instruction still not implemented"));
#else
  BX_INFO(("FISTTP16: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FISTTP32(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_PANIC(("FISTTP32: instruction still not implemented"));
#else
  BX_INFO(("FISTTP32: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FISTTP64(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_PANIC(("FISTTP64: instruction still not implemented"));
#else
  BX_INFO(("FISTTP64: required PNI, configure --enable-pni"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FRSTOR: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNSAVE: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FADD_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FADD_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADDP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FADDP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FADD_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FADD_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIADD_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIADD_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIADD_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIADD_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FMUL_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FMUL_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMULP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FMULP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FMUL_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FMUL_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIMUL_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIMUL_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIMUL_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIMUL_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUB_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBR_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUB_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBR_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBRP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBRP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUB_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBR_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUB_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSUBR_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUB_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISUB_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUBR_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISUBR_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUB_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISUB_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUBR_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FISUBR_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIV_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVR_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIV_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVR_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVRP_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVRP_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIV_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVR_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIV_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDIVR_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIV_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIDIV_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIVR_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIDIVR_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIV_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIDIV_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIVR_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FIDIVR_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOM_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOM_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMP_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMP_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMI_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMI_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMIP_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMIP_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOMI_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FUCOMI_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOMIP_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FUCOMIP_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOM_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FUCOM_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOMP_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FUCOMP_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOM_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOM_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMP_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMP_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOM_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOM_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMP_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMP_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOM_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FICOM_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOMP_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FICOMP_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOM_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FICOM_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOMP_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FICOMP_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMPP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOMPP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOMPP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FUCOMPP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCMOVB_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVB_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVE_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVBE_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVBE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVU_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVU_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVNB_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVNB_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVNE_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVNE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVNBE_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVNBE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FCMOVNU_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU && (BX_CPU_LEVEL == 6)
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCMOVNU_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FXCH_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FXCH_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNOP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  // Perform no FPU operation. This instruction takes up space in the
  // instruction stream but does not affect the FPU or machine
  // context, except the EIP register.
#else
  BX_INFO(("FNOP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCHS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCHS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FABS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FABS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FTST(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FTST: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FXAM(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FXAM: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::F2XM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("F2XM1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FYL2X(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FYL2X: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPTAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FPTAN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPATAN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FPATAN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FXTRACT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FXTRACT: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPREM1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FPREM1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDECSTP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FDECSTP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FINCSTP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FINCSTP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FPREM(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FPREM: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FYL2XP1(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FYL2XP1: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSQRT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSQRT: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSINCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSINCOS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FRNDINT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FRNDINT: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSCALE(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSCALE: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSIN(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FSIN: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOS(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FCOS: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNCLEX(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNCLEX: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FNINIT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FNINIT: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FFREE_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU();

  fpu_execute(i);
#else
  BX_INFO(("FFREE_STi: required FPU, configure --enable-fpu"));
#endif
}
