/////////////////////////////////////////////////////////////////////////
// $Id: shift8.cc,v 1.4.4.1 2002-03-17 08:57:02 bdenney Exp $
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
BX_CPU_C::ROL_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x07; // use only lowest 3 bits

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (count) {
    result_8 = (op1_8 << count) | (op1_8 >> (8 - count));

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_8BIT_REG(i->rm, result_8);
      }
    else {
      write_RMW_virtual_byte(result_8);
      }

    /* set eflags:
     * ROL count affects the following flags: C
     */

    set_CF(result_8 & 0x01);
    if (count == 1)
      set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}




  void
BX_CPU_C::ROR_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  Bit8u result_b7;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;


  count &= 0x07; /* use only bottom 3 bits */

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (count) {
    result_8 = (op1_8 >> count) | (op1_8 << (8 - count));

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_8BIT_REG(i->rm, result_8);
      }
    else {
      write_RMW_virtual_byte(result_8);
      }

    /* set eflags:
     * ROR count affects the following flags: C
     */
    result_b7 = result_8 & 0x80;

    set_CF(result_b7 != 0);
    if (count == 1)
      set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}



  void
BX_CPU_C::RCL_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count = (count & 0x1F) % 9;


  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (count) {
    result_8 = (op1_8 << count) |
             (get_CF() << (count - 1)) |
             (op1_8 >> (9 - count));

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_8BIT_REG(i->rm, result_8);
      }
    else {
      write_RMW_virtual_byte(result_8);
      }

    /* set eflags:
     * RCL count affects the following flags: C
     */
    if (count == 1)
      set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    set_CF((op1_8 >> (8 - count)) & 0x01);
    }
}



  void
BX_CPU_C::RCR_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count = ( count & 0x1F ) % 9;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (count) {
    result_8 = (op1_8 >> count) |
             (get_CF() << (8 - count)) |
             (op1_8 << (9 - count));

    /* now write result back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_8BIT_REG(i->rm, result_8);
      }
    else {
      write_RMW_virtual_byte(result_8);
      }

    /* set eflags:
     * RCR count affects the following flags: C
     */

    set_CF((op1_8 >> (count - 1)) & 0x01);
    if (count == 1)
      set_OF(((op1_8 ^ result_8) & 0x80) > 0);
    }
}




  void
BX_CPU_C::SHL_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1F;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (!count) return;

  result_8 = (op1_8 << count);

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result_8);
    }
  else {
    write_RMW_virtual_byte(result_8);
    }

  SET_FLAGS_OSZAPC_8(op1_8, count, result_8, BX_INSTR_SHL8);
}



  void
BX_CPU_C::SHR_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1F;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (!count) return;

  result_8 = (op1_8 >> count);

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result_8);
    }
  else {
    write_RMW_virtual_byte(result_8);
    }

  SET_FLAGS_OSZAPC_8(op1_8, count, result_8, BX_INSTR_SHR8);
}




  void
BX_CPU_C::SAR_Eb(BxInstruction_t *i)
{
  Bit8u op1_8, result_8;
  unsigned count;

  if (i->b1 == 0xc0)
    count = i->Ib;
  else if (i->b1 == 0xd0)
    count = 1;
  else // 0xd2
    count = CL;

  count &= 0x1F;

  /* op1 is a register or memory reference */
  if (i->mod == 0xc0) {
    op1_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_byte(i->seg, i->rm_addr, &op1_8);
    }

  if (!count) return;

  if (count < 8) {
    if (op1_8 & 0x80) {
      result_8 = (op1_8 >> count) | (0xff << (8 - count));
      }
    else {
      result_8 = (op1_8 >> count);
      }
    }
  else {
    if (op1_8 & 0x80) {
      result_8 = 0xff;
      }
    else {
      result_8 = 0;
      }
    }

  /* now write result back to destination */
  if (i->mod == 0xc0) {
    BX_WRITE_8BIT_REG(i->rm, result_8);
    }
  else {
    write_RMW_virtual_byte(result_8);
    }

  /* set eflags:
   * SAR count affects the following flags: S,Z,P,C
   */

  if (count < 8) {
    set_CF((op1_8 >> (count - 1)) & 0x01);
    }
  else {
    if (op1_8 & 0x80) {
      set_CF(1);
      }
    else {
      set_CF(0);
      }
    }

  set_ZF(result_8 == 0);
  set_SF(result_8 >> 7);
  if (count == 1)
    set_OF(0);
  set_PF_base(result_8);
}
