/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2019  The Bochs Project
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

bool hw_rand_ready()
{
  return HW_RANDOM_GENERATOR_READY;
}

// provide a byte of data from Hardware Random Generator (TBD: implement as device)
Bit8u hw_rand8()
{
  return rand() & 0xff;     // hack using std C rand() function
}

// provide a 16-bit of data from Hardware Random Generator (TBD: implement as device)
Bit16u hw_rand16()
{
  Bit16u val_16 = 0;

  val_16 |= hw_rand8();
  val_16 <<= 8;
  val_16 |= hw_rand8();

  return val_16;
}

// provide a 32-bit of data from Hardware Random Generator (TBD: implement as device)
Bit32u hw_rand32()
{
  Bit32u val_32 = 0;

  val_32 |= hw_rand16();
  val_32 <<= 16;
  val_32 |= hw_rand16();

  return val_32;
}

// provide a 64-bit of data from Hardware Random Generator (TBD: implement as device)
Bit64u hw_rand64()
{
  Bit64u val_64 = 0;

  val_64 |= hw_rand32();
  val_64 <<= 32;
  val_64 |= hw_rand32();

  return val_64;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Ew(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDRAND_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDRAND, BX_READ);
    }
  }
#endif

  Bit16u val_16 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_16 = hw_rand16();
    assert_CF();
  }

  BX_WRITE_16BIT_REG(i->dst(), val_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDRAND_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDRAND, BX_READ);
    }
  }
#endif

  Bit32u val_32 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_32 = hw_rand32();
    assert_CF();
  }

  BX_WRITE_32BIT_REGZ(i->dst(), val_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDRAND_Eq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDRAND_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDRAND, BX_READ);
    }
  }
#endif

  Bit64u val_64 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_64 = hw_rand64();
    assert_CF();
  }

  BX_WRITE_64BIT_REG(i->dst(), val_64);

  BX_NEXT_INSTR(i);
}
#endif

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Ew(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDSEED_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDSEED, BX_READ);
    }
  }
#endif

  Bit16u val_16 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_16 = hw_rand16();
    assert_CF();
  }

  BX_WRITE_16BIT_REG(i->dst(), val_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Ed(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDSEED_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDSEED, BX_READ);
    }
  }
#endif

  Bit32u val_32 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_32 = hw_rand32();
    assert_CF();
  }

  BX_WRITE_32BIT_REGZ(i->dst(), val_32);

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::RDSEED_Eq(bxInstruction_c *i)
{
#if BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    if (BX_CPU_THIS_PTR vmcs.vmexec_ctrls2.RDSEED_VMEXIT()) {
      VMexit_Instruction(i, VMX_VMEXIT_RDSEED, BX_READ);
    }
  }
#endif

  Bit64u val_64 = 0;

  clearEFlagsOSZAPC();

  if (HW_RANDOM_GENERATOR_READY) {
    val_64 = hw_rand64();
    assert_CF();
  }

  BX_WRITE_64BIT_REG(i->dst(), val_64);

  BX_NEXT_INSTR(i);
}
#endif
