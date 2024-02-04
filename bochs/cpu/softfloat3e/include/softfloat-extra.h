/*============================================================================

This C header file is part of the SoftFloat IEEE Floating-Point Arithmetic
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

#ifndef softfloat_extra_h
#define softfloat_extra_h 1

#include "platform.h"
#include "internals.h"

// ======= float16 ======= //

BX_CPP_INLINE bool f16_sign(float16_t a)
{
    return signF16UI(a);
}

BX_CPP_INLINE int8_t f16_exp(float16_t a)
{
    return expF16UI(a);
}

BX_CPP_INLINE uint16_t f16_fraction(float16_t a)
{
    return fracF16UI(a);
}

BX_CPP_INLINE float16_t f16_denormal_to_zero(float16_t a)
{
    if (! expF16UI(a) && fracF16UI(a))
        return a & 0x8000;
    return a;
}

// ======= float32 ======= //

BX_CPP_INLINE bool f32_sign(float32_t a)
{
    return signF32UI(a);
}

BX_CPP_INLINE int16_t f32_exp(float32_t a)
{
    return expF32UI(a);
}

BX_CPP_INLINE uint32_t f32_fraction(float32_t a)
{
    return fracF32UI(a);
}

BX_CPP_INLINE float32_t f32_denormal_to_zero(float32_t a)
{
    if (! expF32UI(a) && fracF32UI(a))
        return a & 0x80000000;
    return a;
}

// ======= float64 ======= //

BX_CPP_INLINE bool f64_sign(float64_t a)
{
    return signF64UI(a);
}

BX_CPP_INLINE int16_t f64_exp(float64_t a)
{
    return expF64UI(a);
}

BX_CPP_INLINE uint64_t f64_fraction(float64_t a)
{
    return fracF64UI(a);
}

BX_CPP_INLINE float64_t f64_denormal_to_zero(float64_t a)
{
    if (! expF64UI(a) && fracF64UI(a))
        return a & ((uint64_t)(1) << 63);
    return a;
}

// ======= floatx80 ======= //

BX_CPP_INLINE bool extF80_isUnsupported(extFloat80_t a)
{
    return ((a.signExp & 0x7FFF) && !(a.signif & BX_CONST64(0x8000000000000000)));
}

#endif
