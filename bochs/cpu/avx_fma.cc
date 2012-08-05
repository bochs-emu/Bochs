/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2012 Stanislav Shwartsman
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

extern void mxcsr_to_softfloat_status_word(float_status_t &status, bx_mxcsr_t mxcsr);

#include "simd_pfp.h"

//////////////////////////
// AVX FMA Instructions //
//////////////////////////

// FMADDPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmaddpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmaddps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMADDSD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSD_VpdHsdWsdR(bxInstruction_c *i)
{
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
  float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float64_muladd(op1, op2, op3, 0, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FMADDSS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSS_VpsHssWssR(bxInstruction_c *i)
{
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
  float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float32_muladd(op1, op2, op3, 0, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FMADDSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmaddsubpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMADDSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmaddsubps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBADDPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmsubaddpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmsubaddps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmsubpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fmsubps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBSD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBSD_VpdHsdWsdR(bxInstruction_c *i)
{
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
  float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float64_muladd(op1, op2, op3, float_muladd_negate_c, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FMSUBSS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBSS_VpsHssWssR(bxInstruction_c *i)
{
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
  float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float32_muladd(op1, op2, op3, float_muladd_negate_c, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMADDPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fnmaddpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fnmaddps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMADDSD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDSD_VpdHsdWsdR(bxInstruction_c *i)
{
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
  float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float64_muladd(op1, op2, op3, float_muladd_negate_product, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMADDSS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDSS_VpsHssWssR(bxInstruction_c *i)
{
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
  float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float32_muladd(op1, op2, op3, float_muladd_negate_product, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fnmsubpd(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister op3 = BX_READ_AVX_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    fnmsubps(&op1.avx128(n), &op2.avx128(n), &op3.avx128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMSUBSD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBSD_VpdHsdWsdR(bxInstruction_c *i)
{
  float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());
  float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());
  float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float64_muladd(op1, op2, op3, float_muladd_negate_result, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMSUBSS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBSS_VpsHssWssR(bxInstruction_c *i)
{
  float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());
  float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());
  float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);
  op1 = float32_muladd(op1, op2, op3, float_muladd_negate_result, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH(i->dst());

  BX_NEXT_INSTR(i);
}

//////////////////////////////////
// FMA4 (AMD) specific handlers //
//////////////////////////////////

#define FMA4_SINGLE_SCALAR(HANDLER, func)                                     \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float32 op1 = BX_READ_XMM_REG_LO_DWORD(i->src1());                        \
    float32 op2 = BX_READ_XMM_REG_LO_DWORD(i->src2());                        \
    float32 op3 = BX_READ_XMM_REG_LO_DWORD(i->src3());                        \
                                                                              \
    BxPackedXmmRegister dest;                                                 \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
                                                                              \
    dest.xmm64u(0) = (func)(op1, op2, op3, status);                           \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(status.float_exception_flags);                        \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_SINGLE_SCALAR(VFMADDSS_VssHssWssVIbR, float32_fmadd)
FMA4_SINGLE_SCALAR(VFMSUBSS_VssHssWssVIbR, float32_fmsub)

FMA4_SINGLE_SCALAR(VFNMADDSS_VssHssWssVIbR, float32_fnmadd)
FMA4_SINGLE_SCALAR(VFNMSUBSS_VssHssWssVIbR, float32_fnmsub)

#define FMA4_DOUBLE_SCALAR(HANDLER, func)                                     \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    float64 op1 = BX_READ_XMM_REG_LO_QWORD(i->src1());                        \
    float64 op2 = BX_READ_XMM_REG_LO_QWORD(i->src2());                        \
    float64 op3 = BX_READ_XMM_REG_LO_QWORD(i->src3());                        \
    BxPackedXmmRegister dest;                                                 \
                                                                              \
    float_status_t status;                                                    \
    mxcsr_to_softfloat_status_word(status, MXCSR);                            \
                                                                              \
    dest.xmm64u(0) = (func)(op1, op2, op3, status);                           \
    dest.xmm64u(1) = 0;                                                       \
                                                                              \
    check_exceptionsSSE(status.float_exception_flags);                        \
                                                                              \
    BX_WRITE_XMM_REG_CLEAR_HIGH(i->dst(), dest);                              \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

FMA4_DOUBLE_SCALAR(VFMADDSD_VsdHsdWsdVIbR, float64_fmadd)
FMA4_DOUBLE_SCALAR(VFMSUBSD_VsdHsdWsdVIbR, float64_fmsub)

FMA4_DOUBLE_SCALAR(VFNMADDSD_VsdHsdWsdVIbR, float64_fnmadd)
FMA4_DOUBLE_SCALAR(VFNMSUBSD_VsdHsdWsdVIbR, float64_fnmsub)

#endif
