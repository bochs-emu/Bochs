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

extern float_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr);

/* Opcode: VEX.F3.0F 2A (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = i32_to_f32(BX_READ_32BIT_REG(i->src2()), &status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F 2A (VEX.W=1) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSI2SS_VssEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm32u(0) = i64_to_f32(BX_READ_64BIT_REG(i->src2()), &status);

  check_exceptionsSSE(get_exception_flags(status));

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

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  op1.xmm64u(0) = i64_to_f64(BX_READ_64BIT_REG(i->src2()), &status);

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.0F 5A (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PD_VpdWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister result;
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
     result.vmm64u(n) = f32_to_f64(op.ymm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F 5A (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPD2PS_VpsWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    result.vmm32u(n) = f64_to_f32(op.vmm64u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); // write half vector
  }

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSS2SD_VsdWssR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm64u(0) = f32_to_f64(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F3.0F 5A (VEX.W ignore) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTSD2SS_VssWsdR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  op1.xmm32u(0) = f64_to_f32(op2, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), op1);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTDQ2PS_VpsWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.vmm32u(n) = i32_to_f32(op.vmm32s(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.66.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.vmm32s(n) = f32_to_i32(op.vmm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.NDS.F3.0F 5B (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    op.vmm32s(n) = f32_to_i32_round_to_zero(op.vmm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTPD2DQ_VdqWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    result.vmm32s(n) = f64_to_i32_round_to_zero(op.vmm64u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); // write half vector
  }

  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.F2.0F.E6 (VEX.W ignore, VEX.VVV #UD) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPD2DQ_VdqWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    result.vmm32s(n) = f64_to_i32(op.vmm64u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  if (len == BX_VL128) {
    BX_WRITE_XMM_REG_LO_QWORD_CLEAR_HIGH(i->dst(), result.vmm64u(0));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len >> 1); // write half vector
  }

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

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  status.denormals_are_zeros = 0; // ignore MXCSR.DAZ
  // no denormal exception is reported on MXCSR
  status.float_suppress_exception = float_flag_denormal;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
     result.vmm32u(n) = f16_to_f32(op.ymm16u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

/* Opcode: VEX.66.0F.3A.1D (VEX.W=0) */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTPS2PH_WpsVpsIb(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  unsigned len = i->getVL();

  Bit8u control = i->Ib();

  status.flush_underflow_to_zero = 0; // ignore MXCSR.FUZ
  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    result.vmm16u(n) = f32_to_f16(op.vmm32u(n), &status);
  }

  check_exceptionsSSE(get_exception_flags(status));

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
