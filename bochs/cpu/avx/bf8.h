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

BX_CPP_INLINE float16 convert_bf8_to_fp16(float_bf8 op)
{
  return Bit16u(op) << 8;
}

// Convert half precision floating point number (fp16) to E5M2 BF8 number
// The bias paramemter is the bias 8b integer to be added to data before the downconvert
BX_CPP_INLINE float_bf8 convert_ne_fp16_to_bf8_bias(float16 a, Bit8u bias, bool saturate_overflow, struct softfloat_status_t *status)
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
  Bit16u roundA = a + rounding_bias;

  if (((z & 0x7F00) == 0x7C00) && saturate_overflow) {
    softfloat_raiseFlags(status, softfloat_flag_overflow);
    z = ((roundA >> 8) & 0x80) | 0x7B;
  }
  else {
    z = roundA >> 8;
    if (roundA != a)
      softfloat_raiseFlags(status, softfloat_flag_inexact);
  }

  return z;
}

// convert half precision floating point number (fp16) to E5M2 BF8 number
BX_CPP_INLINE float_bf8 convert_ne_fp16_to_bf8(float16 a, bool saturate_overflow, struct softfloat_status_t *status)
{
  // without bias argument rounding bias is 0x7F + bit(a[8])
  return convert_ne_fp16_to_bf8_bias(a, 0x007F + ((a >> 8) & 0x1), saturate_overflow, status);
}

#endif
