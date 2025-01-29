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

#ifndef BX_BF8_HELPER
#define BX_BF8_HELPER

// TF32: s|eeeeeeee|mmmmmmmmmm-------------
// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm
//  BF8: s|eeeee|mm                          (E5M2)
//  HF8: s|eeee|mmm                          (E4M3)

typedef Bit8u float_bf8;

// E5M2
BX_CPP_INLINE int bf8_sign(float_bf8 bf8) { return bf8 >> 7; }
BX_CPP_INLINE int bf8_exp(float_bf8 bf8) { return (bf8 & 0x7c) >> 2; }
BX_CPP_INLINE int bf8_signif(float_bf8 bf8) { return bf8 & 0x03; }

BX_CPP_INLINE int bf8_is_denormal(float_bf8 bf8) { return (bf8 & 0x7c) == 0 && (bf8 & 0x03) != 0; }

BX_CPP_INLINE float16 convert_bf8_to_fp16(float_bf8 op)
{
  return Bit16u(op) << 8;
}

// Convert half precision floating point number (fp16) to E5M2 BF8 number.
// Add a 8-bit unsigned BIAS number to the 8 bits below the LSB of the rounded number and
// then convert the result into a E5M2 FP8 value by normalizing and truncation to align
// with E4M3 dynamic range. The addition is aligned FP16 LSB.
// The result is truncated.

// Infinity is preserved for the non-saturated versions. NaN at input is propagated as QNaN.
// If the result is infinity or too big to be represented, then for the saturated version, E5M2_MAX is returned,
// for the non-saturated versions, infinity is returned.
// The UE (underflow) flag is set when the result is both denormal and inexact.

// DAZ is not obeyed and always assumed DAZ==0. FTZ is not obeyed and always assumed FTZ==0.
// All MXCSR mask bits DM, IM, OM, PM, UM are implicitly set.

BX_CPP_INLINE float_bf8 convert_truncate_fp16_to_bf8_bias(float16 a, Bit8u bias, bool saturate_overflow, struct softfloat_status_t *status)
{
  int signA   = signF16UI(a);
  Bit8s expA  = expF16UI(a);
  Bit16u sigA = fracF16UI(a);

  float_bf8 z;

  if (expA == 0x1F) {
    if (! sigA) {
      // a is infinity
      if (saturate_overflow)
        z = (signA << 7) | 0x7B; // max signif value
      else
        z = a >> 8;              // keep infinity

      return z;
    }

    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return (a >> 8) | 0x02; // make it QNaN
  }

  if (! expA && sigA)
    softfloat_raiseFlags(status, softfloat_flag_denormal);

  // normal, zero or denormal number, convert applying round-to-nearest-even
  Bit16u rounding_bias = (Bit16u) bias;
  Bit16u roundA = (a + rounding_bias) >> 8;

  if (((roundA & 0x7F) == 0x7C) && saturate_overflow) {
    z = (roundA & 0x80) | 0x7B;
  }
  else {
    z = (Bit8u) roundA;
    if ((roundA<<8) != a) {
      softfloat_raiseFlags(status, softfloat_flag_inexact);
      if (bf8_is_denormal(z))
        softfloat_raiseFlags(status, softfloat_flag_underflow);
    }
  }

  return z;
}

// convert half precision floating point number (fp16) to E5M2 BF8 number
BX_CPP_INLINE float_bf8 convert_ne_fp16_to_bf8(float16 a, bool saturate_overflow, struct softfloat_status_t *status)
{
  // BF8 matching FP16 format but mantissa is 8-bit shorter
  // so conversion of FP16 to BF8 require to round FP16 mantissa to its bit 8
  // this function performs according to round-to-nearest-even (RNE) rounding mode

  // RNE rounding:                      1.BBGRXXXXX
  //                                        ||-----
  //   Guard bit (LSB of the result)   -----+|  Sticky bit = OR of all remaining bits
  //   Round bit (First bit removed)   ------+
  //
  //   GRS
  //   ---
  //   x0x -> no increment
  //   010 -> no increment
  //   110 -> increment
  //   x11 -> increment

  // Rounding-to-nearest-even to bit 8 is equivalent to converting with bias matching increment value

  // Sticky bits: 0x7F
  // Value of Guard bit: ((a >> 8) & 0x1)          Increment value: 0x7F + bit(a[8])

  return convert_truncate_fp16_to_bf8_bias(a, 0x007F + ((a >> 8) & 0x1), saturate_overflow, status);
}

#endif
