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

#if BX_SUPPORT_SSE >= 1

void BX_CPU_C::prepareSSE(void)
{
  if(BX_CPU_THIS_PTR cr0.ts)
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.em)
    exception(BX_UD_EXCEPTION, 0, 0);

  if(! (BX_CPU_THIS_PTR cr4.get_OSFXSR()))
    exception(BX_UD_EXCEPTION, 0, 0);
}

#define BX_MXCSR_REGISTER (BX_CPU_THIS_PTR mxcsr.mxcsr)

#endif

/* 0F AE Grp15 010 */
void BX_CPU_C::LDMXCSR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u new_mxcsr;

  read_virtual_dword(i->seg(), RMAddr(i), &new_mxcsr);
  if(new_mxcsr & ~MXCSR_MASK)
      exception(BX_GP_EXCEPTION, 0, 0);

  BX_MXCSR_REGISTER = new_mxcsr;
#else
  BX_INFO(("LDMXCSR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 011 */
void BX_CPU_C::STMXCSR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u mxcsr = BX_MXCSR_REGISTER & MXCSR_MASK;
  write_virtual_dword(i->seg(), RMAddr(i), &mxcsr);
#else
  BX_INFO(("STMXCSR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 000 */
void BX_CPU_C::FXSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BxPackedXmmRegister xmm;
  Bit16u twd = BX_CPU_THIS_PTR the_i387.soft.twd, tbd = 0;
  unsigned index;

  xmm.xmm16u(0) = BX_CPU_THIS_PTR the_i387.soft.cwd;
  xmm.xmm16u(1) = BX_CPU_THIS_PTR the_i387.soft.swd;

  if(twd & 0x0003 == 0x0003) tbd |= 0x0100;
  if(twd & 0x000c == 0x000c) tbd |= 0x0200;
  if(twd & 0x0030 == 0x0030) tbd |= 0x0400;
  if(twd & 0x00c0 == 0x00c0) tbd |= 0x0800;
  if(twd & 0x0300 == 0x0300) tbd |= 0x1000;
  if(twd & 0x0c00 == 0x0c00) tbd |= 0x2000;
  if(twd & 0x3000 == 0x3000) tbd |= 0x4000;
  if(twd & 0xc000 == 0xc000) tbd |= 0x8000;

  xmm.xmm16u(2) = tbd;

  /* x87 FPU Opcode (16 bits) */
  /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
  xmm.xmm16u(3) = 0;  /* still not implemented */

  /* 
   * x87 FPU IP Offset (32 bits)
   * The contents of this field differ depending on the current 
   * addressing mode (16/32 bit) when the FXSAVE instruction was executed:
   *   + 32-bit mode-32-bit IP offset
   *   + 16-bit mode-low 16 bits are IP offset; high 16 bits are reserved.
   *
   * x87 CS FPU IP Selector (16 bits)
   */
  xmm.xmm64u(1) = 0;  /* still not implemented */
 
  writeVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &xmm);

  /* 
   * x87 FPU Instruction Operand (Data) Pointer Offset (32 bits)
   * The contents of this field differ depending on the current 
   * addressing mode (16/32 bit) when the FXSAVE instruction was executed:
   *   + 32-bit mode-32-bit offset
   *   + 16-bit mode-low 16 bits are offset; high 16 bits are reserved.
   *
   * x87 DS FPU Instruction Operand (Data) Pointer Selector (16 bits)
   */
  xmm.xmm64u(0) = 0;  /* still not implemented */

  xmm.xmm32u(2) = BX_MXCSR_REGISTER;
  xmm.xmm32u(3) = MXCSR_MASK;

  writeVirtualDQwordAligned(i->seg(), RMAddr(i) + 16, (Bit8u *) &xmm);

  /* store i387 register file */
  for(index=0; index < 8; index++)
  {
    Bit8u *r387 = (Bit8u *) &(BX_CPU_THIS_PTR the_i387.soft.st_space[index]);
    writeVirtualDQwordAligned(i->seg(), RMAddr(i)+index*16+32, r387);
  }

  /* store XMM register file */
  for(index=0; index < BX_XMM_REGISTERS; index++)
  {
    Bit8u *r128 = (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]);
    writeVirtualDQwordAligned(i->seg(), RMAddr(i)+index*16+160, r128);
  }

  /* do not touch reserved fields */
#else
  BX_INFO(("FXSAVE: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 001 */
void BX_CPU_C::FXRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BxPackedXmmRegister xmm;
  Bit32u tbd, twd = 0;
  unsigned index;

  readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &xmm);
  
  BX_CPU_THIS_PTR the_i387.soft.cwd = xmm.xmm16u(0);
  BX_CPU_THIS_PTR the_i387.soft.swd = xmm.xmm16u(1);

  /* TOS restore still not implemented */
  /* FOO/FPU IP restore still not implemented */

  /* 
   * Note that the original format for FTW can be recreated from the stored 
   * FTW valid bits and the stored 80-bit FP data (assuming the stored data 
   * was not the contents of MMX registers) using the following table:
    
     | Exponent | Exponent | Fraction | J,M bits | FTW valid | x87 FTW |
     |  all 1s  |  all 0s  |  all 0s  |          |           |         |
     -------------------------------------------------------------------
     |    0     |    0     |    0     |    0x    |     1     | S    10 |
     |    0     |    0     |    0     |    1x    |     1     | V    00 |
     -------------------------------------------------------------------
     |    0     |    0     |    1     |    00    |     1     | S    10 |
     |    0     |    0     |    1     |    10    |     1     | V    00 |
     -------------------------------------------------------------------
     |    0     |    1     |    0     |    0x    |     1     | S    10 |
     |    0     |    1     |    0     |    1x    |     1     | V    10 |
     -------------------------------------------------------------------
     |    0     |    1     |    1     |    00    |     1     | S    01 |
     |    0     |    1     |    1     |    10    |     1     | V    10 |
     -------------------------------------------------------------------
     |    1     |    0     |    0     |    1x    |     1     | S    10 |
     |    1     |    0     |    0     |    1x    |     1     | V    10 |
     -------------------------------------------------------------------
     |    1     |    0     |    1     |    00    |     1     | S    10 |
     |    1     |    0     |    1     |    10    |     1     | V    10 |
     -------------------------------------------------------------------
     |        all combinations above             |     1     | E    11 |

   *
   * The J-bit is defined to be the 1-bit binary integer to the left 
   * of the decimal place in the significand.
   * 
   * The M-bit is defined to be the most significant bit of the fractional 
   * portion of the significand (i.e., the bit immediately to the right of 
   * the decimal place). When the M-bit is the most significant bit of the 
   * fractional portion  of the significand, it must be  0 if the fraction 
   * is all 0's.  
   */                    /* still not implemented */

  tbd = xmm.xmm16u(2);
  if(tbd & 0x0100) twd |= 0x0003;
  if(tbd & 0x0200) twd |= 0x000c;
  if(tbd & 0x0400) twd |= 0x0030;
  if(tbd & 0x0800) twd |= 0x00c0;
  if(tbd & 0x1000) twd |= 0x0300;
  if(tbd & 0x2000) twd |= 0x0c00;
  if(tbd & 0x4000) twd |= 0x3000;
  if(tbd & 0x8000) twd |= 0xc000;
  BX_CPU_THIS_PTR the_i387.soft.twd = twd;

  readVirtualDQwordAligned(i->seg(), RMAddr(i) + 16, (Bit8u *) &xmm);

  /* FPU DP restore still not implemented */

  Bit32u new_mxcsr = xmm.xmm32u(2);
  Bit32u mxcsr_msk = xmm.xmm32u(3);

  if(! mxcsr_msk) mxcsr_msk = MXCSR_MASK; 

  if(new_mxcsr & ~mxcsr_msk)
      exception(BX_GP_EXCEPTION, 0, 0);

  BX_MXCSR_REGISTER = new_mxcsr;

  /* load i387 register file */
  for(index=0; index < 8; index++)
  {
    Bit8u *r387 = (Bit8u *) &(BX_CPU_THIS_PTR the_i387.soft.st_space[index]);
    readVirtualDQwordAligned(i->seg(), RMAddr(i)+index*16+32, r387);
  }

  /* load XMM register file */
  for(index=0; index < BX_XMM_REGISTERS; index++)
  {
    Bit8u *r128 = (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]);
    readVirtualDQwordAligned(i->seg(), RMAddr(i)+index*16+160, r128);
  }

#else
  BX_INFO(("FXRSTOR: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

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
  BX_INFO(("MOVUPS/MOVUPD/MOVDQU_VpsWps: SSE not supported in current configuration"));
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
  BX_INFO(("MOVUPS/MOVUPD/MOVDQU_WpsVps: SSE not supported in current configuration"));
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
  BX_INFO(("MOVAPS/MOVAPD/MOVDQA_VpsWps: SSE not supported in current configuration"));
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
  BX_INFO(("MOVAPS/MOVAPD/MOVDQA_WpsVps: SSE not supported in current configuration"));
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

/* 0F 2B */
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
  BX_INFO(("MOVNTPS/MOVNTPD_MdqVps: SSE not supported in current configuration"));
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

void BX_CPU_C::MASKMOVQ_PqPRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_PANIC(("MASKMOVQ_PqPRq: SSE instruction still not implemented"));
#else
  BX_INFO(("MASKMOVQ_PqPRq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* ANDPS:    0F 54 */
/* ANDPD: 66 0F 54 */
/* PAND:  F3 0F DB */
void BX_CPU_C::PAND_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
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
  BX_INFO(("ANDPS/ANDPD/PAND_VdqWdq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* ANDNPS:    0F 55 */
/* ANDNPD: 66 0F 55 */
/* PANDN:  F3 0F DF */
void BX_CPU_C::PANDN_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
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

  result.xmm64u(0) = ~(op1.xmm64u(0)) & op2.xmm64u(0);
  result.xmm64u(1) = ~(op1.xmm64u(1)) & op2.xmm64u(1);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("ANDNPS/ANDNPD/PANDN_VdqWdq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* ORPS:    0F 56 */
/* ORPD: 66 0F 56 */
/* POR:  F3 0F EB */
void BX_CPU_C::POR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
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
  BX_INFO(("ORPS/ORPD/POR_VdqWdq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}

/* XORPS:    0F 57 */
/* XORPD: 66 0F 57 */
/* PXOR:  F3 0F EF */
void BX_CPU_C::PXOR_VdqWdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
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
  BX_INFO(("XORPS/XORPD/PXOR_VdqWdq: SSE not supported in current configuration"));
  UndefinedOpcode(i);
#endif
}
