/////////////////////////////////////////////////////////////////////////
// $Id: logical8.cc,v 1.21 2002-10-25 18:26:28 sshwarts Exp $
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



  void
BX_CPU_C::XOR_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    result = op1 ^ op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    result = op1 ^ op2;
    Write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}

  void
BX_CPU_C::XOR_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2, result;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
    }

  result = op1 ^ op2;

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), result);

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}


  void
BX_CPU_C::XOR_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2, sum;

  op1 = AL;
  op2 = i->Ib();

  sum = op1 ^ op2;

  AL = sum;

  SET_FLAGS_OSZAPC_8(op1, op2, sum, BX_INSTR_XOR8);
}


  void
BX_CPU_C::XOR_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    result = op1 ^ op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    result = op1 ^ op2;
    Write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_XOR8);
}



  void
BX_CPU_C::OR_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    result = op1 | op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    result = op1 | op2;
    Write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  void
BX_CPU_C::NOT_Eb(bxInstruction_c *i)
{
  Bit8u op1_8, result_8;

  if (i->modC0()) {
    op1_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    result_8 = ~op1_8;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1_8);
    result_8 = ~op1_8;
    Write_RMW_virtual_byte(result_8);
    }
}


  void
BX_CPU_C::OR_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    result = op1 | op2;
    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);
    result = op1 | op2;
    Write_RMW_virtual_byte(result);
    }

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
}


  void
BX_CPU_C::OR_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2, result;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmOr8(result, op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result = op1 | op2;
#endif

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), result);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
#endif
}


  void
BX_CPU_C::OR_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2, result;

  op1 = AL;
  op2 = i->Ib();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmOr8(result, op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result = op1 | op2;
#endif

  AL = result;

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_OR8);
#endif
}



  void
BX_CPU_C::AND_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;

    asmAnd8(result, op1, op2, flags32);
    setEFlagsOSZAPC(flags32);
#else
    result = op1 & op2;
#endif

    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;

    asmAnd8(result, op1, op2, flags32);
    setEFlagsOSZAPC(flags32);
#else
    result = op1 & op2;
#endif

    Write_RMW_virtual_byte(result);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
#endif
}


  void
BX_CPU_C::AND_GbEb(bxInstruction_c *i)
{
  Bit8u op1, op2, result;

  op1 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op2 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op2);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmAnd8(result, op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result = op1 & op2;
#endif

  BX_WRITE_8BIT_REGx(i->nnn(), i->extend8bitL(), result);

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
#endif
}


  void
BX_CPU_C::AND_ALIb(bxInstruction_c *i)
{
  Bit8u op1, op2, result;


  op1 = AL;
  op2 = i->Ib();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmAnd8(result, op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result = op1 & op2;
#endif

  AL = result;

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
#endif
}




  void
BX_CPU_C::AND_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1, result;


  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;

    asmAnd8(result, op1, op2, flags32);
    setEFlagsOSZAPC(flags32);
#else
    result = op1 & op2;
#endif

    BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result);
    }
  else {
    read_RMW_virtual_byte(i->seg(), RMAddr(i), &op1);

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
    Bit32u flags32;

    asmAnd8(result, op1, op2, flags32);
    setEFlagsOSZAPC(flags32);
#else
    result = op1 & op2;
#endif

    Write_RMW_virtual_byte(result);
    }

#if !(defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_AND8);
#endif
}


  void
BX_CPU_C::TEST_EbGb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = BX_READ_8BIT_REGx(i->nnn(),i->extend8bitL());

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmTest8(op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  Bit8u result;
  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
#endif
}


  void
BX_CPU_C::TEST_ALIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op1 = AL;
  op2 = i->Ib();

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmTest8(op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  Bit8u result;
  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
#endif
}



  void
BX_CPU_C::TEST_EbIb(bxInstruction_c *i)
{
  Bit8u op2, op1;

  op2 = i->Ib();

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
    }

#if (defined(__i386__) && defined(__GNUC__) && BX_SupportHostAsms)
  Bit32u flags32;

  asmTest8(op1, op2, flags32);
  setEFlagsOSZAPC(flags32);
#else
  Bit8u result;
  result = op1 & op2;

  SET_FLAGS_OSZAPC_8(op1, op2, result, BX_INSTR_TEST8);
#endif
}
