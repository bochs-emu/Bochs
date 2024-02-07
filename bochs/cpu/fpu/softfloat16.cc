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
