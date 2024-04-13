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

/*----------------------------------------------------------------------------
| Multiplies the 128-bit value formed by concatenating `a0' and `a1' by
| `b' to obtain a 192-bit product.  The product is broken into three 64-bit
| pieces which are stored at the locations pointed to by `z0Ptr', `z1Ptr', and
| `z2Ptr'.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE void mul128By64To192(Bit64u a64, Bit64u a0, Bit64u b, Bit64u *z0Ptr, Bit64u *z1Ptr, Bit64u *z2Ptr)
{
    Bit64u zPtr[4];

    softfloat_mul128To256M(a64, a0, 0, b, (uint64_t*) zPtr);

    assert(zPtr[indexWord(4, 3)] == 0);

    *z0Ptr = zPtr[indexWord(4, 2)];
    *z1Ptr = zPtr[indexWord(4, 1)];
    *z2Ptr = zPtr[indexWord(4, 0)];
}

/*----------------------------------------------------------------------------
| Shifts the 128-bit value formed by concatenating `a0' and `a1' left by the
| number of bits given in `count'.  Any bits shifted off are lost.  The value
| of `count' must be less than 64.  The result is broken into two 64-bit
| pieces which are stored at the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE void shortShift128Left(Bit64u a0, Bit64u a1, int count, Bit64u *z0Ptr, Bit64u *z1Ptr)
{
    *z1Ptr = a1<<count;
    *z0Ptr = (count == 0) ? a0 : (a0<<count) | (a1>>((-count) & 63));
}

/*----------------------------------------------------------------------------
| Shifts the 128-bit value formed by concatenating `a0' and `a1' right by the
| number of bits given in `count'.  Any bits shifted off are lost.  The value
| of `count' must be less than 64. The result is broken into two 64-bit pieces
| which are stored at the locations pointed to by `z0Ptr' and `z1Ptr'.
*----------------------------------------------------------------------------*/

BX_CPP_INLINE void shortShift128Right(Bit64u a0, Bit64u a1, int count, Bit64u *z0Ptr, Bit64u *z1Ptr)
{
    Bit64u z0 = a0, z1 = a1;
    int negCount = (-count) & 63;

    if (count != 0) {
        z1 = (a0<<negCount) | (a1>>count);
        z0 = a0>>count;
    }
    *z1Ptr = z1;
    *z0Ptr = z0;
}

#endif
