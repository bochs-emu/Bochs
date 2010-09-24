/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer8.cc,v 1.47 2010-09-24 21:15:16 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RLIb(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->opcodeReg(), i->extend8bitL(), i->Ib());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RHIb(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGH(i->opcodeReg() & 0x03, i->Ib());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EbGbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_byte(i->seg(), eaddr, BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GbEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u val8 = read_virtual_byte(i->seg(), eaddr);
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), val8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GbEbR(bxInstruction_c *i)
{
  Bit8u op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op2);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_ALOd(bxInstruction_c *i)
{
  AL = read_virtual_byte_32(i->seg(), i->Id());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OdAL(bxInstruction_c *i)
{
  write_virtual_byte_32(i->seg(), i->Id(), AL);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EbIbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_byte(i->seg(), eaddr, i->Ib());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XLAT(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    AL = read_virtual_byte_64(i->seg(), RBX + AL);
  }
  else
#endif
  if (i->as32L()) {
    AL = read_virtual_byte(i->seg(), (Bit32u) (EBX + AL));
  }
  else {
    AL = read_virtual_byte_32(i->seg(), (Bit16u) (BX + AL));
  }
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  /* pointer, segment address pair */
  op1 = read_RMW_virtual_byte(i->seg(), eaddr);
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());

  write_RMW_virtual_byte(op2);
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EbGbR(bxInstruction_c *i)
{
  Bit8u op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  Bit8u op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op2);
}
