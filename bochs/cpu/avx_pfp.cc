/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 6

extern void mxcsr_to_softfloat_status_word(float_status_t &status, bx_mxcsr_t mxcsr);

extern float32 approximate_rsqrt(float32 op);
extern float32 approximate_rcp(float32 op);

#include "fpu/softfloat-compare.h"
#include "fpu/softfloat-specialize.h"

void BX_CPU_C::print_state_AVX(void)
{
  BX_DEBUG(("MXCSR: 0x%08x\n", BX_MXCSR_REGISTER));
  for(int n=0;n<BX_XMM_REGISTERS;n++) {
    BxPackedAvxRegister vmm = BX_READ_AVX_REG(n);
    BX_DEBUG(("XMM%02u: %08x%08x:%08x%08x:%08x%08x:%08x%08x\n", n,
       vmm.avx32u(7), vmm.avx32u(6), vmm.avx32u(5), vmm.avx32u(4),
       vmm.avx32u(3), vmm.avx32u(2), vmm.avx32u(1), vmm.avx32u(0)));
  }
}

/* Comparison predicate for VCMPSS/VCMPPS instructions */
static float32_compare_method compare32[32] = {
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
static float64_compare_method compare64[32] = {
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

#endif

/* Opcode: VEX.F3.0F 2A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  op1.xmm32u(0) = int32_to_float32(BX_READ_32BIT_REG(i->rm()), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.F3.0F 2A (VEX.W=1) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  op1.xmm32u(0) = int64_to_float32(BX_READ_64BIT_REG(i->rm()), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.F2.0F 2A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SD_VsdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  op1.xmm64u(0) = int32_to_float64(BX_READ_32BIT_REG(i->rm()));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.F2.0F 2A (VEX.W=1) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SD_VsdEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  op1.xmm64u(0) = int64_to_float64(BX_READ_64BIT_REG(i->rm()), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.0F 51 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {
    if (MXCSR.get_DAZ()) 
        op.avx32u(n) = float32_denormal_to_zero(op.avx32u(n));
    op.avx32u(n) = float32_sqrt(op.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.66.0F 51 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {
    if (MXCSR.get_DAZ()) 
        op.avx64u(n) = float64_denormal_to_zero(op.avx64u(n));
    op.avx64u(n) = float64_sqrt(op.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 51 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op2 = float32_denormal_to_zero(op2);
  op1.xmm32u(0) = float32_sqrt(op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 51 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSQRTSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op2 = float64_denormal_to_zero(op2);
  op1.xmm64u(0) = float64_sqrt(op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.0F 52 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n++)
    op.avx32u(n) = approximate_rsqrt(op.avx32u(n));

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 52 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRSQRTSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  op1.xmm32u(0) = approximate_rsqrt(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.0F 53 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (4*len); n++)
    op.avx32u(n) = approximate_rcp(op.avx32u(n));

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 53 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VRCPSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  op1.xmm32u(0) = approximate_rcp(op2);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 58 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    op1.avx32u(n) = float32_add(op1.avx32u(n), op2.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 58 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }

    op1.avx64u(n) = float64_add(op1.avx64u(n), op2.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 58 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  op1.xmm32u(0) = float32_add(op1.xmm32u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 58 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  op1.xmm64u(0) = float64_add(op1.xmm64u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 59 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    op1.avx32u(n) = float32_mul(op1.avx32u(n), op2.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 59 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }

    op1.avx64u(n) = float64_mul(op1.avx64u(n), op2.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 59 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  op1.xmm32u(0) = float32_mul(op1.xmm32u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 59 (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMULSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  op1.xmm64u(0) = float64_mul(op1.xmm64u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.0F 5A (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PD_VpdWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister result;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ())
      op.xmm32u(n) = float32_denormal_to_zero(op.xmm32u(n));

     result.avx64u(n) = float32_to_float64(op.xmm32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F 5A (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPD2PS_VpsWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  BxPackedXmmRegister result;

  result.xmm64u(1) = 0; /* clear upper part of the result for case of VL128 */

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx64u(n) = float64_denormal_to_zero(op.avx64u(n));

    result.xmm32u(n) = float64_to_float32(op.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), result);
#endif
}

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SD_VsdWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op2 = float32_denormal_to_zero(op2);
  op1.xmm64u(0) = float32_to_float64(op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SS_VssWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op2 = float64_denormal_to_zero(op2);
  op1.xmm32u(0) = float64_to_float32(op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTDQ2PS_VpsWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {
    op.avx32u(n) = int32_to_float32(op.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx32u(n) = float32_denormal_to_zero(op.avx32u(n));

    op.avx32u(n) = float32_to_int32(op.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx32u(n) = float32_denormal_to_zero(op.avx32u(n));

    op.avx32u(n) = float32_to_int32_round_to_zero(op.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.NDS.0F 5C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    op1.avx32u(n) = float32_sub(op1.avx32u(n), op2.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 5C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }

    op1.avx64u(n) = float64_sub(op1.avx64u(n), op2.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 5C (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  op1.xmm32u(0) = float32_sub(op1.xmm32u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 5C (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSUBSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  op1.xmm64u(0) = float64_sub(op1.xmm64u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 5D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    int rc = float32_compare(op1.avx32u(n), op2.avx32u(n), status_word);
    op1.avx32u(n) = (rc == float_relation_less) ? op1.avx32u(n) : op2.avx32u(n);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 5D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }
    
    int rc = float64_compare(op1.avx64u(n), op2.avx64u(n), status_word);
    op1.avx64u(n) = (rc == float_relation_less) ? op1.avx64u(n) : op2.avx64u(n);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 5D (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare(op1.xmm32u(0), op2, status_word);
  op1.xmm32u(0) = (rc == float_relation_less) ? op1.xmm32u(0) : op2;
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 5D (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare(op1.xmm64u(0), op2, status_word);
  op1.xmm64u(0) = (rc == float_relation_less) ? op1.xmm64u(0) : op2;
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 5E (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    op1.avx32u(n) = float32_div(op1.avx32u(n), op2.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 5E (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }

    op1.avx64u(n) = float64_div(op1.avx64u(n), op2.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 5E (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  op1.xmm32u(0) = float32_div(op1.xmm32u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 5E (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDIVSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  op1.xmm64u(0) = float64_div(op1.xmm64u(0), op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.0F 5F (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    int rc = float32_compare(op1.avx32u(n), op2.avx32u(n), status_word);
    op1.avx32u(n) = (rc == float_relation_greater) ? op1.avx32u(n) : op2.avx32u(n);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 5F (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }
    
    int rc = float64_compare(op1.avx64u(n), op2.avx64u(n), status_word);
    op1.avx64u(n) = (rc == float_relation_greater) ? op1.avx64u(n) : op2.avx64u(n);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F3.0F 5F (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare(op1.xmm32u(0), op2, status_word);
  op1.xmm32u(0) = (rc == float_relation_greater) ? op1.xmm32u(0) : op2;
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F 5F (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMAXSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare(op1.xmm64u(0), op2, status_word);
  op1.xmm64u(0) = (rc == float_relation_greater) ? op1.xmm64u(0) : op2;
  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.66.0F 7C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VHADDPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n+=2) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n)   = float64_denormal_to_zero(op1.avx64u(n));
      op1.avx64u(n+1) = float64_denormal_to_zero(op1.avx64u(n+1));
      op2.avx64u(n)   = float64_denormal_to_zero(op2.avx64u(n));
      op2.avx64u(n+1) = float64_denormal_to_zero(op2.avx64u(n+1));
    }

    op1.avx64u(n)   = float64_add(op1.avx64u(n), op1.avx64u(n+1), status_word);
    op1.avx64u(n+1) = float64_add(op2.avx64u(n), op2.avx64u(n+1), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F2.0F 7C (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VHADDPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n+=4) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n)   = float32_denormal_to_zero(op1.avx32u(n));
      op1.avx32u(n+1) = float32_denormal_to_zero(op1.avx32u(n+1));
      op1.avx32u(n+2) = float32_denormal_to_zero(op1.avx32u(n+2));
      op1.avx32u(n+3) = float32_denormal_to_zero(op1.avx32u(n+3));

      op2.avx32u(n)   = float32_denormal_to_zero(op2.avx32u(n));
      op2.avx32u(n+1) = float32_denormal_to_zero(op2.avx32u(n+1));
      op2.avx32u(n+2) = float32_denormal_to_zero(op2.avx32u(n+2));
      op2.avx32u(n+3) = float32_denormal_to_zero(op2.avx32u(n+3));
    }

    op1.avx32u(n)   = float32_add(op1.avx32u(n),   op1.avx32u(n+1), status_word);
    op1.avx32u(n+1) = float32_add(op1.avx32u(n+2), op1.avx32u(n+3), status_word);
    op1.avx32u(n+2) = float32_add(op2.avx32u(n),   op2.avx32u(n+1), status_word);
    op1.avx32u(n+3) = float32_add(op2.avx32u(n+2), op2.avx32u(n+3), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F 7D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VHSUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n+=2) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n)   = float64_denormal_to_zero(op1.avx64u(n));
      op1.avx64u(n+1) = float64_denormal_to_zero(op1.avx64u(n+1));
      op2.avx64u(n)   = float64_denormal_to_zero(op2.avx64u(n));
      op2.avx64u(n+1) = float64_denormal_to_zero(op2.avx64u(n+1));
    }

    op1.avx64u(n)   = float64_sub(op1.avx64u(n), op1.avx64u(n+1), status_word);
    op1.avx64u(n+1) = float64_sub(op2.avx64u(n), op2.avx64u(n+1), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F2.0F 7D (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VHSUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n+=4) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n)   = float32_denormal_to_zero(op1.avx32u(n));
      op1.avx32u(n+1) = float32_denormal_to_zero(op1.avx32u(n+1));
      op1.avx32u(n+2) = float32_denormal_to_zero(op1.avx32u(n+2));
      op1.avx32u(n+3) = float32_denormal_to_zero(op1.avx32u(n+3));

      op2.avx32u(n)   = float32_denormal_to_zero(op2.avx32u(n));
      op2.avx32u(n+1) = float32_denormal_to_zero(op2.avx32u(n+1));
      op2.avx32u(n+2) = float32_denormal_to_zero(op2.avx32u(n+2));
      op2.avx32u(n+3) = float32_denormal_to_zero(op2.avx32u(n+3));
    }

    op1.avx32u(n)   = float32_sub(op1.avx32u(n),   op1.avx32u(n+1), status_word);
    op1.avx32u(n+1) = float32_sub(op1.avx32u(n+2), op1.avx32u(n+3), status_word);
    op1.avx32u(n+2) = float32_sub(op2.avx32u(n),   op2.avx32u(n+1), status_word);
    op1.avx32u(n+3) = float32_sub(op2.avx32u(n+2), op2.avx32u(n+3), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n) = float32_denormal_to_zero(op1.avx32u(n));
      op2.avx32u(n) = float32_denormal_to_zero(op2.avx32u(n));
    }

    op1.avx32u(n) = compare32[ib](op1.avx32u(n), op2.avx32u(n), status) ? 0xFFFFFFFF : 0;
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.66.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 0x1F;

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n) = float64_denormal_to_zero(op1.avx64u(n));
      op2.avx64u(n) = float64_denormal_to_zero(op2.avx64u(n));
    }

    op1.avx64u(n) = compare64[ib](op1.avx64u(n), op2.avx64u(n), status) ?
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F2.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSD_VsdWsdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib() & 0x1F;

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op2           = float64_denormal_to_zero(op2);
  }

  if(compare64[ib](op1.xmm64u(0), op2, status_word)) {
    op1.xmm64u(0) = BX_CONST64(0xFFFFFFFFFFFFFFFF);
  } else {
    op1.xmm64u(0) = 0;
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F3.0F C2 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCMPSS_VssWssIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib() & 0x1F;

  if (MXCSR.get_DAZ())
  {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op2           = float32_denormal_to_zero(op2);
  }

  if(compare32[ib](op1.xmm32u(0), op2, status_word)) {
    op1.xmm32u(0) = 0xFFFFFFFF;
  } else {
    op1.xmm32u(0) = 0;
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.NDS.F2.0F D0 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (2*len); n+=2) {

    if (MXCSR.get_DAZ()) {
      op1.avx64u(n)   = float64_denormal_to_zero(op1.avx64u(n));
      op1.avx64u(n+1) = float64_denormal_to_zero(op1.avx64u(n+1));
      op2.avx64u(n)   = float64_denormal_to_zero(op2.avx64u(n));
      op2.avx64u(n+1) = float64_denormal_to_zero(op2.avx64u(n+1));
    }

    op1.avx64u(n)   = float64_sub(op1.avx64u(n),   op2.avx64u(n),   status_word);
    op1.avx64u(n+1) = float64_add(op1.avx64u(n+1), op2.avx64u(n+1), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.NDS.F2.0F D0 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VADDSUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n+=2) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n)   = float32_denormal_to_zero(op1.avx32u(n));
      op1.avx32u(n+1) = float32_denormal_to_zero(op1.avx32u(n+1));
      op2.avx32u(n)   = float32_denormal_to_zero(op2.avx32u(n));
      op2.avx32u(n+1) = float32_denormal_to_zero(op2.avx32u(n+1));
    }

    op1.avx32u(n)   = float32_sub(op1.avx32u(n),   op2.avx32u(n),   status_word);
    op1.avx32u(n+1) = float32_add(op1.avx32u(n+1), op2.avx32u(n+1), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTPD2DQ_VqWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  BxPackedXmmRegister result;

  result.xmm64u(1) = 0; /* clear upper part of the result for case of VL128 */

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx64u(n) = float64_denormal_to_zero(op.avx64u(n));

    result.xmm32u(n) = float64_to_int32_round_to_zero(op.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), result);
#endif
}

/* Opcode: VEX.F2.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPD2DQ_VqWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  BxPackedXmmRegister result;

  result.xmm64u(1) = 0; /* clear upper part of the result for case of VL128 */

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx64u(n) = float64_denormal_to_zero(op.avx64u(n));

    result.xmm32u(n) = float64_to_int32(op.avx64u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), result);
#endif
}

/* Opcode: VEX.F3.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTDQ2PD_VpdWqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister result;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  unsigned len = i->getVL();

  for (unsigned n=0; n < (2*len); n++) {
     result.avx64u(n) = int32_to_float64(op.xmm32u(n));
  }

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
#endif
}

/* Opcode: VEX.66.0F.38.0E (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->nnn()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < (2*len); n++) {
    if ((op2.avx64u(n) &  op1.avx64u(n) & BX_CONST64(0x8000000080000000)) != 0)
      result &= ~EFlagsZFMask;

    if ((op2.avx64u(n) & ~op1.avx64u(n) & BX_CONST64(0x8000000080000000)) != 0)
      result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);
#endif
}

/* Opcode: VEX.66.0F.38.0F (VEX.W=0, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VTESTPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->nnn()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  unsigned result = EFlagsZFMask | EFlagsCFMask;

  for (unsigned n=0; n < (2*len); n++) {
    if ((op2.avx64u(n) &  op1.avx64u(n) & BX_CONST64(0x8000000000000000)) != 0)
      result &= ~EFlagsZFMask;

    if ((op2.avx64u(n) & ~op1.avx64u(n) & BX_CONST64(0x8000000000000000)) != 0)
      result &= ~EFlagsCFMask;
  }

  setEFlagsOSZAPC(result);
#endif
}

/* Opcode: VEX.66.0F.3A.08 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  for(unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx32u(n) = float32_denormal_to_zero(op.avx32u(n));

    op.avx32u(n) = float32_round_to_int(op.avx32u(n), status_word);
  }

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.66.0F.3A.09 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  for(unsigned n=0; n < (2*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx64u(n) = float64_denormal_to_zero(op.avx64u(n));

    op.avx64u(n) = float64_round_to_int(op.avx64u(n), status_word);
  }

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op, len);
#endif
}

/* Opcode: VEX.66.0F.3A.0A (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSS_VssWssIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) op2 = float32_denormal_to_zero(op2);
  op1.xmm32u(0) = float32_round_to_int(op2, status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.66.0F.3A.0B (VEX.W ignore, VEX.L ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VROUNDSD_VsdWsdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->vvv());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) op2 = float64_denormal_to_zero(op2);
  op1.xmm64u(0) = float64_round_to_int(op2, status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->nnn(), op1);
#endif
}

/* Opcode: VEX.66.0F.3A.40 (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDPPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->vvv()), op2 = BX_READ_AVX_REG(i->rm());
  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n+=4) {

    if (MXCSR.get_DAZ()) {
      op1.avx32u(n+0) = float32_denormal_to_zero(op1.avx32u(n));
      op1.avx32u(n+1) = float32_denormal_to_zero(op1.avx32u(n+1));
      op1.avx32u(n+2) = float32_denormal_to_zero(op1.avx32u(n+2));
      op1.avx32u(n+3) = float32_denormal_to_zero(op1.avx32u(n+3));

      op2.avx32u(n+0) = float32_denormal_to_zero(op2.avx32u(n));
      op2.avx32u(n+1) = float32_denormal_to_zero(op2.avx32u(n+1));
      op2.avx32u(n+2) = float32_denormal_to_zero(op2.avx32u(n+2));
      op2.avx32u(n+3) = float32_denormal_to_zero(op2.avx32u(n+3));
    }

    BxPackedXmmRegister tmp;

    tmp.xmm64u(0) = tmp.xmm64u(1) = 0;

    if (mask & 0x10)
       tmp.xmm32u(0) = float32_mul(op1.avx32u(n+0), op2.avx32u(n+0), status_word);
    if (mask & 0x20)
       tmp.xmm32u(1) = float32_mul(op1.avx32u(n+1), op2.avx32u(n+1), status_word);
    if (mask & 0x40)
       tmp.xmm32u(2) = float32_mul(op1.avx32u(n+2), op2.avx32u(n+2), status_word);
    if (mask & 0x80)
       tmp.xmm32u(3) = float32_mul(op1.avx32u(n+3), op2.avx32u(n+3), status_word);

    float32 r1 = float32_add(tmp.xmm32u(0), tmp.xmm32u(1), status_word);
    float32 r2 = float32_add(tmp.xmm32u(2), tmp.xmm32u(3), status_word);
    float32 r  = float32_add(r1, r2, status_word);

    op1.avx32u(n+0) = (mask & 0x01) ? r : 0;
    op1.avx32u(n+1) = (mask & 0x02) ? r : 0;
    op1.avx32u(n+2) = (mask & 0x04) ? r : 0;
    op1.avx32u(n+3) = (mask & 0x08) ? r : 0;
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), op1, len);
#endif
}

/* Opcode: VEX.66.0F.3A.13 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPH2PS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  unsigned len = i->getVL();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ())
      op.xmm16u(n) = float16_denormal_to_zero(op.xmm16u(n));

     result.avx32u(n) = float16_to_float32(op.xmm16u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->nnn(), result, len);
}

/* Opcode: VEX.66.0F.3A.1D (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PH_WpsVpsIb(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->nnn());
  BxPackedXmmRegister result;

  result.xmm64u(1) = 0; /* clear upper part of the result for case of VL128 */

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  unsigned len = i->getVL();

  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  for (unsigned n=0; n < (4*len); n++) {

    if (MXCSR.get_DAZ())
      op.avx32u(n) = float32_denormal_to_zero(op.avx32u(n));

    result.xmm16u(n) = float32_to_float16(op.avx32u(n), status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);

  if (i->modC0()) {
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->rm(), result);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

    if (len == BX_VL256)
      write_virtual_dqword(i->seg(), eaddr, &result);
    else
      write_virtual_qword(i->seg(), eaddr, result.xmm64u(0));
  }
}

#endif
