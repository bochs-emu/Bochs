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
  for (int n=len-1; n >= 0; n++) {
    result <<= 4;
    result |= avx512_compare32[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUD_MASK_KGwHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 4;
    result |= avx512_compare32u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 2;
    result |= avx512_compare64[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPUQ_MASK_KGbHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();
  unsigned ib = i->Ib() & 7;

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 2;
    result |= avx512_compare64u[ib](&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

///////////////////////////////////////////////////////////////////////////////////////////

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPGTD_MASK_KGwHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 4;
    result |= xmm_pcmpgtd_mask(&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPGTQ_MASK_KGbHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 2;
    result |= xmm_pcmpgtq_mask(&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPEQD_MASK_KGwHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 4;
    result |= xmm_pcmpeqd_mask(&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_16BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPCMPEQQ_MASK_KGbHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  Bit32u result = 0;
  for (int n=len-1; n >= 0; n++) {
    result <<= 2;
    result |= xmm_pcmpeqq_mask(&op1.vmm128(n), &op2.vmm128(n));
  }

  if (i->opmask())
    result &= (Bit32u) BX_READ_8BIT_OPMASK(i->opmask());

  BX_WRITE_OPMASK(i->dst(), result);
}

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
AVX512_2OP_DWORD_EL(VPSRAVD_MASK_VdqHdqWdqR, xmm_psravd)
AVX512_2OP_DWORD_EL(VPSRLVD_MASK_VdqHdqWdqR, xmm_psrlvd)
AVX512_2OP_DWORD_EL(VPSLLVD_MASK_VdqHdqWdqR, xmm_psllvd)
AVX512_2OP_DWORD_EL(VPRORVD_MASK_VdqHdqWdqR, xmm_prorvd)
AVX512_2OP_DWORD_EL(VPROLVD_MASK_VdqHdqWdqR, xmm_prolvd)

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
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), BX_READ_XMM_REG_LO_QWORD(i->src2()));             \
                                                                              \
    avx512_write_regd_masked(i, &op, BX_READ_16BIT_OPMASK(i->opmask()));      \
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
    unsigned len = i->getVL();                                                \
                                                                              \
    for (unsigned n=0; n < len; n++)                                          \
      (func)(&op.vmm128(n), BX_READ_XMM_REG_LO_QWORD(i->src2()));             \
                                                                              \
    avx512_write_regq_masked(i, &op, BX_READ_8BIT_OPMASK(i->opmask()));       \
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
    avx512_write_regd_masked(i, &op, BX_READ_16BIT_OPMASK(i->opmask()));      \
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
    avx512_write_regq_masked(i, &op, BX_READ_8BIT_OPMASK(i->opmask()));       \
                                                                              \
    BX_NEXT_INSTR(i);                                                         \
  }

AVX512_PSHIFT_IMM_QWORD_EL(VPSRLQ_MASK_UdqIb, xmm_psrlq);
AVX512_PSHIFT_IMM_QWORD_EL(VPSRAQ_MASK_UdqIb, xmm_psraq);
AVX512_PSHIFT_IMM_QWORD_EL(VPSLLQ_MASK_UdqIb, xmm_psllq);
AVX512_PSHIFT_IMM_QWORD_EL(VPRORQ_MASK_UdqIb, xmm_prorq);
AVX512_PSHIFT_IMM_QWORD_EL(VPROLQ_MASK_UdqIb, xmm_prolq);

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSD_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsd(&op.vmm128(n));

  avx512_write_regd_masked(i, &op, BX_READ_16BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPABSQ_MASK_VdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pabsq(&op.vmm128(n));

  avx512_write_regq_masked(i, &op, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VSHUFPS_MASK_VpsHpsWpsIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_shufps(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), i->Ib());

  avx512_write_regd_masked(i, &result, BX_READ_16BIT_OPMASK(i->opmask()));
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

  avx512_write_regq_masked(i, &result, BX_READ_8BIT_OPMASK(i->opmask()));
  BX_NEXT_INSTR(i);
}


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
