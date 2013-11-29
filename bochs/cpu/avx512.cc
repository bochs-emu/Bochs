/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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

#include "simd_int.h"
#include "simd_compare.h"

#define AVX512_2OP_DWORD_EL(HANDLER, func)                                                  \
  /* AVX-512 instruction with two src operands working on DWORD elements */                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regd_masked(i, &op1, BX_READ_16BIT_OPMASK(i->opmask()));                   \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_2OP_DWORD_EL(VPADDD_MASK_VdqHdqWdqR, xmm_paddd)
AVX512_2OP_DWORD_EL(VPSUBD_MASK_VdqHdqWdqR, xmm_psubd)
AVX512_2OP_DWORD_EL(VPANDD_MASK_VdqHdqWdqR, xmm_andps)
AVX512_2OP_DWORD_EL(VPANDND_MASK_VdqHdqWdqR, xmm_andnps)
AVX512_2OP_DWORD_EL(VPORD_MASK_VdqHdqWdqR, xmm_orps)
AVX512_2OP_DWORD_EL(VPXORD_MASK_VdqHdqWdqR, xmm_xorps)
AVX512_2OP_DWORD_EL(VPMAXSD_MASK_VdqHdqWdqR, xmm_pmaxsd)
AVX512_2OP_DWORD_EL(VPMAXUD_MASK_VdqHdqWdqR, xmm_pmaxud)
AVX512_2OP_DWORD_EL(VPMINSD_MASK_VdqHdqWdqR, xmm_pminsd)
AVX512_2OP_DWORD_EL(VPMINUD_MASK_VdqHdqWdqR, xmm_pminud)
AVX512_2OP_DWORD_EL(VUNPCKLPS_MASK_VpsHpsWpsR, xmm_unpcklps)
AVX512_2OP_DWORD_EL(VUNPCKHPS_MASK_VpsHpsWpsR, xmm_unpckhps)
AVX512_2OP_DWORD_EL(VPMULLD_MASK_VdqHdqWdqR, xmm_pmulld)

#define AVX512_2OP_QWORD_EL(HANDLER, func)                                                  \
  /* AVX-512 instruction with two src operands working on DWORD elements */                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regq_masked(i, &op1, BX_READ_8BIT_OPMASK(i->opmask()));                    \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_2OP_QWORD_EL(VPADDQ_MASK_VdqHdqWdqR, xmm_paddq)
AVX512_2OP_QWORD_EL(VPSUBQ_MASK_VdqHdqWdqR, xmm_psubq)
AVX512_2OP_QWORD_EL(VPANDQ_MASK_VdqHdqWdqR, xmm_andps)
AVX512_2OP_QWORD_EL(VPANDNQ_MASK_VdqHdqWdqR, xmm_andnps)
AVX512_2OP_QWORD_EL(VPORQ_MASK_VdqHdqWdqR, xmm_orps)
AVX512_2OP_QWORD_EL(VPXORQ_MASK_VdqHdqWdqR, xmm_xorps)
AVX512_2OP_QWORD_EL(VPMAXSQ_MASK_VdqHdqWdqR, xmm_pmaxsq)
AVX512_2OP_QWORD_EL(VPMAXUQ_MASK_VdqHdqWdqR, xmm_pmaxuq)
AVX512_2OP_QWORD_EL(VPMINSQ_MASK_VdqHdqWdqR, xmm_pminsq)
AVX512_2OP_QWORD_EL(VPMINUQ_MASK_VdqHdqWdqR, xmm_pminuq)
AVX512_2OP_DWORD_EL(VUNPCKLPD_MASK_VpdHpdWpdR, xmm_unpcklpd)
AVX512_2OP_DWORD_EL(VUNPCKHPD_MASK_VpdHpdWpdR, xmm_unpckhpd)

/*
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VALIGND_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BX_PANIC(("%s: not implemented yet !", i->getIaOpcodeName()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VALIGNQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BX_PANIC(("%s: not implemented yet !", i->getIaOpcodeName()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BX_PANIC(("%s: not implemented yet !", i->getIaOpcodeName()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BX_PANIC(("%s: not implemented yet !", i->getIaOpcodeName()));
  BX_NEXT_INSTR(i);
}
*/

#endif
