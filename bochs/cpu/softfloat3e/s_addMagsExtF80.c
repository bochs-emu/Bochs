/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014 The Regents of the University of California.
All rights reserved.

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
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

extFloat80_t softfloat_addMagsExtF80(uint16_t uiA64, uint64_t uiA0, uint16_t uiB64, uint64_t uiB0, bool signZ, struct softfloat_status_t *status)
{
    int32_t expA;
    uint64_t sigA;
    int32_t expB;
    uint64_t sigB;
    int32_t expDiff;
    uint16_t uiZ64;
    uint64_t uiZ0, sigZ, sigZExtra;
    struct exp32_sig64 normExpSig;
    int32_t expZ;
    struct uint64_extra sig64Extra;
    struct uint128 uiZ;
    extFloat80_t z;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expA = expExtF80UI64(uiA64);
    sigA = uiA0;
    expB = expExtF80UI64(uiB64);
    sigB = uiB0;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expA - expB;
    if (! expDiff) {
        if (expA == 0x7FFF) {
            if ((sigA | sigB) & UINT64_C(0x7FFFFFFFFFFFFFFF)) {
                goto propagateNaN;
            }
            uiZ64 = uiA64;
            uiZ0  = uiA0;
            goto uiZ;
        }
        sigZ = sigA + sigB;
        sigZExtra = 0;
        if (! expA) {
            if (sigA | sigB) softfloat_raiseFlags(status, softfloat_flag_denormal);
            normExpSig = softfloat_normSubnormalExtF80Sig(sigZ);
            expZ = normExpSig.exp + 1;
            sigZ = normExpSig.sig;
            goto roundAndPack;
        }
        expZ = expA;
        goto shiftRight1;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expDiff < 0) {
        if (expB == 0x7FFF) {
            if (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)) goto propagateNaN;
            uiZ64 = packToExtF80UI64(signZ, 0x7FFF);
            uiZ0  = uiB0;
            goto uiZ;
        }
        expZ = expB;
        if (! expA) {
            if (sigA) softfloat_raiseFlags(status, softfloat_flag_denormal);
            ++expDiff;
            sigZExtra = 0;
            if (! expDiff) goto newlyAligned;
        }
        sig64Extra = softfloat_shiftRightJam64Extra(sigA, 0, -expDiff);
        sigA = sig64Extra.v;
        sigZExtra = sig64Extra.extra;
    } else {
        if (expA == 0x7FFF) {
            if (sigA & UINT64_C(0x7FFFFFFFFFFFFFFF)) goto propagateNaN;
            uiZ64 = uiA64;
            uiZ0  = uiA0;
            goto uiZ;
        }
        expZ = expA;
        if (! expB) {
            if (sigB) softfloat_raiseFlags(status, softfloat_flag_denormal);
            --expDiff;
            sigZExtra = 0;
            if (! expDiff) goto newlyAligned;
        }
        sig64Extra = softfloat_shiftRightJam64Extra(sigB, 0, expDiff);
        sigB = sig64Extra.v;
        sigZExtra = sig64Extra.extra;
    }
 newlyAligned:
    sigZ = sigA + sigB;
    if (sigZ & UINT64_C(0x8000000000000000)) goto roundAndPack;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 shiftRight1:
    sig64Extra = softfloat_shortShiftRightJam64Extra(sigZ, sigZExtra, 1);
    sigZ = sig64Extra.v | UINT64_C(0x8000000000000000);
    sigZExtra = sig64Extra.extra;
    ++expZ;
 roundAndPack:
    return softfloat_roundPackToExtF80(signZ, expZ, sigZ, sigZExtra, softfloat_extF80_roundingPrecision(status), status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNExtF80UI(uiA64, uiA0, uiB64, uiB0, status);
    uiZ64 = uiZ.v64;
    uiZ0  = uiZ.v0;
 uiZ:
    z.signExp = uiZ64;
    z.signif  = uiZ0;
    return z;
}
