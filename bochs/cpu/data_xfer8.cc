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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EbIbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_byte(i->seg(), eaddr, BX_READ_8BIT_REGx(i->src(), i->extend8bitL()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u val8 = read_virtual_byte(i->seg(), eaddr);
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), val8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GbEbR(bxInstruction_c *i)
{
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op2);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_ALOd(bxInstruction_c *i)
{
  AL = read_virtual_byte_32(i->seg(), i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OdAL(bxInstruction_c *i)
{
  write_virtual_byte_32(i->seg(), i->Id(), AL);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_byte(i->seg(), eaddr, i->Ib());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XLAT(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    AL = read_linear_byte(i->seg(), get_laddr64(i->seg(), RBX + AL));
  }
  else
#endif
  if (i->as32L()) {
    AL = read_virtual_byte(i->seg(), (Bit32u) (EBX + AL));
  }
  else {
    AL = read_virtual_byte_32(i->seg(), (Bit16u) (BX + AL));
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  write_RMW_virtual_byte(op2);
  BX_WRITE_8BIT_REGx(i->src(), i->extend8bitL(), op1);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EbGbR(bxInstruction_c *i)
{
  Bit8u op1 = BX_READ_8BIT_REGx(i->dst(), i->extend8bitL());
  Bit8u op2 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  BX_WRITE_8BIT_REGx(i->src(), i->extend8bitL(), op1);
  BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), op2);

  BX_NEXT_INSTR(i);
}
