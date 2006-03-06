/////////////////////////////////////////////////////////////////////////
// $Id: mult32.cc,v 1.20 2006-03-06 22:03:01 sshwarts Exp $
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
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR


#if BX_SUPPORT_X86_64==0
#define RAX EAX
#define RDX EDX
#endif


void BX_CPU_C::MUL_EAXEd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, product_32h, product_32l;
  Bit64u product_64;

  op1_32 = EAX;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  product_64  = ((Bit64u) op1_32) * ((Bit64u) op2_32);
  product_32l = (Bit32u) (product_64 & 0xFFFFFFFF);
  product_32h = (Bit32u) (product_64 >> 32);

  /* set EFLAGS */
  SET_FLAGS_OSZAPC_S1S2_32(product_32l, product_32h, BX_INSTR_MUL32);

  /* now write product back to destination */
  RAX = product_32l;
  RDX = product_32h;
}

void BX_CPU_C::IMUL_EAXEd(bxInstruction_c *i)
{
  Bit32s op1_32, op2_32;

  op1_32 = EAX;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), (Bit32u *) &op2_32);
  }

  Bit64s product_64  = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  Bit32u product_32l = (Bit32u) (product_64 & 0xFFFFFFFF);
  Bit32u product_32h = (Bit32u) (product_64 >> 32);

  /* now write product back to destination */
  RAX = product_32l;
  RDX = product_32h;

  /* set eflags:
   * IMUL r/m32: condition for clearing CF & OF:
   *   EDX:EAX = sign-extend of EAX
   */
  SET_FLAGS_OSZAPC_S1S2_32(product_32l, product_32h, BX_INSTR_IMUL32);
}

void BX_CPU_C::DIV_EAXEd(bxInstruction_c *i)
{
  Bit32u op2_32, remainder_32, quotient_32l;
  Bit64u op1_64, quotient_64;

  op1_64 = (((Bit64u) EDX) << 32) + ((Bit64u) EAX);

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  if (op2_32 == 0) {
    exception(BX_DE_EXCEPTION, 0, 0);
  }

  quotient_64  = op1_64 / op2_32;
  remainder_32 = (Bit32u) (op1_64 % op2_32);
  quotient_32l = (Bit32u) (quotient_64 & 0xFFFFFFFF);

  if (quotient_64 != quotient_32l)
  {
    exception(BX_DE_EXCEPTION, 0, 0);
  }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_32l;
  RDX = remainder_32;
}

void BX_CPU_C::IDIV_EAXEd(bxInstruction_c *i)
{
  Bit32s op2_32, remainder_32, quotient_32l;
  Bit64s op1_64, quotient_64;

  op1_64 = (((Bit64u) EDX) << 32) | ((Bit64u) EAX);

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), (Bit32u *) &op2_32);
  }

  if (op2_32 == 0)
    exception(BX_DE_EXCEPTION, 0, 0);

  /* check MIN_INT divided by -1 case */
  if ((op1_64 == ((Bit64s)BX_CONST64(0x8000000000000000))) && (op2_32 == -1))
    exception(BX_DE_EXCEPTION, 0, 0);

  quotient_64  = op1_64 / op2_32;
  remainder_32 = (Bit32s) (op1_64 % op2_32);
  quotient_32l = (Bit32s) (quotient_64 & 0xFFFFFFFF);

  if (quotient_64 != quotient_32l)
  {
    exception(BX_DE_EXCEPTION, 0, 0);
  }

  /* set EFLAGS:
   * IDIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */
  RAX = quotient_32l;
  RDX = remainder_32;
}

void BX_CPU_C::IMUL_GdEdId(bxInstruction_c *i)
{
  Bit32s op2_32, op3_32;

  op3_32 = i->Id();

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), (Bit32u *) &op2_32);
  }

  Bit64s product_64  = ((Bit64s) op2_32) * ((Bit64s) op3_32);
  Bit32u product_32l = (product_64 & 0xFFFFFFFF);
  Bit32u product_32h = (product_64 >> 32);

  /* now write product back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), product_32l);

  /* set eflags:
   * IMUL r32,r/m32,imm32: condition for clearing CF & OF:
   *   result exactly fits within r32
   */
  SET_FLAGS_OSZAPC_S1S2_32(product_32l, product_32h, BX_INSTR_IMUL32);
}

void BX_CPU_C::IMUL_GdEd(bxInstruction_c *i)
{
  Bit32s op1_32, op2_32;

  /* op2 is a register or memory reference */
  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), (Bit32u *) &op2_32);
  }

  op1_32 = BX_READ_32BIT_REG(i->nnn());

  Bit64s product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  Bit32u product_32l = (product_64 & 0xFFFFFFFF);
  Bit32u product_32h = (product_64 >> 32);

  /* now write product back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), product_32l);

  /* set eflags:
   * IMUL r32,r/m32,imm32: condition for clearing CF & OF:
   *   result exactly fits within r32
   */
  SET_FLAGS_OSZAPC_S1S2_32(product_32l, product_32h, BX_INSTR_IMUL32);
}
