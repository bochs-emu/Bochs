/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013-2019 Stanislav Shwartsman
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
#include "wide_int.h"

// compare

typedef Bit32u (*avx512_compare_method)(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2);

static avx512_compare_method avx512_compare8[8] = {
  xmm_pcmpeqb_mask,     // equal
  xmm_pcmpltb_mask,     // less than
  xmm_pcmpleb_mask,     // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpneb_mask,     // not equal
  xmm_pcmpgeb_mask,     // not less than => greater or equal than
  xmm_pcmpgtb_mask,     // not less or equal than => greater than
  xmm_pcmptrueb_mask    // true
};

static avx512_compare_method avx512_compare16[8] = {
  xmm_pcmpeqw_mask,     // equal
  xmm_pcmpltw_mask,     // less than
  xmm_pcmplew_mask,     // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpnew_mask,     // not equal
  xmm_pcmpgew_mask,     // not less than => greater or equal than
  xmm_pcmpgtw_mask,     // not less or equal than => greater than
  xmm_pcmptruew_mask    // true
};

static avx512_compare_method avx512_compare32[8] = {
  xmm_pcmpeqd_mask,     // equal
  xmm_pcmpltd_mask,     // less than
  xmm_pcmpled_mask,     // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpned_mask,     // not equal
  xmm_pcmpged_mask,     // not less than => greater or equal than
  xmm_pcmpgtd_mask,     // not less or equal than => greater than
  xmm_pcmptrued_mask    // true
};

static avx512_compare_method avx512_compare64[8] = {
  xmm_pcmpeqq_mask,     // equal
  xmm_pcmpltq_mask,     // less than
  xmm_pcmpleq_mask,     // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpneq_mask,     // not equal
  xmm_pcmpgeq_mask,     // not less than => greater or equal than
  xmm_pcmpgtq_mask,     // not less or equal than => greater than
  xmm_pcmptrueq_mask    // true
};

static avx512_compare_method avx512_compare8u[8] = {
  xmm_pcmpeqb_mask,     // equal
  xmm_pcmpltub_mask,    // less than
  xmm_pcmpleub_mask,    // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpneb_mask,     // not equal
  xmm_pcmpgeub_mask,    // not less than => greater or equal than
  xmm_pcmpgtub_mask,    // not less or equal than => greater than
  xmm_pcmptrueb_mask    // true
};

static avx512_compare_method avx512_compare16u[8] = {
  xmm_pcmpeqw_mask,     // equal
  xmm_pcmpltuw_mask,    // less than
  xmm_pcmpleuw_mask,    // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpnew_mask,     // not equal
  xmm_pcmpgeuw_mask,    // not less than => greater or equal than
  xmm_pcmpgtuw_mask,    // not less or equal than => greater than
  xmm_pcmptruew_mask    // true
};

static avx512_compare_method avx512_compare32u[8] = {
  xmm_pcmpeqd_mask,     // equal
  xmm_pcmpltud_mask,    // less than
  xmm_pcmpleud_mask,    // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpned_mask,     // not equal
  xmm_pcmpgeud_mask,    // not less than => greater or equal than
  xmm_pcmpgtud_mask,    // not less or equal than => greater than
  xmm_pcmptrued_mask    // true
};

static avx512_compare_method avx512_compare64u[8] = {
  xmm_pcmpeqq_mask,     // equal
  xmm_pcmpltuq_mask,    // less than
  xmm_pcmpleuq_mask,    // less or equal than
  xmm_pcmpfalse_mask,   // false
  xmm_pcmpneq_mask,     // not equal
  xmm_pcmpgeuq_mask,    // not less than => greater or equal than
  xmm_pcmpgtuq_mask,    // not less or equal than => greater than
  xmm_pcmptrueq_mask    // true
};

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPB_MASK_KGqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit64u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 16;
    result |= avx512_compare8[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= BX_READ_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUB_MASK_KGqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit64u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 16;
    result |= avx512_compare8u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= BX_READ_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPW_MASK_KGdHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 8;
    result |= avx512_compare16[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_32BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUW_MASK_KGdHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 8;
    result |= avx512_compare16u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_32BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPD_MASK_KGwHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 4;
    result |= avx512_compare32[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUD_MASK_KGwHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 4;
    result |= avx512_compare32u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 2;
    result |= avx512_compare64[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n--) {
    result <<= 2;
    result |= avx512_compare64u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
  BX_NEXT_INSTR(i);
}

///////////////////////////////////////////////////////////////////////////////////////////

#define AVX512_COMPARE_BYTE_EL(HANDLER, func)                                               \
  /* AVX-512 compare instruction with two src operands working on BYTE elements */          \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    Bit64u result = 0;                                                                      \
    for (int n=len-1; n >= 0; n--) {                                                        \
      result <<= 16;                                                                        \
      result |= (func)(&op1.vmm128(n), &op2.vmm128(n));                                     \
    }                                                                                       \
                                                                                            \
    if (i->opmask())                                                                        \
      result &= BX_READ_OPMASK(i->opmask());                                                \
                                                                                            \
    BX_WRITE_OPMASK(i->dst(), result);                                                      \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_COMPARE_BYTE_EL(VPCMPGTB_MASK_KGqHdqWdqR, xmm_pcmpgtb_mask)
AVX512_COMPARE_BYTE_EL(VPCMPEQB_MASK_KGqHdqWdqR, xmm_pcmpeqb_mask)
AVX512_COMPARE_BYTE_EL(VPTESTMB_MASK_KGqHdqWdqR, xmm_ptestmb_mask)
AVX512_COMPARE_BYTE_EL(VPTESTNMB_MASK_KGqHdqWdqR, xmm_ptestnmb_mask)

#define AVX512_COMPARE_WORD_EL(HANDLER, func)                                               \
  /* AVX-512 compare instruction with two src operands working on WORD elements */          \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    Bit32u result = 0;                                                                      \
    for (int n=len-1; n >= 0; n--) {                                                        \
      result <<= 8;                                                                         \
      result |= (func)(&op1.vmm128(n), &op2.vmm128(n));                                     \
    }                                                                                       \
                                                                                            \
    if (i->opmask())                                                                        \
      result &= (Bit32u) BX_READ_32BIT_OPMASK(i->opmask());                                 \
                                                                                            \
    BX_WRITE_OPMASK(i->dst(), result);                                                      \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_COMPARE_WORD_EL(VPCMPGTW_MASK_KGdHdqWdqR, xmm_pcmpgtw_mask)
AVX512_COMPARE_WORD_EL(VPCMPEQW_MASK_KGdHdqWdqR, xmm_pcmpeqw_mask)
AVX512_COMPARE_WORD_EL(VPTESTMW_MASK_KGdHdqWdqR, xmm_ptestmw_mask)
AVX512_COMPARE_WORD_EL(VPTESTNMW_MASK_KGdHdqWdqR, xmm_ptestnmw_mask)

#define AVX512_COMPARE_DWORD_EL(HANDLER, func)                                              \
  /* AVX-512 compare instruction with two src operands working on DWORD elements */         \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    Bit32u result = 0;                                                                      \
    for (int n=len-1; n >= 0; n--) {                                                        \
      result <<= 4;                                                                         \
      result |= (func)(&op1.vmm128(n), &op2.vmm128(n));                                     \
    }                                                                                       \
                                                                                            \
    if (i->opmask())                                                                        \
      result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());                                 \
                                                                                            \
    BX_WRITE_OPMASK(i->dst(), result);                                                      \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_COMPARE_DWORD_EL(VPCMPGTD_MASK_KGwHdqWdqR, xmm_pcmpgtd_mask)
AVX512_COMPARE_DWORD_EL(VPCMPEQD_MASK_KGwHdqWdqR, xmm_pcmpeqd_mask)
AVX512_COMPARE_DWORD_EL(VPTESTMD_MASK_KGwHdqWdqR, xmm_ptestmd_mask)
AVX512_COMPARE_DWORD_EL(VPTESTNMD_MASK_KGwHdqWdqR, xmm_ptestnmd_mask)

#define AVX512_COMPARE_QWORD_EL(HANDLER, func)                                              \
  /* AVX-512 compare instruction with two src operands working on QWORD elements */         \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    Bit32u result = 0;                                                                      \
    for (int n=len-1; n >= 0; n--) {                                                        \
      result <<= 2;                                                                         \
      result |= (func)(&op1.vmm128(n), &op2.vmm128(n));                                     \
    }                                                                                       \
                                                                                            \
    if (i->opmask())                                                                        \
      result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());                                  \
                                                                                            \
    BX_WRITE_OPMASK(i->dst(), result);                                                      \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_COMPARE_QWORD_EL(VPCMPGTQ_MASK_KGbHdqWdqR, xmm_pcmpgtq_mask)
AVX512_COMPARE_QWORD_EL(VPCMPEQQ_MASK_KGbHdqWdqR, xmm_pcmpeqq_mask)
AVX512_COMPARE_QWORD_EL(VPTESTMQ_MASK_KGbHdqWdqR, xmm_ptestmq_mask)
AVX512_COMPARE_QWORD_EL(VPTESTNMQ_MASK_KGbHdqWdqR, xmm_ptestnmq_mask)

// compute, shift and rotate

#define AVX512_2OP_QWORD_EL(HANDLER, func)                                                  \
  /* AVX-512 instruction with two src operands working on QWORD elements */                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regq_masked(i, &op1, len, BX_READ_8BIT_OPMASK(i->opmask()));               \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_2OP_QWORD_EL(VPMULLQ_MASK_VdqHdqWdqR, xmm_pmullq)
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
AVX512_2OP_QWORD_EL(VUNPCKLPD_MASK_VpdHpdWpdR, xmm_unpcklpd)
AVX512_2OP_QWORD_EL(VUNPCKHPD_MASK_VpdHpdWpdR, xmm_unpckhpd)
AVX512_2OP_QWORD_EL(VPMULDQ_MASK_VdqHdqWdqR, xmm_pmuldq)
AVX512_2OP_QWORD_EL(VPMULUDQ_MASK_VdqHdqWdqR, xmm_pmuludq)
AVX512_2OP_QWORD_EL(VPSRAVQ_MASK_VdqHdqWdqR, xmm_psravq)
AVX512_2OP_QWORD_EL(VPSRLVQ_MASK_VdqHdqWdqR, xmm_psrlvq)
AVX512_2OP_QWORD_EL(VPSLLVQ_MASK_VdqHdqWdqR, xmm_psllvq)
AVX512_2OP_QWORD_EL(VPRORVQ_MASK_VdqHdqWdqR, xmm_prorvq)
AVX512_2OP_QWORD_EL(VPROLVQ_MASK_VdqHdqWdqR, xmm_prolvq)

#define AVX512_2OP_DWORD_EL(HANDLER, func)                                                  \
  /* AVX-512 instruction with two src operands working on DWORD elements */                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regd_masked(i, &op1, len, BX_READ_16BIT_OPMASK(i->opmask()));              \
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
AVX512_2OP_DWORD_EL(VPSRAVD_MASK_VdqHdqWdqR, xmm_psravd)
AVX512_2OP_DWORD_EL(VPSRLVD_MASK_VdqHdqWdqR, xmm_psrlvd)
AVX512_2OP_DWORD_EL(VPSLLVD_MASK_VdqHdqWdqR, xmm_psllvd)
AVX512_2OP_DWORD_EL(VPRORVD_MASK_VdqHdqWdqR, xmm_prorvd)
AVX512_2OP_DWORD_EL(VPROLVD_MASK_VdqHdqWdqR, xmm_prolvd)
AVX512_2OP_DWORD_EL(VPMADDWD_MASK_VdqHdqWdqR, xmm_pmaddwd)

#define AVX512_2OP_WORD_EL(HANDLER, func)                                                   \
  /* AVX-512 instruction with two src operands working on WORD elements */                  \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regw_masked(i, &op1, len, BX_READ_32BIT_OPMASK(i->opmask()));              \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_2OP_WORD_EL(VPADDW_MASK_VdqHdqWdqR, xmm_paddw)
AVX512_2OP_WORD_EL(VPADDSW_MASK_VdqHdqWdqR, xmm_paddsw)
AVX512_2OP_WORD_EL(VPADDUSW_MASK_VdqHdqWdqR, xmm_paddusw)
AVX512_2OP_WORD_EL(VPSUBW_MASK_VdqHdqWdqR, xmm_psubw)
AVX512_2OP_WORD_EL(VPSUBSW_MASK_VdqHdqWdqR, xmm_psubsw)
AVX512_2OP_WORD_EL(VPSUBUSW_MASK_VdqHdqWdqR, xmm_psubusw)
AVX512_2OP_WORD_EL(VPMINSW_MASK_VdqHdqWdqR, xmm_pminsw)
AVX512_2OP_WORD_EL(VPMINUW_MASK_VdqHdqWdqR, xmm_pminuw)
AVX512_2OP_WORD_EL(VPMAXSW_MASK_VdqHdqWdqR, xmm_pmaxsw)
AVX512_2OP_WORD_EL(VPMAXUW_MASK_VdqHdqWdqR, xmm_pmaxuw)
AVX512_2OP_WORD_EL(VPMADDUBSW_MASK_VdqHdqWdqR, xmm_pmaddubsw)
AVX512_2OP_WORD_EL(VPAVGW_MASK_VdqHdqWdqR, xmm_pavgw)
AVX512_2OP_WORD_EL(VPMULLW_MASK_VdqHdqWdqR, xmm_pmullw)
AVX512_2OP_WORD_EL(VPMULHW_MASK_VdqHdqWdqR, xmm_pmulhw)
AVX512_2OP_WORD_EL(VPMULHUW_MASK_VdqHdqWdqR, xmm_pmulhuw)
AVX512_2OP_WORD_EL(VPMULHRSW_MASK_VdqHdqWdqR, xmm_pmulhrsw)
AVX512_2OP_WORD_EL(VPACKSSDW_MASK_VdqHdqWdqR, xmm_packssdw)
AVX512_2OP_WORD_EL(VPACKUSDW_MASK_VdqHdqWdqR, xmm_packusdw)
AVX512_2OP_WORD_EL(VPUNPCKLWD_MASK_VdqHdqWdqR, xmm_punpcklwd)
AVX512_2OP_WORD_EL(VPUNPCKHWD_MASK_VdqHdqWdqR, xmm_punpckhwd)
AVX512_2OP_WORD_EL(VPSRAVW_MASK_VdqHdqWdqR, xmm_psravw)
AVX512_2OP_WORD_EL(VPSRLVW_MASK_VdqHdqWdqR, xmm_psrlvw)
AVX512_2OP_WORD_EL(VPSLLVW_MASK_VdqHdqWdqR, xmm_psllvw)

#define AVX512_2OP_BYTE_EL(HANDLER, func)                                                   \
  /* AVX-512 instruction with two src operands working on BYTE elements */                  \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    avx512_write_regb_masked(i, &op1, len, BX_READ_OPMASK(i->opmask()));                    \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX512_2OP_BYTE_EL(VPADDB_MASK_VdqHdqWdqR, xmm_paddb)
AVX512_2OP_BYTE_EL(VPADDSB_MASK_VdqHdqWdqR, xmm_paddsb)
AVX512_2OP_BYTE_EL(VPADDUSB_MASK_VdqHdqWdqR, xmm_paddusb)
AVX512_2OP_BYTE_EL(VPSUBB_MASK_VdqHdqWdqR, xmm_psubb)
AVX512_2OP_BYTE_EL(VPSUBSB_MASK_VdqHdqWdqR, xmm_psubsb)
AVX512_2OP_BYTE_EL(VPSUBUSB_MASK_VdqHdqWdqR, xmm_psubusb)
AVX512_2OP_BYTE_EL(VPMINSB_MASK_VdqHdqWdqR, xmm_pminsb)
AVX512_2OP_BYTE_EL(VPMINUB_MASK_VdqHdqWdqR, xmm_pminub)
AVX512_2OP_BYTE_EL(VPMAXUB_MASK_VdqHdqWdqR, xmm_pmaxub)
AVX512_2OP_BYTE_EL(VPMAXSB_MASK_VdqHdqWdqR, xmm_pmaxsb)
AVX512_2OP_BYTE_EL(VPAVGB_MASK_VdqHdqWdqR, xmm_pavgb)
AVX512_2OP_BYTE_EL(VPACKSSWB_MASK_VdqHdqWdqR, xmm_packsswb)
AVX512_2OP_BYTE_EL(VPACKUSWB_MASK_VdqHdqWdqR, xmm_packuswb)
AVX512_2OP_BYTE_EL(VPUNPCKLBW_MASK_VdqHdqWdqR, xmm_punpcklbw)
AVX512_2OP_BYTE_EL(VPUNPCKHBW_MASK_VdqHdqWdqR, xmm_punpckhbw)

#define AVX512_PSHIFT_WORD_EL(HANDLER, func)                                  \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src1());                     \
    Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());                       \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), count);                                           \
                                                                              \
    avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask())); \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_WORD_EL(VPSRLW_MASK_VdqHdqWdqR, xmm_psrlw);
AVX512_PSHIFT_WORD_EL(VPSRAW_MASK_VdqHdqWdqR, xmm_psraw);
AVX512_PSHIFT_WORD_EL(VPSLLW_MASK_VdqHdqWdqR, xmm_psllw);

#define AVX512_PSHIFT_DWORD_EL(HANDLER, func)                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src1());                     \
    Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());                       \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), count);                                           \
                                                                              \
    avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask())); \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_DWORD_EL(VPSRLD_MASK_VdqHdqWdqR, xmm_psrld);
AVX512_PSHIFT_DWORD_EL(VPSRAD_MASK_VdqHdqWdqR, xmm_psrad);
AVX512_PSHIFT_DWORD_EL(VPSLLD_MASK_VdqHdqWdqR, xmm_pslld);

#define AVX512_PSHIFT_QWORD_EL(HANDLER, func)                                 \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src1());                     \
    Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());                       \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), count);                                           \
                                                                              \
    avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));  \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_QWORD_EL(VPSRLQ_MASK_VdqHdqWdqR, xmm_psrlq);
AVX512_PSHIFT_QWORD_EL(VPSRAQ_MASK_VdqHdqWdqR, xmm_psraq);
AVX512_PSHIFT_QWORD_EL(VPSLLQ_MASK_VdqHdqWdqR, xmm_psllq);

#define AVX512_PSHIFT_IMM_WORD_EL(HANDLER, func)                              \
  /* AVX packed shift with imm8 instruction */                                \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src());                      \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), i->Ib());                                         \
                                                                              \
    avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask())); \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_IMM_WORD_EL(VPSRLW_MASK_UdqIb, xmm_psrlw);
AVX512_PSHIFT_IMM_WORD_EL(VPSRAW_MASK_UdqIb, xmm_psraw);
AVX512_PSHIFT_IMM_WORD_EL(VPSLLW_MASK_UdqIb, xmm_psllw);

#define AVX512_PSHIFT_IMM_DWORD_EL(HANDLER, func)                             \
  /* AVX packed shift with imm8 instruction */                                \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src());                      \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), i->Ib());                                         \
                                                                              \
    avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask())); \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_IMM_DWORD_EL(VPSRLD_MASK_UdqIb, xmm_psrld);
AVX512_PSHIFT_IMM_DWORD_EL(VPSRAD_MASK_UdqIb, xmm_psrad);
AVX512_PSHIFT_IMM_DWORD_EL(VPSLLD_MASK_UdqIb, xmm_pslld);
AVX512_PSHIFT_IMM_DWORD_EL(VPRORD_MASK_UdqIb, xmm_prord);
AVX512_PSHIFT_IMM_DWORD_EL(VPROLD_MASK_UdqIb, xmm_prold);

#define AVX512_PSHIFT_IMM_QWORD_EL(HANDLER, func)                             \
  /* AVX packed shift with imm8 instruction */                                \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
  {                                                                           \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src());                      \
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), i->Ib());                                         \
                                                                              \
    avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));  \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_IMM_QWORD_EL(VPSRLQ_MASK_UdqIb, xmm_psrlq);
AVX512_PSHIFT_IMM_QWORD_EL(VPSRAQ_MASK_UdqIb, xmm_psraq);
AVX512_PSHIFT_IMM_QWORD_EL(VPSLLQ_MASK_UdqIb, xmm_psllq);
AVX512_PSHIFT_IMM_QWORD_EL(VPRORQ_MASK_UdqIb, xmm_prorq);
AVX512_PSHIFT_IMM_QWORD_EL(VPROLQ_MASK_UdqIb, xmm_prolq);

// concatenate and shift

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDW_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 15;

  if (count) {
    for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
      op1.vmm16u(n) = (op1.vmm16u(n) << count) | (op2.vmm16u(n) >> (16 - count));
    }
  }

  if (i->opmask())
    avx512_write_regw_masked(i, &op1, len, BX_READ_32BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDVW_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm16u(n) & 15;
    if (count) {
      dst.vmm16u(n) = (dst.vmm16u(n) << count) | (op1.vmm16u(n) >> (16 - count));
    }
  }

  if (i->opmask())
    avx512_write_regw_masked(i, &dst, len, BX_READ_32BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDD_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 31;

  if (count) {
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
      op1.vmm32u(n) = (op1.vmm32u(n) << count) | (op2.vmm32u(n) >> (32 - count));
    }
  }

  if (i->opmask())
    avx512_write_regd_masked(i, &op1, len, BX_READ_16BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDVD_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm32u(n) & 31;
    if (count) {
      dst.vmm32u(n) = (dst.vmm32u(n) << count) | (op1.vmm32u(n) >> (32 - count));
    }
  }

  if (i->opmask())
    avx512_write_regd_masked(i, &dst, len, BX_READ_16BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 63;

  if (count) {
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
      op1.vmm64u(n) = (op1.vmm64u(n) << count) | (op2.vmm64u(n) >> (64 - count));
    }
  }

  if (i->opmask())
    avx512_write_regq_masked(i, &op1, len, BX_READ_8BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHLDVQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm64u(n) & 63;
    if (count) {
      dst.vmm64u(n) = (dst.vmm64u(n) << count) | (op1.vmm64u(n) >> (64 - count));
    }
  }

  if (i->opmask())
    avx512_write_regq_masked(i, &dst, len, BX_READ_8BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDW_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 15;

  if (count) {
    for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
      op1.vmm16u(n) = (op1.vmm16u(n) >> count) | (op2.vmm16u(n) << (16 - count));
    }
  }

  if (i->opmask())
    avx512_write_regw_masked(i, &op1, len, BX_READ_32BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDVW_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm16u(n) & 15;
    if (count) {
      dst.vmm16u(n) = (dst.vmm16u(n) >> count) | (op1.vmm16u(n) << (16 - count));
    }
  }

  if (i->opmask())
    avx512_write_regw_masked(i, &dst, len, BX_READ_32BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDD_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 31;

  if (count) {
    for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
      op1.vmm32u(n) = (op1.vmm32u(n) >> count) | (op2.vmm32u(n) << (32 - count));
    }
  }

  if (i->opmask())
    avx512_write_regd_masked(i, &op1, len, BX_READ_16BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDVD_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm32u(n) & 31;
    if (count) {
      dst.vmm32u(n) = (dst.vmm32u(n) >> count) | (op1.vmm32u(n) << (32 - count));
    }
  }

  if (i->opmask())
    avx512_write_regd_masked(i, &dst, len, BX_READ_16BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned count = i->Ib() & 63;

  if (count) {
    for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
      op1.vmm64u(n) = (op1.vmm64u(n) >> count) | (op2.vmm64u(n) << (64 - count));
    }
  }

  if (i->opmask())
    avx512_write_regq_masked(i, &op1, len, BX_READ_8BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHRDVQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    unsigned count = op2.vmm64u(n) & 63;
    if (count) {
      dst.vmm64u(n) = (dst.vmm64u(n) >> count) | (op1.vmm64u(n) << (64 - count));
    }
  }

  if (i->opmask())
    avx512_write_regq_masked(i, &dst, len, BX_READ_8BIT_OPMASK(i->opmask()));
  else
    BX_WRITE_AVX_REGZ(i->dst(), dst, len);

  BX_NEXT_INSTR(i);
}


// absolute value

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSB_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsb(&op.vmm128(n));

  avx512_write_regb_masked(i, &op, len, BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSW_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsw(&op.vmm128(n));

  avx512_write_regw_masked(i, &op, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsd(&op.vmm128(n));

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsq(&op.vmm128(n));

  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

// shuffle and permute

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFHW_MASK_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshufhw(&result.vmm128(n), &op.vmm128(n), order);

  avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFLW_MASK_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshuflw(&result.vmm128(n), &op.vmm128(n), order);

  avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFB_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshufb(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  avx512_write_regb_masked(i, &result, len, BX_READ_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), i->Ib());

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;

  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_shufpd(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), order);
    order >>= 2;
  }

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilps(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilpd(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_MASK_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.vmm128(n), &op1.vmm128(n), &op1.vmm128(n), i->Ib());

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_MASK_VpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  for (unsigned n=0; n < len; n++) {
    xmm_shufpd(&result.vmm128(n), &op1.vmm128(n), &op1.vmm128(n), order);
    order >>= 2;
  }

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFF32x4_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  if (len == BX_VL256) {
    result.vmm128(0) = op1.vmm128(order & 0x1);
    result.vmm128(1) = op2.vmm128((order>>1) & 0x1);
  }
  else {
    result.vmm128(0) = op1.vmm128(order & 0x3);
    result.vmm128(1) = op1.vmm128((order>>2) & 0x3);
    result.vmm128(2) = op2.vmm128((order>>4) & 0x3);
    result.vmm128(3) = op2.vmm128((order>>6) & 0x3);
  }

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFF64x2_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();
  Bit8u order = i->Ib();

  if (len == BX_VL256) {
    result.vmm128(0) = op1.vmm128(order & 0x1);
    result.vmm128(1) = op2.vmm128((order>>1) & 0x1);
  }
  else {
    result.vmm128(0) = op1.vmm128(order & 0x3);
    result.vmm128(1) = op1.vmm128((order>>2) & 0x3);
    result.vmm128(2) = op2.vmm128((order>>4) & 0x3);
    result.vmm128(3) = op2.vmm128((order>>6) & 0x3);
  }

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPALIGNR_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++)
    xmm_palignr(&op2.vmm128(n), &op1.vmm128(n), i->Ib());

  avx512_write_regb_masked(i, &op2, len, BX_READ_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VALIGND_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements_mask = DWORD_ELEMENTS(len) - 1;
  unsigned shift = i->Ib() & elements_mask;

  for (unsigned n=0; n <= elements_mask; n++) {
    unsigned index = (shift + n) & elements_mask;
    result.vmm32u(n) = ((n + shift) <= elements_mask) ? op2.vmm32u(index) : op1.vmm32u(index);
  }

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VALIGNQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements_mask = QWORD_ELEMENTS(len) - 1;
  unsigned shift = i->Ib() & elements_mask;

  for (unsigned n=0; n <= elements_mask; n++) {
    unsigned index = (shift + n) & elements_mask;
    result.vmm64u(n) = ((n + shift) <= elements_mask) ? op2.vmm64u(index) : op1.vmm64u(index);
  }

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMQ_MASK_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u control = i->Ib();
  unsigned len = i->getVL();

  ymm_vpermq(&result.vmm256(0), &op.vmm256(0), control);
  if (len == BX_VL512)
    ymm_vpermq(&result.vmm256(1), &op.vmm256(1), control);

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMT2B_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = BYTE_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (op1.vmmubyte(n) & shuffle_control_mask);
    result.vmmubyte(n) = (op1.vmmubyte(n) & elements) ? op2.vmmubyte(shuffle_control) : dst.vmmubyte(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regb_masked(i, &result, len, BX_READ_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMT2W_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = WORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (op1.vmm16u(n) & shuffle_control_mask);
    result.vmm16u(n) = (op1.vmm16u(n) & elements) ? op2.vmm16u(shuffle_control) : dst.vmm16u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMT2PS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = DWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (op1.vmm32u(n) & shuffle_control_mask);
    result.vmm32u(n) = (op1.vmm32u(n) & elements) ? op2.vmm32u(shuffle_control) : dst.vmm32u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMT2PD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = QWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (op1.vmm64u(n) & shuffle_control_mask);
    result.vmm64u(n) = (op1.vmm64u(n) & elements) ? op2.vmm64u(shuffle_control) : dst.vmm64u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMI2B_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = BYTE_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (dst.vmmubyte(n) & shuffle_control_mask);
    result.vmmubyte(n) = (dst.vmmubyte(n) & elements) ? op2.vmmubyte(shuffle_control) : op1.vmmubyte(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regb_masked(i, &result, len, BX_READ_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMI2W_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = WORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (dst.vmm16u(n) & shuffle_control_mask);
    result.vmm16u(n) = (dst.vmm16u(n) & elements) ? op2.vmm16u(shuffle_control) : op1.vmm16u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMI2PS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = DWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (dst.vmm32u(n) & shuffle_control_mask);
    result.vmm32u(n) = (dst.vmm32u(n) & elements) ? op2.vmm32u(shuffle_control) : op1.vmm32u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMI2PD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2());
  BxPackedAvxRegister dst = BX_READ_AVX_REG(i->dst()), result;
  unsigned len = i->getVL(), elements = QWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0; n < elements; n++) {
    unsigned shuffle_control = (unsigned) (dst.vmm64u(n) & shuffle_control_mask);
    result.vmm64u(n) = (dst.vmm64u(n) & elements) ? op2.vmm64u(shuffle_control) : op1.vmm64u(shuffle_control);
  }

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMB_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements = BYTE_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0;n < elements;n++)
    result.vmmubyte(n) = op2.vmmubyte(op1.vmmubyte(n) & shuffle_control_mask);

  if (i->opmask()) {
    avx512_write_regb_masked(i, &result, len, BX_READ_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMW_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements = WORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0;n < elements;n++)
    result.vmm16u(n) = op2.vmm16u(op1.vmm16u(n) & shuffle_control_mask);

  if (i->opmask()) {
    avx512_write_regw_masked(i, &result, len, BX_READ_32BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements = DWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0;n < elements;n++)
    result.vmm32u(n) = op2.vmm32u(op1.vmm32u(n) & shuffle_control_mask);

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL(), elements = QWORD_ELEMENTS(len);
  unsigned shuffle_control_mask = elements - 1;

  for (unsigned n=0;n < elements;n++)
    result.vmm64u(n) = op2.vmm64u(op1.vmm64u(n) & shuffle_control_mask);

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF32x4_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL();
  unsigned offset = i->Ib() & (len-1);
  
  op.vmm128(offset) = BX_READ_XMM_REG(i->src2());
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF64x2_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  unsigned len = i->getVL();
  unsigned offset = i->Ib() & (len-1);
  
  op.vmm128(offset) = BX_READ_XMM_REG(i->src2());
  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF64x4_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  op.vmm256(i->Ib() & 0x1) = BX_READ_YMM_REG(i->src2());
  BX_WRITE_AVX_REGZ(i->dst(), op, BX_VL512);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF64x4_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  op.vmm256(i->Ib() & 0x1) = BX_READ_YMM_REG(i->src2());
  avx512_write_regq_masked(i, &op, BX_VL512, BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VINSERTF32x8_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src1());
  op.vmm256(i->Ib() & 0x1) = BX_READ_YMM_REG(i->src2());
  avx512_write_regd_masked(i, &op, BX_VL512, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF32x4_MASK_WpsVpsIbR(bxInstruction_c *i)
{
  unsigned len = i->getVL(), offset = i->Ib() & (len - 1);
  BxPackedXmmRegister op = BX_READ_AVX_REG_LANE(i->src(), offset);

  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());

  if (i->isZeroMasking())
     xmm_zero_blendps(&BX_READ_XMM_REG(i->dst()), &op, mask);
  else
     xmm_blendps(&BX_READ_XMM_REG(i->dst()), &op, mask);

  BX_CLEAR_AVX_HIGH128(i->dst());
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF32x4_MASK_WpsVpsIbM(bxInstruction_c *i)
{
  unsigned len = i->getVL(), offset = i->Ib() & (len - 1);
  BxPackedAvxRegister op;
  op.vmm128(0) = BX_READ_AVX_REG_LANE(i->src(), offset);

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask()) & 0xf;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &op, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x2_MASK_WpdVpdIbR(bxInstruction_c *i)
{
  unsigned len = i->getVL(), offset = i->Ib() & (len - 1);
  BxPackedXmmRegister op = BX_READ_AVX_REG_LANE(i->src(), offset);

  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());

  if (i->isZeroMasking())
     xmm_zero_blendpd(&BX_READ_XMM_REG(i->dst()), &op, mask);
  else
     xmm_blendpd(&BX_READ_XMM_REG(i->dst()), &op, mask);

  BX_CLEAR_AVX_HIGH128(i->dst());
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x2_MASK_WpdVpdIbM(bxInstruction_c *i)
{
  unsigned len = i->getVL(), offset = i->Ib() & (len - 1);
  BxPackedAvxRegister op;
  op.vmm128(0) = BX_READ_AVX_REG_LANE(i->src(), offset);

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask()) & 0x3;
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store64(i, eaddr, &op, opmask);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x4_WpdVpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  BX_WRITE_YMM_REGZ(i->dst(), op.vmm256(i->Ib() & 0x1));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x4_MASK_WpdVpdIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  if (i->Ib() & 0x1)
    op.vmm256(0) = op.vmm256(1);

  avx512_write_regq_masked(i, &op, BX_VL256, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x4_WpdVpdIbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_ymmword(i->seg(), eaddr, &op.vmm256(i->Ib() & 0x1));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF64x4_MASK_WpdVpdIbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  if (i->Ib() & 0x1)
    op.vmm256(0) = op.vmm256(1);

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask()) & 0xf;

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store64(i, eaddr, &op, opmask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF32x8_MASK_WpsVpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  if (i->Ib() & 0x1)
    op.vmm256(0) = op.vmm256(1);

  avx512_write_regd_masked(i, &op, BX_VL256, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXTRACTF32x8_MASK_WpsVpsIbM(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  if (i->Ib() & 0x1)
    op.vmm256(0) = op.vmm256(1);

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  avx_masked_store32(i, eaddr, &op, opmask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVDDUP_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n+=2) {
    op.vmm64u(n+1) = op.vmm64u(n);
  }

  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSLDUP_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n+=2) {
    op.vmm32u(n+1) = op.vmm32u(n);
  }

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMOVSHDUP_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n+=2) {
    op.vmm32u(n) = op.vmm32u(n+1);
  }

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

// special bit operations

BX_CPP_INLINE Bit32u ternlogd_scalar(Bit32u op1, Bit32u op2, Bit32u op3, unsigned imm8)
{
  Bit32u result = 0;

  for (unsigned bit = 0; bit < 32; bit++) {
    unsigned tmp  = (op1 >> bit) & 0x1;
             tmp <<= 1;
             tmp |= (op2 >> bit) & 0x1;
             tmp <<= 1;
             tmp |= (op3 >> bit) & 0x1;

    result |= ((Bit32u)((imm8 >> tmp) & 0x1)) << bit;
  }

  return result;
}

BX_CPP_INLINE Bit64u ternlogq_scalar(Bit64u op1, Bit64u op2, Bit64u op3, unsigned imm8)
{
  Bit64u result = 0;

  for (unsigned bit = 0; bit < 64; bit++) {
    unsigned tmp  = (op1 >> bit) & 0x1;
             tmp <<= 1;
             tmp |= (op2 >> bit) & 0x1;
             tmp <<= 1;
             tmp |= (op3 >> bit) & 0x1;

    result |= ((Bit64u)((imm8 >> tmp) & 0x1)) << bit;
  }

  return result;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGD_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = DWORD_ELEMENTS(len);
  Bit8u imm8 = i->Ib();

  for (unsigned n=0; n < num_elements; n++) {
    op1.vmm32u(n) = ternlogd_scalar(op1.vmm32u(n), op2.vmm32u(n), op3.vmm32u(n), imm8);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGD_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = DWORD_ELEMENTS(len);
  Bit8u imm8 = i->Ib();

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      op1.vmm32u(n) = ternlogd_scalar(op1.vmm32u(n), op2.vmm32u(n), op3.vmm32u(n), imm8);
    else
      if (i->isZeroMasking()) op1.vmm32u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGQ_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = QWORD_ELEMENTS(len);
  Bit8u imm8 = i->Ib();

  for (unsigned n=0; n < num_elements; n++) {
    op1.vmm64u(n) = ternlogq_scalar(op1.vmm64u(n), op2.vmm64u(n), op3.vmm64u(n), imm8);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = QWORD_ELEMENTS(len);
  Bit8u imm8 = i->Ib();

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());

  for (unsigned n=0; n < num_elements; n++, opmask >>= 1) {
    if (opmask & 0x1)
      op1.vmm64u(n) = ternlogq_scalar(op1.vmm64u(n), op2.vmm64u(n), op3.vmm64u(n), imm8);
    else
      if (i->isZeroMasking()) op1.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

// blend

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDMB_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit64u opmask = (i->opmask() != 0) ? BX_READ_OPMASK(i->opmask()) : BX_CONST64(0xffffffffffffffff);

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 16)
      xmm_zero_pblendb(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 16)
      xmm_pblendb(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDMW_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u opmask = (i->opmask() != 0) ? BX_READ_32BIT_OPMASK(i->opmask()) : 0xffffffff;

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_zero_pblendw(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u opmask = (i->opmask() != 0) ? BX_READ_16BIT_OPMASK(i->opmask()) : 0xffff;

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_zero_blendps(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_blendps(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u opmask = (i->opmask() != 0) ? BX_READ_8BIT_OPMASK(i->opmask()) : 0xff;

  if (i->isZeroMasking()) {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_zero_blendpd(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_blendpd(&op1.vmm128(n), &op2.vmm128(n), opmask);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

// compress, expand

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPEXPANDB_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  Bit64u opmask = BX_READ_OPMASK(i->opmask()), mask = opmask;
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*16; n++, mask >>= 1) {
    if (mask & 0x1) {
      result.vmmubyte(n) = op.vmmubyte(k);
      k++;
    }
    else {
      result.vmmubyte(n) = 0;
    }
  }

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 16)
      xmm_pblendb(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPEXPANDW_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  Bit32u opmask = BX_READ_32BIT_OPMASK(i->opmask()), mask = opmask;
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*8; n++, mask >>= 1) {
    if (mask & 0x1) {
      result.vmm16u(n) = op.vmm16u(k);
      k++;
    }
    else {
      result.vmm16u(n) = 0;
    }
  }

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 8)
      xmm_pblendw(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXPANDPS_MASK_VpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask()), mask = opmask;
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*4; n++, mask >>= 1) {
    if (mask & 0x1) {
      result.vmm32u(n) = op.vmm32u(k);
      k++;
    }
    else {
      result.vmm32u(n) = 0;
    }
  }

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 4)
      xmm_blendps(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXPANDPD_MASK_VpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask()), mask = opmask;
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*2; n++, mask >>= 1) {
    if (mask & 0x1) {
      result.vmm64u(n) = op.vmm64u(k);
      k++;
    }
    else {
      result.vmm64u(n) = 0;
    }
  }

  if (i->isZeroMasking()) {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }
  else {
    for (unsigned n=0; n < len; n++, opmask >>= 2)
      xmm_blendpd(&BX_READ_AVX_REG_LANE(i->dst(), n), &result.vmm128(n), opmask);

    BX_CLEAR_AVX_REGZ(i->dst(), len);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMPRESSB_MASK_WdqVdq(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  
  Bit64u opmask = BX_READ_OPMASK(i->opmask());
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*16; n++, opmask >>= 1) {
    if (opmask & 0x1) {
      result.vmmubyte(k) = op.vmmubyte(n);
      k++;
    }
    if (! opmask) break;
  }

  Bit64u writemask = (BX_CONST64(1) << k) - 1;

  if (i->modC0()) {
    avx512_write_regb_masked(i, &result, len, writemask);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    avx_masked_store8(i, eaddr, &result, writemask);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCOMPRESSW_MASK_WdqVdq(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->opmask());
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*8; n++, opmask >>= 1) {
    if (opmask & 0x1) {
      result.vmm16u(k) = op.vmm16u(n);
      k++;
    }
    if (! opmask) break;
  }

  Bit32u writemask = (1 << k) - 1;

  if (i->modC0()) {
    avx512_write_regw_masked(i, &result, len, writemask);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    avx_masked_store16(i, eaddr, &result, writemask);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMPRESSPS_MASK_WpsVps(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  
  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*4; n++, opmask >>= 1) {
    if (opmask & 0x1) {
      result.vmm32u(k) = op.vmm32u(n);
      k++;
    }
    if (! opmask) break;
  }

  Bit32u writemask = (1 << k) - 1;

  if (i->modC0()) {
    avx512_write_regd_masked(i, &result, len, writemask);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    avx_masked_store32(i, eaddr, &result, writemask);
  }

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMPRESSPD_MASK_WpdVpd(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  
  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL(), n = 0, k = 0;

  for (; n < len*2; n++, opmask >>= 1) {
    if (opmask & 0x1) {
      result.vmm64u(k) = op.vmm64u(n);
      k++;
    }
    if (! opmask) break;
  }

  Bit32u writemask = (1 << k) - 1;

  if (i->modC0()) {
    avx512_write_regq_masked(i, &result, len, writemask);
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
    avx_masked_store64(i, eaddr, &result, writemask);
  }

  BX_NEXT_INSTR(i);
}

// convert mask

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVM2B_VdqKEqR(bxInstruction_c *i)
{
  Bit64u opmask = BX_READ_OPMASK(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++) {
    xmm_pmovm2b(&BX_READ_AVX_REG_LANE(i->dst(), n), (Bit32u) opmask);
    opmask >>= 16;
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVM2W_VdqKEdR(bxInstruction_c *i)
{
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++) {
    xmm_pmovm2w(&BX_READ_AVX_REG_LANE(i->dst(), n), opmask);
    opmask >>= 8;
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVM2D_VdqKEwR(bxInstruction_c *i)
{
  Bit32u opmask = (Bit32u) BX_READ_16BIT_OPMASK(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++) {
    xmm_pmovm2d(&BX_READ_AVX_REG_LANE(i->dst(), n), opmask);
    opmask >>= 4;
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVM2Q_VdqKEbR(bxInstruction_c *i)
{
  Bit32u opmask = (Bit32u) BX_READ_8BIT_OPMASK(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++) {
    xmm_pmovm2q(&BX_READ_AVX_REG_LANE(i->dst(), n), opmask);
    opmask >>= 2;
  }

  BX_CLEAR_AVX_REGZ(i->dst(), len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVB2M_KGqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  Bit64u mask = 0;

  for (unsigned n=0; n<len; n++) {
    mask |= ((Bit64u) xmm_pmovmskb(&op.vmm128(n))) << (16*n);
  }

  BX_WRITE_OPMASK(i->dst(), mask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVW2M_KGdWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n<len; n++) {
    mask |= xmm_pmovmskw(&op.vmm128(n)) << (8*n);
  }

  BX_WRITE_OPMASK(i->dst(), mask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVD2M_KGwWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n<len; n++) {
    mask |= xmm_pmovmskd(&op.vmm128(n)) << (4*n);
  }

  BX_WRITE_OPMASK(i->dst(), mask);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVQ2M_KGbWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();
  Bit32u mask = 0;

  for (unsigned n=0; n<len; n++) {
    mask |= xmm_pmovmskq(&op.vmm128(n)) << (2*n);
  }

  BX_WRITE_OPMASK(i->dst(), mask);
  BX_NEXT_INSTR(i);
}

// sad (sum of absolute differences)

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VDBPSADBW_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst;

  Bit32u opmask = i->opmask() ? BX_READ_32BIT_OPMASK(i->opmask()) : (Bit32u) -1;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    BxPackedXmmRegister tmp;
    xmm_shufps(&tmp, &op2.vmm128(n), &op2.vmm128(n), i->Ib());
    xmm_dbpsadbw(&dst.vmm128(n), &op1.vmm128(n), &tmp);
  }

  avx512_write_regw_masked(i, &dst, len, opmask);

  BX_NEXT_INSTR(i);
}

// multishift (VBMI)

BX_CPP_INLINE Bit64u pmultishiftqb_scalar(Bit64u val_64, Bit64u control)
{
  // use packed register as 64-bit value with convinient accessors
  BxPackedRegister result;

  for (unsigned n=0; n < 8; n++, control >>= 8) {
    unsigned ctrl = (control & 0x3f);
    Bit64u tmp = val_64;
    if (ctrl != 0)
        tmp = (val_64 << (64 - ctrl)) | (val_64 >> ctrl);
    result.ubyte(n) = tmp & 0xff;
  }

  return MMXUQ(result);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMULTISHIFTQB_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    op1.vmm64u(n) = pmultishiftqb_scalar(op2.vmm64u(n), op1.vmm64u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMULTISHIFTQB_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      dst.vmm64u(n) = pmultishiftqb_scalar(op2.vmm64u(n), op1.vmm64u(n));
    else if (i->isZeroMasking())
      dst.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

// 52-bit integer FMA

BX_CPP_INLINE Bit64u pmadd52luq_scalar(Bit64u dst, Bit64u op1, Bit64u op2)
{
  op1 &= BX_CONST64(0x000fffffffffffff);
  op2 &= BX_CONST64(0x000fffffffffffff);

  return dst + ((op1 * op2) & BX_CONST64(0x000fffffffffffff));
}

BX_CPP_INLINE Bit64u pmadd52huq_scalar(Bit64u dst, Bit64u op1, Bit64u op2)
{
  op1 &= BX_CONST64(0x000fffffffffffff);
  op2 &= BX_CONST64(0x000fffffffffffff);

  Bit128u product_128;
  long_mul(&product_128, op1, op2);

  Bit64u temp = (product_128.lo >> 52) | ((product_128.hi & BX_CONST64(0x000000ffffffffff)) << 12);

  return dst + temp;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52LUQ_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm64u(n) = pmadd52luq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52LUQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      dst.vmm64u(n) = pmadd52luq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
    else if (i->isZeroMasking())
      dst.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52HUQ_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++) {
    dst.vmm64u(n) = pmadd52huq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMADD52HUQ_MASK_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), dst = BX_READ_AVX_REG(i->dst());
  Bit32u mask = BX_READ_8BIT_OPMASK(i->opmask());
  unsigned len = i->getVL();

  for (unsigned n=0, tmp_mask = mask; n < QWORD_ELEMENTS(len); n++, tmp_mask >>= 1) {
    if (tmp_mask & 0x1)
      dst.vmm64u(n) = pmadd52huq_scalar(dst.vmm64u(n), op1.vmm64u(n), op2.vmm64u(n));
    else if (i->isZeroMasking())
      dst.vmm64u(n) = 0;
  }

  BX_WRITE_AVX_REGZ(i->dst(), dst, len);
  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VP2INTERSECTD_KGqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit64u mask1 = 0, mask2 = 0;
  unsigned len = i->getVL();

  for (unsigned n=0;n < DWORD_ELEMENTS(len); n++) {
    for (unsigned m=0;m < DWORD_ELEMENTS(len); m++) {
      if (op1.vmm32u(n) == op2.vmm32u(m)) {
        mask1 |= 1<<n;
        mask2 |= 1<<m;
      }
    }
  }

  unsigned mask_base = i->dst() & ~1;
  BX_WRITE_OPMASK(mask_base,   mask1);
  BX_WRITE_OPMASK(mask_base+1, mask2);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VP2INTERSECTQ_KGqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  Bit64u mask1 = 0, mask2 = 0;
  unsigned len = i->getVL();

  for (unsigned n=0;n < QWORD_ELEMENTS(len); n++) {
    for (unsigned m=0;m < QWORD_ELEMENTS(len); m++) {
      if (op1.vmm64u(n) == op2.vmm64u(m)) {
        mask1 |= 1<<n;
        mask2 |= 1<<m;
      }
    }
  }

  unsigned mask_base = i->dst() & ~1;
  BX_WRITE_OPMASK(mask_base,   mask1);
  BX_WRITE_OPMASK(mask_base+1, mask2);

  BX_NEXT_INSTR(i);
}

#endif
