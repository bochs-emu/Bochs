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

#if BX_SUPPORT_SSE

void BX_CPU_C::prepareSSE(void)
{
  if(BX_CPU_THIS_PTR cr0.ts)
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.em)
    exception(BX_UD_EXCEPTION, 0, 0);

  if(! (BX_CPU_THIS_PTR cr4.get_OSXMMEXCPT()))
    exception(BX_UD_EXCEPTION, 0, 0);
}

#define BX_MXCSR_REGISTER (BX_CPU_THIS_PTR mxcsr.mxcsr)

#endif

/* 0F AE Grp15 010 */
void BX_CPU_C::LDMXCSR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  read_virtual_dword(i->seg(), RMAddr(i), &BX_MXCSR_REGISTER);
  if(BX_MXCSR_REGISTER & ~MXCSR_MASK)
      exception(BX_GP_EXCEPTION, 0, 0);
#else
  BX_INFO(("LDMXCSR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 011 */
void BX_CPU_C::STMXCSR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u mxcsr = BX_MXCSR_REGISTER & MXCSR_MASK;
  write_virtual_dword(i->seg(), RMAddr(i), &mxcsr);
#else
  BX_INFO(("STMXCSR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FXSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("FXSAVE: SSE instruction still not implemented"));
#else
  BX_INFO(("FXSAVE: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FXRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("FXRSTOR : SSE instruction still not implemented"));
#else
  BX_INFO(("FXRSTOR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVUPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVUPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVUPS_WpsVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVUPS_WpsVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_WssVss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSS_WssVss: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVSS_WssVss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPS_VpsMq: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVLPS_VpsMq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPS_MqVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVLPS_MqVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKLPS_VpsWq: SSE instruction still not implemented"));
#else
  BX_INFO(("UNPCKLPS_VpsWq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKHPS_VpsWq: SSE instruction still not implemented"));
#else
  BX_INFO(("UNPCKHPS_VpsWq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPS_VpsMq: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVHPS_VpsMq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPS_MqVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVHPS_MqVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVAPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVAPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVAPS_WpsVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVAPS_WpsVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PS_VpsQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPI2PS_VpsQq: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTPI2PS_VpsQq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSI2SS_VssEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSI2SS_VssEd: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTSI2SS_VssEd: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTPS_MdqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVNTPS_MdqVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVNTPS_MdqVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTPS2PI_PqWps: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTTPS2PI_PqWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTTSS2SI_GdWss: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTTSS2SI_GdWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTPS2PI_PqWps: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTPS2PI_PqWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CVTSS2SI_GdWss: SSE instruction still not implemented"));
#else
  BX_INFO(("CVTSS2SI_GdWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UCOMISS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("UCOMISS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("COMISS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("COMISS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPS_GdVRps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVMSKPS_GdVRps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVMSKPS_GdVRps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("SQRTPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SQRTSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("SQRTSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RSQRTPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("RSQRTPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RSQRTSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("RSQRTSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RCPPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("RCPPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("RCPSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("RCPSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ANDPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("ANDPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDNPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ANDNPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("ANDNPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ORPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ORPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("ORPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XORPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("XORPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("XORPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("ADDPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("ADDSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("ADDSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MULPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MULSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MULSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("SUBPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SUBSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("SUBSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MINPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MINSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MINSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("DIVPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("DIVSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("DIVSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXPS_VpsWps: SSE instruction still not implemented"));
#else
  BX_INFO(("MAXPS_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MAXSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MAXSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFLW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PSHUFLW_VqWqIb: SSE instruction still not implemented"));
#else
  BX_INFO(("PSHUFLW_VqWqIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPPS_VpsWpsIb: SSE instruction still not implemented"));
#else
  BX_INFO(("CMPPS_VpsWpsIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSS_VssWssIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("CMPSS_VssWssIb: SSE instruction still not implemented"));
#else
  BX_INFO(("CMPSS_VssWssIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PINSRW_PqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareMMX();

  BX_PANIC(("PINSRW_PqEdIb: SSE instruction still not implemented"));
#else
  BX_INFO(("PINSRW_PqEdIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PEXTRW_PqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareMMX();

  BX_PANIC(("PEXTRW_PqEdIb: SSE instruction still not implemented"));
#else
  BX_INFO(("PEXTRW_PqEdIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SHUFPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SHUFPS_VpsWpsIb: SSE instruction still not implemented"));
#else
  BX_INFO(("SHUFPS_VpsWpsIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMOVMSKB_GdPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("PMOVMSKB_GdPRq: SSE instruction still not implemented"));
#else
  BX_INFO(("PMOVMSKB_GdPRq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTQ_MqPq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("MOVNTQ_MqPq: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVNTQ_MqPq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVQ_PqPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("MASKMOVQ_PqPRq: SSE instruction still not implemented"));
#else
  BX_INFO(("MASKMOVQ_PqPRq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
