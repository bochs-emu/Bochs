/////////////////////////////////////////////////////////////////////////
// $Id: icache.h,v 1.6.2.1 2005-07-07 08:01:51 vruppert Exp $
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
#define BX_ICACHE_H

// bit31: 1=CS is 32/64-bit, 0=CS is 16-bit.
// bit30: 1=Long Mode, 0=not Long Mode.
// bit29: 1=iCache page, 0=Data.
#define ICacheWriteStampInvalid   0x1fffffff
#define ICacheWriteStampMax       0x1fffffff // Decrements from here.
#define ICacheWriteStampMask      0x1fffffff
#define ICacheFetchModeMask       (~ICacheWriteStampMask)
#define iCachePageDataMask        0x20000000

class bxPageWriteStampTable
{
  // A table (dynamically allocated) to store write-stamp generation IDs.  
  // Each time a write occurs to a physical page, a generation ID is 
  // decremented. Only iCache entries which have write stamps matching 
  // the physical page write stamp are valid.

  Bit32u *pageWriteStampTable;
  Bit32u  memSizeInBytes;

public:
  bxPageWriteStampTable(): pageWriteStampTable(NULL), memSizeInBytes(0) {}
  bxPageWriteStampTable(Bit32u memSize) { alloc(memSize); }
 ~bxPageWriteStampTable() { delete [] pageWriteStampTable; }

  BX_CPP_INLINE void alloc(Bit32u memSize)
  {
    memSizeInBytes = memSize;
    pageWriteStampTable = new Bit32u [memSizeInBytes>>12];
    resetWriteStamps();
  }

  BX_CPP_INLINE Bit32u getPageWriteStamp(Bit32u pAddr) const
  {
    if (pAddr < memSizeInBytes) 
       return pageWriteStampTable[pAddr>>12];
    else 
       return ICacheWriteStampInvalid;
  }

  BX_CPP_INLINE void setPageWriteStamp(Bit32u pAddr, Bit32u pageWriteStamp)
  {
    if (pAddr < memSizeInBytes) 
       pageWriteStampTable[pAddr>>12] = pageWriteStamp;
  }

  BX_CPP_INLINE void decWriteStamp(Bit32u a20Addr)
  {
    // Increment page write stamp, so iCache entries with older stamps
    // are effectively invalidated.
    Bit32u pageIndex = a20Addr >> 12;
    Bit32u writeStamp = pageWriteStampTable[pageIndex];

    if (writeStamp & iCachePageDataMask)
    {
      pageWriteStampTable[pageIndex]--;
    }
  }

  BX_CPP_INLINE void resetWriteStamps(void);
};

BX_CPP_INLINE void bxPageWriteStampTable::resetWriteStamps(void)
{
  for (Bit32u i=0; i<(memSizeInBytes>>12); i++) {
    pageWriteStampTable[i] = ICacheWriteStampInvalid;
  }
}

extern bxPageWriteStampTable pageWriteStampTable;

#define BxICacheEntries (32 * 1024)  // Must be a power of 2.

class bxICacheEntry_c {
  public:

  Bit32u pAddr;       // Physical address of the instruction.
  Bit32u writeStamp;  // Generation ID.  Each write to a physical page
                      // decrements this value.
  bxInstruction_c i;  // The instruction decode information.
};

class BOCHSAPI bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];
  Bit32u fetchModeMask;

public:
  bxICache_c()
  {
    // Initially clear the iCache;
    for (unsigned i=0; i<BxICacheEntries; i++) {
      entry[i].writeStamp = ICacheWriteStampInvalid;
    }
    fetchModeMask = 0; // CS is 16-bit, Long Mode disabled, Data page
  }

  BX_CPP_INLINE unsigned hash(Bit32u pAddr) const
  {
    // A pretty dumb hash function for now.
    return pAddr & (BxICacheEntries-1);
  }

  BX_CPP_INLINE void purgeICacheEntries(void);
};

BX_CPP_INLINE void bxICache_c::purgeICacheEntries(void)
{
  // Since the write stamps may overflow if we always simply decrese them,
  // this function has to be called often enough that we can reset them
  // (without invalidating the cache).
  for (unsigned i=0;i<BxICacheEntries;i++)
  {
    bxICacheEntry_c *e = &entry[i];
    Bit32u pageWriteStamp = pageWriteStampTable.getPageWriteStamp(e->pAddr);
    if (e->writeStamp != pageWriteStamp)
      e->writeStamp = ICacheWriteStampInvalid;	// invalidate entry
    else
      e->writeStamp |= 0x1fffffff;
  }
}

extern void purgeICache(void);

#endif
