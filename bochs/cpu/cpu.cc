/////////////////////////////////////////////////////////////////////////
// $Id: cpu.cc,v 1.31 2002-06-05 21:51:30 yakovlev Exp $
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

//unsigned counter[2] = { 0, 0 };

#if BX_FETCHDECODE_CACHE
  static unsigned long bx_fdcache_sel;
  static unsigned long bx_fdcache_ip;

  static Bit32u new_phy_addr;
#endif // BX_FETCHDECODE_CACHE

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
extern void REGISTER_IADDR(Bit32u addr);
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

#if BX_DYNAMIC_TRANSLATION == 0
  void
BX_CPU_C::cpu_loop(Bit32s max_instr_count)
{
  unsigned ret;
  BxInstruction_t *i;
  unsigned maxisize;
  Bit8u *fetch_ptr;
  Boolean is_32;

#if !BX_FETCHDECODE_CACHE
  BxInstruction_t bxinstruction_dummy;
  i = &bxinstruction_dummy;
#endif // #if BX_FETCHDECODE_CACHE

#if BX_DEBUGGER
  BX_CPU_THIS_PTR break_point = 0;
#ifdef MAGIC_BREAKPOINT
  BX_CPU_THIS_PTR magic_break = 0;
#endif
  BX_CPU_THIS_PTR stop_reason = STOP_NO_REASON;
#endif

  (void) setjmp( BX_CPU_THIS_PTR jmp_buf_env );

  // not sure if these two are used during the async handling... --bbd
  BX_CPU_THIS_PTR prev_eip = EIP; // commit new EIP
  BX_CPU_THIS_PTR prev_esp = ESP; // commit new ESP

main_cpu_loop:

  // ???
  BX_CPU_THIS_PTR EXT = 0;
  BX_CPU_THIS_PTR errorno = 0;

  // First check on events which occurred for previous instructions
  // (traps) and ones which are asynchronous to the CPU
  // (hardware interrupts).
  if (BX_CPU_THIS_PTR async_event)
    goto handle_async_event;

async_events_processed:
  // added so that all debugging/tracing code uses the correct EIP even in the
  // instruction just after a trap/interrupt.  If you use the prev_eip that was
  // set before handle_async_event, traces and breakpoints fail to show the
  // first instruction of int/trap handlers.
  BX_CPU_THIS_PTR prev_eip = EIP; // commit new EIP
  BX_CPU_THIS_PTR prev_esp = ESP; // commit new ESP
  
  // Now we can handle things which are synchronous to instruction
  // execution.
  if (BX_CPU_THIS_PTR eflags.rf) {
    BX_CPU_THIS_PTR eflags.rf = 0;
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
      Bit32u eip = BX_CPU_THIS_PTR prev_eip;
      fprintf (stdout, "instr %d, time %lld, pc %04x:%08x, fetch_ptr=%p\n", n, bx_pc_system.time_ticks (), cs, eip, fetch_ptr);
    }
    n++;
  }
#endif

  is_32 = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.d_b;

  if (BX_CPU_THIS_PTR bytesleft == 0) {
    prefetch();
    }
  fetch_ptr = BX_CPU_THIS_PTR fetch_ptr;

#if BX_FETCHDECODE_CACHE
  bx_fdcache_ip = new_phy_addr;
  bx_fdcache_sel = bx_fdcache_ip & BX_FDCACHE_MASK;

  i = &(BX_CPU_THIS_PTR fdcache_i[bx_fdcache_sel]);

  if ((BX_CPU_THIS_PTR fdcache_ip[bx_fdcache_sel] == bx_fdcache_ip) &&
      (BX_CPU_THIS_PTR fdcache_is32[bx_fdcache_sel] == is_32)) {
    // HIT! ;^)
    ret = 1; // success!
    new_phy_addr += i->ilen;
  } else {
    // MISS :'(
#endif // #if BX_FETCHDECODE_CACHE

  maxisize = 16;
  if (BX_CPU_THIS_PTR bytesleft < 16) {
    maxisize = BX_CPU_THIS_PTR bytesleft;
    }
  ret = FetchDecode(fetch_ptr, i, maxisize, is_32);

#if BX_FETCHDECODE_CACHE
    // The instruction straddles a page boundary.
    // Not storing such instructions in the cache is probably the
    //   easiest way to handle them  
    if (ret) {
      Bit32u rpn,rpn_sel,old_rpn;
      if(BX_CPU_THIS_PTR fdcache_ip[bx_fdcache_sel] != 0xFFFFFFFF) {
	Bit32u next_ptr=BX_CPU_THIS_PTR fdcache_rpn_list[bx_fdcache_sel].next;
	Bit32u prev_ptr=BX_CPU_THIS_PTR fdcache_rpn_list[bx_fdcache_sel].prev;
	if(next_ptr != 0xFFFFFFFF) {
	  BX_CPU_THIS_PTR fdcache_rpn_list[next_ptr].prev=prev_ptr;
	}
	if(prev_ptr != 0xFFFFFFFF) {
	  BX_CPU_THIS_PTR fdcache_rpn_list[prev_ptr].next=next_ptr;
	} else {
	  Bit32u temp_rpn_sel = ((BX_CPU_THIS_PTR fdcache_ip[bx_fdcache_sel])>>12) & BX_FDCACHE_RPN_MASK;
	  BX_CPU_THIS_PTR fdcache_rpn_start[temp_rpn_sel] = next_ptr;
	}
      }
      BX_CPU_THIS_PTR fdcache_rpn_list[bx_fdcache_sel].prev = 0xFFFFFFFF;
      BX_CPU_THIS_PTR fdcache_ip[bx_fdcache_sel] = bx_fdcache_ip;
      BX_CPU_THIS_PTR fdcache_is32[bx_fdcache_sel] = is_32;
      new_phy_addr += i->ilen;
      rpn=bx_fdcache_ip>>12;
      rpn_sel=rpn & BX_FDCACHE_RPN_MASK;
      old_rpn=BX_CPU_THIS_PTR fdcache_rpn[rpn_sel];
      if((old_rpn != rpn) && (old_rpn != 0xFFFFFFFF)) {
	Bit32u index = BX_CPU_THIS_PTR fdcache_rpn_start[rpn_sel];
	for(;index!=0xFFFFFFFF;index=BX_CPU_THIS_PTR fdcache_rpn_list[index].next) {
	  BX_CPU_THIS_PTR fdcache_ip[index] = 0xFFFFFFFF;
	}
	BX_CPU_THIS_PTR fdcache_rpn[rpn_sel] = rpn;
	BX_CPU_THIS_PTR fdcache_rpn_start[rpn_sel] = bx_fdcache_sel;
	BX_CPU_THIS_PTR fdcache_rpn_list[bx_fdcache_sel].next=0xFFFFFFFF;
      } else {
	Bit32u index = BX_CPU_THIS_PTR fdcache_rpn_start[rpn_sel];
	BX_CPU_THIS_PTR fdcache_rpn_list[bx_fdcache_sel].next=index;
      }
    } else {
      // Invalidate cache!
      BX_CPU_THIS_PTR fdcache_ip[bx_fdcache_sel] = 0xFFFFFFFF;
    }
  }
#endif // #if BX_FETCHDECODE_CACHE

  if (ret) {
    if (i->ResolveModrm) {
      // call method on BX_CPU_C object
      BX_CPU_CALL_METHOD(i->ResolveModrm, (i));
      }
    BX_CPU_THIS_PTR fetch_ptr += i->ilen;
    BX_CPU_THIS_PTR bytesleft -= i->ilen;
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

    if (i->rep_used && (i->attr & BxRepeatable)) {
repeat_loop:
      if (i->attr & BxRepeatableZF) {
        if (i->as_32) {
          if (ECX != 0) {
            BX_CPU_CALL_METHOD(i->execute, (i));
            ECX -= 1;
            }
          if ((i->rep_used==0xf3) && (get_ZF()==0)) goto repeat_done;
          if ((i->rep_used==0xf2) && (get_ZF()!=0)) goto repeat_done;
          if (ECX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else {
          if (CX != 0) {
            BX_CPU_CALL_METHOD(i->execute, (i));
            CX -= 1;
            }
          if ((i->rep_used==0xf3) && (get_ZF()==0)) goto repeat_done;
          if ((i->rep_used==0xf2) && (get_ZF()!=0)) goto repeat_done;
          if (CX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        }
      else { // normal repeat, no concern for ZF
        if (i->as_32) {
          if (ECX != 0) {
            BX_CPU_CALL_METHOD(i->execute, (i));
            ECX -= 1;
            }
          if (ECX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        else { // 16bit addrsize
          if (CX != 0) {
            BX_CPU_CALL_METHOD(i->execute, (i));
            CX -= 1;
            }
          if (CX == 0) goto repeat_done;
          goto repeat_not_done;
          }
        }
      // shouldn't get here from above
repeat_not_done:
#ifdef REGISTER_IADDR
      REGISTER_IADDR(BX_CPU_THIS_PTR eip + BX_CPU_THIS_PTR sregs[BX_SREG_CS].cache.u.segment.base);
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
      BX_CPU_THIS_PTR eip += i->ilen;
      }
    else {
      // non repeating instruction
      BX_CPU_THIS_PTR eip += i->ilen;
      BX_CPU_CALL_METHOD(i->execute, (i));
      }

    BX_CPU_THIS_PTR prev_eip = EIP; // commit new EIP
    BX_CPU_THIS_PTR prev_esp = ESP; // commit new ESP
#ifdef REGISTER_IADDR
    REGISTER_IADDR(BX_CPU_THIS_PTR eip + BX_CPU_THIS_PTR sregs[BX_SREG_CS].cache.u.segment.base);
#endif

    BX_TICK1_IF_SINGLE_PROCESSOR();

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
    unsigned remain, j;
static Bit8u FetchBuffer[16];
    Bit8u *temp_ptr;

    // read all leftover bytes in current page
    for (j=0; j<BX_CPU_THIS_PTR bytesleft; j++) {
      FetchBuffer[j] = *fetch_ptr++;
      }

    // get remaining bytes for prefetch in next page
    // prefetch() needs eip current
    BX_CPU_THIS_PTR eip += BX_CPU_THIS_PTR bytesleft;
    remain = BX_CPU_THIS_PTR bytesleft;
    prefetch();

    if (BX_CPU_THIS_PTR bytesleft < 16) {
      // make sure (bytesleft - remain) below doesn't go negative
      BX_PANIC(("fetch_decode: bytesleft==0 after prefetch"));
      }
    temp_ptr = fetch_ptr = BX_CPU_THIS_PTR fetch_ptr;

    // read leftover bytes in next page
    for (; j<16; j++) {
      FetchBuffer[j] = *temp_ptr++;
      }
    ret = FetchDecode(FetchBuffer, i, 16, is_32);
    if (ret==0)
      BX_PANIC(("fetchdecode: cross boundary: ret==0"));
    if (i->ResolveModrm) {
      BX_CPU_CALL_METHOD(i->ResolveModrm, (i));
      }
    remain = i->ilen - remain;

    // note: eip has already been advanced to beginning of page
    BX_CPU_THIS_PTR fetch_ptr = fetch_ptr + remain;
    BX_CPU_THIS_PTR bytesleft -= remain;

    #if BX_FETCHDECODE_CACHE
      new_phy_addr += remain;
    #endif // BX_FETCHDECODE_CACHE

    //BX_CPU_THIS_PTR eip += remain;
    BX_CPU_THIS_PTR eip = BX_CPU_THIS_PTR prev_eip;
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
    while (bx_guard.interrupt_requested != 1) {
#else
    while (1) {
#endif
      if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR eflags.if_) {
        break;
        }
      BX_TICK1();
    }
#else      /* BX_SMP_PROCESSORS != 1 */
    // for multiprocessor simulation, even if this CPU is halted we still
    // must give the others a chance to simulate.  If an interrupt has 
    // arrived, then clear the HALT condition; otherwise just return from
    // the CPU loop with stop_reason STOP_CPU_HALTED.
    if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR eflags.if_) {
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
  else if (BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR eflags.if_ && BX_DBG_ASYNC_INTR) {
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
    BX_INSTR_HWINTERRUPT(vector, BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].selector.value, BX_CPU_THIS_PTR eip);
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


  if (BX_CPU_THIS_PTR eflags.tf) {
    // TF is set before execution of next instruction.  Schedule
    // a debug trap (#DB) after execution.  After completion of
    // next instruction, the code above will invoke the trap.
    BX_CPU_THIS_PTR debug_trap |= 0x00004000; // BS flag in DR6
    }

  if ( !(BX_CPU_THIS_PTR INTR ||
         BX_CPU_THIS_PTR debug_trap ||
         BX_HRQ ||
         BX_CPU_THIS_PTR eflags.tf) )
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
  Bit32u new_linear_addr;
#if !BX_FETCHDECODE_CACHE
  Bit32u new_phy_addr;
#endif // !BX_FETCHDECODE_CACHE
  Bit32u temp_eip, temp_limit;

  temp_eip   = BX_CPU_THIS_PTR eip;
  temp_limit = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.limit_scaled;

  new_linear_addr = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + temp_eip;
  BX_CPU_THIS_PTR prev_linear_page = new_linear_addr & 0xfffff000;
  if (temp_eip > temp_limit) {
    BX_PANIC(("prefetch: EIP > CS.limit"));
    }

#if BX_SUPPORT_PAGING
  if (BX_CPU_THIS_PTR cr0.pg) {
    // aligned block guaranteed to be all in one page, same A20 address
    new_phy_addr = itranslate_linear(new_linear_addr, CPL==3);
    new_phy_addr = A20ADDR(new_phy_addr);
    }
  else
#endif // BX_SUPPORT_PAGING
    {
    new_phy_addr = A20ADDR(new_linear_addr);
    }

  if ( new_phy_addr >= BX_CPU_THIS_PTR mem->len ) {
    // don't take this out if dynamic translation enabled,
    // otherwise you must make a check to see if bytesleft is 0 after
    // a call to prefetch() in the dynamic code.
    BX_ERROR(("prefetch: running in bogus memory"));
    }

  // max physical address as confined by page boundary
  BX_CPU_THIS_PTR prev_phy_page = new_phy_addr & 0xfffff000;
  BX_CPU_THIS_PTR max_phy_addr = BX_CPU_THIS_PTR prev_phy_page | 0x00000fff;

  // check if segment boundary comes into play
  //if ((temp_limit - temp_eip) < 4096) {
  //  }

#if BX_PCI_SUPPORT
  if ((new_phy_addr >= 0x000C0000) && (new_phy_addr <= 0x000FFFFF)) {
    BX_CPU_THIS_PTR bytesleft = 0x4000 - (new_phy_addr & 0x3FFF);
    BX_CPU_THIS_PTR fetch_ptr = bx_devices.pci->i440fx_fetch_ptr(new_phy_addr);
  } else {
    BX_CPU_THIS_PTR bytesleft = (BX_CPU_THIS_PTR max_phy_addr - new_phy_addr) + 1;
    BX_CPU_THIS_PTR fetch_ptr = &BX_CPU_THIS_PTR mem->vector[new_phy_addr];
  }
#else
  BX_CPU_THIS_PTR bytesleft = (BX_CPU_THIS_PTR max_phy_addr - new_phy_addr) + 1;
  BX_CPU_THIS_PTR fetch_ptr = &BX_CPU_THIS_PTR mem->vector[new_phy_addr];
#endif
}


  // If control has transfered locally, it is possible the prefetch Q is
  // still valid.  This would happen for repeat instructions, and small
  // branches.
  void
BX_CPU_C::revalidate_prefetch_q(void)
{
  Bit32u new_linear_addr, new_linear_page, new_linear_offset;
#if !BX_FETCHDECODE_CACHE
  Bit32u new_phy_addr;
#endif // !BX_FETCHDECODE_CACHE

  new_linear_addr = BX_CPU_THIS_PTR sregs[BX_SEG_REG_CS].cache.u.segment.base + BX_CPU_THIS_PTR eip;

  new_linear_page = new_linear_addr & 0xfffff000;
  if (new_linear_page == BX_CPU_THIS_PTR prev_linear_page) {
    // same linear address, old linear->physical translation valid
    new_linear_offset = new_linear_addr & 0x00000fff;
    new_phy_addr = BX_CPU_THIS_PTR prev_phy_page | new_linear_offset;
#if BX_PCI_SUPPORT
    if ((new_phy_addr >= 0x000C0000) && (new_phy_addr <= 0x000FFFFF)) {
      BX_CPU_THIS_PTR bytesleft = 0x4000 - (new_phy_addr & 0x3FFF);
      BX_CPU_THIS_PTR fetch_ptr = bx_devices.pci->i440fx_fetch_ptr(new_phy_addr);
      }
    else {
      BX_CPU_THIS_PTR bytesleft = (BX_CPU_THIS_PTR max_phy_addr - new_phy_addr) + 1;
      BX_CPU_THIS_PTR fetch_ptr = &BX_CPU_THIS_PTR mem->vector[new_phy_addr];
      }
#else
    BX_CPU_THIS_PTR bytesleft = (BX_CPU_THIS_PTR max_phy_addr - new_phy_addr) + 1;
    BX_CPU_THIS_PTR fetch_ptr = &BX_CPU_THIS_PTR mem->vector[new_phy_addr];
#endif
    }
  else {
    BX_CPU_THIS_PTR bytesleft = 0; // invalidate prefetch Q
    }
}

  void
BX_CPU_C::invalidate_prefetch_q(void)
{
  BX_CPU_THIS_PTR bytesleft = 0;
}




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
      (BX_CPU_THIS_PTR debug_vm != BX_CPU_THIS_PTR eflags.vm)) {
    BX_INFO(("Caught vm mode switch breakpoint"));
    BX_CPU_THIS_PTR debug_vm = BX_CPU_THIS_PTR eflags.vm;
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

  if ( BX_CPU_THIS_PTR INTR && BX_CPU_THIS_PTR eflags.if_ ) {
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

