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
#include "internals.h"
#include "primitives.h"
#include "primitiveTypes.h"
#include "specialize.h"
#include "softfloat.h"

/*----------------------------------------------------------------------------
| Multiply the quadruple-precision value 'a' by the extended double-precision
| value 'b' and return a quadruple-precision product.
|
| This is 'f128_mul' with its right operand delivered in the 80-bit format: 'b'
| is decoded like an extF80 (explicit integer bit, 64-bit significand, possible
| pseudo-denormal) and its 63-bit fraction is dropped into the same position
| 'f128_mul' would use, so the exact product is formed and rounded once, by
| 'softfloat_roundPackToF128', exactly as for a quad * quad multiply.  The
| 'roundingMode' and every exception flag behave as in 'f128_mul' (a denormal
| operand additionally raises the denormal flag, as the elementary-function
| callers require).
*----------------------------------------------------------------------------*/

float128_t f128_mul_by_extF80(float128_t a, extFloat80_t b, uint8_t roundingMode, uint8_t roundingPrecision, struct softfloat_status_t *status)
{
    uint64_t uiA64, uiA0;
    bool signA;
    int32_t expA;
    struct uint128 sigA;
    uint16_t uiB64;
    uint64_t uiB0;
    bool signB;
    int32_t expB;
    uint64_t sigB;
    bool signZ;
    uint64_t magBits;
    struct exp32_sig128 normExpSigA;
    struct exp32_sig64 normExpSigB;
    int32_t expZ;
    uint64_t sig256Z[4];
    uint64_t sigZExtra;
    struct uint128 sigZ;
    struct uint128_extra sig128Extra;
    struct uint128 uiZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiA64 = a.v64;
    uiA0  = a.v0;
    signA = signF128UI64(uiA64);
    expA  = expF128UI64(uiA64);
    sigA.v64 = fracF128UI64(uiA64);
    sigA.v0  = uiA0;
    uiB64 = b.signExp;
    uiB0  = b.signif;
    signB = signExtF80UI64(uiB64);
    expB  = expExtF80UI64(uiB64);
    sigB  = uiB0;
    signZ = signA ^ signB;
    /*------------------------------------------------------------------------
    | Reject the unsupported extended double-precision encodings of 'b'.
    *------------------------------------------------------------------------*/
    if (extF80_isUnsupported(b)) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        uiZ.v64 = defaultNaNF128UI64;
        uiZ.v0  = defaultNaNF128UI0;
        return uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expA == 0x7FFF) {
        if ((sigA.v64 | sigA.v0) || ((expB == 0x7FFF) && (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)))) {
            goto propagateNaN;
        }
        magBits = expB | sigB;
        goto infArg;
    }
    if (expB == 0x7FFF) {
        if (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)) goto propagateNaN;
        magBits = expA | sigA.v64 | sigA.v0;
        goto infArg;
    }
    /*------------------------------------------------------------------------
    | Decode 'a' (quadruple precision).
    *------------------------------------------------------------------------*/
    if (! expA) {
        if (! (sigA.v64 | sigA.v0)) goto zero;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSigA = softfloat_normSubnormalF128Sig(sigA.v64, sigA.v0);
        expA = normExpSigA.exp;
        sigA = normExpSigA.sig;
    }
    /*------------------------------------------------------------------------
    | Decode 'b' (extended double precision): pseudo-denormals become normal
    | at exponent 1, true denormals are normalized.
    *------------------------------------------------------------------------*/
    if (! expB) {
        if (! sigB) goto zero;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        expB = 1;
    }
    if (! (sigB & UINT64_C(0x8000000000000000))) {
        normExpSigB = softfloat_normSubnormalExtF80Sig(sigB);
        expB += normExpSigB.exp;
        sigB = normExpSigB.sig;
    }
    /*------------------------------------------------------------------------
    | Form the exact product.  sigA is a 113-bit significand (unit bit at bit
    | 112); 'b's 63-bit fraction is placed exactly where f128_mul would place a
    | 128-bit operand's fraction after its 16-bit left shift, and the implicit
    | integer bit of 'b' is added back as the '+ sigA' term.
    *------------------------------------------------------------------------*/
    expZ = expA + expB - 0x4000;
    sigA.v64 |= UINT64_C(0x0001000000000000);
    softfloat_mul128To256M(sigA.v64, sigA.v0, (sigB & UINT64_C(0x7FFFFFFFFFFFFFFF)) << 1, 0, sig256Z);
    sigZExtra = sig256Z[indexWord(4, 1)] | (sig256Z[indexWord(4, 0)] != 0);
    sigZ = softfloat_add128(sig256Z[indexWord(4, 3)], sig256Z[indexWord(4, 2)], sigA.v64, sigA.v0);
    if (UINT64_C(0x0002000000000000) <= sigZ.v64) {
        ++expZ;
        sig128Extra = softfloat_shortShiftRightJam128Extra(sigZ.v64, sigZ.v0, sigZExtra, 1);
        sigZ = sig128Extra.v;
        sigZExtra = sig128Extra.extra;
    }
    return
        softfloat_roundPackToF128(signZ, expZ, sigZ.v64, sigZ.v0, sigZExtra, roundingMode, roundingPrecision, status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    {
        float128_t bf = extF80_to_f128(b, status);
        uiZ = softfloat_propagateNaNF128UI(uiA64, uiA0, bf.v64, bf.v0, status);
    }
    return uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infArg:
    if (! magBits) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        uiZ.v64 = defaultNaNF128UI64;
        uiZ.v0  = defaultNaNF128UI0;
        return uiZ;
    }
    if ((! expA && (sigA.v64 | sigA.v0)) || (! expB && sigB))
        softfloat_raiseFlags(status, softfloat_flag_denormal);
    uiZ.v64 = packToF128UI64(signZ, 0x7FFF, 0);
    uiZ.v0  = 0;
    return uiZ;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zero:
    uiZ.v64 = packToF128UI64(signZ, 0, 0);
    uiZ.v0  = 0;
    return uiZ;
}
