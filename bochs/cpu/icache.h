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


#ifndef BX_ICACHE_H
#  define BX_ICACHE_H 1

#define BxICacheEntries (32 * 1024)  // Must be a power of 2.

// bit31: 1=CS is 32/64-bit, 0=CS is 16-bit.
// bit30: 1=Long Mode, 0=not Long Mode.
// bit29: 1=iCache page, 0=Data.
#define ICacheWriteStampInvalid   0x1fffffff
#define ICacheWriteStampMax       0x1fffffff // Decrements from here.
#define ICacheWriteStampMask      0x1fffffff
#define ICacheFetchModeMask       (~ICacheWriteStampMask)

class bxICacheEntry_c {
  public:

  Bit32u pAddr;       // Physical address of the instruction.
  Bit32u writeStamp;  // Generation ID.  Each write to a physical page
                      // decrements this value.
  bxInstruction_c i;  // The instruction decode information.
};

class BOCHSAPI bxICache_c
{
  public:

  bxICacheEntry_c entry[BxICacheEntries];

  // A table (dynamically allocated) to store write-stamp
  // generation IDs.  Each time a write occurs to a physical page,
  // a generation ID is decremented.  Only iCache entries which have
  // write stamps matching the physical page write stamp are valid.
  Bit32u *pageWriteStampTable; // Allocated later.

  Bit32u  fetchModeMask;

  bxICache_c()
  {
    // Initially clear the iCache;
    memset(this, 0, sizeof(*this));
    pageWriteStampTable = NULL;
    for (unsigned i=0; i<BxICacheEntries; i++) {
      entry[i].writeStamp = ICacheWriteStampInvalid;
    }
  }

  BX_CPP_INLINE void alloc(unsigned memSizeInBytes)
  {
    pageWriteStampTable =
        (Bit32u*) malloc(sizeof(Bit32u) * (memSizeInBytes>>12));
    for (unsigned i=0; i<(memSizeInBytes>>12); i++) {
      pageWriteStampTable[i] = ICacheWriteStampInvalid;
    }
    fetchModeMask = 0; // CS is 16-bit, Long Mode disabled, Data page
  }

  BX_CPP_INLINE void decWriteStamp(Bit32u a20Addr);

  BX_CPP_INLINE unsigned hash(Bit32u pAddr)
  {
    // A pretty dumb hash function for now.
    return pAddr & (BxICacheEntries-1);
  }
};

BX_CPP_INLINE void bxICache_c::decWriteStamp(Bit32u a20Addr)
{
  // Increment page write stamp, so iCache entries with older stamps
  // are effectively invalidated.
  Bit32u pageIndex = a20Addr >> 12;
  Bit32u writeStamp = pageWriteStampTable[pageIndex];
  if (writeStamp & 0x20000000)
  {
    // Page possibly contains iCache code.
    if (writeStamp & ICacheWriteStampMask) {
      // Short case: there is room to decrement the generation counter.
      pageWriteStampTable[pageIndex]--;
    }
    else {
      // Long case: there is no more room to decrement.  We have dump
      // all iCache entries which can possibly hash to this page since
      // we don't keep track of individual entries.

      // Take the hash of the 0th page offset.
      unsigned iCacheHash = hash(a20Addr & 0xfffff000);
      for (unsigned o=0; o<4096; o++) {
        entry[iCacheHash].writeStamp = ICacheWriteStampInvalid;
        iCacheHash = (iCacheHash + 1) % BxICacheEntries;
      }
      // Reset write stamp to highest value to begin the decrementing process
      // again.
      pageWriteStampTable[pageIndex] = ICacheWriteStampInvalid;
    }
  }
}

#endif
