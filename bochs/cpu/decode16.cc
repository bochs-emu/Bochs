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





#define BX_IN_CPU_METHOD 1
#include "bochs.h"



static Bit16u *aaa[8] = {
  & BX,
  & BX,
  & BP,
  & BP,
  & SI,
  & DI,
  & BP,
  & BX,
  };

static Bit16u *bbb[8] = {
  & SI,
  & DI,
  & SI,
  & DI,
  (Bit16u *) & BX_CPU_THIS_PTR empty_register,
  (Bit16u *) & BX_CPU_THIS_PTR empty_register,
  (Bit16u *) & BX_CPU_THIS_PTR empty_register,
  (Bit16u *) & BX_CPU_THIS_PTR empty_register
  };




  void
BX_CPU_C::decode_exgx16(unsigned modrm)
{
  Bit8u  displ8;
  Bit16u displ16;
  unsigned mod, rm;

#if BX_WEIRDISMS
    i->seg_reg = NULL;
#endif

  // |  76 | 543 | 210
  // | mod | ttt |  rm

  BX_INSTR_MODRM16(modrm);
  i->nnn = (modrm>>3) & 0x07;
  mod = modrm & 0xc0;
  rm = modrm & 0x07;

  if (mod == 0xc0) {
    i->rm_addr = rm;
    BX_CPU_THIS_PTR rm_type = BX_REGISTER_REF;
    return;
    }
  else { // mod != 3
    BX_CPU_THIS_PTR rm_type = BX_MEMORY_REF;

    if (mod == 0x40) {
      displ8 = fetch_next_byte();
      i->rm_addr = (Bit16u) (*aaa[rm] + *bbb[rm] + (Bit8s) displ8);
      if (i->seg_reg == NULL)
        i->seg_reg = BX_CPU_THIS_PTR sreg_mod01_rm16[rm];
      else
        i->seg_reg = i->seg_reg;
      return;
      }
    if (mod == 0x80) {
      displ16 = fetch_next_word();
      i->rm_addr = (Bit16u) (*aaa[rm] + *bbb[rm] + (Bit16s) displ16);
      if (i->seg_reg == NULL)
        i->seg_reg = BX_CPU_THIS_PTR sreg_mod10_rm16[rm];
      else
        i->seg_reg = i->seg_reg;
      return;
      }

    // mod == 0x00
    if (rm==6)
      i->rm_addr = fetch_next_word();
    else
      i->rm_addr = (Bit16u) (*aaa[rm] + *bbb[rm]);

    if (i->seg_reg == NULL)
      i->seg_reg = BX_CPU_THIS_PTR sreg_mod00_rm16[rm];
    else
      i->seg_reg = i->seg_reg;
    return;
    }
}
