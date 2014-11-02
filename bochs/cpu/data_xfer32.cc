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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EdIdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  write_virtual_dword(i->seg(), eaddr, i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EdIdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->dst(), i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV32_EdGdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_dword_32(i->seg(), eaddr, BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV32S_EdGdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  stack_write_dword(eaddr, BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_GdEdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV32_GdEdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val32 = read_virtual_dword_32(i->seg(), eaddr);

  BX_WRITE_32BIT_REGZ(i->dst(), val32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV32S_GdEdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
  Bit32u val32 = stack_read_dword(eaddr);

  BX_WRITE_32BIT_REGZ(i->dst(), val32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::LEA_GdM(bxInstruction_c *i)
{
  Bit32u eaddr = (Bit32u) BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  BX_WRITE_32BIT_REGZ(i->dst(), eaddr);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_EAXOd(bxInstruction_c *i)
{
  RAX = read_virtual_dword_32(i->seg(), i->Id());

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOV_OdEAX(bxInstruction_c *i)
{
  write_virtual_dword_32(i->seg(), i->Id(), EAX);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GdEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_virtual_byte(i->seg(), eaddr);

  /* zero extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GdEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* zero extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GdEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op2_16 = read_virtual_word(i->seg(), eaddr);

  /* zero extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVZX_GdEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* zero extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit32u) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GdEbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u op2_8 = read_virtual_byte(i->seg(), eaddr);

  /* sign extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GdEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->src(), i->extend8bitL());

  /* sign extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit8s) op2_8);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GdEwM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u op2_16 = read_virtual_word(i->seg(), eaddr);

  /* sign extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVSX_GdEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->src());

  /* sign extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->dst(), (Bit16s) op2_16);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EdGdM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit32u op1_32 = read_RMW_virtual_dword(i->seg(), eaddr);
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());
  write_RMW_virtual_dword(op2_32);
  BX_WRITE_32BIT_REGZ(i->src(), op1_32);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::XCHG_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->dst());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->src());

  BX_WRITE_32BIT_REGZ(i->src(), op1_32);
  BX_WRITE_32BIT_REGZ(i->dst(), op2_32);

  BX_NEXT_INSTR(i);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVO_GdEdR(bxInstruction_c *i)
{
  if (get_OF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNO_GdEdR(bxInstruction_c *i)
{
  if (!get_OF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVB_GdEdR(bxInstruction_c *i)
{
  if (get_CF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNB_GdEdR(bxInstruction_c *i)
{
  if (!get_CF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVZ_GdEdR(bxInstruction_c *i)
{
  if (get_ZF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNZ_GdEdR(bxInstruction_c *i)
{
  if (!get_ZF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVBE_GdEdR(bxInstruction_c *i)
{
  if (get_CF() || get_ZF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNBE_GdEdR(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF()))
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVS_GdEdR(bxInstruction_c *i)
{
  if (get_SF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNS_GdEdR(bxInstruction_c *i)
{
  if (!get_SF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVP_GdEdR(bxInstruction_c *i)
{
  if (get_PF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNP_GdEdR(bxInstruction_c *i)
{
  if (!get_PF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVL_GdEdR(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNL_GdEdR(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF())
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVLE_GdEdR(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::CMOVNLE_GdEdR(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_REG(i->src()));

  BX_CLEAR_64BIT_HIGH(i->dst()); // always clear upper part of the register

  BX_NEXT_INSTR(i);
}
