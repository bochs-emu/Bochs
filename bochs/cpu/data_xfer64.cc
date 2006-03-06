/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer64.cc,v 1.23 2006-03-06 22:02:53 sshwarts Exp $
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


#if BX_SUPPORT_X86_64

void BX_CPU_C::XCHG_RRXRAX(bxInstruction_c *i)
{
  Bit64u temp64 = RAX;
  RAX = BX_READ_64BIT_REG(i->opcodeReg());
  BX_WRITE_64BIT_REG(i->opcodeReg(), temp64);
}

void BX_CPU_C::MOV_RRXIq(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->opcodeReg(), i->Iq());
}

void BX_CPU_C::MOV_EqGq(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->nnn());

  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op2_64);
  }
  else {
    write_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }
}

void BX_CPU_C::MOV_GqEq(bxInstruction_c *i)
{
  Bit64u op2_64;

  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::LEA_GqM(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_INFO(("LEA_GqM: op2 is a register"));
    UndefinedOpcode(i);
  }

  /* write effective address of op2 in op1 */
  BX_WRITE_64BIT_REG(i->nnn(), RMAddr(i));
}

void BX_CPU_C::MOV_ALOq(bxInstruction_c *i)
{
  Bit8u temp_8;
  bx_address addr = i->Iq();

  read_virtual_byte(i->seg(), addr, &temp_8);

  /* write to register */
  RAX = temp_8;
}

void BX_CPU_C::MOV_OqAL(bxInstruction_c *i)
{
  write_virtual_byte(i->seg(), i->Iq(), &AL);
}

void BX_CPU_C::MOV_AXOq(bxInstruction_c *i)
{
  Bit16u temp_16;
  bx_address addr = i->Iq();

  read_virtual_word(i->seg(), addr, &temp_16);

  /* write to register */
  AX = temp_16;
}

void BX_CPU_C::MOV_OqAX(bxInstruction_c *i)
{
  write_virtual_word(i->seg(), i->Iq(), &AX);
}

void BX_CPU_C::MOV_EAXOq(bxInstruction_c *i)
{
  Bit32u temp_32;
  bx_address addr = i->Iq();

  read_virtual_dword(i->seg(), addr, &temp_32);

  /* write to register */
  RAX = temp_32;
}

void BX_CPU_C::MOV_OqEAX(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), i->Iq(), &EAX);
}

void BX_CPU_C::MOV_RAXOq(bxInstruction_c *i)
{
  read_virtual_qword(i->seg(), i->Iq(), &RAX);
}

void BX_CPU_C::MOV_OqRAX(bxInstruction_c *i)
{
  write_virtual_qword(i->seg(), i->Iq(), &RAX);
}

void BX_CPU_C::MOV_EqId(bxInstruction_c *i)
{
  Bit64u op2_64 = (Bit32s) i->Id();

  /* now write sum back to destination */
  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), op2_64);
  }
  else {
    write_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }
}

void BX_CPU_C::MOVZX_GqEb(bxInstruction_c *i)
{
  Bit8u op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  }

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_8);
}

void BX_CPU_C::MOVZX_GqEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_16);
}

void BX_CPU_C::MOVSX_GqEb(bxInstruction_c *i)
{
  Bit8u op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
  }

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GqEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
  }

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit16s) op2_16);
}

void BX_CPU_C::MOVSX_GqEd(bxInstruction_c *i)
{
  Bit32u op2_32;

  if (i->modC0()) {
    op2_32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg(), RMAddr(i), &op2_32);
  }

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit32s) op2_32);
}

void BX_CPU_C::XCHG_EqGq(bxInstruction_c *i)
{
  Bit64u op2_64, op1_64;

  op2_64 = BX_READ_64BIT_REG(i->nnn());

  /* op1_64 is a register or memory reference */
  if (i->modC0()) {
    op1_64 = BX_READ_64BIT_REG(i->rm());
    BX_WRITE_64BIT_REG(i->rm(), op2_64);
  }
  else {
    /* pointer, segment address pair */
    read_RMW_virtual_qword(i->seg(), RMAddr(i), &op1_64);
    Write_RMW_virtual_qword(op2_64);
  }

  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}

void BX_CPU_C::CMOV_GqEq(bxInstruction_c *i)
{
  // Note: CMOV accesses a memory source operand (read), regardless
  //       of whether condition is true or not.  Thus, exceptions may
  //       occur even if the MOV does not take place.

  bx_bool condition = 0;
  Bit64u op2_64;

  switch (i->b1()) {
    // CMOV opcodes:
    case 0x140: condition = get_OF(); break;
    case 0x141: condition = !get_OF(); break;
    case 0x142: condition = get_CF(); break;
    case 0x143: condition = !get_CF(); break;
    case 0x144: condition = get_ZF(); break;
    case 0x145: condition = !get_ZF(); break;
    case 0x146: condition = get_CF() || get_ZF(); break;
    case 0x147: condition = !get_CF() && !get_ZF(); break;
    case 0x148: condition = get_SF(); break;
    case 0x149: condition = !get_SF(); break;
    case 0x14A: condition = get_PF(); break;
    case 0x14B: condition = !get_PF(); break;
    case 0x14C: condition = getB_SF() != getB_OF(); break;
    case 0x14D: condition = getB_SF() == getB_OF(); break;
    case 0x14E: condition = get_ZF() || (getB_SF() != getB_OF()); break;
    case 0x14F: condition = !get_ZF() && (getB_SF() == getB_OF()); break;
    default:
      BX_PANIC(("CMOV_GqEq: default case"));
  }

  if (i->modC0()) {
    op2_64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg(), RMAddr(i), &op2_64);
  }

  if (condition)
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

#endif /* if BX_SUPPORT_X86_64 */
