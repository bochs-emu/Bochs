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
#include <assert.h>
#include "internals.h"
#include "softfloat.h"

static BX_CPP_INLINE float32 f32_minimum(float32 a, float32 b)
{
    int signA = signF32UI(a);
    int signB = signF32UI(b);

    if (signA != signB)
        return (signA) ? a : b;

    return (signA ^ (a < b)) ? a : b;
}

static BX_CPP_INLINE float32 f32_maximum(float32 a, float32 b)
{
    int signA = signF32UI(a);
    int signB = signF32UI(b);

    if (signA != signB)
        return (signA) ? b : a;

    return (signA ^ (a < b)) ? b : a;
}

static BX_CPP_INLINE float32 f32_minimum_magnitude(float32 a, float32 b)
{
    float32 tmp_a = a & ~0x80000000; // clear the sign bit
    float32 tmp_b = b & ~0x80000000;

    if (tmp_a != tmp_b)
        return (tmp_a < tmp_b) ? a : b;

    return (signF32UI(a)) ? a : b;
}

static BX_CPP_INLINE float32 f32_maximum_magnitude(float32 a, float32 b)
{
    float32 tmp_a = a & ~0x80000000; // clear the sign bit
    float32 tmp_b = b & ~0x80000000;

    if (tmp_a != tmp_b)
        return (tmp_a < tmp_b) ? b : a;

    return (signF32UI(a)) ? b : a;
}

float32 f32_minmax(float32 a, float32 b, int op_select, int sign_ctrl, bool propagate_NaNs, struct softfloat_status_t *status)
{
    if (softfloat_denormalsAreZeros(status)) {
        a = f32_denormal_to_zero(a);
        b = f32_denormal_to_zero(b);
    }

    bool aIsNaN = f32_isNaN(a);
    bool bIsNaN = f32_isNaN(b);
    float32 z;

    if (aIsNaN || bIsNaN) {
        bool aIsSignalingNaN = f32_isSignalingNaN(a);
        bool bIsSignalingNaN = f32_isSignalingNaN(b);

        if (aIsSignalingNaN || bIsSignalingNaN)
            softfloat_raiseFlags(status, softfloat_flag_invalid);

        if (propagate_NaNs) {
            // regular NaN propagation: first operand NaN

            //        | b=SNaN   | b=QNaN  | b
            // -------------------------------------
            // a=SNaN | QNaN(a)  | QNaN(a) | QNaN(a)
            // a=QNaN | QNaN(a)  | QNaN(a) | QNaN(a)
            // a      | QNaN(b)  | QNaN(b) | -------

            // minmax NaN propagation rule: differs when b is SNaN and a is QNaN

            //        | b=SNaN   | b=QNaN  | b
            // -------------------------------------
            // a=SNaN | QNaN(a)  | QNaN(a) | QNaN(a)
            // a=QNaN | QNaN(b)* | QNaN(a) | QNaN(a)
            // a      | QNaN(b)  | QNaN(b) | -------

            if (aIsSignalingNaN || (aIsNaN && !bIsSignalingNaN))
                z = a | 0x00400000;
            else
                z = b | 0x00400000;
        }
        else {
            // minmax_number NaN handling rule:

            //        | b=SNaN  | b=QNaN  | b
            // -------------------------------------
            // a=SNaN | QNaN(a) | QNaN(a) | b
            // a=QNaN | QNaN(b) | QNaN(a) | b
            // a      | a       | a       | -

            if (aIsNaN && bIsNaN) {
                if (aIsSignalingNaN || !bIsSignalingNaN)
                    z = a | 0x00400000;
                else
                    z = b | 0x00400000;
            }
            else if (aIsNaN) {
                z = b; // take not NaN
            }
            else {
                z = a; // take not NaN
            }
        }
    }
    else {
        // when one of the arguments is NaN denormal exception is not raised
        if (! f32_exp(a) && f32_fraction(a))
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        if (! f32_exp(b) && f32_fraction(b))
            softfloat_raiseFlags(status, softfloat_flag_denormal);

        switch(op_select) {
            case 0: z = f32_minimum(a, b); break;
            case 1: z = f32_maximum(a, b); break;
            case 2: z = f32_minimum_magnitude(a, b); break;
            case 3: z = f32_maximum_magnitude(a, b); break;
            default: assert(0);
        }
    }

    if (! f32_isNaN(z)) {
        switch(sign_ctrl) {
        case 0:
            z = (z & ~0x80000000) | (a & 0x80000000); // keep sign of a
            break;
        case 1:
            break; // preserve sign of compare result
        case 2:
            z = z & ~0x80000000; // zero out the sign bit
            break;
        case 3:
            z = z | 0x80000000;  // set the sign bit
            break;
        }
    }

    return z;
}
