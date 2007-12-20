/////////////////////////////////////////////////////////////////////////
// $Id: bit16.cc,v 1.2 2007-12-20 20:58:37 sshwarts Exp $
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


#if BX_CPU_LEVEL >= 3

void BX_CPU_C::BSF_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_16 = read_virtual_word(i->seg(), RMAddr(i));
  }

  if (op2_16 == 0) {
    assert_ZF(); /* op1_16 undefined */
    return;
  }

  op1_16 = 0;
  while ( (op2_16 & 0x01) == 0 ) {
    op1_16++;
    op2_16 >>= 1;
  }

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
  clear_ZF();

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}

void BX_CPU_C::BSR_GwEw(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_16 = read_virtual_word(i->seg(), RMAddr(i));
  }

  if (op2_16 == 0) {
    assert_ZF(); /* op1_16 undefined */
    return;
  }

  op1_16 = 15;
  while ( (op2_16 & 0x8000) == 0 ) {
    op1_16--;
    op2_16 <<= 1;
  }

  SET_FLAGS_OSZAPC_LOGIC_16(op1_16);
  clear_ZF();

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}

void BX_CPU_C::BT_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  op1_16 = read_virtual_word(i->seg(), op1_addr);

  set_CF((op1_16 >> index) & 0x01);
}

void BX_CPU_C::BT_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  set_CF((op1_16 >> op2_16) & 0x01);
}

void BX_CPU_C::BTS_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, bit_i, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  op1_16 = read_RMW_virtual_word(i->seg(), op1_addr);

  bit_i = (op1_16 >> index) & 0x01;
  op1_16 |= (((Bit16u) 1) << index);

  write_RMW_virtual_word(op1_16);

  set_CF(bit_i);
}

void BX_CPU_C::BTS_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  set_CF((op1_16 >> op2_16) & 0x01);
  op1_16 |= (((Bit16u) 1) << op2_16);

  /* now write result back to the destination */
  BX_WRITE_16BIT_REG(i->rm(), op1_16);
}

void BX_CPU_C::BTR_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index;
  Bit32s displacement32;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index = op2_16 & 0x0f;
  displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement32;

  /* pointer, segment address pair */
  op1_16 = read_RMW_virtual_word(i->seg(), op1_addr);

  bx_bool temp_cf = (op1_16 >> index) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_word(op1_16);

  set_CF(temp_cf);
}

void BX_CPU_C::BTR_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;
  set_CF((op1_16 >> op2_16) & 0x01);
  op1_16 &= ~(((Bit16u) 1) << op2_16);

  /* now write result back to the destination */
  BX_WRITE_16BIT_REG(i->rm(), op1_16);
}

void BX_CPU_C::BTC_EwGwM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit16u op1_16, op2_16, index_16;
  Bit16s displacement16;

  op2_16 = BX_READ_16BIT_REG(i->nnn());
  index_16 = op2_16 & 0x0f;

  displacement16 = ((Bit16s) (op2_16 & 0xfff0)) / 16;
  op1_addr = RMAddr(i) + 2 * displacement16;
  op1_16 = read_RMW_virtual_word(i->seg(), op1_addr);

  bx_bool temp_CF = (op1_16 >> index_16) & 0x01;
  op1_16 ^= (((Bit16u) 1) << index_16);  /* toggle bit */
  set_CF(temp_CF);

  write_RMW_virtual_word(op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());
  op2_16 &= 0x0f;

  bx_bool temp_CF = (op1_16 >> op2_16) & 0x01;
  op1_16 ^= (((Bit16u) 1) << op2_16);  /* toggle bit */
  set_CF(temp_CF);

  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BT_EwIbM(bxInstruction_c *i)
{
  Bit16u op1_16 = read_virtual_word(i->seg(), RMAddr(i));
  Bit8u  op2_8  = i->Ib() & 0xf;

  set_CF((op1_16 >> op2_8) & 0x01);
}

void BX_CPU_C::BT_EwIbR(bxInstruction_c *i)
{
  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  Bit8u  op2_8  = i->Ib() & 0xf;

  set_CF((op1_16 >> op2_8) & 0x01);
}

void BX_CPU_C::BTS_EwIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 |= (((Bit16u) 1) << op2_8);
  write_RMW_virtual_word(op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTS_EwIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 |= (((Bit16u) 1) << op2_8);
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EwIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 ^= (((Bit16u) 1) << op2_8);  /* toggle bit */
  write_RMW_virtual_word(op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EwIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 ^= (((Bit16u) 1) << op2_8);  /* toggle bit */
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTR_EwIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << op2_8);
  write_RMW_virtual_word(op1_16);

  set_CF(temp_CF);
}

void BX_CPU_C::BTR_EwIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0xf;

  Bit16u op1_16 = BX_READ_16BIT_REG(i->rm());
  bx_bool temp_CF = (op1_16 >> op2_8) & 0x01;
  op1_16 &= ~(((Bit16u) 1) << op2_8);
  BX_WRITE_16BIT_REG(i->rm(), op1_16);

  set_CF(temp_CF);
}

/* 0F B8 */
void BX_CPU_C::POPCNT_GwEw(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit16u op1_16, op2_16;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_16 = read_virtual_word(i->seg(), RMAddr(i));
  }

  op1_16 = 0;
  while (op2_16 != 0) {
    if (op2_16 & 1) op1_16++;
    op2_16 >>= 1;
  }

  Bit32u flags = op1_16 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
#else
  BX_INFO(("POPCNT_GwEw: required POPCNT support, use --enable-popcnt option"));
  UndefinedOpcode(i);
#endif
}

#endif // (BX_CPU_LEVEL >= 3)
