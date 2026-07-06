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

static BX_CPP_INLINE float64 f64_minimum(float64 a, float64 b)
{
    int signA = signF64UI(a);
    int signB = signF64UI(b);

    if (signA != signB)
        return (signA) ? a : b;

    return (signA ^ (a < b)) ? a : b;
}

static BX_CPP_INLINE float64 f64_maximum(float64 a, float64 b)
{
    int signA = signF64UI(a);
    int signB = signF64UI(b);

    if (signA != signB)
        return (signA) ? b : a;

    return (signA ^ (a < b)) ? b : a;
}

static BX_CPP_INLINE float64 f64_minimum_magnitude(float64 a, float64 b)
{
    float64 tmp_a = a & ~UINT64_C(0x8000000000000000); // clear the sign bit
    float64 tmp_b = b & ~UINT64_C(0x8000000000000000);

    if (tmp_a != tmp_b)
        return (tmp_a < tmp_b) ? a : b;

    return (signF64UI(a)) ? a : b;
}

static BX_CPP_INLINE float64 f64_maximum_magnitude(float64 a, float64 b)
{
    float64 tmp_a = a & ~UINT64_C(0x8000000000000000); // clear the sign bit
    float64 tmp_b = b & ~UINT64_C(0x8000000000000000);

    if (tmp_a != tmp_b)
        return (tmp_a < tmp_b) ? b : a;

    return (signF64UI(a)) ? b : a;
}

float64 f64_minmax(float64 a, float64 b, int op_select, int sign_ctrl, bool propagate_NaNs, struct softfloat_status_t *status)
{
    if (softfloat_denormalsAreZeros(status)) {
        a = f64_denormal_to_zero(a);
        b = f64_denormal_to_zero(b);
    }

    bool aIsNaN = f64_isNaN(a);
    bool bIsNaN = f64_isNaN(b);
    float64 z;

    if (aIsNaN || bIsNaN) {
        bool aIsSignalingNaN = f64_isSignalingNaN(a);
        bool bIsSignalingNaN = f64_isSignalingNaN(b);

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
                z = a | UINT64_C(0x0008000000000000);
            else
                z = b | UINT64_C(0x0008000000000000);
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
                    z = a | UINT64_C(0x0008000000000000);
                else
                    z = b | UINT64_C(0x0008000000000000);
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
        if (! f64_exp(a) && f64_fraction(a))
            softfloat_raiseFlags(status, softfloat_flag_denormal);
        if (! f64_exp(b) && f64_fraction(b))
            softfloat_raiseFlags(status, softfloat_flag_denormal);

        switch(op_select) {
            case 0: z = f64_minimum(a, b); break;
            case 1: z = f64_maximum(a, b); break;
            case 2: z = f64_minimum_magnitude(a, b); break;
            case 3: z = f64_maximum_magnitude(a, b); break;
            default: assert(0);
        }
    }

    if (! f64_isNaN(z)) {
        switch(sign_ctrl) {
        case 0:
            z = (z & ~UINT64_C(0x8000000000000000)) | (a & UINT64_C(0x8000000000000000)); // keep sign of a
            break;
        case 1:
            break; // preserve sign of compare result
        case 2:
            z = z & ~UINT64_C(0x8000000000000000); // zero out the sign bit
            break;
        case 3:
            z = z | UINT64_C(0x8000000000000000);  // set the sign bit
            break;
        }
    }

    return z;
}
