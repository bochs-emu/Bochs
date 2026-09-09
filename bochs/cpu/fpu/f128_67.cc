/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 3e, written for Bochs (x86 achitecture simulator)
floating point emulation.

Emulating the P5/P6 real microcode: x87 transcendentals work in an internal
extended-precision format.  Multiplies keep a ~67-bit significand and truncate;
adds deliver a 64-bit significand rounded to nearest-even.  These helpers wrap
the ordinary float128_t multiply / add accordingly.  For the multiply the
target width is driven by the 'extF80_roundingPrecision' field of the status
word; the field (and, for the add, the rounding mode) are saved and restored so
the caller's programmed state is left untouched.
=============================================================================*/

#include "softfloat3e/include/softfloat.h"

#define F128_EXT_FORMAT_PRECISION 83   /* -> 67-bit significand */

float128_t f128_mul_67_chop(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    uint8_t savedPrecision = status->extF80_roundingPrecision;
    status->extF80_roundingPrecision = F128_EXT_FORMAT_PRECISION;
    float128_t z = f128_mul(a, b, softfloat_round_minMag, status);
    status->extF80_roundingPrecision = savedPrecision;
    return z;
}

float128_t f128_add_64_ne(float128_t a, float128_t b, struct softfloat_status_t *status)
{
    float128_t sum = f128_add(a, b, softfloat_round_near_even, status);

    uint8_t savedMode = status->softfloat_roundingMode;
    status->softfloat_roundingMode = softfloat_round_near_even;
    extFloat80_t z64 = f128_to_extF80(sum, status);   /* round to a 64-bit significand */
    status->softfloat_roundingMode = savedMode;

    return extF80_to_f128(z64, status);
}
