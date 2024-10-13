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

#include "softfloat3e/include/softfloat.h"

softfloat_status_t i387cw_to_softfloat_status_word(Bit16u control_word)
{
  softfloat_status_t status;

  int precision = control_word & FPU_CW_PC;

  switch(precision)
  {
     case FPU_PR_32_BITS:
       status.extF80_roundingPrecision = 32;
       break;
     case FPU_PR_64_BITS:
       status.extF80_roundingPrecision = 64;
       break;
     case FPU_PR_80_BITS:
       status.extF80_roundingPrecision = 80;
       break;
     default:
    /* With the precision control bits set to 01 "(reserved)", a
       real CPU behaves as if the precision control bits were
       set to 11 "80 bits" */
       status.extF80_roundingPrecision = 80;
  }

  status.softfloat_exceptionFlags = 0; // clear exceptions before execution
  status.softfloat_roundingMode = (control_word & FPU_CW_RC) >> 10;
  status.softfloat_flush_underflow_to_zero = 0;
  status.softfloat_suppressException = 0;
  status.softfloat_exceptionMasks = control_word & FPU_CW_Exceptions_Mask;
  status.softfloat_denormals_are_zeros = 0;

  return status;
}

#include "fpu_trans.h"

floatx80 FPU_handle_NaN(floatx80 a, int aIsNaN, float32 b32, int bIsNaN, softfloat_status_t &status)
{
    int aIsSignalingNaN = extF80_isSignalingNaN(a);
    int bIsSignalingNaN = f32_isSignalingNaN(b32);

    if (aIsSignalingNaN | bIsSignalingNaN)
        softfloat_raiseFlags(&status, softfloat_flag_invalid);

    // propagate QNaN to SNaN
    a = softfloat_propagateNaNExtF80UI(a.signExp, a.signif, 0, 0, &status);

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
        if (a.signif < b.signif) return b;
        if (b.signif < a.signif) return a;
        return (a.signExp < b.signExp) ? a : b;
    }
    else {
        return b;
    }
}

bool FPU_handle_NaN(floatx80 a, float32 b, floatx80 &r, softfloat_status_t &status)
{
  if (extF80_isUnsupported(a)) {
     softfloat_raiseFlags(&status, softfloat_flag_invalid);
     r = floatx80_default_nan;
     return true;
  }

  int aIsNaN = extF80_isNaN(a), bIsNaN = f32_isNaN(b);
  if (aIsNaN | bIsNaN) {
     r = FPU_handle_NaN(a, aIsNaN, b, bIsNaN, status);
     return true;
  }
  return false;
}

floatx80 FPU_handle_NaN(floatx80 a, int aIsNaN, float64 b64, int bIsNaN, softfloat_status_t &status)
{
    int aIsSignalingNaN = extF80_isSignalingNaN(a);
    int bIsSignalingNaN = f64_isSignalingNaN(b64);

    if (aIsSignalingNaN | bIsSignalingNaN)
        softfloat_raiseFlags(&status, softfloat_flag_invalid);

    // propagate QNaN to SNaN
    a = softfloat_propagateNaNExtF80UI(a.signExp, a.signif, 0, 0, &status);

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
        if (a.signif < b.signif) return b;
        if (b.signif < a.signif) return a;
        return (a.signExp < b.signExp) ? a : b;
    }
    else {
        return b;
    }
}

bool FPU_handle_NaN(floatx80 a, float64 b, floatx80 &r, softfloat_status_t &status)
{
  if (extF80_isUnsupported(a)) {
     softfloat_raiseFlags(&status, softfloat_flag_invalid);
     r = floatx80_default_nan;
     return true;
  }

  int aIsNaN = extF80_isNaN(a), bIsNaN = f64_isNaN(b);
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_add(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_add(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_add(a, f32_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_add(a, f64_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_add(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_add(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_mul(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_mul(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_mul(a, f32_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_mul(a, f64_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_mul(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_mul(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_sub(a, f32_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = extF80_sub(f32_to_extF80(load_reg, &status), b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_sub(a, f64_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());


  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = extF80_sub(f64_to_extF80(load_reg, &status), b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(BX_READ_FPU_REG(0), i32_to_extF80(load_reg), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sub(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags)) {
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_div(a, f32_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = extF80_div(f32_to_extF80(load_reg, &status), b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 a = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(a, load_reg, result, status))
     result = extF80_div(a, f64_to_extF80(load_reg, &status), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 b = BX_READ_FPU_REG(0), result;
  if (! FPU_handle_NaN(b, load_reg, result, status))
     result = extF80_div(f64_to_extF80(load_reg, &status), b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_div(a, b, &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_sqrt(BX_READ_FPU_REG(0), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
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

  softfloat_status_t status =
     i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  floatx80 result = extF80_roundToInt(BX_READ_FPU_REG(0), &status);

  if (! FPU_exception(i, status.softfloat_exceptionFlags))
     BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

#endif
