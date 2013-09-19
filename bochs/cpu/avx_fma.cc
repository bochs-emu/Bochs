/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmaddpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmaddps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

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
  op1 = float64_fmadd(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

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
  op1 = float32_fmadd(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

  BX_NEXT_INSTR(i);
}

// FMADDSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmaddsubpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMADDSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMADDSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmaddsubps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBADDPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmsubaddpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmsubaddps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmsubpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FMSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFMSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fmsubps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

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
  op1 = float64_fmsub(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

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
  op1 = float32_fmsub(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMADDPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fnmaddpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMADDPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMADDPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fnmaddps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

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
  op1 = float64_fnmadd(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

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
  op1 = float32_fnmadd(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

  BX_NEXT_INSTR(i);
}

// FNMSUBPD
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBPD_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fnmsubpd(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

// FNMSUBPS
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VFNMSUBPS_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2());
  BxPackedYmmRegister op3 = BX_READ_YMM_REG(i->src3());
  unsigned len = i->getVL();

  float_status_t status;
  mxcsr_to_softfloat_status_word(status, MXCSR);

  for (unsigned n=0; n < len; n++)
    xmm_fnmsubps(&op1.ymm128(n), &op2.ymm128(n), &op3.ymm128(n), status);

  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

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
  op1 = float64_fnmsub(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

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
  op1 = float32_fnmsub(op1, op2, op3, status);
  check_exceptionsSSE(status.float_exception_flags);

  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), op1);
  BX_CLEAR_AVX_HIGH128(i->dst());

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
