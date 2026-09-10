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
                zero - f128_*(.., minMag, 83) hits the narrow-precision clamp in
                softfloat_roundPackToF128.
  "_64_ne"    : the value is delivered rounded to a 64-bit (extF80) significand,
                round-to-nearest-even, independent of the programmed mode.  The
                wide operation is rounded to 113 bits and then re-rounded to 64
                (f128_round) - the same wide-then-destination double rounding the
                hardware's internal format performs.

Multiplication is asymmetric: the left operand contributes the full ~67-bit
significand, but only the top 64 bits of the right operand enter the multiplier -
so  mul(x,y)  is NOT  mul(y,x).  'f128_mul_e67' / 'f128_mul_64_ne' make that
right-operand truncation explicit; 'f128_mul_67_chop' relies on its right operand
already being <= 64 significant bits.
=============================================================================*/

#include "softfloat3e/include/softfloat.h"

#define F128_WIDE_PRECISION  83   /* -> 67-bit significand (the wide internal format) */

// Round a float128_t to a 64-bit (extF80) significand using 'mode', regardless
// of the rounding mode currently programmed in 'status'.
static float128_t f128_round_to_64(float128_t v, uint8_t mode, struct softfloat_status_t *status)
{
    return f128_round(v, 80, mode, status);
}

// multiply, result truncated to a ~67-bit significand.  Assumes 'b' already
// carries <= 64 significant bits (true for every call site: 'b' is a constant,
// a prior _64_ne result, or a value with only the top bits populated).
float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    return f128_mul(a, b, softfloat_round_minMag, F128_WIDE_PRECISION, status);
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
    return f128_add(a, b, softfloat_round_minMag, F128_WIDE_PRECISION, status);
}

// divide, result truncated to a ~67-bit significand.
float128_t f128_div_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    return f128_div(a, b, softfloat_round_minMag, F128_WIDE_PRECISION, status);
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
