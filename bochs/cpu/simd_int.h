/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2026 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_SIMD_INT_FUNCTIONS_H
#define BX_SIMD_INT_FUNCTIONS_H

#include "config.h"

/////////////////////////////////////////////////////////////////////////
// Optional host-SIMD-intrinsic acceleration, controlled by
// BX_SUPPORT_HOST_SIMD_INTRINSICS (config.h). This is *not* about which
// instruction sets the emulated guest CPU supports (that's
// BX_SUPPORT_AVX / BX_SUPPORT_EVEX / ... elsewhere in config.h) - it's
// purely a build-host code-generation choice: emulate a packed-integer
// helper with one host SIMD instruction instead of a 2..16-iteration
// per-element C loop, when both (a) the switch is on and (b) the compiler
// is told the host target actually has that instruction set.
//
// Every function below still has its exact original per-element C loop as
// the fallback body, selected whenever either condition isn't met - so
// this file behaves identically to before if BX_SUPPORT_HOST_SIMD_INTRINSICS
// is 0, and safely degrades per-function on a host that lacks a given tier
// even with it on.
//
// ---- capability detection (compile-time) --------------------------------
// GCC/Clang predefine one macro per -m<ext> (or -march=<cpu>) you compile
// with; if the macro isn't set, the corresponding intrinsics simply aren't
// declared and calling one anyway is a compile error - never a silent
// wrong-instruction miscompile. MSVC does not define these macros (except
// __AVX__/__AVX2__ on newer versions with /arch:AVX or /arch:AVX2) and, by
// design, lets you call *any* intrinsic regardless of /arch - it is your
// job to make sure the code path you enable matches both your /arch and
// your actual deployment target. Because of that we only auto-enable SSE2
// on MSVC (the tier that's unconditionally true: guaranteed on every x64
// build, and on x86 it's the compiler's own default /arch since VS2012);
// anything higher on MSVC needs those macros defined explicitly
// by the build (e.g. -D__SSSE3__ / -D__SSE4_1__) once /arch and the actual
// deployment target have been checked to agree.
/////////////////////////////////////////////////////////////////////////

#if defined(__SSE2__) \
 || defined(_M_X64) || defined(_M_AMD64) \
 || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
  #define BX_HOST_SUPPORT_SSE2 1
#endif

// Note: no BX_HOST_SUPPORT_SSE1 tier - andps/andnps/orps/xorps (the only
// candidates) are plain C unconditionally, see "arithmetic (logic)" below.

#if defined(__SSSE3__)
  #define BX_HOST_SUPPORT_SSSE3 1
#endif

#if defined(__SSE4_1__)
  #define BX_HOST_SUPPORT_SSE4_1 1
#endif

#if defined(__AVX2__)
  #define BX_HOST_SUPPORT_AVX2 1
#endif

#if defined(__AVX512F__) && defined(__AVX512VL__)
  #define BX_HOST_SUPPORT_AVX512VL 1
#endif

#if defined(BX_HOST_SUPPORT_AVX512VL) && defined(__AVX512BW__)
  #define BX_HOST_SUPPORT_AVX512BW_VL 1
#endif

#if defined(BX_HOST_SUPPORT_AVX512VL) && defined(__AVX512DQ__)
  #define BX_HOST_SUPPORT_AVX512DQ_VL 1
#endif

#if BX_SUPPORT_HOST_SIMD_INTRINSICS != 0

#if defined(BX_HOST_SUPPORT_SSE2)
#include <emmintrin.h>   // SSE2  (_mm_..._si128 / _mm_..._epiNN)
#endif
#if defined(BX_HOST_SUPPORT_SSSE3)
#include <tmmintrin.h>   // SSSE3
#endif
#if defined(BX_HOST_SUPPORT_SSE4_1)
#include <smmintrin.h>   // SSE4.1
#endif
#if (defined(BX_HOST_SUPPORT_AVX2) || defined(BX_HOST_SUPPORT_AVX512VL))
#include <immintrin.h>   // AVX2 / AVX-512
#endif

#if defined(BX_HOST_SUPPORT_SSE2)

// BxPackedXmmRegister is a plain 16-byte union with byte 0 at the lowest
// address on every non-BX_BIG_ENDIAN build (see cpu/xmm.h) - i.e. exactly
// __m128i's own memory layout on any host where these intrinsics apply at
// all, and (now that bx_xmm_reg_t carries alignas(16)) 16-byte aligned on
// every compiler, so the aligned load/store forms are safe to use and let
// the compiler fold a load directly into an ALU instruction's memory
// operand where profitable (the unaligned forms block that fold).
BX_CPP_INLINE __m128i simd_load128(const BxPackedXmmRegister *r)
{
  return _mm_load_si128((const __m128i*) r);
}

BX_CPP_INLINE void simd_store128(BxPackedXmmRegister *r, __m128i v)
{
  _mm_store_si128((__m128i*) r, v);
}

// PSRAW/PSRAD/PSRLx/PSLLx (register-count form) take the shift count as the
// low 64 bits of an __m128i and already saturate exactly the way the
// per-element C loops below do explicitly (count > element-width => all
// sign bits for arithmetic shifts, all zero for logical shifts) - so the
// count just needs to be placed there, no separate clamping required.
BX_CPP_INLINE __m128i simd_shift_count(Bit64u shift_64)
{
  return _mm_set_epi32(0, 0, (int)(shift_64 >> 32), (int)(shift_64 & 0xffffffff));
}

#if defined(BX_HOST_SUPPORT_AVX512VL)
// broadcast a scalar rotate count into a per-lane vector, so a fixed-count
// rotate can be implemented on top of the *variable*-count AVX-512VL
// rotate intrinsics (there is no immediate-count __m128i rotate below
// AVX-512, and even there the immediate form needs a compile-time
// constant which a runtime shift_ctrl value can't satisfy anyway).
BX_CPP_INLINE __m128i simd_rotate_count32(Bit64u shift_ctrl, unsigned mask)
{
  return _mm_set1_epi32((int)(shift_ctrl & mask));
}
BX_CPP_INLINE __m128i simd_rotate_count64(Bit64u shift_ctrl, Bit64u mask)
{
  return _mm_set1_epi64x((Bit64s)(shift_ctrl & mask));
}
#endif

#endif // BX_HOST_SUPPORT_SSE2

#endif // BX_SUPPORT_HOST_SIMD_INTRINSICS != 0

// absolute value

BX_CPP_INLINE void xmm_pabsb(BxPackedXmmRegister *op)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_abs_epi8(simd_load128(op)));
#else
  for(unsigned n=0; n<16; n++) {
    if(op->xmmsbyte(n) < 0) op->xmmubyte(n) = -op->xmmsbyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pabsw(BxPackedXmmRegister *op)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_abs_epi16(simd_load128(op)));
#else
  for(unsigned n=0; n<8; n++) {
    if(op->xmm16s(n) < 0) op->xmm16u(n) = -op->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pabsd(BxPackedXmmRegister *op)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_abs_epi32(simd_load128(op)));
#else
  for(unsigned n=0; n<4; n++) {
    if(op->xmm32s(n) < 0) op->xmm32u(n) = -op->xmm32s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pabsq(BxPackedXmmRegister *op)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_abs_epi64(simd_load128(op)));
#else
  for(unsigned n=0; n<2; n++) {
    if(op->xmm64s(n) < 0) op->xmm64u(n) = -op->xmm64s(n);
  }
#endif
}

// min/max

BX_CPP_INLINE void xmm_pminsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmsbyte(n) < op1->xmmsbyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epu8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmubyte(n) < op1->xmmubyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16s(n) < op1->xmm16s(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16u(n) < op1->xmm16u(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminsd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32s(n) < op1->xmm32s(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epu32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32u(n) < op1->xmm32u(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminsq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    if(op2->xmm64s(n) < op1->xmm64s(n)) op1->xmm64u(n) = op2->xmm64u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pminuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_min_epu64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    if(op2->xmm64u(n) < op1->xmm64u(n)) op1->xmm64u(n) = op2->xmm64u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmsbyte(n) > op1->xmmsbyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxub(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epu8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    if(op2->xmmubyte(n) > op1->xmmubyte(n)) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16s(n) > op1->xmm16s(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    if(op2->xmm16u(n) > op1->xmm16u(n)) op1->xmm16s(n) = op2->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxsd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32s(n) > op1->xmm32s(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxud(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epu32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    if(op2->xmm32u(n) > op1->xmm32u(n)) op1->xmm32u(n) = op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxsq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    if(op2->xmm64s(n) > op1->xmm64s(n)) op1->xmm64u(n) = op2->xmm64u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaxuq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_max_epu64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    if(op2->xmm64u(n) > op1->xmm64u(n)) op1->xmm64u(n) = op2->xmm64u(n);
  }
#endif
}

// unpack

// unpcklps/unpckhps/unpcklpd/unpckhpd are pure lane copies (1..4 scalar
// assignments, no arithmetic at all) - going through a full 128-bit
// load/shuffle/store round trip costs more than the plain assignments the
// compiler already emits optimally, so these deliberately stay plain C
// regardless of BX_SUPPORT_HOST_SIMD_INTRINSICS.

BX_CPP_INLINE void xmm_unpcklps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(3) = op2->xmm32u(1);
  op1->xmm32u(2) = op1->xmm32u(1);
  op1->xmm32u(1) = op2->xmm32u(0);
//op1->xmm32u(0) = op1->xmm32u(0);
}

BX_CPP_INLINE void xmm_unpckhps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm32u(0) = op1->xmm32u(2);
  op1->xmm32u(1) = op2->xmm32u(2);
  op1->xmm32u(2) = op1->xmm32u(3);
  op1->xmm32u(3) = op2->xmm32u(3);
}

BX_CPP_INLINE void xmm_unpcklpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
//op1->xmm64u(0) = op1->xmm64u(0);
  op1->xmm64u(1) = op2->xmm64u(0);
}

BX_CPP_INLINE void xmm_unpckhpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  op1->xmm64u(0) = op1->xmm64u(1);
  op1->xmm64u(1) = op2->xmm64u(1);
}

BX_CPP_INLINE void xmm_punpcklbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_unpacklo_epi8(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmmubyte(0xF) = op2->xmmubyte(7);
  op1->xmmubyte(0xE) = op1->xmmubyte(7);
  op1->xmmubyte(0xD) = op2->xmmubyte(6);
  op1->xmmubyte(0xC) = op1->xmmubyte(6);
  op1->xmmubyte(0xB) = op2->xmmubyte(5);
  op1->xmmubyte(0xA) = op1->xmmubyte(5);
  op1->xmmubyte(0x9) = op2->xmmubyte(4);
  op1->xmmubyte(0x8) = op1->xmmubyte(4);
  op1->xmmubyte(0x7) = op2->xmmubyte(3);
  op1->xmmubyte(0x6) = op1->xmmubyte(3);
  op1->xmmubyte(0x5) = op2->xmmubyte(2);
  op1->xmmubyte(0x4) = op1->xmmubyte(2);
  op1->xmmubyte(0x3) = op2->xmmubyte(1);
  op1->xmmubyte(0x2) = op1->xmmubyte(1);
  op1->xmmubyte(0x1) = op2->xmmubyte(0);
//op1->xmmubyte(0x0) = op1->xmmubyte(0);
#endif
}

BX_CPP_INLINE void xmm_punpckhbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_unpackhi_epi8(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmmubyte(0x0) = op1->xmmubyte(0x8);
  op1->xmmubyte(0x1) = op2->xmmubyte(0x8);
  op1->xmmubyte(0x2) = op1->xmmubyte(0x9);
  op1->xmmubyte(0x3) = op2->xmmubyte(0x9);
  op1->xmmubyte(0x4) = op1->xmmubyte(0xA);
  op1->xmmubyte(0x5) = op2->xmmubyte(0xA);
  op1->xmmubyte(0x6) = op1->xmmubyte(0xB);
  op1->xmmubyte(0x7) = op2->xmmubyte(0xB);
  op1->xmmubyte(0x8) = op1->xmmubyte(0xC);
  op1->xmmubyte(0x9) = op2->xmmubyte(0xC);
  op1->xmmubyte(0xA) = op1->xmmubyte(0xD);
  op1->xmmubyte(0xB) = op2->xmmubyte(0xD);
  op1->xmmubyte(0xC) = op1->xmmubyte(0xE);
  op1->xmmubyte(0xD) = op2->xmmubyte(0xE);
  op1->xmmubyte(0xE) = op1->xmmubyte(0xF);
  op1->xmmubyte(0xF) = op2->xmmubyte(0xF);
#endif
}

BX_CPP_INLINE void xmm_punpcklwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_unpacklo_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16u(7) = op2->xmm16u(3);
  op1->xmm16u(6) = op1->xmm16u(3);
  op1->xmm16u(5) = op2->xmm16u(2);
  op1->xmm16u(4) = op1->xmm16u(2);
  op1->xmm16u(3) = op2->xmm16u(1);
  op1->xmm16u(2) = op1->xmm16u(1);
  op1->xmm16u(1) = op2->xmm16u(0);
//op1->xmm16u(0) = op1->xmm16u(0);
#endif
}

BX_CPP_INLINE void xmm_punpckhwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_unpackhi_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16u(0) = op1->xmm16u(4);
  op1->xmm16u(1) = op2->xmm16u(4);
  op1->xmm16u(2) = op1->xmm16u(5);
  op1->xmm16u(3) = op2->xmm16u(5);
  op1->xmm16u(4) = op1->xmm16u(6);
  op1->xmm16u(5) = op2->xmm16u(6);
  op1->xmm16u(6) = op1->xmm16u(7);
  op1->xmm16u(7) = op2->xmm16u(7);
#endif
}

// pack

BX_CPP_INLINE void xmm_packuswb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_packus_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmmubyte(0x0) = SaturateWordSToByteU(op1->xmm16s(0));
  op1->xmmubyte(0x1) = SaturateWordSToByteU(op1->xmm16s(1));
  op1->xmmubyte(0x2) = SaturateWordSToByteU(op1->xmm16s(2));
  op1->xmmubyte(0x3) = SaturateWordSToByteU(op1->xmm16s(3));
  op1->xmmubyte(0x4) = SaturateWordSToByteU(op1->xmm16s(4));
  op1->xmmubyte(0x5) = SaturateWordSToByteU(op1->xmm16s(5));
  op1->xmmubyte(0x6) = SaturateWordSToByteU(op1->xmm16s(6));
  op1->xmmubyte(0x7) = SaturateWordSToByteU(op1->xmm16s(7));

  op1->xmmubyte(0x8) = SaturateWordSToByteU(op2->xmm16s(0));
  op1->xmmubyte(0x9) = SaturateWordSToByteU(op2->xmm16s(1));
  op1->xmmubyte(0xA) = SaturateWordSToByteU(op2->xmm16s(2));
  op1->xmmubyte(0xB) = SaturateWordSToByteU(op2->xmm16s(3));
  op1->xmmubyte(0xC) = SaturateWordSToByteU(op2->xmm16s(4));
  op1->xmmubyte(0xD) = SaturateWordSToByteU(op2->xmm16s(5));
  op1->xmmubyte(0xE) = SaturateWordSToByteU(op2->xmm16s(6));
  op1->xmmubyte(0xF) = SaturateWordSToByteU(op2->xmm16s(7));
#endif
}

BX_CPP_INLINE void xmm_packsswb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_packs_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmmsbyte(0x0) = SaturateWordSToByteS(op1->xmm16s(0));
  op1->xmmsbyte(0x1) = SaturateWordSToByteS(op1->xmm16s(1));
  op1->xmmsbyte(0x2) = SaturateWordSToByteS(op1->xmm16s(2));
  op1->xmmsbyte(0x3) = SaturateWordSToByteS(op1->xmm16s(3));
  op1->xmmsbyte(0x4) = SaturateWordSToByteS(op1->xmm16s(4));
  op1->xmmsbyte(0x5) = SaturateWordSToByteS(op1->xmm16s(5));
  op1->xmmsbyte(0x6) = SaturateWordSToByteS(op1->xmm16s(6));
  op1->xmmsbyte(0x7) = SaturateWordSToByteS(op1->xmm16s(7));

  op1->xmmsbyte(0x8) = SaturateWordSToByteS(op2->xmm16s(0));
  op1->xmmsbyte(0x9) = SaturateWordSToByteS(op2->xmm16s(1));
  op1->xmmsbyte(0xA) = SaturateWordSToByteS(op2->xmm16s(2));
  op1->xmmsbyte(0xB) = SaturateWordSToByteS(op2->xmm16s(3));
  op1->xmmsbyte(0xC) = SaturateWordSToByteS(op2->xmm16s(4));
  op1->xmmsbyte(0xD) = SaturateWordSToByteS(op2->xmm16s(5));
  op1->xmmsbyte(0xE) = SaturateWordSToByteS(op2->xmm16s(6));
  op1->xmmsbyte(0xF) = SaturateWordSToByteS(op2->xmm16s(7));
#endif
}

BX_CPP_INLINE void xmm_packusdw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_packus_epi32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16u(0) = SaturateDwordSToWordU(op1->xmm32s(0));
  op1->xmm16u(1) = SaturateDwordSToWordU(op1->xmm32s(1));
  op1->xmm16u(2) = SaturateDwordSToWordU(op1->xmm32s(2));
  op1->xmm16u(3) = SaturateDwordSToWordU(op1->xmm32s(3));

  op1->xmm16u(4) = SaturateDwordSToWordU(op2->xmm32s(0));
  op1->xmm16u(5) = SaturateDwordSToWordU(op2->xmm32s(1));
  op1->xmm16u(6) = SaturateDwordSToWordU(op2->xmm32s(2));
  op1->xmm16u(7) = SaturateDwordSToWordU(op2->xmm32s(3));
#endif
}

BX_CPP_INLINE void xmm_packssdw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_packs_epi32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16s(0) = SaturateDwordSToWordS(op1->xmm32s(0));
  op1->xmm16s(1) = SaturateDwordSToWordS(op1->xmm32s(1));
  op1->xmm16s(2) = SaturateDwordSToWordS(op1->xmm32s(2));
  op1->xmm16s(3) = SaturateDwordSToWordS(op1->xmm32s(3));

  op1->xmm16s(4) = SaturateDwordSToWordS(op2->xmm32s(0));
  op1->xmm16s(5) = SaturateDwordSToWordS(op2->xmm32s(1));
  op1->xmm16s(6) = SaturateDwordSToWordS(op2->xmm32s(2));
  op1->xmm16s(7) = SaturateDwordSToWordS(op2->xmm32s(3));
#endif
}

// shuffle
//
// pshufhw/pshuflw/shufps/shufpd/permilps/permilpd/permil2ps/permil2pd all
// take their shuffle pattern as a *runtime* value (the guest instruction's
// immediate, already decoded to a plain variable by the time it reaches
// here) while the matching host intrinsics (_mm_shuffle_epi32 & co)
// require a compile-time-constant control operand - so they can't be
// swapped in as a single call without a 256-way dispatch table. pshufb is
// the exception: SSSE3's _mm_shuffle_epi8 takes its per-byte index table
// as a runtime __m128i, which is exactly what it already is here.

BX_CPP_INLINE void xmm_pshufb(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(r, _mm_shuffle_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++)
  {
    unsigned mask = op2->xmmubyte(n);
    if (mask & 0x80)
      r->xmmubyte(n) = 0;
    else
      r->xmmubyte(n) = op1->xmmubyte(mask & 0xf);
  }
#endif
}

BX_CPP_INLINE void xmm_pshufhw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op, Bit8u order)
{
  r->xmm64u(0) = op->xmm64u(0);
  r->xmm16u(4) = op->xmm16u(4 + ((order >> 0) & 0x3));
  r->xmm16u(5) = op->xmm16u(4 + ((order >> 2) & 0x3));
  r->xmm16u(6) = op->xmm16u(4 + ((order >> 4) & 0x3));
  r->xmm16u(7) = op->xmm16u(4 + ((order >> 6) & 0x3));
}

BX_CPP_INLINE void xmm_pshuflw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op, Bit8u order)
{
  r->xmm16u(0) = op->xmm16u((order >> 0) & 0x3);
  r->xmm16u(1) = op->xmm16u((order >> 2) & 0x3);
  r->xmm16u(2) = op->xmm16u((order >> 4) & 0x3);
  r->xmm16u(3) = op->xmm16u((order >> 6) & 0x3);
  r->xmm64u(1) = op->xmm64u(1);
}

BX_CPP_INLINE void xmm_shufps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u order)
{
  r->xmm32u(0) = op1->xmm32u((order >> 0) & 0x3);
  r->xmm32u(1) = op1->xmm32u((order >> 2) & 0x3);
  r->xmm32u(2) = op2->xmm32u((order >> 4) & 0x3);
  r->xmm32u(3) = op2->xmm32u((order >> 6) & 0x3);
}

BX_CPP_INLINE void xmm_shufpd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u order)
{
  r->xmm64u(0) = op1->xmm64u((order >> 0) & 0x1);
  r->xmm64u(1) = op2->xmm64u((order >> 1) & 0x1);
}

BX_CPP_INLINE void xmm_permilps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  r->xmm32u(0) = op1->xmm32u(op2->xmm32u(0) & 0x3);
  r->xmm32u(1) = op1->xmm32u(op2->xmm32u(1) & 0x3);
  r->xmm32u(2) = op1->xmm32u(op2->xmm32u(2) & 0x3);
  r->xmm32u(3) = op1->xmm32u(op2->xmm32u(3) & 0x3);
}

BX_CPP_INLINE void xmm_permilpd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  r->xmm64u(0) = op1->xmm64u((op2->xmm32u(0) >> 1) & 0x1);
  r->xmm64u(1) = op1->xmm64u((op2->xmm32u(2) >> 1) & 0x1);
}

BX_CPP_INLINE void xmm_permil2ps(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, unsigned m2z)
{
  for(unsigned n=0; n < 4; n++) {
    Bit32u ctrl = op3->xmm32u(n);
    if ((m2z ^ ((ctrl >> 3) & 0x1)) == 0x3)
      r->xmm32u(n) = 0;
    else
      r->xmm32u(n) = (ctrl & 0x4) ? op1->xmm32u(ctrl & 0x3) : op2->xmm32u(ctrl & 0x3);
  }
}

BX_CPP_INLINE void xmm_permil2pd(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3, unsigned m2z)
{
  for(unsigned n=0; n < 2; n++) {
    Bit32u ctrl = op3->xmm32u(n*2);
    if ((m2z ^ ((ctrl >> 3) & 0x1)) == 0x3)
      r->xmm64u(n) = 0;
    else
      r->xmm64u(n) = (ctrl & 0x4) ? op1->xmm64u((ctrl >> 1) & 0x1) : op2->xmm64u((ctrl >> 1) & 0x1);
  }
}

#if BX_SUPPORT_AVX
BX_CPP_INLINE void ymm_vpermq(BxPackedYmmRegister *r, const BxPackedYmmRegister *op, Bit8u control)
{
  r->ymm64u(0) = op->ymm64u((control)      & 0x3);
  r->ymm64u(1) = op->ymm64u((control >> 2) & 0x3);
  r->ymm64u(2) = op->ymm64u((control >> 4) & 0x3);
  r->ymm64u(3) = op->ymm64u((control >> 6) & 0x3);
}
#endif

// sign

BX_CPP_INLINE void xmm_psignb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sign_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    int sign = (op2->xmmsbyte(n) > 0) - (op2->xmmsbyte(n) < 0);
    op1->xmmsbyte(n) *= sign;
  }
#endif
}

BX_CPP_INLINE void xmm_psignw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sign_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    int sign = (op2->xmm16s(n) > 0) - (op2->xmm16s(n) < 0);
    op1->xmm16s(n) *= sign;
  }
#endif
}

BX_CPP_INLINE void xmm_psignd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sign_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    int sign = (op2->xmm32s(n) > 0) - (op2->xmm32s(n) < 0);
    op1->xmm32s(n) *= sign;
  }
#endif
}

// mask creation

BX_CPP_INLINE Bit32u xmm_pmovmskb(const BxPackedXmmRegister *op)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  return (Bit32u) _mm_movemask_epi8(simd_load128(op));
#else
  Bit32u mask = 0;

  if(op->xmmsbyte(0x0) < 0) mask |= 0x0001;
  if(op->xmmsbyte(0x1) < 0) mask |= 0x0002;
  if(op->xmmsbyte(0x2) < 0) mask |= 0x0004;
  if(op->xmmsbyte(0x3) < 0) mask |= 0x0008;
  if(op->xmmsbyte(0x4) < 0) mask |= 0x0010;
  if(op->xmmsbyte(0x5) < 0) mask |= 0x0020;
  if(op->xmmsbyte(0x6) < 0) mask |= 0x0040;
  if(op->xmmsbyte(0x7) < 0) mask |= 0x0080;
  if(op->xmmsbyte(0x8) < 0) mask |= 0x0100;
  if(op->xmmsbyte(0x9) < 0) mask |= 0x0200;
  if(op->xmmsbyte(0xA) < 0) mask |= 0x0400;
  if(op->xmmsbyte(0xB) < 0) mask |= 0x0800;
  if(op->xmmsbyte(0xC) < 0) mask |= 0x1000;
  if(op->xmmsbyte(0xD) < 0) mask |= 0x2000;
  if(op->xmmsbyte(0xE) < 0) mask |= 0x4000;
  if(op->xmmsbyte(0xF) < 0) mask |= 0x8000;

  return mask;
#endif
}

BX_CPP_INLINE Bit32u xmm_pmovmskw(const BxPackedXmmRegister *op)
{
  Bit32u mask = 0;

  if(op->xmm16s(0) < 0) mask |= 0x01;
  if(op->xmm16s(1) < 0) mask |= 0x02;
  if(op->xmm16s(2) < 0) mask |= 0x04;
  if(op->xmm16s(3) < 0) mask |= 0x08;
  if(op->xmm16s(4) < 0) mask |= 0x10;
  if(op->xmm16s(5) < 0) mask |= 0x20;
  if(op->xmm16s(6) < 0) mask |= 0x40;
  if(op->xmm16s(7) < 0) mask |= 0x80;

  return mask;
}

BX_CPP_INLINE Bit32u xmm_pmovmskd(const BxPackedXmmRegister *op)
{
  Bit32u mask = 0;

  if(op->xmm32s(0) < 0) mask |= 0x1;
  if(op->xmm32s(1) < 0) mask |= 0x2;
  if(op->xmm32s(2) < 0) mask |= 0x4;
  if(op->xmm32s(3) < 0) mask |= 0x8;

  return mask;
}

BX_CPP_INLINE Bit32u xmm_pmovmskq(const BxPackedXmmRegister *op)
{
  Bit32u mask = 0;

  if(op->xmm32s(1) < 0) mask |= 0x1;
  if(op->xmm32s(3) < 0) mask |= 0x2;

  return mask;
}

BX_CPP_INLINE void xmm_pmovm2b(BxPackedXmmRegister *dst, Bit32u mask)
{
  for (unsigned n=0; n < 16; n++, mask >>= 1) {
    dst->xmmsbyte(n) = - Bit8s(mask & 0x1);
  }
}

BX_CPP_INLINE void xmm_pmovm2w(BxPackedXmmRegister *dst, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    dst->xmm16s(n) = - Bit16s(mask & 0x1);
  }
}

BX_CPP_INLINE void xmm_pmovm2d(BxPackedXmmRegister *dst, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    dst->xmm32s(n) = - Bit32s(mask & 0x1);
  }
}

BX_CPP_INLINE void xmm_pmovm2q(BxPackedXmmRegister *dst, Bit32u mask)
{
  dst->xmm64s(0) = (mask & 0x1) ? (Bit64s) -1 : 0;
  dst->xmm64s(1) = (mask & 0x2) ? (Bit64s) -1 : 0;
}

// blend
//
// The xxx_pblendX / xxx_blendXX families select lanes by a runtime bitmask
// (one predicate bit per lane, tested in a loop) rather than a compile-time
// shuffle-style immediate, so - unlike the shuffle family above - they do
// have a clean host mapping: AVX-512BW/F+VL's mask-register blend
// intrinsics (_mm_mask_blend_epi8/16/32/64, _mm_mask_blend_ps/pd) take the
// mask as a plain runtime integer. Left as portable C for now (no SSE/SSE2
// hardware instruction matches this "blend by bitmask" shape below
// AVX-512 - PBLENDW's own immediate is a compile-time shuffle control, not
// a runtime predicate, so it doesn't apply here either); a future
// AVX-512VL tier can add that fast path without touching anything else.
// xmm_pblendvb / blendvps / blendvpd (mask is a full XMM register, tested
// by each lane's own sign bit) map directly to SSE4.1's _mm_blendv_epi8 /
// _mm_blendv_ps / _mm_blendv_pd. xmm_pblendvw has no matching hardware
// instruction at all (there is no word-granularity PBLENDVW) but *is*
// implementable on SSE2 alone via a sign-broadcast trick - see below.

BX_CPP_INLINE void xmm_pblendb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 16; n++, mask >>= 1) {
    if (mask & 0x1) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
}

BX_CPP_INLINE void xmm_zero_pblendb(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 16; n++, mask >>= 1) {
    dst->xmmubyte(n) = (mask & 0x1) ? op->xmmubyte(n) : 0;
  }
}

#if BX_SUPPORT_EVEX
BX_CPP_INLINE void simd_pblendb(BxPackedAvxRegister *op1, const BxPackedAvxRegister *op2, Bit64u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    if (mask & 0x1) op1->vmmubyte(n) = op2->vmmubyte(n);
    mask >>= 1;
  }
}

BX_CPP_INLINE void simd_zero_pblendb(BxPackedAvxRegister *dst, const BxPackedAvxRegister *op, Bit64u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    dst->vmmubyte(n) = (mask & 0x1) ? op->vmmubyte(n) : 0;
    mask >>= 1;
  }
}
#endif

BX_CPP_INLINE void xmm_pblendw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    if (mask & 0x1) op1->xmm16u(n) = op2->xmm16u(n);
  }
}

BX_CPP_INLINE void xmm_zero_pblendw(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 8; n++, mask >>= 1) {
    dst->xmm16u(n) = (mask & 0x1) ? op->xmm16u(n) : 0;
  }
}

#if BX_SUPPORT_EVEX
BX_CPP_INLINE void simd_pblendw(BxPackedAvxRegister *op1, const BxPackedAvxRegister *op2, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    if (mask & 0x1) op1->vmm16u(n) = op2->vmm16u(n);
    mask >>= 1;
  }
}

BX_CPP_INLINE void simd_zero_pblendw(BxPackedAvxRegister *dst, const BxPackedAvxRegister *op, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    dst->vmm16u(n) = (mask & 0x1) ? op->vmm16u(n) : 0;
    mask >>= 1;
  }
}
#endif

BX_CPP_INLINE void xmm_blendps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    if (mask & 0x1) op1->xmm32u(n) = op2->xmm32u(n);
  }
}

BX_CPP_INLINE void xmm_zero_blendps(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 4; n++, mask >>= 1) {
    dst->xmm32u(n) = (mask & 0x1) ? op->xmm32u(n) : 0;
  }
}

#if BX_SUPPORT_EVEX
BX_CPP_INLINE void simd_blendps(BxPackedAvxRegister *op1, const BxPackedAvxRegister *op2, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    if (mask & 0x1) op1->vmm32u(n) = op2->vmm32u(n);
    mask >>= 1;
  }
}

BX_CPP_INLINE void simd_zero_blendps(BxPackedAvxRegister *dst, const BxPackedAvxRegister *op, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    dst->vmm32u(n) = (mask & 0x1) ? op->vmm32u(n) : 0;
    mask >>= 1;
  }
}
#endif

BX_CPP_INLINE void xmm_blendpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    if (mask & 0x1) op1->xmm64u(n) = op2->xmm64u(n);
  }
}

BX_CPP_INLINE void xmm_zero_blendpd(BxPackedXmmRegister *dst, const BxPackedXmmRegister *op, Bit32u mask)
{
  for (unsigned n=0; n < 2; n++, mask >>= 1) {
    dst->xmm64u(n) = (mask & 0x1) ? op->xmm64u(n) : 0;
  }
}

#if BX_SUPPORT_EVEX
BX_CPP_INLINE void simd_blendpd(BxPackedAvxRegister *op1, const BxPackedAvxRegister *op2, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    if (mask & 0x1) op1->vmm64u(n) = op2->vmm64u(n);
    mask >>= 1;
  }
}

BX_CPP_INLINE void simd_zero_blendpd(BxPackedAvxRegister *dst, const BxPackedAvxRegister *op, Bit32u mask, unsigned len)
{
  for (unsigned n=0; n < len; n++) {
    dst->vmm64u(n) = (mask & 0x1) ? op->vmm64u(n) : 0;
    mask >>= 1;
  }
}
#endif

BX_CPP_INLINE void xmm_pblendvb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_blendv_epi8(simd_load128(op1), simd_load128(op2), simd_load128(mask)));
#else
  for(unsigned n=0; n<16; n++) {
    if (mask->xmmsbyte(n) < 0) op1->xmmubyte(n) = op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pblendvw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  // no such hardware instruction as PBLENDVW - build a per-word all-ones/
  // all-zeros selector by arithmetically replicating each word's own sign
  // bit across the whole word, then do a plain bitwise blend with it.
  __m128i sel = _mm_srai_epi16(simd_load128(mask), 15);
  __m128i a = simd_load128(op1);
  __m128i b = simd_load128(op2);
  simd_store128(op1, _mm_or_si128(_mm_and_si128(sel, b), _mm_andnot_si128(sel, a)));
#else
  for(unsigned n=0; n<8; n++) {
    if (mask->xmm16s(n) < 0) op1->xmm16u(n) = op2->xmm16u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_blendvps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  __m128 a = _mm_castsi128_ps(simd_load128(op1));
  __m128 b = _mm_castsi128_ps(simd_load128(op2));
  __m128 m = _mm_castsi128_ps(simd_load128(mask));
  simd_store128(op1, _mm_castps_si128(_mm_blendv_ps(a, b, m)));
#else
  for(unsigned n=0; n<4; n++) {
    if (mask->xmm32s(n) < 0) op1->xmm32u(n) = op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_blendvpd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *mask)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  __m128d a = _mm_castsi128_pd(simd_load128(op1));
  __m128d b = _mm_castsi128_pd(simd_load128(op2));
  __m128d m = _mm_castsi128_pd(simd_load128(mask));
  simd_store128(op1, _mm_castpd_si128(_mm_blendv_pd(a, b, m)));
#else
  if (mask->xmm32s(1) < 0) op1->xmm64u(0) = op2->xmm64u(0);
  if (mask->xmm32s(3) < 0) op1->xmm64u(1) = op2->xmm64u(1);
#endif
}

// arithmetic (logic)
//
// andps/andnps/orps/xorps are a single bitwise op unrolled over 2 64-bit
// lanes - already about as cheap as it gets (the compiler emits ~2
// instructions for the plain C body), so a full load/op/store round trip
// through the vector unit is pure overhead here. Deliberately left plain C
// regardless of BX_SUPPORT_HOST_SIMD_INTRINSICS, same reasoning as the
// unpck* lane-copy functions above.

BX_CPP_INLINE void xmm_andps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++)
    op1->xmm64u(n) &= op2->xmm64u(n);
}

BX_CPP_INLINE void xmm_andnps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++)
    op1->xmm64u(n) = ~(op1->xmm64u(n)) & op2->xmm64u(n);
}

BX_CPP_INLINE void xmm_orps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++)
    op1->xmm64u(n) |= op2->xmm64u(n);
}

BX_CPP_INLINE void xmm_xorps(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for (unsigned n=0; n < 2; n++)
    op1->xmm64u(n) ^= op2->xmm64u(n);
}

// arithmetic (add/sub)

BX_CPP_INLINE void xmm_paddb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_add_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) += op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_paddw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_add_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) += op2->xmm16u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_paddd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_add_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) += op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_paddq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_add_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) += op2->xmm64u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_psubb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sub_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) -= op2->xmmubyte(n);
  }
#endif
}

BX_CPP_INLINE void xmm_psubw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sub_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) -= op2->xmm16u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_psubd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sub_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    op1->xmm32u(n) -= op2->xmm32u(n);
  }
#endif
}

BX_CPP_INLINE void xmm_psubq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sub_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    op1->xmm64u(n) -= op2->xmm64u(n);
  }
#endif
}

// arithmetic (add/sub with saturation)

BX_CPP_INLINE void xmm_paddsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_adds_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmsbyte(n) = SaturateWordSToByteS(Bit16s(op1->xmmsbyte(n)) + Bit16s(op2->xmmsbyte(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_paddsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_adds_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16s(n) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(n)) + Bit32s(op2->xmm16s(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_paddusb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_adds_epu8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = SaturateWordSToByteU(Bit16s(op1->xmmubyte(n)) + Bit16s(op2->xmmubyte(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_paddusw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_adds_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = SaturateDwordSToWordU(Bit32s(op1->xmm16u(n)) + Bit32s(op2->xmm16u(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_psubsb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_subs_epi8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmsbyte(n) = SaturateWordSToByteS(Bit16s(op1->xmmsbyte(n)) - Bit16s(op2->xmmsbyte(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_psubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_subs_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16s(n) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(n)) - Bit32s(op2->xmm16s(n)));
  }
#endif
}

BX_CPP_INLINE void xmm_psubusb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_subs_epu8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++)
  {
    if(op1->xmmubyte(n) > op2->xmmubyte(n))
      op1->xmmubyte(n) -= op2->xmmubyte(n);
    else
      op1->xmmubyte(n) = 0;
  }
#endif
}

BX_CPP_INLINE void xmm_psubusw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_subs_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++)
  {
    if(op1->xmm16u(n) > op2->xmm16u(n))
      op1->xmm16u(n) -= op2->xmm16u(n);
    else
      op1->xmm16u(n) = 0;
  }
#endif
}

// arithmetic (horizontal add/sub)

BX_CPP_INLINE void xmm_phaddw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hadd_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16u(0) = op1->xmm16u(0) + op1->xmm16u(1);
  op1->xmm16u(1) = op1->xmm16u(2) + op1->xmm16u(3);
  op1->xmm16u(2) = op1->xmm16u(4) + op1->xmm16u(5);
  op1->xmm16u(3) = op1->xmm16u(6) + op1->xmm16u(7);

  op1->xmm16u(4) = op2->xmm16u(0) + op2->xmm16u(1);
  op1->xmm16u(5) = op2->xmm16u(2) + op2->xmm16u(3);
  op1->xmm16u(6) = op2->xmm16u(4) + op2->xmm16u(5);
  op1->xmm16u(7) = op2->xmm16u(6) + op2->xmm16u(7);
#endif
}

BX_CPP_INLINE void xmm_phaddd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hadd_epi32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm32u(0) = op1->xmm32u(0) + op1->xmm32u(1);
  op1->xmm32u(1) = op1->xmm32u(2) + op1->xmm32u(3);
  op1->xmm32u(2) = op2->xmm32u(0) + op2->xmm32u(1);
  op1->xmm32u(3) = op2->xmm32u(2) + op2->xmm32u(3);
#endif
}

BX_CPP_INLINE void xmm_phaddsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hadds_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(0)) + Bit32s(op1->xmm16s(1)));
  op1->xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(2)) + Bit32s(op1->xmm16s(3)));
  op1->xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(4)) + Bit32s(op1->xmm16s(5)));
  op1->xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(6)) + Bit32s(op1->xmm16s(7)));

  op1->xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(0)) + Bit32s(op2->xmm16s(1)));
  op1->xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(2)) + Bit32s(op2->xmm16s(3)));
  op1->xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(4)) + Bit32s(op2->xmm16s(5)));
  op1->xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(6)) + Bit32s(op2->xmm16s(7)));
#endif
}

BX_CPP_INLINE void xmm_phsubw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hsub_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16u(0) = op1->xmm16u(0) - op1->xmm16u(1);
  op1->xmm16u(1) = op1->xmm16u(2) - op1->xmm16u(3);
  op1->xmm16u(2) = op1->xmm16u(4) - op1->xmm16u(5);
  op1->xmm16u(3) = op1->xmm16u(6) - op1->xmm16u(7);

  op1->xmm16u(4) = op2->xmm16u(0) - op2->xmm16u(1);
  op1->xmm16u(5) = op2->xmm16u(2) - op2->xmm16u(3);
  op1->xmm16u(6) = op2->xmm16u(4) - op2->xmm16u(5);
  op1->xmm16u(7) = op2->xmm16u(6) - op2->xmm16u(7);
#endif
}

BX_CPP_INLINE void xmm_phsubd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hsub_epi32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm32u(0) = op1->xmm32u(0) - op1->xmm32u(1);
  op1->xmm32u(1) = op1->xmm32u(2) - op1->xmm32u(3);
  op1->xmm32u(2) = op2->xmm32u(0) - op2->xmm32u(1);
  op1->xmm32u(3) = op2->xmm32u(2) - op2->xmm32u(3);
#endif
}

BX_CPP_INLINE void xmm_phsubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_hsubs_epi16(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(0)) - Bit32s(op1->xmm16s(1)));
  op1->xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(2)) - Bit32s(op1->xmm16s(3)));
  op1->xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(4)) - Bit32s(op1->xmm16s(5)));
  op1->xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1->xmm16s(6)) - Bit32s(op1->xmm16s(7)));

  op1->xmm16s(4) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(0)) - Bit32s(op2->xmm16s(1)));
  op1->xmm16s(5) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(2)) - Bit32s(op2->xmm16s(3)));
  op1->xmm16s(6) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(4)) - Bit32s(op2->xmm16s(5)));
  op1->xmm16s(7) = SaturateDwordSToWordS(Bit32s(op2->xmm16s(6)) - Bit32s(op2->xmm16s(7)));
#endif
}

// average

BX_CPP_INLINE void xmm_pavgb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_avg_epu8(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<16; n++) {
    op1->xmmubyte(n) = (op1->xmmubyte(n) + op2->xmmubyte(n) + 1) >> 1;
  }
#endif
}

BX_CPP_INLINE void xmm_pavgw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_avg_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (op1->xmm16u(n) + op2->xmm16u(n) + 1) >> 1;
  }
#endif
}

// multiply

BX_CPP_INLINE void xmm_pmullw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mullo_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16s(n) *= op2->xmm16s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmulhw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mulhi_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    Bit32s product = Bit32s(op1->xmm16s(n)) * Bit32s(op2->xmm16s(n));
    op1->xmm16u(n) = (Bit16u)(product >> 16);
  }
#endif
}

BX_CPP_INLINE void xmm_pmulhuw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mulhi_epu16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    Bit32u product = Bit32u(op1->xmm16u(n)) * Bit32u(op2->xmm16u(n));
    op1->xmm16u(n) = (Bit16u)(product >> 16);
  }
#endif
}

BX_CPP_INLINE void xmm_pmulld(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mullo_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++) {
    op1->xmm32s(n) *= op2->xmm32s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmullq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512DQ_VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mullo_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<2; n++) {
    op1->xmm64s(n) *= op2->xmm64s(n);
  }
#endif
}

BX_CPP_INLINE void xmm_pmuldq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE4_1) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mul_epi32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm64s(0) = Bit64s(op1->xmm32s(0)) * Bit64s(op2->xmm32s(0));
  op1->xmm64s(1) = Bit64s(op1->xmm32s(2)) * Bit64s(op2->xmm32s(2));
#endif
}

BX_CPP_INLINE void xmm_pmuludq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mul_epu32(simd_load128(op1), simd_load128(op2)));
#else
  op1->xmm64u(0) = Bit64u(op1->xmm32u(0)) * Bit64u(op2->xmm32u(0));
  op1->xmm64u(1) = Bit64u(op1->xmm32u(2)) * Bit64u(op2->xmm32u(2));
#endif
}

BX_CPP_INLINE void xmm_pmulhrsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_mulhrs_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++) {
    op1->xmm16u(n) = (((Bit32s(op1->xmm16s(n)) * Bit32s(op2->xmm16s(n))) >> 14) + 1) >> 1;
  }
#endif
}

// multiply/add

BX_CPP_INLINE void xmm_pmaddubsw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSSE3) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_maddubs_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<8; n++)
  {
    Bit32s temp = Bit32s(op1->xmmubyte(n*2))   * Bit32s(op2->xmmsbyte(n*2)) +
                  Bit32s(op1->xmmubyte(n*2+1)) * Bit32s(op2->xmmsbyte(n*2+1));

    op1->xmm16s(n) = SaturateDwordSToWordS(temp);
  }
#endif
}

BX_CPP_INLINE void xmm_pmaddwd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_madd_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0; n<4; n++)
  {
    op1->xmm32u(n) = Bit32s(op1->xmm16s(n*2))   * Bit32s(op2->xmm16s(n*2)) +
                     Bit32s(op1->xmm16s(n*2+1)) * Bit32s(op2->xmm16s(n*2+1));
  }
#endif
}

// broadcast

BX_CPP_INLINE void xmm_pbroadcastb(BxPackedXmmRegister *op, Bit8u val_8)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_set1_epi8((char) val_8));
#else
  for(unsigned n=0; n<16; n++) {
    op->xmmubyte(n) = val_8;
  }
#endif
}

BX_CPP_INLINE void xmm_pbroadcastw(BxPackedXmmRegister *op, Bit16u val_16)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_set1_epi16((short) val_16));
#else
  for(unsigned n=0; n<8; n++) {
    op->xmm16u(n) = val_16;
  }
#endif
}

BX_CPP_INLINE void xmm_pbroadcastd(BxPackedXmmRegister *op, Bit32u val_32)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_set1_epi32((int) val_32));
#else
  for(unsigned n=0; n<4; n++) {
    op->xmm32u(n) = val_32;
  }
#endif
}

BX_CPP_INLINE void xmm_pbroadcastq(BxPackedXmmRegister *op, Bit64u val_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_set1_epi64x((Bit64s) val_64));
#else
  for(unsigned n=0; n<2; n++) {
    op->xmm64u(n) = val_64;
  }
#endif
}

#if BX_SUPPORT_EVEX
BX_CPP_INLINE void simd_pbroadcastb(BxPackedAvxRegister *op, Bit8u val_8, unsigned len)
{
  for(unsigned n=0; n < len; n++) {
    op->vmmubyte(n) = val_8;
  }
}

BX_CPP_INLINE void simd_pbroadcastw(BxPackedAvxRegister *op, Bit16u val_16, unsigned len)
{
  for(unsigned n=0; n < len; n++) {
    op->vmm16u(n) = val_16;
  }
}

BX_CPP_INLINE void simd_pbroadcastd(BxPackedAvxRegister *op, Bit32u val_32, unsigned len)
{
  for(unsigned n=0; n < len; n++) {
    op->vmm32u(n) = val_32;
  }
}

BX_CPP_INLINE void simd_pbroadcastq(BxPackedAvxRegister *op, Bit64u val_64, unsigned len)
{
  for(unsigned n=0; n < len; n++) {
    op->vmm64u(n) = val_64;
  }
}
#endif

// sum of absolute differences (SAD)

BX_CPP_INLINE void xmm_psadbw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sad_epu8(simd_load128(op1), simd_load128(op2)));
#else
  unsigned temp = 0;
  for (unsigned n=0; n < 8; n++)
    temp += abs(op1->xmmubyte(n) - op2->xmmubyte(n));

  op1->xmm64u(0) = Bit64u(temp);

  temp = 0;
  for (unsigned n=8; n < 16; n++)
    temp += abs(op1->xmmubyte(n) - op2->xmmubyte(n));

  op1->xmm64u(1) = Bit64u(temp);
#endif
}

// multiple sum of absolute differences (MSAD)

BX_CPP_INLINE Bit16u sad_quadruple(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, int op1_offset, int op2_offset)
{
  Bit32u r = 0;

  for (unsigned n=0; n < 4; n++) {
    Bit8u temp1 = op1->xmmubyte(n + op1_offset);
    Bit8u temp2 = op2->xmmubyte(n + op2_offset);

    r += abs(temp1 - temp2);
  }

  return r;
}

BX_CPP_INLINE void xmm_mpsadbw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, Bit8u control)
{
  unsigned src_offset = (control & 0x3) * 4;
  unsigned dst_offset = ((control >> 2) & 0x1) * 4;

  for (unsigned j=0; j < 8; j++) {
    r->xmm16u(j) = sad_quadruple(op1, op2, dst_offset + j, src_offset);
  }
}

BX_CPP_INLINE void xmm_dbpsadbw(BxPackedXmmRegister *r, const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  // assuming shuffle of op2 was done outside
  r->xmm16u(0) = sad_quadruple(op1, op2,  0,  0);
  r->xmm16u(1) = sad_quadruple(op1, op2,  0,  1);
  r->xmm16u(2) = sad_quadruple(op1, op2,  4,  2);
  r->xmm16u(3) = sad_quadruple(op1, op2,  4,  3);
  r->xmm16u(4) = sad_quadruple(op1, op2,  8,  8);
  r->xmm16u(5) = sad_quadruple(op1, op2,  8,  9);
  r->xmm16u(6) = sad_quadruple(op1, op2, 12, 10);
  r->xmm16u(7) = sad_quadruple(op1, op2, 12, 11);
}

// conflict

#if BX_SUPPORT_EVEX

BX_CPP_INLINE Bit32u simd_pconflictd(const BxPackedAvxRegister *op, int index)
{
  Bit32u result = 0;
  // compare index element with all previous elements
  for (int i=0; i < index; i++) {
    if (op->vmm32u(index) == op->vmm32u(i)) result |= (1 << i);
  }
  return result;
}

BX_CPP_INLINE Bit32u simd_pconflictq(const BxPackedAvxRegister *op, int index)
{
  Bit32u result = 0;
  // compare index element with all previous elements
  for (int i=0; i < index; i++) {
    if (op->vmm64u(index) == op->vmm64u(i)) result |= (1 << i);
  }
  return result;
}

#endif

// bitwise select

BX_CPP_INLINE void xmm_pselect(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2, const BxPackedXmmRegister *op3)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  __m128i a = simd_load128(op1);
  __m128i b = simd_load128(op2);
  __m128i c = simd_load128(op3);
  simd_store128(op1, _mm_or_si128(_mm_and_si128(c, a), _mm_andnot_si128(c, b)));
#else
  for(unsigned n=0;n < 2;n++) {
    op1->xmm64u(n) = (op3->xmm64u(n) & op1->xmm64u(n)) | (~op3->xmm64u(n) & op2->xmm64u(n));
  }
#endif
}

// shift

BX_CPP_INLINE void xmm_psravw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512BW_VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srav_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 8; n++) {
    unsigned shift = op2->xmm16u(n);
    if(shift > 15)
      op1->xmm16u(n) = (op1->xmm16s(n) < 0) ? 0xffff : 0;
    else
      op1->xmm16u(n) = (Bit16u)(op1->xmm16s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psravd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srav_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = (op1->xmm32s(n) < 0) ? 0xffffffff : 0;
    else
      op1->xmm32u(n) = (Bit32u)(op1->xmm32s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psravq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srav_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 2; n++) {
    Bit64u shift = op2->xmm64u(n);
    if(shift > 63)
      op1->xmm64u(n) = (op1->xmm64s(n) < 0) ? BX_CONST64(0xffffffffffffffff) : 0;
    else
      op1->xmm64u(n) = (Bit64u)(op1->xmm64s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psllvw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512BW_VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sllv_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 8; n++) {
    unsigned shift = op2->xmm16u(n);
    if(shift > 15)
      op1->xmm16u(n) = 0;
    else
      op1->xmm16u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psllvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sllv_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = 0;
    else
      op1->xmm32u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psllvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_sllv_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 2; n++) {
    Bit64u shift = op2->xmm64u(n);
    if(shift > 63)
      op1->xmm64u(n) = 0;
    else
      op1->xmm64u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrlvw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512BW_VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srlv_epi16(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 8; n++) {
    unsigned shift = op2->xmm16u(n);
    if(shift > 15)
      op1->xmm16u(n) = 0;
    else
      op1->xmm16u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrlvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srlv_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 4; n++) {
    Bit32u shift = op2->xmm32u(n);
    if(shift > 31)
      op1->xmm32u(n) = 0;
    else
      op1->xmm32u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrlvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_srlv_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for (unsigned n=0; n < 2; n++) {
    Bit64u shift = op2->xmm64u(n);
    if(shift > 63)
      op1->xmm64u(n) = 0;
    else
      op1->xmm64u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psraw(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sra_epi16(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 15) {
    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) = (op->xmm16s(n) < 0) ? 0xffff : 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) = (Bit16u)(op->xmm16s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psrad(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sra_epi32(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 31) {
    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) = (op->xmm32s(n) < 0) ? 0xffffffff : 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) = (Bit32u)(op->xmm32s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psraq(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sra_epi64(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 63) {
    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) = (op->xmm64s(n) < 0) ? BX_CONST64(0xffffffffffffffff) : 0;
  }
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) = (Bit64u)(op->xmm64s(n) >> shift);
  }
#endif
}

BX_CPP_INLINE void xmm_psrlw(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_srl_epi16(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 15) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrld(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_srl_epi32(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 31) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrlq(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_srl_epi64(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 63) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) >>= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psllw(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sll_epi16(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 15) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 8; n++)
      op->xmm16u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_pslld(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sll_epi32(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 31) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 4; n++)
      op->xmm32u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psllq(BxPackedXmmRegister *op, Bit64u shift_64)
{
#if defined(BX_HOST_SUPPORT_SSE2) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_sll_epi64(simd_load128(op), simd_shift_count(shift_64)));
#else
  if(shift_64 > 63) op->clear();
  else
  {
    Bit8u shift = (Bit8u) shift_64;

    for (unsigned n=0; n < 2; n++)
      op->xmm64u(n) <<= shift;
  }
#endif
}

BX_CPP_INLINE void xmm_psrldq(BxPackedXmmRegister *op, Bit64u shift)
{
  if(shift > 15) op->clear();
  else {
    if (shift > 7) {
      op->xmm64u(0) = op->xmm64u(1);
      op->xmm64u(1) = 0;
      shift -= 8;
    }

    shift <<= 3;

    if (shift != 0) {
      op->xmm64u(0) = (op->xmm64u(0) >> shift) | (op->xmm64u(1) << (64-shift));
      op->xmm64u(1) = (op->xmm64u(1) >> shift);
    }
  }
}

BX_CPP_INLINE void xmm_pslldq(BxPackedXmmRegister *op, Bit64u shift)
{
  if(shift > 15) op->clear();
  else {
    if (shift > 7) {
      op->xmm64u(1) = op->xmm64u(0);
      op->xmm64u(0) = 0;
      shift -= 8;
    }

    shift <<= 3;

    if (shift != 0) {
      op->xmm64u(1) = (op->xmm64u(1) << shift) | (op->xmm64u(0) >> (64-shift));
      op->xmm64u(0) = (op->xmm64u(0) << shift);
    }
  }
}

BX_CPP_INLINE void xmm_palignr(BxPackedXmmRegister *op2, const BxPackedXmmRegister *op1, Bit8u shift)
{
  // op2 = [op1:op2] >> shift

  if (shift > 15) {
    *op2 = *op1;
    xmm_psrldq(op2, shift - 16);
    return;
  }

  shift <<= 3;

  if (shift > 64) {
    shift -= 64;
    op2->xmm64u(0) = (op2->xmm64u(1) >> shift) | (op1->xmm64u(0) << (64-shift));
    op2->xmm64u(1) = (op1->xmm64u(0) >> shift) | (op1->xmm64u(1) << (64-shift));
  }
  else if (shift == 64) {
    op2->xmm64u(0) = op2->xmm64u(1);
    op2->xmm64u(1) = op1->xmm64u(0);
  }
  else if (shift != 0) {
    op2->xmm64u(0) = (op2->xmm64u(0) >> shift) | (op2->xmm64u(1) << (64-shift));
    op2->xmm64u(1) = (op2->xmm64u(1) >> shift) | (op1->xmm64u(0) << (64-shift));
  }
}

// rotate (right)

BX_CPP_INLINE void xmm_prorb(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
  int shift = shift_ctrl & 0x07;

  for(unsigned n=0;n<16;n++) {
    op->xmmubyte(n) = (op->xmmubyte(n) >> shift) | (op->xmmubyte(n) << (8 - shift));
  }
}

BX_CPP_INLINE void xmm_prorw(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
  int shift = shift_ctrl & 0x0f;

  for(unsigned n=0;n<8;n++) {
    op->xmm16u(n) = (op->xmm16u(n) >> shift) | (op->xmm16u(n) << (16 - shift));
  }
}

BX_CPP_INLINE void xmm_prord(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_rorv_epi32(simd_load128(op), simd_rotate_count32(shift_ctrl, 0x1f)));
#else
  int shift = shift_ctrl & 0x1f;

  for(unsigned n=0;n<4;n++) {
    op->xmm32u(n) = (op->xmm32u(n) >> shift) | (op->xmm32u(n) << (32 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prorq(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_rorv_epi64(simd_load128(op), simd_rotate_count64(shift_ctrl, 0x3f)));
#else
  int shift = shift_ctrl & 0x3f;

  for(unsigned n=0;n<2;n++) {
    op->xmm64u(n) = (op->xmm64u(n) >> shift) | (op->xmm64u(n) << (64 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prorvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_rorv_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0;n<4;n++) {
    int shift = op2->xmm32u(n) & 0x1f;
    op1->xmm32u(n) = (op1->xmm32u(n) >> shift) | (op1->xmm32u(n) << (32 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prorvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_rorv_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0;n<2;n++) {
    int shift = op2->xmm64u(n) & 0x3f;
    op1->xmm64u(n) = (op1->xmm64u(n) >> shift) | (op1->xmm64u(n) << (64 - shift));
  }
#endif
}

// rotate (left)

BX_CPP_INLINE void xmm_prolb(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
  int shift = shift_ctrl & 0x07;

  for(unsigned n=0;n<16;n++) {
    op->xmmubyte(n) = (op->xmmubyte(n) << shift) | (op->xmmubyte(n) >> (8 - shift));
  }
}

BX_CPP_INLINE void xmm_prolw(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
  int shift = shift_ctrl & 0x0f;

  for(unsigned n=0;n<8;n++) {
    op->xmm16u(n) = (op->xmm16u(n) << shift) | (op->xmm16u(n) >> (16 - shift));
  }
}

BX_CPP_INLINE void xmm_prold(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_rolv_epi32(simd_load128(op), simd_rotate_count32(shift_ctrl, 0x1f)));
#else
  int shift = shift_ctrl & 0x1f;

  for(unsigned n=0;n<4;n++) {
    op->xmm32u(n) = (op->xmm32u(n) << shift) | (op->xmm32u(n) >> (32 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prolq(BxPackedXmmRegister *op, Bit64u shift_ctrl)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op, _mm_rolv_epi64(simd_load128(op), simd_rotate_count64(shift_ctrl, 0x3f)));
#else
  int shift = shift_ctrl & 0x3f;

  for(unsigned n=0;n<2;n++) {
    op->xmm64u(n) = (op->xmm64u(n) << shift) | (op->xmm64u(n) >> (64 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prolvd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_rolv_epi32(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0;n<4;n++) {
    int shift = op2->xmm32u(n) & 0x1f;
    op1->xmm32u(n) = (op1->xmm32u(n) << shift) | (op1->xmm32u(n) >> (32 - shift));
  }
#endif
}

BX_CPP_INLINE void xmm_prolvq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
#if defined(BX_HOST_SUPPORT_AVX512VL) && BX_SUPPORT_HOST_SIMD_INTRINSICS != 0
  simd_store128(op1, _mm_rolv_epi64(simd_load128(op1), simd_load128(op2)));
#else
  for(unsigned n=0;n<2;n++) {
    int shift = op2->xmm64u(n) & 0x3f;
    op1->xmm64u(n) = (op1->xmm64u(n) << shift) | (op1->xmm64u(n) >> (64 - shift));
  }
#endif
}

// variable shift/rotate (XOP)
//
// Legacy AMD XOP instructions - no portable modern-Intel intrinsic exists
// at this granularity below AVX-512, and no clean equivalent exists at all
// for the byte/word rotate forms. Left as portable C; if you specifically
// target AMD XOP-capable hosts, <x86intrin.h>'s _mm_rot_epi8/16/32/64 and
// _mm_shl_epi8/... intrinsics (guarded by __XOP__) map to these directly.

BX_CPP_INLINE void xmm_protb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // rotate left
      shift &= 0x7;
      op1->xmmubyte(n) = (op1->xmmubyte(n) << shift) | (op1->xmmubyte(n) >> (8 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x7;
      op1->xmmubyte(n) = (op1->xmmubyte(n) >> shift) | (op1->xmmubyte(n) << (8 - shift));
    }
  }
}

BX_CPP_INLINE void xmm_protw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmsbyte(n*2);
    if (shift > 0) {
      // rotate left
      shift &= 0xf;
      op1->xmm16u(n) = (op1->xmm16u(n) << shift) | (op1->xmm16u(n) >> (16 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0xf;
      op1->xmm16u(n) = (op1->xmm16u(n) >> shift) | (op1->xmm16u(n) << (16 - shift));
    }
  }
}

BX_CPP_INLINE void xmm_protd(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // rotate left
      shift &= 0x1f;
      op1->xmm32u(n) = (op1->xmm32u(n) << shift) | (op1->xmm32u(n) >> (32 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x1f;
      op1->xmm32u(n) = (op1->xmm32u(n) >> shift) | (op1->xmm32u(n) << (32 - shift));
    }
  }
}

BX_CPP_INLINE void xmm_protq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // rotate left
      shift &= 0x3f;
      op1->xmm64u(n) = (op1->xmm64u(n) << shift) | (op1->xmm64u(n) >> (64 - shift));
    }
    else if (shift < 0) {
      // rotate right
      shift = -shift & 0x3f;
      op1->xmm64u(n) = (op1->xmm64u(n) >> shift) | (op1->xmm64u(n) << (64 - shift));
    }
  }
}

BX_CPP_INLINE void xmm_pshab(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // shift left
      op1->xmmsbyte(n) <<= (shift & 0x7);
    }
    else if (shift < 0) {
      // shift right
      op1->xmmsbyte(n) >>= (-shift & 0x7);
    }
  }
}

BX_CPP_INLINE void xmm_pshaw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmsbyte(n*2);
    if (shift > 0) {
      // shift left
      op1->xmm16s(n) <<= (shift & 0xf);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm16s(n) >>= (-shift & 0xf);
    }
  }
}

BX_CPP_INLINE void xmm_pshad(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // shift left
      op1->xmm32s(n) <<= (shift & 0x1f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm32s(n) >>= (-shift & 0x1f);
    }
  }
}

BX_CPP_INLINE void xmm_pshaq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // shift left
      op1->xmm64s(n) <<= (shift & 0x3f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm64s(n) >>= (-shift & 0x3f);
    }
  }
}

BX_CPP_INLINE void xmm_pshlb(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 16;n++) {
    int shift = op2->xmmsbyte(n);
    if (shift > 0) {
      // shift left
      op1->xmmubyte(n) <<= (shift & 0x7);
    }
    else if (shift < 0) {
      // shift right
      op1->xmmubyte(n) >>= (-shift & 0x7);
    }
  }
}

BX_CPP_INLINE void xmm_pshlw(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 8;n++) {
    int shift = op2->xmmubyte(n*2);
    if (shift > 0) {
      // shift left
      op1->xmm16u(n) <<= (shift & 0xf);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm16u(n) >>= (-shift & 0xf);
    }
  }
}

BX_CPP_INLINE void xmm_pshld(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 4;n++) {
    int shift = op2->xmmsbyte(n*4);
    if (shift > 0) {
      // shift left
      op1->xmm32u(n) <<= (shift & 0x1f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm32u(n) >>= (-shift & 0x1f);
    }
  }
}

BX_CPP_INLINE void xmm_pshlq(BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2)
{
  for(unsigned n=0;n < 2;n++) {
    int shift = op2->xmmsbyte(n*8);
    if (shift > 0) {
      // shift left
      op1->xmm64u(n) <<= (shift & 0x3f);
    }
    else if (shift < 0) {
      // shift right
      op1->xmm64u(n) >>= (-shift & 0x3f);
    }
  }
}

#endif
