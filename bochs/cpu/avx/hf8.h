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

#ifndef BX_FP16_BIAS
#define BX_FP16_BIAS (0xF)
#endif
#define BX_HF8_BIAS  (0x7)

// convert E4M3 HF8 number to half precision floating point value (fp16)
BX_CPP_INLINE float16 convert_hf8_to_fp16(float_hf8 a, struct softfloat_status_t *status)
{
  int sign = hf8_sign(a);
  int exp = hf8_exp(a);
  int signif = hf8_signif(a);

  int exp_norm = exp + (BX_FP16_BIAS - BX_HF8_BIAS);

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

#endif
