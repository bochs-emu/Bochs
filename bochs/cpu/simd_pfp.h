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
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_add(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_addps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_add(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_addpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_add(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_addpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_add(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_addph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_add(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_addph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_add(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_subps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_sub(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_subps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_sub(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_subpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_sub(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_subpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_sub(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_subph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_sub(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_subph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_sub(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_mulps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_mul(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_mulps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mul(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_mulpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_mul(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_mulpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_mul(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_mulph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_mul(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_mulph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mul(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_divps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_div(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_divps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_div(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_divpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_div(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_divpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_div(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_divph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_div(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_divph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_div(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_addsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = f32_sub(op1->xmm32u(0), op2->xmm32u(0), &status);
  op1->xmm32u(1) = f32_add(op1->xmm32u(1), op2->xmm32u(1), &status);
  op1->xmm32u(2) = f32_sub(op1->xmm32u(2), op2->xmm32u(2), &status);
  op1->xmm32u(3) = f32_add(op1->xmm32u(3), op2->xmm32u(3), &status);
}

BX_CPP_INLINE void xmm_addsubps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm32u(0) = f32_sub(op1->xmm32u(0), op2->xmm32u(0), &status);
  else
    op1->xmm32u(0) = 0;

  if (mask & 0x2)
    op1->xmm32u(1) = f32_add(op1->xmm32u(1), op2->xmm32u(1), &status);
  else
    op1->xmm32u(1) = 0;

  if (mask & 0x4)
    op1->xmm32u(2) = f32_sub(op1->xmm32u(2), op2->xmm32u(2), &status);
  else
    op1->xmm32u(2) = 0;

  if (mask & 0x8)
    op1->xmm32u(3) = f32_add(op1->xmm32u(3), op2->xmm32u(3), &status);
  else
    op1->xmm32u(3) = 0;
}

BX_CPP_INLINE void xmm_addsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = f64_sub(op1->xmm64u(0), op2->xmm64u(0), &status);
  op1->xmm64u(1) = f64_add(op1->xmm64u(1), op2->xmm64u(1), &status);
}

BX_CPP_INLINE void xmm_addsubpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm64u(0) = f64_sub(op1->xmm64u(0), op2->xmm64u(0), &status);
  else
    op1->xmm64u(0) = 0;

  if (mask & 0x2)
    op1->xmm64u(1) = f64_add(op1->xmm64u(1), op2->xmm64u(1), &status);
  else
    op1->xmm64u(1) = 0;
}

// horizontal arithmetic add/sub

BX_CPP_INLINE void xmm_haddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = f32_add(op1->xmm32u(0), op1->xmm32u(1), &status);
  op1->xmm32u(1) = f32_add(op1->xmm32u(2), op1->xmm32u(3), &status);
  op1->xmm32u(2) = f32_add(op2->xmm32u(0), op2->xmm32u(1), &status);
  op1->xmm32u(3) = f32_add(op2->xmm32u(2), op2->xmm32u(3), &status);
}

BX_CPP_INLINE void xmm_haddps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm32u(0) = f32_add(op1->xmm32u(0), op1->xmm32u(1), &status);
  else
    op1->xmm32u(0) = 0;

  if (mask & 0x2)
    op1->xmm32u(1) = f32_add(op1->xmm32u(2), op1->xmm32u(3), &status);
  else
    op1->xmm32u(1) = 0;

  if (mask & 0x4)
    op1->xmm32u(2) = f32_add(op2->xmm32u(0), op2->xmm32u(1), &status);
  else
    op1->xmm32u(2) = 0;

  if (mask & 0x8)
    op1->xmm32u(3) = f32_add(op2->xmm32u(2), op2->xmm32u(3), &status);
  else
    op1->xmm32u(3) = 0;
}

BX_CPP_INLINE void xmm_haddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = f64_add(op1->xmm64u(0), op1->xmm64u(1), &status);
  op1->xmm64u(1) = f64_add(op2->xmm64u(0), op2->xmm64u(1), &status);
}

BX_CPP_INLINE void xmm_haddpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm64u(0) = f64_add(op1->xmm64u(0), op1->xmm64u(1), &status);
  else
    op1->xmm64u(0) = 0;

  if (mask & 0x2)
    op1->xmm64u(1) = f64_add(op2->xmm64u(0), op2->xmm64u(1), &status);
  else
    op1->xmm64u(1) = 0;
}

BX_CPP_INLINE void xmm_hsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm32u(0) = f32_sub(op1->xmm32u(0), op1->xmm32u(1), &status);
  op1->xmm32u(1) = f32_sub(op1->xmm32u(2), op1->xmm32u(3), &status);
  op1->xmm32u(2) = f32_sub(op2->xmm32u(0), op2->xmm32u(1), &status);
  op1->xmm32u(3) = f32_sub(op2->xmm32u(2), op2->xmm32u(3), &status);
}

BX_CPP_INLINE void xmm_hsubps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm32u(0) = f32_sub(op1->xmm32u(0), op1->xmm32u(1), &status);
  else
    op1->xmm32u(0) = 0;

  if (mask & 0x2)
    op1->xmm32u(1) = f32_sub(op1->xmm32u(2), op1->xmm32u(3), &status);
  else
    op1->xmm32u(1) = 0;

  if (mask & 0x4)
    op1->xmm32u(2) = f32_sub(op2->xmm32u(0), op2->xmm32u(1), &status);
  else
    op1->xmm32u(2) = 0;

  if (mask & 0x8)
    op1->xmm32u(3) = f32_sub(op2->xmm32u(2), op2->xmm32u(3), &status);
  else
    op1->xmm32u(3) = 0;
}

BX_CPP_INLINE void xmm_hsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  op1->xmm64u(0) = f64_sub(op1->xmm64u(0), op1->xmm64u(1), &status);
  op1->xmm64u(1) = f64_sub(op2->xmm64u(0), op2->xmm64u(1), &status);
}

BX_CPP_INLINE void xmm_hsubpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm64u(0) = f64_sub(op1->xmm64u(0), op1->xmm64u(1), &status);
  else
    op1->xmm64u(0) = 0;

  if (mask & 0x2)
    op1->xmm64u(1) = f64_sub(op2->xmm64u(0), op2->xmm64u(1), &status);
  else
    op1->xmm64u(1) = 0;
}

// min/max

BX_CPP_INLINE void xmm_minps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_min(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_minps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_min(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_minpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_min(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_minpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_min(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_minph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_min(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_minph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_min(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_maxps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_max(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_maxps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_max(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_maxpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_max(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_maxpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_max(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_maxph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_max(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_maxph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_max(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

// fma

BX_CPP_INLINE void xmm_fmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), 0, &status);
  }
}

BX_CPP_INLINE void xmm_fmaddps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), 0, &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), 0, &status);
  }
}

BX_CPP_INLINE void xmm_fmaddpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), 0, &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmaddph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), 0, &status);
  }
}

BX_CPP_INLINE void xmm_fmaddph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), 0, &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_c, &status);
  }
}

BX_CPP_INLINE void xmm_fmsubps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_c, &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_c, &status);
  }
}

BX_CPP_INLINE void xmm_fmsubpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_c, &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_c, &status);
  }
}

BX_CPP_INLINE void xmm_fmsubph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_c, &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fmaddsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = f32_mulAdd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), softfloat_muladd_negate_c, &status);
  op1->xmm32u(1) = f32_mulAdd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), 0, &status);
  op1->xmm32u(2) = f32_mulAdd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), softfloat_muladd_negate_c, &status);
  op1->xmm32u(3) = f32_mulAdd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), 0, &status);
}

BX_CPP_INLINE void xmm_fmaddsubps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n+=2, mask >>= 2) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_c, &status);
    else
      op1->xmm32u(n) = 0;

    if (mask & 0x2)
      op1->xmm32u(n+1) = f32_mulAdd(op1->xmm32u(n+1), op2->xmm32u(n+1), op3->xmm32u(n+1), 0, &status);
    else
      op1->xmm32u(n+1) = 0;
  }
}

BX_CPP_INLINE void xmm_fmaddsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = f64_mulAdd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), softfloat_muladd_negate_c, &status);
  op1->xmm64u(1) = f64_mulAdd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, &status);
}

BX_CPP_INLINE void xmm_fmaddsubpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm64u(0) = f64_mulAdd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), softfloat_muladd_negate_c, &status);
  else
    op1->xmm64u(0) = 0;

  if (mask & 0x2)
    op1->xmm64u(1) = f64_mulAdd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), 0, &status);
  else
    op1->xmm64u(1) = 0;
}

BX_CPP_INLINE void xmm_fmaddsubph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n+=2) {
    op1->xmm16u(n)   = f16_mulAdd(op1->xmm16u(n),   op2->xmm16u(n),   op3->xmm16u(n),   softfloat_muladd_negate_c, &status);
    op1->xmm16u(n+1) = f16_mulAdd(op1->xmm16u(n+1), op2->xmm16u(n+1), op3->xmm16u(n+1), 0, &status);
  }
}

BX_CPP_INLINE void xmm_fmaddsubph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n+=2, mask >>= 2) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_c, &status);
    else
      op1->xmm16u(n) = 0;

    if (mask & 0x2)
      op1->xmm16u(n+1) = f16_mulAdd(op1->xmm16u(n+1), op2->xmm16u(n+1), op3->xmm16u(n+1), 0, &status);
    else
      op1->xmm16u(n+1) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm32u(0) = f32_mulAdd(op1->xmm32u(0), op2->xmm32u(0), op3->xmm32u(0), 0, &status);
  op1->xmm32u(1) = f32_mulAdd(op1->xmm32u(1), op2->xmm32u(1), op3->xmm32u(1), softfloat_muladd_negate_c, &status);
  op1->xmm32u(2) = f32_mulAdd(op1->xmm32u(2), op2->xmm32u(2), op3->xmm32u(2), 0, &status);
  op1->xmm32u(3) = f32_mulAdd(op1->xmm32u(3), op2->xmm32u(3), op3->xmm32u(3), softfloat_muladd_negate_c, &status);
}

BX_CPP_INLINE void xmm_fmsubaddps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n+=2, mask >>= 2) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), 0, &status);
    else
      op1->xmm32u(n) = 0;

    if (mask & 0x2)
      op1->xmm32u(n+1) = f32_mulAdd(op1->xmm32u(n+1), op2->xmm32u(n+1), op3->xmm32u(n+1), softfloat_muladd_negate_c, &status);
    else
      op1->xmm32u(n+1) = 0;
  }
}

BX_CPP_INLINE void xmm_fmsubaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  op1->xmm64u(0) = f64_mulAdd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, &status);
  op1->xmm64u(1) = f64_mulAdd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), softfloat_muladd_negate_c, &status);
}

BX_CPP_INLINE void xmm_fmsubaddpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  if (mask & 0x1)
    op1->xmm64u(0) = f64_mulAdd(op1->xmm64u(0), op2->xmm64u(0), op3->xmm64u(0), 0, &status);
  else
    op1->xmm64u(0) = 0;

  if (mask & 0x2)
    op1->xmm64u(1) = f64_mulAdd(op1->xmm64u(1), op2->xmm64u(1), op3->xmm64u(1), softfloat_muladd_negate_c, &status);
  else
    op1->xmm64u(1) = 0;
}

BX_CPP_INLINE void xmm_fmsubaddph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n+=2) {
    op1->xmm16u(n)   = f16_mulAdd(op1->xmm16u(n),   op2->xmm16u(n),   op3->xmm16u(n),   0, &status);
    op1->xmm16u(n+1) = f16_mulAdd(op1->xmm16u(n+1), op2->xmm16u(n+1), op3->xmm16u(n+1), softfloat_muladd_negate_c, &status);
  }
}

BX_CPP_INLINE void xmm_fmsubaddph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n+=2, mask >>= 2) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), 0, &status);
    else
      op1->xmm16u(n) = 0;

    if (mask & 0x2)
      op1->xmm16u(n+1) = f16_mulAdd(op1->xmm16u(n+1), op2->xmm16u(n+1), op3->xmm16u(n+1), softfloat_muladd_negate_c, &status);
    else
      op1->xmm16u(n+1) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmaddps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_product, &status);
  }
}

BX_CPP_INLINE void xmm_fnmaddps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_product, &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmaddpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_product, &status);
  }
}

BX_CPP_INLINE void xmm_fnmaddpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_product, &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmaddph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_product, &status);
  }
}

BX_CPP_INLINE void xmm_fnmaddph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_product, &status);
    else
      op1->xmm16u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmsubps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_result, &status);
  }
}

BX_CPP_INLINE void xmm_fnmsubps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_mulAdd(op1->xmm32u(n), op2->xmm32u(n), op3->xmm32u(n), softfloat_muladd_negate_result, &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmsubpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_result, &status);
  }
}

BX_CPP_INLINE void xmm_fnmsubpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_mulAdd(op1->xmm64u(n), op2->xmm64u(n), op3->xmm64u(n), softfloat_muladd_negate_result, &status);
    else
      op1->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_fnmsubph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_result, &status);
  }
}

BX_CPP_INLINE void xmm_fnmsubph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_mulAdd(op1->xmm16u(n), op2->xmm16u(n), op3->xmm16u(n), softfloat_muladd_negate_result, &status);
    else
      op1->xmm16u(n) = 0;
  }
}

// sqrt

BX_CPP_INLINE void xmm_sqrtps(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 4; n++) {
    op->xmm32u(n) = f32_sqrt(op->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_sqrtps_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm32u(n) = f32_sqrt(op->xmm32u(n), &status);
    else
      op->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_sqrtpd(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 2; n++) {
    op->xmm64u(n) = f64_sqrt(op->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_sqrtpd_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm64u(n) = f64_sqrt(op->xmm64u(n), &status);
    else
      op->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_sqrtph(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 8; n++) {
    op->xmm16u(n) = f16_sqrt(op->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_sqrtph_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm16u(n) = f16_sqrt(op->xmm16u(n), &status);
    else
      op->xmm16u(n) = 0;
  }
}

// getexp

BX_CPP_INLINE void xmm_getexpps(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 4; n++) {
    op->xmm32u(n) = f32_getExp(op->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_getexpps_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm32u(n) = f32_getExp(op->xmm32u(n), &status);
    else
      op->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_getexppd(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 2; n++) {
    op->xmm64u(n) = f64_getExp(op->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_getexppd_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm64u(n) = f64_getExp(op->xmm64u(n), &status);
    else
      op->xmm64u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_getexpph(BxPackedXmmRegister *op, float_status_t &status)
{
  for (unsigned n=0; n < 8; n++) {
    op->xmm16u(n) = f16_getExp(op->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_getexpph_mask(BxPackedXmmRegister *op, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op->xmm16u(n) = f16_getExp(op->xmm16u(n), &status);
    else
      op->xmm16u(n) = 0;
  }
}

// scalef

BX_CPP_INLINE void xmm_scalefps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<4;n++) {
    op1->xmm32u(n) = f32_scalef(op1->xmm32u(n), op2->xmm32u(n), &status);
  }
}

BX_CPP_INLINE void xmm_scalefps_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm32u(n) = f32_scalef(op1->xmm32u(n), op2->xmm32u(n), &status);
    else
      op1->xmm32u(n) = 0;
  }
}

BX_CPP_INLINE void xmm_scalefpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<2;n++) {
    op1->xmm64u(n) = f64_scalef(op1->xmm64u(n), op2->xmm64u(n), &status);
  }
}

BX_CPP_INLINE void xmm_scalefpd_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm64u(n) = f64_scalef(op1->xmm64u(n), op2->xmm64u(n), &status);
    else
      op1->xmm64u(n) = 0;
  }
}

#include "fpu/softfloat16.h"

BX_CPP_INLINE void xmm_scalefph(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status)
{
  for (unsigned n=0;n<8;n++) {
    op1->xmm16u(n) = f16_scalef(op1->xmm16u(n), op2->xmm16u(n), &status);
  }
}

BX_CPP_INLINE void xmm_scalefph_mask(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, float_status_t &status, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1)
      op1->xmm16u(n) = f16_scalef(op1->xmm16u(n), op2->xmm16u(n), &status);
    else
      op1->xmm16u(n) = 0;
  }
}

#endif
