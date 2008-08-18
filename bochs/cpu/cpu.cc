/////////////////////////////////////////////////////////////////////////
// $Id: cpu.cc,v 1.238 2008-08-18 05:20:22 sshwarts Exp $
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

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RIP EIP
#define RCX ECX
#endif

// ICACHE instrumentation code
#if BX_SUPPORT_ICACHE

#define InstrumentICACHE 0

#if InstrumentICACHE
static unsigned iCacheLookups=0;
static unsigned iCacheMisses=0;

#define InstrICache_StatsMask 0xffffff

#define InstrICache_Stats() {\
  if ((iCacheLookups & InstrICache_StatsMask) == 0) { \
    BX_INFO(("ICACHE lookups: %u, misses: %u, hit rate = %6.2f%% ", \
          iCacheLookups, \
          iCacheMisses,  \
          (iCacheLookups-iCacheMisses) * 100.0 / iCacheLookups)); \
    iCacheLookups = iCacheMisses = 0; \
  } \
}
#define InstrICache_Increment(v) (v)++

#else
#define InstrICache_Stats()
#define InstrICache_Increment(v)
#endif

#endif // BX_SUPPORT_ICACHE

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
#if BX_DEBUGGER
  BX_CPU_THIS_PTR break_point = 0;
  BX_CPU_THIS_PTR magic_break = 0;
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
#endif

  if (setjmp(BX_CPU_THIS_PTR jmp_buf_env)) {
    // only from exception function we can get here ...
    BX_INSTR_NEW_INSTRUCTION(BX_CPU_ID);
    BX_TICK1_IF_SINGLE_PROCESSOR();
#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
    if (dbg_instruction_epilog()) return;
#endif
    CHECK_MAX_INSTRUCTIONS(max_instr_count);
#if BX_GDBSTUB
    if (bx_dbg.gdbstub_enabled) return;
#endif
  }

  // If the exception() routine has encountered a nasty fault scenario,
  // the debugger may request that control is returned to it so that
  // the situation may be examined.
#if BX_DEBUGGER
  if (bx_guard.interrupt_requested) return;
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

    // check on events which occurred for previous instructions (traps)
    // and ones which are asynchronous to the CPU (hardware interrupts)
    if (BX_CPU_THIS_PTR async_event) {
      if (handleAsyncEvent()) {
        // If request to return to caller ASAP.
        return;
      }
    }

no_async_event:

    Bit32u eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;

    if (eipBiased >= BX_CPU_THIS_PTR eipPageWindowSize) {
      prefetch();
      eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
    }

#if BX_SUPPORT_ICACHE
    bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrA20Page + eipBiased;
    bxICacheEntry_c *entry = BX_CPU_THIS_PTR iCache.get_entry(pAddr);
    bxInstruction_c *i = entry->i;

    InstrICache_Increment(iCacheLookups);
    InstrICache_Stats();

    if ((entry->pAddr == pAddr) &&
        (entry->writeStamp == *(BX_CPU_THIS_PTR currPageWriteStampPtr)))
    {
      // iCache hit. An instruction was found in the iCache
    }
    else {
      // iCache miss. No validated instruction with matching fetch parameters
      // is in the iCache.
      InstrICache_Increment(iCacheMisses);
      serveICacheMiss(entry, eipBiased, pAddr);
      i = entry->i;
    }
#else
    bxInstruction_c iStorage, *i = &iStorage;
    fetchInstruction(i, eipBiased);
#endif

#if BX_SUPPORT_TRACE_CACHE
    unsigned length = entry->ilen;

    for(;;i++) {
#endif

#if BX_INSTRUMENTATION
      BX_INSTR_OPCODE(BX_CPU_ID, BX_CPU_THIS_PTR eipFetchPtr + (RIP + BX_CPU_THIS_PTR eipPageBias),
         i->ilen(), BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b, Is64BitMode());
#endif

#if BX_DEBUGGER || BX_EXTERNAL_DEBUGGER || BX_GDBSTUB
      if (dbg_instruction_prolog()) return;
#endif

#if BX_DISASM
      if (BX_CPU_THIS_PTR trace) {
        // print the instruction that is about to be executed
        debug_disasm_instruction(BX_CPU_THIS_PTR prev_rip);
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
      if (BX_CPU_THIS_PTR async_event) {
        // clear stop trace magic indication that probably was set by repeat or branch32/64
        BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
        break;
      }

      if (--length == 0) goto no_async_event;
    }
#endif
  }  // while (1)
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat(bxInstruction_c *i, BxExecutePtr_tR execute)
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
    }
  }

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

#if BX_SUPPORT_TRACE_CACHE
  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
#endif
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat_ZFL(bxInstruction_c *i, BxExecutePtr_tR execute)
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
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

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_TICK1_IF_SINGLE_PROCESSOR();
    }
  }

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

#if BX_SUPPORT_TRACE_CACHE
  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
#endif
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
    while (1)
    {
      if ((BX_CPU_INTR && (BX_CPU_THIS_PTR get_IF() || (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT_IF))) ||
           BX_CPU_THIS_PTR nmi_pending || BX_CPU_THIS_PTR smi_pending)
      {
        // interrupt ends the HALT condition
#if BX_SUPPORT_MONITOR_MWAIT
        if (BX_CPU_THIS_PTR debug_trap & BX_DEBUG_TRAP_MWAIT)
          BX_MEM(0)->clear_monitor(BX_CPU_THIS_PTR bx_cpuid);
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

#if BX_DEBUGGER
      if (bx_guard.interrupt_requested)
        return 1; // Return to caller of cpu_loop.
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
    // the new EIP.  But here, we call interrupt() much like
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
      bx_address iaddr = get_laddr(BX_SEG_REG_CS, BX_CPU_THIS_PTR prev_rip);
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

  if (!((BX_CPU_INTR && BX_CPU_THIS_PTR get_IF()) ||
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
  bx_address laddr = BX_CPU_THIS_PTR get_laddr(BX_SEG_REG_CS, RIP);
  bx_phy_address pAddr;
  unsigned pageOffset = PAGE_OFFSET(laddr);

  // Calculate RIP at the beginning of the page.
  BX_CPU_THIS_PTR eipPageBias = pageOffset - RIP;
  BX_CPU_THIS_PTR eipPageWindowSize = 4096;

#if BX_SUPPORT_X86_64
  if (Is64BitMode()) {
    if (! IsCanonical(RIP)) {
      BX_ERROR(("prefetch: #GP(0): RIP crossed canonical boundary"));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
  }
  else
#endif
  {
    Bit32u temp_limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;
    if (EIP > temp_limit) {
      BX_ERROR(("prefetch: EIP [%08x] > CS.limit [%08x]", EIP, temp_limit));
      exception(BX_GP_EXCEPTION, 0, 0);
    }
    if (temp_limit + BX_CPU_THIS_PTR eipPageBias < 4096) {
      BX_CPU_THIS_PTR eipPageWindowSize = temp_limit + BX_CPU_THIS_PTR eipPageBias + 1;
    }
  }

  bx_address lpf = LPFOf(laddr);
  unsigned TLB_index = BX_TLB_INDEX_OF(lpf, 0);
  bx_TLB_entry *tlbEntry = &BX_CPU_THIS_PTR TLB.entry[TLB_index];
  Bit8u *fetchPtr = 0;

  if ((tlbEntry->lpf == lpf) && !(tlbEntry->accessBits & USER_PL)) {
    pAddr = A20ADDR(tlbEntry->ppf | pageOffset);
#if BX_SupportGuest2HostTLB
    fetchPtr = (Bit8u*) (tlbEntry->hostPageAddr);
#endif
  }  
  else {
    if (BX_CPU_THIS_PTR cr0.get_PG()) {
      pAddr = translate_linear(laddr, CPL, BX_READ, CODE_ACCESS);
      pAddr = A20ADDR(pAddr);
    } 
    else {
      pAddr = A20ADDR(laddr);
    }
  }

  BX_CPU_THIS_PTR pAddrA20Page = LPFOf(pAddr);

  if (fetchPtr) {
    BX_CPU_THIS_PTR eipFetchPtr = fetchPtr;
  }
  else {
    BX_CPU_THIS_PTR eipFetchPtr = BX_MEM(0)->getHostMemAddr(BX_CPU_THIS,
        BX_CPU_THIS_PTR pAddrA20Page, BX_READ, CODE_ACCESS);
  }

  // Sanity checks
  if (! BX_CPU_THIS_PTR eipFetchPtr) {
    if (pAddr >= BX_MEM(0)->get_memory_len()) {
      BX_PANIC(("prefetch: running in bogus memory, pAddr=0x" FMT_PHY_ADDRX, pAddr));
    }
    else {
      BX_PANIC(("prefetch: getHostMemAddr vetoed direct read, pAddr=0x" FMT_PHY_ADDRX, pAddr));
    }
  }

#if BX_SUPPORT_ICACHE
  BX_CPU_THIS_PTR currPageWriteStampPtr = pageWriteStampTable.getPageWriteStampPtr(pAddr);
  Bit32u pageWriteStamp = *(BX_CPU_THIS_PTR currPageWriteStampPtr);
  pageWriteStamp &= ~ICacheWriteStampFetchModeMask; // Clear out old fetch mode bits
  pageWriteStamp |=  BX_CPU_THIS_PTR fetchModeMask; // And add new ones
  pageWriteStampTable.setPageWriteStamp(pAddr, pageWriteStamp);
#endif
}

void BX_CPU_C::boundaryFetch(const Bit8u *fetchPtr, unsigned remainingInPage, bxInstruction_c *i)
{
  unsigned j;
  Bit8u fetchBuffer[16]; // Really only need 15
  unsigned ret;

  if (remainingInPage >= 15) {
    BX_ERROR(("boundaryFetch #GP(0): too many instruction prefixes"));
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

  unsigned fetchBufferLimit = 15;
  if (BX_CPU_THIS_PTR eipPageWindowSize < 15) {
    BX_DEBUG(("boundaryFetch: small window size after prefetch - %d bytes", BX_CPU_THIS_PTR eipPageWindowSize));
    fetchBufferLimit = BX_CPU_THIS_PTR eipPageWindowSize;
  }

  // We can fetch straight from the 0th byte, which is eipFetchPtr;
  fetchPtr = BX_CPU_THIS_PTR eipFetchPtr;

  // read leftover bytes in next page
  for (; j<fetchBufferLimit; j++) {
    fetchBuffer[j] = *fetchPtr++;
  }
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64)
    ret = fetchDecode64(fetchBuffer, i, fetchBufferLimit);
  else
#endif
    ret = fetchDecode32(fetchBuffer, i, fetchBufferLimit);

  if (ret==0) {
    BX_INFO(("boundaryFetch #GP(0): failed to complete instruction decoding"));
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
  trap_debugger(1, BX_CPU_THIS);
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
  BX_CPU_THIS_PTR guard_found.laddr = BX_CPU_THIS_PTR get_laddr(BX_SEG_REG_CS, debug_eip);
  BX_CPU_THIS_PTR guard_found.is_32bit_code =
    BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
  BX_CPU_THIS_PTR guard_found.is_64bit_code = Is64BitMode();

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
        for (unsigned n=0; n<bx_guard.iaddr.num_virtual; n++) {
          if (bx_guard.iaddr.vir[n].enabled &&
             (bx_guard.iaddr.vir[n].cs  == cs) &&
             (bx_guard.iaddr.vir[n].eip == debug_eip))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_VIR;
            BX_CPU_THIS_PTR guard_found.iaddr_index = n;
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
        for (unsigned n=0; n<bx_guard.iaddr.num_linear; n++) {
          if (bx_guard.iaddr.lin[n].enabled &&
             (bx_guard.iaddr.lin[n].addr == BX_CPU_THIS_PTR guard_found.laddr))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_LIN;
            BX_CPU_THIS_PTR guard_found.iaddr_index = n;
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
        for (unsigned n=0; n<bx_guard.iaddr.num_physical; n++) {
          if (bx_guard.iaddr.phy[n].enabled && (bx_guard.iaddr.phy[n].addr == phy))
          {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_PHY;
            BX_CPU_THIS_PTR guard_found.iaddr_index = n;
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
  BX_CPU_THIS_PTR guard_found.laddr = BX_CPU_THIS_PTR get_laddr(BX_SEG_REG_CS, RIP);
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

  if (BX_CPU_THIS_PTR magic_break) {
    BX_INFO(("[" FMT_LL "d] Stopped on MAGIC BREAKPOINT", bx_pc_system.time_ticks()));
    BX_CPU_THIS_PTR stop_reason = STOP_MAGIC_BREAK_POINT;
    return(1); // on a breakpoint
  }

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
