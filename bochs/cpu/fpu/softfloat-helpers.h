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
 * Adapted for Bochs (x86 achitecture simulator) by
 *            Stanislav Shwartsman [sshwarts at sourceforge net]
 * ==========================================================================*/

#ifndef _SOFTFLOAT_HELPERS_H_
#define _SOFTFLOAT_HELPERS_H_

#include <assert.h>

#include "../softfloat3e/include/primitives.h"
#include "../softfloat3e/include/primitiveTypes.h"

/*----------------------------------------------------------------------------
| Returns an approximation to the 64-bit integer quotient obtained by dividing
| `b' into the 128-bit value formed by concatenating `a0' and `a1'.  The
| divisor `b' must be at least 2^63.  If q is the exact quotient truncated
| toward zero, the approximation returned lies between q and q + 2 inclusive.
| If the exact quotient q is larger than 64 bits, the maximum positive 64-bit
| unsigned integer is returned.
*----------------------------------------------------------------------------*/

#ifdef USE_estimateDiv128To64
static Bit64u estimateDiv128To64(Bit64u a0, Bit64u a1, Bit64u b)
{
    uint128 term, rem;
    Bit64u b0, b1;
    Bit64u z;
    if (b <= a0) return BX_CONST64(0xFFFFFFFFFFFFFFFF);
    b0 = b>>32;
    z = (b0<<32 <= a0) ? BX_CONST64(0xFFFFFFFF00000000) : (a0 / b0)<<32;
    term = softfloat_mul64To128(b, z);
    rem = softfloat_sub128(a0, a1, term.v64, term.v0);
    while (((int64_t) rem.v64) < 0) {
        z -= UINT64_C(0x100000000);
        b1 = b<<32;
        rem = softfloat_add128(rem.v64, rem.v0, b0, b1);
    }
    rem.v64 = (rem.v64<<32) | (rem.v0>>32);
    z |= (b0<<32 <= rem.v64) ? 0xFFFFFFFF : rem.v64 / b0;
    return z;
}
#endif

#endif
