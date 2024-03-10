/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2023 Stanislav Shwartsman
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

#if BX_CPU_LEVEL >= 6

#include "softfloat3e/include/softfloat-compare.h"

#include "simd_pfp.h"
#include "simd_int.h"

void BX_CPU_C::check_exceptionsSSE(int exceptions_flags)
{
  exceptions_flags &= MXCSR_EXCEPTIONS;
  int unmasked = ~(MXCSR.get_exceptions_masks()) & exceptions_flags;
  // unmasked pre-computational exception detected (#IA, #DE or #DZ)
  if (unmasked & 0x7) exceptions_flags &= 0x7;
  MXCSR.set_exceptions(exceptions_flags);

  if (unmasked)
  {
     if(BX_CPU_THIS_PTR cr4.get_OSXMMEXCPT())
        exception(BX_XM_EXCEPTION, 0);
     else
        exception(BX_UD_EXCEPTION, 0);
  }
}

float_status_t mxcsr_to_softfloat_status_word(bx_mxcsr_t mxcsr)
{
  float_status_t status;

  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = mxcsr.get_rounding_mode();
  // if underflow is masked and FUZ is 1, set it to 1, else to 0
  status.flush_underflow_to_zero =
       (mxcsr.get_flush_masked_underflow() && mxcsr.get_UM()) ? 1 : 0;
  status.float_exception_masks = mxcsr.get_exceptions_masks();
  status.float_suppress_exception = 0;
  status.denormals_are_zeros = mxcsr.get_DAZ();

  return status;
}

void mxcsr_to_softfloat_status_word_imm_override(float_status_t &status, Bit8u control)
{
  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status.float_rounding_mode = control & 0x3;
  // ignore precision exception result
  if (control & 0x8)
    status.float_suppress_exception |= float_flag_inexact;
}

/* Comparison predicate for CMPSS/CMPPS instructions */
static float32_compare_method compare32[8] = {
  f32_eq_ordered_quiet,
  f32_lt_ordered_signalling,
  f32_le_ordered_signalling,
  f32_unordered_quiet,
  f32_neq_unordered_quiet,
  f32_nlt_unordered_signalling,
  f32_nle_unordered_signalling,
  f32_ordered_quiet
};

/* Comparison predicate for CMPSD/CMPPD instructions */
static float64_compare_method compare64[8] = {
  f64_eq_ordered_quiet,
  f64_lt_ordered_signalling,
  f64_le_ordered_signalling,
  f64_unordered_quiet,
  f64_neq_unordered_quiet,
  f64_nlt_unordered_signalling,
  f64_nle_unordered_signalling,
  f64_ordered_quiet
};

#endif // BX_CPU_LEVEL >= 6

/*
 * Opcode: 0F 2A
 * Convert two 32bit signed integers from MMX/MEM to two single precision FP
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PS_VpsQqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXUD0(op) = i32_to_f32(MMXSD0(op), &status);
  MMXUD1(op) = i32_to_f32(MMXSD1(op), &status);

  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), MMXUQ(op));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PS_VpsQqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  // do not cause transition to MMX state because no MMX register touched
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  BxPackedMmxRegister op = read_virtual_qword(i->seg(), eaddr);

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXUD0(op) = i32_to_f32(MMXSD0(op), &status);
  MMXUD1(op) = i32_to_f32(MMXSD1(op), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), MMXUQ(op));
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 2A
 * Convert two 32bit signed integers from MMX/MEM to two double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PD_VpdQqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister result;

  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();
  prepareFPU2MMX(); /* cause FPU2MMX state transition */

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->src());

  result.xmm64u(0) = i32_to_f64(MMXSD0(op));
  result.xmm64u(1) = i32_to_f64(MMXSD1(op));

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PD_VpdQqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister result;

  // do not cause transition to MMX state because no MMX register touched
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  BxPackedMmxRegister op = read_virtual_qword(i->seg(), eaddr);

  result.xmm64u(0) = i32_to_f64(MMXSD0(op));
  result.xmm64u(1) = i32_to_f64(MMXSD1(op));

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F 2A
 * Convert one 32bit signed integer to one double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SD_VsdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 result = i32_to_f64(BX_READ_32BIT_REG(i->src()));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SD_VsdEqR(bxInstruction_c *i)
{
  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  float64 result = i64_to_f64(BX_READ_64BIT_REG(i->src()), &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), result);

  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: F3 0F 2A
 * Convert one 32bit signed integer to one single precision FP
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SS_VssEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  float32 result = i32_to_f32(BX_READ_32BIT_REG(i->src()), &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SS_VssEqR(bxInstruction_c *i)
{
  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  float32 result = i64_to_f32(BX_READ_64BIT_REG(i->src()), &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), result);

  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: 0F 2C
 * Convert two single precision FP numbers to two signed doubleword integers
 * in MMX using truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXSD0(op) = f32_to_i32_round_to_zero(MMXUD0(op), &status);
  MMXSD1(op) = f32_to_i32_round_to_zero(MMXUD1(op), &status);

  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 2C
 * Convert two double precision FP numbers to two signed doubleword integers
 * in MMX using truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();

  BxPackedXmmRegister op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

    if (BX_CPU_THIS_PTR mxcsr.get_MM())
      read_virtual_xmmword(i->seg(), eaddr, &op);
    else
      read_virtual_xmmword_aligned(i->seg(), eaddr, &op);
  }

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXSD0(result) = f64_to_i32_round_to_zero(op.xmm64u(0), &status);
  MMXSD1(result) = f64_to_i32_round_to_zero(op.xmm64u(1), &status);

  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F 2C
 * Convert one double precision FP number to doubleword integer using
 * truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTSD2SI_GdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f64_to_i32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTSD2SI_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f64_to_i64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: F3 0F 2C
 * Convert one single precision FP number to doubleword integer using
 * truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTSS2SI_GdWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f32_to_i32_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTSS2SI_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f32_to_i64_round_to_zero(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: 0F 2D
 * Convert two single precision FP numbers to two signed doubleword integers
 * in MMX register. When a conversion is inexact, the value returned is
 * rounded according to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();

  BxPackedMmxRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    /* pointer, segment address pair */
    op = read_virtual_qword(i->seg(), eaddr);
  }

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXSD0(op) = f32_to_i32(MMXUD0(op), &status);
  MMXSD1(op) = f32_to_i32(MMXUD1(op), &status);

  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_MMX_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 2D
 * Convert two double precision FP numbers to two signed doubleword integers
 * in MMX register. When a conversion is inexact, the value returned is
 * rounded according to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* check floating point status word for a pending FPU exceptions */
  FPU_check_pending_exceptions();

  BxPackedXmmRegister op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);

#if BX_SUPPORT_MISALIGNED_SSE
    if (BX_CPU_THIS_PTR mxcsr.get_MM())
      read_virtual_xmmword(i->seg(), eaddr, &op);
    else
#endif
      read_virtual_xmmword_aligned(i->seg(), eaddr, &op);
  }

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  MMXSD0(result) = f64_to_i32(op.xmm64u(0), &status);
  MMXSD1(result) = f64_to_i32(op.xmm64u(1), &status);

  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_MMX_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F 2D
 * Convert one double precision FP number to doubleword integer
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSD2SI_GdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f64_to_i32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSD2SI_GqWsdR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f64_to_i64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: F3 0F 2D
 * Convert one single precision FP number to doubleword integer.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSS2SI_GdWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit32s result = f32_to_i32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSS2SI_GqWssR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  Bit64s result = f32_to_i64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) result);
  BX_NEXT_INSTR(i);
}
#endif

/*
 * Opcode: 0F 5A
 * Convert two single precision FP numbers to two double precision FP numbers
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPS2PD_VpdWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister result;
  BxPackedRegister op;

  // use packed register as 64-bit value with convinient accessors
  op.u64 = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  result.xmm64u(0) = f32_to_f64(op.u32(0), &status);
  result.xmm64u(1) = f32_to_f64(op.u32(1), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 5A
 * Convert two double precision FP numbers to two single precision FP.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPD2PS_VpsWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32u(0) = f64_to_f32(op.xmm64u(0), &status);
  op.xmm32u(1) = f64_to_f32(op.xmm64u(1), &status);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F 5A
 * Convert one double precision FP number to one single precision FP.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSD2SS_VssWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  float32 result = f64_to_f32(op, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F3 0F 5A
 * Convert one single precision FP number to one double precision FP.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSS2SD_VsdWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  float64 result = f32_to_f64(op, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 0F 5B
 * Convert four signed integers to four single precision FP numbers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTDQ2PS_VpsWdqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32u(0) = i32_to_f32(op.xmm32s(0), &status);
  op.xmm32u(1) = i32_to_f32(op.xmm32s(1), &status);
  op.xmm32u(2) = i32_to_f32(op.xmm32s(2), &status);
  op.xmm32u(3) = i32_to_f32(op.xmm32s(3), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 5B
 * Convert four single precision FP to four doubleword integers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32s(0) = f32_to_i32(op.xmm32u(0), &status);
  op.xmm32s(1) = f32_to_i32(op.xmm32u(1), &status);
  op.xmm32s(2) = f32_to_i32(op.xmm32u(2), &status);
  op.xmm32s(3) = f32_to_i32(op.xmm32u(3), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F3 0F 5B
 * Convert four single precision FP to four doubleword integers using
 * truncation if the conversion is inexact.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTPS2DQ_VdqWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32s(0) = f32_to_i32_round_to_zero(op.xmm32u(0), &status);
  op.xmm32s(1) = f32_to_i32_round_to_zero(op.xmm32u(1), &status);
  op.xmm32s(2) = f32_to_i32_round_to_zero(op.xmm32u(2), &status);
  op.xmm32s(3) = f32_to_i32_round_to_zero(op.xmm32u(3), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F E6
 * Convert two double precision FP to two signed doubleword integers using
 * truncation if the conversion is inexact.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTPD2DQ_VqWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32s(0) = f64_to_i32_round_to_zero(op.xmm64u(0), &status);
  op.xmm32s(1) = f64_to_i32_round_to_zero(op.xmm64u(1), &status);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F E6
 * Convert two double precision FP to two signed doubleword integers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPD2DQ_VqWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  op.xmm32s(0) = f64_to_i32(op.xmm64u(0), &status);
  op.xmm32s(1) = f64_to_i32(op.xmm64u(1), &status);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F3 0F E6
 * Convert two 32bit signed integers from XMM/MEM to two double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTDQ2PD_VpdWqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister result;
  BxPackedRegister op;

  // use packed register as 64-bit value with convinient accessors
  op.u64 = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm64u(0) = i32_to_f64(op.s32(0));
  result.xmm64u(1) = i32_to_f64(op.s32(1));

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 0F 2E
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UCOMISS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f32_compare_quiet(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 2E
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UCOMISD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f64_compare_quiet(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 0F 2F
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::COMISS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f32_compare(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F 2F
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::COMISD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  softfloat_status_word_rc_override(status, i);
  int rc = f64_compare(op1, op2, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F 51
 * Square Root scalar double precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  op = f64_sqrt(op, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F3 0F 51
 * Square Root scalar single precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  op = f32_sqrt(op, &status);
  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6

#define SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(HANDLER, func)                                          \
  /* SSE packed shift instruction */                                                            \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                           \
  {                                                                                             \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_DWORD(i->src()); \
                                                                                                \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                              \
    op1 = (func)(op1, op2, &status);                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                           \
    BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);                                                   \
    BX_NEXT_INSTR(i);                                                                           \
  }                                                                                             \

#else

#define SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(HANDLER, func)                                          \
  /* SSE instruction with two src operands */                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)                          \
  {                                                                                             \
    BX_NEXT_INSTR(i);                                                                           \
  }

#endif

SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(ADDSS_VssWssR, f32_add);
SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(SUBSS_VssWssR, f32_sub);
SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(MULSS_VssWssR, f32_mul);
SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(DIVSS_VssWssR, f32_div);
SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(MINSS_VssWssR, f32_min);
SSE_SCALAR_SINGLE_FP_CPU_LEVEL6(MAXSS_VssWssR, f32_max);

#if BX_CPU_LEVEL >= 6

#define SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(HANDLER, func)                                          \
  /* SSE packed shift instruction */                                                            \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                           \
  {                                                                                             \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_QWORD(i->src()); \
                                                                                                \
    float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);                              \
    op1 = (func)(op1, op2, &status);                                                            \
    check_exceptionsSSE(get_exception_flags(status));                                           \
    BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);                                                   \
    BX_NEXT_INSTR(i);                                                                           \
  }                                                                                             \

#else

#define SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(HANDLER, func)                                          \
  /* SSE instruction with two src operands */                                                   \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)                          \
  {                                                                                             \
    BX_NEXT_INSTR(i);                                                                           \
  }

#endif

SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(ADDSD_VsdWsdR, f64_add);
SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(SUBSD_VsdWsdR, f64_sub);
SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(MULSD_VsdWsdR, f64_mul);
SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(DIVSD_VsdWsdR, f64_div);
SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(MINSD_VsdWsdR, f64_min);
SSE_SCALAR_DOUBLE_FP_CPU_LEVEL6(MAXSD_VsdWsdR, f64_max);

/*
 * Opcode: 0F C2
 * Compare packed single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 7;

  op1.xmm32u(0) = compare32[ib](op1.xmm32u(0), op2.xmm32u(0), &status) ? 0xFFFFFFFF : 0;
  op1.xmm32u(1) = compare32[ib](op1.xmm32u(1), op2.xmm32u(1), &status) ? 0xFFFFFFFF : 0;
  op1.xmm32u(2) = compare32[ib](op1.xmm32u(2), op2.xmm32u(2), &status) ? 0xFFFFFFFF : 0;
  op1.xmm32u(3) = compare32[ib](op1.xmm32u(3), op2.xmm32u(3), &status) ? 0xFFFFFFFF : 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: 66 0F C2
 * Compare packed double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 7;

  op1.xmm64u(0) = compare64[ib](op1.xmm64u(0), op2.xmm64u(0), &status) ?
     BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  op1.xmm64u(1) = compare64[ib](op1.xmm64u(1), op2.xmm64u(1), &status) ?
     BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F2 0F C2
 * Compare double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPSD_VsdWsdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 7;

  if(compare64[ib](op1, op2, &status)) {
    op1 = BX_CONST64(0xFFFFFFFFFFFFFFFF);
  } else {
    op1 = 0;
  }

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

/*
 * Opcode: F3 0F C2
 * Compare single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPSS_VssWssIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->dst()), op2 = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  int ib = i->Ib() & 7;

  op1 = compare32[ib](op1, op2, &status) ? 0xFFFFFFFF : 0;

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6

/* 66 0F 3A 08 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDPS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op.xmm32u(0) = f32_roundToInt(op.xmm32u(0), &status);
  op.xmm32u(1) = f32_roundToInt(op.xmm32u(1), &status);
  op.xmm32u(2) = f32_roundToInt(op.xmm32u(2), &status);
  op.xmm32u(3) = f32_roundToInt(op.xmm32u(3), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 09 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDPD_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op.xmm64u(0) = f64_roundToInt(op.xmm64u(0), &status);
  op.xmm64u(1) = f64_roundToInt(op.xmm64u(1), &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 0A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDSS_VssWssIbR(bxInstruction_c *i)
{
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op = f32_roundToInt(op, &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 0B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDSD_VsdWsdIbR(bxInstruction_c *i)
{
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->src());

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);
  mxcsr_to_softfloat_status_word_imm_override(status, i->Ib());

  op = f64_roundToInt(op, &status);

  check_exceptionsSSE(get_exception_flags(status));
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op);

  BX_NEXT_INSTR(i);
}

/* Opcode: 66 0F 3A 40
 * Selectively multiply packed SP floating-point values from xmm1 with
 * packed SP floating-point values from xmm2, add and selectively
 * store the packed SP floating-point values or zero values to xmm1
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DPPS_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src());
  Bit8u mask = i->Ib();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  // op1: [A, B, C, D]
  // op2: [E, F, G, H]

  // after multiplication: op1 = [EA, BF, CG, DH]
  xmm_mulps_mask(&op1, &op2, status, mask >> 4);
  check_exceptionsSSE(get_exception_flags(status));

  // shuffle op2 = [BF, AE, DH, CG]
  xmm_shufps(&op2, &op1, &op1, 0xb1);

  // op2 = [(BF+AE), (AE+BF), (DH+CG), (CG+DH)]
  xmm_addps(&op2, &op1, status);
  check_exceptionsSSE(get_exception_flags(status));

  // shuffle op1 = [(DH+CG), (CG+DH), (BF+AE), (AE+BF)]
  xmm_shufpd(&op1, &op2, &op2, 0x1);

  // op2 = [(BF+AE)+(DH+CG), (AE+BF)+(CG+DH), (DH+CG)+(BF+AE), (CG+DH)+(AE+BF)]
  xmm_addps_mask(&op2, &op1, status, mask);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REG(i->dst(), op2);

  BX_NEXT_INSTR(i);
}

/* Opcode: 66 0F 3A 41
 * Selectively multiply packed DP floating-point values from xmm1 with
 * packed DP floating-point values from xmm2, add and selectively
 * store the packed DP floating-point values or zero values to xmm1
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DPPD_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src2());
  Bit8u mask = i->Ib();

  float_status_t status = mxcsr_to_softfloat_status_word(MXCSR);

  // op1: [A, B]
  // op2: [C, D]

  // after multiplication: op1 = [AC, BD]
  xmm_mulpd_mask(&op1, &op2, status, mask >> 4);
  check_exceptionsSSE(get_exception_flags(status));

  // shuffle op2 = [BD, AC]
  xmm_shufpd(&op2, &op1, &op1, 0x1);

  // op1 = [AC+BD, BD+AC]
  xmm_addpd_mask(&op1, &op2, status, mask);
  check_exceptionsSSE(get_exception_flags(status));

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

#endif // BX_CPU_LEVEL >= 6
