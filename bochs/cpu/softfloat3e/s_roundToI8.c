/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

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
| Takes a 32-bit fixed-point value `sig' with binary point between bits 7
| and 8, and returns the properly rounded 8-bit integer corresponding to the
| input.  If `sign' is 1, the input is negated before being converted to an
| integer.  Bit 31 of `sig' must be zero.  Ordinarily, the fixed-point input
| is simply rounded to an integer, with the inexact exception raised if the
| input cannot be represented exactly as an integer.  However, if the fixed-
| point input is too large, the invalid exception is raised and the integer
| indefinite value is returned.
*----------------------------------------------------------------------------*/

int8_t softfloat_roundToI8(bool sign, uint32_t sig, uint8_t roundingMode, bool exact, bool saturate, struct softfloat_status_t *status)
{
    uint32_t roundIncrement, roundBits;
    union { uint8_t ui; int8_t i; } uZ;
    int8_t z;
    uint32_t origSig = sig >> 8;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    roundIncrement = 0x80;
    if ((roundingMode != softfloat_round_near_maxMag) && (roundingMode != softfloat_round_near_even)) {
        roundIncrement = 0;
        if (sign ? (roundingMode == softfloat_round_min) : (roundingMode == softfloat_round_max)) {
            roundIncrement = 0xFF;
        }
    }
    roundBits = sig & 0xFF;
    sig += roundIncrement;
    if (sig & 0xFFFF0000) goto invalid;
    sig = sig>>8;
    if ((roundBits == 0x80) && (roundingMode == softfloat_round_near_even)) {
        sig &= ~0x1;
    }
    uZ.ui = sign ? -sig : sig;
    z = uZ.i;
    if (z && ((z < 0) ^ sign)) goto invalid;
    if (roundBits) {
        if (exact) softfloat_raiseFlags(status, softfloat_flag_inexact);
        if (sig > origSig)
            softfloat_setRoundingUp(status);
    }
    return z;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    const int8_t NegOverflowResponse = saturate ? i8_minNegativeValue : i8_fromNegOverflow;
    const int8_t PosOverflowResponse = saturate ? i8_maxPositiveValue : i8_fromPosOverflow;

    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return sign ? NegOverflowResponse : PosOverflowResponse;
}
