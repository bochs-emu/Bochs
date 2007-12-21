/////////////////////////////////////////////////////////////////////////
// $Id: data_xfer16.cc,v 1.52 2007-12-21 18:24:19 sshwarts Exp $
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


void BX_CPU_C::MOV_RXIw(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->opcodeReg(), i->Iw());
}

void BX_CPU_C::XCHG_RXAX(bxInstruction_c *i)
{
  Bit16u temp16 = AX;
  AX = BX_READ_16BIT_REG(i->opcodeReg());
  BX_WRITE_16BIT_REG(i->opcodeReg(), temp16);
}

void BX_CPU_C::MOV_EwGwM(bxInstruction_c *i)
{
  write_virtual_word(i->seg(), RMAddr(i), BX_READ_16BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_EwGwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->rm(), BX_READ_16BIT_REG(i->nnn()));
}

void BX_CPU_C::MOV_GwEwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
}

void BX_CPU_C::MOV_GwEwM(bxInstruction_c *i)
{
  Bit16u val16 = read_virtual_word(i->seg(), RMAddr(i));
  BX_WRITE_16BIT_REG(i->nnn(), val16);
}

void BX_CPU_C::MOV_EwSw(bxInstruction_c *i)
{
  /* Illegal to use nonexisting segments */
  if (i->nnn() >= 6) {
    BX_INFO(("MOV_EwSw: using of nonexisting segment register %d", i->nnn()));
    UndefinedOpcode(i);
  }

  Bit16u seg_reg = BX_CPU_THIS_PTR sregs[i->nnn()].selector.value;

  if (i->modC0()) {
    if ( i->os32L() ) {
      BX_WRITE_32BIT_REGZ(i->rm(), seg_reg);
    }
    else {
      BX_WRITE_16BIT_REG(i->rm(), seg_reg);
    }
  }
  else {
    write_virtual_word(i->seg(), RMAddr(i), seg_reg);
  }
}

void BX_CPU_C::MOV_SwEw(bxInstruction_c *i)
{
  Bit16u op2_16;

  /* Attempt to load CS or nonexisting segment register */
  if (i->nnn() >= 6 || i->nnn() == BX_SEG_REG_CS) {
    BX_INFO(("MOV_EwSw: can't use this segment register %d", i->nnn()));
    UndefinedOpcode(i);
  }

  if (i->modC0()) {
    op2_16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    op2_16 = read_virtual_word(i->seg(), RMAddr(i));
  }

  load_seg_reg(&BX_CPU_THIS_PTR sregs[i->nnn()], op2_16);

  if (i->nnn() == BX_SEG_REG_SS) {
    // MOV SS inhibits interrupts, debug exceptions and single-step
    // trap exceptions until the execution boundary following the
    // next instruction is reached.
    // Same code as POP_SS()
    BX_CPU_THIS_PTR inhibit_mask |= BX_INHIBIT_INTERRUPTS | BX_INHIBIT_DEBUG;
    BX_CPU_THIS_PTR async_event = 1;
  }
}

void BX_CPU_C::LEA_GwM(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->nnn(), (Bit16u) RMAddr(i));
}

void BX_CPU_C::MOV_AXOd(bxInstruction_c *i)
{
  AX = read_virtual_word(i->seg(), i->Id());
}

void BX_CPU_C::MOV_OdAX(bxInstruction_c *i)
{
  write_virtual_word(i->seg(), i->Id(), AX);
}

void BX_CPU_C::MOV_EwIwM(bxInstruction_c *i)
{
  write_virtual_word(i->seg(), RMAddr(i), i->Iw());
}

void BX_CPU_C::MOV_EwIwR(bxInstruction_c *i)
{
  BX_WRITE_16BIT_REG(i->rm(), i->Iw());
}

#if BX_CPU_LEVEL >= 3
void BX_CPU_C::MOVZX_GwEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* zero extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->nnn(), (Bit16u) op2_8);
}

void BX_CPU_C::MOVZX_GwEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  /* zero extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->nnn(), (Bit16u) op2_8);
}

void BX_CPU_C::MOVSX_GwEbM(bxInstruction_c *i)
{
  Bit8u op2_8 = read_virtual_byte(i->seg(), RMAddr(i));

  /* sign extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->nnn(), (Bit8s) op2_8);
}

void BX_CPU_C::MOVSX_GwEbR(bxInstruction_c *i)
{
  Bit8u op2_8 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());

  /* sign extend byte op2 into word op1 */
  BX_WRITE_16BIT_REG(i->nnn(), (Bit8s) op2_8);
}
#endif

void BX_CPU_C::XCHG_EwGwM(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

  op1_16 = read_RMW_virtual_word(i->seg(), RMAddr(i));
  op2_16 = BX_READ_16BIT_REG(i->nnn());

  write_RMW_virtual_word(op2_16);
  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
}

void BX_CPU_C::XCHG_EwGwR(bxInstruction_c *i)
{
  Bit16u op1_16, op2_16;

#if BX_DEBUGGER && BX_MAGIC_BREAKPOINT
  // Note for mortals: the instruction to trigger this is "xchgw %bx,%bx"
  if (bx_dbg.magic_break_enabled && (i->nnn() == 3) && (i->rm() == 3))
  {
    BX_CPU_THIS_PTR magic_break = 1;
    return;
  }
#endif

  op1_16 = BX_READ_16BIT_REG(i->rm());
  op2_16 = BX_READ_16BIT_REG(i->nnn());

  BX_WRITE_16BIT_REG(i->nnn(), op1_16);
  BX_WRITE_16BIT_REG(i->rm(),  op2_16);
}

// Note: CMOV accesses a memory source operand (read), regardless
//       of whether condition is true or not.  Thus, exceptions may
//       occur even if the MOV does not take place.

void BX_CPU_C::CMOVO_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_OF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVO_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVO_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_OF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVO_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNO_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (!get_OF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNO_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNO_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_OF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNO_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVB_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_CF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVB_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVB_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_CF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVB_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNB_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (!get_CF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNB_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNB_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_CF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNB_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVZ_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVZ_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVZ_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVZ_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNZ_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (!get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNZ_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNZ_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNZ_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVBE_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_CF() || get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVBE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVBE_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_CF() || get_ZF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVBE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNBE_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (! (get_CF() || get_ZF()))
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNBE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNBE_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (! (get_CF() || get_ZF()))
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNBE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVS_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_SF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVS_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVS_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_SF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVS_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNS_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (!get_SF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNS_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNS_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_SF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNS_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVP_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_PF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVP_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVP_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_PF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVP_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNP_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (!get_PF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNP_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNP_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (!get_PF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNP_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVL_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (getB_SF() != getB_OF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVL_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVL_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (getB_SF() != getB_OF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVL_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNL_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (getB_SF() == getB_OF())
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNL_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNL_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (getB_SF() == getB_OF())
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNL_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVLE_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVLE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVLE_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (get_ZF() || (getB_SF() != getB_OF()))
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVLE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNLE_GwEwM(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  Bit16u op2_16 = read_virtual_word(i->seg(), RMAddr(i));

  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_16BIT_REG(i->nnn(), op2_16);
#else
  BX_INFO(("CMOVNLE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CMOVNLE_GwEwR(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 6
  if (! get_ZF() && (getB_SF() == getB_OF()))
    BX_WRITE_16BIT_REG(i->nnn(), BX_READ_16BIT_REG(i->rm()));
#else
  BX_INFO(("CMOVNLE_GwEw: --enable-cpu-level=6 required"));
  UndefinedOpcode(i);
#endif
}
