/////////////////////////////////////////////////////////////////////////
// $Id: resolve32.cc,v 1.6 2002-09-17 22:50:52 kevinlawton Exp $
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
BX_CPU_C::Resolve32Mod0Rm0(bxInstruction_c *i)
{
  i->rm_addr = EAX;
}
  void
BX_CPU_C::Resolve32Mod0Rm1(bxInstruction_c *i)
{
  i->rm_addr = ECX;
}
  void
BX_CPU_C::Resolve32Mod0Rm2(bxInstruction_c *i)
{
  i->rm_addr = EDX;
}
  void
BX_CPU_C::Resolve32Mod0Rm3(bxInstruction_c *i)
{
  i->rm_addr = EBX;
}
  void
BX_CPU_C::Resolve32Mod0Rm6(bxInstruction_c *i)
{
  i->rm_addr = ESI;
}
  void
BX_CPU_C::Resolve32Mod0Rm7(bxInstruction_c *i)
{
  i->rm_addr = EDI;
}


  void
BX_CPU_C::Resolve32Mod1or2Rm0(bxInstruction_c *i)
{
  i->rm_addr = EAX + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm1(bxInstruction_c *i)
{
  i->rm_addr = ECX + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm2(bxInstruction_c *i)
{
  i->rm_addr = EDX + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm3(bxInstruction_c *i)
{
  i->rm_addr = EBX + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm5(bxInstruction_c *i)
{
  i->rm_addr = EBP + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm6(bxInstruction_c *i)
{
  i->rm_addr = ESI + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Rm7(bxInstruction_c *i)
{
  i->rm_addr = EDI + i->displ32u();
}


  void
BX_CPU_C::Resolve32Mod0Base0(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EAX + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base1(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ECX + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base2(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EDX + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base3(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EBX + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base4(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ESP + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base5(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = i->displ32u() + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base6(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ESI + scaled_index;
}
  void
BX_CPU_C::Resolve32Mod0Base7(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EDI + scaled_index;
}




  void
BX_CPU_C::Resolve32Mod1or2Base0(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EAX + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base1(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ECX + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base2(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EDX + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base3(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EBX + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base4(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ESP + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base5(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EBP + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base6(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = ESI + scaled_index + i->displ32u();
}
  void
BX_CPU_C::Resolve32Mod1or2Base7(bxInstruction_c *i)
{
  Bit32u scaled_index;

  if (i->sibIndex() != 4)
    scaled_index = BX_READ_32BIT_REG(i->sibIndex()) << i->sibScale();
  else
    scaled_index = 0;
  i->rm_addr = EDI + scaled_index + i->displ32u();
}
