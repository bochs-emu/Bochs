/*============================================================================
This source file is an extension to the SoftFloat IEC/IEEE Floating-point
Arithmetic Package, Release 2b, written for Bochs (x86 achitecture simulator)
floating point emulation.

THIS SOFTWARE IS DISTRIBUTED AS IS, FOR FREE.  Although reasonable effort has
been made to avoid it, THIS SOFTWARE MAY CONTAIN FAULTS THAT WILL AT TIMES
RESULT IN INCORRECT BEHAVIOR.  USE OF THIS SOFTWARE IS RESTRICTED TO PERSONS
AND ORGANIZATIONS WHO CAN AND WILL TAKE FULL RESPONSIBILITY FOR ALL LOSSES,
COSTS, OR OTHER PROBLEMS THEY INCUR DUE TO THE SOFTWARE, AND WHO FURTHERMORE
EFFECTIVELY INDEMNIFY JOHN HAUSER AND THE INTERNATIONAL COMPUTER SCIENCE
INSTITUTE (possibly via similar legal warning) AGAINST ALL LOSSES, COSTS, OR
OTHER PROBLEMS INCURRED BY THEIR CUSTOMERS AND CLIENTS DUE TO THE SOFTWARE.

Derivative works are acceptable, even for commercial purposes, so long as
(1) the source code for the derivative work includes prominent notice that
the work is derivative, and (2) the source code includes prominent notice with
these four paragraphs for those parts of this code that are retained.
=============================================================================*/

/*============================================================================
 * Written for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#include "fpu_trans.h"
#define USE_estimateDiv128To64
#include "softfloat-helpers.h"

#include "../softfloat3e/include/specialize.h"  // for softfloat_propagateNaNExtF80UI

#include "../wide_int.h"

/* executes single exponent reduction cycle */
static Bit64u remainder_kernel(Bit64u aSig, Bit64u bSig, int expDiff, Bit64u *zSig)
{
    Bit128u a = (Bit128u) aSig << expDiff;
    Bit64u q = estimateDiv128To64(GET128H(a), GET128L(a), bSig);
    Bit128u z = a - (Bit128u) q * bSig;
    while ((Bit64s) GET128H(z) < 0) {
        --q;
        z += bSig;
    }
    *zSig = (Bit64u) z;
    return q;
}

static int do_fprem(floatx80 a, floatx80 b, floatx80 &r, Bit64u &q, int rounding_mode, struct softfloat_status_t *status)
{
    Bit32s aExp, bExp, zExp, expDiff;
    Bit64u aSig, bSig;
    Bit128u rem;
    int aSign;
    struct exp32_sig64 normExpSig;

    q = 0;

    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a) || extF80_isUnsupported(b)) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        r = floatx80_default_nan;
        return -1;
    }

    aSig = extF80_fraction(a);
    aExp = extF80_exp(a);
    aSign = extF80_sign(a);
    bSig = extF80_fraction(b);
    bExp = extF80_exp(b);

    if (aExp == 0x7FFF) {
        if ((aSig<<1) || ((bExp == 0x7FFF) && (bSig<<1))) {
            r = softfloat_propagateNaNExtF80UI(a.signExp, a.signif, b.signExp, b.signif, status);
            return -1;
        }
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        r = floatx80_default_nan;
        return -1;
    }
    if (bExp == 0x7FFF) {
        if (bSig << 1) {
            r = softfloat_propagateNaNExtF80UI(a.signExp, a.signif, b.signExp, b.signif, status);
            return -1;
        }
        if (! aExp && aSig) {
            softfloat_raiseFlags(status, softfloat_flag_denormal);
            normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
            aExp = normExpSig.exp + 1;
            aSig = normExpSig.sig;
            r = (a.signif & BX_CONST64(0x8000000000000000)) ? packToExtF80(aSign, aExp, aSig) : a;
            return 0;
        }
        r = a;
        return 0;

    }
    if (! bExp) {
        if (! bSig) {
            softfloat_raiseFlags(status, softfloat_flag_invalid);
            r = floatx80_default_nan;
            return -1;
        }
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalExtF80Sig(bSig);
        bExp = normExpSig.exp + 1;
        bSig = normExpSig.sig;
    }
    if (! aExp) {
        if (! aSig) {
            r = a;
            return 0;
        }
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalExtF80Sig(aSig);
        aExp = normExpSig.exp + 1;
        aSig = normExpSig.sig;
    }

    expDiff = aExp - bExp;
    int overflow = 0;

    // the remainder is kept as 128-bit fixed point value, the integer part in the upper 64-bit
    if (expDiff >= 64) {
        int n = (expDiff & 0x1f) | 0x20;
        remainder_kernel(aSig, bSig, n, &aSig);
        rem = MAKE128U(aSig, 0);
        zExp = aExp - n;
        overflow = 1;
    }
    else {
        zExp = bExp;

        Bit128u bSig128 = MAKE128U(bSig, 0);

        if (expDiff < 0) {
            if (expDiff < -1) {
               r = softfloat_normRoundPackToExtF80(aSign, aExp, aSig, 0, 80, status);
               return 0;
            }
            rem = MAKE128U(aSig, 0) >> 1;    // always less than bSig, the quotient is 0
        }
        else if (expDiff > 0) {
            q = remainder_kernel(aSig, bSig, expDiff, &aSig);
            rem = MAKE128U(aSig, 0);
        }
        else {
            rem = MAKE128U(aSig, 0);
            if (bSig128 <= rem) {
               rem -= bSig128;
               q = 1;
            }
        }

        if (rounding_mode == softfloat_round_near_even) {
            Bit128u half = bSig128 >> 1;

            if (rem > half || (rem == half && (q & 1))) {
                aSign = !aSign;
                ++q;
            }
            if (rem > half) {
                rem = bSig128 - rem;
            }
        }
    }

    r = softfloat_normRoundPackToExtF80(aSign, zExp, GET128H(rem), GET128L(rem), 80, status);
    return overflow;
}

/*----------------------------------------------------------------------------
| Returns the remainder of the extended double-precision floating-point value
| `a' with respect to the corresponding value `b'.  The operation is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int floatx80_ieee754_remainder(floatx80 a, floatx80 b, floatx80 &r, Bit64u &q, struct softfloat_status_t *status)
{
    return do_fprem(a, b, r, q, softfloat_round_near_even, status);
}

/*----------------------------------------------------------------------------
| Returns the remainder of the extended double-precision floating-point value
| `a' with  respect to  the corresponding value `b'. Unlike previous function
| the  function  does not compute  the remainder  specified  in  the IEC/IEEE
| Standard  for Binary  Floating-Point  Arithmetic.  This  function  operates
| differently  from the  previous  function in  the way  that it  rounds  the
| quotient of 'a' divided by 'b' to an integer.
*----------------------------------------------------------------------------*/

int floatx80_remainder(floatx80 a, floatx80 b, floatx80 &r, Bit64u &q, struct softfloat_status_t *status)
{
    return do_fprem(a, b, r, q, softfloat_round_to_zero, status);
}
