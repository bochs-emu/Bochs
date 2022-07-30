/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2019 Stanislav Shwartsman
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
#include "msr.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "decoder/ia_opcodes.h"

const Bit64u XSAVEC_COMPACTION_ENABLED = BX_CONST64(0x8000000000000000);

#if BX_CPU_LEVEL >= 6
extern XSaveRestoreStateHelper xsave_restore[];
#endif

#if BX_USE_CPU_SMF == 0
#define CALL_XSAVE_FN(ptrToFunc)  (this->*(ptrToFunc))
#else
#define CALL_XSAVE_FN(ptrToFunc)  (ptrToFunc)
#endif

/* 0F AE /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XSAVE(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_CPU_THIS_PTR prepareXSAVE();

  bool xsaveopt = (i->getIaOpcode() == BX_IA_XSAVEOPT);

  BX_DEBUG(("%s: save processor state XCR0=0x%08x", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR xcr0.get32()));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR alignment_check()) {
    if (laddr & 0x3) {
      BX_ERROR(("%s: access not aligned to 4-byte cause model specific #AC(0)", i->getIaOpcodeNameShort()));
      exception(BX_AC_EXCEPTION, 0);
    }
  }
#endif

  if (laddr & 0x3f) {
    BX_ERROR(("%s: access not aligned to 64-byte", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address asize_mask = i->asize_mask();

  Bit64u xstate_bv = read_virtual_qword(i->seg(), (eaddr + 512) & asize_mask);

  Bit32u requested_feature_bitmap = BX_CPU_THIS_PTR xcr0.get32() & EAX;
  Bit32u xinuse = get_xinuse_vector(requested_feature_bitmap);

  /////////////////////////////////////////////////////////////////////////////
  if ((requested_feature_bitmap & BX_XCR0_FPU_MASK) != 0)
  {
    if (! xsaveopt || (xinuse & BX_XCR0_FPU_MASK) != 0)
      xsave_x87_state(i, eaddr);

    if (xinuse & BX_XCR0_FPU_MASK)
      xstate_bv |=  BX_XCR0_FPU_MASK;
    else
      xstate_bv &= ~BX_XCR0_FPU_MASK;
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((requested_feature_bitmap & (BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK)) != 0)
  {
    // store MXCSR - write cannot cause any boundary cross because XSAVE image is 64-byte aligned
    write_virtual_dword(i->seg(), eaddr + 24, BX_MXCSR_REGISTER);
    write_virtual_dword(i->seg(), eaddr + 28, MXCSR_MASK);
  }

  /////////////////////////////////////////////////////////////////////////////
  for (unsigned feature = xcr0_t::BX_XCR0_SSE_BIT; feature < xcr0_t::BX_XCR0_LAST; feature++)
  {
    Bit32u feature_mask = (1 << feature);

    if ((requested_feature_bitmap & feature_mask) != 0)
    {
      if (! xsave_restore[feature].len) {
        BX_ERROR(("%s: feature #%d requested to save but not implemented !", i->getIaOpcodeNameShort(), feature));
        continue;
      }

      if (! xsaveopt || (xinuse & feature_mask) != 0) {
        BX_ASSERT(xsave_restore[feature].xsave_method);
        CALL_XSAVE_FN(xsave_restore[feature].xsave_method)(i, eaddr+xsave_restore[feature].offset);
      }

      if (xinuse & feature_mask)
        xstate_bv |=  Bit64u(feature_mask);
      else
        xstate_bv &= ~Bit64u(feature_mask);
    }
  }

  // always update header to 'dirty' state
  write_virtual_qword(i->seg(), (eaddr + 512) & asize_mask, xstate_bv);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F C7 /4 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XSAVEC(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_CPU_THIS_PTR prepareXSAVE();

  bool xsaves = (i->getIaOpcode() == BX_IA_XSAVES);
  if (xsaves) {
    if (CPL != 0) {
      BX_ERROR(("%s: with CPL != 0", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_XSAVES_XRSTORS)) {
        BX_ERROR(("%s in VMX guest: not allowed to use instruction !", i->getIaOpcodeNameShort()));
        exception(BX_UD_EXCEPTION, 0);
      }

      VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
      Bit64u requested_features = (((Bit64u) EDX) << 32) | EAX;
      if (requested_features & BX_CPU_THIS_PTR msr.ia32_xss & vm->xss_exiting_bitmap)
        VMexit_Instruction(i, VMX_VMEXIT_XSAVES);
    }
#endif
  }

  BX_DEBUG(("%s: save processor state XCR0=0x%08x XSS=" FMT_LL "x", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR xcr0.get32(), BX_CPU_THIS_PTR msr.ia32_xss));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR alignment_check()) {
    if (laddr & 0x3) {
      BX_ERROR(("%s: access not aligned to 4-byte cause model specific #AC(0)", i->getIaOpcodeNameShort()));
      exception(BX_AC_EXCEPTION, 0);
    }
  }
#endif

  if (laddr & 0x3f) {
    BX_ERROR(("%s: access not aligned to 64-byte", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address asize_mask = i->asize_mask();

  Bit64u xcr0 = (Bit64u) BX_CPU_THIS_PTR xcr0.get32();
  if (xsaves)
    xcr0 |= BX_CPU_THIS_PTR msr.ia32_xss;

  Bit32u requested_feature_bitmap = xcr0 & EAX;
  Bit32u xinuse = get_xinuse_vector(requested_feature_bitmap);
  Bit64u xstate_bv = requested_feature_bitmap & xinuse;
  Bit64u xcomp_bv = requested_feature_bitmap | XSAVEC_COMPACTION_ENABLED;

  if ((requested_feature_bitmap & BX_XCR0_FPU_MASK) != 0)
  {
    if (xinuse & BX_XCR0_FPU_MASK) {
      xsave_x87_state(i, eaddr);
    }
  }

  if ((requested_feature_bitmap & BX_XCR0_SSE_MASK) != 0)
  {
    // write cannot cause any boundary cross because XSAVE image is 64-byte aligned
    write_virtual_dword(i->seg(), eaddr + 24, BX_MXCSR_REGISTER);
    write_virtual_dword(i->seg(), eaddr + 28, MXCSR_MASK);
  }

  Bit32u offset = XSAVE_SSE_STATE_OFFSET;

  /////////////////////////////////////////////////////////////////////////////
  for (unsigned feature = xcr0_t::BX_XCR0_SSE_BIT; feature < xcr0_t::BX_XCR0_LAST; feature++)
  {
    Bit32u feature_mask = (1 << feature);

    if ((requested_feature_bitmap & feature_mask) != 0)
    {
      if (! xsave_restore[feature].len) {
        BX_ERROR(("%s: feature #%d requested to save but not implemented !", i->getIaOpcodeNameShort(), feature));
        continue;
      }

      if (xinuse & feature_mask) {
        BX_ASSERT(xsave_restore[feature].xsave_method);
        CALL_XSAVE_FN(xsave_restore[feature].xsave_method)(i, eaddr+offset);
      }

      offset += xsave_restore[feature].len;
    }
  }

  // always update header to 'dirty' state
  write_virtual_qword(i->seg(), (eaddr + 512) & asize_mask, xstate_bv);
  write_virtual_qword(i->seg(), (eaddr + 520) & asize_mask, xcomp_bv);
#endif

  BX_NEXT_INSTR(i);
}

/* 0F AE /5 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XRSTOR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  BX_CPU_THIS_PTR prepareXSAVE();

  bool xrstors = (i->getIaOpcode() == BX_IA_XRSTORS);
  if (xrstors) {
    if (CPL != 0) {
      BX_ERROR(("%s: with CPL != 0", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

#if BX_SUPPORT_VMX >= 2
    if (BX_CPU_THIS_PTR in_vmx_guest) {
      if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_XSAVES_XRSTORS)) {
        BX_ERROR(("%s in VMX guest: not allowed to use instruction !", i->getIaOpcodeNameShort()));
        exception(BX_UD_EXCEPTION, 0);
      }

      VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
      Bit64u requested_features = (((Bit64u) EDX) << 32) | EAX;
      if (requested_features & BX_CPU_THIS_PTR msr.ia32_xss & vm->xss_exiting_bitmap)
        VMexit_Instruction(i, VMX_VMEXIT_XRSTORS);
    }
#endif
  }

  BX_DEBUG(("%s: restore processor state XCR0=0x%08x XSS=" FMT_LL "x", i->getIaOpcodeNameShort(), BX_CPU_THIS_PTR xcr0.get32(), BX_CPU_THIS_PTR msr.ia32_xss));

  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  bx_address laddr = get_laddr(i->seg(), eaddr);

#if BX_SUPPORT_ALIGNMENT_CHECK && BX_CPU_LEVEL >= 4
  if (BX_CPU_THIS_PTR alignment_check()) {
    if (laddr & 0x3) {
      BX_ERROR(("%s: access not aligned to 4-byte cause model specific #AC(0)", i->getIaOpcodeNameShort()));
      exception(BX_AC_EXCEPTION, 0);
    }
  }
#endif

  if (laddr & 0x3f) {
    BX_ERROR(("%s: access not aligned to 64-byte", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address asize_mask = i->asize_mask();

  Bit64u xstate_bv = read_virtual_qword(i->seg(), (eaddr + 512) & asize_mask);
  Bit64u xcomp_bv = read_virtual_qword(i->seg(), (eaddr + 520) & asize_mask);
  Bit64u header3 = read_virtual_qword(i->seg(), (eaddr + 528) & asize_mask);

  if (header3 != 0) {
    BX_ERROR(("%s: Reserved header3 state is not '0", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bool compaction = (xcomp_bv & XSAVEC_COMPACTION_ENABLED) != 0;

  if (! BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_XSAVEC) || ! compaction) {
    if (xcomp_bv != 0) {
      BX_ERROR(("%s: Reserved header2 state is not '0", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

  Bit64u xcr0 = (Bit64u) BX_CPU_THIS_PTR xcr0.get32();
  if (xrstors)
    xcr0 |= BX_CPU_THIS_PTR msr.ia32_xss;

  if (! compaction) {
    if (xrstors) {
      BX_ERROR(("XRSTORS require compaction XCOMP_BV[63] to be set"));
      exception(BX_GP_EXCEPTION, 0);
    }

    if ((~xcr0 & xstate_bv) != 0) {
      BX_ERROR(("%s: Invalid xsave_bv state", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
  else {
    if ((~xcr0 & xcomp_bv & ~XSAVEC_COMPACTION_ENABLED) != 0) {
      BX_ERROR(("%s: Invalid xcomp_bv state", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
    
    if (xstate_bv & ~xcomp_bv) {
      BX_ERROR(("%s: xstate_bv set a bit which is not in xcomp_bv state", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }

    Bit64u header4 = read_virtual_qword(i->seg(), (eaddr + 536) & asize_mask);
    Bit64u header5 = read_virtual_qword(i->seg(), (eaddr + 544) & asize_mask);
    Bit64u header6 = read_virtual_qword(i->seg(), (eaddr + 552) & asize_mask);
    Bit64u header7 = read_virtual_qword(i->seg(), (eaddr + 560) & asize_mask);
    Bit64u header8 = read_virtual_qword(i->seg(), (eaddr + 568) & asize_mask);

    if (header4 | header5 | header6 | header7 | header8) {
      BX_ERROR(("%s: Reserved header4_header7 state is not '0", i->getIaOpcodeNameShort()));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

  Bit32u requested_feature_bitmap = xcr0 & EAX;

  /////////////////////////////////////////////////////////////////////////////
  if ((requested_feature_bitmap & BX_XCR0_FPU_MASK) != 0)
  {
    if (xstate_bv & BX_XCR0_FPU_MASK)
      xrstor_x87_state(i, eaddr);
    else
      xrstor_init_x87_state();
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((requested_feature_bitmap & BX_XCR0_SSE_MASK) != 0 || 
     ((requested_feature_bitmap & BX_XCR0_YMM_MASK) != 0 && ! compaction))
  {
    // read cannot cause any boundary cross because XSAVE image is 64-byte aligned
    Bit32u new_mxcsr = read_virtual_dword(i->seg(), eaddr + 24);
    if(new_mxcsr & ~MXCSR_MASK)
       exception(BX_GP_EXCEPTION, 0);
    BX_MXCSR_REGISTER = new_mxcsr;
  }

  /////////////////////////////////////////////////////////////////////////////
  if ((requested_feature_bitmap & BX_XCR0_SSE_MASK) != 0)
  {
    if (xstate_bv & BX_XCR0_SSE_MASK)
      xrstor_sse_state(i, eaddr+XSAVE_SSE_STATE_OFFSET);
    else
      xrstor_init_sse_state();
  }

  if (compaction) {
    Bit32u offset = XSAVE_YMM_STATE_OFFSET;

    /////////////////////////////////////////////////////////////////////////////
    for (unsigned feature = xcr0_t::BX_XCR0_YMM_BIT; feature < xcr0_t::BX_XCR0_LAST; feature++)
    {
      Bit32u feature_mask = (1 << feature);
    
      if ((requested_feature_bitmap & feature_mask) != 0)
      {
        if (! xsave_restore[feature].len) {
          BX_ERROR(("%s: feature #%d requested to restore but not implemented !", i->getIaOpcodeNameShort(), feature));
          continue;
        }

        if (xstate_bv & feature_mask) {
          BX_ASSERT(xsave_restore[feature].xrstor_method);
          CALL_XSAVE_FN(xsave_restore[feature].xrstor_method)(i, eaddr+offset);
        }
        else {
          BX_ASSERT(xsave_restore[feature].xrstor_init_method);
          CALL_XSAVE_FN(xsave_restore[feature].xrstor_init_method)();
        }

        offset += xsave_restore[feature].len;
      }
    }
  }
  else {

    /////////////////////////////////////////////////////////////////////////////
    for (unsigned feature = xcr0_t::BX_XCR0_YMM_BIT; feature < xcr0_t::BX_XCR0_LAST; feature++)
    {
      Bit32u feature_mask = (1 << feature);
    
      if ((requested_feature_bitmap & feature_mask) != 0)
      {
        if (! xsave_restore[feature].len) {
          BX_ERROR(("%s: feature #%d requested to restore but not implemented !", i->getIaOpcodeNameShort(), feature));
          continue;
        }

        if (xstate_bv & feature_mask) {
          BX_ASSERT(xsave_restore[feature].xrstor_method);
          CALL_XSAVE_FN(xsave_restore[feature].xrstor_method)(i, eaddr+xsave_restore[feature].offset);
        }
        else {
          BX_ASSERT(xsave_restore[feature].xrstor_init_method);
          CALL_XSAVE_FN(xsave_restore[feature].xrstor_init_method)();
        }
      }
    }
  }

#if BX_SUPPORT_PKEYS
  // take effect of changing the PKRU state
  if ((requested_feature_bitmap & BX_XCR0_PKRU_MASK) != 0) {
    set_PKeys(TMP32, BX_CPU_THIS_PTR pkrs);
  }
#endif

#endif // BX_CPU_LEVEL >= 6

  BX_NEXT_INSTR(i);
}

#if BX_CPU_LEVEL >= 6

// x87 state management //

void BX_CPU_C::xsave_x87_state(bxInstruction_c *i, bx_address offset)
{
  BxPackedXmmRegister xmm;
  bx_address asize_mask = i->asize_mask();

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
  if (i->os64L()) {
    xmm.xmm64u(1) = (BX_CPU_THIS_PTR the_i387.fip);
  }
  else
#endif
  {
    xmm.xmm32u(2) = (Bit32u)(BX_CPU_THIS_PTR the_i387.fip);
    xmm.xmm32u(3) = x87_get_FCS();
  }

  write_virtual_xmmword(i->seg(), offset, &xmm);

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
  // write cannot cause any boundary cross because XSAVE image is 64-byte aligned
  if (i->os64L()) {
    write_virtual_qword(i->seg(), offset + 16, BX_CPU_THIS_PTR the_i387.fdp);
  }
  else
#endif
  {
    write_virtual_dword(i->seg(), offset + 16, (Bit32u) BX_CPU_THIS_PTR the_i387.fdp);
    write_virtual_dword(i->seg(), offset + 20, x87_get_FDS());
  }
  /* do not touch MXCSR state */

  /* store i387 register file */
  for(unsigned index=0; index < 8; index++)
  {
    const floatx80 &fp = BX_READ_FPU_REG(index);

    xmm.xmm64u(0) = fp.fraction;
    xmm.xmm64u(1) = 0;
    xmm.xmm16u(4) = fp.exp;

    write_virtual_xmmword(i->seg(), (offset+index*16+32) & asize_mask, &xmm);
  }
}

void BX_CPU_C::xrstor_x87_state(bxInstruction_c *i, bx_address offset)
{
  BxPackedXmmRegister xmm;
  bx_address asize_mask = i->asize_mask();

  // load FPU state from XSAVE area
  read_virtual_xmmword(i->seg(), offset, &xmm);

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

  /* Restore x87 FPU DP - read cannot cause any boundary cross because XSAVE image is 64-byte aligned */
  read_virtual_xmmword(i->seg(), offset + 16, &xmm);

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
  for(unsigned index=0; index < 8; index++)
  {
    floatx80 reg;
    reg.fraction = read_virtual_qword(i->seg(), (offset+index*16+32) & asize_mask);
    reg.exp      = read_virtual_word (i->seg(), (offset+index*16+40) & asize_mask);

    // update tag only if it is not empty
    BX_WRITE_FPU_REGISTER_AND_TAG(reg,
              IS_TAG_EMPTY(index) ? FPU_Tag_Empty : FPU_tagof(reg), index);
  }

  /* Restore floating point tag word - see desription for FXRSTOR instruction */
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
}

void BX_CPU_C::xrstor_init_x87_state(void)
{
  // initialize FPU with reset values
  BX_CPU_THIS_PTR the_i387.init();

  for (unsigned index=0;index<8;index++) {
    static floatx80 reg = { 0, 0 };
    BX_FPU_REG(index) = reg;
  }
}

bool BX_CPU_C::xsave_x87_state_xinuse(void)
{
  if (BX_CPU_THIS_PTR the_i387.get_control_word() != 0x037F ||
      BX_CPU_THIS_PTR the_i387.get_status_word() != 0 ||
      BX_CPU_THIS_PTR the_i387.get_tag_word() != 0xFFFF ||
      BX_CPU_THIS_PTR the_i387.foo != 0 ||
      BX_CPU_THIS_PTR the_i387.fip != 0 || BX_CPU_THIS_PTR the_i387.fcs != 0 ||
      BX_CPU_THIS_PTR the_i387.fdp != 0 || BX_CPU_THIS_PTR the_i387.fds != 0) return true;

  for (unsigned index=0;index<8;index++) {
    floatx80 reg = BX_FPU_REG(index);
    if (reg.exp != 0 || reg.fraction != 0) return true;
  }

  return false;
}

// SSE state management //

void BX_CPU_C::xsave_sse_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  /* store XMM register file */
  for(unsigned index=0; index < 16; index++) {
    // save XMM8-XMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      write_virtual_xmmword(i->seg(), (offset + index*16) & asize_mask, &BX_READ_XMM_REG(index));
    }
  }
}

void BX_CPU_C::xrstor_sse_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  // load SSE state from XSAVE area
  for(unsigned index=0; index < 16; index++) {
    // restore XMM8-XMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      read_virtual_xmmword(i->seg(), (offset+index*16) & asize_mask, &BX_READ_XMM_REG(index));
    }
  }
}

void BX_CPU_C::xrstor_init_sse_state(void)
{
  // initialize SSE with reset values
  for(unsigned index=0; index < 16; index++) {
    // set XMM8-XMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) BX_CLEAR_XMM_REG(index);
  }
}

bool BX_CPU_C::xsave_sse_state_xinuse(void)
{
  for(unsigned index=0; index < 16; index++) {
    // set XMM8-XMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      const BxPackedXmmRegister *reg = &BX_XMM_REG(index);
      if (! is_clear(reg)) return true;
    }
  }

  return false;
}

#if BX_SUPPORT_AVX

// YMM state management //

void BX_CPU_C::xsave_ymm_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  /* store AVX state */
  for(unsigned index=0; index < 16; index++) {
    // save YMM8-YMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      write_virtual_xmmword(i->seg(), (offset + index*16) & asize_mask, &BX_READ_AVX_REG_LANE(index, 1));
    }
  }
}

void BX_CPU_C::xrstor_ymm_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  // load AVX state from XSAVE area
  for(unsigned index=0; index < 16; index++) {
    // restore YMM8-YMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      read_virtual_xmmword(i->seg(), (offset + index*16) & asize_mask, &BX_READ_AVX_REG_LANE(index, 1));
    }
  }
}

void BX_CPU_C::xrstor_init_ymm_state(void)
{
  // initialize upper part of AVX registers with reset values
  for(unsigned index=0; index < 16; index++) {
    // set YMM8-YMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) BX_CLEAR_AVX_HIGH128(index);
  }
}

bool BX_CPU_C::xsave_ymm_state_xinuse(void)
{
  for(unsigned index=0; index < 16; index++) {
    // set YMM8-YMM15 only in 64-bit mode
    if (index < 8 || long64_mode()) {
      const BxPackedXmmRegister *reg = &BX_READ_AVX_REG_LANE(index, 1);
      if (! is_clear(reg)) return true;
    }
  }

  return false;
}

#if BX_SUPPORT_EVEX

// Opmask state management //

void BX_CPU_C::xsave_opmask_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  // save OPMASK state to XSAVE area
  for(unsigned index=0; index < 8; index++) {
    write_virtual_qword(i->seg(), (offset+index*8) & asize_mask, BX_READ_OPMASK(index));
  }
}

void BX_CPU_C::xrstor_opmask_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  // load opmask registers from XSAVE area
  for(unsigned index=0; index < 8; index++) {
    Bit64u opmask = read_virtual_qword(i->seg(), (offset+index*8) & asize_mask);
    BX_WRITE_OPMASK(index, opmask);
  }
}

void BX_CPU_C::xrstor_init_opmask_state(void)
{
  // initialize opmask registers with reset values
  for(unsigned index=0; index < 8; index++) {
    BX_WRITE_OPMASK(index, 0);
  }
}

bool BX_CPU_C::xsave_opmask_state_xinuse(void)
{
  for(unsigned index=0; index < 8; index++) {
    if (BX_READ_OPMASK(index)) return true;
  }

  return false;
}

// ZMM_HI256 (upper part of zmm0..zmm15 registers) state management //

// In 64-bit mode, ZMM_Hi256 state is in its initial configuration if each of ZMM0_H-ZMM15_H is 0.
// Outside 64-bit mode, ZMM_Hi256 state is in its initial configuration if each of ZMM0_H-ZMM7_H is 0.
// An execution of XRSTOR or XRSTORS outside 64-bit mode does not update ZMM8_H-ZMM15_H.

void BX_CPU_C::xsave_zmm_hi256_state(bxInstruction_c *i, bx_address offset)
{
  unsigned num_regs = long64_mode() ? 16 : 8;

  bx_address asize_mask = i->asize_mask();

  // save upper part of ZMM registers to XSAVE area
  for(unsigned index=0; index < num_regs; index++) {
    write_virtual_ymmword(i->seg(), (offset+index*32) & asize_mask, &BX_READ_ZMM_REG_HI(index));
  }
}

void BX_CPU_C::xrstor_zmm_hi256_state(bxInstruction_c *i, bx_address offset)
{
  unsigned num_regs = long64_mode() ? 16 : 8;

  bx_address asize_mask = i->asize_mask();

  // load upper part of ZMM registers from XSAVE area
  for(unsigned index=0; index < num_regs; index++) {
    read_virtual_ymmword(i->seg(), (offset+index*32) & asize_mask, &BX_READ_ZMM_REG_HI(index));
  }
}

void BX_CPU_C::xrstor_init_zmm_hi256_state(void)
{
  unsigned num_regs = long64_mode() ? 16 : 8;

  // initialize upper part of ZMM registers with reset values
  for(unsigned index=0; index < num_regs; index++) {
    BX_CLEAR_AVX_HIGH256(index);
  }
}

bool BX_CPU_C::xsave_zmm_hi256_state_xinuse(void)
{
  unsigned num_regs = long64_mode() ? 16 : 8;

  for(unsigned index=0; index < num_regs; index++) {
    for (unsigned n=2; n < 4; n++) {
      const BxPackedXmmRegister *reg = &BX_READ_AVX_REG_LANE(index, n);
      if (! is_clear(reg)) return true;
    }
  }

  return false;
}

// HI_ZMM (zmm15..zmm31) state management //

// In 64-bit mode, Hi16_ZMM state is in its initial configuration if each of ZMM16-ZMM31 is 0.
// Outside 64-bit mode, Hi16_ZMM state is always in its initial configuration.
// An execution of XRSTOR or XRSTORS outside 64-bit mode does not update ZMM16-ZMM31.

void BX_CPU_C::xsave_hi_zmm_state(bxInstruction_c *i, bx_address offset)
{
  if (!long64_mode()) return;

  bx_address asize_mask = i->asize_mask();

  // save high ZMM state to XSAVE area
  for(unsigned index=0; index < 16; index++) {
    write_virtual_zmmword(i->seg(), (offset+index*64) & asize_mask, &BX_READ_AVX_REG(index+16));
  }
}

void BX_CPU_C::xrstor_hi_zmm_state(bxInstruction_c *i, bx_address offset)
{
  if (!long64_mode()) return;

  bx_address asize_mask = i->asize_mask();

  // load high ZMM state from XSAVE area
  for(unsigned index=0; index < 16; index++) {
    read_virtual_zmmword(i->seg(), (offset+index*64) & asize_mask, &BX_READ_AVX_REG(index+16));
  }
}

void BX_CPU_C::xrstor_init_hi_zmm_state(void)
{
  if (!long64_mode()) return;

  // initialize high ZMM registers with reset values
  for(unsigned index=16; index < 32; index++) {
    BX_CLEAR_AVX_REG(index);
  }
}

bool BX_CPU_C::xsave_hi_zmm_state_xinuse(void)
{
  if (!long64_mode()) return true;

  for(unsigned index=16; index < 32; index++) {
    for (unsigned n=0; n < 4; n++) {
      const BxPackedXmmRegister *reg = &BX_READ_AVX_REG_LANE(index, n);
      if (! is_clear(reg)) return true;
    }
  }

  return false;
}
#endif // BX_SUPPORT_EVEX

#endif // BX_SUPPORT_AVX

#if BX_SUPPORT_PKEYS
// PKRU state management //
void BX_CPU_C::xsave_pkru_state(bxInstruction_c *i, bx_address offset)
{
  write_virtual_qword(i->seg(), offset, (Bit64u) BX_CPU_THIS_PTR pkru);
}

void BX_CPU_C::xrstor_pkru_state(bxInstruction_c *i, bx_address offset)
{
  // just write the pkru to TMP register for now and don't call set_PKeys
  // calling it will take immediate effect on all future memory accesses including load of other XRSTOR components
  TMP32 = read_virtual_dword(i->seg(), offset);
}

void BX_CPU_C::xrstor_init_pkru_state(void)
{
  // just write the pkru to TMP register for now and don't call set_PKeys
  // calling it will take immediate effect on all future memory accesses including load of other XRSTOR components
  TMP32 = 0;
}

bool BX_CPU_C::xsave_pkru_state_xinuse(void)
{
  return (BX_CPU_THIS_PTR pkru != 0);
}
#endif

#if BX_SUPPORT_CET
// CET U state management //
void BX_CPU_C::xsave_cet_u_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  write_virtual_qword(i->seg(), offset, BX_CPU_THIS_PTR msr.ia32_cet_control[1]);
  write_virtual_qword(i->seg(), (offset + 8) & asize_mask, BX_CPU_THIS_PTR msr.ia32_pl_ssp[3]);
}

void BX_CPU_C::xrstor_cet_u_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  Bit64u ctrl = read_virtual_qword(i->seg(), offset);
  Bit64u ia32_pl3_ssp = read_virtual_qword(i->seg(), (offset + 8) & asize_mask);

  // XRSTOR on CET state does all reserved bits and canonicality check like WRMSR would do
  wrmsr(BX_MSR_IA32_U_CET, ctrl);
  wrmsr(BX_MSR_IA32_PL3_SSP, ia32_pl3_ssp);
}

void BX_CPU_C::xrstor_init_cet_u_state(void)
{
  BX_CPU_THIS_PTR msr.ia32_cet_control[1] = 0;
  BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = 0;
}

bool BX_CPU_C::xsave_cet_u_state_xinuse(void)
{
  return BX_CPU_THIS_PTR msr.ia32_cet_control[1] == 0 &&
         BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] == 0;
}

// CET S state management //
void BX_CPU_C::xsave_cet_s_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  write_virtual_qword(i->seg(),  offset,                    BX_CPU_THIS_PTR msr.ia32_pl_ssp[0]);
  write_virtual_qword(i->seg(), (offset +  8) & asize_mask, BX_CPU_THIS_PTR msr.ia32_pl_ssp[1]);
  write_virtual_qword(i->seg(), (offset + 16) & asize_mask, BX_CPU_THIS_PTR msr.ia32_pl_ssp[2]);
}

void BX_CPU_C::xrstor_cet_s_state(bxInstruction_c *i, bx_address offset)
{
  bx_address asize_mask = i->asize_mask();

  Bit64u ia32_pl0_ssp = read_virtual_qword(i->seg(),  offset);
  Bit64u ia32_pl1_ssp = read_virtual_qword(i->seg(), (offset +  8) & asize_mask);
  Bit64u ia32_pl2_ssp = read_virtual_qword(i->seg(), (offset + 16) & asize_mask);

  // XRSTOR on CET state does all reserved bits and canonicality check like WRMSR would do
  wrmsr(BX_MSR_IA32_PL0_SSP, ia32_pl0_ssp);
  wrmsr(BX_MSR_IA32_PL1_SSP, ia32_pl1_ssp);
  wrmsr(BX_MSR_IA32_PL2_SSP, ia32_pl2_ssp);
}

void BX_CPU_C::xrstor_init_cet_s_state(void)
{
  for (unsigned n=0;n<3;n++)
    BX_CPU_THIS_PTR msr.ia32_pl_ssp[n] = 0;
}

bool BX_CPU_C::xsave_cet_s_state_xinuse(void)
{
  for (unsigned n=0;n<3;n++)
    return BX_CPU_THIS_PTR msr.ia32_pl_ssp[n] != 0;

  return false;
}
#endif

Bit32u BX_CPU_C::get_xinuse_vector(Bit32u requested_feature_bitmap)
{
  Bit32u xinuse = 0;

  /////////////////////////////////////////////////////////////////////////////
  for (unsigned feature = xcr0_t::BX_XCR0_FPU_BIT; feature < xcr0_t::BX_XCR0_LAST; feature++)
  {
    Bit32u feature_mask = (1 << feature);
  
    if ((requested_feature_bitmap & feature_mask) != 0)
    {
      if (! xsave_restore[feature].len) {
        BX_ERROR(("get_xinuse_vector(0x%08x): feature #%d requested but not implemented !", requested_feature_bitmap, feature));
        continue;
      }

      BX_ASSERT(xsave_restore[feature].xstate_in_use_method);
      if (CALL_XSAVE_FN(xsave_restore[feature].xstate_in_use_method)())
        xinuse |= feature_mask;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  if (requested_feature_bitmap & BX_XCR0_SSE_MASK) {
    if (BX_MXCSR_REGISTER != MXCSR_RESET)
      xinuse |= BX_XCR0_SSE_MASK;
  }

  return xinuse;
}

#endif // BX_CPU_LEVEL >= 6

/* 0F 01 D0 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XGETBV(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if(! BX_CPU_THIS_PTR cr4.get_OSXSAVE()) {
    BX_ERROR(("XGETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required

  if (ECX != 0) {
    if (ECX == 1 && BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_XSAVEC)) {
      // Executing XGETBV with ECX = 1 returns in EDX:EAX the logical-AND of XCR0
      // and the current value of the XINUSE state-component bitmap.
      // If XINUSE[i]=0, state component [i] is in its initial configuration.
      RDX = 0;
      RAX = get_xinuse_vector(BX_CPU_THIS_PTR xcr0.get32());
    }
    else {
      BX_ERROR(("XGETBV: Invalid XCR%d register", ECX));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
  else {
    RDX = 0;
    RAX = BX_CPU_THIS_PTR xcr0.get32();
  }
#endif

  BX_NEXT_INSTR(i);
}

/* 0F 01 D1 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::XSETBV(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if(! BX_CPU_THIS_PTR cr4.get_OSXSAVE()) {
    BX_ERROR(("XSETBV: OSXSAVE feature is not enabled in CR4!"));
    exception(BX_UD_EXCEPTION, 0);
  }

#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMexit(VMX_VMEXIT_XSETBV, 0);
  }
#endif

#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_XSETBV)) Svm_Vmexit(SVM_VMEXIT_XSETBV);
  }
#endif

  // CPL is always 3 in vm8086 mode
  if (/* v8086_mode() || */ CPL != 0) {
    BX_ERROR(("XSETBV: The current priveledge level is not 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // For now hardcoded handle only XCR0 register, it should take a few
  // years until extension will be required
  if (ECX != 0) {
    BX_ERROR(("XSETBV: Invalid XCR%d register", ECX));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (EDX != 0 || (EAX & ~BX_CPU_THIS_PTR xcr0_suppmask) != 0 || (EAX & BX_XCR0_FPU_MASK) == 0) {
    BX_ERROR(("XSETBV: Attempt to change reserved bits"));
    exception(BX_GP_EXCEPTION, 0);
  }

#if BX_SUPPORT_AVX
  if ((EAX & (BX_XCR0_YMM_MASK | BX_XCR0_SSE_MASK)) == BX_XCR0_YMM_MASK) {
    BX_ERROR(("XSETBV: Attempt to enable AVX without SSE"));
    exception(BX_GP_EXCEPTION, 0);
  }
#endif

#if BX_SUPPORT_EVEX
  if (EAX & (BX_XCR0_OPMASK_MASK | BX_XCR0_ZMM_HI256_MASK | BX_XCR0_HI_ZMM_MASK)) {
    Bit32u avx512_state_mask = (BX_XCR0_FPU_MASK | BX_XCR0_SSE_MASK | BX_XCR0_YMM_MASK | BX_XCR0_OPMASK_MASK | BX_XCR0_ZMM_HI256_MASK | BX_XCR0_HI_ZMM_MASK);
    if ((EAX & avx512_state_mask) != avx512_state_mask) {
      BX_ERROR(("XSETBV: Illegal attempt to enable AVX-512 state"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
#endif

  BX_CPU_THIS_PTR xcr0.set32(EAX);

#if BX_SUPPORT_AVX
  handleAvxModeChange();
#endif

#endif // BX_CPU_LEVEL >= 6

  BX_NEXT_TRACE(i);
}
