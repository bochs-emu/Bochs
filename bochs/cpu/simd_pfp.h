/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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

BX_CPP_INLINE void xmm_addps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_add(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_add(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_add(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_addpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_add(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_add(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_subps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_sub(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_sub(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_sub(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_subpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_sub(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_mulps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_mul(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_mul(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_mul(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_mul(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_mulpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_mul(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_mul(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_divps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_div(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_div(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_div(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_div(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_divpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_div(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_div(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_addsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_sub(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_add(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_addsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_add(op1->xmm64u(1), op2->xmm64u(1), status);
}

// horizontal arithmetic add/sub

BX_CPP_INLINE void xmm_haddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_add(op1->xmm32u(0), op1->xmm32u(1), status);
  op1->xmm32u(1) = float32_add(op1->xmm32u(2), op1->xmm32u(3), status);
  op1->xmm32u(2) = float32_add(op2->xmm32u(0), op2->xmm32u(1), status);
  op1->xmm32u(3) = float32_add(op2->xmm32u(2), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_haddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_add(op1->xmm64u(0), op1->xmm64u(1), status);
  op1->xmm64u(1) = float64_add(op2->xmm64u(0), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_hsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_sub(op1->xmm32u(0), op1->xmm32u(1), status);
  op1->xmm32u(1) = float32_sub(op1->xmm32u(2), op1->xmm32u(3), status);
  op1->xmm32u(2) = float32_sub(op2->xmm32u(0), op2->xmm32u(1), status);
  op1->xmm32u(3) = float32_sub(op2->xmm32u(2), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_hsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_sub(op1->xmm64u(0), op1->xmm64u(1), status);
  op1->xmm64u(1) = float64_sub(op2->xmm64u(0), op2->xmm64u(1), status);
}

// min/max

BX_CPP_INLINE void xmm_minps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_min(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_min(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_min(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_min(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_minpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_min(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_min(op1->xmm64u(1), op2->xmm64u(1), status);
}

BX_CPP_INLINE void xmm_maxps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = float32_max(op1->xmm32u(0), op2->xmm32u(0), status);
  op1->xmm32u(1) = float32_max(op1->xmm32u(1), op2->xmm32u(1), status);
  op1->xmm32u(2) = float32_max(op1->xmm32u(2), op2->xmm32u(2), status);
  op1->xmm32u(3) = float32_max(op1->xmm32u(3), op2->xmm32u(3), status);
}

BX_CPP_INLINE void xmm_maxpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = float64_max(op1->xmm64u(0), op2->xmm64u(0), status);
  op1->xmm64u(1) = float64_max(op1->xmm64u(1), op2->xmm64u(1), status);
}

// fma

BX_CPP_INLINE void xmm_fmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), 0, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), 0, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), 0, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), 0, status);
}

BX_CPP_INLINE void xmm_fmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, status);
}

BX_CPP_INLINE void xmm_fmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_c, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_c, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_c, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_c, status);
}

BX_CPP_INLINE void xmm_fmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_c, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_c, status);
}

BX_CPP_INLINE void xmm_fmaddsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_c, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), 0, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_c, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), 0, status);
}

BX_CPP_INLINE void xmm_fmaddsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_c, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, status);
}

BX_CPP_INLINE void xmm_fmsubaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), 0, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_c, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), 0, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_c, status);
}

BX_CPP_INLINE void xmm_fmsubaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_c, status);
}

BX_CPP_INLINE void xmm_fnmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_product, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_product, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_product, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_product, status);
}

BX_CPP_INLINE void xmm_fnmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_product, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_product, status);
}

BX_CPP_INLINE void xmm_fnmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = float32_muladd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), float_muladd_negate_result, status);
  op1->xmm32u(1) = float32_muladd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), float_muladd_negate_result, status);
  op1->xmm32u(2) = float32_muladd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), float_muladd_negate_result, status);
  op1->xmm32u(3) = float32_muladd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), float_muladd_negate_result, status);
}

BX_CPP_INLINE void xmm_fnmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = float64_muladd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), float_muladd_negate_result, status);
  op1->xmm64u(1) = float64_muladd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), float_muladd_negate_result, status);
}

#endif
