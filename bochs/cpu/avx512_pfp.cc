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

#include "fpu/softfloat-compare.h"
#include "simd_int.h"
#include "simd_pfp.h"

#define EVEX_OP_PACKED_SINGLE(HANDLER, func)                                                \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)               \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());                                      \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status;                                                                  \
    mxcsr_to_softfloat_status_word(status, MXCSR);                                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)                       \
      (func)(&op1.vmm128(n), &op2.vmm128(n), status, tmp_mask);                             \
                                                                                            \
    check_exceptionsSSE(status.float_exception_flags);                                      \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, mask >>= 4)                                          \
        xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);              \
                                                                                            \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                                \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_PACKED_SINGLE(VADDPS_MASK_VpsHpsWpsR, xmm_addps_mask)
EVEX_OP_PACKED_SINGLE(VSUBPS_MASK_VpsHpsWpsR, xmm_subps_mask)
EVEX_OP_PACKED_SINGLE(VMULPS_MASK_VpsHpsWpsR, xmm_mulps_mask)
EVEX_OP_PACKED_SINGLE(VDIVPS_MASK_VpsHpsWpsR, xmm_divps_mask)
EVEX_OP_PACKED_SINGLE(VMAXPS_MASK_VpsHpsWpsR, xmm_maxps_mask)
EVEX_OP_PACKED_SINGLE(VMINPS_MASK_VpsHpsWpsR, xmm_minps_mask)

#define EVEX_OP_PACKED_DOUBLE(HANDLER, func)                                                \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)               \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());                                       \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status;                                                                  \
    mxcsr_to_softfloat_status_word(status, MXCSR);                                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)                       \
      (func)(&op1.vmm128(n), &op2.vmm128(n), status, tmp_mask);                             \
                                                                                            \
    check_exceptionsSSE(status.float_exception_flags);                                      \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, mask >>= 2)                                          \
        xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);              \
                                                                                            \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                                \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_PACKED_DOUBLE(VADDPD_MASK_VpdHpdWpdR, xmm_addpd_mask)
EVEX_OP_PACKED_DOUBLE(VSUBPD_MASK_VpdHpdWpdR, xmm_subpd_mask)
EVEX_OP_PACKED_DOUBLE(VMULPD_MASK_VpdHpdWpdR, xmm_mulpd_mask)
EVEX_OP_PACKED_DOUBLE(VDIVPD_MASK_VpdHpdWpdR, xmm_divpd_mask)
EVEX_OP_PACKED_DOUBLE(VMAXPD_MASK_VpdHpdWpdR, xmm_maxpd_mask)
EVEX_OP_PACKED_DOUBLE(VMINPD_MASK_VpdHpdWpdR, xmm_minpd_mask)

#define EVEX_OP_SCALAR_SINGLE(HANDLER, func)                                                \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)               \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
                                                                                            \
    if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {                                              \
      float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                                    \
                                                                                            \
      float_status_t status;                                                                \
      mxcsr_to_softfloat_status_word(status, MXCSR);                                        \
      softfloat_status_word_rc_override(status, i);                                         \
      op1.xmm32u(0) = (func)(op1.xmm32u(0), op2, status);                                   \
      check_exceptionsSSE(status.float_exception_flags);                                    \
    }                                                                                       \
    else {                                                                                  \
      if (i->isZeroMasking())                                                               \
        op1.xmm32u(0) = 0;                                                                  \
      else                                                                                  \
        op1.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());                                 \
    }                                                                                       \
                                                                                            \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_SCALAR_SINGLE(VADDSS_MASK_VssHpsWssR, float32_add)
EVEX_OP_SCALAR_SINGLE(VSUBSS_MASK_VssHpsWssR, float32_sub)
EVEX_OP_SCALAR_SINGLE(VMULSS_MASK_VssHpsWssR, float32_mul)
EVEX_OP_SCALAR_SINGLE(VDIVSS_MASK_VssHpsWssR, float32_div)
EVEX_OP_SCALAR_SINGLE(VMINSS_MASK_VssHpsWssR, float32_min)
EVEX_OP_SCALAR_SINGLE(VMAXSS_MASK_VssHpsWssR, float32_max)

#define EVEX_OP_SCALAR_DOUBLE(HANDLER, func)                                                \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)               \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
                                                                                            \
    if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {                                              \
      float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                                    \
                                                                                            \
      float_status_t status;                                                                \
      mxcsr_to_softfloat_status_word(status, MXCSR);                                        \
      softfloat_status_word_rc_override(status, i);                                         \
      op1.xmm64u(0) = (func)(op1.xmm64u(0), op2, status);                                   \
      check_exceptionsSSE(status.float_exception_flags);                                    \
    }                                                                                       \
    else {                                                                                  \
      if (i->isZeroMasking())                                                               \
        op1.xmm64u(0) = 0;                                                                  \
      else                                                                                  \
        op1.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->dst());                                 \
    }                                                                                       \
                                                                                            \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_SCALAR_DOUBLE(VADDSD_MASK_VsdHpdWsdR, float64_add)
EVEX_OP_SCALAR_DOUBLE(VSUBSD_MASK_VsdHpdWsdR, float64_sub)
EVEX_OP_SCALAR_DOUBLE(VMULSD_MASK_VsdHpdWsdR, float64_mul)
EVEX_OP_SCALAR_DOUBLE(VDIVSD_MASK_VsdHpdWsdR, float64_div)
EVEX_OP_SCALAR_DOUBLE(VMINSD_MASK_VsdHpdWsdR, float64_min)
EVEX_OP_SCALAR_DOUBLE(VMAXSD_MASK_VsdHpdWsdR, float64_max)

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned mask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)
    xmm_sqrtps_mask(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(status.float_exception_flags);

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), mask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)
    xmm_sqrtpd_mask(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(status.float_exception_flags);

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), mask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSS_MASK_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_sqrt(op2, status);
    check_exceptionsSSE(status.float_exception_flags);
  }
  else {
    if (i->isZeroMasking())
      op1.xmm32u(0) = 0;
    else
      op1.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSD_MASK_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_sqrt(op2, status);
    check_exceptionsSSE(status.float_exception_flags);
  }
  else {
    if (i->isZeroMasking())
      op1.xmm64u(0) = 0;
    else
      op1.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

extern float32_compare_method avx_compare32[32];
extern float64_compare_method avx_compare64[32];

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPS_MASK_KGwHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = 4 * i->getVL();

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (avx_compare32[ib](op1.vmm32u(n), op2.vmm32u(n), status)) result |= mask;
    }
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_OPMASK(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPD_MASK_KGbHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = 2 * i->getVL();

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (avx_compare64[ib](op1.vmm64u(n), op2.vmm64u(n), status)) result |= mask;
    }
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_OPMASK(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSD_MASK_KGbHsdWsdIbR(bxInstruction_c *i)
{
  Bit32u result = 0;

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    if (avx_compare64[i->Ib() & 0x1F](op1, op2, status)) result = 1;
    check_exceptionsSSE(status.float_exception_flags);
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSS_MASK_KGbHssWssIbR(bxInstruction_c *i)
{
  Bit32u result = 0;

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    if (avx_compare32[i->Ib() & 0x1F](op1, op2, status)) result = 1;
    check_exceptionsSSE(status.float_exception_flags);
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

#endif
