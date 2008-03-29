/////////////////////////////////////////////////////////////////////////
// $Id: icache.h,v 1.31 2008-03-29 21:03:38 sshwarts Exp $
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
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_ICACHE_H
#define BX_ICACHE_H

#if BX_SUPPORT_ICACHE

#include <assert.h>

// bit31: 1=CS is 32/64-bit, 0=CS is 16-bit.
// bit30: 1=Long Mode, 0=not Long Mode.
// Combination bit31=1 & bit30=1 is invalid (data page)
const Bit32u ICacheWriteStampInvalid  = 0xffffffff;
const Bit32u ICacheWriteStampStart    = 0x3fffffff;
const Bit32u ICacheWriteStampFetchModeMask = ~ICacheWriteStampStart;

#if BX_SUPPORT_TRACE_CACHE
extern void stopTraceExecution(void);
#endif

class bxPageWriteStampTable
{
  // A table (dynamically allocated) to store write-stamp generation IDs.
  // Each time a write occurs to a physical page, a generation ID is
  // decremented. Only iCache entries which have write stamps matching
  // the physical page write stamp are valid.
  Bit32u *pageWriteStampTable;

#define PHY_MEM_PAGES (1024*1024)

public:
  bxPageWriteStampTable() {
    pageWriteStampTable = new Bit32u[PHY_MEM_PAGES];
    resetWriteStamps();
  }
 ~bxPageWriteStampTable() { delete [] pageWriteStampTable; }

  BX_CPP_INLINE Bit32u getPageWriteStamp(bx_phy_address pAddr) const
  {
    return pageWriteStampTable[pAddr>>12];
  }

  BX_CPP_INLINE const Bit32u *getPageWriteStampPtr(bx_phy_address pAddr) const
  {
    return &pageWriteStampTable[pAddr>>12];
  }

  BX_CPP_INLINE void setPageWriteStamp(bx_phy_address pAddr, Bit32u pageWriteStamp)
  {
    pageWriteStampTable[pAddr>>12] = pageWriteStamp;
  }

  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr)
  {
    pAddr >>= 12;
#if BX_SUPPORT_TRACE_CACHE
    if ((pageWriteStampTable[pAddr] & ICacheWriteStampFetchModeMask) != ICacheWriteStampFetchModeMask) {
      stopTraceExecution(); // one of the CPUs running trace from this page
    }
#endif
    // Decrement page write stamp, so iCache entries with older stamps
    // are effectively invalidated.
    pageWriteStampTable[pAddr]--;
  }

  BX_CPP_INLINE void resetWriteStamps(void);
  BX_CPP_INLINE void purgeWriteStamps(void);
};

BX_CPP_INLINE void bxPageWriteStampTable::resetWriteStamps(void)
{
  for (Bit32u i=0; i<PHY_MEM_PAGES; i++) {
    pageWriteStampTable[i] = ICacheWriteStampInvalid;
  }
}

BX_CPP_INLINE void bxPageWriteStampTable::purgeWriteStamps(void)
{
  for (Bit32u i=0; i<PHY_MEM_PAGES; i++) {
    pageWriteStampTable[i] |= ICacheWriteStampStart;
  }
}

extern bxPageWriteStampTable pageWriteStampTable;

#define BxICacheEntries (32 * 1024)  // Must be a power of 2.

#if BX_SUPPORT_TRACE_CACHE
  #define BX_MAX_TRACE_LENGTH 15
#endif

struct bxICacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  Bit32u writeStamp;    // Generation ID. Each write to a physical page
                        // decrements this value
#if BX_SUPPORT_TRACE_CACHE
  Bit32u ilen;          // Trace length in instructions
  bxInstruction_c i[BX_MAX_TRACE_LENGTH];
#else
  // ... define as array of 1 to simplify merge with trace cache code
  bxInstruction_c i[1];
#endif
};

class BOCHSAPI bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];

public:
  bxICache_c() { flushICacheEntries(); }

  BX_CPP_INLINE unsigned hash(bx_phy_address pAddr) const
  {
    // A pretty dumb hash function for now.
    return (pAddr + (pAddr>>6)) & (BxICacheEntries-1);
  }

  BX_CPP_INLINE void purgeICacheEntries(void);
  BX_CPP_INLINE void flushICacheEntries(void);

  BX_CPP_INLINE bxICacheEntry_c* get_entry(bx_phy_address pAddr)
  {
    return &(entry[hash(pAddr)]);
  }
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
      e->writeStamp |= ICacheWriteStampStart;
  }
}

extern void purgeICaches(void);
extern void flushICaches(void);

#endif // BX_SUPPORT_ICACHE

#endif
