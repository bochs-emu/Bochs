/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2002 Stanislav Shwartsman
//          Written by Stanislav Shwartsman <gate@fidonet.org.il>
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//



#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

void BX_CPU_C::MOVUPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVUPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVUPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPD_WpdVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVUPD_WpdVpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVUPD_WpdVpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_WsdVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSD_WsdVsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVSD_WsdVsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPD_VsdMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPD_VsdMq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVLPD_VsdMq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPD_MqVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPD_MqVsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVLPD_MqVsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKLPD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UNPCKLPD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKHPD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UNPCKHPD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPD_VpdMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPD_VpdMq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVHPD_VpdMq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPD_MqVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPD_MqVpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVHPD_MqVpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVAPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVAPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPD_WpdVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVAPD_WpdVpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVAPD_WpdVpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PD_VpdQd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPI2PD_VpdQd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPI2PD_VpdQd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSI2SD_VsdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSI2SD_VsdEd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSI2SD_VsdEd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTPD_MdqVpd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVNTPD_MdqVpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVNTPD_MdqVpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPD2PI_PqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPD2PI_PqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTSD2SI_GdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTSD2SI_GdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2PI_PqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2PI_PqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSD2SI_GdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSD2SI_GdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISD_VsdWsd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UCOMISD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UCOMISD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISD_VpdWpd(bxInstruction_c *i)   
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("COMISD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("COMISD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPD_EdVRpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVMSKPD_EdVRpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVMSKPD_EdVRpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SQRTPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SQRTSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ANDPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ANDPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDNPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ANDNPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ANDNPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ORPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ORPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ORPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XORPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("XORPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("XORPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ADDPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ADDSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MULPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MULSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PD_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2PD_VpsWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPS2PD_VpsWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PS_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2PS_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2PS_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SS_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSD2SS_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSD2SS_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SD_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSS2SD_VssWss: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSS2SD_VssWss: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PS_VpsWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTDQ2PS_VpsWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTDQ2PS_VpsWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2DQ_VdqWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPS2DQ_VdqWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPS2DQ_VdqWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPS2DQ_VdqWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SUBPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SUBSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MINPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MINSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("DIVPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("DIVSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MAXPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MAXSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKLBW_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKLBW_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKLWD_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKLWD_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKLDQ_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKLDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 63 */
void BX_CPU_C::PACKSSWB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmmsbyte(0)  = SaturateWordSToByteS(op1.xmm16s(0));
  result.xmmsbyte(1)  = SaturateWordSToByteS(op1.xmm16s(1));
  result.xmmsbyte(2)  = SaturateWordSToByteS(op1.xmm16s(2));
  result.xmmsbyte(3)  = SaturateWordSToByteS(op1.xmm16s(3));
  result.xmmsbyte(4)  = SaturateWordSToByteS(op1.xmm16s(4));
  result.xmmsbyte(5)  = SaturateWordSToByteS(op1.xmm16s(5));
  result.xmmsbyte(6)  = SaturateWordSToByteS(op1.xmm16s(6));
  result.xmmsbyte(7)  = SaturateWordSToByteS(op1.xmm16s(7));

  result.xmmsbyte(8)  = SaturateWordSToByteS(op2.xmm16s(0));
  result.xmmsbyte(9)  = SaturateWordSToByteS(op2.xmm16s(1));
  result.xmmsbyte(10) = SaturateWordSToByteS(op2.xmm16s(2));
  result.xmmsbyte(11) = SaturateWordSToByteS(op2.xmm16s(3));
  result.xmmsbyte(12) = SaturateWordSToByteS(op2.xmm16s(4));
  result.xmmsbyte(13) = SaturateWordSToByteS(op2.xmm16s(5));
  result.xmmsbyte(14) = SaturateWordSToByteS(op2.xmm16s(6));
  result.xmmsbyte(15) = SaturateWordSToByteS(op2.xmm16s(7));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PACKSSWB_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 64 */
void BX_CPU_C::PCMPGTB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) 
  {
      result.xmmsbyte(j) = (op1.xmmsbyte(j) > op2.xmmsbyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPGTB_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 65 */
void BX_CPU_C::PCMPGTW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16s(0) = (op1.xmm16s(0) > op2.xmm16s(0)) ? 0xffff : 0;
  result.xmm16s(1) = (op1.xmm16s(1) > op2.xmm16s(1)) ? 0xffff : 0;
  result.xmm16s(2) = (op1.xmm16s(2) > op2.xmm16s(2)) ? 0xffff : 0;
  result.xmm16s(3) = (op1.xmm16s(3) > op2.xmm16s(3)) ? 0xffff : 0;
  result.xmm16s(4) = (op1.xmm16s(4) > op2.xmm16s(4)) ? 0xffff : 0;
  result.xmm16s(5) = (op1.xmm16s(5) > op2.xmm16s(5)) ? 0xffff : 0;
  result.xmm16s(6) = (op1.xmm16s(6) > op2.xmm16s(6)) ? 0xffff : 0;
  result.xmm16s(7) = (op1.xmm16s(7) > op2.xmm16s(7)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPGTW_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 66 */
void BX_CPU_C::PCMPGTD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm32s(0) = (op1.xmm32s(0) > op2.xmm32s(0)) ? 0xffffffff : 0;
  result.xmm32s(1) = (op1.xmm32s(1) > op2.xmm32s(1)) ? 0xffffffff : 0;
  result.xmm32s(2) = (op1.xmm32s(2) > op2.xmm32s(2)) ? 0xffffffff : 0;
  result.xmm32s(3) = (op1.xmm32s(3) > op2.xmm32s(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPGTD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 67 */
void BX_CPU_C::PACKUSWB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmmubyte(0)  = SaturateWordSToByteU(op1.xmm16s(0));
  result.xmmubyte(1)  = SaturateWordSToByteU(op1.xmm16s(1));
  result.xmmubyte(2)  = SaturateWordSToByteU(op1.xmm16s(2));
  result.xmmubyte(3)  = SaturateWordSToByteU(op1.xmm16s(3));
  result.xmmubyte(4)  = SaturateWordSToByteU(op1.xmm16s(4));
  result.xmmubyte(5)  = SaturateWordSToByteU(op1.xmm16s(5));
  result.xmmubyte(6)  = SaturateWordSToByteU(op1.xmm16s(6));
  result.xmmubyte(7)  = SaturateWordSToByteU(op1.xmm16s(7));

  result.xmmubyte(8)  = SaturateWordSToByteU(op2.xmm16s(0));
  result.xmmubyte(9)  = SaturateWordSToByteU(op2.xmm16s(1));
  result.xmmubyte(10) = SaturateWordSToByteU(op2.xmm16s(2));
  result.xmmubyte(11) = SaturateWordSToByteU(op2.xmm16s(3));
  result.xmmubyte(12) = SaturateWordSToByteU(op2.xmm16s(4));
  result.xmmubyte(13) = SaturateWordSToByteU(op2.xmm16s(5));
  result.xmmubyte(14) = SaturateWordSToByteU(op2.xmm16s(6));
  result.xmmubyte(15) = SaturateWordSToByteU(op2.xmm16s(7));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PACKUSWB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKHBW_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKHBW_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKHWD_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKHWD_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKHDQ_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKHDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PACKSSDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PACKSSDW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PACKSSDW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLQDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKLQDQ_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKLQDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHQDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PUNPCKHQDQ_VdqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PUNPCKHQDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_VdqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVD_VdqEd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVD_VdqEd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQA_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVDQA_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVDQA_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQU_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVDQU_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVDQU_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFD_VdqWdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSHUFD_VdqWdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSHUFD_VdqWdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFHW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSHUFHW_VqWqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSHUFHW_VqWqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 74 */
void BX_CPU_C::PCMPEQB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++)
  {
      result.xmmubyte(j) = (op1.xmmubyte(j) == op2.xmmubyte(j)) ? 0xff : 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPEQB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 75 */
void BX_CPU_C::PCMPEQW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16u(0) = (op1.xmm16u(0) == op2.xmm16u(0)) ? 0xffff : 0;
  result.xmm16u(1) = (op1.xmm16u(1) == op2.xmm16u(1)) ? 0xffff : 0;
  result.xmm16u(2) = (op1.xmm16u(2) == op2.xmm16u(2)) ? 0xffff : 0;
  result.xmm16u(3) = (op1.xmm16u(3) == op2.xmm16u(3)) ? 0xffff : 0;
  result.xmm16u(4) = (op1.xmm16u(4) == op2.xmm16u(4)) ? 0xffff : 0;
  result.xmm16u(5) = (op1.xmm16u(5) == op2.xmm16u(5)) ? 0xffff : 0;
  result.xmm16u(6) = (op1.xmm16u(6) == op2.xmm16u(6)) ? 0xffff : 0;
  result.xmm16u(7) = (op1.xmm16u(7) == op2.xmm16u(7)) ? 0xffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPEQW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 76 */
void BX_CPU_C::PCMPEQD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm32u(0) = (op1.xmm32u(0) == op2.xmm32u(0)) ? 0xffffffff : 0;
  result.xmm32u(1) = (op1.xmm32u(1) == op2.xmm32u(1)) ? 0xffffffff : 0;
  result.xmm32u(2) = (op1.xmm32u(2) == op2.xmm32u(2)) ? 0xffffffff : 0;
  result.xmm32u(3) = (op1.xmm32u(3) == op2.xmm32u(3)) ? 0xffffffff : 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PCMPEQD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_EdVd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVD_EdVd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVD_EdVd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_VqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVD_VqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVD_VqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQA_WdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVDQA_WdqVdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVDQA_WdqVdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQU_WdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVDQU_WdqVdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVDQU_WdqVdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPPD_VpdWpdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CMPPD_VpdWpdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSD_VsdWsdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPSD_VsdWsdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CMPSD_VsdWsdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTI_MdGd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVNTI_MdGd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVNTI_MdGd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PINSRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PINSRW_VdqEdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PINSRW_VdqEdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PEXTRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PEXTRW_VdqEdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PEXTRW_VdqEdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SHUFPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SHUFPD_VpdWpdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SHUFPD_VpdWpdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLD_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLQ_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D4 */
void BX_CPU_C::PADDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) += op2.xmm64u(0);
  op1.xmm64u(1) += op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMULLW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMULLW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ_WqVq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVQ_WqVq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVQ_WqVq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQ2Q_PqVRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVDQ2Q_PqVRq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVDQ2Q_PqVRq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ2DQ_VdqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVQ2DQ_VdqQq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVQ2DQ_VdqQq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMOVMSKB_GdVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMOVMSKB_GdVRdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMOVMSKB_GdVRdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D8 */
void BX_CPU_C::PSUBUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=0; j<16; j++) 
  {
      if(op1.xmmubyte(j) > op2.xmmubyte(j)) {
           result.xmmubyte(j) = op1.xmmubyte(j) - op2.xmmubyte(j);
      }
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBUSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D9 */
void BX_CPU_C::PSUBUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm64u(0) = result.xmm64u(1) = 0;

  for(unsigned j=0; j<8; j++) 
  {
      if(op1.xmm16u(j) > op2.xmm16u(j)) {
           result.xmm16u(j) = op1.xmm16u(j) - op2.xmm16u(j);
      }
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBUSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMINUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_INFO(("PMINUB_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMINUB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DB */
void BX_CPU_C::PAND_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) &= op2.xmm64u(0);
  op1.xmm64u(1) &= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PAND_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PADDUSB_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PADDUSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PADDUSW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PADDUSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMAXUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMAXUB_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMAXUB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DF */
void BX_CPU_C::PANDN_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm64u(0) = ~(op1.xmm64u(0) & op2.xmm64u(0));
  result.xmm64u(1) = ~(op1.xmm64u(1) & op2.xmm64u(1));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PANDN_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E0 */
void BX_CPU_C::PAVGB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++)
  {
      result.xmmubyte(j) = (op1.xmmubyte(j) + op2.xmmubyte(j) + 1) >> 1;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PAVGB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRAW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRAW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRAD_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRAD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E3 */
void BX_CPU_C::PAVGW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  result.xmm16u(0) = (op1.xmm16u(0) + op2.xmm16u(0) + 1) >> 1;
  result.xmm16u(1) = (op1.xmm16u(1) + op2.xmm16u(1) + 1) >> 1;
  result.xmm16u(2) = (op1.xmm16u(2) + op2.xmm16u(2) + 1) >> 1;
  result.xmm16u(3) = (op1.xmm16u(3) + op2.xmm16u(3) + 1) >> 1;
  result.xmm16u(4) = (op1.xmm16u(4) + op2.xmm16u(4) + 1) >> 1;
  result.xmm16u(5) = (op1.xmm16u(5) + op2.xmm16u(5) + 1) >> 1;
  result.xmm16u(6) = (op1.xmm16u(6) + op2.xmm16u(6) + 1) >> 1;
  result.xmm16u(7) = (op1.xmm16u(7) + op2.xmm16u(7) + 1) >> 1;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PAVGW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULHUW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMULHUW_VdqWdq: SSE2 instruction still not implemented"));
#else                                        
  BX_INFO(("PMULHUW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULHW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMULHW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMULHW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPD2DQ_VqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPD2DQ_VqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2DQ_VqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2DQ_VqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTDQ2PD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTDQ2PD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTDQ_MdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVNTDQ_MdqVdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVNTDQ_MdqVdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSUBSB_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSUBSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSUBSW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSUBSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMINSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMINSW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMINSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EB */
void BX_CPU_C::POR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) |= op2.xmm64u(0);
  op1.xmm64u(1) |= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("POR_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PADDSB_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PADDSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PADDSW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PADDSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMAXSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMAXSW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMAXSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EF */
void BX_CPU_C::PXOR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) ^= op2.xmm64u(0);
  op1.xmm64u(1) ^= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PXOR_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLD_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLQ_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULUDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMULUDQ_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMULUDQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMADDWD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMADDWD_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PMADDWD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSADBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSADBW_VdqWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSADBW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVDQU_VdqVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MASKMOVDQU_VdqVRdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MASKMOVDQU_VdqVRdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F8 */
void BX_CPU_C::PSUBB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) 
  {
     op1.xmmubyte(j) -= op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F9 */
void BX_CPU_C::PSUBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm16u(0) -= op2.xmm16u(0);
  op1.xmm16u(1) -= op2.xmm16u(1);
  op1.xmm16u(2) -= op2.xmm16u(2);
  op1.xmm16u(3) -= op2.xmm16u(3);
  op1.xmm16u(4) -= op2.xmm16u(4);
  op1.xmm16u(5) -= op2.xmm16u(5);
  op1.xmm16u(6) -= op2.xmm16u(6);
  op1.xmm16u(7) -= op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FA */
void BX_CPU_C::PSUBD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32u(0) -= op2.xmm32u(0);
  op1.xmm32u(1) -= op2.xmm32u(1);
  op1.xmm32u(2) -= op2.xmm32u(2);
  op1.xmm32u(3) -= op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FB */
void BX_CPU_C::PSUBQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm64u(0) -= op2.xmm64u(0);
  op1.xmm64u(1) -= op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSUBQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FC */
void BX_CPU_C::PADDB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  for(unsigned j=0; j<16; j++) 
  {
     op1.xmmubyte(j) += op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FD */
void BX_CPU_C::PADDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm16u(0) += op2.xmm16u(0);
  op1.xmm16u(1) += op2.xmm16u(1);
  op1.xmm16u(2) += op2.xmm16u(2);
  op1.xmm16u(3) += op2.xmm16u(3);
  op1.xmm16u(4) += op2.xmm16u(4);
  op1.xmm16u(5) += op2.xmm16u(5);
  op1.xmm16u(6) += op2.xmm16u(6);
  op1.xmm16u(7) += op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F FE */
void BX_CPU_C::PADDD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  op1.xmm32u(0) += op2.xmm32u(0);
  op1.xmm32u(1) += op2.xmm32u(1);
  op1.xmm32u(2) += op2.xmm32u(2);
  op1.xmm32u(3) += op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PADDD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLW_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRAW_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRAW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLW_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLD_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRAD_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRAD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLD_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLQ_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLQ_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSRLDQ_WdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSRLDQ_WdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLQ_PdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLQ_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSLLDQ_WdqIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("PSLLDQ_WdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
