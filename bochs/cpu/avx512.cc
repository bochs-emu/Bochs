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

// compare

typedef Bit32u (*avx512_compare_method)(const BxPackedXmmRegister *op1, const BxPackedXmmRegister *op2);

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPD_MASK_KGwHdqWdqIbR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUD_MASK_KGwHdqWdqIbR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
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

#define AVX512_COMPARE_DWORD_EL(HANDLER, func)                                              \
  /* AVX-512 compare instruction with two src operands working on DWORD elements */         \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
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
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
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
AVX512_COMPARE_DWORD_EL(VPTESTMQ_MASK_KGbHdqWdqR, xmm_ptestmq_mask)
AVX512_COMPARE_DWORD_EL(VPTESTNMQ_MASK_KGbHdqWdqR, xmm_ptestnmq_mask)

// compute, shift and rotate

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

#define AVX512_2OP_QWORD_EL(HANDLER, func)                                                  \
  /* AVX-512 instruction with two src operands working on QWORD elements */                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
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
AVX512_2OP_QWORD_EL(VPSRAVQ_MASK_VdqHdqWdqR, xmm_psravq)
AVX512_2OP_QWORD_EL(VPSRLVQ_MASK_VdqHdqWdqR, xmm_psrlvq)
AVX512_2OP_QWORD_EL(VPSLLVQ_MASK_VdqHdqWdqR, xmm_psllvq)
AVX512_2OP_QWORD_EL(VPRORVQ_MASK_VdqHdqWdqR, xmm_prorvq)
AVX512_2OP_QWORD_EL(VPROLVQ_MASK_VdqHdqWdqR, xmm_prolvq)

#define AVX512_PSHIFT_DWORD_EL(HANDLER, func)                                 \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
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
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
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

#define AVX512_PSHIFT_IMM_DWORD_EL(HANDLER, func)                             \
  /* AVX packed shift with imm8 instruction */                                \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
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
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i) \
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

// absolute value

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsd(&op.vmm128(n));

  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsq(&op.vmm128(n));

  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

// shuffle and permute

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), i->Ib());

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPD_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilps(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_permilpd(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPS_MASK_VpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.vmm128(n), &op1.vmm128(n), &op1.vmm128(n), i->Ib());

  avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMILPD_MASK_VpdWpdIbR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFF32x4_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  if (len != BX_VL512) {
    BX_ERROR(("%s: vector length must be 512 bit", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  Bit8u order = i->Ib();

  result.vmm128(0) = op1.vmm128(order & 0x3);
  result.vmm128(1) = op1.vmm128((order>>2) & 0x3);
  result.vmm128(2) = op2.vmm128((order>>4) & 0x3);
  result.vmm128(3) = op2.vmm128((order>>6) & 0x3);

  if (i->opmask()) {
    avx512_write_regd_masked(i, &result, len, BX_READ_16BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFF64x2_MASK_VpdHpdWpdIbR(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  if (len != BX_VL512) {
    BX_ERROR(("%s: vector length must be 512 bit", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()), result;
  Bit8u order = i->Ib();

  result.vmm128(0) = op1.vmm128(order & 0x3);
  result.vmm128(1) = op1.vmm128((order>>2) & 0x3);
  result.vmm128(2) = op2.vmm128((order>>4) & 0x3);
  result.vmm128(3) = op2.vmm128((order>>6) & 0x3);

  if (i->opmask()) {
    avx512_write_regq_masked(i, &result, len, BX_READ_8BIT_OPMASK(i->opmask()));
  }
  else {
    BX_WRITE_AVX_REGZ(i->dst(), result, len);
  }

  BX_NEXT_INSTR(i);
}

// broadcast

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_MASK_VdqWdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_XMM_REG_LO_DWORD(i->src()), len * 4);
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_MASK_VdqWqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_XMM_REG_LO_QWORD(i->src()), len * 2);
  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_VdqEdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_32BIT_REG(i->src()), len * 4);
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_VdqEqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_64BIT_REG(i->src()), len * 2);
  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_MASK_VdqEdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastd(&op, BX_READ_32BIT_REG(i->src()), len * 4);
  avx512_write_regd_masked(i, &op, len, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_MASK_VdqEqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  simd_pbroadcastq(&op, BX_READ_64BIT_REG(i->src()), len * 2);
  avx512_write_regq_masked(i, &op, len, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF32x4_MASK_VpsMps(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedXmmRegister src;
  unsigned len = i->getVL();

#if BX_SUPPORT_EVEX
  if (len == BX_VL128) {
    BX_ERROR(("%s: vector length must be >= 256 bit", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
#endif

  Bit32u opmask = BX_READ_16BIT_OPMASK(i->opmask());
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    read_virtual_xmmword(i->seg(), eaddr, (Bit8u*) &src);

    for (unsigned n=0; n < len; n++) {
      dst.vmm128(n) = src;
    }

    avx512_write_regd_masked(i, &dst, len, opmask);
  }
  else {
    if (i->isZeroMasking()) {
      BX_CLEAR_AVX_REG(i->dst());
    }
    else {
      BX_CLEAR_AVX_REGZ(i->dst(), len);
    }
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF64x4_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedYmmRegister src;

#if BX_SUPPORT_EVEX
  if (i->getVL() != BX_VL512) {
    BX_ERROR(("%s: must have 512-bit vector length", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
#endif

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_ymmword(i->seg(), eaddr, (Bit8u*) &src);

  dst.vmm256(0) = src;
  dst.vmm256(1) = src;

  BX_WRITE_AVX_REG(i->dst(), dst);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBROADCASTF64x4_MASK_VpdMpd(bxInstruction_c *i)
{
  BxPackedAvxRegister dst;
  BxPackedYmmRegister src;

#if BX_SUPPORT_EVEX
  if (i->getVL() != BX_VL512) {
    BX_ERROR(("%s: must have 512-bit vector length", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }
#endif

  Bit32u opmask = BX_READ_8BIT_OPMASK(i->opmask());
  if (opmask != 0) {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    read_virtual_ymmword(i->seg(), eaddr, (Bit8u*) &src);

    dst.vmm256(0) = src;
    dst.vmm256(1) = src;

    avx512_write_regq_masked(i, &dst, BX_VL512, opmask);
  }
  else {
    if (i->isZeroMasking())
      BX_CLEAR_AVX_REG(i->dst());
  }

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGD_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = 4 * len;
  Bit8u imm8 = i->Ib();

  for (unsigned n=0; n < num_elements; n++) {
    op1.vmm32u(n) = ternlogd_scalar(op1.vmm32u(n), op2.vmm32u(n), op3.vmm32u(n), imm8);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGD_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = 4 * len;
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGQ_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = 2 * len;
  Bit8u imm8 = i->Ib();

  for (unsigned n=0; n < num_elements; n++) {
    op1.vmm64u(n) = ternlogq_scalar(op1.vmm64u(n), op2.vmm64u(n), op3.vmm64u(n), imm8);
  }

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPTERNLOGQ_MASK_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->dst()), 
                      op2 = BX_READ_AVX_REG(i->src1()),
                      op3 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL(), num_elements = 2 * len;
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPS_MASK_VpsHpsWpsR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VBLENDMPD_MASK_VpdHpdWpdR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXPANDPS_MASK_VpsWpsR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VEXPANDPD_MASK_VpdWpdR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMPRESSPS_MASK_WpsVps(bxInstruction_c *i)
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
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    avx_masked_store32(i, eaddr, &result, writemask);
  }
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VCOMPRESSPD_MASK_WpdVpd(bxInstruction_c *i)
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
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    avx_masked_store64(i, eaddr, &result, writemask);
  }
}

#endif
