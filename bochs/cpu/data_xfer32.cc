//  Copyright (C) 2000  MandrakeSoft S.A.
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






#include "bochs.h"



  void
BX_CPU_C::XCHG_ERXEAX(BxInstruction_t *i)
{
  Bit32u temp32;

  temp32 = EAX;
  EAX = BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].erx;
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].erx = temp32;
}

  void
BX_CPU_C::MOV_ERXId(BxInstruction_t *i)
{
  BX_CPU_THIS_PTR gen_reg[i->b1 & 0x07].erx = i->Id;
}

  void
BX_CPU_C::MOV_EdGd(BxInstruction_t *i)
{
    Bit32u op2_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    /* now write op2 to op1 */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, op2_32);
      }
    else {
      write_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }
}


  void
BX_CPU_C::MOV_GdEd(BxInstruction_t *i)
{
    Bit32u op2_32;

    if (i->mod == 0xc0) {
      op2_32 = BX_READ_32BIT_REG(i->rm);
      }
    else {
      /* pointer, segment address pair */
      read_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }

    BX_WRITE_32BIT_REG(i->nnn, op2_32);
}


  void
BX_CPU_C::LEA_GdM(BxInstruction_t *i)
{
  if (i->mod == 0xc0) {
    bx_panic("LEA_GvM: op2 is a register");
    UndefinedOpcode(i);
    return;
    }

    /* write effective address of op2 in op1 */
    BX_WRITE_32BIT_REG(i->nnn, i->rm_addr);
}


  void
BX_CPU_C::MOV_EAXOd(BxInstruction_t *i)
{
  Bit32u temp_32;
  Bit32u addr_32;

  addr_32 = i->Id;

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg)) {
    read_virtual_dword(i->seg, addr_32, &temp_32);
    }
  else {
    read_virtual_dword(BX_SEG_REG_DS, addr_32, &temp_32);
    }

  /* write to register */
  EAX = temp_32;
}


  void
BX_CPU_C::MOV_OdEAX(BxInstruction_t *i)
{
  Bit32u temp_32;
  Bit32u addr_32;

  addr_32 = i->Id;

  /* read from register */
  temp_32 = EAX;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg)) {
    write_virtual_dword(i->seg, addr_32, &temp_32);
    }
  else {
    write_virtual_dword(BX_SEG_REG_DS, addr_32, &temp_32);
    }
}



  void
BX_CPU_C::MOV_EdId(BxInstruction_t *i)
{
    Bit32u op2_32;

    op2_32 = i->Id;

    /* now write sum back to destination */
    if (i->mod == 0xc0) {
      BX_WRITE_32BIT_REG(i->rm, op2_32);
      }
    else {
      write_virtual_dword(i->seg, i->rm_addr, &op2_32);
      }
}


  void
BX_CPU_C::MOVZX_GdEb(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOVZX_GvEb: not supported on < 386\n");
#else
  Bit8u  op2_8;

  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

    /* zero extend byte op2 into dword op1 */
    BX_WRITE_32BIT_REG(i->nnn, (Bit32u) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVZX_GdEw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOVZX_GvEw: not supported on < 386\n");
#else
  Bit16u op2_16;

  if (i->mod == 0xc0) {
    op2_16 = BX_READ_16BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg, i->rm_addr, &op2_16);
    }

    /* zero extend word op2 into dword op1 */
    BX_WRITE_32BIT_REG(i->nnn, (Bit32u) op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GdEb(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOVSX_GvEb: not supported on < 386\n");
#else
  Bit8u op2_8;

  if (i->mod == 0xc0) {
    op2_8 = BX_READ_8BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg, i->rm_addr, &op2_8);
    }

    /* sign extend byte op2 into dword op1 */
    BX_WRITE_32BIT_REG(i->nnn, (Bit8s) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GdEw(BxInstruction_t *i)
{
#if BX_CPU_LEVEL < 3
  bx_panic("MOVSX_GvEw: not supported on < 386\n");
#else
  Bit16u op2_16;

  if (i->mod == 0xc0) {
    op2_16 = BX_READ_16BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg, i->rm_addr, &op2_16);
    }

    /* sign extend word op2 into dword op1 */
    BX_WRITE_32BIT_REG(i->nnn, (Bit16s) op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}


  void
BX_CPU_C::XCHG_EdGd(BxInstruction_t *i)
{
    Bit32u op2_32, op1_32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(i->nnn);

    /* op1_32 is a register or memory reference */
    if (i->mod == 0xc0) {
      op1_32 = BX_READ_32BIT_REG(i->rm);
      BX_WRITE_32BIT_REG(i->rm, op2_32);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_dword(i->seg, i->rm_addr, &op1_32);
      write_RMW_virtual_dword(op2_32);
      }

    BX_WRITE_32BIT_REG(i->nnn, op1_32);
}


  void
BX_CPU_C::CMOV_GdEd(BxInstruction_t *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  // Note: CMOV accesses a memory source operand (read), regardless
  //       of whether condition is true or not.  Thus, exceptions may
  //       occur even if the MOV does not take place.

  Boolean condition;
  Bit32u op2_32;


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
      bx_panic("CMOV_GdEd: default case\n");
    }

  if (i->mod == 0xc0) {
    op2_32 = BX_READ_32BIT_REG(i->rm);
    }
  else {
    /* pointer, segment address pair */
    read_virtual_dword(i->seg, i->rm_addr, &op2_32);
    }

  if (condition) {
    BX_WRITE_32BIT_REG(i->nnn, op2_32);
    }
#else
  bx_panic("cmov_gded called\n");
#endif
}
