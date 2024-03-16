/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
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

#ifndef BX_BF16_HELPER
#define BX_BF16_HELPER

// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm

BX_CPP_INLINE float32 convert_bfloat16_to_fp32(bfloat16 op)
{
  return Bit32u(op) << 16;
}

#include "softfloat3e/include/softfloat.h"

BX_CPP_INLINE bfloat16 convert_ne_fp32_to_bfloat16(float32 op)
{
  softfloat_class_t op_class = f32_class(op);

  switch(op_class) {
    case softfloat_zero:
    case softfloat_denormal:
      return (op >> 16) & 0x8000; // sign preserving zero (denormal go to zero)

    case softfloat_negative_inf:
    case softfloat_positive_inf:
      return op >> 16;

    case softfloat_SNaN:
    case softfloat_QNaN:
      return (op >> 16) | 0x40;  // truncate and set msb of the mantisa, force qnan

    case softfloat_normalized:
      break;
  }

  Bit32u rounding_bias = 0x7FFF + ((op >> 16) & 0x1);
  return (op + rounding_bias) >> 16;
}

#endif
