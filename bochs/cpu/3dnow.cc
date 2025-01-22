/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002-2025 Stanislav Shwartsman
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_3DNOW

#if BX_CPU_LEVEL >= 5

#include "softfloat3e/include/softfloat.h"

softfloat_status_t prepare_softfloat_status_word_3dnow(int rounding_mode = softfloat_round_near_even)
{
  softfloat_status_t status;

  status.softfloat_exceptionFlags = 0; // clear exceptions before execution
  status.softfloat_roundingMode = rounding_mode;
  status.softfloat_flush_underflow_to_zero = true;
  status.softfloat_exceptionMasks = softfloat_all_exceptions_mask;
  status.softfloat_suppressException = softfloat_all_exceptions_mask;
  status.softfloat_denormals_are_zeros = true;

  return status;
}

//////////////////////////////////////////////////////////////////////////////////////////
// helper functions
//////////////////////////////////////////////////////////////////////////////////////////

BX_CPP_INLINE static float32 f32_add_3dnow(float32 a, float32 b)
{
  int a_is_zero = (a<<1) == 0;
  int b_is_zero = (b<<1) == 0;

  if (a_is_zero) {
    if (b_is_zero) {
      // result +0/-0 with sign of logical AND of signs of both operands
      return a & b & 0x80000000;
    }
    return b; // otherwise take src2
  }

  if (b_is_zero) {
    return a; // take src1
  }

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even); // Note, actual rounding mode is not specified by 3dNow! manual

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if one or both arguments are Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  return f32_add(a, b, &status);
}

BX_CPP_INLINE static float32 f32_sub_3dnow(float32 a, float32 b)
{
  int a_is_zero = (a<<1) == 0;
  int b_is_zero = (b<<1) == 0;

  if (a_is_zero) {
    if (b_is_zero) {
      // result +0/-0 with sign of logical AND of the sign of src1 and inverse of the sign of src2
      return a & ~b & 0x80000000;
    }
    return b ^ 0x80000000; // otherwise take -src2
  }

  if (b_is_zero) {
    return a; // take src1
  }

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even); // Note, actual rounding mode is not specified by 3dNow! manual

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if one or both arguments are Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  return f32_sub(a, b, &status);
}

BX_CPP_INLINE static float32 f32_mul_3dnow(float32 a, float32 b)
{
  // if either a or b is zero
  if (((a|b) << 1) == 0) {
    // result is zero with sign of logical XOR of signs of both operands
    return (a ^ b) & 0x80000000;
  }

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even); // Note, actual rounding mode is not specified by 3dNow! manual

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if one or both arguments are Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  return f32_mul(a, b, &status);
}

BX_CPP_INLINE static float32 f32_min_3dnow(float32 a, float32 b)
{
  if (a == 0x80000000) a = 0; // remove sign of zero
  if (b == 0x80000000) b = 0; // remove sign of zero

  // both arguments zero: return +0
  // negative value and zero: return +0

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow();

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if one or both arguments are Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  return f32_min(a, b, &status);
}

BX_CPP_INLINE static float32 f32_max_3dnow(float32 a, float32 b)
{
  if (a == 0x80000000) a = 0; // remove sign of zero
  if (b == 0x80000000) b = 0; // remove sign of zero

  // both arguments zero: return +0
  // negative value and zero: return +0

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow();

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if one or both arguments are Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  return f32_max(a, b, &status);
}

// 3dnow! handling of PFCMPEQ/PFCMPGT/PFCMPGE
BX_CPP_INLINE static int f32_compare_3dnow(float32 a, float32 b)
{
  a = f32_denormal_to_zero(a);
  b = f32_denormal_to_zero(b);

  if ((a == b) || ((uint32_t) ((a | b)<<1) == 0)) return softfloat_relation_equal;

  int signA = f32_sign(a);
  int signB = f32_sign(b);
  if (signA != signB)
      return (signA) ? softfloat_relation_less : softfloat_relation_greater;

  if (signA ^ (a < b)) return softfloat_relation_less;
  return softfloat_relation_greater;
}

#endif

/* 0F 0F /r 8E Enhanced 3DNow! */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFPNACC_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  // Mixed Horizonal SUB/ADD
  MMXUD0(op1) = f32_sub_3dnow(MMXUD0(op1), MMXUD1(op1));
  MMXUD1(op1) = f32_add_3dnow(MMXUD0(op2), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 0C Enhanced 3DNow! */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PI2FW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_to_zero);

  MMXUD0(result) = i32_to_f32((Bit32s)(MMXSW0(op)), &status);
  MMXUD1(result) = i32_to_f32((Bit32s)(MMXSW2(op)), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 0D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PI2FD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_to_zero);

  MMXUD0(op) = i32_to_f32(MMXSD0(op), &status);
  MMXUD1(op) = i32_to_f32(MMXSD1(op), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 1C Enhanced 3DNow! */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PF2IW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_to_zero);

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if argument is Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  MMXSD0(op) = f32_to_i32_round_to_zero_saturate(MMXUD0(op), &status);
  if (MMXSD0(op) < -0x8000)
    MMXUD0(op) = 0xFFFF8000;
  else if (MMXSD0(op) > 0x7FFF)
    MMXSD0(op) = 0x7FFF;

  MMXSD1(op) = f32_to_i32_round_to_zero_saturate(MMXUD1(op), &status);
  if (MMXSD1(op) < -0x8000)
    MMXUD1(op) = 0xFFFF8000;
  else if (MMXSD1(op) > 0x7FFF)
    MMXSD1(op) = 0x7FFF;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 1D */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PF2ID_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_to_zero);

  // Note that Inf/NaN handling is not documented in 3Dnow! manuals
  // The manual doesn't specify what result going to be if argument is Inf/NaN (undefined behavior)
  // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
  MMXSD0(op) = f32_to_i32_round_to_zero_saturate(MMXUD0(op), &status);
  MMXSD1(op) = f32_to_i32_round_to_zero_saturate(MMXUD1(op), &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 8A Enhanced 3DNow! */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFNACC_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  // Horizonal SUB
  MMXUD0(op1) = f32_sub_3dnow(MMXUD0(op1), MMXUD1(op1));
  MMXUD1(op1) = f32_sub_3dnow(MMXUD0(op2), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 90 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPGE_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  int relation0 = f32_compare_3dnow(MMXUD0(op1), MMXUD0(op2));
  int relation1 = f32_compare_3dnow(MMXUD1(op1), MMXUD1(op2));

  MMXUD0(op1) = (relation0 == softfloat_relation_greater) || (relation0 == softfloat_relation_equal) ? 0xFFFFFFFF : 0;
  MMXUD1(op1) = (relation1 == softfloat_relation_greater) || (relation1 == softfloat_relation_equal) ? 0xFFFFFFFF : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 94 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMIN_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_min_3dnow(MMXUD0(op1), MMXUD0(op2));
  MMXUD1(op1) = f32_min_3dnow(MMXUD1(op1), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 9A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFSUB_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_sub_3dnow(MMXUD0(op1), MMXUD0(op2));
  MMXUD1(op1) = f32_sub_3dnow(MMXUD1(op1), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r 9E */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFADD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_add_3dnow(MMXUD0(op1), MMXUD0(op2));
  MMXUD1(op1) = f32_add_3dnow(MMXUD1(op1), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r A0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPGT_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = (f32_compare_3dnow(MMXUD0(op1), MMXUD0(op2)) == softfloat_relation_greater) ? 0xFFFFFFFF : 0;
  MMXUD1(op1) = (f32_compare_3dnow(MMXUD1(op1), MMXUD1(op2)) == softfloat_relation_greater) ? 0xFFFFFFFF : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r A4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMAX_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_max_3dnow(MMXUD0(op1), MMXUD0(op2));
  MMXUD1(op1) = f32_max_3dnow(MMXUD1(op1), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r AA */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFSUBR_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_sub_3dnow(MMXUD0(op2), MMXUD0(op1));
  MMXUD1(op1) = f32_sub_3dnow(MMXUD1(op2), MMXUD1(op1));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r AE */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFACC_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  // Horizonal ADD
  MMXUD0(op1) = f32_add_3dnow(MMXUD0(op1), MMXUD1(op1));
  MMXUD1(op1) = f32_add_3dnow(MMXUD0(op2), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r B0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFCMPEQ_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = (f32_compare_3dnow(MMXUD0(op1), MMXUD0(op2)) == softfloat_relation_equal) ? 0xFFFFFFFF : 0;
  MMXUD1(op1) = (f32_compare_3dnow(MMXUD1(op1), MMXUD1(op2)) == softfloat_relation_equal) ? 0xFFFFFFFF : 0;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r B4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFMUL_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(op1) = f32_mul_3dnow(MMXUD0(op1), MMXUD0(op2));
  MMXUD1(op1) = f32_mul_3dnow(MMXUD1(op1), MMXUD1(op2));

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r B7 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PMULHRW_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister op1 = BX_READ_MMX_REG(i->dst()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op2 = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  Bit32s product1 = Bit32s(MMXSW0(op1)) * Bit32s(MMXSW0(op2)) + 0x8000;
  Bit32s product2 = Bit32s(MMXSW1(op1)) * Bit32s(MMXSW1(op2)) + 0x8000;
  Bit32s product3 = Bit32s(MMXSW2(op1)) * Bit32s(MMXSW2(op2)) + 0x8000;
  Bit32s product4 = Bit32s(MMXSW3(op1)) * Bit32s(MMXSW3(op2)) + 0x8000;

  MMXUW0(op1) = Bit16u(product1 >> 16);
  MMXUW1(op1) = Bit16u(product2 >> 16);
  MMXUW2(op1) = Bit16u(product3 >> 16);
  MMXUW3(op1) = Bit16u(product4 >> 16);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 0F /r BB Enhanced 3DNow! */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::PSWAPD_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister result, op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  MMXUD0(result) = MMXUD1(op);
  MMXUD1(result) = MMXUD0(op);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

#endif
