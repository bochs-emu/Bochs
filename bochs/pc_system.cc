/////////////////////////////////////////////////////////////////////////
// $Id: pc_system.cc,v 1.27 2002-10-04 16:26:08 kevinlawton Exp $
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



#include "bochs.h"
#define LOG_THIS bx_pc_system.

#ifdef WIN32
#ifndef __MINGW32__
// #include <winsock2.h> // +++
#include <winsock.h>
#endif
#endif

#if BX_SHOW_IPS
unsigned long ips_count=0;
#endif

#if defined(PROVIDE_M_IPS)
double     m_ips; // Millions of Instructions Per Second
#endif

// Option for turning off BX_TIMER_DEBUG?
// Check out m_ips and ips

#define SpewPeriodicTimerInfo 0
#define MinAllowableTimerPeriod 1


#if SpewPeriodicTimerInfo
// If debugging, set the heartbeat to 5M cycles.  Each heartbeat
// spews the active timer info.
const Bit64u bx_pc_system_c::NullTimerInterval = 5000000;
#else
// This must be the maximum 32-bit unsigned int value, NOT (Bit64u) -1.
const Bit64u bx_pc_system_c::NullTimerInterval = 0xffffffff;
#endif

  // constructor
bx_pc_system_c::bx_pc_system_c(void)
{
  this->put("SYS");

  // Timer[0] is the null timer.  It is initialized as a special
  // case here.  It should never be turned off or modified, and its
  // duration should always remain the same.
  ticksTotal = 0; // Reset ticks since emulator started.
  timer[0].period     = NullTimerInterval;
  timer[0].timeToFire = ticksTotal + NullTimerInterval;
  timer[0].active     = 1;
  timer[0].continuous = 1;
  timer[0].funct      = nullTimer;
  timer[0].this_ptr   = this;
  currCountdown       = NullTimerInterval;
  currCountdownPeriod = NullTimerInterval;
  numTimers = 1; // So far, only the nullTimer.
}

  void
bx_pc_system_c::init_ips(Bit32u ips)
{
  HRQ = 0;

  enable_a20 = 1;
  //set_INTR (0);

#if BX_CPU_LEVEL < 2
  a20_mask   =    0xfffff;
#elif BX_CPU_LEVEL == 2
  a20_mask   =   0xffffff;
#else /* 386+ */
  a20_mask   = 0xffffffff;
#endif

  // parameter 'ips' is the processor speed in Instructions-Per-Second
  m_ips = double(ips) / 1000000.0L;

  BX_DEBUG(("ips = %u", (unsigned) ips));
}

  void
bx_pc_system_c::set_HRQ(Boolean val)
{
  HRQ = val;
  if (val)
    BX_CPU(0)->async_event = 1;
}


#if (BX_NUM_SIMULATORS < 2)
  void
bx_pc_system_c::set_INTR(Boolean value)
{
  if (bx_dbg.interrupts)
    BX_INFO(("pc_system: Setting INTR=%d on bootstrap processor %d", (int)value, BX_BOOTSTRAP_PROCESSOR));
  //INTR = value;
  BX_CPU(BX_BOOTSTRAP_PROCESSOR)->set_INTR(value);
}
#endif

//
// Read from the IO memory address space
//

  Bit32u
bx_pc_system_c::inp(Bit16u addr, unsigned io_len)
{
  Bit32u ret;

  ret = bx_devices.inp(addr, io_len);

  return( ret );
}


//
// Write to the IO memory address space.
//

  void
bx_pc_system_c::outp(Bit16u addr, Bit32u value, unsigned io_len)
{
  bx_devices.outp(addr, value, io_len);
}

  void
bx_pc_system_c::set_enable_a20(Bit8u value)
{
#if BX_CPU_LEVEL < 2
    BX_PANIC(("set_enable_a20() called: 8086 emulation"));
#else

#if BX_SUPPORT_A20
  unsigned old_enable_a20 = enable_a20;

  if (value) {
    enable_a20 = 1;
#if BX_CPU_LEVEL == 2
    a20_mask   = 0xffffff;   /* 286: enable all 24 address lines */
#else /* 386+ */
    a20_mask   = 0xffffffff; /* 386: enable all 32 address lines */
#endif
    }
  else {
    enable_a20 = 0;
    a20_mask   = 0xffefffff;   /* mask off A20 address line */
    }

  BX_DBG_A20_REPORT(value);

  BX_DEBUG(("A20: set() = %u", (unsigned) enable_a20));

  // If there has been a transition, we need to notify the CPUs so
  // they can potentially invalidate certain cache info based on
  // A20-line-applied physical addresses.
  if (old_enable_a20 != enable_a20) {
    for (unsigned i=0; i<BX_SMP_PROCESSORS; i++)
      BX_CPU(i)->pagingA20Changed();
    }
#else
  BX_DEBUG(("set_enable_a20: ignoring: SUPPORT_A20 = 0"));
#endif  // #if BX_SUPPORT_A20

#endif
}

  Boolean
bx_pc_system_c::get_enable_a20(void)
{
#if BX_SUPPORT_A20
  if (bx_dbg.a20)
    BX_INFO(("A20: get() = %u", (unsigned) enable_a20));

  if (enable_a20) return(1);
  else return(0);
#else
  BX_INFO(("get_enable_a20: ignoring: SUPPORT_A20 = 0"));
  return(1);
#endif  // #if BX_SUPPORT_A20
}

  int
bx_pc_system_c::ResetSignal( PCS_OP operation )
{
  UNUSED( operation );
  // Reset the processor.

  BX_ERROR(( "# bx_pc_system_c::ResetSignal() called" ));
  for (int i=0; i<BX_SMP_PROCESSORS; i++)
    BX_CPU(i)->reset(BX_RESET_SOFTWARE);
  bx_devices.reset(BX_RESET_SOFTWARE);
  return(0);
}


  Bit8u
bx_pc_system_c::IAC(void)
{
  return( bx_devices.pic->IAC() );
}

  void
bx_pc_system_c::exit(void)
{
  if (bx_devices.hard_drive)
    bx_devices.hard_drive->close_harddrive();
  BX_INFO(("Last time is %d", bx_cmos.s.timeval));
  bx_gui.exit();
}


// ================================================
// Bochs internal timer delivery framework features
// ================================================

  int
bx_pc_system_c::register_timer( void *this_ptr, void (*funct)(void *),
  Bit32u useconds, Boolean continuous, Boolean active, const char *id)
{
  Bit64u ticks;

  // Convert useconds to number of ticks.
  ticks = (Bit64u) (double(useconds) * m_ips);

  return register_timer_ticks(this_ptr, funct, ticks, continuous, active, id);
}

  int
bx_pc_system_c::register_timer_ticks(void* this_ptr, bx_timer_handler_t funct,
    Bit64u ticks, Boolean continuous, Boolean active, const char *id)
{
  unsigned i;

#if BX_TIMER_DEBUG
  if (numTimers >= BX_MAX_TIMERS) {
    BX_PANIC(("register_timer: too many registered timers."));
    }
  if (this_ptr == NULL)
    BX_PANIC(("register_timer_ticks: this_ptr is NULL"));
  if (funct == NULL)
    BX_PANIC(("register_timer_ticks: funct is NULL"));
#endif

  // If the timer frequency is rediculously low, make it more sane.
  // This happens when 'ips' is too low.
  if (ticks < MinAllowableTimerPeriod) {
    //BX_INFO(("register_timer_ticks: adjusting ticks of %llu to min of %u",
    //          ticks, MinAllowableTimerPeriod));
    ticks = MinAllowableTimerPeriod;
    }

  i = numTimers;
  timer[i].period     = ticks;
  timer[i].timeToFire = (ticksTotal + Bit64u(currCountdownPeriod-currCountdown)) +
                        ticks;
  timer[i].active     = active;
  timer[i].continuous = continuous;
  timer[i].funct      = funct;
  timer[i].this_ptr   = this_ptr;
  strncpy(timer[i].id, id, BxMaxTimerIDLen);
  timer[i].id[BxMaxTimerIDLen-1] = 0; // Null terminate if not already.

  if (active) {
    if (ticks < Bit64u(currCountdown)) {
      // This new timer needs to fire before the current countdown.
      // Skew the current countdown and countdown period to be smaller
      // by the delta.
      currCountdownPeriod -= (currCountdown - Bit32u(ticks));
      currCountdown = Bit32u(ticks);
      }
    }

  numTimers++; // One new timer installed.

  // Return timer id.
  return(i);
}


  void
bx_pc_system_c::countdownEvent(void)
{
  unsigned i;
  Bit64u   minTimeToFire;
  Boolean  triggered[BX_MAX_TIMERS];

  // The countdown decremented to 0.  We need to service all the active
  // timers, and invoke callbacks from those timers which have fired.
#if BX_TIMER_DEBUG
  if (currCountdown != 0)
    BX_PANIC(("countdownEvent: ticks!=0"));
#endif

  // Increment global ticks counter by number of ticks which have
  // elapsed since the last update.
  ticksTotal += Bit64u(currCountdownPeriod);
  minTimeToFire = (Bit64u) -1;

  for (i=0; i < numTimers; i++) {
    triggered[i] = 0; // Reset triggered flag.
    if (timer[i].active) {
#if BX_TIMER_DEBUG
      if (ticksTotal > timer[i].timeToFire)
        BX_PANIC(("countdownEvent: ticksTotal > timeToFire[%u], D %llu", i,
                  timer[i].timeToFire-ticksTotal));
#endif
      if (ticksTotal == timer[i].timeToFire) {
        // This timer is ready to fire.
        triggered[i] = 1;

        if (timer[i].continuous==0) {
          // If triggered timer is one-shot, deactive.
          timer[i].active = 0;
          }
        else {
          // Continuous timer, increment time-to-fire by period.
          timer[i].timeToFire += timer[i].period;
          if (timer[i].timeToFire < minTimeToFire)
            minTimeToFire = timer[i].timeToFire;
          }
        }
      else {
        // This timer is not ready to fire yet.
        if (timer[i].timeToFire < minTimeToFire)
          minTimeToFire = timer[i].timeToFire;
        }
      }
    }

  // Calculate next countdown period.  We need to do this before calling
  // any of the callbacks, as they may call timer features, which need
  // to be advanced to the next countdown cycle.
  currCountdown = currCountdownPeriod =
      Bit32u(minTimeToFire - ticksTotal);

  for (i=0; i < numTimers; i++) {
    // Call requested timer function.  It may request a different
    // timer period or deactivate etc.
    if (triggered[i]) {
      timer[i].funct(timer[i].this_ptr);
      }
    }
}

  void
bx_pc_system_c::nullTimer(void* this_ptr)
{
  // This function is always inserted in timer[0].  It is sort of
  // a heartbeat timer.  It ensures that at least one timer is
  // always active to make the timer logic more simple, and has
  // a duration of less than the maximum 32-bit integer, so that
  // a 32-bit size can be used for the hot countdown timer.  The
  // rest of the timer info can be 64-bits.  This is also a good
  // place for some logic to report actual emulated
  // instructions-per-second (IPS) data when measured relative to
  // the host computer's wall clock.

  UNUSED(this_ptr);

#if SpewPeriodicTimerInfo
  BX_INFO(("==================================="));
  for (unsigned i=0; i < bx_pc_system.numTimers; i++) {
    if (bx_pc_system.timer[i].active) {
      BX_INFO(("BxTimer(%s): period=%llu, continuous=%u",
               bx_pc_system.timer[i].id, bx_pc_system.timer[i].period,
               bx_pc_system.timer[i].continuous));
      }
    }
#endif
}

#if BX_DEBUGGER
  void
bx_pc_system_c::timebp_handler(void* this_ptr)
{
      BX_CPU(0)->break_point = BREAK_POINT_TIME;
      BX_DEBUG(( "Time breakpoint triggered" ));

      if (timebp_queue_size > 1) {
	    Bit64s new_diff = timebp_queue[1] - bx_pc_system.time_ticks();
	    bx_pc_system.activate_timer_ticks(timebp_timer, new_diff, 1);
      }
      timebp_queue_size--;
      for (int i = 0; i < timebp_queue_size; i++)
	    timebp_queue[i] = timebp_queue[i+1];
}
#endif // BX_DEBUGGER

  Bit64u
bx_pc_system_c::time_usec() {
  return (Bit64u) (((double)(Bit64s)time_ticks()) / m_ips );
}

  void
bx_pc_system_c::start_timers(void)
{
}

  void
bx_pc_system_c::activate_timer_ticks(unsigned i, Bit64u ticks, Boolean continuous)
{
#if BX_TIMER_DEBUG
  if (i >= numTimers)
    BX_PANIC(("activate_timer_ticks: timer %u OOB", i));
  if (timer[i].period < MinAllowableTimerPeriod)
    BX_PANIC(("activate_timer_ticks: timer[%u].period of %llu < min of %u",
              i, timer[i].period, MinAllowableTimerPeriod));
#endif

  // If the timer frequency is rediculously low, make it more sane.
  // This happens when 'ips' is too low.
  if (ticks < MinAllowableTimerPeriod) {
    //BX_INFO(("activate_timer_ticks: adjusting ticks of %llu to min of %u",
    //          ticks, MinAllowableTimerPeriod));
    ticks = MinAllowableTimerPeriod;
    }

  timer[i].period = ticks;
  timer[i].timeToFire = (ticksTotal + Bit64u(currCountdownPeriod-currCountdown)) +
                        ticks;
  timer[i].active     = 1;
  timer[i].continuous = continuous;

  if (ticks < Bit64u(currCountdown)) {
    // This new timer needs to fire before the current countdown.
    // Skew the current countdown and countdown period to be smaller
    // by the delta.
    currCountdownPeriod -= (currCountdown - Bit32u(ticks));
    currCountdown = Bit32u(ticks);
    }
}

  void
bx_pc_system_c::activate_timer(unsigned i, Bit32u useconds, Boolean continuous)
{
  Bit64u ticks;

#if BX_TIMER_DEBUG
  if (i >= numTimers)
    BX_PANIC(("activate_timer: timer %u OOB", i));
#endif

  // if useconds = 0, use default stored in period field
  // else set new period from useconds
  if (useconds==0) {
    ticks = timer[i].period;
    }
  else {
    // convert useconds to number of ticks
    ticks = (Bit64u) (double(useconds) * m_ips);

    // If the timer frequency is rediculously low, make it more sane.
    // This happens when 'ips' is too low.
    if (ticks < MinAllowableTimerPeriod) {
      //BX_INFO(("activate_timer: adjusting ticks of %llu to min of %u",
      //          ticks, MinAllowableTimerPeriod));
      ticks = MinAllowableTimerPeriod;
      }

    timer[i].period = ticks;
    }

  activate_timer_ticks(i, ticks, continuous);
}

  void
bx_pc_system_c::deactivate_timer( unsigned i )
{
#if BX_TIMER_DEBUG
  if (i >= numTimers)
    BX_PANIC(("deactivate_timer: timer %u OOB", i));
#endif

  timer[i].active = 0;
}
