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
  BX_INFO(("SSE instruction set not supported in current configuration"));
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
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FXSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("SSE Instruction FXSAVE still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::FXRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("SSE Instruction FXRSTOR still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVUPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVUPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVUPS_WpsVps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_WssVss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVSS_WssVss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVLPS_VpsMq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVLPS_MqVps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction UNPCKLPS_VpsWq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction UNPCKHPS_VpsWq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVHPS_VpsMq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVHPS_MqVps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVAPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVAPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVAPS_WpsVps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPI2PS_VpsQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTPI2PS_VpsQq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSI2SS_VssEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTSI2SS_VssEd still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTPS_MdqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVNTPS_MdqVps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTTPS2PI_PqWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTTSS2SI_GdWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTPS2PI_PqWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTPS2PI_PqWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CVTSS2SI_GdWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CVTSS2SI_GdWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UCOMISS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction UCOMISS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::COMISS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction COMISS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPS_GdVRps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MOVMSKPS_GdVRps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction SQRTPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction SQRTSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction RSQRTPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RSQRTSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction RSQRTSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction RCPPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::RCPSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction RCPSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction ANDPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ANDNPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction ANDNPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ORPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction ORPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::XORPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction XORPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction ADDPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::ADDSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction ADDSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MULPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MULSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MULSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction SUBPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SUBSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction SUBSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MINPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MINSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MINSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction DIVPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::DIVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction DIVSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MAXPS_VpsWps still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MAXSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction MAXSS_VssWss still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PSHUFLW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction PSHUFLW_VqWqIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CMPPS_VpsWpsIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMPSS_VssWssIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction CMPSS_VssWssIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PINSRW_PqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction PINSRW_PqEdIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PEXTRW_PqEdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction PEXTRW_PqEdIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SHUFPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction SHUFPS_VpsWpsIb still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::PMOVMSKB_GdPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SSE Instruction PMOVMSKB_GdPRq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVNTQ_MqPq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("SSE Instruction MOVNTQ_MqPq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVQ_PqPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE
  BX_PANIC(("SSE Instruction MASKMOVQ_PqPRq still not implemented"));
#else
  BX_INFO(("SSE instruction set not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
