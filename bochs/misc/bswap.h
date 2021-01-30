/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2021  The Bochs Project
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

#if !defined(__MORPHOS__)
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
#endif // !MorphOS

// These are some convenience macros which abstract out accesses between
// a variable in native byte ordering to/from guest (x86) memory, which is
// always in little endian format.  You must deal with alignment (if your
// system cares) and endian rearranging.  Don't assume anything.  You could
// put some platform specific asm() statements here, to make use of native
// instructions to help perform these operations more efficiently than C++.

#ifdef BX_LITTLE_ENDIAN

BX_CPP_INLINE void WriteHostWordToLittleEndian(Bit16u *hostPtr, Bit16u nativeVar16)
{
  *(hostPtr) = nativeVar16;
}

BX_CPP_INLINE void WriteHostDWordToLittleEndian(Bit32u *hostPtr, Bit32u nativeVar32)
{
  *(hostPtr) = nativeVar32;
}

BX_CPP_INLINE void WriteHostQWordToLittleEndian(Bit64u *hostPtr, Bit64u nativeVar64)
{
#ifdef ANDROID
// Resolve problems with unaligned access
  ((Bit8u *)(hostPtr))[0] = (Bit8u) (nativeVar64);
  ((Bit8u *)(hostPtr))[1] = (Bit8u) ((nativeVar64)>>8);
  ((Bit8u *)(hostPtr))[2] = (Bit8u) ((nativeVar64)>>16);
  ((Bit8u *)(hostPtr))[3] = (Bit8u) ((nativeVar64)>>24);
  ((Bit8u *)(hostPtr))[4] = (Bit8u) ((nativeVar64)>>32);
  ((Bit8u *)(hostPtr))[5] = (Bit8u) ((nativeVar64)>>40);
  ((Bit8u *)(hostPtr))[6] = (Bit8u) ((nativeVar64)>>48);
  ((Bit8u *)(hostPtr))[7] = (Bit8u) ((nativeVar64)>>56);
#else
  *(hostPtr) = nativeVar64;
#endif
}

BX_CPP_INLINE Bit16u ReadHostWordFromLittleEndian(Bit16u *hostPtr)
{
  return *(hostPtr);
}

BX_CPP_INLINE Bit32u ReadHostDWordFromLittleEndian(Bit32u *hostPtr)
{
  return *(hostPtr);
}

BX_CPP_INLINE Bit64u ReadHostQWordFromLittleEndian(Bit64u *hostPtr)
{
#ifdef ANDROID
// Resolve problems with unaligned access
  Bit64u nativeVar64 = ((Bit64u) ((Bit8u *)(hostPtr))[0]) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[1])<<8) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[2])<<16) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[3])<<24) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[4])<<32) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[5])<<40) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[6])<<48) |
                       (((Bit64u) ((Bit8u *)(hostPtr))[7])<<56);
  return nativeVar64;
#else
  return *(hostPtr);
#endif
}

#else // !BX_LITTLE_ENDIAN

#ifdef __MORPHOS__

#define bx_bswap16 bx_ppc_bswap16
#define bx_bswap32 bx_ppc_bswap32
#define bx_bswap64 bx_ppc_bswap64

BX_CPP_INLINE void WriteHostWordToLittleEndian(Bit16u *hostPtr, Bit16u nativeVar16)
{
  bx_ppc_store_le16(hostPtr, nativeVar16);
}

BX_CPP_INLINE void WriteHostDWordToLittleEndian(Bit32u *hostPtr, Bit32u nativeVar32)
{
  bx_ppc_store_le32(hostPtr, nativeVar32);
}

BX_CPP_INLINE void WriteHostQWordToLittleEndian(Bit64u *hostPtr, Bit64u nativeVar64)
{
  bx_ppc_store_le64(hostPtr, nativeVar64);
}

BX_CPP_INLINE Bit16u ReadHostWordFromLittleEndian(Bit16u *hostPtr)
{
  return bx_ppc_load_le16(hostPtr);
}

BX_CPP_INLINE Bit32u ReadHostDWordFromLittleEndian(Bit32u *hostPtr)
{
  return bx_ppc_load_le32(hostPtr);
}

BX_CPP_INLINE Bit64u ReadHostQWordFromLittleEndian(Bit64u *hostPtr)
{
  return bx_ppc_load_le64(hostPtr);
}

#else // !__MORPHOS__

BX_CPP_INLINE void WriteHostWordToLittleEndian(Bit16u *hostPtr, Bit16u nativeVar16)
{
  *(hostPtr) = bx_bswap16(nativeVar16);
}

BX_CPP_INLINE void WriteHostDWordToLittleEndian(Bit32u *hostPtr, Bit32u nativeVar32)
{
  *(hostPtr) = bx_bswap32(nativeVar32);
}

BX_CPP_INLINE void WriteHostQWordToLittleEndian(Bit64u *hostPtr, Bit64u nativeVar64)
{
  *(hostPtr) = bx_bswap64(nativeVar64);
}

BX_CPP_INLINE Bit16u ReadHostWordFromLittleEndian(Bit16u *hostPtr)
{
  return bx_bswap16(*hostPtr);
}

BX_CPP_INLINE Bit32u ReadHostDWordFromLittleEndian(Bit32u *hostPtr)
{
  return bx_bswap32(*hostPtr);
}

BX_CPP_INLINE Bit64u ReadHostQWordFromLittleEndian(Bit64u *hostPtr)
{
  return bx_bswap64(*hostPtr);
}

#endif // !__MORPHOS__

#endif // !BX_LITTLE_ENDIAN

#endif // BX_BSWAP_H
