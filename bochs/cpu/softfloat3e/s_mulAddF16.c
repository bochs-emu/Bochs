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
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

float16_t softfloat_mulAddF16(uint16_t uiA, uint16_t uiB, uint16_t uiC, uint8_t op, struct softfloat_status_t *status)
{
    bool signA;
    int8_t expA;
    uint16_t sigA;
    bool signB;
    int8_t expB;
    uint16_t sigB;
    bool signC;
    int8_t expC;
    uint16_t sigC;
    bool signProd;
    uint16_t magBits, uiZ;
    struct exp8_sig16 normExpSig;
    int8_t expProd;
    uint32_t sigProd;
    bool signZ;
    int8_t expZ;
    uint16_t sigZ;
    int8_t expDiff;
    uint32_t sig32Z, sig32C;
    int8_t shiftDist;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    signA = signF16UI(uiA);
    expA  = expF16UI(uiA);
    sigA  = fracF16UI(uiA);
    signB = signF16UI(uiB);
    expB  = expF16UI(uiB);
    sigB  = fracF16UI(uiB);
    signC = signF16UI(uiC) ^ (op == softfloat_mulAdd_subC);
    expC  = expF16UI(uiC);
    sigC  = fracF16UI(uiC);
    signProd = signA ^ signB ^ (op == softfloat_mulAdd_subProd);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (softfloat_denormalsAreZeros(status)) {
        if (!expA) sigA = 0;
        if (!expB) sigB = 0;
        if (!expC) sigC = 0;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (expA == 0x1F) {
        if (sigA || ((expB == 0x1F) && sigB)) goto propagateNaN_ABC;
        magBits = expB | sigB;
        goto infProdArg;
    }
    if (expB == 0x1F) {
        if (sigB) goto propagateNaN_ABC;
        magBits = expA | sigA;
        goto infProdArg;
    }
    if (expC == 0x1F) {
        if (sigC) {
            uiZ = 0;
            goto propagateNaN_ZC;
        }
        uiZ = uiC;
        return uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (! expA) {
        if (! sigA) goto zeroProd;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalF16Sig(sigA);
        expA = normExpSig.exp;
        sigA = normExpSig.sig;
    }
    if (! expB) {
        if (! sigB) goto zeroProd;
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalF16Sig(sigB);
        expB = normExpSig.exp;
        sigB = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expProd = expA + expB - 0xE;
    sigA = (sigA | 0x0400)<<4;
    sigB = (sigB | 0x0400)<<4;
    sigProd = (uint32_t) sigA * sigB;
    if (sigProd < 0x20000000) {
        --expProd;
        sigProd <<= 1;
    }
    signZ = signProd;
    if (! expC) {
        if (! sigC) {
            expZ = expProd - 1;
            sigZ = sigProd>>15 | ((sigProd & 0x7FFF) != 0);
            goto roundPack;
        }
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalF16Sig(sigC);
        expC = normExpSig.exp;
        sigC = normExpSig.sig;
    }
    sigC = (sigC | 0x0400)<<3;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    expDiff = expProd - expC;
    if (signProd == signC) {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        if (expDiff <= 0) {
            expZ = expC;
            sigZ = sigC + softfloat_shiftRightJam32(sigProd, 16 - expDiff);
        } else {
            expZ = expProd;
            sig32Z = sigProd + softfloat_shiftRightJam32((uint32_t) sigC<<16, expDiff);
            sigZ = sig32Z>>16 | ((sig32Z & 0xFFFF) != 0);
        }
        if (sigZ < 0x4000) {
            --expZ;
            sigZ <<= 1;
        }
    } else {
        /*--------------------------------------------------------------------
        *--------------------------------------------------------------------*/
        sig32C = (uint32_t) sigC<<16;
        if (expDiff < 0) {
            signZ = signC;
            expZ = expC;
            sig32Z = sig32C - softfloat_shiftRightJam32(sigProd, -expDiff);
        } else if (! expDiff) {
            expZ = expProd;
            sig32Z = sigProd - sig32C;
            if (! sig32Z) goto completeCancellation;
            if (sig32Z & 0x80000000) {
                signZ = ! signZ;
                sig32Z = -sig32Z;
            }
        } else {
            expZ = expProd;
            sig32Z = sigProd - softfloat_shiftRightJam32(sig32C, expDiff);
        }
        shiftDist = softfloat_countLeadingZeros32(sig32Z) - 1;
        expZ -= shiftDist;
        shiftDist -= 16;
        if (shiftDist < 0) {
            sigZ = sig32Z>>(-shiftDist) | ((uint32_t) (sig32Z<<(shiftDist & 31)) != 0);
        } else {
            sigZ = (uint16_t) sig32Z<<shiftDist;
        }
    }
 roundPack:
    return softfloat_roundPackToF16(signZ, expZ, sigZ, status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 propagateNaN_ABC:
    uiZ = softfloat_propagateNaNF16UI(uiA, uiB, status);
    goto propagateNaN_ZC;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 infProdArg:
    if (magBits) {
        uiZ = packToF16UI(signProd, 0x1F, 0);
        if (sigC && expC == 0x1F) goto propagateNaN_ZC;
        if ((sigA && !expA) || (sigB && !expB) || (sigC && !expC))
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        if (expC != 0x1F) return uiZ;
        if (signProd == signC) return uiZ;
    }
    softfloat_raiseFlags(status, softfloat_flag_invalid);
    uiZ = defaultNaNF16UI;
 propagateNaN_ZC:
    return softfloat_propagateNaNF16UI(uiZ, uiC, status);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 zeroProd:
    uiZ = packToF16UI(signC, expC, sigC);
    if (expC && !sigC) {
        /* Exact zero plus a denormal */
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        if (softfloat_flushUnderflowToZero(status)) {
            softfloat_raiseFlags(status, softfloat_flag_underflow | softfloat_flag_inexact);
            return packToF16UI(signC, 0, 0);
        }
    }
    if (! (expC | sigC) && (signProd != signC)) {
 completeCancellation:
        uiZ = packToF16UI((softfloat_getRoundingMode(status) == softfloat_round_min), 0, 0);
    }
    return uiZ;
}
