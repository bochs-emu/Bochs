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
 *            Stanislav Shwartsman (stl at fidonet.org.il)
 * ==========================================================================*/ 

#include "softfloatx80.h"
#include "softfloat-round-pack.h"
#include "softfloat-macros.h"

/* executes single exponent reduction cycle */
static Bit64u remainder_kernel(Bit64u aSig0, Bit64u bSig, int expDiff, Bit64u *zSig0, Bit64u *zSig1)
{
    Bit64u term0, term1;
    Bit64u aSig1 = 0;

    shortShift128Left(aSig1, aSig0, expDiff, &aSig1, &aSig0);
    Bit64u q = estimateDiv128To64(aSig1, aSig0, bSig);
    mul64To128(bSig, q, &term0, &term1);
    sub128(aSig1, aSig0, term0, term1, zSig1, zSig0);
    while ((Bit64s)(*zSig1) < 0) {
        --q;
        add128(*zSig1, *zSig0, 0, bSig, zSig1, zSig0);
    }
    return q;
}

static floatx80 do_fprem(floatx80 a, floatx80 b, Bit64u &q, int rounding_mode, float_status_t &status)
{
    Bit32s aExp, bExp, zExp, expDiff;
    Bit64u aSig0, aSig1, bSig;
    int aSign;
    q = 0;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig0 = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
    bSig = extractFloatx80Frac(b);
    bExp = extractFloatx80Exp(b);

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig0<<1)
             || ((bExp == 0x7FFF) && (Bit64u) (bSig<<1))) 
        {
            return propagateFloatx80NaN(a, b, status);
        }
        goto invalid;
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        return a;
    }
    if (bExp == 0) {
        if (bSig == 0) {
 invalid:
            float_raise(status, float_flag_invalid);
            return floatx80_default_nan;
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if ((Bit64u) (aSig0<<1) == 0) return a;
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig0, &aExp, &aSig0);
    }
    expDiff = aExp - bExp;
    aSig1 = 0;

    if (expDiff >= 64) {
        int n = (expDiff & 0x1f) | 0x20;
        remainder_kernel(aSig0, bSig, n, &aSig0, &aSig1);
        zExp = aExp - n;
        q = (Bit64u) -1;
    }
    else {
        zExp = bExp;

        if (expDiff < 0) {
            if (expDiff < -1)
               return (a.fraction & BX_CONST64(0x8000000000000000)) ? 
                    packFloatx80(aSign, aExp, aSig0) : a;
            shift128Right(aSig0, 0, 1, &aSig0, &aSig1);
            expDiff = 0;
        }

        if (expDiff > 0) {
            q = remainder_kernel(aSig0, bSig, expDiff, &aSig0, &aSig1);
        }
        else {
            if (bSig <= aSig0) {
               aSig0 -= bSig;
               q = 1;
            }
        }

        if (rounding_mode == float_round_nearest_even)
        {
            Bit64u term0, term1;
            shift128Right(bSig, 0, 1, &term0, &term1);

            if (! lt128(aSig0, aSig1, term0, term1))
            {
               int lt = lt128(term0, term1, aSig0, aSig1);
               int eq = eq128(aSig0, aSig1, term0, term1);
                
               if ((eq && (q & 1)) || lt) {
                  aSign = !aSign;
                  ++q;
               }
               if (lt) sub128(bSig, 0, aSig0, aSig1, &aSig0, &aSig1);
            }
        }
    }

    return normalizeRoundAndPackFloatx80(80, aSign, zExp, aSig0, aSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the remainder of the extended double-precision floating-point value
| `a' with respect to the corresponding value `b'.  The operation is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_ieee754_remainder(floatx80 a, floatx80 b, Bit64u &q, float_status_t &status)
{
    return do_fprem(a, b, q, float_round_nearest_even, status);
}

/*----------------------------------------------------------------------------
| Returns the remainder of the extended double-precision floating-point value
| `a' with  respect to  the corresponding value `b'. Unlike previous function
| the  function  does not compute  the remainder  specified  in  the IEC/IEEE 
| Standard  for Binary  Floating-Point  Arithmetic.  This  function  operates
| differently  from the  previous  function in  the way  that it  rounds  the 
| quotient of 'a' divided by 'b' to an integer.
*----------------------------------------------------------------------------*/

floatx80 floatx80_remainder(floatx80 a, floatx80 b, Bit64u &q, float_status_t &status)
{
    return do_fprem(a, b, q, float_round_to_zero, status);
}
