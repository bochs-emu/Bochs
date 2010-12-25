/////////////////////////////////////////////////////////////////////////
// $Id: crc32.cc,v 1.7 2010-12-25 17:04:36 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2008-2009 Stanislav Shwartsman
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

// 3-byte opcodes

#define CRC32_POLYNOMIAL BX_CONST64(0x11edc6f41)

// primitives for CRC32 usage
BX_CPP_INLINE Bit8u BitReflect8(Bit8u val8)
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

  for (int bitpos=31; bitpos>=0; bitpos--) {
    // copy one more bit from the dividend
    remainder = (remainder << 1) | ((dividend >> bitpos) & 1);

    // if MSB is set, then XOR divisor and get new remainder
    if (((remainder >> 32) & 1) == 1) {
      remainder ^= divisor;
    }
  }

  return (Bit32u) remainder;
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CRC32_GdEbR(bxInstruction_c *i)
{
  Bit8u op1 = BX_READ_8BIT_REGx(i->rm(), i->extend8bitL());

  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);

  Bit64u tmp1 = ((Bit64u) BitReflect8 (op1)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) <<  8;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CRC32_GdEw(bxInstruction_c *i)
{
  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);
  Bit16u op1;

  if (i->modC0()) {
    op1 = BX_READ_16BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    op1 = read_virtual_word(i->seg(), eaddr);
  }

  Bit64u tmp1 = ((Bit64u) BitReflect16(op1)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) << 16;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
}

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CRC32_GdEd(bxInstruction_c *i)
{
  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);
  Bit32u op1;

  if (i->modC0()) {
    op1 = BX_READ_32BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    op1 = read_virtual_dword(i->seg(), eaddr);
  }

  Bit64u tmp1 = ((Bit64u) BitReflect32(op1)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) << 32;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2 = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
}

#if BX_SUPPORT_X86_64

void BX_CPP_AttrRegparmN(1) BX_CPU_C::CRC32_GdEq(bxInstruction_c *i)
{
  Bit32u op2 = BX_READ_32BIT_REG(i->nnn());
  op2 = BitReflect32(op2);
  Bit64u op1;

  if (i->modC0()) {
    op1 = BX_READ_64BIT_REG(i->rm());
  }
  else {
    bx_address eaddr = BX_CPU_CALL_METHODR(i->ResolveModrm, (i));
    op1 = read_virtual_qword_64(i->seg(), eaddr);
  }

  Bit64u tmp1 = ((Bit64u) BitReflect32(op1 & 0xffffffff)) << 32;
  Bit64u tmp2 = ((Bit64u) op2) << 32;
  Bit64u tmp3 = tmp1 ^ tmp2;
  op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);
  tmp1 = ((Bit64u) BitReflect32(op1 >> 32)) << 32;
  tmp2 = ((Bit64u) op2) << 32;
  tmp3 = tmp1 ^ tmp2;
  op2  = mod2_64bit(CRC32_POLYNOMIAL, tmp3);

  /* now write result back to destination */
  BX_WRITE_32BIT_REGZ(i->nnn(), BitReflect32(op2));
}

#endif // BX_SUPPORT_X86_64
