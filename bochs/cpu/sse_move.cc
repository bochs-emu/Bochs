/////////////////////////////////////////////////////////////////////////
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

#if BX_CPU_LEVEL >= 6

#include "simd_int.h"

void BX_CPU_C::print_state_SSE(void)
{
  BX_DEBUG(("MXCSR: 0x%08x", BX_MXCSR_REGISTER));
  for(unsigned n=0;n<BX_XMM_REGISTERS;n++) {
    BxPackedXmmRegister xmm = BX_READ_XMM_REG(n);
    BX_DEBUG(("XMM%02u: %08x%08x:%08x%08x", n,
       xmm.xmm32u(3), xmm.xmm32u(2), xmm.xmm32u(1), xmm.xmm32u(0)));
  }
}

#endif

#if BX_SUPPORT_FPU
Bit8u BX_CPU_C::pack_FPU_TW(Bit16u twd)
{
  Bit8u tag_byte = 0;

  if((twd & 0x0003) != 0x0003) tag_byte |= 0x01;
  if((twd & 0x000c) != 0x000c) tag_byte |= 0x02;
  if((twd & 0x0030) != 0x0030) tag_byte |= 0x04;
  if((twd & 0x00c0) != 0x00c0) tag_byte |= 0x08;
  if((twd & 0x0300) != 0x0300) tag_byte |= 0x10;
  if((twd & 0x0c00) != 0x0c00) tag_byte |= 0x20;
  if((twd & 0x3000) != 0x3000) tag_byte |= 0x40;
  if((twd & 0xc000) != 0xc000) tag_byte |= 0x80;

  return tag_byte;
}

Bit16u BX_CPU_C::unpack_FPU_TW(Bit16u tag_byte)
{
  Bit32u twd = 0;

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

  for(int index = 7;index >= 0; index--, twd <<= 2, tag_byte <<= 1)
  {
      if(tag_byte & 0x80) {
         const floatx80 &fpu_reg = BX_FPU_REG(index);
         twd |= FPU_tagof(fpu_reg);
      }
      else {
         twd |= FPU_Tag_Empty;
      }
  }

  return (twd >> 2);
}
#endif

/* ************************************ */
/* SSE: SAVE/RESTORE FPU/MMX/SSEx STATE */
/* ************************************ */

/* 0F AE Grp15 010 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LDMXCSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u new_mxcsr = read_virtual_dword(i->seg(), eaddr);
  if(new_mxcsr & ~MXCSR_MASK)
      exception(BX_GP_EXCEPTION, 0);

  BX_MXCSR_REGISTER = new_mxcsr;
#endif

  BX_NEXT_INSTR(i);
}

/* 0F AE Grp15 011 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STMXCSR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u mxcsr = BX_MXCSR_REGISTER & MXCSR_MASK;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_dword(i->seg(), eaddr, mxcsr);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F AE Grp15 000 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FXSAVE(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_DEBUG(("FXSAVE: save FPU/MMX/SSE state"));

  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  xmm.xmm16u(0) = BX_CPU_THIS_PTR the_i387.get_control_word();
  xmm.xmm16u(1) = BX_CPU_THIS_PTR the_i387.get_status_word();
  xmm.xmm16u(2) = pack_FPU_TW(BX_CPU_THIS_PTR the_i387.get_tag_word());

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
    xmm.xmm32u(2) = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip);
    xmm.xmm32u(3) = x87_get_FCS();
  }

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_xmmword_aligned(i->seg(), eaddr, &xmm);

  bx_address asize_mask = i->asize_mask();

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
    xmm.xmm32u(0) = (Bit32u)(BX_CPU_THIS_PTR the_i387.fdp);
    xmm.xmm32u(1) = x87_get_FDS();
  }

  if (is_cpu_extension_supported(BX_ISA_SSE)) {
    xmm.xmm32u(2) = BX_MXCSR_REGISTER;
    xmm.xmm32u(3) = MXCSR_MASK;
  }
  else {
    xmm.xmm32u(2) = 0;
    xmm.xmm32u(3) = 0;
  }

  write_virtual_xmmword(i->seg(), (eaddr + 16) & asize_mask, &xmm);

  /* store i387 register file */
  for(index=0; index < 8; index++)
  {
    const floatx80 &fp = BX_READ_FPU_REG(index);

    xmm.xmm64u(0) = fp.fraction;
    xmm.xmm64u(1) = 0;
    xmm.xmm16u(4) = fp.exp;

    write_virtual_xmmword(i->seg(), (eaddr+index*16+32) & asize_mask, &xmm);
  }

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_FFXSR() && CPL == 0 && long64_mode()) {
    BX_NEXT_INSTR(i); // skip saving of the XMM state
  }
#endif

  if(BX_CPU_THIS_PTR cr4.get_OSFXSR() && is_cpu_extension_supported(BX_ISA_SSE))
  {
    /* store XMM register file */
    for(index=0; index < 16; index++)
    {
      // save XMM8-XMM15 only in 64-bit mode
      if (index < 8 || long64_mode()) {
         write_virtual_xmmword(i->seg(),
             (eaddr+index*16+160) & asize_mask, &BX_READ_XMM_REG(index));
      }
    }
  }

  /* do not touch reserved fields */
#endif

  BX_NEXT_INSTR(i);
}

/* 0F AE Grp15 001 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::FXRSTOR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister xmm;
  unsigned index;

  BX_DEBUG(("FXRSTOR: restore FPU/MMX/SSE state"));

  if (BX_CPU_THIS_PTR cr0.get_EM() || BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0);

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  read_virtual_xmmword_aligned(i->seg(), eaddr, &xmm);

  bx_address asize_mask = i->asize_mask();

  BX_CPU_THIS_PTR the_i387.cwd =  xmm.xmm16u(0);
  BX_CPU_THIS_PTR the_i387.swd =  xmm.xmm16u(1);
  BX_CPU_THIS_PTR the_i387.tos = (xmm.xmm16u(1) >> 11) & 0x07;

  /* always set bit 6 as '1 */
  BX_CPU_THIS_PTR the_i387.cwd =
     (BX_CPU_THIS_PTR the_i387.cwd & ~FPU_CW_Reserved_Bits) | 0x0040;

  /* Restore x87 FPU Opcode */
  /* The lower 11 bits contain the FPU opcode, upper 5 bits are reserved */
  BX_CPU_THIS_PTR the_i387.foo = xmm.xmm16u(3) & 0x7FF;

  /* Restore x87 FPU IP */
#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    BX_CPU_THIS_PTR the_i387.fip = xmm.xmm64u(1);
    BX_CPU_THIS_PTR the_i387.fcs = 0;
  }
  else
#endif
  {
    BX_CPU_THIS_PTR the_i387.fip = xmm.xmm32u(2);
    BX_CPU_THIS_PTR the_i387.fcs = xmm.xmm16u(6);
  }

  Bit32u tag_byte = xmm.xmmubyte(4);

  /* Restore x87 FPU DP */
  read_virtual_xmmword(i->seg(), (eaddr + 16) & asize_mask, &xmm);

#if BX_SUPPORT_X86_64
  if (i->os64L()) {
    BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm64u(0);
    BX_CPU_THIS_PTR the_i387.fds = 0;
  }
  else
#endif
  {
    BX_CPU_THIS_PTR the_i387.fdp = xmm.xmm32u(0);
    BX_CPU_THIS_PTR the_i387.fds = xmm.xmm16u(2);
  }

  if(/* BX_CPU_THIS_PTR cr4.get_OSFXSR() && */ is_cpu_extension_supported(BX_ISA_SSE))
  {
    Bit32u new_mxcsr = xmm.xmm32u(2);
    if(new_mxcsr & ~MXCSR_MASK)
       exception(BX_GP_EXCEPTION, 0);

    BX_MXCSR_REGISTER = new_mxcsr;
  }

  /* load i387 register file */
  for(index=0; index < 8; index++)
  {
    floatx80 reg;
    reg.fraction = read_virtual_qword(i->seg(), (eaddr+index*16+32) & asize_mask);
    reg.exp      = read_virtual_word (i->seg(), (eaddr+index*16+40) & asize_mask);

    // update tag only if it is not empty
    BX_WRITE_FPU_REGISTER_AND_TAG(reg,
              IS_TAG_EMPTY(index) ? FPU_Tag_Empty : FPU_tagof(reg), index);
  }

  BX_CPU_THIS_PTR the_i387.twd = unpack_FPU_TW(tag_byte);

  /* check for unmasked exceptions */
  if (FPU_PARTIAL_STATUS & ~FPU_CONTROL_WORD & FPU_CW_Exceptions_Mask) {
    /* set the B and ES bits in the status-word */
    FPU_PARTIAL_STATUS |= FPU_SW_Summary | FPU_SW_Backward;
  }
  else {
    /* clear the B and ES bits in the status-word */
    FPU_PARTIAL_STATUS &= ~(FPU_SW_Summary | FPU_SW_Backward);
  }

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR efer.get_FFXSR() && CPL == 0 && long64_mode()) {
    BX_NEXT_INSTR(i); // skip restore of the XMM state
  }
#endif

  /* If the OSFXSR bit in CR4 is not set, the FXRSTOR instruction does
     not restore the states of the XMM and MXCSR registers. */
  if(BX_CPU_THIS_PTR cr4.get_OSFXSR() && is_cpu_extension_supported(BX_ISA_SSE))
  {
    /* load XMM register file */
    for(index=0; index < 16; index++)
    {
      // restore XMM8-XMM15 only in 64-bit mode
      if (index < 8 || long64_mode()) {
         read_virtual_xmmword(i->seg(),
             (eaddr+index*16+160) & asize_mask, &BX_READ_XMM_REG(index));

      }
    }
  }
#endif

  BX_NEXT_INSTR(i);
}

/* *************************** */
/* SSE: MEMORY MOVE OPERATIONS */
/* *************************** */

/* All these opcodes never generate SIMD floating point exeptions */

/* MOVUPS:    0F 10 */
/* MOVUPD: 66 0F 10 */
/* MOVDQU: F3 0F 6F */
/* LDDQU:  F2 0F F0 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVUPS_VpsWpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_xmmword(i->seg(), eaddr, &BX_XMM_REG(i->dst()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVUPS_WpsVpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_xmmword(i->seg(), eaddr, &BX_XMM_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVAPS:    0F 28 */
/* MOVAPD: 66 0F 28 */
/* MOVDQA: F3 0F 6F */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVAPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_XMM_REG(i->dst(), BX_READ_XMM_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVAPS_VpsWpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  read_virtual_xmmword_aligned(i->seg(), eaddr, &BX_XMM_REG(i->dst()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVAPS:     0F 29 */
/* MOVNTPS:    0F 2B */
/* MOVNTPD: 66 0F 2B */
/* MOVNTDQ: 66 0F E7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVAPS_WpsVpsM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_xmmword_aligned(i->seg(), eaddr, &BX_XMM_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* F3 0F 10 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSS_VssWssR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* If the source operand is an XMM register, the high-order
          96 bits of the destination XMM register are not modified. */
  BX_WRITE_XMM_REG_LO_DWORD(i->dst(), BX_READ_XMM_REG_LO_DWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSS_VssWssM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* If the source operand is a memory location, the high-order
          96 bits of the destination XMM register are cleared to 0s */
  op.xmm64u(0) = (Bit64u) read_virtual_dword(i->seg(), eaddr);
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());
#endif

  BX_NEXT_INSTR(i);
}

/* F3 0F 11 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSS_WssVssM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_dword(i->seg(), eaddr, BX_READ_XMM_REG_LO_DWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVLPS:    0F 13 */
/* MOVLPD: 66 0F 13 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSD_WsdVsdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_qword(i->seg(), eaddr, BX_XMM_REG_LO_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* F2 0F 10 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSD_VsdWsdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  /* If the source operand is an XMM register, the high-order
          64 bits of the destination XMM register are not modified. */
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), BX_READ_XMM_REG_LO_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVHLPS:   0F 12 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVHLPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), BX_READ_XMM_REG_HI_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVLPS:    0F 12 */
/* MOVLPD: 66 0F 12 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVLPS_VpsMq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit64u val64 = read_virtual_qword(i->seg(), eaddr);

  BX_WRITE_XMM_REG_LO_QWORD(i->dst(), val64);
#endif

  BX_NEXT_INSTR(i);
}

/* F2 0F 12 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVDDUP_VpdWqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  xmm_pbroadcastq(&BX_XMM_REG(i->dst()), BX_READ_XMM_REG_LO_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* F3 0F 12 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSLDUP_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32u(1) = op.xmm32u(0);
  op.xmm32u(3) = op.xmm32u(2);

  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* F3 0F 16 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSHDUP_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src());

  op.xmm32u(0) = op.xmm32u(1);
  op.xmm32u(2) = op.xmm32u(3);

  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* MOVLHPS:   0F 16 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVLHPS_VpsWpsR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_XMM_REG_HI_QWORD(i->dst(), BX_READ_XMM_REG_LO_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* MOVHPS:    0F 16 */
/* MOVHPD: 66 0F 16 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVHPS_VpsMq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  /* pointer, segment address pair */
  Bit64u val64 = read_virtual_qword(i->seg(), eaddr);

  BX_WRITE_XMM_REG_HI_QWORD(i->dst(), val64);
#endif

  BX_NEXT_INSTR(i);
}

/* MOVHPS:    0F 17 */
/* MOVHPD: 66 0F 17 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVHPS_MqVps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_qword(i->seg(), eaddr, BX_XMM_REG_HI_QWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

/* 66 0F F7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MASKMOVDQU_VdqUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  bx_address rdi = RDI & i->asize_mask();
  BxPackedXmmRegister op = BX_READ_XMM_REG(i->src1()),
    mask = BX_READ_XMM_REG(i->src2()), temp;

  // check for write permissions before writing even if mask is all 0s
  temp.xmm64u(0) = read_RMW_virtual_qword(i->seg(), rdi);
  temp.xmm64u(1) = read_RMW_virtual_qword(i->seg(), (rdi + 8) & i->asize_mask());

  /* no data will be written to memory if mask is all 0s */
  if ((mask.xmm64u(0) | mask.xmm64u(1)) == 0) {
    BX_NEXT_INSTR(i);
  }

  for(unsigned j=0; j<16; j++) {
    if(mask.xmmubyte(j) & 0x80) temp.xmmubyte(j) = op.xmmubyte(j);
  }

  // and write result back to the memory
  write_RMW_virtual_qword(temp.xmm64u(1));
  // write permissions already checked by read_RMW_virtual_qword_64
  write_virtual_qword(i->seg(), rdi, temp.xmm64u(0));
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 50 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVMSKPS_GdUps(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u mask = xmm_pmovmskd(&BX_XMM_REG(i->src()));
  BX_WRITE_32BIT_REGZ(i->dst(), mask);
#endif

  BX_NEXT_INSTR(i);
}

/* 66 0F 50 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVMSKPD_GdUpd(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u mask = xmm_pmovmskq(&BX_XMM_REG(i->src()));
  BX_WRITE_32BIT_REGZ(i->dst(), mask);
#endif

  BX_NEXT_INSTR(i);
}

/* 66 0F 6E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_VdqEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;
  op.xmm64u(0) = (Bit64u) BX_READ_32BIT_REG(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

/* 66 0F 6E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_VdqEqR(bxInstruction_c *i)
{
  BxPackedXmmRegister op;
  op.xmm64u(0) = BX_READ_64BIT_REG(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());

  BX_NEXT_INSTR(i);
}

#endif

/* 66 0F 7E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVD_EdVdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_XMM_REG_LO_DWORD(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

/* 66 0F 7E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_EqVqR(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), BX_READ_XMM_REG_LO_QWORD(i->src()));

  BX_NEXT_INSTR(i);
}

#endif

/* F3 0F 7E */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ_VqWqR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_READ_XMM_REG_LO_QWORD(i->src());
  op.xmm64u(1) = 0; /* zero-extension to 128 bit */

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSD_VsdWsdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BxPackedXmmRegister op;
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  op.xmm64u(0) = read_virtual_qword(i->seg(), eaddr);
  op.xmm64u(1) = 0; /* zero-extension to 128 bit */

  BX_WRITE_XMM_REGZ(i->dst(), op, i->getVL());
#endif

  BX_NEXT_INSTR(i);
}

/* F2 0F D6 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVDQ2Q_PqUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_CPU_THIS_PTR FPU_check_pending_exceptions(); /* check floating point status word for a pending FPU exceptions */
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedMmxRegister mm;
  MMXUQ(mm) = BX_READ_XMM_REG_LO_QWORD(i->src());

  BX_WRITE_MMX_REG(i->dst(), mm);
#endif

  BX_NEXT_INSTR(i);
}

/* F3 0F D6 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVQ2DQ_VdqQq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_CPU_THIS_PTR FPU_check_pending_exceptions(); /* check floating point status word for a pending FPU exceptions */
  BX_CPU_THIS_PTR prepareFPU2MMX();

  BxPackedXmmRegister op;

  op.xmm64u(0) = BX_MMX_REG(i->src());
  op.xmm64u(1) = 0;

  BX_WRITE_XMM_REG(i->dst(), op);
#endif

  BX_NEXT_INSTR(i);
}

/* 66 0F D7 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVMSKB_GdUdq(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u mask = xmm_pmovmskb(&BX_XMM_REG(i->src()));
  BX_WRITE_32BIT_REGZ(i->dst(), mask);
#endif

  BX_NEXT_INSTR(i);
}

/* ************************** */
/* 3-BYTE-OPCODE INSTRUCTIONS */
/* ************************** */

#if BX_CPU_LEVEL >= 6

/* 66 0F 38 20 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXBW_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm16u(0) = MMXSB0(op);
  result.xmm16u(1) = MMXSB1(op);
  result.xmm16u(2) = MMXSB2(op);
  result.xmm16u(3) = MMXSB3(op);
  result.xmm16u(4) = MMXSB4(op);
  result.xmm16u(5) = MMXSB5(op);
  result.xmm16u(6) = MMXSB6(op);
  result.xmm16u(7) = MMXSB7(op);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 21 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXBD_VdqWdR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.xmm32u(0) = (Bit8s) (val32 & 0xFF);
  result.xmm32u(1) = (Bit8s) ((val32 >>  8) & 0xFF);
  result.xmm32u(2) = (Bit8s) ((val32 >> 16) & 0xFF);
  result.xmm32u(3) = (Bit8s) (val32  >> 24);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 22 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXBQ_VdqWwR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit16u val16 = BX_READ_XMM_REG_LO_WORD(i->src());

  result.xmm64u(0) = (Bit8s) (val16 & 0xFF);
  result.xmm64u(1) = (Bit8s) (val16 >> 8);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 23 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXWD_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm32u(0) = (Bit16s) MMXSW0(op);
  result.xmm32u(1) = (Bit16s) MMXSW1(op);
  result.xmm32u(2) = (Bit16s) MMXSW2(op);
  result.xmm32u(3) = (Bit16s) MMXSW3(op);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 24 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXWQ_VdqWdR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.xmm64u(0) = (Bit16s) (val32 & 0xFFFF);
  result.xmm64u(1) = (Bit16s) (val32 >> 16);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 25 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVSXDQ_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit64u val64 = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm64u(0) = (Bit32s) (val64 & 0xFFFFFFFF);
  result.xmm64u(1) = (Bit32s) (val64 >> 32);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 30 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXBW_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm16u(0) = MMXUB0(op);
  result.xmm16u(1) = MMXUB1(op);
  result.xmm16u(2) = MMXUB2(op);
  result.xmm16u(3) = MMXUB3(op);
  result.xmm16u(4) = MMXUB4(op);
  result.xmm16u(5) = MMXUB5(op);
  result.xmm16u(6) = MMXUB6(op);
  result.xmm16u(7) = MMXUB7(op);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 31 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXBD_VdqWdR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.xmm32u(0) = val32 & 0xFF;
  result.xmm32u(1) = (val32 >>  8) & 0xFF;
  result.xmm32u(2) = (val32 >> 16) & 0xFF;
  result.xmm32u(3) = val32  >> 24;

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 32 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXBQ_VdqWwR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit16u val16 = BX_READ_XMM_REG_LO_WORD(i->src());

  result.xmm64u(0) = val16 & 0xFF;
  result.xmm64u(1) = val16 >> 8;

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 33 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXWD_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  BxPackedMmxRegister op;

  // use MMX register as 64-bit value with convinient accessors
  MMXUQ(op) = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm32u(0) = MMXUW0(op);
  result.xmm32u(1) = MMXUW1(op);
  result.xmm32u(2) = MMXUW2(op);
  result.xmm32u(3) = MMXUW3(op);

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 34 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXWQ_VdqWdR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit32u val32 = BX_READ_XMM_REG_LO_DWORD(i->src());

  result.xmm64u(0) = val32 & 0xFFFF;
  result.xmm64u(1) = val32 >> 16;

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 38 35 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PMOVZXDQ_VdqWqR(bxInstruction_c *i)
{
  BxPackedXmmRegister result;
  Bit64u val64 = BX_READ_XMM_REG_LO_QWORD(i->src());

  result.xmm64u(0) = val64 & 0xFFFFFFFF;
  result.xmm64u(1) = val64 >> 32;

  BX_WRITE_XMM_REGZ(i->dst(), result, i->getVL());

  BX_NEXT_INSTR(i);
}

/* 66 0F 3A 0F */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::PALIGNR_VdqWdqIbR(bxInstruction_c *i)
{
  BxPackedXmmRegister op1 = BX_READ_XMM_REG(i->dst()), op2 = BX_READ_XMM_REG(i->src());

  xmm_palignr(&op2, &op1, i->Ib());

  BX_WRITE_XMM_REG(i->dst(), op2);

  BX_NEXT_INSTR(i);
}

#endif
