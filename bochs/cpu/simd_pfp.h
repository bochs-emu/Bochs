/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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

#ifndef BX_SIMD_PFP_FUNCTIONS_H
#define BX_SIMD_PFP_FUNCTIONS_H

// arithmetic add/sub/mul/div

BX_CPP_INLINE void sse_addps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_add(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_add(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_add(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_addpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_add(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_add(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_subps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_sub(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_sub(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_sub(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_subpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_sub(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_mulps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_mul(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_mul(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_mul(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_mul(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_mulpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_mul(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_mul(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_divps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_div(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_div(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_div(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_div(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_divpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_div(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_div(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_addsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_sub(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_add(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_addsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_add(op1->xmm64u(1), op2->xmm64u(1), status);
}

// horizontal arithmetic add/sub

BX_CPP_INLINE void sse_haddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_add(op1->xmm32u(0), op1->xmm32u(1), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(2), op1->xmm32u(3), status);
  op1->xmm32u(2) = float32_add(op2->xmm32u(0), op2->xmm32u(1), status);
  op1->xmm32u(3) = float32_add(op2->xmm32u(2), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_haddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_add(op1->xmm64u(0), op1->xmm64u(1), status);
  op1->xmm64u(1) = float64_add(op2->xmm64u(0), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_hsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op1->xmm32u(1), status);
  op1->xmm32u(1) = float32_sub(op1->xmm32u(2), op1->xmm32u(3), status);
  op1->xmm32u(2) = float32_sub(op2->xmm32u(0), op2->xmm32u(1), status);
  op1->xmm32u(3) = float32_sub(op2->xmm32u(2), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_hsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op1->xmm64u(1), status);
  op1->xmm64u(1) = float64_sub(op2->xmm64u(0), op2->xmm64u(1), status);
}

// min/max

BX_CPP_INLINE void sse_minps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_min(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_min(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_min(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_min(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_minpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_min(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_min(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void sse_maxps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_max(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_max(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_max(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_max(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void sse_maxpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_max(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_max(op1->xmm64u(1), op2->xmm64u(1), status);
}

// fma

BX_CPP_INLINE void fmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), 0, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), 0, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), 0, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), 0, status);
}

BX_CPP_INLINE void fmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, status);
}

BX_CPP_INLINE void fmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_c, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_c, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_c, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_c, status);
}

BX_CPP_INLINE void fmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_c, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_c, status);
}

BX_CPP_INLINE void fmaddsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_c, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), 0, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_c, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), 0, status);
}

BX_CPP_INLINE void fmaddsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_c, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, status);
}

BX_CPP_INLINE void fmsubaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), 0, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_c, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), 0, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_c, status);
}

BX_CPP_INLINE void fmsubaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_c, status);
}

BX_CPP_INLINE void fnmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_product, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_product, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_product, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_product, status);
}

BX_CPP_INLINE void fnmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_product, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_product, status);
}

BX_CPP_INLINE void fnmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_result, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_result, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_result, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_result, status);
}

BX_CPP_INLINE void fnmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_result, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_result, status);
}

#endif
