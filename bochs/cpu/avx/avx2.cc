/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2014 Stanislav Shwartsman
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

#include "simd_int.h"
#include "simd_compare.h"

#define AVX_2OP(HANDLER, func)                                                              \
  /* AVX instruction with two src operands */                                               \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.vmm128(n), &op2.vmm128(n));                                               \
                                                                                            \
    BX_WRITE_AVX_REGZ(i->dst(), op1, len);                                                  \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_2OP(VANDPS_VpsHpsWpsR, xmm_andps)
AVX_2OP(VANDNPS_VpsHpsWpsR, xmm_andnps)
AVX_2OP(VXORPS_VpsHpsWpsR, xmm_xorps)
AVX_2OP(VORPS_VpsHpsWpsR, xmm_orps)

AVX_2OP(VUNPCKLPS_VpsHpsWpsR, xmm_unpcklps)
AVX_2OP(VUNPCKLPD_VpdHpdWpdR, xmm_unpcklpd)
AVX_2OP(VUNPCKHPS_VpsHpsWpsR, xmm_unpckhps)
AVX_2OP(VUNPCKHPD_VpdHpdWpdR, xmm_unpckhpd)

AVX_2OP(VPADDB_VdqHdqWdqR, xmm_paddb)
AVX_2OP(VPADDW_VdqHdqWdqR, xmm_paddw)
AVX_2OP(VPADDD_VdqHdqWdqR, xmm_paddd)
AVX_2OP(VPADDQ_VdqHdqWdqR, xmm_paddq)
AVX_2OP(VPSUBB_VdqHdqWdqR, xmm_psubb)
AVX_2OP(VPSUBW_VdqHdqWdqR, xmm_psubw)
AVX_2OP(VPSUBD_VdqHdqWdqR, xmm_psubd)
AVX_2OP(VPSUBQ_VdqHdqWdqR, xmm_psubq)

AVX_2OP(VPCMPEQB_VdqHdqWdqR, xmm_pcmpeqb)
AVX_2OP(VPCMPEQW_VdqHdqWdqR, xmm_pcmpeqw)
AVX_2OP(VPCMPEQD_VdqHdqWdqR, xmm_pcmpeqd)
AVX_2OP(VPCMPEQQ_VdqHdqWdqR, xmm_pcmpeqq)
AVX_2OP(VPCMPGTB_VdqHdqWdqR, xmm_pcmpgtb)
AVX_2OP(VPCMPGTW_VdqHdqWdqR, xmm_pcmpgtw)
AVX_2OP(VPCMPGTD_VdqHdqWdqR, xmm_pcmpgtd)
AVX_2OP(VPCMPGTQ_VdqHdqWdqR, xmm_pcmpgtq)

AVX_2OP(VPMINSB_VdqHdqWdqR, xmm_pminsb)
AVX_2OP(VPMINSW_VdqHdqWdqR, xmm_pminsw)
AVX_2OP(VPMINSD_VdqHdqWdqR, xmm_pminsd)
AVX_2OP(VPMINSQ_VdqHdqWdqR, xmm_pminsq)
AVX_2OP(VPMINUB_VdqHdqWdqR, xmm_pminub)
AVX_2OP(VPMINUW_VdqHdqWdqR, xmm_pminuw)
AVX_2OP(VPMINUD_VdqHdqWdqR, xmm_pminud)
AVX_2OP(VPMINUQ_VdqHdqWdqR, xmm_pminuq)
AVX_2OP(VPMAXSB_VdqHdqWdqR, xmm_pmaxsb)
AVX_2OP(VPMAXSW_VdqHdqWdqR, xmm_pmaxsw)
AVX_2OP(VPMAXSD_VdqHdqWdqR, xmm_pmaxsd)
AVX_2OP(VPMAXSQ_VdqHdqWdqR, xmm_pmaxsq)
AVX_2OP(VPMAXUB_VdqHdqWdqR, xmm_pmaxub)
AVX_2OP(VPMAXUW_VdqHdqWdqR, xmm_pmaxuw)
AVX_2OP(VPMAXUD_VdqHdqWdqR, xmm_pmaxud)
AVX_2OP(VPMAXUQ_VdqHdqWdqR, xmm_pmaxuq)

AVX_2OP(VPSIGNB_VdqHdqWdqR, xmm_psignb)
AVX_2OP(VPSIGNW_VdqHdqWdqR, xmm_psignw)
AVX_2OP(VPSIGND_VdqHdqWdqR, xmm_psignd)

AVX_2OP(VPSUBSB_VdqHdqWdqR, xmm_psubsb)
AVX_2OP(VPSUBSW_VdqHdqWdqR, xmm_psubsw)
AVX_2OP(VPSUBUSB_VdqHdqWdqR, xmm_psubusb)
AVX_2OP(VPSUBUSW_VdqHdqWdqR, xmm_psubusw)
AVX_2OP(VPADDSB_VdqHdqWdqR, xmm_paddsb)
AVX_2OP(VPADDSW_VdqHdqWdqR, xmm_paddsw)
AVX_2OP(VPADDUSB_VdqHdqWdqR, xmm_paddusb)
AVX_2OP(VPADDUSW_VdqHdqWdqR, xmm_paddusw)

AVX_2OP(VPHADDW_VdqHdqWdqR, xmm_phaddw)
AVX_2OP(VPHADDD_VdqHdqWdqR, xmm_phaddd)
AVX_2OP(VPHADDSW_VdqHdqWdqR, xmm_phaddsw)
AVX_2OP(VPHSUBW_VdqHdqWdqR, xmm_phsubw)
AVX_2OP(VPHSUBD_VdqHdqWdqR, xmm_phsubd)
AVX_2OP(VPHSUBSW_VdqHdqWdqR, xmm_phsubsw)

AVX_2OP(VPAVGB_VdqHdqWdqR, xmm_pavgb)
AVX_2OP(VPAVGW_VdqHdqWdqR, xmm_pavgw)

AVX_2OP(VPACKUSWB_VdqHdqWdqR, xmm_packuswb)
AVX_2OP(VPACKSSWB_VdqHdqWdqR, xmm_packsswb)
AVX_2OP(VPACKUSDW_VdqHdqWdqR, xmm_packusdw)
AVX_2OP(VPACKSSDW_VdqHdqWdqR, xmm_packssdw)

AVX_2OP(VPUNPCKLBW_VdqHdqWdqR, xmm_punpcklbw)
AVX_2OP(VPUNPCKLWD_VdqHdqWdqR, xmm_punpcklwd)
AVX_2OP(VPUNPCKHBW_VdqHdqWdqR, xmm_punpckhbw)
AVX_2OP(VPUNPCKHWD_VdqHdqWdqR, xmm_punpckhwd)

AVX_2OP(VPMULLQ_VdqHdqWdqR, xmm_pmullq)
AVX_2OP(VPMULLD_VdqHdqWdqR, xmm_pmulld)
AVX_2OP(VPMULLW_VdqHdqWdqR, xmm_pmullw)
AVX_2OP(VPMULHW_VdqHdqWdqR, xmm_pmulhw)
AVX_2OP(VPMULHUW_VdqHdqWdqR, xmm_pmulhuw)
AVX_2OP(VPMULDQ_VdqHdqWdqR, xmm_pmuldq)
AVX_2OP(VPMULUDQ_VdqHdqWdqR, xmm_pmuludq)
AVX_2OP(VPMULHRSW_VdqHdqWdqR, xmm_pmulhrsw)

AVX_2OP(VPMADDWD_VdqHdqWdqR, xmm_pmaddwd)
AVX_2OP(VPMADDUBSW_VdqHdqWdqR, xmm_pmaddubsw)

AVX_2OP(VPSADBW_VdqHdqWdqR, xmm_psadbw)

AVX_2OP(VPSRAVW_VdqHdqWdqR, xmm_psravw)
AVX_2OP(VPSRAVD_VdqHdqWdqR, xmm_psravd)
AVX_2OP(VPSRAVQ_VdqHdqWdqR, xmm_psravq)
AVX_2OP(VPSLLVW_VdqHdqWdqR, xmm_psllvw)
AVX_2OP(VPSLLVD_VdqHdqWdqR, xmm_psllvd)
AVX_2OP(VPSLLVQ_VdqHdqWdqR, xmm_psllvq)
AVX_2OP(VPSRLVW_VdqHdqWdqR, xmm_psrlvw)
AVX_2OP(VPSRLVD_VdqHdqWdqR, xmm_psrlvd)
AVX_2OP(VPSRLVQ_VdqHdqWdqR, xmm_psrlvq)
AVX_2OP(VPROLVD_VdqHdqWdqR, xmm_prolvd)
AVX_2OP(VPROLVQ_VdqHdqWdqR, xmm_prolvq)
AVX_2OP(VPRORVD_VdqHdqWdqR, xmm_prorvd)
AVX_2OP(VPRORVQ_VdqHdqWdqR, xmm_prorvq)

#define AVX_1OP(HANDLER, func)                                                             \
  /* AVX instruction with single src operand */                                            \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BxPackedAvxRegister op = BX_READ_AVX_REG(i->src());                                    \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.vmm128(n));                                                               \
                                                                                           \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                  \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_1OP(VPABSB_VdqWdqR, xmm_pabsb)
AVX_1OP(VPABSW_VdqWdqR, xmm_pabsw)
AVX_1OP(VPABSD_VdqWdqR, xmm_pabsd)
AVX_1OP(VPABSQ_VdqWdqR, xmm_pabsq)

#define AVX_PSHIFT(HANDLER, func)                                                          \
  /* AVX packed shift instruction */                                                       \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src1());                                  \
    Bit64u count = BX_READ_XMM_REG_LO_QWORD(i->src2());                                    \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.vmm128(n), count);                                                        \
                                                                                           \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                  \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_PSHIFT(VPSRLW_VdqHdqWdqR, xmm_psrlw);
AVX_PSHIFT(VPSRLD_VdqHdqWdqR, xmm_psrld);
AVX_PSHIFT(VPSRLQ_VdqHdqWdqR, xmm_psrlq);
AVX_PSHIFT(VPSRAW_VdqHdqWdqR, xmm_psraw);
AVX_PSHIFT(VPSRAD_VdqHdqWdqR, xmm_psrad);
AVX_PSHIFT(VPSRAQ_VdqHdqWdqR, xmm_psraq);
AVX_PSHIFT(VPSLLW_VdqHdqWdqR, xmm_psllw);
AVX_PSHIFT(VPSLLD_VdqHdqWdqR, xmm_pslld);
AVX_PSHIFT(VPSLLQ_VdqHdqWdqR, xmm_psllq);

#define AVX_PSHIFT_IMM(HANDLER, func)                                                      \
  /* AVX packed shift with imm8 instruction */                                             \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    BxPackedAvxRegister op  = BX_READ_AVX_REG(i->src());                                   \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.vmm128(n), i->Ib());                                                      \
                                                                                           \
    BX_WRITE_AVX_REGZ(i->dst(), op, len);                                                  \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_PSHIFT_IMM(VPSRLW_UdqIb, xmm_psrlw);
AVX_PSHIFT_IMM(VPSRLD_UdqIb, xmm_psrld);
AVX_PSHIFT_IMM(VPSRLQ_UdqIb, xmm_psrlq);
AVX_PSHIFT_IMM(VPSRAW_UdqIb, xmm_psraw);
AVX_PSHIFT_IMM(VPSRAD_UdqIb, xmm_psrad);
AVX_PSHIFT_IMM(VPSRAQ_UdqIb, xmm_psraq);
AVX_PSHIFT_IMM(VPSLLW_UdqIb, xmm_psllw);
AVX_PSHIFT_IMM(VPSLLD_UdqIb, xmm_pslld);
AVX_PSHIFT_IMM(VPSLLQ_UdqIb, xmm_psllq);
AVX_PSHIFT_IMM(VPROLD_UdqIb, xmm_prold);
AVX_PSHIFT_IMM(VPROLQ_UdqIb, xmm_prolq);
AVX_PSHIFT_IMM(VPRORD_UdqIb, xmm_prord);
AVX_PSHIFT_IMM(VPRORQ_UdqIb, xmm_prorq);

AVX_PSHIFT_IMM(VPSRLDQ_UdqIb, xmm_psrldq);
AVX_PSHIFT_IMM(VPSLLDQ_UdqIb, xmm_pslldq);

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshufhw(&result.vmm128(n), &op.vmm128(n), order);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op = BX_READ_AVX_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshuflw(&result.vmm128(n), &op.vmm128(n), order);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFB_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    xmm_pshufb(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n));

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPSADBW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1());
  BxPackedAvxRegister op2 = BX_READ_AVX_REG(i->src2()), result;

  Bit8u control = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    xmm_mpsadbw(&result.vmm128(n), &op1.vmm128(n), &op2.vmm128(n), control & 0x7);
    control >>= 3;
  }

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());

  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < len; n++)
    xmm_pblendw(&op1.vmm128(n), &op2.vmm128(n), mask);

  BX_WRITE_AVX_REGZ(i->dst(), op1, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_VdqWbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit8u val_8 = BX_READ_XMM_REG_LO_BYTE(i->src());
  
  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastb(&op.vmm128(n), val_8);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_VdqWwR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit16u val_16 = BX_READ_XMM_REG_LO_WORD(i->src());
  
  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastw(&op.vmm128(n), val_16);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_VdqWdR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit32u val_32 = BX_READ_XMM_REG_LO_DWORD(i->src());
  
  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastd(&op.vmm128(n), val_32);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_VdqWqR(bxInstruction_c *i)
{
  BxPackedAvxRegister op;
  unsigned len = i->getVL();

  Bit64u val_64 = BX_READ_XMM_REG_LO_QWORD(i->src());
  
  for (unsigned n=0; n < len; n++)
    xmm_pbroadcastq(&op.vmm128(n), val_64);

  BX_WRITE_AVX_REGZ(i->dst(), op, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16s(n) = (Bit16s) op.ymmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.xmmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmmsbyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32s(n) = (Bit32s) op.ymm16s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.xmm16s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64s(n) = (Bit64s) op.ymm32s(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < WORD_ELEMENTS(len); n++)
    result.vmm16u(n) = (Bit16u) op.ymmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.xmmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmmubyte(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWD_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < DWORD_ELEMENTS(len); n++)
    result.vmm32u(n) = (Bit32u) op.ymm16u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.xmm16u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXDQ_VdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());
  BxPackedAvxRegister result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < QWORD_ELEMENTS(len); n++)
    result.vmm64u(n) = (Bit64u) op.ymm32u(n);

  BX_WRITE_AVX_REGZ(i->dst(), result, len);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPALIGNR_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedAvxRegister op1 = BX_READ_AVX_REG(i->src1()), op2 = BX_READ_AVX_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++)
    xmm_palignr(&op2.vmm128(n), &op1.vmm128(n), i->Ib());

  BX_WRITE_AVX_REGZ(i->dst(), op2, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMD_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;

  for (unsigned n=0;n < 8;n++)
    result.ymm32u(n) = op2.ymm32u(op1.ymm32u(n) & 0x7);

  BX_WRITE_YMM_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMQ_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src()), result;

  ymm_vpermq(&result, &op, i->Ib());

  BX_WRITE_YMM_REGZ(i->dst(), result);
  BX_NEXT_INSTR(i);
}

#endif
