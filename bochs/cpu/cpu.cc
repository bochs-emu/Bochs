/////////////////////////////////////////////////////////////////////////
// $Id: cpu.cc,v 1.43 2002-09-15 15:10:21 kevinlawton Exp $
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

#define BX_INSTR_SPY 0


#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#define LOG_THIS BX_CPU_THIS_PTR

#if BX_USE_CPU_SMF
#define this (BX_CPU(0))
#endif

#if BX_EXTERNAL_DEBUGGER
#include "cpu/extdb.h"
#endif



#if BX_SIM_ID == 0   // only need to define once
// This array defines a look-up table for the even parity-ness
// of an 8bit quantity, for optimal assignment of the parity bit
// in the EFLAGS register
const Boolean bx_parity_lookup[256] = {
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
  };
#endif


#if BX_SMP_PROCESSORS==1
// single processor simulation, so there's one of everything
BX_CPU_C    bx_cpu;
BX_MEM_C    bx_mem;
#else
// multiprocessor simulation, we need an array of cpus and memories
BX_CPU_C    *bx_cpu_array[BX_SMP_PROCESSORS];
BX_MEM_C    *bx_mem_array[BX_ADDRESS_SPACES];
#endif



// notes:
//
// check limit of CS?

#ifdef REGISTER_IADDR
extern void REGISTER_IADDR(bx_addr addr);
#endif

// The CHECK_MAX_INSTRUCTIONS macro allows cpu_loop to execute a few
// instructions and then return so that the other processors have a chance to
// run.  This is used only when simulating multiple processors.
// 
// If maximum instructions have been executed, return.  A count less
// than zero means run forever.
#define CHECK_MAX_INSTRUCTIONS(count) \
  if (count >= 0) {                   \
    count--; if (count == 0) return;  \
  }

#if BX_SMP_PROCESSORS==1
#  define BX_TICK1_IF_SINGLE_PROCESSOR() BX_TICK1()
#else
#  define BX_TICK1_IF_SINGLE_PROCESSOR()
#endif

// Make code more tidy with a few macros.
#if BX_SUPPORT_X86_64==0
#define RIP EIP
#define RSP ESP
#endif


#if BX_DYNAMIC_TRANSLATION == 0
  void
BX_CPU_C::cpu_loop(Bit32s max_instr_count)
{
  unsigned ret;
  BxInstruction_t i;

#if BX_DEBUGGER
  BX_CPU_THIS_PTR break_point = 0;
#ifdef MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
#endif

  (void) setjmp( BX_CPU_THIS_PTR jmp_buf_env );

  // We get here either by a normal function call, or by a longjmp
  // back from an exception() call.  In either case, commit the
  // new EIP/ESP, and set up other environmental fields.  This code
  // mirrors similar code below, after the interrupt() call.
  BX_CPU_THIS_PTR prev_eip = RIP; // commit new EIP
  BX_CPU_THIS_PTR prev_esp = RSP; // commit new ESP
  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR errorno = 0;

main_cpu_loop:

  // First check on events which occurred for previous instructions
  // (traps) and ones which are asynchronous to the CPU
  // (hardware interrupts).
  if (BX_CPU_THIS_PTR async_event)
    goto handle_async_event;

async_events_processed:
  // Now we can handle things which are synchronous to instruction
  // execution.
  if (BX_CPU_THIS_PTR get_RF ()) {
    BX_CPU_THIS_PTR clear_RF ();
    }
#if BX_X86_DEBUGGER
  else {
    // only bother comparing if any breakpoints enabled
    if ( BX_CPU_THIS_PTR dr7 & 0x000000ff ) {
      Bit32u iaddr =
        BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base +
        BX_CPU_THIS_PTR prev_eip;
      Bit32u dr6_bits;
      if ( (dr6_bits = hwdebug_compare(iaddr, 1, BX_HWDebugInstruction,
                                       BX_HWDebugInstruction)) ) {
        // Add to the list of debug events thus far.
        BX_CPU_THIS_PTR debug_trap |= dr6_bits;
        BX_CPU_THIS_PTR async_event = 1;
        // If debug events are not inhibited on this boundary,
        // fire off a debug fault.  Otherwise handle it on the next
        // boundary. (becomes a trap)
        if ( !(BX_CPU_THIS_PTR inhibit_mask & BX_INHIBIT_DEBUG) ) {
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


#if BX_DEBUGGER
  {
  Bit32u debug_eip = BX_CPU_THIS_PTR prev_eip;
  if ( dbg_is_begin_instr_bpoint(
         BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
         debug_eip,
         BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + debug_eip,
         BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b) ) {
    return;
    }
  }
#endif  // #if BX_DEBUGGER

#if BX_INSTR_SPY
  {
    int n=0;
    if ((n & 0xffffff) == 0) {
      Bit32u cs = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value;
      Bit32u rip = BX_CPU_THIS_PTR prev_eip;
      fprintf (stdout, "instr %d, time %lld, pc %04x:%08x, fetch_ptr=%p\n", n, bx_pc_system.time_ticks (), cs, rip, fetch_ptr);
    }
    n++;
  }
#endif

#if BX_EXTERNAL_DEBUGGER
  if (regs.debug_state != debug_run) {
    bx_external_debugger(this);
  }
#endif

  {
  bx_address eipBiased;
  bx_address remainingInPage;
  unsigned maxFetch;
  Bit8u *fetchPtr;
  Boolean is32;

  eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;

  if ( eipBiased >= BX_CPU_THIS_PTR eipPageWindowSize ) {
    prefetch();
    eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
    }

  remainingInPage = (BX_CPU_THIS_PTR eipPageWindowSize - eipBiased);
  maxFetch = 15;
  if (remainingInPage < 15)
    maxFetch = remainingInPage;
  fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;

  is32 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;
#if BX_SUPPORT_X86_64
  if (BX_CPU_THIS_PTR cpu_mode == BX_MODE_LONG_64) {
    ret = FetchDecode64(fetchPtr, &i, maxFetch);
    }
  else
#endif
    {
    ret = FetchDecode(fetchPtr, &i, maxFetch, is32);
    }
  }

  if (ret) {
    if (i.ResolveModrm) {
      // call method on BX_CPU_C object
      BX_CPU_CALL_METHOD(i.ResolveModrm, (&i));
      }
fetch_decode_OK:

#if BX_DEBUGGER
    if (BX_CPU_THIS_PTR trace) {
      // print the instruction that is about to be executed.
#if (BX_SMP_PROCESSORS==1)
      bx_dbg_disassemble_current (0, 1);  // only one cpu, print time stamp
#else
      bx_dbg_disassemble_current (local_apic.get_id (), 1); // this cpu only
#endif
    }
#endif

    if ( !(i.rep_used && (i.attr & BxRepeatable)) ) {
      // non repeating instruction
      RIP += i.ilen;
      BX_CPU_CALL_METHOD(i.execute, (&i));

      BX_CPU_THIS_PTR prev_eip = RIP; // commit new EIP
      BX_CPU_THIS_PTR prev_esp = RSP; // commit new ESP
#ifdef REGISTER_IADDR
      REGISTER_IADDR(RIP + BX_CPU_THIS_PTR sregs[BX_SREG_CS].cache.u.segment.base);
#endif

      BX_TICK1_IF_SINGLE_PROCESSOR();
      }

    else {
repeat_loop:
      if (i.attr & BxRepeatableZF) {
#if BX_SUPPORT_X86_64
        if (i.as_64) {
          if (RCX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            RCX -= 1;
            }
          if ((i.rep_used==0xf3) && (get_ZF()==0)) goto repeat_done;
          if ((i.rep_used==0xf2) && (get_ZF()!=0)) goto repeat_done;
          if (RCX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else
#endif
        if (i.as_32) {
          if (ECX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            ECX -= 1;
            }
          if ((i.rep_used==0xf3) && (get_ZF()==0)) goto repeat_done;
          if ((i.rep_used==0xf2) && (get_ZF()!=0)) goto repeat_done;
          if (ECX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else {
          if (CX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            CX -= 1;
            }
          if ((i.rep_used==0xf3) && (get_ZF()==0)) goto repeat_done;
          if ((i.rep_used==0xf2) && (get_ZF()!=0)) goto repeat_done;
          if (CX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        }
      else { // normal repeat, no concern for ZF
#if BX_SUPPORT_X86_64
        if (i.as_64) {
          if (RCX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            RCX -= 1;
            }
          if (RCX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else
#endif
        if (i.as_32) {
          if (ECX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            ECX -= 1;
            }
          if (ECX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else { // 16bit addrsize
          if (CX != 0) {
            BX_CPU_CALL_METHOD(i.execute, (&i));
            CX -= 1;
            }
          if (CX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        }
      // shouldn't get here from above
repeat_not_done:
#ifdef REGISTER_IADDR
      REGISTER_IADDR(RIP + BX_CPU_THIS_PTR sregs[BX_SREG_CS].cache.u.segment.base);
#endif

      BX_TICK1_IF_SINGLE_PROCESSOR();

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event) {
        invalidate_prefetch_q();
        goto debugger_check;
      }
      goto repeat_loop;
#else  /* if BX_DEBUGGER == 1 */
      invalidate_prefetch_q();
      goto debugger_check;
#endif


repeat_done:
      RIP += i.ilen;

      BX_CPU_THIS_PTR prev_eip = RIP; // commit new EIP
      BX_CPU_THIS_PTR prev_esp = RSP; // commit new ESP
#ifdef REGISTER_IADDR
      REGISTER_IADDR(RIP + BX_CPU_THIS_PTR sregs[BX_SREG_CS].cache.u.segment.base);
#endif

      BX_TICK1_IF_SINGLE_PROCESSOR();
      }

debugger_check:

#if (BX_SMP_PROCESSORS>1 && BX_DEBUGGER==0)
    // The CHECK_MAX_INSTRUCTIONS macro allows cpu_loop to execute a few
    // instructions and then return so that the other processors have a chance
    // to run.  This is used only when simulating multiple processors.  If only
    // one processor, don't waste any cycles on it!  Also, it is not needed
    // with the debugger because its guard mechanism provides the same
    // functionality.
    CHECK_MAX_INSTRUCTIONS(max_instr_count);
#endif

#if BX_DEBUGGER

    // BW vm mode switch support is in dbg_is_begin_instr_bpoint
    // note instr generating exceptions never reach this point.

    // (mch) Read/write, time break point support
    if (BX_CPU_THIS_PTR break_point) {
	  switch (BX_CPU_THIS_PTR break_point) {
		case BREAK_POINT_TIME:
		      BX_INFO(("[%lld] Caught time breakpoint", bx_pc_system.time_ticks()));
		      BX_CPU_THIS_PTR stop_reason = STOP_TIME_BREAK_POINT;
		      return;
		case BREAK_POINT_READ:
		      BX_INFO(("[%lld] Caught read watch point", bx_pc_system.time_ticks()));
		      BX_CPU_THIS_PTR stop_reason = STOP_READ_WATCH_POINT;
		      return;
		case BREAK_POINT_WRITE:
		      BX_INFO(("[%lld] Caught write watch point", bx_pc_system.time_ticks()));
		      BX_CPU_THIS_PTR stop_reason = STOP_WRITE_WATCH_POINT;
		      return;
		default:
		      BX_PANIC(("Weird break point condition"));
	  }
    }
#ifdef MAGIC_BREAKPOINT
    // (mch) Magic break point support
    if (BX_CPU_THIS_PTR magic_break) {
	  if (bx_dbg.magic_break_enabled) {
		BX_DEBUG(("Stopped on MAGIC BREAKPOINT"));
		BX_CPU_THIS_PTR stop_reason = STOP_MAGIC_BREAK_POINT;
		return;
	  } else {
		BX_CPU_THIS_PTR magic_break = 0;
		BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
		BX_DEBUG(("Ignoring MAGIC BREAKPOINT"));
	  }
    }
#endif

    {
      // check for icount or control-C.  If found, set guard reg and return.
    Bit32u debug_eip = BX_CPU_THIS_PTR prev_eip;
    if ( dbg_is_end_instr_bpoint(
           BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value,
           debug_eip,
           BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + debug_eip,
           BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b) ) {
      return;
      }
    }

#endif  // #if BX_DEBUGGER
    goto main_cpu_loop;
    }
  else {
    unsigned j;
    Bit8u fetchBuffer[16]; // Really only need 15
    bx_address eipBiased, remainingInPage;
    Bit8u *fetchPtr;

    eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
    remainingInPage = (BX_CPU_THIS_PTR eipPageWindowSize - eipBiased);
    if (remainingInPage > 15) {
      BX_PANIC(("fetch_decode: remaining > max ilen"));
      }
    fetchPtr = BX_CPU_THIS_PTR eipFetchPtr + eipBiased;

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
      ret = FetchDecode64(fetchBuffer, &i, 15);
      }
    else
#endif
      {
      ret = FetchDecode(fetchBuffer, &i, 15,
          BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b);
      }
    // Restore EIP since we fudged it to start at the 2nd page boundary.
    RIP = BX_CPU_THIS_PTR prev_eip;
    if (ret==0)
      BX_PANIC(("fetchdecode: cross boundary: ret==0"));
    if (i.ResolveModrm) {
      BX_CPU_CALL_METHOD(i.ResolveModrm, (&i));
      }

// Since we cross an instruction boundary, note that we need a prefetch()
// again on the next instruction.  Perhaps we can optimize this to
// eliminate the extra prefetch() since we do it above, but have to
// think about repeated instructions, etc.
BX_CPU_THIS_PTR eipPageWindowSize = 0; // Fixme
    goto fetch_decode_OK;
    }



  //
  // This area is where we process special conditions and events.
  //

handle_async_event:

  if (BX_CPU_THIS_PTR debug_trap & 0x80000000) {
    // I made up the bitmask above to mean HALT state.
#if BX_SMP_PROCESSORS==1
    BX_CPU_THIS_PTR debug_trap = 0; // clear traps for after resume
    BX_CPU_THIS_PTR inhibit_mask = 0; // clear inhibits for after resume
    // for one processor, pass the time as quickly as possible until
    // an interrupt wakes up the CPU.
#if BX_DEBUGGER
    while (bx_guard.interrupt_requested != 1)
#else
    while (1)
#endif
      {
      if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR get_IF ()) {
        break;
        }
      if (BX_CPU_THIS_PTR async_event == 0) {
        BX_INFO(("decode: reset detected in halt state"));
        break;
        }
      BX_TICK1();
      }
#else      /* BX_SMP_PROCESSORS != 1 */
    // for multiprocessor simulation, even if this CPU is halted we still
    // must give the others a chance to simulate.  If an interrupt has 
    // arrived, then clear the HALT condition; otherwise just return from
    // the CPU loop with stop_reason STOP_CPU_HALTED.
    if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR get_IF ()) {
      // interrupt ends the HALT condition
      BX_CPU_THIS_PTR debug_trap = 0; // clear traps for after resume
      BX_CPU_THIS_PTR inhibit_mask = 0; // clear inhibits for after resume
      //bx_printf ("halt condition has been cleared in %s", name);
    } else {
      // HALT condition remains, return so other CPUs have a chance
#if BX_DEBUGGER
      BX_CPU_THIS_PTR stop_reason = STOP_CPU_HALTED;
#endif
      return;
    }
#endif
  } else if (BX_CPU_THIS_PTR kill_bochs_request) {
    // setting kill_bochs_request causes the cpu loop to return ASAP.
    return;
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

  // Priority 4: Traps on Previous Instruction
  //   Breakpoints
  //   Debug Trap Exceptions (TF flag set or data/IO breakpoint)
  if ( BX_CPU_THIS_PTR debug_trap &&
       !(BX_CPU_THIS_PTR inhibit_mask & BX_INHIBIT_DEBUG) ) {
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
  else if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR get_IF () && BX_DBG_ASYNC_INTR) {
    Bit8u vector;

    // NOTE: similar code in ::take_irq()
#if BX_SUPPORT_APIC
    if (BX_CPU_THIS_PTR int_from_local_apic)
      vector = BX_CPU_THIS_PTR local_apic.acknowledge_int ();
    else
      vector = BX_IAC(); // may set INTR with next interrupt
#else
    // if no local APIC, always acknowledge the PIC.
    vector = BX_IAC(); // may set INTR with next interrupt
#endif
    //BX_DEBUG(("decode: interrupt %u",
    //                                   (unsigned) vector));
    BX_CPU_THIS_PTR errorno = 0;
    BX_CPU_THIS_PTR EXT   = 1; /* external event */
    interrupt(vector, 0, 0, 0);
    BX_INSTR_HWINTERRUPT(vector, BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, EIP);
    // Set up environment, as would be when this main cpu loop gets
    // invoked.  At the end of normal instructions, we always commmit
    // the new EIP/ESP values.  But here, we call interrupt() much like
    // it was a sofware interrupt instruction, and need to effect the
    // commit here.  This code mirrors similar code above.
    BX_CPU_THIS_PTR prev_eip = RIP; // commit new RIP
    BX_CPU_THIS_PTR prev_esp = RSP; // commit new RSP
    BX_CPU_THIS_PTR EXT = 0;
    BX_CPU_THIS_PTR errorno = 0;
    }
  else if (BX_HRQ && BX_DBG_ASYNC_DMA) {
    // NOTE: similar code in ::take_dma()
    // assert Hold Acknowledge (HLDA) and go into a bus hold state
    BX_RAISE_HLDA();
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


  if (BX_CPU_THIS_PTR get_TF ()) {
    // TF is set before execution of next instruction.  Schedule
    // a debug trap (#DB) after execution.  After completion of
    // next instruction, the code above will invoke the trap.
    BX_CPU_THIS_PTR debug_trap |= 0x00004000; // BS flag in DR6
    }

  if ( !(BX_CPU_THIS_PTR INTR ||
         BX_CPU_THIS_PTR debug_trap ||
         BX_HRQ ||
         BX_CPU_THIS_PTR get_TF ()) )
    BX_CPU_THIS_PTR async_event = 0;
  goto async_events_processed;
}
#endif  // #if BX_DYNAMIC_TRANSLATION == 0




// boundaries of consideration:
//
//  * physical memory boundary: 1024k (1Megabyte) (increments of...)
//  * A20 boundary:             1024k (1Megabyte)
//  * page boundary:            4k
//  * ROM boundary:             2k (dont care since we are only reading)
//  * segment boundary:         any



  void
BX_CPU_C::prefetch(void)
{
  // cs:eIP
  // prefetch QSIZE byte quantity aligned on corresponding boundary
  bx_address laddr;
  Bit32u paddr;
  bx_address temp_rip;
  Bit32u temp_limit;
  bx_address laddrPageOffset0, eipPageOffset0;

  temp_rip   = RIP;
  temp_limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;

  laddr = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base +
                    temp_rip;

  if (((Bit32u)temp_rip) > temp_limit) {
    BX_PANIC(("prefetch: RIP > CS.limit"));
    }

#if BX_SUPPORT_PAGING
  if (BX_CPU_THIS_PTR cr0.pg) {
    // aligned block guaranteed to be all in one page, same A20 address
    paddr = itranslate_linear(laddr, CPL==3);
    paddr = A20ADDR(paddr);
    }
  else
#endif // BX_SUPPORT_PAGING
    {
    paddr = A20ADDR(laddr);
    }

  // check if segment boundary comes into play
  //if ((temp_limit - (Bit32u)temp_rip) < 4096) {
  //  }

  // Linear address at the beginning of the page.
  laddrPageOffset0 = laddr & 0xfffff000;
  // Calculate RIP at the beginning of the page.
  eipPageOffset0 = RIP - (laddr - laddrPageOffset0);
  BX_CPU_THIS_PTR eipPageBias = - eipPageOffset0;
  BX_CPU_THIS_PTR eipPageWindowSize = 4096; // FIXME:
  BX_CPU_THIS_PTR eipFetchPtr =
      BX_CPU_THIS_PTR mem->getHostMemAddr(A20ADDR(paddr & 0xfffff000), BX_READ);

  // Sanity checks
  if ( !BX_CPU_THIS_PTR eipFetchPtr ) {
    if ( paddr >= BX_CPU_THIS_PTR mem->len ) {
      BX_PANIC(("prefetch: running in bogus memory"));
      }
    else {
      BX_PANIC(("prefetch: getHostMemAddr vetoed direct read, paddr=0x%x.",
                paddr));
      }
    }
}


  // If control has transfered locally, it is possible the prefetch Q is
  // still valid.  This would happen for repeat instructions, and small
  // branches.
  void
BX_CPU_C::revalidate_prefetch_q(void)
{
  bx_address eipBiased;

  eipBiased = RIP + BX_CPU_THIS_PTR eipPageBias;
  if ( eipBiased < BX_CPU_THIS_PTR eipPageWindowSize ) {
    // Good, EIP still within prefetch window.
    }
  else {
    // EIP has branched outside the prefetch window.  Mark the
    // prefetch info as invalid, and requiring update.
    BX_CPU_THIS_PTR eipPageWindowSize = 0;
    }
}

  void
BX_CPU_C::invalidate_prefetch_q(void)
{
  BX_CPU_THIS_PTR eipPageWindowSize = 0;
}

#if BX_SUPPORT_X86_64
  void
BX_CPU_C::ask (int level, const char *prefix, const char *fmt, va_list ap)
{
#if BX_EXTERNAL_DEBUGGER
  char buf1[1024];
  vsprintf (buf1, fmt, ap);
  printf ("%s %s\n", prefix, buf1);
  trap_debugger(1);
#else
  this->logfunctions::ask(level,prefix,fmt,ap);
#endif
}

#if BX_EXTERNAL_DEBUGGER

  void
BX_CPU_C::trap_debugger (Boolean callnow)
{
  regs.debug_state = debug_step;
  if (callnow) {
    bx_external_debugger(this);
  }
}

#endif

#endif  // #if BX_SUPPORT_X86_64



#if BX_DEBUGGER
extern unsigned int dbg_show_mask;

  Boolean
BX_CPU_C::dbg_is_begin_instr_bpoint(Bit32u cs, Bit32u eip, Bit32u laddr,
                                    Bit32u is_32)
{
  //fprintf (stderr, "begin_instr_bp: checking cs:eip %04x:%08x\n", cs, eip);
  BX_CPU_THIS_PTR guard_found.cs  = cs;
  BX_CPU_THIS_PTR guard_found.eip = eip;
  BX_CPU_THIS_PTR guard_found.laddr = laddr;
  BX_CPU_THIS_PTR guard_found.is_32bit_code = is_32;

  // BW mode switch breakpoint
  // instruction which generate exceptions never reach the end of the
  // loop due to a long jump. Thats why we check at start of instr.
  // Downside is that we show the instruction about to be executed
  // (not the one generating the mode switch).
  if (BX_CPU_THIS_PTR mode_break && 
      (BX_CPU_THIS_PTR debug_vm != BX_CPU_THIS_PTR get_VM ())) {
    BX_INFO(("Caught vm mode switch breakpoint"));
    BX_CPU_THIS_PTR debug_vm = BX_CPU_THIS_PTR get_VM ();
    BX_CPU_THIS_PTR stop_reason = STOP_MODE_BREAK_POINT;
    return 1;
  }

  if( (BX_CPU_THIS_PTR show_flag) & (dbg_show_mask)) {
    int rv;
    if((rv = bx_dbg_symbolic_output()))
      return rv;
  }

  // see if debugger is looking for iaddr breakpoint of any type
  if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_ALL) {
#if BX_DBG_SUPPORT_VIR_BPOINT
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_VIR) {
      if (BX_CPU_THIS_PTR guard_found.icount!=0) {
        for (unsigned i=0; i<bx_guard.iaddr.num_virtual; i++) {
          if ( (bx_guard.iaddr.vir[i].cs  == cs) &&
               (bx_guard.iaddr.vir[i].eip == eip) ) {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_VIR;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
            return(1); // on a breakpoint
            }
          }
        }
      }
#endif
#if BX_DBG_SUPPORT_LIN_BPOINT
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_LIN) {
      if (BX_CPU_THIS_PTR guard_found.icount!=0) {
        for (unsigned i=0; i<bx_guard.iaddr.num_linear; i++) {
          if ( bx_guard.iaddr.lin[i].addr == BX_CPU_THIS_PTR guard_found.laddr ) {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_LIN;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
            return(1); // on a breakpoint
            }
          }
        }
      }
#endif
#if BX_DBG_SUPPORT_PHY_BPOINT
    if (bx_guard.guard_for & BX_DBG_GUARD_IADDR_PHY) {
      Bit32u phy;
      Boolean valid;
      dbg_xlate_linear2phy(BX_CPU_THIS_PTR guard_found.laddr,
                              &phy, &valid);
      // The "guard_found.icount!=0" condition allows you to step or
      // continue beyond a breakpoint.  Bryce tried removing it once,
      // and once you get to a breakpoint you are stuck there forever.
      // Not pretty.
      if (valid && (BX_CPU_THIS_PTR guard_found.icount!=0)) {
        for (unsigned i=0; i<bx_guard.iaddr.num_physical; i++) {
          if ( bx_guard.iaddr.phy[i].addr == phy ) {
            BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_IADDR_PHY;
            BX_CPU_THIS_PTR guard_found.iaddr_index = i;
            return(1); // on a breakpoint
            }
          }
        }
      }
#endif
    }
  return(0); // not on a breakpoint
}


  Boolean
BX_CPU_C::dbg_is_end_instr_bpoint(Bit32u cs, Bit32u eip, Bit32u laddr,
                                  Bit32u is_32)
{
  //fprintf (stderr, "end_instr_bp: checking for icount or ^C\n");
  BX_CPU_THIS_PTR guard_found.icount++;

  // convenient point to see if user typed Ctrl-C
  if (bx_guard.interrupt_requested &&
      (bx_guard.guard_for & BX_DBG_GUARD_CTRL_C)) {
    BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_CTRL_C;
    return(1);
    }

  // see if debugger requesting icount guard
  if (bx_guard.guard_for & BX_DBG_GUARD_ICOUNT) {
    if (BX_CPU_THIS_PTR guard_found.icount >= bx_guard.icount) {
      BX_CPU_THIS_PTR guard_found.cs  = cs;
      BX_CPU_THIS_PTR guard_found.eip = eip;
      BX_CPU_THIS_PTR guard_found.laddr = laddr;
      BX_CPU_THIS_PTR guard_found.is_32bit_code = is_32;
      BX_CPU_THIS_PTR guard_found.guard_found = BX_DBG_GUARD_ICOUNT;
      return(1);
      }
    }

#if (BX_NUM_SIMULATORS >= 2)
  // if async event pending, acknowlege them
  if (bx_guard.async_changes_pending.which) {
    if (bx_guard.async_changes_pending.which & BX_DBG_ASYNC_PENDING_A20)
      bx_dbg_async_pin_ack(BX_DBG_ASYNC_PENDING_A20,
                           bx_guard.async_changes_pending.a20);
    if (bx_guard.async_changes_pending.which) {
      BX_PANIC(("decode: async pending unrecognized."));
      }
    }
#endif
  return(0); // no breakpoint
}


  void
BX_CPU_C::dbg_take_irq(void)
{
  unsigned vector;

  // NOTE: similar code in ::cpu_loop()

  if ( BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR get_IF () ) {
    if ( setjmp(BX_CPU_THIS_PTR jmp_buf_env) == 0 ) {
      // normal return from setjmp setup
      vector = BX_IAC(); // may set INTR with next interrupt
      BX_CPU_THIS_PTR errorno = 0;
      BX_CPU_THIS_PTR EXT   = 1; // external event
      BX_CPU_THIS_PTR async_event = 1; // set in case INTR is triggered
      interrupt(vector, 0, 0, 0);
      }
    }
}

  void
BX_CPU_C::dbg_force_interrupt(unsigned vector)
{
  // Used to force slave simulator to take an interrupt, without
  // regard to IF

  if ( setjmp(BX_CPU_THIS_PTR jmp_buf_env) == 0 ) {
    // normal return from setjmp setup
    BX_CPU_THIS_PTR errorno = 0;
    BX_CPU_THIS_PTR EXT   = 1; // external event
    BX_CPU_THIS_PTR async_event = 1; // probably don't need this
    interrupt(vector, 0, 0, 0);
    }
}

  void
BX_CPU_C::dbg_take_dma(void)
{
  // NOTE: similar code in ::cpu_loop()
  if ( BX_HRQ ) {
    BX_CPU_THIS_PTR async_event = 1; // set in case INTR is triggered
    BX_RAISE_HLDA();
    }
}
#endif  // #if BX_DEBUGGER

