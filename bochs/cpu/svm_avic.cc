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

// AMD Advanced Virtual Interrupt Controller (AVIC), xAVIC mode only (APM Vol2 15.29)
// behvaiors not defined by APM tested against hardware

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_SVM

#include "svm.h"
#include "cpuid.h"
#include "scalar_arith.h"   // for most_significant_bitd

#include "memory/memory-bochs.h"

#if BX_SUPPORT_APIC
#include "apic.h"
#endif

// guest vAPIC register access behavior (APM Table 15-22)
enum {
  BX_AVIC_ALLOW = 0,   // access completes in the backing page
  BX_AVIC_FAULT = 1,   // #VMEXIT(AVIC_NOACCEL) before the access
  BX_AVIC_TRAP  = 2,   // access completes, #VMEXIT(AVIC_NOACCEL) after the instruction
  BX_AVIC_ACCEL = 3    // access completes, AVIC hardware acts after the instruction
};

static const char *avic_access_name[] = { "ALLOW", "FAULT", "TRAP", "ACCEL" };

// xAVIC register access behavior for 16-byte aligned backing page offset (APM Table 15-22)
// CPUID Fn8000_000A_EDX[27] (ExtLvtAvicAccessChg) is not supported
static unsigned avic_register_access(unsigned offset, bool write)
{
  switch(offset) {
  case BX_LAPIC_ID:
  case BX_LAPIC_RRD:
  case BX_LAPIC_LDR:
  case BX_LAPIC_DESTINATION_FORMAT:
  case BX_LAPIC_SPURIOUS_VECTOR:
  case BX_LAPIC_ESR:
  case BX_LAPIC_LVT_TIMER:
  case BX_LAPIC_LVT_THERMAL:
  case BX_LAPIC_LVT_PERFMON:
  case BX_LAPIC_LVT_LINT0:
  case BX_LAPIC_LVT_LINT1:
  case BX_LAPIC_LVT_ERROR:
  case BX_LAPIC_TIMER_INITIAL_COUNT:
  case BX_LAPIC_TIMER_DIVIDE_CFG:
    return write ? BX_AVIC_TRAP : BX_AVIC_ALLOW;

  case BX_LAPIC_VERSION:
  case BX_LAPIC_PPR:
  case BX_LAPIC_ISR1: case BX_LAPIC_ISR2: case BX_LAPIC_ISR3: case BX_LAPIC_ISR4:
  case BX_LAPIC_ISR5: case BX_LAPIC_ISR6: case BX_LAPIC_ISR7: case BX_LAPIC_ISR8:
  case BX_LAPIC_TMR1: case BX_LAPIC_TMR2: case BX_LAPIC_TMR3: case BX_LAPIC_TMR4:
  case BX_LAPIC_TMR5: case BX_LAPIC_TMR6: case BX_LAPIC_TMR7: case BX_LAPIC_TMR8:
  case BX_LAPIC_IRR1: case BX_LAPIC_IRR2: case BX_LAPIC_IRR3: case BX_LAPIC_IRR4:
  case BX_LAPIC_IRR5: case BX_LAPIC_IRR6: case BX_LAPIC_IRR7: case BX_LAPIC_IRR8:
    return write ? BX_AVIC_FAULT : BX_AVIC_ALLOW;

  case BX_LAPIC_TPR:
  case BX_LAPIC_EOI:     // level triggered EOI is handled as TRAP by SvmAvicEoi()
  case BX_LAPIC_ICR_LO:  // not accelerated IPI types are reported by SvmAvicSendIPI()
    return write ? BX_AVIC_ACCEL : BX_AVIC_ALLOW;

  case BX_LAPIC_ICR_HI:
    return BX_AVIC_ALLOW;

  case BX_LAPIC_ARBITRATION_PRIORITY:
  case BX_LAPIC_TIMER_CURRENT_COUNT:
    return BX_AVIC_FAULT;

  default:
    break;
  }

  // Extended APIC Feature, Extended APIC Control, SEOI, Interrupt Enable Registers, Extended
  // Interrupt LVT registers and reserved 580h-FFFh. Offsets 430h-470h are not listed in Table 15-22
  // but fault on real hardware too (Zen+ family 17h model 18h)
  if (offset >= BX_LAPIC_EXT_APIC_FEATURE)
    return BX_AVIC_FAULT;

  // any other register location not explicitly defined in Table 15-22
  return BX_AVIC_ALLOW;
}

bool BX_CPP_AttrRegparmN(1) BX_CPU_C::is_avic_backing_page(bx_phy_address paddr)
{
  if (BX_CPU_THIS_PTR in_svm_guest && SVM_AVIC_ENABLED)
    if (PPFOf(paddr) == BX_CPU_THIS_PTR vmcb->ctrls.avic_backing_page) return true;

  return false;
}

// Direct accesses to AVIC memory structures, the guest vAPIC register filter is not applied.

// read register at 'offset' of the own vAPIC backing page
Bit32u BX_CPU_C::SvmAvicRead32(unsigned offset)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcb->ctrls.avic_backing_page + offset;
  Bit32u val_32;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&val_32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr))), BX_READ, BX_SVM_AVIC_ACCESS, (Bit8u*)(&val_32));
  return val_32;
}

// write register at 'offset' of the own vAPIC backing page
void BX_CPU_C::SvmAvicWrite32(unsigned offset, Bit32u val_32)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcb->ctrls.avic_backing_page + offset;
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&val_32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr))), BX_WRITE, BX_SVM_AVIC_ACCESS, (Bit8u*)(&val_32));
}

Bit64u BX_CPU_C::SvmAvicReadPhysicalEntry(unsigned index)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcb->ctrls.avic_physical_table + index*8;
  Bit64u entry;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 8, (Bit8u*)(&entry));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 8, MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr))), BX_READ, BX_SVM_AVIC_ACCESS, (Bit8u*)(&entry));
  return entry;
}

Bit32u BX_CPU_C::SvmAvicReadLogicalEntry(unsigned index)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcb->ctrls.avic_logical_table + index*4;
  Bit32u entry;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&entry));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr))), BX_READ, BX_SVM_AVIC_ACCESS, (Bit8u*)(&entry));
  return entry;
}

// returns the highest vector set in 256-bit APIC register array (ISR, TMR or IRR) of the own backing page, -1 if none
int BX_CPU_C::SvmAvicHighestVector(unsigned arrbase)
{
  for (int n = 7; n >= 0; n--) {
    Bit32u val_32 = SvmAvicRead32(arrbase + 16*n);
    if (val_32) return (n * 32) + most_significant_bitd(val_32);
  }

  return -1;
}

// ===============
//  VMRUN / VMEXIT
// ===============

// called from SvmEnterLoadCheckGuestState() instead of V_IRQ injection when AVIC is enabled
void BX_CPU_C::SvmAvicVmrun(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  // V_TPR and the backing page TPR are synchronized only by guest writes of TPR or CR8, not by VMRUN
  BX_DEBUG(("VMRUN AVIC: V_TPR=%d backing page TPR=0x%02x", ctrls->v_tpr, SvmAvicRead32(BX_LAPIC_TPR) & 0xff));

  // On VMRUN the highest priority pending interrupt in IRR is delivered if masking and priority allow (APM 15.29.8.3).
  // A stale backing page PPR is recomputed from TPR and ISR (real hardware)
  BX_DEBUG(("VMRUN AVIC: backing page PPR=0x%02x", SvmAvicRead32(BX_LAPIC_PPR)));
  SvmAvicUpdatePPR();
}

// ===================================
//  Guest accesses to the vAPIC page
// ===================================

// guest read from the vAPIC backing page, returns if the read is allowed
void BX_CPU_C::SvmAvicRead(bx_phy_address paddr, unsigned len)
{
  unsigned offset = (unsigned) PAGE_OFFSET(paddr);

  // All vAPIC registers are 32-bit wide and 16-byte aligned, the results of accesses to bytes
  // [offset+4:offset+15] are undefined (APM 15.29.3.1). On real hardware a read of any size and
  // alignment is filtered by the 16-byte register slot of its first byte: allowed reads return
  // the backing page bytes as is, also when the read continues into a faulting slot (TPR into APR),
  // a read starting in a faulting slot faults (APR into PPR)
  unsigned access = avic_register_access(offset & ~0xf, false /* read */);
  BX_DEBUG(("AVIC: guest %d-byte read vAPIC offset 0x%03x: %s", len, offset, avic_access_name[access]));

  if (access == BX_AVIC_FAULT) {
    // real hardware reports the read offset rounded down to 8 bytes
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) fault on read of vAPIC offset 0x%03x", offset));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, offset & ~0x7, 0);
  }
}

// guest write to the vAPIC backing page, the write is done here if allowed
void BX_CPU_C::SvmAvicWrite(bx_phy_address paddr, unsigned len, void *data)
{
  unsigned offset = (unsigned) PAGE_OFFSET(paddr);

  // All vAPIC registers are 32-bit wide and 16-byte aligned, the results of accesses to bytes
  // [offset+4:offset+15] are undefined (APM 15.29.3.1). On real hardware a write of any size and
  // alignment is filtered by the 16-byte register slot it touches: allowed and trapping writes
  // store the bytes as written, faulting writes store nothing. AVIC_NOACCEL #VMEXIT reports
  // the write offset rounded down to 8 bytes, the accelerated action is taken for a write
  // to any byte of the register slot.
  unsigned slot = offset & ~0xf;
  unsigned access = avic_register_access(slot, true /* write */);
  unsigned report_offset = offset & ~0x7;

  // A write continuing into another slot: the slot of the last byte is checked like the first one,
  // the same way as a write tickle of a whole memory area. Real hardware: trap into trap (RRD into
  // LDR) writes all bytes and traps once, trap into fault (SVR into ISR) faults before any byte is
  // written and reports the faulting slot, an accelerated write is not stopped by the next slot
  // (TPR into APR writes all bytes as an accelerated write).
  unsigned last_slot = (offset + len - 1) & ~0xf;
  if (last_slot != slot && access != BX_AVIC_ACCEL && access != BX_AVIC_FAULT) {
    unsigned access_last = avic_register_access(last_slot, true /* write */);
    if (access == BX_AVIC_ALLOW || access_last == BX_AVIC_FAULT) {
      access = access_last;
      report_offset = last_slot;
    }
  }

  BX_DEBUG(("AVIC: guest %d-byte write vAPIC offset 0x%03x: %s", len, offset, avic_access_name[access]));

  if (access == BX_AVIC_FAULT) {
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) fault on write of vAPIC offset 0x%03x", report_offset));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, SVM_AVIC_NOACCEL_WRITE | report_offset, 0);
  }

  // ALLOW, TRAP and ACCEL writes complete in the backing page, the TRAP and ACCEL action is taken
  // after the instruction. Real hardware stores accelerated register writes as written: the EOI
  // value, all 32 bits of TPR and the ICRL Delivery Status bit (which is not cleared and does not
  // block the IPI).
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, paddr, len, data);
  BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, len, MEMTYPE(resolve_memtype(memtype_by_mtrr(paddr))), BX_WRITE, BX_SVM_AVIC_ACCESS, (Bit8u*) data);

  if (access == BX_AVIC_ALLOW)
    return;

  // An instruction writing several trapping registers (FNSAVE, FXSAVE, ENTER, far CALL ...) completes
  // all its writes and takes one trap after the instruction. Real hardware (FNSAVE) reports the
  // 8-byte unit holding the last byte written. A rep string instruction takes the trap after the
  // iteration (Bochs repeat loop stops on the event).
  if (is_pending(BX_EVENT_SVM_AVIC_TRAP)) {
    BX_DEBUG(("AVIC: next trapping vAPIC write in one instruction (offset 0x%03x, previous offset 0x%03x)", offset, BX_CPU_THIS_PTR avic_trap_offset));
    BX_CPU_THIS_PTR avic_trap_offset = (offset + len - 1) & ~0x7;
    return;
  }

  BX_CPU_THIS_PTR avic_trap_offset = report_offset;
  signal_event(BX_EVENT_SVM_AVIC_TRAP);
}

// Write tickle of an instruction which writes its memory area in several chunks (FNSAVE, FXSAVE,
// XSAVE ...), called from tickle_write_linear() after the translation of the tickled byte
// (address_xlation.paddress1): such instructions tickle the first and the last byte of the area,
// a faulting vAPIC register there makes the whole instruction fault before any byte is written.
// Real hardware (FNSAVE ending in an ISR register) reports the register slot of the tickled byte.
// The AVIC backing page never has a direct host pointer, so a tickle of it always reaches the
// translation.
void BX_CPU_C::SvmAvicWriteTickle(void)
{
  bx_phy_address paddr = BX_CPU_THIS_PTR address_xlation.paddress1;
  if (! is_avic_backing_page(paddr)) return;

  unsigned slot = (unsigned) PAGE_OFFSET(paddr) & ~0xf;
  if (avic_register_access(slot, true /* write */) == BX_AVIC_FAULT) {
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) fault on write tickle of vAPIC offset 0x%03x", slot));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, SVM_AVIC_NOACCEL_WRITE | slot, 0);
  }
}

// executed as trap-like from handleAsyncEvent after the instruction which wrote the vAPIC register
void BX_CPU_C::SvmAvicTrap(void)
{
  clear_event(BX_EVENT_SVM_AVIC_TRAP);

  // write offset rounded down to 8 bytes, it is reported in EXITINFO1
  unsigned offset = BX_CPU_THIS_PTR avic_trap_offset;
  BX_DEBUG(("AVIC: trap processing for vAPIC offset 0x%03x", offset));

  // an accelerated register action is taken before a pending debug trap of the same instruction,
  // the debug trap is delivered after it (real hardware)
  switch(offset & ~0xf) {
  case BX_LAPIC_TPR:
    SvmAvicTprWrite(SvmAvicRead32(BX_LAPIC_TPR));
    break;

  case BX_LAPIC_EOI:
    SvmAvicEoi();
    break;

  case BX_LAPIC_ICR_LO:
    SvmAvicSendIPI();
    break;

  default:
    // the trap-like #VMEXIT is taken before a pending debug trap of the same instruction,
    // the debug trap is not delivered (real hardware)
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) trap on write of vAPIC offset 0x%03x", offset));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, SVM_AVIC_NOACCEL_WRITE | offset, 0);
  }
}

// =========================
//  TPR, CR8, PPR, EOI
// =========================

// recompute PPR after TPR or highest in-service interrupt change and re-evaluate pending interrupts
void BX_CPU_C::SvmAvicUpdatePPR(void)
{
  Bit32u tpr = SvmAvicRead32(BX_LAPIC_TPR) & 0xff;
  int isrv = SvmAvicHighestVector(BX_LAPIC_ISR1);

  Bit32u ppr;
  if (isrv < 0 || (tpr >> 4) >= ((unsigned) isrv >> 4))
    ppr = tpr;
  else
    ppr = isrv & 0xf0;

  BX_DEBUG(("AVIC: PPR=0x%02x (TPR=0x%02x ISRV=0x%02x)", ppr, tpr, (isrv < 0) ? 0 : isrv));
  SvmAvicWrite32(BX_LAPIC_PPR, ppr);

  SvmAvicEvaluatePendingInterrupt();
}

// new TPR value in the backing page: upper 4 bits of TPR are copied to V_TPR (APM 15.29.3.1)
void BX_CPU_C::SvmAvicTprWrite(Bit32u tpr)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  ctrls->v_tpr = (tpr >> 4) & 0xf;
  BX_DEBUG(("AVIC: TPR write 0x%02x, V_TPR=%d", tpr & 0xff, ctrls->v_tpr));

  SvmAvicUpdatePPR();
}

// MOV to CR8 with AVIC enabled updates V_TPR and the backing page TPR[7:4], TPR[3:0] are set to zero,
// independent of V_INTR_MASKING (APM 15.29.3.1, 15.29.4.1)
void BX_CPU_C::SvmAvicWriteCR8(unsigned tpr)
{
  SvmAvicWrite32(BX_LAPIC_TPR, tpr);
  SvmAvicTprWrite(tpr);
}

void BX_CPU_C::SvmAvicEoi(void)
{
  int vector = SvmAvicHighestVector(BX_LAPIC_ISR1);
  if (vector < 0) {
    // same as for physical local APIC, EOI with empty ISR has no effect
    BX_DEBUG(("AVIC: EOI written without any bit in ISR"));
    return;
  }

  unsigned reg = (vector / 32) * 16;

  // EOI for level triggered interrupt causes #VMEXIT to allow the VMM to emulate level triggered
  // behavior, EXITINFO2 holds the highest in-service vector (APM 15.29.3.1, 15.29.9.2)
  Bit32u tmr = SvmAvicRead32(BX_LAPIC_TMR1 + reg);
  if (tmr & (1 << (vector & 0x1f))) {
    BX_DEBUG(("AVIC: EOI for level triggered vector 0x%02x - #VMEXIT(AVIC_NOACCEL) trap", vector));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, SVM_AVIC_NOACCEL_WRITE | BX_LAPIC_EOI, vector);
  }

  BX_DEBUG(("AVIC: EOI clears ISR vector 0x%02x", vector));
  Bit32u isr = SvmAvicRead32(BX_LAPIC_ISR1 + reg);
  isr &= ~(1 << (vector & 0x1f));
  SvmAvicWrite32(BX_LAPIC_ISR1 + reg, isr);

  SvmAvicUpdatePPR();
}

// ============================
//  Virtual interrupt delivery
// ============================

// AVIC interrupts are taken through the processor virtual interrupt mechanism: the highest vector
// pending in IRR is the virtual interrupt, its priority is compared with TPR of the backing page,
// with the highest in-service vector and (if use_v_tpr) with V_TPR. Real hardware blocks delivery
// by both V_TPR and backing page TPR, while V_IRQ on #VMEXIT ignores V_TPR. Returns the vector
// or -1 if none.
int BX_CPU_C::SvmAvicPendingVector(bool use_v_tpr)
{
  int irrv = SvmAvicHighestVector(BX_LAPIC_IRR1);
  int isrv = SvmAvicHighestVector(BX_LAPIC_ISR1);
  unsigned tpr = SvmAvicRead32(BX_LAPIC_TPR) & 0xff;

  unsigned priority = use_v_tpr ? (SVM_V_TPR & 0xf) : 0;
  if ((tpr >> 4) > priority)
    priority = tpr >> 4;
  if (isrv >= 0 && ((unsigned) isrv >> 4) > priority)
    priority = (unsigned) isrv >> 4;

  BX_DEBUG(("AVIC: IRRV=0x%02x ISRV=0x%02x V_TPR=%d TPR=0x%02x", (irrv < 0) ? 0 : irrv, (isrv < 0) ? 0 : isrv, SVM_V_TPR, tpr));

  if (irrv >= 0 && ((unsigned) irrv >> 4) > priority)
    return irrv;

  return -1;
}

void BX_CPU_C::SvmAvicEvaluatePendingInterrupt(void)
{
  int vector = SvmAvicPendingVector(true);

  if (vector >= 0) {
    BX_DEBUG(("AVIC: pending virtual interrupt vector 0x%02x", vector));
    signal_event(BX_EVENT_SVM_AVIC_INTR);
  }
  else {
    BX_DEBUG(("AVIC: no deliverable virtual interrupt"));
    clear_event(BX_EVENT_SVM_AVIC_INTR);
  }
}

void BX_CPU_C::SvmAvicDeliverInterrupt(void)
{
  int vector = SvmAvicPendingVector(true);
  if (vector < 0) {
    BX_DEBUG(("AVIC: virtual interrupt is not deliverable anymore"));
    clear_event(BX_EVENT_SVM_AVIC_INTR);
    return;
  }

  // the VINTR intercept applies to AVIC interrupts: #VMEXIT before the interrupt is taken, IRR stays set
  // and V_IRQ is written as 1 on #VMEXIT (real hardware)
  if (SVM_INTERCEPT(SVM_INTERCEPT0_VINTR)) {
    BX_DEBUG(("AVIC: VINTR intercept, vector 0x%02x stays pending", vector));
    Svm_Vmexit(SVM_VMEXIT_VINTR);
  }

  BX_DEBUG(("AVIC: deliver virtual interrupt vector 0x%02x", vector));

  unsigned reg = (vector / 32) * 16;
  Bit32u mask = 1 << (vector & 0x1f);

  Bit32u irr = SvmAvicRead32(BX_LAPIC_IRR1 + reg);
  SvmAvicWrite32(BX_LAPIC_IRR1 + reg, irr & ~mask);
  Bit32u isr = SvmAvicRead32(BX_LAPIC_ISR1 + reg);
  SvmAvicWrite32(BX_LAPIC_ISR1 + reg, isr | mask);

  SvmAvicUpdatePPR();

  BX_CPU_THIS_PTR EXT = 1; /* external event */

  BX_INSTR_HWINTERRUPT(BX_CPU_ID, vector,
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);

#if BX_SUPPORT_FRED
  if (BX_CPU_THIS_PTR cr4.get_FRED())
    set_fred_event_info_and_data(vector, BX_EXTERNAL_INTERRUPT, false, 0);
#endif

  interrupt(vector, BX_EXTERNAL_INTERRUPT, 0, 0);

  BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
}

// ======================
//  Inter-processor IPIs
// ======================

void BX_CPU_C::SvmAvicIncompleteIPI(unsigned id, unsigned index)
{

  Bit32u icr_lo = SvmAvicRead32(BX_LAPIC_ICR_LO);
  Bit32u icr_hi = SvmAvicRead32(BX_LAPIC_ICR_HI);

  BX_DEBUG(("AVIC: #VMEXIT(AVIC_INCOMPLETE_IPI) ID=%d index=%d ICR=0x%08x:%08x", id, index, icr_hi, icr_lo));
  Svm_Vmexit(SVM_VMEXIT_AVIC_INCOMPLETE_IPI, GET64_FROM_HI32_LO32(icr_hi, icr_lo), GET64_FROM_HI32_LO32(id, index));
}

// set IRR bit in the backing page of any virtual processor (also own)
void BX_CPU_C::SvmAvicSetTargetIRR(bx_phy_address backing_page, Bit8u vector)
{
  bx_phy_address pAddr = backing_page + BX_LAPIC_IRR1 + (vector / 32) * 16;
#if BX_DEBUGGER || BX_INSTRUMENTATION
  // memory type is used only by the debugger/instrumentation notify
  BxMemtype memtype = MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr)));
#endif

  // Bochs runs all processors in one thread, the read-modify-write is atomic
  Bit32u irr;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&irr));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, memtype, BX_READ, BX_SVM_AVIC_ACCESS, (Bit8u*)(&irr));
  irr |= 1 << (vector & 0x1f);
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&irr));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, memtype, BX_WRITE, BX_SVM_AVIC_ACCESS, (Bit8u*)(&irr));

  BX_DEBUG(("AVIC: set IRR vector 0x%02x in backing page 0x" FMT_PHY_ADDRX, vector, backing_page));
}

// find Physical APIC ID table entry of the sending virtual processor (for 'all excluding self' shorthand):
// real hardware uses the APIC ID register of the own backing page as the table index, the entry
// pointing to the own backing page is not searched
unsigned BX_CPU_C::SvmAvicSelfPhysicalIndex(void)
{
  unsigned apic_id = SvmAvicRead32(BX_LAPIC_ID) >> 24;
  BX_DEBUG(("AVIC: 'all excluding self' IPI, self is Physical APIC ID table entry %d", apic_id));
  return apic_id;
}

// remember the first IPI delivery failure, it is reported after all destinations were checked
static void avic_ipi_failure(int *failure_id, unsigned *failure_index, int id, unsigned index)
{
  if (*failure_id < 0) {
    *failure_id = id;
    *failure_index = index;
  }
}

// guest write to ICRL, AVIC handles Fixed edge triggered IPIs (APM 15.29.3.1, 15.29.6.1)
void BX_CPU_C::SvmAvicSendIPI(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;
  bx_phy_address backing_page = ctrls->avic_backing_page;

  Bit32u icr_lo = SvmAvicRead32(BX_LAPIC_ICR_LO);
  Bit32u icr_hi = SvmAvicRead32(BX_LAPIC_ICR_HI);

  Bit8u vector = icr_lo & 0xff;
  unsigned delivery_mode = (icr_lo >> 8) & 0x7;
  bool logical_dest = (icr_lo >> 11) & 0x1;
  bool level = (icr_lo >> 15) & 0x1;
  unsigned dest_shorthand = (icr_lo >> 18) & 0x3;
  Bit8u dest = icr_hi >> 24;

  BX_DEBUG(("AVIC: IPI ICR=0x%08x:%08x vector=0x%02x delivery mode=%d %s dest=0x%02x trigger=%s shorthand=%d",
      icr_hi, icr_lo, vector, delivery_mode, logical_dest ? "logical" : "physical", dest, level ? "level" : "edge", dest_shorthand));

  // trigger mode level or not Fixed message type, also reported for vector < 16 (real hardware)
  bool invalid_type = (delivery_mode != APIC_DM_FIXED) || level;
  if (invalid_type)
    SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_INT_TYPE, 0);

  // Vector < 16 is not checked, Invalid IPI Vector is never reported: such IPI sets the IRR bit
  // like any other vector (Zen+ hardware, Zen3 hardware does report a vector < 16)

  // Step 1: self IPI - update IRR in the own backing page, signal doorbell to self
  if (dest_shorthand == 1) {
    BX_DEBUG(("AVIC: self IPI vector 0x%02x", vector));
    SvmAvicSetTargetIRR(backing_page, vector);
    SvmAvicEvaluatePendingInterrupt();
    return;
  }

  // Pass 1: lookup all destinations (Steps 2-4), no IRR is modified yet
  unsigned target_entry_index[256];   // Physical APIC ID table index of the target
  unsigned target_report_index[256];  // table index reported in EXITINFO2 for the target
  unsigned num_targets = 0;

  int failure_id = -1;          // first failure, -1 if none
  unsigned failure_index = 0;

  bool broadcast = (dest_shorthand != 0) || (dest == 0xff);

  if (broadcast) {
    unsigned self = 0;
    bool exclude_self = (dest_shorthand == 3);
    if (exclude_self) self = SvmAvicSelfPhysicalIndex();

    for (unsigned n = 0; n <= ctrls->avic_physical_max_index; n++) {
      Bit64u entry = SvmAvicReadPhysicalEntry(n);
      // for broadcast IPIs invalid entries are ignored
      // reserved bits are ignored, a valid entry 255 is a target like any other (real hardware)
      if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID)) continue;
      if (exclude_self && n == self) continue;
      if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK)) {
        avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_BACKING_PAGE, n);
        continue;
      }
      target_entry_index[num_targets] = n;
      target_report_index[num_targets] = n;
      num_targets++;
    }
  }
  else if (! logical_dest) {
    // physical destination: one target, reported with its Physical APIC ID table index; a destination
    // above AVIC_PHYSICAL_MAX_INDEX is an invalid target like a not valid entry, reserved bits of the
    // entry are ignored (real hardware)
    Bit64u entry = (dest <= ctrls->avic_physical_max_index) ? SvmAvicReadPhysicalEntry(dest) : 0;
    if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID))
      avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_TARGET, dest);
    else if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK))
      avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_BACKING_PAGE, dest);
    else {
      target_entry_index[0] = target_report_index[0] = dest;
      num_targets = 1;
    }
  }
  else {
    // logical destination, the Logical APIC ID table format depends on the guest DFR model.
    // Only DFR bit 28 selects the format
    Bit32u dfr = SvmAvicRead32(BX_LAPIC_DESTINATION_FORMAT);
    unsigned logical_index[8];
    unsigned num_logical = 0;

    if (dfr & (1 << 28)) {
      // flat mode: each set bit of the logical destination selects an entry
      for (unsigned bit = 0; bit < 8; bit++)
        if (dest & (1 << bit)) logical_index[num_logical++] = bit;
    }
    else {
      // cluster mode: dest[7:4] cluster, dest[3:0] bit encoded APIC index, cluster 15 (dest != FFh)
      // is used like any other cluster (real hardware)
      unsigned cluster = dest >> 4;
      for (unsigned bit = 0; bit < 4; bit++)
        if (dest & (1 << bit)) logical_index[num_logical++] = cluster * 4 + bit;
    }

    for (unsigned n = 0; n < num_logical; n++) {
      unsigned li = logical_index[n];
      Bit32u logical_entry = SvmAvicReadLogicalEntry(li);
      BX_DEBUG(("AVIC: logical index %d entry 0x%08x", li, logical_entry));
      if (! (logical_entry & SVM_AVIC_LOGICAL_ENTRY_VALID)) {
        avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_TARGET, li);
        continue;
      }
      // reserved bits of logical and physical entries are ignored (real hardware)

      // the guest physical APIC ID of a logical entry is not checked against AVIC_PHYSICAL_MAX_INDEX
      // (real hardware delivered to entry 5 with max index 3)
      unsigned guest_id = logical_entry & SVM_AVIC_LOGICAL_ENTRY_GUEST_ID_MASK;
      Bit64u entry = SvmAvicReadPhysicalEntry(guest_id);
      if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID)) {
        // real hardware reports the logical table index
        avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_TARGET, li);
        continue;
      }
      if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK)) {
        // for directed IPIs the index of the logical or physical table depending on the destination mode
        avic_ipi_failure(&failure_id, &failure_index, SVM_AVIC_IPI_INVALID_BACKING_PAGE, li);
        continue;
      }
      target_entry_index[num_targets] = guest_id;
      target_report_index[num_targets] = li;
      num_targets++;
    }
  }

  // all destinations are checked before any IRR is set: with an invalid destination the valid
  // ones do not get the interrupt either, the first failure (lowest index) is reported (real hardware)
  if (failure_id >= 0)
    SvmAvicIncompleteIPI(failure_id, failure_index);

  // Pass 2: Step 5 - for every valid destination set IRR and send doorbell if IsRunning
  unsigned not_running = 0, not_running_index = 0;

  for (unsigned n = 0; n < num_targets; n++) {
    Bit64u entry = SvmAvicReadPhysicalEntry(target_entry_index[n]);
    bx_phy_address target_backing_page = (bx_phy_address)(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK);

    SvmAvicSetTargetIRR(target_backing_page, vector);

    if (entry & SVM_AVIC_PHYSICAL_ENTRY_IS_RUNNING) {
      // xAVIC uses 8-bit host APIC IDs, bits 11:8 of the field are ignored (real hardware)
      unsigned host_apic_id = (unsigned)(entry & SVM_AVIC_PHYSICAL_ENTRY_HOST_ID_MASK) & 0xff;
      SvmAvicRingDoorbell(host_apic_id);
    }
    else {
      // also the own entry with IsRunning=0 is reported as not running (real hardware)
      BX_DEBUG(("AVIC: IPI target entry %d is not running", target_entry_index[n]));
      if (! not_running) not_running_index = target_report_index[n];
      not_running++;
    }
  }

  // Step 6: any destination not currently scheduled on a physical core causes #VMEXIT, real
  // hardware reports the first not running destination, logical IPIs with the logical table index
  if (not_running) {
    BX_DEBUG(("AVIC: IPI with %d not running targets", not_running));
    SvmAvicIncompleteIPI(SVM_AVIC_IPI_TARGET_NOT_RUNNING, not_running_index);
  }
}

// ==========
//  Doorbell
// ==========

void BX_CPU_C::SvmAvicRingDoorbell(unsigned host_apic_id)
{
#if BX_SUPPORT_APIC
  for (unsigned n = 0; n < BX_SMP_PROCESSORS; n++) {
    if (BX_CPU(n)->lapic->get_id() == host_apic_id) {
      BX_DEBUG(("AVIC: doorbell to CPU%d (APIC ID 0x%02x)", n, host_apic_id));
      BX_CPU(n)->SvmAvicDoorbell();
      return;
    }
  }
#endif

  BX_DEBUG(("AVIC: doorbell to APIC ID 0x%02x - no such processor, ignored", host_apic_id));
}

// doorbell signal received by this processor
void BX_CPU_C::SvmAvicDoorbell(void)
{
  if (BX_CPU_THIS_PTR in_svm_guest && SVM_AVIC_ENABLED) {
    // evaluate the vAPIC state of the currently running virtual processor (APM 15.29.8.2)
    BX_DEBUG(("AVIC: doorbell received in guest mode"));
    SvmAvicEvaluatePendingInterrupt();
  }
  else {
    // the backing page is evaluated on the next VMRUN anyway
    BX_DEBUG(("AVIC: doorbell received outside of AVIC guest, ignored"));
  }
}

#endif // BX_SUPPORT_SVM
