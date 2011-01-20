/////////////////////////////////////////////////////////////////////////
// $Id: sse_pfp.cc,v 1.78 2011-01-20 16:24:42 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2011 Stanislav Shwartsman
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

#include "fpu/softfloat-specialize.h"

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

BX_CPP_INLINE void mxcsr_to_softfloat_status_word(float_status_t &status, bx_mxcsr_t mxcsr)
{
  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = mxcsr.get_rounding_mode();
  // if underflow is masked and FUZ is 1, set it to 1, else to 0
  status.flush_underflow_to_zero =
       (mxcsr.get_flush_masked_underflow() && mxcsr.get_UM()) ? 1 : 0;
  status.float_exception_masks = mxcsr.get_exceptions_masks();
}

/* Comparison predicate for CMPSS/CMPPS instructions */
static float32_compare_method compare32[4] = {
  float32_eq,
  float32_lt,
  float32_le,
  float32_unordered
};

/* Comparison predicate for CMPSD/CMPPD instructions */
static float64_compare_method compare64[4] = {
  float64_eq,
  float64_lt,
  float64_le,
  float64_unordered
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

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  MMXUD0(op) = int32_to_float32(MMXUD0(op), status_word);
  MMXUD1(op) = int32_to_float32(MMXUD1(op), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), MMXUQ(op));
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PS_VpsQqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedMmxRegister op;

  // do not cause transition to MMX state if no MMX register touched
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  MMXUD0(op) = int32_to_float32(MMXUD0(op), status_word);
  MMXUD1(op) = int32_to_float32(MMXUD1(op), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), MMXUQ(op));
#endif
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

  BxPackedMmxRegister op = BX_READ_MMX_REG(i->rm());

  result.xmm64u(0) = int32_to_float64(MMXUD0(op));
  result.xmm64u(1) = int32_to_float64(MMXUD1(op));

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPI2PD_VpdQqM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedMmxRegister op;
  BxPackedXmmRegister result;

  // do not cause transition to MMX state if no MMX register touched
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);

  result.xmm64u(0) = int32_to_float64(MMXUD0(op));
  result.xmm64u(1) = int32_to_float64(MMXUD1(op));

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/*
 * Opcode: F2 0F 2A
 * Convert one 32bit signed integer to one double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SD_VsdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 result;

#if BX_SUPPORT_X86_64
  if (i->os64L()) {   /* 64 bit operand size */
    float_status_t status_word;
    mxcsr_to_softfloat_status_word(status_word, MXCSR);
    result = int64_to_float64(BX_READ_64BIT_REG(i->rm()), status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
  }
  else
#endif
  {
    result = int32_to_float64(BX_READ_32BIT_REG(i->rm()));
  }

  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SD_VsdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 result;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size */
  {
    float_status_t status_word;
    Bit64u op = read_virtual_qword_64(i->seg(), eaddr);
    mxcsr_to_softfloat_status_word(status_word, MXCSR);
    result = int64_to_float64(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
  }
  else
#endif
  {
    Bit32u op = read_virtual_dword(i->seg(), eaddr);
    result = int32_to_float64(op);
  }

  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);
#endif
}

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
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  float32 result;

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size */
    result = int64_to_float32(BX_READ_64BIT_REG(i->rm()), status_word);
  else
#endif
    result = int32_to_float32(BX_READ_32BIT_REG(i->rm()), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSI2SS_VssEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  float32 result;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size */
  {
    Bit64u op = read_virtual_qword_64(i->seg(), eaddr);
    result = int64_to_float32(op, status_word);
  }
  else
#endif
  {
    Bit32u op = read_virtual_dword(i->seg(), eaddr);
    result = int32_to_float32(op, status_word);
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);
#endif
}

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
    MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    MMXUD0(op) = float32_denormal_to_zero(MMXUD0(op));
    MMXUD1(op) = float32_denormal_to_zero(MMXUD1(op));
  }

  MMXUD0(op) = float32_to_int32_round_to_zero(MMXUD0(op), status_word);
  MMXUD1(op) = float32_to_int32_round_to_zero(MMXUD1(op), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  BX_WRITE_MMX_REG(i->nnn(), op);
#endif
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
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_MISALIGNED_SSE
    if (BX_CPU_THIS_PTR mxcsr.get_MM())
      read_virtual_dqword(i->seg(), eaddr, (Bit8u *) &op);
    else
#endif
      read_virtual_dqword_aligned(i->seg(), eaddr, (Bit8u *) &op);
  }

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  MMXUD0(result) = float64_to_int32_round_to_zero(op.xmm64u(0), status_word);
  MMXUD1(result) = float64_to_int32_round_to_zero(op.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  BX_WRITE_MMX_REG(i->nnn(), result);
#endif
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
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op = float64_denormal_to_zero(op);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float64_to_int64_round_to_zero(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else
#endif
  {
    Bit32u result = float64_to_int32_round_to_zero(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REGZ(i->nnn(), result);
  }
#endif
}

/*
 * Opcode: F3 0F 2C
 * Convert one single precision FP number to doubleword integer using
 * truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTTSS2SI_GdWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) op = float32_denormal_to_zero(op);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float32_to_int64_round_to_zero(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else
#endif
  {
    Bit32u result = float32_to_int32_round_to_zero(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REGZ(i->nnn(), result);
  }
#endif
}

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
    MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    MMXUQ(op) = read_virtual_qword(i->seg(), eaddr);
  }

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    MMXUD0(op) = float32_denormal_to_zero(MMXUD0(op));
    MMXUD1(op) = float32_denormal_to_zero(MMXUD1(op));
  }

  MMXUD0(op) = float32_to_int32(MMXUD0(op), status_word);
  MMXUD1(op) = float32_to_int32(MMXUD1(op), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  BX_WRITE_MMX_REG(i->nnn(), op);
#endif
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
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_MISALIGNED_SSE
    if (BX_CPU_THIS_PTR mxcsr.get_MM())
      read_virtual_dqword(i->seg(), eaddr, (Bit8u *) &op);
    else
#endif
      read_virtual_dqword_aligned(i->seg(), eaddr, (Bit8u *) &op);
  }

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  MMXUD0(result) = float64_to_int32(op.xmm64u(0), status_word);
  MMXUD1(result) = float64_to_int32(op.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  prepareFPU2MMX(); /* cause FPU2MMX state transition */
  BX_WRITE_MMX_REG(i->nnn(), result);
#endif
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
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float64_denormal_to_zero(op);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float64_to_int64(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else
#endif
  {
    Bit32u result = float64_to_int32(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REGZ(i->nnn(), result);
  }
#endif
}

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
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float32_denormal_to_zero(op);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float32_to_int64(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else
#endif
  {
    Bit32u result = float32_to_int32(op, status_word);
    check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REGZ(i->nnn(), result);
  }
#endif
}

/*
 * Opcode: 0F 5A
 * Convert two single precision FP numbers to two double precision FP numbers
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPS2PD_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    MMXUD0(op) = float32_denormal_to_zero(MMXUD0(op));
    MMXUD1(op) = float32_denormal_to_zero(MMXUD1(op));
  }

  result.xmm64u(0) = float32_to_float64(MMXUD0(op), status_word);
  result.xmm64u(1) = float32_to_float64(MMXUD1(op), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/*
 * Opcode: 66 0F 5A
 * Convert two double precision FP numbers to two single precision FP.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTPD2PS_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  op.xmm32u(0) = float64_to_float32(op.xmm64u(0), status_word);
  op.xmm32u(1) = float64_to_float32(op.xmm64u(1), status_word);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/*
 * Opcode: F2 0F 5A
 * Convert one double precision FP number to one single precision FP.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSD2SS_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float64_denormal_to_zero(op);
  float32 result = float64_to_float32(op, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);
#endif
}

/*
 * Opcode: F3 0F 5A
 * Convert one single precision FP number to one double precision FP.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CVTSS2SD_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float32_denormal_to_zero(op);
  float64 result = float32_to_float64(op, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);
#endif
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
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  op.xmm32u(0) = int32_to_float32(op.xmm32u(0), status_word);
  op.xmm32u(1) = int32_to_float32(op.xmm32u(1), status_word);
  op.xmm32u(2) = int32_to_float32(op.xmm32u(2), status_word);
  op.xmm32u(3) = int32_to_float32(op.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
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
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm32u(0) = float32_denormal_to_zero(op.xmm32u(0));
    op.xmm32u(1) = float32_denormal_to_zero(op.xmm32u(1));
    op.xmm32u(2) = float32_denormal_to_zero(op.xmm32u(2));
    op.xmm32u(3) = float32_denormal_to_zero(op.xmm32u(3));
  }

  op.xmm32u(0) = float32_to_int32(op.xmm32u(0), status_word);
  op.xmm32u(1) = float32_to_int32(op.xmm32u(1), status_word);
  op.xmm32u(2) = float32_to_int32(op.xmm32u(2), status_word);
  op.xmm32u(3) = float32_to_int32(op.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
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
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm32u(0) = float32_denormal_to_zero(op.xmm32u(0));
    op.xmm32u(1) = float32_denormal_to_zero(op.xmm32u(1));
    op.xmm32u(2) = float32_denormal_to_zero(op.xmm32u(2));
    op.xmm32u(3) = float32_denormal_to_zero(op.xmm32u(3));
  }

  op.xmm32u(0) = float32_to_int32_round_to_zero(op.xmm32u(0), status_word);
  op.xmm32u(1) = float32_to_int32_round_to_zero(op.xmm32u(1), status_word);
  op.xmm32u(2) = float32_to_int32_round_to_zero(op.xmm32u(2), status_word);
  op.xmm32u(3) = float32_to_int32_round_to_zero(op.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
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
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  op.xmm32u(0) = float64_to_int32_round_to_zero(op.xmm64u(0), status_word);
  op.xmm32u(1) = float64_to_int32_round_to_zero(op.xmm64u(1), status_word);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
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
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  op.xmm32u(0) = float64_to_int32(op.xmm64u(0), status_word);
  op.xmm32u(1) = float64_to_int32(op.xmm64u(1), status_word);
  op.xmm64u(1) = 0;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
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
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->rm());

  result.xmm64u(0) = int32_to_float64(MMXUD0(op));
  result.xmm64u(1) = int32_to_float64(MMXUD1(op));

  BX_WRITE_XMM_REG(i->nnn(), result);
#endif
}

/*
 * Opcode: 0F 2E
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UCOMISS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare_quiet(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif
}

/*
 * Opcode: 66 0F 2E
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::UCOMISD_VsdWsdR(bxInstruction_c *i)            	
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare_quiet(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif
}

/*
 * Opcode: 0F 2F
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::COMISS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif
}

/*
 * Opcode: 66 0F 2F
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::COMISD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_CPU_THIS_PTR write_eflags_fpu_compare(rc);
#endif
}

/*
 * Opcode: 0F 51
 * Square Root packed single precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op.xmm32u(0) = float32_denormal_to_zero(op.xmm32u(0));
    op.xmm32u(1) = float32_denormal_to_zero(op.xmm32u(1));
    op.xmm32u(2) = float32_denormal_to_zero(op.xmm32u(2));
    op.xmm32u(3) = float32_denormal_to_zero(op.xmm32u(3));
  }

  op.xmm32u(0) = float32_sqrt(op.xmm32u(0), status_word);
  op.xmm32u(1) = float32_sqrt(op.xmm32u(1), status_word);
  op.xmm32u(2) = float32_sqrt(op.xmm32u(2), status_word);
  op.xmm32u(3) = float32_sqrt(op.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/*
 * Opcode: 66 0F 51
 * Square Root packed double precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  op.xmm64u(0) = float64_sqrt(op.xmm64u(0), status_word);
  op.xmm64u(1) = float64_sqrt(op.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/*
 * Opcode: F2 0F 51
 * Square Root scalar double precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float64_denormal_to_zero(op);
  op = float64_sqrt(op, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op);
#endif
}

/*
 * Opcode: F3 0F 51
 * Square Root scalar single precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SQRTSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = float32_denormal_to_zero(op);
  op = float32_sqrt(op, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op);
#endif
}

/*
 * Opcode: 0F 58
 * Add packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_add(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(1) = float32_add(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(2) = float32_add(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(3) = float32_add(op1.xmm32u(3), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 58
 * Add packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_add(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(1) = float64_add(op1.xmm64u(1), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 58
 * Add the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  op1 = float64_add(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: F3 0F 58
 * Add the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  op1 = float32_add(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: 0F 59
 * Multiply packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_mul(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(1) = float32_mul(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(2) = float32_mul(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(3) = float32_mul(op1.xmm32u(3), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 59
 * Multiply packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_mul(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(1) = float64_mul(op1.xmm64u(1), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 59
 * Multiply the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  op1 = float64_mul(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: F3 0F 59
 * Multiply the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MULSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  op1 = float32_mul(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: 0F 5C
 * Subtract packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_sub(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(1) = float32_sub(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(2) = float32_sub(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(3) = float32_sub(op1.xmm32u(3), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 5C
 * Subtract packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_sub(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(1) = float64_sub(op1.xmm64u(1), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 5C
 * Subtract the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUBSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  op1 = float64_sub(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: F3 0F 5C
 * Subtract the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::SUBSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  op1 = float32_sub(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: 0F 5D
 * Calculate the minimum single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MINPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  rc = float32_compare(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(0) = (rc == float_relation_less) ? op1.xmm32u(0) : op2.xmm32u(0);
  rc = float32_compare(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(1) = (rc == float_relation_less) ? op1.xmm32u(1) : op2.xmm32u(1);
  rc = float32_compare(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(2) = (rc == float_relation_less) ? op1.xmm32u(2) : op2.xmm32u(2);
  rc = float32_compare(op1.xmm32u(3), op2.xmm32u(3), status_word);
  op1.xmm32u(3) = (rc == float_relation_less) ? op1.xmm32u(3) : op2.xmm32u(3);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 5D
 * Calculate the minimum double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MINPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  rc = float64_compare(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(0) = (rc == float_relation_less) ? op1.xmm64u(0) : op2.xmm64u(0);
  rc = float64_compare(op1.xmm64u(1), op2.xmm64u(1), status_word);
  op1.xmm64u(1) = (rc == float_relation_less) ? op1.xmm64u(1) : op2.xmm64u(1);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 5D
 * Calculate the minimum scalar double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MINSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(),
         (rc == float_relation_less) ? op1 : op2);
#endif
}

/*
 * Opcode: F3 0F 5D
 * Calculate the minimum scalar single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MINSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(),
         (rc == float_relation_less) ? op1 : op2);
#endif
}

/*
 * Opcode: 0F 5E
 * Divide packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DIVPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_div(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(1) = float32_div(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(2) = float32_div(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(3) = float32_div(op1.xmm32u(3), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 5E
 * Divide packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DIVPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_div(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(1) = float64_div(op1.xmm64u(1), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 5E
 * Divide the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DIVSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  op1 = float64_div(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: F3 0F 5E
 * Divide the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DIVSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  op1 = float32_div(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: 0F 5F
 * Calculate the maximum single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MAXPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  rc = float32_compare(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(0) = (rc == float_relation_greater) ? op1.xmm32u(0) : op2.xmm32u(0);
  rc = float32_compare(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(1) = (rc == float_relation_greater) ? op1.xmm32u(1) : op2.xmm32u(1);
  rc = float32_compare(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(2) = (rc == float_relation_greater) ? op1.xmm32u(2) : op2.xmm32u(2);
  rc = float32_compare(op1.xmm32u(3), op2.xmm32u(3), status_word);
  op1.xmm32u(3) = (rc == float_relation_greater) ? op1.xmm32u(3) : op2.xmm32u(3);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 5F
 * Calculate the maximum double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MAXPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  rc = float64_compare(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(0) = (rc == float_relation_greater) ? op1.xmm64u(0) : op2.xmm64u(0);
  rc = float64_compare(op1.xmm64u(1), op2.xmm64u(1), status_word);
  op1.xmm64u(1) = (rc == float_relation_greater) ? op1.xmm64u(1) : op2.xmm64u(1);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 5F
 * Calculate the maximum scalar double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MAXSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(),
         (rc == float_relation_greater) ? op1 : op2);
#endif
}

/*
 * Opcode: F3 0F 5F
 * Calculate the maxumim scalar single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::MAXSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(),
         (rc == float_relation_greater) ? op1 : op2);
#endif
}

/*
 * Opcode: 66 0F 7C
 * Add horizontally packed double precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HADDPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_add(op1.xmm64u(0), op1.xmm64u(1), status_word);
  op1.xmm64u(1) = float64_add(op2.xmm64u(0), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 7C
 * Add horizontally packed single precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HADDPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_add(op1.xmm32u(0), op1.xmm32u(1), status_word);
  op1.xmm32u(1) = float32_add(op1.xmm32u(2), op1.xmm32u(3), status_word);
  op1.xmm32u(2) = float32_add(op2.xmm32u(0), op2.xmm32u(1), status_word);
  op1.xmm32u(3) = float32_add(op2.xmm32u(2), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F 7D
 * Subtract horizontally packed double precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HSUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_sub(op1.xmm64u(0), op1.xmm64u(1), status_word);
  op1.xmm64u(1) = float64_sub(op2.xmm64u(0), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F 7D
 * Subtract horizontally packed single precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::HSUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_sub(op1.xmm32u(0), op1.xmm32u(1), status_word);
  op1.xmm32u(1) = float32_sub(op1.xmm32u(2), op1.xmm32u(3), status_word);
  op1.xmm32u(2) = float32_sub(op2.xmm32u(0), op2.xmm32u(1), status_word);
  op1.xmm32u(3) = float32_sub(op2.xmm32u(2), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 0F C2
 * Compare packed single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 7;

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  if(ib < 4)
  {
    op1.xmm32u(0) =
        compare32[ib](op1.xmm32u(0), op2.xmm32u(0), status) ? 0xFFFFFFFF : 0;
    op1.xmm32u(1) =
        compare32[ib](op1.xmm32u(1), op2.xmm32u(1), status) ? 0xFFFFFFFF : 0;
    op1.xmm32u(2) =
        compare32[ib](op1.xmm32u(2), op2.xmm32u(2), status) ? 0xFFFFFFFF : 0;
    op1.xmm32u(3) =
        compare32[ib](op1.xmm32u(3), op2.xmm32u(3), status) ? 0xFFFFFFFF : 0;
  }
  else
  {
    ib -= 4;

    op1.xmm32u(0) =
        compare32[ib](op1.xmm32u(0), op2.xmm32u(0), status) ? 0 : 0xFFFFFFFF;
    op1.xmm32u(1) =
        compare32[ib](op1.xmm32u(1), op2.xmm32u(1), status) ? 0 : 0xFFFFFFFF;
    op1.xmm32u(2) =
        compare32[ib](op1.xmm32u(2), op2.xmm32u(2), status) ? 0 : 0xFFFFFFFF;
    op1.xmm32u(3) =
        compare32[ib](op1.xmm32u(3), op2.xmm32u(3), status) ? 0 : 0xFFFFFFFF;
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F C2
 * Compare packed double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib() & 7;

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  if(ib < 4)
  {
    op1.xmm64u(0) = compare64[ib](op1.xmm64u(0), op2.xmm64u(0), status) ?
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
    op1.xmm64u(1) = compare64[ib](op1.xmm64u(1), op2.xmm64u(1), status) ?
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  }
  else
  {
    ib -= 4;

    op1.xmm64u(0) = compare64[ib](op1.xmm64u(0), op2.xmm64u(0), status) ?
       0 : BX_CONST64(0xFFFFFFFFFFFFFFFF);
    op1.xmm64u(1) = compare64[ib](op1.xmm64u(1), op2.xmm64u(1), status) ?
       0 : BX_CONST64(0xFFFFFFFFFFFFFFFF);
  }

  check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F C2
 * Compare double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPSD_VsdWsdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib() & 7;

  if (MXCSR.get_DAZ())
  {
    op1 = float64_denormal_to_zero(op1);
    op2 = float64_denormal_to_zero(op2);
  }

  if(ib < 4) {
     if(compare64[ib](op1, op2, status_word)) {
        op1 = BX_CONST64(0xFFFFFFFFFFFFFFFF);
     } else {
        op1 = 0;
     }
  } else {
     if(compare64[ib-4](op1, op2, status_word)) {
        op1 = 0;
     } else {
        op1 = BX_CONST64(0xFFFFFFFFFFFFFFFF);
     }
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: F3 0F C2
 * Compare single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CMPSS_VssWssIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib() & 7;

  if (MXCSR.get_DAZ())
  {
    op1 = float32_denormal_to_zero(op1);
    op2 = float32_denormal_to_zero(op2);
  }

  if(ib < 4) {
     if(compare32[ib](op1, op2, status_word)) {
        op1 = 0xFFFFFFFF;
     } else {
        op1 = 0;
     }
  } else {
     if(compare32[ib-4](op1, op2, status_word)) {
        op1 = 0;
     } else {
        op1 = 0xFFFFFFFF;
     }
  }

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op1);
#endif
}

/*
 * Opcode: 66 0F D0
 * Add/Subtract packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDSUBPD_VpdWpdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ())
  {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));
    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  op1.xmm64u(0) = float64_sub(op1.xmm64u(0), op2.xmm64u(0), status_word);
  op1.xmm64u(1) = float64_add(op1.xmm64u(1), op2.xmm64u(1), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/*
 * Opcode: F2 0F D0
 * Add/Substract packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ADDSUBPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2 = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  op1.xmm32u(0) = float32_sub(op1.xmm32u(0), op2.xmm32u(0), status_word);
  op1.xmm32u(1) = float32_add(op1.xmm32u(1), op2.xmm32u(1), status_word);
  op1.xmm32u(2) = float32_sub(op1.xmm32u(2), op2.xmm32u(2), status_word);
  op1.xmm32u(3) = float32_add(op1.xmm32u(3), op2.xmm32u(3), status_word);

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* 66 0F 3A 08 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) {
    op.xmm32u(0) = float32_denormal_to_zero(op.xmm32u(0));
    op.xmm32u(1) = float32_denormal_to_zero(op.xmm32u(1));
    op.xmm32u(2) = float32_denormal_to_zero(op.xmm32u(2));
    op.xmm32u(3) = float32_denormal_to_zero(op.xmm32u(3));
  }

  op.xmm32u(0) = float32_round_to_int(op.xmm32u(0), status_word);
  op.xmm32u(1) = float32_round_to_int(op.xmm32u(1), status_word);
  op.xmm32u(2) = float32_round_to_int(op.xmm32u(2), status_word);
  op.xmm32u(3) = float32_round_to_int(op.xmm32u(3), status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/* 66 0F 3A 09 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) {
    op.xmm64u(0) = float64_denormal_to_zero(op.xmm64u(0));
    op.xmm64u(1) = float64_denormal_to_zero(op.xmm64u(1));
  }

  op.xmm64u(0) = float64_round_to_int(op.xmm64u(0), status_word);
  op.xmm64u(1) = float64_round_to_int(op.xmm64u(1), status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op);
#endif
}

/* 66 0F 3A 0A */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDSS_VssWssIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float32 op = BX_READ_XMM_REG_LO_DWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) op = float32_denormal_to_zero(op);

  op = float32_round_to_int(op, status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), op);
#endif
}

/* 66 0F 3A 0B */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::ROUNDSD_VsdWsdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  float64 op = BX_READ_XMM_REG_LO_QWORD(i->rm());

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Bit8u control = i->Ib();

  // override MXCSR rounding mode with control coming from imm8
  if ((control & 0x4) == 0)
    status_word.float_rounding_mode = control & 0x3;

  if (MXCSR.get_DAZ()) op = float64_denormal_to_zero(op);

  op = float64_round_to_int(op, status_word);

  // ignore precision exception result
  if (control & 0x8)
    status_word.float_exception_flags &= ~float_flag_inexact;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), op);
#endif
}

/* Opcode: 66 0F 3A 40
 * Selectively multiply packed SP floating-point values from xmm1 with
 * packed SP floating-point values from xmm2, add and selectively
 * store the packed SP floating-point values or zero values to xmm1
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DPPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm()), tmp;
  Bit8u mask = i->Ib();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm32u(0) = float32_denormal_to_zero(op1.xmm32u(0));
    op1.xmm32u(1) = float32_denormal_to_zero(op1.xmm32u(1));
    op1.xmm32u(2) = float32_denormal_to_zero(op1.xmm32u(2));
    op1.xmm32u(3) = float32_denormal_to_zero(op1.xmm32u(3));

    op2.xmm32u(0) = float32_denormal_to_zero(op2.xmm32u(0));
    op2.xmm32u(1) = float32_denormal_to_zero(op2.xmm32u(1));
    op2.xmm32u(2) = float32_denormal_to_zero(op2.xmm32u(2));
    op2.xmm32u(3) = float32_denormal_to_zero(op2.xmm32u(3));
  }

  tmp.xmm64u(0) = tmp.xmm64u(1) = 0;

  if (mask & 0x10)
     tmp.xmm32u(0) = float32_mul(op1.xmm32u(0), op2.xmm32u(0), status_word);
  if (mask & 0x20)
     tmp.xmm32u(1) = float32_mul(op1.xmm32u(1), op2.xmm32u(1), status_word);
  if (mask & 0x40)
     tmp.xmm32u(2) = float32_mul(op1.xmm32u(2), op2.xmm32u(2), status_word);
  if (mask & 0x80)
     tmp.xmm32u(3) = float32_mul(op1.xmm32u(3), op2.xmm32u(3), status_word);

  float32 r1 = float32_add(tmp.xmm32u(0), tmp.xmm32u(1), status_word);
  float32 r2 = float32_add(tmp.xmm32u(2), tmp.xmm32u(3), status_word);
  float32 r  = float32_add(r1, r2, status_word);

  op1.xmm64u(0) = op1.xmm64u(1) = 0;

  if (mask & 0x01) op1.xmm32u(0) = r;
  if (mask & 0x02) op1.xmm32u(1) = r;
  if (mask & 0x04) op1.xmm32u(2) = r;
  if (mask & 0x08) op1.xmm32u(3) = r;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}

/* Opcode: 66 0F 3A 41
 * Selectively multiply packed DP floating-point values from xmm1 with
 * packed DP floating-point values from xmm2, add and selectively
 * store the packed DP floating-point values or zero values to xmm1
 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::DPPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->rm()), tmp;
  Bit8u mask = i->Ib();

  float_status_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
    op1.xmm64u(0) = float64_denormal_to_zero(op1.xmm64u(0));
    op1.xmm64u(1) = float64_denormal_to_zero(op1.xmm64u(1));

    op2.xmm64u(0) = float64_denormal_to_zero(op2.xmm64u(0));
    op2.xmm64u(1) = float64_denormal_to_zero(op2.xmm64u(1));
  }

  tmp.xmm64u(0) = tmp.xmm64u(1) = 0;

  if (mask & 0x10)
     tmp.xmm64u(0) = float64_mul(op1.xmm64u(0), op2.xmm64u(0), status_word);
  if (mask & 0x20)
     tmp.xmm64u(1) = float64_mul(op1.xmm64u(1), op2.xmm64u(1), status_word);

  float64 result = float64_add(tmp.xmm64u(0), tmp.xmm64u(1), status_word);

  op1.xmm64u(0) = op1.xmm64u(1) = 0;

  if (mask & 0x01) op1.xmm64u(0) = result;
  if (mask & 0x02) op1.xmm64u(1) = result;

  check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), op1);
#endif
}
