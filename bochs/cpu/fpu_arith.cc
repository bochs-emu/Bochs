/////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2004  MandrakeSoft S.A.
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

///////////////////////////////////////////////////////
/// TODO: Handle FPU last instruction and data pointers 
///////////////////////////////////////////////////////


#if BX_SUPPORT_FPU

/* Returns 1 if unmasked exception occured */
int BX_CPU_C::FPU_exception(int exception)
{
  int unmasked = 0;

  /* Extract only the bits which we use to set the status word */
  exception &= (FPU_SW_Exceptions_Mask);

  /* Set the corresponding exception bits */
  FPU_PARTIAL_STATUS |= exception;

  /* Set summary bits iff exception isn't masked */
  if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask)
  {
      FPU_PARTIAL_STATUS |= (FPU_SW_Summary | FPU_SW_Backward);
      unmasked = 1;
  }

  if (exception & (FPU_SW_Stack_Fault | FPU_EX_Precision))
  {
      if (! (exception & FPU_SW_C1))
        /* This bit distinguishes over- from underflow for a stack fault,
             and roundup from round-down for precision loss. */
        FPU_PARTIAL_STATUS &= ~FPU_SW_C1;
  }

  return unmasked;
}

void BX_CPU_C::FPU_stack_overflow(void)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
      BX_CPU_THIS_PTR the_i387.FPU_push();
      BX_WRITE_FPU_REGISTER_AND_TAG(Const_QNaN, FPU_Tag_Special, 0);
  }
  BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Overflow);
}

void BX_CPU_C::FPU_stack_underflow(int stnr, int pop_stack)
{
  /* The masked response */
  if (BX_CPU_THIS_PTR the_i387.is_IA_masked())
  {
      BX_WRITE_FPU_REGISTER_AND_TAG(Const_QNaN, FPU_Tag_Special, stnr);
      if (pop_stack) 
          BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);
}

softfloat_status_word_t FPU_pre_exception_handling(Bit16u control_word)
{
  softfloat_status_word_t status;

  int precision = control_word & FPU_CW_PC;

/* p 15-5: Precision control bits affect only the following:
   ADD, SUB(R), MUL, DIV(R), and SQRT */
#define FPU_PR_32_BITS        (0x000)
#define FPU_PR_RESERVED_BITS  (0x100)
#define FPU_PR_64_BITS        (0x200)
#define FPU_PR_80_BITS        (0x300)
  
  switch(precision)
  {
     case FPU_PR_32_BITS:
       status.float_rounding_precision = 32;
       break;
     case FPU_PR_64_BITS:
       status.float_rounding_precision = 64;
       break;
     default:
       status.float_rounding_precision = 80;
  }

  status.float_detect_tininess = float_tininess_after_rounding;
  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = (control_word & FPU_CW_RC) >> 10;
  status.flush_underflow_to_zero = 0;

  return status;
}
#endif

void BX_CPU_C::FADD_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(0), 
      BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FADD_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FADD(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(0), 
	      float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FADD_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FADD_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(0), 
	      float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FADD_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIADD_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(0), 
	      int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIADD_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIADD_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(BX_READ_FPU_REG(0), 
	      int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIADD_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(0), 
	BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FMUL_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FMUL(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(0), 
	      float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FMUL_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FMUL_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(0), 
	      float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FMUL_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIMUL_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(0), 
	      int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIMUL_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIMUL_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(BX_READ_FPU_REG(0), 
	      int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIMUL_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUB_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUBR_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FSUB(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FSUBR(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	      float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUB_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(float32_to_floatx80(load_reg, status), 
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUBR_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUB_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	      float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUB_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSUBR_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(float64_to_floatx80(load_reg, status), 
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSUBR_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUB_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	      int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FISUB_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUBR_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(int32_to_floatx80((Bit32s)(load_reg)),
              BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FISUBR_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUB_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), 
	      int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FISUB_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FISUBR_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(int32_to_floatx80(load_reg),
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FISUBR_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIV_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIVR_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(i->rm()), 
	BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FDIV(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_STi_ST0(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = (i->b1() & 0x10) >> 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(i->rm(), pop_stack);
     return;
  }

  softfloat_status_word_t status = 
	FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, i->rm());

  if (pop_stack) 
     BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FDIVR(P)_STi_ST0: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	      float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIV_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(float32_to_floatx80(load_reg, status), 
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIVR_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIV_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	      float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIV_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FDIVR_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(float64_to_floatx80(load_reg, status), 
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FDIVR_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIV_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	      int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIDIV_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIVR_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(int32_to_floatx80((Bit32s)(load_reg)),
              BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIDIVR_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIV_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(BX_READ_FPU_REG(0), 
	      int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIDIV_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FIDIVR_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(int32_to_floatx80(load_reg),
	      BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FIDIVR_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FSQRT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sqrt(BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FSQRT: required FPU, configure --enable-fpu"));
#endif
}

/* D9 FC */
void BX_CPU_C::FRNDINT(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_round_to_int(BX_READ_FPU_REG(0), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_WRITE_FPU_REG(result, 0);
#else
  BX_INFO(("FRNDINT: required FPU, configure --enable-fpu"));
#endif
}
