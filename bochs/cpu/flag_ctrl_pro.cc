/////////////////////////////////////////////////////////////////////////
// $Id: flag_ctrl_pro.cc,v 1.11 2002-10-05 06:33:10 kevinlawton Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR




  void
BX_CPU_C::writeEFlags(Bit32u flags, Bit32u changeMask)
{
  Bit32u supportMask, newEFlags;
  
  // Build a mask of the non-reserved bits:
  // x,x,x,x,VM,RF,x,NT,IOPL,OF,DF,IF,TF,SF,ZF,x,AF,x,PF,x,CF
  supportMask = 0x00037fd5;
#if BX_CPU_LEVEL >= 4
  supportMask |= ((1<<21) | (1<<18)); // ID/AC
#endif

  // Screen out changing of any unsupported bits.
  changeMask &= supportMask;

  newEFlags = (BX_CPU_THIS_PTR eflags.val32 & ~changeMask) |
              (flags & changeMask);
  BX_CPU_THIS_PTR setEFlags( newEFlags );
  BX_CPU_THIS_PTR lf_flags_status = 0; // OSZAPC flags are known.

  if (newEFlags & (1 << 8)) {
    BX_CPU_THIS_PTR async_event = 1; // TF = 1
    }
}


  void
BX_CPU_C::write_flags(Bit16u flags, Boolean change_IOPL, Boolean change_IF)
{
  Bit32u changeMask = 0x0dd5;

#if BX_CPU_LEVEL >= 3
  changeMask |= (1<<14); // NT is modified as requested.
  if (change_IOPL)
    changeMask |= (3<<12); // IOPL is modified as requested.
#endif
  if (change_IF)
    changeMask |= (1<<9);

  writeEFlags(Bit32u(flags), changeMask);
}


#if BX_CPU_LEVEL >= 3
  void
BX_CPU_C::write_eflags(Bit32u eflags_raw, Boolean change_IOPL, Boolean change_IF,
                Boolean change_VM, Boolean change_RF)
{
  Bit32u changeMask = 0x4dd5;

#if BX_CPU_LEVEL >= 4
  changeMask |= ((1<<21) | (1<<18)); // ID/AC
#endif
  if (change_IOPL)
    changeMask |= (3<<12);
  if (change_IF)
    changeMask |= (1<<9);
  if (change_VM)
    changeMask |= (1<<17);
  if (change_RF)
    changeMask |= (1<<16);

  writeEFlags(eflags_raw, changeMask);
}
#endif /* BX_CPU_LEVEL >= 3 */


  Bit16u
BX_CPU_C::read_flags(void)
{
  Bit16u flags16;

  // Cause arithmetic flags to be in known state and cached in val32.
  if (BX_CPU_THIS_PTR lf_flags_status) {
    (void) get_CF();
    (void) get_PF();
    (void) get_AF();
    (void) get_ZF();
    (void) get_SF();
    (void) get_OF();
    }
  flags16 = (Bit16u) BX_CPU_THIS_PTR eflags.val32;

  /* 8086: bits 12-15 always set to 1.
   * 286: in real mode, bits 12-15 always cleared.
   * 386+: real-mode: bit15 cleared, bits 14..12 are last loaded value
   *       protected-mode: bit 15 clear, bit 14 = last loaded, IOPL?
   */
#if BX_CPU_LEVEL < 2
  flags16 |= 0xF000;  /* 8086 nature */
#elif BX_CPU_LEVEL == 2
  if (real_mode()) {
    flags16 &= 0x0FFF;  /* 80286 in real mode nature */
    }
#else /* 386+ */
#endif

  return(flags16);
}


#if BX_CPU_LEVEL >= 3
  Bit32u
BX_CPU_C::read_eflags(void)
{
  Bit32u flags32;

  // Cause arithmetic flags to be in known state and cached in val32.
  if (BX_CPU_THIS_PTR lf_flags_status) {
    (void) get_CF();
    (void) get_PF();
    (void) get_AF();
    (void) get_ZF();
    (void) get_SF();
    (void) get_OF();
    }
  flags32 = BX_CPU_THIS_PTR eflags.val32;

#if 0
  /*
   * 386+: real-mode: bit15 cleared, bits 14..12 are last loaded value
   *       protected-mode: bit 15 clear, bit 14 = last loaded, IOPL?
   */
#endif

  return(flags32);
}
#endif /* BX_CPU_LEVEL >= 3 */
