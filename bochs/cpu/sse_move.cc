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

/* *************************** */
/* SSE: MEMORY MOVE OPERATIONS */
/* *************************** */

/* MOVUPS:    0F 10 */
/* MOVUPD: 66 0F 10 */
/* MOVDQU: F3 0F 6F */

void BX_CPU_C::MOVUPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVUPS/PD/MOVDQU_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* MOVUPS:    0F 11 */
/* MOVUPD: 66 0F 11 */
/* MOVDQU: F3 0F 7F */

void BX_CPU_C::MOVUPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());

  /* op is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_XMM_REG(i->rm(), op);
  }
  else {
    writeVirtualDQword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }
#else
  BX_INFO(("MOVUPS/PD/MOVDQU_WpsVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* MOVAPS:    0F 28 */
/* MOVAPD: 66 0F 28 */
/* MOVDQA: F3 0F 6F */

void BX_CPU_C::MOVAPS_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
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
  BX_INFO(("MOVAPS/PD/MOVDQA_VpsWps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* MOVAPS:    0F 29 */
/* MOVAPD: 66 0F 29 */
/* MOVDQA: F3 0F 7F */

void BX_CPU_C::MOVAPS_WpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());

  /* op is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_XMM_REG(i->rm(), op);
  }
  else {
    writeVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }
#else
  BX_INFO(("MOVAPS/PD/MOVDQA_WpsVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSS_VssWss: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVSS_VssWss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSS_WssVss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSS_WssVss: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVSS_WssVss: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSD_VsdWsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVSD_VsdWsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVSD_WsdVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVSD_WsdVsd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVSD_WsdVsd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPS_VpsMq: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVLPS_VpsMq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVLPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVLPS_MqVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVLPS_MqVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPS_VpsMq: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVHPS_VpsMq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVHPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVHPS_MqVps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVHPS_MqVps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVQ_PqPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_PANIC(("MASKMOVQ_PqPRq: SSE instruction still not implemented"));
#else
  BX_INFO(("MASKMOVQ_PqPRq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPS_GdVRps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVMSKPS_GdVRps: SSE instruction still not implemented"));
#else
  BX_INFO(("MOVMSKPS_GdVRps: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVMSKPD_EdVRpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVMSKPD_EdVRpd: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVMSKPD_EdVRpd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6E */
void BX_CPU_C::MOVD_VdqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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

/* 66 0F 7E */
void BX_CPU_C::MOVD_EdVd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVQ_VqWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVQ_VqWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MOVQ_WqVq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MOVQ_WqVq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MOVQ_WqVq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::MASKMOVDQU_VdqVRdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("MASKMOVDQU_VdqVRdq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("MASKMOVDQU_VdqVRdq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F D6 */
void BX_CPU_C::MOVDQ2Q_PqVRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  BxPackedMmxRegister mm;
  MMXUQ(mm) = op.xmm64u(0);

  FPU_TWD  = 0;
  FPU_TOS  = 0;        /* Each time an MMX instruction is */
  FPU_SWD &= 0xc7ff;   /*      executed, the TOS value is set to 000b */

  BX_WRITE_MMX_REG(i->rm(), mm);
#else
  BX_INFO(("MOVDQ2Q_PqVRq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F D6 */
void BX_CPU_C::MOVQ2DQ_VdqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;
  BxPackedMmxRegister mm = BX_READ_MMX_REG(i->nnn());
  op.xmm64u(0) = MMXUQ(mm);
  op.xmm64u(1) = 0;

  FPU_TWD  = 0;
  FPU_TOS  = 0;        /* Each time an MMX instruction is */
  FPU_SWD &= 0xc7ff;   /*      executed, the TOS value is set to 000b */

  BX_WRITE_XMM_REG(i->rm(), op);
#else
  BX_INFO(("MOVQ2DQ_VdqQq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* ****************************** */
/* SSE: MEMORY SHUFFLE OPERATIONS */
/* ****************************** */

void BX_CPU_C::SHUFPS_VpsWpsIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SHUFPS_VpsWpsIb: SSE instruction still not implemented"));
#else
  BX_INFO(("SHUFPS_VpsWpsIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::SHUFPD_VpdWpdIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("SHUFPD_VpdWpdIb: SSE2 instruction still not implemented"));
#else
  BX_INFO(("SHUFPD_VpdWpdIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKLPS_VpsWq: SSE instruction still not implemented"));
#else
  BX_INFO(("UNPCKLPS_VpsWq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPS_VpsWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKHPS_VpsWq: SSE instruction still not implemented"));
#else
  BX_INFO(("UNPCKHPS_VpsWq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKLPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKLPD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UNPCKLPD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::UNPCKHPD_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BX_PANIC(("UNPCKHPD_VpdWq: SSE2 instruction still not implemented"));
#else
  BX_INFO(("UNPCKHPD_VpdWq: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 60 */
void BX_CPU_C::PUNPCKLBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
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

/* 66 0F 68 */
void BX_CPU_C::PUNPCKHBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
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

/* 66 0F 6C */
void BX_CPU_C::PUNPCKLQDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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
#if BX_SUPPORT_SSE >= 2
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

/* 66 0F 70 */
void BX_CPU_C::PSHUFD_VdqWdqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;
  Bit8u order = i->Ib();

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  result.xmm32u(0) = op.xmm32u((order >> 0) & 0x3);
  result.xmm32u(1) = op.xmm32u((order >> 2) & 0x3);
  result.xmm32u(2) = op.xmm32u((order >> 4) & 0x3);
  result.xmm32u(3) = op.xmm32u((order >> 6) & 0x3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSHUFD_VdqWdqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F 70 */
void BX_CPU_C::PSHUFHW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;
  Bit8u order = i->Ib();

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  result.xmm64u(0) = op.xmm64u(0);
  result.xmm16u(4) = op.xmm16u(4 + ((order >> 0) & 0x3));
  result.xmm16u(5) = op.xmm16u(4 + ((order >> 2) & 0x3));
  result.xmm16u(6) = op.xmm16u(4 + ((order >> 4) & 0x3));
  result.xmm16u(7) = op.xmm16u(4 + ((order >> 6) & 0x3));

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSHUFHW_VqWqIb: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F 70 */
void BX_CPU_C::PSHUFLW_VqWqIb(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op, result;
  Bit8u order = i->Ib();

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  result.xmm16u(0) = op.xmm16u((order >> 0) & 0x3);
  result.xmm16u(1) = op.xmm16u((order >> 2) & 0x3);
  result.xmm16u(2) = op.xmm16u((order >> 4) & 0x3);
  result.xmm16u(3) = op.xmm16u((order >> 6) & 0x3);
  result.xmm64u(1) = op.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PSHUFLW_VqWqIb: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* **************************** */
/* SSE: STORE DATA NON-TEMPORAL */
/* **************************** */

/* 0F C3 */
void BX_CPU_C::MOVNTI_MdGd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2

  if (i->modC0()) {
    BX_INFO(("MOVNTI_MdGd: must be memory reference"));
    UndefinedOpcode(i);
  }

  Bit32u val32 = BX_READ_32BIT_REG(i->nnn());
  write_virtual_dword(i->seg(), RMAddr(i), &val32);

#else
  BX_INFO(("MOVNTI_MdGd: SSE2 not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* MOVNTPS:    0F 2B */
/* MOVNTPD: 66 0F 2B */
/* MOVNTDQ: 66 0F E7 */

void BX_CPU_C::MOVNTPS_MdqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  if (i->modC0()) {
    BX_INFO(("MOVNTPS_MdqVps: must be memory reference"));
    UndefinedOpcode(i);
  }

  BxPackedXmmRegister val128 = BX_READ_XMM_REG(i->nnn());
  writeVirtualDQword(i->seg(), RMAddr(i), (Bit8u *)(&val128));

#else
  BX_INFO(("MOVNTPS/PD/MOVNTDQ_MdqVdq: SSE not supported in current configuration"));
  UndefinedOpcode(i);                      
#endif
}
