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

#ifndef BX_BF16_HELPER
#define BX_BF16_HELPER

// TF32: s|eeeeeeee|mmmmmmmmmm-------------
// FP32: s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm
// BF16: s|eeeeeeee|mmmmmmm
//  F16: s|eeeee|mmmmmmmmmm
//  BF8: s|eeeee|mm                          (E5M2)
//  HF8: s|eeee|mmm                          (E4M3)

BX_CPP_INLINE float32 convert_bfloat16_to_fp32(bfloat16 op)
{
  return Bit32u(op) << 16;
}

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

// convert fp32 to tf32 by zeroing out lower mantissa bits [12:0]
BX_CPP_INLINE float32 fp32_convert_to_tf32(float32 a)
{
  return a & 0xffffe000;
}

// bf16 arithmetics
// These operations do not consult or update MXCSR
// Denormal BF16 input operands are treated as zeros (DAZ) and denormal BF16 outputs are flushed to zero (FTZ)
// Rounding mode is always round-to-nearest-even (RNE)

extern bfloat16 bf16_sqrt(bfloat16 a);

extern bfloat16 bf16_add(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_sub(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_mul(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_div(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_min(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_max(bfloat16 a, bfloat16 b);
extern bfloat16 bf16_scalef(bfloat16 a, bfloat16 b);

extern int bf16_compare(bfloat16 a, bfloat16 b); // always quiet

extern bfloat16 bf16_roundToInt(bfloat16, uint8_t scale);

extern bfloat16 bf16_getExp(bfloat16);
extern bfloat16 bf16_getMant(bfloat16, int, int);

extern bfloat16 bf16_mulAdd(bfloat16 a, bfloat16 b, bfloat16 c, uint8_t op);

BX_CPP_INLINE softfloat_class_t bf16_class(bfloat16 a) { return f32_class(convert_bfloat16_to_fp32(a)); }

extern int8_t bf16_to_i8_saturate(bfloat16 a);
extern uint8_t bf16_to_ui8_saturate(bfloat16 a);
extern int8_t bf16_to_i8_round_to_zero_saturate(bfloat16 a);
extern uint8_t bf16_to_ui8_round_to_zero_saturate(bfloat16 a);

extern bfloat16 bfloat16_approximate_rcp14(bfloat16 a);
extern bfloat16 bfloat16_approximate_rsqrt14(bfloat16 a);

BX_CPP_INLINE bool bf16_sign(bfloat16 a) { return a & 0x8000; }
BX_CPP_INLINE int  bf16_exp(bfloat16 a) { return (a >> 7) & 0xFF; }
BX_CPP_INLINE int  bf16_fraction(bfloat16 a) { return a & 0x7F; }

BX_CPP_INLINE bfloat16 bf16_denormal_to_zero(bfloat16 a)
{
    if (! bf16_exp(a) && bf16_fraction(a))
        return a & 0x8000;
    return a;
}

#endif
