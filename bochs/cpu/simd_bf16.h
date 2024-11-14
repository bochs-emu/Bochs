/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

#ifndef BX_SIMD_BF16_PFP_FUNCTIONS_H
#define BX_SIMD_BF16_PFP_FUNCTIONS_H

#include "avx/bf16.h"

// arithmetic add/sub/mul/div

BX_CPP_INLINE void xmm_addbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_add(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_addbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_add(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_subbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_sub(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_subbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_sub(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_mulbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_mul(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_mulbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_mul(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_divbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_div(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_divbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_div(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

// min/max

BX_CPP_INLINE void xmm_minbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_min(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_minbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_min(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_maxbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_max(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_maxbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_max(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

// fma

BX_CPP_INLINE void xmm_fmaddbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), 0);
  }
}

BX_CPP_INLINE void xmm_fmaddbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), 0);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_c);
  }
}

BX_CPP_INLINE void xmm_fmsubbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_c);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmaddbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_product);
  }
}

BX_CPP_INLINE void xmm_fnmaddbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_product);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmsubbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_result);
  }
}

BX_CPP_INLINE void xmm_fnmsubbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_result);
    else
      op1->xmm16u(n) = 0;
  }
}

// sqrt

BX_CPP_INLINE void xmm_sqrtbf16(BxPackedXmmRegister *op)
{
  for (unsigned n=0; n < 8; n++) {
    op->xmm16u(n) = bf16_sqrt(op->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_sqrtbf16_mask(BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm16u(n) = bf16_sqrt(op->xmm16u(n));
    else
      op->xmm16u(n) = 0;
  }
}

// getexp

BX_CPP_INLINE void xmm_getexpbf16(BxPackedXmmRegister *op)
{
  for (unsigned n=0; n < 8; n++) {
    op->xmm16u(n) = bf16_getExp(op->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_getexpbf16_mask(BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm16u(n) = bf16_getExp(op->xmm16u(n));
    else
      op->xmm16u(n) = 0;
  }
}

// scalef

BX_CPP_INLINE void xmm_scalefbf16(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = bf16_scalef(op1->xmm16u(n), op2->xmm16u(n));
  }
}

BX_CPP_INLINE void xmm_scalefbf16_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = bf16_scalef(op1->xmm16u(n), op2->xmm16u(n));
    else
      op1->xmm16u(n) = 0;
  }
}

#endif
