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

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KADDQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = BX_READ_OPMASK(i->src1()) + BX_READ_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = BX_READ_OPMASK(i->src1()) & BX_READ_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDNQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = ~(BX_READ_OPMASK(i->src1())) & BX_READ_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVQ_KGqKEqM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit64u opmask = read_virtual_qword(i->seg(), eaddr);
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVQ_KGqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVQ_KEqKGqM(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_qword(i->seg(), eaddr, BX_READ_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVQ_KGqEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), BX_READ_64BIT_REG(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVQ_GqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_64BIT_REG(i->dst(), BX_READ_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KUNPCKDQ_KGqKHdKEdR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = BX_READ_32BIT_OPMASK(i->src1());
         opmask = (opmask << 32) | BX_READ_32BIT_OPMASK(i->src2());

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KNOTQ_KGqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  BX_WRITE_OPMASK(i->dst(), ~BX_READ_OPMASK(i->src()));
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = BX_READ_OPMASK(i->src1()) | BX_READ_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KORTESTQ_KGqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u tmp = BX_READ_OPMASK(i->src1()) | BX_READ_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if (tmp == 0)
    assert_ZF();
  else if (tmp == BX_CONST64(0xffffffffffffffff))
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTLQ_KGqKEqIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit64u opmask = 0;
  if (count < 64)
    opmask = BX_READ_OPMASK(i->src()) << count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTRQ_KGqKEqIbR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  unsigned count = i->Ib();
  Bit64u opmask = 0;
  if (count < 64)
    opmask = BX_READ_OPMASK(i->src()) >> count;

  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXNORQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = ~(BX_READ_OPMASK(i->src1()) ^ BX_READ_OPMASK(i->src2()));
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KXORQ_KGqKHqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u opmask = BX_READ_OPMASK(i->src1()) ^ BX_READ_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);
#endif

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::KTESTQ_KGqKEqR(bxInstruction_c *i)
{
#if BX_SUPPORT_EVEX
  Bit64u op1 = BX_READ_OPMASK(i->src1()), op2 = BX_READ_OPMASK(i->src2());
  clearEFlagsOSZAPC();
  if ((op1 & op2) == 0)
    assert_ZF();
  if ((~op1 & op2) == 0)
    assert_CF();
#endif

  BX_NEXT_INSTR(i);
}

#endif
