/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2026 Stanislav Shwartsman
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
#include "icache.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM

#include "svm.h"
#include "cpuid.h"

#include "pc_system.h"
#include "gui/paramtree.h"
#include "decoder/ia_opcodes.h"

#include "bx_debug/debug.h"

extern const char *segname[];

#if BX_SUPPORT_CET
extern bool is_valid_cet_control(Bit64u val);
#endif

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

BX_CPP_INLINE Bit8u BX_CPU_C::vmcb_read8(unsigned offset)
{
  Bit8u val_8;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit8u *hostAddr = (Bit8u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_8 = *hostAddr;
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_8));
  }
  else {
    val_8 = read_physical_byte(pAddr, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }

  return val_8;
}

BX_CPP_INLINE Bit16u BX_CPU_C::vmcb_read16(unsigned offset)
{
  Bit16u val_16;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_16 = ReadHostWordFromLittleEndian(hostAddr);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_16));
  }
  else {
    val_16 = read_physical_word(pAddr, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }

  return val_16;
}

BX_CPP_INLINE Bit32u BX_CPU_C::vmcb_read32(unsigned offset)
{
  Bit32u val_32;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_32 = ReadHostDWordFromLittleEndian(hostAddr);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_32));
  }
  else {
    val_32 = read_physical_dword(pAddr, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }

  return val_32;
}

BX_CPP_INLINE Bit64u BX_CPU_C::vmcb_read64(unsigned offset)
{
  Bit64u val_64;
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    val_64 = ReadHostQWordFromLittleEndian(hostAddr);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_READ, BX_VMCS_ACCESS, (Bit8u*)(&val_64));
  }
  else {
    val_64 = read_physical_qword(pAddr, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }

  return val_64;
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write8(unsigned offset, Bit8u val_8)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit8u *hostAddr = (Bit8u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 1);
    *hostAddr = val_8;
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 1, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_8));
  }
  else {
    write_physical_byte(pAddr, val_8, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write16(unsigned offset, Bit16u val_16)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit16u *hostAddr = (Bit16u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 2);
    WriteHostWordToLittleEndian(hostAddr, val_16);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 2, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_16));
  }
  else {
    write_physical_word(pAddr, val_16, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write32(unsigned offset, Bit32u val_32)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit32u *hostAddr = (Bit32u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 4);
    WriteHostDWordToLittleEndian(hostAddr, val_32);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_32));
  }
  else {
    write_physical_dword(pAddr, val_32, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }
}

BX_CPP_INLINE void BX_CPU_C::vmcb_write64(unsigned offset, Bit64u val_64)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcbptr + offset;

  if (BX_CPU_THIS_PTR vmcbhostptr) {
    Bit64u *hostAddr = (Bit64u*) (BX_CPU_THIS_PTR vmcbhostptr | offset);
    pageWriteStampTable.decWriteStamp(pAddr, 8);
    WriteHostQWordToLittleEndian(hostAddr, val_64);
    BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_WRITE, BX_VMCS_ACCESS, (Bit8u*)(&val_64));
  }
  else {
    write_physical_qword(pAddr, val_64, MEMTYPE(BX_CPU_THIS_PTR vmcb_memtype), BX_VMCS_ACCESS);
  }
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
  // completion of the VMRUN instruction (by #VMEXIT) clears the host EFLAGS.RF
  host->eflags = read_eflags() & ~EFlagsRFMask;
  host->rip = RIP;
  host->rsp = RSP;
  host->rax = RAX;

  host->pat_msr = BX_CPU_THIS_PTR msr.pat;

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    host->ssp = SSP;
    host->msr_ia32_s_cet = BX_CPU_THIS_PTR msr.ia32_cet_control[0];
    host->interrupt_ssp_table_address = BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table;
  }
#endif

  // with SPEC_CTRL virtualization the processor implements separate host and guest SPEC_CTRL registers
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_SPEC_CTRL_VIRT))
    host->spec_ctrl = BX_CPU_THIS_PTR msr.ia32_spec_ctrl;
}

void BX_CPU_C::SvmExitLoadHostState(SVM_HOST_STATE *host)
{
  BX_CPU_THIS_PTR tsc_offset = 0;

  for (unsigned n=0;n < 4; n++) {
    BX_CPU_THIS_PTR sregs[n] = host->sregs[n];
    // we don't save selector details so parse selector again after loading
    parse_selector(BX_CPU_THIS_PTR sregs[n].selector.value, &BX_CPU_THIS_PTR sregs[n].selector);
  }

  // set flags before control registers to avoid false PANIC inside the setEFlags
  setEFlags(host->eflags & ~EFlagsVMMask); // ignore saved copy of EFLAGS.VM

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

  RIP = BX_CPU_THIS_PTR prev_rip = host->rip;
  RSP = host->rsp;
  RAX = host->rax;

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    SSP = host->ssp;
    BX_CPU_THIS_PTR msr.ia32_cet_control[0] = host->msr_ia32_s_cet;
    BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table = host->interrupt_ssp_table_address;
  }
#endif

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_SPEC_CTRL_VIRT))
    BX_CPU_THIS_PTR msr.ia32_spec_ctrl = host->spec_ctrl;

  CPL = 0;

  BX_CPU_THIS_PTR inhibit_mask = 0;

  // host EFLAGS.TF causes #DB trap after the VMRUN completes on the host side (after #VMEXIT)
  if (host->eflags & EFlagsTFMask) {
    BX_CPU_THIS_PTR debug_trap = BX_DEBUG_SINGLE_STEP_BIT;
    BX_CPU_THIS_PTR async_event = 1;
  }
  else {
    BX_CPU_THIS_PTR debug_trap = 0;
  }

  BX_CPU_THIS_PTR activity_state = BX_ACTIVITY_STATE_ACTIVE;

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR wakeup_monitor();
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
  vmcb_write64(SVM_GUEST_CR4, BX_CPU_THIS_PTR cr4.get());

  vmcb_write64(SVM_GUEST_DR6, BX_CPU_THIS_PTR dr6.get32());
  vmcb_write64(SVM_GUEST_DR7, BX_CPU_THIS_PTR dr7.get32());

  vmcb_write64(SVM_GUEST_RFLAGS, read_eflags());
  vmcb_write64(SVM_GUEST_RAX, RAX);
  vmcb_write64(SVM_GUEST_RSP, RSP);
  vmcb_write64(SVM_GUEST_RIP, RIP);

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    vmcb_write64(SVM_GUEST_S_CET, BX_CPU_THIS_PTR msr.ia32_cet_control[0]);
    vmcb_write64(SVM_GUEST_SSP, SSP);
    vmcb_write64(SVM_GUEST_ISST_ADDR, BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table);
  }
#endif

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_SPEC_CTRL_VIRT))
    vmcb_write64(SVM_GUEST_SPEC_CTRL, BX_CPU_THIS_PTR msr.ia32_spec_ctrl);

  vmcb_write8(SVM_GUEST_CPL, CPL);

  vmcb_write8(SVM_CONTROL_INTERRUPT_SHADOW, interrupts_inhibited(BX_INHIBIT_INTERRUPTS));

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  if (ctrls->nested_paging) {
    vmcb_write64(SVM_GUEST_PAT, BX_CPU_THIS_PTR msr.pat.u64);
  }

  vmcb_write8(SVM_CONTROL_VTPR, ctrls->v_tpr);

  // update V_IRQ and VGIF (when Virtual GIF is enabled), keep other bits of the VMCB field
  Bit8u virq_control = vmcb_read8(SVM_CONTROL_VIRQ) & ~0x1;
  if (is_pending(BX_EVENT_SVM_VIRQ_PENDING))
    virq_control |= 0x1;
  if (ctrls->v_gif_enable)
    virq_control = (virq_control & ~0x2) | (ctrls->v_gif ? 0x2 : 0);
  vmcb_write8(SVM_CONTROL_VIRQ, virq_control);
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
  ctrls->intercept_vector[2] = vmcb_read32(SVM_CONTROL32_INTERCEPT3);

  if (! SVM_INTERCEPT(SVM_INTERCEPT1_VMRUN)) {
    BX_ERROR(("VMRUN: VMRUN intercept bit is not set!"));
    return false;
  }

  ctrls->exceptions_intercept = vmcb_read32(SVM_CONTROL32_INTERCEPT_EXCEPTIONS);

  // The MSR or IOIO intercept table must not extend to a physical address that is greater
  // than or equal to the maximum supported physical address:
  //  - IOPM is 64K+3 bits, its last byte is at offset 0x2000 (APM 15.10.1)
  //  - MSRPM is 8K bytes, the address following the table (offset 0x2000) is checked

  // force 4K page alignment
  ctrls->iopm_base = PPFOf(vmcb_read64(SVM_CONTROL64_IOPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->iopm_base) || ! IsValidPhyAddr(ctrls->iopm_base + 0x2000)) {
    BX_ERROR(("VMRUN: invalid IOPM Base Address !"));
    return false;
  }

  // force 4K page alignment
  ctrls->msrpm_base = PPFOf(vmcb_read64(SVM_CONTROL64_MSRPM_BASE_PHY_ADDR));
  if (! IsValidPhyAddr(ctrls->msrpm_base) || ! IsValidPhyAddr(ctrls->msrpm_base + 0x2000)) {
    BX_ERROR(("VMRUN: invalid MSRPM Base Address !"));
    return false;
  }

  Bit32u guest_asid = vmcb_read32(SVM_CONTROL32_GUEST_ASID);
  if (guest_asid == 0) {
    BX_ERROR(("VMRUN: attempt to run guest with host ASID !"));
    return false;
  }

  ctrls->v_tpr = vmcb_read8(SVM_CONTROL_VTPR);
  Bit8u vintr_masking = vmcb_read8(SVM_CONTROL_VINTR_MASKING);
  ctrls->v_intr_masking = vintr_masking & 0x1;
  ctrls->v_intr_vector = vmcb_read8(SVM_CONTROL_VINTR_VECTOR);

  // Virtual GIF: VGIF_EN is VMCB offset 60h bit 25, VGIF value is loaded from VMCB offset 60h bit 9
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_VIRTUAL_GIF))
    ctrls->v_gif_enable = (vintr_masking >> 1) & 0x1;
  else
    ctrls->v_gif_enable = false;
  ctrls->v_gif = ctrls->v_gif_enable ? ((vmcb_read8(SVM_CONTROL_VIRQ) >> 1) & 0x1) : false;

  Bit8u vintr_control = vmcb_read8(SVM_CONTROL_VINTR_PRIO_IGN_TPR);
  ctrls->v_intr_prio = vintr_control & 0xf;
  ctrls->v_ignore_tpr = (vintr_control >> 4) & 0x1;

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_PAUSE_FILTER))
    ctrls->pause_filter_count = vmcb_read16(SVM_CONTROL16_PAUSE_FILTER_COUNT);
  else
    ctrls->pause_filter_count = 0;

  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_PAUSE_FILTER_THRESHOLD))
    ctrls->pause_filter_threshold = vmcb_read16(SVM_CONTROL16_PAUSE_FILTER_THRESHOLD);
  else
    ctrls->pause_filter_threshold = 0;

  ctrls->last_pause_time = 0;

  // VMSAVE and VMLOAD virtualization enable is VMCB offset 0B8h bit 1
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NESTED_VIRTUALIZATION))
    ctrls->virt_vmload_vmsave = (vmcb_read8(SVM_CONTROL_LBR_VIRTUALIZATION_ENABLE) >> 1) & 0x1;
  else
    ctrls->virt_vmload_vmsave = false;

  Bit32u nested_ctrls = vmcb_read32(SVM_CONTROL_NESTED_PAGING_ENABLE);
  ctrls->nested_paging = (nested_ctrls & SVM_NESTED_CTRL_NP_ENABLE) != 0;
  if (! BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NESTED_PAGING)) {
     if (ctrls->nested_paging) {
        BX_ERROR(("VMRUN: Nesting paging is not supported in this SVM configuration !"));
        ctrls->nested_paging = 0;
     }
  }

#if BX_SUPPORT_CET
  ctrls->sss_check = false;
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_SSS_CHECK) && (nested_ctrls & SVM_NESTED_CTRL_SSS_CHECK_ENABLE) != 0) {
    if (! ctrls->nested_paging) {
      BX_ERROR(("VMRUN: attempt to enable supervisor shadow stack restrictions with nested paging disabled !"));
      return false;
    }

    // silently ignored if the host is in legacy non-PAE mode or host EFER.NXE=0
    if (BX_CPU_THIS_PTR cr4.get_PAE() && BX_CPU_THIS_PTR efer.get_NXE())
      ctrls->sss_check = true;
  }
#endif

  if (ctrls->nested_paging) {
    if (! BX_CPU_THIS_PTR cr0.get_PG()) {
      BX_ERROR(("VMRUN: attempt to enter nested paging mode when host paging is disabled !"));
      return false;
    }

    Bit64u guest_pat = vmcb_read64(SVM_GUEST_PAT);
    if (! isValidMSR_PAT(guest_pat)) {
      BX_ERROR(("VMRUN: invalid memory type in guest PAT_MSR !"));
      return false;
    }

    ctrls->ncr3 = vmcb_read64(SVM_CONTROL64_NESTED_PAGING_HOST_CR3);
    if (long_mode()) {
      if (! IsValidPhyAddr(ctrls->ncr3)) {
        BX_ERROR(("VMRUN(): NCR3 reserved bits set !"));
        return false;
      }
    }

    BX_DEBUG(("VMRUN: Starting Nested Paging Mode !"));
  }

  return true;
}

bool BX_CPU_C::SvmEnterLoadCheckGuestState(void)
{
  SVM_GUEST_STATE guest;
  unsigned n;
  bool paged_real_mode = false;

  guest.eflags = vmcb_read32(SVM_GUEST_RFLAGS);
  guest.rip = vmcb_read64(SVM_GUEST_RIP);
  guest.rsp = vmcb_read64(SVM_GUEST_RSP);
  guest.rax = vmcb_read64(SVM_GUEST_RAX);

  Bit32u efer_lo = vmcb_read32(SVM_GUEST_EFER_MSR);
  Bit32u efer_hi = vmcb_read32(SVM_GUEST_EFER_MSR_HI);
  if (efer_hi) {
    BX_ERROR(("VMRUN: Guest EFER[63:32] is not zero"));
    return false;
  }
  guest.efer.set32(efer_lo);

  if (guest.efer.get32() & ~BX_CPU_THIS_PTR efer_suppmask) {
    BX_ERROR(("VMRUN: Guest EFER reserved bits set"));
    return false;
  }

  if (! guest.efer.get_SVME()) {
    BX_ERROR(("VMRUN: Guest EFER.SVME = 0"));
    return false;
  }

  Bit32u cr0_lo = vmcb_read32(SVM_GUEST_CR0);
  Bit32u cr0_hi = vmcb_read32(SVM_GUEST_CR0_HI);
  if (cr0_hi) {
    BX_ERROR(("VMRUN: Guest CR0[63:32] is not zero"));
    return false;
  }
  guest.cr0.set32(cr0_lo);

  if (! guest.cr0.get_CD() && guest.cr0.get_NW()) {
    BX_ERROR(("VMRUN: Guest CR0.NW=1 when CR0.CD=0"));
    return false;
  }

  // always assign EFER.LMA := EFER.LME & CR0.PG
  guest.efer.set_LMA(guest.cr0.get_PG() && guest.efer.get_LME());

  guest.cr2 = vmcb_read64(SVM_GUEST_CR2);
  guest.cr3 = vmcb_read64(SVM_GUEST_CR3);
  if (! IsValidPhyAddr(guest.cr3)) {
    BX_ERROR(("VMRUN: Guest CR3 reserved bits set"));
    return false;
  }

  Bit32u cr4_lo = vmcb_read32(SVM_GUEST_CR4);
  Bit32u cr4_hi = vmcb_read32(SVM_GUEST_CR4_HI);
  if (cr4_hi) {
    BX_ERROR(("VMRUN: Guest CR4[63:32] is not zero"));
    return false;
  }
  guest.cr4.set(cr4_lo);

  if (guest.cr4.get() & ~BX_CPU_THIS_PTR cr4_suppmask) {
    BX_ERROR(("VMRUN: Guest CR4 reserved bits set"));
    return false;
  }

  if (guest.efer.get_LME() && guest.cr0.get_PG()) {
    if (! guest.cr4.get_PAE()) {
      BX_ERROR(("VMRUN: Guest EFER.LME=1 and CR0.PG=1 when CR4.PAE=0"));
      return false;
    }
    if (! guest.cr0.get_PE()) {
      BX_ERROR(("VMRUN: Guest EFER.LME=1 and CR0.PG=1 when CR0.PE=0"));
      return false;
    }
  }

  Bit32u dr6_hi = vmcb_read32(SVM_GUEST_DR6_HI);
  if (dr6_hi) {
    BX_ERROR(("VMRUN: Guest DR6[63:32] is not zero"));
    return false;
  }
  guest.dr6 = vmcb_read32(SVM_GUEST_DR6);

  Bit32u dr7_hi = vmcb_read32(SVM_GUEST_DR7_HI);
  if (dr7_hi) {
    BX_ERROR(("VMRUN: Guest DR7[63:32] is not zero"));
    return false;
  }
  guest.dr7 = vmcb_read32(SVM_GUEST_DR7);

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    guest.msr_ia32_s_cet = vmcb_read64(SVM_GUEST_S_CET);
    if (! IsCanonical(guest.msr_ia32_s_cet) || ! is_valid_cet_control(guest.msr_ia32_s_cet)) {
      BX_ERROR(("VMRUN: Guest S_CET reserved bits set"));
      return false;
    }

    // SSP and ISST_ADDR are canonicalized (not checked) by VMRUN
    guest.ssp = CanonicalizeAddress(vmcb_read64(SVM_GUEST_SSP));
    guest.interrupt_ssp_table_address = CanonicalizeAddress(vmcb_read64(SVM_GUEST_ISST_ADDR));

    if (guest.cr4.get_CET()) {
      if (! guest.cr0.get_WP()) {
        BX_ERROR(("VMRUN: Guest CR4.CET=1 when CR0.WP=0"));
        return false;
      }

      // U_CET is not part of the VMCB state, the current U_CET.SH_STK_EN value is used
      if ((guest.eflags & EFlagsVMMask) != 0 && (BX_CPU_THIS_PTR msr.ia32_cet_control[1] & 0x1) != 0) {
        BX_ERROR(("VMRUN: Guest CR4.CET=1 and U_CET.SH_STK_EN=1 when EFLAGS.VM=1"));
        return false;
      }
    }
  }
#endif

  guest.pat_msr = vmcb_read64(SVM_GUEST_PAT);

  for (n=0;n < 4; n++) {
    svm_segment_read(&guest.sregs[n], SVM_GUEST_ES_SELECTOR + n * 0x10);
  }

  // CS.D_B and CS.L both set is illegal only when entering long mode guest
  if (guest.efer.get_LME() && guest.cr0.get_PG() && guest.cr4.get_PAE()) {
    if (guest.sregs[BX_SEG_REG_CS].cache.u.segment.d_b && guest.sregs[BX_SEG_REG_CS].cache.u.segment.l) {
      BX_ERROR(("VMRUN: VMCB CS.D_B/L mismatch"));
      return false;
    }
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
      guest.cr0.val &= ~BX_CR0_PG_MASK;
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
    return false;
  }
  if (! check_CR4(guest.cr4.get())) {
    BX_ERROR(("SVM: VMRUN CR4 is broken !"));
    return false;
  }

  BX_CPU_THIS_PTR cr0.set32(guest.cr0.get32());
  BX_CPU_THIS_PTR cr4.set(guest.cr4.get());
  BX_CPU_THIS_PTR cr3 = guest.cr3;
  BX_CPU_THIS_PTR cr2 = guest.cr2;

  if (paged_real_mode)
    BX_CPU_THIS_PTR cr0.val |= BX_CR0_PG_MASK;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;
  if (! ctrls->nested_paging) {
    if (BX_CPU_THIS_PTR cr0.get_PG() && BX_CPU_THIS_PTR cr4.get_PAE() && !long_mode()) {
      if (! CheckPDPTR(BX_CPU_THIS_PTR cr3)) {
        BX_ERROR(("SVM: VMRUN PDPTR check failed !"));
        return false;
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

#if BX_SUPPORT_CET
  if (BX_CPUID_SUPPORT_ISA_EXTENSION(BX_ISA_CET)) {
    SSP = guest.ssp;
    BX_CPU_THIS_PTR msr.ia32_cet_control[0] = guest.msr_ia32_s_cet;
    BX_CPU_THIS_PTR msr.ia32_interrupt_ssp_table = guest.interrupt_ssp_table_address;
  }
#endif

  // guest SPEC_CTRL register is in effect (logically ORed with the host SPEC_CTRL) until #VMEXIT
  if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_SPEC_CTRL_VIRT))
    BX_CPU_THIS_PTR msr.ia32_spec_ctrl = (Bit32u) vmcb_read64(SVM_GUEST_SPEC_CTRL);

  CPL = guest.cpl;

  if (guest.inhibit_interrupts) {
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS);
  }

  BX_CPU_THIS_PTR async_event = 0;

  // host EFLAGS have no effect on the guest: single step trap of the VMRUN instruction
  // is taken on the host side after #VMEXIT and must not be delivered to the guest
  BX_CPU_THIS_PTR debug_trap = 0;

  setEFlags((Bit32u) guest.eflags);

  // injecting virtual interrupt
  Bit8u v_irq = vmcb_read8(SVM_CONTROL_VIRQ) & 0x1;
  if (v_irq)
    signal_event(BX_EVENT_SVM_VIRQ_PENDING);

  handleCpuContextChange();

#if BX_SUPPORT_MONITOR_MWAIT
  BX_CPU_THIS_PTR wakeup_monitor();
#endif

  BX_INSTR_TLB_CNTRL(BX_CPU_ID, BX_INSTR_CONTEXT_SWITCH, 0);

  return true;
}

// Collect up to 15 guest instruction bytes at guest CS:RIP for SVM decode
// assist. No new page walk is done (it could fault), the bytes are taken from
// the code pages already translated by prefetch() for the current instruction:
//  - the current fetch window when the instruction starts inside it
//  - the previous page when the fetch window was moved by boundaryFetch()
//    while fetching page split instruction
//  - the 2nd page of page split trace when page split instruction was
//    executed directly from the trace cache
// Returns the number of bytes fetched, zero if the guest CS:RIP was never
// fetched (for example a fault during event delivery).
unsigned BX_CPU_C::SvmFetchGuestInstructionBytes(Bit8u *bytes)
{
  if (BX_CPU_THIS_PTR eipPageWindowSize == 0) return 0; // fetch window is invalid

  bx_address eipBiased = BX_CPU_THIS_PTR prev_rip + BX_CPU_THIS_PTR eipPageBias;

  // the instruction starts at pageOffset in page and may continue to nextPage
  const Bit8u *page, *nextPage = NULL;
  unsigned pageOffset;

  if (eipBiased < BX_CPU_THIS_PTR eipPageWindowSize) {
    // the instruction starts in the current fetch window, if it is page split
    // instruction executed from the trace cache - find its 2nd page
    page = BX_CPU_THIS_PTR eipFetchPtr;
    pageOffset = (unsigned) eipBiased;
    bxTraceCacheEntry_c *e = BX_CPU_THIS_PTR traceCache->find_entry(BX_CPU_THIS_PTR pAddrFetchPage + pageOffset, BX_CPU_THIS_PTR fetchModeMask);
    if (e != NULL && (pageOffset + e->i->ilen()) > 4096) {
      bx_phy_address pAddrNextPage = BX_CPU_THIS_PTR traceCache->find_page_split_ppf(e);
      if (pAddrNextPage != BX_ICACHE_INVALID_PHY_ADDRESS)
        nextPage = (const Bit8u*) getHostMemAddr(pAddrNextPage, BX_EXECUTE);
    }
  }
  else if (BX_CPU_THIS_PTR pAddrFetchPrevPage != BX_ICACHE_INVALID_PHY_ADDRESS && (bx_address)(0 - eipBiased) < 15) {
    // the fetch window was established by boundaryFetch(), the page split
    // instruction starts at the end of the previous page
    page = (const Bit8u*) getHostMemAddr(BX_CPU_THIS_PTR pAddrFetchPrevPage, BX_EXECUTE);
    if (! page) return 0;
    pageOffset = (unsigned) (eipBiased + 4096);
    nextPage = BX_CPU_THIS_PTR eipFetchPtr;
  }
  else {
    return 0; // the instruction at guest CS:RIP was never fetched
  }

  // fetch up to 15 bytes but do not cross CS limit
  unsigned len = 15;
  if (! long64_mode()) {
    Bit32u eip = (Bit32u) BX_CPU_THIS_PTR prev_rip; // fetched instruction, eip <= limit
    Bit32u limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;
    if ((limit - eip) < 14) len = limit - eip + 1;
  }

  unsigned fetched = 4096 - pageOffset;
  if (fetched > len) fetched = len;
  memcpy(bytes, page + pageOffset, fetched);

  if (fetched < len && nextPage) {
    memcpy(bytes + fetched, nextPage, len - fetched);
    fetched = len;
  }

  return fetched;
}

// nRIP is saved on #VMEXITs due to instruction intercepts (APM Table 15-7), MSR and IOIO intercepts
// and exceptions caused by the INT3, INTO and BOUND instructions, for all other intercepts it is reset to zero
static bool svm_vmexit_saves_nrip(int reason)
{
  if (reason < SVM_VMEXIT_EXCEPTION) return true; // CR and DR accesses

  switch(reason) {
  case SVM_VMEXIT_EXCEPTION + BX_BP_EXCEPTION:
  case SVM_VMEXIT_EXCEPTION + BX_OF_EXCEPTION:
  case SVM_VMEXIT_EXCEPTION + BX_BR_EXCEPTION:
    return true;

  case SVM_VMEXIT_TASK_SWITCH:
  case SVM_VMEXIT_FERR_FREEZE:
  case SVM_VMEXIT_SHUTDOWN:
    return false;

  case SVM_VMEXIT_IDLE_HLT:
    return true;
  }

  if (reason >= SVM_VMEXIT_CR0_SEL_WRITE && reason <= SVM_VMEXIT_MSR) return true;
  if (reason >= SVM_VMEXIT_VMRUN && reason <= SVM_VMEXIT_RDPRU) return true;
  if (reason >= SVM_VMEXIT_INVLPGB && reason <= SVM_VMEXIT_TLBSYNC) return true;

  return false;
}

void BX_CPU_C::Svm_Vmexit(int reason, Bit64u exitinfo1, Bit64u exitinfo2)
{
  BX_DEBUG(("SVM VMEXIT reason=%d exitinfo1=%08x%08x exitinfo2=%08x%08x", reason,
    GET32H(exitinfo1), GET32L(exitinfo1), GET32H(exitinfo2), GET32L(exitinfo2)));

  if (! BX_CPU_THIS_PTR in_svm_guest) {
    if (reason != SVM_VMEXIT_INVALID)
      BX_PANIC(("PANIC: VMEXIT %d not in SVM guest mode !", reason));
  }

  if (reason != SVM_VMEXIT_INVALID) {
    if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NRIP_SAVE))
      vmcb_write64(SVM_CONTROL64_NRIP, svm_vmexit_saves_nrip(reason) ? RIP : 0);

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
        Bit8u instr_bytes[16];
        memset(instr_bytes, 0, sizeof(instr_bytes));
        instr_bytes[0] = (Bit8u) SvmFetchGuestInstructionBytes(instr_bytes + 1);

        for (unsigned n=0; n < 16; n++)
          vmcb_write8(SVM_CONTROL64_GUEST_INSTR_BYTES + n, instr_bytes[n]);
      }
      else {
        vmcb_write8(SVM_CONTROL64_GUEST_INSTR_BYTES, 0);
      }
    }
  }

  // VMEXITs are FAULT-like: restore RIP/RSP to value before VMEXIT occurred
  RIP = BX_CPU_THIS_PTR prev_rip;
  if (BX_CPU_THIS_PTR speculative_rsp) {
    RSP = BX_CPU_THIS_PTR prev_rsp;
#if BX_SUPPORT_CET
    SSP = BX_CPU_THIS_PTR prev_ssp;
#endif
  }
  BX_CPU_THIS_PTR speculative_rsp = false;

#if BX_SUPPORT_MPX
  // near branch which requested BND0-3 initialization has faulted
  if (BX_CPU_THIS_PTR bndregs_clear && BX_CPU_THIS_PTR bndregs_clear_icount == get_icount())
    BX_CPU_THIS_PTR bndregs_clear = false;
#endif

  mask_event(BX_EVENT_SVM_VIRQ_PENDING);

  BX_CPU_THIS_PTR in_svm_guest = false;
  BX_CPU_THIS_PTR svm_gif = false;

  //
  // STEP 0: Update exit reason
  //

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  vmcb_write64(SVM_CONTROL64_EXITCODE, (Bit64u) ((Bit64s) reason));
  vmcb_write64(SVM_CONTROL64_EXITINFO1, exitinfo1);
  vmcb_write64(SVM_CONTROL64_EXITINFO2, exitinfo2);

  // clean interrupt injection field
  vmcb_write32(SVM_CONTROL32_EVENT_INJECTION, ctrls->eventinj & ~0x80000000);

  if (BX_CPU_THIS_PTR in_event) {
    vmcb_write32(SVM_CONTROL32_EXITINTINFO, ctrls->exitintinfo | 0x80000000);
    vmcb_write32(SVM_CONTROL32_EXITINTINFO_ERROR_CODE, ctrls->exitintinfo_error_code);
    BX_CPU_THIS_PTR in_event = false;
  }
  else {
    vmcb_write32(SVM_CONTROL32_EXITINTINFO, 0);
  }

  //
  // Step 1: Save guest state in the VMCB
  //
  if (reason != SVM_VMEXIT_INVALID)
    SvmExitSaveGuestState();

  //
  // Step 2:
  //
  SvmExitLoadHostState(&BX_CPU_THIS_PTR vmcb->host_state);

  //
  // STEP 3: Go back to SVM host
  //

  BX_CPU_THIS_PTR activity_state = BX_ACTIVITY_STATE_ACTIVE;
  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR last_exception_type = BX_ET_NONE; // error resolved

#if BX_DEBUGGER
  if (bx_dbg.debugger_active) {
    if (BX_CPU_THIS_PTR vmexit_break) {
      BX_CPU_THIS_PTR stop_reason = STOP_VMEXIT_BREAK_POINT;
      bx_debug_break(); // trap into debugger
    }
  }
#endif

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

extern int get_exception_type(unsigned vector);

// TYPE=3 (exception) event injection must specify a vector of an exception supported by the CPU,
// otherwise VMRUN fails with VMEXIT_INVALID
bool BX_CPU_C::SvmInjectedVectorIsException(unsigned vector)
{
  switch(vector) {
    case BX_DE_EXCEPTION:
    case BX_DB_EXCEPTION:
    case BX_BP_EXCEPTION:
    case BX_OF_EXCEPTION:
    case BX_BR_EXCEPTION:
    case BX_UD_EXCEPTION:
    case BX_NM_EXCEPTION:
    case BX_DF_EXCEPTION:
    case BX_TS_EXCEPTION:
    case BX_NP_EXCEPTION:
    case BX_SS_EXCEPTION:
    case BX_GP_EXCEPTION:
    case BX_PF_EXCEPTION:
    case BX_MF_EXCEPTION:
    case BX_AC_EXCEPTION:
    case BX_MC_EXCEPTION:
    case BX_XM_EXCEPTION:
    case BX_SX_EXCEPTION:
      return true;

    case BX_CP_EXCEPTION:
      return is_cpu_extension_supported(BX_ISA_CET);

    default:
      // NMI, coprocessor segment overrun (9), #VE (raised only by VMX) and reserved vectors
      return false;
  }
}

bool BX_CPU_C::SvmInjectEvents(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  ctrls->eventinj = vmcb_read32(SVM_CONTROL32_EVENT_INJECTION);
  if ((ctrls->eventinj & 0x80000000) == 0) return true;

  /* the VMENTRY injecting event to the guest */
  unsigned vector = ctrls->eventinj & 0xff;
  unsigned type = (ctrls->eventinj >> 8) & 7;
  unsigned push_error = ctrls->eventinj & (1 << 11);
  unsigned error_code = push_error ? vmcb_read32(SVM_CONTROL32_EVENT_INJECTION_ERRORCODE) : 0;

  switch(type) {
    case BX_NMI:
      BX_CPU_THIS_PTR EXT = 1; // injected NMI doesn't mask delivery of further NMIs
      vector = 2;
      break;

    case BX_EXTERNAL_INTERRUPT:
      BX_CPU_THIS_PTR EXT = 1;
      break;

    case BX_HARDWARE_EXCEPTION:
      if (! SvmInjectedVectorIsException(vector)) {
        BX_ERROR(("SvmInjectEvents: invalid vector %d for HW exception", vector));
        return false;
      }
      if ((vector == BX_BR_EXCEPTION || vector == BX_OF_EXCEPTION) && long64_mode()) {
        BX_ERROR(("SvmInjectEvents: invalid vector %d for 64-bit mode", vector));
        return false;
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
      return false;
  }

  BX_DEBUG(("SvmInjectEvents: Injecting vector 0x%02x (error_code 0x%04x)", vector, error_code));

  if (type == BX_HARDWARE_EXCEPTION) {
    // record exception the same way as BX_CPU_C::exception does
    BX_ASSERT(vector < BX_CPU_HANDLED_EXCEPTIONS);
    BX_CPU_THIS_PTR last_exception_type = get_exception_type(vector);
  }

  // injected software interrupt and #BP/#OF exception behave like INTn/INT3/INTO instruction and push
  // the next RIP taken from the VMCB, a fault during the event delivery is reported on the guest RIP (prev_rip)
  if (type == BX_SOFTWARE_INTERRUPT || type == BX_SOFTWARE_EXCEPTION) {
    if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_NRIP_SAVE))
      RIP = vmcb_read64(SVM_CONTROL64_NRIP);
  }

  interrupt(vector, type, push_error, error_code);

  BX_CPU_THIS_PTR last_exception_type = BX_ET_NONE; // error resolved

  return true;
}

void BX_CPU_C::SvmRecordExitIntInfo(unsigned type, unsigned vector, bool push_error, Bit16u error_code)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  // -----------------------------------------
  //              EXITINTINFO
  // -----------------------------------------
  // [07:00] | Interrupt/Exception vector
  // [10:08] | Interrupt/Exception type
  // [11:11] | error code pushed to the stack
  // [30:12] | reserved
  // [31:31] | interruption info valid
  //

  // record the event being delivered through the IDT, INT1/INT3/INTO are reported as exceptions
  unsigned svm_type = BX_HARDWARE_EXCEPTION;
  if (type == BX_EXTERNAL_INTERRUPT || type == BX_NMI || type == BX_SOFTWARE_INTERRUPT)
    svm_type = type;

  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  ctrls->exitintinfo = vector | (svm_type << 8);
  if (push_error)
    ctrls->exitintinfo |= (1 << 11); // error code delivered
  ctrls->exitintinfo_error_code = error_code;
}

void BX_CPU_C::SvmInterceptException(unsigned type, unsigned vector, Bit16u errcode, bool errcode_valid, Bit64u qualification)
{
  if (! BX_CPU_THIS_PTR in_svm_guest) return;

  BX_ASSERT(vector < 32);

  BX_ASSERT(type == BX_HARDWARE_EXCEPTION || type == BX_SOFTWARE_EXCEPTION);

  // EXITINTINFO for non-intercepted exception is recorded by interrupt() after combining with prior events
  if (! SVM_EXCEPTION_INTERCEPTED(vector)) return;

  BX_ERROR(("SVM VMEXIT: event vector 0x%02x type %d error code=0x%04x", vector, type, errcode));

  // VMEXIT is not considered to occur during event delivery if it results
  // in a double fault exception that causes VMEXIT directly
  if (vector == BX_DF_EXCEPTION)
    BX_CPU_THIS_PTR in_event = false; // clear in_event indication on #DF

  BX_CPU_THIS_PTR debug_trap = 0; // clear debug_trap field
  // a #DB intercept keeps the interrupt shadow (e.g. after STI): it is saved in the VMCB
  if (vector != BX_DB_EXCEPTION)
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
  pAddr = BX_CPU_THIS_PTR vmcb->ctrls.iopm_base + (port / 8);
  bitmap[0] = read_physical_byte(pAddr, MEMTYPE(resolve_memtype(pAddr)), BX_IO_BITMAP_ACCESS);

  pAddr++;
  bitmap[1] = read_physical_byte(pAddr, MEMTYPE(resolve_memtype(pAddr)), BX_IO_BITMAP_ACCESS);

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
    bx_phy_address msr_bitmap_addr = BX_CPU_THIS_PTR vmcb->ctrls.msrpm_base + msr_map_offset;
    Bit32u msr_offset = (msr & 0x1fff) * 2 + op;

    bx_phy_address pAddr = msr_bitmap_addr + (msr_offset / 8);
    Bit8u msr_bitmap = read_physical_byte(pAddr, MEMTYPE(resolve_memtype(pAddr)), BX_MSR_BITMAP_ACCESS);

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
    SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;
    // with PAUSE filter threshold set to zero the filter operates in count only mode
    if (BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_PAUSE_FILTER_THRESHOLD) && ctrls->pause_filter_threshold != 0) {
      Bit64u currtime = bx_pc_system.time_ticks();
      Bit64u time_from_last_pause = currtime - ctrls->last_pause_time;
      ctrls->last_pause_time = currtime;
      if (time_from_last_pause > ctrls->pause_filter_threshold) {
        ctrls->pause_filter_count = vmcb_read16(SVM_CONTROL16_PAUSE_FILTER_COUNT);
        return;
      }
    }
    if (ctrls->pause_filter_count) {
      ctrls->pause_filter_count--;
      return;
    }
  }

  Svm_Vmexit(SVM_VMEXIT_PAUSE);
}

void BX_CPU_C::Svm_Update_VM_CR_MSR(Bit64u val_64)
{
  // VM_CR_MSR 0xc0010114

  // [0]: DPD - If set, disables the external hardware debug port and certain internal debug features
  // [1]: R_INIT - If set, non-intercepted INIT signals are converted into an #SX exception
  // [2]: DIS_A20M - If set, disables A20 masking
  // [3]: LOCK - When this bit is set, writes to LOCK and SVMDIS are silently ignored
  // [4]: SVMDIS - When this bit is set, writes to EFER treat the SVME bit as MBZ
  //      Setting SVMDIS while EFER.SVME is 1 generates a #GP fault, regardless of the current state of VM_CR.LOCK
  //      It is cleared by INIT when LOCK is cleared to 0; otherwise, it is not affected.
  if (val_64 >> 5) {
    BX_ERROR(("VM_CR_MSR: attempt to set reserved bits"));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (val_64 & BX_VM_CR_MSR_SVMDIS_MASK) {
    if (BX_CPU_THIS_PTR efer.get_SVME()) {
      BX_ERROR(("VM_CR_MSR: attempt to set SVMDIS when EFER.SVME=1"));
      exception(BX_GP_EXCEPTION, 0);
    }
  }

  if (BX_CPU_THIS_PTR msr.svm_vm_cr & BX_VM_CR_MSR_LOCK_MASK) {
    val_64 = (val_64 & 0x7) | (BX_CPU_THIS_PTR msr.svm_vm_cr & (BX_VM_CR_MSR_LOCK_MASK | BX_VM_CR_MSR_SVMDIS_MASK));
  }

  BX_CPU_THIS_PTR msr.svm_vm_cr = GET32L(val_64);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMRUN(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
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
  SvmEnterSaveHostState(&BX_CPU_THIS_PTR vmcb->host_state);

  //
  // Step 2: Load control information from the VMCB
  //
  if (!SvmEnterLoadCheckControls(&BX_CPU_THIS_PTR vmcb->ctrls))
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  //
  // Step 3: Load guest state from the VMCB and enter SVM
  //
  if (!SvmEnterLoadCheckGuestState())
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  BX_CPU_THIS_PTR in_svm_guest = true;
  handleInterruptMaskChange(); // re-evaluate the interrupt masks as a guest: V_IRQ with IF already set (APM 15.21.4)
  BX_CPU_THIS_PTR svm_gif = true;
  BX_CPU_THIS_PTR async_event = 1;

  //
  // Step 4: Inject events to the guest
  //
  if (!SvmInjectEvents())
    Svm_Vmexit(SVM_VMEXIT_INVALID);

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMMCALL(bxInstruction_c *i)
{
  if (BX_CPU_THIS_PTR efer.get_SVME()) {
    if (BX_CPU_THIS_PTR in_svm_guest) {
      if (SVM_INTERCEPT(SVM_INTERCEPT1_VMMCALL)) Svm_Vmexit(SVM_VMEXIT_VMMCALL);
    }
  }

  exception(BX_UD_EXCEPTION, 0);

  BX_NEXT_TRACE(i);
}

// Check VMLOAD/VMSAVE intercept and return physical address of the VMCB used by the instruction
bx_phy_address BX_CPU_C::SvmVmloadVmsaveAddress(bxInstruction_c *i, bool vmsave)
{
  bool virtualized = false;

  if (BX_CPU_THIS_PTR in_svm_guest) {
    unsigned intercept = vmsave ? SVM_INTERCEPT1_VMSAVE : SVM_INTERCEPT1_VMLOAD;
    int vmexit_reason = vmsave ? SVM_VMEXIT_VMSAVE : SVM_VMEXIT_VMLOAD;

    if (SVM_INTERCEPT(intercept)) Svm_Vmexit(vmexit_reason);

    // VMSAVE and VMLOAD virtualization requires nested paging enabled and the host in 64-bit mode
    if (BX_CPU_THIS_PTR vmcb->ctrls.virt_vmload_vmsave) {
      if (! SVM_NESTED_PAGING_ENABLED || ! BX_CPU_THIS_PTR vmcb->host_state.efer.get_LMA())
        Svm_Vmexit(vmexit_reason);
      virtualized = true;
    }
  }

  bx_address pAddr = RAX & i->asize_mask();
  if (! IsValidPageAlignedPhyAddr(pAddr)) {
    BX_ERROR(("%s: invalid or not page aligned VMCB physical address !", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  // with VMSAVE and VMLOAD virtualization RAX holds guest physical address of the VMCB which is
  // translated by nested page tables, a nested page fault causes #VMEXIT(NPF)
  if (virtualized)
    return nested_walk(pAddr, vmsave ? BX_WRITE : BX_READ, false);

  return pAddr;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMLOAD(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_phy_address pAddr = SvmVmloadVmsaveAddress(i, false);
  bx_phy_address vmcbPtr = BX_CPU_THIS_PTR vmcbptr;
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
  BX_CPU_THIS_PTR msr.fmask = (Bit32u) vmcb_read64(SVM_GUEST_FMASK_MSR); // bits [63:32] are ignored

  BX_CPU_THIS_PTR msr.sysenter_cs_msr = (Bit32u) vmcb_read64(SVM_GUEST_SYSENTER_CS_MSR); // bits [63:32] are ignored
  BX_CPU_THIS_PTR msr.sysenter_eip_msr = CanonicalizeAddress(vmcb_read64(SVM_GUEST_SYSENTER_EIP_MSR));
  BX_CPU_THIS_PTR msr.sysenter_esp_msr = CanonicalizeAddress(vmcb_read64(SVM_GUEST_SYSENTER_ESP_MSR));

  set_VMCBPTR(vmcbPtr);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::VMSAVE(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_phy_address pAddr = SvmVmloadVmsaveAddress(i, true);
  bx_phy_address vmcbPtr = BX_CPU_THIS_PTR vmcbptr;
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

  set_VMCBPTR(vmcbPtr);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SKINIT(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_SKINIT)) Svm_Vmexit(SVM_VMEXIT_SKINIT);
  }

  BX_PANIC(("SVM: SKINIT is not implemented yet"));

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CLGI(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_CLGI)) Svm_Vmexit(SVM_VMEXIT_CLGI);

    // with Virtual GIF enabled the guest CLGI clears VGIF which masks virtual interrupts
    if (SVM_V_GIF_ENABLE) {
      SVM_V_GIF = false;
      handleInterruptMaskChange();
      BX_NEXT_TRACE(i);
    }
  }

  BX_CPU_THIS_PTR svm_gif = false;

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::STGI(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT1_STGI)) Svm_Vmexit(SVM_VMEXIT_STGI);

    // with Virtual GIF enabled the guest STGI sets VGIF which unmasks virtual interrupts
    if (SVM_V_GIF_ENABLE) {
      SVM_V_GIF = true;
      handleInterruptMaskChange();
      BX_CPU_THIS_PTR async_event = 1;
      BX_NEXT_TRACE(i);
    }
  }

  BX_CPU_THIS_PTR svm_gif = true;
  BX_CPU_THIS_PTR async_event = 1;

  BX_NEXT_TRACE(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::INVLPGA(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR efer.get_SVME())
    exception(BX_UD_EXCEPTION, 0);

  if (CPL != 0) {
    BX_ERROR(("%s: with CPL!=0 cause #GP(0)", i->getIaOpcodeNameShort()));
    exception(BX_GP_EXCEPTION, 0);
  }

  bx_address laddr = RAX & i->asize_mask();

  if (BX_CPU_THIS_PTR in_svm_guest) {
    if (SVM_INTERCEPT(SVM_INTERCEPT0_INVLPGA))
      Svm_Vmexit(SVM_VMEXIT_INVLPGA, BX_SUPPORT_SVM_EXTENSION(BX_CPUID_SVM_DECODE_ASSIST) ? laddr : 0);
  }

  TLB_invlpg(laddr); // FIXME: flush all ASID entries for now

  BX_NEXT_TRACE(i);
}

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

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_rd_ctrl, BX_CPU_THIS_PTR vmcb->ctrls.cr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, cr_wr_ctrl, BX_CPU_THIS_PTR vmcb->ctrls.cr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_rd_ctrl, BX_CPU_THIS_PTR vmcb->ctrls.dr_rd_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, dr_wr_ctrl, BX_CPU_THIS_PTR vmcb->ctrls.dr_wr_ctrl);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exceptions_intercept, BX_CPU_THIS_PTR vmcb->ctrls.exceptions_intercept);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector0, BX_CPU_THIS_PTR vmcb->ctrls.intercept_vector[0]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector1, BX_CPU_THIS_PTR vmcb->ctrls.intercept_vector[1]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, intercept_vector2, BX_CPU_THIS_PTR vmcb->ctrls.intercept_vector[2]);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, iopm_base, BX_CPU_THIS_PTR vmcb->ctrls.iopm_base);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, msrpm_base, BX_CPU_THIS_PTR vmcb->ctrls.msrpm_base);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exitintinfo, BX_CPU_THIS_PTR vmcb->ctrls.exitintinfo);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, exitintinfo_errcode, BX_CPU_THIS_PTR vmcb->ctrls.exitintinfo_error_code);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, eventinj, BX_CPU_THIS_PTR vmcb->ctrls.eventinj);

  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_tpr, BX_CPU_THIS_PTR vmcb->ctrls.v_tpr);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_prio, BX_CPU_THIS_PTR vmcb->ctrls.v_intr_prio);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_ignore_tpr, BX_CPU_THIS_PTR vmcb->ctrls.v_ignore_tpr);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_intr_masking, BX_CPU_THIS_PTR vmcb->ctrls.v_intr_masking);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, v_intr_vector, BX_CPU_THIS_PTR vmcb->ctrls.v_intr_vector);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_gif_enable, BX_CPU_THIS_PTR vmcb->ctrls.v_gif_enable);
  BXRS_PARAM_BOOL(vmcb_ctrls, v_gif, BX_CPU_THIS_PTR vmcb->ctrls.v_gif);
  BXRS_PARAM_BOOL(vmcb_ctrls, nested_paging, BX_CPU_THIS_PTR vmcb->ctrls.nested_paging);
  BXRS_PARAM_BOOL(vmcb_ctrls, virt_vmload_vmsave, BX_CPU_THIS_PTR vmcb->ctrls.virt_vmload_vmsave);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, ncr3, BX_CPU_THIS_PTR vmcb->ctrls.ncr3);
#if BX_SUPPORT_CET
  BXRS_PARAM_BOOL(vmcb_ctrls, sss_check, BX_CPU_THIS_PTR vmcb->ctrls.sss_check);
#endif
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, pause_filter_count, BX_CPU_THIS_PTR vmcb->ctrls.pause_filter_count);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, pause_filter_threshold, BX_CPU_THIS_PTR vmcb->ctrls.pause_filter_threshold);
  BXRS_HEX_PARAM_FIELD(vmcb_ctrls, last_pause_time, BX_CPU_THIS_PTR vmcb->ctrls.last_pause_time);

  //
  // VMCB Host State
  //

  bx_list_c *host = new bx_list_c(svm, "VMCB_HOST_STATE");

  for(unsigned n=0; n<4; n++) {
    bx_segment_reg_t *segment = &BX_CPU_THIS_PTR vmcb->host_state.sregs[n];
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
  BXRS_HEX_PARAM_FIELD(GDTR, base, BX_CPU_THIS_PTR vmcb->host_state.gdtr.base);
  BXRS_HEX_PARAM_FIELD(GDTR, limit, BX_CPU_THIS_PTR vmcb->host_state.gdtr.limit);

  bx_list_c *IDTR = new bx_list_c(host, "IDTR");
  BXRS_HEX_PARAM_FIELD(IDTR, base, BX_CPU_THIS_PTR vmcb->host_state.idtr.base);
  BXRS_HEX_PARAM_FIELD(IDTR, limit, BX_CPU_THIS_PTR vmcb->host_state.idtr.limit);

  BXRS_HEX_PARAM_FIELD(host, efer, BX_CPU_THIS_PTR vmcb->host_state.efer.val);
  BXRS_HEX_PARAM_FIELD(host, cr0, BX_CPU_THIS_PTR vmcb->host_state.cr0.val);
  BXRS_HEX_PARAM_FIELD(host, cr3, BX_CPU_THIS_PTR vmcb->host_state.cr3);
  BXRS_HEX_PARAM_FIELD(host, cr4, BX_CPU_THIS_PTR vmcb->host_state.cr4.val);
  BXRS_HEX_PARAM_FIELD(host, eflags, BX_CPU_THIS_PTR vmcb->host_state.eflags);
  BXRS_HEX_PARAM_FIELD(host, rip, BX_CPU_THIS_PTR vmcb->host_state.rip);
  BXRS_HEX_PARAM_FIELD(host, rsp, BX_CPU_THIS_PTR vmcb->host_state.rsp);
  BXRS_HEX_PARAM_FIELD(host, rax, BX_CPU_THIS_PTR vmcb->host_state.rax);
#if BX_SUPPORT_CET
  BXRS_HEX_PARAM_FIELD(host, ssp, BX_CPU_THIS_PTR vmcb->host_state.ssp);
  BXRS_HEX_PARAM_FIELD(host, s_cet, BX_CPU_THIS_PTR vmcb->host_state.msr_ia32_s_cet);
  BXRS_HEX_PARAM_FIELD(host, interrupt_ssp_table_address, BX_CPU_THIS_PTR vmcb->host_state.interrupt_ssp_table_address);
#endif
  BXRS_HEX_PARAM_FIELD(host, spec_ctrl, BX_CPU_THIS_PTR vmcb->host_state.spec_ctrl);
}

#endif // BX_SUPPORT_SVM
