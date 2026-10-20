/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2018  The Bochs Project
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

#if BX_CPU_LEVEL >= 3

// SETcc Eb (memory): store the condition into the byte
#define SETCC_EbM(HANDLER, cond)                                                          \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);                                            \
                                                                                          \
    Bit8u result_8 = cond;                                                                \
    write_virtual_byte(i->seg(), eaddr, result_8);                                        \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

// SETcc Eb (register): write the condition into the byte register
#define SETCC_EbR(HANDLER, cond)                                                          \
  void BX_CPP_AttrRegparmN(1) BX_CPU_C:: HANDLER (bxInstruction_c *i)                     \
  {                                                                                       \
    BX_WRITE_8BIT_REGx(i->dst(), i->extend8bitL(), (cond));                               \
                                                                                          \
    BX_NEXT_INSTR(i);                                                                     \
  }

SETCC_EbM(SETO_EbM, getB_OF())
SETCC_EbM(SETNO_EbM, !getB_OF())
SETCC_EbM(SETB_EbM, getB_CF())
SETCC_EbM(SETNB_EbM, !getB_CF())
SETCC_EbM(SETZ_EbM, getB_ZF())
SETCC_EbM(SETNZ_EbM, !getB_ZF())
SETCC_EbM(SETBE_EbM, getB_CF() | getB_ZF())
SETCC_EbM(SETNBE_EbM, !(getB_CF() | getB_ZF()))
SETCC_EbM(SETS_EbM, getB_SF())
SETCC_EbM(SETNS_EbM, !getB_SF())
SETCC_EbM(SETP_EbM, getB_PF())
SETCC_EbM(SETNP_EbM, !getB_PF())
SETCC_EbM(SETL_EbM, getB_SF() ^ getB_OF())
SETCC_EbM(SETNL_EbM, !(getB_SF() ^ getB_OF()))
SETCC_EbM(SETLE_EbM, getB_ZF() | (getB_SF() ^ getB_OF()))
SETCC_EbM(SETNLE_EbM, !(getB_ZF() | (getB_SF() ^ getB_OF())))

SETCC_EbR(SETO_EbR, getB_OF())
SETCC_EbR(SETNO_EbR, !getB_OF())
SETCC_EbR(SETB_EbR, getB_CF())
SETCC_EbR(SETNB_EbR, !getB_CF())
SETCC_EbR(SETZ_EbR, getB_ZF())
SETCC_EbR(SETNZ_EbR, !getB_ZF())
SETCC_EbR(SETBE_EbR, getB_CF() | getB_ZF())
SETCC_EbR(SETNBE_EbR, !(getB_CF() | getB_ZF()))
SETCC_EbR(SETS_EbR, getB_SF())
SETCC_EbR(SETNS_EbR, !getB_SF())
SETCC_EbR(SETP_EbR, getB_PF())
SETCC_EbR(SETNP_EbR, !getB_PF())
SETCC_EbR(SETL_EbR, getB_SF() ^ getB_OF())
SETCC_EbR(SETNL_EbR, !(getB_SF() ^ getB_OF()))
SETCC_EbR(SETLE_EbR, getB_ZF() | (getB_SF() ^ getB_OF()))
SETCC_EbR(SETNLE_EbR, !(getB_ZF() | (getB_SF() ^ getB_OF())))

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSWAP_RX(bxInstruction_c *i)
{
  BX_ERROR(("BSWAP with 16-bit opsize: undefined behavior !"));
  BX_WRITE_16BIT_REG(i->dst(), 0);

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSWAP_ERX(bxInstruction_c *i)
{
  Bit32u val32 = BX_READ_32BIT_REG(i->dst());

  BX_WRITE_32BIT_REGZ(i->dst(), bx_bswap32(val32));

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSWAP_RRX(bxInstruction_c *i)
{
  Bit64u val64 = BX_READ_64BIT_REG(i->dst());

  BX_WRITE_64BIT_REG(i->dst(), bx_bswap64(val64));

  BX_NEXT_INSTR(i);
}
#endif

#if BX_CPU_LEVEL >= 6

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GwMw(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit16u val16 = read_virtual_word(i->seg(), eaddr);
  BX_WRITE_16BIT_REG(i->dst(), bx_bswap16(val16));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_MwGw(bxInstruction_c *i)
{
  Bit16u val16 = BX_READ_16BIT_REG(i->src());
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_word(i->seg(), eaddr, bx_bswap16(val16));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GdMd(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  Bit32u val32 = read_virtual_dword(i->seg(), eaddr);
  BX_WRITE_32BIT_REGZ(i->dst(), bx_bswap32(val32));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_MdGd(bxInstruction_c *i)
{
  Bit32u val32 = BX_READ_32BIT_REG(i->src());
  bx_address eaddr = BX_CPU_RESOLVE_ADDR(i);
  write_virtual_dword(i->seg(), eaddr, bx_bswap32(val32));

  BX_NEXT_INSTR(i);
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GqMq(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  Bit64u val64 = read_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr));
  BX_WRITE_64BIT_REG(i->dst(), bx_bswap64(val64));

  BX_NEXT_INSTR(i);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_MqGq(bxInstruction_c *i)
{
  Bit64u val64 = BX_READ_64BIT_REG(i->src());
  bx_address eaddr = BX_CPU_RESOLVE_ADDR_64(i);
  write_linear_qword(i->seg(), get_laddr64(i->seg(), eaddr), bx_bswap64(val64));

  BX_NEXT_INSTR(i);
}

#endif // BX_SUPPORT_X86_64

#endif // BX_CPU_LEVEL >= 6

#endif // BX_CPU_LEVEL >= 3
