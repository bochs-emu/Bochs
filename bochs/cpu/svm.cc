/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012 Stanislav Shwartsman
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

#if BX_SUPPORT_SVM

// When loading segment bases from the VMCB or the host save area
// (on VMRUN or #VMEXIT), segment bases are canonicalized (i.e.
// sign-extended from the highest implemented address bit to bit 63)
BX_CPP_INLINE Bit64u CanonicalizeAddress(Bit64u laddr)
{
  if (laddr & BX_CONST64(0x0000800000000000)) {
    return laddr | BX_CONST64(0xffff000000000000);
  }
  else {
    return laddr & BX_CONST64(0x0000ffffffffffff);
  }
}

BX_CPP_INLINE Bit8u BX_CPU_C::vmcb_read8(bx_phy_address vmcbaddr, unsigned offset)
{
  Bit32u val_8;
  bx_phy_address pAddr = vmcbaddr + offset;
  access_read_physical(pAddr, 1, (Bit8u*)(&val_8));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_VMCS_ACCESS | BX_READ, (Bit8u*)(&val_8));
  return val_8;
}

BX_CPP_INLINE Bit16u BX_CPU_C::vmcb_read16(bx_phy_address vmcbaddr, unsigned offset)
{
  Bit32u val_16;
  bx_phy_address pAddr = vmcbaddr + offset;
  access_read_physical(pAddr, 2, (Bit8u*)(&val_16));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 2, BX_VMCS_ACCESS | BX_READ, (Bit8u*)(&val_16));
  return val_16;
}

BX_CPP_INLINE Bit32u BX_CPU_C::vmcb_read32(bx_phy_address vmcbaddr, unsigned offset)
{
  Bit32u val_32;
  bx_phy_address pAddr = vmcbaddr + offset;
  access_read_physical(pAddr, 4, (Bit8u*)(&val_32));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 4, BX_VMCS_ACCESS | BX_READ, (Bit8u*)(&val_32));
  return val_32;
}

BX_CPP_INLINE Bit64u BX_CPU_C::vmcb_read64(bx_phy_address vmcbaddr, unsigned offset)
{
  Bit64u val_64;
  bx_phy_address pAddr = vmcbaddr + offset;
  access_read_physical(pAddr, 8, (Bit8u*)(&val_64));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 8, BX_VMCS_ACCESS | BX_READ, (Bit8u*)(&val_64));
  return val_64;
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write8(bx_phy_address vmcbaddr, unsigned offset, Bit8u val_8)
{
  bx_phy_address pAddr = vmcbaddr + offset;
  access_write_physical(pAddr, 1, (Bit8u*)(&val_8));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_VMCS_ACCESS | BX_WRITE, (Bit8u*)(&val_8));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write16(bx_phy_address vmcbaddr, unsigned offset, Bit16u val_16)
{
  bx_phy_address pAddr = vmcbaddr + offset;
  access_write_physical(pAddr, 2, (Bit8u*)(&val_16));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 2, BX_VMCS_ACCESS | BX_WRITE, (Bit8u*)(&val_16));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write32(bx_phy_address vmcbaddr, unsigned offset, Bit32u val_32)
{
  bx_phy_address pAddr = vmcbaddr + offset;
  access_write_physical(pAddr, 4, (Bit8u*)(&val_32));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 4, BX_VMCS_ACCESS | BX_WRITE, (Bit8u*)(&val_32));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write64(bx_phy_address vmcbaddr, unsigned offset, Bit64u val_64)
{
  bx_phy_address pAddr = vmcbaddr + offset;
  access_write_physical(pAddr, 8, (Bit8u*)(&val_64));
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 8, BX_VMCS_ACCESS | BX_WRITE, (Bit8u*)(&val_64));
}

BX_CPP_INLINE void BX_CPU_C::svm_segment_read(bx_phy_address vmcbaddr, bx_segment_reg_t *seg, unsigned offset)
{
  Bit16u selector = vmcb_read16(vmcbaddr, offset);
  Bit16u attr = vmcb_read16(vmcbaddr, offset + 2);
  Bit32u limit = vmcb_read32(vmcbaddr, offset + 4);
  bx_address base = CanonicalizeAddress(vmcb_read64(vmcbaddr, offset + 8));
  bx_bool valid = (attr >> 7) & 1;

  set_segment_ar_data(seg, valid, selector, base, limit,
       (attr & 0xff) | ((attr & 0xf00) << 8));
}

BX_CPP_INLINE void BX_CPU_C::svm_segment_write(bx_phy_address vmcb_addr, bx_segment_reg_t *seg, unsigned offset)
{
  Bit32u selector = seg->selector.value;
  bx_address base = seg->cache.u.segment.base;
  Bit32u limit = seg->cache.u.segment.limit_scaled;
  Bit32u attr = (seg->cache.valid) ? 
     (get_descriptor_h(&seg->cache) & 0x00f0ff00) : 0;

  vmcb_write16(vmcb_addr, offset, selector);
  vmcb_write16(vmcb_addr, offset + 2, ((attr >> 8) & 0xff) | ((attr >> 20) & 0xf));
  vmcb_write32(vmcb_addr, offset + 4, limit);
  vmcb_write64(vmcb_addr, offset + 8, base);
}

void BX_CPU_C::SvmEnterSaveHostState(SVM_HOST_STATE *host)
{
  for (int n=0;n < 4; n++)
    host->sregs[n] = BX_CPU_THIS_PTR sregs[n];

  host->gdtr = BX_CPU_THIS_PTR gdtr;
  host->idtr = BX_CPU_THIS_PTR idtr;

  host->efer = BX_CPU_THIS_PTR efer.get32();
  host->cr0 = BX_CPU_THIS_PTR cr0.get32();
  host->cr3 = BX_CPU_THIS_PTR cr3;
  host->cr4 = BX_CPU_THIS_PTR cr4.get32();
  host->eflags = read_eflags();
  host->rip = RIP;
  host->rsp = RSP;
  host->rax = RAX;
}

void BX_CPU_C::SvmExitLoadHostState(SVM_HOST_STATE *host)
{
  BX_CPU_THIS_PTR tsc_offset = 0;

  for (unsigned n=0;n < 4; n++) {
    BX_CPU_THIS_PTR sregs[n] = host->sregs[n];
    // we don't save selector details so parse selector again after loading
    parse_selector(BX_CPU_THIS_PTR sregs[n].selector.value, &BX_CPU_THIS_PTR sregs[n].selector);
  }

  BX_CPU_THIS_PTR gdtr = host->gdtr;
  BX_CPU_THIS_PTR idtr = host->idtr;

  BX_CPU_THIS_PTR efer.set32(host->efer);
  BX_CPU_THIS_PTR cr0.set32(host->cr0 | 0x1); // always set the CR0.PE
  BX_CPU_THIS_PTR cr3 = host->cr3;
  BX_CPU_THIS_PTR cr4.set32(host->cr4);

  BX_CPU_THIS_PTR dr7.set32(0x00000400);

  // set flags directly, avoid setEFlags side effects
  BX_CPU_THIS_PTR eflags = host->eflags & ~EFlagsVMMask; // ignore saved copy of EFLAGS.VM
  // Update lazy flags state
  setEFlagsOSZAPC(host->eflags);

  RIP = host->rip;
  RSP = host->rsp;
  RAX = host->rax;

  CPL = 0;

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  invalidate_prefetch_q();
#if BX_SUPPORT_ALIGNMENT_CHECK
  handleAlignmentCheck();
#endif
  handleCpuModeChange();
  handleSseModeChange();
#if BX_SUPPORT_AVX
  handleAvxModeChange();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);
}

bx_bool BX_CPU_C::SvmEnterLoadCheckControls(bx_phy_address vmcb_addr, SVM_CONTROLS *ctrls)
{
  ctrls->cr_rd_ctrl = vmcb_read16(vmcb_addr, SVM_CONTROL_INTERCEPT_CR_READ);
  ctrls->cr_wr_ctrl = vmcb_read16(vmcb_addr, SVM_CONTROL_INTERCEPT_CR_WRITE);

  ctrls->dr_rd_ctrl = vmcb_read16(vmcb_addr, SVM_CONTROL_INTERCEPT_DR_READ);
  ctrls->dr_wr_ctrl = vmcb_read16(vmcb_addr, SVM_CONTROL_INTERCEPT_DR_WRITE);

  ctrls->intercept_vector[0] = vmcb_read32(vmcb_addr, SVM_CONTROL_INTERCEPT1);
  ctrls->intercept_vector[1] = vmcb_read32(vmcb_addr, SVM_CONTROL_INTERCEPT2);

  if (! SVM_INTERCEPT(1, SVM_INTERCEPT1_VMRUN)) {
    BX_ERROR(("VMRUN: VMRUN intercept bit is not set!"));
    return 0;
  }

  ctrls->exceptions_intercept = vmcb_read32(vmcb_addr, SVM_CONTROL_INTERCEPT_EXCEPTIONS);

  // force 4K page alignment
  ctrls->iopm_base = PPFOf(vmcb_read64(vmcb_addr, SVM_CONTROL_IOPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->iopm_base)) {
    BX_ERROR(("VMRUN: invalid IOPM Base Address !"));
    return 0;
  }

  // force 4K page alignment
  ctrls->msrpm_base = PPFOf(vmcb_read64(vmcb_addr, SVM_CONTROL_MSRPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->msrpm_base)) {
    BX_ERROR(("VMRUN: invalid MSRPM Base Address !"));
    return 0;
  }

  Bit64u interrupt_control = vmcb_read32(vmcb_addr, SVM_CONTROL_VINTERRUPT_CONTROL);

  ctrls->v_tpr = (interrupt_control & 0xff);
  ctrls->v_irq = (interrupt_control >> 8) & 0x1;
  ctrls->v_intr_prio = (interrupt_control >> 16) & 0xf;
  ctrls->v_ignore_tpr = (interrupt_control >> 20) & 0x1;
  ctrls->v_intr_masking = (interrupt_control >> 24) & 0x1;
  ctrls->v_intr_vector = vmcb_read8(vmcb_addr, SVM_CONTROL_VINTERRUPT_VECTOR);

  return 1;
}

bx_bool BX_CPU_C::SvmEnterLoadCheckGuestState(bx_phy_address vmcb_addr)
{
  SVM_GUEST_STATE guest;
  Bit32u tmp;
  unsigned n;

  guest.efer.val32 = vmcb_read32(vmcb_addr, SVM_GUEST_EFER_MSR);
  tmp = vmcb_read32(vmcb_addr, SVM_GUEST_EFER_MSR_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest EFER[63:32] is not zero"));
    return 0;
  }

  if (guest.efer.val32 & ~BX_CPU_THIS_PTR efer_suppmask) {
    BX_ERROR(("VMRUN: Guest EFER reserved bits set"));
    return 0;
  }

  guest.cr0.val32 = vmcb_read32(vmcb_addr, SVM_GUEST_CR0);
  tmp = vmcb_read32(vmcb_addr, SVM_GUEST_CR0_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest CR0[63:32] is not zero"));
    return 0;
  }

  if (guest.cr0.get_NW() && !guest.cr0.get_CD()) {
    BX_ERROR(("VMRUN: Guest CR0.NW is set when CR0.CD is clear"));
    return 0;
  }

  guest.cr2 = vmcb_read64(vmcb_addr, SVM_GUEST_CR2);
  guest.cr3 = vmcb_read64(vmcb_addr, SVM_GUEST_CR3);

  guest.cr4.val32 = vmcb_read32(vmcb_addr, SVM_GUEST_CR4);
  tmp = vmcb_read32(vmcb_addr, SVM_GUEST_CR4_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest CR4[63:32] is not zero"));
    return 0;
  }

  if (guest.cr4.val32 & ~BX_CPU_THIS_PTR cr4_suppmask) {
    BX_ERROR(("VMRUN: Guest CR4 reserved bits set"));
    return 0;
  }

  guest.dr6 = vmcb_read32(vmcb_addr, SVM_GUEST_DR6);
  tmp = vmcb_read32(vmcb_addr, SVM_GUEST_DR6_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest DR6[63:32] is not zero"));
    return 0;
  }

  guest.dr7 = vmcb_read32(vmcb_addr, SVM_GUEST_DR7);
  tmp = vmcb_read32(vmcb_addr, SVM_GUEST_DR7_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest DR7[63:32] is not zero"));
    return 0;
  }

  for (n=0;n < 4; n++) {
    svm_segment_read(vmcb_addr, &guest.sregs[n], SVM_GUEST_ES_SELECTOR + n * 0x10);
  }

  if (! guest.sregs[BX_SEG_REG_CS].cache.valid || ! guest.sregs[BX_SEG_REG_CS].selector.value) {
    BX_ERROR(("VMRUN: VMCB null code segment"));
    return 0;
  }

  if (guest.sregs[BX_SEG_REG_CS].cache.u.segment.d_b && guest.sregs[BX_SEG_REG_CS].cache.u.segment.l) {
    BX_ERROR(("VMRUN: VMCB CS.D_B/L mismatch"));
    return 0;
  }

  if (! guest.sregs[BX_SEG_REG_SS].cache.valid || ! guest.sregs[BX_SEG_REG_SS].selector.value) {
    if (! guest.efer.get_LME()) {
      BX_ERROR(("VMRUN: VMCB null stack segment in 32-bit mode"));
      return 0;
    }
  }

  guest.gdtr.base = CanonicalizeAddress(vmcb_read64(vmcb_addr, SVM_GUEST_GDTR_BASE));
  guest.gdtr.limit = vmcb_read16(vmcb_addr, SVM_GUEST_GDTR_LIMIT);

  guest.idtr.base = CanonicalizeAddress(vmcb_read64(vmcb_addr, SVM_GUEST_GDTR_BASE));
  guest.idtr.limit = vmcb_read16(vmcb_addr, SVM_GUEST_GDTR_LIMIT);

  guest.eflags = vmcb_read32(vmcb_addr, SVM_GUEST_RFLAGS);
  guest.rip = vmcb_read64(vmcb_addr, SVM_GUEST_RIP);
  guest.rsp = vmcb_read64(vmcb_addr, SVM_GUEST_RSP);
  guest.rax = vmcb_read64(vmcb_addr, SVM_GUEST_RAX);

  guest.cpl = vmcb_read8(vmcb_addr, SVM_GUEST_CPL);

  guest.inhibit_interrupts = vmcb_read8(vmcb_addr, SVM_CONTROL_INTERRUPT_SHADOW) & 0x1;

  //
  // Load guest state
  //

  BX_CPU_THIS_PTR in_svm = 1;
  BX_CPU_THIS_PTR in_svm_guest = 1;
  BX_CPU_THIS_PTR svm_gif = 1;

  BX_CPU_THIS_PTR tsc_offset = vmcb_read64(vmcb_addr, SVM_CONTROL_TSC_OFFSET);

  BX_CPU_THIS_PTR efer.set32(guest.efer.get32());

  if (! check_CR0(guest.cr0.get32())) {
    BX_PANIC(("SVM: VMRUN CR0 is broken !"));
  }
  if (! check_CR4(guest.cr4.get32())) {
    BX_PANIC(("SVM: VMRUN CR4 is broken !"));
  }

  BX_CPU_THIS_PTR cr0.set32(guest.cr0.get32());
  BX_CPU_THIS_PTR cr4.set32(guest.cr4.get32());
  BX_CPU_THIS_PTR cr3 = guest.cr3;

  BX_CPU_THIS_PTR dr6.set32(guest.dr6);
  BX_CPU_THIS_PTR dr7.set32(guest.dr7 | 0x400);

  // flush TLB is always needed to invalidate possible
  // APIC ACCESS PAGE caching by host
  TLB_flush();

  for (n=0;n < 4; n++) {
    BX_CPU_THIS_PTR sregs[n] = guest.sregs[n];
  }

  BX_CPU_THIS_PTR gdtr = guest.gdtr;
  BX_CPU_THIS_PTR idtr = guest.idtr;
 
  RIP = guest.rip;
  RSP = guest.rsp;
  RAX = guest.rax;

  CPL = guest.cpl;

  if (guest.inhibit_interrupts) {
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS);
  }

  // set flags directly, avoid setEFlags side effects
  BX_CPU_THIS_PTR eflags = guest.eflags;
  // Update lazy flags state
  setEFlagsOSZAPC(guest.eflags);

  BX_CPU_THIS_PTR async_event = 0;
  if (guest.eflags & EFlagsTFMask)
    BX_CPU_THIS_PTR async_event = 1;

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  invalidate_prefetch_q();
#if BX_SUPPORT_ALIGNMENT_CHECK
  handleAlignmentCheck();
#endif
  handleCpuModeChange();
  handleSseModeChange();
#if BX_SUPPORT_AVX
  handleAvxModeChange();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);

  return 1;
}

void BX_CPU_C::Svm_Vmexit(int reason)
{
  if (!BX_CPU_THIS_PTR in_svm || !BX_CPU_THIS_PTR in_svm_guest) {
    if (reason != SVM_VMEXIT_INVALID)
      BX_PANIC(("PANIC: VMEXIT not in SVM guest mode !"));
  }

  // VMEXITs are FAULT-like: restore RIP/RSP to value before VMEXIT occurred
  RIP = BX_CPU_THIS_PTR prev_rip;
  if (BX_CPU_THIS_PTR speculative_rsp)
    RSP = BX_CPU_THIS_PTR prev_rsp;

  BX_CPU_THIS_PTR in_svm_guest = 0;
  BX_CPU_THIS_PTR svm_gif = 0;

  bx_phy_address vmcb_addr = BX_CPU_THIS_PTR vmcbptr;

  //
  // STEP 0: Update exit reason
  //

  vmcb_write64(vmcb_addr, SVM_CONTROL_EXITCODE, (Bit64u) ((Bit64s) reason));

  //
  // Step 1: Save guest state in the VMCB
  //
  for (unsigned n=0;n < 4; n++) {
    svm_segment_write(vmcb_addr, &BX_CPU_THIS_PTR sregs[n], SVM_GUEST_ES_SELECTOR + n * 0x10);
  }

  vmcb_write64(vmcb_addr, SVM_GUEST_GDTR_BASE, BX_CPU_THIS_PTR gdtr.base);
  vmcb_write16(vmcb_addr, SVM_GUEST_GDTR_LIMIT, BX_CPU_THIS_PTR gdtr.limit);

  vmcb_write64(vmcb_addr, SVM_GUEST_IDTR_BASE, BX_CPU_THIS_PTR idtr.base);
  vmcb_write16(vmcb_addr, SVM_GUEST_IDTR_LIMIT, BX_CPU_THIS_PTR idtr.limit);

  vmcb_write64(vmcb_addr, SVM_GUEST_EFER_MSR, BX_CPU_THIS_PTR efer.get32());
  vmcb_write64(vmcb_addr, SVM_GUEST_CR0, BX_CPU_THIS_PTR cr0.get32());
  vmcb_write64(vmcb_addr, SVM_GUEST_CR2, BX_CPU_THIS_PTR cr2);
  vmcb_write64(vmcb_addr, SVM_GUEST_CR3, BX_CPU_THIS_PTR cr3);
  vmcb_write64(vmcb_addr, SVM_GUEST_CR4, BX_CPU_THIS_PTR cr4.get32());

  vmcb_write64(vmcb_addr, SVM_GUEST_DR6, BX_CPU_THIS_PTR dr6.get32());
  vmcb_write64(vmcb_addr, SVM_GUEST_DR7, BX_CPU_THIS_PTR dr7.get32());

  vmcb_write64(vmcb_addr, SVM_GUEST_RAX, RAX);
  vmcb_write64(vmcb_addr, SVM_GUEST_RSP, RSP);
  vmcb_write64(vmcb_addr, SVM_GUEST_RIP, RIP);

  vmcb_write8(vmcb_addr, SVM_GUEST_CPL, CPL);

  if (interrupts_inhibited(BX_INHIBIT_INTERRUPTS)) {
    vmcb_write8(vmcb_addr, SVM_CONTROL_INTERRUPT_SHADOW, 0x01);
  }

  //
  // Step 2:
  //
  SvmExitLoadHostState(&BX_CPU_THIS_PTR vmcb.host_state);

  //
  // STEP 3: Go back to SVM host
  //

  BX_CPU_THIS_PTR errorno = 0;
  BX_CPU_THIS_PTR EXT = 0;

#if BX_DEBUGGER
  if (BX_CPU_THIS_PTR vmexit_break) {
    BX_CPU_THIS_PTR stop_reason = STOP_VMEXIT_BREAK_POINT;
    bx_debug_break(); // trap into debugger
  }
#endif

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

extern struct BxExceptionInfo exceptions_info[];

void BX_CPU_C::SvmInjectEvents(bx_phy_address vmcb_addr)
{
  Bit32u injecting_event = vmcb_read32(vmcb_addr, SVM_CONTROL_EVENT_INJECTION);
  if ((injecting_event & 0x80000000) == 0) return;

  /* the VMENTRY injecting event to the guest */
  unsigned vector = injecting_event & 0xff;
  unsigned type = (injecting_event >> 8) & 7;
  unsigned push_error = injecting_event & (1 << 11);
  unsigned error_code = push_error ? vmcb_read32(vmcb_addr, SVM_CONTROL_EVENT_INJECTION_ERRORCODE) : 0;

  switch(type) {
    case BX_EXTERNAL_INTERRUPT:
    case BX_NMI:
    case BX_HARDWARE_EXCEPTION:
      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_SOFTWARE_INTERRUPT:
      break;

    default:
      BX_PANIC(("SvmInjectEvents: unsupported event injection type %d !", type));
  }

  BX_ERROR(("SvmInjectEvents: Injecting vector 0x%02x (error_code 0x%04x)", vector, error_code));

  if (type == BX_HARDWARE_EXCEPTION) {
    // record exception the same way as BX_CPU_C::exception does
    if (vector < BX_CPU_HANDLED_EXCEPTIONS)
      BX_CPU_THIS_PTR curr_exception = exceptions_info[vector].exception_type;
    else // else take default value
      BX_CPU_THIS_PTR curr_exception = exceptions_info[BX_CPU_HANDLED_EXCEPTIONS].exception_type;

    BX_CPU_THIS_PTR errorno = 1;
  }

//vm->idt_vector_info = vm->vmentry_interr_info & ~0x80000000;
//vm->idt_vector_error_code = error_code;

  RSP_SPECULATIVE;

  if (type == BX_SOFTWARE_INTERRUPT) {
     if (v8086_mode()) {
       // redirect interrupt through virtual-mode idt
       if (v86_redirect_interrupt(vector)) goto done;
     }
  }

  interrupt(vector, type, push_error, error_code);

done:

  RSP_COMMIT;

  BX_CPU_THIS_PTR errorno = 0; // injection success
  BX_CPU_THIS_PTR EXT = 0;
}

void BX_CPU_C::SvmInterceptException(bxInstruction_c *i, unsigned vector, Bit16u errcode, bx_bool errcode_valid, Bit64u qualification)
{
}

#define SVM_VMEXIT_IO_PORTIN        (1 << 0)
#define SVM_VMEXIT_IO_INSTR_STRING  (1 << 2)
#define SVM_VMEXIT_IO_INSTR_REP     (1 << 3)
#define SVM_VMEXIT_IO_INSTR_LEN8    (1 << 4)
#define SVM_VMEXIT_IO_INSTR_LEN16   (1 << 5)
#define SVM_VMEXIT_IO_INSTR_LEN32   (1 << 6)

void BX_CPU_C::SvmInterceptIO(bxInstruction_c *i, unsigned port, unsigned len)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  if (! SVM_INTERCEPT(0, SVM_INTERCEPT0_IO)) return;

  Bit8u bitmap[2];
  bx_phy_address pAddr;

  // access_read_physical cannot read 2 bytes cross 4K boundary :(
  pAddr = BX_CPU_THIS_PTR vmcb.ctrls.iopm_base + (port / 8);
  access_read_physical(pAddr, 1, &bitmap[0]);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_IO_BITMAP_ACCESS | BX_READ, &bitmap[0]);

  pAddr++;
  access_read_physical(pAddr, 1, &bitmap[1]);
  BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, pAddr, 1, BX_IO_BITMAP_ACCESS | BX_READ, &bitmap[1]);

  Bit16u combined_bitmap = bitmap[1];
  combined_bitmap = (combined_bitmap << 8) | bitmap[0];

  unsigned mask = ((1 << len) - 1) << (port & 7);
  if (combined_bitmap & mask) {
    BX_ERROR(("SVM VMEXIT: I/O port 0x%04x", port));

    Bit32u qualification = 0;

    switch(i->getIaOpcode()) {
      case BX_IA_IN_ALIb:
      case BX_IA_IN_AXIb:
      case BX_IA_IN_EAXIb:
      case BX_IA_IN_ALDX:
      case BX_IA_IN_AXDX:
      case BX_IA_IN_EAXDX:
        qualification = SVM_VMEXIT_IO_PORTIN;
        break;

      case BX_IA_OUT_IbAL:
      case BX_IA_OUT_IbAX:
      case BX_IA_OUT_IbEAX:
      case BX_IA_OUT_DXAL:
      case BX_IA_OUT_DXAX:
      case BX_IA_OUT_DXEAX:
        qualification = 0; // PORTOUT
        break;

      case BX_IA_REP_INSB_YbDX:
      case BX_IA_REP_INSW_YwDX:
      case BX_IA_REP_INSD_YdDX:
        qualification = SVM_VMEXIT_IO_PORTIN | SVM_VMEXIT_IO_INSTR_STRING;
        if (i->repUsedL())
           qualification |= SVM_VMEXIT_IO_INSTR_REP;
        break;

      case BX_IA_REP_OUTSB_DXXb:
      case BX_IA_REP_OUTSW_DXXw:
      case BX_IA_REP_OUTSD_DXXd:
        qualification = SVM_VMEXIT_IO_INSTR_STRING; // PORTOUT
        if (i->repUsedL())
           qualification |= SVM_VMEXIT_IO_INSTR_REP;
        break;

      default:
        BX_PANIC(("VMexit_IO: I/O instruction %s unknown", i->getIaOpcodeName()));
    }

    qualification |= (port << 16);
    if (len == 1)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN8;
    else if (len == 2)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN16;
    else if (len == 4)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN32;

    vmcb_write64(BX_CPU_THIS_PTR vmcbptr, SVM_CONTROL_EXITINFO1, qualification);
    Svm_Vmexit(SVM_VMEXIT_IO);
  }
}

void BX_CPU_C::SvmInterceptMSR(bxInstruction_c *i, unsigned op, Bit32u msr)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  if (! SVM_INTERCEPT(0, SVM_INTERCEPT0_MSR)) return;

  BX_ASSERT(op == BX_READ || op == BX_WRITE);

  bx_bool vmexit = 1;

  int msr_map_offset = -1;
  if (msr <= 0x1fff) {
    msr_map_offset = 0;
  }
  else if (msr <= 0xc0001fff) {
    msr_map_offset = 2048;
  }
  else if (msr <= 0xc0011fff) {
    msr_map_offset = 4096;
  }

  if (msr_map_offset >= 0) {
    bx_phy_address msr_bitmap_addr = BX_CPU_THIS_PTR vmcb.ctrls.msrpm_base + msr_map_offset;
    Bit32u msr_offset = (msr & 0x1fff) * 2 + op;

    Bit8u msr_bitmap;
    access_read_physical(msr_bitmap_addr + (msr_offset / 8), 1, (Bit8u*)(&msr_bitmap));
    BX_DBG_PHY_MEMORY_ACCESS(BX_CPU_ID, msr_bitmap_addr + (msr_offset / 8), 1, BX_MSR_BITMAP_ACCESS | BX_READ, &msr_bitmap);

    vmexit = (msr_bitmap >> (msr_offset & 7)) & 0x1;
  }

  if (vmexit) {
    vmcb_write64(BX_CPU_THIS_PTR vmcbptr, SVM_CONTROL_EXITINFO1, op); // 0 - RDMSR, 1 - WRMSR
    Svm_Vmexit(SVM_VMEXIT_MSR);
  }
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMRUN(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("VMRUN with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (SVM_INTERCEPT(1, SVM_INTERCEPT1_VMRUN))
    Svm_Vmexit(SVM_VMEXIT_VMRUN);

  bx_address pAddr = RAX & i->asize_mask();
  if ((pAddr & 0xfff) != 0 || ! IsValidPhyAddr(pAddr)) {
    BX_ERROR(("VMRUN: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }
  BX_CPU_THIS_PTR vmcbptr = pAddr;

  //
  // Step 1: Save host state to physical memory indicated in SVM_HSAVE_PHY_ADDR_MSR
  //
  SvmEnterSaveHostState(&BX_CPU_THIS_PTR vmcb.host_state);

  //
  // Step 2: Load control information from the VMCB
  //
  if (!SvmEnterLoadCheckControls(pAddr, &BX_CPU_THIS_PTR vmcb.ctrls))
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  //
  // Step 3: Load guest state from the VMCB and enter SVM
  //
  if (!SvmEnterLoadCheckGuestState(pAddr))
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  //
  // Step 4: Inject events to the guest
  //
  SvmInjectEvents(pAddr);
#endif

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMMCALL(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR efer.get_SVME()) {
    if (BX_CPU_THIS_PTR in_svm_guest) {
      if (SVM_INTERCEPT(1, SVM_INTERCEPT1_VMMCALL)) Svm_Vmexit(SVM_VMEXIT_VMMCALL);
    }
  }

  exception(BX_UD_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMLOAD(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("VMLOAD with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(1, SVM_INTERCEPT1_VMLOAD)) Svm_Vmexit(SVM_VMEXIT_VMLOAD);
  }

  bx_address pAddr = RAX & i->asize_mask();
  if ((pAddr & 0xfff) != 0 || ! IsValidPhyAddr(pAddr)) {
    BX_ERROR(("VMLOAD: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_segment_reg_t fs, gs, guest_tr, guest_ldtr;

  svm_segment_read(pAddr, &fs, SVM_GUEST_FS_SELECTOR);
  svm_segment_read(pAddr, &gs, SVM_GUEST_GS_SELECTOR);
  svm_segment_read(pAddr, &guest_tr, SVM_GUEST_TR_SELECTOR);
  svm_segment_read(pAddr, &guest_ldtr, SVM_GUEST_LDTR_SELECTOR);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS] = fs;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS] = gs;
  BX_CPU_THIS_PTR tr = guest_tr;
  BX_CPU_THIS_PTR ldtr = guest_ldtr;

  MSR_KERNELGSBASE = CanonicalizeAddress(vmcb_read64(pAddr, SVM_GUEST_KERNEL_GSBASE_MSR));
  MSR_STAR = vmcb_read64(pAddr, SVM_GUEST_STAR_MSR);
  MSR_LSTAR = CanonicalizeAddress(vmcb_read64(pAddr, SVM_GUEST_LSTAR_MSR));
  MSR_CSTAR = CanonicalizeAddress(vmcb_read64(pAddr, SVM_GUEST_CSTAR_MSR));
  MSR_FMASK = vmcb_read64(pAddr, SVM_GUEST_SFMASK_MSR);

  BX_CPU_THIS_PTR msr.sysenter_cs_msr = vmcb_read64(pAddr, SVM_GUEST_SYSENTER_CS_MSR);
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = CanonicalizeAddress(vmcb_read64(pAddr, SVM_GUEST_SYSENTER_EIP_MSR));
  BX_CPU_THIS_PTR msr.sysenter_esp_msr = CanonicalizeAddress(vmcb_read64(pAddr, SVM_GUEST_SYSENTER_ESP_MSR));
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("VMSAVE with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(1, SVM_INTERCEPT1_VMSAVE)) Svm_Vmexit(SVM_VMEXIT_VMSAVE);
  }

  bx_address pAddr = RAX & i->asize_mask();
  if ((pAddr & 0xfff) != 0 || ! IsValidPhyAddr(pAddr)) {
    BX_ERROR(("VMSAVE: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }

  svm_segment_write(pAddr, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], SVM_GUEST_FS_SELECTOR);
  svm_segment_write(pAddr, &BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], SVM_GUEST_GS_SELECTOR);
  svm_segment_write(pAddr, &BX_CPU_THIS_PTR tr, SVM_GUEST_TR_SELECTOR);
  svm_segment_write(pAddr, &BX_CPU_THIS_PTR ldtr, SVM_GUEST_LDTR_SELECTOR);

  vmcb_write64(pAddr, SVM_GUEST_KERNEL_GSBASE_MSR, MSR_KERNELGSBASE);
  vmcb_write64(pAddr, SVM_GUEST_STAR_MSR, MSR_STAR);
  vmcb_write64(pAddr, SVM_GUEST_LSTAR_MSR, MSR_LSTAR);
  vmcb_write64(pAddr, SVM_GUEST_CSTAR_MSR, MSR_CSTAR);
  vmcb_write64(pAddr, SVM_GUEST_SFMASK_MSR, MSR_FMASK);

  vmcb_write64(pAddr, SVM_GUEST_SYSENTER_CS_MSR, BX_CPU_THIS_PTR msr.sysenter_cs_msr);
  vmcb_write64(pAddr, SVM_GUEST_SYSENTER_ESP_MSR, BX_CPU_THIS_PTR msr.sysenter_esp_msr);
  vmcb_write64(pAddr, SVM_GUEST_SYSENTER_EIP_MSR, BX_CPU_THIS_PTR msr.sysenter_eip_msr);
#endif

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::SKINIT(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("SKINIT with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(1, SVM_INTERCEPT1_SKINIT)) Svm_Vmexit(SVM_VMEXIT_SKINIT);
  }

  BX_PANIC(("SVM: SKINIT is not implemented yet"));
#endif

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CLGI(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("CLGI with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(1, SVM_INTERCEPT1_CLGI)) Svm_Vmexit(SVM_VMEXIT_CLGI);
  }

  BX_CPU_THIS_PTR svm_gif = 0;
  BX_CPU_THIS_PTR async_event = 1;
#endif

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::STGI(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("STGI with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(1, SVM_INTERCEPT1_STGI)) Svm_Vmexit(SVM_VMEXIT_STGI);
  }

  BX_CPU_THIS_PTR svm_gif = 1;
  BX_CPU_THIS_PTR async_event = 1;
#endif

  BX_NEXT_TRACE(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::INVLPGA(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("INVLPGA with CPL != 0"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(0, SVM_INTERCEPT0_INVLPGA)) Svm_Vmexit(SVM_VMEXIT_INVLPGA);
  }
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_SVM
void BX_CPU_C::register_svm_state(bx_param_c *parent)
{
  if (! bx_cpuid_support_svm()) return;

  // register SVM state for save/restore param tree
  bx_list_c *svm = new bx_list_c(parent, "SVM", 5);

  BXRS_PARAM_BOOL(svm, in_svm, BX_CPU_THIS_PTR in_svm);
  BXRS_PARAM_BOOL(svm, in_svm_guest, BX_CPU_THIS_PTR in_svm_guest);
  BXRS_PARAM_BOOL(svm, gif, BX_CPU_THIS_PTR svm_gif);

  //
  // VMCB Control Fields
  //

  bx_list_c *vmcb_ctrls = new bx_list_c(svm, "VMCB_CTRLS", 16);

//Bit8u v_tpr;
//bx_bool v_irq;
//Bit8u v_intr_prio;
//bx_bool v_ignore_tpr;
//bx_bool v_intr_masking;
//Bit8u v_intr_vector;

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_rd_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.cr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_wr_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.cr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_rd_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.dr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_wr_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.dr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exceptions_intercept, BX_CPU_THIS_PTR vmcb.ctrls.exceptions_intercept);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector0, BX_CPU_THIS_PTR vmcb.ctrls.intercept_vector[0]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector1, BX_CPU_THIS_PTR vmcb.ctrls.intercept_vector[1]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, tsc_offset, BX_CPU_THIS_PTR vmcb.ctrls.tsc_offset);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, iopm_base, BX_CPU_THIS_PTR vmcb.ctrls.iopm_base);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, msrpm_base, BX_CPU_THIS_PTR vmcb.ctrls.msrpm_base);

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_tpr, BX_CPU_THIS_PTR vmcb.ctrls.v_tpr);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_irq, BX_CPU_THIS_PTR vmcb.ctrls.v_irq);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_prio, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_prio);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_ignore_tpr, BX_CPU_THIS_PTR vmcb.ctrls.v_ignore_tpr);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_intr_masking, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_masking);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_vector, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_vector);

  //
  // VMCB Host State
  //

  bx_list_c *host = new bx_list_c(svm, "VMCB_HOST_STATE", 14);

  for(unsigned n=0; n<4; n++) {
    bx_segment_reg_t *segment = &BX_CPU_THIS_PTR vmcb.host_state.sregs[n];
    bx_list_c *sreg = new bx_list_c(host, strseg(segment), 12);
    BXRS_HEX_PARAM_FIELD(sreg, selector, segment->selector.value);
    BXRS_HEX_PARAM_FIELD(sreg, valid, segment->cache.valid);
    BXRS_PARAM_BOOL(sreg, p, segment->cache.p);
    BXRS_HEX_PARAM_FIELD(sreg, dpl, segment->cache.dpl);
    BXRS_PARAM_BOOL(sreg, segment, segment->cache.segment);
    BXRS_HEX_PARAM_FIELD(sreg, type, segment->cache.type);
    BXRS_HEX_PARAM_FIELD(sreg, base, segment->cache.u.segment.base);
    BXRS_HEX_PARAM_FIELD(sreg, limit_scaled, segment->cache.u.segment.limit_scaled);
    BXRS_PARAM_BOOL(sreg, granularity, segment->cache.u.segment.g);
    BXRS_PARAM_BOOL(sreg, d_b, segment->cache.u.segment.d_b);
#if BX_SUPPORT_X86_64
    BXRS_PARAM_BOOL(sreg, l, segment->cache.u.segment.l);
#endif
    BXRS_PARAM_BOOL(sreg, avl, segment->cache.u.segment.avl);
  }

  bx_list_c *GDTR = new bx_list_c(host, "GDTR", 2);
  BXRS_HEX_PARAM_FIELD(GDTR, base, BX_CPU_THIS_PTR vmcb.host_state.gdtr.base);
  BXRS_HEX_PARAM_FIELD(GDTR, limit, BX_CPU_THIS_PTR vmcb.host_state.gdtr.limit);

  bx_list_c *IDTR = new bx_list_c(host, "IDTR", 2);
  BXRS_HEX_PARAM_FIELD(IDTR, base, BX_CPU_THIS_PTR vmcb.host_state.idtr.base);
  BXRS_HEX_PARAM_FIELD(IDTR, limit, BX_CPU_THIS_PTR vmcb.host_state.idtr.limit);

  BXRS_HEX_PARAM_FIELD(host, efer, BX_CPU_THIS_PTR vmcb.host_state.efer);
  BXRS_HEX_PARAM_FIELD(host, cr0, BX_CPU_THIS_PTR vmcb.host_state.cr0);
  BXRS_HEX_PARAM_FIELD(host, cr3, BX_CPU_THIS_PTR vmcb.host_state.cr3);
  BXRS_HEX_PARAM_FIELD(host, cr4, BX_CPU_THIS_PTR vmcb.host_state.cr4);
  BXRS_HEX_PARAM_FIELD(host, eflags, BX_CPU_THIS_PTR vmcb.host_state.eflags);
  BXRS_HEX_PARAM_FIELD(host, rip, BX_CPU_THIS_PTR vmcb.host_state.rip);
  BXRS_HEX_PARAM_FIELD(host, rsp, BX_CPU_THIS_PTR vmcb.host_state.rsp);
  BXRS_HEX_PARAM_FIELD(host, rax, BX_CPU_THIS_PTR vmcb.host_state.rax);
}
#endif
