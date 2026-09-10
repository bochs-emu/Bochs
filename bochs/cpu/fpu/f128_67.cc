/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 3e, written for Bochs (x86 achitecture simulator)
floating point emulation.

Emulating the P5/P6 real microcode: x87 transcendentals work in an internal
extended-precision format.  The "_67_chop" forms keep a ~67-bit significand and
truncate; the "_64_ne" forms deliver a 64-bit significand rounded to nearest-
even.  (mul_e64 / mul_e64_ne are 'f128_mul_by_extF80'.)  These helpers wrap the
ordinary float128_t operations and leave the caller's programmed rounding state
untouched.
=============================================================================*/

#include "softfloat3e/include/softfloat.h"

#define F128_EXT_FORMAT_PRECISION 83   /* -> 67-bit significand */

// Round a float128_t to a 64-bit (extF80) significand using 'mode', regardless
// of the rounding mode currently programmed in 'status'.
static float128_t f128_round_to_64(float128_t v, uint8_t mode, struct softfloat_status_t *status)
{
    extFloat80_t r64 = f128_to_extF80(v, mode, status);
    return extF80_to_f128(r64, status);
}

float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_mul(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

float128_t f128_add_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_add(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

float128_t f128_div_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_div(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

float128_t f128_add_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    float128_t sum = f128_add(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(sum, softfloat_round_near_even, status);
}

float128_t f128_div_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    float128_t quotient = f128_div(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(quotient, softfloat_round_near_even, status);
}

// P5 'mul_e64_ne': 'a' contributes ~67 significand bits, only the top 64 bits
// of the right operand enter the multiplier, and the product is delivered
// rounded to a 64-bit significand, nearest-even.
float128_t f128_mul_64_ne(float128_t a, extFloat80_t b, struct softfloat_status_t *status)
{
    float128_t product = f128_mul_by_extF80(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(product, softfloat_round_near_even, status);
}

float128_t f128_mul_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    extFloat80_t b64 = f128_to_extF80(b, softfloat_round_minMag, status);
    return f128_mul_64_ne(a, b64, status);
}
