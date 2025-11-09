/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2025 Stanislav Shwartsman
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

#if BX_SUPPORT_X86_64 && BX_SUPPORT_FRED

// FIXME: long_mode64 requirement must be handled through opcode tables attributes

void BX_CPU_C::FRED_EventDelivery(Bit8u vector, unsigned type, bool push_error, Bit16u error_code)
{
  Bit32u old_CPL = CPL;
  Bit32u old_CSL = (CPL == 3) ? 0 : CSL;

  Bit32u old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  old_CS |= old_CSL << 16;
#if BX_SUPPORT_CET
  // FIXME double check: CR4.CET AND IA32_S_CET.ENDBR_EN AND IA32_S_CET.TRACKER
  if (cr4.get_CET() && EndbranchEnabled(0))
    old_CS |= (1 << 18); // cache the shadow stack tracking control in old_CS[18]
#endif

  Bit64u old_SS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value;
  if (interrupts_inhibited(BX_INHIBIT_INTERRUPTS))
    old_SS |= (1 << 16);
  if (type == BX_EVENT_OTHER || type == BX_SOFTWARE_INTERRUPT) // event type is SYSCALL, SYSENTER or INTn
    old_SS |= (1 << 17);
  else if (type == BX_NMI)
    old_SS |= (1 << 18);
  // old_SS[63:32] FRED event information as defined in Section 5.2.1

  Bit64u old_RIP = RIP;
  Bit64u new_RIP = msr.ia32_fred_cfg & ~0xfff;
  if (CPL == 3)
    new_RIP += 256;
  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("FRED Event Delivery: non canonical value in BX_MSR_IA32_FRED_CONFIG !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // if CPL == 3 AND event is not an exception nested on event delivery AND event is not #DF
  Bit32u event_SL = 0;
  if (CPL == 3 && BX_CPU_THIS_PTR last_exception_type == 0 && vector != BX_DF_EXCEPTION) {
    event_SL = 0;
  }
  else {
    if (type == BX_EXTERNAL_INTERRUPT) {
      event_SL = (msr.ia32_fred_cfg >> 9) & 0x3;
    }
    else if (type == BX_NMI || type == BX_HARDWARE_EXCEPTION || type == BX_SOFTWARE_EXCEPTION || type == BX_PRIVILEGED_SOFTWARE_INTERRUPT) {
      BX_ASSERT(vector < 32);
      event_SL = (msr.ia32_fred_stack_levels >> (vector*2)) & 0x3;
    }
  }
  Bit32u new_CSL = MAX(event_SL, old_CSL);

  // determine new RSP
  Bit64u old_RSP = RSP;
  Bit64u new_RSP;
  if (CPL == 3 || new_CSL > old_CSL)
    new_RSP = msr.ia32_fred_rsp[new_CSL];
  else
    new_RSP = (RSP - (msr.ia32_fred_cfg & 0x1C0)) & ~BX_CONST64(0x3F); // decrement RSP and align to 64-bytes

  Bit32u old_flags = read_eflags();

#if BX_SUPPORT_CET
  Bit64u old_SSP = SSP;
  Bit64u new_SSP = 0;
  if (ShadowStackEnabled(0)) {
    if (CPL == 3 || new_CSL > old_CSL) {
      new_SSP = msr.ia32_fred_ssp[new_CSL];
      if (new_SSP & 0x4) {
        BX_ERROR(("FRED Event Delivery: Shadow Stack not 8-byte aligned !"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
    else {
      new_SSP = SSP - (msr.ia32_fred_cfg & 0x8);
    }
  }
#endif

  // ESTABLISH NEW CONTEXT — OLD STATE WILL BE RESTORED IF THERE IS A SUBSEQUENT EXCEPTION -> FIXME: HOW ???

  // update segment registers if event occurred in ring 3
  if (CPL == 3) {

    // set up CS segment, flat, 64-bit DPL=0
    parse_selector((BX_CPU_THIS_PTR msr.star >> 32) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);

    setup_flat_CS(0, true); // CPL0, long mode

    handleCpuModeChange(); // mode change could only happen when in long_mode()

#if BX_SUPPORT_ALIGNMENT_CHECK
    BX_CPU_THIS_PTR alignment_check_mask = 0; // CPL=0
#endif

    // set up SS segment, flat, 64-bit DPL=0
    parse_selector(((BX_CPU_THIS_PTR msr.star >> 32) + 8) & BX_SELECTOR_RPL_MASK,
                       &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    setup_flat_SS(0);

    Bit64u old_GS_BASE = MSR_GSBASE;
    MSR_GSBASE = BX_CPU_THIS_PTR msr.kernelgsbase;
    BX_CPU_THIS_PTR msr.kernelgsbase = old_GS_BASE;
  }

  // update registers defining context
  RIP = new_RIP;
  BX_CPU_THIS_PTR eflags = 0x2; // Clear EFLAGS, bit1 is always set
  clearEFlagsOSZAPC();	        // update lazy flags state
  RSP = new_RSP;
  CSL = new_CSL;

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) SSP = new_SSP;
#endif

  // save state on stack
  // Save return state on new regular stack; memory accesses here have supervisor privilege
  push_64(0);  // first 8 bytes pushed are all zeros
  push_64(0 /*FIXME event_data*/);
  push_64(old_SS);
  push_64(old_RSP);
  push_64(old_flags);
  push_64(old_CS);
  push_64(old_RIP);
  push_64(error_code);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0) && old_CPL == 0) {
    shadow_stack_write_dword(SSP-4, CPL, 0); // store 4 bytes of zeros to SSP-4
    SSP &= ~BX_CONST64(0x7);
    shadow_stack_push_64(old_CS);
    shadow_stack_push_64(old_RIP);
    shadow_stack_push_64(old_SSP);
  }
  if (ShadowStackEnabled(3) && old_CPL == 3) {
    BX_CPU_THIS_PTR msr.ia32_pl_ssp[3] = CanonicalizeAddress(BX_CPU_THIS_PTR msr.ia32_pl_ssp[3]);
    //if IA32_S_CET.ENDBR_EN = 1
    //    THEN IA32_S_CET[11:10] = 0; // IDLE with SUPPRESS = 0 FIXME
  }
#endif

  // update event related state
  // FIXME: If the NMI was being injected by VM entry, the existing treatment applies: physical-NMI blocking
  // is not changed, but virtual NMIs are blocked if the “virtual NMIs” VM-execution control is 1
  if (type == BX_NMI)
    mask_event(BX_EVENT_NMI);
  CPL = 0;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ERETS(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FRED()) {
    BX_ERROR(("%s: FRED in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL must be 0", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  RSP_SPECULATIVE;
  RSP += 8;                // skip error code so that RSP references the return state

  Bit64u new_RIP = pop_64();
  Bit64u temp_CS = pop_64();
  Bit64u new_RFLAGS = pop_64();
  Bit64u new_RSP = pop_64();
  Bit64u temp_SS = pop_64();

  if (! isCanonical(new_RIP) /* FIXME: || tempCS & 0xFFFFFFFF_FFF8FFFF != current CS */ /* newRFLAGS & 0xFFFFFFFF_FFC2802A != 2 */ /* tempSS & 0xFFF8FFFF != currentSS */ ) {
    BX_ERROR(("%s: #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // ERETS will not numberically increase stack level
  Bit32u new_CSL = (temp_CS >> 16) & 0x3;
         new_CSL = MIN(CSL, new_CSL);

  // FIXME: complete implementation
  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ERETU(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FRED()) {
    BX_ERROR(("%s: FRED in not enabled in CR4", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("%s: CPL must be 0", i->getIaOpcodeNameShort()));
    exception(BX_UD_EXCEPTION, 0);
  }

  RSP_SPECULATIVE;
  RSP += 8;                // skip error code so that RSP references the return state

  Bit64u new_RIP = pop_64();
  Bit64u temp_CS = pop_64();
  Bit64u new_RFLAGS = pop_64();
  Bit64u new_RSP = pop_64();
  Bit64u temp_SS = pop_64();

  if (! isCanonical(new_RIP) /* FIXME: || tempCS & 0xFFFFFFFF_FFF8FFFF != current CS */ /* newRFLAGS & 0xFFFFFFFF_FFC2802A != 2 */ /* tempSS & 0xFFF8FFFF != currentSS */ ) {
    BX_ERROR(("%s: #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // FIXME: complete implementation
  RSP_COMMIT;

  BX_NEXT_INSTR(i);
}

#endif
