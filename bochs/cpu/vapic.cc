/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012-2023 Stanislav Shwartsman
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

#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64

#include "memory/memory-bochs.h"

#if BX_SUPPORT_APIC
#include "apic.h"
#endif

bool BX_CPP_AttrRegparmN(1) BX_CPU_C::is_virtual_apic_page(bx_phy_address paddr)
{
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (vm->vmexec_ctrls2.VIRTUALIZE_APIC_ACCESSES())
      if (PPFOf(paddr) == vm->apic_access_page) return true;
  }

  return false;
}

bool BX_CPP_AttrRegparmN(2) BX_CPU_C::virtual_apic_access_vmexit(unsigned offset, unsigned len)
{
  if((offset & ~0x3) != ((offset+len-1) & ~0x3)) {
    BX_ERROR(("Virtual APIC access at offset 0x%08x spans 32-bit boundary !", offset));
    return true;
  }

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (is_pending(BX_EVENT_VMX_VTPR_UPDATE | BX_EVENT_VMX_VEOI_UPDATE | BX_EVENT_VMX_VIRTUAL_APIC_WRITE)) {
    if (vm->apic_access != offset) {
      BX_ERROR(("Second APIC virtualization at offset 0x%08x (first access at offset 0x%08x)", offset, vm->apic_access));
      return true;
    }
  }

  // access is not instruction fetch because cpu::prefetch will crash them
  if (! vm->vmexec_ctrls1.TPR_SHADOW() || len > 4 || offset >= 0x400)
    return true;

  BX_CPU_THIS_PTR vmcs.apic_access = offset;
  return false;
}

Bit32u BX_CPU_C::VMX_Read_Virtual_APIC(unsigned offset)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + offset;
  Bit32u field32;
  // must avoid recursive call to the function when VMX APIC access page = VMX Virtual Apic Page
  BX_MEM(0)->readPhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&field32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(pAddr)), BX_READ, BX_VMX_VAPIC_ACCESS, (Bit8u*)(&field32));
  return field32;
}

void BX_CPU_C::VMX_Write_Virtual_APIC(unsigned offset, int len, Bit8u* val)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + offset;
  // must avoid recursive call to the function when VMX APIC access page = VMX Virtual Apic Page
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pAddr, len, val);
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, len, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_VMX_VAPIC_ACCESS, val);
}

bx_phy_address BX_CPU_C::VMX_Virtual_Apic_Read(bx_phy_address paddr, unsigned len, void *data)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  BX_ASSERT(vm->vmexec_ctrls2.VIRTUALIZE_APIC_ACCESSES());
  BX_INFO(("Virtual Apic RD 0x" FMT_ADDRX " len = %d", paddr, len));

  Bit32u offset = PAGE_OFFSET(paddr);

  bool vmexit = virtual_apic_access_vmexit(offset, len);

  // access is not instruction fetch because cpu::prefetch will crash them
  if (! vmexit) {

    if (!vm->vmexec_ctrls2.VIRTUALIZE_APIC_REGISTERS()) {
      // if 'Virtualize Apic Registers' control is disabled allow only aligned access to VTPR
      if (offset != BX_LAPIC_TPR) vmexit = true;
    }

#if BX_SUPPORT_VMX >= 2
    switch(offset & 0x3fc) {
    case BX_LAPIC_ID:
    case BX_LAPIC_VERSION:
    case BX_LAPIC_TPR:
    case BX_LAPIC_EOI:
    case BX_LAPIC_LDR:
    case BX_LAPIC_DESTINATION_FORMAT:
    case BX_LAPIC_SPURIOUS_VECTOR:
    case BX_LAPIC_ISR1:
    case BX_LAPIC_ISR2:
    case BX_LAPIC_ISR3:
    case BX_LAPIC_ISR4:
    case BX_LAPIC_ISR5:
    case BX_LAPIC_ISR6:
    case BX_LAPIC_ISR7:
    case BX_LAPIC_ISR8:
    case BX_LAPIC_TMR1:
    case BX_LAPIC_TMR2:
    case BX_LAPIC_TMR3:
    case BX_LAPIC_TMR4:
    case BX_LAPIC_TMR5:
    case BX_LAPIC_TMR6:
    case BX_LAPIC_TMR7:
    case BX_LAPIC_TMR8:
    case BX_LAPIC_IRR1:
    case BX_LAPIC_IRR2:
    case BX_LAPIC_IRR3:
    case BX_LAPIC_IRR4:
    case BX_LAPIC_IRR5:
    case BX_LAPIC_IRR6:
    case BX_LAPIC_IRR7:
    case BX_LAPIC_IRR8:
    case BX_LAPIC_ESR:
    case BX_LAPIC_ICR_LO:
    case BX_LAPIC_ICR_HI:
    case BX_LAPIC_LVT_TIMER:
    case BX_LAPIC_LVT_THERMAL:
    case BX_LAPIC_LVT_PERFMON:
    case BX_LAPIC_LVT_LINT0:
    case BX_LAPIC_LVT_LINT1:
    case BX_LAPIC_LVT_ERROR:
    case BX_LAPIC_TIMER_INITIAL_COUNT:
    case BX_LAPIC_TIMER_DIVIDE_CFG:
      break;

    default:
      vmexit = true;
      break;
    }
#endif
  }

  if (vmexit) {
    Bit32u qualification = offset |
      ((BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_READ_INSTRUCTION_EXECUTION);
    VMexit(VMX_VMEXIT_APIC_ACCESS, qualification);
  }

  // remap access to virtual apic page
  paddr = vm->virtual_apic_page_addr + offset;
  BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, len, MEMTYPE(resolve_memtype(paddr)), BX_READ, BX_VMX_VAPIC_ACCESS, (Bit8u*) data);
  return paddr;
}

void BX_CPU_C::VMX_Virtual_Apic_Write(bx_phy_address paddr, unsigned len, void *data)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  BX_ASSERT(vm->vmexec_ctrls2.VIRTUALIZE_APIC_ACCESSES());
  BX_INFO(("Virtual Apic WR 0x" FMT_ADDRX " len = %d", paddr, len));

  Bit32u offset = PAGE_OFFSET(paddr);

  bool vmexit = virtual_apic_access_vmexit(offset, len);

  if (! vmexit) {

    if (offset == BX_LAPIC_TPR) {
      Bit8u vtpr = *((Bit8u *) data);
      VMX_Write_Virtual_APIC(BX_LAPIC_TPR, vtpr);
      signal_event(BX_EVENT_VMX_VTPR_UPDATE);
      return;
    }

#if BX_SUPPORT_VMX >= 2
    if (vm->vmexec_ctrls2.VIRTUAL_INT_DELIVERY()) {
      if (offset == BX_LAPIC_EOI) {
         signal_event(BX_EVENT_VMX_VEOI_UPDATE);
      }
    }

    bool virtualize_access = false;
    switch(offset & 0x3fc) {
    case BX_LAPIC_ID:
    case BX_LAPIC_TPR:
    case BX_LAPIC_LDR:
    case BX_LAPIC_DESTINATION_FORMAT:
    case BX_LAPIC_SPURIOUS_VECTOR:
    case BX_LAPIC_ESR:
    case BX_LAPIC_ICR_HI:
    case BX_LAPIC_LVT_TIMER:
    case BX_LAPIC_LVT_THERMAL:
    case BX_LAPIC_LVT_PERFMON:
    case BX_LAPIC_LVT_LINT0:
    case BX_LAPIC_LVT_LINT1:
    case BX_LAPIC_LVT_ERROR:
    case BX_LAPIC_TIMER_INITIAL_COUNT:
    case BX_LAPIC_TIMER_DIVIDE_CFG:
      if (vm->vmexec_ctrls2.VIRTUALIZE_APIC_REGISTERS()) {
        virtualize_access = true;
      }
      break;

    case BX_LAPIC_EOI:
    case BX_LAPIC_ICR_LO:
      if (vm->vmexec_ctrls2.VIRTUALIZE_APIC_REGISTERS() || vm->vmexec_ctrls2.VIRTUAL_INT_DELIVERY()) {
        virtualize_access = true;
      }
      break;

    default:
      break;
    }

    if (virtualize_access) {
      VMX_Write_Virtual_APIC(offset, len, (Bit8u*) data);
      signal_event(BX_EVENT_VMX_VIRTUAL_APIC_WRITE);
      return;
    }
#endif
  }

  Bit32u qualification = offset |
      ((BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_WRITE_INSTRUCTION_EXECUTION);
  VMexit(VMX_VMEXIT_APIC_ACCESS, qualification);
}

#if BX_SUPPORT_VMX >= 2

BX_CPP_INLINE void BX_CPU_C::vapic_set_vector(unsigned arrbase, Bit8u vector)
{
  unsigned reg = vector / 32;
  Bit32u regval = VMX_Read_Virtual_APIC(arrbase + 16*reg);
  regval |= (1 << (vector & 0x1f));
  VMX_Write_Virtual_APIC(arrbase + 16*reg, regval);
}

#include "scalar_arith.h"   // for lzcntd

BX_CPP_INLINE Bit8u BX_CPU_C::vapic_clear_and_find_highest_priority_int(unsigned arrbase, Bit8u vector)
{
  Bit32u arr[8];
  int n;

  for (n=0;n<8;n++)
    arr[n] = VMX_Read_Virtual_APIC(arrbase + 16*n);

  bx_local_apic_c::clear_vector(arr, vector);
  unsigned reg = vector / 32;
  VMX_Write_Virtual_APIC(arrbase + 16*reg, arr[reg]);

  for (n = 7; n >= 0; n--) {
    if (! arr[n]) continue;
    return (n * 32) + most_significant_bitd(arr[n]);
  }

  return 0;
}

void BX_CPU_C::VMX_Evaluate_Pending_Virtual_Interrupts(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (! vm->vmexec_ctrls1.INTERRUPT_WINDOW_VMEXIT() && ((vm->rvi >> 4) > (vm->vppr >> 4)))
  {
    BX_INFO(("Pending Virtual Interrupt Vector 0x%x", vm->rvi));
    signal_event(BX_EVENT_PENDING_VMX_VIRTUAL_INTR);
  }
  else {
    BX_INFO(("Clear Virtual Interrupt Vector 0x%x", vm->rvi));
    clear_event(BX_EVENT_PENDING_VMX_VIRTUAL_INTR);
  }
}

#endif

// may be executed as trap-like from handleAsyncEvent and also directy from CR8 write or WRMSR
void BX_CPU_C::VMX_TPR_Virtualization(void)
{
  BX_DEBUG(("Trap Event: VTPR Write Trap"));

  clear_event(BX_EVENT_VMX_VTPR_UPDATE);

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.VIRTUAL_INT_DELIVERY()) {
    VMX_PPR_Virtualization();
    VMX_Evaluate_Pending_Virtual_Interrupts();
  }
  else
#endif
  {
    Bit8u tpr_shadow = (VMX_Read_Virtual_APIC(BX_LAPIC_TPR) & 0xff) >> 4;
    if (tpr_shadow < BX_CPU_THIS_PTR vmcs.vm_tpr_threshold) {
      VMexit(VMX_VMEXIT_TPR_THRESHOLD, 0); // trap-like VMEXIT
    }
  }
}

#if BX_SUPPORT_VMX >= 2

void BX_CPU_C::VMX_PPR_Virtualization(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  Bit8u vtpr = (Bit8u) VMX_Read_Virtual_APIC(BX_LAPIC_TPR);
  Bit8u tpr_shadow = vtpr >> 4;

  if (tpr_shadow >= (vm->svi >> 4))
    vm->vppr = vtpr;
  else
    vm->vppr = vm->svi & 0xf0;

  VMX_Write_Virtual_APIC(BX_LAPIC_PPR, vm->vppr);
}

// may be executed as trap-like from handleAsyncEvent and also directy from WRMSR
void BX_CPU_C::VMX_EOI_Virtualization(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  BX_DEBUG(("Trap Event: VEOI Write Trap"));

  clear_event(BX_EVENT_VMX_VEOI_UPDATE);

  if (vm->vmexec_ctrls2.VIRTUAL_INT_DELIVERY())
  {
    VMX_Write_Virtual_APIC(BX_LAPIC_EOI, 0);

    unsigned vector = vm->svi;
    vm->svi = vapic_clear_and_find_highest_priority_int(BX_LAPIC_ISR1, vector);

    VMX_PPR_Virtualization();

    bool bit_high = bx_local_apic_c::get_vector(vm->eoi_exit_bitmap, vector);
    if (bit_high) {
      VMexit(VMX_VMEXIT_VIRTUALIZED_EOI, vector); // trap-like VMEXIT
    }
    else {
      VMX_Evaluate_Pending_Virtual_Interrupts();
    }
  }
  else {
    VMexit(VMX_VMEXIT_APIC_WRITE, BX_LAPIC_EOI); // trap-like vmexit
  }
}

// may be executed as trap-like from handleAsyncEvent and also directy from WRMSR
void BX_CPU_C::VMX_Self_IPI_Virtualization(Bit8u vector)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  vapic_set_vector(BX_LAPIC_IRR1, vector);
  if (vector >= vm->rvi) vm->rvi = vector;

  VMX_Evaluate_Pending_Virtual_Interrupts();
}

void BX_CPU_C::VMX_Deliver_Virtual_Interrupt(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  Bit8u vector = vm->rvi;

  vapic_set_vector(BX_LAPIC_ISR1, vector);

  vm->svi = vector;
  vm->vppr = vector & 0xf0;
  VMX_Write_Virtual_APIC(BX_LAPIC_PPR, vm->vppr);
  vm->rvi = vapic_clear_and_find_highest_priority_int(BX_LAPIC_IRR1, vector);
  clear_event(BX_EVENT_PENDING_VMX_VIRTUAL_INTR);

  BX_CPU_THIS_PTR EXT = 1; /* external event */

#if BX_SUPPORT_UINTR
  if (BX_CPU_THIS_PTR cr4.get_UINTR() && long64_mode() && vector == BX_CPU_THIS_PTR uintr.uinv)
  {
    unsigned vector = vm->svi;
    vm->svi = vapic_clear_and_find_highest_priority_int(BX_LAPIC_ISR1, vector);
    VMX_PPR_Virtualization();

    Process_UINTR_Notification();
  }
  else
#endif
  {
    BX_INSTR_HWINTERRUPT(BX_CPU_ID, vector,
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
    interrupt(vector, BX_EXTERNAL_INTERRUPT, 0, 0);
  }

  BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
  BX_CPU_THIS_PTR EXT = 0;

  // might be not necessary but cleaner code
  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

BX_CPP_INLINE bool self_IPI_VICR(Bit32u vicr)
{
  // reserved bits (31:20, 17:16, 13) and bit 12 (delivery status) must be '0
  // bit 15 (trigger mode) must be '0
  // bits 10:8 (delivery mode) must be '0
  // destination shorthand: must be self = '01
  unsigned dest_shorthand = (vicr >> 18) & 0x3;

  return ((vicr & 0xfff3b700) == 0) && (dest_shorthand == 0x1);
}

void BX_CPU_C::VMX_Write_VICR(void)
{
  Bit32u vicr = VMX_Read_Virtual_APIC(BX_LAPIC_ICR_LO);
  Bit8u vector = vicr & 0xff;

  if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.VIRTUAL_INT_DELIVERY() && self_IPI_VICR(vicr) && vector >= 16) 
  {
    VMX_Self_IPI_Virtualization(vector);
    return;
  }

  VMexit(VMX_VMEXIT_APIC_WRITE, BX_LAPIC_ICR_LO); // trap-like vmexit
}

void BX_CPU_C::VMX_Write_VICR_HI(void)
{
  // clear bits (2:0) of VICR_HI, no VMexit should happen
  BX_DEBUG(("Virtual Apic Access Trap: Clearing ICR_HI[23:0]"));
  Bit32u vicr_hi = VMX_Read_Virtual_APIC(BX_LAPIC_ICR_HI);
         vicr_hi &= 0xff000000;
  VMX_Write_Virtual_APIC(BX_LAPIC_ICR_HI, vicr_hi);
}

#endif // BX_SUPPORT_VMX >= 2

// executed as trap-like from handleAsyncEvent
void BX_CPU_C::VMX_Virtual_Apic_Access_Trap(void)
{
  clear_event(BX_EVENT_VMX_VIRTUAL_APIC_WRITE);

  if (is_pending(BX_EVENT_VMX_VTPR_UPDATE)) {
    VMX_TPR_Virtualization();
  }
#if BX_SUPPORT_VMX >= 2
  else if (is_pending(BX_EVENT_VMX_VEOI_UPDATE)) {
    VMX_EOI_Virtualization();
  }
  else {
    unsigned apic_offset = BX_CPU_THIS_PTR vmcs.apic_access;

    BX_DEBUG(("Trap Event: Virtual Apic Access Trap offset = %08x", apic_offset));

    if (apic_offset >= BX_LAPIC_ICR_HI && apic_offset <= BX_LAPIC_ICR_HI+3) {
      VMX_Write_VICR_HI();
    }
    else if (apic_offset == BX_LAPIC_ICR_LO) {
      VMX_Write_VICR();
    }
    else {
      VMexit(VMX_VMEXIT_APIC_WRITE, apic_offset); // trap-like vmexit
    }
  }
#endif

  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

bool BX_CPU_C::Virtualize_X2APIC_Write(unsigned msr, Bit64u val_64)
{
  if (msr == 0x808) {
    if ((val_64 >> 8) != 0)
      exception(BX_GP_EXCEPTION, 0);

    VMX_Write_Virtual_X2APIC(BX_LAPIC_TPR, val_64 & 0xff);
    VMX_TPR_Virtualization();

    return true;
  }

#if BX_SUPPORT_VMX >= 2
  if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.VIRTUAL_INT_DELIVERY()) {
    if (msr == 0x80b) {
      // EOI virtualization
      if (val_64 != 0)
        exception(BX_GP_EXCEPTION, 0);

      VMX_EOI_Virtualization();
      return true;
    }

    if (msr == 0x83f) {
      // Self IPI virtualization
      if ((val_64 >> 8) != 0)
        exception(BX_GP_EXCEPTION, 0);

      Bit8u vector = val_64 & 0xff;
      if (vector < 16) {
        VMX_Write_Virtual_X2APIC(BX_LAPIC_SELF_IPI, vector);
        VMexit(VMX_VMEXIT_APIC_WRITE, BX_LAPIC_SELF_IPI); // trap-like vmexit
      }
      else {
        VMX_Self_IPI_Virtualization(vector);
      }

      return true;
    }
  }
#endif

  return false;
}

#if BX_SUPPORT_VMX >= 2
bool BX_CPU_C::VMX_Posted_Interrupt_Processing(Bit8u vector)
{
  BX_ASSERT(BX_CPU_THIS_PTR in_vmx_guest);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (! vm->pin_vmexec_ctrls.PROCESS_POSTED_INTERRUPTS())
    return false;

  if (vector != vm->posted_intr_notification_vector)
    return false;

  // ----------- PID format --------
  // | 255:000 | Posted interrupt requests (PIR)
  // |         | One bit for each interrupt vector
  // |     256 | Outstanding notification (PID.ON)
  // | 511-257 | user available
  // --------------------------------

  BxMemtype pid_memtype = BX_MEMTYPE_INVALID;
#if BX_SUPPORT_MEMTYPE
            pid_memtype = resolve_memtype(vm->pid_addr);
#endif

  // clear PID.ON using atomic RMW 'bit clear' operation
  Bit8u pid_ON = read_physical_byte(vm->pid_addr + 32, MEMTYPE(pid_memtype), BX_VMX_PID);
  pid_ON &= ~0x1;
  write_physical_byte(vm->pid_addr + 32, pid_ON, MEMTYPE(pid_memtype), BX_VMX_PID);

  // write(0) into EOI register in the local APIC; this dismisses the interrupt with
  // posted-interrupt notification vector from local APIC.
  // instead of writing to memory, do it directly
  BX_CPU_THIS_PTR lapic->receive_EOI();

  // OR PIR into VIRR and clear PIR atomically (using lock xchg operation)
  // no other agent supposed to read or write PIR between the time it is read and cleared
  BxPackedYmmRegister PIR, tmp;
  access_read_physical(vm->pid_addr, 32, (Bit8u*)(&PIR));
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->pid_addr, 32, MEMTYPE(pid_memtype), BX_READ, BX_VMX_PID, (Bit8u*)(&PIR));
  tmp.clear();
  access_write_physical(vm->pid_addr, 32, (Bit8u*)(&tmp));
  BX_NOTIFY_PHY_MEMORY_ACCESS(vm->pid_addr, 32, MEMTYPE(pid_memtype), BX_WRITE, BX_VMX_PID, (Bit8u*)(&tmp));

  Bit32u virr[8];
  for (unsigned n=0;n<8;n++) {
    virr[n] = VMX_Read_Virtual_APIC(BX_LAPIC_IRR1 + 16*n); // atomic RMW 
    virr[n] |= PIR.ymm32u(n);
    VMX_Write_Virtual_APIC(BX_LAPIC_IRR1 + 16*n, virr[n]);
  }

  vector = BX_CPU_THIS_PTR lapic->highest_priority_int(virr);
  if (vector >= vm->rvi) vm->rvi = vector;

  VMX_Evaluate_Pending_Virtual_Interrupts();

  return true;
}
#endif

#endif // BX_SUPPORT_VMX && BX_SUPPORT_X86_64
