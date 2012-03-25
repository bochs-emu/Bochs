/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2011  The Bochs Project
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::setEFlags(Bit32u val)
{
  // VM flag could not be set from long mode
#if BX_SUPPORT_X86_64
  if (long_mode()) {
    if (BX_CPU_THIS_PTR get_VM()) BX_PANIC(("VM is set in long mode !"));
    val &= ~EFlagsVMMask;
  }
#endif

  if (val & EFlagsRFMask) invalidate_prefetch_q();

  if (val & EFlagsTFMask) {
    BX_CPU_THIS_PTR async_event = 1; // TF == 1 || RF == 1
  }

  if (val & EFlagsIFMask) {
    if (! BX_CPU_THIS_PTR get_IF())
      BX_CPU_THIS_PTR async_event = 1; // IF bit was set
  }

  BX_CPU_THIS_PTR eflags = val;
  setEFlagsOSZAPC(val);			// update lazy flags state

#if BX_CPU_LEVEL >= 4
  handleAlignmentCheck(/* EFLAGS.AC reloaded */);
#endif

  handleCpuModeChange(); // VM flag might be changed
}

  void BX_CPP_AttrRegparmN(2)
BX_CPU_C::writeEFlags(Bit32u flags, Bit32u changeMask)
{
  // Build a mask of the non-reserved bits:
  // ID,VIP,VIF,AC,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u supportMask = 0x00037fd5;
#if BX_CPU_LEVEL >= 4
  supportMask |= (EFlagsIDMask | EFlagsACMask); // ID/AC
#endif
#if BX_CPU_LEVEL >= 5
  supportMask |= (EFlagsVIPMask | EFlagsVIFMask); // VIP/VIF
#endif

  // Screen out changing of any unsupported bits.
  changeMask &= supportMask;

  Bit32u newEFlags = (read_eflags() & ~changeMask) | (flags & changeMask);
  setEFlags(newEFlags);
}

  void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_flags(Bit16u flags, bx_bool change_IOPL, bx_bool change_IF)
{
  // Build a mask of the following bits:
  // x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  Bit32u changeMask = 0x0dd5;

#if BX_CPU_LEVEL >= 3
  changeMask |= EFlagsNTMask;     // NT is modified as requested.
  if (change_IOPL)
    changeMask |= EFlagsIOPLMask; // IOPL is modified as requested.
#endif
  if (change_IF)
    changeMask |= EFlagsIFMask;

  writeEFlags(Bit32u(flags), changeMask);
}

// Cause arithmetic flags to be in known state and cached in val32.
Bit32u BX_CPU_C::force_flags(void)
{
  Bit32u newflags  = getB_CF();
         newflags |= getB_PF() << 2;
         newflags |= getB_AF() << 4;
         newflags |= getB_ZF() << 6;
         newflags |= getB_SF() << 7;
         newflags |= getB_OF() << 11;

  BX_CPU_THIS_PTR eflags = (BX_CPU_THIS_PTR eflags & ~EFlagsOSZAPCMask)
    | (newflags & EFlagsOSZAPCMask);

  return BX_CPU_THIS_PTR eflags;
}
