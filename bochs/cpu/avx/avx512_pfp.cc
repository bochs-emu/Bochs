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
EVEX_OP_PACKED_SINGLE(VSCALEFPS_MASK_VpsHpsWpsR, xmm_scalefps_mask)

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
EVEX_OP_PACKED_DOUBLE(VSCALEFPD_MASK_VpdHpdWpdR, xmm_scalefpd_mask)

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
EVEX_OP_SCALAR_SINGLE(VSCALEFSS_MASK_VssHpsWssR, float32_scalef)

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
EVEX_OP_SCALAR_DOUBLE(VSCALEFSD_MASK_VsdHpdWsdR, float64_scalef)

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

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
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

float32 float32_fixupimm(float32 dst, float32 op1, Bit32u op2, unsigned imm8, float_status_t &status)
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

    case float_SNaN:
      token = BX_FIXUPIMM_SNAN_TOKEN;
      ie_fault_mask = 0x10;
      break;

    case float_QNaN:
      token = BX_FIXUPIMM_QNAN_TOKEN;
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
    op1 = dst; break;
  }

  return op1;
}

float64 float64_fixupimm(float64 dst, float64 op1, Bit32u op2, unsigned imm8, float_status_t &status)
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

    case float_SNaN:
      token = BX_FIXUPIMM_SNAN_TOKEN;
      ie_fault_mask = 0x10;
      break;

    case float_QNaN:
      token = BX_FIXUPIMM_QNAN_TOKEN;
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
    op1 = dst; break;
  }

  return op1;
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMSS_MASK_VssHssWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  Bit32u op_dst = BX_READ_XMM_REG_LO_DWORD(i->dst());

  if (i->opmask() == 0 || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    Bit32u op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_fixupimm(op_dst, op1.xmm32u(0), op2, i->Ib(), status);
    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm32u(0) = 0;
    else
      op1.xmm32u(0) = op_dst;
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMSD_MASK_VsdHsdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  Bit64u op_dst = BX_READ_XMM_REG_LO_QWORD(i->dst());

  if (i->opmask() == 0 || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    Bit32u op2 = (Bit32u) BX_READ_XMM_REG_LO_QWORD(i->src2());

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_fixupimm(op_dst, op1.xmm64u(0), op2, i->Ib(), status);
    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm64u(0) = 0;
    else
      op1.xmm64u(0) = op_dst;
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op1.vmm32u(n) = float32_fixupimm(dst.vmm32u(n), op1.vmm32u(n), op2.vmm32u(n), i->Ib(), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm32u(n) = float32_fixupimm(dst.vmm32u(n), op1.vmm32u(n), op2.vmm32u(n), i->Ib(), status);
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
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op1.vmm64u(n) = float64_fixupimm(dst.vmm64u(n), op1.vmm64u(n), (Bit32u) op2.vmm64u(n), i->Ib(), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFIXUPIMMPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm64u(n) = float64_fixupimm(dst.vmm64u(n), op1.vmm64u(n), (Bit32u) op2.vmm64u(n), i->Ib(), status);
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

// fpclass

static int fpclass(float_class_t op_class, int sign, int selector)
{
  return ((op_class == float_QNaN) && (selector & 0x01) != 0) || // QNaN
         ((op_class == float_zero) && ! sign && (selector & 0x02) != 0) || // positive zero
         ((op_class == float_zero) && sign && (selector & 0x04) != 0) || // negative zero
         ((op_class == float_positive_inf) && (selector & 0x08) != 0) || // positive inf
         ((op_class == float_negative_inf) && (selector & 0x10) != 0) || // negative inf
         ((op_class == float_denormal) && (selector & 0x20) != 0) || // negative inf
         ((op_class == float_denormal || op_class == float_normalized) && sign && (selector & 0x40) != 0) || // negative finite
         ((op_class == float_SNaN) && (selector & 0x80) != 0); // SNaN
}

static BX_CPP_INLINE int float32_fpclass(float32 op, int selector, int daz)
{
  if (daz) 
    op = float32_denormal_to_zero(op);

  return fpclass(float32_class(op), float32_sign(op), selector);
}

static BX_CPP_INLINE int float64_fpclass(float64 op, int selector, int daz)
{
  if (daz) 
    op = float64_denormal_to_zero(op);

  return fpclass(float64_class(op), float64_sign(op), selector);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSPS_MASK_KGwWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned num_elements = DWORD_ELEMENTS(i->getVL());

  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;
  int selector = i->Ib(), daz = MXCSR.get_DAZ();

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (float32_fpclass(op.vmm32u(n), selector, daz)) result |= mask;
    }
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSPD_MASK_KGbWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned num_elements = QWORD_ELEMENTS(i->getVL());

  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;
  int selector = i->Ib(), daz = MXCSR.get_DAZ();

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (float64_fpclass(op.vmm64u(n), selector, daz)) result |= mask;
    }
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSSS_MASK_KGbWssIbR(bxInstruction_c *i)
{
  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    BX_WRITE_OPMASK(i->dst(), float32_fpclass(BX_READ_XMM_REG_LO_DWORD(i->src()), i->Ib(), MXCSR.get_DAZ()));
  }
  else {
    BX_WRITE_OPMASK(i->dst(), 0);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSSD_MASK_KGbWsdIbR(bxInstruction_c *i)
{
  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    BX_WRITE_OPMASK(i->dst(), float64_fpclass(BX_READ_XMM_REG_LO_QWORD(i->src()), i->Ib(), MXCSR.get_DAZ()));
  }
  else {
    BX_WRITE_OPMASK(i->dst(), 0);
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

// rndscale

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALEPS_MASK_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  Bit8u control = i->Ib(), scale = control >> 4;

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm32u(n) = float32_round_to_int(op.vmm32u(n), scale, status);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALESS_MASK_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);

    // override MXCSR rounding mode with control coming from imm8
    if ((control & 0x4) == 0)
      status.float_rounding_mode = control & 0x3;
    // ignore precision exception result
    if (control & 0x8)
      status.float_suppress_exception |= float_flag_inexact;

    op1.xmm32u(0) = float32_round_to_int(op2, scale, status);

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALEPD_MASK_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  Bit8u control = i->Ib(), scale = control >> 4;

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm64u(n) = float64_round_to_int(op.vmm64u(n), scale, status);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALESD_MASK_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);

    // override MXCSR rounding mode with control coming from imm8
    if ((control & 0x4) == 0)
      status.float_rounding_mode = control & 0x3;
    // ignore precision exception result
    if (control & 0x8)
      status.float_suppress_exception |= float_flag_inexact;

    op1.xmm64u(0) = float64_round_to_int(op2, scale, status);

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

// scalef

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCALEFPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_scalefps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCALEFPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  
  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_scalefpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCALEFSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_scalef(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSCALEFSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_scalef(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

// range

static BX_CPP_INLINE float32 float32_range(float32 a, float32 b, int opselect, int sign_ctrl, float_status_t &status)
{
  float32 minmax = float32_minmax(a, b, opselect & 0x1, (opselect >> 1) & 0x1, status);

  if (! float32_is_signaling_nan(a) && ! float32_is_signaling_nan(b)) {
    if (sign_ctrl == 0) {
      minmax = (minmax & ~0x80000000) | (a & 0x80000000); // keep sign of a
    }
    else if (sign_ctrl == 2) {
      minmax &= ~0x80000000; // zero out sign it
    }
    else if (sign_ctrl == 3) {
      minmax |=  0x80000000; // set the sign it
    }
    // else preserve the sign of compare result
  }

  return minmax;
}

static BX_CPP_INLINE float64 float64_range(float64 a, float64 b, int opselect, int sign_ctrl, float_status_t &status)
{
  float64 minmax = float64_minmax(a, b, opselect & 0x1, (opselect >> 1) & 0x1, status);

  if (! float64_is_signaling_nan(a) && ! float64_is_signaling_nan(b)) {
    if (sign_ctrl == 0) {
      minmax = (minmax & ~BX_CONST64(0x8000000000000000)) | (a & BX_CONST64(0x8000000000000000)); // keep sign of a
    }
    else if (sign_ctrl == 2) {
      minmax &= ~BX_CONST64(0x8000000000000000); // zero out sign it
    }
    else if (sign_ctrl == 3) {
      minmax |=  BX_CONST64(0x8000000000000000); // set the sign it
    }
    // else preserve the sign of compare result
  }

  return minmax;
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRANGEPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int opselect = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op1.vmm32u(n) = float32_range(op1.vmm32u(n), op2.vmm32u(n), opselect, sign_ctrl, status);
    else
      op1.vmm32u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRANGEPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int opselect = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op1.vmm64u(n) = float64_range(op1.vmm64u(n), op2.vmm64u(n), opselect, sign_ctrl, status);
    else
      op1.vmm64u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op1.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRANGESS_MASK_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    int sign_ctrl = (i->Ib() >> 2) & 0x3;
    int opselect = i->Ib() & 0x3;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = float32_range(op1.xmm32u(0), op2, opselect, sign_ctrl, status);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRANGESD_MASK_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    int sign_ctrl = (i->Ib() >> 2) & 0x3;
    int opselect = i->Ib() & 0x3;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = float64_range(op1.xmm64u(0), op2, opselect, sign_ctrl, status);
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

// reduce

static BX_CPP_INLINE float32 float32_reduce(float32 a, Bit8u scale, float_status_t &status)
{
  if (a == float32_negative_inf || a == float32_positive_inf)
    return 0;

  float32 tmp = float32_round_to_int(a, scale, status);
  return float32_sub(a, tmp, status);
}

static BX_CPP_INLINE float64 float64_reduce(float64 a, Bit8u scale, float_status_t &status)
{
  if (a == float64_negative_inf || a == float64_positive_inf)
    return 0;

  float64 tmp = float64_round_to_int(a, scale, status);
  return float64_sub(a, tmp, status);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCEPS_MASK_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  Bit8u control = i->Ib(), scale = control >> 4;

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm32u(n) = float32_reduce(op.vmm32u(n), scale, status);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCESS_MASK_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);

    // override MXCSR rounding mode with control coming from imm8
    if ((control & 0x4) == 0)
      status.float_rounding_mode = control & 0x3;
    // ignore precision exception result
    if (control & 0x8)
      status.float_suppress_exception |= float_flag_inexact;

    op1.xmm32u(0) = float32_reduce(op2, scale, status);

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCEPD_MASK_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  Bit8u control = i->Ib(), scale = control >> 4;

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm64u(n) = float64_reduce(op.vmm64u(n), scale, status);
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCESD_MASK_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status;
    mxcsr_to_softfloat_status_word(status, MXCSR);
    softfloat_status_word_rc_override(status, i);

    // override MXCSR rounding mode with control coming from imm8
    if ((control & 0x4) == 0)
      status.float_rounding_mode = control & 0x3;
    // ignore precision exception result
    if (control & 0x8)
      status.float_suppress_exception |= float_flag_inexact;

    op1.xmm64u(0) = float64_reduce(op2, scale, status);

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

#endif
