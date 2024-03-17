/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2023 Stanislav Shwartsman
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

// scalar

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2USI_GdWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f32_to_ui32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2USI_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f32_to_ui64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2USI_GdWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f32_to_ui32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2USI_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f32_to_ui64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2USI_GdWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f64_to_ui32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2USI_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f64_to_ui64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2USI_GdWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f64_to_ui32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2USI_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f64_to_ui64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SS_VssEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = ui32_to_f32(BX_READ_32BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SS_VssEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = ui64_to_f32(BX_READ_64BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SD_VsdEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  op1.xmm64u(0) = ui32_to_f64(BX_READ_32BIT_REG(i->src2()));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUSI2SD_VsdEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = ui64_to_f64(BX_READ_64BIT_REG(i->src2()), &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SS_MASK_VssWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm32u(0) = f64_to_f32(op2, &status);
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SD_MASK_VsdWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  if (BX_SCALAR_ELEMENT_MASK(i->opmask())) {
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
    softfloat_status_word_rc_override(status, i);
    op1.xmm64u(0) = f32_to_f64(op2, &status);
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

// packed

#define AVX512_CVT64_TO_32(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;                             \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.vmm32u(n) = (func)(op.vmm64u(n), &status);                                     \
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

AVX512_CVT64_TO_32(VCVTPD2UDQ_VdqWpdR, f64_to_ui32)
AVX512_CVT64_TO_32(VCVTTPD2UDQ_VdqWpdR, f64_to_ui32_round_to_zero)
AVX512_CVT64_TO_32(VCVTQQ2PS_VpsWdqR, i64_to_f32)
AVX512_CVT64_TO_32(VCVTUQQ2PS_VpsWdqR, ui64_to_f32)

#define AVX512_CVT64_TO_32_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result = BX_READ_AVX_REG(i->dst()); \
    unsigned opmask = BX_READ_8BIT_OPMASK(i->opmask());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++, opmask >>= 1) {                        \
      if (opmask & 0x1)                                                                     \
        result.vmm32u(n) = (func)(op.vmm64u(n), &status);                                   \
      else if (i->isZeroMasking())                                                          \
        result.vmm32u(n) = 0;                                                               \
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

AVX512_CVT64_TO_32_MASK(VCVTPD2PS_MASK_VpsWpdR, f64_to_f32)
AVX512_CVT64_TO_32_MASK(VCVTPD2DQ_MASK_VdqWpdR, f64_to_i32)
AVX512_CVT64_TO_32_MASK(VCVTTPD2DQ_MASK_VdqWpdR, f64_to_i32_round_to_zero)
AVX512_CVT64_TO_32_MASK(VCVTPD2UDQ_MASK_VdqWpdR, f64_to_ui32)
AVX512_CVT64_TO_32_MASK(VCVTTPD2UDQ_MASK_VdqWpdR, f64_to_ui32_round_to_zero)
AVX512_CVT64_TO_32_MASK(VCVTQQ2PS_MASK_VpsWdqR, i64_to_f32)
AVX512_CVT64_TO_32_MASK(VCVTUQQ2PS_MASK_VpsWdqR, ui64_to_f32)

#define AVX512_CVT32_TO_32(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {                                      \
      op.vmm32u(n) = (func)(op.vmm32u(n), &status);                                         \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                   \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT32_TO_32(VCVTPS2UDQ_VdqWpsR, f32_to_ui32)
AVX512_CVT32_TO_32(VCVTTPS2UDQ_VdqWpsR, f32_to_ui32_round_to_zero)
AVX512_CVT32_TO_32(VCVTUDQ2PS_VpsWdqR, ui32_to_f32)

#define AVX512_CVT32_TO_32_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
    Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());                                      \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {              \
      if (opmask & mask)                                                                    \
        op.vmm32u(n) = (func)(op.vmm32u(n), &status);                                       \
      else                                                                                  \
        op.vmm32u(n) = 0;                                                                   \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, opmask >>= 4)                                        \
        xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);             \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                 \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT32_TO_32_MASK(VCVTPS2DQ_MASK_VdqWpsR, f32_to_i32)
AVX512_CVT32_TO_32_MASK(VCVTTPS2DQ_MASK_VdqWpsR, f32_to_i32_round_to_zero)
AVX512_CVT32_TO_32_MASK(VCVTPS2UDQ_MASK_VdqWpsR, f32_to_ui32)
AVX512_CVT32_TO_32_MASK(VCVTTPS2UDQ_MASK_VdqWpsR, f32_to_ui32_round_to_zero)
AVX512_CVT32_TO_32_MASK(VCVTDQ2PS_MASK_VpsWdqR, i32_to_f32)
AVX512_CVT32_TO_32_MASK(VCVTUDQ2PS_MASK_VpsWdqR, ui32_to_f32)

#define AVX512_CVT64_TO_64(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      op.vmm64s(n) = (func)(op.vmm64u(n), &status);                                         \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                   \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT64_TO_64(VCVTPD2QQ_VdqWpdR, f64_to_i64)
AVX512_CVT64_TO_64(VCVTTPD2QQ_VdqWpdR, f64_to_i64_round_to_zero)
AVX512_CVT64_TO_64(VCVTPD2UQQ_VdqWpdR, f64_to_ui64)
AVX512_CVT64_TO_64(VCVTTPD2UQQ_VdqWpdR, f64_to_ui64_round_to_zero)
AVX512_CVT64_TO_64(VCVTQQ2PD_VpdWdqR, i64_to_f64)
AVX512_CVT64_TO_64(VCVTUQQ2PD_VpdWdqR, ui64_to_f64)

#define AVX512_CVT64_TO_64_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
    Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());                                       \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {              \
      if (opmask & mask)                                                                    \
        op.vmm64u(n) = (func)(op.vmm64u(n), &status);                                       \
      else                                                                                  \
        op.vmm64u(n) = 0;                                                                   \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    if (! i->isZeroMasking()) {                                                             \
      for (unsigned n=0; n < len; n++, opmask >>= 2)                                        \
        xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &op.vmm128(n), opmask);             \
      BX_CLEAR_AVX_REGZ(i->dst(), len);                                                     \
    }                                                                                       \
    else {                                                                                  \
      BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                 \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT64_TO_64_MASK(VCVTPD2QQ_MASK_VdqWpdR, f64_to_i64)
AVX512_CVT64_TO_64_MASK(VCVTTPD2QQ_MASK_VdqWpdR, f64_to_i64_round_to_zero)
AVX512_CVT64_TO_64_MASK(VCVTPD2UQQ_MASK_VdqWpdR, f64_to_ui64)
AVX512_CVT64_TO_64_MASK(VCVTTPD2UQQ_MASK_VdqWpdR, f64_to_ui64_round_to_zero)
AVX512_CVT64_TO_64_MASK(VCVTQQ2PD_MASK_VpdWdqR, i64_to_f64)
AVX512_CVT64_TO_64_MASK(VCVTUQQ2PD_MASK_VpdWdqR, ui64_to_f64)

#define AVX512_CVT32_TO_64(HANDLER, func)                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.vmm64u(n) = (func)(op.ymm32u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                       \
                                                                                            \
    BX_WRITE_AVX_REGZ(i->dst(), result, len);                                               \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_CVT32_TO_64(VCVTPS2QQ_VdqWpsR, f32_to_i64)
AVX512_CVT32_TO_64(VCVTTPS2QQ_VdqWpsR, f32_to_i64_round_to_zero)
AVX512_CVT32_TO_64(VCVTPS2UQQ_VdqWpsR, f32_to_ui64)
AVX512_CVT32_TO_64(VCVTTPS2UQQ_VdqWpsR, f32_to_ui64_round_to_zero)

#define AVX512_CVT32_TO_64_MASK(HANDLER, func)                                              \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                     \
    unsigned opmask = BX_READ_8BIT_OPMASK(i->opmask());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                          \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0, tmp_mask = opmask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {   \
      if (tmp_mask & 0x1)                                                                   \
        result.vmm64u(n) = (func)(op.ymm32u(n), &status);                                   \
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

AVX512_CVT32_TO_64_MASK(VCVTPS2PD_MASK_VpdWpsR, f32_to_f64)
AVX512_CVT32_TO_64_MASK(VCVTPS2QQ_MASK_VdqWpsR, f32_to_i64)
AVX512_CVT32_TO_64_MASK(VCVTTPS2QQ_MASK_VdqWpsR, f32_to_i64_round_to_zero)
AVX512_CVT32_TO_64_MASK(VCVTPS2UQQ_MASK_VdqWpsR, f32_to_ui64)
AVX512_CVT32_TO_64_MASK(VCVTTPS2UQQ_MASK_VdqWpsR, f32_to_ui64_round_to_zero)

// DQ2PD is a special case because it doesn't require MXCSR
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUDQ2PD_MASK_VpdWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  BxPackedAvxRegister result;
  if (i->isZeroMasking())
    result.clear();
  else
    result = BX_READ_AVX_REG(i->dst());

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      result.vmm64u(n) = ui32_to_f64(op.ymm32u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

// DQ2PD is a special case because it doesn't require MXCSR
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTDQ2PD_MASK_VpdWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  BxPackedAvxRegister result;
  if (i->isZeroMasking())
    result.clear();
  else
    result = BX_READ_AVX_REG(i->dst());

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0, mask = 0x1; n < QWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      result.vmm64u(n) = i32_to_f64(op.ymm32s(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

// not masked

// DQ2PD is a special case because it doesn't require MXCSR
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTUDQ2PD_VpdWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    result.vmm64u(n) = ui32_to_f64(op.ymm32u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

// float16 convert

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPH2PS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  BxPackedAvxRegister result;
  if (i->isZeroMasking())
    result.clear();
  else
    result = BX_READ_AVX_REG(i->dst());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  status.denormals_are_zeros = 0; // ignore MXCSR.DAZ
  // no denormal exception is reported on MXCSR
  status.float_suppress_exception = float_flag_denormal;

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      result.vmm32u(n) = f16_to_f32(op.ymm16u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PH_MASK_WpsVpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), dst = BX_READ_AVX_REG(i->dst());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  unsigned len = i->getVL();

  Bit8u control = i->Ib();

  status.flush_underflow_to_zero = 0; // ignore MXCSR.FUZ
  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmm16u(n) = f32_to_f16(op.vmm32u(n), &status);
    else if (i->isZeroMasking())
      dst.vmm16u(n) = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), dst.vmm64u(0));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), dst, len >> 1); // write half vector
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PH_MASK_WpsVpsIbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  unsigned len = i->getVL();

  Bit8u control = i->Ib();

  status.flush_underflow_to_zero = 0; // ignore MXCSR.FUZ
  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());
  opmask &= CUT_OPMASK_TO(DWORD_ELEMENTS(len));

  for (unsigned n=0, mask = 0x1; n < DWORD_ELEMENTS(len); n++, mask <<= 1) {
    if (opmask & mask)
      result.vmm16u(n) = f32_to_f16(op.vmm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store16(i, eaddr, &result, opmask);

  BX_NEXT_INSTR(i);
}

#endif
