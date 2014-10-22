/////////////////////////////////////////////////6////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003-2014 Stanislav Shwartsman
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

SSE_2OP(PHADDW_VdqWdqR, xmm_phaddw)
SSE_2OP(PHADDSW_VdqWdqR, xmm_phaddsw)
SSE_2OP(PHADDD_VdqWdqR, xmm_phaddd)
SSE_2OP(PHSUBW_VdqWdqR, xmm_phsubw)
SSE_2OP(PHSUBSW_VdqWdqR, xmm_phsubsw)
SSE_2OP(PHSUBD_VdqWdqR, xmm_phsubd)

SSE_2OP(PSIGNB_VdqWdqR, xmm_psignb)
SSE_2OP(PSIGNW_VdqWdqR, xmm_psignw)
SSE_2OP(PSIGND_VdqWdqR, xmm_psignd)

SSE_2OP(PCMPEQQ_VdqWdqR, xmm_pcmpeqq)
SSE_2OP(PCMPGTQ_VdqWdqR, xmm_pcmpgtq)

SSE_2OP(PMINSB_VdqWdqR, xmm_pminsb)
SSE_2OP(PMINSD_VdqWdqR, xmm_pminsd)
SSE_2OP(PMINUW_VdqWdqR, xmm_pminuw)
SSE_2OP(PMINUD_VdqWdqR, xmm_pminud)
SSE_2OP(PMAXSB_VdqWdqR, xmm_pmaxsb)
SSE_2OP(PMAXSD_VdqWdqR, xmm_pmaxsd)
SSE_2OP(PMAXUW_VdqWdqR, xmm_pmaxuw)
SSE_2OP(PMAXUD_VdqWdqR, xmm_pmaxud)

SSE_2OP(PACKUSDW_VdqWdqR, xmm_packusdw)

SSE_2OP(PMULLD_VdqWdqR, xmm_pmulld)
SSE_2OP(PMULDQ_VdqWdqR, xmm_pmuldq)
SSE_2OP(PMULHRSW_VdqWdqR, xmm_pmulhrsw)

SSE_2OP(PMADDUBSW_VdqWdqR, xmm_pmaddubsw)

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

SSE_2OP_CPU_LEVEL6(PMINUB_VdqWdqR, xmm_pminub)
SSE_2OP_CPU_LEVEL6(PMINSW_VdqWdqR, xmm_pminsw)
SSE_2OP_CPU_LEVEL6(PMAXUB_VdqWdqR, xmm_pmaxub)
SSE_2OP_CPU_LEVEL6(PMAXSW_VdqWdqR, xmm_pmaxsw)

SSE_2OP_CPU_LEVEL6(PAVGB_VdqWdqR, xmm_pavgb)
SSE_2OP_CPU_LEVEL6(PAVGW_VdqWdqR, xmm_pavgw)

SSE_2OP_CPU_LEVEL6(PCMPEQB_VdqWdqR, xmm_pcmpeqb)
SSE_2OP_CPU_LEVEL6(PCMPEQW_VdqWdqR, xmm_pcmpeqw)
SSE_2OP_CPU_LEVEL6(PCMPEQD_VdqWdqR, xmm_pcmpeqd)
SSE_2OP_CPU_LEVEL6(PCMPGTB_VdqWdqR, xmm_pcmpgtb)
SSE_2OP_CPU_LEVEL6(PCMPGTW_VdqWdqR, xmm_pcmpgtw)
SSE_2OP_CPU_LEVEL6(PCMPGTD_VdqWdqR, xmm_pcmpgtd)

SSE_2OP_CPU_LEVEL6(ANDPS_VpsWpsR, xmm_andps)
SSE_2OP_CPU_LEVEL6(ANDNPS_VpsWpsR, xmm_andnps)
SSE_2OP_CPU_LEVEL6(ORPS_VpsWpsR, xmm_orps)
SSE_2OP_CPU_LEVEL6(XORPS_VpsWpsR, xmm_xorps)

SSE_2OP_CPU_LEVEL6(PSUBB_VdqWdqR, xmm_psubb)
SSE_2OP_CPU_LEVEL6(PSUBW_VdqWdqR, xmm_psubw)
SSE_2OP_CPU_LEVEL6(PSUBD_VdqWdqR, xmm_psubd)
SSE_2OP_CPU_LEVEL6(PSUBQ_VdqWdqR, xmm_psubq)
SSE_2OP_CPU_LEVEL6(PADDB_VdqWdqR, xmm_paddb)
SSE_2OP_CPU_LEVEL6(PADDW_VdqWdqR, xmm_paddw)
SSE_2OP_CPU_LEVEL6(PADDD_VdqWdqR, xmm_paddd)
SSE_2OP_CPU_LEVEL6(PADDQ_VdqWdqR, xmm_paddq)

SSE_2OP_CPU_LEVEL6(PSUBSB_VdqWdqR, xmm_psubsb)
SSE_2OP_CPU_LEVEL6(PSUBUSB_VdqWdqR, xmm_psubusb)
SSE_2OP_CPU_LEVEL6(PSUBSW_VdqWdqR, xmm_psubsw)
SSE_2OP_CPU_LEVEL6(PSUBUSW_VdqWdqR, xmm_psubusw)
SSE_2OP_CPU_LEVEL6(PADDSB_VdqWdqR, xmm_paddsb)
SSE_2OP_CPU_LEVEL6(PADDUSB_VdqWdqR, xmm_paddusb)
SSE_2OP_CPU_LEVEL6(PADDSW_VdqWdqR, xmm_paddsw)
SSE_2OP_CPU_LEVEL6(PADDUSW_VdqWdqR, xmm_paddusw)

SSE_2OP_CPU_LEVEL6(PACKUSWB_VdqWdqR, xmm_packuswb)
SSE_2OP_CPU_LEVEL6(PACKSSWB_VdqWdqR, xmm_packsswb)
SSE_2OP_CPU_LEVEL6(PACKSSDW_VdqWdqR, xmm_packssdw)

SSE_2OP_CPU_LEVEL6(UNPCKLPS_VpsWpsR, xmm_unpcklps)
SSE_2OP_CPU_LEVEL6(UNPCKHPS_VpsWpsR, xmm_unpckhps)
SSE_2OP_CPU_LEVEL6(PUNPCKLQDQ_VdqWdqR, xmm_unpcklpd)
SSE_2OP_CPU_LEVEL6(PUNPCKHQDQ_VdqWdqR, xmm_unpckhpd)

SSE_2OP_CPU_LEVEL6(PUNPCKLBW_VdqWdqR, xmm_punpcklbw)
SSE_2OP_CPU_LEVEL6(PUNPCKLWD_VdqWdqR, xmm_punpcklwd)
SSE_2OP_CPU_LEVEL6(PUNPCKHBW_VdqWdqR, xmm_punpckhbw)
SSE_2OP_CPU_LEVEL6(PUNPCKHWD_VdqWdqR, xmm_punpckhwd)

SSE_2OP_CPU_LEVEL6(PMULLW_VdqWdqR, xmm_pmullw)
SSE_2OP_CPU_LEVEL6(PMULHW_VdqWdqR, xmm_pmulhw)
SSE_2OP_CPU_LEVEL6(PMULHUW_VdqWdqR, xmm_pmulhuw)
SSE_2OP_CPU_LEVEL6(PMULUDQ_VdqWdqR, xmm_pmuludq)
SSE_2OP_CPU_LEVEL6(PMADDWD_VdqWdqR, xmm_pmaddwd)

SSE_2OP_CPU_LEVEL6(PSADBW_VdqWdqR, xmm_psadbw)

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

SSE_1OP(PABSB_VdqWdqR, xmm_pabsb)
SSE_1OP(PABSW_VdqWdqR, xmm_pabsw)
SSE_1OP(PABSD_VdqWdqR, xmm_pabsd)

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFB_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  xmm_pshufb(&result, &op1, &op2);

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDVB_VdqWdqR(bxInstruction_c *i)
{
  xmm_pblendvb(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPS_VpsWpsR(bxInstruction_c *i)
{
  xmm_blendvps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDVPD_VpdWpdR(bxInstruction_c *i)
{
  xmm_blendvpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), &BX_XMM_REG(0));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PTEST_VdqWdqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  clearEFlagsOSZAPC();

  if ((op2.xmm64u(0) &  op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) &  op1.xmm64u(1)) == 0) assert_ZF();

  if ((op2.xmm64u(0) & ~op1.xmm64u(0)) == 0 &&
      (op2.xmm64u(1) & ~op1.xmm64u(1)) == 0) assert_CF();

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
  xmm_blendps(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BLENDPD_VpdWpdIbR(bxInstruction_c *i)
{
  xmm_blendpd(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PBLENDW_VdqWdqIbR(bxInstruction_c *i)
{
  xmm_pblendw(&BX_XMM_REG(i->dst()), &BX_XMM_REG(i->src()), i->Ib());

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

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRQ_EqVdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  Bit64u result = op.xmm64u(i->Ib() & 1);
  BX_WRITE_64BIT_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRD_EdVdqIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

#if BX_SUPPORT_X86_64
  if (i->os64L())  /* 64 bit operand size mode */
  {
     Bit64u result = op.xmm64u(i->Ib() & 1);
     write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), result);
  }
  else
#endif
{
     Bit32u result = op.xmm32u(i->Ib() & 3);
     write_virtual_dword(i->seg(), eaddr, result);
  }

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PEXTRQ_EqVdqIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());
  Bit64u result = op.xmm64u(i->Ib() & 1);
  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), result);

  BX_NEXT_INSTR(i);
}
#endif

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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqHdqEbIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  op1.xmmubyte(i->Ib() & 0xF) = BX_READ_8BIT_REGL(i->src2()); // won't allow reading of AH/CH/BH/DH

  BX_WRITE_XMM_REGZ(i->dst(), op1, i->getVL());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PINSRB_VdqHdqEbIbM(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->src1());

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1.xmmubyte(i->Ib() & 0xF) = read_virtual_byte(i->seg(), eaddr);

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
    Bit64u op2 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
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

  xmm_mpsadbw(&result, &op1, &op2, i->Ib() & 0x7);

  BX_WRITE_XMM_REG(i->dst(), result);

  BX_NEXT_INSTR(i);
}

#endif // BX_CPU_LEVEL >= 6

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFD_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_shufps(&result, &op, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFHW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_pshufhw(&result, &op, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PSHUFLW_VdqWdqIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src()), result;

  xmm_pshuflw(&result, &op, i->Ib());

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

  xmm_shufps(&result, &op1, &op2, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), result);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SHUFPD_VpdWpdIbR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst());
  BxPackedXmmRegister op2 = BX_READ_XMM_REG(i->src()), result;

  xmm_shufpd(&result, &op1, &op2, i->Ib());

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

SSE_PSHIFT_CPU_LEVEL6(PSRLW_VdqWdqR, xmm_psrlw);
SSE_PSHIFT_CPU_LEVEL6(PSRLD_VdqWdqR, xmm_psrld);
SSE_PSHIFT_CPU_LEVEL6(PSRLQ_VdqWdqR, xmm_psrlq);
SSE_PSHIFT_CPU_LEVEL6(PSRAW_VdqWdqR, xmm_psraw);
SSE_PSHIFT_CPU_LEVEL6(PSRAD_VdqWdqR, xmm_psrad);
SSE_PSHIFT_CPU_LEVEL6(PSLLW_VdqWdqR, xmm_psllw);
SSE_PSHIFT_CPU_LEVEL6(PSLLD_VdqWdqR, xmm_pslld);
SSE_PSHIFT_CPU_LEVEL6(PSLLQ_VdqWdqR, xmm_psllq);

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

SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLW_UdqIb, xmm_psrlw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLD_UdqIb, xmm_psrld);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLQ_UdqIb, xmm_psrlq);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRAW_UdqIb, xmm_psraw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSRAD_UdqIb, xmm_psrad);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLW_UdqIb, xmm_psllw);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLD_UdqIb, xmm_pslld);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLQ_UdqIb, xmm_psllq);

SSE_PSHIFT_IMM_CPU_LEVEL6(PSRLDQ_UdqIb, xmm_psrldq);
SSE_PSHIFT_IMM_CPU_LEVEL6(PSLLDQ_UdqIb, xmm_pslldq);

/* ************************ */
/* SSE4A (AMD) INSTRUCTIONS */
/* ************************ */

#if BX_CPU_LEVEL >= 6
BX_CPP_INLINE Bit64u xmm_extrq(Bit64u src, unsigned shift, unsigned len)
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

BX_CPP_INLINE Bit64u xmm_insertq(Bit64u dest, Bit64u src, unsigned shift, unsigned len)
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
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), i->Ib2(), i->Ib()));
#endif
  
  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::EXTRQ_VdqUq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u ctrl = BX_READ_XMM_REG_LO_WORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_extrq(BX_READ_XMM_REG_LO_QWORD(i->dst()), ctrl >> 8, ctrl));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUqIbIb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst()), src = BX_READ_XMM_REG_LO_QWORD(i->src());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_insertq(dst, src, i->Ib2(), i->Ib()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INSERTQ_VdqUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister src = BX_READ_XMM_REG(i->src());

  Bit64u dst = BX_READ_XMM_REG_LO_QWORD(i->dst());

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), xmm_insertq(dst, src.xmm64u(0), src.xmmubyte(9), src.xmmubyte(8)));
#endif

  BX_NEXT_INSTR(i);
}
