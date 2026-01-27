/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2026 Stanislav Shwartsman
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

void BX_CPU_C::FRED_EventDelivery(Bit8u vector, unsigned type, Bit16u error_code)
{
#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = true;
#endif

  Bit32u old_CPL = CPL;
  Bit32u old_CSL = (CPL == 3) ? 0 : CSL;

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
  // old_SS[63:32] contains FRED event information
  old_SS |= Bit64u(BX_CPU_THIS_PTR fred_event_info) << 32;

  Bit64u old_RIP = RIP;
  Bit64u new_RIP = BX_CPU_THIS_PTR msr.ia32_fred_cfg & ~BX_CONST64(0xfff);
  if (CPL == 0)
    new_RIP += 256;
  if (! IsCanonical(new_RIP)) {
    BX_ERROR(("FRED Event Delivery: non canonical value in BX_MSR_IA32_FRED_CONFIG !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bool nested = (BX_CPU_THIS_PTR fred_event_info & (1 << 26)) != 0;

  // if CPL == 3 AND event is not an exception nested on event delivery AND event is not #DF
  Bit32u event_SL = 0;
  if (CPL == 3 && !nested && vector != BX_DF_EXCEPTION) {
    event_SL = 0;
  }
  else {
    switch (type) {
    case BX_EXTERNAL_INTERRUPT:
      event_SL = (BX_CPU_THIS_PTR msr.ia32_fred_cfg >> 9) & 0x3;
      break;
    case BX_NMI:
    case BX_HARDWARE_EXCEPTION:
    case BX_SOFTWARE_EXCEPTION:
    case BX_PRIVILEGED_SOFTWARE_INTERRUPT:
      BX_ASSERT(vector < 32);
      event_SL = (BX_CPU_THIS_PTR msr.ia32_fred_stack_levels >> (vector*2)) & 0x3;
      break;
    case BX_SOFTWARE_INTERRUPT:
    case BX_EVENT_OTHER:
      event_SL = 0;
      break;
    default:
      BX_ASSERT(0);
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

  // ESTABLISH NEW CONTEXT: OLD STATE WILL BE RESTORED IF THERE IS A SUBSEQUENT EXCEPTION -> FIXME: HOW ???

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

    swapgs();
  }

  // update registers defining context
  RIP = new_RIP;
  BX_CPU_THIS_PTR eflags = 0x2; // Clear EFLAGS, bit1 is always set
  clearEFlagsOSZAPC();	        // update lazy flags state
  RSP = new_RSP;
  set_CSL(new_CSL);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) SSP = new_SSP;
#endif

  // save state on stack
  // Save return state on new regular stack; memory accesses here have supervisor privilege
  write_new_stack_qword(new_RSP - 8,  0, 0);    // first 8 bytes pushed are all zeros
  write_new_stack_qword(new_RSP - 16, 0, BX_CPU_THIS_PTR fred_event_data);
  write_new_stack_qword(new_RSP - 24, 0, old_SS);
  write_new_stack_qword(new_RSP - 32, 0, old_RSP);
  write_new_stack_qword(new_RSP - 40, 0, old_flags);
  write_new_stack_qword(new_RSP - 48, 0, old_CS);
  write_new_stack_qword(new_RSP - 56, 0, old_RIP);
  write_new_stack_qword(new_RSP - 64, 0, error_code);
  new_RSP -= 64;

#if BX_SUPPORT_CET
  if (BX_CPU_THIS_PTR cr4.get_CET()) {
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
    }

    reset_endbranch_tracker(0);
  }
#endif

  // update event related state

  if (type == BX_NMI) {
#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.NMI_EXITING()) {
      if (BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI()) mask_event(BX_EVENT_VMX_VIRTUAL_NMI);
    }
    else
#endif
      mask_event(BX_EVENT_NMI);
  }

  CPL = 0;
  BX_CPU_THIS_PTR debug_trap = 0;
  BX_CPU_THIS_PTR fred_event_info = 0;
  BX_CPU_THIS_PTR fred_event_data = 0;

#if BX_SUPPORT_VMX || BX_SUPPORT_SVM
  BX_CPU_THIS_PTR in_event = false;
#endif
}

Bit64u BX_CPP_AttrRegparmN(2) BX_CPU_C::get_fred_event_data(Bit8u vector, unsigned type)
{
  if (type == BX_HARDWARE_EXCEPTION) {
    if (vector == BX_PF_EXCEPTION)
      return BX_CPU_THIS_PTR cr2;

    if (vector == BX_NM_EXCEPTION)
      return 0; // until MSR_XFD_ERR is implemented
  }

  if (vector == BX_DB_EXCEPTION)
    return BX_CPU_THIS_PTR debug_trap & 0x0000400f;

  if (type == BX_NMI)
    return 0; // until NMI source reporting is implemented

  return 0;
}

Bit32u BX_CPU_C::get_fred_event_info(Bit8u vector, unsigned type, bool nested_exception, unsigned ilen)
{
  Bit32u event_info = vector | (type << 16);

  if (long64_mode())
    event_info |= (1 << 25);

  if (vector != BX_DF_EXCEPTION) {
    if (nested_exception) event_info |= (1 << 26);
  }

  // add ilen() of instruction caused the event to bits [31:28] for INTn, INT1, INT3/INTO, SYSCALL and SYSENTER
  event_info |= (ilen << 28);

  return event_info;
}

void BX_CPU_C::set_fred_event_info_and_data(Bit8u vector, unsigned type, bool nested_exception, unsigned ilen)
{
  BX_CPU_THIS_PTR fred_event_info = get_fred_event_info(vector, type, nested_exception, ilen);
  BX_CPU_THIS_PTR fred_event_data = get_fred_event_data(vector, type);
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
         new_CSL = BX_MIN(CSL, new_CSL);

#if BX_SUPPORT_CET
  if (ShadowStackEnabled(0)) {
    Bit64u new_SSP = shadow_stack_restore(temp_CS, new_RIP);
    if (! IsCanonical(new_SSP)) {
      BX_ERROR(("ERETS: new SSP not canonical !"));
      exception(BX_GP_EXCEPTION, 0);
    }
    if (new_CSL < CSL && BX_CPU_THIS_PTR msr.ia32_fred_ssp[CSL] != SSP) {
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
  set_CSL(new_CSL);

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
  if (NMI_unblock) {
#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.NMI_EXITING()) {
      if (BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI()) unmask_event(BX_EVENT_VMX_VIRTUAL_NMI);
    }
    else
#endif
      unmask_event(BX_EVENT_NMI);
  }

  BX_NEXT_TRACE(i);
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

  if (CSL > 0) {
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

  bool to_long_mode = false, flat = false;
  bx_selector_t cs_selector, ss_selector;
  bx_descriptor_t cs_descriptor, ss_descriptor;
  Bit16u raw_cs_selector = temp_CS & 0xffff;
  Bit16u raw_ss_selector = temp_SS & 0xffff;

  if (((temp_CS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 16) &&
      ((temp_SS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 8))
  {
    // return to CPL3 in standard 64-bit configuration
    to_long_mode = true;
    flat = true;
  }
  else if (((temp_CS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48)) &&
           ((temp_SS & 0x7FFF) == (BX_CPU_THIS_PTR msr.star >> 48) + 8))
  {
    // return to CPL3 in standard compatibility mode configuration
    to_long_mode = false;
    flat = true;
  }
  else {
    // load newCS using tempCS[15:0];
    // load newSS using tempSS[15:0];
    // load same way as if it was done by IRET, including checks that may lead to a fault
    Bit32u dword1, dword2;

    parse_selector(raw_cs_selector, &cs_selector);
    // return CS selector must be non-null, else #GP(0)
    if ((raw_cs_selector & 0xfffc) == 0) {
      BX_ERROR(("ERETU: return CS selector null"));
      exception(BX_GP_EXCEPTION, 0);
    }

    fetch_raw_descriptor(&cs_selector, &dword1, &dword2, BX_GP_EXCEPTION);
    parse_descriptor(dword1, dword2, &cs_descriptor);

    // return CS selector RPL must be >= CPL, else #GP(return selector)
    if (cs_selector.rpl < CPL) {
      BX_ERROR(("ERETU: return selector RPL < CPL"));
      exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc);
    }

    // check code-segment descriptor
    check_cs(&cs_descriptor, raw_cs_selector, 0, cs_selector.rpl);

    parse_selector(raw_ss_selector, &ss_selector);
    fetch_ss_descriptor(raw_ss_selector, &ss_selector, &ss_descriptor, cs_selector.rpl, BX_GP_EXCEPTION);
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

  if (flat) {
    parse_selector(temp_CS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector);
    parse_selector(temp_SS & 0x7FFF, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS].selector);

    setup_flat_CS(3, to_long_mode);
    setup_flat_SS(3);
  }
  else {
    load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);

    if ((raw_ss_selector & 0xfffc) != 0) {
      // load SS:RSP from stack
      // load the SS-cache with SS descriptor
      load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
    }
    else {
      // we are in 64-bit mode !
      load_null_selector(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_SS], raw_ss_selector);
    }
  }

  RSP_COMMIT;

  RIP = new_RIP;
  setEFlags(new_RFLAGS);
  RSP = new_RSP;
  CPL = 3;

  // CS = new_CS
  // SS = new_SS

  swapgs();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_umonitor();
#endif

  bool pending_DB = (temp_SS >> 17) & 0x1;
  if (pending_DB && BX_CPU_THIS_PTR get_TF()) {
    // Schedule a debug trap (#DB) to be delivered after ERETU
    BX_CPU_THIS_PTR debug_trap |= BX_DEBUG_SINGLE_STEP_BIT;
    BX_CPU_THIS_PTR async_event = 1;
  }

  bool NMI_unblock = (temp_SS >> 18) & 0x1;
  if (NMI_unblock) {
#if BX_SUPPORT_VMX
    if (BX_CPU_THIS_PTR in_vmx_guest && BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.NMI_EXITING()) {
      if (BX_CPU_THIS_PTR vmcs.pin_vmexec_ctrls.VIRTUAL_NMI()) unmask_event(BX_EVENT_VMX_VIRTUAL_NMI);
    }
    else
#endif
      unmask_event(BX_EVENT_NMI);
  }

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LKGS_Ew(bxInstruction_c *i)
{
  if (CPL > 0) {
    BX_ERROR(("LKGS_Ew: CPL must be 0"));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u segsel;

  if (i->modC0()) {
    segsel = BX_READ_16BIT_REG(i->dst());
  }
  else {
    bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
    segsel  = read_linear_word(i->seg(), get_laddr64(i->seg(), eaddr));
  }

  // back up current GS segment base into MSR_KERNEL_GS_BASE
  swapgs();

  load_seg_reg(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], segsel);

  // restore old GS segment base and put new loaded base into MSR_KERNEL_GS_BASE
  swapgs();

  BX_NEXT_INSTR(i);
}

#endif
