/////////////////////////////////////////////////////////////////////////
// $Id: bcd.cc,v 1.9 2004-03-10 20:14:56 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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
BX_CPU_C::DAS(bxInstruction_c *)
{
  /* The algorithm for DAS is fashioned after the pseudo code in the
   * Pentium Processor Family Developer's Manual, volume 3.  It seems
   * to have changed from earlier processor's manuals.  I'm not sure
   * if this is a correction in the algorithm printed, or Intel has
   * changed the handling of instruction. Validated against Intel 
   * Pentium family hardware.
   */

  Bit8u tmpAL = AL;
  int tmpCF = 0;

  /* DAS effect the following flags: A,C,S,Z,P */

  if (((tmpAL & 0x0F) > 0x09) || get_AF())
  {
    tmpCF = (AL < 0x06) || get_CF();
    AL = AL - 0x06;
    set_AF(1);
  }
  else
    set_AF(0);

  if ((tmpAL > 0x99) || get_CF())
  {
    AL = AL - 0x60;
    tmpCF = 1;
  }
  else
    tmpCF = 0;

  set_CF(tmpCF);
  set_SF(AL >> 7);
  set_ZF(AL==0);
  set_PF_base(AL);
}

  void
BX_CPU_C::AAA(bxInstruction_c *)
{
  /* 
   *  Note: This istruction incorrectly documented in Intel's materials.
   *        The right description is:
   *
   *    IF (((AL and 0FH) > 9) or (AF==1)
   *    THEN {
   *        IF CPU<286 THEN {  AL <- AL+6 }
   *                   ELSE {  AX <- AX+6 }
   *        AH <- AH+1
   *        CF <- 1
   *        AF <- 1
   *    } ELSE {
   *        CF <- 0
   *        AF <- 0
   *	}
   *	AL <- AL and 0Fh
   */	

  /* Validated against Intel Pentium family hardware. */

  /* AAA affects the following flags: A,C */

  if ( ((AL & 0x0f) > 9) || get_AF() )
  {
    AX = AX + 6;
    AH = AH + 1;
    set_AF(1);
    set_CF(1);
  }
  else {
    set_AF(0);
    set_CF(0);
  }

  AL = AL & 0x0f;
}

  void
BX_CPU_C::AAS(bxInstruction_c *)
{
  /* AAS affects the following flags: A,C */

  if ( ((AL & 0x0F) > 0x09) || get_AF() )
  {
    AL = AL - 6;	/* never borrow */
    AH = AH - 1;
    set_AF(1);
    set_CF(1);
  }
  else {
    set_CF(0);
    set_AF(0);
  }

  AL = AL & 0x0f;
}

  void
BX_CPU_C::AAM(bxInstruction_c *i)
{
  Bit8u al, imm8;

  imm8 = i->Ib();

  if (imm8 == 0)
    exception(BX_DE_EXCEPTION, 0, 0);

  al = AL;
  AH = al / imm8;
  AL = al % imm8;

  /* AAM always clears the flags A and C */
  set_AF(0);
  set_CF(0);

  /* AAM affects the following flags: S,Z,P */
  set_SF((AL & 0x80) > 0);
  set_ZF(AL == 0);
  set_PF_base(AL);
}

  void
BX_CPU_C::AAD(bxInstruction_c *i)
{
  Bit8u al, imm8;
  Bit16u ax1, ax2;

  imm8 = i->Ib();

  ax1 = AH * imm8;
  ax2 = ax1 + AL;
  al = AL;
  AL = (Bit8u)(ax2 & 0xFF);
  AH = 0;

  /* AAD effects the following flags: A,C,O,S,Z,P */
  /* modification of flags A,C,O is undocumented */
  set_AF((ax1 & 0x08) != (ax2 & 0x08));
  set_CF(ax2 > 0xff);
  set_OF((AL & 0x80) != (al & 0x80));
  set_SF(AL >= 0x80);
  set_ZF(AL == 0);
  set_PF_base(AL);
}

  void
BX_CPU_C::DAA(bxInstruction_c *)
{
  Bit8u tmpAL = AL;
  int   tmpCF = 0;

  /* Validated against Intel Pentium family hardware. */

  // DAA affects the following flags: S,Z,A,P,C

  if (((tmpAL & 0x0F) > 0x09) || get_AF())
  {
    tmpCF = ((AL > 0xF9) || get_CF());
    AL = AL + 0x06;
    set_AF(1);
  }
  else
    set_AF(0);

  if ((tmpAL > 0x99) || get_CF())
  {
    AL = AL + 0x60;
    tmpCF = 1;
  }
  else
    tmpCF = 0;

  set_CF(tmpCF);
  set_SF(AL >> 7);
  set_ZF(AL==0);
  set_PF_base(AL);
}
