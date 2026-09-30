/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2025 Stanislav Shwartsman
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

#ifndef BX_ICACHE_H
#define BX_ICACHE_H

extern void handleSMC(bx_phy_address pAddr, Bit32u mask);

class alignas(64) bxPageWriteStampTable
{
  const Bit32u PHY_MEM_PAGES_IN_4G_SPACE;
  Bit32u *fineGranularityMapping;

public:
  bxPageWriteStampTable(): PHY_MEM_PAGES_IN_4G_SPACE(1024*1024) {
    fineGranularityMapping = new Bit32u[PHY_MEM_PAGES_IN_4G_SPACE];
    resetWriteStamps();
  }
 ~bxPageWriteStampTable() { delete [] fineGranularityMapping; }

  BX_CPP_INLINE static Bit32u hash(bx_phy_address pAddr) {
    // can share writeStamps between multiple pages if >32 bit phy address
    return ((Bit32u) pAddr) >> 12;
  }

  BX_CPP_INLINE Bit32u getFineGranularityMapping(bx_phy_address pAddr) const
  {
    return fineGranularityMapping[hash(pAddr)];
  }

  BX_CPP_INLINE void markICache(bx_phy_address pAddr, unsigned len)
  {
    Bit32u mask  = 1 << (PAGE_OFFSET((Bit32u) pAddr) >> 7);
           mask |= 1 << (PAGE_OFFSET((Bit32u) pAddr + len - 1) >> 7);

    fineGranularityMapping[hash(pAddr)] |= mask;
  }

  BX_CPP_INLINE void markICacheMask(bx_phy_address pAddr, Bit32u mask)
  {
    fineGranularityMapping[hash(pAddr)] |= mask;
  }

  // whole page is being altered
  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr)
  {
    Bit32u index = hash(pAddr);

    if (fineGranularityMapping[index]) {
      handleSMC(pAddr, 0xffffffff); // one of the CPUs might be running trace from this page
      fineGranularityMapping[index] = 0;
    }
  }

  // assumption: write does not split 4K page
  BX_CPP_INLINE void decWriteStamp(bx_phy_address pAddr, unsigned len)
  {
    Bit32u index = hash(pAddr);

    if (fineGranularityMapping[index]) {
       Bit32u mask  = 1 << (PAGE_OFFSET((Bit32u) pAddr) >> 7);
              mask |= 1 << (PAGE_OFFSET((Bit32u) pAddr + len - 1) >> 7);

       if (fineGranularityMapping[index] & mask) {
          // one of the CPUs might be running trace from this page
          handleSMC(pAddr, mask);
          fineGranularityMapping[index] &= ~mask;
       }
    }
  }

  BX_CPP_INLINE void resetWriteStamps(void);
};

BX_CPP_INLINE void bxPageWriteStampTable::resetWriteStamps(void)
{
  for (Bit32u i=0; i<PHY_MEM_PAGES_IN_4G_SPACE; i++) {
    fineGranularityMapping[i] = 0;
  }
}

extern bxPageWriteStampTable pageWriteStampTable;

// fetchModeMask is XOR'ed into the entry index, its bits above bit 6 could move
// the entry into another 128-byte "cache line" of the page
#define BX_ICACHE_LINE_DISPLACEMENT_MASK (((1 << BX_FETCH_MODE_MASK_BITS) - 1) >> 7)

// XOR'ing fetchModeMask into the entry index must keep the entry within its 4K page
static_assert(BX_FETCH_MODE_MASK_BITS <= 12, "fetchModeMask is too wide for the trace cache and instruction cache index");

static const bx_phy_address BX_ICACHE_INVALID_PHY_ADDRESS = bx_phy_address(-1);

#define BxICacheEntries (512 * 1024)  // Must be a power of 2.

// Instruction cache - physical address indexed direct mapped cache of
// individual decoded instructions, shared by all CPUs. Instructions crossing
// the page boundary are never stored in the instruction cache.
struct bxICacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  bxInstruction_c i;
};

// XOR'ing fetchModeMask into the entry index must keep the entry within its 4K page
static_assert(BxICacheEntries >= 4096, "the instruction cache is too small");

class alignas(64) bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];

public:
  bxICache_c() { flushICacheEntries(); }

  // fetchModeMask is XOR'ed into the entry index, so an entry which matches
  // pAddr could be only created with the same fetchModeMask and no need to
  // keep the fetchModeMask in the entry
  BX_CPP_INLINE static unsigned hash(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    return ((pAddr) & (BxICacheEntries-1)) ^ fetchModeMask;
  }

  BX_CPP_INLINE bxICacheEntry_c* get_entry(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    return &(entry[hash(pAddr, fetchModeMask)]);
  }

  BX_CPP_INLINE bxICacheEntry_c* find_entry(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    bxICacheEntry_c* e = get_entry(pAddr, fetchModeMask);
    if (e->pAddr != pAddr)
       return NULL;

    return e;
  }

  BX_CPP_INLINE void handleSMC(bx_phy_address pAddr, Bit32u mask);

  BX_CPP_INLINE void flushICacheEntries(void);
};

BX_CPP_INLINE void bxICache_c::flushICacheEntries(void)
{
  bxICacheEntry_c* e = entry;

  for (unsigned i=0; i<BxICacheEntries; i++, e++) {
    e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
  }
}

BX_CPP_INLINE void bxICache_c::handleSMC(bx_phy_address pAddr, Bit32u mask)
{
  Bit32u pAddrIndex = bxPageWriteStampTable::hash(pAddr);

  // Invalidate all instructions touching one of the modified "cache lines".
  // Multiple physical addresses could be mapped into single pageWriteStampTable
  // entry and all of them have to be invalidated here now.

  bxICacheEntry_c *e = get_entry(LPFOf(pAddr), 0);

  // go over 32 "cache lines" of 128 byte each
  for (unsigned n=0; n < 32; n++) {
    // fetchModeMask is XOR'ed into entry index (see hash() function) and could
    // move the entry away from its natural "cache line"
    Bit32u line_mask = (1 << (n & ~BX_ICACHE_LINE_DISPLACEMENT_MASK));
    if (line_mask > mask) break;
    for (unsigned index=0; index < 128; index++, e++) {
      if (pAddrIndex == bxPageWriteStampTable::hash(e->pAddr)) {
        Bit32u pageOffset = PAGE_OFFSET((Bit32u) e->pAddr);
        Bit32u instrMask  = 1 << (pageOffset >> 7);
               instrMask |= 1 << ((pageOffset + e->i.ilen() - 1) >> 7);
        if (instrMask & mask)
          e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
      }
    }
  }
}

extern bxICache_c iCache;

#define BxTraceCacheEntries (64  * 1024)  // Must be a power of 2.
#define BxTraceCacheMemPool (576 * 1024)

struct bxTraceCacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  Bit32u traceMask;

  Bit32u tlen;          // Trace length in instructions
  bxInstruction_c *i;
};

#define BX_MAX_TRACE_LENGTH 32

void flushSMC(bxTraceCacheEntry_c *e);

class alignas(64) bxTraceCache_c {
public:
  bxTraceCacheEntry_c entry[BxTraceCacheEntries];
  bxInstruction_c mpool[BxTraceCacheMemPool];
  unsigned mpindex;

  Bit32u traceLinkTimeStamp;

#define BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES 8 /* must be power of two */
  struct pageSplitEntryIndex {
    bx_phy_address ppf; // Physical address of 2nd page of the trace
    bxTraceCacheEntry_c *e; // Pointer to trace cache entry
  } pageSplitIndex[BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES];
  int nextPageSplitIndex;

public:
  bxTraceCache_c() { flushTraceCacheEntries(); }

  BX_CPP_INLINE static unsigned hash(bx_phy_address pAddr, unsigned fetchModeMask)
  {
//  return ((pAddr + (pAddr << 2) + (pAddr>>6)) & (BxTraceCacheEntries-1)) ^ fetchModeMask;
    return ((pAddr) & (BxTraceCacheEntries-1)) ^ fetchModeMask;
  }

  BX_CPP_INLINE void alloc_trace(bxTraceCacheEntry_c *e)
  {
    // took +1 garbend for instruction chaining speedup (end-of-trace opcode)
    if ((mpindex + BX_MAX_TRACE_LENGTH + 1) > BxTraceCacheMemPool) {
      flushTraceCacheEntries();
    }
    e->i = &mpool[mpindex];
    e->tlen = 0;
  }

  BX_CPP_INLINE void commit_trace(unsigned len) { mpindex += len; }

  BX_CPP_INLINE void commit_page_split_trace(bx_phy_address paddr, bxTraceCacheEntry_c *e)
  {
    mpindex += e->tlen;

    // register page split entry
    if (pageSplitIndex[nextPageSplitIndex].ppf != BX_ICACHE_INVALID_PHY_ADDRESS)
      pageSplitIndex[nextPageSplitIndex].e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;

    pageSplitIndex[nextPageSplitIndex].ppf = paddr;
    pageSplitIndex[nextPageSplitIndex].e = e;

    nextPageSplitIndex = (nextPageSplitIndex+1) & (BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES-1);
  }

  BX_CPP_INLINE void handleSMC(bx_phy_address pAddr, Bit32u mask);

  BX_CPP_INLINE void flushTraceCacheEntries(void);
  BX_CPP_INLINE void invalidatePageSplitTraceCacheEntries(void);

  BX_CPP_INLINE bxTraceCacheEntry_c* get_entry(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    return &(entry[hash(pAddr, fetchModeMask)]);
  }

  BX_CPP_INLINE bxTraceCacheEntry_c* find_entry(bx_phy_address pAddr, unsigned fetchModeMask)
  {
    bxTraceCacheEntry_c* e = get_entry(pAddr, fetchModeMask);
    if (e->pAddr != pAddr)
       return NULL;

    return e;
  }

  BX_CPP_INLINE bool breakLinks()
  {
    invalidatePageSplitTraceCacheEntries();

    // break all links between traces
    if (++traceLinkTimeStamp == 0xffffffff) {
      flushTraceCacheEntries();
      return true;
    }
    return false;
  }
};

BX_CPP_INLINE void bxTraceCache_c::flushTraceCacheEntries(void)
{
  bxTraceCacheEntry_c* e = entry;

  for (unsigned i=0; i<BxTraceCacheEntries; i++, e++) {
    e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
    e->traceMask = 0;
  }

  // flush all page split entries
  nextPageSplitIndex = 0;
  for (unsigned i=0;i<BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES;i++)
    pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;

  mpindex = 0;

  traceLinkTimeStamp = 0;
}

BX_CPP_INLINE void bxTraceCache_c::handleSMC(bx_phy_address pAddr, Bit32u mask)
{
  Bit32u pAddrIndex = bxPageWriteStampTable::hash(pAddr);

  // break all links between traces
  if (breakLinks()) return;

  // Need to invalidate all traces in the trace cache that might include an
  // instruction that was modified.  But this is not enough, it is possible
  // that some another trace is linked into  invalidated trace and it won't
  // be invalidated. In order to solve this issue  replace all instructions
  // from the invalidated trace with dummy EndOfTrace opcodes.

  // Another corner case that has to be handled - pageWriteStampTable wrap.
  // Multiple physical addresses could be mapped into single pageWriteStampTable
  // entry and all of them have to be invalidated here now.

  if (mask & 0x1) {
    // the store touched 1st cache line in the page, check for
    // page split traces to invalidate.
    for (unsigned i=0;i<BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES;i++) {
      if (pageSplitIndex[i].ppf != BX_ICACHE_INVALID_PHY_ADDRESS) {
        if (pAddrIndex == bxPageWriteStampTable::hash(pageSplitIndex[i].ppf)) {
          pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;
          flushSMC(pageSplitIndex[i].e);
        }
      }
    }
  }

  bxTraceCacheEntry_c *e = get_entry(LPFOf(pAddr), 0);

  // go over 32 "cache lines" of 128 byte each
  for (unsigned n=0; n < 32; n++) {
    // fetchModeMask is XOR'ed into entry index (see hash() function) and could
    // move the entry away from its natural "cache line"
    Bit32u line_mask = (1 << (n & ~BX_ICACHE_LINE_DISPLACEMENT_MASK));
    if (line_mask > mask) break;
    for (unsigned index=0; index < 128; index++, e++) {
      if (pAddrIndex == bxPageWriteStampTable::hash(e->pAddr) && (e->traceMask & mask) != 0) {
        flushSMC(e);
      }
    }
  }
}

BX_CPP_INLINE void bxTraceCache_c::invalidatePageSplitTraceCacheEntries(void)
{
  for (unsigned i=0;i<BX_TRACE_CACHE_PAGE_SPLIT_ENTRIES;i++) {
    if (pageSplitIndex[i].ppf != BX_ICACHE_INVALID_PHY_ADDRESS) {
      pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;
      flushSMC(pageSplitIndex[i].e);
    }
  }
  nextPageSplitIndex = 0;
}

extern void flushICaches(void);

#endif
