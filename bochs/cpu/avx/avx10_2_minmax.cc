/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2024 Stanislav Shwartsman
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

#include "softfloat3e/include/softfloat.h"
#include "simd_int.h"

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned num_elements = QWORD_ELEMENTS(len);
  Bit32u opmask = i->opmask() ? BX_READ_8BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
  int op_select =  i->Ib() & 0x3; 
  int sign_ctrl = (i->Ib() >> 2) & 0x3;

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm64u(n) = f64_minmax(op1.vmm64u(n), op2.vmm64u(n), op_select, sign_ctrl, propagate_NaNs, &status);
    else
      op1.vmm64u(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (! i->isZeroMasking()) {
    simd_blendpd(&BX_READ_AVX_REG(i->dst()), &op1, opmask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXSD_MASK_VsdHpdWsdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

    bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
    int op_select =  i->Ib() & 0x3; 
    int sign_ctrl = (i->Ib() >> 2) & 0x3;

    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);

    op1.xmm64u(0) = f64_minmax(op1.xmm64u(0), op2, op_select, sign_ctrl, propagate_NaNs, &status);

    check_exceptionsSSE(softfloat_getExceptionFlags(&status));
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned num_elements = DWORD_ELEMENTS(len);
  Bit32u opmask = i->opmask() ? BX_READ_16BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
  int op_select =  i->Ib() & 0x3; 
  int sign_ctrl = (i->Ib() >> 2) & 0x3;

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm32u(n) = f32_minmax(op1.vmm32u(n), op2.vmm32u(n), op_select, sign_ctrl, propagate_NaNs, &status);
    else
      op1.vmm32u(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (! i->isZeroMasking()) {
    simd_blendps(&BX_READ_AVX_REG(i->dst()), &op1, opmask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXSS_MASK_VssHpsWssIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

    bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
    int op_select =  i->Ib() & 0x3; 
    int sign_ctrl = (i->Ib() >> 2) & 0x3;

    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);

    op1.xmm32u(0) = f32_minmax(op1.xmm32u(0), op2, op_select, sign_ctrl, propagate_NaNs, &status);

    check_exceptionsSSE(softfloat_getExceptionFlags(&status));
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXPH_MASK_VphHphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
  int op_select =  i->Ib() & 0x3; 
  int sign_ctrl = (i->Ib() >> 2) & 0x3;

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      op1.vmm16u(n) = f16_minmax(op1.vmm16u(n), op2.vmm16u(n), op_select, sign_ctrl, propagate_NaNs, &status);
    else
      op1.vmm16u(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (! i->isZeroMasking()) {
    simd_pblendw(&BX_READ_AVX_REG(i->dst()), &op1, opmask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXSH_MASK_VshHphWshIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

    bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
    int op_select =  i->Ib() & 0x3; 
    int sign_ctrl = (i->Ib() >> 2) & 0x3;

    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);

    op1.xmm16u(0) = f16_minmax(op1.xmm16u(0), op2, op_select, sign_ctrl, propagate_NaNs, &status);

    check_exceptionsSSE(softfloat_getExceptionFlags(&status));
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

#include "bf16.h"

extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMINMAXBF16_MASK_VphHphWphIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  bool propagate_NaNs = (i->Ib() & 0x10) ? 0 : 1; // propagate_NaNs if imm[4] == 0
  int op_select =  i->Ib() & 0x3; 
  int sign_ctrl = (i->Ib() >> 2) & 0x3;

  // This instruction does not generate floating point exceptions and does not consult or update MXCSR
  softfloat_status_t status = prepare_ne_softfloat_status_helper(true);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      // right shift by 16 to convert back to BF16
      op1.vmm16u(n) = f32_minmax(convert_bfloat16_to_fp32(op1.vmm16u(n)), convert_bfloat16_to_fp32(op2.vmm16u(n)), op_select, sign_ctrl, propagate_NaNs, &status) >> 16;
    else
      op1.vmm16u(n) = 0;
  }

  if (! i->isZeroMasking()) {
    simd_pblendw(&BX_READ_AVX_REG(i->dst()), &op1, opmask, num_elements);
    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  }

  BX_NEXT_INSTR(i);
}

#endif
