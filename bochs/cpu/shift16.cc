/////////////////////////////////////////////////////////////////////////
// $Id: shift16.cc,v 1.27 2005-10-13 19:28:10 sshwarts Exp $
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


void BX_CPU_C::SHLD_EwGw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;

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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (!count) return;
  // count is 1..31

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  temp_32 = ((Bit32u)(op1_16) << 16) | (op2_16); // double formed by op1:op2
  result_32 = temp_32 << count;
  // Hack to act like x86 SHLD when count > 16
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
    Write_RMW_virtual_word(result_16);
  }

  /* set eflags:
   * SHLD count affects the following flags: O,S,Z,A,P,C
   */
  SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SHL16);
}

void BX_CPU_C::SHRD_EwGw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16, result_16;
  Bit32u temp_32, result_32;
  unsigned count;

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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (!count) return;
  // count is 1..31

  op2_16 = BX_READ_16BIT_REG(i->nnn());

  // Hack to act like x86 SHLD when count > 16
  temp_32 = (op2_16 << 16) | op1_16; // double formed by op2:op1
  result_32 = temp_32 >> count;
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
    Write_RMW_virtual_word(result_16);
  }

  /* set eflags:
   * SHRD count affects the following flags: S,Z,P,C,O
   */

  set_CF((op1_16 >> (count - 1)) & 0x01);
  set_ZF(result_16 == 0);
  set_SF(result_16 >> 15);
  set_AF(0);
  /* for shift of 1, OF set if sign change occurred. */
  if (count == 1)
    set_OF(((op1_16 ^ result_16) & 0x8000) > 0);
  set_PF_base((Bit8u) result_16);
}

void BX_CPU_C::ROL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if ( (count & 0x0f) == 0 ) {
    if ( count & 0x10 ) {
      unsigned bit0 = op1_16 & 1;
      set_CF(bit0);
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
    Write_RMW_virtual_word(result_16);
  }

  /* set eflags:
   * ROL count affects the following flags: C, O
   */
  bx_bool temp_CF = (result_16 & 0x01);

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_16 >> 15));
}

void BX_CPU_C::ROR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
    count = 1;
  else // 0xd3
    count = CL;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if ( (count & 0x0f) == 0 ) {
    if ( count & 0x10 ) {
      unsigned MSB = op1_16 >> 15;
      set_CF(MSB);
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
    Write_RMW_virtual_word(result_16);
  }
  
  /* set eflags:
   * ROR count affects the following flags: C, O
   */
  bx_bool result_b15 = (result_16 & 0x8000) != 0;
  bx_bool result_b14 = (result_16 & 0x4000) != 0;

  set_CF(result_b15);
  set_OF(result_b15 ^ result_b14);
}

void BX_CPU_C::RCL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
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
    Write_RMW_virtual_word(result_16);
  }

  /* set eflags:
   * RCL count affects the following flags: C, O
   */
  bx_bool temp_CF = (op1_16 >> (16 - count)) & 0x01;

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_16 >> 15));
}

void BX_CPU_C::RCR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (! count) return;

  result_16 = (op1_16 >> count) |
          (getB_CF() << (16 - count)) |
          (op1_16 << (17 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    Write_RMW_virtual_word(result_16);
  }

  /* set eflags:
   * RCR count affects the following flags: C, O
   */

  set_CF((op1_16 >> (count - 1)) & 0x01);
  set_OF((((result_16 << 1) ^ result_16) & 0x8000) > 0);
}

void BX_CPU_C::SHL_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (!count) return;

  result_16 = (op1_16 << count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    Write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SHL16);
}

void BX_CPU_C::SHR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (!count) return;

#if defined(BX_HostAsm_Shr16)
  Bit32u flags32;
  asmShr16(result_16, op1_16, count, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result_16 = (op1_16 >> count);
  SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SHR16);
#endif

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    Write_RMW_virtual_word(result_16);
  }
}

void BX_CPU_C::SAR_Ew(bxInstruction_c *i)
{
  Bit16u op1_16, result_16;
  unsigned count;

  if ( i->b1() == 0xc1 )
    count = i->Ib();
  else if ( i->b1() == 0xd1 )
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
    read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
  }

  if (!count) return;

  if (count < 16) {
    if (op1_16 & 0x8000) {
      result_16 = (op1_16 >> count) | (0xffff << (16 - count));
    }
    else {
      result_16 = (op1_16 >> count);
    }
  }
  else {
    if (op1_16 & 0x8000) {
      result_16 = 0xffff;
    }
    else {
      result_16 = 0;
    }
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), result_16);
  }
  else {
    Write_RMW_virtual_word(result_16);
  }

  SET_FLAGS_OSZAPC_16(op1_16, count, result_16, BX_INSTR_SAR16);
}
