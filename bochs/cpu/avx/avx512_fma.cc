/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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

#if BX_SUPPORT_EVEX

extern void mxcsr_to_softfloat_status_word(float_status_t &status, bx_mxcsr_t mxcsr);

#include "simd_int.h"
#include "simd_pfp.h"

//////////////////////////////
// AVX-512 FMA Instructions //
//////////////////////////////

#define EVEX_FMA_PACKED_SINGLE(HANDLER, func)                                   \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)   \
  {                                                                             \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());                       \
    BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());                       \
    BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());                       \
    unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());                          \
    unsigned len = i->getVL();                                                  \
                                                                                \
    float_status_t status;                                                      \
    mxcsr_to_softfloat_status_word(status, MXCSR);                              \
    softfloat_status_word_rc_override(status, i);                               \
                                                                                \
    for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)           \
      (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status, tmp_mask); \
                                                                                \
    check_exceptionsSSE(get_exception_flags(status));                           \
                                                                                \
    if (! i->isZeroMasking()) {                                                 \
      for (unsigned n=0; n < len; n++, mask >>= 4)                              \
        xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);  \
                                                                                \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                         \
    }                                                                           \
    else {                                                                      \
      BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                    \
    }                                                                           \
                                                                                \
    BX_NEXT_INSTR(i);                                                           \
  }

EVEX_FMA_PACKED_SINGLE(VFMADDPS_MASK_VpsHpsWpsR, xmm_fmaddps_mask)
EVEX_FMA_PACKED_SINGLE(VFMADDSUBPS_MASK_VpsHpsWpsR, xmm_fmaddsubps_mask)
EVEX_FMA_PACKED_SINGLE(VFMSUBADDPS_MASK_VpsHpsWpsR, xmm_fmsubaddps_mask)
EVEX_FMA_PACKED_SINGLE(VFMSUBPS_MASK_VpsHpsWpsR, xmm_fmsubps_mask)
EVEX_FMA_PACKED_SINGLE(VFNMADDPS_MASK_VpsHpsWpsR, xmm_fnmaddps_mask)
EVEX_FMA_PACKED_SINGLE(VFNMSUBPS_MASK_VpsHpsWpsR, xmm_fnmsubps_mask)

#define EVEX_FMA_PACKED_DOUBLE(HANDLER, func)                                   \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)   \
  {                                                                             \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());                       \
    BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());                       \
    BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());                       \
    unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());                           \
    unsigned len = i->getVL();                                                  \
                                                                                \
    float_status_t status;                                                      \
    mxcsr_to_softfloat_status_word(status, MXCSR);                              \
    softfloat_status_word_rc_override(status, i);                               \
                                                                                \
    for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)           \
      (func)(&op1.vmm128(n), &op2.vmm128(n), &op3.vmm128(n), status, tmp_mask); \
                                                                                \
    check_exceptionsSSE(get_exception_flags(status));                           \
                                                                                \
    if (! i->isZeroMasking()) {                                                 \
      for (unsigned n=0; n < len; n++, mask >>= 2)                              \
        xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);  \
                                                                                \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                         \
    }                                                                           \
    else {                                                                      \
      BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                    \
    }                                                                           \
                                                                                \
    BX_NEXT_INSTR(i);                                                           \
  }

EVEX_FMA_PACKED_DOUBLE(VFMADDPD_MASK_VpdHpdWpdR, xmm_fmaddpd_mask)
EVEX_FMA_PACKED_DOUBLE(VFMADDSUBPD_MASK_VpdHpdWpdR, xmm_fmaddsubpd_mask)
EVEX_FMA_PACKED_DOUBLE(VFMSUBADDPD_MASK_VpdHpdWpdR, xmm_fmsubaddpd_mask)
EVEX_FMA_PACKED_DOUBLE(VFMSUBPD_MASK_VpdHpdWpdR, xmm_fmsubpd_mask)
EVEX_FMA_PACKED_DOUBLE(VFNMADDPD_MASK_VpdHpdWpdR, xmm_fnmaddpd_mask)
EVEX_FMA_PACKED_DOUBLE(VFNMSUBPD_MASK_VpdHpdWpdR, xmm_fnmsubpd_mask)

#define EVEX_FMA_SCALAR_SINGLE(HANDLER, func)                                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {                                \
      float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                      \
      float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                      \
      float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                      \
                                                                              \
      float_status_t status;                                                  \
      mxcsr_to_softfloat_status_word(status, MXCSR);                          \
      softfloat_status_word_rc_override(status, i);                           \
      op1 = (func)(op1, op2, op3, status);                                    \
      check_exceptionsSSE(get_exception_flags(status));                       \
                                                                              \
      BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);                               \
    }                                                                         \
    else {                                                                    \
      if (i->isZeroMasking())                                                 \
        BX_WRITE_XMM_REG_LO_DWORD(i->dst(), 0);                               \
    }                                                                         \
                                                                              \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
    BX_NEXT_INSTR(i);                                                         \
  }

EVEX_FMA_SCALAR_SINGLE(VFMADDSS_MASK_VpsHssWssR, float32_fmadd)
EVEX_FMA_SCALAR_SINGLE(VFMSUBSS_MASK_VpsHssWssR, float32_fmsub)
EVEX_FMA_SCALAR_SINGLE(VFNMADDSS_MASK_VpsHssWssR, float32_fnmadd)
EVEX_FMA_SCALAR_SINGLE(VFNMSUBSS_MASK_VpsHssWssR, float32_fnmsub)

#define EVEX_FMA_SCALAR_DOUBLE(HANDLER, func)                                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {                                \
      float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                      \
      float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                      \
      float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                      \
                                                                              \
      float_status_t status;                                                  \
      mxcsr_to_softfloat_status_word(status, MXCSR);                          \
      softfloat_status_word_rc_override(status, i);                           \
      op1 = (func)(op1, op2, op3, status);                                    \
      check_exceptionsSSE(get_exception_flags(status));                       \
                                                                              \
      BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);                               \
    }                                                                         \
    else {                                                                    \
      if (i->isZeroMasking())                                                 \
        BX_WRITE_XMM_REG_LO_QWORD(i->dst(), 0);                               \
    }                                                                         \
                                                                              \
    BX_CLEAR_AVX_HIGH128(i->dst());                                           \
    BX_NEXT_INSTR(i);                                                         \
  }

EVEX_FMA_SCALAR_DOUBLE(VFMADDSD_MASK_VpdHsdWsdR, float64_fmadd)
EVEX_FMA_SCALAR_DOUBLE(VFMSUBSD_MASK_VpdHsdWsdR, float64_fmsub)
EVEX_FMA_SCALAR_DOUBLE(VFNMADDSD_MASK_VpdHsdWsdR, float64_fnmadd)
EVEX_FMA_SCALAR_DOUBLE(VFNMSUBSD_MASK_VpdHsdWsdR, float64_fnmsub)

#endif
