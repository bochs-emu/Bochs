/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.cc,v 1.6 2001-10-03 13:10:38 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
#include "bochs.h"
#include <errno.h>

#if BX_USE_SLOWDOWN_TIMER

#define BX_HAVE_USLEEP 1

//These need to stay printfs because they are useless in the log file.
#define BX_SLOWDOWN_PRINTF_FEEDBACK 0

#define SECINUSEC 1000000
#define usectosec(a) ((a)/SECINUSEC)
#define sectousec(a) ((a)*SECINUSEC)
#define nsectousec(a) ((a)/1000)

#if BX_HAVE_USLEEP
#  define Qval 10000
#else
#  define Qval SECINUSEC
#endif

#define MAXMULT 1.5
#define REALTIME_Q SECINUSEC

bx_slowdown_timer_c bx_slowdown_timer;

bx_slowdown_timer_c::bx_slowdown_timer_c() {
  s.start_time=0;
  s.start_emulated_time=0;
  s.timer_handle=BX_NULL_TIMER_HANDLE;
}

int
bx_slowdown_timer_c::init() {
  s.MAXmultiplier=MAXMULT;
  s.Q=Qval;

  if(s.MAXmultiplier<1)
    s.MAXmultiplier=1;

  s.start_time=sectousec(time(NULL));
  s.start_emulated_time = bx_pc_system.time_usec();
  s.lasttime=0;
  s.timer_handle=bx_pc_system.register_timer(this, timer_handler, 100 , 1, 1);
  bx_pc_system.deactivate_timer(s.timer_handle);
  bx_pc_system.activate_timer(s.timer_handle,(Bit32u)s.Q,0);
  return 0;
}

void
bx_slowdown_timer_c::timer_handler(void * this_ptr) {
  bx_slowdown_timer_c * class_ptr = (bx_slowdown_timer_c *) this_ptr;

  class_ptr->handle_timer();
}

void
bx_slowdown_timer_c::handle_timer() {
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
				(Bit32u)((Bit64u)
					 (s.MAXmultiplier * (float)s.Q)),
				0);
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("running at MAX speed\n");
#endif
  } else {
    bx_pc_system.deactivate_timer(s.timer_handle);
    bx_pc_system.activate_timer(s.timer_handle,s.Q,0);
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
#else
    sleep(usectosec(s.Q));
#endif
    //delay(wanttime-totaltime);
    /*alternatively: delay(Q);
     * This works okay because we share the delay between
     * two time quantums.
     */
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("DELAYING for a quantum\n");
#endif
  }
  s.lasttime=thistime;

  //Diagnostic info:
  if(wanttime > (totaltime+REALTIME_Q)) {
    if(totaltime > total_emu_time) {
      if(0) printf("Solving OpenBSD problem.\n");
    } else {
      if(0) printf("too fast.\n");
    }
  } else {
    if(totaltime > total_emu_time) {
      if(0) printf("too slow.\n");
    } else {
      if(0) printf("sometimes invalid state, normally okay.\n");
    }
  }
}

#endif

