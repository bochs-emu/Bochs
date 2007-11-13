/////////////////////////////////////////////////////////////////////////
// $Id: resolve32.cc,v 1.12 2007-11-13 17:30:54 sshwarts Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod0Rm(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->rm());
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod0Base(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->sibBase());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod0Disp(bxInstruction_c *i)
{
  RMAddr(i) = i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod0BaseIndex(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale());
}
  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod0DispIndex(bxInstruction_c *i)
{
  RMAddr(i) = i->displ32u() + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale());
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod1or2Rm(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->rm()) + i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod1or2Base(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->sibBase()) + i->displ32u();
}

  void  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Mod1or2BaseIndex(bxInstruction_c *i)
{
  RMAddr(i) = BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + i->displ32u();
}
