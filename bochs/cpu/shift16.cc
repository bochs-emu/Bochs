/////////////////////////////////////////////////////////////////////////
// $Id: shift16.cc,v 1.39 2007-12-20 20:58:37 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


void BX_CPU_C::SHLD_EwGw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned of, cf;

  /* op1:op2 << count.  result stored in op1 */
  if (i->b1() == 0x1a4)
    count = i->Ib();
  else // 0x1a5
    count = CL;

  count &= 0x1f; // use only 5 LSB's

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  /* count < 32, since only lower 5 bits used */
  temp_32 = ((Bit32u)(op1_16) << 16) | (op2_16); // double formed by op1:op2
  result_32 = temp_32 << count;

  // hack to act like x86 SHLD when count > 16
  if (count > 16) {
    // when count > 16 actually shifting op1:op2:op2 << count,
    // it is the same as shifting op2:op2 by count-16
    result_32 |= (op1_16 << (count - 16));
  }

  result_16 = result_32 >> 16;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_LOGIC_16(result_16); /* handle SF, ZF and AF flags */

  cf = (temp_32 >> (32 - count)) & 0x1;
  of = cf ^ (result_16 >> 15); // of = cf ^ result15
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::SHRD_EwGw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;
  unsigned cf, of;

  if (i->b1() == 0x1ac)
    count = i->Ib();
  else // 0x1ad
    count = CL;

  count &= 0x1f; /* use only 5 LSB's */

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  /* count < 32, since only lower 5 bits used */
  temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
  result_32 = temp_32 >> count;

  // hack to act like x86 SHRD when count > 16
  if (count > 16) {
    // when count > 16 actually shifting op2:op2:op1 >> count,
    // it is the same as shifting op2:op2 by count-16
    result_32 |= (op1_16 << (32 - count));
  }
  
  result_16 = result_32;

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_LOGIC_16(result_16); /* handle SF, ZF and AF flags */

  cf = (op1_16 >> (count - 1)) & 0x1;
  of = ((result_16 << 1) ^ result_16) >> 15; // of = result14 ^ result15
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::ROL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned bit0, bit15;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit0  = (op1_16 & 0x1);
      bit15 = (op1_16 >> 15);
      // of = cf ^ result15
      SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
    }
    return;
  }

  count &= 0x0f; // only use bottom 4 bits

  result_16 = (op1_16 << count) | (op1_16 >> (16 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  bit0  = (result_16 & 0x1);
  bit15 = (result_16 >> 15);
  // of = cf ^ result15
  SET_FLAGS_OxxxxC(bit0 ^ bit15, bit0);
}

void BX_CPU_C::ROR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned bit14, bit15;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if ((count & 0x0f) == 0) {
    if (count & 0x10) {
      bit14 = (op1_16 >> 14) & 1;
      bit15 = (op1_16 >> 15) & 1;
      // of = result14 ^ result15 
      SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
    }
    return;
  }

  count &= 0x0f;  // use only 4 LSB's

  result_16 = (op1_16 >> count) | (op1_16 << (16 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }
  
  bit14 = (result_16 >> 14) & 1;
  bit15 = (result_16 >> 15) & 1;
  // of = result14 ^ result15 
  SET_FLAGS_OxxxxC(bit14 ^ bit15, bit15);
}

void BX_CPU_C::RCL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned of, cf;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  count = (count & 0x1f) % 17;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  if (count==1) {
    result_16 = (op1_16 << 1) | getB_CF();
  }
  else if (count==16) {
    result_16 = (getB_CF() << 15) | (op1_16 >> 1);
  }
  else { // 2..15
    result_16 = (op1_16 << count) | (getB_CF() << (count - 1)) |
                (op1_16 >> (17 - count));
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  cf = (op1_16 >> (16 - count)) & 0x1;
  of = cf ^ (result_16 >> 15); // of = cf ^ result15
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::RCR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned of, cf;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  count = (count & 0x1f) % 17;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (! count) return;

  result_16 = (op1_16 >> count) | (getB_CF() << (16 - count)) |
              (op1_16 << (17 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  cf = (op1_16 >> (count - 1)) & 0x1;
  of = ((result_16 << 1) ^ result_16) >> 15; // of = result15 ^ result14
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::SHL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned of = 0, cf = 0;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1f; /* use only 5 LSB's */

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  if (count <= 16) {
    result_16 = (op1_16 << count);
    cf = (op1_16 >> (16 - count)) & 0x1;
    of = cf ^ (result_16 >> 15); // of = cf ^ result15
  }
  else {
    result_16 = 0;
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_LOGIC_16(result_16); /* handle SF, ZF and AF flags */
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::SHR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;
  unsigned of, cf;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1f; /* use only 5 LSB's */

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  result_16 = (op1_16 >> count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }

  cf = (op1_16 >> (count - 1)) & 0x1;
  // note, that of == result15 if count == 1 and
  //            of == 0        if count >= 2
  of = ((result_16 << 1) ^ result_16) >> 15;

  SET_FLAGS_OSZAPC_LOGIC_16(result_16); /* handle SF, ZF and AF flags */
  SET_FLAGS_OxxxxC(of, cf);
}

void BX_CPU_C::SAR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count, cf;

  if (i->b1() == 0xc1)
    count = i->Ib();
  else if (i->b1() == 0xd1)
    count = 1;
  else // 0xd3
    count = CL;

  count &= 0x1f;  /* use only 5 LSB's */

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  }

  if (!count) return;

  if (count < 16) {
    if (op1_16 & 0x8000) {
      result_16 = (op1_16 >> count) | (0xffff << (16 - count));
    }
    else {
      result_16 = (op1_16 >> count);
    }

    cf = (op1_16 >> (count - 1)) & 0x1;
  }
  else {
    if (op1_16 & 0x8000) {
      result_16 = 0xffff;
    }
    else {
      result_16 = 0;
    }

    cf = (result_16 & 0x1);
  }

  SET_FLAGS_OSZAPC_LOGIC_16(result_16); /* handle SF, ZF and AF flags */
  /* signed overflow cannot happen in SAR instruction */
  SET_FLAGS_OxxxxC(0, cf); 

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    write_RMW_virtual_word(result_16);
  }
}
