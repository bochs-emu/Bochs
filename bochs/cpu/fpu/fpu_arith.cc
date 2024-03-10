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

float_status_t i387cw_to_softfloat_status_word(Bit16u control_word)
{
  float_status_t status;

  int precision = control_word & FPU_CW_PC;

  switch(precision)
  {
     case FPU_PR_32_BITS:
       status.float_rounding_precision = 32;
       break;
     case FPU_PR_64_BITS:
       status.float_rounding_precision = 64;
       break;
     case FPU_PR_80_BITS:
       status.float_rounding_precision = 80;
       break;
     default:
    /* With the precision control bits set to 01 "(reserved)", a
       real CPU behaves as if the precision control bits were
       set to 11 "80 bits" */
       status.float_rounding_precision = 80;
  }

  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = (control_word & FPU_CW_RC) >> 10;
  status.flush_underflow_to_zero = 0;
  status.float_suppress_exception = 0;
  status.float_exception_masks = control_word & FPU_CW_Exceptions_Mask;
  status.denormals_are_zeros = 0;

  return status;
}

#include "softfloatx80.h"

floatx80 FPU_handle_NaN(floatx80 a, int aIsNaN, float32 b32, int bIsNaN, float_status_t &status)
{
    int aIsSignalingNaN = floatx80_is_signaling_nan(a);
    int bIsSignalingNaN = float32_is_signaling_nan(b32);

    if (aIsSignalingNaN | bIsSignalingNaN)
        float_raise(status, float_flag_invalid);

    // propagate QNaN to SNaN
    a = propagateFloatx80NaN(a, status);

    if (aIsNaN & !bIsNaN) return a;

    // float32 is NaN so conversion will propagate SNaN to QNaN and raise
    // appropriate exception flags
    floatx80 b = f32_to_extF80(b32, &status);

    if (aIsSignalingNaN) {
        if (bIsSignalingNaN) goto returnLargerSignificand;
        return bIsNaN ? b : a;
    }
    else if (aIsNaN) {
        if (bIsSignalingNaN) return a;
 returnLargerSignificand:
        if (a.fraction < b.fraction) return b;
        if (b.fraction < a.fraction) return a;
        return (a.exp < b.exp) ? a : b;
    }
    else {
        return b;
    }
}

bool FPU_handle_NaN(floatx80 a, float32 b, floatx80 &r, float_status_t &status)
{
  if (floatx80_is_unsupported(a)) {
     float_raise(status, float_flag_invalid);
     r = floatx80_default_nan;
     return true;
  }

  int aIsNaN = floatx80_is_nan(a), bIsNaN = float32_is_nan(b);
  if (aIsNaN | bIsNaN) {
     r = FPU_handle_NaN(a, aIsNaN, b, bIsNaN, status);
     return true;
  }
  return false;
}

floatx80 FPU_handle_NaN(floatx80 a, int aIsNaN, float64 b64, int bIsNaN, float_status_t &status)
{
    int aIsSignalingNaN = floatx80_is_signaling_nan(a);
    int bIsSignalingNaN = float64_is_signaling_nan(b64);

    if (aIsSignalingNaN | bIsSignalingNaN)
        float_raise(status, float_flag_invalid);

    // propagate QNaN to SNaN
    a = propagateFloatx80NaN(a, status);

    if (aIsNaN & !bIsNaN) return a;

    // float64 is NaN so conversion will propagate SNaN to QNaN and raise
    // appropriate exception flags
    floatx80 b = f64_to_extF80(b64, &status);

    if (aIsSignalingNaN) {
        if (bIsSignalingNaN) goto returnLargerSignificand;
        return bIsNaN ? b : a;
    }
    else if (aIsNaN) {
        if (bIsSignalingNaN) return a;
 returnLargerSignificand:
        if (a.fraction < b.fraction) return b;
        if (b.fraction < a.fraction) return a;
        return (a.exp < b.exp) ? a : b;
    }
    else {
        return b;
    }
}

bool FPU_handle_NaN(floatx80 a, float64 b, floatx80 &r, float_status_t &status)
{
  if (floatx80_is_unsupported(a)) {
     float_raise(status, float_flag_invalid);
     r = floatx80_default_nan;
     return true;
  }

  int aIsNaN = floatx80_is_nan(a), bIsNaN = float64_is_nan(b);
  if (aIsNaN | bIsNaN) {
     r = FPU_handle_NaN(a, aIsNaN, b, bIsNaN, status);
     return true;
  }
  return false;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FADD_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->src());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FADD_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->dst());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FADD_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_add(a, f32_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FADD_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_add(a, f64_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIADD_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80((Bit32s)(load_reg));

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIADD_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80(load_reg);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_add(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FMUL_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->src());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FMUL_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->dst());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FMUL_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_mul(a, f32_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FMUL_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_mul(a, f64_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIMUL_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80((Bit32s)(load_reg));

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIMUL_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80(load_reg);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_mul(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUB_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->src());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUBR_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->src());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUB_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->dst());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUBR_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->dst());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUB_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_sub(a, f32_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUBR_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = floatx80_sub(f32_to_extF80(load_reg, &status), b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUB_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_sub(a, f64_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSUBR_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());


  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = floatx80_sub(f64_to_extF80(load_reg, &status), b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISUB_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80((Bit32s)(load_reg));

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISUBR_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = i32_to_extF80((Bit32s)(load_reg));
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISUB_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(BX_READ_FPU_REG(0), i32_to_extF80(load_reg), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISUBR_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = i32_to_extF80(load_reg);
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sub(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIV_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->src());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIVR_ST0_STj(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->src()))
  {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->src());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIV_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(i->dst());
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIVR_STi_ST0(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  int pop_stack = i->b1() & 2;

  clear_C1();

  if (IS_TAG_EMPTY(0) || IS_TAG_EMPTY(i->dst()))
  {
     FPU_stack_underflow(i, i->dst(), pop_stack);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = BX_READ_FPU_REG(i->dst());

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags)) {
     BX_WRITE_FPU_REG(result, i->dst());
     if (pop_stack)
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIV_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_div(a, f32_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIVR_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = floatx80_div(f32_to_extF80(load_reg, &status), b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIV_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = floatx80_div(a, f64_to_extF80(load_reg, &status), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FDIVR_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = floatx80_div(f64_to_extF80(load_reg, &status), b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIDIV_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80((Bit32s)(load_reg));

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIDIVR_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = i32_to_extF80((Bit32s)(load_reg));
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIDIV_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = BX_READ_FPU_REG(0);
  floatx80 b = i32_to_extF80(load_reg);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIDIVR_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  floatx80 a = i32_to_extF80(load_reg);
  floatx80 b = BX_READ_FPU_REG(0);

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_div(a, b, status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSQRT(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_sqrt(BX_READ_FPU_REG(0), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

/* D9 FC */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FRNDINT(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  BX_CPU_THIS_PTR FPU_update_last_instruction(i);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
     FPU_stack_underflow(i, 0);
     BX_NEXT_INSTR(i);
  }

  float_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = floatx80_round_to_int(BX_READ_FPU_REG(0), status);

  if (! FPU_exception(i, status.float_exception_flags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

#endif
