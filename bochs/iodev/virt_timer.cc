#include "bochs.h"

bx_virt_timer_c bx_virt_timer;

static void
bx_virt_timer_c::nullTimer(void* this_ptr) {
  UNUSED(this_ptr);
}


//Get the current virtual time.
//  This may return the same value on subsequent calls.
Bit64u
bx_virt_timer_c::get_virtual_time(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.time_usec();
  }
}

//Get the current virtual time.
//  This will return a monotonically increasing value.
Bit64u
bx_virt_timer_c::time_usec_sequential(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.time_usec_sequential();
  }
}



//Register a timer handler to go off after a given interval.
//Register a timer handler to go off with a periodic interval.
int
bx_virt_timer_c::register_timer( void *this_ptr, bx_timer_handler_t handler,
				 Bit32u useconds,
				 bx_bool continuous, bx_bool active,
				 const char *id) {
  if(!use_virtual_timers) {
    return bx_pc_system.register_timer(this_ptr, handler, useconds,
				       continuous, active, id);
  }
}

//unregister a previously registered timer.
unsigned
bx_virt_timer_c::unregisterTimer(int timerID) {
  if(!use_virtual_timers) {
    return bx_pc_system.unregisterTimer(timerID);
  }
}

void
bx_virt_timer_c::start_timers(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.start_timers();
  }
}

//activate a deactivated but registered timer.
void
bx_virt_timer_c::activate_timer( unsigned timer_index, Bit32u useconds,
		       bx_bool continuous ) {
  if(!use_virtual_timers) {
    return bx_pc_system.activate_timer(timer_index, useconds, continuous);
  }
}

//deactivate (but don't unregister) a currently registered timer.
void
bx_virt_timer_c::deactivate_timer( unsigned timer_index ) {
  if(!use_virtual_timers) {
    return bx_pc_system.deactivate_timer(timer_index);
  }
}

