/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2011-2013 Stanislav Shwartsman
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::VMFUNC(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX >= 2
  if (! BX_CPU_THIS_PTR in_vmx_guest || ! SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_VMFUNC_ENABLE))
    exception(BX_UD_EXCEPTION, 0);

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  Bit32u function = EAX;

  if (function >= 64) {
    BX_ERROR(("VMFUNC: invalid function 0x%08x", function));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (0 == (vm->vmfunc_ctrls & (BX_CONST64(1)<<function))) {
    BX_ERROR(("VMFUNC: function %d not enabled", function));
    VMexit(VMX_VMEXIT_VMFUNC, 0);
  }

  switch(function) {
  case VMX_VMFUNC_EPTP_SWITCHING:
     vmfunc_eptp_switching();
     break;

  default:
     BX_PANIC(("VMFUNC: invalid function 0x%08x", function));
  }
#endif

  BX_NEXT_TRACE(i);
}

#if BX_SUPPORT_VMX >= 2
void BX_CPU_C::vmfunc_eptp_switching(void)
{
  Bit32u eptp_list_entry = ECX;

  if (eptp_list_entry >= 512) {
    BX_ERROR(("vmfunc_eptp_switching: invalid EPTP list entry %d", eptp_list_entry));
    VMexit(VMX_VMEXIT_VMFUNC, 0);
  }

  VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
  Bit64u temp_eptp;

  access_read_physical(vm->eptp_list_address + 8 * ECX, 8, &temp_eptp);
  if (! is_eptptr_valid(temp_eptp)) {
    BX_ERROR(("vmfunc_eptp_switching: invalid EPTP value in EPTP entry %d", ECX));
    VMexit(VMX_VMEXIT_VMFUNC, 0);
  }

  vm->eptptr = temp_eptp;
  VMwrite64(VMCS_64BIT_CONTROL_EPTPTR, temp_eptp);
  TLB_flush();

  if (BX_SUPPORT_VMX_EXTENSION(BX_VMX_EPT_EXCEPTION)) {
    vm->eptp_index = eptp_list_entry /* & 0xffff */ /* not needed because eptp_list_entry < 512 */;
    VMwrite16(VMCS_16BIT_CONTROL_EPTP_INDEX, vm->eptp_index);
  }
}
#endif
