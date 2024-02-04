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

#include "simd_int.h"
#include "fpu/softfloat16.h"

// scalar

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SH_VshWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = f64_to_f16(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SH_MASK_VshWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm16u(0) = f64_to_f16(op2, &status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SH_VshWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = f32_to_f16(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SH_MASK_VshWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm16u(0) = f32_to_f16(op2, &status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SD_VsdWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = f16_to_f64(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SD_MASK_VsdWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = f16_to_f64(op2, &status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SS_VssWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = f16_to_f32(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SS_MASK_VssWshR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float16 op2 = BX_READ_XMM_REG_LO_WORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = f16_to_f32(op2, &status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SI_GdWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f16_to_i32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2SI_GqWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f16_to_i64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2USI_GdWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f16_to_ui32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSH2USI_GqWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f16_to_ui64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSH2SI_GdWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f16_to_i32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSH2SI_GqWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f16_to_i64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSH2USI_GdWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f16_to_ui32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSH2USI_GqWshR(bxInstruction_c *i)
{
  float16 op = BX_READ_XMM_REG_LO_WORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f16_to_ui64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SH_VshEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = i32_to_f16(BX_READ_32BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SH_VshEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = i64_to_f16(BX_READ_64BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SH_VshEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = ui32_to_f16(BX_READ_32BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SH_VshEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm16u(0) = ui64_to_f16(BX_READ_64BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

// packed

#define AVX512_CVT64_TO_16(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    BxPackedXmmRegister result;                                                             \
    result.xmm64u(0) = 0;                                                                   \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.xmm16u(n) = (func)(op.vmm64u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (len != BX_VL512) {                                                                  \
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.xmm64u(0));                     \
    } else {                                                                                \
      BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), result);                                        \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT64_TO_16(VCVTPD2PH_VphWpdR, f64_to_f16)
AVX512_CVT64_TO_16(VCVTQQ2PH_VphWdqR, i64_to_f16)
AVX512_CVT64_TO_16(VCVTUQQ2PH_VphWdqR, ui64_to_f16)

#define AVX512_CVT64_TO_16_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    BxPackedXmmRegister result = BX_READ_XMM_REG(i->dst());                                 \
    unsigned opmask = BX_READ_8BIT_OPMASK(i->opmask());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, opmask >>= 1) {                        \
      if (opmask & 0x1)                                                                     \
        result.xmm16u(n) = (func)(op.vmm64u(n), &status);                                   \
      else if (i->isZeroMasking())                                                          \
        result.xmm16u(n) = 0;                                                               \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (len == BX_VL128) {                                                                  \
      BX_WRITE_XMM_REG_LO_DWORD_CLEAR_HIGH(i->dst(), result.xmm32u(0));                     \
    } else if (len == BX_VL256) {                                                           \
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.xmm64u(0));                     \
    } else {                                                                                \
      BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), result);                                        \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT64_TO_16_MASK(VCVTPD2PH_MASK_VphWpdR, f64_to_f16)
AVX512_CVT64_TO_16_MASK(VCVTQQ2PH_MASK_VphWdqR, i64_to_f16)
AVX512_CVT64_TO_16_MASK(VCVTUQQ2PH_MASK_VphWdqR, ui64_to_f16)

#define AVX512_CVT32_TO_16(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;                             \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {                                      \
      result.vmm16u(n) = (func)(op.vmm32u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (len == BX_VL128) {                                                                  \
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));                     \
    } else {                                                                                \
      BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); /* write half vector */                \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT32_TO_16(VCVTPS2PHX_VphWpsR, f32_to_f16)
AVX512_CVT32_TO_16(VCVTDQ2PH_VphWdqR, i32_to_f16)
AVX512_CVT32_TO_16(VCVTUDQ2PH_VphWdqR, ui32_to_f16)

#define AVX512_CVT32_TO_16_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result = BX_READ_AVX_REG(i->dst()); \
    unsigned opmask = BX_READ_16BIT_OPMASK(i->opmask());                                    \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++, opmask >>= 1) {                        \
      if (opmask & 0x1)                                                                     \
        result.vmm16u(n) = (func)(op.vmm32u(n), &status);                                   \
      else if (i->isZeroMasking())                                                          \
        result.vmm16u(n) = 0;                                                               \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (len == BX_VL128) {                                                                  \
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));                     \
    } else {                                                                                \
      BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); /* write half vector */                \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT32_TO_16_MASK(VCVTPS2PHX_MASK_VphWpsR, f32_to_f16)
AVX512_CVT32_TO_16_MASK(VCVTDQ2PH_MASK_VphWdqR, i32_to_f16)
AVX512_CVT32_TO_16_MASK(VCVTUDQ2PH_MASK_VphWdqR, ui32_to_f16)

#define AVX512_CVT16_TO_16(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {                                       \
      op.vmm16u(n) = (func)(op.vmm16u(n), status);                                          \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                   \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_16(VCVTPH2UW_VdqWphR, float16_to_uint16)
AVX512_CVT16_TO_16(VCVTPH2W_VdqWphR, float16_to_int16)
AVX512_CVT16_TO_16(VCVTTPH2UW_VdqWphR, float16_to_uint16_round_to_zero)
AVX512_CVT16_TO_16(VCVTTPH2W_VdqWphR, float16_to_int16_round_to_zero)
AVX512_CVT16_TO_16(VCVTUW2PH_VphWdqR, uint16_to_float16)
AVX512_CVT16_TO_16(VCVTW2PH_VphWdqR, int16_to_float16)

#define AVX512_CVT16_TO_16_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
    Bit32u opmask = BX_READ_32BIT_OPMASK(i->opmask());                                      \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, mask = 0x1; n < WORD_ELEMENTS(len); n++, mask <<= 1) {               \
      if (opmask & mask)                                                                    \
        op.vmm16u(n) = (func)(op.vmm16u(n), status);                                        \
      else                                                                                  \
        op.vmm16u(n) = 0;                                                                   \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, opmask >>= 8)                                        \
        xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);             \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                 \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_16_MASK(VCVTPH2UW_MASK_VdqWphR, float16_to_uint16)
AVX512_CVT16_TO_16_MASK(VCVTPH2W_MASK_VdqWphR, float16_to_int16)
AVX512_CVT16_TO_16_MASK(VCVTTPH2UW_MASK_VdqWphR, float16_to_uint16_round_to_zero)
AVX512_CVT16_TO_16_MASK(VCVTTPH2W_MASK_VdqWphR, float16_to_int16_round_to_zero)
AVX512_CVT16_TO_16_MASK(VCVTUW2PH_MASK_VphWdqR, uint16_to_float16)
AVX512_CVT16_TO_16_MASK(VCVTW2PH_MASK_VphWdqR, int16_to_float16)

#define AVX512_CVT16_TO_32(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {                                      \
      result.vmm32u(n) = (func)(op.ymm16u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    BX_WRITE_AVX_REGZ(i->dst(), result, len);                                               \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_32(VCVTPH2PSX_VpsWphR, f16_to_f32)
AVX512_CVT16_TO_32(VCVTPH2DQ_VdqWphR, f16_to_i32)
AVX512_CVT16_TO_32(VCVTTPH2DQ_VdqWphR, f16_to_i32_round_to_zero)
AVX512_CVT16_TO_32(VCVTPH2UDQ_VdqWphR, f16_to_ui32)
AVX512_CVT16_TO_32(VCVTTPH2UDQ_VdqWphR, f16_to_ui32_round_to_zero)

#define AVX512_CVT16_TO_32_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                     \
    unsigned opmask = BX_READ_16BIT_OPMASK(i->opmask());                                    \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, tmp_mask = opmask; n < DWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {   \
      if (tmp_mask & 0x1)                                                                   \
        result.vmm32u(n) = (func)(op.ymm16u(n), &status);                                   \
      else                                                                                  \
        result.vmm32u(n) = 0;                                                               \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, opmask >>= 4)                                        \
        xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);         \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), result, len);                                             \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_32_MASK(VCVTPH2PSX_MASK_VpsWphR, f16_to_f32)
AVX512_CVT16_TO_32_MASK(VCVTPH2DQ_MASK_VdqWphR, f16_to_i32)
AVX512_CVT16_TO_32_MASK(VCVTTPH2DQ_MASK_VdqWphR, f16_to_i32_round_to_zero)
AVX512_CVT16_TO_32_MASK(VCVTPH2UDQ_MASK_VdqWphR, f16_to_ui32)
AVX512_CVT16_TO_32_MASK(VCVTTPH2UDQ_MASK_VdqWphR, f16_to_ui32_round_to_zero)

#define AVX512_CVT16_TO_64(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.vmm64u(n) = (func)(op.xmm16u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    BX_WRITE_AVX_REGZ(i->dst(), result, len);                                               \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_64(VCVTPH2PD_VpdWphR, f16_to_f64)
AVX512_CVT16_TO_64(VCVTPH2QQ_VdqWphR, f16_to_i64)
AVX512_CVT16_TO_64(VCVTTPH2QQ_VdqWphR, f16_to_i64_round_to_zero)
AVX512_CVT16_TO_64(VCVTPH2UQQ_VdqWphR, f16_to_ui64)
AVX512_CVT16_TO_64(VCVTTPH2UQQ_VdqWphR, f16_to_ui64_round_to_zero)

#define AVX512_CVT16_TO_64_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());                                     \
    unsigned opmask = BX_READ_8BIT_OPMASK(i->opmask());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, tmp_mask = opmask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {   \
      if (tmp_mask & 0x1)                                                                   \
        result.vmm64u(n) = (func)(op.xmm16u(n), &status);                                   \
      else                                                                                  \
        result.vmm64u(n) = 0;                                                               \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, opmask >>= 2)                                        \
        xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);         \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), result, len);                                             \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT16_TO_64_MASK(VCVTPH2PD_MASK_VpdWphR, f16_to_f64)
AVX512_CVT16_TO_64_MASK(VCVTPH2QQ_MASK_VdqWphR, f16_to_i64)
AVX512_CVT16_TO_64_MASK(VCVTTPH2QQ_MASK_VdqWphR, f16_to_i64_round_to_zero)
AVX512_CVT16_TO_64_MASK(VCVTPH2UQQ_MASK_VdqWphR, f16_to_ui64)
AVX512_CVT16_TO_64_MASK(VCVTTPH2UQQ_MASK_VdqWphR, f16_to_ui64_round_to_zero)

#endif
