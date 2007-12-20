/////////////////////////////////////////////////////////////////////////
// $Id: bit.cc,v 1.47 2007-12-20 18:29:38 sshwarts Exp $
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


#if BX_CPU_LEVEL >= 3

void BX_CPU_C::SETO_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_OF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETO_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_OF());
}

void BX_CPU_C::SETNO_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !getB_OF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNO_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_OF());
}

void BX_CPU_C::SETB_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_CF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETB_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_CF());
}

void BX_CPU_C::SETNB_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !getB_CF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNB_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_CF());
}

void BX_CPU_C::SETZ_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_ZF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETZ_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_ZF());
}

void BX_CPU_C::SETNZ_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !getB_ZF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNZ_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_ZF());
}

void BX_CPU_C::SETBE_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = (getB_CF() | getB_ZF());
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETBE_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), (getB_CF() | getB_ZF()));
}

void BX_CPU_C::SETNBE_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !(getB_CF() | getB_ZF());
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNBE_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !(getB_CF() | getB_ZF()));
}

void BX_CPU_C::SETS_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_SF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETS_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_SF());
}

void BX_CPU_C::SETNS_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !getB_SF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNS_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_SF());
}

void BX_CPU_C::SETP_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_PF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETP_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), getB_PF());
}

void BX_CPU_C::SETNP_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !getB_PF();
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNP_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !getB_PF());
}

void BX_CPU_C::SETL_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = (getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETL_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), (getB_SF() ^ getB_OF()));
}

void BX_CPU_C::SETNL_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !(getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNL_EbR(bxInstruction_c *i)
{
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), !(getB_SF() ^ getB_OF()));
}

void BX_CPU_C::SETLE_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = getB_ZF() | (getB_SF() ^ getB_OF());
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETLE_EbR(bxInstruction_c *i)
{
  Bit8u result_8 = getB_ZF() | (getB_SF() ^ getB_OF());
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
}

void BX_CPU_C::SETNLE_EbM(bxInstruction_c *i)
{
  Bit8u result_8 = !(getB_ZF() | (getB_SF() ^ getB_OF()));
  write_virtual_byte(i->seg(), RMAddr(i), result_8);
}

void BX_CPU_C::SETNLE_EbR(bxInstruction_c *i)
{
  Bit8u result_8 = !(getB_ZF() | (getB_SF() ^ getB_OF()));
  BX_WRITE_8BIT_REGx(i->rm(), i->extend8bitL(), result_8);
}

void BX_CPU_C::BSWAP_ERX(bxInstruction_c *i)
{
#if BX_CPU_LEVEL >= 4
  Bit32u val32, b0, b1, b2, b3;

  if (i->os32L() == 0) {
    BX_ERROR(("BSWAP with 16-bit opsize: undefined behavior !"));
  }

  val32 = BX_READ_32BIT_REG(i->opcodeReg());
  b0  = val32 & 0xff; val32 >>= 8;
  b1  = val32 & 0xff; val32 >>= 8;
  b2  = val32 & 0xff; val32 >>= 8;
  b3  = val32;
  val32 = (b0<<24) | (b1<<16) | (b2<<8) | b3;  // zero extended

  // in 64-bit mode, hi-order 32 bits are not modified
  BX_WRITE_32BIT_REGZ(i->opcodeReg(), val32);
#else
  BX_INFO(("BSWAP_ERX: required CPU >= 4, use --enable-cpu-level=4 option"));
  UndefinedOpcode(i);
#endif
}

#if BX_SUPPORT_X86_64
void BX_CPU_C::BSWAP_RRX(bxInstruction_c *i)
{
  Bit64u val64, b0, b1, b2, b3, b4, b5, b6, b7;

  val64 = BX_READ_64BIT_REG(i->opcodeReg());
  b0  = val64 & 0xff; val64 >>= 8;
  b1  = val64 & 0xff; val64 >>= 8;
  b2  = val64 & 0xff; val64 >>= 8;
  b3  = val64 & 0xff; val64 >>= 8;
  b4  = val64 & 0xff; val64 >>= 8;
  b5  = val64 & 0xff; val64 >>= 8;
  b6  = val64 & 0xff; val64 >>= 8;
  b7  = val64;
  val64 = (b0<<56) | (b1<<48) | (b2<<40) | (b3<<32) | (b4<<24) | (b4<<16) | (b4<<8) | b7;

  BX_WRITE_64BIT_REG(i->opcodeReg(), val64);
}
#endif

// 3-byte opcodes
#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

#define CRC32_POLYNOMIAL BX_CONST64(0x11edc6f41)

#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
// primitives for CRC32 usage
static Bit8u BitReflect8(Bit8u val8)
{
   return ((val8 & 0x80) >> 7) |
          ((val8 & 0x40) >> 5) |
          ((val8 & 0x20) >> 3) |
          ((val8 & 0x10) >> 1) |
          ((val8 & 0x08) << 1) |
          ((val8 & 0x04) << 3) |
          ((val8 & 0x02) << 5) |
          ((val8 & 0x01) << 7);
}

BX_CPP_INLINE Bit16u BitReflect16(Bit16u val16)
{
   return ((Bit16u)(BitReflect8(val16 & 0xff)) << 8) | BitReflect8(val16 >> 8);
}

BX_CPP_INLINE Bit32u BitReflect32(Bit32u val32)
{
   return ((Bit32u)(BitReflect16(val32 & 0xffff)) << 16) | BitReflect16(val32 >> 16);
}

static Bit32u mod2_64bit(Bit64u divisor, Bit64u dividend)
{
    Bit64u remainder = dividend >> 32;

    for (int bitpos=31; bitpos>=0; bitpos--)
    {
	// copy one more bit from the dividend
	remainder = (remainder << 1) | ((dividend >> bitpos) & 1);

	// if MSB is set, then XOR divisor and get new remainder
	if (((remainder >> 32) & 1) == 1)
	{
	    remainder ^= divisor;
	}
    }

    return remainder;
}
#endif // (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)

void BX_CPU_C::CRC32_GdEb(bxInstruction_c *i)
{
#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)
  Bit8u  op1;

  if (i->modC0()) {
    op1 = BX_READ_8BIT_REGx(i->rm(),i->extend8bitL());
  }
  else {
    read_virtual_byte(i->seg(), RMAddr(i), &op1);
  }

  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);

  Bit64u tmp1 = ((Bit64u) BitReflect8 (op1)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) <<  8;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
#else
  BX_INFO(("CRC32_GdEb: required SSE4_2 support, required SSE4.2, use --enable-sse and --enable-sse-extension options"));
  UndefinedOpcode(i);
#endif
}

void BX_CPU_C::CRC32_GdEv(bxInstruction_c *i)
{
#if (BX_SUPPORT_SSE >= 5) || (BX_SUPPORT_SSE >= 4 && BX_SUPPORT_SSE_EXTENSION > 0)

  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);

#if BX_SUPPORT_X86_64
  if (i->os64L())   /* 64 bit operand size */
  {
    Bit64u op1;

    if (i->modC0()) {
      op1 = BX_READ_64BIT_REG(i->rm());
    }
    else {
      read_virtual_qword(i->seg(), RMAddr(i), &op1);
    }

    Bit64u tmp1 = ((Bit64u) BitReflect32(op1 & 0xffffffff)) << 32;
    Bit64u tmp2 = ((Bit64u) op2) << 32;
    Bit64u tmp3 = tmp1 ^ tmp2;
    op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    tmp1 = ((Bit64u) BitReflect32(op1 >> 32)) << 32;
    tmp2 = ((Bit64u) op2) << 32;
    tmp3 = tmp1 ^ tmp2;
    op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
  }
  else
#endif
  {
    if (i->os32L()) /* 32 bit operand size */
    {
      Bit32u op1;

      if (i->modC0()) {
        op1 = BX_READ_32BIT_REG(i->rm());
      }
      else {
        read_virtual_dword(i->seg(), RMAddr(i), &op1);
      }

      Bit64u tmp1 = ((Bit64u) BitReflect32(op1)) << 32;
      Bit64u tmp2 = ((Bit64u) op2) << 32;
      Bit64u tmp3 = tmp1 ^ tmp2;
      op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    }
    else {          /* 16 bit operand size */
      Bit16u op1;

      if (i->modC0()) {
        op1 = BX_READ_16BIT_REG(i->rm());
      }
      else {
        read_virtual_word(i->seg(), RMAddr(i), &op1);
      }

      Bit64u tmp1 = ((Bit64u) BitReflect16(op1)) << 32;
      Bit64u tmp2 = ((Bit64u) op2) << 16;
      Bit64u tmp3 = tmp1 ^ tmp2;
      op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
    }
  }

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
  
#else
  BX_INFO(("CRC32_GdEv: required SSE4_2 support, required SSE4.2, use --enable-sse and --enable-sse-extension options"));
  UndefinedOpcode(i);
#endif
}

#endif // (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)

#endif // (BX_CPU_LEVEL >= 3)
