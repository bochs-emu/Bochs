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
// fp8.cc
//
// Conversions between FP16/FP32 and the OCP 8/6/4-bit floating point
// formats (E5M2/BF8, E4M3/HF8, E3M2/BF6, E2M3/HF6, E2M1/BF4). Declarations
// and the trivial bit-field accessors live in fp8.h; this file has the
// actual conversion logic.
//
/////////////////////////////////////////////////////////////////////////

#include "softfloat3e/include/softfloat.h"
#include "fp8.h"

/*----------------------------------------------------------------------------
 * BF8 (E5M2)
 *----------------------------------------------------------------------------*/

float16 convert_bf8_to_fp16(float_bf8 op)
{
  return Bit16u(op) << 8;
}

// Renormalize a confirmed-nonzero FP16 denormal significand into a virtual
// (exponent, 10-bit fraction) pair, so the exponent-rebias-and-round logic
// in the conversion functions below can treat it uniformly with a normal
// FP16 input (BF8's denormal range overlaps FP16's, unlike HF8's, so this
// case is reachable and must be handled exactly, not just flushed to zero).
// Mirrors softfloat_normSubnormalF16Sig(). Only call this when the input is
// actually a denormal (exp == 0 && sig != 0); it does nothing else.
struct bf8_fp16_denorm_t { int exp; int frac; };

static bf8_fp16_denorm_t bf8_renormalize_fp16_denormal(Bit16u sigA)
{
  int shiftDist = 0;
  Bit16u mant = sigA;
  while (! (mant & 0x400)) {
    mant <<= 1;
    shiftDist++;
  }

  bf8_fp16_denorm_t result;
  result.frac = mant & 0x3FF;
  result.exp  = 1 - shiftDist; // (-14 - shiftDist) + 15
  return result;
}

// Convert half precision floating point number (fp16) to E5M2 BF8 number
// using bias rounding: add an 8-bit unsigned BIAS number to the renormalized
// mantissa and then truncate (no rounding) to the 2-bit E5M2 mantissa.
//
// Infinity is preserved for the non-saturated versions. NaN at input is propagated as QNaN.
// If the result is infinity or too big to be represented, then for the saturated version, E5M2_MAX is returned,
// for the non-saturated versions, infinity is returned.
//
// DAZ is not obeyed and always assumed DAZ==0. FTZ is not obeyed and always assumed FTZ==0.
// All MXCSR mask bits DM, IM, OM, PM, UM are implicitly set.

float_bf8 convert_truncate_fp16_to_bf8_bias(float16 a, Bit8u bias, bool saturate_overflow)
{
  int signA   = f16_sign(a);
  Bit8s expA  = f16_exp(a);
  Bit16u sigA = f16_fraction(a);

  if (expA == 0x1F) {
    if (! sigA) {
      // a is infinity
      if (saturate_overflow)
        return pack_float_bf8(signA, 0x1E, 0x3); // max signif value
      else
        return (float_bf8) (a >> 8);              // keep infinity
    }

    return (float_bf8) ((a >> 8) | 0x02); // make it QNaN
  }

  int e_n = expA, m_n = sigA;
  if (! expA) {
    if (! sigA) return pack_float_bf8(signA, 0, 0); // zero
    bf8_fp16_denorm_t n = bf8_renormalize_fp16_denormal(sigA);
    e_n = n.exp;
    m_n = n.frac;
  }

  int e_b = e_n;
  int m_b = m_n + bias;
  if (m_b & 0xFC00) e_b++; // carry out of the 10-bit mantissa
  m_b &= 0x3FF;

  if (e_b >= 0x1F) {
    if (saturate_overflow) return pack_float_bf8(signA, 0x1E, 0x3);
    else                   return pack_float_bf8(signA, 0x1F, 0x0);
  }

  int e_o, m_o;
  if (e_b <= 0) {
    // Underflow into the BF8 denormal range or all the way to zero. When the
    // shift needed exceeds the width of the (J-bit + mantissa) field, the
    // result is exactly zero regardless of how large bias is - an 8-bit
    // bias term can never be large enough to rescue a value this far below
    // the smallest denormal.
    e_o = m_o = 0;
    if ((9 - e_b) <= 11) {
      int mant_u = m_b | 0x400;
      m_o = mant_u >> (9 - e_b);
    }
  }
  else {
    e_o = e_b;
    m_o = m_b >> 8;
  }

  return pack_float_bf8(signA, e_o & 0x1F, m_o & 0x3);
}

// convert half precision floating point number (fp16) to E5M2 BF8 number,
// using round-to-nearest-even (RNE) - independent from the bias/SR path
// above since RNE's tie-breaking rounding step differs from SR's plain
// truncation.
float_bf8 convert_ne_fp16_to_bf8(float16 a, bool saturate_overflow)
{
  int signA   = f16_sign(a);
  Bit8s expA  = f16_exp(a);
  Bit16u sigA = f16_fraction(a);

  if (expA == 0x1F) {
    if (! sigA) {
      if (saturate_overflow)
        return pack_float_bf8(signA, 0x1E, 0x3);
      else
        return (float_bf8) (a >> 8);
    }

    return (float_bf8) ((a >> 8) | 0x02);
  }

  int e_n = expA, m_n = sigA;
  if (! expA) {
    if (! sigA) return pack_float_bf8(signA, 0, 0); // zero
    bf8_fp16_denorm_t n = bf8_renormalize_fp16_denormal(sigA);
    e_n = n.exp;
    m_n = n.frac;
  }

  if (e_n >= 31) {
    if (saturate_overflow) return pack_float_bf8(signA, 0x1E, 0x3);
    else                   return pack_float_bf8(signA, 0x1F, 0x0);
  }

  int e_o, m_o;
  if (e_n <= 0) {
    e_o = m_o = 0;
    if ((9 - e_n) <= 11) {
      int shift = 9 - e_n;
      int mant_u = m_n | 0x400;
      m_o = mant_u >> shift;
      int lowmant = mant_u & ((1 << shift) - 1);
      int halfway = 1 << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 1))) {
        m_o++;
        if ((m_o & 0x3) == 0) e_o++;
      }
    }
  }
  else {
    e_o = e_n;
    m_o = m_n >> 8;
    int lowmant = m_n & 0xFF;
    if (lowmant > 0x80 || (lowmant == 0x80 && (m_o & 1))) {
      if (! (saturate_overflow && e_o == 0x1E && m_o == 0x3)) {
        m_o++;
        if ((m_o & 0x3) == 0) e_o++;
      }
    }
    if (e_o >= 0x1F) {
      if (saturate_overflow) return pack_float_bf8(signA, 0x1E, 0x3);
      else                   return pack_float_bf8(signA, 0x1F, 0x0);
    }
  }

  return pack_float_bf8(signA, e_o & 0x1F, m_o & 0x3);
}

// Convert single precision floating point number (fp32) to E5M2 BF8 number
// using round-to-nearest-even (RNE) - the only rounding mode FP32->E5M2
// supports (round-to-odd applies to E4M3/HF8 only, per spec Section 9.2).
float_bf8 convert_ne_fp32_to_bf8(float32 a, bool saturate_overflow)
{
  bool sign   = f32_sign(a);
  Bit16s expA = f32_exp(a);
  Bit32u sigA = f32_fraction(a);

  if (expA == 0xFF) { // Inf or NaN
    if (saturate_overflow) {
      if (! sigA) return pack_float_bf8(sign, 0x1E, 0x3); // clamp to max_normal
      return pack_float_bf8(sign, 0x1F, 0x2 | ((sigA >> 21) & 0x1));
    }
    return pack_float_bf8(sign, 0x1F, sigA ? (0x2 | ((sigA >> 21) & 0x1)) : 0x0);
  }
  if (! expA) return pack_float_bf8(sign, 0, 0); // zero or denormal

  int newexp = int(expA) - 127 + 15;

  if (newexp >= 31) { // overflow
    if (saturate_overflow) return pack_float_bf8(sign, 0x1E, 0x3);
    else                   return pack_float_bf8(sign, 0x1F, 0x0);
  }

  int e_o = 0, m_o = 0;
  if (newexp <= 0) { // underflow -> subnormal or zero
    if ((22 - newexp) <= 24) {
      Bit32u mant = sigA | 0x800000;
      int shift = 22 - newexp;
      m_o = mant >> shift;
      Bit32u lowmant = mant & ((Bit32u(1) << shift) - 1);
      Bit32u halfway = Bit32u(1) << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
        m_o++;
        if ((m_o & 0x3) == 0) e_o++;
      }
    }
  }
  else { // normal
    e_o = newexp;
    m_o = sigA >> 21;
    if (sigA & 0x100000) {
      if (((sigA & 0x1FFFFF) > 0x100000) || (m_o & 0x1)) {
        if (! (saturate_overflow && e_o == 0x1E && m_o == 0x3)) {
          m_o++;
          if ((m_o & 0x3) == 0) e_o++;
        }
      }
    }
  }

  return pack_float_bf8(sign, e_o & 0x1F, m_o & 0x3);
}

// Convert single precision floating point number (fp32) to E5M2 BF8 number
// using bias rounding (see convert_truncate_fp16_to_bf8_bias above for the
// general bias-rounding rationale). The bias operand covers the full 21-bit
// truncated mantissa range (23 source bits - 2 target bits).
float_bf8 convert_truncate_fp32_to_bf8_bias(float32 a, Bit32u bias, bool saturate_overflow)
{
  bool sign   = f32_sign(a);
  Bit16s expA = f32_exp(a);
  Bit32u sigA = f32_fraction(a);

  if (expA == 0xFF) { // Inf or NaN
    if (saturate_overflow) {
      if (! sigA) return pack_float_bf8(sign, 0x1E, 0x3);
      return pack_float_bf8(sign, 0x1F, 0x2 | ((sigA >> 21) & 0x1));
    }
    return pack_float_bf8(sign, 0x1F, sigA ? (0x2 | ((sigA >> 21) & 0x1)) : 0x0);
  }
  if (! expA) return pack_float_bf8(sign, 0, 0); // zero or denormal

  Bit32u e_b = expA;
  Bit32u m_b = sigA + (bias & 0x1FFFFF);
  if (m_b & 0xFF800000) e_b++; // carry out of the 23-bit mantissa
  m_b &= 0x7FFFFF;

  int newexp = int(e_b) - 127 + 15;

  if (newexp >= 31) {
    if (saturate_overflow) return pack_float_bf8(sign, 0x1E, 0x3);
    else                   return pack_float_bf8(sign, 0x1F, 0x0);
  }

  int e_o = 0, m_o = 0;
  if (newexp <= 0) { // underflow into the BF8 denormal range or to zero
    if ((22 - newexp) <= 24) {
      Bit32u mant = m_b | 0x800000;
      m_o = mant >> (22 - newexp);
    }
  }
  else {
    e_o = newexp;
    m_o = m_b >> 21;
  }

  return pack_float_bf8(sign, e_o & 0x1F, m_o & 0x3);
}

// Convert E5M2 BF8 number to single precision floating point value (fp32).
// All BF8 encodings map precisely to FP32; no rounding is needed.
float32 convert_bf8_to_fp32(float_bf8 a)
{
  int sign = bf8_sign(a);
  int exp  = bf8_exp(a);
  int frac = bf8_signif(a);

  if (exp == 0x1F) { // Inf or NaN
    int m_o = frac ? ((frac | 0x2) << 21) : 0; // set qBit (bit 22 of FP32 mantissa)
    return packToF32UI(sign, 0xFF, m_o);
  }

  if (! exp) {
    if (! frac) return packToF32UI(sign, 0, 0); // zero
    if (frac == 0x1) return packToF32UI(sign, 127 - 16, 0); // 0.01 x 2^-14 = 1.0 x 2^-16
    return packToF32UI(sign, 127 - 15, (frac & 0x1) << 22); // 0.1x x 2^-14
  }

  // normal
  int e_o = exp + (127 - 15);
  int m_o = frac << 21;
  return packToF32UI(sign, e_o, m_o);
}

// Convert packed FP8 E5M2 to FP6 E3M2, RNE rounding with saturation.
float_bf6 convert_bf8_to_bf6(float_bf8 a)
{
  int sign = bf8_sign(a);
  int exp  = bf8_exp(a);
  int frac = bf8_signif(a);
  int exp_rebias = BX_BF8_BIAS - BX_BF6_BIAS; // 15 - 3
  int new_exp = exp - exp_rebias;

  if (exp == 0x1F) return pack_float_bf6(sign, 0x7, 0x3); // NaN/Inf -> clamp to FP6 max
  if (exp > (exp_rebias + 7) || (exp == (exp_rebias + 7) && frac > 0x3))
    return pack_float_bf6(sign, 0x7, 0x3); // overflow -> clamp
  if (! exp) return pack_float_bf6(sign, 0, 0); // all E5M2 subnormals lie below the smallest FP6 E3M2 subnormal

  int e_o = 0, m_o = 0;
  if (new_exp <= 0) { // underflow -> subnormal FP6 or zero
    if ((1 - new_exp) <= 3) {
      int mant = frac | 0x4;
      int shift = 1 - new_exp;
      m_o = mant >> shift;
      int lowmant = mant & ((1 << shift) - 1);
      int halfway = 1 << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
        m_o++;
        if ((m_o & 0x3) == 0) e_o++;
      }
    }
  }
  else { // normal: direct rebias (mantissa same width)
    e_o = exp - exp_rebias;
    m_o = frac;
  }

  return pack_float_bf6(sign, e_o, m_o);
}

// Convert packed FP8 E5M2 to FP4 E2M1, RNE rounding with saturation.
float_bf4 convert_bf8_to_bf4(float_bf8 a)
{
  int sign = bf8_sign(a);
  int exp  = bf8_exp(a);
  int frac = bf8_signif(a);
  int exp_rebias = BX_BF8_BIAS - BX_BF4_BIAS; // 15 - 1
  int new_exp = exp - exp_rebias;

  if (exp == 0x1F) return pack_float_bf4(sign, 0x3, 0x1); // NaN/Inf -> clamp to FP4 max
  if (exp > (exp_rebias + 3) || (exp == (exp_rebias + 3) && frac > 0x2))
    return pack_float_bf4(sign, 0x3, 0x1); // overflow -> clamp
  if (! exp && ! frac) return pack_float_bf4(sign, 0, 0); // zero

  int e_o = 0, m_o = 0;
  if (new_exp <= 0) { // underflow -> subnormal or zero
    if ((2 - new_exp) <= 3) {
      int mant = frac | 0x4; // restore hidden bit
      int shift = 2 - new_exp;
      m_o = mant >> shift;
      int lowmant = mant & ((1 << shift) - 1);
      int halfway = 1 << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
        m_o++;
        if ((m_o & 0x1) == 0) e_o++; // FP4 mantissa is 1-bit
      }
    }
  }
  else { // normal: direct rebias + truncate
    int fixup = (frac >> 1) & 0x1;
    int rnex = a + fixup;
    e_o = bf8_exp(rnex) - exp_rebias;
    m_o = bf8_signif(rnex) >> 1;
  }

  return pack_float_bf4(sign, e_o, m_o);
}

/*----------------------------------------------------------------------------
 * HF8 (E4M3)
 *----------------------------------------------------------------------------*/

// convert E4M3 HF8 number to half precision floating point value (fp16)
float16 convert_hf8_to_fp16(float_hf8 a)
{
  int sign = hf8_sign(a);
  int exp = hf8_exp(a);
  int signif = hf8_signif(a);

  int exp_norm = exp + fp16_to_hf8_exp_rebias;

  if (! exp && signif) {
    // convert denormal into a normal fp16 number
    int lzcnt = (signif > 0x1) ? 1 : 2;
        lzcnt = (signif > 0x3) ? 0 : lzcnt;
    exp_norm -= lzcnt;
    signif = (signif << (lzcnt + 1)) & 0x07;
  }
  else if ((exp|signif) == 0) {
    exp_norm = 0;
  }
  else if (exp == 0xf && signif == 0x7) {
    exp_norm = 0x1f; // NaN, infinity
  }

  float16 z = (exp_norm << 10) | (signif << 7) | (sign << 15);
  return z;
}

// Convert half precision floating point number (fp16) to E4M3 HF8 number
// using bias rounding: add an 8-bit unsigned BIAS number (shifted right by
// 1, since E4M3 discards one fewer mantissa bit than E5M2) to the mantissa
// and truncate. Bias rounding is not a rounding mode selectable via a mode
// operand - it is a distinct operation that takes the bias as an explicit
// per-element operand.
//
// Infinity is preserved for the non-saturated versions. NaN at input is propagated as QNaN.
// If the result is infinity or too big to be represented, then for the saturated version, E4M3_MAX is returned,
// for the non-saturated versions, NaN is returned.
//
// DAZ is not obeyed and always assumed DAZ==0. FTZ is not obeyed and always assumed FTZ==0.
// All MXCSR mask bits DM, IM, OM, PM, UM are implicitly set.

float_hf8 convert_truncate_fp16_to_hf8_bias(float16 a, Bit8u bias, bool saturate_overflow)
{
  int aSign   = f16_sign(a);
  Bit8s aExp  = f16_exp(a);
  Bit16u aSig = f16_fraction(a);

  if (aExp == 0x1F) {
    if (! aSig) {
      // a is infinity: NaN/Inf = s.1111.111, or saturate to E4M3_MAX = s.1111.110
      return pack_float_hf8(aSign, 0xF, 0x7 - saturate_overflow);
    }

    return pack_float_hf8(aSign, 0xF, 0x7);
  }

  // Every FP16 zero or denormal lies below the smallest E4M3 denormal, so
  // the result is exactly zero - unconditionally, regardless of rounding
  // mode or bias (unlike BF8, whose denormal range overlaps FP16's).
  if (! aExp) return pack_float_hf8(aSign, 0, 0);

  int e_b = aExp;
  int m_b = aSig + (bias >> 1);
  if (m_b & 0xFC00) e_b++; // carry out of the 10-bit mantissa
  m_b &= 0x3FF;

  int newexp = e_b - fp16_to_hf8_exp_rebias;
  if (newexp >= 16) {
    return pack_float_hf8(aSign, 0xF, 0x7 - saturate_overflow);
  }

  if (newexp <= 0) {
    // Underflow into the E4M3 denormal range or all the way to zero. When
    // the shift needed exceeds the width of the (J-bit + mantissa) field,
    // the result is exactly zero regardless of how large bias is - an
    // 8-bit bias term can never be large enough to rescue a value this
    // far below the smallest denormal.
    int signif = 0;
    if ((fp16_to_hf8_exp_rebias - newexp) <= 11) {
      int mant = m_b | 0x400;
      signif = mant >> (fp16_to_hf8_exp_rebias - newexp);
    }
    return pack_float_hf8(aSign, 0, signif);
  }

  int signif = m_b >> 7;
  if (saturate_overflow && newexp == 0xF && signif == 0x7)
    signif = 0x6;
  return pack_float_hf8(aSign, newexp, signif);
}

// Convert half precision floating point number (fp16) to E4M3 HF8 number
float_hf8 convert_ne_fp16_to_hf8(float16 a, bool saturate_overflow)
{
  int aSign   = f16_sign(a);
  Bit8s aExp  = f16_exp(a);
  Bit16u aSig = f16_fraction(a);

  int signif, exp;

  if (aExp == 0x1F) {
    if (! aSig)
      return pack_float_hf8(aSign, 0xF, 0x7 - saturate_overflow);

    return pack_float_hf8(aSign, 0xF, 0x7);
  }

  // overflow -> make in HF8 NaN/Inf = s.1111.111
  // or saturate to E4M3_MAX = s.1111.110
  if (aExp > (fp16_to_hf8_exp_rebias + 15) || (aExp == (fp16_to_hf8_exp_rebias + 15) && aSig > 0x0340)) {
    return pack_float_hf8(aSign, 0xF, 0x7 - saturate_overflow);
  }

  // handle zero
  if (! aExp && !aSig) {
    return pack_float_hf8(aSign, 0, 0);
  }

  // underflow
  if (aExp <= fp16_to_hf8_exp_rebias) {
    signif = aSig | 0x0400;
    signif = signif >> (fp16_to_hf8_exp_rebias + 1 - aExp);
    int shiftOutSticky = ((aSig & 0x7f) + 0x7f) >> 7;
    signif |= shiftOutSticky;
    signif += 0x003f + ((signif >> 7) & 0x1);
    exp = signif >> 10; // exp=1 in case of round overflow
    signif = (signif >> 7) & 0x7; // truncate and ignore carry
    return pack_float_hf8(aSign, exp, signif);
  }

  // normal
  int fixup = (aSig >> 7) & 0x1;
  int roundA = a + 0x003f + fixup;
  exp = f16_exp((float16) roundA);
  exp -= fp16_to_hf8_exp_rebias;
  signif = f16_fraction((float16) roundA);
  signif >>= 7;
  return pack_float_hf8(aSign, exp, signif);
}

// Convert single precision floating point number (fp32) to E4M3 HF8 number
// using either RNE or round-to-odd (RTO); rounding is a real softfloat
// rounding-mode constant (softfloat_round_near_even or softfloat_round_odd -
// the only two rounding modes FP32->E4M3 supports outside of bias rounding).
float_hf8 convert_fp32_to_hf8(float32 a, bool saturate_overflow, unsigned rounding)
{
  bool sign   = f32_sign(a);
  Bit16s expA = f32_exp(a);
  Bit32u sigA = f32_fraction(a);

  if (expA == 0xFF) { // Inf or NaN
    if (saturate_overflow && ! sigA) return pack_float_hf8(sign, 0xF, 0x6); // clamp to max_normal
    return pack_float_hf8(sign, 0xF, 0x7);
  }
  if (! expA) return pack_float_hf8(sign, 0, 0); // zero or denormal

  int newexp = int(expA) - 127 + 7;
  int e_o = 0, m_o = 0;

  if (newexp >= 16)
    return pack_float_hf8(sign, 0xF, 0x7 - saturate_overflow); // overflow

  if (rounding == softfloat_round_near_even) {
    if (newexp <= 0) { // underflow
      if ((21 - newexp) <= 24) {
        Bit32u mant = sigA | 0x800000;
        int shift = 21 - newexp;
        m_o = mant >> shift;
        Bit32u lowmant = mant & ((Bit32u(1) << shift) - 1);
        Bit32u halfway = Bit32u(1) << (shift - 1);
        if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
          m_o++;
          if ((m_o & 0x7) == 0) e_o++;
        }
      }
    }
    else { // normal
      e_o = newexp;
      m_o = sigA >> 20;
      if (saturate_overflow && e_o == 0xF && m_o == 0x7) m_o = 0x6;
      if (sigA & 0x80000) {
        if (((sigA & 0xFFFFF) > 0x80000) || (m_o & 0x1)) {
          bool clamp_sat = saturate_overflow && e_o == 0xF && m_o == 0x6;
          bool clamp_nan = (! saturate_overflow) && e_o == 0xF && m_o == 0x7;
          if (! (clamp_sat || clamp_nan)) {
            m_o++;
            if ((m_o & 0x7) == 0) e_o++;
          }
        }
      }
    }
  }
  else { // softfloat_round_odd
    if (newexp <= 0) {
      if ((21 - newexp) <= 24) {
        // FP32 normal, magnitude in E4M3 subnormal range (shift <= 24).
        // J-bit at or above bit 0 of the E4M3 mantissa field; sticky bit
        // propagates any nonzero residual under RTO.
        Bit32u mant = sigA | 0x800000;
        int shift = 21 - newexp;
        m_o = mant >> shift;
        m_o |= (mant & ((Bit32u(1) << shift) - 1)) ? 1 : 0;
      }
      else {
        // FP32 normal with magnitude < 2^-10 (shift > 24): J-bit falls
        // below bit 0 of the E4M3 subnormal mantissa field.
        m_o = 1;
      }
    }
    else {
      e_o = newexp;
      m_o = sigA >> 20;
      m_o |= (sigA & 0xFFFFF) ? 1 : 0;
      if (saturate_overflow && e_o == 0xF && m_o == 0x7) m_o = 0x6; // clamp NaN->max_normal
    }
  }

  return pack_float_hf8(sign, e_o & 0xF, m_o & 0x7);
}

// Convert single precision floating point number (fp32) to E4M3 HF8 number
// using bias rounding. The bias operand covers the full 20-bit truncated
// mantissa range (23 source bits - 3 target bits). Underflow into the HF8
// denormal range shifts the biased mantissa into place and truncates (no
// rounding), mirroring convert_truncate_fp32_to_bf8_bias's E5M2 underflow
// branch - the spec PDF's own Section 16.1 pseudocode text omits this
// computation for E4M3 (unlike every other branch, including E5M2's BIAS
// branch and E4M3's own RNE/RTO branches, which all compute it), but that
// omission does not match real hardware: cross-checked against Intel SDE's
// (Diamond Rapids/ACE) actual execution, which returns a nonzero denormal
// here, exactly matching this shift-and-truncate formula.
float_hf8 convert_truncate_fp32_to_hf8_bias(float32 a, Bit32u bias, bool saturate_overflow)
{
  bool sign   = f32_sign(a);
  Bit16s expA = f32_exp(a);
  Bit32u sigA = f32_fraction(a);

  if (expA == 0xFF) { // Inf or NaN
    if (saturate_overflow && ! sigA) return pack_float_hf8(sign, 0xF, 0x6);
    return pack_float_hf8(sign, 0xF, 0x7);
  }
  if (! expA) return pack_float_hf8(sign, 0, 0); // zero or denormal

  Bit32u e_b = expA;
  Bit32u m_b = sigA + (bias & 0xFFFFF); // 20-bit bias: covers 23->3 bit truncation
  if (m_b & 0xFF800000) e_b++; // carry out of the 23-bit mantissa
  m_b &= 0x7FFFFF;

  int newexp = int(e_b) - 127 + 7;

  if (newexp >= 16) return pack_float_hf8(sign, 0xF, 0x7 - saturate_overflow);

  int e_o = 0, m_o = 0;
  if (newexp <= 0) { // underflow into the HF8 denormal range or to zero
    if ((21 - newexp) <= 24) {
      Bit32u mant = m_b | 0x800000;
      m_o = mant >> (21 - newexp);
    }
  }
  else {
    e_o = newexp;
    m_o = m_b >> 20;
  }

  return pack_float_hf8(sign, e_o & 0xF, m_o & 0x7);
}

// Convert E4M3 HF8 number to single precision floating point value (fp32).
// Denormal handling mirrors convert_hf8_to_fp16()'s lzcnt-based renormalize
// above - same 3-bit E4M3 significand, just rebiased and shifted into
// FP32's wider (23-bit) mantissa field instead of FP16's.
float32 convert_hf8_to_fp32(float_hf8 a)
{
  int sign = hf8_sign(a);
  int exp  = hf8_exp(a);
  int frac = hf8_signif(a);

  int exp_norm = exp + (127 - 7);

  if (! exp && frac) {
    // convert denormal into a normal fp32 number
    int lzcnt = (frac > 0x1) ? 1 : 2;
        lzcnt = (frac > 0x3) ? 0 : lzcnt;
    exp_norm -= lzcnt;
    frac = (frac << (lzcnt + 1)) & 0x07;
  }
  else if ((exp|frac) == 0) {
    exp_norm = 0;
  }
  else if (exp == 0xF && frac == 0x7) {
    exp_norm = 0xFF; // NaN (E4M3 has no true Inf)
  }

  return packToF32UI(sign, exp_norm, frac << 20);
}

// Convert packed FP8 E4M3 to FP6 E2M3, RNE rounding with saturation.
float_hf6 convert_hf8_to_hf6(float_hf8 a)
{
  int sign = hf8_sign(a);
  int exp  = hf8_exp(a);
  int frac = hf8_signif(a);
  int exp_rebias = BX_HF8_BIAS - BX_HF6_BIAS; // 7 - 1
  int new_exp = exp - exp_rebias;

  if (exp == 0xF) return pack_float_hf6(sign, 0x3, 0x7); // NaN/max -> clamp to FP6 max
  if (exp > (exp_rebias + 3) || (exp == (exp_rebias + 3) && frac > 0x7))
    return pack_float_hf6(sign, 0x3, 0x7); // overflow -> clamp
  if (! exp && ! frac) return pack_float_hf6(sign, 0, 0); // zero

  int e_o = 0, m_o = 0;
  if (new_exp <= 0) { // underflow
    if ((1 - new_exp) <= 4) {
      int mant = frac | 0x8;
      int shift = 1 - new_exp;
      m_o = mant >> shift;
      int lowmant = mant & ((1 << shift) - 1);
      int halfway = 1 << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
        m_o++;
        if ((m_o & 0x7) == 0) e_o++;
      }
    }
  }
  else { // normal: direct rebias (mantissa same width)
    e_o = exp - exp_rebias;
    m_o = frac;
  }

  return pack_float_hf6(sign, e_o, m_o);
}

// Convert packed FP8 E4M3 to FP4 E2M1; NaN (0x7F) clamps; RNE in subnormal.
float_bf4 convert_hf8_to_bf4(float_hf8 a)
{
  int sign = hf8_sign(a);
  int exp  = hf8_exp(a);
  int frac = hf8_signif(a);
  int exp_rebias = BX_HF8_BIAS - BX_BF4_BIAS; // 7 - 1
  int new_exp = exp - exp_rebias;

  if (exp == 0xF && frac == 0x7) return pack_float_bf4(sign, 0x3, 0x1); // NaN -> clamp
  if (exp > (exp_rebias + 3) || (exp == (exp_rebias + 3) && frac > 0x4))
    return pack_float_bf4(sign, 0x3, 0x1); // overflow -> clamp
  if (! exp) return pack_float_bf4(sign, 0, 0); // zero / denorm -> zero

  int e_o = 0, m_o = 0;
  if (new_exp <= 0) { // underflow -> subnormal or zero
    if ((3 - new_exp) <= 4) {
      int mant = frac | 0x8; // restore hidden bit
      int shift = 3 - new_exp;
      m_o = mant >> shift;
      int lowmant = mant & ((1 << shift) - 1);
      int halfway = 1 << (shift - 1);
      if (lowmant > halfway || (lowmant == halfway && (m_o & 0x1))) {
        m_o++;
        if ((m_o & 0x1) == 0) e_o++;
      }
    }
  }
  else { // normal
    int fixup = (frac >> 2) & 0x1;
    int rnex = a + 0x01 + fixup;
    e_o = hf8_exp(rnex) - exp_rebias;
    m_o = hf8_signif(rnex) >> 2;
  }

  return pack_float_bf4(sign, e_o, m_o);
}

/*----------------------------------------------------------------------------
 * FP4/FP6 -> HF8 (the only FP8 target for these narrower formats)
 *----------------------------------------------------------------------------*/

// Convert FP4 E2M1 to FP8 E4M3 (lossless; all FP4 values are representable
// in E4M3, so this is a direct 8-entry lookup, no rounding needed).
float_hf8 convert_bf4_to_hf8(float_bf4 a)
{
  int sign = bf4_sign(a);
  static const Bit8u lut[8] = {
    0x00, // 0x0: 0.0
    0x30, // 0x1: 0.5
    0x38, // 0x2: 1.0
    0x3C, // 0x3: 1.5
    0x40, // 0x4: 2.0
    0x44, // 0x5: 3.0
    0x48, // 0x6: 4.0
    0x4C, // 0x7: 6.0
  };
  return float_hf8(lut[a & 0x7] | (sign << 7));
}

// Convert FP6 E3M2 to FP8 E4M3 (lossless widening; all FP6 values are
// representable in E4M3, so no rounding is needed).
float_hf8 convert_bf6_to_hf8(float_bf6 a)
{
  int sign = bf6_sign(a);
  int exp  = bf6_exp(a);
  int frac = bf6_signif(a);

  if (! exp) { // FP6 subnormal: exp=0, frac in [0..3]
    if (! frac) return pack_float_hf8(sign, 0, 0); // zero
    if (frac == 1) return pack_float_hf8(sign, 3, 0); // 0.01 x 2^-2 = 2^-4 -> FP8 e=3,m=0
    return pack_float_hf8(sign, 4, (frac & 0x1) << 2); // frac=2/3: 0.10/0.11 x 2^-2
  }

  // FP6 normal: rebias 3->7, shift mantissa left 1
  int e_o = exp + (BX_HF8_BIAS - BX_BF6_BIAS);
  int m_o = frac << 1; // 2-bit frac -> bits [2:1] of 3-bit frac
  return pack_float_hf8(sign, e_o, m_o & 0x7); // never overflows by construction
}

// Convert FP6 E2M3 to FP8 E4M3 (lossless widening; all FP6 values are
// representable in E4M3, so no rounding is needed).
float_hf8 convert_hf6_to_hf8(float_hf6 a)
{
  int sign = hf6_sign(a);
  int exp  = hf6_exp(a);
  int frac = hf6_signif(a);

  if (! exp) { // FP6 subnormal: 0.mmm x 2^0 = frac x 2^-3
    if (! frac) return pack_float_hf8(sign, 0, 0); // zero
    if (frac == 1) return pack_float_hf8(sign, 4, 0); // 2^-3 -> FP8 e=4 (=7-3), m=0
    if (frac <= 3) return pack_float_hf8(sign, 5, (frac & 0x1) << 2); // leading 1 at bit 1 -> 2^-2
    return pack_float_hf8(sign, 6, (frac & 0x3) << 1); // leading 1 at bit 2 -> 2^-1
  }

  // FP6 normal: rebias 1->7, mantissa unchanged
  int e_o = exp + (BX_HF8_BIAS - BX_HF6_BIAS);
  int m_o = frac; // 3-bit frac fits directly
  return pack_float_hf8(sign, e_o, m_o & 0x7); // never overflows by construction
}
