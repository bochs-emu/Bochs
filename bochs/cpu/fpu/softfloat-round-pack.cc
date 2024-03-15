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
| Normalizes the subnormal extended double-precision floating-point value
| represented by the denormalized significand `aSig'.  The normalized exponent
| and significand are stored at the locations pointed to by `zExpPtr' and
| `zSigPtr', respectively.
*----------------------------------------------------------------------------*/

void normalizeFloatx80Subnormal(Bit64u aSig, Bit32s *zExpPtr, Bit64u *zSigPtr)
{
    int shiftCount = countLeadingZeros64(aSig);
    *zSigPtr = aSig<<shiftCount;
    *zExpPtr = 1 - shiftCount;
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and extended significand formed by the concatenation of `zSig0' and `zSig1',
| and returns the proper extended double-precision floating-point value
| corresponding to the abstract input.  Ordinarily, the abstract value is
| rounded and packed into the extended double-precision format, with the
| inexact exception raised if the abstract input cannot be represented
| exactly.  However, if the abstract value is too large, the overflow and
| inexact exceptions are raised and an infinity or maximal finite value is
| returned.  If the abstract value is too small, the input value is rounded to
| a subnormal number, and the underflow and inexact exceptions are raised if
| the abstract input cannot be represented exactly as a subnormal extended
| double-precision floating-point number.
|     If `roundingPrecision' is 32 or 64, the result is rounded to the same
| number of bits as single or double precision, respectively.  Otherwise, the
| result is rounded to the full precision of the extended double-precision
| format.
|     The input significand must be normalized or smaller.  If the input
| significand is not normalized, `zExp' must be 0; in that case, the result
| returned is a subnormal number, and it must not require rounding.  The
| handling of underflow and overflow follows the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

floatx80 SoftFloatRoundAndPackFloatx80(int roundingPrecision,
        int zSign, Bit32s zExp, Bit64u zSig0, Bit64u zSig1, float_status_t &status)
{
    Bit64u roundIncrement, roundMask, roundBits;
    int increment;
    Bit64u zSigExact; /* support rounding-up response */

    Bit8u roundingMode = get_float_rounding_mode(status);
    int roundNearestEven = (roundingMode == float_round_nearest_even);
    if (roundingPrecision == 64) {
        roundIncrement = BX_CONST64(0x0000000000000400);
        roundMask = BX_CONST64(0x00000000000007FF);
    }
    else if (roundingPrecision == 32) {
        roundIncrement = BX_CONST64(0x0000008000000000);
        roundMask = BX_CONST64(0x000000FFFFFFFFFF);
    }
    else goto precision80;

    zSig0 |= (zSig1 != 0);
    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) roundIncrement = 0;
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
    roundBits = zSig0 & roundMask;
    if (0x7FFD <= (Bit32u) (zExp - 1)) {
        if ((0x7FFE < zExp)
             || ((zExp == 0x7FFE) && (zSig0 + roundIncrement < zSig0)))
        {
            goto overflow;
        }
        if (zExp <= 0) {
            int isTiny = (zExp < 0) || (zSig0 <= zSig0 + roundIncrement);
            zSig0 = shift64RightJamming(zSig0, 1 - zExp);
            zSigExact = zSig0;
            zExp = 0;
            roundBits = zSig0 & roundMask;
            if (isTiny) {
                if (roundBits || (zSig0 && !float_exception_masked(status, float_flag_underflow)))
                    float_raise(status, float_flag_underflow);
            }
            zSig0 += roundIncrement;
            if ((Bit64s) zSig0 < 0) zExp = 1;
            roundIncrement = roundMask + 1;
            if (roundNearestEven && (roundBits<<1 == roundIncrement))
                roundMask |= roundIncrement;
            zSig0 &= ~roundMask;
            if (roundBits) {
                float_raise(status, float_flag_inexact);
                if (zSig0 > zSigExact) set_float_rounding_up(status);
            }
            return packFloatx80(zSign, zExp, zSig0);
        }
    }
    if (roundBits) float_raise(status, float_flag_inexact);
    zSigExact = zSig0;
    zSig0 += roundIncrement;
    if (zSig0 < roundIncrement) {
        // Basically scale by shifting right and keep overflow
        ++zExp;
        zSig0 = BX_CONST64(0x8000000000000000);
        zSigExact >>= 1; // must scale also, or else later tests will fail
    }
    roundIncrement = roundMask + 1;
    if (roundNearestEven && (roundBits<<1 == roundIncrement))
        roundMask |= roundIncrement;
    zSig0 &= ~roundMask;
    if (zSig0 > zSigExact) set_float_rounding_up(status);
    if (zSig0 == 0) zExp = 0;
    return packFloatx80(zSign, zExp, zSig0);
 precision80:
    increment = ((Bit64s) zSig1 < 0);
    if (! roundNearestEven) {
        if (roundingMode == float_round_to_zero) increment = 0;
        else {
            if (zSign) {
                increment = (roundingMode == float_round_down) && zSig1;
            }
            else {
                increment = (roundingMode == float_round_up) && zSig1;
            }
        }
    }
    if (0x7FFD <= (Bit32u) (zExp - 1)) {
        if ((0x7FFE < zExp)
             || ((zExp == 0x7FFE)
                  && (zSig0 == BX_CONST64(0xFFFFFFFFFFFFFFFF))
                  && increment))
        {
            roundMask = 0;
 overflow:
            float_raise(status, float_flag_overflow | float_flag_inexact);
            if ((roundingMode == float_round_to_zero)
                 || (zSign && (roundingMode == float_round_up))
                 || (! zSign && (roundingMode == float_round_down)))
            {
                return packFloatx80(zSign, 0x7FFE, ~roundMask);
            }
            set_float_rounding_up(status);
            return packFloatx80(zSign, 0x7FFF, BX_CONST64(0x8000000000000000));
        }
        if (zExp <= 0) {
            int isTiny = (zExp < 0) || (! increment)
                || (zSig0 < BX_CONST64(0xFFFFFFFFFFFFFFFF));
            shift64ExtraRightJamming(zSig0, zSig1, 1 - zExp, &zSig0, &zSig1);
            zExp = 0;
            if (isTiny) {
                if (zSig1 || (zSig0 && !float_exception_masked(status, float_flag_underflow)))
                    float_raise(status, float_flag_underflow);
            }
            if (zSig1) float_raise(status, float_flag_inexact);
            if (roundNearestEven) increment = ((Bit64s) zSig1 < 0);
            else {
                if (zSign) {
                    increment = (roundingMode == float_round_down) && zSig1;
                } else {
                    increment = (roundingMode == float_round_up) && zSig1;
                }
            }
            if (increment) {
                zSigExact = zSig0++;
                zSig0 &= ~(((Bit64u) (zSig1<<1) == 0) & roundNearestEven);
                if (zSig0 > zSigExact) set_float_rounding_up(status);
                if ((Bit64s) zSig0 < 0) zExp = 1;
            }
            return packFloatx80(zSign, zExp, zSig0);
        }
    }
    if (zSig1) float_raise(status, float_flag_inexact);
    if (increment) {
        zSigExact = zSig0++;
        if (zSig0 == 0) {
            zExp++;
            zSig0 = BX_CONST64(0x8000000000000000);
            zSigExact >>= 1;  // must scale also, or else later tests will fail
        }
        else {
            zSig0 &= ~(((Bit64u) (zSig1<<1) == 0) & roundNearestEven);
        }
        if (zSig0 > zSigExact) set_float_rounding_up(status);
    }
    else {
        if (zSig0 == 0) zExp = 0;
    }
    return packFloatx80(zSign, zExp, zSig0);
}

floatx80 roundAndPackFloatx80(int roundingPrecision,
        int zSign, Bit32s zExp, Bit64u zSig0, Bit64u zSig1, float_status_t &status)
{
    float_status_t round_status = status;
    floatx80 result = SoftFloatRoundAndPackFloatx80(roundingPrecision, zSign, zExp, zSig0, zSig1, status);

    // bias unmasked undeflow
    if (status.float_exception_flags & ~status.float_exception_masks & float_flag_underflow) {
       float_raise(round_status, float_flag_underflow);
       return SoftFloatRoundAndPackFloatx80(roundingPrecision, zSign, zExp + 0x6000, zSig0, zSig1, status = round_status);
    }

    // bias unmasked overflow
    if (status.float_exception_flags & ~status.float_exception_masks & float_flag_overflow) {
       float_raise(round_status, float_flag_overflow);
       return SoftFloatRoundAndPackFloatx80(roundingPrecision, zSign, zExp - 0x6000, zSig0, zSig1, status = round_status);
    }

    return result;
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent
| `zExp', and significand formed by the concatenation of `zSig0' and `zSig1',
| and returns the proper extended double-precision floating-point value
| corresponding to the abstract input.  This routine is just like
| `roundAndPackFloatx80' except that the input significand does not have to be
| normalized.
*----------------------------------------------------------------------------*/

floatx80 normalizeRoundAndPackFloatx80(int roundingPrecision,
        int zSign, Bit32s zExp, Bit64u zSig0, Bit64u zSig1, float_status_t &status)
{
    if (zSig0 == 0) {
        zSig0 = zSig1;
        zSig1 = 0;
        zExp -= 64;
    }
    int shiftCount = countLeadingZeros64(zSig0);
    shortShift128Left(zSig0, zSig1, shiftCount, &zSig0, &zSig1);
    zExp -= shiftCount;
    return
        roundAndPackFloatx80(roundingPrecision, zSign, zExp, zSig0, zSig1, status);
}

#endif

#ifdef FLOAT128

/*----------------------------------------------------------------------------
| Normalizes the subnormal quadruple-precision floating-point value
| represented by the denormalized significand formed by the concatenation of
| `aSig0' and `aSig1'.  The normalized exponent is stored at the location
| pointed to by `zExpPtr'.  The most significant 49 bits of the normalized
| significand are stored at the location pointed to by `zSig0Ptr', and the
| least significant 64 bits of the normalized significand are stored at the
| location pointed to by `zSig1Ptr'.
*----------------------------------------------------------------------------*/

void normalizeFloat128Subnormal(
     Bit64u aSig0, Bit64u aSig1, Bit32s *zExpPtr, Bit64u *zSig0Ptr, Bit64u *zSig1Ptr)
{
    int shiftCount;

    if (aSig0 == 0) {
        shiftCount = countLeadingZeros64(aSig1) - 15;
        if (shiftCount < 0) {
            *zSig0Ptr = aSig1 >>(-shiftCount);
            *zSig1Ptr = aSig1 << (shiftCount & 63);
        }
        else {
            *zSig0Ptr = aSig1 << shiftCount;
            *zSig1Ptr = 0;
        }
        *zExpPtr = - shiftCount - 63;
    }
    else {
        shiftCount = countLeadingZeros64(aSig0) - 15;
        shortShift128Left(aSig0, aSig1, shiftCount, zSig0Ptr, zSig1Ptr);
        *zExpPtr = 1 - shiftCount;
    }
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and extended significand formed by the concatenation of `zSig0', `zSig1',
| and `zSig2', and returns the proper quadruple-precision floating-point value
| corresponding to the abstract input.  Ordinarily, the abstract value is
| simply rounded and packed into the quadruple-precision format, with the
| inexact exception raised if the abstract input cannot be represented
| exactly.  However, if the abstract value is too large, the overflow and
| inexact exceptions are raised and an infinity or maximal finite value is
| returned.  If the abstract value is too small, the input value is rounded to
| a subnormal number, and the underflow and inexact exceptions are raised if
| the abstract input cannot be represented exactly as a subnormal quadruple-
| precision floating-point number.
|     The input significand must be normalized or smaller.  If the input
| significand is not normalized, `zExp' must be 0; in that case, the result
| returned is a subnormal number, and it must not require rounding.  In the
| usual case that the input significand is normalized, `zExp' must be 1 less
| than the ``true'' floating-point exponent.  The handling of underflow and
| overflow follows the IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float128 roundAndPackFloat128(
     int zSign, Bit32s zExp, Bit64u zSig0, Bit64u zSig1, Bit64u zSig2, float_status_t &status)
{
    int increment = ((Bit64s) zSig2 < 0);
    if (0x7FFD <= (Bit32u) zExp) {
        if ((0x7FFD < zExp)
             || ((zExp == 0x7FFD)
                  && eq128(BX_CONST64(0x0001FFFFFFFFFFFF),
                         BX_CONST64(0xFFFFFFFFFFFFFFFF), zSig0, zSig1)
                  && increment))
        {
            float_raise(status, float_flag_overflow | float_flag_inexact);
            return packFloat128(zSign, 0x7FFF, 0, 0);
        }
        if (zExp < 0) {
            int isTiny = (zExp < -1)
                || ! increment
                || lt128(zSig0, zSig1,
                       BX_CONST64(0x0001FFFFFFFFFFFF),
                       BX_CONST64(0xFFFFFFFFFFFFFFFF));
            shift128ExtraRightJamming(
                zSig0, zSig1, zSig2, -zExp, &zSig0, &zSig1, &zSig2);
            zExp = 0;
            if (isTiny && zSig2) float_raise(status, float_flag_underflow);
            increment = ((Bit64s) zSig2 < 0);
        }
    }
    if (zSig2) float_raise(status, float_flag_inexact);
    if (increment) {
        add128(zSig0, zSig1, 0, 1, &zSig0, &zSig1);
        zSig1 &= ~((zSig2 + zSig2 == 0) & 1);
    }
    else {
        if ((zSig0 | zSig1) == 0) zExp = 0;
    }
    return packFloat128(zSign, zExp, zSig0, zSig1);
}

/*----------------------------------------------------------------------------
| Takes an abstract floating-point value having sign `zSign', exponent `zExp',
| and significand formed by the concatenation of `zSig0' and `zSig1', and
| returns the proper quadruple-precision floating-point value corresponding
| to the abstract input.  This routine is just like `roundAndPackFloat128'
| except that the input significand has fewer bits and does not have to be
| normalized.  In all cases, `zExp' must be 1 less than the ``true'' floating-
| point exponent.
*----------------------------------------------------------------------------*/

float128 normalizeRoundAndPackFloat128(
     int zSign, Bit32s zExp, Bit64u zSig0, Bit64u zSig1, float_status_t &status)
{
    Bit64u zSig2;

    if (zSig0 == 0) {
        zSig0 = zSig1;
        zSig1 = 0;
        zExp -= 64;
    }
    int shiftCount = countLeadingZeros64(zSig0) - 15;
    if (0 <= shiftCount) {
        zSig2 = 0;
        shortShift128Left(zSig0, zSig1, shiftCount, &zSig0, &zSig1);
    }
    else {
        shift128ExtraRightJamming(
            zSig0, zSig1, 0, -shiftCount, &zSig0, &zSig1, &zSig2);
    }
    zExp -= shiftCount;
    return roundAndPackFloat128(zSign, zExp, zSig0, zSig1, zSig2, status);
}

#endif
