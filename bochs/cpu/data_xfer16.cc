/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer16.cc,v 1.15 2002-09-22 18:22:24 kevinlawton Exp $
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
BX_CPU_C::MOV_RXIw(bxInstruction_c *i)
{
  BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx = i->Iw();
}

  void
BX_CPU_C::XCHG_RXAX(bxInstruction_c *i)
{
  Bit16u temp16;

  temp16 = AX;
  AX = BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx;
  BX_CPU_THIS_PTR gen_reg[i->opcodeReg()].word.rx = temp16;
}


  void
BX_CPU_C::MOV_EwGw(bxInstruction_c *i)
{
    Bit16u op2_16;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    /* now write op2 to op1 */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op2_16);
      }
    else {
      write_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }
}


  void
BX_CPU_C::MOV_GwEw(bxInstruction_c *i)
{
    Bit16u op2_16;

    if (i->modC0()) {
      op2_16 = BX_READ_16BIT_REG(i->rm());
      }
    else {
      /* pointer, segment address pair */
      read_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }

    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
}

  void
BX_CPU_C::MOV_EwSw(bxInstruction_c *i)
{
  Bit16u seg_reg;

#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_EwSw: incomplete for CPU < 3"));
#endif

  seg_reg = BX_CPU_THIS_PTR sregs[i->nnn()].selector.value;

  if (i->modC0()) {
    // ??? BX_WRITE_16BIT_REG(mem_addr, seg_reg);
    if ( i->os32L() ) {
      BX_WRITE_32BIT_REGZ(i->rm(), seg_reg);
      }
    else {
      BX_WRITE_16BIT_REG(i->rm(), seg_reg);
      }
    }
  else {
    write_virtual_word(i->seg(), RMAddr(i), &seg_reg);
    }
}

  void
BX_CPU_C::MOV_SwEw(bxInstruction_c *i)
{
  Bit16u op2_16;

#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOV_SwEw: incomplete for CPU < 3"));
#endif

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

  load_seg_reg(&BX_CPU_THIS_PTR sregs[i->nnn()], op2_16);

  if (i->nnn() == BX_SEG_REG_SS) {
    // MOV SS inhibits interrupts, debug exceptions and single-step
    // trap exceptions until the execution boundary following the
    // next instruction is reached.
    // Same code as POP_SS()
    BX_CPU_THIS_PTR inhibit_mask |=
      BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
    BX_CPU_THIS_PTR async_event = 1;
    }
}

  void
BX_CPU_C::LEA_GwM(bxInstruction_c *i)
{
  if (i->modC0()) {
    BX_PANIC(("LEA_GvM: op2 is a register"));
    UndefinedOpcode(i);
    return;
    }

    BX_WRITE_16BIT_REG(i->nnn(), (Bit16u) RMAddr(i));
}


  void
BX_CPU_C::MOV_AXOw(bxInstruction_c *i)
{
  Bit16u temp_16;
  bx_address addr;

  addr = i->Id();

  /* read from memory address */

  if (!BX_NULL_SEG_REG(i->seg())) {
    read_virtual_word(i->seg(), addr, &temp_16);
    }
  else {
    read_virtual_word(BX_SEG_REG_DS, addr, &temp_16);
    }

  /* write to register */
  AX = temp_16;
}


  void
BX_CPU_C::MOV_OwAX(bxInstruction_c *i)
{
  Bit16u temp_16;
  bx_address addr;

  addr = i->Id();

  /* read from register */
  temp_16 = AX;

  /* write to memory address */
  if (!BX_NULL_SEG_REG(i->seg())) {
    write_virtual_word(i->seg(), addr, &temp_16);
    }
  else {
    write_virtual_word(BX_SEG_REG_DS, addr, &temp_16);
    }
}



  void
BX_CPU_C::MOV_EwIw(bxInstruction_c *i)
{
    Bit16u op2_16;

    op2_16 = i->Iw();

    /* now write sum back to destination */
    if (i->modC0()) {
      BX_WRITE_16BIT_REG(i->rm(), op2_16);
      }
    else {
      write_virtual_word(i->seg(), RMAddr(i), &op2_16);
      }
}


  void
BX_CPU_C::MOVZX_GwEb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVZX_GvEb: not supported on < 386"));
#else
  Bit8u  op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
    }

    /* zero extend byte op2 into word op1 */
    BX_WRITE_16BIT_REG(i->nnn(), (Bit16u) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVZX_GwEw(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVZX_GvEw: not supported on < 386"));
#else
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

    /* normal move */
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GwEb(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVSX_GvEb: not supported on < 386"));
#else
  Bit8u op2_8;

  if (i->modC0()) {
    op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
    }
  else {
    /* pointer, segment address pair */
    read_virtual_byte(i->seg(), RMAddr(i), &op2_8);
    }

    /* sign extend byte op2 into word op1 */
    BX_WRITE_16BIT_REG(i->nnn(), (Bit8s) op2_8);
#endif /* BX_CPU_LEVEL < 3 */
}

  void
BX_CPU_C::MOVSX_GwEw(bxInstruction_c *i)
{
#if BX_CPU_LEVEL < 3
  BX_PANIC(("MOVSX_GvEw: not supported on < 386"));
#else
  Bit16u op2_16;

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

    /* normal move */
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#endif /* BX_CPU_LEVEL < 3 */
}


  void
BX_CPU_C::XCHG_EwGw(bxInstruction_c *i)
{
    Bit16u op2_16, op1_16;

#ifdef MAGIC_BREAKPOINT
#if BX_DEBUGGER
  // (mch) Magic break point
  if (i->nnn() == 3 && i->modC0() && i->rm() == 3) {
    BX_CPU_THIS_PTR magic_break = 1;
    }
#endif
#endif

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(i->nnn());

    /* op1_16 is a register or memory reference */
    if (i->modC0()) {
      op1_16 = BX_READ_16BIT_REG(i->rm());
      BX_WRITE_16BIT_REG(i->rm(), op2_16);
      }
    else {
      /* pointer, segment address pair */
      read_RMW_virtual_word(i->seg(), RMAddr(i), &op1_16);
      Write_RMW_virtual_word(op2_16);
      }

    BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}


  void
BX_CPU_C::CMOV_GwEw(bxInstruction_c *i)
{
#if (BX_CPU_LEVEL >= 6) || (BX_CPU_LEVEL_HACKED >= 6)
  // Note: CMOV accesses a memory source operand (read), regardless
  //       of whether condition is true or not.  Thus, exceptions may
  //       occur even if the MOV does not take place.

  Boolean condition;
  Bit16u op2_16;

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
      condition = 0;
      BX_PANIC(("CMOV_GwEw: default case"));
    }

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
    }
  else {
    /* pointer, segment address pair */
    read_virtual_word(i->seg(), RMAddr(i), &op2_16);
    }

  if (condition) {
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
    }
#else
  BX_INFO(("cmov_gwew called"));
  UndefinedOpcode(i);
#endif
}
