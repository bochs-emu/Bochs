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

/*-------------------------------------------------------------------------------
| Takes a 32-bit fixed-point value `sig' with binary point between bits 7 and 8
| and returns the properly rounded 8-bit unsigned  integer corresponding to the
| input. Ordinarily, the fixed-point input is simply rounded to an integer, with
| the inexact exception raised if the input cannot be represented exactly as an
| integer. However, if the fixed-point input is too large, the invalid exception
| is raised and the largest unsigned integer is returned.
*-------------------------------------------------------------------------------*/

uint8_t
 softfloat_roundToUI8(bool sign, uint32_t sig, uint8_t roundingMode, bool exact, bool saturate, struct softfloat_status_t *status)
{
    uint32_t roundIncrement, roundBits;
    uint8_t z;
    uint32_t origSig = sig >> 8;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    roundIncrement = 0x80;
    if ((roundingMode != softfloat_round_near_maxMag) && (roundingMode != softfloat_round_near_even)) {
        roundIncrement = 0;
        if (sign) {
            if (!sig) return 0;
            if (roundingMode == softfloat_round_min) goto invalid;
        } else {
            if (roundingMode == softfloat_round_max) roundIncrement = 0xFF;
        }
    }
    roundBits = sig & 0xFF;
    sig += roundIncrement;
    if (sig & 0xFFFF0000) goto invalid;
    z = sig>>8;
    if ((roundBits == 0x80) && (roundingMode == softfloat_round_near_even)) {
        z &= ~0x1;
    }
    if (sign && z) goto invalid;
    if (roundBits) {
        if (exact) softfloat_raiseFlags(status, softfloat_flag_inexact);
        if (z > origSig)
            softfloat_setRoundingUp(status);
    }
    return z;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    const uint8_t NegOverflowResponse = saturate ? ui8_minValue : ui8_fromNegOverflow;
    const uint8_t PosOverflowResponse = saturate ? ui8_maxValue : ui8_fromPosOverflow;

    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return sign ? NegOverflowResponse : PosOverflowResponse;
}
