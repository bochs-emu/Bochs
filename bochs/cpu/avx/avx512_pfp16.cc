/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2023 Stanislav Shwartsman
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

extern float_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);
extern void mxcsr_to_softfloat_status_word_imm_override(float_status_t &status, Bit8u immb);

#include "softfloat3e/include/softfloat-compare.h"
#include "fpu/softfloat16.h"
#include "simd_int.h"
#include "simd_pfp.h"

#define EVEX_OP_SCALAR_HALF(HANDLER, func)                                                  \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
                                                                                            \
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());                                       \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
    op1.xmm16u(0) = (func)(op1.xmm16u(0), op2, status);                                     \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_SCALAR_HALF(VADDSH_VshHphWshR, float16_add)
EVEX_OP_SCALAR_HALF(VSUBSH_VshHphWshR, float16_sub)
EVEX_OP_SCALAR_HALF(VMULSH_VshHphWshR, float16_mul)
EVEX_OP_SCALAR_HALF(VDIVSH_VshHphWshR, float16_div)
EVEX_OP_SCALAR_HALF(VMINSH_VshHphWshR, float16_min)
EVEX_OP_SCALAR_HALF(VMAXSH_VshHphWshR, float16_max)
EVEX_OP_SCALAR_HALF(VSCALEFSH_VshHphWshR, float16_scalef)

#define EVEX_OP_SCALAR_HALF_MASK(HANDLER, func)                                             \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
                                                                                            \
    if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {                                              \
      float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());                                     \
      float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                        \
      softfloat_status_word_rc_override(status, i);                                         \
      op1.xmm16u(0) = (func)(op1.xmm16u(0), op2, status);                                   \
      check_exceptionsSSE(get_exception_flags(status));                                     \
    }                                                                                       \
    else {                                                                                  \
      if (i->isZeroMasking())                                                               \
        op1.xmm16u(0) = 0;                                                                  \
      else                                                                                  \
        op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());                                  \
    }                                                                                       \
                                                                                            \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
    BX_NEXT_INSTR(i);                                                                       \
  }

EVEX_OP_SCALAR_HALF_MASK(VADDSH_MASK_VshHphWshR, float16_add)
EVEX_OP_SCALAR_HALF_MASK(VSUBSH_MASK_VshHphWshR, float16_sub)
EVEX_OP_SCALAR_HALF_MASK(VMULSH_MASK_VshHphWshR, float16_mul)
EVEX_OP_SCALAR_HALF_MASK(VDIVSH_MASK_VshHphWshR, float16_div)
EVEX_OP_SCALAR_HALF_MASK(VMINSH_MASK_VshHphWshR, float16_min)
EVEX_OP_SCALAR_HALF_MASK(VMAXSH_MASK_VshHphWshR, float16_max)
EVEX_OP_SCALAR_HALF_MASK(VSCALEFSH_MASK_VshHphWshR, float16_scalef)

extern float16_compare_method avx_compare16[32];

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSH_MASK_KGbHshWshIbR(bxInstruction_c *i)
{
  Bit32u result = 0;

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op1 = BX_READ_XMM_REG_LO_WORD(i->src1());
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    if (avx_compare16[i->Ib() & 0x1F](op1, op2, &status)) result = 1;
    check_exceptionsSSE(get_exception_flags(status));
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPH_MASK_KGdHphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned num_elements = WORD_ELEMENTS(i->getVL());

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (avx_compare16[ib](op1.vmm16u(n), op2.vmm16u(n), &status)) result |= mask;
    }
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_OPMASK(i->dst(), result);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::UCOMISH_VshWshR(bxInstruction_c *i)
{
  float16 op1 = BX_READ_XMM_REG_LO_WORD(i->dst()), op2 = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f16_compare_quiet(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::COMISH_VshWshR(bxInstruction_c *i)
{
  float16 op1 = BX_READ_XMM_REG_LO_WORD(i->dst()), op2 = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f16_compare(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);

  BX_NEXT_INSTR(i);
}

#include "fpu/softfloat-specialize.h"

static BX_CPP_INLINE int f16_fpclass(float16 op, int selector, int daz)
{
  extern int fpclass(softfloat_class_t op_class, int sign, int selector);

  if (daz)
    op = f16_denormal_to_zero(op);

  return fpclass(f16_class(op), f16_sign(op), selector);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSPH_MASK_KGdWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned num_elements = WORD_ELEMENTS(i->getVL());

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  Bit32u result = 0;
  int selector = i->Ib(), daz = MXCSR.get_DAZ();

  for (unsigned n=0, mask = 0x1; n < num_elements; n++, mask <<= 1) {
    if (opmask & mask) {
      if (f16_fpclass(op.vmm16u(n), selector, daz)) result |= mask;
    }
  }

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFPCLASSSH_MASK_KGbWshIbR(bxInstruction_c *i)
{
  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    BX_WRITE_OPMASK(i->dst(), f16_fpclass(BX_READ_XMM_REG_LO_WORD(i->src()), i->Ib(), MXCSR.get_DAZ()));
  }
  else {
    BX_WRITE_OPMASK(i->dst(), 0);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSH_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = f16_sqrt(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm16u(0) = f16_sqrt(op2, &status);
    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPSH_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = f16_getExp(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETEXPSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm16u(0) = f16_getExp(op2, &status);
    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTSH_MASK_VshHphWshIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    int sign_ctrl = (i->Ib() >> 2) & 0x3;
    int interv = i->Ib() & 0x3;

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm16u(0) = float16_getmant(op2, status, sign_ctrl, interv);
    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VGETMANTPH_MASK_VphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  int sign_ctrl = (i->Ib() >> 2) & 0x3;
  int interv = i->Ib() & 0x3;

  for (unsigned n=0, mask = 0x1; n < WORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm16u(n) = float16_getmant(op.vmm16u(n), status, sign_ctrl, interv);
    else
      op.vmm16u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

static BX_CPP_INLINE float16 float16_reduce(float16 a, Bit8u scale, float_status_t &status)
{
  if (a == float16_negative_inf || a == float16_positive_inf)
    return 0;

  float16 tmp = float16_round_to_int(a, scale, status);
  return f16_sub(a, tmp, &status);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCEPH_MASK_VphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  Bit8u control = i->Ib(), scale = control >> 4;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  mxcsr_to_softfloat_status_word_imm_override(status, control);

  for (unsigned n=0, mask = 0x1; n < WORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm16u(n) = float16_reduce(op.vmm16u(n), scale, status);
    else
      op.vmm16u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VREDUCESH_MASK_VshHphWshIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    mxcsr_to_softfloat_status_word_imm_override(status, control);

    op1.xmm16u(0) = float16_reduce(op2, scale, status);

    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALEPH_MASK_VphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  Bit8u control = i->Ib(), scale = control >> 4;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  mxcsr_to_softfloat_status_word_imm_override(status, control);

  for (unsigned n=0, mask = 0x1; n < WORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      op.vmm16u(n) = float16_round_to_int(op.vmm16u(n), scale, status);
    else
      op.vmm16u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (! i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRNDSCALESH_MASK_VshHphWshIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    Bit8u control = i->Ib(), scale = control >> 4;

    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    mxcsr_to_softfloat_status_word_imm_override(status, control);

    op1.xmm16u(0) = float16_round_to_int(op2, scale, status);

    check_exceptionsSSE(get_exception_flags(status));
  }
  else {
    if (i->isZeroMasking())
      op1.xmm16u(0) = 0;
    else
      op1.xmm16u(0) = BX_READ_XMM_REG_LO_WORD(i->dst());
  }

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

#include "cpu/decoder/ia_opcodes.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFCMULCSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  if (i->dst() == i->src1() || i->dst() == i->src2()) {
    BX_DEBUG(("%s: dst register must be different from sources", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedXmmRegister *op1 = &BX_READ_XMM_REG(i->src1()), *op2 = &BX_READ_XMM_REG(i->src2()), dst = BX_READ_XMM_REG(i->src1());

  dst.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  status.float_exception_masks = float_all_exceptions_mask;

  int fma_control1 = (i->getIaOpcode() == BX_IA_V512_VFMULCSH_VshHphWsh_Kmask) ? softfloat_muladd_negate_product : 0;
  int fma_control2 = (i->getIaOpcode() == BX_IA_V512_VFMULCSH_VshHphWsh_Kmask) ? 0 : softfloat_muladd_negate_product;

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    dst.xmm16u(0) = f16_mul(op1->xmm16u(0), op2->xmm16u(0), &status);
    dst.xmm16u(1) = f16_mul(op1->xmm16u(1), op2->xmm16u(0), &status);
    dst.xmm16u(0) = f16_mulAdd(op1->xmm16u(1), op2->xmm16u(1), dst.xmm16u(0), fma_control1, &status);
    dst.xmm16u(1) = f16_mulAdd(op1->xmm16u(0), op2->xmm16u(1), dst.xmm16u(1), fma_control2, &status);
  }
  else if (i->isZeroMasking()) {
    dst.xmm32u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFCMULCPH_MASK_VphHphWphR(bxInstruction_c *i)
{
  if (i->dst() == i->src1() || i->dst() == i->src2()) {
    BX_DEBUG(("%s: dst register must be different from sources", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *op1 = &BX_READ_AVX_REG(i->src1()), *op2 = &BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL(), n;
  Bit32u mask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff, tmp_mask;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  status.float_exception_masks = float_all_exceptions_mask;

  for (n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm16u(2*n)   = f16_mul(op1->vmm16u(2*n),   op2->vmm16u(2*n), &status);
      dst.vmm16u(2*n+1) = f16_mul(op1->vmm16u(2*n+1), op2->vmm16u(2*n), &status);
    }
  }

  int fma_control1 = (i->getIaOpcode() == BX_IA_V512_VFMULCPH_VphHphWph_Kmask) ? softfloat_muladd_negate_product : 0;
  int fma_control2 = (i->getIaOpcode() == BX_IA_V512_VFMULCPH_VphHphWph_Kmask) ? 0 : softfloat_muladd_negate_product;

  for (n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm16u(2*n)   = f16_mulAdd(op1->vmm16u(2*n+1), op2->vmm16u(2*n+1), dst.vmm16u(2*n),   fma_control1, &status);
      dst.vmm16u(2*n+1) = f16_mulAdd(op1->vmm16u(2*n),   op2->vmm16u(2*n+1), dst.vmm16u(2*n+1), fma_control2, &status);
    }
    else if (i->isZeroMasking()) {
      dst.vmm32u(n) = 0;
    }
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFCMADDCSH_MASK_VshHphWshR(bxInstruction_c *i)
{
  if (i->dst() == i->src1() || i->dst() == i->src2()) {
    BX_DEBUG(("%s: dst register must be different from sources", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedXmmRegister *op1 = &BX_READ_XMM_REG(i->src1()), *op2 = &BX_READ_XMM_REG(i->src2()), dst = BX_READ_XMM_REG(i->src1());

  dst.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->dst());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  status.float_exception_masks = float_all_exceptions_mask;

  int fma_control1 = (i->getIaOpcode() == BX_IA_V512_VFMADDCSH_VshHphWsh_Kmask) ? softfloat_muladd_negate_product : 0;
  int fma_control2 = (i->getIaOpcode() == BX_IA_V512_VFMADDCSH_VshHphWsh_Kmask) ? 0 : softfloat_muladd_negate_product;

  if (! i->opmask() || BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    dst.xmm16u(0) = f16_mulAdd(op1->xmm16u(0), op2->xmm16u(0), dst.xmm16u(0), 0, &status);
    dst.xmm16u(1) = f16_mulAdd(op1->xmm16u(1), op2->xmm16u(0), dst.xmm16u(1), 0, &status);
    dst.xmm16u(0) = f16_mulAdd(op1->xmm16u(1), op2->xmm16u(1), dst.xmm16u(0), fma_control1, &status);
    dst.xmm16u(1) = f16_mulAdd(op1->xmm16u(0), op2->xmm16u(1), dst.xmm16u(1), fma_control2, &status);
  }
  else if (i->isZeroMasking()) {
    dst.xmm32u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dst);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VFCMADDCPH_MASK_VphHphWphR(bxInstruction_c *i)
{
  if (i->dst() == i->src1() || i->dst() == i->src2()) {
    BX_DEBUG(("%s: dst register must be different from sources", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister *op1 = &BX_READ_AVX_REG(i->src1()), *op2 = &BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL(), n;
  Bit32u mask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff, tmp_mask;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  status.float_exception_masks = float_all_exceptions_mask;

  for (n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm16u(2*n)   = f16_mulAdd(op1->vmm16u(2*n),   op2->vmm16u(2*n), dst.vmm16u(2*n),   0, &status);
      dst.vmm16u(2*n+1) = f16_mulAdd(op1->vmm16u(2*n+1), op2->vmm16u(2*n), dst.vmm16u(2*n+1), 0, &status);
    }
  }

  int fma_control1 = (i->getIaOpcode() == BX_IA_V512_VFMADDCPH_VphHphWph_Kmask) ? softfloat_muladd_negate_product : 0;
  int fma_control2 = (i->getIaOpcode() == BX_IA_V512_VFMADDCPH_VphHphWph_Kmask) ? 0 : softfloat_muladd_negate_product;

  for (n=0, tmp_mask = mask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1) {
      dst.vmm16u(2*n)   = f16_mulAdd(op1->vmm16u(2*n+1), op2->vmm16u(2*n+1), dst.vmm16u(2*n),   fma_control1, &status);
      dst.vmm16u(2*n+1) = f16_mulAdd(op1->vmm16u(2*n),   op2->vmm16u(2*n+1), dst.vmm16u(2*n+1), fma_control2, &status);
    }
    else if (i->isZeroMasking()) {
      dst.vmm32u(n) = 0;
    }
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

#endif
