/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014-2018 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_AVX

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KADDB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = BX_READ_8BIT_OPMASK(i->src1()) + BX_READ_8BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = BX_READ_8BIT_OPMASK(i->src1()) & BX_READ_8BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDNB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = ~(BX_READ_8BIT_OPMASK(i->src1())) & BX_READ_8BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVB_KGbKEbM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit8u opmask = read_virtual_byte(i->seg(), eaddr);
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVB_KGbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_8BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVB_KEbKGbM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_byte(i->seg(), eaddr, BX_READ_8BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVB_KGbEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_8BIT_REGL(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVB_GdKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_8BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KNOTB_KGbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = ~BX_READ_8BIT_OPMASK(i->src());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = BX_READ_8BIT_OPMASK(i->src1()) | BX_READ_8BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORTESTB_KGbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u tmp = BX_READ_8BIT_OPMASK(i->src1()) | BX_READ_8BIT_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if (tmp == 0)
    assert_ZF();
  else if (tmp == 0xff)
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTLB_KGbKEbIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit8u opmask = 0;
  if (count < 8)
    opmask = BX_READ_8BIT_OPMASK(i->src()) << count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTRB_KGbKEbIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit8u opmask = 0;
  if (count < 8)
    opmask = BX_READ_8BIT_OPMASK(i->src()) >> count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXNORB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = ~(BX_READ_8BIT_OPMASK(i->src1()) ^ BX_READ_8BIT_OPMASK(i->src2()));
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXORB_KGbKHbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u opmask = BX_READ_8BIT_OPMASK(i->src1()) ^ BX_READ_8BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KTESTB_KGbKEbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit8u op1 = BX_READ_8BIT_OPMASK(i->src1()), op2 = BX_READ_8BIT_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if ((op1 & op2) == 0)
    assert_ZF();
  if ((~op1 & op2) == 0)
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

#endif
