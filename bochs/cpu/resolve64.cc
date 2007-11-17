/////////////////////////////////////////////////////////////////////////
// $Id: resolve64.cc,v 1.12 2007-11-17 23:28:32 sshwarts Exp $
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
BX_CPU_C::BxResolve32Mod0Rip(bxInstruction_c *i)
{
  // RIP hasn't been bumped yet when this is called.  must choose the saved value.
  RMAddr(i) = (Bit32u) (BX_CPU_THIS_PTR prev_eip + i->ilen() + (Bit32s)i->displ32u());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0Rip(bxInstruction_c *i)
{
  // RIP hasn't been bumped yet when this is called.  must choose the saved value.
  RMAddr(i) = BX_CPU_THIS_PTR prev_eip + i->ilen() + (Bit32s)i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0Rm(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->rm());
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0Base(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0Disp(bxInstruction_c *i)
{
  RMAddr(i) = (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0BaseIndex(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase()) + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod0DispIndex(bxInstruction_c *i)
{
  RMAddr(i) = (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod1or2Rm(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->rm()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod1or2Base(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase()) + (Bit32s) i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Mod1or2BaseIndex(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_64BIT_REG(i->sibBase()) + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
}

#endif /* if BX_SUPPORT_X86_64 */
