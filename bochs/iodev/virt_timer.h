////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef _BX_VIRT_TIMER_H
#define _BX_VIRT_TIMER_H

// should be adjusted if want to support more SMP processors
#define BX_MAX_VIRTUAL_TIMERS (32)

#define BX_MAX_VIRTUAL_TIME (0x7fffffff)

class BOCHSAPI bx_virt_timer_c : public logfunctions {
private:

  struct {
    bx_bool inUse;      // Timer slot is in-use (currently registered).
    Bit64u  period;     // Timer periodocity in virtual useconds.
    Bit64u  timeToFire; // Time to fire next (in virtual useconds).
    bx_bool active;     // 0=inactive, 1=active.
    bx_bool continuous; // 0=one-shot timer, 1=continuous periodicity.
    bx_bool realtime;   // 0=standard timer, 1=use realtime mode
    bx_timer_handler_t funct;  // A callback function for when the
                               //   timer fires.
                               //   This function MUST return.
    void *this_ptr;            // The this-> pointer for C++ callbacks
                               //   has to be stored as well.
    char id[BxMaxTimerIDLen]; // String ID of timer.
  } timer[BX_MAX_VIRTUAL_TIMERS];

  unsigned   numTimers;  // Number of currently allocated timers.

  struct {
    //Variables for the timer subsystem:
    Bit64u current_timers_time;
    Bit64u timers_next_event_time;
    Bit64u last_sequential_time;

    //Variables for the time sync subsystem:
    Bit64u virtual_next_event_time;
    Bit64u current_virtual_time;

    int system_timer_id;
  } s[2];

  bx_bool in_timer_handler;

  // Local copy of IPS value
  Bit64u ips;

  bx_bool init_done;

  //Real time variables:
  Bit64u last_real_time;
  Bit64u total_real_usec;
  Bit64u last_realtime_delta;
  Bit64u real_time_delay;
  //System time variables:
  Bit64u last_usec;
  Bit64u usec_per_second;
  Bit64u stored_delta;
  Bit64u last_system_usec;
  Bit64u em_last_realtime;
  //Virtual timer variables:
  Bit64u total_ticks;
  Bit64u last_realtime_ticks;
  Bit64u ticks_per_second;

  // A special null timer is always inserted in the timer[0] slot.  This
  // make sure that at least one timer is always active, and that the
  // duration is always less than a maximum 32-bit integer, so a 32-bit
  // counter can be used for the current countdown.
  static const Bit64u NullTimerInterval;
  static void nullTimer(void* this_ptr);

  //Step the given number of cycles, optionally calling any timer handlers.
  void periodic(Bit64u time_passed, bx_bool mode);

  //Called when next_event_time changes.
  void next_event_time_update(bx_bool mode);

  //Called to advance the virtual time.
  // calls periodic as needed.
  void advance_virtual_time(Bit64u time_passed, bx_bool mode);

public:

  bx_virt_timer_c();
  virtual ~bx_virt_timer_c() {}

  //Get the current virtual time.
  //  This may return the same value on subsequent calls.
  Bit64u time_usec(bx_bool mode);

  //Get the current virtual time.
  //  This will return a monotonically increasing value.
  // MUST NOT be called from within a timer handler.
  Bit64u time_usec_sequential(bx_bool mode);

  //Register a timer handler to go off after a given interval.
  //Register a timer handler to go off with a periodic interval.
  int  register_timer(void *this_ptr, bx_timer_handler_t handler,
                         Bit32u useconds, bx_bool continuous,
                         bx_bool active, bx_bool realtime, const char *id);

  //unregister a previously registered timer.
  bx_bool unregisterTimer(unsigned timerID);

  void start_timers(void);

  //activate a deactivated but registered timer.
  void activate_timer(unsigned timer_index, Bit32u useconds,
                         bx_bool continuous);

  //deactivate (but don't unregister) a currently registered timer.
  void deactivate_timer(unsigned timer_index);


  //Timer handlers passed to pc_system
  static void pc_system_timer_handler_0(void* this_ptr);
  static void pc_system_timer_handler_1(void* this_ptr);

  //The real timer handler.
  void timer_handler(bx_bool mode);

  //Initialization step #1 in constructor and for cleanup
  void setup(void);

  //Initialization step #2
  void init(void);

  void register_state(void);

  //Determine the real time elapsed during runtime config or between save and
  //restore.
  void set_realtime_delay(void);
};

BOCHSAPI extern bx_virt_timer_c bx_virt_timer;

#endif // _BX_VIRT_TIMER_H
