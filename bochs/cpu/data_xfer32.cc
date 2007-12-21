/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer32.cc,v 1.51 2007-12-21 18:24:19 sshwarts Exp $
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

#if BX_SUPPORT_X86_64==0
// Make life easier for merging cpu64 and cpu32 code.
#define RAX EAX
#endif

void BX_CPU_C::XCHG_ERXEAX(bxInstruction_c *i)
{
#if BX_SUPPORT_X86_64
  if (i->opcodeReg() == 0)  // 'xchg eax, eax' is NOP even in 64-bit mode
    return;
#endif

  Bit32u temp32 = EAX;
  RAX = BX_READ_32BIT_REG(i->opcodeReg());
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), temp32);
}

void BX_CPU_C::MOV_ERXId(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), i->Id());
}

void BX_CPU_C::MOV_EdGdM(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), RMAddr(i), BX_READ_32BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_EdGdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->rm(), BX_READ_32BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_GdEdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
}

void BX_CPU_C::MOV_GdEdM(bxInstruction_c *i)
{
  Bit32u val32 = read_virtual_dword(i->seg(), RMAddr(i));
  BX_WRITE_32BIT_REGZ(i->nnn(), val32);
}

void BX_CPU_C::LEA_GdM(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->nnn(), RMAddr(i));
}

void BX_CPU_C::MOV_EAXOd(bxInstruction_c *i)
{
  RAX = read_virtual_dword(i->seg(), i->Id());
}

void BX_CPU_C::MOV_OdEAX(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), i->Id(), EAX);
}

void BX_CPU_C::MOV_EdIdM(bxInstruction_c *i)
{
  write_virtual_dword(i->seg(), RMAddr(i), i->Id());
}

void BX_CPU_C::MOV_EdIdR(bxInstruction_c *i)
{
  BX_WRITE_32BIT_REGZ(i->rm(), i->Id());
}

void BX_CPU_C::MOVZX_GdEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* zero extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_8);
}

void BX_CPU_C::MOVZX_GdEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  /* zero extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_8);
}

void BX_CPU_C::MOVZX_GdEwM(bxInstruction_c *i)
{
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  /* zero extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_16);
}

void BX_CPU_C::MOVZX_GdEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->rm());

  /* zero extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit32u) op2_16);
}

void BX_CPU_C::MOVSX_GdEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* sign extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GdEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  /* sign extend byte op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GdEwM(bxInstruction_c *i)
{
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  /* sign extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit16s) op2_16);
}

void BX_CPU_C::MOVSX_GdEwR(bxInstruction_c *i)
{
  Bit16u op2_16 = BX_READ_16BIT_REG(i->rm());

  /* sign extend word op2 into dword op1 */
  BX_WRITE_32BIT_REGZ(i->nnn(), (Bit16s) op2_16);
}

void BX_CPU_C::XCHG_EdGdM(bxInstruction_c *i)
{
  Bit32u op2_32, op1_32;

  op1_32 = read_RMW_virtual_dword(i->seg(), RMAddr(i));
  op2_32 = BX_READ_32BIT_REG(i->nnn());
  write_RMW_virtual_dword(op2_32);

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
}

void BX_CPU_C::XCHG_EdGdR(bxInstruction_c *i)
{
  Bit32u op1_32 = BX_READ_32BIT_REG(i->rm());
  Bit32u op2_32 = BX_READ_32BIT_REG(i->nnn());

  BX_WRITE_32BIT_REGZ(i->nnn(), op1_32);
  BX_WRITE_32BIT_REGZ(i->rm(),  op2_32);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

void BX_CPU_C::CMOVO_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVO_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVO_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVO_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNO_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (!get_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNO_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNO_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNO_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVB_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_CF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVB_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVB_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_CF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVB_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNB_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (!get_CF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNB_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNB_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_CF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNB_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVZ_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVZ_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVZ_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVZ_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNZ_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (!get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNZ_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNZ_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNZ_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVBE_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_CF() || get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVBE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVBE_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_CF() || get_ZF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVBE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNBE_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (! (get_CF() || get_ZF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);
  
  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNBE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNBE_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (! (get_CF() || get_ZF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNBE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVS_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_SF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVS_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVS_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_SF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVS_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNS_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (!get_SF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNS_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNS_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_SF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNS_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVP_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_PF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVP_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVP_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_PF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVP_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNP_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (!get_PF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNP_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNP_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_PF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNP_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVL_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (getB_SF() != getB_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVL_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVL_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (getB_SF() != getB_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVL_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNL_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (getB_SF() == getB_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNL_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNL_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (getB_SF() == getB_OF())
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNL_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVLE_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVLE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVLE_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVLE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNLE_GdEdM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit32u op2_32 = read_virtual_dword(i->seg(), RMAddr(i));

  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), op2_32);

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNLE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNLE_GdEdR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_32BIT_REGZ(i->nnn(), BX_READ_32BIT_REG(i->rm()));

  BX_CLEAR_64BIT_HIGH(i->nnn()); // always clear upper part of the register
#else
  BX_INFO(("CMOVNLE_GdEd: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}
