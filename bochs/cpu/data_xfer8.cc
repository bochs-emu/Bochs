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
BX_CPU_C::MOV_RLIb(BxInstruction_t *i)
{
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x03].word.byte.rl = i->Ib;
}

  void
BX_CPU_C::MOV_RHIb(BxInstruction_t *i)
{
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x03].word.byte.rh = i->Ib;
}


  void
BX_CPU_C::MOV_EbGb(BxInstruction_t *i)
{
  Bit8u op2;

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* now write op2 to op1 */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, op2);
    }
  else {
    write_virtual_byte(i->seg, i->rm_addr, &op2);
    }
}


  void
BX_CPU_C::MOV_GbEb(BxInstruction_t *i)
{
  Bit8u op2;

  if (i->mod == 0xc0) {
    op2 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2);
    }

  BX_WRITE_8BIT_REG(i->nnn, op2);
}



  void
BX_CPU_C::MOV_ALOb(BxInstruction_t *i)
{
  Bit8u  temp_8;
  Bit32u addr_32;

  addr_32 = i->Id;

  /* read from memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_byte(i->seg, addr_32, &temp_8);
    }
  else {
    read_virtual_byte(BX_SEG_REG_DS, addr_32, &temp_8);
    }


  /* write to register */
  AL = temp_8;
}


  void
BX_CPU_C::MOV_ObAL(BxInstruction_t *i)
{
  Bit8u  temp_8;
  Bit32u addr_32;

  addr_32 = i->Id;

  /* read from register */
  temp_8 = AL;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_byte(i->seg, addr_32, &temp_8);
    }
  else {
    write_virtual_byte(BX_SEG_REG_DS, addr_32, &temp_8);
    }
}


  void
BX_CPU_C::MOV_EbIb(BxInstruction_t *i)
{
  Bit8u op2;

  op2 = i->Ib;

  /* now write op2 back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, op2);
    }
  else {
    write_virtual_byte(i->seg, i->rm_addr, &op2);
    }
}



  void
BX_CPU_C::XLAT(BxInstruction_t *i)
{
  Bit32u offset_32;
  Bit8u  al;


#if BX_CPU_LEVEL >= 3
  if (i->as_32) {
    offset_32 = EBX + AL;
    }
  else
#endif /* BX_CPU_LEVEL >= 3 */
    {
    offset_32 = BX + AL;
    }

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_byte(i->seg, offset_32, &al);
    }
  else {
    read_virtual_byte(BX_SEG_REG_DS, offset_32, &al);
    }
  AL = al;
}

  void
BX_CPU_C::XCHG_EbGb(BxInstruction_t *i)
{
  Bit8u op2, op1;

  /* op2 is a register, op2_addr is an index of a register */
  op2 = BX_READ_8BIT_REG(i->nnn);

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1 = BX_READ_8BIT_REG(i->rm);
    BX_WRITE_8BIT_REG(i->rm, op2);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1);
    write_RMW_virtual_byte(op2);
    }

  BX_WRITE_8BIT_REG(i->nnn, op1);
}
