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





#include "bochs.h"




  void
BX_CPU_C::Resolve16Mod0Rm0(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BX + SI);
}
  void
BX_CPU_C::Resolve16Mod0Rm1(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BX + DI);
}
  void
BX_CPU_C::Resolve16Mod0Rm2(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BP + SI);
}
  void
BX_CPU_C::Resolve16Mod0Rm3(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BP + DI);
}
  void
BX_CPU_C::Resolve16Mod0Rm4(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) SI;
}
  void
BX_CPU_C::Resolve16Mod0Rm5(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) DI;
}
  void
BX_CPU_C::Resolve16Mod0Rm7(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) BX;
}

  void
BX_CPU_C::Resolve16Mod1or2Rm0(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BX + SI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm1(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BX + DI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm2(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BP + SI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm3(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BP + DI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm4(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (SI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm5(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (DI + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm6(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BP + (Bit16s) i->displ16u);
}
  void
BX_CPU_C::Resolve16Mod1or2Rm7(BxInstruction_t *i)
{
  i->rm_addr = (Bit16u) (BX + (Bit16s) i->displ16u);
}
