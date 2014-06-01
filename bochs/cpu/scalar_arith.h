/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2014 Stanislav Shwartsman
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

#ifndef BX_SCALAR_ARITH_FUNCTIONS_H
#define BX_SCALAR_ARITH_FUNCTIONS_H

// tzcnt

BX_CPP_INLINE unsigned tzcntw(Bit16u val_16)
{
  Bit16u mask = 0x1;
  unsigned count = 0;

  while ((val_16 & mask) == 0 && mask) {
    mask <<= 1;
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned tzcntd(Bit32u val_32)
{
  Bit32u mask = 0x1;
  unsigned count = 0;

  while ((val_32 & mask) == 0 && mask) {
    mask <<= 1;
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned tzcntq(Bit64u val_64)
{
  Bit64u mask = 0x1;
  unsigned count = 0;

  while ((val_64 & mask) == 0 && mask) {
    mask <<= 1;
    count++;
  }

  return count;
}

// lzcnt

BX_CPP_INLINE unsigned lzcntw(Bit16u val_16)
{
  Bit16u mask = 0x8000;
  unsigned count = 0;

  while ((val_16 & mask) == 0 && mask) {
    mask >>= 1;
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned lzcntd(Bit32u val_32)
{
  Bit32u mask = 0x80000000;
  unsigned count = 0;

  while ((val_32 & mask) == 0 && mask) {
    mask >>= 1;
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned lzcntq(Bit64u val_64)
{
  Bit64u mask = BX_CONST64(0x8000000000000000);
  unsigned count = 0;

  while ((val_64 & mask) == 0 && mask) {
    mask >>= 1;
    count++;
  }

  return count;
}

// popcnt

BX_CPP_INLINE unsigned popcntw(Bit16u val_16)
{
  unsigned count = 0;
  while (val_16 != 0) {
    val_16 &= (val_16-1);
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned popcntd(Bit32u val_32)
{
  unsigned count = 0;
  while (val_32 != 0) {
    val_32 &= (val_32-1);
    count++;
  }

  return count;
}

BX_CPP_INLINE unsigned popcntq(Bit64u val_64)
{
  unsigned count = 0;
  while (val_64 != 0) {
    val_64 &= (val_64-1);
    count++;
  }

  return count;
}

// bit extract

BX_CPP_INLINE Bit32u bextrd(Bit32u val_32, unsigned start, unsigned len)
{
  Bit32u result = 0;

  if (start < 32 && len > 0) {
    result = val_32 >> start;

    if (len < 32) {
      Bit32u extract_mask = (1 << len) - 1;
      result &= extract_mask;
    }
  }

  return result;
}

BX_CPP_INLINE Bit64u bextrq(Bit64u val_64, unsigned start, unsigned len)
{
  Bit64u result = 0;

  if (start < 64 && len > 0) {
    result = val_64 >> start;

    if (len < 64) {
      Bit64u extract_mask = (BX_CONST64(1) << len) - 1;
      result &= extract_mask;
    }
  }

  return result;
}

#endif
