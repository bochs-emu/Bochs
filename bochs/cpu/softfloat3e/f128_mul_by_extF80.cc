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
| Multiply the quadruple-precision (128-bit) floating-point value 'a' by the
| extended double-precision (80-bit) floating-point value 'b' and return the
| product rounded to a full 64-bit extended double-precision significand.
|
| The two significands are combined without any preliminary loss of precision
| (float128 carries a 113-bit significand, extF80 a 64-bit one, and the exact
| 177-bit product is formed before a single rounding), so no double rounding
| occurs.  Every floating-point exception is produced: invalid, denormal
| (raised whenever either source operand is a denormal - or, for the extF80
| operand, a pseudo-denormal), overflow, underflow and inexact.
|
| The result always carries a 64-bit significand: this is the historical P5
| "e64" internal format, and the extF80 rounding-precision control (24/53/64
| bit) does not apply to these intermediate elementary-function products.
|
| 'roundingMode' selects the rounding direction applied when the exact product
| is packed, overriding the mode currently in 'status'.  This is what
| distinguishes the historical P5 elementary-function primitives 'mul_e64'
| (round per the current mode) from 'mul_e64_ne' (force round-to-nearest-even
| for the intermediate step).  All floating-point exception flags are still
| reported through 'status'.
|
| Modeled on 'f128_mul' (quad * quad) and 'extF80_mul' (the extF80 operand
| decoding and result packing).
*----------------------------------------------------------------------------*/

extFloat80_t f128_mul_by_extF80(float128_t a, extFloat80_t b, uint8_t roundingMode, struct softfloat_status_t *status)
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
    struct exp32_sig128 normExpSig128;
    struct exp32_sig64 normExpSig64;
    int32_t expZ;
    uint64_t sig256Z[4];
    uint64_t sig2, sig1, sig0;
    struct uint128 sigZ;
    uint64_t sigZExtra;
    struct commonNaN commonNaN;
    struct uint128 uiAExtF80;
    uint16_t uiZ64;
    uint64_t uiZ0;

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
    | Handle unsupported extended double-precision floating encodings.
    *------------------------------------------------------------------------*/
    if (extF80_isUnsupported(b)) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        return packToExtF80(defaultNaNExtF80UI64, defaultNaNExtF80UI0);
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
    | Decode / normalize operand 'a' (quadruple precision).
    *------------------------------------------------------------------------*/
    if (! expA) {
        if (! (sigA.v64 | sigA.v0)) {
            if (! expB && sigB)
                softfloat_raiseFlags(status, softfloat_flag_denormal);
            return packToExtF80(signZ, 0, 0);
        }
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig128 = softfloat_normSubnormalF128Sig(sigA.v64, sigA.v0);
        expA = normExpSig128.exp;
        sigA = normExpSig128.sig;
    }
    /*------------------------------------------------------------------------
    | Decode / normalize operand 'b' (extended double precision).
    *------------------------------------------------------------------------*/
    if (! expB) {
        expB = 1;
        if (sigB)
            softfloat_raiseFlags(status, softfloat_flag_denormal);
    }
    if (! (sigB & UINT64_C(0x8000000000000000))) {
        if (! sigB) return packToExtF80(signZ, 0, 0);
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig64 = softfloat_normSubnormalExtF80Sig(sigB);
        expB += normExpSig64.exp;
        sigB = normExpSig64.sig;
    }
    /*------------------------------------------------------------------------
    | Both significands are now normalized:
    |   sigA : 113-bit integer, unit bit at bit 112 (bit 48 of sigA.v64)
    |   sigB :  64-bit integer, unit bit at bit 63
    | so the exact product occupies at most 177 bits, with its unit bit at
    | bit 175 (mantissa in [1,2)) or bit 176 (mantissa in [2,4)).
    *------------------------------------------------------------------------*/
    expZ = expA + expB - 0x3FFF;
    sigA.v64 |= UINT64_C(0x0001000000000000);
    softfloat_mul128To256M(sigA.v64, sigA.v0, 0, sigB, sig256Z);
    sig2 = sig256Z[indexWord(4, 2)];
    sig1 = sig256Z[indexWord(4, 1)];
    sig0 = sig256Z[indexWord(4, 0)];
    /*------------------------------------------------------------------------
    | Extract a 64-bit significand (unit bit at bit 63) plus a 64-bit 'extra'
    | word for rounding, jamming everything below into the extra word.
    *------------------------------------------------------------------------*/
    if (sig2 & UINT64_C(0x0001000000000000)) {
        /* mantissa in [2,4): unit bit at bit 176, shift right by 49 */
        ++expZ;
        sigZ.v64  = (sig2 << 15) | (sig1 >> 49);
        sigZ.v0   = (sig1 << 15) | (sig0 >> 49);
        sigZExtra = sig0 & UINT64_C(0x0001FFFFFFFFFFFF);
    } else {
        /* mantissa in [1,2): unit bit at bit 175, shift right by 48 */
        sigZ.v64  = (sig2 << 16) | (sig1 >> 48);
        sigZ.v0   = (sig1 << 16) | (sig0 >> 48);
        sigZExtra = sig0 & UINT64_C(0x0000FFFFFFFFFFFF);
    }
    /*------------------------------------------------------------------------
    | 'softfloat_roundPackToExtF80' consumes only 'sigZ.v64' and a single
    | 64-bit extra word, so fold both the discarded low product bits and the
    | second half of the 128-bit significand into that extra word (as a sticky
    | bit when they cannot be represented exactly).
    *------------------------------------------------------------------------*/
    sigZExtra = sigZ.v0 | (sigZExtra != 0);
    return
        softfloat_roundPackToExtF80(signZ, expZ, sigZ.v64, sigZExtra, 80, roundingMode, status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN:
    if (isNaNExtF80UI(uiB64, uiB0)) {
        softfloat_extF80UIToCommonNaN(uiB64, uiB0, &commonNaN, status);
        uiAExtF80 = softfloat_commonNaNToExtF80UI(&commonNaN);
    } else {
        uiAExtF80.v64 = 0;
        uiAExtF80.v0  = 0;
    }
    return softfloat_propagateNaNExtF80UI((uint16_t) uiAExtF80.v64, uiAExtF80.v0, uiB64, uiB0, status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infArg:
    if (! magBits) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        uiZ64 = defaultNaNExtF80UI64;
        uiZ0  = defaultNaNExtF80UI0;
    } else {
        if ((! expA && (sigA.v64 | sigA.v0)) || (! expB && sigB))
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        uiZ64 = packToExtF80UI64(signZ, 0x7FFF);
        uiZ0  = UINT64_C(0x8000000000000000);
    }
    return packToExtF80(uiZ64, uiZ0);
}
