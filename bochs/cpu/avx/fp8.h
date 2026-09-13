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
// fp8.h
//
// Data types and accessors for the OCP 8/6/4-bit floating point formats
// (E5M2/BF8, E4M3/HF8, E3M2/BF6, E2M3/HF6, E2M1/BF4), plus prototypes for
// the conversions between them and FP16/FP32.
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_FP8_HELPER
#define BX_FP8_HELPER

#include "softfloat3e/include/softfloat_types.h"

// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm
//  BF8: s|eeeee|mm                          (E5M2)
//  HF8: s|eeee|mmm                          (E4M3)
//  BF6: s|eee|mm                            (E3M2)
//  HF6: s|ee|mmm                            (E2M3)
//  BF4: s|ee|m                              (E2M1)

/*----------------------------------------------------------------------------
 * E5M2 (BF8)
 *----------------------------------------------------------------------------*/
typedef Bit8u float_bf8;

BX_CPP_INLINE int bf8_sign(float_bf8 bf8) { return bf8 >> 7; }
BX_CPP_INLINE int bf8_exp(float_bf8 bf8) { return (bf8 & 0x7c) >> 2; }
BX_CPP_INLINE int bf8_signif(float_bf8 bf8) { return bf8 & 0x03; }

BX_CPP_INLINE float_bf8 pack_float_bf8(int sign, int exp, int signif) { return (float_bf8) ((sign << 7) | (exp << 2) | signif); }

#ifndef BX_FP16_BIAS
#define BX_FP16_BIAS (0xF)
#endif
#define BX_BF8_BIAS  (0xF) // E5M2 shares FP16's exponent width and bias

/*----------------------------------------------------------------------------
 * E4M3 (HF8)
 *
 * The E4M3 format has a non-standard definition, including the same
 * representation for Infinity and NaN in order to increase its range.
 *
 * HF8
 * -----------------------------------------------
 *    ExpBias    |                7
 *  Max Normal   | S.1111.110 = 448.0 (1.75 * 2^8)
 *  Min Normal   | S.0001.000 = 1.56e-2 (2^-6)
 * Max Denormal  | S.0000.111 = 1.36e-2 (0.875 * 2^-6)
 * Min Denormal  | S.0000.001 = 1.95e-3 (0.125 * 2^-6)
 * NaNs and Inf  |           S.1111.111
 * -----------------------------------------------
 *----------------------------------------------------------------------------*/
typedef Bit8u float_hf8;

BX_CPP_INLINE int hf8_sign(float_hf8 hf8) { return hf8 >> 7; }
BX_CPP_INLINE int hf8_exp(float_hf8 hf8) { return (hf8 & 0x78) >> 3; }
BX_CPP_INLINE int hf8_signif(float_hf8 hf8) { return hf8 & 0x07; }

BX_CPP_INLINE float_hf8 pack_float_hf8(int sign, int exp, int signif) { return (float_hf8) ((sign << 7) | (exp << 3) | signif); }

#define BX_HF8_BIAS  (0x7)

static const int fp16_to_hf8_exp_rebias = BX_FP16_BIAS - BX_HF8_BIAS;

/*----------------------------------------------------------------------------
 * E3M2 (BF6)
 *
 * FP6 is only ever packed six-per-6-bytes (6-bit-packed); float_bf6 holds
 * one decoded 6-bit element right-justified in a byte.
 *----------------------------------------------------------------------------*/
typedef Bit8u float_bf6;

BX_CPP_INLINE int bf6_sign(float_bf6 bf6) { return bf6 >> 5; }
BX_CPP_INLINE int bf6_exp(float_bf6 bf6) { return (bf6 & 0x1C) >> 2; }
BX_CPP_INLINE int bf6_signif(float_bf6 bf6) { return bf6 & 0x03; }

BX_CPP_INLINE float_bf6 pack_float_bf6(int sign, int exp, int signif) { return (float_bf6) ((sign << 5) | (exp << 2) | signif); }

#define BX_BF6_BIAS (0x3)

/*----------------------------------------------------------------------------
 * E2M3 (HF6)
 *
 * FP6 is only ever packed six-per-6-bytes (6-bit-packed); float_hf6 holds
 * one decoded 6-bit element right-justified in a byte.
 *----------------------------------------------------------------------------*/
typedef Bit8u float_hf6;

BX_CPP_INLINE int hf6_sign(float_hf6 hf6) { return hf6 >> 5; }
BX_CPP_INLINE int hf6_exp(float_hf6 hf6) { return (hf6 & 0x18) >> 3; }
BX_CPP_INLINE int hf6_signif(float_hf6 hf6) { return hf6 & 0x07; }

BX_CPP_INLINE float_hf6 pack_float_hf6(int sign, int exp, int signif) { return (float_hf6) ((sign << 5) | (exp << 3) | signif); }

#define BX_HF6_BIAS (0x1)

/*----------------------------------------------------------------------------
 * E2M1 (BF4)
 *
 * FP4 is only ever packed two-per-byte (nibble-packed); float_bf4 holds one
 * decoded 4-bit element right-justified in a byte.
 *----------------------------------------------------------------------------*/
typedef Bit8u float_bf4;

BX_CPP_INLINE int bf4_sign(float_bf4 bf4) { return bf4 >> 3; }
BX_CPP_INLINE int bf4_exp(float_bf4 bf4) { return (bf4 & 0x6) >> 1; }
BX_CPP_INLINE int bf4_signif(float_bf4 bf4) { return bf4 & 0x1; }

BX_CPP_INLINE float_bf4 pack_float_bf4(int sign, int exp, int signif) { return (float_bf4) ((sign << 3) | (exp << 1) | signif); }

#define BX_BF4_BIAS (0x1)

/*----------------------------------------------------------------------------
 * Conversions - implemented in fp8.cc. Grouped by source/destination format:
 * BF8 (E5M2) first, then HF8 (E4M3), then the FP6/FP4 pair conversions that
 * only ever target HF8.
 *----------------------------------------------------------------------------*/

// ---- BF8 (E5M2) ----
float16   convert_bf8_to_fp16(float_bf8 a);
float_bf8 convert_truncate_fp16_to_bf8_bias(float16 a, Bit8u bias, bool saturate_overflow);
float_bf8 convert_ne_fp16_to_bf8(float16 a, bool saturate_overflow);
float_bf8 convert_ne_fp32_to_bf8(float32 a, bool saturate_overflow);
float_bf8 convert_truncate_fp32_to_bf8_bias(float32 a, Bit32u bias, bool saturate_overflow);
float32   convert_bf8_to_fp32(float_bf8 a);
float_bf6 convert_bf8_to_bf6(float_bf8 a);
float_bf4 convert_bf8_to_bf4(float_bf8 a);

// ---- HF8 (E4M3) ----
float16   convert_hf8_to_fp16(float_hf8 a);
float_hf8 convert_truncate_fp16_to_hf8_bias(float16 a, Bit8u bias, bool saturate_overflow);
float_hf8 convert_ne_fp16_to_hf8(float16 a, bool saturate_overflow);
float_hf8 convert_fp32_to_hf8(float32 a, bool saturate_overflow, unsigned rounding);
float_hf8 convert_truncate_fp32_to_hf8_bias(float32 a, Bit32u bias, bool saturate_overflow);
float32   convert_hf8_to_fp32(float_hf8 a);
float_hf6 convert_hf8_to_hf6(float_hf8 a);
float_bf4 convert_hf8_to_bf4(float_hf8 a);

// ---- FP4/FP6 -> HF8 (the only FP8 target for these narrower formats) ----
float_hf8 convert_bf4_to_hf8(float_bf4 a);
float_hf8 convert_bf6_to_hf8(float_bf6 a);
float_hf8 convert_hf6_to_hf8(float_hf6 a);

#endif
