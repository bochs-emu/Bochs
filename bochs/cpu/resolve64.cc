/////////////////////////////////////////////////////////////////////////
// $Id: resolve64.cc,v 1.4 2002-09-18 08:00:40 kevinlawton Exp $
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
BX_CPU_C::Resolve64Mod0Rm0(bxInstruction_c *i)
{
  RMAddr(i) = RAX;
}
  void
BX_CPU_C::Resolve64Mod0Rm1(bxInstruction_c *i)
{
  RMAddr(i) = RCX;
}
  void
BX_CPU_C::Resolve64Mod0Rm2(bxInstruction_c *i)
{
  RMAddr(i) = RDX;
}
  void
BX_CPU_C::Resolve64Mod0Rm3(bxInstruction_c *i)
{
  RMAddr(i) = RBX;
}
  void
BX_CPU_C::Resolve64Mod0Rm5(bxInstruction_c *i)
{
  // eip hasn't been bumped yet when this is called.  must choose the saved value.
  RMAddr(i) = BX_CPU_THIS_PTR prev_eip + i->ilen() + (Bit32s)i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod0Rm6(bxInstruction_c *i)
{
  RMAddr(i) = RSI;
}
  void
BX_CPU_C::Resolve64Mod0Rm7(bxInstruction_c *i)
{
  RMAddr(i) = RDI;
}
  void
BX_CPU_C::Resolve64Mod0Rm8(bxInstruction_c *i)
{
  RMAddr(i) = R8;
}
  void
BX_CPU_C::Resolve64Mod0Rm9(bxInstruction_c *i)
{
  RMAddr(i) = R9;
}
  void
BX_CPU_C::Resolve64Mod0Rm10(bxInstruction_c *i)
{
  RMAddr(i) = R10;
}
  void
BX_CPU_C::Resolve64Mod0Rm11(bxInstruction_c *i)
{
  RMAddr(i) = R11;
}
  void
BX_CPU_C::Resolve64Mod0Rm12(bxInstruction_c *i)
{
  RMAddr(i) = R12;
}
  void
BX_CPU_C::Resolve64Mod0Rm13(bxInstruction_c *i)
{
  RMAddr(i) = R13;
}

  void
BX_CPU_C::Resolve64Mod0Rm14(bxInstruction_c *i)
{
  RMAddr(i) = R14;
}

  void
BX_CPU_C::Resolve64Mod0Rm15(bxInstruction_c *i)
{
  RMAddr(i) = R15;
}


  void
BX_CPU_C::Resolve64Mod1or2Rm0(bxInstruction_c *i)
{
  RMAddr(i) = RAX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm1(bxInstruction_c *i)
{
  RMAddr(i) = RCX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm2(bxInstruction_c *i)
{
  RMAddr(i) = RDX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm3(bxInstruction_c *i)
{
  RMAddr(i) = RBX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm5(bxInstruction_c *i)
{
  RMAddr(i) = RBP + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm6(bxInstruction_c *i)
{
  RMAddr(i) = RSI + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm7(bxInstruction_c *i)
{
  RMAddr(i) = RDI + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm8(bxInstruction_c *i)
{
  RMAddr(i) = R8 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm9(bxInstruction_c *i)
{
  RMAddr(i) = R9 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm10(bxInstruction_c *i)
{
  RMAddr(i) = R10 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm11(bxInstruction_c *i)
{
  RMAddr(i) = R11 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm12(bxInstruction_c *i)
{
  RMAddr(i) = R12 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm13(bxInstruction_c *i)
{
  RMAddr(i) = R13 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm14(bxInstruction_c *i)
{
  RMAddr(i) = R14 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Rm15(bxInstruction_c *i)
{
  RMAddr(i) = R15 + (Bit32s) i->displ32u();
}



  void
BX_CPU_C::Resolve64Mod0Base0(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RAX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RAX;
}
  void
BX_CPU_C::Resolve64Mod0Base1(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RCX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RCX;
}
  void
BX_CPU_C::Resolve64Mod0Base2(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RDX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RDX;
}
  void
BX_CPU_C::Resolve64Mod0Base3(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RBX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RBX;
}
  void
BX_CPU_C::Resolve64Mod0Base4(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RSP + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RSP;
}
  void
BX_CPU_C::Resolve64Mod0Base5(bxInstruction_c *i)
{
  if (i->sibIndex() != 4) {
    RMAddr(i) = (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
    }
  else
    RMAddr(i) = (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod0Base6(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RSI + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RSI;
}
  void
BX_CPU_C::Resolve64Mod0Base7(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RDI + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = RDI;
}
  void
BX_CPU_C::Resolve64Mod0Base8(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R8 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R8;
}
  void
BX_CPU_C::Resolve64Mod0Base9(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R9 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R9;
}
  void
BX_CPU_C::Resolve64Mod0Base10(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R10 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R10;
}
  void
BX_CPU_C::Resolve64Mod0Base11(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R11 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R11;
}
  void
BX_CPU_C::Resolve64Mod0Base12(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R12 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R12;
}
  void
BX_CPU_C::Resolve64Mod0Base13(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R13 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R13;
}
  void
BX_CPU_C::Resolve64Mod0Base14(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R14 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R14;
}
  void
BX_CPU_C::Resolve64Mod0Base15(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R15 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale());
  else
    RMAddr(i) = R15;
}




  void
BX_CPU_C::Resolve64Mod1or2Base0(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RAX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RAX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base1(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RCX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RCX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base2(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RDX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RDX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base3(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RBX + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RBX + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base4(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RSP + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RSP + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base5(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RBP + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RBP + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base6(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RSI + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RSI + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base7(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = RDI + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = RDI + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base8(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R8 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R8 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base9(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R9 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R9 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base10(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R10 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R10 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base11(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R11 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R11 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base12(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R12 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R12 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base13(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R13 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R13 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base14(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R14 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R14 + (Bit32s) i->displ32u();
}
  void
BX_CPU_C::Resolve64Mod1or2Base15(bxInstruction_c *i)
{
  if (i->sibIndex() != 4)
    RMAddr(i) = R15 + (BX_READ_64BIT_REG(i->sibIndex()) << i->sibScale()) + (Bit32s) i->displ32u();
  else
    RMAddr(i) = R15 + (Bit32s) i->displ32u();
}

