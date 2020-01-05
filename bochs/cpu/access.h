/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2015-2018 Stanislav Shwartsman
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

#ifndef BX_MEMACCESS_H
#define BX_MEMACCESS_H

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_byte_32(unsigned s, Bit32u offset, Bit8u data)
{
  Bit32u laddr = agen_write32(s, offset, 1);
  write_linear_byte(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_word_32(unsigned s, Bit32u offset, Bit16u data)
{
  Bit32u laddr = agen_write32(s, offset, 2);
  write_linear_word(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dword_32(unsigned s, Bit32u offset, Bit32u data)
{
  Bit32u laddr = agen_write32(s, offset, 4);
  write_linear_dword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_qword_32(unsigned s, Bit32u offset, Bit64u data)
{
  Bit32u laddr = agen_write32(s, offset, 8);
  write_linear_qword(s, laddr, data);
}

#if BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_xmmword_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  Bit32u laddr = agen_write32(s, offset, 16);
  write_linear_xmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_xmmword_aligned_32(unsigned s, Bit32u offset, const BxPackedXmmRegister *data)
{
  Bit32u laddr = agen_write_aligned32(s, offset, 16);
  write_linear_xmmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_ymmword_32(unsigned s, Bit32u offset, const BxPackedYmmRegister *data)
{
  Bit32u laddr = agen_write32(s, offset, 32);
  write_linear_ymmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_ymmword_aligned_32(unsigned s, Bit32u offset, const BxPackedYmmRegister *data)
{
  Bit32u laddr = agen_write_aligned32(s, offset, 32);
  write_linear_ymmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_zmmword_32(unsigned s, Bit32u offset, const BxPackedZmmRegister *data)
{
  Bit32u laddr = agen_write32(s, offset, 64);
  write_linear_zmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_zmmword_aligned_32(unsigned s, Bit32u offset, const BxPackedZmmRegister *data)
{
  Bit32u laddr = agen_write_aligned32(s, offset, 64);
  write_linear_zmmword_aligned(s, laddr, data);
}

#endif // BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(2)
BX_CPU_C::tickle_read_virtual_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_read32(s, offset, 1);
  tickle_read_linear(s, laddr);
}

  BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_byte_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_read32(s, offset, 1);
  return read_linear_byte(s, laddr);
}

  BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_word_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_read32(s, offset, 2);
  return read_linear_word(s, laddr);
}

  BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_dword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_read32(s, offset, 4);
  return read_linear_dword(s, laddr);
}

  BX_CPP_INLINE Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_qword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_read32(s, offset, 8);
  return read_linear_qword(s, laddr);
}

#if BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_xmmword_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  Bit32u laddr = agen_read32(s, offset, 16);
  read_linear_xmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_xmmword_aligned_32(unsigned s, Bit32u offset, BxPackedXmmRegister *data)
{
  Bit32u laddr = agen_read_aligned32(s, offset, 16);
  read_linear_xmmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_ymmword_32(unsigned s, Bit32u offset, BxPackedYmmRegister *data)
{
  Bit32u laddr = agen_read32(s, offset, 32);
  read_linear_ymmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_ymmword_aligned_32(unsigned s, Bit32u offset, BxPackedYmmRegister *data)
{
  Bit32u laddr = agen_read_aligned32(s, offset, 32);
  read_linear_ymmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_zmmword_32(unsigned s, Bit32u offset, BxPackedZmmRegister *data)
{
  Bit32u laddr = agen_read32(s, offset, 64);
  read_linear_zmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_zmmword_aligned_32(unsigned s, Bit32u offset, BxPackedZmmRegister *data)
{
  Bit32u laddr = agen_read_aligned32(s, offset, 64);
  read_linear_zmmword_aligned(s, laddr, data);
}

#endif // BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_byte(unsigned s, bx_address offset, Bit8u data)
{
  bx_address laddr = agen_write(s, offset, 1);
  write_linear_byte(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_word(unsigned s, bx_address offset, Bit16u data)
{
  bx_address laddr = agen_write(s, offset, 2);
  write_linear_word(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_dword(unsigned s, bx_address offset, Bit32u data)
{
  bx_address laddr = agen_write(s, offset, 4);
  write_linear_dword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_qword(unsigned s, bx_address offset, Bit64u data)
{
  bx_address laddr = agen_write(s, offset, 8);
  write_linear_qword(s, laddr, data);
}

#if BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_xmmword(unsigned s, bx_address offset, const BxPackedXmmRegister *data)
{
  bx_address laddr = agen_write(s, offset, 16);
  write_linear_xmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_xmmword_aligned(unsigned s, bx_address offset, const BxPackedXmmRegister *data)
{
  bx_address laddr = agen_write_aligned(s, offset, 16);
  write_linear_xmmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_ymmword(unsigned s, bx_address offset, const BxPackedYmmRegister *data)
{
  bx_address laddr = agen_write(s, offset, 32);
  write_linear_ymmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_ymmword_aligned(unsigned s, bx_address offset, const BxPackedYmmRegister *data)
{
  bx_address laddr = agen_write_aligned(s, offset, 32);
  write_linear_ymmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_zmmword(unsigned s, bx_address offset, const BxPackedZmmRegister *data)
{
  bx_address laddr = agen_write(s, offset, 64);
  write_linear_zmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::write_virtual_zmmword_aligned(unsigned s, bx_address offset, const BxPackedZmmRegister *data)
{
  bx_address laddr = agen_write_aligned(s, offset, 64);
  write_linear_zmmword_aligned(s, laddr, data);
}

#endif // BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(2)
BX_CPU_C::tickle_read_virtual(unsigned s, bx_address offset)
{
  bx_address laddr = agen_read(s, offset, 1);
  tickle_read_linear(s, laddr);
}

  BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_byte(unsigned s, bx_address offset)
{
  bx_address laddr = agen_read(s, offset, 1);
  return read_linear_byte(s, laddr);
}

  BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_word(unsigned s, bx_address offset)
{
  bx_address laddr = agen_read(s, offset, 2);
  return read_linear_word(s, laddr);
}

  BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_dword(unsigned s, bx_address offset)
{
  bx_address laddr = agen_read(s, offset, 4);
  return read_linear_dword(s, laddr);
}

  BX_CPP_INLINE Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_virtual_qword(unsigned s, bx_address offset)
{
  bx_address laddr = agen_read(s, offset, 8);
  return read_linear_qword(s, laddr);
}

#if BX_CPU_LEVEL >= 6

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_xmmword(unsigned s, bx_address offset, BxPackedXmmRegister *data)
{
  bx_address laddr = agen_read(s, offset, 16);
  read_linear_xmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_xmmword_aligned(unsigned s, bx_address offset, BxPackedXmmRegister *data)
{
  bx_address laddr = agen_read_aligned(s, offset, 16);
  read_linear_xmmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_ymmword(unsigned s, bx_address offset, BxPackedYmmRegister *data)
{
  bx_address laddr = agen_read(s, offset, 32);
  read_linear_ymmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_ymmword_aligned(unsigned s, bx_address offset, BxPackedYmmRegister *data)
{
  bx_address laddr = agen_read_aligned(s, offset, 32);
  read_linear_ymmword_aligned(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_zmmword(unsigned s, bx_address offset, BxPackedZmmRegister *data)
{
  bx_address laddr = agen_read(s, offset, 64);
  read_linear_zmmword(s, laddr, data);
}

  BX_CPP_INLINE void BX_CPP_AttrRegparmN(3)
BX_CPU_C::read_virtual_zmmword_aligned(unsigned s, bx_address offset, BxPackedZmmRegister *data)
{
  bx_address laddr = agen_read_aligned(s, offset, 64);
  read_linear_zmmword_aligned(s, laddr, data);
}

#endif // BX_CPU_LEVEL >= 6

//////////////////////////////////////////////////////////////
// special Read-Modify-Write operations                     //
// address translation info is kept across read/write calls //
//////////////////////////////////////////////////////////////

  BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_write32(s, offset, 1);
  return read_RMW_linear_byte(s, laddr);
}

  BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_word_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_write32(s, offset, 2);
  return read_RMW_linear_word(s, laddr);
}

  BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_dword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_write32(s, offset, 4);
  return read_RMW_linear_dword(s, laddr);
}

  BX_CPP_INLINE Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_qword_32(unsigned s, Bit32u offset)
{
  Bit32u laddr = agen_write32(s, offset, 8);
  return read_RMW_linear_qword(s, laddr);
}

  BX_CPP_INLINE Bit8u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_byte(unsigned s, bx_address offset)
{
  bx_address laddr = agen_write(s, offset, 1);
  return read_RMW_linear_byte(s, laddr);
}

  BX_CPP_INLINE Bit16u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_word(unsigned s, bx_address offset)
{
  bx_address laddr = agen_write(s, offset, 2);
  return read_RMW_linear_word(s, laddr);
}

  BX_CPP_INLINE Bit32u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_dword(unsigned s, bx_address offset)
{
  bx_address laddr = agen_write(s, offset, 4);
  return read_RMW_linear_dword(s, laddr);
}

  BX_CPP_INLINE Bit64u BX_CPP_AttrRegparmN(2)
BX_CPU_C::read_RMW_virtual_qword(unsigned s, bx_address offset)
{
  bx_address laddr = agen_write(s, offset, 8);
  return read_RMW_linear_qword(s, laddr);
}

#endif
