/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer64.cc,v 1.1 2002-09-13 15:53:22 kevinlawton Exp $
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
BX_CPU_C::XCHG_RRXRAX(BxInstruction_t *i)
{
  Bit64u temp64;

  temp64 = RAX;
  RAX = BX_CPU_THIS_PTR gen_reg[i->nnn].rrx;
  BX_CPU_THIS_PTR gen_reg[i->nnn].rrx = temp64;
}

  void
BX_CPU_C::MOV_RRXIq(BxInstruction_t *i)
{
  BX_CPU_THIS_PTR gen_reg[i->nnn].rrx = i->Iq;
}

  void
BX_CPU_C::MOV_EqGq(BxInstruction_t *i)
{
    Bit64u op2_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    /* now write op2 to op1 */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, op2_64);
      }
    else {
      write_virtual_qword(i->seg, i->rm_addr, &op2_64);
      }
}


  void
BX_CPU_C::MOV_GqEq(BxInstruction_t *i)
{
    Bit64u op2_64;

    //BX_DEBUG (("MOV_GqEq mod=%02x nnn=%d rm=%d rm_addr=%08x seg=%d",i->mod,i->nnn,i->rm,i->rm_addr,i->seg));
    if (i->mod == 0xc0) {
      op2_64 = BX_READ_64BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      //BX_DEBUG (("call read_virtual_qword"));
      read_virtual_qword(i->seg, i->rm_addr, &op2_64);
      //BX_DEBUG (("done read_virtual_qword"));
      }

    BX_WRITE_64BIT_REG(i->nnn, op2_64);
}


  void
BX_CPU_C::LEA_GqM(BxInstruction_t *i)
{
  if (i->mod == 0xc0) {
    BX_PANIC(("LEA_GvM: op2 is a register"));
    UndefinedOpcode(i);
    return;
    }

    /* write effective address of op2 in op1 */
    BX_WRITE_64BIT_REG(i->nnn, i->rm_addr);
}


  void
BX_CPU_C::MOV_ALOq(BxInstruction_t *i)
{
  Bit8u temp_8;
  bx_address addr;

  addr = i->Iq;

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_byte(i->seg, addr, &temp_8);
    }
  else {
    read_virtual_byte(BX_SEG_REG_DS, addr, &temp_8);
    }

  /* write to register */
  RAX = temp_8;
}


  void
BX_CPU_C::MOV_OqAL(BxInstruction_t *i)
{
  Bit8u temp_8;
  bx_address addr;

  addr = i->Iq;

  /* read from register */
  temp_8 = AL;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_byte(i->seg, addr, &temp_8);
    }
  else {
    write_virtual_byte(BX_SEG_REG_DS, addr, &temp_8);
    }
}

  void
BX_CPU_C::MOV_AXOq(BxInstruction_t *i)
{
  Bit16u temp_16;
  bx_address addr;

  addr = i->Iq;

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_word(i->seg, addr, &temp_16);
    }
  else {
    read_virtual_word(BX_SEG_REG_DS, addr, &temp_16);
    }

  /* write to register */
  AX = temp_16;
}


  void
BX_CPU_C::MOV_OqAX(BxInstruction_t *i)
{
  Bit16u temp_16;
  bx_address addr;

  addr = i->Iq;

  /* read from register */
  temp_16 = AX;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_word(i->seg, addr, &temp_16);
    }
  else {
    write_virtual_word(BX_SEG_REG_DS, addr, &temp_16);
    }
}

  void
BX_CPU_C::MOV_EAXOq(BxInstruction_t *i)
{
  Bit32u temp_32;
  bx_address addr;

  addr = i->Iq;

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_dword(i->seg, addr, &temp_32);
    }
  else {
    read_virtual_dword(BX_SEG_REG_DS, addr, &temp_32);
    }

  /* write to register */
  RAX = temp_32;
}


  void
BX_CPU_C::MOV_OqEAX(BxInstruction_t *i)
{
  Bit32u temp_32;
  bx_address addr;

  addr = i->Iq;

  /* read from register */
  temp_32 = EAX;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_dword(i->seg, addr, &temp_32);
    }
  else {
    write_virtual_dword(BX_SEG_REG_DS, addr, &temp_32);
    }
}

  void
BX_CPU_C::MOV_RAXOq(BxInstruction_t *i)
{
  Bit64u temp_64;
  bx_address addr;

  addr = i->Iq;

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_qword(i->seg, addr, &temp_64);
    }
  else {
    read_virtual_qword(BX_SEG_REG_DS, addr, &temp_64);
    }

  /* write to register */
  RAX = temp_64;
}


  void
BX_CPU_C::MOV_OqRAX(BxInstruction_t *i)
{
  Bit64u temp_64;
  bx_address addr;

  addr = i->Iq;

  /* read from register */
  temp_64 = RAX;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_qword(i->seg, addr, &temp_64);
    }
  else {
    write_virtual_qword(BX_SEG_REG_DS, addr, &temp_64);
    }
}



  void
BX_CPU_C::MOV_EqId(BxInstruction_t *i)
{
    Bit64u op2_64;

    op2_64 = (Bit32s) i->Id;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_64BIT_REG(i->rm, op2_64);
      }
    else {
      write_virtual_qword(i->seg, i->rm_addr, &op2_64);
      }
}


  void
BX_CPU_C::MOVZX_GqEb(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVZX_GvEb: not supported on < 386"));
#else
  Bit8u  op2_8;

  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm,i->extend8bit);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

    /* zero extend byte op2 into qword op1 */
    BX_WRITE_64BIT_REG(i->nnn, (Bit64u) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVZX_GqEw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVZX_GvEw: not supported on < 386"));
#else
  Bit16u op2_16;

  if (i->mod == 0xc0) {
    op2_16 = BX_READ_16BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg, i->rm_addr, &op2_16);
    }

    /* zero extend word op2 into qword op1 */
    BX_WRITE_64BIT_REG(i->nnn, (Bit64u) op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GqEb(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVSX_GvEb: not supported on < 386"));
#else
  Bit8u op2_8;

  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm,i->extend8bit);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

    /* sign extend byte op2 into qword op1 */
    BX_WRITE_64BIT_REG(i->nnn, (Bit8s) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GqEw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVSX_GvEw: not supported on < 386"));
#else
  Bit16u op2_16;

  if (i->mod == 0xc0) {
    op2_16 = BX_READ_16BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg, i->rm_addr, &op2_16);
    }

    /* sign extend word op2 into qword op1 */
    BX_WRITE_64BIT_REG(i->nnn, (Bit16s) op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GqEd(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVSX_GvEw: not supported on < 386"));
#else
  Bit32u op2_32;

  if (i->mod == 0xc0) {
    op2_32 = BX_READ_32BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg, i->rm_addr, &op2_32);
    }

    /* sign extend word op2 into qword op1 */
    BX_WRITE_64BIT_REG(i->nnn, (Bit16s) op2_32);
#endif /* BX_CPU_LEVEL < 3 */
}


  void
BX_CPU_C::XCHG_EqGq(BxInstruction_t *i)
{
    Bit64u op2_64, op1_64;

    /* op2_64 is a register, op2_addr is an index of a register */
    op2_64 = BX_READ_64BIT_REG(i->nnn);

    /* op1_64 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_64 = BX_READ_64BIT_REG(i->rm);
      BX_WRITE_64BIT_REG(i->rm, op2_64);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_qword(i->seg, i->rm_addr, &op1_64);
      write_RMW_virtual_qword(op2_64);
      }

    BX_WRITE_64BIT_REG(i->nnn, op1_64);
}


  void
BX_CPU_C::CMOV_GqEq(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  // Note: CMOV accesses a memory source operand (read), regardless
  //       of whether condition is true or not.  Thus, exceptions may
  //       occur even if the MOV does not take place.

  Boolean condition;
  Bit64u op2_64;


  switch (i->b1) {
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
    case 0x14C: condition = get_SF() != get_OF(); break;
    case 0x14D: condition = get_SF() == get_OF(); break;
    case 0x14E: condition = get_ZF() || (get_SF() != get_OF()); break;
    case 0x14F: condition = !get_ZF() && (get_SF() == get_OF()); break;
    default:
      condition = 0;
      BX_PANIC(("CMOV_GdEd: default case"));
    }

  if (i->mod == 0xc0) {
    op2_64 = BX_READ_64BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_qword(i->seg, i->rm_addr, &op2_64);
    }

  if (condition) {
    BX_WRITE_64BIT_REG(i->nnn, op2_64);
    }
#else
  BX_PANIC(("cmov_gded called"));
#endif
}
