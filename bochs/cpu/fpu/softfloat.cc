/*============================================================================
This C source file is part of the SoftFloat IEC/IEEE Floating-point Arithmetic
Package, Release 2b.

Written by John R. Hauser.  This work was made possible in part by the
International Computer Science Institute, located at Suite 600, 1947 Center
Street, Berkeley, California 94704.  Funding was partially provided by the
National Science Foundation under grant MIP-9311980.  The original version
of this code was written as part of a project to build a fixed-point vector
processor in collaboration with the University of California at Berkeley,
overseen by Profs. Nelson Morgan and John Wawrzynek.  More information
is available through the Web page `http://www.cs.berkeley.edu/~jhauser/
arithmetic/SoftFloat.html'.

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

#define FLOAT128

/*============================================================================
 * Adapted for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#include "softfloat.h"
#include "softfloat-round-pack.h"

/*----------------------------------------------------------------------------
| Primitive arithmetic functions, including multi-word arithmetic, and
| division and square root approximations. (Can be specialized to target
| if desired).
*----------------------------------------------------------------------------*/
#define USE_estimateDiv128To64
#define USE_estimateSqrt32
#include "softfloat-macros.h"

/*----------------------------------------------------------------------------
| Functions and definitions to determine:  (1) whether tininess for underflow
| is detected before or after rounding by default, (2) what (if anything)
| happens when exceptions are raised, (3) how signaling NaNs are distinguished
| from quiet NaNs, (4) the default generated quiet NaNs, and (5) how NaNs
| are propagated from function inputs to output.  These details are target-
| specific.
*----------------------------------------------------------------------------*/
#include "softfloat-specialize.h"

#ifdef FLOATX80

/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the extended double-precision floating-point format.  The conversion
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 int32_to_floatx80(Bit32s a)
{
    if (a == 0) return packFloatx80(0, 0, 0);
    int   zSign = (a < 0);
    Bit32u absA = zSign ? -a : a;
    int    shiftCount = countLeadingZeros32(absA) + 32;
    Bit64u zSig = absA;
    return packFloatx80(zSign, 0x403E - shiftCount, zSig<<shiftCount);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the extended double-
| precision floating-point values `a' and `b'.  If `zSign' is 1, the sum is
| negated before being returned.  `zSign' is ignored if the result is a NaN.
| The addition is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static floatx80 addFloatx80Sigs(floatx80 a, floatx80 b, int zSign, float_status_t &status)
{
    Bit32s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig0, zSig1;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    bSig = extractFloatx80Frac(b);
    bExp = extractFloatx80Exp(b);

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1) || ((bExp == 0x7FFF) && (Bit64u) (bSig<<1)))
            return propagateFloatx80NaN(a, b, status);
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if ((bExp == 0) && bSig) {
                float_raise(status, float_flag_denormal);
                normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
            }
            return roundAndPackFloatx80(get_float_rounding_precision(status),
                    zSign, bExp, bSig, 0, status);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if (bSig == 0)
            return roundAndPackFloatx80(get_float_rounding_precision(status),
                    zSign, aExp, aSig, 0, status);

        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }
    Bit32s expDiff = aExp - bExp;
    zExp = aExp;
    if (0 < expDiff) {
        shift64ExtraRightJamming(bSig, 0,  expDiff, &bSig, &zSig1);
    }
    else if (expDiff < 0) {
        shift64ExtraRightJamming(aSig, 0, -expDiff, &aSig, &zSig1);
        zExp = bExp;
    }
    else {
        zSig0 = aSig + bSig;
        zSig1 = 0;
        goto shiftRight1;
    }
    zSig0 = aSig + bSig;
    if ((Bit64s) zSig0 < 0) goto roundAndPack;
 shiftRight1:
    shift64ExtraRightJamming(zSig0, zSig1, 1, &zSig0, &zSig1);
    zSig0 |= BX_CONST64(0x8000000000000000);
    zExp++;
 roundAndPack:
    return
        roundAndPackFloatx80(get_float_rounding_precision(status),
            zSign, zExp, zSig0, zSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the extended
| double-precision floating-point values `a' and `b'.  If `zSign' is 1, the
| difference is negated before being returned.  `zSign' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static floatx80 subFloatx80Sigs(floatx80 a, floatx80 b, int zSign, float_status_t &status)
{
    Bit32s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig0, zSig1;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    bSig = extractFloatx80Frac(b);
    bExp = extractFloatx80Exp(b);

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (bExp == 0x7FFF) {
            if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
            float_raise(status, float_flag_invalid);
            return floatx80_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloatx80(zSign ^ 1, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if (bExp == 0) {
                if (bSig) {
                    float_raise(status, float_flag_denormal);
                    normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
                    return roundAndPackFloatx80(get_float_rounding_precision(status),
                        zSign ^ 1, bExp, bSig, 0, status);
                }
                return packFloatx80(get_float_rounding_mode(status) == float_round_down, 0, 0);
            }
            return roundAndPackFloatx80(get_float_rounding_precision(status),
                    zSign ^ 1, bExp, bSig, 0, status);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if (bSig == 0)
            return roundAndPackFloatx80(get_float_rounding_precision(status),
                    zSign, aExp, aSig, 0, status);

        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }
    Bit32s expDiff = aExp - bExp;
    if (0 < expDiff) {
        shift128RightJamming(bSig, 0, expDiff, &bSig, &zSig1);
        goto aBigger;
    }
    if (expDiff < 0) {
        shift128RightJamming(aSig, 0, -expDiff, &aSig, &zSig1);
        goto bBigger;
    }
    zSig1 = 0;
    if (bSig < aSig) goto aBigger;
    if (aSig < bSig) goto bBigger;
    return packFloatx80(get_float_rounding_mode(status) == float_round_down, 0, 0);
 bBigger:
    sub128(bSig, 0, aSig, zSig1, &zSig0, &zSig1);
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aBigger:
    sub128(aSig, 0, bSig, zSig1, &zSig0, &zSig1);
    zExp = aExp;
 normalizeRoundAndPack:
    return
        normalizeRoundAndPackFloatx80(get_float_rounding_precision(status),
            zSign, zExp, zSig0, zSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the extended double-precision floating-point
| values `a' and `b'.  The operation is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_add(floatx80 a, floatx80 b, float_status_t &status)
{
    int aSign = extractFloatx80Sign(a);
    int bSign = extractFloatx80Sign(b);

    if (aSign == bSign)
        return addFloatx80Sigs(a, b, aSign, status);
    else
        return subFloatx80Sigs(a, b, aSign, status);
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the extended double-precision floating-
| point values `a' and `b'.  The operation is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_sub(floatx80 a, floatx80 b, float_status_t &status)
{
    int aSign = extractFloatx80Sign(a);
    int bSign = extractFloatx80Sign(b);

    if (aSign == bSign)
        return subFloatx80Sigs(a, b, aSign, status);
    else
        return addFloatx80Sigs(a, b, aSign, status);
}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the extended double-precision floating-
| point values `a' and `b'.  The operation is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_mul(floatx80 a, floatx80 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit32s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig0, zSig1;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
 invalid:
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
    bSig = extractFloatx80Frac(b);
    bExp = extractFloatx80Exp(b);
    bSign = extractFloatx80Sign(b);
    zSign = aSign ^ bSign;

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1) || ((bExp == 0x7FFF) && (Bit64u) (bSig<<1))) {
            return propagateFloatx80NaN(a, b, status);
        }
        if (bExp == 0) {
            if (bSig == 0) goto invalid;
            float_raise(status, float_flag_denormal);
        }
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (aExp == 0) {
            if (aSig == 0) goto invalid;
            float_raise(status, float_flag_denormal);
        }
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
            return packFloatx80(zSign, 0, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if (bSig == 0) return packFloatx80(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }
    zExp = aExp + bExp - 0x3FFE;
    mul64To128(aSig, bSig, &zSig0, &zSig1);
    if (0 < (Bit64s) zSig0) {
        shortShift128Left(zSig0, zSig1, 1, &zSig0, &zSig1);
        --zExp;
    }
    return
        roundAndPackFloatx80(get_float_rounding_precision(status),
             zSign, zExp, zSig0, zSig1, status);
}

#endif

#ifdef FLOAT128

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the quadruple-precision floating-point format. The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 floatx80_to_float128(floatx80 a, float_status_t &status)
{
    Bit64u zSig0, zSig1;

    Bit64u aSig = extractFloatx80Frac(a);
    Bit32s aExp = extractFloatx80Exp(a);
    int   aSign = extractFloatx80Sign(a);

    if ((aExp == 0x7FFF) && (Bit64u) (aSig<<1))
        return commonNaNToFloat128(floatx80ToCommonNaN(a, status));

    shift128Right(aSig<<1, 0, 16, &zSig0, &zSig1);
    return packFloat128(aSign, aExp, zSig0, zSig1);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the quadruple-precision floating-point
| value `a' to the extended double-precision floating-point format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 float128_to_floatx80(float128 a, float_status_t &status)
{
    Bit32s aExp;
    Bit64u aSig0, aSig1;

    aSig1 = extractFloat128Frac1(a);
    aSig0 = extractFloat128Frac0(a);
    aExp = extractFloat128Exp(a);
    int aSign = extractFloat128Sign(a);

    if (aExp == 0x7FFF) {
        if (aSig0 | aSig1)
            return commonNaNToFloatx80(float128ToCommonNaN(a, status));

        return packFloatx80(aSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }

    if (aExp == 0) {
        if ((aSig0 | aSig1) == 0) return packFloatx80(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(aSig0, aSig1, &aExp, &aSig0, &aSig1);
    }
    else aSig0 |= BX_CONST64(0x0001000000000000);

    shortShift128Left(aSig0, aSig1, 15, &aSig0, &aSig1);
    return roundAndPackFloatx80(80, aSign, aExp, aSig0, aSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the extended double-precision floating-
| point value `a' and quadruple-precision floating point value `b'. The
| operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_mul(floatx80 a, float128 b, float_status_t &status)
{
    Bit32s aExp, bExp, zExp;
    Bit64u aSig, bSig0, bSig1, zSig0, zSig1, zSig2;
    int aSign, bSign, zSign;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
 invalid:
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
    bSig0 = extractFloat128Frac0(b);
    bSig1 = extractFloat128Frac1(b);
    bExp = extractFloat128Exp(b);
    bSign = extractFloat128Sign(b);

    zSign = aSign ^ bSign;

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1)
             || ((bExp == 0x7FFF) && (bSig0 | bSig1)))
        {
            floatx80 r = commonNaNToFloatx80(float128ToCommonNaN(b, status));
            return propagateFloatx80NaN(a, r, status);
        }
        if (bExp == 0) {
            if ((bSig0 | bSig1) == 0) goto invalid;
            float_raise(status, float_flag_denormal);
        }
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (bExp == 0x7FFF) {
        if (bSig0 | bSig1) {
            floatx80 r = commonNaNToFloatx80(float128ToCommonNaN(b, status));
            return propagateFloatx80NaN(a, r, status);
        }
        if (aExp == 0) {
            if (aSig == 0) goto invalid;
            float_raise(status, float_flag_denormal);
        }
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if ((bExp == 0) && (bSig0 | bSig1)) float_raise(status, float_flag_denormal);
            return packFloatx80(zSign, 0, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if ((bSig0 | bSig1) == 0) return packFloatx80(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(bSig0, bSig1, &bExp, &bSig0, &bSig1);
    }
    else bSig0 |= BX_CONST64(0x0001000000000000);

    zExp = aExp + bExp - 0x3FFE;
    shortShift128Left(bSig0, bSig1, 15, &bSig0, &bSig1);
    mul128By64To192(bSig0, bSig1, aSig, &zSig0, &zSig1, &zSig2);
    if (0 < (Bit64s) zSig0) {
        shortShift128Left(zSig0, zSig1, 1, &zSig0, &zSig1);
        --zExp;
    }
    return
        roundAndPackFloatx80(get_float_rounding_precision(status),
             zSign, zExp, zSig0, zSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the quadruple-precision
| floating-point values `a' and `b'. If `zSign' is 1, the sum is negated
| before being returned. `zSign' is ignored if the result is a NaN.
| The addition is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float128 addFloat128Sigs(float128 a, float128 b, int zSign, float_status_t &status)
{
    Bit32s aExp, bExp, zExp;
    Bit64u aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2;
    Bit32s expDiff;

    aSig1 = extractFloat128Frac1(a);
    aSig0 = extractFloat128Frac0(a);
    aExp = extractFloat128Exp(a);
    bSig1 = extractFloat128Frac1(b);
    bSig0 = extractFloat128Frac0(b);
    bExp = extractFloat128Exp(b);
    expDiff = aExp - bExp;

    if (0 < expDiff) {
        if (aExp == 0x7FFF) {
            if (aSig0 | aSig1) return propagateFloat128NaN(a, b, status);
            return a;
        }
        if (bExp == 0) --expDiff;
        else bSig0 |= BX_CONST64(0x0001000000000000);
        shift128ExtraRightJamming(bSig0, bSig1, 0, expDiff, &bSig0, &bSig1, &zSig2);
        zExp = aExp;
    }
    else if (expDiff < 0) {
        if (bExp == 0x7FFF) {
            if (bSig0 | bSig1) return propagateFloat128NaN(a, b, status);
            return packFloat128(zSign, 0x7FFF, 0, 0);
        }
        if (aExp == 0) ++expDiff;
        else aSig0 |= BX_CONST64(0x0001000000000000);
        shift128ExtraRightJamming(aSig0, aSig1, 0, -expDiff, &aSig0, &aSig1, &zSig2);
        zExp = bExp;
    }
    else {
        if (aExp == 0x7FFF) {
            if (aSig0 | aSig1 | bSig0 | bSig1)
                return propagateFloat128NaN(a, b, status);

            return a;
        }
        add128(aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1);
        if (aExp == 0) return packFloat128(zSign, 0, zSig0, zSig1);
        zSig2 = 0;
        zSig0 |= BX_CONST64(0x0002000000000000);
        zExp = aExp;
        goto shiftRight1;
    }
    aSig0 |= BX_CONST64(0x0001000000000000);
    add128(aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1);
    --zExp;
    if (zSig0 < BX_CONST64(0x0002000000000000)) goto roundAndPack;
    ++zExp;
 shiftRight1:
    shift128ExtraRightJamming(zSig0, zSig1, zSig2, 1, &zSig0, &zSig1, &zSig2);
 roundAndPack:
    return roundAndPackFloat128(zSign, zExp, zSig0, zSig1, zSig2, status);
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the quadruple-
| precision floating-point values `a' and `b'.  If `zSign' is 1, the
| difference is negated before being returned.  `zSign' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float128 subFloat128Sigs(float128 a, float128 b, int zSign, float_status_t &status)
{
    Bit32s aExp, bExp, zExp;
    Bit64u aSig0, aSig1, bSig0, bSig1, zSig0, zSig1;
    Bit32s expDiff;

    aSig1 = extractFloat128Frac1(a);
    aSig0 = extractFloat128Frac0(a);
    aExp = extractFloat128Exp(a);
    bSig1 = extractFloat128Frac1(b);
    bSig0 = extractFloat128Frac0(b);
    bExp = extractFloat128Exp(b);

    expDiff = aExp - bExp;
    shortShift128Left(aSig0, aSig1, 14, &aSig0, &aSig1);
    shortShift128Left(bSig0, bSig1, 14, &bSig0, &bSig1);
    if (0 < expDiff) goto aExpBigger;
    if (expDiff < 0) goto bExpBigger;
    if (aExp == 0x7FFF) {
        if (aSig0 | aSig1 | bSig0 | bSig1)
            return propagateFloat128NaN(a, b, status);

        float_raise(status, float_flag_invalid);
        return float128_default_nan;
    }
    if (aExp == 0) {
        aExp = 1;
        bExp = 1;
    }
    if (bSig0 < aSig0) goto aBigger;
    if (aSig0 < bSig0) goto bBigger;
    if (bSig1 < aSig1) goto aBigger;
    if (aSig1 < bSig1) goto bBigger;
    return packFloat128(0, 0);

 bExpBigger:
    if (bExp == 0x7FFF) {
        if (bSig0 | bSig1) return propagateFloat128NaN(a, b, status);
        return packFloat128(zSign ^ 1, 0x7FFF, 0, 0);
    }
    if (aExp == 0) ++expDiff;
    else {
        aSig0 |= BX_CONST64(0x4000000000000000);
    }
    shift128RightJamming(aSig0, aSig1, - expDiff, &aSig0, &aSig1);
    bSig0 |= BX_CONST64(0x4000000000000000);
 bBigger:
    sub128(bSig0, bSig1, aSig0, aSig1, &zSig0, &zSig1);
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aExpBigger:
    if (aExp == 0x7FFF) {
        if (aSig0 | aSig1) return propagateFloat128NaN(a, b, status);
        return a;
    }
    if (bExp == 0) --expDiff;
    else {
        bSig0 |= BX_CONST64(0x4000000000000000);
    }
    shift128RightJamming(bSig0, bSig1, expDiff, &bSig0, &bSig1);
    aSig0 |= BX_CONST64(0x4000000000000000);
 aBigger:
    sub128(aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1);
    zExp = aExp;
 normalizeRoundAndPack:
    --zExp;
    return normalizeRoundAndPackFloat128(zSign, zExp - 14, zSig0, zSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the quadruple-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 float128_add(float128 a, float128 b, float_status_t &status)
{
    int aSign = extractFloat128Sign(a);
    int bSign = extractFloat128Sign(b);

    if (aSign == bSign) {
        return addFloat128Sigs(a, b, aSign, status);
    }
    else {
        return subFloat128Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the quadruple-precision floating-point
| values `a' and `b'.  The operation is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 float128_sub(float128 a, float128 b, float_status_t &status)
{
    int aSign = extractFloat128Sign(a);
    int bSign = extractFloat128Sign(b);

    if (aSign == bSign) {
        return subFloat128Sigs(a, b, aSign, status);
    }
    else {
        return addFloat128Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the quadruple-precision floating-point
| values `a' and `b'.  The operation is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 float128_mul(float128 a, float128 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit32s aExp, bExp, zExp;
    Bit64u aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2, zSig3;

    aSig1 = extractFloat128Frac1(a);
    aSig0 = extractFloat128Frac0(a);
    aExp = extractFloat128Exp(a);
    aSign = extractFloat128Sign(a);
    bSig1 = extractFloat128Frac1(b);
    bSig0 = extractFloat128Frac0(b);
    bExp = extractFloat128Exp(b);
    bSign = extractFloat128Sign(b);

    zSign = aSign ^ bSign;
    if (aExp == 0x7FFF) {
        if ((aSig0 | aSig1) || ((bExp == 0x7FFF) && (bSig0 | bSig1))) {
            return propagateFloat128NaN(a, b, status);
        }
        if ((bExp | bSig0 | bSig1) == 0) {
            float_raise(status, float_flag_invalid);
            return float128_default_nan;
        }
        return packFloat128(zSign, 0x7FFF, 0, 0);
    }
    if (bExp == 0x7FFF) {
        if (bSig0 | bSig1) return propagateFloat128NaN(a, b, status);
        if ((aExp | aSig0 | aSig1) == 0) {
            float_raise(status, float_flag_invalid);
            return float128_default_nan;
        }
        return packFloat128(zSign, 0x7FFF, 0, 0);
    }
    if (aExp == 0) {
        if ((aSig0 | aSig1) == 0) return packFloat128(zSign, 0, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(aSig0, aSig1, &aExp, &aSig0, &aSig1);
    }
    if (bExp == 0) {
        if ((bSig0 | bSig1) == 0) return packFloat128(zSign, 0, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(bSig0, bSig1, &bExp, &bSig0, &bSig1);
    }
    zExp = aExp + bExp - 0x4000;
    aSig0 |= BX_CONST64(0x0001000000000000);
    shortShift128Left(bSig0, bSig1, 16, &bSig0, &bSig1);
    mul128To256(aSig0, aSig1, bSig0, bSig1, &zSig0, &zSig1, &zSig2, &zSig3);
    add128(zSig0, zSig1, aSig0, aSig1, &zSig0, &zSig1);
    zSig2 |= (zSig3 != 0);
    if (BX_CONST64(0x0002000000000000) <= zSig0) {
        shift128ExtraRightJamming(zSig0, zSig1, zSig2, 1, &zSig0, &zSig1, &zSig2);
        ++zExp;
    }
    return roundAndPackFloat128(zSign, zExp, zSig0, zSig1, zSig2, status);
}

/*----------------------------------------------------------------------------
| Returns the result of dividing the quadruple-precision floating-point value
| `a' by the corresponding value `b'.  The operation is performed according to
| the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 float128_div(float128 a, float128 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit32s aExp, bExp, zExp;
    Bit64u aSig0, aSig1, bSig0, bSig1, zSig0, zSig1, zSig2;
    Bit64u rem0, rem1, rem2, rem3, term0, term1, term2, term3;

    aSig1 = extractFloat128Frac1(a);
    aSig0 = extractFloat128Frac0(a);
    aExp = extractFloat128Exp(a);
    aSign = extractFloat128Sign(a);
    bSig1 = extractFloat128Frac1(b);
    bSig0 = extractFloat128Frac0(b);
    bExp = extractFloat128Exp(b);
    bSign = extractFloat128Sign(b);

    zSign = aSign ^ bSign;
    if (aExp == 0x7FFF) {
        if (aSig0 | aSig1) return propagateFloat128NaN(a, b, status);
        if (bExp == 0x7FFF) {
            if (bSig0 | bSig1) return propagateFloat128NaN(a, b, status);
            float_raise(status, float_flag_invalid);
            return float128_default_nan;
        }
        return packFloat128(zSign, 0x7FFF, 0, 0);
    }
    if (bExp == 0x7FFF) {
        if (bSig0 | bSig1) return propagateFloat128NaN(a, b, status);
        return packFloat128(zSign, 0, 0, 0);
    }
    if (bExp == 0) {
        if ((bSig0 | bSig1) == 0) {
            if ((aExp | aSig0 | aSig1) == 0) {
                float_raise(status, float_flag_invalid);
                return float128_default_nan;
            }
            float_raise(status, float_flag_divbyzero);
            return packFloat128(zSign, 0x7FFF, 0, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(bSig0, bSig1, &bExp, &bSig0, &bSig1);
    }
    if (aExp == 0) {
        if ((aSig0 | aSig1) == 0) return packFloat128(zSign, 0, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat128Subnormal(aSig0, aSig1, &aExp, &aSig0, &aSig1);
    }
    zExp = aExp - bExp + 0x3FFD;
    shortShift128Left(
        aSig0 | BX_CONST64(0x0001000000000000), aSig1, 15, &aSig0, &aSig1);
    shortShift128Left(
        bSig0 | BX_CONST64(0x0001000000000000), bSig1, 15, &bSig0, &bSig1);
    if (le128(bSig0, bSig1, aSig0, aSig1)) {
        shift128Right(aSig0, aSig1, 1, &aSig0, &aSig1);
        ++zExp;
    }
    zSig0 = estimateDiv128To64(aSig0, aSig1, bSig0);
    mul128By64To192(bSig0, bSig1, zSig0, &term0, &term1, &term2);
    sub192(aSig0, aSig1, 0, term0, term1, term2, &rem0, &rem1, &rem2);
    while ((Bit64s) rem0 < 0) {
        --zSig0;
        add192(rem0, rem1, rem2, 0, bSig0, bSig1, &rem0, &rem1, &rem2);
    }
    zSig1 = estimateDiv128To64(rem1, rem2, bSig0);
    if ((zSig1 & 0x3FFF) <= 4) {
        mul128By64To192(bSig0, bSig1, zSig1, &term1, &term2, &term3);
        sub192(rem1, rem2, 0, term1, term2, term3, &rem1, &rem2, &rem3);
        while ((Bit64s) rem1 < 0) {
            --zSig1;
            add192(rem1, rem2, rem3, 0, bSig0, bSig1, &rem1, &rem2, &rem3);
        }
        zSig1 |= ((rem1 | rem2 | rem3) != 0);
    }
    shift128ExtraRightJamming(zSig0, zSig1, 0, 15, &zSig0, &zSig1, &zSig2);
    return roundAndPackFloat128(zSign, zExp, zSig0, zSig1, zSig2, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 64-bit two's complement integer `a' to
| the quadruple-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 int64_to_float128(Bit64s a)
{
    Bit64u zSig0, zSig1;

    if (a == 0) return packFloat128(0, 0, 0, 0);
    int zSign = (a < 0);
    Bit64u absA = zSign ? - a : a;
    Bit8u shiftCount = countLeadingZeros64(absA) + 49;
    Bit32s zExp = 0x406E - shiftCount;
    if (64 <= shiftCount) {
        zSig1 = 0;
        zSig0 = absA;
        shiftCount -= 64;
    }
    else {
        zSig1 = absA;
        zSig0 = 0;
    }
    shortShift128Left(zSig0, zSig1, shiftCount, &zSig0, &zSig1);
    return packFloat128(zSign, zExp, zSig0, zSig1);
}

#endif
