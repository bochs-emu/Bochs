/////////////////////////////////////////////////////////////////////////
// $Id: resolve64.cc,v 1.1 2002-09-13 15:53:22 kevinlawton Exp $
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
BX_CPU_C::Resolve64Mod0Rm0(BxInstruction_t *i)
{
  i->rm_addr = RAX;
}
  void
BX_CPU_C::Resolve64Mod0Rm1(BxInstruction_t *i)
{
  i->rm_addr = RCX;
}
  void
BX_CPU_C::Resolve64Mod0Rm2(BxInstruction_t *i)
{
  i->rm_addr = RDX;
}
  void
BX_CPU_C::Resolve64Mod0Rm3(BxInstruction_t *i)
{
  i->rm_addr = RBX;
}
  void
BX_CPU_C::Resolve64Mod0Rm5(BxInstruction_t *i)
{
  // eip hasn't been bumped yet when this is called.  must choose the saved value.
  i->rm_addr = BX_CPU_THIS_PTR prev_eip + i->ilen + (Bit32s)i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod0Rm6(BxInstruction_t *i)
{
  i->rm_addr = RSI;
}
  void
BX_CPU_C::Resolve64Mod0Rm7(BxInstruction_t *i)
{
  i->rm_addr = RDI;
}
  void
BX_CPU_C::Resolve64Mod0Rm8(BxInstruction_t *i)
{
  i->rm_addr = R8;
}
  void
BX_CPU_C::Resolve64Mod0Rm9(BxInstruction_t *i)
{
  i->rm_addr = R9;
}
  void
BX_CPU_C::Resolve64Mod0Rm10(BxInstruction_t *i)
{
  i->rm_addr = R10;
}
  void
BX_CPU_C::Resolve64Mod0Rm11(BxInstruction_t *i)
{
  i->rm_addr = R11;
}
  void
BX_CPU_C::Resolve64Mod0Rm12(BxInstruction_t *i)
{
  i->rm_addr = R12;
}
  void
BX_CPU_C::Resolve64Mod0Rm13(BxInstruction_t *i)
{
  i->rm_addr = R13;
}

  void
BX_CPU_C::Resolve64Mod0Rm14(BxInstruction_t *i)
{
  i->rm_addr = R14;
}

  void
BX_CPU_C::Resolve64Mod0Rm15(BxInstruction_t *i)
{
  i->rm_addr = R15;
}


  void
BX_CPU_C::Resolve64Mod1or2Rm0(BxInstruction_t *i)
{
  i->rm_addr = RAX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm1(BxInstruction_t *i)
{
  i->rm_addr = RCX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm2(BxInstruction_t *i)
{
  i->rm_addr = RDX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm3(BxInstruction_t *i)
{
  i->rm_addr = RBX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm5(BxInstruction_t *i)
{
  i->rm_addr = RBP + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm6(BxInstruction_t *i)
{
  i->rm_addr = RSI + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm7(BxInstruction_t *i)
{
  i->rm_addr = RDI + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm8(BxInstruction_t *i)
{
  i->rm_addr = R8 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm9(BxInstruction_t *i)
{
  i->rm_addr = R9 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm10(BxInstruction_t *i)
{
  i->rm_addr = R10 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm11(BxInstruction_t *i)
{
  i->rm_addr = R11 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm12(BxInstruction_t *i)
{
  i->rm_addr = R12 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm13(BxInstruction_t *i)
{
  i->rm_addr = R13 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm14(BxInstruction_t *i)
{
  i->rm_addr = R14 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Rm15(BxInstruction_t *i)
{
  i->rm_addr = R15 + (Bit32s) i->displ32u;
}



  void
BX_CPU_C::Resolve64Mod0Base0(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RAX + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RAX;
}
  void
BX_CPU_C::Resolve64Mod0Base1(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RCX + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RCX;
}
  void
BX_CPU_C::Resolve64Mod0Base2(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RDX + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RDX;
}
  void
BX_CPU_C::Resolve64Mod0Base3(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RBX + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RBX;
}
  void
BX_CPU_C::Resolve64Mod0Base4(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RSP + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RSP;
}
  void
BX_CPU_C::Resolve64Mod0Base5(BxInstruction_t *i)
{
  if (i->index != 4) {
    i->rm_addr = (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
    }
  else
    i->rm_addr = (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod0Base6(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RSI + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RSI;
}
  void
BX_CPU_C::Resolve64Mod0Base7(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RDI + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = RDI;
}
  void
BX_CPU_C::Resolve64Mod0Base8(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R8 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R8;
}
  void
BX_CPU_C::Resolve64Mod0Base9(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R9 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R9;
}
  void
BX_CPU_C::Resolve64Mod0Base10(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R10 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R10;
}
  void
BX_CPU_C::Resolve64Mod0Base11(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R11 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R11;
}
  void
BX_CPU_C::Resolve64Mod0Base12(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R12 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R12;
}
  void
BX_CPU_C::Resolve64Mod0Base13(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R13 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R13;
}
  void
BX_CPU_C::Resolve64Mod0Base14(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R14 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R14;
}
  void
BX_CPU_C::Resolve64Mod0Base15(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R15 + (BX_READ_64BIT_REG(i->index) << i->scale);
  else
    i->rm_addr = R15;
}




  void
BX_CPU_C::Resolve64Mod1or2Base0(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RAX + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RAX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base1(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RCX + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RCX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base2(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RDX + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RDX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base3(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RBX + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RBX + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base4(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RSP + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RSP + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base5(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RBP + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RBP + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base6(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RSI + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RSI + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base7(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = RDI + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = RDI + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base8(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R8 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R8 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base9(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R9 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R9 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base10(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R10 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R10 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base11(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R11 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R11 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base12(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R12 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R12 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base13(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R13 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R13 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base14(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R14 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R14 + (Bit32s) i->displ32u;
}
  void
BX_CPU_C::Resolve64Mod1or2Base15(BxInstruction_t *i)
{
  if (i->index != 4)
    i->rm_addr = R15 + (BX_READ_64BIT_REG(i->index) << i->scale) + (Bit32s) i->displ32u;
  else
    i->rm_addr = R15 + (Bit32s) i->displ32u;
}

