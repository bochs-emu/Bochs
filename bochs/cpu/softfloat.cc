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

/*============================================================================
 * Adapted for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman (gate@fidonet.org.il)
 * ==========================================================================*/ 

#include "softfloat.h"

/*----------------------------------------------------------------------------
| Primitive arithmetic functions, including multi-word arithmetic, and
| division and square root approximations.  (Can be specialized to target if
| desired.)
*----------------------------------------------------------------------------*/
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
| Takes a 64-bit fixed-point value `absZ' with binary point between bits 6
| and 7, and returns the properly rounded 32-bit integer corresponding to the
| input.  If `zSign' is 1, the input is negated before being converted to an
| integer.  Bit 63 of `absZ' must be zero.  Ordinarily, the fixed-point input
| is simply rounded to an integer, with the inexact exception raised if the
| input cannot be represented exactly as an integer.  However, if the fixed-
| point input is too large, the invalid exception is raised and the largest
| positive or negative integer is returned.
*----------------------------------------------------------------------------*/

static Bit32s roundAndPackInt32(flag zSign, Bit64u absZ, float_status_t &status)
{
    int roundingMode;
    Bit8s roundIncrement, roundBits;
    Bit32s z;

    roundingMode = get_float_rounding_mode(status);
    int roundNearestEven = (roundingMode == float_round_nearest_even);
    roundIncrement = 0x40;
    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) {
            roundIncrement = 0;
        }
        else {
            roundIncrement = 0x7F;
            if (zSign) {
                if (roundingMode == float_round_up) roundIncrement = 0;
            }
            else {
                if (roundingMode == float_round_down) roundIncrement = 0;
            }
        }
    }
    roundBits = absZ & 0x7F;
    absZ = (absZ + roundIncrement)>>7;
    absZ &= ~(((roundBits ^ 0x40) == 0) & roundNearestEven);
    z = absZ;
    if (zSign) z = -z;
    if ((absZ>>32) || (z && ((z < 0) ^ zSign))) {
        float_raise(status, float_flag_invalid);
        return zSign ? (Bit32s) 0x80000000 : 0x7FFFFFFF;
    }
    if (roundBits) float_raise(status, float_flag_inexact);
    return z;
}

/*----------------------------------------------------------------------------
| Takes the 128-bit fixed-point value formed by concatenating `absZ0' and
| `absZ1', with binary point between bits 63 and 64 (between the input words),
| and returns the properly rounded 64-bit integer corresponding to the input.
| If `zSign' is 1, the input is negated before being converted to an integer.
| Ordinarily, the fixed-point input is simply rounded to an integer, with
| the inexact exception raised if the input cannot be represented exactly as
| an integer.  However, if the fixed-point input is too large, the invalid
| exception is raised and the largest positive or negative integer is
| returned.
*----------------------------------------------------------------------------*/

static Bit64s roundAndPackInt64(flag zSign, Bit64u absZ0, Bit64u absZ1, float_status_t &status)
{
    int roundingMode;
    flag roundNearestEven, increment;
    Bit64s z;

    roundingMode = get_float_rounding_mode(status);
    roundNearestEven = (roundingMode == float_round_nearest_even);
    increment = ((Bit64s) absZ1 < 0);
    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) {
            increment = 0;
        }
        else {
            if (zSign) {
                increment = (roundingMode == float_round_down) && absZ1;
            }
            else {
                increment = (roundingMode == float_round_up) && absZ1;
            }
        }
    }
    if (increment) {
        ++absZ0;
        if (absZ0 == 0) goto overflow;
        absZ0 &= ~(((Bit64u) (absZ1<<1) == 0) & roundNearestEven);
    }
    z = absZ0;
    if (zSign) z = -z;
    if (z && ((z < 0) ^ zSign)) {
 overflow:
        float_raise(status, float_flag_invalid);
        return
              zSign ? (Bit64s) BX_CONST64(0x8000000000000000) :
                               BX_CONST64(0x7FFFFFFFFFFFFFFF);
    }
    if (absZ1) float_raise(status, float_flag_inexact);
    return z;
}

/*----------------------------------------------------------------------------
| Determine single-precision floating-point number class
*----------------------------------------------------------------------------*/

float_class_t float32_class(float32 a)
{
   Bit16s aExp = extractFloat32Exp(a);
   Bit32u aSig = extractFloat32Frac(a);
   flag  aSign = extractFloat32Sign(a);

   if(aExp == 0xFF) {
       if (aSig == 0) {
           return (aSign) ? float_negative_inf : float_positive_inf;
       }
       return float_NaN;
   }

   if(aExp == 0) {
       if (aSig == 0) {
           return (aSign) ? float_negative_zero : float_positive_zero;
       }
       return float_denormal;
   }

   return float_normalized;
}

/*----------------------------------------------------------------------------
| Normalizes the subnormal single-precision floating-point value represented
| by the denormalized significand `aSig'.  The normalized exponent and
| significand are stored at the locations pointed to by `zExpPtr' and
| `zSigPtr', respectively.
*----------------------------------------------------------------------------*/

static void
    normalizeFloat32Subnormal(Bit32u aSig, Bit16s *zExpPtr, Bit32u *zSigPtr)
{
    int shiftCount = countLeadingZeros32(aSig) - 8;
    *zSigPtr = aSig<<shiftCount;
    *zExpPtr = 1 - shiftCount;
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand `zSig', and returns the proper single-precision floating-
| point value corresponding to the abstract input.  Ordinarily, the abstract
| value is simply rounded and packed into the single-precision format, with
| the inexact exception raised if the abstract input cannot be represented
| exactly.  However, if the abstract value is too large, the overflow and
| inexact exceptions are raised and an infinity or maximal finite value is
| returned.  If the abstract value is too small, the input value is rounded to
| a subnormal number, and the underflow and inexact exceptions are raised if
| the abstract input cannot be represented exactly as a subnormal single-
| precision floating-point number.
|     The input significand `zSig' has its binary point between bits 30
| and 29, which is 7 bits to the left of the usual location.  This shifted
| significand must be normalized or smaller.  If `zSig' is not normalized,
| `zExp' must be 0; in that case, the result returned is a subnormal number,
| and it must not require rounding.  In the usual case that `zSig' is
| normalized, `zExp' must be 1 less than the ``true'' floating-point exponent.
| The handling of underflow and overflow follows the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float32 roundAndPackFloat32(flag zSign, Bit16s zExp, Bit32u zSig, float_status_t &status)
{
    int roundingMode;
    Bit32s roundIncrement, roundBits, roundMask;

    roundingMode = get_float_rounding_mode(status);
    int roundNearestEven = (roundingMode == float_round_nearest_even);
    roundIncrement = 0x40;
    roundMask = 0x7F;

    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) {
            roundIncrement = 0;
        }
        else {
            roundIncrement = roundMask;
            if (zSign) {
                if (roundingMode == float_round_up) roundIncrement = 0;
            }
            else {
                if (roundingMode == float_round_down) roundIncrement = 0;
            }
        }
    }
    roundBits = zSig & roundMask;
    if (0xFD <= (Bit16u) zExp) {
        if ((0xFD < zExp)
             || ((zExp == 0xFD)
                  && ((Bit32s) (zSig + roundIncrement) < 0)))
        {
            float_raise(status, float_flag_overflow | float_flag_inexact);
            return packFloat32(zSign, 0xFF, 0) - (roundIncrement == 0);
        }
        if (zExp < 0) {
            int isTiny =
                   (status.float_detect_tininess == float_tininess_before_rounding)
                || (zExp < -1)
                || (zSig + roundIncrement < 0x80000000);
            shift32RightJamming(zSig, -zExp, &zSig);
            zExp = 0;
            roundBits = zSig & roundMask;
            if (isTiny && roundBits) {
                float_raise(status, float_flag_underflow);
                if(get_flush_underflow_to_zero(status)) {
                    float_raise(status, float_flag_inexact);
                    return packFloat32(zSign, 0, 0);
                }
            }
        }
    }
    if (roundBits) float_raise(status, float_flag_inexact);
    zSig = ((zSig + roundIncrement) & ~roundMask) >> 7;
    zSig &= ~(((roundBits ^ 0x40) == 0) & roundNearestEven);
    if (zSig == 0) zExp = 0;
    return packFloat32(zSign, zExp, zSig);
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand `zSig', and returns the proper single-precision floating-
| point value corresponding to the abstract input.  This routine is just like
| `roundAndPackFloat32' except that `zSig' does not have to be normalized.
| Bit 31 of `zSig' must be zero, and `zExp' must be 1 less than the ``true''
| floating-point exponent.
*----------------------------------------------------------------------------*/

static float32
    normalizeRoundAndPackFloat32(flag zSign, Bit16s zExp, Bit32u zSig, float_status_t &status)
{
    int shiftCount = countLeadingZeros32(zSig) - 1;
    return roundAndPackFloat32(zSign, zExp - shiftCount, zSig<<shiftCount, status);
}

/*----------------------------------------------------------------------------
| Determine double-precision floating-point number class
*----------------------------------------------------------------------------*/

float_class_t float64_class(float64 a)
{
   Bit16s aExp = extractFloat64Exp(a);
   Bit64u aSig = extractFloat64Frac(a);
   flag  aSign = extractFloat64Sign(a);

   if(aExp == 0x7FF) {
       if (aSig == 0) {
           return (aSign) ? float_negative_inf : float_positive_inf;
       }
       return float_NaN;
   }
    
   if(aExp == 0) {
       if (aSig == 0) {
           return (aSign) ? float_negative_zero : float_positive_zero;
       }
       return float_denormal;
   }

   return float_normalized;
}

/*----------------------------------------------------------------------------
| Normalizes the subnormal double-precision floating-point value represented
| by the denormalized significand `aSig'.  The normalized exponent and
| significand are stored at the locations pointed to by `zExpPtr' and
| `zSigPtr', respectively.
*----------------------------------------------------------------------------*/

static void
    normalizeFloat64Subnormal(Bit64u aSig, Bit16s *zExpPtr, Bit64u *zSigPtr)
{
    int shiftCount = countLeadingZeros64(aSig) - 11;
    *zSigPtr = aSig<<shiftCount;
    *zExpPtr = 1 - shiftCount;
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand `zSig', and returns the proper double-precision floating-
| point value corresponding to the abstract input.  Ordinarily, the abstract
| value is simply rounded and packed into the double-precision format, with
| the inexact exception raised if the abstract input cannot be represented
| exactly.  However, if the abstract value is too large, the overflow and
| inexact exceptions are raised and an infinity or maximal finite value is
| returned.  If the abstract value is too small, the input value is rounded
| to a subnormal number, and the underflow and inexact exceptions are raised
| if the abstract input cannot be represented exactly as a subnormal double-
| precision floating-point number.
|     The input significand `zSig' has its binary point between bits 62
| and 61, which is 10 bits to the left of the usual location.  This shifted
| significand must be normalized or smaller.  If `zSig' is not normalized,
| `zExp' must be 0; in that case, the result returned is a subnormal number,
| and it must not require rounding.  In the usual case that `zSig' is
| normalized, `zExp' must be 1 less than the ``true'' floating-point exponent.
| The handling of underflow and overflow follows the IEC/IEEE Standard for
| Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

static float64 roundAndPackFloat64(flag zSign, Bit16s zExp, Bit64u zSig, float_status_t &status)
{
    int roundingMode;
    Bit16s roundIncrement, roundBits;

    roundingMode = get_float_rounding_mode(status);
    int roundNearestEven = (roundingMode == float_round_nearest_even);
    roundIncrement = 0x200;
    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) {
            roundIncrement = 0;
        }
        else {
            roundIncrement = 0x3FF;
            if (zSign) {
                if (roundingMode == float_round_up) roundIncrement = 0;
            }
            else {
                if (roundingMode == float_round_down) roundIncrement = 0;
            }
        }
    }
    roundBits = zSig & 0x3FF;
    if (0x7FD <= (Bit16u) zExp) {
        if ((0x7FD < zExp)
             || ((zExp == 0x7FD)
                  && ((Bit64s) (zSig + roundIncrement) < 0)))
        {
            float_raise(status, float_flag_overflow | float_flag_inexact);
            return packFloat64(zSign, 0x7FF, 0) - (roundIncrement == 0);
        }
        if (zExp < 0) {
            int isTiny =
                   (status.float_detect_tininess == float_tininess_before_rounding)
                || (zExp < -1)
                || (zSig + roundIncrement < BX_CONST64(0x8000000000000000));
            shift64RightJamming(zSig, -zExp, &zSig);
            zExp = 0;
            roundBits = zSig & 0x3FF;
            if (isTiny && roundBits) {
                float_raise(status, float_flag_underflow);
                if(get_flush_underflow_to_zero(status)) {
                    float_raise(status, float_flag_inexact);
                    return packFloat64(zSign, 0, 0);
                }
            }
        }
    }
    if (roundBits) float_raise(status, float_flag_inexact);
    zSig = (zSig + roundIncrement)>>10;
    zSig &= ~(((roundBits ^ 0x200) == 0) & roundNearestEven);
    if (zSig == 0) zExp = 0;
    return packFloat64(zSign, zExp, zSig);
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand `zSig', and returns the proper double-precision floating-
| point value corresponding to the abstract input.  This routine is just like
| `roundAndPackFloat64' except that `zSig' does not have to be normalized.
| Bit 63 of `zSig' must be zero, and `zExp' must be 1 less than the ``true''
| floating-point exponent.
*----------------------------------------------------------------------------*/

static float64
    normalizeRoundAndPackFloat64(flag zSign, Bit16s zExp, Bit64u zSig, float_status_t &status)
{
    int shiftCount = countLeadingZeros64(zSig) - 1;
    return roundAndPackFloat64(zSign, zExp - shiftCount, zSig<<shiftCount, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the single-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 int32_to_float32(Bit32s a, float_status_t &status)
{
    if (a == 0) return 0;
    if (a == (Bit32s) 0x80000000) return packFloat32(1, 0x9E, 0);
    flag zSign = (a < 0);
    return normalizeRoundAndPackFloat32(zSign, 0x9C, zSign ? -a : a, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 32-bit two's complement integer `a'
| to the double-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 int32_to_float64(Bit32s a)
{
    Bit32u absA;
    int shiftCount;
    Bit64u zSig;

    if (a == 0) return 0;
    flag zSign = (a < 0);
    absA = zSign ? -a : a;
    shiftCount = countLeadingZeros32(absA) + 21;
    zSig = absA;
    return packFloat64(zSign, 0x432 - shiftCount, zSig<<shiftCount);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the 64-bit two's complement integer `a'
| to the single-precision floating-point format.  The conversion is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 int64_to_float32(Bit64s a, float_status_t &status)
{
    Bit64u absA;
    int shiftCount;
    Bit32u zSig;

    if (a == 0) return 0;
    flag zSign = (a < 0);
    absA = zSign ? -a : a;
    shiftCount = countLeadingZeros64(absA) - 40;
    if (0 <= shiftCount) {
        return packFloat32(zSign, 0x95 - shiftCount, absA<<shiftCount);
    }
    else {
        shiftCount += 7;
        if (shiftCount < 0) {
            shift64RightJamming(absA, -shiftCount, &absA);
        }
        else {
            absA <<= shiftCount;
        }
        return roundAndPackFloat32(zSign, 0x9C - shiftCount, absA, status);
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
    flag zSign = (a < 0);
    return normalizeRoundAndPackFloat64(zSign, 0x43C, zSign ? -a : a, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic---which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN, the largest
| positive integer is returned.  Otherwise, if the conversion overflows, the
| largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

Bit32s float32_to_int32(float32 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit32u aSig;
    Bit64u aSig64;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    if ((aExp == 0xFF) && aSig) aSign = 0;
    if (aExp) aSig |= 0x00800000;
    shiftCount = 0xAF - aExp;
    aSig64 = aSig;
    aSig64 <<= 32;
    if (0 < shiftCount) shift64RightJamming(aSig64, shiftCount, &aSig64);
    return roundAndPackInt32(aSign, aSig64, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN, the largest positive integer is returned.  Otherwise, if
| the conversion overflows, the largest integer with the same sign as `a' is
| returned.
*----------------------------------------------------------------------------*/

Bit32s float32_to_int32_round_to_zero(float32 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit32u aSig;
    Bit32s z;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    shiftCount = aExp - 0x9E;
    if (0 <= shiftCount) {
        if (a != 0xCF000000) {
            float_raise(status, float_flag_invalid);
            if (! aSign || ((aExp == 0xFF) && aSig)) return 0x7FFFFFFF;
        }
        return (Bit32s) 0x80000000;
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
| Arithmetic---which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN, the largest
| positive integer is returned.  Otherwise, if the conversion overflows, the
| largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

Bit64s float32_to_int64(float32 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit32u aSig;
    Bit64u aSig64, aSigExtra;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    shiftCount = 0xBE - aExp;
    if (shiftCount < 0) {
        float_raise(status, float_flag_invalid);
        if (! aSign || ((aExp == 0xFF) && aSig)) {
            return BX_CONST64(0x7FFFFFFFFFFFFFFF);
        }
        return (Bit64s) BX_CONST64(0x8000000000000000);
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
| Arithmetic, except that the conversion is always rounded toward zero.  If
| `a' is a NaN, the largest positive integer is returned.  Otherwise, if the
| conversion overflows, the largest integer with the same sign as `a' is
| returned.
*----------------------------------------------------------------------------*/

Bit64s float32_to_int64_round_to_zero(float32 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit32u aSig;
    Bit64u aSig64;
    Bit64s z;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    shiftCount = aExp - 0xBE;
    if (0 <= shiftCount) {
        if (a != 0xDF000000) {
            float_raise(status, float_flag_invalid);
            if (! aSign || ((aExp == 0xFF) && aSig)) {
                return BX_CONST64(0x7FFFFFFFFFFFFFFF);
            }
        }
        return (Bit64s) BX_CONST64(0x8000000000000000);
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
    flag  aSign = extractFloat32Sign(a);

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
    flag aSign;
    Bit16s aExp;
    Bit32u lastBitMask, roundBitsMask;
    int roundingMode;
    float32 z;

    aExp = extractFloat32Exp(a);
    if (0x96 <= aExp) {
        if ((aExp == 0xFF) && extractFloat32Frac(a)) {
            return propagateFloat32NaN(a, a, status);
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

static float32 addFloat32Sigs(float32 a, float32 b, flag zSign, float_status_t &status)
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
        if ((aExp == 0) && aSig) {
            float_raise(status, float_flag_denormal);
        }
        if (bExp == 0) {
            if (bSig) float_raise(status, float_flag_denormal);
            --expDiff;
        }
        else {
            bSig |= 0x20000000;
        }
        shift32RightJamming(bSig, expDiff, &bSig);
        zExp = aExp;
    }
    else if (expDiff < 0) {
        if (bExp == 0xFF) {
            if (bSig) return propagateFloat32NaN(a, b, status);
            if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat32(zSign, 0xFF, 0);
        }
        if ((bExp == 0) && bSig) {
            float_raise(status, float_flag_denormal);
        }
        if (aExp == 0) {
            if (aSig) float_raise(status, float_flag_denormal);
            ++expDiff;
        }
        else {
            aSig |= 0x20000000;
        }
        shift32RightJamming(aSig, -expDiff, &aSig);
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

static float32 subFloat32Sigs(float32 a, float32 b, flag zSign, float_status_t &status)
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
    if ((bExp == 0) && bSig) {
        float_raise(status, float_flag_denormal);
    }
    if (aExp == 0) {
        if (aSig) float_raise(status, float_flag_denormal);
        ++expDiff;
    }
    else {
        aSig |= 0x40000000;
    }
    shift32RightJamming(aSig, -expDiff, &aSig);
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
    if ((aExp == 0) && aSig) {
        float_raise(status, float_flag_denormal);
    }
    if (bExp == 0) {
        if (bSig) float_raise(status, float_flag_denormal);
        --expDiff;
    }
    else {
        bSig |= 0x40000000;
    }
    shift32RightJamming(bSig, expDiff, &bSig);
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
    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);

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
    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);

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
    flag aSign, bSign, zSign;
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
        if (aSig || ((bExp == 0xFF) && bSig)) {
            return propagateFloat32NaN(a, b, status);
        }
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
    shift64RightJamming(((Bit64u) aSig) * bSig, 32, &zSig64);
    zSig = zSig64;
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
    flag aSign, bSign, zSign;
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
| Returns the remainder of the single-precision floating-point value `a'
| with respect to the corresponding value `b'.  The operation is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_rem(float32 a, float32 b, float_status_t &status)
{
    flag aSign, bSign, zSign;
    Bit16s aExp, bExp, expDiff;
    Bit32u aSig, bSig;
    Bit32u q;
    Bit64u aSig64, bSig64, q64;
    Bit32u alternateASig;
    Bit32s sigMean;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    bSig = extractFloat32Frac(b);
    bExp = extractFloat32Exp(b);
    bSign = extractFloat32Sign(b);
    if (aExp == 0xFF) {
        if (aSig || ((bExp == 0xFF) && bSig)) {
            return propagateFloat32NaN(a, b, status);
        }
        float_raise(status, float_flag_invalid);
        return float32_default_nan;
    }
    if (bExp == 0xFF) {
        if (bSig) return propagateFloat32NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if (bExp == 0) {
        if (bSig == 0) {
            float_raise(status, float_flag_invalid);
            return float32_default_nan;
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat32(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat32Subnormal(aSig, &aExp, &aSig);
    }
    expDiff = aExp - bExp;
    aSig |= 0x00800000;
    bSig |= 0x00800000;
    if (expDiff < 32) {
        aSig <<= 8;
        bSig <<= 8;
        if (expDiff < 0) {
            if (expDiff < -1) return a;
            aSig >>= 1;
        }
        q = (bSig <= aSig);
        if (q) aSig -= bSig;
        if (0 < expDiff) {
            q = (((Bit64u) aSig)<<32) / bSig;
            q >>= 32 - expDiff;
            bSig >>= 2;
            aSig = ((aSig>>1)<<(expDiff - 1)) - bSig * q;
        }
        else {
            aSig >>= 2;
            bSig >>= 2;
        }
    }
    else {
        if (bSig <= aSig) aSig -= bSig;
        aSig64 = ((Bit64u) aSig)<<40;
        bSig64 = ((Bit64u) bSig)<<40;
        expDiff -= 64;
        while (0 < expDiff) {
            q64 = estimateDiv128To64(aSig64, 0, bSig64);
            q64 = (2 < q64) ? q64 - 2 : 0;
            aSig64 = -((bSig * q64)<<38);
            expDiff -= 62;
        }
        expDiff += 64;
        q64 = estimateDiv128To64(aSig64, 0, bSig64);
        q64 = (2 < q64) ? q64 - 2 : 0;
        q = q64>>(64 - expDiff);
        bSig <<= 6;
        aSig = ((aSig64>>33)<<(expDiff - 1)) - bSig * q;
    }
    do {
        alternateASig = aSig;
        ++q;
        aSig -= bSig;
    } while (0 <= (Bit32s) aSig);
    sigMean = aSig + alternateASig;
    if ((sigMean < 0) || ((sigMean == 0) && (q & 1))) {
        aSig = alternateASig;
    }
    zSign = ((Bit32s) aSig < 0);
    if (zSign) aSig = -aSig;
    return normalizeRoundAndPackFloat32(aSign ^ zSign, bExp, aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the square root of the single-precision floating-point value `a'.
| The operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float32 float32_sqrt(float32 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, zExp;
    Bit32u aSig, zSig;
    Bit64u rem, term;

    aSig = extractFloat32Frac(a);
    aExp = extractFloat32Exp(a);
    aSign = extractFloat32Sign(a);
    if (aExp == 0xFF) {
        if (aSig) return propagateFloat32NaN(a, 0, status);
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
    shift32RightJamming(zSig, 1, &zSig);
 roundAndPack:
    return roundAndPackFloat32(0, zExp, zSig, status);
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
    if (aSign != bSign) {
        return (aSign) ? float_relation_less : float_relation_greater;
    }
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

    flag aSign = extractFloat32Sign(a);
    flag bSign = extractFloat32Sign(b);
    if (aSign != bSign) {
        return (aSign) ? float_relation_less : float_relation_greater;
    }
    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic---which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN, the largest
| positive integer is returned.  Otherwise, if the conversion overflows, the
| largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

Bit32s float64_to_int32(float64 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit64u aSig;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if ((aExp == 0x7FF) && aSig) aSign = 0;
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    shiftCount = 0x42C - aExp;
    if (0 < shiftCount) shift64RightJamming(aSig, shiftCount, &aSig);
    return roundAndPackInt32(aSign, aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the result of converting the double-precision floating-point value
| `a' to the 32-bit two's complement integer format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic, except that the conversion is always rounded toward zero.
| If `a' is a NaN, the largest positive integer is returned.  Otherwise, if
| the conversion overflows, the largest integer with the same sign as `a' is
| returned.
*----------------------------------------------------------------------------*/

Bit32s float64_to_int32_round_to_zero(float64 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit64u aSig, savedASig;
    Bit32s z;

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
    z = aSig;
    if (aSign) z = -z;
    if ((z < 0) ^ aSign) {
 invalid:
        float_raise(status, float_flag_invalid);
        return aSign ? (Bit32s) 0x80000000 : 0x7FFFFFFF;
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
| Arithmetic---which means in particular that the conversion is rounded
| according to the current rounding mode.  If `a' is a NaN, the largest
| positive integer is returned.  Otherwise, if the conversion overflows, the
| largest integer with the same sign as `a' is returned.
*----------------------------------------------------------------------------*/

Bit64s float64_to_int64(float64 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit64u aSig, aSigExtra;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    shiftCount = 0x433 - aExp;
    if (shiftCount <= 0) {
        if (0x43E < aExp) {
            float_raise(status, float_flag_invalid);
            if (! aSign || ((aExp == 0x7FF)
                      && (aSig != BX_CONST64(0x0010000000000000))))
            {
                return BX_CONST64(0x7FFFFFFFFFFFFFFF);
            }
            return (Bit64s) BX_CONST64(0x8000000000000000);
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
| If `a' is a NaN, the largest positive integer is returned.  Otherwise, if
| the conversion overflows, the largest integer with the same sign as `a' is
| returned.
*----------------------------------------------------------------------------*/

Bit64s float64_to_int64_round_to_zero(float64 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, shiftCount;
    Bit64u aSig;
    Bit64s z;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp) aSig |= BX_CONST64(0x0010000000000000);
    shiftCount = aExp - 0x433;
    if (0 <= shiftCount) {
        if (0x43E <= aExp) {
            if (a != BX_CONST64(0xC3E0000000000000)) {
                float_raise(status, float_flag_invalid);
                if (! aSign || ((aExp == 0x7FF)
                          && (aSig != BX_CONST64(0x0010000000000000))))
                {
                    return BX_CONST64(0x7FFFFFFFFFFFFFFF);
                }
            }
            return (Bit64s) BX_CONST64(0x8000000000000000);
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
    flag aSign;
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
    shift64RightJamming(aSig, 22, &aSig);
    zSig = aSig;
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
    flag aSign;
    Bit16s aExp;
    Bit64u lastBitMask, roundBitsMask;
    int roundingMode;
    float64 z;

    aExp = extractFloat64Exp(a);
    if (0x433 <= aExp) {
        if ((aExp == 0x7FF) && extractFloat64Frac(a)) {
            return propagateFloat64NaN(a, a, status);
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

static float64 addFloat64Sigs(float64 a, float64 b, flag zSign, float_status_t &status)
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
        if ((aExp == 0) && aSig) {
            float_raise(status, float_flag_denormal);
        }
        if (bExp == 0) {
            if (bSig) float_raise(status, float_flag_denormal);
            --expDiff;
        }
        else {
            bSig |= BX_CONST64(0x2000000000000000);
        }
        shift64RightJamming(bSig, expDiff, &bSig);
        zExp = aExp;
    }
    else if (expDiff < 0) {
        if (bExp == 0x7FF) {
            if (bSig) return propagateFloat64NaN(a, b, status);
            if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
            return packFloat64(zSign, 0x7FF, 0);
        }
        if ((bExp == 0) && bSig) {
            float_raise(status, float_flag_denormal);
        }
        if (aExp == 0) {
            if (aSig) float_raise(status, float_flag_denormal);
            ++expDiff;
        }
        else {
            aSig |= BX_CONST64(0x2000000000000000);
        }
        shift64RightJamming(aSig, -expDiff, &aSig);
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

static float64 subFloat64Sigs(float64 a, float64 b, flag zSign, float_status_t &status)
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
    if ((bExp == 0) && bSig) {
        float_raise(status, float_flag_denormal);
    }
    if (aExp == 0) {
        if (aSig) float_raise(status, float_flag_denormal);
        ++expDiff;
    }
    else {
        aSig |= BX_CONST64(0x4000000000000000);
    }
    shift64RightJamming(aSig, -expDiff, &aSig);
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
    if ((aExp == 0) && aSig) {
        float_raise(status, float_flag_denormal);
    }
    if (bExp == 0) {
        if (bSig) float_raise(status, float_flag_denormal);
        --expDiff;
    }
    else {
        bSig |= BX_CONST64(0x4000000000000000);
    }
    shift64RightJamming(bSig, expDiff, &bSig);
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
    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);

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
    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);

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
    flag aSign, bSign, zSign;
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
    flag aSign, bSign, zSign;
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
| Returns the remainder of the double-precision floating-point value `a'
| with respect to the corresponding value `b'.  The operation is performed
| according to the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_rem(float64 a, float64 b, float_status_t &status)
{
    flag aSign, bSign, zSign;
    Bit16s aExp, bExp, expDiff;
    Bit64u aSig, bSig;
    Bit64u q, alternateASig;
    Bit64s sigMean;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    bSig = extractFloat64Frac(b);
    bExp = extractFloat64Exp(b);
    bSign = extractFloat64Sign(b);
    if (aExp == 0x7FF) {
        if (aSig || ((bExp == 0x7FF) && bSig)) {
            return propagateFloat64NaN(a, b, status);
        }
        float_raise(status, float_flag_invalid);
        return float64_default_nan;
    }
    if (bExp == 0x7FF) {
        if (bSig) return propagateFloat64NaN(a, b, status);
        if (aSig && (aExp == 0)) float_raise(status, float_flag_denormal);
        return a;
    }
    if (bExp == 0) {
        if (bSig == 0) {
            float_raise(status, float_flag_invalid);
            return float64_default_nan;
        }
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(bSig, &bExp, &bSig);
    }
    if (aExp == 0) {
        if (aSig == 0) return packFloat64(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
        normalizeFloat64Subnormal(aSig, &aExp, &aSig);
    }
    expDiff = aExp - bExp;
    aSig = (aSig | BX_CONST64(0x0010000000000000))<<11;
    bSig = (bSig | BX_CONST64(0x0010000000000000))<<11;
    if (expDiff < 0) {
        if (expDiff < -1) return a;
        aSig >>= 1;
    }
    q = (bSig <= aSig);
    if (q) aSig -= bSig;
    expDiff -= 64;
    while (0 < expDiff) {
        q = estimateDiv128To64(aSig, 0, bSig);
        q = (2 < q) ? q - 2 : 0;
        aSig = -((bSig>>2) * q);
        expDiff -= 62;
    }
    expDiff += 64;
    if (0 < expDiff) {
        q = estimateDiv128To64(aSig, 0, bSig);
        q = (2 < q) ? q - 2 : 0;
        q >>= 64 - expDiff;
        bSig >>= 2;
        aSig = ((aSig>>1)<<(expDiff - 1)) - bSig * q;
    }
    else {
        aSig >>= 2;
        bSig >>= 2;
    }
    do {
        alternateASig = aSig;
        ++q;
        aSig -= bSig;
    } while (0 <= (Bit64s) aSig);
    sigMean = aSig + alternateASig;
    if ((sigMean < 0) || ((sigMean == 0) && (q & 1))) {
        aSig = alternateASig;
    }
    zSign = ((Bit64s) aSig < 0);
    if (zSign) aSig = -aSig;
    return normalizeRoundAndPackFloat64(aSign ^ zSign, bExp, aSig, status);
}

/*----------------------------------------------------------------------------
| Returns the square root of the double-precision floating-point value `a'.
| The operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 float64_sqrt(float64 a, float_status_t &status)
{
    flag aSign;
    Bit16s aExp, zExp;
    Bit64u aSig, zSig, doubleZSig;
    Bit64u rem0, rem1, term0, term1;
    float64 z;

    aSig = extractFloat64Frac(a);
    aExp = extractFloat64Exp(a);
    aSign = extractFloat64Sign(a);
    if (aExp == 0x7FF) {
        if (aSig) return propagateFloat64NaN(a, a, status);
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
    zSig = estimateSqrt32(aExp, aSig>>21);
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
    if (aSign != bSign) {
        return (aSign) ? float_relation_less : float_relation_greater;
    }
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

    flag aSign = extractFloat64Sign(a);
    flag bSign = extractFloat64Sign(b);
    if (aSign != bSign) {
        return (aSign) ? float_relation_less : float_relation_greater;
    }
    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}
