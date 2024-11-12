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
| Takes a 64-bit fixed-point value `sig' with binary point between bits 11
| and 12, and returns the properly rounded 32-bit integer corresponding to the
| input.  If `sign' is 1, the input is negated before being converted to an
| integer.  Bit 63 of `sig' must be zero.  Ordinarily, the fixed-point input
| is simply rounded to an integer, with the inexact exception raised if the
| input cannot be represented exactly as an integer.  However, if the fixed-
| point input is too large, the invalid exception is raised and the integer
| indefinite value is returned.
*----------------------------------------------------------------------------*/

int32_t softfloat_roundToI32(bool sign, uint64_t sig, uint8_t roundingMode, bool exact, struct softfloat_status_t *status)
{
    uint32_t roundIncrement, roundBits;
    uint32_t sig32;
    union { uint32_t ui; int32_t i; } uZ;
    int32_t z;
    uint64_t origSig = sig >> 12;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    roundIncrement = 0x800;
    if ((roundingMode != softfloat_round_near_maxMag) && (roundingMode != softfloat_round_near_even)) {
        roundIncrement = 0;
        if (sign ? (roundingMode == softfloat_round_min) : (roundingMode == softfloat_round_max)) {
            roundIncrement = 0xFFF;
        }
    }
    roundBits = sig & 0xFFF;
    sig += roundIncrement;
    if (sig & UINT64_C(0xFFFFF00000000000)) goto invalid;
    sig32 = sig>>12;
    if ((roundBits == 0x800) && (roundingMode == softfloat_round_near_even)) {
        sig32 &= ~(uint32_t) 1;
    }
    uZ.ui = sign ? -sig32 : sig32;
    z = uZ.i;
    if (z && ((z < 0) ^ sign)) goto invalid;
    if (roundBits) {
        if (exact) softfloat_raiseFlags(status, softfloat_flag_inexact);
        if (sig32 > (uint32_t)origSig)
            softfloat_setRoundingUp(status);
    }
    return z;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 invalid:
    softfloat_raiseFlags(status, softfloat_flag_invalid);
    return sign ? i32_fromNegOverflow : i32_fromPosOverflow;
}
