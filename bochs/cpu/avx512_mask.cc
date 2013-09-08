/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2013 Stanislav Shwartsman
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

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDW_KGwKHwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = BX_READ_16BIT_OPMASK(i->src1()) & BX_READ_16BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KANDNW_KGwKHwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = ~(BX_READ_16BIT_OPMASK(i->src1())) & BX_READ_16BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVW_KGwKEwM(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit16u opmask = read_virtual_word(i->seg(), eaddr);

  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVW_KGwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_WRITE_OPMASK(i->dst(), BX_READ_16BIT_OPMASK(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVW_KEwKGwM(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  write_virtual_word(i->seg(), eaddr, BX_READ_16BIT_OPMASK(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVW_KGwEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_WRITE_OPMASK(i->dst(), BX_READ_16BIT_REG(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KMOVW_GdKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  BX_WRITE_32BIT_REGZ(i->dst(), BX_READ_16BIT_OPMASK(i->src()));

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KUNPCKBW_KGwKHwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = BX_READ_8BIT_OPMASK(i->src1());
         opmask = (opmask << 8) | BX_READ_8BIT_OPMASK(i->src2());

  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KNOTW_KGwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = ~BX_READ_16BIT_OPMASK(i->src());
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KORW_KGwKHwKEwR(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = BX_READ_16BIT_OPMASK(i->src1()) | BX_READ_16BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KORTESTW_KGwKEw(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u tmp = BX_READ_16BIT_OPMASK(i->src1()) | BX_READ_16BIT_OPMASK(i->src2());
  if (tmp == 0)
    setEFlagsOSZAPC(EFlagsZFMask);
  else if (tmp == 0xffff)
    setEFlagsOSZAPC(EFlagsCFMask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTLW_KGwKEw(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned count = i->Ib();
  Bit16u opmask = 0;
  if (count < 15)
    opmask = BX_READ_16BIT_OPMASK(i->src()) << count;

  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KSHIFTRW_KGwKEw(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  unsigned count = i->Ib();
  Bit16u opmask = 0;
  if (count < 15)
    opmask = BX_READ_16BIT_OPMASK(i->src()) >> count;

  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KXNORW_KGwKHwKEw(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = ~(BX_READ_16BIT_OPMASK(i->src1()) ^ BX_READ_16BIT_OPMASK(i->src2()));
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

BX_INSF_TYPE BX_CPP_AttrRegparmN(1) BX_CPU_C::KXORW_KGwKHwKEw(bxInstruction_c *i)
{
  if (! BX_CPU_THIS_PTR xcr0.get_OPMASK()) {
    BX_ERROR(("%s: XCR0.OPMASK should be set", i->getIaOpcodeName()));
    exception(BX_UD_EXCEPTION, 0);
  }

  Bit16u opmask = BX_READ_16BIT_OPMASK(i->src1()) ^ BX_READ_16BIT_OPMASK(i->src2());
  BX_WRITE_OPMASK(i->dst(), opmask);

  BX_NEXT_INSTR(i);
}

#endif
