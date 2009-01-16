/////////////////////////////////////////////////////////////////////////
// $Id: bit32.cc,v 1.15 2009-01-16 18:18:58 sshwarts Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_CPU_LEVEL >= 3

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSF_GdEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->rm());

  if (op2_32 == 0) {
    assert_ZF(); /* op1_32 undefined */
  }
  else {
    Bit32u op1_32 = 0;
    while ((op2_32 & 0x01) == 0) {
      op1_32++;
      op2_32 >>= 1;
    }

    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
    clear_ZF();

    /* now write result back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSR_GdEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->rm());

  if (op2_32 == 0) {
    assert_ZF(); /* op1_32 undefined */
  }
  else {
    Bit32u op1_32 = 31;
    while ((op2_32 & 0x80000000) == 0) {
      op1_32--;
      op2_32 <<= 1;
    }

    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
    clear_ZF();

    /* now write result back to destination */
    BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;
  if (! i->as32L())
    op1_addr = (Bit16u) op1_addr;
#if BX_SUPPORT_X86_64
  else if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;
#endif

  /* pointer, segment address pair */
  op1_32 = read_virtual_dword(i->seg(), op1_addr);

  set_CF((op1_32 >> index) & 0x01);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;

  set_CF((op1_32 >> op2_32) & 0x01);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;
  bx_bool bit_i;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;
  if (! i->as32L())
    op1_addr = (Bit16u) op1_addr;
#if BX_SUPPORT_X86_64
  else if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;
#endif

  /* pointer, segment address pair */
  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr);

  bit_i = (op1_32 >> index) & 0x01;
  op1_32 |= (((Bit32u) 1) << index);

  write_RMW_virtual_dword(op1_32);

  set_CF(bit_i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;
  set_CF((op1_32 >> op2_32) & 0x01);
  op1_32 |= (((Bit32u) 1) << op2_32);

  /* now write result back to the destination */
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;
  if (! i->as32L())
    op1_addr = (Bit16u) op1_addr;
#if BX_SUPPORT_X86_64
  else if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;
#endif

  /* pointer, segment address pair */
  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr);

  bx_bool temp_cf = (op1_32 >> index) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << index);

  /* now write back to destination */
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_cf);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;
  set_CF((op1_32 >> op2_32) & 0x01);
  op1_32 &= ~(((Bit32u) 1) << op2_32);

  /* now write result back to the destination */
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index_32;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->nnn());
  index_32 = op2_32 & 0x1f;

  displacement32 = ((Bit32s) (op2_32 & 0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;
  if (! i->as32L())
    op1_addr = (Bit16u) op1_addr;
#if BX_SUPPORT_X86_64
  else if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;
#endif

  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr);
  bx_bool temp_CF = (op1_32 >> index_32) & 0x01;
  op1_32 ^= (((Bit32u) 1) << index_32);  /* toggle bit */
  set_CF(temp_CF);

  write_RMW_virtual_dword(op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->rm());
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  op2_32 &= 0x1f;

  bx_bool temp_CF = (op1_32 >> op2_32) & 0x01;
  op1_32 ^= (((Bit32u) 1) << op2_32);  /* toggle bit */
  set_CF(temp_CF);

  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_virtual_dword(i->seg(), eaddr);
  Bit8u  op2_8  = i->Ib() & 0x1f;

  set_CF((op1_32 >> op2_8) & 0x01);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdIbR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  Bit8u  op2_8  = i->Ib() & 0x1f;

  set_CF((op1_32 >> op2_8) & 0x01);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 |= (((Bit32u) 1) << op2_8);
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 |= (((Bit32u) 1) << op2_8);
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  set_CF(temp_CF);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 ^= (((Bit32u) 1) << op2_8);  /* toggle bit */
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 ^= (((Bit32u) 1) << op2_8);  /* toggle bit */
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  set_CF(temp_CF);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << op2_8);
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 &= ~(((Bit32u) 1) << op2_8);
  BX_WRITE_32BIT_REGZ(i->rm(), op1_32);

  set_CF(temp_CF);
}

/* 0F B8 */
void BX_CPP_AttrRegparmN(1) BX_CPU_C::POPCNT_GdEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_POPCNT || (BX_SUPPORT_SSE > 4) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit32u op2_32 = BX_READ_32BIT_REG(i->rm());

  Bit32u op1_32 = 0;
  while (op2_32 != 0) {
    if (op2_32 & 1) op1_32++;
    op2_32 >>= 1;
  }

  Bit32u flags = op1_32 ? 0 : EFlagsZFMask;
  setEFlagsOSZAPC(flags);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
#else
  BX_INFO(("POPCNT_GdEd: required POPCNT support, use --enable-popcnt option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#endif // (BX_CPU_LEVEL >= 3)
