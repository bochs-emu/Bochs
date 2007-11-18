/////////////////////////////////////////////////////////////////////////
// $Id: resolve64.cc,v 1.13 2007-11-18 19:46:14 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_X86_64

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Rip(bxInstruction_c *i)
{
  // RIP hasn't been bumped yet when this is called.  must choose the saved value.
  RMAddr(i) = (Bit32u) (BX_CPU_THIS_PTR prev_eip + i->ilen() + (Bit32s) i->displ32u());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Rip(bxInstruction_c *i)
{
  // RIP hasn't been bumped yet when this is called.  must choose the saved value.
  RMAddr(i) = BX_CPU_THIS_PTR prev_eip + i->ilen() + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64DispIndex(bxInstruction_c *i)
{
  RMAddr(i) = (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Rm(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->rm()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Disp(bxInstruction_c *i)
{
  RMAddr(i) = (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Base(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64BaseIndex(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase()) + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
}

#endif /* if BX_SUPPORT_X86_64 */
