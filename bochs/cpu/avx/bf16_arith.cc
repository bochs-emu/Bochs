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

#include "config.h"
#include "softfloat3e/include/softfloat.h"
#include "bf16.h"

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

// Denormal BF16 input operands are treated as zeros (DAZ) and denormal BF16 outputs are flushed to zero (FTZ)
// Rounding Mode is always RNE
static softfloat_status_t tmp_status = prepare_ne_softfloat_status_helper(true);

bfloat16 bf16_add(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_add(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_sub(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_sub(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_mul(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_mul(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_div(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_div(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_min(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_min(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_max(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_max(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_scalef(bfloat16 a, bfloat16 b) {
  return convert_ne_fp32_to_bfloat16(f32_scalef(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), &tmp_status));
}

bfloat16 bf16_sqrt(bfloat16 a) {
  return convert_ne_fp32_to_bfloat16(f32_sqrt(convert_bfloat16_to_fp32(a), &tmp_status));
}

int bf16_compare(bfloat16 a, bfloat16 b) {
  return f32_compare(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), true, &tmp_status);
}

bfloat16 bf16_mulAdd(bfloat16 a, bfloat16 b, bfloat16 c, uint8_t op) {
  return convert_ne_fp32_to_bfloat16(f32_mulAdd(convert_bfloat16_to_fp32(a), convert_bfloat16_to_fp32(b), convert_bfloat16_to_fp32(c), op, &tmp_status));
}

bfloat16 bf16_getExp(bfloat16 a) {
  return convert_ne_fp32_to_bfloat16(f32_getExp(convert_bfloat16_to_fp32(a), &tmp_status));
}

bfloat16 bf16_getMant(bfloat16 a, int sign_ctrl, int interv) {
  return convert_ne_fp32_to_bfloat16(f32_getMant(convert_bfloat16_to_fp32(a), &tmp_status, sign_ctrl, interv));
}

bfloat16 bf16_roundToInt(bfloat16 a, uint8_t scale)
{
  return convert_ne_fp32_to_bfloat16(f32_roundToInt(convert_bfloat16_to_fp32(a), scale, softfloat_round_near_even, false, &tmp_status));
}

int8_t bf16_to_i8_saturate(bfloat16 a) {
  return f32_to_i8_saturate(convert_bfloat16_to_fp32(a), &tmp_status);
}
int8_t bf16_to_i8_round_to_zero_saturate(bfloat16 a) {
  return f32_to_i8_round_to_zero_saturate(convert_bfloat16_to_fp32(a), &tmp_status);
}

uint8_t bf16_to_ui8_saturate(bfloat16 a) {
  return f32_to_ui8_saturate(convert_bfloat16_to_fp32(a), &tmp_status);
}
uint8_t bf16_to_ui8_round_to_zero_saturate(bfloat16 a) {
  return f32_to_ui8_round_to_zero_saturate(convert_bfloat16_to_fp32(a), &tmp_status);
}

extern float32 approximate_rcp14(float32 op, const softfloat_status_t &status);

bfloat16 bfloat16_approximate_rcp14(bfloat16 a) {
  float32 f32 = approximate_rcp14(convert_bfloat16_to_fp32(a), tmp_status);
  return convert_ne_fp32_to_bfloat16(f32);
}

extern float32 approximate_rsqrt14(float32 op, bool daz);

bfloat16 bfloat16_approximate_rsqrt14(bfloat16 a) {
  float32 f32 = approximate_rsqrt14(convert_bfloat16_to_fp32(a), true);
  return convert_ne_fp32_to_bfloat16(f32);
}
