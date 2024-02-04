/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2023 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_AVX

extern float_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

#include "simd_pfp.h"

//////////////////////////
// AVX FMA Instructions //
//////////////////////////

#define AVX2_FMA_SCALAR_SINGLE(HANDLER, func)                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)         \
  {                                                                           \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                        \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                        \
    float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                        \
                                                                              \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);            \
    softfloat_status_word_rc_override(status, i);                             \
    op1 = (func)(op1, op2, op3, &status);                                     \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);                                 \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX2_FMA_SCALAR_SINGLE(VFMADDSS_VpsHssWssR, f32_fmadd)
AVX2_FMA_SCALAR_SINGLE(VFMSUBSS_VpsHssWssR, f32_fmsub)
AVX2_FMA_SCALAR_SINGLE(VFNMADDSS_VpsHssWssR, f32_fnmadd)
AVX2_FMA_SCALAR_SINGLE(VFNMSUBSS_VpsHssWssR, f32_fnmsub)

#define AVX2_FMA_SCALAR_DOUBLE(HANDLER, func)                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)         \
  {                                                                           \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                        \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                        \
    float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                        \
                                                                              \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);            \
    softfloat_status_word_rc_override(status, i);                             \
    op1 = (func)(op1, op2, op3, &status);                                     \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);                                 \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX2_FMA_SCALAR_DOUBLE(VFMADDSD_VpdHsdWsdR, f64_fmadd)
AVX2_FMA_SCALAR_DOUBLE(VFMSUBSD_VpdHsdWsdR, f64_fmsub)
AVX2_FMA_SCALAR_DOUBLE(VFNMADDSD_VpdHsdWsdR, f64_fnmadd)
AVX2_FMA_SCALAR_DOUBLE(VFNMSUBSD_VpdHsdWsdR, f64_fnmsub)

//////////////////////////////////
// FMA4 (AMD) specific handlers //
//////////////////////////////////

#define FMA4_SINGLE_SCALAR(HANDLER, func)                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)         \
  {                                                                           \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                        \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                        \
    float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                        \
                                                                              \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);            \
                                                                              \
    BxPackedXmmRegister dest;                                                 \
    dest.xmm64u(0) = (func)(op1, op2, op3, &status);                          \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_SINGLE_SCALAR(VFMADDSS_VssHssWssVIbR, f32_fmadd)
FMA4_SINGLE_SCALAR(VFMSUBSS_VssHssWssVIbR, f32_fmsub)

FMA4_SINGLE_SCALAR(VFNMADDSS_VssHssWssVIbR, f32_fnmadd)
FMA4_SINGLE_SCALAR(VFNMSUBSS_VssHssWssVIbR, f32_fnmsub)

#define FMA4_DOUBLE_SCALAR(HANDLER, func)                                     \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)         \
  {                                                                           \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                        \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                        \
    float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                        \
                                                                              \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);            \
                                                                              \
    BxPackedXmmRegister dest;                                                 \
    dest.xmm64u(0) = (func)(op1, op2, op3, &status);                          \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_DOUBLE_SCALAR(VFMADDSD_VsdHsdWsdVIbR, f64_fmadd)
FMA4_DOUBLE_SCALAR(VFMSUBSD_VsdHsdWsdVIbR, f64_fmsub)

FMA4_DOUBLE_SCALAR(VFNMADDSD_VsdHsdWsdVIbR, f64_fnmadd)
FMA4_DOUBLE_SCALAR(VFNMSUBSD_VsdHsdWsdVIbR, f64_fnmsub)

#endif
