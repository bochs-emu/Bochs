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

/* 66 0F 60 */
void BX_CPU_C::PUNPCKLBW_VdqWq(bxInstruction_c *i)
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

  result.xmmubyte(0x0) = op1.xmmubyte(0);
  result.xmmubyte(0x1) = op2.xmmubyte(0);
  result.xmmubyte(0x2) = op1.xmmubyte(1);
  result.xmmubyte(0x3) = op2.xmmubyte(1);
  result.xmmubyte(0x4) = op1.xmmubyte(2);
  result.xmmubyte(0x5) = op2.xmmubyte(2);
  result.xmmubyte(0x6) = op1.xmmubyte(3);
  result.xmmubyte(0x7) = op2.xmmubyte(3);
  result.xmmubyte(0x8) = op1.xmmubyte(4);
  result.xmmubyte(0x9) = op2.xmmubyte(4);
  result.xmmubyte(0xA) = op1.xmmubyte(5);
  result.xmmubyte(0xB) = op2.xmmubyte(5);
  result.xmmubyte(0xC) = op1.xmmubyte(6);
  result.xmmubyte(0xD) = op2.xmmubyte(6);
  result.xmmubyte(0xE) = op1.xmmubyte(7);
  result.xmmubyte(0xF) = op2.xmmubyte(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKLBW_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 61 */
void BX_CPU_C::PUNPCKLWD_VdqWq(bxInstruction_c *i)
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

  result.xmm16u(0) = op1.xmm16u(0);
  result.xmm16u(1) = op2.xmm16u(0);
  result.xmm16u(2) = op1.xmm16u(1);
  result.xmm16u(3) = op2.xmm16u(1);
  result.xmm16u(4) = op1.xmm16u(2);
  result.xmm16u(5) = op2.xmm16u(2);
  result.xmm16u(6) = op1.xmm16u(3);
  result.xmm16u(7) = op2.xmm16u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKLWD_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 62 */
void BX_CPU_C::PUNPCKLDQ_VdqWq(bxInstruction_c *i)
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

  result.xmm32u(0) = op1.xmm32u(0);
  result.xmm32u(1) = op2.xmm32u(0);
  result.xmm32u(2) = op1.xmm32u(1);
  result.xmm32u(3) = op2.xmm32u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
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

  result.xmmsbyte(0x0) = SaturateWordSToByteS(op1.xmm16s(0));
  result.xmmsbyte(0x1) = SaturateWordSToByteS(op1.xmm16s(1));
  result.xmmsbyte(0x2) = SaturateWordSToByteS(op1.xmm16s(2));
  result.xmmsbyte(0x3) = SaturateWordSToByteS(op1.xmm16s(3));
  result.xmmsbyte(0x4) = SaturateWordSToByteS(op1.xmm16s(4));
  result.xmmsbyte(0x5) = SaturateWordSToByteS(op1.xmm16s(5));
  result.xmmsbyte(0x6) = SaturateWordSToByteS(op1.xmm16s(6));
  result.xmmsbyte(0x7) = SaturateWordSToByteS(op1.xmm16s(7));

  result.xmmsbyte(0x8) = SaturateWordSToByteS(op2.xmm16s(0));
  result.xmmsbyte(0x9) = SaturateWordSToByteS(op2.xmm16s(1));
  result.xmmsbyte(0xA) = SaturateWordSToByteS(op2.xmm16s(2));
  result.xmmsbyte(0xB) = SaturateWordSToByteS(op2.xmm16s(3));
  result.xmmsbyte(0xC) = SaturateWordSToByteS(op2.xmm16s(4));
  result.xmmsbyte(0xD) = SaturateWordSToByteS(op2.xmm16s(5));
  result.xmmsbyte(0xE) = SaturateWordSToByteS(op2.xmm16s(6));
  result.xmmsbyte(0xF) = SaturateWordSToByteS(op2.xmm16s(7));

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

  result.xmmubyte(0x0) = SaturateWordSToByteU(op1.xmm16s(0));
  result.xmmubyte(0x1) = SaturateWordSToByteU(op1.xmm16s(1));
  result.xmmubyte(0x2) = SaturateWordSToByteU(op1.xmm16s(2));
  result.xmmubyte(0x3) = SaturateWordSToByteU(op1.xmm16s(3));
  result.xmmubyte(0x4) = SaturateWordSToByteU(op1.xmm16s(4));
  result.xmmubyte(0x5) = SaturateWordSToByteU(op1.xmm16s(5));
  result.xmmubyte(0x6) = SaturateWordSToByteU(op1.xmm16s(6));
  result.xmmubyte(0x7) = SaturateWordSToByteU(op1.xmm16s(7));

  result.xmmubyte(0x8) = SaturateWordSToByteU(op2.xmm16s(0));
  result.xmmubyte(0x9) = SaturateWordSToByteU(op2.xmm16s(1));
  result.xmmubyte(0xA) = SaturateWordSToByteU(op2.xmm16s(2));
  result.xmmubyte(0xB) = SaturateWordSToByteU(op2.xmm16s(3));
  result.xmmubyte(0xC) = SaturateWordSToByteU(op2.xmm16s(4));
  result.xmmubyte(0xD) = SaturateWordSToByteU(op2.xmm16s(5));
  result.xmmubyte(0xE) = SaturateWordSToByteU(op2.xmm16s(6));
  result.xmmubyte(0xF) = SaturateWordSToByteU(op2.xmm16s(7));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PACKUSWB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 68 */
void BX_CPU_C::PUNPCKHBW_VdqWq(bxInstruction_c *i)
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

  result.xmmubyte(0x0) = op1.xmmubyte(0x8);
  result.xmmubyte(0x1) = op2.xmmubyte(0x8);
  result.xmmubyte(0x2) = op1.xmmubyte(0x9);
  result.xmmubyte(0x3) = op2.xmmubyte(0x9);
  result.xmmubyte(0x4) = op1.xmmubyte(0xA);
  result.xmmubyte(0x5) = op2.xmmubyte(0xA);
  result.xmmubyte(0x6) = op1.xmmubyte(0xB);
  result.xmmubyte(0x7) = op2.xmmubyte(0xB);
  result.xmmubyte(0x8) = op1.xmmubyte(0xC);
  result.xmmubyte(0x9) = op2.xmmubyte(0xC);
  result.xmmubyte(0xA) = op1.xmmubyte(0xD);
  result.xmmubyte(0xB) = op2.xmmubyte(0xD);
  result.xmmubyte(0xC) = op1.xmmubyte(0xE);
  result.xmmubyte(0xD) = op2.xmmubyte(0xE);
  result.xmmubyte(0xE) = op1.xmmubyte(0xF);
  result.xmmubyte(0xF) = op2.xmmubyte(0xF);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKHBW_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 69 */
void BX_CPU_C::PUNPCKHWD_VdqWq(bxInstruction_c *i)
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

  result.xmm16u(0) = op1.xmm16u(4);
  result.xmm16u(1) = op2.xmm16u(4);
  result.xmm16u(2) = op1.xmm16u(5);
  result.xmm16u(3) = op2.xmm16u(5);
  result.xmm16u(4) = op1.xmm16u(6);
  result.xmm16u(5) = op2.xmm16u(6);
  result.xmm16u(6) = op1.xmm16u(7);
  result.xmm16u(7) = op2.xmm16u(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKHWD_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6A */
void BX_CPU_C::PUNPCKHDQ_VdqWq(bxInstruction_c *i)
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

  result.xmm32u(0) = op1.xmm32u(2);
  result.xmm32u(1) = op2.xmm32u(2);
  result.xmm32u(2) = op1.xmm32u(3);
  result.xmm32u(3) = op2.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKHDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6B */
void BX_CPU_C::PACKSSDW_VdqWdq(bxInstruction_c *i)
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

  result.xmm16s(0) = SaturateDwordSToWordS(op1.xmm32s(0));
  result.xmm16s(1) = SaturateDwordSToWordS(op1.xmm32s(1));
  result.xmm16s(2) = SaturateDwordSToWordS(op1.xmm32s(2));
  result.xmm16s(3) = SaturateDwordSToWordS(op1.xmm32s(3));

  result.xmm16s(4) = SaturateDwordSToWordS(op2.xmm32s(0));
  result.xmm16s(5) = SaturateDwordSToWordS(op2.xmm32s(1));
  result.xmm16s(6) = SaturateDwordSToWordS(op2.xmm32s(2));
  result.xmm16s(7) = SaturateDwordSToWordS(op2.xmm32s(3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PACKSSDW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6C */
void BX_CPU_C::PUNPCKLQDQ_VdqWq(bxInstruction_c *i)
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

  op1.xmm64u(1) = op2.xmm64u(0);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PUNPCKLQDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6D */
void BX_CPU_C::PUNPCKHQDQ_VdqWq(bxInstruction_c *i)
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

  result.xmm64u(0) = op1.xmm64u(1);
  result.xmm64u(1) = op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PUNPCKHQDQ_VdqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6E */
void BX_CPU_C::MOVD_VdqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;
  Bit32u val32;

  /* val32 is a register or memory reference */
  if (i->modC0()) {
    val32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);
  }

  op.xmm64u(1) = 0;
  op.xmm32u(1) = 0;
  op.xmm32u(0) = val32;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVD_VdqEd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6F */
void BX_CPU_C::MOVDQA_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
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

/* 66 0F 7E */
void BX_CPU_C::MOVD_EdVd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u val32 = op.xmm32u(0);

  /* destination is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_32BIT_REG(i->rm(), val32);
  }
  else {
    write_virtual_dword(i->seg(), RMAddr(i), &val32);
  }
#else
  BX_INFO(("MOVD_EdVd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ_VqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVQ_VqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVQ_VqWq: SSE2 not supported in current configuration"));
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

/* 66 0F D1 */
void BX_CPU_C::PSRLW_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 15) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm16u(0) >>= shift;
    op1.xmm16u(1) >>= shift;
    op1.xmm16u(2) >>= shift;
    op1.xmm16u(3) >>= shift;
    op1.xmm16u(4) >>= shift;
    op1.xmm16u(5) >>= shift;
    op1.xmm16u(6) >>= shift;
    op1.xmm16u(7) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSRLW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D2 */
void BX_CPU_C::PSRLD_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 31) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm32u(0) >>= shift;
    op1.xmm32u(1) >>= shift;
    op1.xmm32u(2) >>= shift;
    op1.xmm32u(3) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSRLD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D3 */
void BX_CPU_C::PSRLQ_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 63) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm64u(0) >>= shift;
    op1.xmm64u(1) >>= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
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

/* 66 0F D7 */
void BX_CPU_C::PMOVMSKB_GdVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->rm());
  Bit32u result = 0;

  if(op.xmmubyte(0x0) & 0x80) result |= 0x0001; 
  if(op.xmmubyte(0x1) & 0x80) result |= 0x0002; 
  if(op.xmmubyte(0x2) & 0x80) result |= 0x0004; 
  if(op.xmmubyte(0x3) & 0x80) result |= 0x0008; 
  if(op.xmmubyte(0x4) & 0x80) result |= 0x0010; 
  if(op.xmmubyte(0x5) & 0x80) result |= 0x0020; 
  if(op.xmmubyte(0x6) & 0x80) result |= 0x0040; 
  if(op.xmmubyte(0x7) & 0x80) result |= 0x0080; 
  if(op.xmmubyte(0x8) & 0x80) result |= 0x0100; 
  if(op.xmmubyte(0x9) & 0x80) result |= 0x0200; 
  if(op.xmmubyte(0xA) & 0x80) result |= 0x0400; 
  if(op.xmmubyte(0xB) & 0x80) result |= 0x0800; 
  if(op.xmmubyte(0xC) & 0x80) result |= 0x1000; 
  if(op.xmmubyte(0xD) & 0x80) result |= 0x2000; 
  if(op.xmmubyte(0xE) & 0x80) result |= 0x4000; 
  if(op.xmmubyte(0xF) & 0x80) result |= 0x8000; 

  /* now write result back to destination */
  BX_WRITE_32BIT_REG(i->nnn(), result);
  
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
      if(op1.xmmubyte(j) > op2.xmmubyte(j)) 
      {
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
      if(op1.xmm16u(j) > op2.xmm16u(j)) 
      {
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

/* 66 0F DA */
void BX_CPU_C::PMINUB_VdqWdq(bxInstruction_c *i)
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
    if(op2.xmmubyte(j) < op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
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

/* 66 0F DC */
void BX_CPU_C::PADDUSB_VdqWdq(bxInstruction_c *i)
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
    result.xmmubyte(j) = SaturateWordSToByteU(Bit16s(op1.xmmubyte(j)) + Bit16s(op2.xmmubyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDUSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DD */
void BX_CPU_C::PADDUSW_VdqWdq(bxInstruction_c *i)
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

  result.xmm16u(0) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(0)) + Bit32s(op2.xmm16u(0)));
  result.xmm16u(1) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(1)) + Bit32s(op2.xmm16u(1)));
  result.xmm16u(2) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(2)) + Bit32s(op2.xmm16u(2)));
  result.xmm16u(3) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(3)) + Bit32s(op2.xmm16u(3)));
  result.xmm16u(4) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(4)) + Bit32s(op2.xmm16u(4)));
  result.xmm16u(5) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(5)) + Bit32s(op2.xmm16u(5)));
  result.xmm16u(6) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(6)) + Bit32s(op2.xmm16u(6)));
  result.xmm16u(7) = SaturateDwordSToWordU(Bit32s(op1.xmm16u(7)) + Bit32s(op2.xmm16u(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDUSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F DE */
void BX_CPU_C::PMAXUB_VdqWdq(bxInstruction_c *i)
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
    if(op2.xmmubyte(j) > op1.xmmubyte(j)) op1.xmmubyte(j) = op2.xmmubyte(j);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
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

/* 66 0F E1 */
void BX_CPU_C::PSRAW_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 15) 
  {
    result.xmm16u(0) = (op1.xmm16u(0) & 0x8000) ? 0xffff : 0;
    result.xmm16u(1) = (op1.xmm16u(1) & 0x8000) ? 0xffff : 0;
    result.xmm16u(2) = (op1.xmm16u(2) & 0x8000) ? 0xffff : 0;
    result.xmm16u(3) = (op1.xmm16u(3) & 0x8000) ? 0xffff : 0;
    result.xmm16u(4) = (op1.xmm16u(4) & 0x8000) ? 0xffff : 0;
    result.xmm16u(5) = (op1.xmm16u(5) & 0x8000) ? 0xffff : 0;
    result.xmm16u(6) = (op1.xmm16u(6) & 0x8000) ? 0xffff : 0;
    result.xmm16u(7) = (op1.xmm16u(7) & 0x8000) ? 0xffff : 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    result.xmm16u(0) = op1.xmm16u(0) >> shift;
    result.xmm16u(1) = op1.xmm16u(1) >> shift;
    result.xmm16u(2) = op1.xmm16u(2) >> shift;
    result.xmm16u(3) = op1.xmm16u(3) >> shift;
    result.xmm16u(4) = op1.xmm16u(4) >> shift;
    result.xmm16u(5) = op1.xmm16u(5) >> shift;
    result.xmm16u(6) = op1.xmm16u(6) >> shift;
    result.xmm16u(7) = op1.xmm16u(7) >> shift;

    if(op1.xmm16u(0) & 0x8000) result.xmm16u(0) |= (0xffff << (16 - shift));
    if(op1.xmm16u(1) & 0x8000) result.xmm16u(1) |= (0xffff << (16 - shift));
    if(op1.xmm16u(2) & 0x8000) result.xmm16u(2) |= (0xffff << (16 - shift));
    if(op1.xmm16u(3) & 0x8000) result.xmm16u(3) |= (0xffff << (16 - shift));
    if(op1.xmm16u(4) & 0x8000) result.xmm16u(4) |= (0xffff << (16 - shift));
    if(op1.xmm16u(5) & 0x8000) result.xmm16u(5) |= (0xffff << (16 - shift));
    if(op1.xmm16u(6) & 0x8000) result.xmm16u(6) |= (0xffff << (16 - shift));
    if(op1.xmm16u(7) & 0x8000) result.xmm16u(7) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSRAW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E2 */
void BX_CPU_C::PSRAD_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 31) 
  {
    result.xmm32u(0) = (op1.xmm32u(0) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(1) = (op1.xmm32u(1) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(2) = (op1.xmm32u(2) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(3) = (op1.xmm32u(3) & 0x80000000) ? 0xffffffff : 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    result.xmm32u(0) = op1.xmm32u(0) >> shift;
    result.xmm32u(1) = op1.xmm32u(1) >> shift;
    result.xmm32u(2) = op1.xmm32u(2) >> shift;
    result.xmm32u(3) = op1.xmm32u(3) >> shift;

    if(op1.xmm32u(0) & 0x80000000) result.xmm32u(0) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(1) & 0x80000000) result.xmm32u(1) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(2) & 0x80000000) result.xmm32u(2) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(3) & 0x80000000) result.xmm32u(3) |= (0xffffffff << (32-shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
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

/* 66 0F E8 */
void BX_CPU_C::PSUBSB_VdqWdq(bxInstruction_c *i)
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
    result.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) - Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F E9 */
void BX_CPU_C::PSUBSW_VdqWdq(bxInstruction_c *i)
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

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) - Bit32s(op2.xmm16s(0)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) - Bit32s(op2.xmm16s(1)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) - Bit32s(op2.xmm16s(2)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) - Bit32s(op2.xmm16s(3)));
  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) - Bit32s(op2.xmm16s(4)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) - Bit32s(op2.xmm16s(5)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) - Bit32s(op2.xmm16s(6)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) - Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSUBSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F EA */
void BX_CPU_C::PMINSW_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm16s(0) < op1.xmm16s(0)) op1.xmm16s(0) = op2.xmm16s(0);
  if(op2.xmm16s(1) < op1.xmm16s(1)) op1.xmm16s(1) = op2.xmm16s(1);
  if(op2.xmm16s(2) < op1.xmm16s(2)) op1.xmm16s(2) = op2.xmm16s(2);
  if(op2.xmm16s(3) < op1.xmm16s(3)) op1.xmm16s(3) = op2.xmm16s(3);
  if(op2.xmm16s(4) < op1.xmm16s(4)) op1.xmm16s(4) = op2.xmm16s(4);
  if(op2.xmm16s(5) < op1.xmm16s(5)) op1.xmm16s(5) = op2.xmm16s(5);
  if(op2.xmm16s(6) < op1.xmm16s(6)) op1.xmm16s(6) = op2.xmm16s(6);
  if(op2.xmm16s(7) < op1.xmm16s(7)) op1.xmm16s(7) = op2.xmm16s(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
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

/* 66 0F EC */
void BX_CPU_C::PADDSB_VdqWdq(bxInstruction_c *i)
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
    result.xmmsbyte(j) = SaturateWordSToByteS(Bit16s(op1.xmmsbyte(j)) + Bit16s(op2.xmmsbyte(j)));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDSB_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F ED */
void BX_CPU_C::PADDSW_VdqWdq(bxInstruction_c *i)
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

  result.xmm16s(0) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(0)) + Bit32s(op2.xmm16s(0)));
  result.xmm16s(1) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(1)) + Bit32s(op2.xmm16s(1)));
  result.xmm16s(2) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(2)) + Bit32s(op2.xmm16s(2)));
  result.xmm16s(3) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(3)) + Bit32s(op2.xmm16s(3)));
  result.xmm16s(4) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(4)) + Bit32s(op2.xmm16s(4)));
  result.xmm16s(5) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(5)) + Bit32s(op2.xmm16s(5)));
  result.xmm16s(6) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(6)) + Bit32s(op2.xmm16s(6)));
  result.xmm16s(7) = SaturateDwordSToWordS(Bit32s(op1.xmm16s(7)) + Bit32s(op2.xmm16s(7)));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PADDSW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMAXSW_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm16s(0) > op1.xmm16s(0)) op1.xmm16s(0) = op2.xmm16s(0);
  if(op2.xmm16s(1) > op1.xmm16s(1)) op1.xmm16s(1) = op2.xmm16s(1);
  if(op2.xmm16s(2) > op1.xmm16s(2)) op1.xmm16s(2) = op2.xmm16s(2);
  if(op2.xmm16s(3) > op1.xmm16s(3)) op1.xmm16s(3) = op2.xmm16s(3);
  if(op2.xmm16s(4) > op1.xmm16s(4)) op1.xmm16s(4) = op2.xmm16s(4);
  if(op2.xmm16s(5) > op1.xmm16s(5)) op1.xmm16s(5) = op2.xmm16s(5);
  if(op2.xmm16s(6) > op1.xmm16s(6)) op1.xmm16s(6) = op2.xmm16s(6);
  if(op2.xmm16s(7) > op1.xmm16s(7)) op1.xmm16s(7) = op2.xmm16s(7);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
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

/* 66 0F F1 */
void BX_CPU_C::PSLLW_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 15) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm16u(0) <<= shift;
    op1.xmm16u(1) <<= shift;
    op1.xmm16u(2) <<= shift;
    op1.xmm16u(3) <<= shift;
    op1.xmm16u(4) <<= shift;
    op1.xmm16u(5) <<= shift;
    op1.xmm16u(6) <<= shift;
    op1.xmm16u(7) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSLLW_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F2 */
void BX_CPU_C::PSLLD_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 31) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm32u(0) <<= shift;
    op1.xmm32u(1) <<= shift;
    op1.xmm32u(2) <<= shift;
    op1.xmm32u(3) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSLLD_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F3 */
void BX_CPU_C::PSLLQ_VdqWdq(bxInstruction_c *i)
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

  if(op2.xmm64u(1) != 0 || op2.xmm64u(0) > 63) 
  {
    op1.xmm64u(0) = 0;
    op1.xmm64u(1) = 0;
  }
  else
  {
    Bit8u shift = op2.xmmubyte(0);

    op1.xmm64u(0) <<= shift;
    op1.xmm64u(1) <<= shift;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("PSLLQ_VdqWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F4 */
void BX_CPU_C::PMULUDQ_VdqWdq(bxInstruction_c *i)
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

  result.xmm64u(0) = Bit64u(op1.xmm32u(0)) * Bit64u(op2.xmm32u(0));
  result.xmm64u(1) = Bit64u(op1.xmm32u(3)) * Bit64u(op2.xmm32u(3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
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

/* 66 0F F6 */
void BX_CPU_C::PSADBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->nnn()), op2, result;
  Bit16u temp1 = 0, temp2 = 0;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op2);
  }

  temp1 += abs(op1.xmmubyte(0x0) - op2.xmmubyte(0x0));
  temp1 += abs(op1.xmmubyte(0x1) - op2.xmmubyte(0x1));
  temp1 += abs(op1.xmmubyte(0x2) - op2.xmmubyte(0x2));
  temp1 += abs(op1.xmmubyte(0x3) - op2.xmmubyte(0x3));
  temp1 += abs(op1.xmmubyte(0x4) - op2.xmmubyte(0x4));
  temp1 += abs(op1.xmmubyte(0x5) - op2.xmmubyte(0x5));
  temp1 += abs(op1.xmmubyte(0x6) - op2.xmmubyte(0x6));
  temp1 += abs(op1.xmmubyte(0x7) - op2.xmmubyte(0x7));

  temp2 += abs(op1.xmmubyte(0x8) - op2.xmmubyte(0x8));
  temp2 += abs(op1.xmmubyte(0x9) - op2.xmmubyte(0x9));
  temp2 += abs(op1.xmmubyte(0xA) - op2.xmmubyte(0xA));
  temp2 += abs(op1.xmmubyte(0xB) - op2.xmmubyte(0xB));
  temp2 += abs(op1.xmmubyte(0xC) - op2.xmmubyte(0xC));
  temp2 += abs(op1.xmmubyte(0xD) - op2.xmmubyte(0xD));
  temp2 += abs(op1.xmmubyte(0xE) - op2.xmmubyte(0xE));
  temp2 += abs(op1.xmmubyte(0xF) - op2.xmmubyte(0xF));

  result.xmm64u(0) = Bit64u(temp1);
  result.xmm64u(1) = Bit64u(temp2);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
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

/* 66 0F 71 Grp12 010 */
void BX_CPU_C::PSRLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm16u(0) >>= shift;
  op1.xmm16u(1) >>= shift;
  op1.xmm16u(2) >>= shift;
  op1.xmm16u(3) >>= shift;
  op1.xmm16u(4) >>= shift;
  op1.xmm16u(5) >>= shift;
  op1.xmm16u(6) >>= shift;
  op1.xmm16u(7) >>= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
#else
  BX_INFO(("PSRLW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 71 Grp12 100 */
void BX_CPU_C::PSRAW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift > 15) {
    result.xmm16u(0) = (op1.xmm16u(0) & 0x8000) ? 0xffff : 0;
    result.xmm16u(1) = (op1.xmm16u(1) & 0x8000) ? 0xffff : 0;
    result.xmm16u(2) = (op1.xmm16u(2) & 0x8000) ? 0xffff : 0;
    result.xmm16u(3) = (op1.xmm16u(3) & 0x8000) ? 0xffff : 0;
    result.xmm16u(4) = (op1.xmm16u(4) & 0x8000) ? 0xffff : 0;
    result.xmm16u(5) = (op1.xmm16u(5) & 0x8000) ? 0xffff : 0;
    result.xmm16u(6) = (op1.xmm16u(6) & 0x8000) ? 0xffff : 0;
    result.xmm16u(7) = (op1.xmm16u(7) & 0x8000) ? 0xffff : 0;
  }
  else {
    result.xmm16u(0) = op1.xmm16u(0) >> shift;
    result.xmm16u(1) = op1.xmm16u(1) >> shift;
    result.xmm16u(2) = op1.xmm16u(2) >> shift;
    result.xmm16u(3) = op1.xmm16u(3) >> shift;
    result.xmm16u(4) = op1.xmm16u(4) >> shift;
    result.xmm16u(5) = op1.xmm16u(5) >> shift;
    result.xmm16u(6) = op1.xmm16u(6) >> shift;
    result.xmm16u(7) = op1.xmm16u(7) >> shift;

    if(op1.xmm16u(0) & 0x8000) result.xmm16u(0) |= (0xffff << (16 - shift));
    if(op1.xmm16u(1) & 0x8000) result.xmm16u(1) |= (0xffff << (16 - shift));
    if(op1.xmm16u(2) & 0x8000) result.xmm16u(2) |= (0xffff << (16 - shift));
    if(op1.xmm16u(3) & 0x8000) result.xmm16u(3) |= (0xffff << (16 - shift));
    if(op1.xmm16u(4) & 0x8000) result.xmm16u(4) |= (0xffff << (16 - shift));
    if(op1.xmm16u(5) & 0x8000) result.xmm16u(5) |= (0xffff << (16 - shift));
    if(op1.xmm16u(6) & 0x8000) result.xmm16u(6) |= (0xffff << (16 - shift));
    if(op1.xmm16u(7) & 0x8000) result.xmm16u(7) |= (0xffff << (16 - shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSRAW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 71 Grp12 110 */
void BX_CPU_C::PSLLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm16u(0) <<= shift;
  op1.xmm16u(1) <<= shift;
  op1.xmm16u(2) <<= shift;
  op1.xmm16u(3) <<= shift;
  op1.xmm16u(4) <<= shift;
  op1.xmm16u(5) <<= shift;
  op1.xmm16u(6) <<= shift;
  op1.xmm16u(7) <<= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
#else
  BX_INFO(("PSLLW_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 72 Grp13 010 */
void BX_CPU_C::PSRLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm32u(0) >>= shift;
  op1.xmm32u(1) >>= shift;
  op1.xmm32u(2) >>= shift;
  op1.xmm32u(3) >>= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
#else
  BX_INFO(("PSRLD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F 72 Grp13 100 */
void BX_CPU_C::PSRAD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm()), result;
  Bit8u shift = i->Ib();

  if(shift > 31) {
    result.xmm32u(0) = (op1.xmm32u(0) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(1) = (op1.xmm32u(1) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(2) = (op1.xmm32u(2) & 0x80000000) ? 0xffffffff : 0;
    result.xmm32u(3) = (op1.xmm32u(3) & 0x80000000) ? 0xffffffff : 0;
  }
  else {
    result.xmm32u(0) = op1.xmm32u(0) >> shift;
    result.xmm32u(1) = op1.xmm32u(1) >> shift;
    result.xmm32u(2) = op1.xmm32u(2) >> shift;
    result.xmm32u(3) = op1.xmm32u(3) >> shift;

    if(op1.xmm32u(0) & 0x80000000) result.xmm32u(0) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(1) & 0x80000000) result.xmm32u(1) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(2) & 0x80000000) result.xmm32u(2) |= (0xffffffff << (32-shift));
    if(op1.xmm32u(3) & 0x80000000) result.xmm32u(3) |= (0xffffffff << (32-shift));
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), result);
#else
  BX_INFO(("PSRAD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 72 Grp13 110 */
void BX_CPU_C::PSLLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm32u(0) <<= shift;
  op1.xmm32u(1) <<= shift;
  op1.xmm32u(2) <<= shift;
  op1.xmm32u(3) <<= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
#else
  BX_INFO(("PSLLD_PdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 73 Grp14 010 */
void BX_CPU_C::PSRLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm64u(0) >>= shift;
  op1.xmm64u(1) >>= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
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

/* 66 0F 73 Grp14 110 */
void BX_CPU_C::PSLLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->rm());
  Bit8u shift = i->Ib();

  op1.xmm64u(0) <<= shift;
  op1.xmm64u(1) <<= shift;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->rm(), op1);
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
