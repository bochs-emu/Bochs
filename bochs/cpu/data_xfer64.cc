/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RRXIq(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), i->Iq());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64_GdEdM(bxInstruction_c *i)
{
  Bit64u eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val32 = read_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr));
  BX_WRITE_32BIT_REGZ(i->dst(), val32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64_EdGdM(bxInstruction_c *i)
{
  Bit64u eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr), BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64S_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  stack_write_qword(eaddr, BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GqEqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit64u val64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));

  BX_WRITE_64BIT_REG(i->dst(), val64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV64S_GqEqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_WRITE_64BIT_REG(i->dst(), stack_read_qword(eaddr));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GqEqR(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEA_GqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_WRITE_64BIT_REG(i->dst(), eaddr);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_ALOq(bxInstruction_c *i)
{
  AL = read_linear_byte(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqAL(bxInstruction_c *i)
{
  write_linear_byte(i->seg(), get_laddr64(i->seg(), i->Iq()), AL);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_AXOq(bxInstruction_c *i)
{
  AX = read_linear_word(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqAX(bxInstruction_c *i)
{
  write_linear_word(i->seg(), get_laddr64(i->seg(), i->Iq()), AX);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EAXOq(bxInstruction_c *i)
{
  RAX = read_linear_dword(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqEAX(bxInstruction_c *i)
{
  write_linear_dword(i->seg(), get_laddr64(i->seg(), i->Iq()), EAX);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_RAXOq(bxInstruction_c *i)
{
  RAX = read_linear_qword(i->seg(), get_laddr64(i->seg(), i->Iq()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OqRAX(bxInstruction_c *i)
{
  write_linear_qword(i->seg(), get_laddr64(i->seg(), i->Iq()), RAX);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqIdM(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), op_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EqIdR(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();
  BX_WRITE_64BIT_REG(i->dst(), op_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_linear_byte(i->seg(), get_laddr64(i->seg(), eaddr));

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op2_16 = read_linear_word(i->seg(), get_laddr64(i->seg(), eaddr));

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit64u) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_linear_byte(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op2_16 = read_linear_word(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op2_32 = read_linear_dword(i->seg(), get_laddr64(i->seg(), eaddr));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit32s) op2_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GqEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->dst(), (Bit32s) op2_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EqGqM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit64u op1_64 = read_RMW_virtual_qword_64(i->seg(), get_laddr64(i->seg(), eaddr));
  Bit64u op2_64 = BX_READ_64BIT_REG(i->src());

  write_RMW_virtual_qword(op2_64);
  BX_WRITE_64BIT_REG(i->src(), op1_64);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EqGqR(bxInstruction_c *i)
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVO_GqEqR(bxInstruction_c *i)
{
  if (get_OF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNO_GqEqR(bxInstruction_c *i)
{
  if (!get_OF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVB_GqEqR(bxInstruction_c *i)
{
  if (get_CF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNB_GqEqR(bxInstruction_c *i)
{
  if (!get_CF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVZ_GqEqR(bxInstruction_c *i)
{
  if (get_ZF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNZ_GqEqR(bxInstruction_c *i)
{
  if (!get_ZF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVBE_GqEqR(bxInstruction_c *i)
{
  if (get_CF() || get_ZF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNBE_GqEqR(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF()))
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVS_GqEqR(bxInstruction_c *i)
{
  if (get_SF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNS_GqEqR(bxInstruction_c *i)
{
  if (!get_SF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVP_GqEqR(bxInstruction_c *i)
{
  if (get_PF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNP_GqEqR(bxInstruction_c *i)
{
  if (!get_PF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVL_GqEqR(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNL_GqEqR(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF())
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVLE_GqEqR(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNLE_GqEqR(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_64BIT_REG(i->dst(), BX_READ_64BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

#endif /* if BX_SUPPORT_X86_64 */
