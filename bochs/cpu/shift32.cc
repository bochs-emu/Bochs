/////////////////////////////////////////////////////////////////////////
// $Id: shift32.cc,v 1.27 2005-07-01 14:06:02 sshwarts Exp $
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


void BX_CPU_C::SHLD_EdGd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, result_32;
  unsigned count;

  /* op1:op2 << count.  result stored in op1 */

  if (i->b1() == 0x1a4)
    count = i->Ib() & 0x1f;
  else // 0x1a5
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  result_32 = (op1_32 << count) | (op2_32 >> (32 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * SHLD count affects the following flags: O,S,Z,A,P,C
   */
  SET_FLAGS_OSZAPC_32(op1_32, count, result_32, BX_INSTR_SHL32);
}

void BX_CPU_C::SHRD_EdGd(bxInstruction_c *i)
{
  Bit32u op1_32, op2_32, result_32;
  unsigned count;

  if (i->b1() == 0x1ac)
    count = i->Ib() & 0x1f;
  else // 0x1ad
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  op2_32 = BX_READ_32BIT_REG(i->nnn());

  result_32 = (op2_32 << (32 - count)) | (op1_32 >> count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * SHRD count affects the following flags: S,Z,P,C,O
   */

  set_CF((op1_32 >> (count - 1)) & 0x01);
  set_ZF(result_32 == 0);
  set_SF(result_32 >> 31);
  set_AF(0);
  /* for shift of 1, OF set if sign change occurred. */
  if (count == 1)
    set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
  set_PF_base(result_32);
}

void BX_CPU_C::ROL_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (! count) return;

  result_32 = (op1_32 << count) | (op1_32 >> (32 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * ROL count affects the following flags: C, O
   */
  bx_bool temp_CF = (result_32 & 0x01);

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_32 >> 31));
}

void BX_CPU_C::ROR_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (! count) return;

  result_32 = (op1_32 >> count) | (op1_32 << (32 - count));

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * ROR count affects the following flags: C, O
   */
  bx_bool result_b31 = (result_32 & 0x80000000) != 0;

  set_CF(result_b31);
  if (count == 1)
    set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
}

void BX_CPU_C::RCL_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  if (count==1) {
    result_32 = (op1_32 << 1) | getB_CF();
  }
  else {
    result_32 = (op1_32 << count) | (getB_CF() << (count - 1)) |
                (op1_32 >> (33 - count));
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * RCL count affects the following flags: C, O
   */
  bx_bool temp_CF = (op1_32 >> (32 - count)) & 0x01;

  set_CF(temp_CF);
  set_OF(temp_CF ^ (result_32 >> 31));
}

void BX_CPU_C::RCR_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  if (count==1) {
    result_32 = (op1_32 >> 1) | (getB_CF() << 31);
  }
  else {
    result_32 = (op1_32 >> count) | (getB_CF() << (32 - count)) |
                (op1_32 << (33 - count));
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  /* set eflags:
   * RCR count affects the following flags: C, O
   */

  set_CF((op1_32 >> (count - 1)) & 0x01);
  if (count == 1)
    set_OF(((op1_32 ^ result_32) & 0x80000000) > 0);
}

void BX_CPU_C::SHL_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  result_32 = (op1_32 << count);

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  SET_FLAGS_OSZAPC_32(op1_32, count, result_32, BX_INSTR_SHL32);
}

void BX_CPU_C::SHR_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

    if (!count) return;

#if defined(BX_HostAsm_Shr32)
  Bit32u flags32;
  asmShr32(result_32, op1_32, count, flags32);
  setEFlagsOSZAPC(flags32);
#else
  result_32 = (op1_32 >> count);
  SET_FLAGS_OSZAPC_32(op1_32, count, result_32, BX_INSTR_SHR32);
#endif

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }
}

void BX_CPU_C::SAR_Ed(bxInstruction_c *i)
{
  Bit32u op1_32, result_32;
  unsigned count;

  if (i->b1() == 0xc1)
    count = i->Ib() & 0x1f;
  else if (i->b1() == 0xd1)
    count = 1;
  else // (i->b1() == 0xd3)
    count = CL & 0x1f;

  /* op1 is a register or memory reference */
  if (i->modC0()) {
    op1_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_dword(i->seg(), RMAddr(i), &op1_32);
  }

  if (!count) return;

  /* count < 32, since only lower 5 bits used */
  if (op1_32 & 0x80000000) {
    result_32 = (op1_32 >> count) | (0xffffffff << (32 - count));
  }
  else {
    result_32 = (op1_32 >> count);
  }

  /* now write result back to destination */
  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), result_32);
  }
  else {
    Write_RMW_virtual_dword(result_32);
  }

  SET_FLAGS_OSZAPC_32(op1_32, count, result_32, BX_INSTR_SAR32);
}
