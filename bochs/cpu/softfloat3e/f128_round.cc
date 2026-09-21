/*============================================================================

This C source file is an extension to the SoftFloat IEEE Floating-Point
Arithmetic Package, Release 3e, written for Bochs (x86 architecture simulator).

Round the significand of a float128_t value to a narrower precision in place.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "softfloat.h"

/*----------------------------------------------------------------------------
| Round the float128_t value 'a' to 'roundingPrecision' using 'roundingMode',
| leaving the result in float128_t format.
|
| 'roundingPrecision' is the x87-style precision selector: the retained
| significand is (roundingPrecision - 16) bits wide -
|
|     80  ->  64-bit significand   (extended precision, the x87 register width)
|     83  ->  67-bit significand   (x87 transcendental internal "wide" format)
|     64  ->  48-bit significand
|     32  ->  16-bit significand
|
| A value of 128 or more leaves 'a' untouched (the full 113-bit significand is
| already at least that wide).
|
| Only the inexact flag is reported (plus overflow if a round-up carries the
| value past the largest finite float128_t).  This is a pure significand
| narrowing: it does not model the x87 register-range overflow/underflow or the
| C1 round-up bit - those belong to the instruction's final store, not to the
| intermediate roundings this routine performs.
*----------------------------------------------------------------------------*/

float128_t f128_round(float128_t a, uint8_t roundingPrecision, uint8_t roundingMode, struct softfloat_status_t *status)
{
    if (roundingPrecision >= 128)
        return a;

    bool sign      = signF128UI64(a.v64);
    int32_t exp    = expF128UI64(a.v64);
    uint64_t sigHi = fracF128UI64(a.v64);
    uint64_t sigLo = a.v0;

    /*------------------------------------------------------------------------
    | NaN / infinity pass through unchanged; so does a zero.
    *------------------------------------------------------------------------*/
    if (exp == 0x7FFF) return a;
    if (! (exp | sigHi | sigLo)) return a;

    if (exp)
        sigHi |= UINT64_C(0x0001000000000000);   // restore the implicit integer bit

    /*------------------------------------------------------------------------
    | 'drop' low bits of the 113-bit significand (sigHi:sigLo, integer bit at
    | bit 48 of sigHi) are discarded.  drop = 113 - (roundingPrecision - 16),
    | i.e. 2..97 for roundingPrecision 127..32.
    *------------------------------------------------------------------------*/
    unsigned drop = 129 - roundingPrecision;

    uint64_t loDropMask = (drop >= 64) ? ~UINT64_C(0) : ((UINT64_C(1) << drop) - 1);
    uint64_t hiDropMask = (drop <= 64) ? UINT64_C(0)  : ((UINT64_C(1) << (drop - 64)) - 1);

    bool inexact = ((sigLo & loDropMask) | (sigHi & hiDropMask)) != 0;

    unsigned rpos = drop - 1;                     // position of the round bit
    uint64_t roundBit = (rpos < 64) ? ((sigLo >> rpos) & 1) : ((sigHi >> (rpos - 64)) & 1);
    uint64_t lsb      = (drop < 64) ? ((sigLo >> drop) & 1) : ((sigHi >> (drop - 64)) & 1);
    bool sticky;
    if (rpos < 64)
        sticky = (sigLo & ((UINT64_C(1) << rpos) - 1)) != 0;
    else
        sticky = (sigLo | (sigHi & ((UINT64_C(1) << (rpos - 64)) - 1))) != 0;

    bool inc;
    switch (roundingMode) {
      default: /* softfloat_round_minMag - truncate toward zero */
        inc = false;
        break;
      case softfloat_round_near_even:
        inc = roundBit && (sticky || lsb);
        break;
      case softfloat_round_near_maxMag:
        inc = roundBit != 0;
        break;
      case softfloat_round_min:
        inc = sign && inexact;
        break;
      case softfloat_round_max:
        inc = ! sign && inexact;
        break;
    }

    sigLo &= ~loDropMask;
    sigHi &= ~hiDropMask;

    if (inc) {
        if (drop < 64) {
            uint64_t t = sigLo + (UINT64_C(1) << drop);
            if (t < sigLo) sigHi++;
            sigLo = t;
        } else {
            sigHi += (UINT64_C(1) << (drop - 64));
        }
        if (sigHi & UINT64_C(0x0002000000000000)) {   // significand carried up to 2.0
            sigLo = (sigLo >> 1) | (sigHi << 63);
            sigHi >>= 1;
            exp++;
        }
    }

    if (inexact)
        softfloat_raiseFlags(status, softfloat_flag_inexact);

    float128_t z;
    if (exp >= 0x7FFF) {                            // rounded up out of the finite range
        softfloat_raiseFlags(status, softfloat_flag_overflow | softfloat_flag_inexact);
        z.v64 = packToF128UI64(sign, 0x7FFF, 0);
        z.v0  = 0;
        return z;
    }
    if (exp == 0) {                                 // subnormal (a round-up may have normalised it)
        z.v64 = ((uint64_t) sign << 63) | sigHi;
        z.v0  = sigLo;
        return z;
    }
    z.v64 = ((uint64_t) sign << 63) + ((uint64_t) (exp - 1) << 48) + sigHi;
    z.v0  = sigLo;
    return z;
}
