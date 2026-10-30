/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2026  The Bochs Project
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

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RRXIq(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), i->Iq());

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64_GdEdM(bxInstruction_c *i)
{
  Bit64u eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  Bit32u val32 = read_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr));
  BX_WRITE_32BIT_REGZ(i->dst(), val32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64_EdGdM(bxInstruction_c *i)
{
  Bit64u eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  write_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr), BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64S_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  stack_write_qword(eaddr, BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GqEqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  Bit64u val64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  BX_WRITE_64BIT_REG(i->dst(), val64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64S_GqEqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  BX_WRITE_64BIT_REG(i->dst(), stack_read_qword(eaddr));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GqEqR(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::LEA_GqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  BX_WRITE_64BIT_REG(i->dst(), eaddr);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_ALOq(bxInstruction_c *i)
{
  AL = read_linear_byte(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqAL(bxInstruction_c *i)
{
  write_linear_byte(i->seg(), get_laddr64(i->seg(), i->Iq()), AL);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_AXOq(bxInstruction_c *i)
{
  AX = read_linear_word(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqAX(bxInstruction_c *i)
{
  write_linear_word(i->seg(), get_laddr64(i->seg(), i->Iq()), AX);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EAXOq(bxInstruction_c *i)
{
  RAX = read_linear_dword(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqEAX(bxInstruction_c *i)
{
  write_linear_dword(i->seg(), get_laddr64(i->seg(), i->Iq()), EAX);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RAXOq(bxInstruction_c *i)
{
  RAX = read_linear_qword(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqRAX(bxInstruction_c *i)
{
  write_linear_qword(i->seg(), get_laddr64(i->seg(), i->Iq()), RAX);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqIdM(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();

  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), op_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqIdR(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();
  BX_WRITE_64BIT_REG(i->dst(), op_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit8u op2_8 = read_linear_byte(i->seg(), get_laddr64(i->seg(), eaddr));

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit16u op2_16 = read_linear_word(i->seg(), get_laddr64(i->seg(), eaddr));

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit8u op2_8 = read_linear_byte(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit16u op2_16 = read_linear_word(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);

  Bit32u op2_32 = read_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit32s) op2_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit32s) op2_32);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  Bit64u op1_64 = read_RMW_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr)); // always locked
  write_RMW_linear_qword(BX_READ_64BIT_REG(i->src()));
  BX_WRITE_64BIT_REG(i->src(), op1_64);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->dst());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  BX_WRITE_64BIT_REG(i->src(), op1_64);
  BX_WRITE_64BIT_REG(i->dst(), op2_64);

  BX_NEXT_INSTR(i);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

// CMOVcc Gq, Eq: move when the condition is true
#define CMOV_GqEqR(HANDLER, cond)                                                         \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    if (cond)                                                                             \
      BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));                          \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

CMOV_GqEqR(CMOVO_GqEqR, get_OF())
CMOV_GqEqR(CMOVNO_GqEqR, !get_OF())
CMOV_GqEqR(CMOVB_GqEqR, get_CF())
CMOV_GqEqR(CMOVNB_GqEqR, !get_CF())
CMOV_GqEqR(CMOVZ_GqEqR, get_ZF())
CMOV_GqEqR(CMOVNZ_GqEqR, !get_ZF())
CMOV_GqEqR(CMOVBE_GqEqR, get_CF() || get_ZF())
CMOV_GqEqR(CMOVNBE_GqEqR, ! (get_CF() || get_ZF()))
CMOV_GqEqR(CMOVS_GqEqR, get_SF())
CMOV_GqEqR(CMOVNS_GqEqR, !get_SF())
CMOV_GqEqR(CMOVP_GqEqR, get_PF())
CMOV_GqEqR(CMOVNP_GqEqR, !get_PF())
CMOV_GqEqR(CMOVL_GqEqR, getB_SF() != getB_OF())
CMOV_GqEqR(CMOVNL_GqEqR, getB_SF() == getB_OF())
CMOV_GqEqR(CMOVLE_GqEqR, get_ZF() || (getB_SF() != getB_OF()))
CMOV_GqEqR(CMOVNLE_GqEqR, ! get_ZF() && (getB_SF() == getB_OF()))

#endif /* if BX_SUPPORT_X86_64 */
