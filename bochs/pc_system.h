/////////////////////////////////////////////////////////////////////////
// $Id: pc_system.h,v 1.17 2002-10-04 16:26:09 kevinlawton Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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




#define BX_MAX_TIMERS 16
#define BX_NULL_TIMER_HANDLE 10000


#if BX_SHOW_IPS
extern unsigned long ips_count;
#endif


typedef void (*bx_timer_handler_t)(void *);


extern class bx_pc_system_c bx_pc_system;

#ifdef PROVIDE_M_IPS
extern double m_ips;
#endif

class bx_pc_system_c : private logfunctions {
private:

  // ===============================
  // Timer oriented private features
  // ===============================

  struct {
    Bit64u  period;     // Timer periodocity in cpu ticks.
    Bit64u  timeToFire; // Time to fire next (in absolute ticks).
    Boolean active;     // 0=inactive, 1=active.
    Boolean continuous; // 0=one-shot timer, 1=continuous periodicity.
    bx_timer_handler_t funct;  // A callback function for when the
                               //   timer fires.
    void *this_ptr;            // The this-> pointer for C++ callbacks
                               //   has to be stored as well.
#define BxMaxTimerIDLen 32
    char id[BxMaxTimerIDLen]; // String ID of timer.
    } timer[BX_MAX_TIMERS];

  unsigned   numTimers;  // Number of currently allocated timers.
  Bit32u     currCountdown; // Current countdown ticks value (decrements to 0).
  Bit32u     currCountdownPeriod; // Length of current countdown period.
  Bit64u     ticksTotal; // Num ticks total since start of emulator execution.

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

  void   init_ips(Bit32u ips);
  int    register_timer( void *this_ptr, bx_timer_handler_t, Bit32u useconds,
                         Boolean continuous, Boolean active, const char *id);
  void   start_timers(void);
  void   activate_timer( unsigned timer_index, Bit32u useconds,
                         Boolean continuous );
  void   deactivate_timer( unsigned timer_index );
  static BX_CPP_INLINE void tick1(void) {
#if BX_SHOW_IPS
  {
  extern unsigned long ips_count;
  ips_count++;
  }
#endif
    if (--bx_pc_system.currCountdown == 0) {
      bx_pc_system.countdownEvent();
      }
    }
  static BX_CPP_INLINE void tickn(Bit64u n) {
#if BX_SHOW_IPS
  {
  extern unsigned long ips_count;
  ips_count += n;
  }
#endif
    while (n >= Bit64u(bx_pc_system.currCountdown)) {
      n -= Bit64u(bx_pc_system.currCountdown);
      bx_pc_system.currCountdown = 0;
      bx_pc_system.countdownEvent();
      // bx_pc_system.currCountdown is adjusted to new value by countdownevent().
      };
    // 'n' is not (or no longer) >= the countdown size.  We can just decrement
    // the remaining requested ticks and continue.
    bx_pc_system.currCountdown -= Bit32u(n);
    }

  int register_timer_ticks(void* this_ptr, bx_timer_handler_t, Bit64u ticks,
                           Boolean continuous, Boolean active, const char *id);
  void activate_timer_ticks(unsigned index, Bit64u instructions,
                            Boolean continuous);
  Bit64u time_usec();
  static BX_CPP_INLINE Bit64u time_ticks() {
    return bx_pc_system.ticksTotal +
      Bit64u(bx_pc_system.currCountdownPeriod - bx_pc_system.currCountdown);
    }
  static BX_CPP_INLINE Bit64u getTicksTotal(void) {
    return bx_pc_system.ticksTotal;
    }

  static BX_CPP_INLINE Bit32u  getNumCpuTicksLeftNextEvent(void) {
    return bx_pc_system.currCountdown;
    }
#if BX_DEBUGGER
  static void timebp_handler(void* this_ptr);
#endif


  // ===========================
  // Non-timer oriented features
  // ===========================

  Boolean HRQ;     // Hold Request
  //Boolean INTR;    // Interrupt


    // Address line 20 control:
    //   1 = enabled: extended memory is accessible
    //   0 = disabled: A20 address line is forced low to simulate
    //       an 8088 address map
  Boolean enable_a20;

    // start out masking physical memory addresses to:
    //   8086:      20 bits
    //    286:      24 bits
    //    386:      32 bits
    // when A20 line is disabled, mask physical memory addresses to:
    //    286:      20 bits
    //    386:      20 bits
    //
  Bit32u  a20_mask;

  void set_HRQ(Boolean val);  // set the Hold ReQuest line
  void set_INTR(Boolean value); // set the INTR line to value

  int IntEnabled( void );
  int InterruptSignal( PCS_OP operation );
  int ResetSignal( PCS_OP operation );
  Bit8u  IAC(void);

  bx_pc_system_c(void);

  Bit32u  inp(Bit16u addr, unsigned io_len);
  void    outp(Bit16u addr, Bit32u value, unsigned io_len);
  void    set_enable_a20(Bit8u value);
  Boolean get_enable_a20(void);
  void    exit(void);

  };
