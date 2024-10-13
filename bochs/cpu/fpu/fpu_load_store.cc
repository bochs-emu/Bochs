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

#define swap_values16u(a, b) { Bit16u tmp = a; a = b; b = tmp; }

extern softfloat_status_t i387cw_to_softfloat_status_word(Bit16u control_word);

#include "softfloat-specialize.h"
#include "fpu_trans.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLD_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
    FPU_stack_overflow(i);
    BX_NEXT_INSTR(i);
  }

  floatx80 sti_reg = floatx80_default_nan;

  if (IS_TAG_EMPTY(i->src()))
  {
    FPU_exception(i, FPU_EX_Stack_Underflow);

    if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
      BX_NEXT_INSTR(i);
  }
  else {
    sti_reg = BX_READ_FPU_REG(i->src());
  }

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(sti_reg, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLD_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float32 load_reg = read_virtual_dword(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
    BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
    i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  // convert to floatx80 format
  floatx80 result = f32_to_extF80(load_reg, &status);

  unsigned unmasked = FPU_exception(i, status.softfloat_exceptionFlags);
  if (! (unmasked & FPU_CW_Invalid)) {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLD_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  float64 load_reg = read_virtual_qword(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
    BX_NEXT_INSTR(i);
  }

  softfloat_status_t status =
    i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

  // convert to floatx80 format
  floatx80 result = f64_to_extF80(load_reg, &status);

  unsigned unmasked = FPU_exception(i, status.softfloat_exceptionFlags);
  if (! (unmasked & FPU_CW_Invalid)) {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FLD_EXTENDED_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  floatx80 result;

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  result.signif  = read_virtual_qword(i->seg(), RMAddr(i));
  result.signExp = read_virtual_word(i->seg(), (RMAddr(i)+8) & i->asize_mask());

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
  }
  else {
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* DF /0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FILD_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16s load_reg = (Bit16s) read_virtual_word(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
  }
  else {
    floatx80 result = i32_to_extF80((Bit32s) load_reg);
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* DB /0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FILD_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit32s load_reg = (Bit32s) read_virtual_dword(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
  }
  else {
    floatx80 result = i32_to_extF80(load_reg);
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* DF /5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FILD_QWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit64s load_reg = (Bit64s) read_virtual_qword(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1)) {
    FPU_stack_overflow(i);
  }
  else {
    floatx80 result = i64_to_extF80(load_reg);
    BX_CPU_THIS_PTR the_i387.FPU_push();
    BX_WRITE_FPU_REG(result, 0);
  }

  BX_NEXT_INSTR(i);
}

/* DF /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FBLD_PACKED_BCD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);
  Bit16u hi2 = read_virtual_word(i->seg(), (RMAddr(i) + 8) & i->asize_mask());
  Bit64u lo8 = read_virtual_qword(i->seg(), RMAddr(i));

  FPU_update_last_instruction(i);

  clear_C1();

  if (! IS_TAG_EMPTY(-1))
  {
    FPU_stack_overflow(i);
    BX_NEXT_INSTR(i);
  }

  // convert packed BCD to 64-bit integer
  Bit64s scale = 1;
  Bit64s val64 = 0;

  for (int n = 0; n < 16; n++)
  {
    val64 += (lo8 & 0x0f) * scale;
    lo8 >>= 4;
    scale *= 10;
  }

  val64 += (hi2 & 0x0f) * scale;
  val64 += ((hi2>>4) & 0x0f) * scale * 10;

  floatx80 result = (floatx80) i64_to_extF80(val64);
  if (hi2 & 0x8000)        // set negative
      floatx80_chs(result);

  BX_CPU_THIS_PTR the_i387.FPU_push();
  BX_WRITE_FPU_REG(result, 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FST_STi(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();
  FPU_update_last_instruction(i);

  unsigned opcode = i->getIaOpcode();
  int pop_stack = (opcode != BX_IA_FST_STi);

  clear_C1();

  if (IS_TAG_EMPTY(0)) {
    // D9D8..D9DF - Behaves the same as FSTP (DDD8..DDDF) but won't cause a stack underflow exception
    if (opcode != BX_IA_FSTP_SPECIAL_STi)
        FPU_stack_underflow(i, i->dst(), pop_stack);
    else
        BX_CPU_THIS_PTR the_i387.FPU_pop();
  }
  else {
    floatx80 st0_reg = BX_READ_FPU_REG(0);

    BX_WRITE_FPU_REG(st0_reg, i->dst());
    if (pop_stack)
      BX_CPU_THIS_PTR the_i387.FPU_pop();
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FST_SINGLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  clear_C1();

  float32 save_reg = float32_default_nan; /* The masked response */

  int pop_stack = (i->getIaOpcode() == BX_IA_FSTP_SINGLE_REAL);

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_f32(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_dword(i->seg(), RMAddr(i), save_reg);

  FPU_PARTIAL_STATUS = x87_sw;
  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FST_DOUBLE_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  clear_C1();

  float64 save_reg = float64_default_nan; /* The masked response */

  int pop_stack = (i->getIaOpcode() == BX_IA_FSTP_DOUBLE_REAL);

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_f64(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_qword(i->seg(), RMAddr(i), save_reg);

  FPU_PARTIAL_STATUS = x87_sw;
  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

/* DB /7 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FSTP_EXTENDED_REAL(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  clear_C1();

  floatx80 save_reg = floatx80_default_nan; /* The masked response */

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     save_reg = BX_READ_FPU_REG(0);
  }

  write_virtual_qword(i->seg(), RMAddr(i), save_reg.signif);
  write_virtual_word(i->seg(), (RMAddr(i) + 8) & i->asize_mask(), save_reg.signExp);

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIST_WORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit16s save_reg = int16_indefinite;

  int pop_stack = (i->getIaOpcode() == BX_IA_FISTP_WORD_INTEGER);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i16(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_word(i->seg(), RMAddr(i), (Bit16u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;
  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FIST_DWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit32s save_reg = int32_indefinite; /* The masked response */

  int pop_stack = (i->getIaOpcode() == BX_IA_FISTP_DWORD_INTEGER);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i32(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
         BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_dword(i->seg(), RMAddr(i), (Bit32u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;
  if (pop_stack)
     BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISTP_QWORD_INTEGER(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit64s save_reg = int64_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i64(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
         BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_qword(i->seg(), RMAddr(i), (Bit64u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::FBSTP_PACKED_BCD(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  /*
   * The packed BCD integer indefinite encoding (FFFFC000000000000000H)
   * is stored in response to a masked floating-point invalid-operation
   * exception.
   */
  Bit16u save_reg_hi = 0xFFFF;
  Bit64u save_reg_lo = BX_CONST64(0xC000000000000000);

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
        i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     floatx80 reg = BX_READ_FPU_REG(0);
     Bit64s save_val = extF80_to_i64(reg, &status);

     int sign = extF80_sign(reg);
     if (sign)
        save_val = -save_val;

     if (save_val > BX_CONST64(999999999999999999)) {
        softfloat_setFlags(&status, softfloat_flag_invalid); // throw away other flags
     }

     if (! (status.softfloat_exceptionFlags & softfloat_flag_invalid))
     {
        save_reg_hi = (sign) ? 0x8000 : 0;
        save_reg_lo = 0;

        for (int i=0; i<16; i++) {
           save_reg_lo += ((Bit64u)(save_val % 10)) << (4*i);
           save_val /= 10;
        }

        save_reg_hi += (Bit16u)(save_val % 10);
        save_val /= 10;
        save_reg_hi += (Bit16u)(save_val % 10) << 4;
    }

    /* check for fpu arithmetic exceptions */
    if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  // write packed bcd to memory
  write_virtual_qword(i->seg(), RMAddr(i), save_reg_lo);
  write_virtual_word(i->seg(), (RMAddr(i) + 8) & i->asize_mask(), save_reg_hi);

  FPU_PARTIAL_STATUS = x87_sw;

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

/* DF /1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISTTP16(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit16s save_reg = int16_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i16_round_to_zero(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_word(i->seg(), RMAddr(i), (Bit16u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

/* DB /1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISTTP32(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit32s save_reg = int32_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i32_round_to_zero(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_dword(i->seg(), RMAddr(i), (Bit32u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

/* DD /1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::FISTTP64(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  RMAddr(i) = BX_CPU_RESOLVE_ADDR(i);

  FPU_update_last_instruction(i);

  Bit16u x87_sw = FPU_PARTIAL_STATUS;

  Bit64s save_reg = int64_indefinite; /* The masked response */

  clear_C1();

  if (IS_TAG_EMPTY(0))
  {
     FPU_exception(i, FPU_EX_Stack_Underflow);

     if (! BX_CPU_THIS_PTR the_i387.is_IA_masked())
        BX_NEXT_INSTR(i);
  }
  else
  {
     softfloat_status_t status =
         i387cw_to_softfloat_status_word(BX_CPU_THIS_PTR the_i387.get_control_word());

     save_reg = extF80_to_i64_round_to_zero(BX_READ_FPU_REG(0), &status);

     if (FPU_exception(i, status.softfloat_exceptionFlags, 1))
        BX_NEXT_INSTR(i);
  }

  // store to the memory might generate an exception, in this case origial FPU_SW must be kept
  swap_values16u(x87_sw, FPU_PARTIAL_STATUS);

  write_virtual_qword(i->seg(), RMAddr(i), (Bit64u)(save_reg));

  FPU_PARTIAL_STATUS = x87_sw;

  BX_CPU_THIS_PTR the_i387.FPU_pop();

  BX_NEXT_INSTR(i);
}

#endif
