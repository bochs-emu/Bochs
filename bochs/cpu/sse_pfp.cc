/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate@fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "softfloat.h"


#if BX_SUPPORT_SSE

void BX_CPU_C::check_exceptionsSSE(int exceptions_flags)
{
  int unmasked = ~(MXCSR.get_exceptions_masks()) & exceptions_flags;
  MXCSR.set_exceptions(exceptions_flags);

  if (unmasked) 
  {
     if(BX_CPU_THIS_PTR cr4.get_OSXMMEXCPT())
        exception(BX_XM_EXCEPTION, 0, 0);
     else
        exception(BX_UD_EXCEPTION, 0, 0);
  }
}

static void mxcsr_to_softfloat_status_word(softfloat_status_word_t &status, bx_mxcsr_t mxcsr)
{
  status.float_detect_tininess = float_tininess_before_rounding;
  status.float_exception_flags = 0; // clear exceptions before execution
  status.float_nan_handling_mode = float_first_operand_nan;
  status.float_rounding_mode = mxcsr.get_rounding_mode();
  // if underflow is masked and FUZ is 1, set it to 1, else to 0
  status.flush_underflow_to_zero = 
       (mxcsr.get_flush_masked_underflow() && mxcsr.get_UM()) ? 1 : 0;
}

// handle DAZ
static float32 handleDAZ(float32 op)
{
  if (float32_class(op) == float_denormal) op &= ((Bit32u)(1) << 31);
  return op;                      
}

static float64 handleDAZ(float64 op)
{
  if (float64_class(op) == float_denormal) op &= ((Bit64u)(1) << 63);
  return op;
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

#endif

/* 
 * Opcode: 0F 2A
 * Convert two 32bit signed integers from MMX/MEM to two single precision FP
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPU_C::CVTPI2PS_VpsQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedMmxRegister op;
  BxPackedXmmRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = int32_to_float32(MMXUD0(op), status_word);
  result.xmm32u(1) = int32_to_float32(MMXUD1(op), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result.xmm64u(0));
#else
  BX_INFO(("CVTPI2PS_VpsQq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 2A
 * Convert two 32bit signed integers from MMX/MEM to two double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPU_C::CVTPI2PD_VpdQd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedMmxRegister op;
  BxPackedXmmRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_MMX_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  result.xmm64u(0) = int32_to_float64(MMXUD0(op));
  result.xmm64u(1) = int32_to_float64(MMXUD1(op));

  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTPI2PD_VpdQd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 2A
 * Convert one 32bit signed integer to one double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPU_C::CVTSI2SD_VsdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Float64 result;

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u op;

    /* op is a register or memory reference */
    if (i->modC0()) {
      op = BX_READ_64BIT_REG(i->rm());
    }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op);
    }

    result = int64_to_float64(op, status_word);
  }
  else 
#endif
  {
    Bit32u op;

    /* op is a register or memory reference */
    if (i->modC0()) {
      op = BX_READ_32BIT_REG(i->rm());
    }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op);
    }

    result = int32_to_float64(op);
  }

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);
#else
  BX_INFO(("CVTSI2SD_VsdEd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 2A
 * Convert one 32bit signed integer to one single precision FP
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPU_C::CVTSI2SS_VssEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  Float32 result;

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u op;

    /* op is a register or memory reference */
    if (i->modC0()) {
      op = BX_READ_64BIT_REG(i->rm());
    }
    else {
      /* pointer, segment address pair */
      read_virtual_qword(i->seg(), RMAddr(i), &op);
    }

    result = int64_to_float32(op, status_word);
  }
  else 
#endif
  {
    Bit32u op;

    /* op is a register or memory reference */
    if (i->modC0()) {
      op = BX_READ_32BIT_REG(i->rm());
    }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg(), RMAddr(i), &op);
    }

    result = int32_to_float32(op, status_word);
  }
 
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);
#else
  BX_INFO(("CVTSI2SS_VssEd: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 2C
 * Convert two single precision FP numbers to two signed doubleword integers 
 * in MMX using truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  Bit64u op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  Float32 r0 = (Float32)(op & 0xFFFFFFFF);
  Float32 r1 = (Float32)(op >> 32);

  MMXUD0(result) = float32_to_int32_round_to_zero(r0, status_word);
  MMXUD1(result) = float32_to_int32_round_to_zero(r1, status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("CVTTPS2PI_PqWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 2C
 * Convert two double precision FP numbers to two signed doubleword integers 
 * in MMX using truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedXmmRegister op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  MMXUD0(result) = float64_to_int32_round_to_zero(op.xmm64u(0), status_word);
  MMXUD1(result) = float64_to_int32_round_to_zero(op.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("CVTTPD2PI_PqWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 2C
 * Convert one double precision FP number to doubleword integer using 
 * truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float64_to_int64_round_to_zero(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else 
#endif
  {
    Bit32u result = float64_to_int32_round_to_zero(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REG(i->nnn(), result);
  }

#else
  BX_INFO(("CVTTSD2SI_GdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 2C
 * Convert one single precision FP number to doubleword integer using 
 * truncation if the conversion is inexact
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float32_to_int64_round_to_zero(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else 
#endif
  {
    Bit32u result = float32_to_int32_round_to_zero(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REG(i->nnn(), result);
  }

#else
  BX_INFO(("CVTTSS2SI_GdWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/*
 * Opcode: 0F 2D
 * Convert two single precision FP numbers to two signed doubleword integers 
 * in MMX register. When a conversion is inexact, the value returned is 
 * rounded according to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  Bit64u op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  Float32 r0 = (Float32)(op & 0xFFFFFFFF);
  Float32 r1 = (Float32)(op >> 32);

  MMXUD0(result) = float32_to_int32(r0, status_word);
  MMXUD1(result) = float32_to_int32(r1, status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("CVTPS2PI_PqWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 2D
 * Convert two double precision FP numbers to two signed doubleword integers 
 * in MMX register. When a conversion is inexact, the value returned is 
 * rounded according to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTPD2PI_PqWpd(bxInstruction_c *i)
{                      
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedXmmRegister op;
  BxPackedMmxRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  MMXUD0(result) = float64_to_int32(op.xmm64u(0), status_word);
  MMXUD1(result) = float64_to_int32(op.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_MMX_REG(i->nnn(), result);
#else
  BX_INFO(("CVTPD2PI_PqWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 2D
 * Convert one double precision FP number to doubleword integer
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float64_to_int64(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else 
#endif
  {
    Bit32u result = float64_to_int32(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REG(i->nnn(), result);
  }

#else
  BX_INFO(("CVTSD2SI_GdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/*
 * Opcode: F3 0F 2D
 * Convert one single precision FP number to doubleword integer.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

#if BX_SUPPORT_X86_64 
  if (i->os64L())   /* 64 bit operand size mode */
  {
    Bit64u result = float32_to_int64(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_64BIT_REG(i->nnn(), result);
  }
  else 
#endif
  {
    Bit32u result = float32_to_int32(op, status_word);
    BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
    BX_WRITE_32BIT_REG(i->nnn(), result);
  }

#else
  BX_INFO(("CVTSS2SI_GdWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5A
 * Convert two single precision FP numbers to two double precision FP numbers
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CVTPS2PD_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Bit64u op;
  BxPackedXmmRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  Float32 r0 = (Float32)(op & 0xFFFFFFFF);
  Float32 r1 = (Float32)(op >> 32);

  if (MXCSR.get_DAZ()) {
     r0 = handleDAZ(r0);
     r1 = handleDAZ(r1);
  }

  result.xmm32u(0) = 
     float32_to_float64(r0, status_word);
  result.xmm32u(1) = 
     float32_to_float64(r1, status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("CVTPS2PD_VpsWps: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5A
 * Convert two double precision FP numbers to two single precision FP. 
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPU_C::CVTPD2PS_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op.xmm64u(0) = handleDAZ(op.xmm64u(0));
	op.xmm64u(0) = handleDAZ(op.xmm64u(1));
  }

  result.xmm32u(0) = 
     float64_to_float32(op.xmm64u(0), status_word);
  result.xmm32u(1) = 
     float64_to_float32(op.xmm64u(1), status_word);
  result.xmm64u(1) = 0;

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("CVTPD2PS_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 5A
 * Convert one double precision FP number to one single precision FP. 
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #D, #O, #I, #P
 */
void BX_CPU_C::CVTSD2SS_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op;
  Float32 result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = handleDAZ(op);
  result = float64_to_float32(op, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("CVTSD2SS_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5A
 * Convert one single precision FP number to one double precision FP. 
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CVTSS2SD_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op;
  Float64 result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = handleDAZ(op);
  result = float32_to_float64(op, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("CVTSS2SD_VssWss: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5B
 * Convert four signed integers to four single precision FP numbers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #P
 */
void BX_CPU_C::CVTDQ2PS_VpsWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = 
      int32_to_float32(op.xmm32u(0), status_word);
  result.xmm32u(1) = 
      int32_to_float32(op.xmm32u(1), status_word);
  result.xmm32u(2) = 
      int32_to_float32(op.xmm32u(2), status_word);
  result.xmm32u(3) = 
      int32_to_float32(op.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTDQ2PS_VpsWdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5B
 * Convert four single precision FP to four doubleword integers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = 
      float32_to_int32(op.xmm32u(0), status_word);
  result.xmm32u(1) = 
      float32_to_int32(op.xmm32u(1), status_word);
  result.xmm32u(2) = 
      float32_to_int32(op.xmm32u(2), status_word);
  result.xmm32u(3) = 
      float32_to_int32(op.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTPS2DQ_VdqWps: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5B
 * Convert four single precision FP to four doubleword integers using 
 * truncation if the conversion is inexact.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = 
      float32_to_int32_round_to_zero(op.xmm32u(0), status_word);
  result.xmm32u(1) = 
      float32_to_int32_round_to_zero(op.xmm32u(1), status_word);
  result.xmm32u(2) = 
      float32_to_int32_round_to_zero(op.xmm32u(2), status_word);
  result.xmm32u(3) = 
      float32_to_int32_round_to_zero(op.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTTPS2DQ_VdqWps: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F E6
 * Convert two double precision FP to two signed doubleword integers using 
 * truncation if the conversion is inexact.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = 
      float64_to_int32_round_to_zero(op.xmm64u(0), status_word);
  result.xmm32u(1) = 
      float64_to_int32_round_to_zero(op.xmm64u(1), status_word);
  result.xmm64u(1) = 0;

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTTPD2DQ_VqWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F E6
 * Convert two double precision FP to two signed doubleword integers.
 * When a conversion is inexact, the value returned is rounded according
 * to rounding control bits in MXCSR register.
 * Possible floating point exceptions: #I, #P
 */
void BX_CPU_C::CVTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  result.xmm32u(0) = 
      float64_to_int32(op.xmm64u(0), status_word);
  result.xmm32u(1) = 
      float64_to_int32(op.xmm64u(1), status_word);
  result.xmm64u(1) = 0;

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTPD2DQ_VqWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F E6
 * Convert two 32bit signed integers from XMM/MEM to two double precision FP
 * Possible floating point exceptions: -
 */
void BX_CPU_C::CVTDQ2PD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Bit64u op;
  BxPackedXmmRegister result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op);
  }

  Bit32u r0 = (Bit32u)(op & 0xFFFFFFFF);
  Bit32u r1 = (Bit32u)(op >> 32);

  result.xmm64u(0) = int32_to_float64(r0);
  result.xmm64u(1) = int32_to_float64(r1);

  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("CVTDQ2PD_VpdWq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 2E
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::UCOMISS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float32_compare_quiet(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);

  switch(rc) {
     case float_relation_unordered:
         setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
         break;

     case float_relation_greater:
         setEFlagsOSZAPC(0);
         break;

     case float_relation_less:
         setEFlagsOSZAPC(EFlagsCFMask);
         break;

     case float_relation_equal:
         setEFlagsOSZAPC(EFlagsZFMask);
         break;
  }

#else
  BX_INFO(("UCOMISS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 2E
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::UCOMISD_VsdWsd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float64_compare_quiet(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);

  switch(rc) {
     case float_relation_unordered:
         setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
         break;

     case float_relation_greater:
         setEFlagsOSZAPC(0);
         break;

     case float_relation_less:
         setEFlagsOSZAPC(EFlagsCFMask);
         break;

     case float_relation_equal:
         setEFlagsOSZAPC(EFlagsZFMask);
         break;
  }

#else
  BX_INFO(("UCOMISD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 2F
 * Compare two single precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::COMISS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);

  switch(rc) {
     case float_relation_unordered:
         setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
         break;

     case float_relation_greater:
         setEFlagsOSZAPC(0);
         break;

     case float_relation_less:
         setEFlagsOSZAPC(EFlagsCFMask);
         break;

     case float_relation_equal:
         setEFlagsOSZAPC(EFlagsZFMask);
         break;
  }

#else
  BX_INFO(("COMISS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 2F
 * Compare two double precision FP numbers and set EFLAGS accordintly.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::COMISD_VpdWpd(bxInstruction_c *i)   
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);

  switch(rc) {
     case float_relation_unordered:
         setEFlagsOSZAPC(EFlagsZFMask | EFlagsPFMask | EFlagsCFMask);
         break;

     case float_relation_greater:
         setEFlagsOSZAPC(0);
         break;

     case float_relation_less:
         setEFlagsOSZAPC(EFlagsCFMask);
         break;

     case float_relation_equal:
         setEFlagsOSZAPC(EFlagsZFMask);
         break;
  }

#else
  BX_INFO(("COMISD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 51
 * Square Root packed single precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPU_C::SQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op.xmm32u(0) = handleDAZ(op.xmm32u(0));
     op.xmm32u(1) = handleDAZ(op.xmm32u(1));
     op.xmm32u(2) = handleDAZ(op.xmm32u(2));
     op.xmm32u(3) = handleDAZ(op.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_sqrt(op.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_sqrt(op.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_sqrt(op.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_sqrt(op.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("SQRTPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 51
 * Square Root packed double precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPU_C::SQRTPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op.xmm64u(0) = handleDAZ(op.xmm64u(0));
     op.xmm64u(1) = handleDAZ(op.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_sqrt(op.xmm64u(0), status_word);
  result.xmm64u(1) = 
     float64_sqrt(op.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("SQRTPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 51
 * Square Root scalar double precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPU_C::SQRTSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = handleDAZ(op);
  result = float64_sqrt(op, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("SQRTSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 51
 * Square Root scalar single precision.
 * Possible floating point exceptions: #I, #D, #P
 */
void BX_CPU_C::SQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  if (MXCSR.get_DAZ()) op = handleDAZ(op);
  result = float32_sqrt(op, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("SQRTSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 58
 * Add packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_add(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_add(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_add(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_add(op1.xmm32u(3), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("ADDPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 58
 * Add packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_add(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(1) =              
     float64_add(op1.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("ADDPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 58
 * Add the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  result = float64_add(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("ADDSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 58
 * Add the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  result = float32_add(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("ADDSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 59
 * Multiply packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::MULPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_mul(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_mul(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_mul(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_mul(op1.xmm32u(3), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MULPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 59
 * Multiply packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::MULPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_mul(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(1) = 
     float64_mul(op1.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MULPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 59
 * Multiply the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::MULSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  result = float64_mul(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("MULSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 59
 * Multiply the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::MULSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  result = float32_mul(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("MULSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5C
 * Subtract packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::SUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_sub(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_sub(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_sub(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_sub(op1.xmm32u(3), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("SUBPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5C
 * Subtract packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::SUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_sub(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(1) = 
     float64_sub(op1.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("SUBPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 5C
 * Subtract the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::SUBSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  result = float64_sub(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("SUBSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5C
 * Subtract the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::SUBSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  result = float32_sub(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("SUBSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5D
 * Calculate the minimum single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MINPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  rc = float32_compare(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(0) = 
      (rc == float_relation_less) ? op1.xmm32u(0) : op2.xmm32u(0);
  rc = float32_compare(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(1) = 
      (rc == float_relation_less) ? op1.xmm32u(1) : op2.xmm32u(1);
  rc = float32_compare(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(2) = 
      (rc == float_relation_less) ? op1.xmm32u(2) : op2.xmm32u(2);
  rc = float32_compare(op1.xmm32u(3), op2.xmm32u(3), status_word);
  result.xmm32u(3) = 
      (rc == float_relation_less) ? op1.xmm32u(3) : op2.xmm32u(3);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MINPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5D
 * Calculate the minimum double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MINPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  rc = float64_compare(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(0) = 
      (rc == float_relation_less) ? op1.xmm64u(0) : op2.xmm64u(0);
  rc = float64_compare(op1.xmm64u(1), op2.xmm64u(1), status_word);
  result.xmm64u(1) = 
      (rc == float_relation_less) ? op1.xmm64u(1) : op2.xmm64u(1);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MINPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 5D
 * Calculate the minimum scalar double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MINSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), 
         (rc == float_relation_less) ? op1 : op2);

#else
  BX_INFO(("MINSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5D
 * Calculate the minimum scalar single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MINSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), 
         (rc == float_relation_less) ? op1 : op2);

#else
  BX_INFO(("MINSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5E
 * Divide packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPU_C::DIVPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_div(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_div(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_div(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_div(op1.xmm32u(3), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("DIVPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5E
 * Divide packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPU_C::DIVPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_div(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(1) = 
     float64_div(op1.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("DIVPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 5E
 * Divide the lower double precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPU_C::DIVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  result = float64_div(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("DIVSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5E
 * Divide the lower single precision FP number from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #Z, #O, #U, #P
 */
void BX_CPU_C::DIVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  result = float32_div(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("DIVSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F 5F
 * Calculate the maximum single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MAXPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  rc = float32_compare(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(0) = 
      (rc == float_relation_greater) ? op1.xmm32u(0) : op2.xmm32u(0);
  rc = float32_compare(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(1) = 
      (rc == float_relation_greater) ? op1.xmm32u(1) : op2.xmm32u(1);
  rc = float32_compare(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(2) = 
      (rc == float_relation_greater) ? op1.xmm32u(2) : op2.xmm32u(2);
  rc = float32_compare(op1.xmm32u(3), op2.xmm32u(3), status_word);
  result.xmm32u(3) = 
      (rc == float_relation_greater) ? op1.xmm32u(3) : op2.xmm32u(3);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MAXPS_VpsWps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 5F
 * Calculate the maximum double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MAXPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  rc = float64_compare(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(0) = 
      (rc == float_relation_greater) ? op1.xmm64u(0) : op2.xmm64u(0);
  rc = float64_compare(op1.xmm64u(1), op2.xmm64u(1), status_word);
  result.xmm64u(1) = 
      (rc == float_relation_greater) ? op1.xmm64u(1) : op2.xmm64u(1);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("MAXPD_VpdWpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 5F
 * Calculate the maximum scalar double precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MAXSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
     op1 = handleDAZ(op1);
     op2 = handleDAZ(op2);
  }

  int rc = float64_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), 
         (rc == float_relation_greater) ? op1 : op2);

#else
  BX_INFO(("MAXSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F 5F
 * Calculate the maxumim scalar single precision FP between XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::MAXSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  int rc = float32_compare(op1, op2, status_word);
  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), 
         (rc == float_relation_greater) ? op1 : op2);

#else
  BX_INFO(("MAXSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 7C
 * Add horizontally packed double precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::HADDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_add(op1.xmm64u(0), op1.xmm64u(1), status_word);
  result.xmm64u(1) = 
     float64_add(op2.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("HADDPD_VpdWpd: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 7C
 * Add horizontally packed single precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::HADDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_add(op1.xmm32u(0), op1.xmm32u(1), status_word);
  result.xmm32u(1) = 
     float32_add(op1.xmm32u(2), op1.xmm32u(3), status_word);
  result.xmm32u(2) = 
     float32_add(op2.xmm32u(0), op2.xmm32u(1), status_word);
  result.xmm32u(3) = 
     float32_add(op2.xmm32u(2), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("HADDPS_VpsWps: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F 7D
 * Subtract horizontally packed double precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::HSUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_sub(op1.xmm64u(0), op1.xmm64u(1), status_word);
  result.xmm64u(1) = 
     float64_sub(op2.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("HSUBPD_VpdWpd: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F 7D
 * Subtract horizontally packed single precision FP in XMM2/MEM from XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::HSUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int rc;

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_sub(op1.xmm32u(0), op1.xmm32u(1), status_word);
  result.xmm32u(1) = 
     float32_sub(op1.xmm32u(2), op1.xmm32u(3), status_word);
  result.xmm32u(2) = 
     float32_sub(op2.xmm32u(0), op2.xmm32u(1), status_word);
  result.xmm32u(3) = 
     float32_sub(op2.xmm32u(2), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("HSUBPS_VpsWps: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 0F C2
 * Compare packed single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CMPPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib();

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  if(ib < 4) 
  {
    result.xmm32u(0) = 
        compare32[ib](op1.xmm32u(0), op2.xmm32u(0), status) ? 0xFFFFFFFF : 0;
    result.xmm32u(1) = 
        compare32[ib](op1.xmm32u(1), op2.xmm32u(1), status) ? 0xFFFFFFFF : 0;
    result.xmm32u(2) = 
        compare32[ib](op1.xmm32u(2), op2.xmm32u(2), status) ? 0xFFFFFFFF : 0;
    result.xmm32u(3) = 
        compare32[ib](op1.xmm32u(3), op2.xmm32u(3), status) ? 0xFFFFFFFF : 0;
  }
  else if(ib < 8)
  {
    ib -= 4;

    result.xmm32u(0) = 
        compare32[ib](op1.xmm32u(0), op2.xmm32u(0), status) ? 0 : 0xFFFFFFFF;
    result.xmm32u(1) = 
        compare32[ib](op1.xmm32u(1), op2.xmm32u(1), status) ? 0 : 0xFFFFFFFF;
    result.xmm32u(2) = 
        compare32[ib](op1.xmm32u(2), op2.xmm32u(2), status) ? 0 : 0xFFFFFFFF;
    result.xmm32u(3) = 
        compare32[ib](op1.xmm32u(3), op2.xmm32u(3), status) ? 0 : 0xFFFFFFFF;
  }
  else {
    BX_PANIC(("CMPPS_VpsWpsIb: unrecognized predicate %u", ib));
  }

  BX_CPU_THIS_PTR check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("CMPPS_VpsWpsIb: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F C2
 * Compare packed double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CMPPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  int ib = i->Ib();

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  if(ib < 4) 
  {
    result.xmm64u(0) = compare64[ib](op1.xmm64u(0), op2.xmm64u(0), status) ? 
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
    result.xmm64u(1) = compare64[ib](op1.xmm64u(1), op2.xmm64u(1), status) ? 
       BX_CONST64(0xFFFFFFFFFFFFFFFF) : 0;
  }
  else if(ib < 8)
  {
    ib -= 4;

    result.xmm64u(0) = compare64[ib](op1.xmm64u(0), op2.xmm64u(0), status) ? 
       0 : BX_CONST64(0xFFFFFFFFFFFFFFFF);
    result.xmm64u(1) = compare64[ib](op1.xmm64u(1), op2.xmm64u(1), status) ? 
       0 : BX_CONST64(0xFFFFFFFFFFFFFFFF);
  }
  else {
    BX_PANIC(("CMPPS_VpdWpdIb: unrecognized predicate %u", ib));
  }

  BX_CPU_THIS_PTR check_exceptionsSSE(status.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("CMPPD_VpdWpdIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F C2
 * Compare double precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CMPSD_VsdWsdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), (Bit64u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib();

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  if(ib < 4) {
     if(compare64[ib](op1, op2, status_word)) {
        result = BX_CONST64(0xFFFFFFFFFFFFFFFF);
     } else {
        result = 0;
     }
  } else if(ib < 8) {
     if(compare64[ib-4](op1, op2, status_word)) {
        result = 0;
     } else {
        result = BX_CONST64(0xFFFFFFFFFFFFFFFF);
     }
  } else {
     BX_PANIC(("CMPPS_VsdWsdIb: unrecognized predicate %u", ib));
  }

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), result);

#else
  BX_INFO(("CMPSD_VsdWsdIb: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F3 0F C2
 * Compare single precision FP values using Ib as comparison predicate.
 * Possible floating point exceptions: #I, #D
 */
void BX_CPU_C::CMPSS_VssWssIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), (Bit32u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);
  int ib = i->Ib();

  if (MXCSR.get_DAZ()) 
  {
	op1 = handleDAZ(op1);
	op2 = handleDAZ(op2);
  }

  if(ib < 4) {
     if(compare32[ib](op1, op2, status_word)) {
        result = 0xFFFFFFFF;
     } else {
        result = 0;
     }
  } else if(ib < 8) {
     if(compare32[ib-4](op1, op2, status_word)) {
        result = 0;
     } else {
        result = 0xFFFFFFFF;
     }
  } else {
     BX_PANIC(("CMPPS_VssWssIb: unrecognized predicate %u", ib));
  }

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG_LO_DWORD(i->nnn(), result);

#else
  BX_INFO(("CMPSS_VssWssIb: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: 66 0F D0
 * Add/Subtract packed double precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDSUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) 
  {
	op1.xmm64u(0) = handleDAZ(op1.xmm64u(0));
	op1.xmm64u(1) = handleDAZ(op1.xmm64u(1));

	op2.xmm64u(0) = handleDAZ(op2.xmm64u(0));
	op2.xmm64u(1) = handleDAZ(op2.xmm64u(1));
  }

  result.xmm64u(0) = 
     float64_sub(op1.xmm64u(0), op2.xmm64u(0), status_word);
  result.xmm64u(1) = 
     float64_add(op1.xmm64u(1), op2.xmm64u(1), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("ADDSUBPD_VpdWpd: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}

/* 
 * Opcode: F2 0F D0
 * Add/Substract packed single precision FP numbers from XMM2/MEM to XMM1.
 * Possible floating point exceptions: #I, #D, #O, #U, #P
 */
void BX_CPU_C::ADDSUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_PNI
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  softfloat_status_word_t status_word;
  mxcsr_to_softfloat_status_word(status_word, MXCSR);

  if (MXCSR.get_DAZ()) {
	op1.xmm32u(0) = handleDAZ(op1.xmm32u(0));
	op1.xmm32u(1) = handleDAZ(op1.xmm32u(1));
	op1.xmm32u(2) = handleDAZ(op1.xmm32u(2));
	op1.xmm32u(3) = handleDAZ(op1.xmm32u(3));

	op2.xmm32u(0) = handleDAZ(op2.xmm32u(0));
	op2.xmm32u(1) = handleDAZ(op2.xmm32u(1));
	op2.xmm32u(2) = handleDAZ(op2.xmm32u(2));
	op2.xmm32u(3) = handleDAZ(op2.xmm32u(3));
  }

  result.xmm32u(0) = 
     float32_sub(op1.xmm32u(0), op2.xmm32u(0), status_word);
  result.xmm32u(1) = 
     float32_add(op1.xmm32u(1), op2.xmm32u(1), status_word);
  result.xmm32u(2) = 
     float32_sub(op1.xmm32u(2), op2.xmm32u(2), status_word);
  result.xmm32u(3) = 
     float32_add(op1.xmm32u(3), op2.xmm32u(3), status_word);

  BX_CPU_THIS_PTR check_exceptionsSSE(status_word.float_exception_flags);
  BX_WRITE_XMM_REG(i->nnn(), result);

#else
  BX_INFO(("ADDSUBPS_VpsWps: required PNI, use --enable-pni option"));
  UndefinedOpcode(i);
#endif
}
