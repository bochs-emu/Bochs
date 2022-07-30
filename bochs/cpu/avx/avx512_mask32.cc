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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KADDD_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->src1()) + BX_READ_32BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDD_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->src1()) & BX_READ_32BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDND_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = ~(BX_READ_32BIT_OPMASK(i->src1())) & BX_READ_32BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVD_KGdKEdM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit32u opmask = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVD_KGdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_32BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVD_KEdKGdM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_dword(i->seg(), eaddr, BX_READ_32BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVD_KGdEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_32BIT_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVD_GdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_32BIT_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KUNPCKWD_KGdKHwKEwR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = BX_READ_16BIT_OPMASK(i->src1());
         opmask = (opmask << 16) | BX_READ_16BIT_OPMASK(i->src2());

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KNOTD_KGdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = ~BX_READ_32BIT_OPMASK(i->src());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORD_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->src1()) | BX_READ_32BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORTESTD_KGdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u tmp = BX_READ_32BIT_OPMASK(i->src1()) | BX_READ_32BIT_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if (tmp == 0)
    assert_ZF();
  else if (tmp == 0xffffffff)
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTLD_KGdKEdIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit32u opmask = 0;
  if (count < 32)
    opmask = BX_READ_32BIT_OPMASK(i->src()) << count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTRD_KGdKEdIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit32u opmask = 0;
  if (count < 32)
    opmask = BX_READ_32BIT_OPMASK(i->src()) >> count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXNORD_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = ~(BX_READ_32BIT_OPMASK(i->src1()) ^ BX_READ_32BIT_OPMASK(i->src2()));
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXORD_KGdKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u opmask = BX_READ_32BIT_OPMASK(i->src1()) ^ BX_READ_32BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KTESTD_KGdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit32u op1 = BX_READ_32BIT_OPMASK(i->src1()), op2 = BX_READ_32BIT_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if ((op1 & op2) == 0)
    assert_ZF();
  if ((~op1 & op2) == 0)
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

#endif
