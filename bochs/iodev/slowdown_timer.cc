/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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

#include "bochs.h"
#include "param_names.h"
#include "slowdown_timer.h"

#include <errno.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

//These need to stay printfs because they are useless in the log file.
#define BX_SLOWDOWN_PRINTF_FEEDBACK 0

#define SECINUSEC 1000000
#define usectosec(a) ((a)/SECINUSEC)
#define sectousec(a) ((a)*SECINUSEC)
#define nsectousec(a) ((a)/1000)

#define MSECINUSEC 1000
#define usectomsec(a) ((a)/MSECINUSEC)

#define Qval 1000
#define MAXMULT 1.5
#define REALTIME_Q SECINUSEC

#define LOG_THIS bx_slowdown_timer.

bx_slowdown_timer_c bx_slowdown_timer;

bx_slowdown_timer_c::bx_slowdown_timer_c()
{
  put("slowdown_timer", "STIMER");

  s.start_time=0;
  s.start_emulated_time=0;
  s.timer_handle=BX_NULL_TIMER_HANDLE;
}

void bx_slowdown_timer_c::init(void)
{
  // Return early if slowdown timer not selected
  if ((SIM->get_param_enum(BXPN_CLOCK_SYNC)->get() != BX_CLOCK_SYNC_SLOWDOWN) &&
      (SIM->get_param_enum(BXPN_CLOCK_SYNC)->get() != BX_CLOCK_SYNC_BOTH))
    return;

  BX_INFO(("using 'slowdown' timer synchronization method"));
  s.MAXmultiplier=MAXMULT;
  s.Q=Qval;

  if(s.MAXmultiplier<1)
    s.MAXmultiplier=1;

  s.start_time=sectousec(time(NULL));
  s.start_emulated_time = bx_pc_system.time_usec();
  s.lasttime=0;
  if (s.timer_handle == BX_NULL_TIMER_HANDLE) {
    s.timer_handle=bx_pc_system.register_timer(this, timer_handler, 100 , 1, 1,
      "slowdown_timer");
  }
  bx_pc_system.deactivate_timer(s.timer_handle);
  bx_pc_system.activate_timer(s.timer_handle,(Bit32u)s.Q,0);
}

void bx_slowdown_timer_c::exit(void)
{
  s.timer_handle = BX_NULL_TIMER_HANDLE;
}

void bx_slowdown_timer_c::after_restore_state(void)
{
  s.start_emulated_time = bx_pc_system.time_usec();
}

void bx_slowdown_timer_c::timer_handler(void * this_ptr)
{
  bx_slowdown_timer_c * class_ptr = (bx_slowdown_timer_c *) this_ptr;
  class_ptr->handle_timer();
}

void bx_slowdown_timer_c::handle_timer()
{
  Bit64u total_emu_time = (bx_pc_system.time_usec()) - s.start_emulated_time;
  Bit64u wanttime = s.lasttime+s.Q;
  Bit64u totaltime = sectousec(time(NULL)) - s.start_time;
  Bit64u thistime=(wanttime>totaltime)?wanttime:totaltime;

#if BX_SLOWDOWN_PRINTF_FEEDBACK
  printf("Entering slowdown timer handler.\n");
#endif

  /* Decide if we're behind.
   * Set interrupt interval accordingly. */
  if(totaltime > total_emu_time) {
    bx_pc_system.deactivate_timer(s.timer_handle);
    bx_pc_system.activate_timer(s.timer_handle,
      (Bit32u)(s.MAXmultiplier * (float)((Bit64s)s.Q)), 0);
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("running at MAX speed\n");
#endif
  } else {
    bx_pc_system.deactivate_timer(s.timer_handle);
    bx_pc_system.activate_timer(s.timer_handle,(Bit32u)s.Q,0);
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("running at NORMAL speed\n");
#endif
  }

  /* Make sure we took at least one time quantum. */
  /* This is a little strange.  I'll try to explain.
   * We're running bochs one second ahead of real time.
   *  this gives us a very precise division on whether
   *  we're ahead or behind the second line.
   * Basically, here's how it works:
   * *****|******************|***********...
   *      Time               Time+1sec
   *                        <^Bochs doesn't delay.
   *                          ^>Bochs delays.
   *    <^Bochs runs at MAX speed.
   *      ^>Bochs runs at normal
   */
  if(wanttime > (totaltime+REALTIME_Q)) {
#if BX_HAVE_USLEEP
    usleep(s.Q);
#elif BX_HAVE_MSLEEP
    msleep(usectomsec((Bit32u)s.Q));
#elif BX_HAVE_SLEEP
    sleep(usectosec(s.Q));
#else
#error do not know have to sleep
#endif    //delay(wanttime-totaltime);
    /* alternatively: delay(Q);
     * This works okay because we share the delay between
     * two time quantums.
     */
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("DELAYING for a quantum\n");
#endif
  }
  s.lasttime=thistime;

  //Diagnostic info:
#if 0
  if(wanttime > (totaltime+REALTIME_Q)) {
    if(totaltime > total_emu_time) {
      printf("Solving OpenBSD problem.\n");
    } else {
      printf("too fast.\n");
    }
  } else {
    if(totaltime > total_emu_time) {
      printf("too slow.\n");
    } else {
      printf("sometimes invalid state, normally okay.\n");
    }
  }
#endif // Diagnostic info
}

