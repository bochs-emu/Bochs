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
  Bit32u old_CSL = (CPL == 3) ? 0 : BX_CPU_THIS_PTR CSL;

  Bit32u old_CS  = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  old_CS |= old_CSL << 16;
#if BX_SUPPORT_CET
  if (BX_CPU_THIS_PTR cr4.get_CET() && WaitingForEndbranch(0))
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
  Bit64u new_RIP = BX_CPU_THIS_PTR msr.ia32_fred_cfg & ~0xfff;
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
      event_SL = (BX_CPU_THIS_PTR msr.ia32_fred_cfg >> 9) & 0x3;
    }
    else if (type == BX_NMI || type == BX_HARDWARE_EXCEPTION || type == BX_SOFTWARE_EXCEPTION || type == BX_PRIVILEGED_SOFTWARE_INTERRUPT) {
      BX_ASSERT(vector < 32);
      event_SL = (BX_CPU_THIS_PTR msr.ia32_fred_stack_levels >> (vector*2)) & 0x3;
    }
  }
  Bit32u new_CSL = BX_MAX(event_SL, old_CSL);

  // determine new RSP
  Bit64u old_RSP = RSP;
  Bit64u new_RSP;
  if (CPL == 3 || new_CSL > old_CSL)
    new_RSP = BX_CPU_THIS_PTR msr.ia32_fred_rsp[new_CSL];
  else
    new_RSP = (RSP - (BX_CPU_THIS_PTR msr.ia32_fred_cfg & 0x1C0)) & ~BX_CONST64(0x3F); // decrement RSP and align to 64-bytes

  Bit32u old_flags = read_eflags();

#if BX_SUPPORT_CET
  Bit64u old_SSP = SSP;
  Bit64u new_SSP = 0;
  if (ShadowStackEnabled(0)) {
    if (CPL == 3 || new_CSL > old_CSL) {
      new_SSP = BX_CPU_THIS_PTR msr.ia32_fred_ssp[new_CSL];
      if (new_SSP & 0x4) {
        BX_ERROR(("FRED Event Delivery: Shadow Stack not 8-byte aligned !"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
    else {
      new_SSP = SSP - (BX_CPU_THIS_PTR msr.ia32_fred_cfg & 0x8);
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
  BX_CPU_THIS_PTR CSL = new_CSL;

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) SSP = new_SSP;
#endif

  // save state on stack
  // Save return state on new regular stack; memory accesses here have supervisor privilege
  write_new_stack_qword(new_RSP - 8,  0, 0);    // first 8 bytes pushed are all zeros
  write_new_stack_qword(new_RSP - 16, 0, 0 /*FIXME event_data*/);
  write_new_stack_qword(new_RSP - 24, 0, old_SS);
  write_new_stack_qword(new_RSP - 32, 0, old_RSP);
  write_new_stack_qword(new_RSP - 40, 0, old_flags);
  write_new_stack_qword(new_RSP - 48, 0, old_CS);
  write_new_stack_qword(new_RSP - 56, 0, old_RIP);
  write_new_stack_qword(new_RSP - 64, 0, error_code);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) {
    if (old_CPL == 0) {
      shadow_stack_write_dword(new_SSP - 4,  CPL, 0); // store 4 bytes of zeros to SSP-4
      new_SSP &= ~BX_CONST64(0x7);
      shadow_stack_write_qword(new_SSP - 8,  CPL, old_CS);
      shadow_stack_write_qword(new_SSP - 16, CPL, old_RIP);
      shadow_stack_write_qword(new_SSP - 24, CPL, old_SSP);
      new_SSP -= 24;
    }
    SSP = new_SSP;
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

  if (! IsCanonical(new_RIP) || 
        (temp_CS & BX_CONST64(0xFFFFFFFFFFF8FFFF)) != BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value ||
        (new_RFLAGS & BX_CONST64(0xFFFFFFFFFFC2802A)) != 2 ||
        (temp_SS & 0xFFF8FFFF) != BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector.value) // do not check ss_selector[63:32]
  {
    BX_ERROR(("ERETS: corrupted old state #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // ERETS will not numberically increase stack level
  Bit32u new_CSL = (temp_CS >> 16) & 0x3;
         new_CSL = BX_MIN(BX_CPU_THIS_PTR CSL, new_CSL);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) {
    Bit64u new_SSP = shadow_stack_restore(temp_CS, new_RIP);
    if (! IsCanonical(new_SSP)) {
      BX_ERROR(("ERETS: new SSP not canonical !"));
      exception(BX_GP_EXCEPTION, 0);
    }
    if (new_CSL < BX_CPU_THIS_PTR CSL && BX_CPU_THIS_PTR msr.ia32_fred_ssp[BX_CPU_THIS_PTR CSL] != SSP) {
      BX_ERROR(("IRETS changing stack level: SSP mismatch"));
      exception(BX_CP_EXCEPTION, BX_CP_FAR_RET_IRET);
    }
    SSP = new_SSP;
  }

  if (EndbranchEnabledAndNotSuppressed(0)) {
    bool IBT_restore = (temp_CS >> 18) & 0x1;
    if (IBT_restore)
      track_indirect(0);
  }
#endif

  RSP_COMMIT;

  RIP = new_RIP;
  setEFlags(new_RFLAGS);
  RSP = new_RSP;
  BX_CPU_THIS_PTR CSL = new_CSL;

  // update event-related state
  bool STI_block = (temp_SS >> 16) & 0x1;
  if (STI_block && BX_CPU_THIS_PTR get_IF() /* STI blocking was no in effect prior to the instruction execution */)
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS);

  bool pending_DB = (temp_SS >> 17) & 0x1;
  if (pending_DB && BX_CPU_THIS_PTR get_TF()) {
    // Schedule a debug trap (#DB) to be delivered after ERETS
    BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_SINGLE_STEP_BIT;
    BX_CPU_THIS_PTR async_event = 1;
  }
    
  bool NMI_unblock = (temp_SS >> 18) & 0x1;
  if (NMI_unblock)
    unmask_event(BX_EVENT_NMI);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::ERETU(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR cr4.get_FRED()) {
    BX_ERROR(("ERETU: FRED in not enabled in CR4"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (CPL > 0) {
    BX_ERROR(("ERETU: CPL must be 0"));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR CSL > 0) {
    BX_ERROR(("ERETU: CSL must be 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  RSP_SPECULATIVE;
  RSP += 8;                // skip error code so that RSP references the return state

  Bit64u new_RIP = pop_64();
  Bit64u temp_CS = pop_64();
  Bit64u new_RFLAGS = pop_64();
  Bit64u new_RSP = pop_64();
  Bit64u temp_SS = pop_64();

  if (! IsCanonical(new_RIP) || 
        (temp_CS & BX_CONST64(0xFFFFFFFFFFFF0003)) != 3 ||
        (new_RFLAGS & BX_CONST64(0xFFFFFFFFFFC2B02A)) != 2 ||
        (temp_SS & 0xFFF80003) != 3) // do not check ss_selector[63:32]
  {
    BX_ERROR(("ERETU: corrupted old state #GP(0)"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bool to_long_mode = false;

  if (((temp_CS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 16) &&
      ((temp_SS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 8))
  {
    // return to CPL3 in standard 64-bit configuration 
    to_long_mode = true;

    parse_selector(temp_CS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);
    setup_flat_CS(3, true);

    parse_selector(temp_SS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);
    setup_flat_SS(3);
  }
  else if (((temp_CS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48)) &&
           ((temp_SS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 8)) 
  {
    // return to CPL3 in standard compatibility mode configuration 
    to_long_mode = false;

    parse_selector(temp_CS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);
    setup_flat_CS(3, false);

    parse_selector(temp_SS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);
    setup_flat_SS(3);
  }
  else {
    // load newCS using tempCS[15:0]; // load each as is done by IRET, including
    // load newSS using tempSS[15:0]; // checks that may lead to a fault    
  }

  if (to_long_mode) {
    if (! IsCanonical(new_RIP)) {
      BX_ERROR(("ERETU: new RIP not canonical !"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }
  else {
    new_RIP &= 0xFFFFFFFF;
    new_RSP &= 0xFFFFFFFF;

    /* instruction pointer must be in code segment limit else #GP(0) */
    if (new_RIP > BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled) {
      BX_ERROR(("ERETU: RIP > limit"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(3)) {
    if (! to_long_mode && GET32H(BX_CPU_THIS_PTR msr.ia32_pl_ssp[3])) {
      BX_ERROR(("ERETU: attempt to return to compatibility mode while MSR_IA32_PL3_SSP[63:32] != 0"));
      exception(BX_GP_EXCEPTION, 0);
    }
    SSP = BX_CPU_THIS_PTR msr.ia32_pl_ssp[3];
  }

  if (ShadowStackEnabled(0) && BX_CPU_THIS_PTR msr.ia32_pl_ssp[0] != SSP) {
    BX_ERROR(("IRETU: supervisor shadow stack SSP mismatch"));
    exception(BX_CP_EXCEPTION, BX_CP_FAR_RET_IRET);
  }
#endif

  RSP_COMMIT;

  RIP = new_RIP;
  setEFlags(new_RFLAGS);
  RSP = new_RSP;
  CPL = 3;

  // CS = new_CS
  // SS = new_SS

  Bit64u temp_GS_base = MSR_GSBASE;
  MSR_GSBASE = BX_CPU_THIS_PTR msr.kernelgsbase;
  BX_CPU_THIS_PTR msr.kernelgsbase = temp_GS_base;

  bool pending_DB = (temp_SS >> 17) & 0x1;
  if (pending_DB && BX_CPU_THIS_PTR get_TF()) {
    // Schedule a debug trap (#DB) to be delivered after ERETU
    BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_SINGLE_STEP_BIT;
    BX_CPU_THIS_PTR async_event = 1;
  }
    
  bool NMI_unblock = (temp_SS >> 18) & 0x1;
  if (NMI_unblock)
    unmask_event(BX_EVENT_NMI);

  BX_NEXT_INSTR(i);
}

#endif
