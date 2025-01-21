/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2025 Stanislav Shwartsman
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

#if BX_SUPPORT_3DNOW && BX_CPU_LEVEL >= 5

extern softfloat_status_t prepare_softfloat_status_word_3dnow(int rounding_mode);

// use 11-bit SSE approximation function, ideally need to use AVX-512 14-bit approximation but it was compiled only under BX_SUPPORT_AVX/BX_SUPPORT_EVEX so far
extern float32 approximate_rcp(float32 a);

// use 11-bit SSE approximation function, ideally need to use AVX-512 14-bit approximation but it was compiled only under BX_SUPPORT_AVX/BX_SUPPORT_EVEX so far
extern float32 approximate_rsqrt(float32 a);

// Thanks for Cristina Iordache and David W. Matula for reverse-engineering of 3dNow! Reciprocal instructions and writing the article
// Analysis of Reciprocal and Square Root Reciprocal Instructions in the AMD K6-2 Implementation of 3DNow!
// https://www.sciencedirect.com/science/article/pii/S1571066105806218

#include "softfloat3e/include/softfloat.h"
#include "fpu/softfloat-specialize.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCP_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister dst;
  float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    dst = BX_READ_MMX_REG(i->src());
    op = MMXUD0(dst);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  op = f32_denormal_to_zero(op);

  // for zero argument produce maximum normal value with the sign of source operand
  if ((op << 1) == 0) {
    op = packFloat32(f32_sign(op), 0x7F, 0x7FFFFF);
  }
  else {
    // Note that Inf/NaN handling is not documented in 3Dnow! manuals
    // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
    op = approximate_rcp(op);
  }

  MMXUD0(dst) = op;
  MMXUD1(dst) = op;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), dst);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRCPIT1_PqQq(bxInstruction_c *i)
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

  // Compute first step of Newton-Raphson Approximation iteration
  // One iteration of the 1/b reciprocal approximation is:
  //       X1 = X0 * (2 - b * X0)

  // Use FMA call to compute (2 - b * X0) as the first step

  // Second step in instruction PFRCPIT2 just has to multiply arguments to complete approximation
  // PFRCPIT2 is aliased to PFMUL and doesn't have Bochs handler

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even);
  const float32 f32_two = 0x40000000;

  MMXUD0(op1) = f32_fnmadd(MMXUD0(op1), MMXUD0(op2), f32_two, &status);
  MMXUD1(op1) = f32_fnmadd(MMXUD1(op1), MMXUD1(op2), f32_two, &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQRT_PqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 5
  BX_CPU_THIS_PTR FPU_check_pending_exceptions();

  BxPackedMmxRegister dst;
  float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    dst = BX_READ_MMX_REG(i->src());
    op = MMXUD0(dst);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_dword(i->seg(), eaddr);
  }

  BX_CPU_THIS_PTR prepareFPU2MMX(); /* FPU2MMX transition */

  op = f32_denormal_to_zero(op);

  // Negative operands are treated as positive operands for purposes of reciprocal square
  // root computation, with the sign of the result the same as the sign of the source operand
  Bit32u sign = op & 0x80000000; // remember the sign

  op &= 0x7FFFFFFF; // reset the sign for now

  // for zero argument produce maximum normal value with the sign of source operand
  if (op == 0) {
    op = sign ^ packFloat32(0, 0x7F, 0x7FFFFF);
  }
  else {
    // Note that Inf/NaN handling is not documented in 3Dnow! manuals
    // This implementation choose IEEE-754 behavior which might not necessary match actual AMD's hardware
    op = sign ^ approximate_rsqrt(op);
  }

  MMXUD0(dst) = op;
  MMXUD1(dst) = op;

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), dst);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::PFRSQIT1_PqQq(bxInstruction_c *i)
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

  // Compute first step of Newton-Raphson Approximation iteration
  // One iteration of the sqrt(1/b) reciprocal approximation is:
  //       X1 = 0.5 * X0 * (3 - b * X0^2)

  // Usage for the instruction to compute a = 1/sqrt(b):

  //    X0 = PFRSQRT(b)                 initial approximation for 1/sqrt(b)
  //    X1 = PFMUL(X0, X0)              compute X0^2
  //    X2 = PFRSQIT1(b,X1)             this instruction compute 1st step of approximation 0.5 * (3 - b * X0^2)
  //                                    from arguments provided to the instruction are: X0^2 and b
  //     a = PFRCPIT2(X2,X0)            multiply by X0

  static softfloat_status_t status = prepare_softfloat_status_word_3dnow(softfloat_round_near_even);
  const float32 f32_three = 0x40400000, f32_half = 0x3f000000;

  // Use FMA call to compute (3 - b * X0^2) as the first step
  MMXUD0(op1) = f32_fnmadd(MMXUD0(op1), MMXUD0(op2), f32_three, &status);
  MMXUD1(op1) = f32_fnmadd(MMXUD1(op1), MMXUD1(op2), f32_three, &status);

  // Multiply result by 0.5
  MMXUD0(op1) = f32_mul(MMXUD0(op1), f32_half, &status);
  MMXUD1(op1) = f32_mul(MMXUD1(op1), f32_half, &status);

  /* now write result back to destination */
  BX_WRITE_MMX_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

#endif
