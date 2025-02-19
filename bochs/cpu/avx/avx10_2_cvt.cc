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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVT2PS2PHX_MASK_VphHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister src1 = BX_READ_AVX_REG(i->src1()), src2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();
  unsigned num_elements_from_source = DWORD_ELEMENTS(len);
  unsigned n = 0;

  // This instruction updates MXCSR as if all MXCSR numerical exceptions flags are masked and does not
  // generate floating point exceptions. MXCSR (and EVEX embedded rounding) determine the rounding
  // mode. Input FP32 denormals are affected by MXCSR.DAZ but output FP16 denormals are not affected by
  // MXCSR.FUZ and not flushed to zero
  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  status.softfloat_flush_underflow_to_zero = 0; // ignore MXCSR.FUZ
  status.softfloat_exceptionMasks = softfloat_all_exceptions_mask;
  status.softfloat_suppressException = softfloat_all_exceptions_mask;

  Bit32u mask, opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;

  // first half of the elements come from src2
  for (mask = 0x1; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmm16u(n) = f32_to_f16(src2.vmm32u(n), &status);
    else if (i->isZeroMasking())
      dst.vmm16u(n) = 0;
  }

  // second half of the elements come from src1
  for (n = 0; n < num_elements_from_source; n++, mask <<= 1) {
    if (opmask & mask)
      dst.vmm16u(n + num_elements_from_source) = f32_to_f16(src1.vmm32u(n), &status);
    else if (i->isZeroMasking())
      dst.vmm16u(n + num_elements_from_source) = 0;
  }

  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2USIS_GdWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f64_to_ui32_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2USIS_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f64_to_ui64_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2USIS_GdWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32u result = f32_to_ui32_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_32BIT_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2USIS_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64u result = f32_to_ui64_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_64BIT_REG(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2SIS_GdWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f64_to_i32_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSD2SIS_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f64_to_i64_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2SIS_GdWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f32_to_i32_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCVTTSS2SIS_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  softfloat_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f32_to_i64_round_to_zero_saturate(op, &status);
  check_exceptionsSSE(softfloat_getExceptionFlags(&status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}

#endif
