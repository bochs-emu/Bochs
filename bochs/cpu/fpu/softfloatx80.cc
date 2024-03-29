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

#include "softfloatx80.h"
#include "softfloat-round-pack.h"
#include "softfloat-macros.h"

/*----------------------------------------------------------------------------
| Separate the source extended double-precision floating point value `a'
| into its exponent and significand, store the significant back to the
| 'a' and return the exponent. The operation performed is a superset of
| the IEC/IEEE recommended logb(x) function.
*----------------------------------------------------------------------------*/

floatx80 floatx80_extract(floatx80 &a, float_status_t &status)
{
    Bit64u aSig = extractFloatx80Frac(a);
    Bit32s aExp = extractFloatx80Exp(a);
    int   aSign = extractFloatx80Sign(a);

    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        a = floatx80_default_nan;
        return a;
    }

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1))
        {
            a = propagateFloatx80NaN(a, status);
            return a;
        }
        return packFloatx80(0, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0)
    {
        if (aSig == 0) {
            float_raise(status, float_flag_divbyzero);
            a = packFloatx80(aSign, 0, 0);
            return packFloatx80(1, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }

    a.exp = (aSign << 15) + 0x3FFF;
    a.fraction = aSig;
    return int32_to_floatx80(aExp - 0x3FFF);
}

/*----------------------------------------------------------------------------
| Scales extended double-precision floating-point value in operand `a' by
| value `b'. The function truncates the value in the second operand 'b' to
| an integral value and adds that value to the exponent of the operand 'a'.
| The operation performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_scale(floatx80 a, floatx80 b, float_status_t &status)
{
    Bit32s aExp, bExp;
    Bit64u aSig, bSig;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);
    bSig = extractFloatx80Frac(b);
    bExp = extractFloatx80Exp(b);
    int bSign = extractFloatx80Sign(b);

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1) || ((bExp == 0x7FFF) && (Bit64u) (bSig<<1)))
        {
            return propagateFloatx80NaN(a, b, status);
        }
        if ((bExp == 0x7FFF) && bSign) {
            float_raise(status, float_flag_invalid);
            return floatx80_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        if ((aExp | aSig) == 0) {
            if (! bSign) {
                float_raise(status, float_flag_invalid);
                return floatx80_default_nan;
            }
            return a;
        }
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        if (bSign) return packFloatx80(aSign, 0, 0);
        return packFloatx80(aSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        if (aSig == 0) return a;
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
        if (bExp < 0x3FFF)
            return normalizeRoundAndPackFloatx80(80, aSign, aExp, aSig, 0, status);
    }
    if (bExp == 0) {
        if (bSig == 0) return a;
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }

    if (bExp > 0x400E) {
        /* generate appropriate overflow/underflow */
        return roundAndPackFloatx80(80, aSign,
                          bSign ? -0x3FFF : 0x7FFF, aSig, 0, status);
    }

    if (bExp < 0x3FFF) return a;

    int shiftCount = 0x403E - bExp;
    bSig >>= shiftCount;
    Bit32s scale = (Bit32s) bSig;
    if (bSign) scale = -scale; /* -32768..32767 */
    return
        roundAndPackFloatx80(80, aSign, aExp+scale, aSig, 0, status);
}
