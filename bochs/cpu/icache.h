/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2015 Stanislav Shwartsman
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

class bxPageWriteStampTable
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

#define BxICacheEntries (64  * 1024)  // Must be a power of 2.
#define BxICacheMemPool (576 * 1024)

struct bxICacheEntry_c
{
  bx_phy_address pAddr; // Physical address of the instruction
  Bit32u traceMask;

  Bit32u tlen;          // Trace length in instructions
  bxInstruction_c *i;
};

#define BX_MAX_TRACE_LENGTH 32

static const bx_phy_address BX_ICACHE_INVALID_PHY_ADDRESS = bx_phy_address(-1);

void flushSMC(bxICacheEntry_c *e);

class BOCHSAPI bxICache_c {
public:
  bxICacheEntry_c entry[BxICacheEntries];
  bxInstruction_c mpool[BxICacheMemPool];
  unsigned mpindex;

  Bit32u traceLinkTimeStamp;

#define BX_ICACHE_PAGE_SPLIT_ENTRIES 8 /* must be power of two */
  struct pageSplitEntryIndex {
    bx_phy_address ppf; // Physical address of 2nd page of the trace
    bxICacheEntry_c *e; // Pointer to icache entry
  } pageSplitIndex[BX_ICACHE_PAGE_SPLIT_ENTRIES];
  int nextPageSplitIndex;

public:
  bxICache_c() { flushICacheEntries(); }

  BX_CPP_INLINE static unsigned hash(bx_phy_address pAddr, unsigned fetchModeMask)
  {
//  return ((pAddr + (pAddr << 2) + (pAddr>>6)) & (BxICacheEntries-1)) ^ fetchModeMask;
    return ((pAddr) & (BxICacheEntries-1)) ^ fetchModeMask;
  }

  BX_CPP_INLINE void alloc_trace(bxICacheEntry_c *e)
  {
    // took +1 garbend for instruction chaining speedup (end-of-trace opcode)
    if ((mpindex + BX_MAX_TRACE_LENGTH + 1) > BxICacheMemPool) {
      flushICacheEntries();
    }
    e->i = &mpool[mpindex];
    e->tlen = 0;
  }

  BX_CPP_INLINE void commit_trace(unsigned len) { mpindex += len; }

  BX_CPP_INLINE void commit_page_split_trace(bx_phy_address paddr, bxICacheEntry_c *e)
  {
    mpindex += e->tlen;

    // register page split entry
    if (pageSplitIndex[nextPageSplitIndex].ppf != BX_ICACHE_INVALID_PHY_ADDRESS)
      pageSplitIndex[nextPageSplitIndex].e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;

    pageSplitIndex[nextPageSplitIndex].ppf = paddr;
    pageSplitIndex[nextPageSplitIndex].e = e;

    nextPageSplitIndex = (nextPageSplitIndex+1) & (BX_ICACHE_PAGE_SPLIT_ENTRIES-1);
  }

  BX_CPP_INLINE void handleSMC(bx_phy_address pAddr, Bit32u mask);

  BX_CPP_INLINE void flushICacheEntries(void);

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

  BX_CPP_INLINE bool breakLinks()
  {
    // break all links bewteen traces
    if (++traceLinkTimeStamp == 0xffffffff) {
      flushICacheEntries();
      return true;
    }
    return false;
  }
};

BX_CPP_INLINE void bxICache_c::flushICacheEntries(void)
{
  bxICacheEntry_c* e = entry;
  unsigned i;

  for (i=0; i<BxICacheEntries; i++, e++) {
    e->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
    e->traceMask = 0;
  }

  nextPageSplitIndex = 0;
  for (i=0;i<BX_ICACHE_PAGE_SPLIT_ENTRIES;i++)
    pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;

  mpindex = 0;

  traceLinkTimeStamp = 0;
}

BX_CPP_INLINE void bxICache_c::handleSMC(bx_phy_address pAddr, Bit32u mask)
{
  Bit32u pAddrIndex = bxPageWriteStampTable::hash(pAddr);

  // break all links bewteen traces
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
    for (unsigned i=0;i<BX_ICACHE_PAGE_SPLIT_ENTRIES;i++) {
      if (pageSplitIndex[i].ppf != BX_ICACHE_INVALID_PHY_ADDRESS) {
        if (pAddrIndex == bxPageWriteStampTable::hash(pageSplitIndex[i].ppf)) {
          pageSplitIndex[i].ppf = BX_ICACHE_INVALID_PHY_ADDRESS;
          flushSMC(pageSplitIndex[i].e);
        }
      }
    }
  }

  bxICacheEntry_c *e = get_entry(LPFOf(pAddr), 0);

  // go over 32 "cache lines" of 128 byte each
  for (unsigned n=0; n < 32; n++) {
    Bit32u line_mask = (1 << n);
    if (line_mask > mask) break;
    for (unsigned index=0; index < 128; index++, e++) {
      if (pAddrIndex == bxPageWriteStampTable::hash(e->pAddr) && (e->traceMask & mask) != 0) {
        flushSMC(e);
      }
    }
  }
}

extern void flushICaches(void);

#endif
