/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 3e, written for Bochs (x86 achitecture simulator)
floating point emulation.

Internal extended-precision arithmetic primitives for the x87 transcendental
instructions (F2XM1, FYL2X/FYL2XP1, FPATAN, FSIN/FCOS/FSINCOS/FPTAN).
----------------------------------------------------------------------------
Historically the x87 evaluates the elementary functions in a wider-than-80-bit
internal format and every step of the polynomial / table evaluation rounds to a
fixed narrow width - not to the programmed FPU precision.  To reproduce the exact
last bit of the hardware result we must model those exact intermediate roundings.
Two internal widths are used:

  "_67_chop"  : the value is kept to a ~67-bit significand and TRUNCATED toward
                zero (this is the width the multiplier/adder feed forward).
  "_64_ne"    : the value is delivered rounded to a 64-bit significand,
                round-to-nearest-even, independent of the programmed mode.

Multiplication is asymmetric: the left operand contributes the full ~67-bit
significand, but only the top 64 bits of the right operand enter the multiplier -
so  mul(x,y)  is NOT  mul(y,x).  'f128_mul_e67' / 'f128_mul_64_ne' make that
right-operand truncation explicit; 'f128_mul_67_chop' relies on its right operand
already being <= 64 significant bits.

All of these helpers save and restore the caller's programmed rounding state, so
the only rounding that reaches SW.C1 / the destination is the caller's final one.
=============================================================================*/

#include "softfloat3e/include/softfloat.h"

#define F128_EXT_FORMAT_PRECISION 83   /* s_roundPackToF128 clamp value -> 67-bit significand */

// Round a float128_t to a 64-bit significand using 'mode', regardless of the
// rounding mode currently programmed in 'status'.  ('f128_round' with the
// x87 precision selector 80 -> 64-bit significand; softfloat3e/f128_round.cc.)
static float128_t f128_round_to_64(float128_t v, uint8_t mode, struct softfloat_status_t *status)
{
    return f128_round(v, 80, mode, status);
}

// multiply, result truncated to a ~67-bit significand.  Assumes 'b' already
// carries <= 64 significant bits (true for every call site: 'b' is a constant,
// a prior _64_ne result, or a value with only the top bits populated).
float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_mul(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

// multiply, result truncated to ~67 bits, with the right operand first reduced
// (toward zero) to a 64-bit significand - used where 'b' may be wider than 64
// bits (e.g. r*r, s*s in the sin/cos polynomial).
float128_t f128_mul_e67(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    extFloat80_t b64 = f128_to_extF80(b, softfloat_round_minMag, status);
    float128_t bt = extF80_to_f128(b64, status);
    return f128_mul_67_chop(a, bt, status);
}

// add / subtract, result truncated to a ~67-bit significand.
float128_t f128_add_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_add(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

// divide, result truncated to a ~67-bit significand.
float128_t f128_div_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_div(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

// add, result rounded to a 64-bit significand, nearest-even.  This is what every
// polynomial Horner step uses.
float128_t f128_add_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    float128_t sum = f128_add(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(sum, softfloat_round_near_even, status);
}

// divide, result rounded to a 64-bit significand, nearest-even.
float128_t f128_div_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    float128_t quotient = f128_div(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(quotient, softfloat_round_near_even, status);
}

// multiply with only the top 64 bits of the right operand entering the
// multiplier, result rounded to a 64-bit significand, nearest-even.  Overloaded
// on the right operand: pass it as extFloat80_t when it originates as one (no
// widen-then-narrow), or as float128_t (truncated here toward zero to 64 bits).
float128_t f128_mul_64_ne(float128_t a, extFloat80_t b, struct softfloat_status_t *status)
{
    float128_t product = f128_mul_by_extF80(a, b, softfloat_round_near_even, status);
    return f128_round_to_64(product, softfloat_round_near_even, status);
}

// multiply, result rounded to a 64-bit significand, nearest-even
float128_t f128_mul_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    extFloat80_t b64 = f128_to_extF80(b, softfloat_round_minMag, status);
    return f128_mul_64_ne(a, b64, status);
}
