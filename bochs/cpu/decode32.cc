/////////////////////////////////////////////////////////////////////////
// $Id: decode32.cc,v 1.4.4.1 2002-03-17 08:57:01 bdenney Exp $
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
BX_CPU_C::decode_exgx32(unsigned modrm)
{
  unsigned  mod, rm, ss;
  unsigned sib, base, index;
  Bit32u displ32, index_reg_val, base_reg_val;
  Bit8u  displ8;

  /* NOTES:
   * seg_reg_mod01_base & mod10_base aren't correct???
   */
  /* use 32bit addressing modes.  orthogonal base & index registers,
     scaling available, etc. */

  BX_INSTR_MODRM32(modrm);

  mod = modrm & 0xc0;
  rm  = modrm & 0x07;

  i->nnn = (modrm>>3) & 0x07;

  if (mod == 0xc0) { /* mod, reg, reg */
    i->rm_addr = rm;
    BX_CPU_THIS_PTR rm_type = BX_REGISTER_REF;
#if BX_WEIRDISMS
    i->seg_reg = NULL;
#endif
    }
  else { /* mod != 3 */
    BX_CPU_THIS_PTR rm_type = BX_MEMORY_REF;
    if (rm != 4) { /* rm != 100b, no s-i-b byte */
      // one byte modrm
      if (mod == 0x00) {
        if (i->seg_reg)
          i->seg_reg = i->seg_reg;
        else
          i->seg_reg = & BX_CPU_THIS_PTR ds;
        if (rm == 5) { // no reg, 32-bit displacement
          i->rm_addr = fetch_next_dword();
          }
        else {
          // else reg indirect, no displacement
          i->rm_addr = BX_READ_32BIT_REG(rm);
          }
        return;
        }
      if (mod == 0x40) {
        if (i->seg_reg)
          i->seg_reg = i->seg_reg;
        else
          i->seg_reg = BX_CPU_THIS_PTR sreg_mod01_rm32[rm];
        // reg, 8-bit displacement, sign extend
        displ8 = fetch_next_byte();
        i->rm_addr = BX_READ_32BIT_REG(rm);
        i->rm_addr += ((Bit8s) displ8);
        return;
        }
      // mod == 0x80
      if (i->seg_reg)
        i->seg_reg = i->seg_reg;
      else
        i->seg_reg = BX_CPU_THIS_PTR sreg_mod10_rm32[rm];
      // reg, 32-bit displacement
      displ32 = fetch_next_dword();
      i->rm_addr = BX_READ_32BIT_REG(rm);
      i->rm_addr += displ32;
      return;
      }
    else { /* rm == 4, s-i-b byte follows */
      sib = fetch_next_byte();
      BX_INSTR_SIB32(sib);
      base  = sib & 0x07; sib >>= 3;
      index = sib & 0x07; sib >>= 3;
      ss    = sib;

      if (mod == 0x00) {
        if (i->seg_reg)
          i->seg_reg = i->seg_reg;
        else
          i->seg_reg = BX_CPU_THIS_PTR sreg_mod00_base32[base];
        if (base != 5) /* base != 101b, no displacement */
          base_reg_val = BX_READ_32BIT_REG(base);
        else {
          BX_INSTR_SIB_mod0_base5(ss);
          base_reg_val = fetch_next_dword();
          }
        index_reg_val = 0;
        if (index != 4) {
          index_reg_val = BX_READ_32BIT_REG(index);
          index_reg_val <<= ss;
          }
#ifdef BX_INSTR_SIB_MOD0_IND4
        else {
          BX_INSTR_SIB_MOD0_IND4();
          }
#endif
        i->rm_addr = base_reg_val + index_reg_val;
        return;
        }
      if (mod == 0x40) {
        if (i->seg_reg)
          i->seg_reg = i->seg_reg;
        else
          i->seg_reg = BX_CPU_THIS_PTR sreg_mod01_base32[base];
        displ8 = fetch_next_byte();
        base_reg_val = BX_READ_32BIT_REG(base);
        index_reg_val = 0;
        if (index != 4) {
          index_reg_val = BX_READ_32BIT_REG(index);
          index_reg_val <<= ss;
          }
#ifdef BX_INSTR_SIB_MOD1_IND4
        else {
          BX_INSTR_SIB_MOD1_IND4();
          }
#endif
        i->rm_addr = base_reg_val + index_reg_val + (Bit8s) displ8;
        return;
        }

      // mod == 0x80
      if (i->seg_reg)
        i->seg_reg = i->seg_reg;
      else
        i->seg_reg = BX_CPU_THIS_PTR sreg_mod10_base32[base];
      displ32 = fetch_next_dword();
      base_reg_val = BX_READ_32BIT_REG(base);
      index_reg_val = 0;
      if (index != 4) {
        index_reg_val = BX_READ_32BIT_REG(index);
        index_reg_val <<= ss;
        }
#ifdef BX_INSTR_SIB_MOD2_IND4
      else {
        BX_INSTR_SIB_MOD2_IND4();
        }
#endif
      i->rm_addr = base_reg_val + index_reg_val + displ32;
      return;
      }
    } /* if (mod != 3) */
}
