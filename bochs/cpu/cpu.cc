/////////////////////////////////////////////////////////////////////////
// $Id: cpu.cc,v 1.192 2007-12-22 12:43:17 sshwarts Exp $
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
/////////////////////////////////////////////////////////////////////////


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "iodev/iodev.h"

#if BX_EXTERNAL_DEBUGGER
#include "extdb.h"
#endif

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

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RIP EIP
#define RCX ECX
#endif

#if BX_SUPPORT_TRACE_CACHE

bxICacheEntry_c* BX_CPU_C::fetchInstructionTrace(bxInstruction_c *iStorage, bx_address eipBiased)
{
  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrA20Page + eipBiased;
  unsigned iCacheHash = BX_CPU_THIS_PTR iCache.hash(pAddr);
  bxICacheEntry_c *trace = &(BX_CPU_THIS_PTR iCache.entry[iCacheHash]);
  Bit32u pageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);

  InstrICache_Increment(iCacheLookups);
  InstrICache_Stats();

  if ((trace->pAddr == pAddr) &&
      (trace->writeStamp == pageWriteStamp))
  {
     InstrICache_Increment(iCacheTraceLengh[trace->ilen-1]);
     return trace; // We are lucky - trace cache hit !
  }

  // We are not so lucky, but let's be optimistic - try to build trace from
  // incoming instruction bytes stream !
  trace->pAddr = pAddr;
  trace->writeStamp = pageWriteStamp;
  trace->ilen = 0;

  InstrICache_Increment(iCacheMisses);

  unsigned remainingInPage = (unsigned)(BX_CPU_THIS_PTR eipPageWindowSize - eipBiased);
  unsigned maxFetch = 15;
  if (remainingInPage < 15) maxFetch = remainingInPage;
  Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;
  unsigned ret;

  // We could include in trace maximum BX_MAX_TRACE_LEN instructions
  unsigned max_length = BX_MAX_TRACE_LENGTH;
  if ((pageWriteStamp & ICacheWriteStampMask) != ICacheWriteStampStart)
    max_length = 1;  // seems like the entry has SMC ping-pong problem

  bxInstruction_c *i = trace->i;

  for (unsigned len=0;len<max_length;len++,i++)
  {
#if BX_SUPPORT_X86_64
    if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
      ret = fetchDecode64(fetchPtr, i, maxFetch);
    else
#endif
      ret = fetchDecode32(fetchPtr, i, maxFetch);

    if (ret==0) {
      // Fetching instruction on segment/page boundary
      if (len > 0) {
         // The trace is already valid, it has several instructions inside,
         // in this case just drop the boundary instruction and stop
         // tracing.
         break;
      }
      // First instruction is boundary fetch, return iStorage and leave 
      // the trace cache entry invalid (do not cache the instruction)
      trace->writeStamp = ICacheWriteStampInvalid;
      boundaryFetch(fetchPtr, remainingInPage, iStorage);
      return 0;
    }

    // add instruction to the trace ...
    unsigned iLen = i->ilen();
    trace->ilen++;

    // ... and continue to the next instruction
    remainingInPage -= iLen;
    if (remainingInPage < 15) {
      if (remainingInPage == 0) break;
      maxFetch = remainingInPage;
    }
    pAddr += iLen;
    fetchPtr += iLen;

    if (i->getStopTraceAttr()) {
      unsigned b1 = i->b1() & 0x1f0;
      if (b1 == 0x70 || b1 == 0x180) {    // JCC instruction
         // try cross JCC branch merge of traces
         mergeTraces(trace, i+1, pAddr);
      }
      break;
    }

    // try to find a trace starting from current pAddr and merge
    if (mergeTraces(trace, i+1, pAddr)) break;
  }

  return trace;
}

bx_bool BX_CPU_C::mergeTraces(bxICacheEntry_c *trace, bxInstruction_c *i, bx_phy_address pAddr)
{
  bxICacheEntry_c *e = &(BX_CPU_THIS_PTR iCache.entry[BX_CPU_THIS_PTR iCache.hash(pAddr)]);

  if ((e->pAddr == pAddr) && (e->writeStamp == trace->writeStamp))
  {
    // We are lucky - another trace hit !
    InstrICache_Increment(iCacheMergeTraces);

    // determine max amount of instruction to take from another trace
    unsigned max_length = e->ilen;
    if (max_length + trace->ilen > BX_MAX_TRACE_LENGTH)
        max_length = BX_MAX_TRACE_LENGTH - trace->ilen;
    if(max_length == 0) return 0;

    memcpy(i, e->i, sizeof(bxInstruction_c)*max_length);
    trace->ilen += max_length;
    BX_ASSERT(trace->ilen <= BX_MAX_TRACE_LENGTH);

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

#else

bxInstruction_c* BX_CPU_C::fetchInstruction(bxInstruction_c *iStorage, bx_address eipBiased)
{
  unsigned ret;
  bxInstruction_c *i = iStorage;

#if BX_SUPPORT_ICACHE
  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrA20Page + eipBiased;
  unsigned iCacheHash = BX_CPU_THIS_PTR iCache.hash(pAddr);
  bxICacheEntry_c *cache_entry = &(BX_CPU_THIS_PTR iCache.entry[iCacheHash]);
  i = &(cache_entry->i);

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
  unsigned remainingInPage = (unsigned)(BX_CPU_THIS_PTR eipPageWindowSize - eipBiased);
  unsigned maxFetch = 15;
  if (remainingInPage < 15) maxFetch = remainingInPage;
  Bit8u *fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;

#if BX_SUPPORT_ICACHE
  // The entry will be marked valid if fetchdecode will succeed
  cache_entry->writeStamp = ICacheWriteStampInvalid;
  InstrICache_Increment(iCacheMisses);
#endif

#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    ret = fetchDecode64(fetchPtr, i, maxFetch);
  else
#endif
    ret = fetchDecode32(fetchPtr, i, maxFetch);

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

#endif

// The CHECK_MAX_INSTRUCTIONS macro allows cpu_loop to execute a few
// instructions and then return so that the other processors have a chance to
// run.  This is used by bochs internal debugger or when simulating 
// multiple processors.
// 
// If maximum instructions have been executed, return. The zero-count
// means run forever.
#if BX_SUPPORT_SMP || BX_DEBUGGER
  #define CHECK_MAX_INSTRUCTIONS(count) \
    if ((count) > 0) {                  \
      (count)--;                        \
      if ((count) == 0) return;         \
    }
#else
  #define CHECK_MAX_INSTRUCTIONS(count)
#endif

void BX_CPU_C::cpu_loop(Bit32u max_instr_count)
{
  bxInstruction_c iStorage BX_CPP_AlignN(32);

#if BX_DEBUGGER
  BX_CPU_THIS_PTR break_point = 0;
#if BX_MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
#endif

  if (setjmp(BX_CPU_THIS_PTR jmp_buf_env)) 
  { 
    // only from exception function we can get here ...
    BX_INSTR_NEW_INSTRUCTION(BX_CPU_ID);
#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
    if (dbg_instruction_epilog()) return;
#endif
    CHECK_MAX_INSTRUCTIONS(max_instr_count);
#if BX_GDBSTUB
    if (bx_dbg.gdbstub_enabled) return;
#endif
  }

#if BX_DEBUGGER
  // If the exception() routine has encountered a nasty fault scenario,
  // the debugger may request that control is returned to it so that
  // the situation may be examined.
  if (bx_guard.interrupt_requested) {
    BX_ERROR(("CPU_LOOP bx_guard.interrupt_requested=%d", bx_guard.interrupt_requested));
    return;
  }
#endif

  // We get here either by a normal function call, or by a longjmp
  // back from an exception() call.  In either case, commit the
  // new EIP/ESP, and set up other environmental fields.  This code
  // mirrors similar code below, after the interrupt() call.
  BX_CPU_THIS_PTR prev_rip = RIP; // commit new EIP
  BX_CPU_THIS_PTR speculative_rsp = 0;
  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR errorno = 0;

  while (1) {

#if BX_SUPPORT_TRACE_CACHE
    // clear stop trace magic indication that probably was set by branch32/64
    BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
#endif

    // First check on events which occurred for previous instructions
    // (traps) and ones which are asynchronous to the CPU
    // (hardware interrupts).
    if (BX_CPU_THIS_PTR async_event) {
      if (handleAsyncEvent()) {
        // If request to return to caller ASAP.
        return;
      }
    }

    bx_address eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;

    if (eipBiased >= BX_CPU_THIS_PTR eipPageWindowSize) {
      prefetch();
      eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
    }

#if BX_SUPPORT_TRACE_CACHE == 0
    // fetch and decode single instruction
    bxInstruction_c *i = fetchInstruction(&iStorage, eipBiased);
#else
    unsigned n, length = 1;
    bxInstruction_c *i = &iStorage;
    bxICacheEntry_c *trace = fetchInstructionTrace(&iStorage, eipBiased);
    if (trace) {
      i = trace->i; // execute from first instruction in trace
      length = trace->ilen;
    }
    Bit32u currPageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);

    for (n=0; n < length; n++, i++) {
#endif

      BX_CPU_CALL_METHODR(i->ResolveModrm, (i));

      // An instruction will have been fetched using either the normal case,
      // or the boundary fetch (across pages), by this point.
      BX_INSTR_FETCH_DECODE_COMPLETED(BX_CPU_ID, i);

#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
      if (dbg_instruction_prolog()) return;
#endif

#if BX_DISASM
      if (BX_CPU_THIS_PTR trace) {
        // print the instruction that is about to be executed
#if BX_DEBUGGER
        bx_dbg_disassemble_current(BX_CPU_ID, 1); // only one cpu, print time stamp
#else
        debug_disasm_instruction(BX_CPU_THIS_PTR prev_rip);
#endif
      }
#endif

      // decoding instruction compeleted -> continue with execution
      BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
      RIP += i->ilen();
      BX_CPU_CALL_METHOD(i->execute, (i)); // might iterate repeat instruction
      BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
      BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, i);
      BX_TICK1_IF_SINGLE_PROCESSOR();

      // inform instrumentation about new instruction
      BX_INSTR_NEW_INSTRUCTION(BX_CPU_ID);

      // note instructions generating exceptions never reach this point
#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
      if (dbg_instruction_epilog()) return;
#endif

      CHECK_MAX_INSTRUCTIONS(max_instr_count);

#if BX_SUPPORT_TRACE_CACHE
      if (currPageWriteStamp != *(BX_CPU_THIS_PTR currPageWriteStampPtr))
        break; // probably it is self modifying code ...

      if (BX_CPU_THIS_PTR async_event) break;
    }
#endif
  }  // while (1)
}

void BX_CPU_C::repeat(bxInstruction_c *i, BxExecutePtr_t execute)
{
  // non repeated instruction
  if (! i->repUsedL()) {
    BX_CPU_CALL_METHOD(execute, (i));
    return;
  }

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    while(1) {
      if (RCX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX --;
      }
      if (RCX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }
  else
#endif
  if (i->as32L()) {
    while(1) {
      if (ECX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX = ECX - 1;
      }
      if (ECX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }
  else  // 16bit addrsize
  {
    while(1) {
      if (CX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        CX --;
      }
      if (CX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP
}

void BX_CPU_C::repeat_ZFL(bxInstruction_c *i, BxExecutePtr_t execute)
{
  // non repeated instruction
  if (! i->repUsedL()) {
    BX_CPU_CALL_METHOD(execute, (i));
    return;
  }

  unsigned rep = i->repUsedValue();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    while(1) {
      if (RCX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX --;
      }
      if (rep==3 && get_ZF()==0) return;
      if (rep==2 && get_ZF()!=0) return;
      if (RCX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }
  else
#endif
  if (i->as32L()) {
    while(1) {
      if (ECX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX = ECX - 1;
      }
      if (rep==3 && get_ZF()==0) return;
      if (rep==2 && get_ZF()!=0) return;
      if (ECX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }
  else  // 16bit addrsize
  {
    while(1) {
      if (CX != 0) {
        BX_CPU_CALL_METHOD(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        CX --;
      }
      if (rep==3 && get_ZF()==0) return;
      if (rep==2 && get_ZF()!=0) return;
      if (CX == 0) return;

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled
    }
  }

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP
}

unsigned BX_CPU_C::handleAsyncEvent(void)
{
  //
  // This area is where we process special conditions and events.
  //
  if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_SPECIAL) {
    // I made up the bitmask above to mean HALT state.
    // for one processor, pass the time as quickly as possible until
    // an interrupt wakes up the CPU.
#if BX_DEBUGGER
    while (bx_guard.interrupt_requested != 1)
#else
    while (1)
#endif
    {
      if ((BX_CPU_INTR && (BX_CPU_THIS_PTR get_IF() || (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT_IF))) || 
           BX_CPU_THIS_PTR nmi_pending || BX_CPU_THIS_PTR smi_pending)
      {
        // interrupt ends the HALT condition
#if BX_SUPPORT_MONITOR_MWAIT
        if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT)
          BX_CPU_THIS_PTR mem->clear_monitor(BX_CPU_THIS_PTR bx_cpuid);
#endif
        BX_CPU_THIS_PTR debug_trap = 0; // clear traps for after resume
        BX_CPU_THIS_PTR inhibit_mask = 0; // clear inhibits for after resume
        break;
      }
      if ((BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_SPECIAL) == 0) {
        BX_INFO(("handleAsyncEvent: reset detected in HLT state"));
        break;
      }
      // for multiprocessor simulation, even if this CPU is halted we still
      // must give the others a chance to simulate.  If an interrupt has 
      // arrived, then clear the HALT condition; otherwise just return from
      // the CPU loop with stop_reason STOP_CPU_HALTED.
#if BX_SUPPORT_SMP
      if (BX_SMP_PROCESSORS > 1) {
        // HALT condition remains, return so other CPUs have a chance
#if BX_DEBUGGER
        BX_CPU_THIS_PTR stop_reason = STOP_CPU_HALTED;
#endif
        return 1; // Return to caller of cpu_loop.
      }
#endif
      BX_TICK1();
    }
  } else if (bx_pc_system.kill_bochs_request) {
    // setting kill_bochs_request causes the cpu loop to return ASAP.
    return 1; // Return to caller of cpu_loop.
  }

  // Priority 1: Hardware Reset and Machine Checks
  //   RESET
  //   Machine Check
  // (bochs doesn't support these)

  // Priority 2: Trap on Task Switch
  //   T flag in TSS is set
  if (BX_CPU_THIS_PTR debug_trap & 0x00008000) {
    BX_CPU_THIS_PTR dr6 |= BX_CPU_THIS_PTR debug_trap;
    exception(BX_DB_EXCEPTION, 0, 0); // no error, not interrupt
  }

  // Priority 3: External Hardware Interventions
  //   FLUSH
  //   STOPCLK
  //   SMI
  //   INIT
  // (bochs doesn't support these)
  if (BX_CPU_THIS_PTR smi_pending && ! BX_CPU_THIS_PTR smm_mode())
  {
    // clear SMI pending flag and disable NMI when SMM was accepted
    BX_CPU_THIS_PTR smi_pending = 0;
    BX_CPU_THIS_PTR nmi_disable = 1;
    enter_system_management_mode();
  }

  // Priority 4: Traps on Previous Instruction
  //   Breakpoints
  //   Debug Trap Exceptions (TF flag set or data/IO breakpoint)
  if (BX_CPU_THIS_PTR debug_trap &&
       !(BX_CPU_THIS_PTR inhibit_mask & BX_INHIBIT_DEBUG))
  {
    // A trap may be inhibited on this boundary due to an instruction
    // which loaded SS.  If so we clear the inhibit_mask below
    // and don't execute this code until the next boundary.
    // Commit debug events to DR6
    BX_CPU_THIS_PTR dr6 |= BX_CPU_THIS_PTR debug_trap;
    exception(BX_DB_EXCEPTION, 0, 0); // no error, not interrupt
  }

  // Priority 5: External Interrupts
  //   NMI Interrupts
  //   Maskable Hardware Interrupts
  if (BX_CPU_THIS_PTR inhibit_mask & BX_INHIBIT_INTERRUPTS) {
    // Processing external interrupts is inhibited on this
    // boundary because of certain instructions like STI.
    // inhibit_mask is cleared below, in which case we will have
    // an opportunity to check interrupts on the next instruction
    // boundary.
  }
  else if (BX_CPU_THIS_PTR nmi_pending) {
    BX_CPU_THIS_PTR nmi_pending = 0;
    BX_CPU_THIS_PTR nmi_disable = 1;
    BX_CPU_THIS_PTR errorno = 0;
    BX_CPU_THIS_PTR EXT = 1; /* external event */
    BX_INSTR_HWINTERRUPT(BX_CPU_ID, 2, BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
    interrupt(2, 0, 0, 0);
  }
  else if (BX_CPU_INTR && BX_CPU_THIS_PTR get_IF() && BX_DBG_ASYNC_INTR)
  {
    Bit8u vector;

    // NOTE: similar code in ::take_irq()
#if BX_SUPPORT_APIC
    if (BX_CPU_THIS_PTR local_apic.INTR)
      vector = BX_CPU_THIS_PTR local_apic.acknowledge_int();
    else
#endif
      // if no local APIC, always acknowledge the PIC.
      vector = DEV_pic_iac(); // may set INTR with next interrupt
    BX_CPU_THIS_PTR errorno = 0;
    BX_CPU_THIS_PTR EXT = 1; /* external event */
    BX_INSTR_HWINTERRUPT(BX_CPU_ID, vector,
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, RIP);
    interrupt(vector, 0, 0, 0);
    // Set up environment, as would be when this main cpu loop gets
    // invoked.  At the end of normal instructions, we always commmit
    // the new EIP/ESP values.  But here, we call interrupt() much like
    // it was a sofware interrupt instruction, and need to effect the
    // commit here.  This code mirrors similar code above.
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
    BX_CPU_THIS_PTR speculative_rsp = 0;
    BX_CPU_THIS_PTR EXT = 0;
    BX_CPU_THIS_PTR errorno = 0;
  }
  else if (BX_HRQ && BX_DBG_ASYNC_DMA) {
    // NOTE: similar code in ::take_dma()
    // assert Hold Acknowledge (HLDA) and go into a bus hold state
    DEV_dma_raise_hlda();
  }

  // Priority 6: Faults from fetching next instruction
  //   Code breakpoint fault
  //   Code segment limit violation (priority 7 on 486/Pentium)
  //   Code page fault (priority 7 on 486/Pentium)
  // (handled in main decode loop)

  // Priority 7: Faults from decoding next instruction
  //   Instruction length > 15 bytes
  //   Illegal opcode
  //   Coprocessor not available
  // (handled in main decode loop etc)

  // Priority 8: Faults on executing an instruction
  //   Floating point execution
  //   Overflow
  //   Bound error
  //   Invalid TSS
  //   Segment not present
  //   Stack fault
  //   General protection
  //   Data page fault
  //   Alignment check
  // (handled by rest of the code)

  if (BX_CPU_THIS_PTR get_TF())
  {
    // TF is set before execution of next instruction.  Schedule
    // a debug trap (#DB) after execution.  After completion of
    // next instruction, the code above will invoke the trap.
    BX_CPU_THIS_PTR debug_trap |= 0x00004000; // BS flag in DR6
  }

  // Now we can handle things which are synchronous to instruction
  // execution.
  if (BX_CPU_THIS_PTR get_RF()) {
    BX_CPU_THIS_PTR clear_RF();
  }
#if BX_X86_DEBUGGER
  else {
    // only bother comparing if any breakpoints enabled
    if (BX_CPU_THIS_PTR dr7 & 0x000000ff) {
      bx_address iaddr = BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS) + BX_CPU_THIS_PTR prev_rip;
      Bit32u dr6_bits = hwdebug_compare(iaddr, 1, BX_HWDebugInstruction, BX_HWDebugInstruction);
      if (dr6_bits)
      {
        // Add to the list of debug events thus far.
        BX_CPU_THIS_PTR async_event = 1;
        BX_CPU_THIS_PTR debug_trap |= dr6_bits;
        // If debug events are not inhibited on this boundary,
        // fire off a debug fault.  Otherwise handle it on the next
        // boundary. (becomes a trap)
        if (! (BX_CPU_THIS_PTR inhibit_mask & BX_INHIBIT_DEBUG)) {
          // Commit debug events to DR6
          BX_CPU_THIS_PTR dr6 = BX_CPU_THIS_PTR debug_trap;
          exception(BX_DB_EXCEPTION, 0, 0); // no error, not interrupt
        }
      }
    }
  }
#endif

  // We have ignored processing of external interrupts and
  // debug events on this boundary.  Reset the mask so they
  // will be processed on the next boundary.
  BX_CPU_THIS_PTR inhibit_mask = 0;

  if ( !(BX_CPU_INTR ||
         BX_CPU_THIS_PTR debug_trap ||
         BX_HRQ ||
         BX_CPU_THIS_PTR get_TF() 
#if BX_X86_DEBUGGER
         || (BX_CPU_THIS_PTR dr7 & 0xff)
#endif
        ))
    BX_CPU_THIS_PTR async_event = 0;

  return 0; // Continue executing cpu_loop.
}


// boundaries of consideration:
//
//  * physical memory boundary: 1024k (1Megabyte) (increments of...)
//  * A20 boundary:             1024k (1Megabyte)
//  * page boundary:            4k
//  * ROM boundary:             2k (dont care since we are only reading)
//  * segment boundary:         any

void BX_CPU_C::prefetch(void)
{
  bx_address temp_rip = RIP;
  bx_address laddr = BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS) + temp_rip;
  bx_phy_address pAddr;

  // Calculate RIP at the beginning of the page.
  bx_address eipPageOffset0 = RIP - (laddr & 0xfff);
  BX_CPU_THIS_PTR eipPageBias = -eipPageOffset0;
  BX_CPU_THIS_PTR eipPageWindowSize = 4096;

  if (! Is64BitMode()) {
    Bit32u temp_limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;
    if (((Bit32u) temp_rip) > temp_limit) {
      BX_ERROR(("prefetch: EIP [%08x] > CS.limit [%08x]", (Bit32u) temp_rip, temp_limit));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    if (temp_limit + BX_CPU_THIS_PTR eipPageBias < 4096) {
      BX_CPU_THIS_PTR eipPageWindowSize = temp_limit + BX_CPU_THIS_PTR eipPageBias + 1;
    }
  }

  if (BX_CPU_THIS_PTR cr0.get_PG()) {
    // aligned block guaranteed to be all in one page, same A20 address
    pAddr = itranslate_linear(laddr, CPL);
    pAddr = A20ADDR(pAddr);
  }
  else
  {
    pAddr = A20ADDR(laddr);
  }

  BX_CPU_THIS_PTR pAddrA20Page = pAddr & 0xfffff000;
  BX_CPU_THIS_PTR eipFetchPtr =
    BX_CPU_THIS_PTR mem->getHostMemAddr(BX_CPU_THIS, 
          BX_CPU_THIS_PTR pAddrA20Page, BX_READ, CODE_ACCESS);

  // Sanity checks
  if (! BX_CPU_THIS_PTR eipFetchPtr) {
    if (pAddr >= BX_CPU_THIS_PTR mem->len) {
      BX_PANIC(("prefetch: running in bogus memory, pAddr=0x%08x", pAddr));
    }
    else {
      BX_PANIC(("prefetch: getHostMemAddr vetoed direct read, pAddr=0x%08x", pAddr));
    }
  }

#if BX_SUPPORT_ICACHE
  BX_CPU_THIS_PTR currPageWriteStampPtr = pageWriteStampTable.getPageWriteStampPtr(pAddr);
  Bit32u pageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);
  Bit32u fetchModeMask  = BX_CPU_THIS_PTR fetchModeMask;
  if ((pageWriteStamp & ICacheFetchModeMask) != fetchModeMask)
  {
    // The current CPU mode does not match iCache entries for this
    // physical page.
    pageWriteStamp &= ICacheWriteStampMask; // Clear out old fetch mode bits.
    pageWriteStamp |= fetchModeMask;        // Add in new ones.
    pageWriteStampTable.setPageWriteStamp(pAddr, pageWriteStamp);
  }
#endif
}

void BX_CPU_C::boundaryFetch(Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *i)
{
  unsigned j;
  Bit8u fetchBuffer[16]; // Really only need 15
  unsigned ret;

  if (remainingInPage >= 15) {
    BX_INFO(("fetchDecode #GP(0): too many instruction prefixes"));
    exception(BX_GP_EXCEPTION, 0, 0);
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
  if (BX_CPU_THIS_PTR eipPageWindowSize < 15) {
    BX_PANIC(("fetch_decode: small window size after prefetch"));
  }

  // We can fetch straight from the 0th byte, which is eipFetchPtr;
  fetchPtr = BX_CPU_THIS_PTR eipFetchPtr;

  // read leftover bytes in next page
  for (; j<15; j++) {
    fetchBuffer[j] = *fetchPtr++;
  }
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    ret = fetchDecode64(fetchBuffer, i, 15);
  }
  else
#endif
  {
    ret = fetchDecode32(fetchBuffer, i, 15);
  }

  if (ret==0) {
    BX_INFO(("fetchDecode #GP(0): too many instruction prefixes"));
    exception(BX_GP_EXCEPTION, 0, 0);
  }

  // Restore EIP since we fudged it to start at the 2nd page boundary.
  RIP = BX_CPU_THIS_PTR prev_rip;

  // Since we cross an instruction boundary, note that we need a prefetch()
  // again on the next instruction.  Perhaps we can optimize this to
  // eliminate the extra prefetch() since we do it above, but have to
  // think about repeated instructions, etc.
  invalidate_prefetch_q();

  BX_INSTR_OPCODE(BX_CPU_ID, fetchBuffer, i->ilen(),
      BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, Is64BitMode());
}

void BX_CPU_C::deliver_NMI(void)
{
  BX_CPU_THIS_PTR nmi_pending = 1;
  BX_CPU_THIS_PTR async_event = 1;
}

void BX_CPU_C::deliver_SMI(void)
{
  BX_CPU_THIS_PTR smi_pending = 1;
  BX_CPU_THIS_PTR async_event = 1;
}

#if BX_EXTERNAL_DEBUGGER
void BX_CPU_C::ask(int level, const char *prefix, const char *fmt, va_list ap)
{
  char buf1[1024];
  vsprintf (buf1, fmt, ap);
  printf ("%s %s\n", prefix, buf1);
  trap_debugger(1);
}
#endif

#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
bx_bool BX_CPU_C::dbg_instruction_prolog(void)
{
#if BX_DEBUGGER
  if(dbg_check_begin_instr_bpoint()) return 1;
#endif

#if BX_EXTERNAL_DEBUGGER
  bx_external_debugger(BX_CPU_THIS);
#endif

  return 0;
}

bx_bool BX_CPU_C::dbg_instruction_epilog(void)
{
#if BX_DEBUGGER
  if (dbg_check_end_instr_bpoint()) return 1;
#endif

#if BX_GDBSTUB
  if (bx_dbg.gdbstub_enabled) {
    unsigned reason = bx_gdbstub_check(EIP);
    if (reason != GDBSTUB_STOP_NO_REASON) return 1;
  }
#endif

  return 0;
}
#endif // BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB

#if BX_DEBUGGER
extern unsigned dbg_show_mask;

bx_bool BX_CPU_C::dbg_check_begin_instr_bpoint(void)
{ 
  Bit64u tt = bx_pc_system.time_ticks();
  bx_address debug_eip = RIP;
  Bit16u cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;

  BX_CPU_THIS_PTR guard_found.cs  = cs;
  BX_CPU_THIS_PTR guard_found.eip = debug_eip;
  BX_CPU_THIS_PTR guard_found.laddr = 
    BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS) + debug_eip;
  BX_CPU_THIS_PTR guard_found.is_32bit_code = 
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  BX_CPU_THIS_PTR guard_found.is_64bit_code = Is64BitMode();

  // mode switch breakpoint
  // instruction which generate exceptions never reach the end of the
  // loop due to a long jump. Thats why we check at start of instr.
  // Downside is that we show the instruction about to be executed
  // (not the one generating the mode switch).
  if (BX_CPU_THIS_PTR mode_break && 
     (BX_CPU_THIS_PTR dbg_cpu_mode != BX_CPU_THIS_PTR get_cpu_mode()))
  {
    BX_INFO(("[" FMT_LL "d] Caught mode switch breakpoint, switching from '%s' to '%s'",
        bx_pc_system.time_ticks(), cpu_mode_string(BX_CPU_THIS_PTR dbg_cpu_mode),
        cpu_mode_string(BX_CPU_THIS_PTR get_cpu_mode())));
    BX_CPU_THIS_PTR dbg_cpu_mode = BX_CPU_THIS_PTR get_cpu_mode();
    BX_CPU_THIS_PTR stop_reason = STOP_MODE_BREAK_POINT;
    return(1);
  }

  // support for 'show' command in debugger
  if(dbg_show_mask) {
    int rv = bx_dbg_show_symbolic();
    if (rv) return(rv);
  }

  // see if debugger is looking for iaddr breakpoint of any type
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_ALL) {
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_VIR) {
      if ((BX_CPU_THIS_PTR guard_found.icount!=0) ||
          (tt != BX_CPU_THIS_PTR guard_found.time_tick))
      {
        for (unsigned i=0; i<bx_guard.iaddr.num_virtual; i++) {
          if (bx_guard.iaddr.vir[i].enabled &&
             (bx_guard.iaddr.vir[i].cs  == cs) &&
             (bx_guard.iaddr.vir[i].eip == debug_eip))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_VIR;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
	    BX_CPU_THIS_PTR guard_found.time_tick = tt;
            return(1); // on a breakpoint
          }
        }
      }
    }
#endif
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_LIN) {
      if ((BX_CPU_THIS_PTR guard_found.icount!=0) ||
          (tt != BX_CPU_THIS_PTR guard_found.time_tick))
      {
        for (unsigned i=0; i<bx_guard.iaddr.num_linear; i++) {
          if (bx_guard.iaddr.lin[i].enabled && 
             (bx_guard.iaddr.lin[i].addr == BX_CPU_THIS_PTR guard_found.laddr))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_LIN;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
	    BX_CPU_THIS_PTR guard_found.time_tick = tt;
            return(1); // on a breakpoint
          }
        }
      }
    }
#endif
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_PHY) {
      bx_phy_address phy;
      bx_bool valid = dbg_xlate_linear2phy(BX_CPU_THIS_PTR guard_found.laddr, &phy);
      // The "guard_found.icount!=0" condition allows you to step or
      // continue beyond a breakpoint.  Bryce tried removing it once,
      // and once you get to a breakpoint you are stuck there forever.
      // Not pretty.
      if (valid && ((BX_CPU_THIS_PTR guard_found.icount!=0) ||
          (tt != BX_CPU_THIS_PTR guard_found.time_tick)))
      {
        for (unsigned i=0; i<bx_guard.iaddr.num_physical; i++) {
          if (bx_guard.iaddr.phy[i].enabled && (bx_guard.iaddr.phy[i].addr == phy))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_PHY;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
	    BX_CPU_THIS_PTR guard_found.time_tick = tt;
            return(1); // on a breakpoint
          }
        }
      }
    }
#endif
  }

  return(0); // not on a breakpoint
}

bx_bool BX_CPU_C::dbg_check_end_instr_bpoint(void)
{
  BX_CPU_THIS_PTR guard_found.icount++;
  BX_CPU_THIS_PTR guard_found.cs  = 
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  BX_CPU_THIS_PTR guard_found.eip = RIP;
  BX_CPU_THIS_PTR guard_found.laddr = 
    BX_CPU_THIS_PTR get_segment_base(BX_SEG_REG_CS) + RIP;
  BX_CPU_THIS_PTR guard_found.is_32bit_code = 
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  BX_CPU_THIS_PTR guard_found.is_64bit_code = Is64BitMode();

  // Check if we hit read/write or time breakpoint
  if (BX_CPU_THIS_PTR break_point) {
    switch (BX_CPU_THIS_PTR break_point) {
    case BREAK_POINT_TIME:
      BX_INFO(("[" FMT_LL "d] Caught time breakpoint", bx_pc_system.time_ticks()));
      BX_CPU_THIS_PTR stop_reason = STOP_TIME_BREAK_POINT;
      return(1); // on a breakpoint
    case BREAK_POINT_READ:
      BX_INFO(("[" FMT_LL "d] Caught read watch point", bx_pc_system.time_ticks()));
      BX_CPU_THIS_PTR stop_reason = STOP_READ_WATCH_POINT;
      return(1); // on a breakpoint
    case BREAK_POINT_WRITE:
      BX_INFO(("[" FMT_LL "d] Caught write watch point", bx_pc_system.time_ticks()));
      BX_CPU_THIS_PTR stop_reason = STOP_WRITE_WATCH_POINT;
      return(1); // on a breakpoint
    default:
      BX_PANIC(("Weird break point condition"));
    }
  }

#if BX_MAGIC_BREAKPOINT
  if (BX_CPU_THIS_PTR magic_break) {
    BX_INFO(("[" FMT_LL "d] Stopped on MAGIC BREAKPOINT", bx_pc_system.time_ticks()));
    BX_CPU_THIS_PTR stop_reason = STOP_MAGIC_BREAK_POINT;
    return(1); // on a breakpoint
  }
#endif

  // convenient point to see if user typed Ctrl-C
  if (bx_guard.interrupt_requested &&
     (bx_guard.guard_for & BX_DBG_GUARD_CTRL_C))
  {
    BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_CTRL_C;
    return(1); // Ctrl-C pressed
  }

  return(0); // no breakpoint
}

void BX_CPU_C::dbg_take_irq(void)
{
  // NOTE: similar code in ::cpu_loop()

  if (BX_CPU_INTR && BX_CPU_THIS_PTR get_IF()) {
    if (setjmp(BX_CPU_THIS_PTR jmp_buf_env) == 0) {
      // normal return from setjmp setup
      unsigned vector = DEV_pic_iac(); // may set INTR with next interrupt
      BX_CPU_THIS_PTR errorno = 0;
      BX_CPU_THIS_PTR EXT = 1; // external event
      BX_CPU_THIS_PTR async_event = 1; // set in case INTR is triggered
      interrupt(vector, 0, 0, 0);
    }
  }
}

void BX_CPU_C::dbg_force_interrupt(unsigned vector)
{
  // Used to force simulator to take an interrupt, without
  // regard to IF

  if (setjmp(BX_CPU_THIS_PTR jmp_buf_env) == 0) {
    // normal return from setjmp setup
    BX_CPU_THIS_PTR errorno = 0;
    BX_CPU_THIS_PTR EXT = 1; // external event
    BX_CPU_THIS_PTR async_event = 1; // probably don't need this
    interrupt(vector, 0, 0, 0);
  }
}

void BX_CPU_C::dbg_take_dma(void)
{
  // NOTE: similar code in ::cpu_loop()
  if (BX_HRQ) {
    BX_CPU_THIS_PTR async_event = 1; // set in case INTR is triggered
    DEV_dma_raise_hlda();
  }
}

#endif  // #if BX_DEBUGGER
