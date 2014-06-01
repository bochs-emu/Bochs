/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2014 Stanislav Shwartsman
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

extern float32 approximate_rsqrt(float32 op);
extern float32 approximate_rcp(float32 op);

#include "fpu/softfloat-compare.h"
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
  float32_eq_ordered_quiet,
  float32_lt_ordered_signalling,
  float32_le_ordered_signalling,
  float32_unordered_quiet,
  float32_neq_unordered_quiet,
  float32_nlt_unordered_signalling,
  float32_nle_unordered_signalling,
  float32_ordered_quiet,
  float32_eq_unordered_quiet,
  float32_nge_unordered_signalling,
  float32_ngt_unordered_signalling,
  float32_false_quiet,
  float32_neq_ordered_quiet,
  float32_ge_ordered_signalling,
  float32_gt_ordered_signalling,
  float32_true_quiet,
  float32_eq_ordered_signalling,
  float32_lt_ordered_quiet,
  float32_le_ordered_quiet,
  float32_unordered_signalling,
  float32_neq_unordered_signalling,
  float32_nlt_unordered_quiet,
  float32_nle_unordered_quiet,
  float32_ordered_signalling,
  float32_eq_unordered_signalling,
  float32_nge_unordered_quiet,
  float32_ngt_unordered_quiet,
  float32_false_signalling,
  float32_neq_ordered_signalling,
  float32_ge_ordered_quiet,
  float32_gt_ordered_quiet,
  float32_true_signalling
};

/* Comparison predicate for VCMPSD/VCMPPD instructions */
float64_compare_method avx_compare64[32] = {
  float64_eq_ordered_quiet,
  float64_lt_ordered_signalling,
  float64_le_ordered_signalling,
  float64_unordered_quiet,
  float64_neq_unordered_quiet,
  float64_nlt_unordered_signalling,
  float64_nle_unordered_signalling,
  float64_ordered_quiet,
  float64_eq_unordered_quiet,
  float64_nge_unordered_signalling,
  float64_ngt_unordered_signalling,
  float64_false_quiet,
  float64_neq_ordered_quiet,
  float64_ge_ordered_signalling,
  float64_gt_ordered_signalling,
  float64_true_quiet,
  float64_eq_ordered_signalling,
  float64_lt_ordered_quiet,
  float64_le_ordered_quiet,
  float64_unordered_signalling,
  float64_neq_unordered_signalling,
  float64_nlt_unordered_quiet,
  float64_nle_unordered_quiet,
  float64_ordered_signalling,
  float64_eq_unordered_signalling,
  float64_nge_unordered_quiet,
  float64_ngt_unordered_quiet,
  float64_false_signalling,
  float64_neq_ordered_signalling,
  float64_ge_ordered_quiet,
  float64_gt_ordered_quiet,
  float64_true_signalling
};

/* Opcode: VEX.0F 51 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_sqrtps(&op.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F 51 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPD_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_sqrtpd(&op.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 51 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = float32_sqrt(op2, status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 51 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = float64_sqrt(op2, status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F 52 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    op.ymm32u(n) = approximate_rsqrt(op.ymm32u(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 52 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  op1.xmm32u(0) = approximate_rsqrt(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F 53 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPPS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    op.vmm32u(n) = approximate_rcp(op.vmm32u(n));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 53 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  op1.xmm32u(0) = approximate_rcp(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 58 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_addps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 58 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  
  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_addpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 58 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_add(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 58 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_add(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 59 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_mulps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 59 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_mulpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 59 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_mul(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 59 (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_mul(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 5C (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_subps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 5C (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_subpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 5C (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_sub(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 5C (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_sub(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 5D (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_minps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 5D (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_minpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 5D (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_min(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 5D (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_min(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 5E (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_divps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 5E (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_divpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 5E (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_div(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 5E (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_div(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 5F (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_maxps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 5F (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_maxpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 5F (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXSS_VssHpsWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = float32_max(op1.xmm32u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 5F (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXSD_VsdHpdWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = float64_max(op1.xmm64u(0), op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 7C (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VHADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_haddpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 7C (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VHADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_haddps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 7D (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VHSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_hsubpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F 7D (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VHSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_hsubps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F C2 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op1.ymm32u(n) = avx_compare32[ib](op1.ymm32u(n), op2.ymm32u(n), status) ? 0xFFFFFFFF : 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F C2 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op1.ymm64u(n) = avx_compare64[ib](op1.ymm64u(n), op2.ymm64u(n), status) ?
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F C2 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSD_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  if(avx_compare64[ib](op1.xmm64u(0), op2, status)) {
    op1.xmm64u(0) = BX_CONST64(0xFFFFFFFFFFFFFFFF);
  } else {
    op1.xmm64u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F C2 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSS_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  if(avx_compare32[ib](op1.xmm32u(0), op2, status)) {
    op1.xmm32u(0) = 0xFFFFFFFF;
  } else {
    op1.xmm32u(0) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F D0 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_addsubpd(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F2.0F D0 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < len; n++) {
    xmm_addsubps(&op1.vmm128(n), &op2.vmm128(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.38.0E (VEX.W=0, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPS_VpsWpsR(bxInstruction_c *i)
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
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPD_VpdWpdR(bxInstruction_c *i)
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
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for(unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.ymm32u(n) = float32_round_to_int(op.ymm32u(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.09 (VEX.W ignore, VEX.VVV #UD) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPD_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  for(unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op.ymm64u(n) = float64_round_to_int(op.ymm64u(n), status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.0A (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSS_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  op1.xmm32u(0) = float32_round_to_int(op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.0B (VEX.W ignore, VEX.L ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSD_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;

  op1.xmm64u(0) = float64_round_to_int(op2, status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.40 (VEX.W ignore) */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPPS_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

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
