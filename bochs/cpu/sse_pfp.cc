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


void BX_CPU_C::CVTSI2SD_VsdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSI2SD_VsdEd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSI2SD_VsdEd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PD_VpdQd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPI2PD_VpdQd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPI2PD_VpdQd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTSD2SI_GdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTSD2SI_GdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SI_GdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSD2SI_GdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSD2SI_GdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2PI_PqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPD2PI_PqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPD2PI_PqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PD_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2PD_VpsWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPS2PD_VpsWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PS_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2PS_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2PS_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2PI_PqWpd(bxInstruction_c *i)
{                      
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2PI_PqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2PI_PqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSD2SS_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSD2SS_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSD2SS_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SD_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSS2SD_VssWss: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTSS2SD_VssWss: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PS_VpsWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTDQ2PS_VpsWdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTDQ2PS_VpsWdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2DQ_VdqWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPS2DQ_VdqWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2DQ_VdqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPS2DQ_VdqWps: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPS2DQ_VdqWps: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PS_VpsQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPI2PS_VpsQq: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTPI2PS_VpsQq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSI2SS_VssEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSI2SS_VssEd: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTSI2SS_VssEd: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPS2PI_PqWps: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTTPS2PI_PqWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTSS2SI_GdWss: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTTSS2SI_GdWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2PI_PqWps: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTPS2PI_PqWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSS2SI_GdWss: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTSS2SI_GdWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPD2DQ_VqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTTPD2DQ_VqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPD2DQ_VqWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPD2DQ_VqWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTPD2DQ_VqWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTDQ2PD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTDQ2PD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CVTDQ2PD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("COMISS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("COMISS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISD_VpdWpd(bxInstruction_c *i)   
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("COMISD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("COMISD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UCOMISS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("UCOMISS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISD_VsdWsd(bxInstruction_c *i)            	
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UCOMISD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UCOMISD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("SQRTSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SQRTSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("SQRTPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SQRTPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RSQRTPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("RSQRTPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RSQRTSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("RSQRTSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RCPPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("RCPPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RCPSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("RCPSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("ADDSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ADDSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("ADDPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("ADDPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MULSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MULSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MULPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MULPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("SUBSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SUBSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("SUBPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SUBPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MINSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MINSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MINPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MINPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("DIVSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("DIVSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("DIVPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("DIVPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MAXPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MAXSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MAXSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPD_VpdWpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXPD_VpdWpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MAXPD_VpdWpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSS_VssWssIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPSS_VssWssIb: SSE instruction still not implemented"));
#else
  BX_INFO(("CMPSS_VssWssIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPPS_VpsWpsIb: SSE instruction still not implemented"));
#else
  BX_INFO(("CMPPS_VpsWpsIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSD_VsdWsdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPSD_VsdWsdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CMPSD_VsdWsdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPPD_VpdWpdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("CMPPD_VpdWpdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
