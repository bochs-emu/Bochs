/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2018 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu/cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_FPU

#include "cpu/decoder/ia_opcodes.h"

#include "softfloat3e/include/softfloat.h"

#include "softfloat-specialize.h"

extern softfloat_status_t i387cw_to_softfloat_status_word(Bit16u control_word);

static int status_word_flags_fpu_compare(int float_relation)
{
  switch(float_relation) {
     case softfloat_relation_unordered:
         return (FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

     case softfloat_relation_greater:
         return (0);

     case softfloat_relation_less:
         return (FPU_SW_C0);

     case softfloat_relation_equal:
         return (FPU_SW_C3);
  }

  return (-1);        // should never get here
}

void BX_CPU_C::write_eflags_fpu_compare(int float_relation)
{
  switch(float_relation) {
   case softfloat_relation_unordered:
      setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
      break;

   case softfloat_relation_greater:
      clearEFlagsOSZAPC();
      break;

   case softfloat_relation_less:
      clearEFlagsOSZAPC();
      assert_CF();
      break;

   case softfloat_relation_equal:
      clearEFlagsOSZAPC();
      assert_ZF();
      break;

   default:
      BX_PANIC(("write_eflags: unknown floating point compare relation"));
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOM_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = (i->getIaOpcode() == BX_IA_FCOMP_STi);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->src()), &status);
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOMI_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 4;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->src()), &status);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FUCOMI_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 4;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->src()), &status);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FUCOM_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = (i->getIaOpcode() == BX_IA_FUCOMP_STi);

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare_quiet(BX_READ_FPU_REG(0), BX_READ_FPU_REG(i->src()), &status);
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOM_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  int rc, pop_stack = (i->getIaOpcode() == BX_IA_FCOMP_SINGLE_REAL);

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0);

  if (extF80_isNaN(a) || extF80_isUnsupported(a) || f32_isNaN(load_reg)) {
    rc = softfloat_relation_unordered;
    softfloat_raiseFlags(&status, softfloat_flag_invalid);
  }
  else {
    rc = extF80_compare(a, f32_to_extF80(load_reg, &status), &status);
  }
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOM_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  int rc, pop_stack = (i->getIaOpcode() == BX_IA_FCOMP_DOUBLE_REAL);

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0);

  if (extF80_isNaN(a) || extF80_isUnsupported(a) || f64_isNaN(load_reg)) {
    rc = softfloat_relation_unordered;
    softfloat_raiseFlags(&status, softfloat_flag_invalid);
  }
  else {
    rc = extF80_compare(a, f64_to_extF80(load_reg, &status), &status);
  }
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FICOM_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  int pop_stack = (i->getIaOpcode() == BX_IA_FICOMP_WORD_INTEGER);

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare(BX_READ_FPU_REG(0),
                      i32_to_extF80((Bit32s)(load_reg)), &status);
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FICOM_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  int pop_stack = (i->getIaOpcode() == BX_IA_FICOMP_DWORD_INTEGER);

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          if (pop_stack)
              BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare(BX_READ_FPU_REG(0), i32_to_extF80(load_reg), &status);
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     if (pop_stack)
         BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

/* DE D9 - FCOMPP  */
/* DA E9 - FUCOMPP */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FCOMPP(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(1))
  {
      FPU_exception(i, FPU_EX_Stack_Underflow);
      setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);

      if(BX_CPU_THIS_PTR the_i387.is_IA_masked())
      {
          BX_CPU_THIS_PTR the_i387.FPU_pop();
          BX_CPU_THIS_PTR the_i387.FPU_pop();
      }
      BX_NEXT_INSTR(i);
  }

  bool quiet = (i->getIaOpcode() == BX_IA_FUCOMPP);

  softfloat_status_t status =
      i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  int rc = extF80_compare(BX_READ_FPU_REG(0), BX_READ_FPU_REG(1), quiet, &status);
  setcc(status_word_flags_fpu_compare(rc));

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
     BX_CPU_THIS_PTR the_i387.FPU_pop();
     BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

/* D9 E4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FTST(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_exception(i, FPU_EX_Stack_Underflow);
     setcc(FPU_SW_C0|FPU_SW_C2|FPU_SW_C3);
  }
  else {
     static floatx80 Const_Z = packFloatx80(0, 0x0000, 0);

     softfloat_status_t status =
        i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     int rc = extF80_compare(BX_READ_FPU_REG(0), Const_Z, &status);
     setcc(status_word_flags_fpu_compare(rc));
     FPU_exception(i, status.softfloat_exceptionFlags);
  }

  BX_NEXT_INSTR(i);
}

/* D9 E5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FXAM(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  floatx80 reg = BX_READ_FPU_REG(0);
  int sign = extF80_sign(reg);

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
      softfloat_class_t aClass = extF80_class(reg);

      switch(aClass)
      {
        case softfloat_zero:
           setcc(FPU_SW_C3|FPU_SW_C1);
           break;

        case softfloat_SNaN:
        case softfloat_QNaN:
           // unsupported handled as NaNs
           if (extF80_isUnsupported(reg)) {
               setcc(FPU_SW_C1);
           } else {
               setcc(FPU_SW_C1|FPU_SW_C0);
           }
           break;

        case softfloat_negative_inf:
        case softfloat_positive_inf:
           setcc(FPU_SW_C2|FPU_SW_C1|FPU_SW_C0);
           break;

        case softfloat_denormal:
           setcc(FPU_SW_C3|FPU_SW_C2|FPU_SW_C1);
           break;

        case softfloat_normalized:
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

  BX_NEXT_INSTR(i);
}

#endif
