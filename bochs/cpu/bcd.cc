//  Copyright (C) 2000  MandrakeSoft S.A.
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
BX_CPU_C::DAS(BxInstruction_t *)
{
  Bit8u tmpCF, tmpAL;

  /* ??? */
  /* the algorithm for DAS is fashioned after the pseudo code in the
   * Pentium Processor Family Developer's Manual, volume 3.  It seems
   * to have changed from earlier processor's manuals.  I'm not sure
   * if this is a correction in the algorithm printed, or Intel has
   * changed the handling of instruction.  It might not even be
   * correct yet...
   */

  tmpCF = 0;
  tmpAL = AL;

  /* DAS effect the following flags: A,C,S,Z,P */

  if (((tmpAL & 0x0F) > 0x09) || get_AF()) {
    set_AF(1);
    tmpCF = (AL < 0x06) || get_CF();
    AL = AL - 0x06;
    /*tmpCF = (AL < 0) || CF;*/
    }
  if ( (tmpAL > 0x99) || get_CF() ) {
    AL = AL - 0x60;
    tmpCF = 1;
    }

  set_CF(tmpCF);
  set_SF(AL >> 7);
  set_ZF(AL==0);
  set_PF_base(AL);
}

  void
BX_CPU_C::AAA(BxInstruction_t *)
{
  Bit8u ALcarry;

  ALcarry = AL > 0xf9;

  /* AAA effects the following flags: A,C */
  if ( ((AL & 0x0f) > 9) || get_AF() ) {
    AL = (AL + 6) & 0x0f;
    AH = AH + 1 + ALcarry;
    set_AF(1);
    set_CF(1);
    }
  else {
    set_AF(0);
    set_CF(0);
    AL = AL & 0x0f;
    }
}

  void
BX_CPU_C::AAS(BxInstruction_t *)
{
  Bit8u ALborrow;

  /* AAS affects the following flags: A,C */

  ALborrow = AL < 6;

  if ( ((AL & 0x0F) > 0x09) || get_AF() ) {
    AL = (AL - 6) & 0x0f;
    AH = AH - 1 - ALborrow;
    set_AF(1);
    set_CF(1);
    }
  else {
    set_CF(0);
    set_AF(0);
    AL = AL & 0x0f;
    }
}

  void
BX_CPU_C::AAM(BxInstruction_t *i)
{
  Bit8u al, imm8;

  imm8 = i->Ib;

  al = AL;
  AH = al / imm8;
  AL = al % imm8;

  /* AAM affects the following flags: S,Z,P */
  set_SF((AH & 0x80) > 0);
  set_ZF(AX==0);
  set_PF_base(AL); /* ??? */
}

  void
BX_CPU_C::AAD(BxInstruction_t *i)
{
  Bit8u imm8;

  imm8 = i->Ib;

  AL = AH * imm8 + AL;
  AH = 0;

  /* AAD effects the following flags: S,Z,P */
  set_SF(AL >= 0x80);
  set_ZF(AL == 0);
  set_PF_base(AL);
}

  void
BX_CPU_C::DAA(BxInstruction_t *)
{
  Bit8u al;

  al = AL;

  // DAA affects the following flags: S,Z,A,P,C
  // ???

  if (((al & 0x0F) > 0x09) || get_AF()) {
    al = al + 0x06;
    set_AF(1);
    }
  else
    set_AF(0);

  if ((al > 0x9F) || get_CF()) {
    al = al + 0x60;
    set_CF(1);
    }

  AL = al;

  set_SF(al >> 7);
  set_ZF(al==0);
  set_PF_base(al);
}
