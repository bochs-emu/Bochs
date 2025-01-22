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

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);
extern softfloat_status_t prepare_ne_softfloat_status_helper(bool denormals_are_zeros);

#include "bf8.h"
#include "hf8.h"
#include "simd_int.h"

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTHF82PH_VphWf8R(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      result.vmm16u(n) = convert_hf8_to_fp16(op.ymmubyte(n), &status);
    else
      result.vmm16u(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (! i->isZeroMasking()) {
      simd_pblendw(&BX_READ_AVX_REG(i->dst()), &result, opmask, num_elements);
      BX_CLEAR_AVX_REGZ(i->dst(), len);
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

#include "cpu/decoder/ia_opcodes.h"

// PH2BF8

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVT2PH2BF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister src1 = BX_READ_AVX_REG(i->src1()), src2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  unsigned num_elements_from_source = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVT2PH2BF8_Vf8HdqWph_Kmask);
  unsigned n = 0;

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  Bit32u mask, opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  // first half of the elements come from src2
  for (mask = 0x1; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmmubyte(n) = convert_ne_fp16_to_bf8(src2.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      dst.vmmubyte(n) = 0;
  }

  // second half of the elements come from src1
  for (n = 0; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmmubyte(n + num_elements_from_source) = convert_ne_fp16_to_bf8(src1.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      dst.vmmubyte(n + num_elements_from_source) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPH2BF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), res = BX_READ_AVX_REG(i->dst());
  unsigned opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTPH2BF8S_Vf8HdqWph_Kmask);

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      res.vmmubyte(n) = convert_ne_fp16_to_bf8(op.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      res.vmmubyte(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), res.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), res, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBIASPH2BF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), res = BX_READ_AVX_REG(i->dst());
  unsigned opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTBIASPH2BF8S_Vf8HdqWph_Kmask);

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      res.vmmubyte(n) = convert_truncate_fp16_to_bf8_bias(op2.vmm16u(n), op1.vmm16u(n) & 0xFF, saturate, &status);
    else if (i->isZeroMasking())
      res.vmmubyte(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), res.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), res, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

// PH2HF8

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVT2PH2HF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister src1 = BX_READ_AVX_REG(i->src1()), src2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  unsigned num_elements_from_source = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVT2PH2HF8_Vf8HdqWph_Kmask);
  unsigned n = 0;

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  Bit32u mask, opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  // first half of the elements come from src2
  for (mask = 0x1; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmmubyte(n) = convert_ne_fp16_to_hf8(src2.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      dst.vmmubyte(n) = 0;
  }

  // second half of the elements come from src1
  for (n = 0; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmmubyte(n + num_elements_from_source) = convert_ne_fp16_to_hf8(src1.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      dst.vmmubyte(n + num_elements_from_source) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPH2HF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), res = BX_READ_AVX_REG(i->dst());
  unsigned opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTPH2HF8S_Vf8HdqWph_Kmask);

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      res.vmmubyte(n) = convert_ne_fp16_to_hf8(op.vmm16u(n), saturate, &status);
    else if (i->isZeroMasking())
      res.vmmubyte(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), res.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), res, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTBIASPH2HF8_Vf8HdqWphR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), res = BX_READ_AVX_REG(i->dst());
  unsigned opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();
  unsigned num_elements = WORD_ELEMENTS(len);
  bool saturate = (i->getIaOpcode() == BX_IA_EVEX_VCVTBIASPH2HF8S_Vf8HdqWph_Kmask);

  softfloat_status_t status = prepare_ne_softfloat_status_helper(false);

  for (unsigned n=0, tmp_mask = opmask; n < num_elements; n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      res.vmmubyte(n) = convert_truncate_fp16_to_hf8_bias(op2.vmm16u(n), op1.vmm16u(n) & 0xFF, saturate, &status);
    else if (i->isZeroMasking())
      res.vmmubyte(n) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), res.vmm64u(0));
  } else {
    BX_WRITE_AVX_REGZ(i->dst(), res, len >> 1); /* write half vector */
  }

  BX_NEXT_INSTR(i);
}

#endif
