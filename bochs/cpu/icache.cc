/////////////////////////////////////////////////////////////////////////
// $Id: icache.cc,v 1.43 2011-01-25 20:59:26 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2007-2011 Stanislav Shwartsman
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

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RIP EIP
#endif

bxPageWriteStampTable pageWriteStampTable;

void flushICaches(void)
{
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
    BX_CPU(i)->iCache.flushICacheEntries();
#if BX_SUPPORT_TRACE_CACHE
    BX_CPU(i)->async_event |= BX_ASYNC_EVENT_STOP_TRACE;
#endif
  }

  pageWriteStampTable.resetWriteStamps();
}

void handleSMC(bx_phy_address pAddr, Bit32u mask)
{
  for (unsigned i=0; i<BX_SMP_PROCESSORS; i++) {
#if BX_SUPPORT_TRACE_CACHE
    BX_CPU(i)->async_event |= BX_ASYNC_EVENT_STOP_TRACE;
#endif
    BX_CPU(i)->iCache.handleSMC(pAddr, mask);
  }
}

#if BX_SUPPORT_TRACE_CACHE

void BX_CPU_C::serveICacheMiss(bxICacheEntry_c *entry, Bit32u eipBiased, bx_phy_address pAddr)
{
  BX_CPU_THIS_PTR iCache.alloc_trace(entry);

  // Cache miss. We weren't so lucky, but let's be optimistic - try to build 
  // trace from incoming instruction bytes stream !
  entry->pAddr = pAddr;
  entry->traceMask = 0;

  unsigned remainingInPage = BX_CPU_THIS_PTR eipPageWindowSize - eipBiased;
  const Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;
  int ret;

  bxInstruction_c *i = entry->i;

  Bit32u pageOffset = PAGE_OFFSET((Bit32u) pAddr);
  Bit32u traceMask = 0;

  for (unsigned n=0;n<BX_MAX_TRACE_LENGTH;n++)
  {
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
      ret = fetchDecode64(fetchPtr, i, remainingInPage);
    else
#endif
      ret = fetchDecode32(fetchPtr, i, remainingInPage);

    if (ret < 0) {
      // Fetching instruction on segment/page boundary
      if (n > 0) {
         // The trace is already valid, it has several instructions inside,
         // in this case just drop the boundary instruction and stop
         // tracing.
         break;
      }
      // First instruction is boundary fetch, leave the trace cache entry 
      // invalid for now because boundaryFetch() can fault
      entry->pAddr = ~entry->pAddr;
      entry->tlen = 1;
      boundaryFetch(fetchPtr, remainingInPage, i);

      // Add the instruction to trace cache
      entry->pAddr = ~entry->pAddr;
      entry->traceMask = 0x80000000; /* last line in page */
      pageWriteStampTable.markICacheMask(entry->pAddr, entry->traceMask);
      pageWriteStampTable.markICacheMask(BX_CPU_THIS_PTR pAddrPage, 0x1);
      BX_CPU_THIS_PTR iCache.commit_page_split_trace(BX_CPU_THIS_PTR pAddrPage, entry);
      return;
    }

    // add instruction to the trace
    unsigned iLen = i->ilen();
    entry->tlen++;

    BX_INSTR_OPCODE(BX_CPU_ID, fetchPtr, iLen,
       BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, long64_mode());

    traceMask |= 1 <<  (pageOffset >> 7);
    traceMask |= 1 << ((pageOffset + iLen - 1) >> 7);

    // continue to the next instruction
    remainingInPage -= iLen;
    if (ret != 0 /* stop trace indication */ || remainingInPage == 0) break;
    pAddr += iLen;
    pageOffset += iLen;
    fetchPtr += iLen;
    i++;

    // try to find a trace starting from current pAddr and merge
    if (remainingInPage >= 15) // avoid merging with page split trace
      if (mergeTraces(entry, i, pAddr)) break;
  }

//BX_INFO(("commit trace %08x len=%d mask %08x", (Bit32u) entry->pAddr, entry->tlen, pageWriteStampTable.getFineGranularityMapping(entry->pAddr)));

  entry->traceMask |= traceMask;

  pageWriteStampTable.markICacheMask(pAddr, entry->traceMask);

  BX_CPU_THIS_PTR iCache.commit_trace(entry->tlen);
}

bx_bool BX_CPU_C::mergeTraces(bxICacheEntry_c *entry, bxInstruction_c *i, bx_phy_address pAddr)
{
  bxICacheEntry_c *e = BX_CPU_THIS_PTR iCache.get_entry(pAddr, BX_CPU_THIS_PTR fetchModeMask);

  if (e->pAddr == pAddr)
  {
    // determine max amount of instruction to take from another entry
    unsigned max_length = e->tlen;
    if (max_length + entry->tlen > BX_MAX_TRACE_LENGTH)
        max_length = BX_MAX_TRACE_LENGTH - entry->tlen;
    if(max_length == 0) return 0;

    memcpy(i, e->i, sizeof(bxInstruction_c)*max_length);
    entry->tlen += max_length;
    BX_ASSERT(entry->tlen <= BX_MAX_TRACE_LENGTH);

    entry->traceMask |= e->traceMask;

    return 1;
  }

  return 0;
}

#else // BX_SUPPORT_TRACE_CACHE == 0

bx_bool BX_CPU_C::fetchInstruction(bxInstruction_c *iStorage, Bit32u eipBiased)
{
  unsigned remainingInPage = BX_CPU_THIS_PTR eipPageWindowSize - eipBiased;
  const Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;
  int ret;

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    ret = fetchDecode64(fetchPtr, iStorage, remainingInPage);
  else
#endif
    ret = fetchDecode32(fetchPtr, iStorage, remainingInPage);

  if (ret < 0) {
    // handle instrumentation callback inside boundaryFetch
    boundaryFetch(fetchPtr, remainingInPage, iStorage);
    return 0;
  }

#if BX_INSTRUMENTATION
  BX_INSTR_OPCODE(BX_CPU_ID, fetchPtr, iStorage->ilen(),
       BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, long64_mode());
#endif

  return 1;
}

void BX_CPU_C::serveICacheMiss(bxICacheEntry_c *entry, Bit32u eipBiased, bx_phy_address pAddr)
{
  // The entry will be marked valid if fetchdecode will succeed
  if (fetchInstruction(entry->i, eipBiased)) {
    entry->pAddr = pAddr;
    pageWriteStampTable.markICache(pAddr, entry->i->ilen());
  }
  else {
    entry->pAddr = BX_ICACHE_INVALID_PHY_ADDRESS;
  }
}

#endif

void BX_CPU_C::boundaryFetch(const Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *i)
{
  unsigned j, k;
  Bit8u fetchBuffer[32];
  int ret;

  if (remainingInPage >= 15) {
    BX_ERROR(("boundaryFetch #GP(0): too many instruction prefixes"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // Read all leftover bytes in current page up to boundary.
  for (j=0; j<remainingInPage; j++) {
    fetchBuffer[j] = *fetchPtr++;
  }

  // The 2nd chunk of the instruction is on the next page.
  // Set RIP to the 0th byte of the 2nd page, and force a
  // prefetch so direct access of that physical page is possible, and
  // all the associated info is updated.
  RIP += remainingInPage;
  prefetch();

  unsigned fetchBufferLimit = 15;
  if (BX_CPU_THIS_PTR eipPageWindowSize < 15) {
    BX_DEBUG(("boundaryFetch: small window size after prefetch=%d bytes, remainingInPage=%d bytes", BX_CPU_THIS_PTR eipPageWindowSize, remainingInPage));
    fetchBufferLimit = BX_CPU_THIS_PTR eipPageWindowSize;
  }

  // We can fetch straight from the 0th byte, which is eipFetchPtr;
  fetchPtr = BX_CPU_THIS_PTR eipFetchPtr;

  // read leftover bytes in next page
  for (k=0; k<fetchBufferLimit; k++, j++) {
    fetchBuffer[j] = *fetchPtr++;
  }

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    ret = fetchDecode64(fetchBuffer, i, remainingInPage+fetchBufferLimit);
  else
#endif
    ret = fetchDecode32(fetchBuffer, i, remainingInPage+fetchBufferLimit);

  if (ret < 0) {
    BX_INFO(("boundaryFetch #GP(0): failed to complete instruction decoding"));
    exception(BX_GP_EXCEPTION, 0);
  }

  // Restore EIP since we fudged it to start at the 2nd page boundary.
  RIP = BX_CPU_THIS_PTR prev_rip;

  // Since we cross an instruction boundary, note that we need a prefetch()
  // again on the next instruction.  Perhaps we can optimize this to
  // eliminate the extra prefetch() since we do it above, but have to
  // think about repeated instructions, etc.
  // invalidate_prefetch_q();

  BX_INSTR_OPCODE(BX_CPU_ID, fetchBuffer, i->ilen(),
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, long64_mode());
}
