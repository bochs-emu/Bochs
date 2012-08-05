/////////////////////////////////////////////////6////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2012 Stanislav Shwartsman
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

/* ********************************************** */
/* SSE Integer Operations (128bit MMX extensions) */
/* ********************************************** */

#if BX_CPU_LEVEL >= 6

#include "simd_int.h"
#include "simd_compare.h"

#define SSE_2OP(HANDLER, func)                                                             \
  /* SSE instruction with two src operands */                                              \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());  \
    (func)(&op1, &op2);                                                                    \
    BX_WRITE_XMM_REG(i->dst(), op1);                                                       \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

SSE_2OP(PHADDW_VdqWdqR, sse_phaddw)
SSE_2OP(PHADDSW_VdqWdqR, sse_phaddsw)
SSE_2OP(PHADDD_VdqWdqR, sse_phaddd)
SSE_2OP(PHSUBW_VdqWdqR, sse_phsubw)
SSE_2OP(PHSUBSW_VdqWdqR, sse_phsubsw)
SSE_2OP(PHSUBD_VdqWdqR, sse_phsubd)

SSE_2OP(PSIGNB_VdqWdqR, sse_psignb)
SSE_2OP(PSIGNW_VdqWdqR, sse_psignw)
SSE_2OP(PSIGND_VdqWdqR, sse_psignd)

SSE_2OP(PCMPEQQ_VdqWdqR, sse_pcmpeqq)
SSE_2OP(PCMPGTQ_VdqWdqR, sse_pcmpgtq)

SSE_2OP(PMINSB_VdqWdqR, sse_pminsb)
SSE_2OP(PMINSD_VdqWdqR, sse_pminsd)
SSE_2OP(PMINUW_VdqWdqR, sse_pminuw)
SSE_2OP(PMINUD_VdqWdqR, sse_pminud)
SSE_2OP(PMAXSB_VdqWdqR, sse_pmaxsb)
SSE_2OP(PMAXSD_VdqWdqR, sse_pmaxsd)
SSE_2OP(PMAXUW_VdqWdqR, sse_pmaxuw)
SSE_2OP(PMAXUD_VdqWdqR, sse_pmaxud)

SSE_2OP(PACKUSDW_VdqWdqR, sse_packusdw)

SSE_2OP(PMULLD_VdqWdqR, sse_pmulld)
SSE_2OP(PMULDQ_VdqWdqR, sse_pmuldq)
SSE_2OP(PMULHRSW_VdqWdqR, sse_pmulhrsw)

SSE_2OP(PMADDUBSW_VdqWdqR, sse_pmaddubsw)

#endif // BX_CPU_LEVEL >= 6

#if BX_CPU_LEVEL >= 6

#define SSE_2OP_CPU_LEVEL6(HANDLER, func) \
  SSE_2OP(HANDLER, func)

#else

#define SSE_2OP_CPU_LEVEL6(HANDLER, func)                                                  \
  /* SSE instruction with two src operands */                                              \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BX_NEXT_INSTR(i);                                                                      \
  }

#endif

SSE_2OP_CPU_LEVEL6(PMINUB_VdqWdqR, sse_pminub)
SSE_2OP_CPU_LEVEL6(PMINSW_VdqWdqR, sse_pminsw)
SSE_2OP_CPU_LEVEL6(PMAXUB_VdqWdqR, sse_pmaxub)
SSE_2OP_CPU_LEVEL6(PMAXSW_VdqWdqR, sse_pmaxsw)

SSE_2OP_CPU_LEVEL6(PAVGB_VdqWdqR, sse_pavgb)
SSE_2OP_CPU_LEVEL6(PAVGW_VdqWdqR, sse_pavgw)

SSE_2OP_CPU_LEVEL6(PCMPEQB_VdqWdqR, sse_pcmpeqb)
SSE_2OP_CPU_LEVEL6(PCMPEQW_VdqWdqR, sse_pcmpeqw)
SSE_2OP_CPU_LEVEL6(PCMPEQD_VdqWdqR, sse_pcmpeqd)
SSE_2OP_CPU_LEVEL6(PCMPGTB_VdqWdqR, sse_pcmpgtb)
SSE_2OP_CPU_LEVEL6(PCMPGTW_VdqWdqR, sse_pcmpgtw)
SSE_2OP_CPU_LEVEL6(PCMPGTD_VdqWdqR, sse_pcmpgtd)

SSE_2OP_CPU_LEVEL6(ANDPS_VpsWpsR, sse_andps)
SSE_2OP_CPU_LEVEL6(ANDNPS_VpsWpsR, sse_andnps)
SSE_2OP_CPU_LEVEL6(ORPS_VpsWpsR, sse_orps)
SSE_2OP_CPU_LEVEL6(XORPS_VpsWpsR, sse_xorps)

SSE_2OP_CPU_LEVEL6(PSUBB_VdqWdqR, sse_psubb)
SSE_2OP_CPU_LEVEL6(PSUBW_VdqWdqR, sse_psubw)
SSE_2OP_CPU_LEVEL6(PSUBD_VdqWdqR, sse_psubd)
SSE_2OP_CPU_LEVEL6(PSUBQ_VdqWdqR, sse_psubq)
SSE_2OP_CPU_LEVEL6(PADDB_VdqWdqR, sse_paddb)
SSE_2OP_CPU_LEVEL6(PADDW_VdqWdqR, sse_paddw)
SSE_2OP_CPU_LEVEL6(PADDD_VdqWdqR, sse_paddd)
SSE_2OP_CPU_LEVEL6(PADDQ_VdqWdqR, sse_paddq)

SSE_2OP_CPU_LEVEL6(PSUBSB_VdqWdqR, sse_psubsb)
SSE_2OP_CPU_LEVEL6(PSUBUSB_VdqWdqR, sse_psubusb)
SSE_2OP_CPU_LEVEL6(PSUBSW_VdqWdqR, sse_psubsw)
SSE_2OP_CPU_LEVEL6(PSUBUSW_VdqWdqR, sse_psubusw)
SSE_2OP_CPU_LEVEL6(PADDSB_VdqWdqR, sse_paddsb)
SSE_2OP_CPU_LEVEL6(PADDUSB_VdqWdqR, sse_paddusb)
SSE_2OP_CPU_LEVEL6(PADDSW_VdqWdqR, sse_paddsw)
SSE_2OP_CPU_LEVEL6(PADDUSW_VdqWdqR, sse_paddusw)

SSE_2OP_CPU_LEVEL6(PACKUSWB_VdqWdqR, sse_packuswb)
SSE_2OP_CPU_LEVEL6(PACKSSWB_VdqWdqR, sse_packsswb)
SSE_2OP_CPU_LEVEL6(PACKSSDW_VdqWdqR, sse_packssdw)

SSE_2OP_CPU_LEVEL6(UNPCKLPS_VpsWpsR, sse_unpcklps)
SSE_2OP_CPU_LEVEL6(UNPCKHPS_VpsWpsR, sse_unpckhps)
SSE_2OP_CPU_LEVEL6(PUNPCKLQDQ_VdqWdqR, sse_unpcklpd)
SSE_2OP_CPU_LEVEL6(PUNPCKHQDQ_VdqWdqR, sse_unpckhpd)

SSE_2OP_CPU_LEVEL6(PUNPCKLBW_VdqWdqR, sse_punpcklbw)
SSE_2OP_CPU_LEVEL6(PUNPCKLWD_VdqWdqR, sse_punpcklwd)
SSE_2OP_CPU_LEVEL6(PUNPCKHBW_VdqWdqR, sse_punpckhbw)
SSE_2OP_CPU_LEVEL6(PUNPCKHWD_VdqWdqR, sse_punpckhwd)

SSE_2OP_CPU_LEVEL6(PMULLW_VdqWdqR, sse_pmullw)
SSE_2OP_CPU_LEVEL6(PMULHW_VdqWdqR, sse_pmulhw)
SSE_2OP_CPU_LEVEL6(PMULHUW_VdqWdqR, sse_pmulhuw)
SSE_2OP_CPU_LEVEL6(PMULUDQ_VdqWdqR, sse_pmuludq)
SSE_2OP_CPU_LEVEL6(PMADDWD_VdqWdqR, sse_pmaddwd)

SSE_2OP_CPU_LEVEL6(PSADBW_VdqWdqR, sse_psadbw)

#if BX_CPU_LEVEL >= 6

#define SSE_1OP(HANDLER, func)                                                             \
  /* SSE instruction with single src operand */                                            \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());                                    \
    (func)(&op);                                                                           \
    BX_WRITE_XMM_REG(i->dst(), op);                                                        \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

SSE_1OP(PABSB_VdqWdqR, sse_pabsb)
SSE_1OP(PABSW_VdqWdqR, sse_pabsw)
SSE_1OP(PABSD_VdqWdqR, sse_pabsd)

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFB_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  sse_pshufb(&result, &op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDVB_VdqWdqR(bxInstruction_c *i)
{
  sse_pblendvb(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPS_VpsWpsR(bxInstruction_c *i)
{
  sse_blendvps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPD_VpdWpdR(bxInstruction_c *i)
{
  sse_blendvpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PTEST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());
  unsigned result = 0;

  if ((op2.xmm64u(0) &  op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) &  op1.xmm64u(1)) == 0) result |= EFlagsZFMask;

  if ((op2.xmm64u(0) & ~op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) & ~op1.xmm64u(1)) == 0) result |= EFlagsCFMask;

  setEFlagsOSZAPC(result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PHMINPOSUW_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  unsigned min = 0;

  for (unsigned j=1; j < 8; j++) {
     if (op.xmm16u(j) < op.xmm16u(min)) min = j;
  }

  op.xmm16u(0) = op.xmm16u(min);
  op.xmm16u(1) = min;
  op.xmm32u(1) = 0;
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPS_VpsWpsIbR(bxInstruction_c *i)
{
  sse_blendps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPD_VpdWpdIbR(bxInstruction_c *i)
{
  sse_blendpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDW_VdqWdqIbR(bxInstruction_c *i)
{
  sse_pblendw(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_EbdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRB_EbdVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u result = op.xmmubyte(i->Ib() & 0xF);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_byte(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_EwdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit16u result = op.xmm16u(i->Ib() & 7);
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_EwdVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit16u result = op.xmm16u(i->Ib() & 7);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_word(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
     Bit64u result = op.xmm64u(i->Ib() & 1);
     BX_WRITE_64BIT_REG(i->dst(), result);
  }
  else
#endif
{
     Bit32u result = op.xmm32u(i->Ib() & 3);
     BX_WRITE_32BIT_REGZ(i->dst(), result);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
     Bit64u result = op.xmm64u(i->Ib() & 1);
     write_virtual_qword_64(i->seg(), eaddr, result);
  }
  else
#endif
{
     Bit32u result = op.xmm32u(i->Ib() & 3);
     write_virtual_dword(i->seg(), eaddr, result);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRACTPS_EdVpsIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit32u result = op.xmm32u(i->Ib() & 3);
  BX_WRITE_32BIT_REGZ(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRACTPS_EdVpsIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit32u result = op.xmm32u(i->Ib() & 3);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_dword(i->seg(), eaddr, result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqHdqEbIb(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  Bit8u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = (Bit8u) BX_READ_16BIT_REG(i->src2()); // won't allow reading of AH/CH/BH/DH
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    op2 = read_virtual_byte(i->seg(), eaddr);
  }

  op1.xmmubyte(i->Ib() & 0xF) = op2;

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTPS_VpsHpsWssIb(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  Bit8u control = i->Ib();
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    BxPackedXmmRegister temp = BX_READ_XMM_REG(i->src2());
    op2 = temp.xmm32u((control >> 6) & 3);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    op2 = read_virtual_dword(i->seg(), eaddr);
  }

  op1.xmm32u((control >> 4) & 3) = op2;

  if (control & 1) op1.xmm32u(0) = 0;
  if (control & 2) op1.xmm32u(1) = 0;
  if (control & 4) op1.xmm32u(2) = 0;
  if (control & 8) op1.xmm32u(3) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqHdqEdIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

#if BX_SUPPORT_X86_64
  if (i->os64L()) { /* 64 bit operand size mode */
    op1.xmm64u(i->Ib() & 1) = BX_READ_64BIT_REG(i->src2());
  }
  else
#endif
  {
    op1.xmm32u(i->Ib() & 3) = BX_READ_32BIT_REG(i->src2());
  }

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRD_VdqHdqEdIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L()) {  /* 64 bit operand size mode */
    Bit64u op2 = read_virtual_qword_64(i->seg(), eaddr);
    op1.xmm64u(i->Ib() & 1) = op2;
  }
  else
#endif
  {
    Bit32u op2 = read_virtual_dword(i->seg(), eaddr);
    op1.xmm32u(i->Ib() & 3) = op2;
  }

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MPSADBW_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  sse_mpsadbw(&result, &op1, &op2, i->Ib() & 0x7);

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

#endif // BX_CPU_LEVEL >= 6

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFD_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  sse_shufps(&result, &op, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  sse_pshufhw(&result, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  sse_pshuflw(&result, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRW_VdqHdqEwIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());
  Bit8u count = i->Ib() & 0x7;

  op1.xmm16u(count) = BX_READ_16BIT_REG(i->src2());

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRW_GdUdqIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit8u count = i->Ib() & 0x7;
  Bit32u result = (Bit32u) op.xmm16u(count);
  BX_WRITE_32BIT_REGZ(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPS_VpsWpsIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  sse_shufps(&result, &op1, &op2, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  sse_shufpd(&result, &op1, &op2, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6

#define SSE_PSHIFT_CPU_LEVEL6(HANDLER, func)                                               \
  /* SSE packed shift instruction */                                                       \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    BxPackedXmmRegister op = BX_READ_XMM_REG(i->dst());                                    \
                                                                                           \
    (func)(&op, BX_READ_XMM_REG_LO_QWORD(i->src()));                                       \
                                                                                           \
    BX_WRITE_XMM_REG(i->dst(), op);                                                        \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

#else

#define SSE_PSHIFT_CPU_LEVEL6(HANDLER, func)                                               \
  /* SSE instruction with two src operands */                                              \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BX_NEXT_INSTR(i);                                                                      \
  }

#endif

SSE_PSHIFT_CPU_LEVEL6(PSRLW_VdqWdqR, sse_psrlw);
SSE_PSHIFT_CPU_LEVEL6(PSRLD_VdqWdqR, sse_psrld);
SSE_PSHIFT_CPU_LEVEL6(PSRLQ_VdqWdqR, sse_psrlq);
SSE_PSHIFT_CPU_LEVEL6(PSRAW_VdqWdqR, sse_psraw);
SSE_PSHIFT_CPU_LEVEL6(PSRAD_VdqWdqR, sse_psrad);
SSE_PSHIFT_CPU_LEVEL6(PSLLW_VdqWdqR, sse_psllw);
SSE_PSHIFT_CPU_LEVEL6(PSLLD_VdqWdqR, sse_pslld);
SSE_PSHIFT_CPU_LEVEL6(PSLLQ_VdqWdqR, sse_psllq);

#if BX_CPU_LEVEL >= 6

#define SSE_PSHIFT_IMM_CPU_LEVEL6(HANDLER, func) \
  /* SSE packed shift with imm8 instruction */                                             \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)              \
  {                                                                                        \
    (func)(&BX_XMM_REG(i->dst()), i->Ib());                                                \
                                                                                           \
    BX_NEXT_INSTR(i);                                                                      \
  }

#else

#define SSE_PSHIFT_IMM_CPU_LEVEL6(HANDLER, func)                                           \
  /* SSE packed shift with imm8 instruction */                                             \
  BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C :: HANDLER (bxInstruction_c *i)             \
  {                                                                                        \
    BX_NEXT_INSTR(i);                                                                      \
  }

#endif

SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLW_UdqIb, sse_psrlw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLD_UdqIb, sse_psrld);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLQ_UdqIb, sse_psrlq);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRAW_UdqIb, sse_psraw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRAD_UdqIb, sse_psrad);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLW_UdqIb, sse_psllw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLD_UdqIb, sse_pslld);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLQ_UdqIb, sse_psllq);

SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLDQ_UdqIb, sse_psrldq);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLDQ_UdqIb, sse_pslldq);

/* ************************ */
/* SSE4A (AMD) INSTRUCTIONS */
/* ************************ */

#if BX_CPU_LEVEL >= 6
BX_CPP_INLINE Bit64u sse_extrq(Bit64u src, unsigned shift, unsigned len)
{
  len   &= 0x3f;
  shift &= 0x3f;

  src >>= shift;
  if (len) {
    Bit64u mask = (BX_CONST64(1) << len) - 1;
    return src & mask;
  }

  return src;
}

BX_CPP_INLINE Bit64u sse_insertq(Bit64u dest, Bit64u src, unsigned shift, unsigned len)
{
  Bit64u mask;

  len   &= 0x3f;
  shift &= 0x3f;

  if (len == 0) {
    mask = BX_CONST64(0xffffffffffffffff);
  } else {
    mask = (BX_CONST64(1) << len) - 1;
  }

  return (dest & ~(mask << shift)) | ((src & mask) << shift);
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRQ_UdqIbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), sse_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), i->Ib2(), i->Ib()));
#endif
  
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRQ_VdqUq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u ctrl = BX_READ_XMM_REG_LO_WORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), sse_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), ctrl >> 8, ctrl));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUqIbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst()), src = BX_READ_XMM_REG_LO_QWORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), sse_insertq(dst, src, i->Ib2(), i->Ib()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister src = BX_READ_XMM_REG(i->src());

  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), sse_insertq(dst, src.xmm64u(0), src.xmmubyte(9), src.xmmubyte(8)));
#endif

  BX_NEXT_INSTR(i);
}
