#include "bochs.h"

#ifdef BX_USE_SLOWDOWN_TIMER

#define BX_SLOWDOWN_PRINTF_FEEDBACK 0

bx_slowdown_timer_c bx_slowdown_timer;

bx_slowdown_timer_c::bx_slowdown_timer_c() {
  s.start_time=0;
  s.start_emulated_time=0;
  s.timer_handle=BX_NULL_TIMER_HANDLE;
}

int
bx_slowdown_timer_c::init() {
  s.MAXmultiplier=2;
  s.Q=1000000;

  if(s.MAXmultiplier<1)
    s.MAXmultiplier=1;

  s.start_time=time(NULL)*1000000;
  s.start_emulated_time = bx_pc_system.time_usec();
  s.lasttime=0;
  s.timer_handle=bx_pc_system.register_timer(this, timer_handler, 100 , 1, 1);
  bx_pc_system.deactivate_timer(s.timer_handle);
  bx_pc_system.activate_timer(s.timer_handle,s.Q,0);
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
  Bit64u totaltime = (time(NULL)*1000000) - s.start_time;
  Bit64u thistime=(wanttime>totaltime)?wanttime:totaltime;

#if BX_SLOWDOWN_PRINTF_FEEDBACK
  printf("Entering slowdown timer handler.\n");
#endif

  /* Decide if we're behind.
   * Set interrupt interval accordingly. */
  if(totaltime > total_emu_time) {
    bx_pc_system.deactivate_timer(s.timer_handle);
    bx_pc_system.activate_timer(s.timer_handle,s.MAXmultiplier * s.Q,0);
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
  if(wanttime > totaltime) {
    sleep(s.Q/1000000);
    //delay(wanttime-totaltime);
    /*alternatively: delay(Q);
     * This works okay because we share the delay between
     * two time quantums.
     */
#if BX_SLOWDOWN_PRINTF_FEEDBACK
    printf("Delaying for a quantum\n");
#endif
  }
  s.lasttime=thistime;
}

#endif

