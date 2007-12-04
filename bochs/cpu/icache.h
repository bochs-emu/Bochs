/////////////////////////////////////////////////////////////////////////
// $Id: icache.h,v 1.20 2007-12-04 17:34:20 sshwarts Exp $
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

#include <assert.h>

// bit31: 1=CS is 32/64-bit, 0=CS is 16-bit.
// bit30: 1=Long Mode, 0=not Long Mode.
// Combination bit31=1 & bit30=1 is invalid.
const Bit32u ICacheWriteStampInvalid = 0xffffffff;
const Bit32u ICacheWriteStampStart   = 0x3fffffff;
const Bit32u ICacheWriteStampMask    = 0x3fffffff;
const Bit32u ICacheFetchModeMask     = ~ICacheWriteStampMask;

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
    if (memSizeInBytes > 0) {
      delete [] pageWriteStampTable;
    }
    memSizeInBytes = memSize;
    pageWriteStampTable = new Bit32u [memSizeInBytes>>12];
    resetWriteStamps();
  }

  BX_CPP_INLINE Bit32u getPageWriteStamp(bx_phy_address pAddr) const
  {
    if (pAddr < memSizeInBytes) 
       return pageWriteStampTable[pAddr>>12];
    else 
       return ICacheWriteStampInvalid;
  }

  BX_CPP_INLINE const Bit32u *getPageWriteStampPtr(bx_phy_address pAddr) const
  {
    if (pAddr < memSizeInBytes) 
       return &pageWriteStampTable[pAddr>>12];
    else 
       return &ICacheWriteStampInvalid;
  }

  BX_CPP_INLINE void setPageWriteStamp(bx_phy_address pAddr, Bit32u pageWriteStamp)
  {
    if (pAddr < memSizeInBytes)
       pageWriteStampTable[pAddr>>12] = pageWriteStamp;
  }

  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr)
  {
    assert(pAddr < memSizeInBytes);
    // Decrement page write stamp, so iCache entries with older stamps
    // are effectively invalidated.
    pageWriteStampTable[pAddr >> 12]--;
  }

  BX_CPP_INLINE void resetWriteStamps(void);
  BX_CPP_INLINE void purgeWriteStamps(void);
};

BX_CPP_INLINE void bxPageWriteStampTable::resetWriteStamps(void)
{
  for (Bit32u i=0; i<(memSizeInBytes>>12); i++) {
    pageWriteStampTable[i] = ICacheWriteStampInvalid;
  }
}

BX_CPP_INLINE void bxPageWriteStampTable::purgeWriteStamps(void)
{
  for (Bit32u i=0; i<(memSizeInBytes>>12); i++) {
    pageWriteStampTable[i] |= ICacheWriteStampMask;
  }
}

extern bxPageWriteStampTable pageWriteStampTable;

#define BxICacheEntries (32 * 1024)  // Must be a power of 2.

struct bxICacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  Bit32u writeStamp;    // Generation ID. Each write to a physical page
                        // decrements this value
  bxInstruction_c i;    // The instruction decode information
};

class BOCHSAPI bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];

public:
  bxICache_c() { flushICacheEntries(); }

  BX_CPP_INLINE unsigned hash(bx_phy_address pAddr) const
  {
    // A pretty dumb hash function for now.
    return pAddr & (BxICacheEntries-1);
  }

  BX_CPP_INLINE void purgeICacheEntries(void);
  BX_CPP_INLINE void flushICacheEntries(void);
};

BX_CPP_INLINE void bxICache_c::flushICacheEntries(void)
{
    bxICacheEntry_c* e = entry;
    for (unsigned i=0; i<BxICacheEntries; i++, e++) {
      e->writeStamp = ICacheWriteStampInvalid;
    }
}

BX_CPP_INLINE void bxICache_c::purgeICacheEntries(void)
{
  bxICacheEntry_c* e = entry;
    
  // Since the write stamps may overflow if we always simply decrese them,
  // this function has to be called often enough that we can reset them
  // (without invalidating the cache).
  for (unsigned i=0;i<BxICacheEntries;i++,e++)
  {
    Bit32u pageWriteStamp = pageWriteStampTable.getPageWriteStamp(e->pAddr);
    if (e->writeStamp != pageWriteStamp)
      e->writeStamp = ICacheWriteStampInvalid;	// invalidate entry
    else
      e->writeStamp |= ICacheWriteStampMask;
  }
}

extern void purgeICaches(void);
extern void flushICaches(void);

#endif
