/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer64.cc,v 1.32 2007-12-20 20:58:37 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


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

void BX_CPU_C::MOV_EqGqM(bxInstruction_c *i)
{
  write_virtual_qword(i->seg(), RMAddr(i), BX_READ_64BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_EqGqR(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->nnn());
  BX_WRITE_64BIT_REG(i->rm(), op2_64);
}

void BX_CPU_C::MOV_GqEqM(bxInstruction_c *i)
{
  Bit64u val64 = read_virtual_qword(i->seg(), RMAddr(i));
  BX_WRITE_64BIT_REG(i->nnn(), val64);
}

void BX_CPU_C::MOV_GqEqR(bxInstruction_c *i)
{
  Bit64u op2_64 = BX_READ_64BIT_REG(i->rm());
  BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::LEA_GqM(bxInstruction_c *i)
{
  BX_WRITE_64BIT_REG(i->nnn(), RMAddr(i));
}

void BX_CPU_C::MOV_ALOq(bxInstruction_c *i)
{
  AL = read_virtual_byte(i->seg(), i->Iq());
}

void BX_CPU_C::MOV_OqAL(bxInstruction_c *i)
{
  write_virtual_byte(i->seg(), i->Iq(), AL);
}

void BX_CPU_C::MOV_AXOq(bxInstruction_c *i)
{
  AX = read_virtual_word(i->seg(), i->Iq());
}

void BX_CPU_C::MOV_OqAX(bxInstruction_c *i)
{
  write_virtual_word(i->seg(), i->Iq(), AX);
}

void BX_CPU_C::MOV_EAXOq(bxInstruction_c *i)
{
  RAX = read_virtual_dword(i->seg(), i->Iq());
}

void BX_CPU_C::MOV_OqEAX(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), i->Iq(), EAX);
}

void BX_CPU_C::MOV_RAXOq(bxInstruction_c *i)
{
  RAX = read_virtual_qword(i->seg(), i->Iq());
}

void BX_CPU_C::MOV_OqRAX(bxInstruction_c *i)
{
  write_virtual_qword(i->seg(), i->Iq(), RAX);
}

void BX_CPU_C::MOV_EqIdM(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();
  write_virtual_qword(i->seg(), RMAddr(i), op_64);
}

void BX_CPU_C::MOV_EqIdR(bxInstruction_c *i)
{
  Bit64u op_64 = (Bit32s) i->Id();
  BX_WRITE_64BIT_REG(i->rm(), op_64);
}

void BX_CPU_C::MOVZX_GqEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_8);
}

void BX_CPU_C::MOVZX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  /* zero extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_8);
}

void BX_CPU_C::MOVZX_GqEwM(bxInstruction_c *i)
{
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_16);
}

void BX_CPU_C::MOVZX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->rm());

  /* zero extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit64u) op2_16);
}

void BX_CPU_C::MOVSX_GqEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GqEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  /* sign extend byte op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GqEwM(bxInstruction_c *i)
{
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit16s) op2_16);
}

void BX_CPU_C::MOVSX_GqEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->rm());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit16s) op2_16);
}

void BX_CPU_C::MOVSX_GqEdM(bxInstruction_c *i)
{
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit32s) op2_32);
}

void BX_CPU_C::MOVSX_GqEdR(bxInstruction_c *i)
{
  Bit32u op2_32 = BX_READ_32BIT_REG(i->rm());

  /* sign extend word op2 into qword op1 */
  BX_WRITE_64BIT_REG(i->nnn(), (Bit32s) op2_32);
}

void BX_CPU_C::XCHG_EqGqM(bxInstruction_c *i)
{
  Bit64u op2_64, op1_64;

  op1_64 = read_RMW_virtual_qword(i->seg(), RMAddr(i));
  op2_64 = BX_READ_64BIT_REG(i->nnn());
  write_RMW_virtual_qword(op2_64);

  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
}

void BX_CPU_C::XCHG_EqGqR(bxInstruction_c *i)
{
  Bit64u op1_64 = BX_READ_64BIT_REG(i->rm());
  Bit64u op2_64 = BX_READ_64BIT_REG(i->nnn());

  BX_WRITE_64BIT_REG(i->nnn(), op1_64);
  BX_WRITE_64BIT_REG(i->rm(), op2_64);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

void BX_CPU_C::CMOVO_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_OF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVO_GqEqR(bxInstruction_c *i)
{
  if (get_OF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNO_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (!get_OF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNO_GqEqR(bxInstruction_c *i)
{
  if (!get_OF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVB_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_CF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVB_GqEqR(bxInstruction_c *i)
{
  if (get_CF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNB_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (!get_CF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNB_GqEqR(bxInstruction_c *i)
{
  if (!get_CF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVZ_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVZ_GqEqR(bxInstruction_c *i)
{
  if (get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNZ_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (!get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNZ_GqEqR(bxInstruction_c *i)
{
  if (!get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVBE_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_CF() || get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVBE_GqEqR(bxInstruction_c *i)
{
  if (get_CF() || get_ZF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNBE_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (! (get_CF() || get_ZF()))
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNBE_GqEqR(bxInstruction_c *i)
{
  if (! (get_CF() || get_ZF()))
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVS_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_SF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVS_GqEqR(bxInstruction_c *i)
{
  if (get_SF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNS_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (!get_SF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNS_GqEqR(bxInstruction_c *i)
{
  if (!get_SF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVP_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_PF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVP_GqEqR(bxInstruction_c *i)
{
  if (get_PF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNP_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (!get_PF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNP_GqEqR(bxInstruction_c *i)
{
  if (!get_PF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVL_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (getB_SF() != getB_OF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVL_GqEqR(bxInstruction_c *i)
{
  if (getB_SF() != getB_OF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNL_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (getB_SF() == getB_OF())
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNL_GqEqR(bxInstruction_c *i)
{
  if (getB_SF() == getB_OF())
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVLE_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVLE_GqEqR(bxInstruction_c *i)
{
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

void BX_CPU_C::CMOVNLE_GqEqM(bxInstruction_c *i)
{
  Bit64u op2_64 = read_virtual_qword(i->seg(), RMAddr(i));

  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_64BIT_REG(i->nnn(), op2_64);
}

void BX_CPU_C::CMOVNLE_GqEqR(bxInstruction_c *i)
{
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_64BIT_REG(i->nnn(), BX_READ_64BIT_REG(i->rm()));
}

#endif /* if BX_SUPPORT_X86_64 */
