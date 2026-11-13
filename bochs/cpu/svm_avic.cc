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
//
// Behavior not defined by the APM is not guessed: such cases BX_PANIC and name the
// open question number (Qnn) from avic-implementation-plan.md. They are to be decided
// by tests on real AMD hardware.

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
  case BX_LAPIC_EXT_APIC_FEATURE:
  case BX_LAPIC_EXT_APIC_CONTROL:
  case BX_LAPIC_SPECIFIC_EOI:
    return BX_AVIC_FAULT;

  default:
    break;
  }

  // Interrupt Enable Registers, Extended Interrupt LVT registers and reserved 580h-FFFh
  if (offset >= BX_LAPIC_IER1)
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

  // On VMRUN the highest priority pending interrupt in IRR is delivered if masking and priority allow (APM 15.29.8.3)
  Bit32u ppr = SvmAvicRead32(BX_LAPIC_PPR);
  SvmAvicUpdatePPR();
  if (SvmAvicRead32(BX_LAPIC_PPR) != ppr)
    BX_PANIC(("VMRUN AVIC: stale backing page PPR=0x%08x, PPR recompute on VMRUN is not defined by APM (avic-implementation-plan Q15)", ppr));
}

// ===================================
//  Guest accesses to the vAPIC page
// ===================================

// guest read from the vAPIC backing page, returns if the read is allowed
void BX_CPU_C::SvmAvicRead(bx_phy_address paddr, unsigned len)
{
  unsigned offset = (unsigned) PAGE_OFFSET(paddr);

  // All vAPIC registers are 32-bit wide and 16-byte aligned, the results of accesses to bytes
  // [offset+4:offset+15] are undefined (APM 15.29.3.1)
  if (len != 4 || (offset & 0xf) != 0)
    BX_PANIC(("AVIC: %d-byte read at vAPIC offset 0x%03x, odd-sized access behavior is not defined by APM (avic-implementation-plan Q11)", len, offset));

  unsigned access = avic_register_access(offset, false /* read */);
  BX_DEBUG(("AVIC: guest read vAPIC offset 0x%03x: %s", offset, avic_access_name[access]));

  if (access == BX_AVIC_FAULT) {
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) fault on read of vAPIC offset 0x%03x", offset));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, offset, 0);
  }
}

// guest write to the vAPIC backing page, the write is done here if allowed
void BX_CPU_C::SvmAvicWrite(bx_phy_address paddr, unsigned len, void *data)
{
  unsigned offset = (unsigned) PAGE_OFFSET(paddr);

  // All vAPIC registers are 32-bit wide and 16-byte aligned, the results of accesses to bytes
  // [offset+4:offset+15] are undefined (APM 15.29.3.1)
  if (len != 4 || (offset & 0xf) != 0)
    BX_PANIC(("AVIC: %d-byte write at vAPIC offset 0x%03x, odd-sized access behavior is not defined by APM (avic-implementation-plan Q11)", len, offset));

  Bit32u val_32 = *((Bit32u *) data);

  unsigned access = avic_register_access(offset, true /* write */);
  BX_DEBUG(("AVIC: guest write vAPIC offset 0x%03x value 0x%08x: %s", offset, val_32, avic_access_name[access]));

  if (access == BX_AVIC_ALLOW) {
    SvmAvicWrite32(offset, val_32);
    return;
  }

  if (is_pending(BX_EVENT_SVM_AVIC_TRAP))
    BX_PANIC(("AVIC: second not accelerated vAPIC write in one instruction (offset 0x%03x, first offset 0x%03x) is not defined by APM (avic-implementation-plan Q11)",
        offset, BX_CPU_THIS_PTR avic_trap_offset));

  if (access == BX_AVIC_FAULT) {
    BX_DEBUG(("AVIC: #VMEXIT(AVIC_NOACCEL) fault on write of vAPIC offset 0x%03x", offset));
    Svm_Vmexit(SVM_VMEXIT_AVIC_NOACCEL, SVM_AVIC_NOACCEL_WRITE | offset, 0);
  }

  if (access == BX_AVIC_ACCEL) {
    if (offset == BX_LAPIC_TPR) {
      if (val_32 >> 8)
        BX_PANIC(("AVIC: TPR write 0x%08x with bits 31:8 set is not defined by APM (avic-implementation-plan Q22)", val_32));
    }
    else if (offset == BX_LAPIC_EOI) {
      if (val_32 != SvmAvicRead32(offset))
        BX_PANIC(("AVIC: EOI write 0x%08x, storing of EOI value in the backing page is not defined by APM (avic-implementation-plan Q20)", val_32));
    }
    else if (offset == BX_LAPIC_ICR_LO) {
      if (val_32 & (1 << 12))
        BX_PANIC(("AVIC: ICRL write 0x%08x with Delivery Status bit set is not defined by APM (avic-implementation-plan Q21)", val_32));
    }
  }

  // TRAP and ACCEL writes complete in the backing page, the action is taken after the instruction
  SvmAvicWrite32(offset, val_32);

  BX_CPU_THIS_PTR avic_trap_offset = offset;
  BX_CPU_THIS_PTR avic_trap_len = len;
  signal_event(BX_EVENT_SVM_AVIC_TRAP);
}

// executed as trap-like from handleAsyncEvent after the instruction which wrote the vAPIC register
void BX_CPU_C::SvmAvicTrap(void)
{
  clear_event(BX_EVENT_SVM_AVIC_TRAP);

  unsigned offset = BX_CPU_THIS_PTR avic_trap_offset;
  BX_DEBUG(("AVIC: trap processing for vAPIC offset 0x%03x", offset));

  switch(offset) {
  case BX_LAPIC_TPR:
    SvmAvicTprWrite();
    break;

  case BX_LAPIC_EOI:
    SvmAvicEoi();
    break;

  case BX_LAPIC_ICR_LO:
    SvmAvicSendIPI();
    break;

  default:
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

// guest write to TPR: upper 4 bits of TPR are copied to V_TPR (APM 15.29.3.1)
void BX_CPU_C::SvmAvicTprWrite(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  Bit32u tpr = SvmAvicRead32(BX_LAPIC_TPR);
  ctrls->v_tpr = (tpr >> 4) & 0xf;
  BX_DEBUG(("AVIC: TPR write 0x%02x, V_TPR=%d", tpr & 0xff, ctrls->v_tpr));

  SvmAvicUpdatePPR();
}

// MOV to CR8 with AVIC enabled updates V_TPR and the backing page TPR[7:4], TPR[3:0] are set to zero,
// independent of V_INTR_MASKING (APM 15.29.3.1, 15.29.4.1)
void BX_CPU_C::SvmAvicWriteCR8(unsigned tpr)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  ctrls->v_tpr = (tpr >> 4) & 0xf;
  BX_DEBUG(("AVIC: CR8 write %d, backing page TPR=0x%02x", ctrls->v_tpr, tpr));
  SvmAvicWrite32(BX_LAPIC_TPR, tpr);

  SvmAvicUpdatePPR();
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
// pending in IRR is the virtual interrupt, its priority is compared with V_TPR (not with TPR of
// the backing page) and with the highest in-service vector. Returns the vector or -1 if none.
int BX_CPU_C::SvmAvicPendingVector(void)
{
  int irrv = SvmAvicHighestVector(BX_LAPIC_IRR1);
  int isrv = SvmAvicHighestVector(BX_LAPIC_ISR1);

  unsigned priority = SVM_V_TPR & 0xf;
  if (isrv >= 0 && ((unsigned) isrv >> 4) > priority)
    priority = (unsigned) isrv >> 4;

  BX_DEBUG(("AVIC: IRRV=0x%02x ISRV=0x%02x V_TPR=%d", (irrv < 0) ? 0 : irrv, (isrv < 0) ? 0 : isrv, SVM_V_TPR));

  if (irrv >= 0 && ((unsigned) irrv >> 4) > priority)
    return irrv;

  return -1;
}

void BX_CPU_C::SvmAvicEvaluatePendingInterrupt(void)
{
  int vector = SvmAvicPendingVector();

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
  int vector = SvmAvicPendingVector();
  if (vector < 0) {
    BX_DEBUG(("AVIC: virtual interrupt is not deliverable anymore"));
    clear_event(BX_EVENT_SVM_AVIC_INTR);
    return;
  }

  if (SVM_INTERCEPT(SVM_INTERCEPT0_VINTR))
    BX_PANIC(("AVIC: virtual interrupt delivery with VINTR intercept enabled is not defined by APM (avic-implementation-plan Q6)"));

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
  BxMemtype memtype = MEMTYPE(resolve_memtype(memtype_by_mtrr(pAddr)));

  // Bochs runs all processors in one thread, the read-modify-write is atomic
  Bit32u irr;
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&irr));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, memtype, BX_READ, BX_SVM_AVIC_ACCESS, (Bit8u*)(&irr));
  irr |= 1 << (vector & 0x1f);
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&irr));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, memtype, BX_WRITE, BX_SVM_AVIC_ACCESS, (Bit8u*)(&irr));

  BX_DEBUG(("AVIC: set IRR vector 0x%02x in backing page 0x" FMT_PHY_ADDRX, vector, backing_page));
}

// find Physical APIC ID table entry of the sending virtual processor (for 'all excluding self' shorthand)
unsigned BX_CPU_C::SvmAvicSelfPhysicalIndex(void)
{
  SVM_CONTROLS *ctrls = &BX_CPU_THIS_PTR vmcb->ctrls;

  unsigned apic_id = SvmAvicRead32(BX_LAPIC_ID) >> 24;

  int self = -1;
  for (unsigned n = 0; n <= ctrls->avic_physical_max_index; n++) {
    Bit64u entry = SvmAvicReadPhysicalEntry(n);
    if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID)) continue;
    if ((entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK) == ctrls->avic_backing_page) {
      if (self >= 0)
        BX_PANIC(("AVIC: own backing page is referenced by Physical APIC ID table entries %d and %d (avic-implementation-plan Q25)", self, n));
      self = n;
    }
  }

  // the APM does not define how the sending virtual processor is identified, it is accepted only
  // when the backing page pointer and the vAPIC ID register agree
  if (self < 0 || (unsigned) self != apic_id)
    BX_PANIC(("AVIC: 'all excluding self' IPI, self identification is not defined by APM: backing page entry %d, vAPIC ID %d (avic-implementation-plan Q25)", self, apic_id));

  return (unsigned) self;
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

  bool invalid_type = (delivery_mode != APIC_DM_FIXED) || level;
  if (invalid_type && vector < 16)
    BX_PANIC(("AVIC: IPI with both invalid type and vector < 16, failure ID priority is not defined by APM (avic-implementation-plan Q28)"));

  // trigger mode level or not Fixed message type
  if (invalid_type)
    SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_INT_TYPE, 0);

  if (vector < 16)
    SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_VECTOR, 0);

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

  unsigned failures = 0, failure_id = 0, failure_index = 0;

  bool broadcast = (dest_shorthand != 0) || (dest == 0xff);

  if (broadcast) {
    unsigned self = 0;
    bool exclude_self = (dest_shorthand == 3);
    if (exclude_self) self = SvmAvicSelfPhysicalIndex();

    for (unsigned n = 0; n <= ctrls->avic_physical_max_index; n++) {
      Bit64u entry = SvmAvicReadPhysicalEntry(n);
      // for broadcast IPIs invalid entries are ignored
      if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID)) continue;
      if (entry & SVM_AVIC_PHYSICAL_ENTRY_RESERVED_MASK)
        BX_PANIC(("AVIC: Physical APIC ID table entry %d has reserved bits set: 0x" FMT_ADDRX64 " (avic-implementation-plan Q27)", n, entry));
      if (n == 0xff)
        BX_PANIC(("AVIC: valid Physical APIC ID table entry 255 (reserved broadcast ID) (avic-implementation-plan Q27)"));
      if (exclude_self && n == self) continue;
      if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK)) {
        if (! failures) { failure_id = SVM_AVIC_IPI_INVALID_BACKING_PAGE; failure_index = n; }
        failures++;
        continue;
      }
      target_entry_index[num_targets] = n;
      target_report_index[num_targets] = n;
      num_targets++;
    }
  }
  else if (! logical_dest) {
    // physical destination
    if (dest > ctrls->avic_physical_max_index) {
      SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_TARGET, dest);
    }
    Bit64u entry = SvmAvicReadPhysicalEntry(dest);
    if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID)) {
      SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_TARGET, dest);
    }
    if (entry & SVM_AVIC_PHYSICAL_ENTRY_RESERVED_MASK)
      BX_PANIC(("AVIC: Physical APIC ID table entry %d has reserved bits set: 0x" FMT_ADDRX64 " (avic-implementation-plan Q27)", dest, entry));
    if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK)) {
      SvmAvicIncompleteIPI(SVM_AVIC_IPI_INVALID_BACKING_PAGE, dest);
    }
    target_entry_index[0] = dest;
    target_report_index[0] = dest;
    num_targets = 1;
  }
  else {
    // logical destination, the Logical APIC ID table format depends on the guest DFR model
    unsigned dfr_model = SvmAvicRead32(BX_LAPIC_DESTINATION_FORMAT) >> 28;
    unsigned logical_index[8];
    unsigned num_logical = 0;

    if (dfr_model == 0xf) {
      // flat mode: each set bit of the logical destination selects an entry
      for (unsigned bit = 0; bit < 8; bit++)
        if (dest & (1 << bit)) logical_index[num_logical++] = bit;
    }
    else if (dfr_model == 0x0) {
      // cluster mode: dest[7:4] cluster, dest[3:0] bit encoded APIC index
      unsigned cluster = dest >> 4;
      if (cluster == 0xf)
        BX_PANIC(("AVIC: logical cluster destination 0x%02x with reserved cluster 15 (avic-implementation-plan Q26)", dest));
      for (unsigned bit = 0; bit < 4; bit++)
        if (dest & (1 << bit)) logical_index[num_logical++] = cluster * 4 + bit;
    }
    else {
      BX_PANIC(("AVIC: logical IPI with DFR model 0x%x is not defined by APM (avic-implementation-plan Q26)", dfr_model));
    }

    for (unsigned n = 0; n < num_logical; n++) {
      unsigned li = logical_index[n];
      Bit32u logical_entry = SvmAvicReadLogicalEntry(li);
      BX_DEBUG(("AVIC: logical index %d entry 0x%08x", li, logical_entry));
      if (! (logical_entry & SVM_AVIC_LOGICAL_ENTRY_VALID)) {
        if (! failures) { failure_id = SVM_AVIC_IPI_INVALID_TARGET; failure_index = li; }
        failures++;
        continue;
      }
      if (logical_entry & SVM_AVIC_LOGICAL_ENTRY_RESERVED_MASK)
        BX_PANIC(("AVIC: Logical APIC ID table entry %d has reserved bits set: 0x%08x (avic-implementation-plan Q27)", li, logical_entry));

      unsigned guest_id = logical_entry & SVM_AVIC_LOGICAL_ENTRY_GUEST_ID_MASK;
      if (guest_id > ctrls->avic_physical_max_index)
        BX_PANIC(("AVIC: logical index %d maps to guest physical APIC ID %d above AVIC_PHYSICAL_MAX_INDEX, failure index is not defined by APM (avic-implementation-plan Q24)", li, guest_id));
      Bit64u entry = SvmAvicReadPhysicalEntry(guest_id);
      if (! (entry & SVM_AVIC_PHYSICAL_ENTRY_VALID))
        BX_PANIC(("AVIC: logical index %d maps to invalid Physical APIC ID table entry %d, failure index is not defined by APM (avic-implementation-plan Q24)", li, guest_id));
      if (entry & SVM_AVIC_PHYSICAL_ENTRY_RESERVED_MASK)
        BX_PANIC(("AVIC: Physical APIC ID table entry %d has reserved bits set: 0x" FMT_ADDRX64 " (avic-implementation-plan Q27)", guest_id, entry));
      if (! IsValidPhyAddr(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK)) {
        // for directed IPIs the index of the logical or physical table depending on the destination mode
        if (! failures) { failure_id = SVM_AVIC_IPI_INVALID_BACKING_PAGE; failure_index = li; }
        failures++;
        continue;
      }
      target_entry_index[num_targets] = guest_id;
      target_report_index[num_targets] = li;
      num_targets++;
    }
  }

  if (failures) {
    if (failures > 1)
      BX_PANIC(("AVIC: IPI with %d invalid targets, reported index is not defined by APM (avic-implementation-plan Q24)", failures));
    if (num_targets > 0)
      BX_PANIC(("AVIC: IPI with both valid and invalid targets, delivery to valid targets is not defined by APM (avic-implementation-plan Q23)"));
    SvmAvicIncompleteIPI(failure_id, failure_index);
  }

  // Pass 2: Step 5 - for every valid destination set IRR and send doorbell if IsRunning
  unsigned not_running = 0, not_running_index = 0;

  for (unsigned n = 0; n < num_targets; n++) {
    Bit64u entry = SvmAvicReadPhysicalEntry(target_entry_index[n]);
    bx_phy_address target_backing_page = (bx_phy_address)(entry & SVM_AVIC_PHYSICAL_ENTRY_BACKING_MASK);

    SvmAvicSetTargetIRR(target_backing_page, vector);

    if (entry & SVM_AVIC_PHYSICAL_ENTRY_IS_RUNNING) {
      unsigned host_apic_id = (unsigned)(entry & SVM_AVIC_PHYSICAL_ENTRY_HOST_ID_MASK);
      if (host_apic_id > 0xff)
        BX_PANIC(("AVIC: Host Physical APIC ID 0x%03x above 8-bit xAPIC ID (avic-implementation-plan Q29)", host_apic_id));
      SvmAvicRingDoorbell(host_apic_id);
    }
    else {
      if (target_backing_page == backing_page)
        BX_PANIC(("AVIC: IPI to own Physical APIC ID table entry with IsRunning=0 (avic-implementation-plan Q33)"));
      BX_DEBUG(("AVIC: IPI target entry %d is not running", target_entry_index[n]));
      if (! not_running) not_running_index = target_report_index[n];
      not_running++;
    }
  }

  // Step 6: any destination not currently scheduled on a physical core causes #VMEXIT
  if (not_running) {
    if (not_running > 1)
      BX_PANIC(("AVIC: IPI with %d not running targets, reported index is not defined by APM (avic-implementation-plan Q24)", not_running));
    if (logical_dest && ! broadcast)
      BX_PANIC(("AVIC: logical IPI to not running target, logical or physical index report is not defined by APM (avic-implementation-plan Q24)"));
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
