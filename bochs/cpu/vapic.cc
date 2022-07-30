/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2012-2015 Stanislav Shwartsman
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

#include "memory/memory-bochs.h"

#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64

bool BX_CPP_AttrRegparmN(1) BX_CPU_C::is_virtual_apic_page(bx_phy_address paddr)
{
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES))
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

  if (is_pending(BX_EVENT_VMX_VTPR_UPDATE | BX_EVENT_VMX_VEOI_UPDATE | BX_EVENT_VMX_VIRTUAL_APIC_WRITE)) {
    if (BX_CPU_THIS_PTR vmcs.apic_access != offset) {
      BX_ERROR(("Second APIC virtualization at offset 0x%08x (first access at offset 0x%08x)", offset, BX_CPU_THIS_PTR vmcs.apic_access));
      return true;
    }
  }

  // access is not instruction fetch because cpu::prefetch will crash them
  if (! VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW) || len > 4 || offset >= 0x400)
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

void BX_CPU_C::VMX_Write_Virtual_APIC(unsigned offset, Bit32u val32)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + offset;
  // must avoid recursive call to the function when VMX APIC access page = VMX Virtual Apic Page
  BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, pAddr, 4, (Bit8u*)(&val32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, MEMTYPE(resolve_memtype(pAddr)), BX_WRITE, BX_VMX_VAPIC_ACCESS, (Bit8u*)(&val32));
}

bx_phy_address BX_CPU_C::VMX_Virtual_Apic_Read(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

  BX_INFO(("Virtual Apic RD 0x" FMT_ADDRX " len = %d", paddr, len));

  Bit32u offset = PAGE_OFFSET(paddr);

  bool vmexit = virtual_apic_access_vmexit(offset, len);

  // access is not instruction fetch because cpu::prefetch will crash them
  if (! vmexit) {

    if (!SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS)) {
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
  paddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + offset;
  BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, len, MEMTYPE(resolve_memtype(paddr)), BX_READ, BX_VMX_VAPIC_ACCESS, (Bit8u*) data);
  return paddr;
}

void BX_CPU_C::VMX_Virtual_Apic_Write(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

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
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY)) {
      if (offset == BX_LAPIC_EOI) {
         signal_event(BX_EVENT_VMX_VEOI_UPDATE);
      }
    }

    switch(offset & 0x3fc) {
    case BX_LAPIC_ID:
    case BX_LAPIC_TPR:
    case BX_LAPIC_ICR_HI:
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
      // VMX_VMEXIT_APIC_ACCESS if the control is disabled
      if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS)) break;
      // else fall through

    case BX_LAPIC_EOI:
    case BX_LAPIC_ICR_LO:
      // VMX_VMEXIT_APIC_ACCESS if both controls are disabled
      if (! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY) &&
          ! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_REGISTERS)) break;
      // else fall through

      // remap access to virtual apic page
      paddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + offset;
      // must avoid recursive call to the function when VMX APIC access page = VMX Virtual Apic Page
      BX_MEM(0)->writePhysicalPage(BX_CPU_THIS, paddr, len, (Bit8u *) data);
      BX_NOTIFY_PHY_MEMORY_ACCESS(paddr, len, MEMTYPE(resolve_memtype(paddr)), BX_WRITE, BX_VMX_VAPIC_ACCESS, (Bit8u *) data);
      signal_event(BX_EVENT_VMX_VIRTUAL_APIC_WRITE);
      return;

    default:
      break;
    }
#endif
  }

  Bit32u qualification = offset | 
      ((BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_WRITE_INSTRUCTION_EXECUTION);
  VMexit(VMX_VMEXIT_APIC_ACCESS, qualification);
}

#if BX_SUPPORT_VMX >= 2

BX_CPP_INLINE bool vapic_read_vector(Bit32u *arr, Bit8u vector)
{
  unsigned apic_reg = vector / 32;

  return arr[apic_reg] & (1 << (vector & 0x1f));
}

BX_CPP_INLINE void BX_CPU_C::vapic_set_vector(unsigned arrbase, Bit8u vector)
{
  unsigned reg = vector / 32;
  Bit32u regval = VMX_Read_Virtual_APIC(arrbase + 0x10*reg);
  regval |= (1 << (vector & 0x1f));
  VMX_Write_Virtual_APIC(arrbase + 0x10*reg, regval);
}

BX_CPP_INLINE Bit8u BX_CPU_C::vapic_clear_and_find_highest_priority_int(unsigned arrbase, Bit8u vector)
{
  Bit32u arr[8];
  int n;

  for (n=0;n<8;n++)
    arr[n] = VMX_Read_Virtual_APIC(arrbase + 0x10*n);

  unsigned reg = vector / 32;
  arr[reg] &= ~(1 << (vector & 0x1f));

  VMX_Write_Virtual_APIC(arrbase + 0x10*reg, arr[reg]);

  for (n = 7; n >= 0; n--) {
    if (! arr[n]) continue;

    for (int bit = 31; bit >= 0; bit--) {
      if (arr[n] & (1<<bit)) {
        return (n * 32 + bit);
      }
    }
  }

  return 0;
}

void BX_CPU_C::VMX_Evaluate_Pending_Virtual_Interrupts(void)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;

  if (! VMEXIT(VMX_VM_EXEC_CTRL2_INTERRUPT_WINDOW_VMEXIT) && (vm->rvi >> 4) > (vm->vppr >> 4))
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
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY)) {
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

  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY))
  {
    VMX_Write_Virtual_APIC(BX_LAPIC_EOI, 0);

    unsigned vector = vm->svi;
    vm->svi = vapic_clear_and_find_highest_priority_int(BX_LAPIC_ISR1, vector);

    VMX_PPR_Virtualization();

    if (vapic_read_vector(vm->eoi_exit_bitmap, vector)) {
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

  BX_INSTR_HWINTERRUPT(BX_CPU_ID, vector,
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
  interrupt(vector, BX_EXTERNAL_INTERRUPT, 0, 0);

  BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
  BX_CPU_THIS_PTR EXT = 0;

  // might be not necessary but cleaner code
  longjmp(BX_CPU_THIS_PTR jmp_buf_env, 1); // go back to main decode loop
}

void BX_CPU_C::VMX_Write_VICR(void)
{
  Bit32u vicr = VMX_Read_Virtual_APIC(BX_LAPIC_ICR_LO);

  unsigned dest_shorthand = (vicr >> 18) & 0x3;
  Bit8u vector = vicr & 0xff;

  // reserved bits (31:20, 17:16, 13), 15 (trigger mode), 12 (delivery status), 10:8 (delivery mode) must be 0
  // destination shorthand: must be self
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY) &&
     (vicr & 0xfff3b700) == 0 && (dest_shorthand == 0x1) && vector >= 16)
  {
    VMX_Self_IPI_Virtualization(vector);
  }
  else {
    VMexit(VMX_VMEXIT_APIC_WRITE, BX_LAPIC_ICR_LO); // trap-like vmexit
  }
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
      // clear bits (2:0) of VICR_HI, no VMexit should happen
      BX_DEBUG(("Virtual Apic Access Trap: Clearing ICR_HI[23:0]"));
      Bit32u vicr_hi = VMX_Read_Virtual_APIC(BX_LAPIC_ICR_HI);
      VMX_Write_Virtual_APIC(BX_LAPIC_ICR_HI, vicr_hi & 0xff000000);
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

    VMX_Write_Virtual_APIC(BX_LAPIC_TPR, val_64 & 0xff);
    VMX_Write_Virtual_APIC(BX_LAPIC_TPR + 4, 0);
    VMX_TPR_Virtualization();

    return true;
  }

#if BX_SUPPORT_VMX >= 2
  if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUAL_INT_DELIVERY)) {
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
        VMX_Write_Virtual_APIC(BX_LAPIC_SELF_IPI, vector);
        VMX_Write_Virtual_APIC(BX_LAPIC_SELF_IPI + 4, 0);
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

#endif // BX_SUPPORT_VMX && BX_SUPPORT_X86_64
