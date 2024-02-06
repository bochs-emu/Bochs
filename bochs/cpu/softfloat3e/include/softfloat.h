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

/*============================================================================
| Note:  If SoftFloat is made available as a general library for programs to
| use, it is strongly recommended that a platform-specific version of this
| header, "softfloat.h", be created that folds in "softfloat_types.h" and that
| eliminates all dependencies on compile-time macros.
*============================================================================*/

#ifndef softfloat_h
#define softfloat_h 1

#include <stdbool.h>
#include <stdint.h>
#include "softfloat_types.h"

struct softfloat_status_t
{
    uint8_t softfloat_roundingMode;
    int softfloat_exceptionFlags;
    int softfloat_exceptionMasks;

    bool softfloat_denormals_are_zeros;
    bool softfloat_flush_underflow_to_zero;

    /*----------------------------------------------------------------------------
    | Rounding precision for 80-bit extended double-precision floating-point.
    | Valid values are 32, 64, and 80.
    *----------------------------------------------------------------------------*/
    uint8_t extF80_roundingPrecision;

#define float_rounding_mode softfloat_roundingMode
#define float_exception_flags softfloat_exceptionFlags
#define flush_underflow_to_zero softfloat_flush_underflow_to_zero
#define denormals_are_zeros softfloat_denormals_are_zeros
#define float_rounding_precision extF80_roundingPrecision
#define float_exception_masks softfloat_exceptionMasks

    int float_suppress_exception;
    int float_nan_handling_mode;    /* flag register */     // might be redundant
};

typedef struct softfloat_status_t float_status_t;

/*----------------------------------------------------------------------------
| Software floating-point rounding mode.  (Mode "odd" is supported only if
| SoftFloat is compiled with macro 'SOFTFLOAT_ROUND_ODD' defined.)
*----------------------------------------------------------------------------*/
enum {
    softfloat_round_near_even   = 0,
    softfloat_round_min         = 1,
    softfloat_round_down        = softfloat_round_min,
    softfloat_round_max         = 2,
    softfloat_round_up          = softfloat_round_max,
    softfloat_round_minMag      = 3,
    softfloat_round_to_zero     = softfloat_round_minMag,
    softfloat_round_near_maxMag = 4,
    softfloat_round_odd         = 6
};

/*----------------------------------------------------------------------------
| Software floating-point exception flags.
*----------------------------------------------------------------------------*/
enum softfloat_exception_flag_t {
    softfloat_flag_invalid   = 0x01,
    softfloat_flag_denormal  = 0x02,
    softfloat_flag_divbyzero = 0x04,
    softfloat_flag_infinite  = softfloat_flag_divbyzero,
    softfloat_flag_overflow  = 0x08,
    softfloat_flag_underflow = 0x10,
    softfloat_flag_inexact   = 0x20
};

/*----------------------------------------------------------------------------
| Software IEC/IEEE floating-point ordering relations
*----------------------------------------------------------------------------*/
enum {
    softfloat_relation_less      = -1,
    softfloat_relation_equal     =  0,
    softfloat_relation_greater   =  1,
    softfloat_relation_unordered =  2
};

/*----------------------------------------------------------------------------
| Software IEC/IEEE floating-point class.
*----------------------------------------------------------------------------*/
typedef enum {
    softfloat_zero,
    softfloat_SNaN,
    softfloat_QNaN,
    softfloat_negative_inf,
    softfloat_positive_inf,
    softfloat_denormal,
    softfloat_normalized
} softfloat_class_t;

/*----------------------------------------------------------------------------
| Options to indicate which negations to perform in f*_muladd()
| Using these differs from negating an input or output before calling
| the muladd function in that this means that a NaN doesn't have its
| sign bit inverted before it is propagated.
*----------------------------------------------------------------------------*/
enum {
    softfloat_mulAdd_subC           = 1,
    softfloat_muladd_negate_c       = softfloat_mulAdd_subC,
    softfloat_mulAdd_subProd        = 2,
    softfloat_muladd_negate_product = softfloat_mulAdd_subProd,
    softfloat_muladd_negate_result  = softfloat_muladd_negate_c | softfloat_muladd_negate_product
};

/*----------------------------------------------------------------------------
| Routine to raise any or all of the software floating-point exception flags.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE void softfloat_raiseFlags(struct softfloat_status_t *status, int flags) {
    status->softfloat_exceptionFlags |= flags;
}

/*----------------------------------------------------------------------------
| Check if exception is masked.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE int softfloat_isMaskedException(struct softfloat_status_t *status, int flags) {
    return status->softfloat_exceptionMasks & flags;
}

/*----------------------------------------------------------------------------
| Obtain current rounding mode.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE uint8_t softfloat_getRoundingMode(struct softfloat_status_t *status) {
    return status->softfloat_roundingMode;
}

/*----------------------------------------------------------------------------
| Read denormals-are-zeroes flag.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE bool softfloat_denormalsAreZeros(struct softfloat_status_t *status) {
    return status->softfloat_denormals_are_zeros;
}

/*----------------------------------------------------------------------------
| Read flush-underflow-to-zero flag.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE bool softfloat_flushUnderflowToZero(struct softfloat_status_t *status) {
    return status->softfloat_flush_underflow_to_zero;
}

/*----------------------------------------------------------------------------
| Obtain current rounding precision for F80.
*----------------------------------------------------------------------------*/
BX_CPP_INLINE uint8_t softfloat_extF80_roundingPrecision(struct softfloat_status_t *status) {
    return status->extF80_roundingPrecision;
}

/*----------------------------------------------------------------------------
| Integer-to-floating-point conversion routines.
*----------------------------------------------------------------------------*/
float16_t ui16_to_f16(uint16_t, struct softfloat_status_t *);
float16_t ui32_to_f16(uint32_t, struct softfloat_status_t *);
float32_t ui32_to_f32(uint32_t, struct softfloat_status_t *);
float64_t ui32_to_f64(uint32_t);
extFloat80_t ui32_to_extF80(uint32_t);
float128_t ui32_to_f128(uint32_t);
float16_t ui64_to_f16(uint64_t, struct softfloat_status_t *);
float32_t ui64_to_f32(uint64_t, struct softfloat_status_t *);
float64_t ui64_to_f64(uint64_t, struct softfloat_status_t *);
extFloat80_t ui64_to_extF80(uint64_t);
float128_t ui64_to_f128(uint64_t);
float16_t i16_to_f16(int16_t, struct softfloat_status_t *);
float16_t i32_to_f16(int32_t, struct softfloat_status_t *);
float32_t i32_to_f32(int32_t, struct softfloat_status_t *);
float64_t i32_to_f64(int32_t);
extFloat80_t i32_to_extF80(int32_t);
float128_t i32_to_f128(int32_t);
float16_t i64_to_f16(int64_t, struct softfloat_status_t *);
float32_t i64_to_f32(int64_t, struct softfloat_status_t *);
float64_t i64_to_f64(int64_t, struct softfloat_status_t *);
extFloat80_t i64_to_extF80(int64_t);
float128_t i64_to_f128(int64_t);

BX_CPP_INLINE float16_t i16_to_f16(int16_t a, struct softfloat_status_t *status) {
  return i32_to_f16((int32_t)(a), status);
}

BX_CPP_INLINE float16_t ui16_to_f16(uint16_t a, struct softfloat_status_t *status) {
  return ui32_to_f16((uint32_t)(a), status);
}

/*----------------------------------------------------------------------------
| 16-bit (half-precision) floating-point operations.
*----------------------------------------------------------------------------*/
uint32_t f16_to_ui32(float16_t, uint8_t, bool, struct softfloat_status_t *);
uint64_t f16_to_ui64(float16_t, uint8_t, bool, struct softfloat_status_t *);
int32_t f16_to_i32(float16_t, uint8_t, bool, struct softfloat_status_t *);
int64_t f16_to_i64(float16_t, uint8_t, bool, struct softfloat_status_t *);
uint32_t f16_to_ui32_r_minMag(float16_t, bool, struct softfloat_status_t *);
uint64_t f16_to_ui64_r_minMag(float16_t, bool, struct softfloat_status_t *);
int32_t f16_to_i32_r_minMag(float16_t, bool, struct softfloat_status_t *);
int64_t f16_to_i64_r_minMag(float16_t, bool, struct softfloat_status_t *);
float32_t f16_to_f32(float16_t, struct softfloat_status_t *);
float64_t f16_to_f64(float16_t, struct softfloat_status_t *);
extFloat80_t f16_to_extF80(float16_t, struct softfloat_status_t *);
float16_t f16_roundToInt(float16_t, uint8_t, uint8_t, bool, struct softfloat_status_t *);
float16_t f16_add(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_sub(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_mul(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_mulAdd(float16_t, float16_t, float16_t, uint8_t op, struct softfloat_status_t *);
float16_t f16_div(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_min(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_max(float16_t, float16_t, struct softfloat_status_t *);
float16_t f16_getExp(float16_t, struct softfloat_status_t *);
int f16_compare(float16_t, float16_t, bool, struct softfloat_status_t *);
float16_t f16_sqrt(float16_t, struct softfloat_status_t *);
softfloat_class_t f16_class(float16_t);

bool f16_isSignalingNaN(float16_t);
bool f16_isNaN(float16_t);

bool f16_sign(float16_t);
int8_t f16_exp(float16_t);
uint16_t f16_fraction(float16_t);
float16_t f16_denormal_to_zero(float16_t);

BX_CPP_INLINE int f16_compare(float16_t a, float16_t b, softfloat_status_t *status) {
  return f16_compare(a, b, 0, status);
}

BX_CPP_INLINE int f16_compare_quiet(float16_t a, float16_t b, softfloat_status_t *status) {
  return f16_compare(a, b, 1, status);
}

BX_CPP_INLINE float16_t f16_roundToInt(float16_t a, uint8_t scale, struct softfloat_status_t *status) {
    return f16_roundToInt(a, scale, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE float16_t f16_roundToInt(float16_t a, struct softfloat_status_t *status) {
    return f16_roundToInt(a, 0, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f16_to_i32(float16_t a, struct softfloat_status_t *status) {
    return f16_to_i32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE int64_t f16_to_i64(float16_t a, struct softfloat_status_t *status) {
    return f16_to_i64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f16_to_i32_round_to_zero(float16_t a, struct softfloat_status_t *status) {
    return f16_to_i32_r_minMag(a, true, status);
}
BX_CPP_INLINE int64_t f16_to_i64_round_to_zero(float16_t a, struct softfloat_status_t *status) {
    return f16_to_i64_r_minMag(a, true, status);
}

BX_CPP_INLINE uint32_t f16_to_ui32(float16_t a, struct softfloat_status_t *status) {
    return f16_to_ui32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE uint64_t f16_to_ui64(float16_t a, struct softfloat_status_t *status) {
    return f16_to_ui64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE uint32_t f16_to_ui32_round_to_zero(float16_t a, struct softfloat_status_t *status) {
    return f16_to_ui32_r_minMag(a, true, status);
}
BX_CPP_INLINE uint64_t f16_to_ui64_round_to_zero(float16_t a, struct softfloat_status_t *status) {
    return f16_to_ui64_r_minMag(a, true, status);
}

BX_CPP_INLINE float16_t f16_fmadd(float16_t a, float16_t b, float16_t c, softfloat_status_t *status) {
  return f16_mulAdd(a, b, c, 0, status);
}
BX_CPP_INLINE float16_t f16_fmsub(float16_t a, float16_t b, float16_t c, softfloat_status_t *status) {
  return f16_mulAdd(a, b, c, softfloat_muladd_negate_c, status);
}
BX_CPP_INLINE float16_t f16_fnmadd(float16_t a, float16_t b, float16_t c, softfloat_status_t *status) {
  return f16_mulAdd(a, b, c, softfloat_muladd_negate_product, status);
}
BX_CPP_INLINE float16_t f16_fnmsub(float16_t a, float16_t b, float16_t c, softfloat_status_t *status) {
  return f16_mulAdd(a, b, c, softfloat_muladd_negate_result, status);
}

/*----------------------------------------------------------------------------
| 32-bit (single-precision) floating-point operations.
*----------------------------------------------------------------------------*/
uint32_t f32_to_ui32(float32_t, uint8_t, bool, struct softfloat_status_t *);
uint64_t f32_to_ui64(float32_t, uint8_t, bool, struct softfloat_status_t *);
int32_t f32_to_i32(float32_t, uint8_t, bool, struct softfloat_status_t *);
int64_t f32_to_i64(float32_t, uint8_t, bool, struct softfloat_status_t *);
uint32_t f32_to_ui32_r_minMag(float32_t, bool, struct softfloat_status_t *);
uint64_t f32_to_ui64_r_minMag(float32_t, bool, struct softfloat_status_t *);
int32_t f32_to_i32_r_minMag(float32_t, bool, struct softfloat_status_t *);
int64_t f32_to_i64_r_minMag(float32_t, bool, struct softfloat_status_t *);
float16_t f32_to_f16(float32_t, struct softfloat_status_t *);
float64_t f32_to_f64(float32_t, struct softfloat_status_t *);
extFloat80_t f32_to_extF80(float32_t, struct softfloat_status_t *);
float128_t f32_to_f128(float32_t, struct softfloat_status_t *);
float32_t f32_roundToInt(float32_t, uint8_t, uint8_t, bool, struct softfloat_status_t *);
float32_t f32_add(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_sub(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_mul(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_mulAdd(float32_t, float32_t, float32_t, uint8_t op, struct softfloat_status_t *);
float32_t f32_div(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_min(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_max(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_scalef(float32_t, float32_t, struct softfloat_status_t *);
float32_t f32_getExp(float32_t, struct softfloat_status_t *);
int f32_compare(float32_t, float32_t, bool, struct softfloat_status_t *);
float32_t f32_sqrt(float32_t, struct softfloat_status_t *);
softfloat_class_t f32_class(float32_t);

bool f32_isSignalingNaN(float32_t);
bool f32_isNaN(float32_t);

bool f32_sign(float32_t);
int16_t f32_exp(float32_t);
uint32_t f32_fraction(float32_t);
float32_t f32_denormal_to_zero(float32_t);

BX_CPP_INLINE int f32_compare(float32_t a, float32_t b, softfloat_status_t *status) {
  return f32_compare(a, b, 0, status);
}

BX_CPP_INLINE int f32_compare_quiet(float32_t a, float32_t b, softfloat_status_t *status) {
  return f32_compare(a, b, 1, status);
}

BX_CPP_INLINE float32_t f32_roundToInt(float32_t a, uint8_t scale, struct softfloat_status_t *status) {
    return f32_roundToInt(a, scale, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE float32_t f32_roundToInt(float32_t a, struct softfloat_status_t *status) {
    return f32_roundToInt(a, 0, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f32_to_i32(float32_t a, struct softfloat_status_t *status) {
    return f32_to_i32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE int64_t f32_to_i64(float32_t a, struct softfloat_status_t *status) {
    return f32_to_i64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f32_to_i32_round_to_zero(float32_t a, struct softfloat_status_t *status) {
    return f32_to_i32_r_minMag(a, true, status);
}
BX_CPP_INLINE int64_t f32_to_i64_round_to_zero(float32_t a, struct softfloat_status_t *status) {
    return f32_to_i64_r_minMag(a, true, status);
}

BX_CPP_INLINE uint32_t f32_to_ui32(float32_t a, struct softfloat_status_t *status) {
    return f32_to_ui32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE uint64_t f32_to_ui64(float32_t a, struct softfloat_status_t *status) {
    return f32_to_ui64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE uint32_t f32_to_ui32_round_to_zero(float32_t a, struct softfloat_status_t *status) {
    return f32_to_ui32_r_minMag(a, true, status);
}
BX_CPP_INLINE uint64_t f32_to_ui64_round_to_zero(float32_t a, struct softfloat_status_t *status) {
    return f32_to_ui64_r_minMag(a, true, status);
}

BX_CPP_INLINE float32_t f32_fmadd(float32_t a, float32_t b, float32_t c, softfloat_status_t *status) {
  return f32_mulAdd(a, b, c, 0, status);
}
BX_CPP_INLINE float32_t f32_fmsub(float32_t a, float32_t b, float32_t c, softfloat_status_t *status) {
  return f32_mulAdd(a, b, c, softfloat_muladd_negate_c, status);
}
BX_CPP_INLINE float32_t f32_fnmadd(float32_t a, float32_t b, float32_t c, softfloat_status_t *status) {
  return f32_mulAdd(a, b, c, softfloat_muladd_negate_product, status);
}
BX_CPP_INLINE float32_t f32_fnmsub(float32_t a, float32_t b, float32_t c, softfloat_status_t *status) {
  return f32_mulAdd(a, b, c, softfloat_muladd_negate_result, status);
}

/*----------------------------------------------------------------------------
| 64-bit (double-precision) floating-point operations.
*----------------------------------------------------------------------------*/
uint32_t f64_to_ui32(float64_t, uint8_t, bool, struct softfloat_status_t *);
uint64_t f64_to_ui64(float64_t, uint8_t, bool, struct softfloat_status_t *);
int32_t f64_to_i32(float64_t, uint8_t, bool, struct softfloat_status_t *);
int64_t f64_to_i64(float64_t, uint8_t, bool, struct softfloat_status_t *);
uint32_t f64_to_ui32_r_minMag(float64_t, bool, struct softfloat_status_t *);
uint64_t f64_to_ui64_r_minMag(float64_t, bool, struct softfloat_status_t *);
int32_t f64_to_i32_r_minMag(float64_t, bool, struct softfloat_status_t *);
int64_t f64_to_i64_r_minMag(float64_t, bool, struct softfloat_status_t *);
float16_t f64_to_f16(float64_t, struct softfloat_status_t *);
float32_t f64_to_f32(float64_t, struct softfloat_status_t *);
extFloat80_t f64_to_extF80(float64_t, struct softfloat_status_t *);
float128_t f64_to_f128(float64_t, struct softfloat_status_t *);
float64_t f64_roundToInt(float64_t, uint8_t, uint8_t, bool, struct softfloat_status_t *);
float64_t f64_add(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_sub(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_mul(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_mulAdd(float64_t, float64_t, float64_t, uint8_t op, struct softfloat_status_t *);
float64_t f64_div(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_min(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_max(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_scalef(float64_t, float64_t, struct softfloat_status_t *);
float64_t f64_getExp(float64_t, struct softfloat_status_t *);
int f64_compare(float64_t, float64_t, bool, struct softfloat_status_t *);
float64_t f64_sqrt(float64_t, struct softfloat_status_t *);
softfloat_class_t f64_class(float64_t);

bool f64_isSignalingNaN(float64_t);
bool f64_isNaN(float64_t);

bool f64_sign(float64_t);
int16_t f64_exp(float64_t);
uint64_t f64_fraction(float64_t);
float64_t f64_denormal_to_zero(float64_t);

BX_CPP_INLINE int f64_compare(float64_t a, float64_t b, softfloat_status_t *status) {
  return f64_compare(a, b, 0, status);
}

BX_CPP_INLINE int f64_compare_quiet(float64_t a, float64_t b, softfloat_status_t *status) {
  return f64_compare(a, b, 1, status);
}

BX_CPP_INLINE float64_t f64_roundToInt(float64_t a, uint8_t scale, struct softfloat_status_t *status) {
    return f64_roundToInt(a, scale, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE float64_t f64_roundToInt(float64_t a, struct softfloat_status_t *status) {
    return f64_roundToInt(a, 0, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f64_to_i32(float64_t a, struct softfloat_status_t *status) {
    return f64_to_i32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE int64_t f64_to_i64(float64_t a, struct softfloat_status_t *status) {
    return f64_to_i64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE int32_t f64_to_i32_round_to_zero(float64_t a, struct softfloat_status_t *status) {
    return f64_to_i32_r_minMag(a, true, status);
}
BX_CPP_INLINE int64_t f64_to_i64_round_to_zero(float64_t a, struct softfloat_status_t *status) {
    return f64_to_i64_r_minMag(a, true, status);
}

BX_CPP_INLINE uint32_t f64_to_ui32(float64_t a, struct softfloat_status_t *status) {
    return f64_to_ui32(a, softfloat_getRoundingMode(status), true, status);
}
BX_CPP_INLINE uint64_t f64_to_ui64(float64_t a, struct softfloat_status_t *status) {
    return f64_to_ui64(a, softfloat_getRoundingMode(status), true, status);
}

BX_CPP_INLINE uint32_t f64_to_ui32_round_to_zero(float64_t a, struct softfloat_status_t *status) {
    return f64_to_ui32_r_minMag(a, true, status);
}
BX_CPP_INLINE uint64_t f64_to_ui64_round_to_zero(float64_t a, struct softfloat_status_t *status) {
    return f64_to_ui64_r_minMag(a, true, status);
}

BX_CPP_INLINE float64_t f64_fmadd(float64_t a, float64_t b, float64_t c, softfloat_status_t *status) {
  return f64_mulAdd(a, b, c, 0, status);
}
BX_CPP_INLINE float64_t f64_fmsub(float64_t a, float64_t b, float64_t c, softfloat_status_t *status) {
  return f64_mulAdd(a, b, c, softfloat_muladd_negate_c, status);
}
BX_CPP_INLINE float64_t f64_fnmadd(float64_t a, float64_t b, float64_t c, softfloat_status_t *status) {
  return f64_mulAdd(a, b, c, softfloat_muladd_negate_product, status);
}
BX_CPP_INLINE float64_t f64_fnmsub(float64_t a, float64_t b, float64_t c, softfloat_status_t *status) {
  return f64_mulAdd(a, b, c, softfloat_muladd_negate_result, status);
}

/*----------------------------------------------------------------------------
| 80-bit extended double-precision floating-point operations.
*----------------------------------------------------------------------------*/
uint32_t extF80_to_ui32(extFloat80_t, uint8_t, bool, struct softfloat_status_t *);
uint64_t extF80_to_ui64(extFloat80_t, uint8_t, bool, struct softfloat_status_t *);
int32_t extF80_to_i32(extFloat80_t, uint8_t, bool, struct softfloat_status_t *);
int64_t extF80_to_i64(extFloat80_t, uint8_t, bool, struct softfloat_status_t *);
uint32_t extF80_to_ui32_r_minMag(extFloat80_t, bool, struct softfloat_status_t *);
uint64_t extF80_to_ui64_r_minMag(extFloat80_t, bool, struct softfloat_status_t *);
int32_t extF80_to_i32_r_minMag(extFloat80_t, bool, struct softfloat_status_t *);
int64_t extF80_to_i64_r_minMag(extFloat80_t, bool, struct softfloat_status_t *);
float16_t extF80_to_f16(extFloat80_t, struct softfloat_status_t *);
float32_t extF80_to_f32(extFloat80_t, struct softfloat_status_t *);
float64_t extF80_to_f64(extFloat80_t, struct softfloat_status_t *);
float128_t extF80_to_f128(extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_roundToInt(extFloat80_t, uint8_t, bool, struct softfloat_status_t *);
extFloat80_t extF80_add(extFloat80_t, extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_sub(extFloat80_t, extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_mul(extFloat80_t, extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_div(extFloat80_t, extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_rem(extFloat80_t, extFloat80_t, struct softfloat_status_t *);
extFloat80_t extF80_sqrt(extFloat80_t, struct softfloat_status_t *);
bool extF80_isSignalingNaN(extFloat80_t);
bool extF80_isNaN(extFloat80_t);

/*----------------------------------------------------------------------------
| 128-bit (quadruple-precision) floating-point operations.
*----------------------------------------------------------------------------*/
uint32_t f128_to_ui32(float128_t, uint8_t, bool, struct softfloat_status_t *);
uint64_t f128_to_ui64(float128_t, uint8_t, bool, struct softfloat_status_t *);
int32_t f128_to_i32(float128_t, uint8_t, bool, struct softfloat_status_t *);
int64_t f128_to_i64(float128_t, uint8_t, bool, struct softfloat_status_t *);
uint32_t f128_to_ui32_r_minMag(float128_t, bool, struct softfloat_status_t *);
uint64_t f128_to_ui64_r_minMag(float128_t, bool, struct softfloat_status_t *);
int32_t f128_to_i32_r_minMag(float128_t, bool, struct softfloat_status_t *);
int64_t f128_to_i64_r_minMag(float128_t, bool, struct softfloat_status_t *);
float32_t f128_to_f32(float128_t, struct softfloat_status_t *);
float64_t f128_to_f64(float128_t, struct softfloat_status_t *);
extFloat80_t f128_to_extF80(float128_t, struct softfloat_status_t *);
float128_t f128_roundToInt(float128_t, uint8_t, bool, struct softfloat_status_t *);
float128_t f128_add(float128_t, float128_t, struct softfloat_status_t *);
float128_t f128_sub(float128_t, float128_t, struct softfloat_status_t *);
float128_t f128_mul(float128_t, float128_t, struct softfloat_status_t *);
float128_t f128_mulAdd(float128_t, float128_t, float128_t, uint8_t op, struct softfloat_status_t *);
float128_t f128_div(float128_t, float128_t, struct softfloat_status_t *);
float128_t f128_sqrt(float128_t, struct softfloat_status_t *);
bool f128_isSignalingNaN(float128_t);
bool f128_isNaN(float128_t);

#include "softfloat-extra.h"

#endif
