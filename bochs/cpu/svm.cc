/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2018 Stanislav Shwartsman
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

#include "decoder/ia_opcodes.h"

extern const char *segname[];

void BX_CPU_C::set_VMCBPTR(Bit64u vmcbptr)
{
  BX_CPU_THIS_PTR vmcbptr = vmcbptr;

  if (vmcbptr != 0) {
    BX_CPU_THIS_PTR vmcbhostptr = BX_CPU_THIS_PTR getHostMemAddr(vmcbptr, BX_WRITE);
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR vmcb_memtype = resolve_memtype(BX_CPU_THIS_PTR vmcbptr);
#endif
  }
  else {
    BX_CPU_THIS_PTR vmcbhostptr = 0;
#if BX_SUPPORT_MEMTYPE
    BX_CPU_THIS_PTR vmcb_memtype = BX_MEMTYPE_UC;
#endif
  }
}

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

BX_CPP_INLINE Bit8u BX_CPU_C::vmcb_read8(unsigned offset)
{
  Bit8u val_8;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit8u *hostAddr = (Bit8u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_8 = *hostAddr;
  }
  else {
    access_read_physical(pAddr, 1, (Bit8u*)(&val_8));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_8));
  return val_8;
}

BX_CPP_INLINE Bit16u BX_CPU_C::vmcb_read16(unsigned offset)
{
  Bit16u val_16;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_16 = ReadHostWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 2, (Bit8u*)(&val_16));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_16));
  return val_16;
}

BX_CPP_INLINE Bit32u BX_CPU_C::vmcb_read32(unsigned offset)
{
  Bit32u val_32;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_32 = ReadHostDWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 4, (Bit8u*)(&val_32));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_32));
  return val_32;
}

BX_CPP_INLINE Bit64u BX_CPU_C::vmcb_read64(unsigned offset)
{
  Bit64u val_64;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_64 = ReadHostQWordFromLittleEndian(hostAddr);
  }
  else {
    access_read_physical(pAddr, 8, (Bit8u*)(&val_64));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_64));
  return val_64;
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write8(unsigned offset, Bit8u val_8)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit8u *hostAddr = (Bit8u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 1);
    *hostAddr = val_8;
  }
  else {
    access_write_physical(pAddr, 1, (Bit8u*)(&val_8));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_8));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write16(unsigned offset, Bit16u val_16)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 2);
    WriteHostWordToLittleEndian(hostAddr, val_16);
  }
  else {
    access_write_physical(pAddr, 2, (Bit8u*)(&val_16));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_16));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write32(unsigned offset, Bit32u val_32)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 4);
    WriteHostDWordToLittleEndian(hostAddr, val_32);
  }
  else {
    access_write_physical(pAddr, 4, (Bit8u*)(&val_32));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_32));
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write64(unsigned offset, Bit64u val_64)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 8);
    WriteHostQWordToLittleEndian(hostAddr, val_64);
  }
  else {
    access_write_physical(pAddr, 8, (Bit8u*)(&val_64));
  }

  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_64));
}

BX_CPP_INLINE void BX_CPU_C::svm_segment_read(bx_segment_reg_t *seg, unsigned offset)
{
  Bit16u selector = vmcb_read16(offset);
  Bit16u attr = vmcb_read16(offset + 2);
  Bit32u limit = vmcb_read32(offset + 4);
  bx_address base = CanonicalizeAddress(vmcb_read64(offset + 8));
  bool valid = (attr >> 7) & 1;

  set_segment_ar_data(seg, valid, selector, base, limit,
       (attr & 0xff) | ((attr & 0xf00) << 4));
}

BX_CPP_INLINE void BX_CPU_C::svm_segment_write(bx_segment_reg_t *seg, unsigned offset)
{
  Bit32u selector = seg->selector.value;
  bx_address base = seg->cache.u.segment.base;
  Bit32u limit = seg->cache.u.segment.limit_scaled;
  Bit32u attr = (seg->cache.valid) ? 
     (get_descriptor_h(&seg->cache) & 0x00f0ff00) : 0;

  vmcb_write16(offset, selector);
  vmcb_write16(offset + 2, ((attr >> 8) & 0xff) | ((attr >> 12) & 0xf00));
  vmcb_write32(offset + 4, limit);
  vmcb_write64(offset + 8, base);
}

void BX_CPU_C::SvmEnterSaveHostState(SVM_HOST_STATE *host)
{
  for (int n=0;n < 4; n++)
    host->sregs[n] = BX_CPU_THIS_PTR sregs[n];

  host->gdtr = BX_CPU_THIS_PTR gdtr;
  host->idtr = BX_CPU_THIS_PTR idtr;

  host->efer = BX_CPU_THIS_PTR efer;
  host->cr0 = BX_CPU_THIS_PTR cr0;
  host->cr3 = BX_CPU_THIS_PTR cr3;
  host->cr4 = BX_CPU_THIS_PTR cr4;
  host->eflags = read_eflags();
  host->rip = RIP;
  host->rsp = RSP;
  host->rax = RAX;

  host->pat_msr = BX_CPU_THIS_PTR msr.pat;
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

  BX_CPU_THIS_PTR efer = host->efer;
  BX_CPU_THIS_PTR cr0.set32(host->cr0.get32() | BX_CR0_PE_MASK); // always set the CR0.PE
  BX_CPU_THIS_PTR cr3 = host->cr3;
  BX_CPU_THIS_PTR cr4 = host->cr4;

  if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
    if (! CheckPDPTR(BX_CPU_THIS_PTR cr3)) {
      BX_ERROR(("SvmExitLoadHostState(): PDPTR check failed !"));
      shutdown();
    }
  }

  BX_CPU_THIS_PTR msr.pat = host->pat_msr;

  BX_CPU_THIS_PTR dr7.set32(0x00000400);

  setEFlags(host->eflags & ~EFlagsVMMask); // ignore saved copy of EFLAGS.VM

  RIP = BX_CPU_THIS_PTR prev_rip = host->rip;
  RSP = host->rsp;
  RAX = host->rax;

  CPL = 0;

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);
}

void BX_CPU_C::SvmExitSaveGuestState(void)
{
  for (unsigned n=0;n < 4; n++) {
    svm_segment_write(&BX_CPU_THIS_PTR sregs[n], SVM_GUEST_ES_SELECTOR + n * 0x10);
  }

  vmcb_write64(SVM_GUEST_GDTR_BASE, BX_CPU_THIS_PTR gdtr.base);
  vmcb_write16(SVM_GUEST_GDTR_LIMIT, BX_CPU_THIS_PTR gdtr.limit);

  vmcb_write64(SVM_GUEST_IDTR_BASE, BX_CPU_THIS_PTR idtr.base);
  vmcb_write16(SVM_GUEST_IDTR_LIMIT, BX_CPU_THIS_PTR idtr.limit);

  vmcb_write64(SVM_GUEST_EFER_MSR, BX_CPU_THIS_PTR efer.get32());
  vmcb_write64(SVM_GUEST_CR0, BX_CPU_THIS_PTR cr0.get32());
  vmcb_write64(SVM_GUEST_CR2, BX_CPU_THIS_PTR cr2);
  vmcb_write64(SVM_GUEST_CR3, BX_CPU_THIS_PTR cr3);
  vmcb_write64(SVM_GUEST_CR4, BX_CPU_THIS_PTR cr4.get32());

  vmcb_write64(SVM_GUEST_DR6, BX_CPU_THIS_PTR dr6.get32());
  vmcb_write64(SVM_GUEST_DR7, BX_CPU_THIS_PTR dr7.get32());

  vmcb_write64(SVM_GUEST_RFLAGS, read_eflags());
  vmcb_write64(SVM_GUEST_RAX, RAX);
  vmcb_write64(SVM_GUEST_RSP, RSP);
  vmcb_write64(SVM_GUEST_RIP, RIP);

  vmcb_write8(SVM_GUEST_CPL, CPL);

  vmcb_write8(SVM_CONTROL_INTERRUPT_SHADOW, interrupts_inhibited(BX_INHIBIT_INTERRUPTS));

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;

  if (ctrls->nested_paging) {
    vmcb_write64(SVM_GUEST_PAT, BX_CPU_THIS_PTR msr.pat.u64);
  }

  vmcb_write8(SVM_CONTROL_VTPR, ctrls->v_tpr);
  vmcb_write8(SVM_CONTROL_VIRQ, is_pending(BX_EVENT_SVM_VIRQ_PENDING));
  clear_event(BX_EVENT_SVM_VIRQ_PENDING);
}

extern bool isValidMSR_PAT(Bit64u pat_msr);

bool BX_CPU_C::SvmEnterLoadCheckControls(SVM_CONTROLS *ctrls)
{
  ctrls->cr_rd_ctrl = vmcb_read16(SVM_CONTROL16_INTERCEPT_CR_READ);
  ctrls->cr_wr_ctrl = vmcb_read16(SVM_CONTROL16_INTERCEPT_CR_WRITE);

  ctrls->dr_rd_ctrl = vmcb_read16(SVM_CONTROL16_INTERCEPT_DR_READ);
  ctrls->dr_wr_ctrl = vmcb_read16(SVM_CONTROL16_INTERCEPT_DR_WRITE);

  ctrls->intercept_vector[0] = vmcb_read32(SVM_CONTROL32_INTERCEPT1);
  ctrls->intercept_vector[1] = vmcb_read32(SVM_CONTROL32_INTERCEPT2);

  if (! SVM_INTERCEPT(SVM_INTERCEPT1_VMRUN)) {
    BX_ERROR(("VMRUN: VMRUN intercept bit is not set!"));
    return 0;
  }

  ctrls->exceptions_intercept = vmcb_read32(SVM_CONTROL32_INTERCEPT_EXCEPTIONS);

  // force 4K page alignment
  ctrls->iopm_base = PPFOf(vmcb_read64(SVM_CONTROL64_IOPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->iopm_base)) {
    BX_ERROR(("VMRUN: invalid IOPM Base Address !"));
    return 0;
  }

  // force 4K page alignment
  ctrls->msrpm_base = PPFOf(vmcb_read64(SVM_CONTROL64_MSRPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->msrpm_base)) {
    BX_ERROR(("VMRUN: invalid MSRPM Base Address !"));
    return 0;
  }

  Bit32u guest_asid = vmcb_read32(SVM_CONTROL32_GUEST_ASID);
  if (guest_asid == 0) {
    BX_ERROR(("VMRUN: attempt to run guest with host ASID !"));
    return 0;
  }

  ctrls->v_tpr = vmcb_read8(SVM_CONTROL_VTPR);
  ctrls->v_intr_masking = vmcb_read8(SVM_CONTROL_VINTR_MASKING) & 0x1;
  ctrls->v_intr_vector = vmcb_read8(SVM_CONTROL_VINTR_VECTOR);

  Bit8u vintr_control = vmcb_read8(SVM_CONTROL_VINTR_PRIO_IGN_TPR);
  ctrls->v_intr_prio = vintr_control & 0xf;
  ctrls->v_ignore_tpr = (vintr_control >> 4) & 0x1;

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_PAUSE_FILTER))
    ctrls->pause_filter_count = vmcb_read16(SVM_CONTROL16_PAUSE_FILTER_COUNT);
  else
    ctrls->pause_filter_count = 0;

  ctrls->nested_paging = vmcb_read8(SVM_CONTROL_NESTED_PAGING_ENABLE);
  if (! BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NESTED_PAGING)) {
     if (ctrls->nested_paging) {
        BX_ERROR(("VMRUN: Nesting paging is not supported in this SVM configuration !"));
        ctrls->nested_paging = 0;
     }
  }

  if (ctrls->nested_paging) {
    if (! BX_CPU_THIS_PTR cr0.get_PG()) {
      BX_ERROR(("VMRUN: attempt to enter nested paging mode when host paging is disabled !"));
      return 0;
    }

    Bit64u guest_pat = vmcb_read64(SVM_GUEST_PAT);
    if (! isValidMSR_PAT(guest_pat)) {
      BX_ERROR(("VMRUN: invalid memory type in guest PAT_MSR !"));
      return 0;
    }

    ctrls->ncr3 = vmcb_read64(SVM_CONTROL64_NESTED_PAGING_HOST_CR3);
    if (long_mode()) {
      if (! IsValidPhyAddr(ctrls->ncr3)) {
        BX_ERROR(("VMRUN(): NCR3 reserved bits set !"));
        return 0;
      }
    }

    BX_DEBUG(("VMRUN: Starting Nested Paging Mode !"));
  }

  return 1;
}

bool BX_CPU_C::SvmEnterLoadCheckGuestState(void)
{
  SVM_GUEST_STATE guest;
  Bit32u tmp;
  unsigned n;
  bool paged_real_mode = false;

  guest.eflags = vmcb_read32(SVM_GUEST_RFLAGS);
  guest.rip = vmcb_read64(SVM_GUEST_RIP);
  guest.rsp = vmcb_read64(SVM_GUEST_RSP);
  guest.rax = vmcb_read64(SVM_GUEST_RAX);

  guest.efer.val32 = vmcb_read32(SVM_GUEST_EFER_MSR);
  tmp = vmcb_read32(SVM_GUEST_EFER_MSR_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest EFER[63:32] is not zero"));
    return 0;
  }

  if (guest.efer.val32 & ~BX_CPU_THIS_PTR efer_suppmask) {
    BX_ERROR(("VMRUN: Guest EFER reserved bits set"));
    return 0;
  }

  if (! guest.efer.get_SVME()) {
    BX_ERROR(("VMRUN: Guest EFER.SVME = 0"));
    return 0;
  }

  guest.cr0.val32 = vmcb_read32(SVM_GUEST_CR0);
  tmp = vmcb_read32(SVM_GUEST_CR0_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest CR0[63:32] is not zero"));
    return 0;
  }

  // always assign EFER.LMA := EFER.LME & CR0.PG
  guest.efer.set_LMA(guest.cr0.get_PG() && guest.efer.get_LME());

  guest.cr2 = vmcb_read64(SVM_GUEST_CR2);
  guest.cr3 = vmcb_read64(SVM_GUEST_CR3);

  guest.cr4.val32 = vmcb_read32(SVM_GUEST_CR4);
  tmp = vmcb_read32(SVM_GUEST_CR4_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest CR4[63:32] is not zero"));
    return 0;
  }

  if (guest.cr4.val32 & ~BX_CPU_THIS_PTR cr4_suppmask) {
    BX_ERROR(("VMRUN: Guest CR4 reserved bits set"));
    return 0;
  }

  guest.dr6 = vmcb_read32(SVM_GUEST_DR6);
  tmp = vmcb_read32(SVM_GUEST_DR6_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest DR6[63:32] is not zero"));
    return 0;
  }

  guest.dr7 = vmcb_read32(SVM_GUEST_DR7);
  tmp = vmcb_read32(SVM_GUEST_DR7_HI);
  if (tmp) {
    BX_ERROR(("VMRUN: Guest DR7[63:32] is not zero"));
    return 0;
  }

  guest.pat_msr = vmcb_read64(SVM_GUEST_PAT);

  for (n=0;n < 4; n++) {
    svm_segment_read(&guest.sregs[n], SVM_GUEST_ES_SELECTOR + n * 0x10);
  }

  if (guest.sregs[BX_SEG_REG_CS].cache.u.segment.d_b && guest.sregs[BX_SEG_REG_CS].cache.u.segment.l) {
    BX_ERROR(("VMRUN: VMCB CS.D_B/L mismatch"));
    return 0;
  }

  if (! guest.cr0.get_PE() || (guest.eflags & EFlagsVMMask) != 0)
  {
    // real or vm8086 mode: make all segments valid
    for (n=0;n < 4; n++) {
      guest.sregs[n].cache.valid = SegValidCache;
    }

    if (! guest.cr0.get_PE() && guest.cr0.get_PG()) {
      // special case : entering paged real mode
      BX_DEBUG(("VMRUN: entering paged real mode"));
      paged_real_mode = true;
      guest.cr0.val32 &= ~BX_CR0_PG_MASK;
    }
  }

  guest.cpl = vmcb_read8(SVM_GUEST_CPL);

  //
  // FIXME: patch segment attributes
  //

  // CS: only D, L, and R are observed

  // DS/ES/FS/GS: only D, P, DPL, E, W, and Code/Data are observed

  // SS: only B, P, E, W, and Code/Data are observed
  guest.sregs[BX_SEG_REG_SS].cache.dpl = guest.cpl;

  guest.gdtr.base = CanonicalizeAddress(vmcb_read64(SVM_GUEST_GDTR_BASE));
  guest.gdtr.limit = vmcb_read16(SVM_GUEST_GDTR_LIMIT);

  guest.idtr.base = CanonicalizeAddress(vmcb_read64(SVM_GUEST_IDTR_BASE));
  guest.idtr.limit = vmcb_read16(SVM_GUEST_IDTR_LIMIT);

  guest.inhibit_interrupts = vmcb_read8(SVM_CONTROL_INTERRUPT_SHADOW) & 0x1;

  //
  // Load guest state
  //

  BX_CPU_THIS_PTR tsc_offset = vmcb_read64(SVM_CONTROL64_TSC_OFFSET);

  BX_CPU_THIS_PTR efer.set32(guest.efer.get32());

  if (! check_CR0(guest.cr0.get32())) {
    BX_ERROR(("SVM: VMRUN CR0 is broken !"));
    return 0;
  }
  if (! check_CR4(guest.cr4.get32())) {
    BX_ERROR(("SVM: VMRUN CR4 is broken !"));
    return 0;
  }

  BX_CPU_THIS_PTR cr0.set32(guest.cr0.get32());
  BX_CPU_THIS_PTR cr4.set32(guest.cr4.get32());
  BX_CPU_THIS_PTR cr3 = guest.cr3;

  if (paged_real_mode)
    BX_CPU_THIS_PTR cr0.val32 |= BX_CR0_PG_MASK;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;
  if (! ctrls->nested_paging) {
    if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
      if (! CheckPDPTR(BX_CPU_THIS_PTR cr3)) {
        BX_ERROR(("SVM: VMRUN PDPTR check failed !"));
        return 0;
      }
    }
  }
  else {
    // load guest PAT when nested paging is enabled
    BX_CPU_THIS_PTR msr.pat = guest.pat_msr;
  }

  BX_CPU_THIS_PTR dr6.set32(guest.dr6);
  BX_CPU_THIS_PTR dr7.set32(guest.dr7 | 0x400);

  for (n=0;n < 4; n++) {
    BX_CPU_THIS_PTR sregs[n] = guest.sregs[n];
  }

  BX_CPU_THIS_PTR gdtr = guest.gdtr;
  BX_CPU_THIS_PTR idtr = guest.idtr;
 
  RIP = BX_CPU_THIS_PTR prev_rip = guest.rip;
  RSP = guest.rsp;
  RAX = guest.rax;

  CPL = guest.cpl;

  if (guest.inhibit_interrupts) {
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS);
  }

  BX_CPU_THIS_PTR async_event = 0;

  setEFlags((Bit32u) guest.eflags);

  // injecting virtual interrupt
  Bit8u v_irq = vmcb_read8(SVM_CONTROL_VIRQ) & 0x1;
  if (v_irq)
    signal_event(BX_EVENT_SVM_VIRQ_PENDING);

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR monitor.reset_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);

  return 1;
}

void BX_CPU_C::Svm_Vmexit(int reason, Bit64u exitinfo1, Bit64u exitinfo2)
{
  BX_DEBUG(("SVM VMEXIT reason=%d exitinfo1=%08x%08x exitinfo2=%08x%08x", reason,
    GET32H(exitinfo1), GET32L(exitinfo1), GET32H(exitinfo2), GET32L(exitinfo2)));

  if (! BX_CPU_THIS_PTR in_svm_guest) {
    if (reason != SVM_VMEXIT_INVALID)
      BX_PANIC(("PANIC: VMEXIT %d not in SVM guest mode !", reason));
  }

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NRIP_SAVE))
    vmcb_write64(SVM_CONTROL64_NRIP, RIP);

  // VMEXITs are FAULT-like: restore RIP/RSP to value before VMEXIT occurred
  RIP = BX_CPU_THIS_PTR prev_rip;
  if (BX_CPU_THIS_PTR speculative_rsp)
    RSP = BX_CPU_THIS_PTR prev_rsp;
  BX_CPU_THIS_PTR speculative_rsp = 0;

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_DECODE_ASSIST)) {
    //
    // In the case of a Nested #PF or intercepted #PF, guest instruction bytes at
    // guest CS:RIP are stored into the 16-byte wide field Guest Instruction Bytes.
    // Up to 15 bytes are recorded, read from guest CS:RIP. The number of bytes
    // fetched is put into the first byte of this field. Zero indicates that no
    // bytes were fetched.
    //
    // This field is filled in only during data page faults. Instruction-fetch
    // page faults provide no additional information. All other intercepts clear
    // bits 0:7 in this field to zero.
    //

    if ((reason == SVM_VMEXIT_PF_EXCEPTION || reason == SVM_VMEXIT_NPF) && !(exitinfo1 & 0x10))
    {
      // TODO
    }
    else {
      vmcb_write8(SVM_CONTROL64_GUEST_INSTR_BYTES, 0);
    }
  }

  mask_event(BX_EVENT_SVM_VIRQ_PENDING);

  BX_CPU_THIS_PTR in_svm_guest = 0;
  BX_CPU_THIS_PTR svm_gif = 0;

  //
  // STEP 0: Update exit reason
  //

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;

  vmcb_write64(SVM_CONTROL64_EXITCODE, (Bit64u) ((Bit64s) reason));
  vmcb_write64(SVM_CONTROL64_EXITINFO1, exitinfo1);
  vmcb_write64(SVM_CONTROL64_EXITINFO2, exitinfo2);

  // clean interrupt injection field
  vmcb_write32(SVM_CONTROL32_EVENT_INJECTION, ctrls->eventinj & ~0x80000000);

  if (BX_CPU_THIS_PTR in_event) {
    vmcb_write32(SVM_CONTROL32_EXITINTINFO, ctrls->exitintinfo | 0x80000000);
    vmcb_write32(SVM_CONTROL32_EXITINTINFO_ERROR_CODE, ctrls->exitintinfo_error_code);
    BX_CPU_THIS_PTR in_event = 0;
  }
  else {
    vmcb_write32(SVM_CONTROL32_EXITINTINFO, 0);
  }

  //
  // Step 1: Save guest state in the VMCB
  //
  SvmExitSaveGuestState();

  //
  // Step 2:
  //
  SvmExitLoadHostState(&BX_CPU_THIS_PTR vmcb.host_state);

  //
  // STEP 3: Go back to SVM host
  //

  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR last_exception_type = 0;

#if BX_DEBUGGER
  if (BX_CPU_THIS_PTR vmexit_break) {
    BX_CPU_THIS_PTR stop_reason = STOP_VMEXIT_BREAK_POINT;
    bx_debug_break(); // trap into debugger
  }
#endif

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

extern struct BxExceptionInfo exceptions_info[];

bool BX_CPU_C::SvmInjectEvents(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;

  ctrls->eventinj = vmcb_read32(SVM_CONTROL32_EVENT_INJECTION);
  if ((ctrls->eventinj & 0x80000000) == 0) return 1;

  /* the VMENTRY injecting event to the guest */
  unsigned vector = ctrls->eventinj & 0xff;
  unsigned type = (ctrls->eventinj >> 8) & 7;
  unsigned push_error = ctrls->eventinj & (1 << 11);
  unsigned error_code = push_error ? vmcb_read32(SVM_CONTROL32_EVENT_INJECTION_ERRORCODE) : 0;

  switch(type) {
    case BX_NMI:
      mask_event(BX_EVENT_NMI);
      BX_CPU_THIS_PTR EXT = 1;
      vector = 2;
      break;

    case BX_EXTERNAL_INTERRUPT:
      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_HARDWARE_EXCEPTION:
      if (vector == 2 || vector > 31) {
        BX_ERROR(("SvmInjectEvents: invalid vector %d for HW exception", vector));
        return 0;
      }
      if (vector == BX_BP_EXCEPTION || vector == BX_OF_EXCEPTION) {
        type = BX_SOFTWARE_EXCEPTION;
      }
      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_SOFTWARE_INTERRUPT:
      break;

    default:
      BX_ERROR(("SvmInjectEvents: unsupported event injection type %d !", type));
      return 0;
  }

  BX_DEBUG(("SvmInjectEvents: Injecting vector 0x%02x (error_code 0x%04x)", vector, error_code));

  if (type == BX_HARDWARE_EXCEPTION) {
    // record exception the same way as BX_CPU_C::exception does
    BX_ASSERT(vector < BX_CPU_HANDLED_EXCEPTIONS);
    BX_CPU_THIS_PTR last_exception_type = exceptions_info[vector].exception_type;
  }

  ctrls->exitintinfo = ctrls->eventinj & ~0x80000000;
  ctrls->exitintinfo_error_code = error_code;

  interrupt(vector, type, push_error, error_code);

  BX_CPU_THIS_PTR last_exception_type = 0; // error resolved

  return 1;
}

void BX_CPU_C::SvmInterceptException(unsigned type, unsigned vector, Bit16u errcode, bool errcode_valid, Bit64u qualification)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  BX_ASSERT(vector < 32);

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;

  BX_ASSERT(type == BX_HARDWARE_EXCEPTION || type == BX_SOFTWARE_EXCEPTION);

  if (! SVM_EXCEPTION_INTERCEPTED(vector)) {

    // -----------------------------------------
    //              EXITINTINFO
    // -----------------------------------------
    // [07:00] | Interrupt/Exception vector
    // [10:08] | Interrupt/Exception type
    // [11:11] | error code pushed to the stack
    // [30:12] | reserved
    // [31:31] | interruption info valid
    //

    // record IDT vectoring information 
    ctrls->exitintinfo_error_code = errcode;
    ctrls->exitintinfo = vector | (BX_HARDWARE_EXCEPTION << 8);
    if (errcode_valid)
      BX_CPU_THIS_PTR vmcb.ctrls.exitintinfo |= (1 << 11); // error code delivered
    return;
  }

  BX_ERROR(("SVM VMEXIT: event vector 0x%02x type %d error code=0x%04x", vector, type, errcode));

  // VMEXIT is not considered to occur during event delivery if it results
  // in a double fault exception that causes VMEXIT directly
  if (vector == BX_DF_EXCEPTION)
    BX_CPU_THIS_PTR in_event = 0; // clear in_event indication on #DF

  BX_CPU_THIS_PTR debug_trap = 0; // clear debug_trap field
  BX_CPU_THIS_PTR inhibit_mask = 0;

  Svm_Vmexit(SVM_VMEXIT_EXCEPTION + vector, (errcode_valid ? errcode : 0), qualification);
}

enum {
  SVM_VMEXIT_IO_PORTIN        = (1 << 0),
  SVM_VMEXIT_IO_INSTR_STRING  = (1 << 2),
  SVM_VMEXIT_IO_INSTR_REP     = (1 << 3),
  SVM_VMEXIT_IO_INSTR_LEN8    = (1 << 4),
  SVM_VMEXIT_IO_INSTR_LEN16   = (1 << 5),
  SVM_VMEXIT_IO_INSTR_LEN32   = (1 << 6),
  SVM_VMEXIT_IO_INSTR_ASIZE16 = (1 << 7),
  SVM_VMEXIT_IO_INSTR_ASIZE32 = (1 << 8),
  SVM_VMEXIT_IO_INSTR_ASIZE64 = (1 << 9)
};

void BX_CPU_C::SvmInterceptIO(bxInstruction_c *i, unsigned port, unsigned len)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  if (! SVM_INTERCEPT(SVM_INTERCEPT0_IO)) return;

  Bit8u bitmap[2];
  bx_phy_address pAddr;

  // access_read_physical cannot read 2 bytes cross 4K boundary :(
  pAddr = BX_CPU_THIS_PTR vmcb.ctrls.iopm_base + (port / 8);
  access_read_physical(pAddr, 1, &bitmap[0]);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[0]);

  pAddr++;
  access_read_physical(pAddr, 1, &bitmap[1]);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_IO_BITMAP_ACCESS, &bitmap[1]);

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
        BX_PANIC(("VMexit_IO: I/O instruction %s unknown", i->getIaOpcodeNameShort()));
    }

    qualification |= (port << 16);
    if (len == 1)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN8;
    else if (len == 2)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN16;
    else if (len == 4)
      qualification |= SVM_VMEXIT_IO_INSTR_LEN32;

    if (i->as64L())
      qualification |= SVM_VMEXIT_IO_INSTR_ASIZE64;
    else if (i->as32L())
      qualification |= SVM_VMEXIT_IO_INSTR_ASIZE32;
    else 
      qualification |= SVM_VMEXIT_IO_INSTR_ASIZE16;

    Svm_Vmexit(SVM_VMEXIT_IO, qualification, RIP);
  }
}

void BX_CPU_C::SvmInterceptMSR(unsigned op, Bit32u msr)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  if (! SVM_INTERCEPT(SVM_INTERCEPT0_MSR)) return;

  BX_ASSERT(op == BX_READ || op == BX_WRITE);

  bool vmexit = true;

  int msr_map_offset = -1;
  if (msr <= 0x1fff) msr_map_offset = 0;
  else if (msr >= 0xc0000000 && msr <= 0xc0001fff) msr_map_offset = 2048;
  else if (msr >= 0xc0010000 && msr <= 0xc0011fff) msr_map_offset = 4096;

  if (msr_map_offset >= 0) {
    bx_phy_address msr_bitmap_addr = BX_CPU_THIS_PTR vmcb.ctrls.msrpm_base + msr_map_offset;
    Bit32u msr_offset = (msr & 0x1fff) * 2 + op;

    Bit8u msr_bitmap;
    bx_phy_address pAddr = msr_bitmap_addr + (msr_offset / 8);
    access_read_physical(pAddr, 1, (Bit8u*)(&msr_bitmap));
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_MSR_BITMAP_ACCESS, &msr_bitmap);

    vmexit = (msr_bitmap >> (msr_offset & 7)) & 0x1;
  }

  if (vmexit) {
    Svm_Vmexit(SVM_VMEXIT_MSR, op); // 0 - RDMSR, 1 - WRMSR
  }
}

void BX_CPU_C::SvmInterceptTaskSwitch(Bit16u tss_selector, unsigned source, bool push_error, Bit32u error_code)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_svm_guest);

  BX_DEBUG(("SVM VMEXIT: task switch"));

  //
  // SVM VMexit EXITINFO2:
  // --------------------
  // EXITINFO2[31-0] - error code to push into new stack (if applicable)
  // EXITINFO2[36]   - task switch caused by IRET
  // EXITINFO2[38]   - task switch caused by JUMP FAR
  // EXITINFO2[44]   - task switch has error code to push
  // EXITINFO2[48]   - value of EFLAGS.RF that would be saved in outgoing TSS
  //

  Bit64u qualification = error_code;

  if (source == BX_TASK_FROM_IRET)
    qualification |= BX_CONST64(1) << 36;
  if (source == BX_TASK_FROM_JUMP)
    qualification |= BX_CONST64(1) << 38;
  if (push_error)
    qualification |= BX_CONST64(1) << 44;

  Bit32u flags = read_eflags();
  if (flags & EFlagsRFMask)
    qualification |= BX_CONST64(1) << 48;

  Svm_Vmexit(SVM_VMEXIT_TASK_SWITCH, tss_selector, qualification);
}

void BX_CPU_C::SvmInterceptPAUSE(void)
{
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_PAUSE_FILTER)) {
    SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb.ctrls;
    if (ctrls->pause_filter_count) {
      ctrls->pause_filter_count--;
      return;
    }
  }

  Svm_Vmexit(SVM_VMEXIT_PAUSE);
}

#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMRUN(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_VMRUN))
      Svm_Vmexit(SVM_VMEXIT_VMRUN);
  }

  bx_address pAddr = RAX & i->asize_mask();
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("VMRUN: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }
  set_VMCBPTR(pAddr);

  BX_DEBUG(("VMRUN VMCB ptr: 0x" FMT_ADDRX64, BX_CPU_THIS_PTR vmcbptr));

  //
  // Step 1: Save host state to physical memory indicated in SVM_HSAVE_PHY_ADDR_MSR
  //
  SvmEnterSaveHostState(&BX_CPU_THIS_PTR vmcb.host_state);

  //
  // Step 2: Load control information from the VMCB
  //
  if (!SvmEnterLoadCheckControls(&BX_CPU_THIS_PTR vmcb.ctrls))
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  //
  // Step 3: Load guest state from the VMCB and enter SVM
  //
  if (!SvmEnterLoadCheckGuestState())
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  BX_CPU_THIS_PTR in_svm_guest = 1;
  BX_CPU_THIS_PTR svm_gif = 1;
  BX_CPU_THIS_PTR async_event = 1;

  //
  // Step 4: Inject events to the guest
  //
  if (!SvmInjectEvents())
    Svm_Vmexit(SVM_VMEXIT_INVALID);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMMCALL(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (BX_CPU_THIS_PTR efer.get_SVME()) {
    if (BX_CPU_THIS_PTR in_svm_guest) {
      if (SVM_INTERCEPT(SVM_INTERCEPT1_VMMCALL)) Svm_Vmexit(SVM_VMEXIT_VMMCALL);
    }
  }

  exception(BX_UD_EXCEPTION, 0);
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMLOAD(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_VMLOAD)) Svm_Vmexit(SVM_VMEXIT_VMLOAD);
  }

  bx_address pAddr = RAX & i->asize_mask();
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("VMLOAD: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }
  set_VMCBPTR(pAddr);

  BX_DEBUG(("VMLOAD VMCB ptr: 0x" FMT_ADDRX64, BX_CPU_THIS_PTR vmcbptr));

  bx_segment_reg_t fs, gs, guest_tr, guest_ldtr;

  svm_segment_read(&fs, SVM_GUEST_FS_SELECTOR);
  svm_segment_read(&gs, SVM_GUEST_GS_SELECTOR);
  svm_segment_read(&guest_tr, SVM_GUEST_TR_SELECTOR);
  svm_segment_read(&guest_ldtr, SVM_GUEST_LDTR_SELECTOR);

  BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS] = fs;
  BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS] = gs;
  BX_CPU_THIS_PTR tr = guest_tr;
  BX_CPU_THIS_PTR ldtr = guest_ldtr;

  BX_CPU_THIS_PTR msr.kernelgsbase = CanonicalizeAddress(vmcb_read64(SVM_GUEST_KERNEL_GSBASE_MSR));
  BX_CPU_THIS_PTR msr.star = vmcb_read64(SVM_GUEST_STAR_MSR);
  BX_CPU_THIS_PTR msr.lstar = CanonicalizeAddress(vmcb_read64(SVM_GUEST_LSTAR_MSR));
  BX_CPU_THIS_PTR msr.cstar = CanonicalizeAddress(vmcb_read64(SVM_GUEST_CSTAR_MSR));
  BX_CPU_THIS_PTR msr.fmask = vmcb_read64(SVM_GUEST_FMASK_MSR);

  BX_CPU_THIS_PTR msr.sysenter_cs_msr = vmcb_read64(SVM_GUEST_SYSENTER_CS_MSR);
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = CanonicalizeAddress(vmcb_read64(SVM_GUEST_SYSENTER_EIP_MSR));
  BX_CPU_THIS_PTR msr.sysenter_esp_msr = CanonicalizeAddress(vmcb_read64(SVM_GUEST_SYSENTER_ESP_MSR));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_VMSAVE)) Svm_Vmexit(SVM_VMEXIT_VMSAVE);
  }

  bx_address pAddr = RAX & i->asize_mask();
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("VMSAVE: invalid or not page aligned VMCB physical address !"));
    exception(BX_GP_EXCEPTION, 0);
  }
  set_VMCBPTR(pAddr);

  BX_DEBUG(("VMSAVE VMCB ptr: 0x" FMT_ADDRX64, BX_CPU_THIS_PTR vmcbptr));

  svm_segment_write(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_FS], SVM_GUEST_FS_SELECTOR);
  svm_segment_write(&BX_CPU_THIS_PTR sregs[BX_SEG_REG_GS], SVM_GUEST_GS_SELECTOR);
  svm_segment_write(&BX_CPU_THIS_PTR tr, SVM_GUEST_TR_SELECTOR);
  svm_segment_write(&BX_CPU_THIS_PTR ldtr, SVM_GUEST_LDTR_SELECTOR);

  vmcb_write64(SVM_GUEST_KERNEL_GSBASE_MSR, BX_CPU_THIS_PTR msr.kernelgsbase);
  vmcb_write64(SVM_GUEST_STAR_MSR, BX_CPU_THIS_PTR msr.star);
  vmcb_write64(SVM_GUEST_LSTAR_MSR, BX_CPU_THIS_PTR msr.lstar);
  vmcb_write64(SVM_GUEST_CSTAR_MSR, BX_CPU_THIS_PTR msr.cstar);
  vmcb_write64(SVM_GUEST_FMASK_MSR, BX_CPU_THIS_PTR msr.fmask);

  vmcb_write64(SVM_GUEST_SYSENTER_CS_MSR, BX_CPU_THIS_PTR msr.sysenter_cs_msr);
  vmcb_write64(SVM_GUEST_SYSENTER_ESP_MSR, BX_CPU_THIS_PTR msr.sysenter_esp_msr);
  vmcb_write64(SVM_GUEST_SYSENTER_EIP_MSR, BX_CPU_THIS_PTR msr.sysenter_eip_msr);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SKINIT(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_SKINIT)) Svm_Vmexit(SVM_VMEXIT_SKINIT);
  }

  BX_PANIC(("SVM: SKINIT is not implemented yet"));
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLGI(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_CLGI)) Svm_Vmexit(SVM_VMEXIT_CLGI);
  }

  BX_CPU_THIS_PTR svm_gif = 0;
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STGI(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_STGI)) Svm_Vmexit(SVM_VMEXIT_STGI);
  }

  BX_CPU_THIS_PTR svm_gif = 1;
  BX_CPU_THIS_PTR async_event = 1;
#endif

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVLPGA(bxInstruction_c *i)
{
#if BX_SUPPORT_SVM
  if (! protected_mode() || ! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_INVLPGA)) Svm_Vmexit(SVM_VMEXIT_INVLPGA);
  }

  TLB_flush(); // FIXME: flush all entries for now
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_SVM
void BX_CPU_C::register_svm_state(bx_param_c *parent)
{
  if (! is_cpu_extension_supported(BX_ISA_SVM)) return;

  // register SVM state for save/restore param tree
  bx_list_c *svm = new bx_list_c(parent, "SVM");

  BXRS_HEX_PARAM_FIELD(svm, vmcbptr, BX_CPU_THIS_PTR vmcbptr);
  BXRS_PARAM_BOOL(svm, in_svm_guest, BX_CPU_THIS_PTR in_svm_guest);
  BXRS_PARAM_BOOL(svm, gif, BX_CPU_THIS_PTR svm_gif);

  //
  // VMCB Control Fields
  //

  bx_list_c *vmcb_ctrls = new bx_list_c(svm, "VMCB_CTRLS");

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_rd_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.cr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_wr_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.cr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_rd_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.dr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_wr_ctrl, BX_CPU_THIS_PTR vmcb.ctrls.dr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exceptions_intercept, BX_CPU_THIS_PTR vmcb.ctrls.exceptions_intercept);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector0, BX_CPU_THIS_PTR vmcb.ctrls.intercept_vector[0]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector1, BX_CPU_THIS_PTR vmcb.ctrls.intercept_vector[1]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, iopm_base, BX_CPU_THIS_PTR vmcb.ctrls.iopm_base);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, msrpm_base, BX_CPU_THIS_PTR vmcb.ctrls.msrpm_base);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exitintinfo, BX_CPU_THIS_PTR vmcb.ctrls.exitintinfo);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exitintinfo_errcode, BX_CPU_THIS_PTR vmcb.ctrls.exitintinfo_error_code);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, eventinj, BX_CPU_THIS_PTR vmcb.ctrls.eventinj);

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_tpr, BX_CPU_THIS_PTR vmcb.ctrls.v_tpr);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_prio, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_prio);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_ignore_tpr, BX_CPU_THIS_PTR vmcb.ctrls.v_ignore_tpr);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_intr_masking, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_masking);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_vector, BX_CPU_THIS_PTR vmcb.ctrls.v_intr_vector);
  BXRS_PARAM_BOOL(vmcb_ctrls, nested_paging, BX_CPU_THIS_PTR vmcb.ctrls.nested_paging);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, ncr3, BX_CPU_THIS_PTR vmcb.ctrls.ncr3);

  //
  // VMCB Host State
  //

  bx_list_c *host = new bx_list_c(svm, "VMCB_HOST_STATE");

  for(unsigned n=0; n<4; n++) {
    bx_segment_reg_t *segment = &BX_CPU_THIS_PTR vmcb.host_state.sregs[n];
    bx_list_c *sreg = new bx_list_c(host, segname[n]);
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

  bx_list_c *GDTR = new bx_list_c(host, "GDTR");
  BXRS_HEX_PARAM_FIELD(GDTR, base, BX_CPU_THIS_PTR vmcb.host_state.gdtr.base);
  BXRS_HEX_PARAM_FIELD(GDTR, limit, BX_CPU_THIS_PTR vmcb.host_state.gdtr.limit);

  bx_list_c *IDTR = new bx_list_c(host, "IDTR");
  BXRS_HEX_PARAM_FIELD(IDTR, base, BX_CPU_THIS_PTR vmcb.host_state.idtr.base);
  BXRS_HEX_PARAM_FIELD(IDTR, limit, BX_CPU_THIS_PTR vmcb.host_state.idtr.limit);

  BXRS_HEX_PARAM_FIELD(host, efer, BX_CPU_THIS_PTR vmcb.host_state.efer.val32);
  BXRS_HEX_PARAM_FIELD(host, cr0, BX_CPU_THIS_PTR vmcb.host_state.cr0.val32);
  BXRS_HEX_PARAM_FIELD(host, cr3, BX_CPU_THIS_PTR vmcb.host_state.cr3);
  BXRS_HEX_PARAM_FIELD(host, cr4, BX_CPU_THIS_PTR vmcb.host_state.cr4.val32);
  BXRS_HEX_PARAM_FIELD(host, eflags, BX_CPU_THIS_PTR vmcb.host_state.eflags);
  BXRS_HEX_PARAM_FIELD(host, rip, BX_CPU_THIS_PTR vmcb.host_state.rip);
  BXRS_HEX_PARAM_FIELD(host, rsp, BX_CPU_THIS_PTR vmcb.host_state.rsp);
  BXRS_HEX_PARAM_FIELD(host, rax, BX_CPU_THIS_PTR vmcb.host_state.rax);
}
#endif
