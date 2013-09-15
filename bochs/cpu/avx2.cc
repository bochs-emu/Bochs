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

#include "simd_int.h"
#include "simd_compare.h"

#define AVX_2OP(HANDLER, func)                                                              \
  /* AVX instruction with two src operands */                                               \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)              \
  {                                                                                         \
    BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2()); \
    unsigned len = i->getVL();                                                              \
                                                                                            \
    for (unsigned n=0; n < len; n++)                                                        \
      (func)(&op1.ymm128(n), &op2.ymm128(n));                                               \
                                                                                            \
    BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);                                             \
                                                                                            \
    BX_NEXT_INSTR(i);                                                                       \
  }

AVX_2OP(VANDPS_VpsHpsWpsR, sse_andps)
AVX_2OP(VANDNPS_VpsHpsWpsR, sse_andnps)
AVX_2OP(VXORPS_VpsHpsWpsR, sse_xorps)
AVX_2OP(VORPS_VpsHpsWpsR, sse_orps)

AVX_2OP(VUNPCKLPS_VpsHpsWpsR, sse_unpcklps)
AVX_2OP(VUNPCKLPD_VpdHpdWpdR, sse_unpcklpd)
AVX_2OP(VUNPCKHPS_VpsHpsWpsR, sse_unpckhps)
AVX_2OP(VUNPCKHPD_VpdHpdWpdR, sse_unpckhpd)

AVX_2OP(VPADDB_VdqHdqWdqR, sse_paddb)
AVX_2OP(VPADDW_VdqHdqWdqR, sse_paddw)
AVX_2OP(VPADDD_VdqHdqWdqR, sse_paddd)
AVX_2OP(VPADDQ_VdqHdqWdqR, sse_paddq)
AVX_2OP(VPSUBB_VdqHdqWdqR, sse_psubb)
AVX_2OP(VPSUBW_VdqHdqWdqR, sse_psubw)
AVX_2OP(VPSUBD_VdqHdqWdqR, sse_psubd)
AVX_2OP(VPSUBQ_VdqHdqWdqR, sse_psubq)

AVX_2OP(VPCMPEQB_VdqHdqWdqR, sse_pcmpeqb)
AVX_2OP(VPCMPEQW_VdqHdqWdqR, sse_pcmpeqw)
AVX_2OP(VPCMPEQD_VdqHdqWdqR, sse_pcmpeqd)
AVX_2OP(VPCMPEQQ_VdqHdqWdqR, sse_pcmpeqq)
AVX_2OP(VPCMPGTB_VdqHdqWdqR, sse_pcmpgtb)
AVX_2OP(VPCMPGTW_VdqHdqWdqR, sse_pcmpgtw)
AVX_2OP(VPCMPGTD_VdqHdqWdqR, sse_pcmpgtd)
AVX_2OP(VPCMPGTQ_VdqHdqWdqR, sse_pcmpgtq)

AVX_2OP(VPMINSB_VdqHdqWdqR, sse_pminsb)
AVX_2OP(VPMINSW_VdqHdqWdqR, sse_pminsw)
AVX_2OP(VPMINSD_VdqHdqWdqR, sse_pminsd)
AVX_2OP(VPMINUB_VdqHdqWdqR, sse_pminub)
AVX_2OP(VPMINUW_VdqHdqWdqR, sse_pminuw)
AVX_2OP(VPMINUD_VdqHdqWdqR, sse_pminud)
AVX_2OP(VPMAXSB_VdqHdqWdqR, sse_pmaxsb)
AVX_2OP(VPMAXSW_VdqHdqWdqR, sse_pmaxsw)
AVX_2OP(VPMAXSD_VdqHdqWdqR, sse_pmaxsd)
AVX_2OP(VPMAXUB_VdqHdqWdqR, sse_pmaxub)
AVX_2OP(VPMAXUW_VdqHdqWdqR, sse_pmaxuw)
AVX_2OP(VPMAXUD_VdqHdqWdqR, sse_pmaxud)

AVX_2OP(VPSIGNB_VdqHdqWdqR, sse_psignb)
AVX_2OP(VPSIGNW_VdqHdqWdqR, sse_psignw)
AVX_2OP(VPSIGND_VdqHdqWdqR, sse_psignd)

AVX_2OP(VPSUBSB_VdqHdqWdqR, sse_psubsb)
AVX_2OP(VPSUBSW_VdqHdqWdqR, sse_psubsw)
AVX_2OP(VPSUBUSB_VdqHdqWdqR, sse_psubusb)
AVX_2OP(VPSUBUSW_VdqHdqWdqR, sse_psubusw)
AVX_2OP(VPADDSB_VdqHdqWdqR, sse_paddsb)
AVX_2OP(VPADDSW_VdqHdqWdqR, sse_paddsw)
AVX_2OP(VPADDUSB_VdqHdqWdqR, sse_paddusb)
AVX_2OP(VPADDUSW_VdqHdqWdqR, sse_paddusw)

AVX_2OP(VPHADDW_VdqHdqWdqR, sse_phaddw)
AVX_2OP(VPHADDD_VdqHdqWdqR, sse_phaddd)
AVX_2OP(VPHADDSW_VdqHdqWdqR, sse_phaddsw)
AVX_2OP(VPHSUBW_VdqHdqWdqR, sse_phsubw)
AVX_2OP(VPHSUBD_VdqHdqWdqR, sse_phsubd)
AVX_2OP(VPHSUBSW_VdqHdqWdqR, sse_phsubsw)

AVX_2OP(VPAVGB_VdqHdqWdqR, sse_pavgb)
AVX_2OP(VPAVGW_VdqHdqWdqR, sse_pavgw)

AVX_2OP(VPACKUSWB_VdqHdqWdqR, sse_packuswb)
AVX_2OP(VPACKSSWB_VdqHdqWdqR, sse_packsswb)
AVX_2OP(VPACKUSDW_VdqHdqWdqR, sse_packusdw)
AVX_2OP(VPACKSSDW_VdqHdqWdqR, sse_packssdw)

AVX_2OP(VPUNPCKLBW_VdqHdqWdqR, sse_punpcklbw)
AVX_2OP(VPUNPCKLWD_VdqHdqWdqR, sse_punpcklwd)
AVX_2OP(VPUNPCKHBW_VdqHdqWdqR, sse_punpckhbw)
AVX_2OP(VPUNPCKHWD_VdqHdqWdqR, sse_punpckhwd)

AVX_2OP(VPMULLD_VdqHdqWdqR, sse_pmulld)
AVX_2OP(VPMULLW_VdqHdqWdqR, sse_pmullw)
AVX_2OP(VPMULHW_VdqHdqWdqR, sse_pmulhw)
AVX_2OP(VPMULHUW_VdqHdqWdqR, sse_pmulhuw)
AVX_2OP(VPMULDQ_VdqHdqWdqR, sse_pmuldq)
AVX_2OP(VPMULUDQ_VdqHdqWdqR, sse_pmuludq)
AVX_2OP(VPMULHRSW_VdqHdqWdqR, sse_pmulhrsw)

AVX_2OP(VPMADDWD_VdqHdqWdqR, sse_pmaddwd)
AVX_2OP(VPMADDUBSW_VdqHdqWdqR, sse_pmaddubsw)

AVX_2OP(VPSADBW_VdqHdqWdqR, sse_psadbw)

AVX_2OP(VPSRAVD_VdqHdqWdqR, sse_psravd)
AVX_2OP(VPSLLVD_VdqHdqWdqR, sse_psllvd)
AVX_2OP(VPSLLVQ_VdqHdqWdqR, sse_psllvq)
AVX_2OP(VPSRLVD_VdqHdqWdqR, sse_psrlvd)
AVX_2OP(VPSRLVQ_VdqHdqWdqR, sse_psrlvq)

#define AVX_1OP(HANDLER, func)                                                             \
  /* AVX instruction with single src operand */                                            \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BxPackedYmmRegister op = BX_READ_YMM_REG(i->src());                                    \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.ymm128(n));                                                               \
                                                                                           \
    BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);                                             \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_1OP(VPABSB_VdqWdqR, sse_pabsb)
AVX_1OP(VPABSW_VdqWdqR, sse_pabsw)
AVX_1OP(VPABSD_VdqWdqR, sse_pabsd)

#define AVX_PSHIFT(HANDLER, func)                                                          \
  /* AVX packed shift instruction */                                                       \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    BxPackedYmmRegister op  = BX_READ_YMM_REG(i->src1());                                  \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.ymm128(n), BX_READ_XMM_REG_LO_QWORD(i->src2()));                          \
                                                                                           \
    BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);                                             \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_PSHIFT(VPSRLW_VdqHdqWdqR, sse_psrlw);
AVX_PSHIFT(VPSRLD_VdqHdqWdqR, sse_psrld);
AVX_PSHIFT(VPSRLQ_VdqHdqWdqR, sse_psrlq);
AVX_PSHIFT(VPSRAW_VdqHdqWdqR, sse_psraw);
AVX_PSHIFT(VPSRAD_VdqHdqWdqR, sse_psrad);
AVX_PSHIFT(VPSLLW_VdqHdqWdqR, sse_psllw);
AVX_PSHIFT(VPSLLD_VdqHdqWdqR, sse_pslld);
AVX_PSHIFT(VPSLLQ_VdqHdqWdqR, sse_psllq);

#define AVX_PSHIFT_IMM(HANDLER, func)                                                      \
  /* AVX packed shift with imm8 instruction */                                             \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    BxPackedYmmRegister op  = BX_READ_YMM_REG(i->src());                                   \
    unsigned len = i->getVL();                                                             \
                                                                                           \
    for (unsigned n=0; n < len; n++)                                                       \
      (func)(&op.ymm128(n), i->Ib());                                                      \
                                                                                           \
    BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);                                             \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

AVX_PSHIFT_IMM(VPSRLW_UdqIb, sse_psrlw);
AVX_PSHIFT_IMM(VPSRLD_UdqIb, sse_psrld);
AVX_PSHIFT_IMM(VPSRLQ_UdqIb, sse_psrlq);
AVX_PSHIFT_IMM(VPSRAW_UdqIb, sse_psraw);
AVX_PSHIFT_IMM(VPSRAD_UdqIb, sse_psrad);
AVX_PSHIFT_IMM(VPSLLW_UdqIb, sse_psllw);
AVX_PSHIFT_IMM(VPSLLD_UdqIb, sse_pslld);
AVX_PSHIFT_IMM(VPSLLQ_UdqIb, sse_psllq);

AVX_PSHIFT_IMM(VPSRLDQ_UdqIb, sse_psrldq);
AVX_PSHIFT_IMM(VPSLLDQ_UdqIb, sse_pslldq);

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    sse_pshufhw(&result.ymm128(n), &op.ymm128(n), order);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op = BX_READ_YMM_REG(i->src()), result;
  Bit8u order = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    sse_pshuflw(&result.ymm128(n), &op.ymm128(n), order);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPSHUFB_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++)
    sse_pshufb(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n));

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMPSADBW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;

  Bit8u control = i->Ib();
  unsigned len = i->getVL();

  for (unsigned n=0; n < len; n++) {
    sse_mpsadbw(&result.ymm128(n), &op1.ymm128(n), &op2.ymm128(n), control & 0x7);
    control >>= 3;
  }

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), result, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBLENDW_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());

  unsigned len = i->getVL();
  Bit8u mask = i->Ib();

  for (unsigned n=0; n < len; n++)
    sse_pblendw(&op1.ymm128(n), &op2.ymm128(n), mask);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op1, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTB_VdqWb(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  BxPackedYmmRegister op;

  simd_pbroadcastb(op.ymm_ubyteptr(), BX_READ_XMM_REG_LO_BYTE(i->src()), len*16);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTW_VdqWw(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  BxPackedYmmRegister op;

  simd_pbroadcastw(op.ymm_u16ptr(), BX_READ_XMM_REG_LO_WORD(i->src()), len*8);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTD_VdqWd(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  BxPackedYmmRegister op;

  simd_pbroadcastd(op.ymm_u32ptr(), BX_READ_XMM_REG_LO_DWORD(i->src()), len*4);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPBROADCASTQ_VdqWq(bxInstruction_c *i)
{
  unsigned len = i->getVL();
  BxPackedYmmRegister op;

  simd_pbroadcastq(op.ymm_u64ptr(), BX_READ_XMM_REG_LO_QWORD(i->src()), len*2);

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op, len);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBW256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  for (int n=0; n<16; n++)
    result.ymm16u(n) = (Bit8s) op.xmmsbyte(n);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBD256_VdqWqR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.ymm32u(0) = (Bit8s) MMXSB0(op);
  result.ymm32u(1) = (Bit8s) MMXSB1(op);
  result.ymm32u(2) = (Bit8s) MMXSB2(op);
  result.ymm32u(3) = (Bit8s) MMXSB3(op);
  result.ymm32u(4) = (Bit8s) MMXSB4(op);
  result.ymm32u(5) = (Bit8s) MMXSB5(op);
  result.ymm32u(6) = (Bit8s) MMXSB6(op);
  result.ymm32u(7) = (Bit8s) MMXSB7(op);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXBQ256_VdqWdR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.ymm64u(0) = (Bit8s) (val32 & 0xFF);
  result.ymm64u(1) = (Bit8s) ((val32 >> 8) & 0xFF);
  result.ymm64u(2) = (Bit8s) ((val32 >> 16) & 0xFF);
  result.ymm64u(3) = (Bit8s) (val32 >> 24);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWD256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  result.ymm32u(0) = op.xmm16s(0);
  result.ymm32u(1) = op.xmm16s(1);
  result.ymm32u(2) = op.xmm16s(2);
  result.ymm32u(3) = op.xmm16s(3);
  result.ymm32u(4) = op.xmm16s(4);
  result.ymm32u(5) = op.xmm16s(5);
  result.ymm32u(6) = op.xmm16s(6);
  result.ymm32u(7) = op.xmm16s(7);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXWQ256_VdqWqR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.ymm64u(0) = MMXSW0(op);
  result.ymm64u(1) = MMXSW1(op);
  result.ymm64u(2) = MMXSW2(op);
  result.ymm64u(3) = MMXSW3(op);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVSXDQ256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  result.ymm64u(0) = op.xmm32s(0);
  result.ymm64u(1) = op.xmm32s(1);
  result.ymm64u(2) = op.xmm32s(2);
  result.ymm64u(3) = op.xmm32s(3);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBW256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  for (int n=0; n<16; n++)
    result.ymm16u(n) = op.xmmubyte(n);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBD256_VdqWqR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.ymm32u(0) = MMXUB0(op);
  result.ymm32u(1) = MMXUB1(op);
  result.ymm32u(2) = MMXUB2(op);
  result.ymm32u(3) = MMXUB3(op);
  result.ymm32u(4) = MMXUB4(op);
  result.ymm32u(5) = MMXUB5(op);
  result.ymm32u(6) = MMXUB6(op);
  result.ymm32u(7) = MMXUB7(op);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXBQ256_VdqWdR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.ymm64u(0) = (Bit8u) (val32 & 0xFF);
  result.ymm64u(1) = (Bit8u) ((val32 >> 8) & 0xFF);
  result.ymm64u(2) = (Bit8u) ((val32 >> 16) & 0xFF);
  result.ymm64u(3) = (Bit8u) (val32 >> 24);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWD256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  result.ymm32u(0) = op.xmm16u(0);
  result.ymm32u(1) = op.xmm16u(1);
  result.ymm32u(2) = op.xmm16u(2);
  result.ymm32u(3) = op.xmm16u(3);
  result.ymm32u(4) = op.xmm16u(4);
  result.ymm32u(5) = op.xmm16u(5);
  result.ymm32u(6) = op.xmm16u(6);
  result.ymm32u(7) = op.xmm16u(7);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXWQ256_VdqWqR(bxInstruction_c *i)
{
  BxPackedYmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.ymm64u(0) = MMXUW0(op);
  result.ymm64u(1) = MMXUW1(op);
  result.ymm64u(2) = MMXUW2(op);
  result.ymm64u(3) = MMXUW3(op);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPMOVZXDQ256_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  BxPackedYmmRegister result;

  result.ymm64u(0) = op.xmm32u(0);
  result.ymm64u(1) = op.xmm32u(1);
  result.ymm64u(2) = op.xmm32u(2);
  result.ymm64u(3) = op.xmm32u(3);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPALIGNR_VdqHdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1()), op2 = BX_READ_YMM_REG(i->src2());
  unsigned len = i->getVL();

  for (unsigned n=0; n<len; n++)
    sse_palignr(&op2.ymm128(n), &op1.ymm128(n), i->Ib());

  BX_WRITE_YMM_REGZ_VLEN(i->dst(), op2, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMD_VdqHdqWdqR(bxInstruction_c *i)
{
  BxPackedYmmRegister op1 = BX_READ_YMM_REG(i->src1());
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src2()), result;

  result.ymm32u(0) = op2.ymm32u(op1.ymm32u(0) & 0x7);
  result.ymm32u(1) = op2.ymm32u(op1.ymm32u(1) & 0x7);
  result.ymm32u(2) = op2.ymm32u(op1.ymm32u(2) & 0x7);
  result.ymm32u(3) = op2.ymm32u(op1.ymm32u(3) & 0x7);
  result.ymm32u(4) = op2.ymm32u(op1.ymm32u(4) & 0x7);
  result.ymm32u(5) = op2.ymm32u(op1.ymm32u(5) & 0x7);
  result.ymm32u(6) = op2.ymm32u(op1.ymm32u(6) & 0x7);
  result.ymm32u(7) = op2.ymm32u(op1.ymm32u(7) & 0x7);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VPERMQ_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedYmmRegister op2 = BX_READ_YMM_REG(i->src()), result;
  Bit8u control = i->Ib();

  result.ymm64u(0) = op2.ymm64u((control)      & 0x3);
  result.ymm64u(1) = op2.ymm64u((control >> 2) & 0x3);
  result.ymm64u(2) = op2.ymm64u((control >> 4) & 0x3);
  result.ymm64u(3) = op2.ymm64u((control >> 6) & 0x3);

  BX_WRITE_YMM_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

#endif
