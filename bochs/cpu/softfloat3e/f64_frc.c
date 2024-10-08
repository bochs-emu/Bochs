/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

/*----------------------------------------------------------------------------
| Extracts the fractional portion of double-precision floating-point value `a',
| and returns the result  as a  double-precision  floating-point value. The
| fractional results are precise. The operation is performed according to the
| IEC/IEEE Standard for Binary Floating-Point Arithmetic.
*----------------------------------------------------------------------------*/

float64 f64_frc(float64 a, softfloat_status_t *status)
{
    int roundingMode = softfloat_getRoundingMode(status);

    bool signA;
    int16_t expA;
    uint64_t sigA;
    uint64_t lastBitMask;
    uint64_t roundBitsMask;

    signA = signF64UI(a);
    expA  = expF64UI(a);
    sigA  = fracF64UI(a);

    if (expA == 0x7FF) {
        if (sigA) return softfloat_propagateNaNF64UI(a, 0, status);
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        return defaultNaNF64UI;
    }

    if (expA >= 0x433) {
        return packToF64UI(roundingMode == softfloat_round_down, 0, 0);
    }

    if (expA < 0x3FF) {
        if (! expA) {
            if (! sigA || softfloat_denormalsAreZeros(status))
                return packToF64UI(roundingMode == softfloat_round_down, 0, 0);

            softfloat_raiseFlags(status, softfloat_flag_denormal);
            if (! softfloat_isMaskedException(status, softfloat_flag_underflow))
                softfloat_raiseFlags(status, softfloat_flag_underflow);

            if (softfloat_flushUnderflowToZero(status)) {
                softfloat_raiseFlags(status, softfloat_flag_underflow | softfloat_flag_inexact);
                return packToF64UI(signA, 0, 0);
            }
        }
        return a;
    }

    lastBitMask = UINT64_C(1) << (0x433 - expA);
    roundBitsMask = lastBitMask - 1;

    sigA &= roundBitsMask;
    sigA <<= 10;
    expA--;

    if (! sigA)
       return packToF64UI(roundingMode == softfloat_round_down, 0, 0);

    return softfloat_normRoundPackToF64(signA, expA, sigA, status);
}
