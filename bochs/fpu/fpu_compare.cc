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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOM(P)_STi: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FCOMI_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
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

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FCOMI(P)_ST0_STj: required FPU, configure --enable-fpu"));
#endif
}

void BX_CPU_C::FUCOMI_ST0_STj(bxInstruction_c *i)
{
#if BX_SUPPORT_FPU
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

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FUCOMI(P)_ST0_STj: required FPU, configure --enable-fpu"));
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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->rm()), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float32 load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	float32_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  float64 load_reg;
  read_virtual_qword(i->seg(), RMAddr(i), &load_reg);

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	float64_to_floatx80(load_reg, status), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  Bit16s load_reg;
  read_virtual_word(i->seg(), RMAddr(i), (Bit16u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	int32_to_floatx80((Bit32s)(load_reg)), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  Bit32s load_reg;
  read_virtual_dword(i->seg(), RMAddr(i), (Bit32u*)(&load_reg));

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), 
  	int32_to_floatx80(load_reg), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

          BX_CPU_THIS_PTR the_i387.FPU_pop();
          BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

          BX_CPU_THIS_PTR the_i387.FPU_pop();
          BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));

  BX_CPU_THIS_PTR the_i387.FPU_pop();
  BX_CPU_THIS_PTR the_i387.FPU_pop();
#else
  BX_INFO(("FUCOMPP: required FPU, configure --enable-fpu"));
#endif
}

/* DA C0 */
void BX_CPU_C::FCMOVB_ST0_STj(bxInstruction_c *i)
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

  if (get_CF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);
 
#else
  BX_INFO(("FCMOVB_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DA C8 */
void BX_CPU_C::FCMOVE_ST0_STj(bxInstruction_c *i)
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

  if (get_ZF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DA D0 */
void BX_CPU_C::FCMOVBE_ST0_STj(bxInstruction_c *i)
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

  if (get_CF() || get_ZF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVBE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DA D8 */
void BX_CPU_C::FCMOVU_ST0_STj(bxInstruction_c *i)
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

  if (get_PF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVU_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DB C0 */
void BX_CPU_C::FCMOVNB_ST0_STj(bxInstruction_c *i)
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

  if (! get_CF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVNB_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DB C8 */
void BX_CPU_C::FCMOVNE_ST0_STj(bxInstruction_c *i)
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

  if (! get_ZF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVNE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DB D0 */
void BX_CPU_C::FCMOVNBE_ST0_STj(bxInstruction_c *i)
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

  if ((!get_CF()) && (!get_ZF()))
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVNBE_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
  UndefinedOpcode(i);
#endif
}

/* DB D8 */
void BX_CPU_C::FCMOVNU_ST0_STj(bxInstruction_c *i)
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

  if (! get_PF())
     BX_WRITE_FPU_REGISTER_AND_TAG(sti_reg, sti_tag, 0);

#else
  BX_INFO(("FCMOVNU_ST0_STj: required P6 FPU, configure --enable-fpu, cpu-level=6"));
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
          SETCC(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
      }
      return;
  }

  softfloat_status_word_t status = 
      FPU_pre_exception_handling(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = floatx80_compare(BX_READ_FPU_REG(0), Const_Z, status);

  if (BX_CPU_THIS_PTR FPU_exception(status.float_exception_flags))
      return;

  SETCC(status_word_flags_fpu_compare(rc));
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
      SETCC(FPU_SW_C3|FPU_SW_C1|FPU_SW_C0);
  }
  else
  {
      float_class_t aClass = floatx80_class(reg);

      switch(aClass)
      {
        case float_zero:
           SETCC(FPU_SW_C3|FPU_SW_C1);
           break;
       
        case float_NaN:
           // unsupported handled as NaNs
           if (floatx80_is_unsupported(reg)) {
               SETCC(FPU_SW_C1); 
           } else {
               SETCC(FPU_SW_C1|FPU_SW_C0);
           }
           break;
       
        case float_negative_inf:
        case float_positive_inf:
           SETCC(FPU_SW_C2|FPU_SW_C1|FPU_SW_C0);
           break;
       
        case float_denormal:
           SETCC(FPU_SW_C3|FPU_SW_C2|FPU_SW_C1);
           break;
       
        case float_normalized:
           SETCC(FPU_SW_C2|FPU_SW_C1);
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
