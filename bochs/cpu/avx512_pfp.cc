/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2014 Stanislav Shwartsman
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
    check_exceptionsSSE(get_exception_flags(status));                                       \
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
    check_exceptionsSSE(get_exception_flags(status));                                       \
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
      check_exceptionsSSE(get_exception_flags(status));                                     \
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
      check_exceptionsSSE(get_exception_flags(status));                                     \
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

  check_exceptionsSSE(get_exception_flags(status));

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

  check_exceptionsSSE(get_exception_flags(status));

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
    check_exceptionsSSE(get_exception_flags(status));
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
    check_exceptionsSSE(get_exception_flags(status));
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

// compare

extern float32_compare_method avx_compare32[32];
extern float64_compare_method avx_compare64[32];

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPS_MASK_KGwHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = DWORD_ELEMENTS(i->getVL());

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

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_OPMASK(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPD_MASK_KGbHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = QWORD_ELEMENTS(i->getVL());

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

  check_exceptionsSSE(get_exception_flags(status));
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
    check_exceptionsSSE(get_exception_flags(status));
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
    check_exceptionsSSE(get_exception_flags(status));
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

// fixup

enum {
  BX_FIXUPIMM_QNAN_TOKEN = 0,
  BX_FIXUPIMM_SNAN_TOKEN = 1,
  BX_FIXUPIMM_ZERO_VALUE_TOKEN = 2,
  BX_FIXUPIMM_POS_ONE_VALUE_TOKEN = 3,
  BX_FIXUPIMM_NEG_INF_TOKEN = 4,
  BX_FIXUPIMM_POS_INF_TOKEN = 5,
  BX_FIXUPIMM_NEG_VALUE_TOKEN = 6,
  BX_FIXUPIMM_POS_VALUE_TOKEN = 7
};

#include "fpu/softfloat-specialize.h"

const float32 float32_value_90      = 0x42b40000;
const float32 float32_pi_half       = 0x3fc90fdb;
const float32 float32_positive_half = 0x3f000000;

const float64 float64_value_90      = BX_CONST64(0x4056800000000000);
const float64 float64_pi_half       = BX_CONST64(0x3ff921fb54442d18);
const float64 float64_positive_half = BX_CONST64(0x3fe0000000000000);

float32 float32_fixupimm(float32 op1, Bit32u op2, unsigned imm8, float_status_t &status)
{
  float32 tmp_op1 = op1;
  if (get_denormals_are_zeros(status))
    tmp_op1 = float32_denormal_to_zero(op1);

  float_class_t op1_class = float32_class(tmp_op1);
  int sign = float32_sign(tmp_op1);
  unsigned token = 0, ie_fault_mask = 0, divz_fault_mask = 0;

  switch(op1_class)
  {
    case float_zero:
      token = BX_FIXUPIMM_ZERO_VALUE_TOKEN;
      divz_fault_mask = 0x01;
        ie_fault_mask = 0x02;
      break;

    case float_negative_inf:
      token = BX_FIXUPIMM_NEG_INF_TOKEN;
      ie_fault_mask = 0x20;
      break;

    case float_positive_inf:
      token = BX_FIXUPIMM_POS_INF_TOKEN;
      ie_fault_mask = 0x80;
      break;

    case float_NaN:
      if (float32_is_signaling_nan(tmp_op1)) {
        token = BX_FIXUPIMM_SNAN_TOKEN;
        ie_fault_mask = 0x10;
      }
      else {
        token = BX_FIXUPIMM_QNAN_TOKEN;
      }
      break;

    case float_denormal:
    case float_normalized:
      if (tmp_op1 == float32_positive_one) {
        token = BX_FIXUPIMM_POS_ONE_VALUE_TOKEN;
        divz_fault_mask = 0x04;
          ie_fault_mask = 0x08;
      }
      else {
        if (sign) {
          token = BX_FIXUPIMM_NEG_VALUE_TOKEN;
          ie_fault_mask = 0x40;
        }
        else {
          token = BX_FIXUPIMM_POS_VALUE_TOKEN;
        }
      }
      break;

    default:
        break;
  }

  if (imm8 & ie_fault_mask)
    float_raise(status, float_flag_invalid);

  if (imm8 & divz_fault_mask)
    float_raise(status, float_flag_divbyzero);

  // access response table, each response is encoded with 4-bit value in the op2
  unsigned token_response = (op2 >> (token*4)) & 0xf;

  switch(token_response) {
  case 0x1: // apply DAZ to the op1 value
    op1 = tmp_op1;
    break;
  case 0x2: op1 = convert_to_QNaN(tmp_op1); break;
  case 0x3: op1 = float32_default_nan; break;
  case 0x4: op1 = float32_negative_inf; break;
  case 0x5: op1 = float32_positive_inf; break;
  case 0x6:
    op1 = sign ? float32_negative_inf : float32_positive_inf;
    break;
  case 0x7: op1 = float32_negative_zero; break;
  case 0x8: op1 = float32_positive_zero; break;
  case 0x9: op1 = float32_negative_one; break;
  case 0xA: op1 = float32_positive_one; break;
  case 0xB: op1 = float32_positive_half; break;
  case 0xC: op1 = float32_value_90; break;
  case 0xD: op1 = float32_pi_half; break;
  case 0xE: op1 = float32_max_float; break;
  case 0xF: op1 = float32_min_float; break;
  default: // preserve the op1 value
    break;
  }

  return op1;
}

float64 float64_fixupimm(float64 op1, Bit32u op2, unsigned imm8, float_status_t &status)
{
  float64 tmp_op1 = op1;
  if (get_denormals_are_zeros(status))
    tmp_op1 = float64_denormal_to_zero(op1);

  float_class_t op1_class = float64_class(tmp_op1);
  int sign = float64_sign(tmp_op1);
  unsigned token = 0, ie_fault_mask = 0, divz_fault_mask = 0;

  switch(op1_class)
  {
    case float_zero:
      token = BX_FIXUPIMM_ZERO_VALUE_TOKEN;
      divz_fault_mask = 0x01;
        ie_fault_mask = 0x02;
      break;

    case float_negative_inf:
      token = BX_FIXUPIMM_NEG_INF_TOKEN;
      ie_fault_mask = 0x20;
      break;

    case float_positive_inf:
      token = BX_FIXUPIMM_POS_INF_TOKEN;
      ie_fault_mask = 0x80;
      break;

    case float_NaN:
      if (float64_is_signaling_nan(tmp_op1)) {
        token = BX_FIXUPIMM_SNAN_TOKEN;
        ie_fault_mask = 0x10;
      }
      else {
        token = BX_FIXUPIMM_QNAN_TOKEN;
      }
      break;

    case float_denormal:
    case float_normalized:
      if (tmp_op1 == float64_positive_one) {
        token = BX_FIXUPIMM_POS_ONE_VALUE_TOKEN;
        divz_fault_mask = 0x04;
          ie_fault_mask = 0x08;
      }
      else {
        if (sign) {
          token = BX_FIXUPIMM_NEG_VALUE_TOKEN;
          ie_fault_mask = 0x40;
        }
        else {
          token = BX_FIXUPIMM_POS_VALUE_TOKEN;
        }
      }
      break;

    default:
        break;
  }

  if (imm8 & ie_fault_mask)
    float_raise(status, float_flag_invalid);

  if (imm8 & divz_fault_mask)
    float_raise(status, float_flag_divbyzero);

  // access response table, each response is encoded with 4-bit value in the op2
  unsigned token_response = (op2 >> (token*4)) & 0xf;

  switch(token_response) {
  case 0x1: // apply DAZ to the op1 value
    op1 = tmp_op1;
    break;
  case 0x2: op1 = convert_to_QNaN(tmp_op1); break;
  case 0x3: op1 = float64_default_nan; break;
  case 0x4: op1 = float64_negative_inf; break;
  case 0x5: op1 = float64_positive_inf; break;
  case 0x6:
    op1 = sign ? float64_negative_inf : float64_positive_inf;
    break;
  case 0x7: op1 = float64_negative_zero; break;
  case 0x8: op1 = float64_positive_zero; break;
  case 0x9: op1 = float64_negative_one; break;
  case 0xA: op1 = float64_positive_one; break;
  case 0xB: op1 = float64_positive_half; break;
  case 0xC: op1 = float64_value_90; break;
  case 0xD: op1 = float64_pi_half; break;
  case 0xE: op1 = float64_max_float; break;
  case 0xF: op1 = float64_min_float; break;
  default: // preserve the op1 value
    break;
  }

  return op1;
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMSS_MASK_VssHssWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (i->opmask() == 0 || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    Bit32u op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_fixupimm(op1.xmm32u(0), op2, i->Ib(), status);
    check_exceptionsSSE(get_exception_flags(status));
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMSD_MASK_VsdHsdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (i->opmask() == 0 || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    Bit32u op2 = (Bit32u) BX_READ_XMM_REG_LO_QWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_fixupimm(op1.xmm64u(0), op2, i->Ib(), status);
    check_exceptionsSSE(get_exception_flags(status));
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op1.vmm32u(n) = float32_fixupimm(op1.vmm32u(n), op2.vmm32u(n), i->Ib(), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit32u mask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm32u(n) = float32_fixupimm(op1.vmm32u(n), op2.vmm32u(n), i->Ib(), status);
    else
      op1.vmm32u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op1.vmm64u(n) = float64_fixupimm(op1.vmm64u(n), (Bit32u) op2.vmm64u(n), i->Ib(), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit32u mask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm64u(n) = float64_fixupimm(op1.vmm64u(n), (Bit32u) op2.vmm64u(n), i->Ib(), status);
    else
      op1.vmm64u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, mask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), mask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

// getexp

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPPS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 4)
    xmm_getexpps_mask(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPPD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u mask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < len; n++, tmp_mask >>= 2)
    xmm_getexppd_mask(&op.vmm128(n), status, tmp_mask);

  check_exceptionsSSE(get_exception_flags(status));

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPSS_MASK_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_getexp(op2, status);
    check_exceptionsSSE(get_exception_flags(status));
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPSD_MASK_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_getexp(op2, status);
    check_exceptionsSSE(get_exception_flags(status));
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

// getmant

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTSS_MASK_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    int sign_ctrl = (i->Ib() >> 2) & 0x3;
    int interv = i->Ib() & 0x3;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_getmant(op2, status, sign_ctrl, interv);
    check_exceptionsSSE(get_exception_flags(status));
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTSD_MASK_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    int sign_ctrl = (i->Ib() >> 2) & 0x3;
    int interv = i->Ib() & 0x3;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_getmant(op2, status, sign_ctrl, interv);
    check_exceptionsSSE(get_exception_flags(status));
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTPS_MASK_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int interv = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm32u(n) = float32_getmant(op.vmm32u(n), status, sign_ctrl, interv);
    else
      op.vmm32u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTPD_MASK_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int interv = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm64u(n) = float64_getmant(op.vmm64u(n), status, sign_ctrl, interv);
    else
      op.vmm64u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

#endif
