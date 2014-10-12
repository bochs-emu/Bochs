/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2012  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EwIwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_word(i->seg(), eaddr, i->Iw());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EwIwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->dst(), i->Iw());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EwGwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_word(i->seg(), eaddr, BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GwEwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GwEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit16u val16 = read_virtual_word(i->seg(), eaddr);
  BX_WRITE_16BIT_REG(i->dst(), val16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EwSwR(bxInstruction_c *i)
{
  /* Illegal to use nonexisting segments */
  if (i->src() >= 6) {
    BX_INFO(("MOV_EwSw: using of nonexisting segment register %d", i->src()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u seg_reg = BX_CPU_THIS_PTR sregs[i->src()].selector.value;

  if (i->os32L()) {
    BX_WRITE_32BIT_REGZ(i->dst(), seg_reg);
  }
  else {
    BX_WRITE_16BIT_REG(i->dst(), seg_reg);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EwSwM(bxInstruction_c *i)
{
  /* Illegal to use nonexisting segments */
  if (i->src() >= 6) {
    BX_INFO(("MOV_EwSw: using of nonexisting segment register %d", i->src()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u seg_reg = BX_CPU_THIS_PTR sregs[i->src()].selector.value;
  write_virtual_word(i->seg(), eaddr, seg_reg);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_SwEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  /* Attempt to load CS or nonexisting segment register */
  if (i->dst() >= 6 || i->dst() == BX_SEG_REG_CS) {
    BX_INFO(("MOV_EwSw: can't use this segment register %d", i->dst()));
    exception(BX_UD_EXCEPTION, 0);
  }

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->src());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    /* pointer, segment address pair */
    op2_16 = read_virtual_word(i->seg(), eaddr);
  }

  load_seg_reg(&BX_CPU_THIS_PTR sregs[i->dst()], op2_16);

  if (i->dst() == BX_SEG_REG_SS) {
    // MOV SS inhibits interrupts, debug exceptions and single-step
    // trap exceptions until the execution boundary following the
    // next instruction is reached.
    // Same code as POP_SS()
    inhibit_interrupts(BX_INHIBIT_INTERRUPTS_BY_MOVSS);
  }

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEA_GwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_WRITE_16BIT_REG(i->dst(), (Bit16u) eaddr);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_AXOd(bxInstruction_c *i)
{
  AX = read_virtual_word_32(i->seg(), i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OdAX(bxInstruction_c *i)
{
  write_virtual_word_32(i->seg(), i->Id(), AX);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GwEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_virtual_byte(i->seg(), eaddr);

  /* zero extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->dst(), (Bit16u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GwEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* zero extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->dst(), (Bit16u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GwEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_virtual_byte(i->seg(), eaddr);

  /* sign extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GwEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(),i->extend8bitL());

  /* sign extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  op1_16 = read_RMW_virtual_word(i->seg(), eaddr);
  op2_16 = BX_READ_16BIT_REG(i->src());

  write_RMW_virtual_word(op2_16);
  BX_WRITE_16BIT_REG(i->src(), op1_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

#if BX_DEBUGGER
  // Note for mortals: the instruction to trigger this is "xchgw %bx,%bx"
  if (bx_dbg.magic_break_enabled && (i->src() == 3) && (i->dst() == 3))
  {
    BX_CPU_THIS_PTR magic_break = 1;
    BX_NEXT_INSTR(i);
  }
#endif

  op1_16 = BX_READ_16BIT_REG(i->dst());
  op2_16 = BX_READ_16BIT_REG(i->src());
  
  BX_WRITE_16BIT_REG(i->src(), op1_16);
  BX_WRITE_16BIT_REG(i->dst(), op2_16);

  BX_NEXT_INSTR(i);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVO_GwEwR(bxInstruction_c *i)
{
  if (get_OF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNO_GwEwR(bxInstruction_c *i)
{
  if (!get_OF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVB_GwEwR(bxInstruction_c *i)
{
  if (get_CF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNB_GwEwR(bxInstruction_c *i)
{
  if (!get_CF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVZ_GwEwR(bxInstruction_c *i)
{
  if (get_ZF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNZ_GwEwR(bxInstruction_c *i)
{
  if (!get_ZF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVBE_GwEwR(bxInstruction_c *i)
{
  if (get_CF() || get_ZF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNBE_GwEwR(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF()))
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVS_GwEwR(bxInstruction_c *i)
{
  if (get_SF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNS_GwEwR(bxInstruction_c *i)
{
  if (!get_SF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVP_GwEwR(bxInstruction_c *i)
{
  if (get_PF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNP_GwEwR(bxInstruction_c *i)
{
  if (!get_PF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVL_GwEwR(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNL_GwEwR(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF())
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVLE_GwEwR(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNLE_GwEwR(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_16BIT_REG(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}
