/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer8.cc,v 1.34 2007-12-21 17:30:49 sshwarts Exp $
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


void BX_CPU_C::MOV_RLIb(bxInstruction_c *i)
{
  BX_READ_8BIT_REGx(i->opcodeReg(), i->extend8bitL()) = i->Ib();
}

void BX_CPU_C::MOV_RHIb(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR gen_reg[i->b1() & 0x03].word.byte.rh = i->Ib();
}

void BX_CPU_C::MOV_EbGbM(bxInstruction_c *i)
{
  write_virtual_byte(i->seg(), RMAddr(i), BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL()));
}

void BX_CPU_C::MOV_EbGbR(bxInstruction_c *i)
{
  Bit8u op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op2);
}

void BX_CPU_C::MOV_GbEbM(bxInstruction_c *i)
{
  Bit8u val8 = read_virtual_byte(i->seg(), RMAddr(i));
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), val8);
}

void BX_CPU_C::MOV_GbEbR(bxInstruction_c *i)
{
  Bit8u op2 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op2);
}

void BX_CPU_C::MOV_ALOd(bxInstruction_c *i)
{
  AL = read_virtual_byte(i->seg(), i->Id());
}

void BX_CPU_C::MOV_OdAL(bxInstruction_c *i)
{
  write_virtual_byte(i->seg(), i->Id(), AL);
}

void BX_CPU_C::MOV_EbIbM(bxInstruction_c *i)
{
  write_virtual_byte(i->seg(), RMAddr(i), i->Ib());
}

void BX_CPU_C::MOV_EbIbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), i->Ib());
}

void BX_CPU_C::XLAT(bxInstruction_c *i)
{
  bx_address offset;

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    offset = RBX + AL;
  }
  else
#endif
  if (i->as32L()) {
    offset = EBX + AL;
  }
  else {
    offset =  BX + AL;
  }

  AL = read_virtual_byte(i->seg(), offset);
}

void BX_CPU_C::XCHG_EbGbM(bxInstruction_c *i)
{
  Bit8u op1, op2;

  /* pointer, segment address pair */
  op1 = read_RMW_virtual_byte(i->seg(), RMAddr(i));
  op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());

  write_RMW_virtual_byte(op2);
  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
}

void BX_CPU_C::XCHG_EbGbR(bxInstruction_c *i)
{
  Bit8u op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());
  Bit8u op2 = BX_READ_8BIT_REGx(i->nnn(), i->extend8bitL());

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), op1);
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), op2);
}
