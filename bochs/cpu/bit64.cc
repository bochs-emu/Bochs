/////////////////////////////////////////////////////////////////////////
// $Id: bit64.cc,v 1.2 2007-12-20 20:58:37 sshwarts Exp $
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

#if BX_SUPPORT_X86_64

void BX_CPU_C::BSF_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64;

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_64 = read_virtual_qword(i->seg(), RMAddr(i));
  }

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
    return;
  }

  op1_64 = 0;
  while ( (op2_64 & 0x01) == 0 ) {
    op1_64++;
    op2_64 >>= 1;
  }

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);
  clear_ZF();

  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}

void BX_CPU_C::BSR_GqEq(bxInstruction_c *i)
{
  /* for 64 bit operand size mode */
  Bit64u op1_64, op2_64;

  /* op2_64 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_64 = read_virtual_qword(i->seg(), RMAddr(i));
  }

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
    return;
  }

  op1_64 = 63;
  while ( (op2_64 & BX_CONST64(0x8000000000000000)) == 0 ) {
    op1_64--;
    op2_64 <<= 1;
  }

  SET_FLAGS_OSZAPC_LOGIC_64(op1_64);
  clear_ZF();
  
  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}

void BX_CPU_C::BT_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  op1_64 = read_virtual_qword(i->seg(), op1_addr);

  set_CF((op1_64 >> index) & 0x01);
}

void BX_CPU_C::BT_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);
}

void BX_CPU_C::BTS_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;
  Bit64u bit_i;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword(i->seg(), op1_addr);

  bit_i = (op1_64 >> index) & 0x01;
  op1_64 |= (((Bit64u) 1) << index);

  write_RMW_virtual_qword(op1_64);

  set_CF(bit_i);
}

void BX_CPU_C::BTS_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);
  op1_64 |= (((Bit64u) 1) << op2_64);

  /* now write result back to the destination */
  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}

void BX_CPU_C::BTR_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword(i->seg(), op1_addr);

  bx_bool temp_cf = (op1_64 >> index) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_cf);
}

void BX_CPU_C::BTR_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);
  op1_64 &= ~(((Bit64u) 1) << op2_64);

  /* now write result back to the destination */
  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}

void BX_CPU_C::BTC_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  op2_64 = BX_READ_64BIT_REG(i->nnn());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = RMAddr(i) + 8 * displacement64;
  op1_64 = read_RMW_virtual_qword(i->seg(), op1_addr);

  bx_bool temp_CF = (op1_64 >> index) & 0x01;
  op1_64 ^= (((Bit64u) 1) << index);  /* toggle bit */
  set_CF(temp_CF);

  write_RMW_virtual_qword(op1_64);
}

void BX_CPU_C::BTC_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  op2_64 &= 0x3f;

  bx_bool temp_CF = (op1_64 >> op2_64) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_64);  /* toggle bit */
  set_CF(temp_CF);

  BX_WRITE_64BIT_REG(i->rm(), op1_64);
}

void BX_CPU_C::BT_EqIbM(bxInstruction_c *i)
{
  Bit64u op1_64 = read_virtual_qword(i->seg(), RMAddr(i));
  Bit8u  op2_8  = i->Ib() & 0x3f;

  set_CF((op1_64 >> op2_8) & 0x01);
}

void BX_CPU_C::BT_EqIbR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  Bit8u  op2_8  = i->Ib() & 0x3f;

  set_CF((op1_64 >> op2_8) & 0x01);
}

void BX_CPU_C::BTS_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 |= (((Bit64u) 1) << op2_8);
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);
}

void BX_CPU_C::BTS_EqIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 |= (((Bit64u) 1) << op2_8);
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);
}

void BX_CPU_C::BTC_EqIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  set_CF(temp_CF);
}

void BX_CPU_C::BTR_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = read_RMW_virtual_qword(i->seg(), RMAddr(i));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << op2_8);
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);
}

void BX_CPU_C::BTR_EqIbR(bxInstruction_c *i)
{
  Bit64u op1_64;

  Bit8u op2_8 = i->Ib() & 0x3f;

  op1_64 = BX_READ_64BIT_REG(i->rm());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << op2_8);
  BX_WRITE_64BIT_REG(i->rm(), op1_64);

  set_CF(temp_CF);
}

/* 0F B8 */
void BX_CPU_C::POPCNT_GqEq(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit64u op1_64, op2_64;

  /* op2_16 is a register or memory reference */
  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    op2_64 = read_virtual_qword(i->seg(), RMAddr(i));
  }

  op1_64 = 0;
  while (op2_64 != 0) {
    if (op2_64 & 1) op1_64++;
    op2_64 >>= 1;
  }

  Bit32u flags = op1_64 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
#else
  BX_INFO(("POPCNT_GqEq: required POPCNT support, use --enable-popcnt option"));
  UndefinedOpcode(i);
#endif
}

#endif // BX_SUPPORT_X86_64
