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
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#include "softfloat.h"

#ifdef FLOAT16

#include "softfloat-round-pack.h"
#include "softfloat-specialize.h"
#include "softfloat-macros.h"

/*----------------------------------------------------------------------------
| Determine half-precision floating-point number class
*----------------------------------------------------------------------------*/

float_class_t float16_class(float16 a)
{
    Bit16s aExp = extractFloat16Exp(a);
    Bit16u aSig = extractFloat16Frac(a);
    int  aSign = extractFloat16Sign(a);

    if(aExp == 0x1F) {
        if (aSig == 0)
            return (aSign) ? float_negative_inf : float_positive_inf;

        return (aSig & 0x200) ? float_QNaN : float_SNaN;
    }

    if(aExp == 0) {
        if (aSig == 0) return float_zero;
        return float_denormal;
    }

    return float_normalized;
}

/*----------------------------------------------------------------------------
| Compare  between  two  half  precision  floating  point  numbers. Returns
| 'float_relation_equal'  if the operands are equal, 'float_relation_less' if
| the    value    'a'   is   less   than   the   corresponding   value   `b',
| 'float_relation_greater' if the value 'a' is greater than the corresponding
| value `b', or 'float_relation_unordered' otherwise.
*----------------------------------------------------------------------------*/

int float16_compare(float16 a, float16 b, int quiet, float_status_t &status)
{
    if (get_denormals_are_zeros(status)) {
        a = float16_denormal_to_zero(a);
        b = float16_denormal_to_zero(b);
    }

    float_class_t aClass = float16_class(a);
    float_class_t bClass = float16_class(b);

    if (aClass == float_SNaN || bClass == float_SNaN) {
        float_raise(status, float_flag_invalid);
        return float_relation_unordered;
    }

    if (aClass == float_QNaN || bClass == float_QNaN) {
        if (! quiet) float_raise(status, float_flag_invalid);
        return float_relation_unordered;
    }

    if (aClass == float_denormal || bClass == float_denormal) {
        float_raise(status, float_flag_denormal);
    }

    if ((a == b) || ((Bit16u) ((a | b)<<1) == 0)) return float_relation_equal;

    int aSign = extractFloat16Sign(a);
    int bSign = extractFloat16Sign(b);
    if (aSign != bSign)
        return (aSign) ? float_relation_less : float_relation_greater;

    if (aSign ^ (a < b)) return float_relation_less;
    return float_relation_greater;
}

/*----------------------------------------------------------------------------
| Compare between two half precision floating point numbers and return the
| smaller of them.
*----------------------------------------------------------------------------*/

float16 float16_min(float16 a, float16 b, float_status_t &status)
{
    if (get_denormals_are_zeros(status)) {
        a = float16_denormal_to_zero(a);
        b = float16_denormal_to_zero(b);
    }

    return (float16_compare(a, b, status) == float_relation_less) ? a : b;
}

/*----------------------------------------------------------------------------
| Compare between two half precision floating point numbers and return the
| larger of them.
*----------------------------------------------------------------------------*/

float16 float16_max(float16 a, float16 b, float_status_t &status)
{
    if (get_denormals_are_zeros(status)) {
        a = float16_denormal_to_zero(a);
        b = float16_denormal_to_zero(b);
    }

    return (float16_compare(a, b, status) == float_relation_greater) ? a : b;
}

/*----------------------------------------------------------------------------
| Returns the result of converting the single-precision floating-point value
| `a' to the half-precision floating-point format.  The conversion is
| performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float16 float32_to_float16(float32 a, float_status_t &status)
{
    Bit32u aSig = extractFloat32Frac(a);
    Bit16s aExp = extractFloat32Exp(a);
    int aSign = extractFloat32Sign(a);

    if (aExp == 0xFF) {
        if (aSig) return commonNaNToFloat16(float32ToCommonNaN(a, status));
        return packFloat16(aSign, 0x1F, 0);
    }
    if (aExp == 0) {
        if (get_denormals_are_zeros(status)) aSig = 0;
        if (aSig == 0) return packFloat16(aSign, 0, 0);
        float_raise(status, float_flag_denormal);
    }

    aSig = shift32RightJamming(aSig, 9);
    Bit16u zSig = (Bit16u) aSig;
    if (aExp || zSig) {
        zSig |= 0x4000;
        aExp -= 0x71;
    }

    return roundAndPackFloat16(aSign, aExp, zSig, status);
}

/*----------------------------------------------------------------------------
| Rounds the half-precision floating-point value `a' to an integer,
| and returns the result as a half-precision floating-point value. The
| operation is performed according to the IEC/IEEE Standard for Binary
| Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float16 float16_round_to_int(float16 a, Bit8u scale, float_status_t &status)
{
    int aSign;
    Bit16u lastBitMask, roundBitsMask;
    Bit16u z;
    Bit16s aExp = extractFloat16Exp(a);
    scale &= 0xf;

    if ((aExp == 0x1F) && extractFloat16Frac(a)) {
        return propagateFloat16NaN(a, status);
    }

    aExp += scale; // scale the exponent

    if (0x19 <= aExp) {
        return a;
    }

    if (get_denormals_are_zeros(status)) {
        a = float32_denormal_to_zero(a);
    }

    if (aExp <= 0xE) {
        if ((Bit16u) (a<<1) == 0) return a;
        float_raise(status, float_flag_inexact);
        aSign = extractFloat16Sign(a);
        switch(get_float_rounding_mode(status)) {
        case float_round_nearest_even:
            if ((aExp == 0xE) && extractFloat16Frac(a)) {
                return packFloat16(aSign, 0xF - scale, 0);
            }
            break;
        case float_round_down:
            return aSign ? packFloat16(aSign, 0xF - scale, 0) : float16_positive_zero;
        case float_round_up:
            /* -0.0/1.0f */
            return aSign ? float16_negative_zero : packFloat32(0, 0xF - scale, 0);
        }
        return packFloat16(aSign, 0, 0);
    }
    lastBitMask = 1;
    lastBitMask <<= 0x19 - aExp;
    roundBitsMask = lastBitMask - 1;
    z = a;
    switch(get_float_rounding_mode(status)) {
    case float_round_nearest_even:
        z += lastBitMask>>1;
        if ((z & roundBitsMask) == 0) {
            z &= ~lastBitMask;
        }
        break;
    case float_round_to_zero:
        break;
    case float_round_up:
        if (!extractFloat16Sign(z)) {
            z += roundBitsMask;
        }
        break;
    case float_round_down:
        if (extractFloat16Sign(z)) {
            z += roundBitsMask;
        }
        break;
    default:
        break;
    }
    z &= ~roundBitsMask;
    if (z != a) {
        float_raise(status, float_flag_inexact);
    }
    return z;
}

#endif
