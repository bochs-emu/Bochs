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

extFloat80_t extF80_mul(extFloat80_t a, extFloat80_t b, struct softfloat_status_t *status)
{
    uint16_t uiA64;
    uint64_t uiA0;
    bool signA;
    int32_t expA;
    uint64_t sigA;
    uint16_t uiB64;
    uint64_t uiB0;
    bool signB;
    int32_t expB;
    uint64_t sigB;
    bool signZ;
    uint64_t magBits;
    struct exp32_sig64 normExpSig;
    int32_t expZ;
    struct uint128 sig128Z, uiZ;
    uint16_t uiZ64;
    uint64_t uiZ0;
    extFloat80_t z;

    // handle unsupported extended double-precision floating encodings
    if (extF80_isUnsupported(a) || extF80_isUnsupported(b)) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        uiZ64 = defaultNaNExtF80UI64;
        uiZ0  = defaultNaNExtF80UI0;
        goto uiZ;
    }

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiA64 = a.signExp;
    uiA0  = a.signif;
    signA = signExtF80UI64(uiA64);
    expA  = expExtF80UI64(uiA64);
    sigA  = uiA0;
    uiB64 = b.signExp;
    uiB0  = b.signif;
    signB = signExtF80UI64(uiB64);
    expB  = expExtF80UI64(uiB64);
    sigB  = uiB0;
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expA == 0x7FFF) {
        if ((sigA & UINT64_C(0x7FFFFFFFFFFFFFFF)) || ((expB == 0x7FFF) && (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)))) {
            goto propagateNaN;
        }
        magBits = expB | sigB;
        goto infArg;
    }
    if (expB == 0x7FFF) {
        if (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)) goto propagateNaN;
        magBits = expA | sigA;
        goto infArg;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (! expA) expA = 1;
    if (! (sigA & UINT64_C(0x8000000000000000))) {
        if (! sigA) goto zero;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalExtF80Sig(sigA);
        expA += normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if (! expB) expB = 1;
    if (! (sigB & UINT64_C(0x8000000000000000))) {
        if (! sigB) goto zero;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalExtF80Sig(sigB);
        expB += normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expZ = expA + expB - 0x3FFE;
    sig128Z = softfloat_mul64To128(sigA, sigB);
    if (sig128Z.v64 < UINT64_C(0x8000000000000000)) {
        --expZ;
        sig128Z = softfloat_add128(sig128Z.v64, sig128Z.v0, sig128Z.v64, sig128Z.v0);
    }
    return
        softfloat_roundPackToExtF80(signZ, expZ, sig128Z.v64, sig128Z.v0, softfloat_extF80_roundingPrecision(status), status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    uiZ = softfloat_propagateNaNExtF80UI(uiA64, uiA0, uiB64, uiB0, status);
    uiZ64 = uiZ.v64;
    uiZ0  = uiZ.v0;
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infArg:
    if (! magBits) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        uiZ64 = defaultNaNExtF80UI64;
        uiZ0  = defaultNaNExtF80UI0;
    } else {
        uiZ64 = packToExtF80UI64(signZ, 0x7FFF);
        uiZ0  = UINT64_C(0x8000000000000000);
    }
    goto uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ64 = packToExtF80UI64(signZ, 0);
    uiZ0  = 0;
 uiZ:
    z.signExp = uiZ64;
    z.signif  = uiZ0;
    return z;
}
