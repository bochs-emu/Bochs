/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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

/////////////////////////////////////////////////////////////////////////
//
// amx_top.cc
//
// ACE v1 Section 14: Tile Outer Product Instructions.
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_AMX

#include "amx.h"
#include "cpu/decoder/ia_opcodes.h"
#include "softfloat3e/include/softfloat.h"
#include "bf16.h"
#include "wide_int.h"

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

// Rank-2 BF16 outer product subtile: two BF16 pairs -> FP32 sum of products
// -> FP32 accumulate. Spec Section 14.3.5 "op2bf16_subtile". DAZ is applied
// per-operand via f32_denormal_to_zero() exactly where the spec calls for
// it (both BF16-widened inputs, and the accumulator srcdest, but not the
// products); prepare_ne_softfloat_status_helper(false) already gives RNE
// with FTZ=1 on the f32_add/f32_mul outputs.
static float32 op2bf16_subtile(float32 srcdest, Bit32u op1, Bit32u op2)
{
  float32 a0 = f32_denormal_to_zero(convert_bfloat16_to_fp32((bfloat16) op1));
  float32 a1 = f32_denormal_to_zero(convert_bfloat16_to_fp32((bfloat16)(op1 >> 16)));
  float32 b0 = f32_denormal_to_zero(convert_bfloat16_to_fp32((bfloat16) op2));
  float32 b1 = f32_denormal_to_zero(convert_bfloat16_to_fp32((bfloat16)(op2 >> 16)));

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);
  float32 p0 = f32_mul(a0, b0, &status);
  float32 p1 = f32_mul(a1, b1, &status);
  float32 sop = f32_add(p0, p1, &status);

  return f32_add(f32_denormal_to_zero(srcdest), sop, &status);
}

/* ==========================================================================
 * BF16 Rank-2 Outer Product (TOP2BF16PS). No block scaling, no masking or
 * zeroing support per spec Section 14.3.
 * ==========================================================================
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP2BF16PS_TnnnWdqHdq(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile_dst = i->dst();
  check_tile(i, tile_dst);

  const BxPackedAvxRegister &src1 = BX_READ_AVX_REG(i->src1()); // A (row)
  const BxPackedAvxRegister &src2 = BX_READ_AVX_REG(i->src2()); // B (col)

  AMX::TILE *tdst = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);

  for (unsigned row=0; row < 16; row++) {
    Bit32u op1 = src1.vmm32u(row);
    for (unsigned col=0; col < 16; col++) {
      Bit32u op2 = src2.vmm32u(col);
      tdst->row[row].vmm32u(col) = op2bf16_subtile(tdst->row[row].vmm32u(col), op1, op2);
    }
  }

  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * INT8 Byte Rank-4 Outer Products (TOP4B[U|S][U|S]D). No block scaling, no
 * masking or zeroing support per spec Section 14.4.
 * ==========================================================================
 */
void BX_CPP_AttrRegparmN(3) BX_CPU_C::top4b_execute(bxInstruction_c *i, bool a_signed, bool b_signed)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile_dst = i->dst();
  check_tile(i, tile_dst);

  const BxPackedAvxRegister &src1 = BX_READ_AVX_REG(i->src1()); // A (row)
  const BxPackedAvxRegister &src2 = BX_READ_AVX_REG(i->src2()); // B (col)

  AMX::TILE *tdst = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);

  for (unsigned row=0; row < 16; row++) {
    Bit32u op1 = src1.vmm32u(row);
    for (unsigned col=0; col < 16; col++) {
      Bit32u op2 = src2.vmm32u(col);
      Bit32s acc = 0;
      for (unsigned k=0; k < 4; k++) {
        Bit8u a_byte = (Bit8u)(op1 >> (k*8));
        Bit8u b_byte = (Bit8u)(op2 >> (k*8));
        Bit32s a = a_signed ? (Bit32s)(Bit8s) a_byte : (Bit32s) a_byte;
        Bit32s b = b_signed ? (Bit32s)(Bit8s) b_byte : (Bit32s) b_byte;
        acc += a * b;
      }
      tdst->row[row].vmm32u(col) += (Bit32u) acc;
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4BSSD_TnnnWdqHdq(bxInstruction_c *i) { top4b_execute(i, true, true); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4BSUD_TnnnWdqHdq(bxInstruction_c *i) { top4b_execute(i, true, false); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4BUSD_TnnnWdqHdq(bxInstruction_c *i) { top4b_execute(i, false, true); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4BUUD_TnnnWdqHdq(bxInstruction_c *i) { top4b_execute(i, false, false); }

/* ==========================================================================
 * MX INT8/FP8 Rank-4 Outer Products - shared fixed-point-to-FP32 conversion.
 * Spec Section 16.5: left-normalize a signed 128-bit fixed-point magnitude so
 * its J-bit lands at bit 126, giving fixed guard/round/sticky bit positions,
 * then round to nearest-even with FTZ on underflow. Built on the existing
 * Bit128u/Bit128s hi:lo pair and long_neg()/long_shl() from wide_int.h.
 * ==========================================================================
 */
static float32 convert_fixpoint128_scaled_to_fp32_ftz_rne(Bit128s x, int adjust)
{
  if (x.lo == 0 && x.hi == 0) return 0;

  bool sign = (x.hi < 0);

  Bit128u magnitude;
  magnitude.lo = x.lo;
  magnitude.hi = (Bit64u) x.hi;
  if (sign) long_neg((Bit128s*) &magnitude);

  // bit126 of the 128-bit value == bit62 of the hi half (126-64).
  int Jbit_position = 126;
  while (((magnitude.hi >> 62) & 1) == 0) {
    Jbit_position--;
    long_shl(&magnitude);
  }

  // bits[101:0] straddle both halves: all of lo, plus hi bits[37:0].
  bool sticky = (magnitude.lo != 0) || ((magnitude.hi & ((BX_CONST64(1) << 38) - 1)) != 0);
  bool Gbit = (bool)((magnitude.hi >> 38) & 1); // bit102 -> hi bit38
  bool Lbit = (bool)((magnitude.hi >> 39) & 1); // bit103 -> hi bit39
  bool RndAdd = Gbit && (Lbit || sticky);

  Bit32u Mantissa = (Bit32u)(magnitude.hi >> 39); // bits[126:103] -> 24 bits
  Bit32u RndMantissa = Mantissa + (RndAdd ? 1 : 0);
  bool Ovf = (RndMantissa >> 24) != 0; // rounding carry into the exponent

  int biased = 127 + Jbit_position + adjust + (Ovf ? 1 : 0);
  Bit32u sign_bit = sign ? 0x80000000u : 0u;

  if (biased > 254) return sign_bit | 0x7F800000u; // overflow -> +-Inf
  if (biased <= 0)  return sign_bit;                // FTZ -> +-0

  Bit32u frac = RndMantissa & 0x7FFFFF;
  return sign_bit | ((Bit32u) biased << 23) | frac;
}

/* ==========================================================================
 * MX INT8 Rank-4 Outer Product (TOP4MXBSSPS). OCP MX INT8 is always signed.
 * BSR-scaled (E8M0), FP32 accumulate. No masking or zeroing support per
 * spec Section 14.2.
 * ==========================================================================
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4MXBSSPS_TnnnWdqHdqIb(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile_dst = i->dst();
  check_tile(i, tile_dst);

  const BxPackedAvxRegister &src1 = BX_READ_AVX_REG(i->src1()); // A (row)
  const BxPackedAvxRegister &src2 = BX_READ_AVX_REG(i->src2()); // B (col)

  Bit8u imm8 = i->Ib();
  unsigned a_group = imm8 & 0x3;
  unsigned b_group = (imm8 >> 3) & 0x3;

  const AMX::SCALEDATA &bsr = BX_CPU_THIS_PTR amx->scaledata;

  Bit8u src1_scales[16], src2_scales[16];
  for (unsigned s=0; s < 16; s++) {
    src1_scales[s] = bsr.scale[1].vmmubyte(s*4 + a_group); // A = upper BSR half
    src2_scales[s] = bsr.scale[0].vmmubyte(s*4 + b_group); // B = lower BSR half
  }

  AMX::TILE *tdst = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);

  for (unsigned row=0; row < 16; row++) {
    Bit32u src1_quad = src1.vmm32u(row);
    Bit8u src1_scale = src1_scales[row];

    for (unsigned col=0; col < 16; col++) {
      Bit32u src2_quad = src2.vmm32u(col);
      Bit8u src2_scale = src2_scales[col];

      if (src1_scale == 0xFF || src2_scale == 0xFF) {
        // E8M0 NaN -> QNaN_Indefinite. Confirmed against SDE (-future-ag):
        // real hardware returns the positive-sign QNaN 0x7FC00000 here, not
        // Bochs's usual (negative-sign) default_nan convention.
        tdst->row[row].vmm32u(col) = 0x7FC00000u;
        continue;
      }

      // Sum of 4 int8*int8 products: max magnitude 4*127*127, far below
      // what a 64-bit accumulator can hold; sign-extend into 128 bits for
      // the shared converter (which does need the full width for the FP8
      // rank-4 case, where a single product can exceed 64 bits).
      Bit64s sop = 0;
      for (unsigned k=0; k < 4; k++) {
        Bit8s a = (Bit8s)(src1_quad >> (k*8));
        Bit8s b = (Bit8s)(src2_quad >> (k*8));
        sop += (Bit64s)((Bit32s) a * (Bit32s) b);
      }

      Bit128s sop128;
      sop128.lo = (Bit64u) sop;
      sop128.hi = (sop < 0) ? (Bit64s) BX_CONST64(-1) : (Bit64s) 0; // sign-extend

      int exp_adjust = -12 + (int) src1_scale + (int) src2_scale - 254;
      float32 sop_fp32 = convert_fixpoint128_scaled_to_fp32_ftz_rne(sop128, exp_adjust);

      // Accumulate into the FP32 tile element: DAZ=1 on srcdest only.
      softfloat_status_t status = prepare_ne_softfloat_status_helper(false);
      float32 srcdest = f32_denormal_to_zero(tdst->row[row].vmm32u(col));
      tdst->row[row].vmm32u(col) = f32_add(srcdest, sop_fp32, &status);
    }
  }

  BX_NEXT_INSTR(i);
}

/* ==========================================================================
 * MX FP8 Rank-4 Outer Products (TOP4MX[B,H,BH,HB]F8PS). BSR-scaled (E8M0),
 * FP32 accumulate. No masking or zeroing support per spec Section 14.1.
 * ==========================================================================
 */

// FP8 -> 64-bit fixed-point integer (exact bit-shift, no rounding). Spec
// Section 16.5: BF8 (E5M2) result = 2^16 * float_value; HF8 (E4M3) result =
// 2^9 * float_value. The alignment enables exact accumulation.
static Bit64s convert_bf8_to_fixpoint64(Bit8u fp8_byte)
{
  int sign = (fp8_byte & 0x80) >> 7;
  int exp  = (fp8_byte & 0x7C) >> 2;
  int frac = (fp8_byte & 0x03);
  int mant = exp ? (frac | 0x4) : frac; // set J-bit for normal numbers
  int e_count = exp ? (exp - 1) : 0;
  Bit64s magnitude = (Bit64s) mant << e_count;
  return sign ? -magnitude : magnitude;
}

static Bit64s convert_hf8_to_fixpoint64(Bit8u fp8_byte)
{
  int sign = (fp8_byte & 0x80) >> 7;
  int exp  = (fp8_byte & 0x78) >> 3;
  int frac = (fp8_byte & 0x07);
  int mant = exp ? (frac | 0x8) : frac; // set J-bit for normal numbers
  int e_count = exp ? (exp - 1) : 0;
  Bit64s magnitude = (Bit64s) mant << e_count;
  return sign ? -magnitude : magnitude;
}

// Rank-4 MX FP8 outer product subtile. A single fixed-point product of two
// FP8-derived Bit64s values can exceed 64 bits (e.g. BF8 magnitude up to
// 7<<30, squared ~2^66), so the multiply goes through long_imul() into a
// genuine 128-bit product, accumulated via long_add(). Spec Section 14.1.6
// "op4mxf8_subtile".
static float32 op4mxf8_subtile(float32 srcdest, Bit32u src1_quad, bool a_is_bf8, Bit8u src1_scale,
                                                 Bit32u src2_quad, bool b_is_bf8, Bit8u src2_scale)
{
  if (src1_scale == 0xFF || src2_scale == 0xFF)
    return 0x7FC00000u; // E8M0 NaN -> QNaN_Indefinite (positive sign, confirmed via SDE)

  Bit128s sop;
  sop.lo = 0;
  sop.hi = 0;

  for (unsigned k=0; k < 4; k++) {
    Bit8u byte1 = (Bit8u)(src1_quad >> (k*8));
    Bit8u byte2 = (Bit8u)(src2_quad >> (k*8));
    Bit64s s1ei = a_is_bf8 ? convert_bf8_to_fixpoint64(byte1) : convert_hf8_to_fixpoint64(byte1);
    Bit64s s2ei = b_is_bf8 ? convert_bf8_to_fixpoint64(byte2) : convert_hf8_to_fixpoint64(byte2);

    Bit128s product;
    long_imul(&product, s1ei, s2ei);
    long_add((Bit128u*) &sop, (Bit128u*) &product);
  }

  // Combined exponent adjustment: fp8 fixpoint correction + E8M0 scale shifts.
  int factor = (a_is_bf8 && b_is_bf8) ? 32 : (!a_is_bf8 && !b_is_bf8) ? 18 : 25;
  int exp_adjust = -factor + (int) src1_scale + (int) src2_scale - 254;

  float32 sop_fp32 = convert_fixpoint128_scaled_to_fp32_ftz_rne(sop, exp_adjust);

  // Accumulate into the FP32 tile element: DAZ=1 on srcdest only.
  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);
  return f32_add(f32_denormal_to_zero(srcdest), sop_fp32, &status);
}

void BX_CPP_AttrRegparmN(3) BX_CPU_C::top4mxf8ps_execute(bxInstruction_c *i, bool a_is_bf8, bool b_is_bf8)
{
  if (BX_CPU_THIS_PTR amx->get_palette_id() != 2) {
    BX_ERROR(("%s: not supported under pallette %d", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR amx->get_palette_id()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned tile_dst = i->dst();
  check_tile(i, tile_dst);

  const BxPackedAvxRegister &src1 = BX_READ_AVX_REG(i->src1()); // A (row)
  const BxPackedAvxRegister &src2 = BX_READ_AVX_REG(i->src2()); // B (col)

  Bit8u imm8 = i->Ib();
  unsigned a_group = imm8 & 0x3;
  unsigned b_group = (imm8 >> 3) & 0x3;

  const AMX::SCALEDATA &bsr = BX_CPU_THIS_PTR amx->scaledata;

  Bit8u src1_scales[16], src2_scales[16];
  for (unsigned s=0; s < 16; s++) {
    src1_scales[s] = bsr.scale[1].vmmubyte(s*4 + a_group); // A = upper BSR half
    src2_scales[s] = bsr.scale[0].vmmubyte(s*4 + b_group); // B = lower BSR half
  }

  AMX::TILE *tdst = &(BX_CPU_THIS_PTR amx->tile[tile_dst]);

  for (unsigned row=0; row < 16; row++) {
    Bit32u src1_quad = src1.vmm32u(row);
    Bit8u src1_scale = src1_scales[row];

    for (unsigned col=0; col < 16; col++) {
      Bit32u src2_quad = src2.vmm32u(col);
      Bit8u src2_scale = src2_scales[col];

      tdst->row[row].vmm32u(col) = op4mxf8_subtile(tdst->row[row].vmm32u(col),
                                                     src1_quad, a_is_bf8, src1_scale,
                                                     src2_quad, b_is_bf8, src2_scale);
    }
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4MXBF8PS_TnnnWdqHdqIb(bxInstruction_c *i)  { top4mxf8ps_execute(i, true, true); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4MXBHF8PS_TnnnWdqHdqIb(bxInstruction_c *i) { top4mxf8ps_execute(i, true, false); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4MXHBF8PS_TnnnWdqHdqIb(bxInstruction_c *i) { top4mxf8ps_execute(i, false, true); }
void BX_CPP_AttrRegparmN(1) BX_CPU_C::TOP4MXHF8PS_TnnnWdqHdqIb(bxInstruction_c *i)  { top4mxf8ps_execute(i, false, false); }

#endif // BX_SUPPORT_AMX
