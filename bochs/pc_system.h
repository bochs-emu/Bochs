/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#ifndef BX_PCSYS_H
#define BX_PCSYS_H

#define BX_MAX_TIMERS 64
#define BX_NULL_TIMER_HANDLE 10000

typedef void (*bx_timer_handler_t)(void *);

BOCHSAPI extern class bx_pc_system_c bx_pc_system;

#ifdef PROVIDE_M_IPS
extern double m_ips;
#endif

class BOCHSAPI bx_pc_system_c : private logfunctions {
private:

  // ===============================
  // Timer oriented private features
  // ===============================

  struct {
    bx_bool inUse;      // Timer slot is in-use (currently registered).
    Bit64u  period;     // Timer periodocity in cpu ticks.
    Bit64u  timeToFire; // Time to fire next (in absolute ticks).
    bx_bool active;     // 0=inactive, 1=active.
    bx_bool continuous; // 0=one-shot timer, 1=continuous periodicity.
    bx_timer_handler_t funct;  // A callback function for when the
                               //   timer fires.
    void *this_ptr;            // The this-> pointer for C++ callbacks
                               //   has to be stored as well.
#define BxMaxTimerIDLen 32
    char id[BxMaxTimerIDLen];  // String ID of timer.
    Bit32u param;              // Device-specific value assigned to timer (optional)
  } timer[BX_MAX_TIMERS];

  unsigned   numTimers;  // Number of currently allocated timers.
  unsigned   triggeredTimer;  // ID of the actually triggered timer.
  Bit32u     currCountdown; // Current countdown ticks value (decrements to 0).
  Bit32u     currCountdownPeriod; // Length of current countdown period.
  Bit64u     ticksTotal; // Num ticks total since start of emulator execution.
  Bit64u     lastTimeUsec; // Last sequentially read time in usec.
  Bit64u     usecSinceLast; // Number of useconds claimed since then.

  // A special null timer is always inserted in the timer[0] slot.  This
  // make sure that at least one timer is always active, and that the
  // duration is always less than a maximum 32-bit integer, so a 32-bit
  // counter can be used for the current countdown.
  static const Bit64u NullTimerInterval;
  static void nullTimer(void* this_ptr);

#if !defined(PROVIDE_M_IPS)
  // This is the emulator speed, as measured in millions of
  // x86 instructions per second that it can emulate on some hypothetically
  // nomimal workload.
  double     m_ips; // Millions of Instructions Per Second
#endif

  // This handler is called when the function which decrements the clock
  // ticks finds that an event has occurred.
  void   countdownEvent(void);

public:

  // ==============================
  // Timer oriented public features
  // ==============================

  void   initialize(Bit32u ips);
  int    register_timer(void *this_ptr, bx_timer_handler_t, Bit32u useconds,
                         bx_bool continuous, bx_bool active, const char *id);
  bx_bool unregisterTimer(unsigned timerID);
  void   setTimerParam(unsigned timerID, Bit32u param);
  void   start_timers(void);
  void   activate_timer(unsigned timer_index, Bit32u useconds, bx_bool continuous);
  void   deactivate_timer(unsigned timer_index);
  unsigned triggeredTimerID(void) {
    return triggeredTimer;
  }
  Bit32u triggeredTimerParam(void) {
    return timer[triggeredTimer].param;
  }
  static BX_CPP_INLINE void tick1(void) {
    if (--bx_pc_system.currCountdown == 0) {
      bx_pc_system.countdownEvent();
    }
  }
  static BX_CPP_INLINE void tickn(Bit32u n) {
    while (n >= bx_pc_system.currCountdown) {
      n -= bx_pc_system.currCountdown;
      bx_pc_system.currCountdown = 0;
      bx_pc_system.countdownEvent();
      // bx_pc_system.currCountdown is adjusted to new value by countdownevent().
    }
    // 'n' is not (or no longer) >= the countdown size.  We can just decrement
    // the remaining requested ticks and continue.
    bx_pc_system.currCountdown -= n;
  }

  int register_timer_ticks(void* this_ptr, bx_timer_handler_t, Bit64u ticks,
                           bx_bool continuous, bx_bool active, const char *id);
  void activate_timer_ticks(unsigned index, Bit64u instructions,
                            bx_bool continuous);
  Bit64u time_usec();
  Bit64u time_usec_sequential();
  static BX_CPP_INLINE Bit64u time_ticks() {
    return bx_pc_system.ticksTotal +
      Bit64u(bx_pc_system.currCountdownPeriod - bx_pc_system.currCountdown);
  }

  static BX_CPP_INLINE Bit32u  getNumCpuTicksLeftNextEvent(void) {
    return bx_pc_system.currCountdown;
  }
#if BX_DEBUGGER
  static void timebp_handler(void* this_ptr);
#endif
  static void benchmarkTimer(void* this_ptr);
#if BX_ENABLE_STATISTICS
  static void dumpStatsTimer(void* this_ptr);
#endif

  // ===========================
  // Non-timer oriented features
  // ===========================

  bx_bool HRQ;     // Hold Request

  // Address line 20 control:
  //   1 = enabled: extended memory is accessible
  //   0 = disabled: A20 address line is forced low to simulate
  //       an 8088 address map
  bx_bool enable_a20;

  // start out masking physical memory addresses to:
  //   8086:      20 bits
  //    286:      24 bits
  //    386:      32 bits
  // when A20 line is disabled, mask physical memory addresses to:
  //    286:      20 bits
  //    386:      20 bits
  bx_phy_address a20_mask;

  volatile bx_bool kill_bochs_request;

  void set_HRQ(bx_bool val);  // set the Hold ReQuest line

  void raise_INTR(void);
  void clear_INTR(void);

  // Cpu and System Reset
  int Reset(unsigned type);
  Bit8u  IAC(void);

  bx_pc_system_c();

  Bit32u  inp(Bit16u addr, unsigned io_len) BX_CPP_AttrRegparmN(2);
  void    outp(Bit16u addr, Bit32u value, unsigned io_len) BX_CPP_AttrRegparmN(3);
  void    set_enable_a20(bx_bool value);
  bx_bool get_enable_a20(void);
  void    MemoryMappingChanged(void); // flush TLB in all CPUs
  void    invlpg(bx_address addr);    // flush TLB page in all CPUs
  void    exit(void);
  void    register_state(void);
};

#endif
