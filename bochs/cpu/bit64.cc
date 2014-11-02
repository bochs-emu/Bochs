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

#if BX_SUPPORT_X86_64

#include "scalar_arith.h"

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BSF_GqEqR(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
  }
  else {
    Bit64u op1_64 = tzcntq(op2_64);
    SET_FLAGS_OSZAPC_LOGIC_64(op1_64);
    clear_ZF();

    BX_WRITE_64BIT_REG(i->dst(), op1_64);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BSR_GqEqR(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  if (op2_64 == 0) {
    assert_ZF(); /* op1_64 undefined */
  }
  else {
    Bit64u op1_64 = 63;
    while ((op2_64 & BX_CONST64(0x8000000000000000)) == 0) {
      op1_64--;
      op2_64 <<= 1;
    }

    SET_FLAGS_OSZAPC_LOGIC_64(op1_64);
    clear_ZF();

    BX_WRITE_64BIT_REG(i->dst(), op1_64);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_64 = BX_READ_64BIT_REG(i->src());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = eaddr + 8 * displacement64;
  if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;

  /* pointer, segment address pair */
  op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), op1_addr));

  set_CF((op1_64 >> index) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;
  bx_bool bit_i;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_64 = BX_READ_64BIT_REG(i->src());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = eaddr + 8 * displacement64;
  if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), op1_addr));
  bit_i = (op1_64 >> index) & 0x01;
  op1_64 |= (((Bit64u) 1) << index);
  write_RMW_virtual_qword(op1_64);

  set_CF(bit_i);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);
  op1_64 |= (((Bit64u) 1) << op2_64);

  /* now write result back to the destination */
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64, index;
  Bit64s displacement64;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_64 = BX_READ_64BIT_REG(i->src());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = eaddr + 8 * displacement64;
  if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;

  /* pointer, segment address pair */
  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), op1_addr));
  bx_bool temp_cf = (op1_64 >> index) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << index);
  /* now write back to destination */
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_cf);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op2_64 &= 0x3f;
  set_CF((op1_64 >> op2_64) & 0x01);
  op1_64 &= ~(((Bit64u) 1) << op2_64);

  /* now write result back to the destination */
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EqGqM(bxInstruction_c *i)
{
  bx_address op1_addr;
  Bit64u op1_64, op2_64;
  Bit64s displacement64;
  Bit64u index;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op2_64 = BX_READ_64BIT_REG(i->src());
  index = op2_64 & 0x3f;
  displacement64 = ((Bit64s) (op2_64 & BX_CONST64(0xffffffffffffffc0))) / 64;
  op1_addr = eaddr + 8 * displacement64;
  if (! i->as64L())
    op1_addr = (Bit32u) op1_addr;

  op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), op1_addr));
  bx_bool temp_CF = (op1_64 >> index) & 0x01;
  op1_64 ^= (((Bit64u) 1) << index);  /* toggle bit */
  set_CF(temp_CF);

  write_RMW_virtual_qword(op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64, op2_64;

  op1_64 = BX_READ_64BIT_REG(i->dst());
  op2_64 = BX_READ_64BIT_REG(i->src());
  op2_64 &= 0x3f;

  bx_bool temp_CF = (op1_64 >> op2_64) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_64);  /* toggle bit */
  set_CF(temp_CF);

  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EqIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  Bit8u  op2_8  = i->Ib() & 0x3f;

  set_CF((op1_64 >> op2_8) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BT_EqIbR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit8u  op2_8  = i->Ib() & 0x3f;

  set_CF((op1_64 >> op2_8) & 0x01);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 |= (((Bit64u) 1) << op2_8);
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTS_EqIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 |= (((Bit64u) 1) << op2_8);
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTC_EqIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 ^= (((Bit64u) 1) << op2_8);  /* toggle bit */
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EqIbM(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << op2_8);
  write_RMW_virtual_qword(op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::BTR_EqIbR(bxInstruction_c *i)
{
  Bit8u op2_8 = i->Ib() & 0x3f;

  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  bx_bool temp_CF = (op1_64 >> op2_8) & 0x01;
  op1_64 &= ~(((Bit64u) 1) << op2_8);
  BX_WRITE_64BIT_REG(i->dst(), op1_64);

  set_CF(temp_CF);

  BX_NEXT_INSTR(i);
}

/* F3 0F B8 */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::POPCNT_GqEqR(bxInstruction_c *i)
{
  Bit32u op_32 = popcntq(BX_READ_64BIT_REG(i->src()));

  clearEFlagsOSZAPC();
  if (! op_32) assert_ZF();

  BX_WRITE_32BIT_REGZ(i->dst(), op_32);

  BX_NEXT_INSTR(i);
}

/* F3 0F BC */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::TZCNT_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = tzcntq(op1_64);

  set_CF(! op1_64);
  set_ZF(! result_64);
  
  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

/* F3 0F BD */
BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LZCNT_GqEqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->src());
  Bit64u result_64 = lzcntq(op1_64);

  set_CF(! op1_64);
  set_ZF(! result_64);
  
  BX_WRITE_64BIT_REG(i->dst(), result_64);

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_X86_64
