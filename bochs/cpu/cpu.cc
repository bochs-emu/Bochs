/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2018  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#define LOG_THIS BX_CPU_THIS_PTR

#include "memory/memory-bochs.h"
#include "pc_system.h"
#include "cpustats.h"

#include "icache.h"

#include "bx_debug/debug.h"

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS

#define BX_SYNC_TIME_IF_SINGLE_PROCESSOR(allowed_delta) {                               \
  if (BX_SMP_PROCESSORS == 1) {                                                         \
    Bit32u delta = (Bit32u)(BX_CPU_THIS_PTR icount - BX_CPU_THIS_PTR icount_last_sync); \
    if (delta >= allowed_delta) {                                                       \
      BX_CPU_THIS_PTR sync_icount();                                                    \
      BX_TICKN(delta);                                                                  \
    }                                                                                   \
  }                                                                                     \
}

#else

#define BX_SYNC_TIME_IF_SINGLE_PROCESSOR(allowed_delta) \
  if (BX_SMP_PROCESSORS == 1) BX_TICK1()

#endif

jmp_buf BX_CPU_C::jmp_buf_env;

#if BX_DEBUGGER
void BX_CPU_C::cpu_loop_debugger(void)
{
  BX_CPU_THIS_PTR break_point = 0;
  BX_CPU_THIS_PTR magic_break = 0;
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;

  // Remember icount on entry: code breakpoint must not be reported on the
  // instruction we are resuming from before at least one instruction was
  // executed (e.g. continue from a breakpoint).
  BX_CPU_THIS_PTR dbg_code_bp_hit = false;
  BX_CPU_THIS_PTR dbg_loop_icount = get_icount();

  if (setjmp(BX_CPU_THIS_PTR jmp_buf_env)) {
    // can get here only from exception function or VMEXIT
    BX_CPU_THIS_PTR icount++;
    if (BX_SMP_PROCESSORS == 1) BX_TICK1();
    if (dbg_instruction_epilog()) return;
  }

  // If the exception() routine has encountered a nasty fault scenario,
  // the debugger may request that control is returned to it so that
  // the situation may be examined.
  if (bx_guard.interrupt_requested) return;

  // We get here either by a normal function call, or by a longjmp
  // back from an exception() call.  In either case, commit the
  // new EIP/ESP, and set up other environmental fields.  This code
  // mirrors similar code below, after the interrupt() call.
  BX_CPU_THIS_PTR prev_rip = RIP; // commit new EIP
  BX_CPU_THIS_PTR speculative_rsp = false;

  while (1) {

    // check on events which occurred for previous instructions (traps)
    // and ones which are asynchronous to the CPU (hardware interrupts)
    Bit32u handle_event = BX_CPU_THIS_PTR async_event & ~BX_ASYNC_EVENT_STOP_TRACE;
    if (handle_event) {
      if (handleAsyncEvent()) {
        // If request to return to caller ASAP.
        return;
      }

      // Event delivery (interrupt, SMI, VMEXIT etc.) could redirect RIP
      // without leaving current fetch window, in such case prefetch() won't
      // be called for the new RIP. Check code breakpoints here, the check
      // after page change is done by prefetch() through dbg_code_bp_after_fetch().
      if (BX_CPU_THIS_PTR dbg_code_bp_on_page && get_icount() != BX_CPU_THIS_PTR dbg_loop_icount) {
        bx_address eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
        if (eipBiased < BX_CPU_THIS_PTR eipPageWindowSize) {
          if (dbg_check_code_bpoints()) return;
        }
      }
    }

    // stop tracing after every instruction to handle in internal debugger
    BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;

    bxTraceCacheEntry_c *entry = getTraceCacheEntry();
    if (dbg_code_bp_after_fetch()) return;
    bxInstruction_c *i = entry->i;
    bxInstruction_c *last = i + (entry->tlen);

    for(;;) {
      if (BX_CPU_THIS_PTR trace)
        debug_disasm_instruction(BX_CPU_THIS_PTR prev_rip);

      // want to allow changing of the instruction inside instrumentation callback
      BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
      RIP += i->ilen();
      BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS == 0
      BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
      BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, i);
      BX_CPU_THIS_PTR icount++;
#endif
      if (BX_SMP_PROCESSORS == 1) BX_TICK1();

      // note instructions generating exceptions never reach this point
      if (dbg_instruction_epilog()) return;

      if (BX_CPU_THIS_PTR async_event & ~BX_ASYNC_EVENT_STOP_TRACE) break;

      if (++i == last) {
        entry = getTraceCacheEntry();
        if (dbg_code_bp_after_fetch()) return;
        i = entry->i;
        last = i + (entry->tlen);
      }
    }
  }  // while (1)
}
#endif // BX_DEBUGGER

void BX_CPU_C::cpu_loop(void)
{
#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  volatile Bit8u stack_anchor = 0;

  BX_CPU_THIS_PTR cpuloop_stack_anchor = &stack_anchor;
#endif

#if BX_DEBUGGER
  BX_ASSERT(! bx_dbg.debugger_active);
#endif

  if (setjmp(BX_CPU_THIS_PTR jmp_buf_env)) {
    // can get here only from exception function or VMEXIT
    BX_CPU_THIS_PTR icount++;
    BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);
#if BX_GDBSTUB
    if (gdbstub_instruction_epilog() || bx_dbg.gdbstub_enabled) return;
#endif
  }

  // We get here either by a normal function call, or by a longjmp
  // back from an exception() call.  In either case, commit the
  // new EIP/ESP, and set up other environmental fields.  This code
  // mirrors similar code below, after the interrupt() call.
  BX_CPU_THIS_PTR prev_rip = RIP; // commit new EIP
  BX_CPU_THIS_PTR speculative_rsp = false;

#if BX_SUPPORT_CET && BX_SUPPORT_VMX
  if (BX_CPU_THIS_PTR in_vmx_guest) {
    VMCS_CACHE *vm = &BX_CPU_THIS_PTR vmcs;
    if (vm->shadow_stack_prematurely_busy)
      BX_PANIC(("Shadow stack prematurely busy is left set !"));
    vm->shadow_stack_prematurely_busy = false; // for safety
  }
#endif

  while (1) {

    // check on events which occurred for previous instructions (traps)
    // and ones which are asynchronous to the CPU (hardware interrupts)
    if (BX_CPU_THIS_PTR async_event) {
      if (handleAsyncEvent()) {
        // If request to return to caller ASAP.
        return;
      }
    }

    bxTraceCacheEntry_c *entry = getTraceCacheEntry();
    bxInstruction_c *i = entry->i;

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
    for(;;) {
      // want to allow changing of the instruction inside instrumentation callback
      BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
      RIP += i->ilen();
      // when handlers chaining is enabled this single call will execute entire trace
      BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction

      BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);

      if (BX_CPU_THIS_PTR async_event) break;

      i = getTraceCacheEntry()->i;
    }
#else // BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS == 0

    bxInstruction_c *last = i + (entry->tlen);

    for(;;) {

      // want to allow changing of the instruction inside instrumentation callback
      BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
      RIP += i->ilen();
      BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
      BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
      BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, i);
      BX_CPU_THIS_PTR icount++;

      BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);

      // note instructions generating exceptions never reach this point
#if BX_GDBSTUB
      if (gdbstub_instruction_epilog()) return;
#endif

      if (BX_CPU_THIS_PTR async_event) break;

      if (++i == last) {
        entry = getTraceCacheEntry();
        i = entry->i;
        last = i + (entry->tlen);
      }
    }
#endif

    // clear stop trace magic indication that probably was set by repeat or branch32/64
    BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;

  }  // while (1)
}

#if BX_SUPPORT_SMP

void BX_CPU_C::cpu_run_trace(void)
{
  // check on events which occurred for previous instructions (traps)
  // and ones which are asynchronous to the CPU (hardware interrupts)
  if (BX_CPU_THIS_PTR async_event) {
    if (handleAsyncEvent()) {
      // If request to return to caller ASAP.
      return;
    }
  }

  bxTraceCacheEntry_c *entry = getTraceCacheEntry();
  bxInstruction_c *i = entry->i;

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
  // want to allow changing of the instruction inside instrumentation callback
  BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
  RIP += i->ilen();
  // when handlers chaining is enabled this single call will execute entire trace
  BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction

  if (BX_CPU_THIS_PTR async_event) {
    // clear stop trace magic indication that probably was set by repeat or branch32/64
    BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
  }
#else
  bxInstruction_c *last = i + (entry->tlen);

  for(;;) {
    // want to allow changing of the instruction inside instrumentation callback
    BX_INSTR_BEFORE_EXECUTION(BX_CPU_ID, i);
    RIP += i->ilen();
    BX_CPU_CALL_METHOD(i->execute1, (i)); // might iterate repeat instruction
    BX_CPU_THIS_PTR prev_rip = RIP; // commit new RIP
    BX_INSTR_AFTER_EXECUTION(BX_CPU_ID, i);
    BX_CPU_THIS_PTR icount++;

    if (BX_CPU_THIS_PTR async_event) {
      // clear stop trace magic indication that probably was set by repeat or branch32/64
      BX_CPU_THIS_PTR async_event &= ~BX_ASYNC_EVENT_STOP_TRACE;
      break;
    }

    if (++i == last) break;
  }
#endif // BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS
}

#endif

#include "decoder/ia_opcodes.h"

#if BX_SUPPORT_CET
// called when the CPU is waiting for ENDBRANCH, i is the next instruction to execute
void BX_CPP_AttrRegparmN(1) BX_CPU_C::CheckEndbranch(bxInstruction_c *i)
{
  if (i->getIaOpcode() != (long64_mode() ? BX_IA_ENDBRANCH64 : BX_IA_ENDBRANCH32) && i->getIaOpcode() != BX_IA_INT3) {
    if (LegacyEndbranchTreatment(CPL)) {
      BX_ERROR(("#CP(ENDBRANCH): Endbranch is expected for CPL=%d", CPL));
      exception(BX_CP_EXCEPTION, BX_CP_ENDBRANCH);
    }
  }
}
#endif

bxTraceCacheEntry_c* BX_CPU_C::getTraceCacheEntry(void)
{
  bx_address eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;

  if (eipBiased >= BX_CPU_THIS_PTR eipPageWindowSize) {
    prefetch();
    eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
  }

  INC_ICACHE_STAT(traceCacheLookups);

  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrFetchPage + eipBiased;
  bxTraceCacheEntry_c *entry = BX_CPU_THIS_PTR traceCache->find_entry(pAddr, BX_CPU_THIS_PTR fetchModeMask);

  if (entry == NULL || entry->i->ilen() == 0)
  {
    // Trace cache miss. No valid trace with matching fetch parameters is in the trace cache.
    INC_ICACHE_STAT(traceCacheMisses);
    entry = serveTraceCacheMiss((Bit32u) eipBiased, pAddr);
  }

#if BX_SUPPORT_CET
  if (WaitingForEndbranch(CPL))
    CheckEndbranch(entry->i);
#endif

  BX_ASSERT(entry->i->ilen() != 0);

  return entry;
}

#if BX_SUPPORT_HANDLERS_CHAINING_SPEEDUPS && BX_ENABLE_TRACE_LINKING

// The function is called after taken branch instructions and tries to link the branch to the next trace
void BX_CPP_AttrRegparmN(1) BX_CPU_C::linkTrace(bxInstruction_c *i)
{
  volatile Bit8u stack_anchor = 0;

  if (bx_dbg.debugger_active)
    return;

#if BX_SUPPORT_SMP
  if (BX_SMP_PROCESSORS > 1)
    return;
#endif

#define BX_HANDLERS_CHAINING_MAX_LINK_DEPTH 1000

  // do not allow extreme trace link depth / avoid host stack overflow
  // (could happen with badly compiled instruction handlers)
  static Bit32u linkDepth = 0;

  if (BX_CPU_THIS_PTR async_event || ++linkDepth > BX_HANDLERS_CHAINING_MAX_LINK_DEPTH) {
    linkDepth = 0;
    return;
  }

#define BX_HANDLERS_CHAINING_MAX_STACK_DEPTH 0x10000

  size_t stack_depth = BX_CPU_THIS_PTR cpuloop_stack_anchor - &stack_anchor;
  if (stack_depth > BX_HANDLERS_CHAINING_MAX_STACK_DEPTH) {
    linkDepth = 0;
    return;
  }

  Bit32u delta = (Bit32u) (BX_CPU_THIS_PTR icount - BX_CPU_THIS_PTR icount_last_sync);
  if(delta >= bx_pc_system.getNumCpuTicksLeftNextEvent()) {
    linkDepth = 0;
    return;
  }

  BX_SYNC_TIME_IF_SINGLE_PROCESSOR(0);

  bxInstruction_c *next = i->getNextTrace(BX_CPU_THIS_PTR traceCache->traceLinkTimeStamp);
  if (next) {
    BX_EXECUTE_INSTRUCTION(next);
    return;
  }

  bx_address eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
  if (eipBiased >= BX_CPU_THIS_PTR eipPageWindowSize) {
    prefetch();
    eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
  }

  INC_ICACHE_STAT(traceCacheLookups);

  bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrFetchPage + eipBiased;
  bxTraceCacheEntry_c *entry = BX_CPU_THIS_PTR traceCache->find_entry(pAddr, BX_CPU_THIS_PTR fetchModeMask);

  if (entry != NULL) // link traces - handle only hit cases
  {
    i->setNextTrace(entry->i, BX_CPU_THIS_PTR traceCache->traceLinkTimeStamp);
    i = entry->i;
    BX_EXECUTE_INSTRUCTION(i);
  }
}

#endif

#define BX_REPEAT_TIME_UPDATE_INTERVAL (BX_MAX_TRACE_LENGTH-1)

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat(bxInstruction_c *i, BxRepIterationPtr_tR execute)
{
  // non repeated instruction
  if (! i->repUsedL()) {
    BX_CPU_CALL_REP_ITERATION(execute, (i));
    return;
  }

  BX_ASSERT(! bx_dbg.debugger_active || BX_CPU_THIS_PTR async_event);

  BX_CPU_THIS_PTR clear_RF();

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    while(1) {
      if (RCX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX --;
      }
      if (RCX == 0) return;

      if (BX_CPU_THIS_PTR async_event)
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

      BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }
  else
#endif
  if (i->as32L()) {
    while(1) {
      if (ECX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX = ECX - 1;
      }
      if (ECX == 0) return;

      if (BX_CPU_THIS_PTR async_event)
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

      BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }
  else  // 16bit addrsize
  {
    while(1) {
      if (CX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        CX --;
      }
      if (CX == 0) return;

      if (BX_CPU_THIS_PTR async_event)
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

      BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }

  BX_CPU_THIS_PTR assert_RF();

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat_ZF(bxInstruction_c *i, BxRepIterationPtr_tR execute)
{
  unsigned rep = i->lockRepUsedValue();

  // non repeated instruction
  if (rep < 2) {
    BX_CPU_CALL_REP_ITERATION(execute, (i));
    return;
  }

  BX_CPU_THIS_PTR clear_RF();

  if (rep == 3) { /* repeat prefix 0xF3 */
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (! get_ZF() || RCX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else
#endif
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (! get_ZF() || ECX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else  // 16bit addrsize
    {
      while(1) {
        if (CX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          CX --;
        }
        if (! get_ZF() || CX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
  }
  else {          /* repeat prefix 0xF2 */
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (get_ZF() || RCX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else
#endif
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (get_ZF() || ECX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else  // 16bit addrsize
    {
      while(1) {
        if (CX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          CX --;
        }
        if (get_ZF() || CX == 0) return;

        if (BX_CPU_THIS_PTR async_event)
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
  }

  BX_CPU_THIS_PTR assert_RF();

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
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
  bx_address laddr;
  unsigned pageOffset;

  INC_ICACHE_STAT(iCachePrefetch);

#if BX_SUPPORT_X86_64
  if (long64_mode()) {
    if (! IsCanonicalAccess(RIP, BX_EXECUTE, USER_PL)) {
      BX_ERROR(("prefetch: #GP(0): RIP crossed canonical boundary"));
      exception(BX_GP_EXCEPTION, 0);
    }

    // linear address is equal to RIP in 64-bit long mode
    pageOffset = PAGE_OFFSET(EIP);
    laddr = RIP;

    // Calculate RIP at the beginning of the page.
    BX_CPU_THIS_PTR eipPageBias = pageOffset - RIP;
    BX_CPU_THIS_PTR eipPageWindowSize = 4096;
  }
  else
#endif
  {

#if BX_CPU_LEVEL >= 4
    if (USER_PL && BX_CPU_THIS_PTR get_VIP() && BX_CPU_THIS_PTR get_VIF()) {
      if (BX_CPU_THIS_PTR cr4.get_PVI() || (v8086_mode() && BX_CPU_THIS_PTR cr4.get_VME())) {
        BX_ERROR(("prefetch: inconsistent VME state"));
        exception(BX_GP_EXCEPTION, 0);
      }
    }
#endif

    BX_CLEAR_64BIT_HIGH(BX_64BIT_REG_RIP); /* avoid 32-bit EIP wrap */
    laddr = get_laddr32(BX_SEG_REG_CS, EIP);
    pageOffset = PAGE_OFFSET(laddr);

    // Calculate RIP at the beginning of the page.
    BX_CPU_THIS_PTR eipPageBias = (bx_address) pageOffset - EIP;

    Bit32u limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;
    if (EIP > limit) {
      BX_ERROR(("prefetch: EIP [%08x] > CS.limit [%08x]", EIP, limit));
      exception(BX_GP_EXCEPTION, 0);
    }

    BX_CPU_THIS_PTR eipPageWindowSize = 4096;
    if (limit + BX_CPU_THIS_PTR eipPageBias < 4096) {
      BX_CPU_THIS_PTR eipPageWindowSize = (Bit32u)(limit + BX_CPU_THIS_PTR eipPageBias + 1);
    }
  }

#if BX_X86_DEBUGGER
  if (hwbreakpoint_check(laddr, BX_HWDebugInstruction, BX_HWDebugInstruction)) {
    signal_event(BX_EVENT_CODE_BREAKPOINT_ASSIST);
    if (! interrupts_inhibited(BX_INHIBIT_DEBUG)) {
       // The next instruction could already hit a code breakpoint but
       // async_event won't take effect immediatelly.
       // Check if the next executing instruction hits code breakpoint

       // check only if not fetching page cross instruction
       // this check is 32-bit wrap safe as well
       if (EIP == (Bit32u) BX_CPU_THIS_PTR prev_rip) {
         Bit32u dr6_bits = code_breakpoint_match(laddr);
         if (dr6_bits & BX_DEBUG_TRAP_HIT) {
           BX_ERROR(("#DB: x86 code breakpoint caught"));
           BX_CPU_THIS_PTR debug_trap |= dr6_bits;
           exception(BX_DB_EXCEPTION, 0);
         }
       }
    }
  }
  else {
    clear_event(BX_EVENT_CODE_BREAKPOINT_ASSIST);
  }
#endif

  BX_CPU_THIS_PTR clear_RF();

  bx_address lpf = LPFOf(laddr);
  bx_TLB_entry *tlbEntry = BX_ITLB_ENTRY_OF(laddr);
  Bit8u *fetchPtr = 0;

  if ((tlbEntry->lpf == lpf) && (tlbEntry->accessBits & (1 << unsigned(USER_PL))) != 0) {
    BX_CPU_THIS_PTR pAddrFetchPage = tlbEntry->ppf;
    fetchPtr = (Bit8u*) tlbEntry->hostPageAddr;
  }
  else {
    bx_phy_address pAddr = translate_linear(tlbEntry, laddr, USER_PL, BX_EXECUTE);
    BX_CPU_THIS_PTR pAddrFetchPage = PPFOf(pAddr);
  }

  if (fetchPtr) {
    BX_CPU_THIS_PTR eipFetchPtr = fetchPtr;
  }
  else {
    BX_CPU_THIS_PTR eipFetchPtr = (const Bit8u*) getHostMemAddr(BX_CPU_THIS_PTR pAddrFetchPage, BX_EXECUTE);

    // Sanity checks
    if (! BX_CPU_THIS_PTR eipFetchPtr) {
      bx_phy_address pAddr = BX_CPU_THIS_PTR pAddrFetchPage + pageOffset;
      if (pAddr >= BX_MEM(0)->get_memory_len()) {
        BX_PANIC(("prefetch: running in bogus memory, pAddr=0x" FMT_PHY_ADDRX, pAddr));
      }
      else {
        BX_PANIC(("prefetch: getHostMemAddr vetoed direct read, pAddr=0x" FMT_PHY_ADDRX, pAddr));
      }
    }
  }

#if BX_DEBUGGER
  // New fetch window is established: recompute debugger code breakpoints
  // page filter for it. Similar to x86 HW code breakpoints the instruction
  // at the beginning of the new window will be executed before the next
  // dbg_instruction_epilog() so check it right here. The check is done only
  // on instruction boundary (not for page split instruction fetch) and only
  // if at least one instruction was executed since cpu_loop_debugger() entry.
  if (bx_dbg.debugger_active) {
    BX_CPU_THIS_PTR dbg_fetch_lpf = lpf;
    dbg_update_code_bp_page();
    if (BX_CPU_THIS_PTR dbg_code_bp_on_page) {
      if (RIP == BX_CPU_THIS_PTR prev_rip && get_icount() != BX_CPU_THIS_PTR dbg_loop_icount) {
        if (dbg_check_code_bpoints())
          BX_CPU_THIS_PTR dbg_code_bp_hit = true;
      }
    }
  }
#endif
}

#if BX_DEBUGGER
bool BX_CPU_C::dbg_instruction_epilog(void)
{
  // support for 'show' command in debugger
  extern unsigned dbg_show_mask;
  if(dbg_show_mask) {
    bx_dbg_show_symbolic(BX_CPU_ID);
  }

  //
  // Take care of break point conditions generated during instruction execution
  //

  // Check if we hit read/write or time breakpoint
  if (BX_CPU_THIS_PTR break_point) {
    Bit64u tt = bx_pc_system.time_ticks();
    dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
    switch (BX_CPU_THIS_PTR break_point) {
    case BREAK_POINT_TIME:
      BX_INFO(("[" FMT_LL "d] Caught time breakpoint", tt));
      BX_CPU_THIS_PTR stop_reason = STOP_TIME_BREAK_POINT;
      return true; // on a breakpoint
    case BREAK_POINT_READ:
      BX_INFO(("[" FMT_LL "d] Caught read watch point", tt));
      BX_CPU_THIS_PTR stop_reason = STOP_READ_WATCH_POINT;
      return true; // on a breakpoint
    case BREAK_POINT_WRITE:
      BX_INFO(("[" FMT_LL "d] Caught write watch point", tt));
      BX_CPU_THIS_PTR stop_reason = STOP_WRITE_WATCH_POINT;
      return true; // on a breakpoint
    default:
      BX_PANIC(("Weird break point condition"));
    }
  }

  if (BX_CPU_THIS_PTR magic_break) {
    BX_INFO(("[" FMT_LL "d] Stopped on MAGIC BREAKPOINT", bx_pc_system.time_ticks()));
    BX_CPU_THIS_PTR stop_reason = STOP_MAGIC_BREAK_POINT;
    dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
    return true; // on a breakpoint
  }

  // convenient point to see if user requested debug break or typed Ctrl-C
  if (bx_guard.interrupt_requested) {
    dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
    return true;
  }

  // Just committed an instruction, before fetching a new one
  // see if debugger is looking for iaddr breakpoint of any type
  if (bx_guard.guard_for) {

    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_ALL) {
      bx_address eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
      if (eipBiased < BX_CPU_THIS_PTR eipPageWindowSize) {
        // RIP is still inside current fetch window so the page filter
        // computed by prefetch() is valid for it. Skip the breakpoints
        // lookup completely if no code breakpoint is on this page.
        if (BX_CPU_THIS_PTR dbg_code_bp_on_page) {
          if (dbg_check_code_bpoints()) return true; // on a breakpoint
        }
      }
      else {
        // RIP left current fetch window, normally the next prefetch() will
        // take care of the code breakpoint check for the new RIP. But if we
        // are about to leave cpu_loop_debugger() (icount guard reached or
        // async event could request return) there will be no such prefetch()
        // before the next cpu_loop_debugger() entry, so do the check now.
        if ((BX_CPU_THIS_PTR async_event & ~BX_ASYNC_EVENT_STOP_TRACE) ||
           ((bx_guard.guard_for & BX_DBG_GUARD_ICOUNT) && get_icount() >= BX_CPU_THIS_PTR guard_found.icount_max))
        {
          if (dbg_check_code_bpoints()) return true; // on a breakpoint
        }
      }
    }

    // see if debugger requesting icount guard
    if (bx_guard.guard_for & BX_DBG_GUARD_ICOUNT) {
      if (get_icount() >= BX_CPU_THIS_PTR guard_found.icount_max) {
        dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
        return true;
      }
    }
  }

  return false;
}

// Recompute debugger code breakpoints page filter for the current fetch
// window. Called by prefetch() and when debugger breakpoints are changed.
// The check is conservative: virtual breakpoint CS selector is ignored and
// only EIP is compared against the window, so CS reload which doesn't
// invalidate the fetch window could never hide a breakpoint.
void BX_CPU_C::dbg_update_code_bp_page(void)
{
  BX_CPU_THIS_PTR dbg_code_bp_on_page = false;

  if (! (bx_guard.guard_for & BX_DBG_GUARD_IADDR_ALL)) return;

#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_VIR) {
    for (unsigned n=0; n<bx_guard.iaddr.num_virtual; n++) {
      if (bx_guard.iaddr.vir[n].enabled) {
        bx_address eipBiased = bx_guard.iaddr.vir[n].eip + BX_CPU_THIS_PTR eipPageBias;
        if (eipBiased < BX_CPU_THIS_PTR eipPageWindowSize) {
          BX_CPU_THIS_PTR dbg_code_bp_on_page = true;
          return;
        }
      }
    }
  }
#endif
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_LIN) {
    for (unsigned n=0; n<bx_guard.iaddr.num_linear; n++) {
      if (bx_guard.iaddr.lin[n].enabled && LPFOf(bx_guard.iaddr.lin[n].addr) == BX_CPU_THIS_PTR dbg_fetch_lpf) {
        BX_CPU_THIS_PTR dbg_code_bp_on_page = true;
        return;
      }
    }
  }
#endif
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_PHY) {
    for (unsigned n=0; n<bx_guard.iaddr.num_physical; n++) {
      if (bx_guard.iaddr.phy[n].enabled && PPFOf(bx_guard.iaddr.phy[n].addr) == BX_CPU_THIS_PTR pAddrFetchPage) {
        BX_CPU_THIS_PTR dbg_code_bp_on_page = true;
        return;
      }
    }
  }
#endif
}

// Exact code breakpoints match for the instruction at current CS:RIP.
// Physical address of the instruction is taken from the current fetch window
// when RIP is inside it (no page walk), otherwise translated by debugger.
bool BX_CPU_C::dbg_check_code_bpoints(void)
{
  bx_address debug_eip = RIP;
  Bit16u cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
  bx_address laddr = get_laddr(BX_SEG_REG_CS, debug_eip);

#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_VIR) {
    for (unsigned n=0; n<bx_guard.iaddr.num_virtual; n++) {
      if (bx_guard.iaddr.vir[n].enabled &&
         (bx_guard.iaddr.vir[n].cs  == cs) &&
         (bx_guard.iaddr.vir[n].eip == debug_eip))
      {
        if (! bx_guard.iaddr.vir[n].condition || bx_dbg_eval_condition(bx_guard.iaddr.vir[n].condition)) {
          dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
          BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_VIR;
          BX_CPU_THIS_PTR guard_found.iaddr_index = n;
          return true; // on a breakpoint
        }
      }
    }
  }
#endif
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_LIN) {
    for (unsigned n=0; n<bx_guard.iaddr.num_linear; n++) {
      if (bx_guard.iaddr.lin[n].enabled &&
         (bx_guard.iaddr.lin[n].addr == laddr))
      {
        if (! bx_guard.iaddr.lin[n].condition || bx_dbg_eval_condition(bx_guard.iaddr.lin[n].condition)) {
          dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
          BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_LIN;
          BX_CPU_THIS_PTR guard_found.iaddr_index = n;
          return true; // on a breakpoint
        }
      }
    }
  }
#endif
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_PHY) {
    bx_phy_address phy;
    bool valid = true;
    bx_address eipBiased = debug_eip + BX_CPU_THIS_PTR eipPageBias;
    if (eipBiased < BX_CPU_THIS_PTR eipPageWindowSize)
      phy = BX_CPU_THIS_PTR pAddrFetchPage + eipBiased;
    else
      valid = dbg_xlate_linear2phy(laddr, &phy);
    if (valid) {
      for (unsigned n=0; n<bx_guard.iaddr.num_physical; n++) {
        if (bx_guard.iaddr.phy[n].enabled && (bx_guard.iaddr.phy[n].addr == phy))
        {
          if (! bx_guard.iaddr.phy[n].condition || bx_dbg_eval_condition(bx_guard.iaddr.phy[n].condition)) {
            dbg_get_guard_state(&BX_CPU_THIS_PTR guard_found.guard_state);
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_PHY;
            BX_CPU_THIS_PTR guard_found.iaddr_index = n;
            return true; // on a breakpoint
          }
        }
      }
    }
  }
#endif

  return false;
}

// Called by cpu_loop_debugger() right after getTraceCacheEntry(), before the
// fetched instruction is executed. Returns true if prefetch() found code
// breakpoint on the first instruction of the new fetch window.
bool BX_CPU_C::dbg_code_bp_after_fetch(void)
{
  if (BX_CPU_THIS_PTR dbg_code_bp_hit) {
    BX_CPU_THIS_PTR dbg_code_bp_hit = false;
    return true;
  }

  return false;
}
#endif

#if BX_GDBSTUB
bool BX_CPU_C::gdbstub_instruction_epilog(void)
{
  if (bx_dbg.gdbstub_enabled) {
    unsigned reason =
#if BX_SUPPORT_X86_64 == 0
      bx_gdbstub_check(EIP);
#else
      bx_gdbstub_check(RIP);
#endif
    if (reason != GDBSTUB_STOP_NO_REASON) 
      return(1);
  }

  return(0);
}
#endif
