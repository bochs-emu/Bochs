/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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

extern void mxcsr_to_softfloat_status_word(float_status_t &status, bx_mxcsr_t mxcsr);

#include "simd_pfp.h"

//////////////////////////
// AVX FMA Instructions //
//////////////////////////

#define AVX2_FMA_PACKED(HANDLER, func)                                        \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());                     \
    BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());                     \
    BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());                     \
    unsigned len = i->getVL();                                                \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
    softfloat_status_word_rc_override(status, i);                             \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status);         \
                                                                              \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                    \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX2_FMA_PACKED(VFMADDPD_VpdHpdWpdR, xmm_fmaddpd)
AVX2_FMA_PACKED(VFMADDPS_VpsHpsWpsR, xmm_fmaddps)
AVX2_FMA_PACKED(VFMADDSUBPD_VpdHpdWpdR, xmm_fmaddsubpd)
AVX2_FMA_PACKED(VFMADDSUBPS_VpsHpsWpsR, xmm_fmaddsubps)
AVX2_FMA_PACKED(VFMSUBADDPD_VpdHpdWpdR, xmm_fmsubaddpd)
AVX2_FMA_PACKED(VFMSUBADDPS_VpsHpsWpsR, xmm_fmsubaddps)
AVX2_FMA_PACKED(VFMSUBPD_VpdHpdWpdR, xmm_fmsubpd)
AVX2_FMA_PACKED(VFMSUBPS_VpsHpsWpsR, xmm_fmsubps)
AVX2_FMA_PACKED(VFNMADDPD_VpdHpdWpdR, xmm_fnmaddpd)
AVX2_FMA_PACKED(VFNMADDPS_VpsHpsWpsR, xmm_fnmaddps)
AVX2_FMA_PACKED(VFNMSUBPD_VpdHpdWpdR, xmm_fnmsubpd)
AVX2_FMA_PACKED(VFNMSUBPS_VpsHpsWpsR, xmm_fnmsubps)

#define AVX2_FMA_SCALAR_SINGLE(HANDLER, func)                                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                        \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                        \
    float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                        \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
    softfloat_status_word_rc_override(status, i);                             \
    op1 = (func)(op1, op2, op3, status);                                      \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);                                 \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX2_FMA_SCALAR_SINGLE(VFMADDSS_VpsHssWssR, float32_fmadd)
AVX2_FMA_SCALAR_SINGLE(VFMSUBSS_VpsHssWssR, float32_fmsub)
AVX2_FMA_SCALAR_SINGLE(VFNMADDSS_VpsHssWssR, float32_fnmadd)
AVX2_FMA_SCALAR_SINGLE(VFNMSUBSS_VpsHssWssR, float32_fnmsub)

#define AVX2_FMA_SCALAR_DOUBLE(HANDLER, func)                                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                        \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                        \
    float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                        \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
    softfloat_status_word_rc_override(status, i);                             \
    op1 = (func)(op1, op2, op3, status);                                      \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);                                 \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX2_FMA_SCALAR_DOUBLE(VFMADDSD_VpdHsdWsdR, float64_fmadd)
AVX2_FMA_SCALAR_DOUBLE(VFMSUBSD_VpdHsdWsdR, float64_fmsub)
AVX2_FMA_SCALAR_DOUBLE(VFNMADDSD_VpdHsdWsdR, float64_fnmadd)
AVX2_FMA_SCALAR_DOUBLE(VFNMSUBSD_VpdHsdWsdR, float64_fnmsub)

//////////////////////////////////
// FMA4 (AMD) specific handlers //
//////////////////////////////////

#define FMA4_SINGLE_SCALAR(HANDLER, func)                                     \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                        \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                        \
    float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                        \
                                                                              \
    BxPackedXmmRegister dest;                                                 \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
                                                                              \
    dest.xmm64u(0) = (func)(op1, op2, op3, status);                           \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_SINGLE_SCALAR(VFMADDSS_VssHssWssVIbR, float32_fmadd)
FMA4_SINGLE_SCALAR(VFMSUBSS_VssHssWssVIbR, float32_fmsub)

FMA4_SINGLE_SCALAR(VFNMADDSS_VssHssWssVIbR, float32_fnmadd)
FMA4_SINGLE_SCALAR(VFNMSUBSS_VssHssWssVIbR, float32_fnmsub)

#define FMA4_DOUBLE_SCALAR(HANDLER, func)                                     \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                        \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                        \
    float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                        \
    BxPackedXmmRegister dest;                                                 \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
                                                                              \
    dest.xmm64u(0) = (func)(op1, op2, op3, status);                           \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(get_exception_flags(status));                         \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_DOUBLE_SCALAR(VFMADDSD_VsdHsdWsdVIbR, float64_fmadd)
FMA4_DOUBLE_SCALAR(VFMSUBSD_VsdHsdWsdVIbR, float64_fmsub)

FMA4_DOUBLE_SCALAR(VFNMADDSD_VsdHsdWsdVIbR, float64_fnmadd)
FMA4_DOUBLE_SCALAR(VFNMSUBSD_VsdHsdWsdVIbR, float64_fnmsub)

#endif
