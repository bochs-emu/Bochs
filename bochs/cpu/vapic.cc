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

#if BX_SUPPORT_VMX && BX_SUPPORT_X86_64

bx_bool BX_CPP_AttrRegparmN(1) BX_CPU_C::is_virtual_apic_page(bx_phy_address paddr)
{
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES))
      if (PPFOf(paddr) == PPFOf(vm->apic_access_page)) return 1;
  }

  return 0;
}

Bit32u BX_CPU_C::VMX_Read_VTPR(void)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR vmcs.virtual_apic_page_addr + BX_LAPIC_TPR;
  Bit32u vtpr;
  access_read_physical(pAddr, 4, (Bit8u*)(&vtpr));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, BX_READ, BX_VMX_VAPIC_ACCESS, (Bit8u*)(&vtpr));
  return vtpr;
}

void BX_CPU_C::VMX_Write_VTPR(Bit8u vtpr)
{
  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  bx_phy_address pAddr = vm->virtual_apic_page_addr + BX_LAPIC_TPR;
  Bit32u field32 = vtpr;

  access_write_physical(pAddr, 4, (Bit8u*)(&field32));
  BX_NOTIFY_PHY_MEMORY_ACCESS(pAddr, 4, BX_WRITE, BX_VMX_VAPIC_ACCESS, (Bit8u*)(&field32));

  Bit8u tpr_shadow = vtpr >> 4;
  if (tpr_shadow < vm->vm_tpr_threshold) {
    // commit current instruction to produce trap-like VMexit
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
    VMexit(VMX_VMEXIT_TPR_THRESHOLD, 0);
  }
}

void BX_CPU_C::VMX_Virtual_Apic_Read(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

  Bit32u offset = PAGE_OFFSET(paddr);

  // access is not instruction fetch because cpu::prefetch will crash them
  if (VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW) && offset == BX_LAPIC_TPR && len <= 4) {
     // VTPR access
     Bit32u vtpr = VMX_Read_VTPR();
     if (len == 1)
       *((Bit8u *) data) = vtpr & 0xff;
     else if (len == 2)
       *((Bit16u *) data) = vtpr & 0xffff;
     else if (len == 4)
       *((Bit32u *) data) = vtpr;
     else
       BX_PANIC(("PANIC: Unsupported Virtual APIC access len = 3 !"));
     return;
  }

  Bit32u qualification = offset | 
      (BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_READ_INSTRUCTION_EXECUTION;
  VMexit(VMX_VMEXIT_APIC_ACCESS, qualification);
}

void BX_CPU_C::VMX_Virtual_Apic_Write(bx_phy_address paddr, unsigned len, void *data)
{
  BX_ASSERT(SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VIRTUALIZE_APIC_ACCESSES));

  Bit32u offset = PAGE_OFFSET(paddr);

  if (VMEXIT(VMX_VM_EXEC_CTRL2_TPR_SHADOW) && offset == BX_LAPIC_TPR && len <= 4) {
    // VTPR access
    VMX_Write_VTPR(*((Bit8u *) data));
    return;
  }

  Bit32u qualification = offset | 
      (BX_CPU_THIS_PTR in_event) ? VMX_APIC_ACCESS_DURING_EVENT_DELIVERY : VMX_APIC_WRITE_INSTRUCTION_EXECUTION;
  VMexit(VMX_VMEXIT_APIC_ACCESS, qualification);
}

#endif // BX_SUPPORT_VMX && BX_SUPPORT_X86_64
