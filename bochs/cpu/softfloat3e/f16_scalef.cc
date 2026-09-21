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
| Return the result of a floating point scale of the half-precision floating
| point value `a' by multiplying it by 2 power of the half-precision floating
| point value 'b' converted to integral value (rounded toward -infinity, i.e.
| floor). If the result cannot be represented in half precision, then the
| proper overflow response (for positive scaling operand), or the proper
| underflow response (for negative scaling operand) is issued. The operation
| is performed according to the IEC/IEEE Standard for Binary Floating-Point
| Arithmetic.
*----------------------------------------------------------------------------*/

float16 f16_scalef(float16 a, float16 b, struct softfloat_status_t *status)
{
    bool signA;
    int16_t expA;
    uint16_t sigA;
    bool signB;
    int16_t expB;
    uint16_t sigB;
    int shiftCount;
    int scale = 0;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    signA = signF16UI(a);
    expA  = expF16UI(a);
    sigA  = fracF16UI(a);
    signB = signF16UI(b);
    expB  = expF16UI(b);
    sigB  = fracF16UI(b);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expB == 0x1F) {
        if (sigB) return softfloat_propagateNaNF16UI(a, b, status);
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (softfloat_denormalsAreZeros(status)) {
        if (!expA) sigA = 0;
        if (!expB) sigB = 0;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expA == 0x1F) {
        if (sigA) {
            int aIsSignalingNaN = (sigA & 0x0200) == 0;
            if (aIsSignalingNaN || expB != 0x1F || sigB)
                return softfloat_propagateNaNF16UI(a, b, status);

            return signB ? 0 : packToF16UI(0, 0x1F, 0);
        }

        if (expB == 0x1F && signB) {
            softfloat_raiseFlags(status, softfloat_flag_invalid);
            return defaultNaNF16UI;
        }

        return a;
    }
    /*------------------------------------------------------------------------
    | Architectural rule for the denormal-operand exception on this
    | instruction (confirmed against the ISA reference): it is checked for
    | src1 (A) only - never independently for src2 (B) - and only when B is
    | not NaN (already excluded above, which returns before this point). If
    | A is a genuine denormal, DE is signaled regardless of B's value,
    | *including* when B is +-Infinity or exactly zero. A == 0 is not
    | itself denormal, so it never raises DE (it has its own special-case
    | handling, combined with B == +-Infinity, immediately below).
    *------------------------------------------------------------------------*/
    if (! expA && ! sigA) {
        if (expB == 0x1F && ! signB) {
            softfloat_raiseFlags(status, softfloat_flag_invalid);
            return defaultNaNF16UI;
        }
        return packToF16UI(signA, 0, 0);
    }

    if (! expA) softfloat_raiseFlags(status, softfloat_flag_denormal);

    if (expB == 0x1F) {
        if (signB) return packToF16UI(signA, 0, 0);
        return packToF16UI(signA, 0x1F, 0);
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ((expB | sigB) == 0)
        return packToF16UI(signA, expA, sigA); // honor DAZ

    if (expB >= 0x1A) {
        // handle obvious overflow/underflow result. Include A's implicit
        // leading-1 bit (when A is normal) before handing sigA to the
        // extreme right-shift-and-jam inside roundPackToF16 - otherwise an
        // exact-power-of-two A (sigA == 0, e.g. a=0x0400) looks like "zero
        // significand, nothing lost" and the genuine underflow+inexact
        // (the true infinite-precision result is nonzero, just far too
        // small to represent) silently fails to get flagged.
        uint16_t sigForBailout = sigA | (expA ? 0x0400 : 0);
        return softfloat_roundPackToF16(signA, signB ? -0xF : 0x1F, sigForBailout, status);
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expB <= 0xE) {
        scale = -int(signB);
    }
    else {
        sigB |= 0x0400;
        shiftCount = 0x19 - expB;
        uint16_t prev_sigB = sigB;
        sigB = (uint16_t) (sigB >> shiftCount);
        scale = (int) sigB;

        if (signB) {
            if ((uint16_t) (sigB << shiftCount) != prev_sigB) scale++;
            scale = -scale;
        }

        if (scale >  0x40) scale =  0x40;
        if (scale < -0x40) scale = -0x40;
    }

    if (expA != 0) {
        sigA |= 0x0400;
    } else {
        expA++;
    }

    expA += scale - 1;
    sigA <<= 4;
    return softfloat_normRoundPackToF16(signA, expA, sigA, status);
}
