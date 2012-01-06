/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2009 Stanislav Shwartsman
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

//
// 16 bit address size
//

  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve16BaseIndex(bxInstruction_c *i)
{
  return (Bit16u) (BX_READ_16BIT_REG(i->sibBase()) + BX_READ_16BIT_REG(i->sibIndex()) + i->displ16s());
}

//
// 32 bit address size
//

  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32Base(bxInstruction_c *i)
{
  return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + i->displ32s());
}
  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve32BaseIndex(bxInstruction_c *i)
{
  return (Bit32u) (BX_READ_32BIT_REG(i->sibBase()) + (BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale()) + i->displ32s());
}

//
// 64 bit address size
//

#if BX_SUPPORT_X86_64
  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64Base(bxInstruction_c *i)
{
  return BX_READ_64BIT_REG(i->sibBase()) + i->displ32s();
}
  bx_address  BX_CPP_AttrRegparmN(1)
BX_CPU_C::BxResolve64BaseIndex(bxInstruction_c *i)
{
  return BX_READ_64BIT_REG(i->sibBase()) + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + i->displ32s();
}
#endif
