/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

#include "scalar_arith.h"

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BSF_GdEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  if (op2_32 == 0) {
    assert_ZF(); /* op1_32 undefined */
  }
  else {
    Bit32u op1_32 = tzcntd(op2_32);
    SET_FLAGS_OSZAPC_LOGIC_32(op1_32);
    clear_ZF();

    BX_WRITE_32BIT_REGZ(i->dst(), op1_32);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BSR_GdEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

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

    BX_WRITE_32BIT_REGZ(i->dst(), op1_32);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->src());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;

  /* pointer, segment address pair */
  op1_32 = read_virtual_dword(i->seg(), op1_addr & i->asize_mask());

  set_CF((op1_32 >> index) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op2_32 &= 0x1f;

  set_CF((op1_32 >> op2_32) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;
  bx_bool bit_i;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->src());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;

  /* pointer, segment address pair */
  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr & i->asize_mask());

  bit_i = (op1_32 >> index) & 0x01;
  op1_32 |= (1 << index);

  write_RMW_virtual_dword(op1_32);

  set_CF(bit_i);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op2_32 &= 0x1f;
  set_CF((op1_32 >> op2_32) & 0x01);
  op1_32 |= (1 << op2_32);

  /* now write result back to the destination */
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->src());
  index = op2_32 & 0x1f;
  displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;

  /* pointer, segment address pair */
  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr & i->asize_mask());

  bx_bool temp_cf = (op1_32 >> index) & 0x01;
  op1_32 &= ~(1 << index);

  /* now write back to destination */
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op2_32 &= 0x1f;
  set_CF((op1_32 >> op2_32) & 0x01);
  op1_32 &= ~(1 << op2_32);

  /* now write result back to the destination */
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdGdM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit32u op1_32, op2_32, index_32;
  Bit32s displacement32;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_32 = BX_READ_32BIT_REG(i->src());
  index_32 = op2_32 & 0x1f;

  displacement32 = ((Bit32s) (op2_32 & 0xffffffe0)) / 32;
  op1_addr = eaddr + 4 * displacement32;

  op1_32 = read_RMW_virtual_dword(i->seg(), op1_addr & i->asize_mask());
  bx_bool temp_CF = (op1_32 >> index_32) & 0x01;
  op1_32 ^= (1 << index_32);  /* toggle bit */
  set_CF(temp_CF);

  write_RMW_virtual_dword(op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32;

  op1_32 = BX_READ_32BIT_REG(i->dst());
  op2_32 = BX_READ_32BIT_REG(i->src());
  op2_32 &= 0x1f;

  bx_bool temp_CF = (op1_32 >> op2_32) & 0x01;
  op1_32 ^= (1 << op2_32);  /* toggle bit */
  set_CF(temp_CF);

  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_virtual_dword(i->seg(), eaddr);
  Bit8u  op2_8  = i->Ib() & 0x1f;

  set_CF((op1_32 >> op2_8) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EdIbR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit8u  op2_8  = i->Ib() & 0x1f;

  set_CF((op1_32 >> op2_8) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 |= (1 << op2_8);
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 |= (1 << op2_8);
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 ^= (1 << op2_8);  /* toggle bit */
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 ^= (1 << op2_8);  /* toggle bit */
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 &= ~(1 << op2_8);
  write_RMW_virtual_dword(op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EdIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x1f;

  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  bx_bool temp_CF = (op1_32 >> op2_8) & 0x01;
  op1_32 &= ~(1 << op2_8);
  BX_WRITE_32BIT_REGZ(i->dst(), op1_32);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

/* F3 0F B8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPCNT_GdEdR(bxInstruction_c *i)
{
  Bit32u op_32 = popcntd(BX_READ_32BIT_REG(i->src()));

  clearEFlagsOSZAPC();
  if (! op_32) assert_ZF();

  BX_WRITE_32BIT_REGZ(i->dst(), op_32);

  BX_NEXT_INSTR(i);
}

/* F3 0F BC */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TZCNT_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = tzcntd(op1_32);

  set_CF(! op1_32);
  set_ZF(! result_32);
  
  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

/* F3 0F BD */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LZCNT_GdEdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->src());
  Bit32u result_32 = lzcntd(op1_32);

  set_CF(! op1_32);
  set_ZF(! result_32);
  
  BX_WRITE_32BIT_REGZ(i->dst(), result_32);

  BX_NEXT_INSTR(i);
}

#endif // (BX_CPU_LEVEL >= 3)
