/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#ifndef _SOFTFLOAT_COMPARE_H_
#define _SOFTFLOAT_COMPARE_H_

#include "softfloat.h"

// ======= float16 ======= //

typedef int (*float16_compare_method)(float16, float16, softfloat_status_t *status);

// 0x00
BX_CPP_INLINE int f16_eq_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x01
BX_CPP_INLINE int f16_lt_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x02
BX_CPP_INLINE int f16_le_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x03
BX_CPP_INLINE int f16_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x04
BX_CPP_INLINE int f16_neq_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x05
BX_CPP_INLINE int f16_nlt_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x06
BX_CPP_INLINE int f16_nle_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x07
BX_CPP_INLINE int f16_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x08
BX_CPP_INLINE int f16_eq_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x09
BX_CPP_INLINE int f16_nge_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x0a
BX_CPP_INLINE int f16_ngt_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x0b
BX_CPP_INLINE int f16_false_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   f16_compare_quiet(a, b, status);
   return 0;
}

// 0x0c
BX_CPP_INLINE int f16_neq_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x0d
BX_CPP_INLINE int f16_ge_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x0e
BX_CPP_INLINE int f16_gt_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x0f
BX_CPP_INLINE int f16_true_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   f16_compare_quiet(a, b, status);
   return 1;
}

// 0x10
BX_CPP_INLINE int f16_eq_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x11
BX_CPP_INLINE int f16_lt_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x12
BX_CPP_INLINE int f16_le_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x13
BX_CPP_INLINE int f16_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x14
BX_CPP_INLINE int f16_neq_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x15
BX_CPP_INLINE int f16_nlt_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x16
BX_CPP_INLINE int f16_nle_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x17
BX_CPP_INLINE int f16_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x18
BX_CPP_INLINE int f16_eq_unordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x19
BX_CPP_INLINE int f16_nge_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x1a
BX_CPP_INLINE int f16_ngt_unordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x1b
BX_CPP_INLINE int f16_false_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   f16_compare(a, b, status);
   return 0;
}

// 0x1c
BX_CPP_INLINE int f16_neq_ordered_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x1d
BX_CPP_INLINE int f16_ge_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x1e
BX_CPP_INLINE int f16_gt_ordered_quiet(float16 a, float16 b, softfloat_status_t *status)
{
   int relation = f16_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x1f
BX_CPP_INLINE int f16_true_signalling(float16 a, float16 b, softfloat_status_t *status)
{
   f16_compare(a, b, status);
   return 1;
}

// ======= float32 ======= //

typedef int (*float32_compare_method)(float32, float32, softfloat_status_t *status);

// 0x00
BX_CPP_INLINE int f32_eq_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x01
BX_CPP_INLINE int f32_lt_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x02
BX_CPP_INLINE int f32_le_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x03
BX_CPP_INLINE int f32_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x04
BX_CPP_INLINE int f32_neq_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x05
BX_CPP_INLINE int f32_nlt_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x06
BX_CPP_INLINE int f32_nle_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x07
BX_CPP_INLINE int f32_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x08
BX_CPP_INLINE int f32_eq_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x09
BX_CPP_INLINE int f32_nge_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x0a
BX_CPP_INLINE int f32_ngt_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x0b
BX_CPP_INLINE int f32_false_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   f32_compare_quiet(a, b, status);
   return 0;
}

// 0x0c
BX_CPP_INLINE int f32_neq_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x0d
BX_CPP_INLINE int f32_ge_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x0e
BX_CPP_INLINE int f32_gt_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x0f
BX_CPP_INLINE int f32_true_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   f32_compare_quiet(a, b, status);
   return 1;
}

// 0x10
BX_CPP_INLINE int f32_eq_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x11
BX_CPP_INLINE int f32_lt_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x12
BX_CPP_INLINE int f32_le_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x13
BX_CPP_INLINE int f32_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x14
BX_CPP_INLINE int f32_neq_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x15
BX_CPP_INLINE int f32_nlt_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x16
BX_CPP_INLINE int f32_nle_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x17
BX_CPP_INLINE int f32_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x18
BX_CPP_INLINE int f32_eq_unordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x19
BX_CPP_INLINE int f32_nge_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x1a
BX_CPP_INLINE int f32_ngt_unordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x1b
BX_CPP_INLINE int f32_false_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   f32_compare(a, b, status);
   return 0;
}

// 0x1c
BX_CPP_INLINE int f32_neq_ordered_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x1d
BX_CPP_INLINE int f32_ge_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x1e
BX_CPP_INLINE int f32_gt_ordered_quiet(float32 a, float32 b, softfloat_status_t *status)
{
   int relation = f32_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x1f
BX_CPP_INLINE int f32_true_signalling(float32 a, float32 b, softfloat_status_t *status)
{
   f32_compare(a, b, status);
   return 1;
}

// ======= float64 ======= //

typedef int (*float64_compare_method)(float64, float64, softfloat_status_t *status);

// 0x00
BX_CPP_INLINE int f64_eq_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x01
BX_CPP_INLINE int f64_lt_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x02
BX_CPP_INLINE int f64_le_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x03
BX_CPP_INLINE int f64_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x04
BX_CPP_INLINE int f64_neq_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x05
BX_CPP_INLINE int f64_nlt_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x06
BX_CPP_INLINE int f64_nle_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x07
BX_CPP_INLINE int f64_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x08
BX_CPP_INLINE int f64_eq_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x09
BX_CPP_INLINE int f64_nge_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x0a
BX_CPP_INLINE int f64_ngt_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x0b
BX_CPP_INLINE int f64_false_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   f64_compare_quiet(a, b, status);
   return 0;
}

// 0x0c
BX_CPP_INLINE int f64_neq_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x0d
BX_CPP_INLINE int f64_ge_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x0e
BX_CPP_INLINE int f64_gt_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x0f
BX_CPP_INLINE int f64_true_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   f64_compare_quiet(a, b, status);
   return 1;
}

// 0x10
BX_CPP_INLINE int f64_eq_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_equal);
}

// 0x11
BX_CPP_INLINE int f64_lt_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less);
}

// 0x12
BX_CPP_INLINE int f64_le_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_equal);
}

// 0x13
BX_CPP_INLINE int f64_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_unordered);
}

// 0x14
BX_CPP_INLINE int f64_neq_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_equal);
}

// 0x15
BX_CPP_INLINE int f64_nlt_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less);
}

// 0x16
BX_CPP_INLINE int f64_nle_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_less) && (relation != softfloat_relation_equal);
}

// 0x17
BX_CPP_INLINE int f64_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_unordered);
}

// 0x18
BX_CPP_INLINE int f64_eq_unordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation == softfloat_relation_equal) || (relation == softfloat_relation_unordered);
}

// 0x19
BX_CPP_INLINE int f64_nge_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_less) || (relation == softfloat_relation_unordered);
}

// 0x1a
BX_CPP_INLINE int f64_ngt_unordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation != softfloat_relation_greater);
}

// 0x1b
BX_CPP_INLINE int f64_false_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   f64_compare(a, b, status);
   return 0;
}

// 0x1c
BX_CPP_INLINE int f64_neq_ordered_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare(a, b, status);
   return (relation != softfloat_relation_equal) && (relation != softfloat_relation_unordered);
}

// 0x1d
BX_CPP_INLINE int f64_ge_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater) || (relation == softfloat_relation_equal);
}

// 0x1e
BX_CPP_INLINE int f64_gt_ordered_quiet(float64 a, float64 b, softfloat_status_t *status)
{
   int relation = f64_compare_quiet(a, b, status);
   return (relation == softfloat_relation_greater);
}

// 0x1f
BX_CPP_INLINE int f64_true_signalling(float64 a, float64 b, softfloat_status_t *status)
{
   f64_compare(a, b, status);
   return 1;
}

#endif
