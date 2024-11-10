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

#ifndef BX_HF8_HELPER
#define BX_HF8_HELPER

// TF32: s|eeeeeeee|mmmmmmmmmm-------------
// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm
//  BF8: s|eeeee|mm                          (E5M2)
//  HF8: s|eeee|mmm                          (E4M3)

// The E4M3 format has a non-standard definition, including the
// same representation for Infinity and NaN in order to increase its range

// HF8
// -----------------------------------------------
//    ExpBias    |                7
//  Max Normal   | S.1111.110 = 448.0 (1.75 * 2^8)
//  Min Normal   | S.0001.000 = 1.56e-2 (2^-6)
// Max Denormal  | S.0000.111 = 1.36e-2 (0.875 * 2^-6)
// Min Denormal  | S.0000.001 = 1.95e-3 (0.125 * 2^-6)
// NaNs and Inf  |           S.1111.111
// -----------------------------------------------

typedef Bit8u float_hf8;

// E4M3
BX_CPP_INLINE int hf8_sign(float_hf8 hf8) { return hf8 >> 7; }
BX_CPP_INLINE int hf8_exp(float_hf8 hf8) { return (hf8 & 0x78) >> 3; }
BX_CPP_INLINE int hf8_signif(float_hf8 hf8) { return hf8 & 0x07; }

BX_CPP_INLINE float_hf8 pack_float_hf8(int sign, int exp, int signif) { return (sign << 7) | (exp << 3) | signif; }

#ifndef BX_FP16_BIAS
#define BX_FP16_BIAS (0xF)
#endif
#define BX_HF8_BIAS  (0x7)

static const int fp16_to_hf8_exp_rebias = BX_FP16_BIAS - BX_HF8_BIAS;

// convert E4M3 HF8 number to half precision floating point value (fp16)
BX_CPP_INLINE float16 convert_hf8_to_fp16(float_hf8 a, struct softfloat_status_t *status)
{
  int sign = hf8_sign(a);
  int exp = hf8_exp(a);
  int signif = hf8_signif(a);

  int exp_norm = exp + fp16_to_hf8_exp_rebias;

  if (! exp && signif) {
    // convert denormal into a normal fp16 number
    softfloat_raiseFlags(status, softfloat_flag_denormal);
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

// Convert half precision floating point number (fp16) to E4M3 HF8 number.
// Add a 8-bit unsigned BIAS number (shited right by 1) to the 8 bits below the LSB of the rounded number and
// then convert the result into a E4M3 FP8 value by normalizing and truncation to align
// with E4M3 dynamic range. First the BIAS is shifted right by 1 and then the addition is aligned FP16 LSB.
// The result is truncated.

// Infinity is preserved for the non-saturated versions. NaN at input is propagated as QNaN.
// If the result is infinity or too big to be represented, then for the saturated version, E4M3_MAX is returned,
// for the non-saturated versions, NaN is returned.

// DAZ is not obeyed and always assumed DAZ==0. FTZ is not obeyed and always assumed FTZ==0.
// All MXCSR mask bits DM, IM, OM, PM, UM are implicitly set.

BX_CPP_INLINE float_hf8 convert_truncate_fp16_to_hf8_bias(float16 a, Bit8u bias, bool saturate_overflow, struct softfloat_status_t *status)
{
  int aSign   = signF16UI(a);
  Bit8s aExp  = expF16UI(a);
  Bit16u aSig = fracF16UI(a);

  int a_bias = a + (bias >> 1);

  Bit8s aExpBias = expF16UI(a_bias);
  Bit16u aSigBias = fracF16UI(a_bias);

  if (aExp == 0x1F) {
    if (! aSig)
      goto overflow;   // a is infinity

    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return pack_float_hf8(aSign, 0xF, 0x7);
  }

  // overflow -> make in HF8 NaN/Inf = s.1111.111
  // or saturate to E4M3_MAX = s.1111.110
  if (aExpBias > (fp16_to_hf8_exp_rebias + 15) || (aExpBias == (fp16_to_hf8_exp_rebias + 15) && aSigBias >= 0x0380)) {
overflow:
    if (saturate_overflow)
      return pack_float_hf8(aSign, 0xF, 0x6);
    else
      return pack_float_hf8(aSign, 0xF, 0x7);
  }

  int signif, exp;

  // handle input denormal or zero
  if (! aExp) {
    if (aSig) softfloat_raiseFlags(status, softfloat_flag_denormal);
    signif = (aSig + (int(bias) << 7));
    signif = signif >> (fp16_to_hf8_exp_rebias + 7);
    return pack_float_hf8(aSign, 0, signif);
  }

  // underflow
  if (aExpBias <= fp16_to_hf8_exp_rebias) {
    signif = aSig | 0x0400;
    signif += (int(bias) << (fp16_to_hf8_exp_rebias - aExp));
    signif = signif >> (fp16_to_hf8_exp_rebias + 1 - aExp);
    exp = signif >> 10; // exp=1 in case of round overflow
    signif = (signif >> 7) & 0x7; // truncate and ignore carry
    return pack_float_hf8(aSign, exp, signif);
  }

  // normal number truncate
  exp = aExpBias - fp16_to_hf8_exp_rebias;
  signif = aSigBias >> 7;

  return pack_float_hf8(aSign, exp, signif);
}

// Convert half precision floating point number (fp16) to E4M3 HF8 number
BX_CPP_INLINE float_hf8 convert_ne_fp16_to_hf8(float16 a, bool saturate_overflow, struct softfloat_status_t *status)
{
  int aSign   = signF16UI(a);
  Bit8s aExp  = expF16UI(a);
  Bit16u aSig = fracF16UI(a);

  int signif, exp;

  if (aExp == 0x1F) {
    if (! aSig)
      goto overflow;   // a is infinity

    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return pack_float_hf8(aSign, 0xF, 0x7);
  }

  // overflow -> make in HF8 NaN/Inf = s.1111.111
  // or saturate to E4M3_MAX = s.1111.110
  if (aExp > (fp16_to_hf8_exp_rebias + 15) || (aExp == (fp16_to_hf8_exp_rebias + 15) && aSig > 0x0340)) {
overflow:
    if (saturate_overflow)
      return pack_float_hf8(aSign, 0xF, 0x6);
    else
      return pack_float_hf8(aSign, 0xF, 0x7);
  }

  // raise denormal or handle zero
  if (! aExp) {
    if (aSig) softfloat_raiseFlags(status, softfloat_flag_denormal);
    else return pack_float_hf8(aSign, 0, 0);
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
  exp = expF16UI(roundA);
  exp -= fp16_to_hf8_exp_rebias;
  signif = fracF16UI(roundA);
  signif >>= 7;
  return pack_float_hf8(aSign, exp, signif);
}

#endif
