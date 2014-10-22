/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012  The Bochs Project
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

#include <stdlib.h>

#define HW_RANDOM_GENERATOR_READY (1)

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Ew(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDRAND_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDRAND, 0);
    }
  }
#endif

  Bit16u val_16 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_16 |= rand() & 0xff;  // hack using std C rand() function
    val_16 <<= 8;
    val_16 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_16BIT_REG(i->dst(), val_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDRAND_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDRAND, 0);
    }
  }
#endif

  Bit32u val_32 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_32 |= rand() & 0xff;  // hack using std C rand() function
    val_32 <<= 8;
    val_32 |= rand() & 0xff;
    val_32 <<= 8;
    val_32 |= rand() & 0xff;
    val_32 <<= 8;
    val_32 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_32BIT_REGZ(i->dst(), val_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Eq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDRAND_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDRAND, 0);
    }
  }
#endif

  Bit64u val_64 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_64 |= rand() & 0xff;  // hack using std C rand() function
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_64BIT_REG(i->dst(), val_64);

  BX_NEXT_INSTR(i);
}
#endif

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Ew(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDSEED_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDSEED, 0);
    }
  }
#endif

  Bit16u val_16 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_16 |= rand() & 0xff;  // hack using std C rand() function
    val_16 <<= 8;
    val_16 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_16BIT_REG(i->dst(), val_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDSEED_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDSEED, 0);
    }
  }
#endif

  Bit32u val_32 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_32 |= rand() & 0xff;  // hack using std C rand() function
    val_32 <<= 8;
    val_32 |= rand() & 0xff;
    val_32 <<= 8;
    val_32 |= rand() & 0xff;
    val_32 <<= 8;
    val_32 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_32BIT_REGZ(i->dst(), val_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Eq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (SECONDARY_VMEXEC_CONTROL(VMX_VM_EXEC_CTRL3_RDSEED_VMEXIT)) {
      VMexit(VMX_VMEXIT_RDSEED, 0);
    }
  }
#endif

  Bit64u val_64 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_64 |= rand() & 0xff;  // hack using std C rand() function
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;
    val_64 <<= 8;
    val_64 |= rand() & 0xff;

    assert_CF();
  }

  BX_WRITE_64BIT_REG(i->dst(), val_64);

  BX_NEXT_INSTR(i);
}
#endif
