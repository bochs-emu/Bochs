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
  BX_CPU_THIS_PTR PrepareSSEInstruction();

  BX_PANIC(("SSE2 Instruction MOVUPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPD_WpdVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVUPD_WpdVpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_WsdVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVSD_WsdVsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPD_VsdMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVLPD_VsdMq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPD_MqVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVLPD_MqVsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction UNPCKLPD_VpdWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction UNPCKHPD_VpdWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPD_VpdMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVHPD_VpdMq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPD_MqVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVHPD_MqVpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVAPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPD_WpdVpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVAPD_WpdVpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PD_VpdQd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPI2PD_VpdQd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSI2SD_VsdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTSI2SD_VsdEd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTPD_MdqVpd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVNTPD_MdqVpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTTPD2PI_PqWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTTSD2SI_GdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPD2PI_PqWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTSD2SI_GdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISD_VsdWsd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction UCOMISD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISD_VpdWpd(bxInstruction_c *i)   
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction COMISD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPD_EdVRpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVMSKPD_EdVRpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction SQRTPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction SQRTSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction ANDPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDNPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction ANDNPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ORPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction ORPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XORPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction XORPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction ADDPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction ADDSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MULPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MULSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PD_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPS2PD_VpsWps still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PS_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPD2PS_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SS_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTSD2SS_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SD_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTSS2SD_VssWss still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PS_VpsWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTDQ2PS_VpsWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPS2DQ_VdqWps still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTTPS2DQ_VdqWps still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction SUBPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction SUBSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MINPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MINSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction DIVPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction DIVSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MAXPD_VpdWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MAXSD_VsdWsd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKLBW_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKLWD_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKLDQ_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PACKSSWB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PACKSSWB_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPGTB_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPGTB_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPGTW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPGTW_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPGTD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPGTD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PACKUSWB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PACKUSWB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKHBW_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKHWD_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKHDQ_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PACKSSDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PACKSSDW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKLQDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKLQDQ_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PUNPCKHQDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PUNPCKHQDQ_VdqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_VdqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVD_VdqEd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQA_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVDQA_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQU_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVDQU_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFD_VdqWdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSHUFD_VdqWdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFHW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSHUFHW_VqWqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPEQB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPEQB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPEQW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPEQW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PCMPEQD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PCMPEQD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_EdVd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVD_EdVd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVD_VqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVD_VqWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQA_WdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVDQA_WdqVdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQU_WdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVDQU_WdqVdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CMPPD_VpdWpdIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSD_VsdWsdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CMPSD_VsdWsdIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTI_MdGd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVNTI_MdGd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PINSRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PINSRW_VdqEdIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PEXTRW_VdqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PEXTRW_VdqEdIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SHUFPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction SHUFPD_VpdWpdIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLQ_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDQ_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMULLW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ_WqVq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVQ_WqVq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVDQ2Q_PqVRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVDQ2Q_PqVRq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ2DQ_VdqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVQ2DQ_VdqQq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMOVMSKB_GdVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMOVMSKB_GdVRdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBUSB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBUSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMINUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMINUB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PAND_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PAND_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDUSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDUSB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDUSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDUSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMAXUB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMAXUB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PANDN_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PANDN_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PAVGB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PAVGB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRAW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRAD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PAVGW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PAVGW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULHUW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMULHUW_VdqWdq still not implemented"));
#else                                        
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULHW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMULHW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTTPD2DQ_VqWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTPD2DQ_VqWpd still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction CVTDQ2PD_VpdWq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTDQ_MdqVdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MOVNTDQ_MdqVdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBSB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMINSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMINSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::POR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction POR_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDSB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDSB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMAXSW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMAXSW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PXOR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PXOR_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLQ_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMULUDQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMULUDQ_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMADDWD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PMADDWD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSADBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSADBW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVDQU_VdqVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction MASKMOVDQU_VdqVRdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSUBQ_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSUBQ_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDB_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDB_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDW_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDW_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PADDD_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PADDD_VdqWdq still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLW_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRAW_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLW_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLW_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLD_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRAD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRAD_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLD_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLD_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLQ_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSRLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSRLDQ_WdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLQ_PdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLQ_PdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSLLDQ_WdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE2
  BX_CPU_THIS_PTR PrepareSseInstruction();

  BX_PANIC(("SSE2 Instruction PSLLDQ_WdqIb still not implemented"));
#else
  BX_INFO(("SSE2 instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
