/////////////////////////////////////////////////////////////////////////
// $Id: bit.cc,v 1.64 2009-11-02 15:00:47 sshwarts Exp $
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA B 02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_CPU_LEVEL >= 3

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETO_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_OF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETO_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_OF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNO_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !getB_OF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNO_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_OF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETB_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_CF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETB_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_CF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNB_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !getB_CF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNB_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_CF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETZ_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_ZF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETZ_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_ZF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNZ_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !getB_ZF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNZ_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_ZF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETBE_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = (getB_CF() | getB_ZF());
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETBE_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), (getB_CF() | getB_ZF()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNBE_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !(getB_CF() | getB_ZF());
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNBE_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !(getB_CF() | getB_ZF()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETS_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_SF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETS_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_SF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNS_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !getB_SF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNS_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_SF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETP_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_PF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETP_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_PF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNP_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !getB_PF();
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNP_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_PF());
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETL_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = (getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETL_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), (getB_SF() ^ getB_OF()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNL_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !(getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNL_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !(getB_SF() ^ getB_OF()));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETLE_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = getB_ZF() | (getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETLE_EbR(bxInstruction_c *i)
{
  Bit8u result_8 = getB_ZF() | (getB_SF() ^ getB_OF());
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNLE_EbM(bxInstruction_c *i)
{
  bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

  Bit8u result_8 = !(getB_ZF() | (getB_SF() ^ getB_OF()));
  write_virtual_byte(i->seg(), eaddr, result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::SETNLE_EbR(bxInstruction_c *i)
{
  Bit8u result_8 = !(getB_ZF() | (getB_SF() ^ getB_OF()));
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSWAP_ERX(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u val32, b0, b1, b2, b3;

  if (i->os32L() == 0) {
    BX_ERROR(("BSWAP with 16-bit opsize: undefined behavior !"));
  }

  val32 = BX_READ_32BIT_REG(i->opcodeReg());
  b0 = val32 & 0xff; val32 >>= 8;
  b1 = val32 & 0xff; val32 >>= 8;
  b2 = val32 & 0xff; val32 >>= 8;
  b3 = val32;
  val32 = (b0<<24) | (b1<<16) | (b2<<8) | b3;

  BX_WRITE_32BIT_REGZ(i->opcodeReg(), val32);
#else
  BX_INFO(("BSWAP_ERX: required CPU >= 4, use --enable-cpu-level=4 option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPP_AttrRegparmN(1) BX_CPU_C::BSWAP_RRX(bxInstruction_c *i)
{
  Bit64u val64, b0, b1, b2, b3, b4, b5, b6, b7;

  val64 = BX_READ_64BIT_REG(i->opcodeReg());
  b0 = val64 & 0xff; val64 >>= 8;
  b1 = val64 & 0xff; val64 >>= 8;
  b2 = val64 & 0xff; val64 >>= 8;
  b3 = val64 & 0xff; val64 >>= 8;
  b4 = val64 & 0xff; val64 >>= 8;
  b5 = val64 & 0xff; val64 >>= 8;
  b6 = val64 & 0xff; val64 >>= 8;
  b7 = val64;
  val64 = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;

  BX_WRITE_64BIT_REG(i->opcodeReg(), val64);
}
#endif

// 3-byte opcodes
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GwEw(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit16u val16, b0, b1;

  if (i->modC0()) {
    val16 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    val16 = read_virtual_word(i->seg(), eaddr);
  }
  
  b0 = val16 & 0xff; val16 >>= 8;
  b1 = val16;
  val16 = (b0<<8) | b1;

  BX_WRITE_16BIT_REG(i->nnn(), val16);
#else
  BX_INFO(("MOVBE_GwEw: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_EwGw(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit16u val16 = BX_READ_16BIT_REG(i->nnn()), b0, b1;

  b0 = val16 & 0xff; val16 >>= 8;
  b1 = val16;
  val16 = (b0<<8) | b1;

  if (i->modC0()) {
    BX_WRITE_16BIT_REG(i->rm(), val16);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    write_virtual_word(i->seg(), eaddr, val16);
  }
#else
  BX_INFO(("MOVBE_EwGw: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GdEd(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit32u val32, b0, b1, b2, b3;

  if (i->modC0()) {
    val32 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    val32 = read_virtual_dword(i->seg(), eaddr);
  }
  
  b0 = val32 & 0xff; val32 >>= 8;
  b1 = val32 & 0xff; val32 >>= 8;
  b2 = val32 & 0xff; val32 >>= 8;
  b3 = val32;
  val32 = (b0<<24) | (b1<<16) | (b2<<8) | b3;

  BX_WRITE_32BIT_REGZ(i->nnn(), val32);
#else
  BX_INFO(("MOVBE_GdEd: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_EdGd(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit32u val32 = BX_READ_32BIT_REG(i->nnn()), b0, b1, b2, b3;

  b0 = val32 & 0xff; val32 >>= 8;
  b1 = val32 & 0xff; val32 >>= 8;
  b2 = val32 & 0xff; val32 >>= 8;
  b3 = val32;
  val32 = (b0<<24) | (b1<<16) | (b2<<8) | b3;

  if (i->modC0()) {
    BX_WRITE_32BIT_REGZ(i->rm(), val32);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    write_virtual_dword(i->seg(), eaddr, val32);
  }
#else
  BX_INFO(("MOVBE_EdGd: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_GqEq(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit64u val64, b0, b1, b2, b3, b4, b5, b6, b7;

  if (i->modC0()) {
    val64 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    val64 = read_virtual_qword(i->seg(), eaddr);
  }
  
  b0 = val64 & 0xff; val64 >>= 8;
  b1 = val64 & 0xff; val64 >>= 8;
  b2 = val64 & 0xff; val64 >>= 8;
  b3 = val64 & 0xff; val64 >>= 8;
  b4 = val64 & 0xff; val64 >>= 8;
  b5 = val64 & 0xff; val64 >>= 8;
  b6 = val64 & 0xff; val64 >>= 8;
  b7 = val64;
  val64 = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;

  BX_WRITE_64BIT_REG(i->nnn(), val64);
#else
  BX_INFO(("MOVBE_GqEq: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::MOVBE_EqGq(bxInstruction_c *i)
{
#if BX_SUPPORT_MOVBE
  Bit64u val64 = BX_READ_64BIT_REG(i->nnn());
  Bit64u b0, b1, b2, b3, b4, b5, b6, b7;

  b0 = val64 & 0xff; val64 >>= 8;
  b1 = val64 & 0xff; val64 >>= 8;
  b2 = val64 & 0xff; val64 >>= 8;
  b3 = val64 & 0xff; val64 >>= 8;
  b4 = val64 & 0xff; val64 >>= 8;
  b5 = val64 & 0xff; val64 >>= 8;
  b6 = val64 & 0xff; val64 >>= 8;
  b7 = val64;
  val64 = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b5<<16) | (b6<<8) | b7;

  if (i->modC0()) {
    BX_WRITE_64BIT_REG(i->rm(), val64);
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    write_virtual_qword_64(i->seg(), eaddr, val64);
  }
#else
  BX_INFO(("MOVBE_EqGq: required MOVBE support, use --enable-movbe option"));
  exception(BX_UD_EXCEPTION, 0, 0);
#endif
}

#endif // (BX_SUPPORT_X86_64)

#endif // (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

#endif // (BX_CPU_LEVEL >= 3)
