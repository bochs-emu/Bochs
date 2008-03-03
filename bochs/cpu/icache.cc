/////////////////////////////////////////////////////////////////////////
// $Id: icache.cc,v 1.9 2008-03-03 16:22:31 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007 Stanislav Shwartsman
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_SUPPORT_ICACHE

bxPageWriteStampTable pageWriteStampTable;

void purgeICaches(void)
{
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++)
    BX_CPU(i)->iCache.purgeICacheEntries();

  pageWriteStampTable.purgeWriteStamps();
}

void flushICaches(void)
{
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++)
    BX_CPU(i)->iCache.flushICacheEntries();

  pageWriteStampTable.resetWriteStamps();
}

#define InstrumentTRACECACHE 0

#if InstrumentTRACECACHE
static Bit32u iCacheLookups=0;
static Bit32u iCacheMisses=0;
static Bit32u iCacheMergeTraces=0;
static Bit32u iCacheTraceLengh[BX_MAX_TRACE_LENGTH];

#define InstrICache_StatsMask 0x3ffffff

#define InstrICache_Stats() {\
  if ((iCacheLookups & InstrICache_StatsMask) == 0) { \
    BX_INFO(("ICACHE lookups: %u, misses: %u, merges: %u, hit rate = %3.2f%%", \
          (unsigned) iCacheLookups, \
          (unsigned) iCacheMisses,  \
          (unsigned) iCacheMergeTraces, \
          (iCacheLookups-iCacheMisses) * 100.0 / iCacheLookups)); \
    for (unsigned trace_len_idx=0; trace_len_idx<BX_MAX_TRACE_LENGTH;trace_len_idx++) { \
      BX_INFO(("trace[%02d]: %u\t(%3.2f%%)", trace_len_idx+1, \
         iCacheTraceLengh[trace_len_idx], \
         iCacheTraceLengh[trace_len_idx] * 100.0/(iCacheLookups-iCacheMisses))); \
      iCacheTraceLengh[trace_len_idx] = 0; \
    } \
    iCacheLookups = iCacheMisses = iCacheMergeTraces = 0; \
  } \
}
#define InstrICache_Increment(v) (v)++

#else
#define InstrICache_Stats()
#define InstrICache_Increment(v)
#endif

#endif // InstrumentTRACECACHE

#if BX_SUPPORT_TRACE_CACHE

bxInstruction_c* BX_CPU_C::fetchInstructionTrace(Bit32u eipBiased, unsigned *len)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrA20Page + eipBiased;
  bxICacheEntry_c *trace = BX_CPU_THIS_PTR iCache.get_entry(pAddr);
  Bit32u pageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);

  InstrICache_Increment(iCacheLookups);
  InstrICache_Stats();

  if ((trace->pAddr == pAddr) &&
      (trace->writeStamp == pageWriteStamp))
  {
     // We are lucky - trace cache hit !
     InstrICache_Increment(iCacheTraceLengh[trace->ilen-1]);
     *len = trace->ilen;
     return trace->i;
  }

  // We are not so lucky, but let's be optimistic - try to build trace from
  // incoming instruction bytes stream !
  trace->pAddr = pAddr;
  trace->writeStamp = pageWriteStamp;
  trace->ilen = 0;

  InstrICache_Increment(iCacheMisses);

  unsigned remainingInPage = BX_CPU_THIS_PTR eipPageWindowSize - eipBiased;
  const Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;
  unsigned ret;

  bxInstruction_c *i = trace->i;

  for (unsigned n=0;n<BX_MAX_TRACE_LENGTH;n++)
  {
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
      ret = fetchDecode64(fetchPtr, i, remainingInPage);
    else
#endif
      ret = fetchDecode32(fetchPtr, i, remainingInPage);

    if (ret==0) {
      // Fetching instruction on segment/page boundary
      if (n > 0) {
         // The trace is already valid, it has several instructions inside,
         // in this case just drop the boundary instruction and stop
         // tracing.
         break;
      }
      // First instruction is boundary fetch, return iStorage and leave
      // the trace cache entry invalid (do not cache the instruction)
      trace->writeStamp = ICacheWriteStampInvalid;
      trace->ilen = 1;
      boundaryFetch(fetchPtr, remainingInPage, trace->i);
      break;
    }

    // add instruction to the trace ...
    unsigned iLen = i->ilen();
    trace->ilen++;

    // ... and continue to the next instruction
    remainingInPage -= iLen;
    if (i->getStopTraceAttr() || remainingInPage == 0) break;
    pAddr += iLen;
    fetchPtr += iLen;
    i++;

    // try to find a trace starting from current pAddr and merge
    if (mergeTraces(trace, i, pAddr)) break;
  }

  *len = trace->ilen;
  return trace->i;
}

bx_bool BX_CPU_C::mergeTraces(bxICacheEntry_c *entry, bxInstruction_c *i, bx_phy_address pAddr)
{
  bxICacheEntry_c *e = BX_CPU_THIS_PTR iCache.get_entry(pAddr);

  if ((e->pAddr == pAddr) && (e->writeStamp == entry->writeStamp))
  {
    // We are lucky - another trace hit !
    InstrICache_Increment(iCacheMergeTraces);

    // determine max amount of instruction to take from another trace
    unsigned max_length = e->ilen;
    if (max_length + entry->ilen > BX_MAX_TRACE_LENGTH)
        max_length = BX_MAX_TRACE_LENGTH - entry->ilen;
    if(max_length == 0) return 0;

    memcpy(i, e->i, sizeof(bxInstruction_c)*max_length);
    entry->ilen += max_length;
    BX_ASSERT(entry->ilen <= BX_MAX_TRACE_LENGTH);

    return 1;
  }

  return 0;
}

void BX_CPU_C::instrumentTraces(void)
{
  Bit32u currPageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);
  bxICacheEntry_c *e = BX_CPU_THIS_PTR iCache.entry;
  Bit32u trace_length[BX_MAX_TRACE_LENGTH], invalid_entries = 0;
  unsigned i;

  for (i=0; i < BX_MAX_TRACE_LENGTH; i++) trace_length[i] = 0;

  for (i=0; i<BxICacheEntries; i++, e++) {
    if (e->writeStamp == currPageWriteStamp)
      trace_length[e->ilen-1]++;
    else
      invalid_entries++;
  }

  for (i=0; i < BX_MAX_TRACE_LENGTH; i++) {
    BX_INFO(("traces[%02d]: %u\t%f%%", i+1,
       trace_length[i], trace_length[i]*100.0/BxICacheEntries));
  }
  BX_INFO(("invalid entries: %u\t%f%%",
       invalid_entries, invalid_entries*100.0/BxICacheEntries));
}

#else // BX_SUPPORT_TRACE_CACHE == 0

bxInstruction_c* BX_CPU_C::fetchInstruction(bxInstruction_c *iStorage, Bit32u eipBiased)
{
  unsigned ret;
  bxInstruction_c *i = iStorage;

#if BX_SUPPORT_ICACHE
  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrA20Page + eipBiased;
  bxICacheEntry_c *cache_entry = BX_CPU_THIS_PTR iCache.get_entry(pAddr);
  i = cache_entry->i;

  Bit32u pageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);

  InstrICache_Increment(iCacheLookups);
  InstrICache_Stats();

  if ((cache_entry->pAddr == pAddr) &&
      (cache_entry->writeStamp == pageWriteStamp))
  {
    // iCache hit. Instruction is already decoded and stored in the
    // instruction cache.
#if BX_INSTRUMENTATION
    // An instruction was found in the iCache.
    BX_INSTR_OPCODE(BX_CPU_ID, BX_CPU_THIS_PTR eipFetchPtr + eipBiased,
       i->ilen(), BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, Is64BitMode());
#endif
    return i;
  }
#endif

  // iCache miss. No validated instruction with matching fetch parameters
  // is in the iCache. Or we're not compiling iCache support in, in which
  // case we always have an iCache miss.  :^)
  unsigned remainingInPage = BX_CPU_THIS_PTR eipPageWindowSize - eipBiased;
  const Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;

#if BX_SUPPORT_ICACHE
  // The entry will be marked valid if fetchdecode will succeed
  cache_entry->writeStamp = ICacheWriteStampInvalid;
  InstrICache_Increment(iCacheMisses);
#endif

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    ret = fetchDecode64(fetchPtr, i, remainingInPage);
  else
#endif
    ret = fetchDecode32(fetchPtr, i, remainingInPage);

  if (ret==0) {
    // return iStorage and leave icache entry invalid (do not cache instr)
    boundaryFetch(fetchPtr, remainingInPage, iStorage);
    return iStorage;
  }
  else
  {
#if BX_SUPPORT_ICACHE
    cache_entry->pAddr = pAddr;
    cache_entry->writeStamp = pageWriteStamp;
#endif
#if BX_INSTRUMENTATION
    // An instruction was either fetched, or found in the iCache.
    BX_INSTR_OPCODE(BX_CPU_ID, fetchPtr, i->ilen(),
         BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, Is64BitMode());
#endif
  }

  return i;
}

#endif // #if BX_SUPPORT_ICACHE
