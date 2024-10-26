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

#ifndef BX_BF16_SOFTFLOAT_COMPARE_HELPER
#define BX_BF16_SOFTFLOAT_COMPARE_HELPER

// ======= bfloat16 ======= //

// These operations do not consult or update MXCSR
// Denormal BF16 input operands are treated as zeros (DAZ) and denormal BF16 outputs are flushed to zero (FTZ)
// Rounding mode is always round-to-nearest-even (RNE)

typedef int (*bfloat16_compare_method)(bfloat16, bfloat16);

// 0x00
BX_CPP_INLINE int bf16_eq_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_equal);
}

// 0x03
BX_CPP_INLINE int bf16_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_unordered);
}

// 0x04
BX_CPP_INLINE int bf16_neq_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_equal);
}

// 0x07
BX_CPP_INLINE int bf16_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_unordered);
}

// 0x08
BX_CPP_INLINE int bf16_eq_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x0b
BX_CPP_INLINE int bf16_false(bfloat16 a, bfloat16 b)
{
   return 0;
}

// 0x0c
BX_CPP_INLINE int bf16_neq_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x0f
BX_CPP_INLINE int bf16_true(bfloat16 a, bfloat16 b)
{
   return 1;
}

// 0x11
BX_CPP_INLINE int bf16_lt_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_less);
}

// 0x12
BX_CPP_INLINE int bf16_le_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x15
BX_CPP_INLINE int bf16_nlt_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_less);
}

// 0x16
BX_CPP_INLINE int bf16_nle_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x19
BX_CPP_INLINE int bf16_nge_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x1a
BX_CPP_INLINE int bf16_ngt_unordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation != softfloat_relation_greater);
}

// 0x1d
BX_CPP_INLINE int bf16_ge_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x1e
BX_CPP_INLINE int bf16_gt_ordered(bfloat16 a, bfloat16 b)
{
   int relation = bf16_compare(a, b);
   return (relation == softfloat_relation_greater);
}

#endif
