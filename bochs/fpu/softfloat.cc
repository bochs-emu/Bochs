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

/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the single-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 int32_to_float32(Bit32s a, float_status_t &status)
{
    if (a == 0) return 0;
    if (a == (Bit32s) 0x80000000) return packFloat32(1, 0x9E, 0);
    int zSign = (a < 0);
    return normalizeRoundAndPackFloat32(zSign, 0x9C, zSign ? -a : a, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the double-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 int32_to_float64(Bit32s a)
{
    if (a == 0) return 0;
    int zSign = (a < 0);
    Bit32u absA = zSign ? -a : a;
    int shiftCount = countLeadingZeros32(absA) + 21;
    Bit64u zSig = absA;
    return packFloat64(zSign, 0x432 - shiftCount, zSig<<shiftCount);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 64-bit two's complement integer `a'
| to the single-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 int64_to_float32(Bit64s a, float_status_t &status)
{
    if (a == 0) return 0;
    int zSign = (a < 0);
    Bit64u absA = zSign ? -a : a;
    int shiftCount = countLeadingZeros64(absA) - 40;
    if (0 <= shiftCount) {
        return packFloat32(zSign, 0x95 - shiftCount, (Bit32u)(absA<<shiftCount));
    }
    else {
        shiftCount += 7;
        if (shiftCount < 0) {
            absA = shift64RightJamming(absA, -shiftCount);
        }
        else {
            absA <<= shiftCount;
        }
        return roundAndPackFloat32(zSign, 0x9C - shiftCount, (Bit32u) absA, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 64-bit two's complement integer `a'
| to the double-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 int64_to_float64(Bit64s a, float_status_t &status)
{
    if (a == 0) return 0;
    if (a == (Bit64s) BX_CONST64(0x8000000000000000)) {
        return packFloat64(1, 0x43E, 0);
    }
    int zSign = (a < 0);
    return normalizeRoundAndPackFloat64(zSign, 0x43C, zSign ? -a : a, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic - which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN or the
| conversion overflows the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit32s float32_to_int32(float32 a, float_status_t &status)
{
    Bit32u aSig = extractFloat32Frac(a);
    Bit16s aExp = extractFloat32Exp(a);
    int aSign = extractFloat32Sign(a);
    if ((aExp == 0xFF) && aSig) aSign = 0;
    if (aExp) aSig |= 0x00800000;
    int shiftCount = 0xAF - aExp;
    Bit64u aSig64 = aSig;
    aSig64 <<= 32;
    if (0 < shiftCount) aSig64 = shift64RightJamming(aSig64, shiftCount);
    return roundAndPackInt32(aSign, aSig64, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN or the conversion overflows, the integer indefinite
| value is returned.
*----------------------------------------------------------------------------*/

Bit32s float32_to_int32_round_to_zero(float32 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit32u aSig;
    Bit32s z;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    int shiftCount = aExp - 0x9E;
    if (0 <= shiftCount) {
        if (a != 0xCF000000) {
            float_raise(status, float_flag_invalid);
        }
        return (Bit32s)(int32_indefinite);
    }
    else if (aExp <= 0x7E) {
        if (aExp | aSig) float_raise(status, float_flag_inexact);
        return 0;
    }
    aSig = (aSig | 0x00800000)<<8;
    z = aSig>>(-shiftCount);
    if ((Bit32u) (aSig<<(shiftCount & 31))) {
        float_raise(status, float_flag_inexact);
    }
    if (aSign) z = -z;
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 64-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic - which means in particular that the conversion is rounded
| according to the current rounding mode. If `a' is a NaN or the
| conversion overflows, the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit64s float32_to_int64(float32 a, float_status_t &status)
{
    Bit64u aSig64, aSigExtra;

    Bit32u aSig = extractFloat32Frac(a);
    Bit16s aExp = extractFloat32Exp(a);
    int aSign = extractFloat32Sign(a);

    int shiftCount = 0xBE - aExp;
    if (shiftCount < 0) {
        float_raise(status, float_flag_invalid);
        return (Bit64s)(int64_indefinite);
    }
    if (aExp) aSig |= 0x00800000;
    aSig64 = aSig;
    aSig64 <<= 40;
    shift64ExtraRightJamming(aSig64, 0, shiftCount, &aSig64, &aSigExtra);
    return roundAndPackInt64(aSign, aSig64, aSigExtra, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 64-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN or the conversion overflows, the integer indefinite
| value is returned.
*----------------------------------------------------------------------------*/

Bit64s float32_to_int64_round_to_zero(float32 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit32u aSig;
    Bit64u aSig64;
    Bit64s z;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    int shiftCount = aExp - 0xBE;
    if (0 <= shiftCount) {
        if (a != 0xDF000000) {
            float_raise(status, float_flag_invalid);
        }
        return (Bit64s)(int64_indefinite);
    }
    else if (aExp <= 0x7E) {
        if (aExp | aSig) float_raise(status, float_flag_inexact);
        return 0;
    }
    aSig64 = aSig | 0x00800000;
    aSig64 <<= 40;
    z = aSig64>>(-shiftCount);
    if ((Bit64u) (aSig64<<(shiftCount & 63))) {
        float_raise(status, float_flag_inexact);
    }
    if (aSign) z = -z;
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the double-precision floating-point format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float64 float32_to_float64(float32 a, float_status_t &status)
{
    Bit32u aSig = extractFloat32Frac(a);
    Bit16s aExp = extractFloat32Exp(a);
    int  aSign = extractFloat32Sign(a);

    if (aExp == 0xFF) {
        if (aSig) return commonNaNToFloat64(float32ToCommonNaN(a, status));
        return packFloat64(aSign, 0x7FF, 0);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat64(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
        --aExp;
    }
    return packFloat64(aSign, aExp + 0x380, ((Bit64u) aSig)<<29);
}

/*----------------------------------------------------------------------------
| Rounds the single-precision floating-point value `a' to an integer, and
| returns the result as a single-precision floating-point value.  The
| operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_round_to_int(float32 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit32u lastBitMask, roundBitsMask;
    int roundingMode;
    float32 z;

    aExp = extractFloat32Exp(a);
    if (0x96 <= aExp) {
        if ((aExp == 0xFF) && extractFloat32Frac(a)) {
            return propagateFloat32NaN(a, status);
        }
        return a;
    }
    if (aExp <= 0x7E) {
        if ((Bit32u) (a<<1) == 0) return a;
        float_raise(status, float_flag_inexact);
        aSign = extractFloat32Sign(a);
        switch (get_float_rounding_mode(status)) {
         case float_round_nearest_even:
            if ((aExp == 0x7E) && extractFloat32Frac(a)) {
                return packFloat32(aSign, 0x7F, 0);
            }
            break;
         case float_round_down:
            return aSign ? 0xBF800000 : 0;
         case float_round_up:
            return aSign ? 0x80000000 : 0x3F800000;
        }
        return packFloat32(aSign, 0, 0);
    }
    lastBitMask = 1;
    lastBitMask <<= 0x96 - aExp;
    roundBitsMask = lastBitMask - 1;
    z = a;
    roundingMode = get_float_rounding_mode(status);
    if (roundingMode == float_round_nearest_even) {
        z += lastBitMask>>1;
        if ((z & roundBitsMask) == 0) z &= ~lastBitMask;
    }
    else if (roundingMode != float_round_to_zero) {
        if (extractFloat32Sign(z) ^ (roundingMode == float_round_up)) {
            z += roundBitsMask;
        }
    }
    z &= ~roundBitsMask;
    if (z != a) float_raise(status, float_flag_inexact);
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the single-precision
| floating-point values `a' and `b'.  If `zSign' is 1, the sum is negated
| before being returned.  `zSign' is ignored if the result is a NaN.
| The addition is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float32 addFloat32Sigs(float32 a, float32 b, int zSign, float_status_t &status)
{
    Bit16s aExp, bExp, zExp;
    Bit32u aSig, bSig, zSig;
    Bit16s expDiff;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    bSig = extractFloat32Frac(b);
    bExp = extractFloat32Exp(b);

    expDiff = aExp - bExp;
    aSig <<= 6;
    bSig <<= 6;

    if (0 < expDiff) {
        if (aExp == 0xFF) {
            if (aSig) return propagateFloat32NaN(a, b, status);
            if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
            return a;
        }
        if ((aExp == 0) && aSig)
            float_raise(status, float_flag_denormal);

        if (bExp == 0) {
            if (bSig) float_raise(status, float_flag_denormal);
            --expDiff;
        }
        else bSig |= 0x20000000;

        bSig = shift32RightJamming(bSig, expDiff);
        zExp = aExp;
    }
    else if (expDiff < 0) {
        if (bExp == 0xFF) {
            if (bSig) return propagateFloat32NaN(a, b, status);
            if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat32(zSign, 0xFF, 0);
        }
        if ((bExp == 0) && bSig)
            float_raise(status, float_flag_denormal);

        if (aExp == 0) {
            if (aSig) float_raise(status, float_flag_denormal);
            ++expDiff;
        }
        else aSig |= 0x20000000;

        aSig = shift32RightJamming(aSig, -expDiff);
        zExp = bExp;
    }
    else {
        if (aExp == 0xFF) {
            if (aSig | bSig) return propagateFloat32NaN(a, b, status);
            return a;
        }
        if (aExp == 0) {
            if (aSig | bSig) float_raise(status, float_flag_denormal);
            return packFloat32(zSign, 0, (aSig + bSig)>>6);
        }
        zSig = 0x40000000 + aSig + bSig;
        zExp = aExp;
        goto roundAndPack;
    }
    aSig |= 0x20000000;
    zSig = (aSig + bSig)<<1;
    --zExp;
    if ((Bit32s) zSig < 0) {
        zSig = aSig + bSig;
        ++zExp;
    }
 roundAndPack:
    return roundAndPackFloat32(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the single-
| precision floating-point values `a' and `b'.  If `zSign' is 1, the
| difference is negated before being returned.  `zSign' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float32 subFloat32Sigs(float32 a, float32 b, int zSign, float_status_t &status)
{
    Bit16s aExp, bExp, zExp;
    Bit32u aSig, bSig, zSig;
    Bit16s expDiff;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    bSig = extractFloat32Frac(b);
    bExp = extractFloat32Exp(b);

    expDiff = aExp - bExp;
    aSig <<= 7;
    bSig <<= 7;
    if (0 < expDiff) goto aExpBigger;
    if (expDiff < 0) goto bExpBigger;
    if (aExp == 0xFF) {
        if (aSig | bSig) return propagateFloat32NaN(a, b, status);
        float_raise(status, float_flag_invalid);
        return float32_default_nan;
    }
    if (aExp == 0) {
        if (aSig | bSig) float_raise(status, float_flag_denormal);
        aExp = 1;
        bExp = 1;
    }
    if (bSig < aSig) goto aBigger;
    if (aSig < bSig) goto bBigger;
    return packFloat32(get_float_rounding_mode(status) == float_round_down, 0, 0);
 bExpBigger:
    if (bExp == 0xFF) {
        if (bSig) return propagateFloat32NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat32(zSign ^ 1, 0xFF, 0);
    }
    if ((bExp == 0) && bSig)
        float_raise(status, float_flag_denormal);

    if (aExp == 0) {
        if (aSig) float_raise(status, float_flag_denormal);
        ++expDiff;
    }
    else aSig |= 0x40000000;

    aSig = shift32RightJamming(aSig, -expDiff);
    bSig |= 0x40000000;
 bBigger:
    zSig = bSig - aSig;
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aExpBigger:
    if (aExp == 0xFF) {
        if (aSig) return propagateFloat32NaN(a, b, status);
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if ((aExp == 0) && aSig)
        float_raise(status, float_flag_denormal);

    if (bExp == 0) {
        if (bSig) float_raise(status, float_flag_denormal);
        --expDiff;
    }
    else bSig |= 0x40000000;

    bSig = shift32RightJamming(bSig, expDiff);
    aSig |= 0x40000000;
 aBigger:
    zSig = aSig - bSig;
    zExp = aExp;
 normalizeRoundAndPack:
    --zExp;
    return normalizeRoundAndPackFloat32(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the single-precision floating-point values `a'
| and `b'.  The operation is performed according to the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_add(float32 a, float32 b, float_status_t &status)
{
    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);

    if (aSign == bSign) {
        return addFloat32Sigs(a, b, aSign, status);
    }
    else {
        return subFloat32Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the single-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_sub(float32 a, float32 b, float_status_t &status)
{
    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);

    if (aSign == bSign) {
        return subFloat32Sigs(a, b, aSign, status);
    }
    else {
        return addFloat32Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the single-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_mul(float32 a, float32 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit16s aExp, bExp, zExp;
    Bit32u aSig, bSig;
    Bit64u zSig64;
    Bit32u zSig;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    bSig = extractFloat32Frac(b);
    bExp = extractFloat32Exp(b);
    bSign = extractFloat32Sign(b);
    zSign = aSign ^ bSign;
    if (aExp == 0xFF) {
        if (aSig || ((bExp == 0xFF) && bSig))
            return propagateFloat32NaN(a, b, status);

        if ((bExp | bSig) == 0) {
            float_raise(status, float_flag_invalid);
            return float32_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat32(zSign, 0xFF, 0);
    }
    if (bExp == 0xFF) {
        if (bSig) return propagateFloat32NaN(a, b, status);
        if ((aExp | aSig) == 0) {
            float_raise(status, float_flag_invalid);
            return float32_default_nan;
        }
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat32(zSign, 0xFF, 0);
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat32(zSign, 0, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if (bSig == 0) return packFloat32(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(bSig, &bExp, &bSig);
    }
    zExp = aExp + bExp - 0x7F;
    aSig = (aSig | 0x00800000)<<7;
    bSig = (bSig | 0x00800000)<<8;
    zSig64 = shift64RightJamming(((Bit64u) aSig) * bSig, 32);
    zSig = (Bit32u) zSig64;
    if (0 <= (Bit32s) (zSig<<1)) {
        zSig <<= 1;
        --zExp;
    }
    return roundAndPackFloat32(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of dividing the single-precision floating-point value `a'
| by the corresponding value `b'.  The operation is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_div(float32 a, float32 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit16s aExp, bExp, zExp;
    Bit32u aSig, bSig, zSig;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    bSig = extractFloat32Frac(b);
    bExp = extractFloat32Exp(b);
    bSign = extractFloat32Sign(b);
    zSign = aSign ^ bSign;
    if (aExp == 0xFF) {
        if (aSig) return propagateFloat32NaN(a, b, status);
        if (bExp == 0xFF) {
            if (bSig) return propagateFloat32NaN(a, b, status);
            float_raise(status, float_flag_invalid);
            return float32_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat32(zSign, 0xFF, 0);
    }
    if (bExp == 0xFF) {
        if (bSig) return propagateFloat32NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat32(zSign, 0, 0);
    }
    if (bExp == 0) {
        if (bSig == 0) {
            if ((aExp | aSig) == 0) {
                float_raise(status, float_flag_invalid);
                return float32_default_nan;
            }
            float_raise(status, float_flag_divbyzero);
            return packFloat32(zSign, 0xFF, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat32(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
    }
    zExp = aExp - bExp + 0x7D;
    aSig = (aSig | 0x00800000)<<7;
    bSig = (bSig | 0x00800000)<<8;
    if (bSig <= (aSig + aSig)) {
        aSig >>= 1;
        ++zExp;
    }
    zSig = (((Bit64u) aSig)<<32) / bSig;
    if ((zSig & 0x3F) == 0) {
        zSig |= ((Bit64u) bSig * zSig != ((Bit64u) aSig)<<32);
    }
    return roundAndPackFloat32(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the square root of the single-precision floating-point value `a'.
| The operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_sqrt(float32 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp, zExp;
    Bit32u aSig, zSig;
    Bit64u rem, term;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    if (aExp == 0xFF) {
        if (aSig) return propagateFloat32NaN(a, status);
        if (! aSign) return a;
        float_raise(status, float_flag_invalid);
        return float32_default_nan;
    }
    if (aSign) {
        if ((aExp | aSig) == 0) return a;
        float_raise(status, float_flag_invalid);
        return float32_default_nan;
    }
    if (aExp == 0) {
        if (aSig == 0) return 0;
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
    }
    zExp = ((aExp - 0x7F)>>1) + 0x7E;
    aSig = (aSig | 0x00800000)<<8;
    zSig = estimateSqrt32(aExp, aSig) + 2;
    if ((zSig & 0x7F) <= 5) {
        if (zSig < 2) {
            zSig = 0x7FFFFFFF;
            goto roundAndPack;
        }
        aSig >>= aExp & 1;
        term = ((Bit64u) zSig) * zSig;
        rem = (((Bit64u) aSig)<<32) - term;
        while ((Bit64s) rem < 0) {
            --zSig;
            rem += (((Bit64u) zSig)<<1) | 1;
        }
        zSig |= (rem != 0);
    }
    zSig = shift32RightJamming(zSig, 1);
 roundAndPack:
    return roundAndPackFloat32(0, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Determine single-precision floating-point number class
*----------------------------------------------------------------------------*/

float_class_t float32_class(float32 a)
{
   Bit16s aExp = extractFloat32Exp(a);
   Bit32u aSig = extractFloat32Frac(a);
   int  aSign = extractFloat32Sign(a);

   if(aExp == 0xFF) {
       if (aSig == 0)
           return (aSign) ? float_negative_inf : float_positive_inf;

       return float_NaN;
   }

   if(aExp == 0) {
       if (aSig == 0) return float_zero;
       return float_denormal;
   }

   return float_normalized;
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is equal to
| the corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float32_eq(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float32_is_signaling_nan(a) || float32_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return (a == b) || ((Bit32u) ((a | b)<<1) == 0);
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is less than
| or equal to the corresponding value `b', and 0 otherwise.  The comparison
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

int float32_le(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign) return aSign || ((Bit32u) ((a | b)<<1) == 0);
    return (a == b) || (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float32_lt(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign) return aSign && ((Bit32u) ((a | b)<<1) != 0);
    return (a != b) && (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is equal to
| the corresponding value `b', and 0 otherwise.  The invalid exception is
| raised if either operand is a NaN.  Otherwise, the comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float32_eq_signaling(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return (a == b) || ((Bit32u) ((a | b)<<1) == 0);
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is less than or
| equal to the corresponding value `b', and 0 otherwise.  Quiet NaNs do not
| cause an exception.  Otherwise, the comparison is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float32_le_quiet(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float32_is_signaling_nan(a) || float32_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign) return aSign || ((Bit32u) ((a | b)<<1) == 0);
    return (a == b) || (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the single-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  Quiet NaNs do not cause an
| exception.  Otherwise, the comparison is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float32_lt_quiet(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float32_is_signaling_nan(a) || float32_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign) return aSign && ((Bit32u) ((a | b)<<1) != 0);
    return (a != b) && (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| The unordered relationship is true when at least one of two source operands
| being compared is a NaN. Quiet NaNs do not cause an exception.
*----------------------------------------------------------------------------*/

int float32_unordered(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float32_is_signaling_nan(a) || float32_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 1;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return 0;
}

/*----------------------------------------------------------------------------
| Compare  between  two  single  precision  floating  point  numbers. Returns
| 'float_relation_equal'  if the operands are equal, 'float_relation_less' if
| the    value    'a'   is   less   than   the   corresponding   value   `b',
| 'float_relation_greater' if the value 'a' is greater than the corresponding
| value `b', or 'float_relation_unordered' otherwise.
*----------------------------------------------------------------------------*/

int float32_compare(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return float_relation_unordered;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    if ((a == b) || ((Bit32u) ((a | b)<<1) == 0)) return float_relation_equal;

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign)
        return (aSign) ? float_relation_less : float_relation_greater;

    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

/*----------------------------------------------------------------------------
| Compare  between  two  double  precision  floating  point  numbers. Returns
| 'float_relation_equal'  if the operands are equal, 'float_relation_less' if
| the    value    'a'   is   less   than   the   corresponding   value   `b',
| 'float_relation_greater' if the value 'a' is greater than the corresponding
| value `b', or 'float_relation_unordered' otherwise. Quiet NaNs do not cause
| an exception.
*----------------------------------------------------------------------------*/

int float32_compare_quiet(float32 a, float32 b, float_status_t &status)
{
    float_class_t aClass = float32_class(a);
    float_class_t bClass = float32_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float32_is_signaling_nan(a) || float32_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return float_relation_unordered;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    if ((a == b) || ((Bit32u) ((a | b)<<1) == 0)) return float_relation_equal;

    int aSign = extractFloat32Sign(a);
    int bSign = extractFloat32Sign(b);
    if (aSign != bSign)
        return (aSign) ? float_relation_less : float_relation_greater;

    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic - which means in particular that the conversion is rounded
| according to the current rounding mode. If `a' is a NaN or the
| conversion overflows, the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit32s float64_to_int32(float64 a, float_status_t &status)
{
    Bit64u aSig = extractFloat64Frac(a);
    Bit16s aExp = extractFloat64Exp(a);
    int aSign = extractFloat64Sign(a);
    if ((aExp == 0x7FF) && aSig) aSign = 0;
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    int shiftCount = 0x42C - aExp;
    if (0 < shiftCount) aSig = shift64RightJamming(aSig, shiftCount);
    return roundAndPackInt32(aSign, aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN or the conversion overflows, the integer indefinite
| value  is returned.
*----------------------------------------------------------------------------*/

Bit32s float64_to_int32_round_to_zero(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit64u aSig, savedASig;
    Bit32s z;
    int shiftCount;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (0x41E < aExp) {
        if ((aExp == 0x7FF) && aSig) aSign = 0;
        goto invalid;
    }
    else if (aExp < 0x3FF) {
        if (aExp || aSig) float_raise(status, float_flag_inexact);
        return 0;
    }
    aSig |= BX_CONST64(0x0010000000000000);
    shiftCount = 0x433 - aExp;
    savedASig = aSig;
    aSig >>= shiftCount;
    z = (Bit32s) aSig;
    if (aSign) z = -z;
    if ((z < 0) ^ aSign) {
 invalid:
        float_raise(status, float_flag_invalid);
        return (Bit32s)(int32_indefinite);
    }
    if ((aSig<<shiftCount) != savedASig) {
        float_raise(status, float_flag_inexact);
    }
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 64-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic - which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN, the largest
| positive integer is returned.  Otherwise, if the conversion overflows, the
| largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

Bit64s float64_to_int64(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit64u aSig, aSigExtra;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    int shiftCount = 0x433 - aExp;
    if (shiftCount <= 0) {
        if (0x43E < aExp) {
            float_raise(status, float_flag_invalid);
            return (Bit64s)(int64_indefinite);
        }
        aSigExtra = 0;
        aSig <<= -shiftCount;
    }
    else {
        shift64ExtraRightJamming(aSig, 0, shiftCount, &aSig, &aSigExtra);
    }
    return roundAndPackInt64(aSign, aSig, aSigExtra, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 64-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN or the conversion overflows, the integer indefinite
| value  is returned.
*----------------------------------------------------------------------------*/

Bit64s float64_to_int64_round_to_zero(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit64u aSig;
    Bit64s z;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    int shiftCount = aExp - 0x433;
    if (0 <= shiftCount) {
        if (0x43E <= aExp) {
            if (a != BX_CONST64(0xC3E0000000000000)) {
                float_raise(status, float_flag_invalid);
            }
            return (Bit64s)(int64_indefinite);
        }
        z = aSig<<shiftCount;
    }
    else {
        if (aExp < 0x3FE) {
            if (aExp | aSig) float_raise(status, float_flag_inexact);
            return 0;
        }
        z = aSig>>(-shiftCount);
        if ((Bit64u) (aSig<<(shiftCount & 63))) {
             float_raise(status, float_flag_inexact);
        }
    }
    if (aSign) z = -z;
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the single-precision floating-point format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float32 float64_to_float32(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit64u aSig;
    Bit32u zSig;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp == 0x7FF) {
        if (aSig) return commonNaNToFloat32(float64ToCommonNaN(a, status));
        return packFloat32(aSign, 0xFF, 0);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat32(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
    }
    aSig = shift64RightJamming(aSig, 22);
    zSig = (Bit32u) aSig;
    if (aExp || zSig) {
        zSig |= 0x40000000;
        aExp -= 0x381;
    }
    return roundAndPackFloat32(aSign, aExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Rounds the double-precision floating-point value `a' to an integer, and
| returns the result as a double-precision floating-point value.  The
| operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_round_to_int(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp;
    Bit64u lastBitMask, roundBitsMask;
    int roundingMode;
    float64 z;

    aExp = extractFloat64Exp(a);
    if (0x433 <= aExp) {
        if ((aExp == 0x7FF) && extractFloat64Frac(a)) {
            return propagateFloat64NaN(a, status);
        }
        return a;
    }
    if (aExp < 0x3FF) {
        if ((Bit64u) (a<<1) == 0) return a;
        float_raise(status, float_flag_inexact);
        aSign = extractFloat64Sign(a);
        switch (get_float_rounding_mode(status)) {
         case float_round_nearest_even:
            if ((aExp == 0x3FE) && extractFloat64Frac(a)) {
              return packFloat64(aSign, 0x3FF, 0);
            }
            break;
         case float_round_down:
            return aSign ? BX_CONST64(0xBFF0000000000000) : 0;
         case float_round_up:
            return
              aSign ? BX_CONST64(0x8000000000000000) : BX_CONST64(0x3FF0000000000000);
        }
        return packFloat64(aSign, 0, 0);
    }
    lastBitMask = 1;
    lastBitMask <<= 0x433 - aExp;
    roundBitsMask = lastBitMask - 1;
    z = a;
    roundingMode = get_float_rounding_mode(status);
    if (roundingMode == float_round_nearest_even) {
        z += lastBitMask>>1;
        if ((z & roundBitsMask) == 0) z &= ~lastBitMask;
    }
    else if (roundingMode != float_round_to_zero) {
        if (extractFloat64Sign(z) ^ (roundingMode == float_round_up)) {
            z += roundBitsMask;
        }
    }
    z &= ~roundBitsMask;
    if (z != a) float_raise(status, float_flag_inexact);
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of adding the absolute values of the double-precision
| floating-point values `a' and `b'.  If `zSign' is 1, the sum is negated
| before being returned.  `zSign' is ignored if the result is a NaN.
| The addition is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float64 addFloat64Sigs(float64 a, float64 b, int zSign, float_status_t &status)
{
    Bit16s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig;
    Bit16s expDiff;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    bSig = extractFloat64Frac(b);
    bExp = extractFloat64Exp(b);

    expDiff = aExp - bExp;
    aSig <<= 9;
    bSig <<= 9;
    if (0 < expDiff) {
        if (aExp == 0x7FF) {
            if (aSig) return propagateFloat64NaN(a, b, status);
            if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
            return a;
        }
        if ((aExp == 0) && aSig)
            float_raise(status, float_flag_denormal);

        if (bExp == 0) {
            if (bSig) float_raise(status, float_flag_denormal);
            --expDiff;
        }
        else bSig |= BX_CONST64(0x2000000000000000);

        bSig = shift64RightJamming(bSig, expDiff);
        zExp = aExp;
    }
    else if (expDiff < 0) {
        if (bExp == 0x7FF) {
            if (bSig) return propagateFloat64NaN(a, b, status);
            if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat64(zSign, 0x7FF, 0);
        }
        if ((bExp == 0) && bSig)
            float_raise(status, float_flag_denormal);

        if (aExp == 0) {
            if (aSig) float_raise(status, float_flag_denormal);
            ++expDiff;
        }
        else aSig |= BX_CONST64(0x2000000000000000);

        aSig = shift64RightJamming(aSig, -expDiff);
        zExp = bExp;
    }
    else {
        if (aExp == 0x7FF) {
            if (aSig | bSig) return propagateFloat64NaN(a, b, status);
            return a;
        }
        if (aExp == 0) {
            if (aSig | bSig) float_raise(status, float_flag_denormal);
            return packFloat64(zSign, 0, (aSig + bSig)>>9);
        }
        zSig = BX_CONST64(0x4000000000000000) + aSig + bSig;
        zExp = aExp;
        goto roundAndPack;
    }
    aSig |= BX_CONST64(0x2000000000000000);
    zSig = (aSig + bSig)<<1;
    --zExp;
    if ((Bit64s) zSig < 0) {
        zSig = aSig + bSig;
        ++zExp;
    }
 roundAndPack:
    return roundAndPackFloat64(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the absolute values of the double-
| precision floating-point values `a' and `b'.  If `zSign' is 1, the
| difference is negated before being returned.  `zSign' is ignored if the
| result is a NaN.  The subtraction is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float64 subFloat64Sigs(float64 a, float64 b, int zSign, float_status_t &status)
{
    Bit16s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig;
    Bit16s expDiff;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    bSig = extractFloat64Frac(b);
    bExp = extractFloat64Exp(b);

    expDiff = aExp - bExp;
    aSig <<= 10;
    bSig <<= 10;
    if (0 < expDiff) goto aExpBigger;
    if (expDiff < 0) goto bExpBigger;
    if (aExp == 0x7FF) {
        if (aSig | bSig) return propagateFloat64NaN(a, b, status);
        float_raise(status, float_flag_invalid);
        return float64_default_nan;
    }
    if (aExp == 0) {
        if (aSig | bSig) float_raise(status, float_flag_denormal);
        aExp = 1;
        bExp = 1;
    }
    if (bSig < aSig) goto aBigger;
    if (aSig < bSig) goto bBigger;
    return packFloat64(get_float_rounding_mode(status) == float_round_down, 0, 0);
 bExpBigger:
    if (bExp == 0x7FF) {
        if (bSig) return propagateFloat64NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat64(zSign ^ 1, 0x7FF, 0);
    }
    if ((bExp == 0) && bSig)
        float_raise(status, float_flag_denormal);

    if (aExp == 0) {
        if (aSig) float_raise(status, float_flag_denormal);
        ++expDiff;
    }
    else aSig |= BX_CONST64(0x4000000000000000);

    aSig = shift64RightJamming(aSig, -expDiff);
    bSig |= BX_CONST64(0x4000000000000000);
 bBigger:
    zSig = bSig - aSig;
    zExp = bExp;
    zSign ^= 1;
    goto normalizeRoundAndPack;
 aExpBigger:
    if (aExp == 0x7FF) {
        if (aSig) return propagateFloat64NaN(a, b, status);
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if ((aExp == 0) && aSig)
        float_raise(status, float_flag_denormal);

    if (bExp == 0) {
        if (bSig) float_raise(status, float_flag_denormal);
        --expDiff;
    }
    else bSig |= BX_CONST64(0x4000000000000000);

    bSig = shift64RightJamming(bSig, expDiff);
    aSig |= BX_CONST64(0x4000000000000000);
 aBigger:
    zSig = aSig - bSig;
    zExp = aExp;
 normalizeRoundAndPack:
    --zExp;
    return normalizeRoundAndPackFloat64(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of adding the double-precision floating-point values `a'
| and `b'.  The operation is performed according to the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_add(float64 a, float64 b, float_status_t &status)
{
    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);

    if (aSign == bSign) {
        return addFloat64Sigs(a, b, aSign, status);
    }
    else {
        return subFloat64Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of subtracting the double-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_sub(float64 a, float64 b, float_status_t &status)
{
    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);

    if (aSign == bSign) {
        return subFloat64Sigs(a, b, aSign, status);
    }
    else {
        return addFloat64Sigs(a, b, aSign, status);
    }
}

/*----------------------------------------------------------------------------
| Returns the result of multiplying the double-precision floating-point values
| `a' and `b'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_mul(float64 a, float64 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit16s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig0, zSig1;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    bSig = extractFloat64Frac(b);
    bExp = extractFloat64Exp(b);
    bSign = extractFloat64Sign(b);
    zSign = aSign ^ bSign;
    if (aExp == 0x7FF) {
        if (aSig || ((bExp == 0x7FF) && bSig)) {
            return propagateFloat64NaN(a, b, status);
        }
        if ((bExp | bSig) == 0) {
            float_raise(status, float_flag_invalid);
            return float64_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat64(zSign, 0x7FF, 0);
    }
    if (bExp == 0x7FF) {
        if (bSig) return propagateFloat64NaN(a, b, status);
        if ((aExp | aSig) == 0) {
            float_raise(status, float_flag_invalid);
            return float64_default_nan;
        }
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat64(zSign, 0x7FF, 0);
    }
    if (aExp == 0) {
        if (aSig == 0) {
            if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat64(zSign, 0, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(aSig, &aExp, &aSig);
    }
    if (bExp == 0) {
        if (bSig == 0) return packFloat64(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(bSig, &bExp, &bSig);
    }
    zExp = aExp + bExp - 0x3FF;
    aSig = (aSig | BX_CONST64(0x0010000000000000))<<10;
    bSig = (bSig | BX_CONST64(0x0010000000000000))<<11;
    mul64To128(aSig, bSig, &zSig0, &zSig1);
    zSig0 |= (zSig1 != 0);
    if (0 <= (Bit64s) (zSig0<<1)) {
        zSig0 <<= 1;
        --zExp;
    }
    return roundAndPackFloat64(zSign, zExp, zSig0, status);
}

/*----------------------------------------------------------------------------
| Returns the result of dividing the double-precision floating-point value `a'
| by the corresponding value `b'.  The operation is performed according to
| the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_div(float64 a, float64 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit16s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig;
    Bit64u rem0, rem1;
    Bit64u term0, term1;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    bSig = extractFloat64Frac(b);
    bExp = extractFloat64Exp(b);
    bSign = extractFloat64Sign(b);
    zSign = aSign ^ bSign;
    if (aExp == 0x7FF) {
        if (aSig) return propagateFloat64NaN(a, b, status);
        if (bExp == 0x7FF) {
            if (bSig) return propagateFloat64NaN(a, b, status);
            float_raise(status, float_flag_invalid);
            return float64_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat64(zSign, 0x7FF, 0);
    }
    if (bExp == 0x7FF) {
        if (bSig) return propagateFloat64NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloat64(zSign, 0, 0);
    }
    if (bExp == 0) {
        if (bSig == 0) {
            if ((aExp | aSig) == 0) {
                float_raise(status, float_flag_invalid);
                return float64_default_nan;
            }
            float_raise(status, float_flag_divbyzero);
            return packFloat64(zSign, 0x7FF, 0);
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat64(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(aSig, &aExp, &aSig);
    }
    zExp = aExp - bExp + 0x3FD;
    aSig = (aSig | BX_CONST64(0x0010000000000000))<<10;
    bSig = (bSig | BX_CONST64(0x0010000000000000))<<11;
    if (bSig <= (aSig + aSig)) {
        aSig >>= 1;
        ++zExp;
    }
    zSig = estimateDiv128To64(aSig, 0, bSig);
    if ((zSig & 0x1FF) <= 2) {
        mul64To128(bSig, zSig, &term0, &term1);
        sub128(aSig, 0, term0, term1, &rem0, &rem1);
        while ((Bit64s) rem0 < 0) {
            --zSig;
            add128(rem0, rem1, 0, bSig, &rem0, &rem1);
        }
        zSig |= (rem1 != 0);
    }
    return roundAndPackFloat64(zSign, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Returns the square root of the double-precision floating-point value `a'.
| The operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_sqrt(float64 a, float_status_t &status)
{
    int aSign;
    Bit16s aExp, zExp;
    Bit64u aSig, zSig, doubleZSig;
    Bit64u rem0, rem1, term0, term1;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp == 0x7FF) {
        if (aSig) return propagateFloat64NaN(a, status);
        if (! aSign) return a;
        float_raise(status, float_flag_invalid);
        return float64_default_nan;
    }
    if (aSign) {
        if ((aExp | aSig) == 0) return a;
        float_raise(status, float_flag_invalid);
        return float64_default_nan;
    }
    if (aExp == 0) {
        if (aSig == 0) return 0;
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(aSig, &aExp, &aSig);
    }
    zExp = ((aExp - 0x3FF)>>1) + 0x3FE;
    aSig |= BX_CONST64(0x0010000000000000);
    zSig = estimateSqrt32(aExp, (Bit32u)(aSig>>21));
    aSig <<= 9 - (aExp & 1);
    zSig = estimateDiv128To64(aSig, 0, zSig<<32) + (zSig<<30);
    if ((zSig & 0x1FF) <= 5) {
        doubleZSig = zSig<<1;
        mul64To128(zSig, zSig, &term0, &term1);
        sub128(aSig, 0, term0, term1, &rem0, &rem1);
        while ((Bit64s) rem0 < 0) {
            --zSig;
            doubleZSig -= 2;
            add128(rem0, rem1, zSig>>63, doubleZSig | 1, &rem0, &rem1);
        }
        zSig |= ((rem0 | rem1) != 0);
    }
    return roundAndPackFloat64(0, zExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Determine double-precision floating-point number class
*----------------------------------------------------------------------------*/

float_class_t float64_class(float64 a)
{
   Bit16s aExp = extractFloat64Exp(a);
   Bit64u aSig = extractFloat64Frac(a);
   int  aSign = extractFloat64Sign(a);

   if(aExp == 0x7FF) {
       if (aSig == 0)
           return (aSign) ? float_negative_inf : float_positive_inf;

       return float_NaN;
   }

   if(aExp == 0) {
       if (aSig == 0)
           return float_zero;
       return float_denormal;
   }

   return float_normalized;
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is equal to the
| corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float64_eq(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float64_is_signaling_nan(a) || float64_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return (a == b) || ((Bit64u) ((a | b)<<1) == 0);
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than or
| equal to the corresponding value `b', and 0 otherwise.  The comparison is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

int float64_le(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign) return aSign || ((Bit64u) ((a | b)<<1) == 0);
    return (a == b) || (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  The comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float64_lt(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign) return aSign && ((Bit64u) ((a | b)<<1) != 0);
    return (a != b) && (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is equal to the
| corresponding value `b', and 0 otherwise.  The invalid exception is raised
| if either operand is a NaN.  Otherwise, the comparison is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float64_eq_signaling(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return (a == b) || ((Bit64u) ((a | b)<<1) == 0);
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than or
| equal to the corresponding value `b', and 0 otherwise.  Quiet NaNs do not
| cause an exception.  Otherwise, the comparison is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float64_le_quiet(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float64_is_signaling_nan(a) || float64_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign) return aSign || ((Bit64u) ((a | b)<<1) == 0);
    return (a == b) || (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| Returns 1 if the double-precision floating-point value `a' is less than
| the corresponding value `b', and 0 otherwise.  Quiet NaNs do not cause an
| exception.  Otherwise, the comparison is performed according to the IEC/IEEE
| Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

int float64_lt_quiet(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float64_is_signaling_nan(a) || float64_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 0;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign) return aSign && ((Bit64u) ((a | b)<<1) != 0);
    return (a != b) && (aSign ^ (a < b));
}

/*----------------------------------------------------------------------------
| The unordered relationship is true when at least one of two source operands
| being compared is a NaN. Quiet NaNs do not cause an exception.
*----------------------------------------------------------------------------*/

int float64_unordered(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float64_is_signaling_nan(a) || float64_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return 1;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    return 0;
}

/*----------------------------------------------------------------------------
| Compare  between  two  double  precision  floating  point  numbers. Returns
| 'float_relation_equal'  if the operands are equal, 'float_relation_less' if
| the    value    'a'   is   less   than   the   corresponding   value   `b',
| 'float_relation_greater' if the value 'a' is greater than the corresponding
| value `b', or 'float_relation_unordered' otherwise.
*----------------------------------------------------------------------------*/

int float64_compare(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN) {
        float_raise(status, float_flag_invalid);
        return float_relation_unordered;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    if ((a == b) || ((Bit64u) ((a | b)<<1) == 0)) return float_relation_equal;

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign)
        return (aSign) ? float_relation_less : float_relation_greater;

    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

/*----------------------------------------------------------------------------
| Compare  between  two  double  precision  floating  point  numbers. Returns
| 'float_relation_equal'  if the operands are equal, 'float_relation_less' if
| the    value    'a'   is   less   than   the   corresponding   value   `b',
| 'float_relation_greater' if the value 'a' is greater than the corresponding
| value `b', or 'float_relation_unordered' otherwise. Quiet NaNs do not cause
| an exception.
*----------------------------------------------------------------------------*/

int float64_compare_quiet(float64 a, float64 b, float_status_t &status)
{
    float_class_t aClass = float64_class(a);
    float_class_t bClass = float64_class(b);

    if (aClass == float_NaN || bClass == float_NaN)
    {
        if (float64_is_signaling_nan(a) || float64_is_signaling_nan(b))
        {
            float_raise(status, float_flag_invalid);
        }
        return float_relation_unordered;
    }

    if (aClass == float_denormal || bClass == float_denormal)
    {
        float_raise(status, float_flag_denormal);
    }

    if ((a == b) || ((Bit64u) ((a | b)<<1) == 0)) return float_relation_equal;

    int aSign = extractFloat64Sign(a);
    int bSign = extractFloat64Sign(b);
    if (aSign != bSign)
        return (aSign) ? float_relation_less : float_relation_greater;

    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

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
| Returns the result of converting the 64-bit two's complement integer `a'
| to the extended double-precision floating-point format.  The conversion
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 int64_to_floatx80(Bit64s a)
{
    if (a == 0) return packFloatx80(0, 0, 0);
    int   zSign = (a < 0);
    Bit64u absA = zSign ? -a : a;
    int    shiftCount = countLeadingZeros64(absA);
    return packFloatx80(zSign, 0x403E - shiftCount, absA<<shiftCount);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the extended double-precision floating-point format.  The conversion
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 float32_to_floatx80(float32 a, float_status_t &status)
{
    Bit32u aSig = extractFloat32Frac(a);
    Bit16s aExp = extractFloat32Exp(a);
    int aSign = extractFloat32Sign(a);
    if (aExp == 0xFF) {
        if (aSig) return commonNaNToFloatx80(float32ToCommonNaN(a, status));
        return packFloatx80(aSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloatx80(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
    }
    aSig |= 0x00800000;
    return packFloatx80(aSign, aExp + 0x3F80, ((Bit64u) aSig)<<40);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the extended double-precision floating-point format.  The conversion
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 float64_to_floatx80(float64 a, float_status_t &status)
{
    Bit64u aSig = extractFloat64Frac(a);
    Bit16s aExp = extractFloat64Exp(a);
    int aSign = extractFloat64Sign(a);

    if (aExp == 0x7FF) {
        if (aSig) return commonNaNToFloatx80(float64ToCommonNaN(a, status));
        return packFloatx80(aSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloatx80(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(aSig, &aExp, &aSig);
    }
    return
        packFloatx80(
            aSign, aExp + 0x3C00, (aSig | BX_CONST64(0x0010000000000000))<<11);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 32-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic - which means in particular that the conversion
| is rounded according to the current rounding mode. If `a' is a NaN or the
| conversion overflows, the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit32s floatx80_to_int32(floatx80 a, float_status_t &status)
{
    Bit64u aSig = extractFloatx80Frac(a);
    Bit32s aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return int32_indefinite;
    }

    if ((aExp == 0x7FFF) && (Bit64u) (aSig<<1)) aSign = 0;
    int shiftCount = 0x4037 - aExp;
    if (shiftCount <= 0) shiftCount = 1;
    aSig = shift64RightJamming(aSig, shiftCount);
    return roundAndPackInt32(aSign, aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 32-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic, except that the conversion is always rounded
| toward zero.  If `a' is a NaN or the conversion overflows, the integer
| indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit32s floatx80_to_int32_round_to_zero(floatx80 a, float_status_t &status)
{
    Bit32s aExp;
    Bit64u aSig, savedASig;
    Bit32s z;
    int shiftCount;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return int32_indefinite;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);

    if (aExp > 0x401E) goto invalid;
    if (aExp < 0x3FFF) {
        if (aExp || aSig) float_raise(status, float_flag_inexact);
        return 0;
    }
    shiftCount = 0x403E - aExp;
    savedASig = aSig;
    aSig >>= shiftCount;
    z = (Bit32s) aSig;
    if (aSign) z = -z;
    if ((z < 0) ^ aSign) {
 invalid:
        float_raise(status, float_flag_invalid);
        return (Bit32s)(int32_indefinite);
    }
    if ((aSig<<shiftCount) != savedASig)
    {
        float_raise(status, float_flag_inexact);
    }
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 64-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic - which means in particular that the conversion
| is rounded according to the current rounding mode. If `a' is a NaN or the
| conversion overflows, the integer indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit64s floatx80_to_int64(floatx80 a, float_status_t &status)
{
    Bit32s aExp;
    Bit64u aSig, aSigExtra;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return int64_indefinite;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);

    int shiftCount = 0x403E - aExp;
    if (shiftCount <= 0)
    {
        if (shiftCount)
        {
            float_raise(status, float_flag_invalid);
            return (Bit64s)(int64_indefinite);
        }
        aSigExtra = 0;
    }
    else {
        shift64ExtraRightJamming(aSig, 0, shiftCount, &aSig, &aSigExtra);
    }

    return roundAndPackInt64(aSign, aSig, aSigExtra, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the 64-bit two's complement integer format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic, except that the conversion is always rounded
| toward zero.  If `a' is a NaN or the conversion overflows, the integer
| indefinite value is returned.
*----------------------------------------------------------------------------*/

Bit64s floatx80_to_int64_round_to_zero(floatx80 a, float_status_t &status)
{
    int aSign;
    Bit32s aExp;
    Bit64u aSig;
    Bit64s z;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return int64_indefinite;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
    int shiftCount = aExp - 0x403E;
    if (0 <= shiftCount) {
        aSig &= BX_CONST64(0x7FFFFFFFFFFFFFFF);
        if ((a.exp != 0xC03E) || aSig) {
            float_raise(status, float_flag_invalid);
        }
        return (Bit64s)(int64_indefinite);
    }
    else if (aExp < 0x3FFF) {
        if (aExp | aSig) float_raise(status, float_flag_inexact);
        return 0;
    }
    z = aSig>>(-shiftCount);
    if ((Bit64u) (aSig<<(shiftCount & 63))) {
        float_raise(status, float_flag_inexact);
    }
    if (aSign) z = -z;
    return z;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the single-precision floating-point format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 floatx80_to_float32(floatx80 a, float_status_t &status)
{
    Bit64u aSig = extractFloatx80Frac(a);
    Bit32s aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return float32_default_nan;
    }

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1))
            return commonNaNToFloat32(floatx80ToCommonNaN(a, status));

        return packFloat32(aSign, 0xFF, 0);
    }
    aSig = shift64RightJamming(aSig, 33);
    if (aExp || aSig) aExp -= 0x3F81;
    return roundAndPackFloat32(aSign, aExp, (Bit32u) aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the extended double-precision floating-
| point value `a' to the double-precision floating-point format.  The
| conversion is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 floatx80_to_float64(floatx80 a, float_status_t &status)
{
    Bit32s aExp;
    Bit64u aSig, zSig;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return float64_default_nan;
    }

    aSig = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    int aSign = extractFloatx80Sign(a);

    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig<<1)) {
            return commonNaNToFloat64(floatx80ToCommonNaN(a, status));
        }
        return packFloat64(aSign, 0x7FF, 0);
    }
    zSig = shift64RightJamming(aSig, 1);
    if (aExp || aSig) aExp -= 0x3C01;
    return roundAndPackFloat64(aSign, aExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Rounds the extended double-precision floating-point value `a' to an integer,
| and returns the result as an extended double-precision floating-point
| value.  The operation is performed according to the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_round_to_int(floatx80 a, float_status_t &status)
{
    int aSign;
    Bit64u lastBitMask, roundBitsMask;
    Bit8u roundingMode;
    floatx80 z;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    Bit32s aExp = extractFloatx80Exp(a);
    Bit64u aSig = extractFloatx80Frac(a);
    if (0x403E <= aExp) {
        if ((aExp == 0x7FFF) && (Bit64u) (aSig<<1)) {
            return propagateFloatx80NaN(a, status);
        }
        return a;
    }
    if (aExp < 0x3FFF) {
        if (aExp == 0) {
            if ((aSig<<1) == 0) return a;
            float_raise(status, float_flag_denormal);
        }
        float_raise(status, float_flag_inexact);
        aSign = extractFloatx80Sign(a);
        switch (get_float_rounding_mode(status)) {
         case float_round_nearest_even:
            if ((aExp == 0x3FFE) && (Bit64u) (aSig<<1)) {
                set_float_rounding_up(status);
                return packFloatx80(aSign, 0x3FFF, BX_CONST64(0x8000000000000000));
            }
            break;
         case float_round_down:
            if (aSign) {
                set_float_rounding_up(status);
                return packFloatx80(1, 0x3FFF, BX_CONST64(0x8000000000000000));
            }
            else {
                return packFloatx80(0, 0, 0);
            }
         case float_round_up:
            if (aSign) {
                return packFloatx80(1, 0, 0);
            }
            else {
                set_float_rounding_up(status);
                return packFloatx80(0, 0x3FFF, BX_CONST64(0x8000000000000000));
            }
        }
        return packFloatx80(aSign, 0, 0);
    }
    lastBitMask = 1;
    lastBitMask <<= 0x403E - aExp;
    roundBitsMask = lastBitMask - 1;
    z = a;
    roundingMode = get_float_rounding_mode(status);
    if (roundingMode == float_round_nearest_even) {
        z.fraction += lastBitMask>>1;
        if ((z.fraction & roundBitsMask) == 0) z.fraction &= ~lastBitMask;
    }
    else if (roundingMode != float_round_to_zero) {
        if (extractFloatx80Sign(z) ^ (roundingMode == float_round_up))
            z.fraction += roundBitsMask;
    }
    z.fraction &= ~roundBitsMask;
    if (z.fraction == 0) {
        z.exp++;
        z.fraction = BX_CONST64(0x8000000000000000);
    }
    if (z.fraction != a.fraction) {
        float_raise(status, float_flag_inexact);
        if (z.fraction > a.fraction || z.exp > a.exp)
            set_float_rounding_up(status);
    }
    return z;
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

/*----------------------------------------------------------------------------
| Returns the result of dividing the extended double-precision floating-point
| value `a' by the corresponding value `b'.  The operation is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_div(floatx80 a, floatx80 b, float_status_t &status)
{
    int aSign, bSign, zSign;
    Bit32s aExp, bExp, zExp;
    Bit64u aSig, bSig, zSig0, zSig1;
    Bit64u rem0, rem1, rem2, term0, term1, term2;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a) || floatx80_is_unsupported(b))
    {
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
        if ((Bit64u) (aSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (bExp == 0x7FFF) {
            if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
            float_raise(status, float_flag_invalid);
            return floatx80_default_nan;
        }
        if (bSig && (bExp == 0)) float_raise(status, float_flag_denormal);
        return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
    }
    if (bExp == 0x7FFF) {
        if ((Bit64u) (bSig<<1)) return propagateFloatx80NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return packFloatx80(zSign, 0, 0);
    }
    if (bExp == 0) {
        if (bSig == 0) {
            if ((aExp | aSig) == 0) {
                float_raise(status, float_flag_invalid);
                return floatx80_default_nan;
            }
            float_raise(status, float_flag_divbyzero);
            return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloatx80(zSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig, &aExp, &aSig);
    }
    zExp = aExp - bExp + 0x3FFE;
    rem1 = 0;
    if (bSig <= aSig) {
        shift128Right(aSig, 0, 1, &aSig, &rem1);
        ++zExp;
    }
    zSig0 = estimateDiv128To64(aSig, rem1, bSig);
    mul64To128(bSig, zSig0, &term0, &term1);
    sub128(aSig, rem1, term0, term1, &rem0, &rem1);
    while ((Bit64s) rem0 < 0) {
        --zSig0;
        add128(rem0, rem1, 0, bSig, &rem0, &rem1);
    }
    zSig1 = estimateDiv128To64(rem1, 0, bSig);
    if ((Bit64u) (zSig1<<1) <= 8) {
        mul64To128(bSig, zSig1, &term1, &term2);
        sub128(rem1, 0, term1, term2, &rem1, &rem2);
        while ((Bit64s) rem1 < 0) {
            --zSig1;
            add128(rem1, rem2, 0, bSig, &rem1, &rem2);
        }
        zSig1 |= ((rem1 | rem2) != 0);
    }
    return
        roundAndPackFloatx80(get_float_rounding_precision(status),
            zSign, zExp, zSig0, zSig1, status);
}

/*----------------------------------------------------------------------------
| Returns the square root of the extended double-precision floating-point
| value `a'.  The operation is performed according to the IEC/IEEE Standard
| for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 floatx80_sqrt(floatx80 a, float_status_t &status)
{
    int aSign;
    Bit32s aExp, zExp;
    Bit64u aSig0, aSig1, zSig0, zSig1, doubleZSig0;
    Bit64u rem0, rem1, rem2, rem3, term0, term1, term2, term3;

    // handle unsupported extended double-precision floating encodings
    if (floatx80_is_unsupported(a))
    {
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }

    aSig0 = extractFloatx80Frac(a);
    aExp = extractFloatx80Exp(a);
    aSign = extractFloatx80Sign(a);
    if (aExp == 0x7FFF) {
        if ((Bit64u) (aSig0<<1)) return propagateFloatx80NaN(a, status);
        if (! aSign) return a;
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }
    if (aSign) {
        if ((aExp | aSig0) == 0) return a;
        float_raise(status, float_flag_invalid);
        return floatx80_default_nan;
    }
    if (aExp == 0) {
        if (aSig0 == 0) return packFloatx80(0, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloatx80Subnormal(aSig0, &aExp, &aSig0);
    }
    zExp = ((aExp - 0x3FFF)>>1) + 0x3FFF;
    zSig0 = estimateSqrt32(aExp, aSig0>>32);
    shift128Right(aSig0, 0, 2 + (aExp & 1), &aSig0, &aSig1);
    zSig0 = estimateDiv128To64(aSig0, aSig1, zSig0<<32) + (zSig0<<30);
    doubleZSig0 = zSig0<<1;
    mul64To128(zSig0, zSig0, &term0, &term1);
    sub128(aSig0, aSig1, term0, term1, &rem0, &rem1);
    while ((Bit64s) rem0 < 0) {
        --zSig0;
        doubleZSig0 -= 2;
        add128(rem0, rem1, zSig0>>63, doubleZSig0 | 1, &rem0, &rem1);
    }
    zSig1 = estimateDiv128To64(rem1, 0, doubleZSig0);
    if ((zSig1 & BX_CONST64(0x3FFFFFFFFFFFFFFF)) <= 5) {
        if (zSig1 == 0) zSig1 = 1;
        mul64To128(doubleZSig0, zSig1, &term1, &term2);
        sub128(rem1, 0, term1, term2, &rem1, &rem2);
        mul64To128(zSig1, zSig1, &term2, &term3);
        sub192(rem1, rem2, 0, 0, term2, term3, &rem1, &rem2, &rem3);
        while ((Bit64s) rem1 < 0) {
            --zSig1;
            shortShift128Left(0, zSig1, 1, &term2, &term3);
            term3 |= 1;
            term2 |= doubleZSig0;
            add192(rem1, rem2, rem3, 0, term2, term3, &rem1, &rem2, &rem3);
        }
        zSig1 |= ((rem1 | rem2 | rem3) != 0);
    }
    shortShift128Left(0, zSig1, 1, &zSig0, &zSig1);
    zSig0 |= doubleZSig0;
    return
        roundAndPackFloatx80(get_float_rounding_precision(status),
            0, zExp, zSig0, zSig1, status);
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
