/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2013  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
/////////////////////////////////////////////////////////////////////////

#ifndef BX_BSWAP_H
#define BX_BSWAP_H

BX_CPP_INLINE Bit16u bx_bswap16(Bit16u val16)
{
  return (val16<<8) | (val16>>8);
}

#if BX_HAVE___BUILTIN_BSWAP32
#define bx_bswap32 __builtin_bswap32
#else
BX_CPP_INLINE Bit32u bx_bswap32(Bit32u val32)
{
  val32 = ((val32<<8) & 0xFF00FF00) | ((val32>>8) & 0x00FF00FF);
  return (val32<<16) | (val32>>16);
}
#endif

#if BX_HAVE___BUILTIN_BSWAP64
#define bx_bswap64 __builtin_bswap64
#else
BX_CPP_INLINE Bit64u bx_bswap64(Bit64u val64)
{
  Bit32u lo = bx_bswap32((Bit32u)(val64 >> 32));
  Bit32u hi = bx_bswap32((Bit32u)(val64 & 0xFFFFFFFF));
  return ((Bit64u)hi << 32) | (Bit64u)lo;
}
#endif

#endif
