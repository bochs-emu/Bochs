/////////////////////////////////////////////////////////////////////////
// $Id: xsave.cc,v 1.1 2008-02-12 22:42:47 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

/* 0F AE /4 */
void BX_CPU_C::XSAVE(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE

  BX_PANIC(("XSAVE still not implemented !"));

#else
  BX_INFO(("XSAVE: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F AE /5 */
void BX_CPU_C::XRSTOR(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE

  BX_PANIC(("XRSTOR still not implemented !"));

#else
  BX_INFO(("XRSTOR: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 01 D0 */
void BX_CPU_C::XGETBV(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE

  BX_PANIC(("XGETBV still not implemented !"));

#else
  BX_INFO(("XGETBV: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}

/* 0F 01 D1 */
void BX_CPU_C::XSETBV(bxInstruction_c *i)
{
#if BX_SUPPORT_XSAVE

  BX_PANIC(("XSETBV still not implemented !"));

#else
  BX_INFO(("XSETBV: required XSAVE support, use --enable-xsave option"));
  UndefinedOpcode(i);
#endif
}
