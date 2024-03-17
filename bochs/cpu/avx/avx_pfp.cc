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
extern void mxcsr_to_softfloat_status_word_imm_override(float_status_t &status, Bit8u immb);

extern float32 approximate_rsqrt(float32 op);
extern float32 approximate_rcp(float32 op);

#include "softfloat3e/include/softfloat-compare.h"
#include "simd_pfp.h"
#include "simd_int.h"

void BX_CPU_C::print_state_AVX(void)
{
  BX_DEBUG(("MXCSR: 0x%08x", BX_MXCSR_REGISTER));
  for(int n=0;n<BX_XMM_REGISTERS;n++) {
#if BX_SUPPORT_EVEX
    BxPackedZmmRegister vmm = BX_READ_AVX_REG(n);
    BX_DEBUG(("VMM%02u: %08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x:%08x%08x", n,
       vmm.zmm32u(15), vmm.zmm32u(14), vmm.zmm32u(13), vmm.zmm32u(12),
       vmm.zmm32u(11), vmm.zmm32u(10), vmm.zmm32u(9),  vmm.zmm32u(8),
       vmm.zmm32u(7),  vmm.zmm32u(6),  vmm.zmm32u(5),  vmm.zmm32u(4),
       vmm.zmm32u(3),  vmm.zmm32u(2),  vmm.zmm32u(1),  vmm.zmm32u(0)));
#else
    BxPackedYmmRegister vmm = BX_READ_YMM_REG(n);
    BX_DEBUG(("VMM%02u: %08x%08x:%08x%08x:%08x%08x:%08x%08x", n,
       vmm.ymm32u(7), vmm.ymm32u(6), vmm.ymm32u(5), vmm.ymm32u(4),
       vmm.ymm32u(3), vmm.ymm32u(2), vmm.ymm32u(1), vmm.ymm32u(0)));
#endif
  }
}

/* Comparison predicate for VCMPSS/VCMPPS instructions */
float32_compare_method avx_compare32[32] = {
  f32_eq_ordered_quiet,
  f32_lt_ordered_signalling,
  f32_le_ordered_signalling,
  f32_unordered_quiet,
  f32_neq_unordered_quiet,
  f32_nlt_unordered_signalling,
  f32_nle_unordered_signalling,
  f32_ordered_quiet,
  f32_eq_unordered_quiet,
  f32_nge_unordered_signalling,
  f32_ngt_unordered_signalling,
  f32_false_quiet,
  f32_neq_ordered_quiet,
  f32_ge_ordered_signalling,
  f32_gt_ordered_signalling,
  f32_true_quiet,
  f32_eq_ordered_signalling,
  f32_lt_ordered_quiet,
  f32_le_ordered_quiet,
  f32_unordered_signalling,
  f32_neq_unordered_signalling,
  f32_nlt_unordered_quiet,
  f32_nle_unordered_quiet,
  f32_ordered_signalling,
  f32_eq_unordered_signalling,
  f32_nge_unordered_quiet,
  f32_ngt_unordered_quiet,
  f32_false_signalling,
  f32_neq_ordered_signalling,
  f32_ge_ordered_quiet,
  f32_gt_ordered_quiet,
  f32_true_signalling
};

/* Comparison predicate for VCMPSD/VCMPPD instructions */
float64_compare_method avx_compare64[32] = {
  f64_eq_ordered_quiet,
  f64_lt_ordered_signalling,
  f64_le_ordered_signalling,
  f64_unordered_quiet,
  f64_neq_unordered_quiet,
  f64_nlt_unordered_signalling,
  f64_nle_unordered_signalling,
  f64_ordered_quiet,
  f64_eq_unordered_quiet,
  f64_nge_unordered_signalling,
  f64_ngt_unordered_signalling,
  f64_false_quiet,
  f64_neq_ordered_quiet,
  f64_ge_ordered_signalling,
  f64_gt_ordered_signalling,
  f64_true_quiet,
  f64_eq_ordered_signalling,
  f64_lt_ordered_quiet,
  f64_le_ordered_quiet,
  f64_unordered_signalling,
  f64_neq_unordered_signalling,
  f64_nlt_unordered_quiet,
  f64_nle_unordered_quiet,
  f64_ordered_signalling,
  f64_eq_unordered_signalling,
  f64_nge_unordered_quiet,
  f64_ngt_unordered_quiet,
  f64_false_signalling,
  f64_neq_ordered_signalling,
  f64_ge_ordered_quiet,
  f64_gt_ordered_quiet,
  f64_true_signalling
};

#if BX_SUPPORT_EVEX
/* Comparison predicate for VCMPSH/VCMPPH instructions */
float16_compare_method avx_compare16[32] = {
  f16_eq_ordered_quiet,
  f16_lt_ordered_signalling,
  f16_le_ordered_signalling,
  f16_unordered_quiet,
  f16_neq_unordered_quiet,
  f16_nlt_unordered_signalling,
  f16_nle_unordered_signalling,
  f16_ordered_quiet,
  f16_eq_unordered_quiet,
  f16_nge_unordered_signalling,
  f16_ngt_unordered_signalling,
  f16_false_quiet,
  f16_neq_ordered_quiet,
  f16_ge_ordered_signalling,
  f16_gt_ordered_signalling,
  f16_true_quiet,
  f16_eq_ordered_signalling,
  f16_lt_ordered_quiet,
  f16_le_ordered_quiet,
  f16_unordered_signalling,
  f16_neq_unordered_signalling,
  f16_nlt_unordered_quiet,
  f16_nle_unordered_quiet,
  f16_ordered_signalling,
  f16_eq_unordered_signalling,
  f16_nge_unordered_quiet,
  f16_ngt_unordered_quiet,
  f16_false_signalling,
  f16_neq_ordered_signalling,
  f16_ge_ordered_quiet,
  f16_gt_ordered_quiet,
  f16_true_signalling
};
#endif

/* Opcode: VEX.NDS.F3.0F 51 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = f32_sqrt(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 51 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = f64_sqrt(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F 52 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    op.ymm32u(n) = approximate_rsqrt(op.ymm32u(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 52 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  op1.xmm32u(0) = approximate_rsqrt(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F 53 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    op.vmm32u(n) = approximate_rcp(op.vmm32u(n));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 53 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  op1.xmm32u(0) = approximate_rcp(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

#define AVX_SCALAR_SINGLE_FP(HANDLER, func)                                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                                      \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    op1.xmm32u(0) = (func)(op1.xmm32u(0), op2, &status);                                    \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_SCALAR_SINGLE_FP(VADDSS_VssHpsWssR, f32_add);
AVX_SCALAR_SINGLE_FP(VSUBSS_VssHpsWssR, f32_sub);
AVX_SCALAR_SINGLE_FP(VMULSS_VssHpsWssR, f32_mul);
AVX_SCALAR_SINGLE_FP(VDIVSS_VssHpsWssR, f32_div);
AVX_SCALAR_SINGLE_FP(VMINSS_VssHpsWssR, f32_min);
AVX_SCALAR_SINGLE_FP(VMAXSS_VssHpsWssR, f32_max);
#if BX_SUPPORT_EVEX
AVX_SCALAR_SINGLE_FP(VSCALEFSS_VssHpsWssR, f32_scalef);
#endif

#define AVX_SCALAR_DOUBLE_FP(HANDLER, func)                                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());                                   \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                                      \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    op1.xmm64u(0) = (func)(op1.xmm64u(0), op2, &status);                                    \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);                                             \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_SCALAR_DOUBLE_FP(VADDSD_VsdHpdWsdR, f64_add);
AVX_SCALAR_DOUBLE_FP(VSUBSD_VsdHpdWsdR, f64_sub);
AVX_SCALAR_DOUBLE_FP(VMULSD_VsdHpdWsdR, f64_mul);
AVX_SCALAR_DOUBLE_FP(VDIVSD_VsdHpdWsdR, f64_div);
AVX_SCALAR_DOUBLE_FP(VMINSD_VsdHpdWsdR, f64_min);
AVX_SCALAR_DOUBLE_FP(VMAXSD_VsdHpdWsdR, f64_max);
#if BX_SUPPORT_EVEX
AVX_SCALAR_DOUBLE_FP(VSCALEFSD_VsdHpdWsdR, f64_scalef);
#endif

/* Opcode: VEX.NDS.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op1.ymm32u(n) = avx_compare32[ib](op1.ymm32u(n), op2.ymm32u(n), &status) ? 0xFFFFFFFF : 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op1.ymm64u(n) = avx_compare64[ib](op1.ymm64u(n), op2.ymm64u(n), &status) ?
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSD_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 0x1F;

  if(avx_compare64[ib](op1.xmm64u(0), op2, &status)) {
    op1.xmm64u(0) = BX_CONST64(0xFFFFFFFFFFFFFFFF);
  } else {
    op1.xmm64u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSS_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 0x1F;

  if(avx_compare32[ib](op1.xmm32u(0), op2, &status)) {
    op1.xmm32u(0) = 0xFFFFFFFF;
  } else {
    op1.xmm32u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38.0E (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->dst()), op2 = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    if ((op2.ymm64u(n) &  op1.ymm64u(n) & BX_CONST64(0x8000000080000000)) != 0)
      result &= ~EFlagsZFMask;

    if ((op2.ymm64u(n) & ~op1.ymm64u(n) & BX_CONST64(0x8000000080000000)) != 0)
      result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38.0F (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPD_VpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->dst()), op2 = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    if ((op2.ymm64u(n) &  op1.ymm64u(n) & BX_CONST64(0x8000000000000000)) != 0)
      result &= ~EFlagsZFMask;

    if ((op2.ymm64u(n) & ~op1.ymm64u(n) & BX_CONST64(0x8000000000000000)) != 0)
      result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.08 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  for(unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.ymm32u(n) = f32_roundToInt(op.ymm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.09 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPD_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  for(unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op.ymm64u(n) = f64_roundToInt(op.ymm64u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.0A (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSS_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op1.xmm32u(0) = f32_roundToInt(op2, &status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.0B (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSD_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op1.xmm64u(0) = f64_roundToInt(op2, &status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.40 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0; n < len; n++) {

    // op1: [A, B, C, D]
    // op2: [E, F, G, H]

    // after multiplication: op1 = [AE, BF, CG, DH]
    xmm_mulps_mask(&op1.ymm128(n), &op2.ymm128(n), status, mask >> 4);

    // shuffle op2 = [BF, AE, DH, CG]
    xmm_shufps(&op2.ymm128(n), &op1.ymm128(n), &op1.ymm128(n), 0xb1);

    // op2 = [(BF+AE), (AE+BF), (DH+CG), (CG+DH)]
    xmm_addps(&op2.ymm128(n), &op1.ymm128(n), status);

    // shuffle op1 = [(DH+CG), (CG+DH), (BF+AE), (AE+BF)]
    xmm_shufpd(&op1.ymm128(n), &op2.ymm128(n), &op2.ymm128(n), 0x1);

    // op2 = [(BF+AE)+(DH+CG), (AE+BF)+(CG+DH), (DH+CG)+(BF+AE), (CG+DH)+(AE+BF)]
    xmm_addps_mask(&op2.ymm128(n), &op1.ymm128(n), status, mask);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op2, len);

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_AVX
