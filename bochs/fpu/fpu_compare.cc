/////////////////////////////////////////////////////////////////////////
// $Id: fpu_compare.cc,v 1.10 2007-04-02 10:46:57 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

extern float_status_t FPU_pre_exception_handling(Bit16u control_word);

#if BX_SUPPORT_FPU

#include "softfloatx80.h"

static int status_word_flags_fpu_compare(int float_relation)
{
  switch(float_relation) {
     case float_relation_unordered:
         return (FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

     case float_relation_greater:
         return (0);

     case float_relation_less:
         return (FPU_SW_C0);

     case float_relation_equal:
         return (FPU_SW_C3);
  }

  return (-1);	// should never get here
}
#endif

#if BX_SUPPORT_FPU || BX_SUPPORT_SSE >= 1
void BX_CPU_C::write_eflags_fpu_compare(int float_relation)
{
  switch(float_relation) {
   case float_relation_unordered:
      setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
      break;

   case float_relation_greater:
      setEFlagsOSZAPC(0);
      break;

   case float_relation_less:
      setEFlagsOSZAPC(EFlagsCFMask);
      break;

   case float_relation_equal:
      setEFlagsOSZAPC(EFlagsZFMask);
      break;

   default:
      BX_PANIC(("write_eflags: unknown floating point compare relation"));
  }
}
#endif

void BX_CPU_C::FCOM_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOM(P)_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMI_ST0_STj(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->b1() & 4;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOMI(P)_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FUCOMI_ST0_STj(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->b1() & 4;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FUCOMI(P)_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FUCOM_STi(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->rm()))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FUCOM(P)_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOM_SINGLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOM(P)_SINGLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOM_DOUBLE_REAL(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOM(P)_DOUBLE_REAL: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOM_WORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FICOM(P)_WORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FICOM_DWORD_INTEGER(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  int pop_stack = i->nnn() & 1;

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FICOM(P)_DWORD_INTEGER: required FPU, configure --enable-fpu"));
#endif
}

/* DE D9 */
void BX_CPU_C::FCOMPP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

          BX_CPU_THIS_PTR the_i387.FPU_pop();
          BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOMPP: required FPU, configure --enable-fpu"));
#endif
}

/* DA E9 */
void BX_CPU_C::FUCOMPP(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

          BX_CPU_THIS_PTR the_i387.FPU_pop();
          BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FUCOMPP: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCMOV_ST0_STj(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  BX_CPU_THIS_PTR prepareFPU(i);

  int st0_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(0);
  int sti_tag = BX_CPU_THIS_PTR the_i387.FPU_gettagi(i->rm());

  if (st0_tag == FPU_Tag_Empty || sti_tag == FPU_Tag_Empty)
  {
     BX_CPU_THIS_PTR FPU_stack_underflow(0);
     return;
  }

  floatx80 sti_reg = BX_READ_FPU_REG(i->rm());

  bx_bool condition = 0;
  switch(i->nnn())
  {
     case 0: condition = get_CF(); break;
     case 1: condition = get_ZF(); break;
     case 2: condition = get_CF() || get_ZF(); break;
     case 3: condition = get_PF(); break;
     default:
        BX_PANIC(("FCMOV_ST0_STj: default case"));
  }
  if (i->b1() & 1) 
     condition = !condition;

  if (condition)
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);
 
#else
  BX_INFO(("FCMOV_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* D9 E4 */
void BX_CPU_C::FTST(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      BX_CPU_THIS_PTR FPU_exception(FPU_EX_Stack_Underflow);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          /* the masked response */
          setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
      }
      return;
  }

  extern const floatx80 Const_Z;

  float_status_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), Const_Z, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  setcc(status_word_flags_fpu_compare(rc));
#else
  BX_INFO(("FTST: required FPU, configure --enable-fpu"));
#endif
}

/* D9 E5 */
void BX_CPU_C::FXAM(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
  BX_CPU_THIS_PTR prepareFPU(i);

  floatx80 reg = BX_READ_FPU_REG(0);
  int sign = floatx80_sign(reg);

  /* 
   * Examine the contents of the ST(0) register and sets the condition 
   * code flags C0, C2 and C3 in the FPU status word to indicate the 
   * class of value or number in the register.
   */

  if (IS_TAG_EMPTY(0))
  {
      setcc(FPU_SW_C3|FPU_SW_C1|FPU_SW_C0);
  }
  else
  {
      float_class_t aClass = floatx80_class(reg);

      switch(aClass)
      {
        case float_zero:
           setcc(FPU_SW_C3|FPU_SW_C1);
           break;
       
        case float_NaN:
           // unsupported handled as NaNs
           if (floatx80_is_unsupported(reg)) {
               setcc(FPU_SW_C1); 
           } else {
               setcc(FPU_SW_C1|FPU_SW_C0);
           }
           break;
       
        case float_negative_inf:
        case float_positive_inf:
           setcc(FPU_SW_C2|FPU_SW_C1|FPU_SW_C0);
           break;
       
        case float_denormal:
           setcc(FPU_SW_C3|FPU_SW_C2|FPU_SW_C1);
           break;
       
        case float_normalized:
           setcc(FPU_SW_C2|FPU_SW_C1);
           break;
      }
  }

  /* 
   * The C1 flag is set to the sign of the value in ST(0), regardless 
   * of whether the register is empty or full.
   */
  if (! sign)
    clear_C1();

#else
  BX_INFO(("FXAM: required FPU, configure --enable-fpu"));
#endif
}
