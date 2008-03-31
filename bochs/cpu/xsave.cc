/////////////////////////////////////////////////////////////////////////
// $Id: xsave.cc,v 1.6 2008-03-31 20:56:27 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
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

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RAX EAX
#define RDX EDX
#endif

#if BX_SUPPORT_XSAVE
void BX_CPU_C::prepareXSAVE(void)
{
  if(! (BX_CPU_THIS_PTR cr4.get_OSXSAVE()))
    exception(BX_UD_EXCEPTION, 0, 0);

  if(BX_CPU_THIS_PTR cr0.get_TS())
    exception(BX_NM_EXCEPTION, 0, 0);
}
#endif

/* 0F AE /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_CPU_THIS_PTR prepareXSAVE();

  BX_DEBUG(("XSAVE: save processor state XCR0=0x%08x", BX_CPU_THIS_PTR xcr0.getRegister()));

  BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  bx_address laddr = BX_CPU_THIS_PTR get_segment_base(i->seg()) + RMAddr(i);

  if (laddr & 0x3f) {
    BX_ERROR(("XSAVE: access not aligned to 64-byte"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  //
  // We will go feature-by-feature and not run over all XCR0 bits
  //

  /////////////////////////////////////////////////////////////////////////////
  if (BX_CPU_THIS_PTR xcr0.get_FPU() && (EAX & BX_XCR0_FPU_MASK) != 0)
  {
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
    if (i->os64L()) {
      xmm.xmm64u(1) = (BX_CPU_THIS_PTR the_i387.fip);
    }
    else
#endif
    {
      xmm.xmm32u(2) = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip) & 0xffffffff;
      xmm.xmm32u(3) =         (BX_CPU_THIS_PTR the_i387.fcs);
    }

    write_virtual_dqword(i->seg(), RMAddr(i), (Bit8u *) &xmm);

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
    if (i->os64L()) {
      write_virtual_qword(i->seg(), RMAddr(i) + 16, BX_CPU_THIS_PTR the_i387.fdp);
    }
    else
#endif
    {
      write_virtual_dword(i->seg(), RMAddr(i) + 16, 
            (Bit32u)(BX_CPU_THIS_PTR the_i387.fdp & 0xffffffff));
      write_virtual_dword(i->seg(), RMAddr(i) + 20, 
            (Bit32u)(BX_CPU_THIS_PTR the_i387.fds));
    }
    /* do not touch MXCSR state */

    /* store i387 register file */
    for(index=0; index < 8; index++)
    {
      const floatx80 &fp = BX_FPU_REG(index);

      xmm.xmm64u(0) = fp.fraction;
      xmm.xmm64u(1) = 0;
      xmm.xmm16u(4) = fp.exp;

      write_virtual_dqword(i->seg(), RMAddr(i)+index*16+32, (Bit8u *) &xmm);
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  if (BX_CPU_THIS_PTR xcr0.get_SSE() && (EAX & BX_XCR0_SSE_MASK) != 0)
  {
    write_virtual_dword(i->seg(), RMAddr(i) + 24, BX_MXCSR_REGISTER);
    write_virtual_dword(i->seg(), RMAddr(i) + 28, MXCSR_MASK);

    /* store XMM register file */
    for(index=0; index < BX_XMM_REGISTERS; index++)
    {
      // save XMM8-XMM15 only in 64-bit mode
      if (index < 8 || Is64BitMode()) {
        write_virtual_dqword(i->seg(),
           RMAddr(i)+index*16+160, (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]));
      }
    }
  }

  // skip header update for now, required to know if a CPU feature is in its initial state
#else
  BX_INFO(("XSAVE: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE /5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE
  unsigned index;
  BxPackedXmmRegister xmm;

  BX_CPU_THIS_PTR prepareXSAVE();

  BX_DEBUG(("XRSTOR: restore processor state XCR0=0x%08x", BX_CPU_THIS_PTR xcr0.getRegister()));

  BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  bx_address laddr = BX_CPU_THIS_PTR get_segment_base(i->seg()) + RMAddr(i);

  if (laddr & 0x3f) {
    BX_ERROR(("XRSTOR: access not aligned to 64-byte"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  Bit64u header1 = read_virtual_qword(i->seg(), RMAddr(i) + 512);
  Bit64u header2 = read_virtual_qword(i->seg(), RMAddr(i) + 520);
  Bit64u header3 = read_virtual_qword(i->seg(), RMAddr(i) + 528);

  if ((~BX_CPU_THIS_PTR xcr0.getRegister() & header1) != 0) {
    BX_ERROR(("XRSTOR: Broken header state"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  if (header2 != 0 || header3 != 0) {
    BX_ERROR(("XRSTOR: Reserved header state is not '0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  //
  // We will go feature-by-feature and not run over all XCR0 bits
  //

  /////////////////////////////////////////////////////////////////////////////
  if (BX_CPU_THIS_PTR xcr0.get_FPU() && (EAX & BX_XCR0_FPU_MASK) != 0)
  {
    if (header1 & BX_XCR0_FPU_MASK) {
      // load FPU state from XSAVE area
      read_virtual_dqword(i->seg(), RMAddr(i), (Bit8u *) &xmm);

      BX_CPU_THIS_PTR the_i387.cwd =  xmm.xmm16u(0);
      BX_CPU_THIS_PTR the_i387.swd =  xmm.xmm16u(1);
      BX_CPU_THIS_PTR the_i387.tos = (xmm.xmm16u(1) >> 11) & 0x07;

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
        BX_CPU_THIS_PTR the_i387.fcs = xmm.xmm16u(5);
      }

      Bit32u tag_byte = xmm.xmmubyte(4);

      /* Restore x87 FPU DP */
      read_virtual_dqword(i->seg(), RMAddr(i) + 16, (Bit8u *) &xmm);

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

      /* load i387 register file */
      for(index=0; index < 8; index++)
      {
        read_virtual_tword(i->seg(), RMAddr(i)+index*16+32, &(BX_FPU_REG(index)));
      }

      /* Restore floating point tag word - see desription for FXRSTOR instruction */
      BX_CPU_THIS_PTR the_i387.twd = unpack_FPU_TW(tag_byte);
    }
    else {
       // initialize FPU with reset values
       BX_CPU_THIS_PTR the_i387.init();

       for (index=0;index<8;index++) {
         BX_CPU_THIS_PTR the_i387.st_space[index].exp      = 0;
         BX_CPU_THIS_PTR the_i387.st_space[index].fraction = 0;
       }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  if (BX_CPU_THIS_PTR xcr0.get_SSE() && (EAX & BX_XCR0_SSE_MASK) != 0)
  {
    Bit32u new_mxcsr = read_virtual_dword(i->seg(), RMAddr(i) + 24);
    if(new_mxcsr & ~MXCSR_MASK)
       exception(BX_GP_EXCEPTION, 0, 0);

    if (header1 & BX_XCR0_SSE_MASK) {
      // load SSE state from XSAVE area
      for(index=0; index < BX_XMM_REGISTERS; index++)
      {
         // restore XMM8-XMM15 only in 64-bit mode
         if (index < 8 || Is64BitMode()) {
           read_virtual_dqword(i->seg(),
               RMAddr(i)+index*16+160, (Bit8u *) &(BX_CPU_THIS_PTR xmm[index]));
         }
      }
    }
    else {
       // initialize SSE with reset values
       for(index=0; index < BX_XMM_REGISTERS; index++) {
         // set XMM8-XMM15 only in 64-bit mode
         if (index < 8 || Is64BitMode()) {
           BX_CPU_THIS_PTR xmm[index].xmm64u(0) = 0;
           BX_CPU_THIS_PTR xmm[index].xmm64u(1) = 0;
         }
       }
    }
  }
#else
  BX_INFO(("XRSTOR: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 01 D0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XGETBV(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE
  if(! (BX_CPU_THIS_PTR cr4.get_OSXSAVE())) {
    BX_ERROR(("XGETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required
  if (ECX != 0) {
    BX_ERROR(("XGETBV: Invalid XCR register %d", ECX));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  RDX = 0;
  RAX = BX_CPU_THIS_PTR xcr0.getRegister();
#else
  BX_INFO(("XGETBV: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 01 D1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XSETBV(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE
  if (real_mode() || v8086_mode()) {
    BX_ERROR(("XSETBV: not recognized in real or virtual-8086 mode"));
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  if(! (BX_CPU_THIS_PTR cr4.get_OSXSAVE())) {
    BX_ERROR(("XSETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0, 0);
  }

  if (CPL != 0) {
    BX_ERROR(("XSETBV: The current priveledge level is not 0"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required
  if (ECX != 0) {
    BX_ERROR(("XSETBV: Invalid XCR register %d", ECX));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  if (EDX != 0 || (EAX & ~BX_XCR0_SUPPORTED_BITS) != 0 || (EAX & 1) == 0) {
    BX_ERROR(("XSETBV: Attempting to change reserved bits!"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  BX_CPU_THIS_PTR xcr0.setRegister(EAX);
#else
  BX_INFO(("XSETBV: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}
