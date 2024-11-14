/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2018 Stanislav Shwartsman
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

#include "softfloat3e/include/softfloat.h"

extern softfloat_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

/* Opcode: VEX.F3.0F 2A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = i32_to_f32(BX_READ_32BIT_REG(i->src2()), &status);

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F 2A (VEX.W=1) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = i64_to_f32(BX_READ_64BIT_REG(i->src2()), &status);

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F2.0F 2A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SD_VsdEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  op1.xmm64u(0) = i32_to_f64(BX_READ_32BIT_REG(i->src2()));
  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F2.0F 2A (VEX.W=1) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SD_VsdEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = i64_to_f64(BX_READ_64BIT_REG(i->src2()), &status);

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

// packed

#define AVX_CVT32_TO_64(HANDLER, func)                                                      \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister result;                                                             \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                      \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.vmm64u(n) = (func)(op.ymm32u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(softfloat_getExceptionFlags(&status));                              \
                                                                                            \
    BX_WRITE_AVX_REGZ(i->dst(), result, len);                                               \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_CVT32_TO_64(VCVTPS2PD_VpdWpsR, f32_to_f64)
#if BX_SUPPORT_EVEX
AVX_CVT32_TO_64(VCVTPS2QQ_VdqWpsR, f32_to_i64)
AVX_CVT32_TO_64(VCVTTPS2QQ_VdqWpsR, f32_to_i64_round_to_zero)
AVX_CVT32_TO_64(VCVTTPS2QQS_VdqWpsR, f32_to_i64_round_to_zero_saturate) // AVX 10.2
AVX_CVT32_TO_64(VCVTPS2UQQ_VdqWpsR, f32_to_ui64)
AVX_CVT32_TO_64(VCVTTPS2UQQ_VdqWpsR, f32_to_ui64_round_to_zero)
AVX_CVT32_TO_64(VCVTTPS2UQQS_VdqWpsR, f32_to_ui64_round_to_zero_saturate) // AVX 10.2
#endif

#define AVX_CVT64_TO_32(HANDLER, func)                                                      \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;                             \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                      \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {                                      \
      result.vmm32u(n) = (func)(op.vmm64u(n), &status);                                     \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(softfloat_getExceptionFlags(&status));                              \
                                                                                            \
    if (len == BX_VL128) {                                                                  \
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));                     \
    } else {                                                                                \
      BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); /* write half vector */                \
    }                                                                                       \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_CVT64_TO_32(VCVTPD2PS_VpsWpdR, f64_to_f32)
AVX_CVT64_TO_32(VCVTPD2DQ_VdqWpdR, f64_to_i32)
AVX_CVT64_TO_32(VCVTTPD2DQ_VdqWpdR, f64_to_i32_round_to_zero)
#if BX_SUPPORT_EVEX
AVX_CVT64_TO_32(VCVTTPD2DQS_VdqWpdR, f64_to_i32_round_to_zero_saturate) // AVX 10.2
AVX_CVT64_TO_32(VCVTPD2UDQ_VdqWpdR, f64_to_ui32)
AVX_CVT64_TO_32(VCVTTPD2UDQ_VdqWpdR, f64_to_ui32_round_to_zero)
AVX_CVT64_TO_32(VCVTTPD2UDQS_VdqWpdR, f64_to_ui32_round_to_zero_saturate) // AVX 10.2
AVX_CVT64_TO_32(VCVTQQ2PS_VpsWdqR, i64_to_f32)
AVX_CVT64_TO_32(VCVTUQQ2PS_VpsWdqR, ui64_to_f32)
#endif

#define AVX_CVT32_TO_32(HANDLER, func)                                                      \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                       \
  {                                                                                         \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                     \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                      \
    softfloat_status_word_rc_override(status, i);                                           \
                                                                                            \
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {                                      \
      op.vmm32u(n) = (func)(op.vmm32u(n), &status);                                         \
    }                                                                                       \
                                                                                            \
    check_exceptionsSSE(softfloat_getExceptionFlags(&status));                              \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                   \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_CVT32_TO_32(VCVTDQ2PS_VpsWdqR, i32_to_f32)
AVX_CVT32_TO_32(VCVTPS2DQ_VdqWpsR, f32_to_i32)
AVX_CVT32_TO_32(VCVTTPS2DQ_VdqWpsR, f32_to_i32_round_to_zero)
#if BX_SUPPORT_EVEX
AVX_CVT32_TO_32(VCVTTPS2DQS_VdqWpsR, f32_to_i32_round_to_zero_saturate) // AVX 10.2
AVX_CVT32_TO_32(VCVTPS2UDQ_VdqWpsR, f32_to_ui32)
AVX_CVT32_TO_32(VCVTTPS2UDQ_VdqWpsR, f32_to_ui32_round_to_zero)
AVX_CVT32_TO_32(VCVTTPS2UDQS_VdqWpsR, f32_to_ui32_round_to_zero_saturate) // AVX 10.2
AVX_CVT32_TO_32(VCVTUDQ2PS_VpsWdqR, ui32_to_f32)
AVX_CVT32_TO_32(VCVTPS2IBS_V8bWpsR, f32_to_i8_saturate) // AVX 10.2
AVX_CVT32_TO_32(VCVTPS2IUBS_V8bWpsR, f32_to_ui8_saturate) // AVX 10.2
AVX_CVT32_TO_32(VCVTTPS2IBS_V8bWpsR, f32_to_i8_round_to_zero_saturate) // AVX 10.2
AVX_CVT32_TO_32(VCVTTPS2IUBS_V8bWpsR, f32_to_ui8_round_to_zero_saturate) // AVX 10.2
#endif

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SD_VsdWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = f32_to_f64(op2, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SS_VssWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = f64_to_f32(op2, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTDQ2PD_VpdWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
     result.vmm64u(n) = i32_to_f64(op.ymm32s(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

// float16 convert

/* Opcode: VEX.66.0F.3A.13 (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPH2PS_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  status.softfloat_denormals_are_zeros = 0; // ignore MXCSR.DAZ
  // no denormal exception is reported on MXCSR
  status.softfloat_suppressException = softfloat_flag_denormal;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
     result.vmm32u(n) = f16_to_f32(op.ymm16u(n), &status);
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.1D (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PH_WpsVpsIb(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  unsigned len = i->getVL();

  Bit8u control = i->Ib();

  status.softfloat_flush_underflow_to_zero = 0; // ignore MXCSR.FUZ
  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.softfloat_roundingMode = control & 0x3;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    result.vmm16u(n) = f32_to_f16(op.vmm32u(n), &status);
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  if (i->modC0()) {
    if (len == BX_VL128) {
      BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
    }
    else {
      BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); // write half vector
    }
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

#if BX_SUPPORT_EVEX
    if (len == BX_VL512)
      write_virtual_ymmword(i->seg(), eaddr, &result.vmm256(0));
    else
#endif
    {
      if (len == BX_VL256)
        write_virtual_xmmword(i->seg(), eaddr, &result.vmm128(0));
      else
        write_virtual_qword(i->seg(), eaddr, result.vmm64u(0));
    }
  }

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_AVX
