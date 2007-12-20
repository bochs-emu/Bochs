/////////////////////////////////////////////////////////////////////////
// $Id: sse_move.cc,v 1.71 2007-12-20 18:29:38 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2003 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_SSE

void BX_CPU_C::prepareSSE(void)
{
  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0, 0);

  if(! (BX_CPU_THIS_PTR cr4.get_OSFXSR()))
    exception(BX_UD_EXCEPTION, 0, 0);
}

#define BX_MXCSR_REGISTER (BX_CPU_THIS_PTR mxcsr.mxcsr)

void BX_CPU_C::print_state_SSE(void)
{
  BX_DEBUG(("MXCSR: 0x%08x\n", BX_MXCSR_REGISTER));
  for(unsigned i=0;i<BX_XMM_REGISTERS;i++) {
    BxPackedXmmRegister xmm = BX_READ_XMM_REG(i);
    BX_DEBUG(("XMM%02u: %08x%08x:%08x%08x\n", i,
       xmm.xmm32u(3), xmm.xmm32u(2), xmm.xmm32u(1), xmm.xmm32u(0)));
  }
}

#endif

/* ************************************ */
/* SSE: SAVE/RESTORE FPU/MMX/SSEx STATE */
/* ************************************ */

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
  BX_INFO(("LDMXCSR: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 011 */
void BX_CPU_C::STMXCSR(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u mxcsr = BX_MXCSR_REGISTER & MXCSR_MASK;
  write_virtual_dword(i->seg(), RMAddr(i), mxcsr);
#else
  BX_INFO(("STMXCSR: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 000 */
void BX_CPU_C::FXSAVE(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_DEBUG(("FXSAVE: save FPU/MMX/SSE state"));

#if BX_SUPPORT_MMX
  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0, 0);
#endif

  xmm.xmm16u(0) = BX_CPU_THIS_PTR the_i387.get_control_word();
  xmm.xmm16u(1) = BX_CPU_THIS_PTR the_i387.get_status_word ();

  Bit16u twd = BX_CPU_THIS_PTR the_i387.get_tag_word(), tag_byte = 0;

  if((twd & 0x0003) != 0x0003) tag_byte |= 0x01;
  if((twd & 0x000c) != 0x000c) tag_byte |= 0x02;
  if((twd & 0x0030) != 0x0030) tag_byte |= 0x04;
  if((twd & 0x00c0) != 0x00c0) tag_byte |= 0x08;
  if((twd & 0x0300) != 0x0300) tag_byte |= 0x10;
  if((twd & 0x0c00) != 0x0c00) tag_byte |= 0x20;
  if((twd & 0x3000) != 0x3000) tag_byte |= 0x40;
  if((twd & 0xc000) != 0xc000) tag_byte |= 0x80;

  xmm.xmm16u(2) = tag_byte;

  /* x87 FPU Opcode (16 bits) */
  /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
  xmm.xmm16u(3) = BX_CPU_THIS_PTR the_i387.foo;

  /* 
   * x87 FPU IP Offset (32/64 bits)
   * The contents of this field differ depending on the current 
   * addressing mode (16/32/64 bit) when the FXSAVE instruction was executed:
   *   + 64-bit mode - 64-bit IP offset
   *   + 32-bit mode - 32-bit IP offset
   *   + 16-bit mode - low 16 bits are IP offset; high 16 bits are reserved.
   * x87 CS FPU IP Selector
   *   + 16 bit, in 16/32 bit mode only
   */
#if BX_SUPPORT_X86_64
  if (i->os64L()) /* 64 bit operand size mode */
  {
    xmm.xmm64u(1) = (BX_CPU_THIS_PTR the_i387.fip);
  }
  else
#endif
  {
    xmm.xmm32u(2) = (BX_CPU_THIS_PTR the_i387.fip) & 0xffffffff;
    xmm.xmm32u(3) = (BX_CPU_THIS_PTR the_i387.fcs);
  }
 
  write_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *) &xmm);

  /* 
   * x87 FPU Instruction Operand (Data) Pointer Offset (32/64 bits)
   * The contents of this field differ depending on the current 
   * addressing mode (16/32 bit) when the FXSAVE instruction was executed:
   *   + 64-bit mode - 64-bit offset
   *   + 32-bit mode - 32-bit offset
   *   + 16-bit mode - low 16 bits are offset; high 16 bits are reserved.
   * x87 DS FPU Instruction Operand (Data) Pointer Selector 
   *   + 16 bit, in 16/32 bit mode only
   */
#if BX_SUPPORT_X86_64
  if (i->os64L()) /* 64 bit operand size mode */
  {
    xmm.xmm64u(0) = (BX_CPU_THIS_PTR the_i387.fdp);
  }
  else
#endif
  {
    xmm.xmm32u(0) = (BX_CPU_THIS_PTR the_i387.fdp) & 0xffffffff;
    xmm.xmm32u(1) = (BX_CPU_THIS_PTR the_i387.fds);
  }

#if BX_SUPPORT_SSE >= 1
  xmm.xmm32u(2) = BX_MXCSR_REGISTER;
  xmm.xmm32u(3) = MXCSR_MASK;
#else
  xmm.xmm32u(2) = 0;
  xmm.xmm32u(3) = 0;
#endif

  write_virtual_dqword_aligned(i->seg(), RMAddr(i) + 16, (Bit8u *) &xmm);

  /* store i387 register file */
  for(index=0; index < 8; index++)
  {
    const floatx80 &fp = BX_FPU_REG(index);

    xmm.xmm64u(0) = fp.fraction;
    xmm.xmm64u(1) = 0;
    xmm.xmm16u(4) = fp.exp;
    
    write_virtual_dqword_aligned(i->seg(), RMAddr(i)+index*16+32, (Bit8u *) &xmm);
  }

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.ffxsr && CPL == 0 && Is64BitMode())
    return; // skip saving of the XMM state
#endif

#if BX_SUPPORT_SSE >= 1
  /* store XMM register file */
  for(index=0; index < BX_XMM_REGISTERS; index++)
  {
    // save XMM8-XMM15 only in 64-bit mode
    if (index < 8 || Is64BitMode()) {
       write_virtual_dqword_aligned(i->seg(), 
           RMAddr(i)+index*16+160, (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]));
    }
  }
#endif

  /* do not touch reserved fields */
#else
  BX_INFO(("FXSAVE: required P6 support, use --enable-cpu-level=6 option"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE Grp15 001 */
void BX_CPU_C::FXRSTOR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister xmm;
  int index;

  BX_DEBUG(("FXRSTOR: restore FPU/MMX/SSE state"));

#if BX_SUPPORT_MMX
  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_EM())
    exception(BX_UD_EXCEPTION, 0, 0);
#endif

  read_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *) &xmm);
  
  BX_CPU_THIS_PTR the_i387.cwd = xmm.xmm16u(0);
  BX_CPU_THIS_PTR the_i387.swd = xmm.xmm16u(1);
  BX_CPU_THIS_PTR the_i387.tos = (xmm.xmm16u(1) >> 11) & 0x07;

  /* Restore x87 FPU Opcode */
  /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
  BX_CPU_THIS_PTR the_i387.foo = xmm.xmm16u(3) & 0x7FF;

  /* Restore x87 FPU IP */
#if BX_SUPPORT_X86_64
  if (i->os64L()) /* 64 bit operand size mode */
  {
    BX_CPU_THIS_PTR the_i387.fip = xmm.xmm64u(1);
  }
  else
#endif
  {
    BX_CPU_THIS_PTR the_i387.fip = xmm.xmm32u(2);
    BX_CPU_THIS_PTR the_i387.fcs = xmm.xmm16u(5);
  }

  Bit32u twd = 0, tag_byte = xmm.xmm16u(2);

  /* Restore x87 FPU DP */
  read_virtual_dqword_aligned(i->seg(), RMAddr(i) + 16, (Bit8u *) &xmm);

#if BX_SUPPORT_X86_64
  if (i->os64L()) /* 64 bit operand size mode */
  {
    BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm64u(0);
  }
  else
#endif
  {
    BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm32u(0);
    BX_CPU_THIS_PTR the_i387.fds = xmm.xmm16u(2);
  }

#if BX_SUPPORT_SSE >= 1
  /* If the OSFXSR bit in CR4 is not set, the FXRSTOR instruction does
     not restore the states of the XMM and MXCSR registers. */
  if(BX_CPU_THIS_PTR cr4.get_OSFXSR())
  {
    Bit32u new_mxcsr = xmm.xmm32u(2);
    if(new_mxcsr & ~MXCSR_MASK)
       exception(BX_GP_EXCEPTION, 0, 0);

    BX_MXCSR_REGISTER = new_mxcsr;
  }
#endif

  /* load i387 register file */
  for(index=0; index < 8; index++)
  {
    read_virtual_tword(i->seg(), RMAddr(i)+index*16+32, &(BX_FPU_REG(index)));
  }

  /*                                 FTW
   *
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
     |    0     |    1     |    0     |    1x    |     1     | S    10 |
     -------------------------------------------------------------------
     |    0     |    1     |    1     |    00    |     1     | Z    01 |
     |    0     |    1     |    1     |    10    |     1     | S    10 |
     -------------------------------------------------------------------
     |    1     |    0     |    0     |    1x    |     1     | S    10 |
     |    1     |    0     |    0     |    1x    |     1     | S    10 |
     -------------------------------------------------------------------
     |    1     |    0     |    1     |    00    |     1     | S    10 |
     |    1     |    0     |    1     |    10    |     1     | S    10 |
     -------------------------------------------------------------------
     |        all combinations above             |     0     | E    11 |

   *
   * The J-bit is defined to be the 1-bit binary integer to the left of
   * the decimal place in the significand.
   * 
   * The M-bit is defined to be the most significant bit of the fractional 
   * portion of the significand (i.e., the bit immediately to the right of 
   * the decimal place). When the M-bit is the most significant bit of the 
   * fractional portion  of the significand, it must be  0 if the fraction 
   * is all 0's.  
   */

  for(index = 7;index >= 0; index--, twd <<= 2, tag_byte <<= 1)
  {
      if(tag_byte & 0x80) {
         const floatx80 &fpu_reg = BX_FPU_REG(index);
         twd |= FPU_tagof(fpu_reg);
      }
      else {
         twd |= FPU_Tag_Empty;
      }
  }

  BX_CPU_THIS_PTR the_i387.twd = (twd >> 2);

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.ffxsr && CPL == 0 && Is64BitMode())
    return; // skip restore of the XMM state
#endif

#if BX_SUPPORT_SSE >= 1
  /* If the OSFXSR bit in CR4 is not set, the FXRSTOR instruction does
     not restore the states of the XMM and MXCSR registers. */
  if(BX_CPU_THIS_PTR cr4.get_OSFXSR())
  {
    /* load XMM register file */
    for(index=0; index < BX_XMM_REGISTERS; index++)
    {
      // restore XMM8-XMM15 only in 64-bit mode
      if (index < 8 || Is64BitMode()) {
         read_virtual_dqword_aligned(i->seg(),
             RMAddr(i)+index*16+160, (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]));
      }
    }
  }
#endif

#else
  BX_INFO(("FXRSTOR: required P6 support, use --enable-cpu-level=6 option"));
  UndefinedOpcode(i);
#endif
}

/* *************************** */
/* SSE: MEMORY MOVE OPERATIONS */
/* *************************** */

/* All these opcodes never generate SIMD floating point exeptions */

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
    read_virtual_dqword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVUPS_VpsWps: required SSE, use --enable-sse option"));
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
    write_virtual_dqword(i->seg(), RMAddr(i), (Bit8u *) &op);
  }
#else
  BX_INFO(("MOVUPS_WpsVps: required SSE, use --enable-sse option"));
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
    read_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVAPS_VpsWps: required SSE, use --enable-sse option"));
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
    write_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }
#else
  BX_INFO(("MOVAPS_WpsVps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F 10 */
void BX_CPU_C::MOVSS_VssWss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u val32;

  /* op2 is a register or memory reference */
  if (i->modC0()) 
  {
    /* If the source operand is an XMM register, the high-order 
            96 bits of the destination XMM register are not modified. */
    op.xmm32u(0) = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);

    /* If the source operand is a memory location, the high-order
            96 bits of the destination XMM register are cleared to 0s */
    op.xmm32u(0) = val32;
    op.xmm32u(1) = 0;
    op.xmm64u(1) = 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVSS_VssWss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F 11 */
void BX_CPU_C::MOVSS_WssVss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->nnn());

  /* destination is a register or memory reference */
  if (i->modC0()) 
  {
    /* If the source operand is an XMM register, the high-order 
            96 bits of the destination XMM register are not modified. */
    BX_WRITE_XMM_REG_LO_DWORD(i->rm(), val32);
  }
  else {
    /* pointer, segment address pair */
    write_virtual_dword(i->seg(), RMAddr(i), val32);
  }
#else
  BX_INFO(("MOVSS_WssVss: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F 10 */
void BX_CPU_C::MOVSD_VsdWsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit64u val64;

  /* op2 is a register or memory reference */
  if (i->modC0()) 
  {
    /* If the source operand is an XMM register, the high-order 
            64 bits of the destination XMM register are not modified. */
    op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);

    /* If the source operand is a memory location, the high-order
            64 bits of the destination XMM register are cleared to 0s */
    op.xmm64u(0) = val64;
    op.xmm64u(1) = 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVSD_VsdWsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F 11 */
void BX_CPU_C::MOVSD_WsdVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Bit64u val64 = BX_READ_XMM_REG_LO_QWORD(i->nnn());

  /* destination is a register or memory reference */
  if (i->modC0()) 
  {
    /* If the source operand is an XMM register, the high-order 
            64 bits of the destination XMM register are not modified. */
    BX_WRITE_XMM_REG_LO_QWORD(i->rm(), val64);
  }
  else {
    /* pointer, segment address pair */
    write_virtual_qword(i->seg(), RMAddr(i), val64);
  }
#else
  BX_INFO(("MOVSD_WsdVsd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* MOVLPS:    0F 12 */
/* MOVLPD: 66 0F 12 */
void BX_CPU_C::MOVLPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  Bit64u val64;

  if (i->modC0()) /* MOVHLPS xmm1, xmm2 opcode */
  {
    val64 = BX_READ_XMM_REG_HI_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG_LO_QWORD(i->nnn(), val64);
#else
  BX_INFO(("MOVLPS_VpsMq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F 12 */
void BX_CPU_C::MOVDDUP_VpdWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 3
  BX_CPU_THIS_PTR prepareSSE();
  Bit64u val64;
  BxPackedXmmRegister op;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  op.xmm64u(0) = val64;
  op.xmm64u(1) = val64;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVDDUP_VpdWq: required SSE3, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F 12 */
void BX_CPU_C::MOVSLDUP_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 3
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  result.xmm32u(0) = op.xmm32u(0);
  result.xmm32u(1) = op.xmm32u(0);
  result.xmm32u(2) = op.xmm32u(2);
  result.xmm32u(3) = op.xmm32u(2);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("MOVSLDUP_VpsWps: required SSE3, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F 16 */
void BX_CPU_C::MOVSHDUP_VpsWps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 3
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister op, result;

  /* op is a register or memory reference */
  if (i->modC0()) {
    op = BX_READ_XMM_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    readVirtualDQwordAligned(i->seg(), RMAddr(i), (Bit8u *) &op);
  }

  result.xmm32u(0) = op.xmm32u(1);
  result.xmm32u(1) = op.xmm32u(1);
  result.xmm32u(2) = op.xmm32u(3);
  result.xmm32u(3) = op.xmm32u(3);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("MOVHLDUP_VpsWps: required SSE3, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* MOVLPS:    0F 13 */
/* MOVLPD: 66 0F 13 */
void BX_CPU_C::MOVLPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  write_virtual_qword(i->seg(), RMAddr(i), BX_XMM_REG_LO_QWORD(i->nnn()));
#else
  BX_INFO(("MOVLPS_MqVps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* MOVHPS:    0F 16 */
/* MOVHPD: 66 0F 16 */
void BX_CPU_C::MOVHPS_VpsMq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  Bit64u val64;

  if (i->modC0()) /* MOVLHPS xmm1, xmm2 opcode */
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG_HI_QWORD(i->nnn(), val64);
#else
  BX_INFO(("MOVHPS_VpsMq: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* MOVHPS:    0F 17 */
/* MOVHPD: 66 0F 17 */
void BX_CPU_C::MOVHPS_MqVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  write_virtual_qword(i->seg(), RMAddr(i), BX_XMM_REG_HI_QWORD(i->nnn()));
#else
  BX_INFO(("MOVHPS_MqVps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F F0 */
void BX_CPU_C::LDDQU_VdqMdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 3
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;
  read_virtual_dqword(i->seg(), RMAddr(i), (Bit8u *) &op);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);

#else
  BX_INFO(("LDDQU_VdqMdq: required SSE3, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F F7 */
void BX_CPU_C::MASKMOVDQU_VdqUdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  bx_address rdi;
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn()), 
    mask = BX_READ_XMM_REG(i->rm());

#if BX_SUPPORT_X86_64
  if (i->as64L()) { 	/* 64 bit address mode */
      rdi = RDI;
  } 
  else
#endif
  if (i->as32L()) {
      rdi = EDI;
  }
  else {                /* 16 bit address mode */
      rdi = DI;
  }

  /* partial write, no data will be written to memory if mask is all 0s */
  for(unsigned j=0; j<16; j++) 
  {
    if(mask.xmmubyte(j) & 0x80)
        write_virtual_byte(BX_SEG_REG_DS, rdi+j, op.xmmubyte(j));
  }

#else
  BX_INFO(("MASKMOVDQU_VdqUdq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 50 */
void BX_CPU_C::MOVMSKPS_GdVRps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u val32 = 0;

  if(op.xmm32u(0) & 0x80000000) val32 |= 0x1;
  if(op.xmm32u(1) & 0x80000000) val32 |= 0x2;
  if(op.xmm32u(2) & 0x80000000) val32 |= 0x4;
  if(op.xmm32u(3) & 0x80000000) val32 |= 0x8;

  BX_WRITE_32BIT_REGZ(i->rm(), val32);
#else
  BX_INFO(("MOVMSKPS_GdVRps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 50 */
void BX_CPU_C::MOVMSKPD_GdVRpd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());
  Bit32u val32 = 0;

  if(op.xmm32u(1) & 0x80000000) val32 |= 0x1;
  if(op.xmm32u(3) & 0x80000000) val32 |= 0x2;
 
  BX_WRITE_32BIT_REGZ(i->rm(), val32);
#else
  BX_INFO(("MOVMSKPD_GdVRpd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 6E */
void BX_CPU_C::MOVD_VdqEd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1;
  Bit32u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2);
  }

  op1.xmm64u(0) = (Bit64u)(op2);
  op1.xmm64u(1) = 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("MOVD_VdqEd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64

/* 66 0F 6E */
void BX_CPU_C::MOVQ_VdqEq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op1;
  Bit64u op2;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2);
  }

  op1.xmm64u(0) = op2;
  op1.xmm64u(1) = 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op1);
#else
  BX_INFO(("MOVQ_VdqEq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#endif

/* 66 0F 7E */
void BX_CPU_C::MOVD_EdVd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Bit32u op2 = BX_READ_XMM_REG_LO_DWORD(i->nnn());

  /* destination is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), op2);
  }
  else {
    /* pointer, segment address pair */
    write_virtual_dword(i->seg(), RMAddr(i), op2);
  }
#else
  BX_INFO(("MOVD_EdVd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64

/* 66 0F 7E */
void BX_CPU_C::MOVQ_EqVq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  Bit64u op2 = BX_READ_XMM_REG_LO_QWORD(i->nnn());

  /* destination is a register or memory reference */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op2);
  }
  else {
    /* pointer, segment address pair */
    write_virtual_qword(i->seg(), RMAddr(i), op2);
  }
#else
  BX_INFO(("MOVQ_EqVq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#endif

/* F3 0F 7E */
void BX_CPU_C::MOVQ_VqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;
  Bit64u val64;

  if (i->modC0()) {
    op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
    op.xmm64u(0) = val64;
  }

  /* zero-extension to 128 bit */
  op.xmm64u(1) = 0;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);
#else
  BX_INFO(("MOVQ_VqWq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D6 */
void BX_CPU_C::MOVQ_WqVq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op = BX_READ_XMM_REG(i->nnn());

  if (i->modC0()) 
  {
    op.xmm64u(1) = 0; /* zero-extension to 128 bits */
    BX_WRITE_XMM_REG(i->rm(), op);
  }
  else {
    write_virtual_qword(i->seg(), RMAddr(i), op.xmm64u(0));
  }
#else
  BX_INFO(("MOVQ_WqVq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F2 0F D6 */
void BX_CPU_C::MOVDQ2Q_PqVRq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedMmxRegister mm;
  MMXUQ(mm) = BX_READ_XMM_REG_LO_QWORD(i->nnn());

  BX_WRITE_MMX_REG(i->rm(), mm);
#else
  BX_INFO(("MOVDQ2Q_PqVRq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* F3 0F D6 */
void BX_CPU_C::MOVQ2DQ_VdqQq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  BX_CPU_THIS_PTR prepareSSE();
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedXmmRegister op;
  BxPackedMmxRegister mm = BX_READ_MMX_REG(i->nnn());

  op.xmm64u(0) = MMXUQ(mm);
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG(i->rm(), op);
#else
  BX_INFO(("MOVQ2DQ_VdqQq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F D7 */
void BX_CPU_C::PMOVMSKB_GdUdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
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
  BX_WRITE_32BIT_REGZ(i->nnn(), result);
  
#else
  BX_INFO(("PMOVMSKB_GdUdq: required SSE2, use --enable-sse option"));
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
  write_virtual_dword(i->seg(), RMAddr(i), BX_READ_32BIT_REG(i->nnn()));
#else
  BX_INFO(("MOVNTI_MdGd: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64 

/* 0F C3 */
void BX_CPU_C::MOVNTI_MqGq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 2
  write_virtual_qword(i->seg(), RMAddr(i), BX_READ_64BIT_REG(i->nnn()));
#else
  BX_INFO(("MOVNTI_MqGq: required SSE2, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

#endif

/* MOVNTPS:    0F 2B */
/* MOVNTPD: 66 0F 2B */
/* MOVNTDQ: 66 0F E7 */
void BX_CPU_C::MOVNTPS_MpsVps(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 1
  BX_CPU_THIS_PTR prepareSSE();
  write_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *)(&BX_READ_XMM_REG(i->nnn())));
#else
  BX_INFO(("MOVNTPS_MpsVps: required SSE, use --enable-sse option"));
  UndefinedOpcode(i);                      
#endif
}

/* MOVNTSD: F2 0F 2B */
void BX_CPU_C::MOVNTSD_MsdVsd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE4A
  BX_CPU_THIS_PTR prepareSSE();
  write_virtual_dword(i->seg(), RMAddr(i), BX_READ_XMM_REG_LO_QWORD(i->nnn()));
#else
  BX_INFO(("MOVNTSD_MsdVsd: required SSE4A, use --enable-sse4a option"));
  UndefinedOpcode(i);                      
#endif
}

/* MOVNTSS: F3 0F 2B */
void BX_CPU_C::MOVNTSS_MssVss(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE4A
  BX_CPU_THIS_PTR prepareSSE();
  write_virtual_dword(i->seg(), RMAddr(i), BX_READ_XMM_REG_LO_DWORD(i->nnn()));
#else
  BX_INFO(("MOVNTSS_MssVss: required SSE4A, use --enable-sse4a option"));
  UndefinedOpcode(i);                      
#endif
}

/* ************************** */
/* 3-BYTE-OPCODE INSTRUCTIONS */
/* ************************** */

#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

/* 66 0F 38 20 */
void BX_CPU_C::PMOVSXBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm16u(0) = (Bit8s) (val64 & 0xFF);
  result.xmm16u(1) = (Bit8s) ((val64 >>  8) & 0xFF);
  result.xmm16u(2) = (Bit8s) ((val64 >> 16) & 0xFF);
  result.xmm16u(3) = (Bit8s) ((val64 >> 24) & 0xFF);
  result.xmm16u(4) = (Bit8s) ((val64 >> 32) & 0xFF);
  result.xmm16u(5) = (Bit8s) ((val64 >> 40) & 0xFF);
  result.xmm16u(6) = (Bit8s) ((val64 >> 48) & 0xFF);
  result.xmm16u(7) = (Bit8s) (val64  >> 56);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXBW_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 21 */
void BX_CPU_C::PMOVSXBD_VdqWd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit32u val32;

  if (i->modC0())
  {
    val32 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);
  }

  result.xmm32u(0) = (Bit8s) (val32 & 0xFF);
  result.xmm32u(1) = (Bit8s) ((val32 >>  8) & 0xFF);
  result.xmm32u(2) = (Bit8s) ((val32 >> 16) & 0xFF);
  result.xmm32u(3) = (Bit8s) (val32  >> 24);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXBD_VdqWd: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 22 */
void BX_CPU_C::PMOVSXBQ_VdqWw(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit16u val16;

  if (i->modC0())
  {
    val16 = BX_READ_XMM_REG_LO_WORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &val16);
  }

  result.xmm64u(0) = (Bit8s) (val16 & 0xFF);
  result.xmm64u(1) = (Bit8s) (val16 >> 8);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXBQ_VdqWw: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 23 */
void BX_CPU_C::PMOVSXWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm32u(0) = (Bit16s) (val64 & 0xFFFF);
  result.xmm32u(1) = (Bit16s) ((val64 >> 16) & 0xFFFF);
  result.xmm32u(2) = (Bit16s) ((val64 >> 32) & 0xFFFF);
  result.xmm32u(3) = (Bit16s) (val64  >> 48);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXWD_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 24 */
void BX_CPU_C::PMOVSXWQ_VdqWd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit32u val32;

  if (i->modC0())
  {
    val32 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);
  }

  result.xmm64u(0) = (Bit16s) (val32 & 0xFFFF);
  result.xmm64u(1) = (Bit16s) (val32 >> 16);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXWQ_VdqWd: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 25 */
void BX_CPU_C::PMOVSXDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm64u(0) = (Bit32s) (val64 & 0xFFFFFFFF);
  result.xmm64u(1) = (Bit32s) (val64 >> 32);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVSXDQ_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 2A */
void BX_CPU_C::MOVNTDQA_VdqMdq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  /* source must be memory reference */
  if (i->modC0()) {
    BX_INFO(("MOVNTDQA_VdqMdq: must be memory reference"));
    UndefinedOpcode(i);
  }

  BX_CPU_THIS_PTR prepareSSE();

  BxPackedXmmRegister op;

  read_virtual_dqword_aligned(i->seg(), RMAddr(i), (Bit8u *) &op);

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), op);

#else
  BX_INFO(("MOVNTDQA_VdqMdq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 30 */
void BX_CPU_C::PMOVZXBW_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm16u(0) = val64 & 0xFF;
  result.xmm16u(1) = (val64 >>  8) & 0xFF;
  result.xmm16u(2) = (val64 >> 16) & 0xFF;
  result.xmm16u(3) = (val64 >> 24) & 0xFF;
  result.xmm16u(4) = (val64 >> 32) & 0xFF;
  result.xmm16u(5) = (val64 >> 40) & 0xFF;
  result.xmm16u(6) = (val64 >> 48) & 0xFF;
  result.xmm16u(7) = val64 >> 56;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXBW_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 31 */
void BX_CPU_C::PMOVZXBD_VdqWd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit32u val32;

  if (i->modC0())
  {
    val32 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);
  }

  result.xmm32u(0) = val32 & 0xFF;
  result.xmm32u(1) = (val32 >>  8) & 0xFF;
  result.xmm32u(2) = (val32 >> 16) & 0xFF;
  result.xmm32u(3) = val32  >> 24;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXBD_VdqWd: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 32 */
void BX_CPU_C::PMOVZXBQ_VdqWw(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit16u val16;

  if (i->modC0())
  {
    val16 = BX_READ_XMM_REG_LO_WORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &val16);
  }

  result.xmm64u(0) = val16 & 0xFF;
  result.xmm64u(1) = val16 >> 8;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXBQ_VdqWw: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 33 */
void BX_CPU_C::PMOVZXWD_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm32u(0) = val64 & 0xFFFF;
  result.xmm32u(1) = (val64 >> 16) & 0xFFFF;
  result.xmm32u(2) = (val64 >> 32) & 0xFFFF;
  result.xmm32u(3) = val64  >> 48;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXWD_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 34 */
void BX_CPU_C::PMOVZXWQ_VdqWd(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit32u val32;

  if (i->modC0())
  {
    val32 = BX_READ_XMM_REG_LO_DWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &val32);
  }

  result.xmm64u(0) = val32 & 0xFFFF;
  result.xmm64u(1) = val32 >> 16;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXWQ_VdqWd: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 38 35 */
void BX_CPU_C::PMOVZXDQ_VdqWq(bxInstruction_c *i)
{
#if BX_SUPPORT_SSE >= 4
  BX_CPU_THIS_PTR prepareSSE();
  BxPackedXmmRegister result;
  Bit64u val64;

  if (i->modC0())
  {
    val64 = BX_READ_XMM_REG_LO_QWORD(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &val64);
  }

  result.xmm64u(0) = val64 & 0xFFFFFFFF;
  result.xmm64u(1) = val64 >> 32;

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PMOVZXDQ_VdqWq: required SSE4, use --enable-sse option"));
  UndefinedOpcode(i);
#endif
}

/* 66 0F 3A 0F */
void BX_CPU_C::PALIGNR_VdqWdqIb(bxInstruction_c *i)
{
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
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

  unsigned shift = i->Ib() * 8;

  if(shift == 0) {
    result.xmm64u(0) = op2.xmm64u(0);
    result.xmm64u(1) = op2.xmm64u(1);
  }
  else if(shift < 64) {
    result.xmm64u(0) = (op2.xmm64u(0) >> shift) | (op2.xmm64u(1) << (64-shift));
    result.xmm64u(1) = (op2.xmm64u(1) >> shift) | (op1.xmm64u(0) << (64-shift));
  }
  else if(shift == 64) {
    result.xmm64u(0) = op2.xmm64u(1);
    result.xmm64u(1) = op1.xmm64u(0);
  }
  else if(shift < 128) {
    shift -= 64;
    result.xmm64u(0) = (op2.xmm64u(1) >> shift) | (op1.xmm64u(0) << (64-shift));
    result.xmm64u(1) = (op1.xmm64u(0) >> shift) | (op1.xmm64u(1) << (64-shift));
  }
  else if(shift == 128) {
    result.xmm64u(0) = op1.xmm64u(0);
    result.xmm64u(1) = op1.xmm64u(1);
  }
  else if(shift < 192) {
    shift -= 128;
    result.xmm64u(0) = (op1.xmm64u(0) >> shift) | (op1.xmm64u(1) << (64-shift));
    result.xmm64u(1) = (op1.xmm64u(1) >> shift);
  }
  else if(shift < 256) {
    result.xmm64u(0) = op1.xmm64u(1) >> (shift - 192);
    result.xmm64u(1) = 0;
  }
  else {
    result.xmm64u(0) = 0;
    result.xmm64u(1) = 0;
  }

  /* now write result back to destination */
  BX_WRITE_XMM_REG(i->nnn(), result);
#else
  BX_INFO(("PALIGNR_VdqWdqIb: required SSE3E, use --enable-sse and --enable-sse-extension options"));
  UndefinedOpcode(i);
#endif
}

#endif // BX_SUPPORT_SSE >= 4 || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
