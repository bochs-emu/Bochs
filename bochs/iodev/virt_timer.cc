#include "bochs.h"

bx_virt_timer_c bx_virt_timer;

static void
bx_virt_timer_c::nullTimer(void* this_ptr) {
  UNUSED(this_ptr);
}

Bit64u get_next_event_time(void) {
  return next_event_time;
}

void periodic(Bit64u time_passed) {
  //Assert that we haven't skipped any events.
  BX_ASSERT (time_passed <= next_event_time);

  //Update time variables.
  next_event_time -= time_passed;
  current_virtual_time += time_passed;

  //If no events are occurring, just pass the time and we're done.
  if( time_passed < next_event_time ) {
    return;
  }
  //Otherwise, cause any events to occur that should.
  for(i=0;i<numTimers;i++) {
    if( timer[i].inUse && timer[i].active ) {
      //Assert that we haven't skipped any timers.
      BX_ASSERT(current_virtual_time <= timer[i].timeToFire);
      if(timer[i].timeToFire == current_virtual_time) {
	if(timer[i].continuous) {
	  timer[i].timeToFire+=timer[i].period;
	} else {
	  timer[i].active = 0;
	}
	//This function MUST return, or the timer mechanism
	// will be broken.
	timer[i].funct(this_ptr);
      }
    }
  }
  //Use a second FOR loop so that a timer function call can
  //  change the behavior of another timer.
  //next_event_time normally contains a cycle count, not a cycle time.
  //  here we use it as a temporary variable that IS a cycle time,
  //  but then convert it back to a cycle count afterwards.
  next_event_time = current_virtual_time + BX_MAX_VIRTUAL_TIME;
  for(i=0;i<numTimers;i++) {
    if( timer[i].inUse && timer[i].active && ((timer[i].timeToFire)<next_event_time) ) {
      next_event_time = timer[i].timeToFire;
    }
  }
  next_event_time-=current_virtual_time;
  //FIXME
}


//Get the current virtual time.
//  This may return the same value on subsequent calls.
Bit64u
bx_virt_timer_c::get_virtual_time(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.time_usec();
  }

  //I might choose to update the time here, but this is
  // safer, since it prevents call stack loops.
  return current_virtual_time;
}

//Get the current virtual time.
//  This will return a monotonically increasing value.
// MUST NOT be called from within a timer interrupt.
Bit64u
bx_virt_timer_c::time_usec_sequential(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.time_usec_sequential();
  }

  //Can't prevent call stack loops here, so this
  // MUST NOT be called from within a timer interrupt.
  BX_ASSERT(next_event_time>0);
  periodic(1);
  return current_virtual_time;
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

  //We don't like starting with a zero period timer.
  BX_ASSERT((!active) || (period>0));

  //Search for an unused timer.
  int i;
  for (i=0; i < numTimers; i++) {
    if (timer[i].inUse == 0 || i==numTimers)
      break;
    }
  // If we didn't find a free slot, increment the bound, numTimers.
  if (i==numTimers)
    numTimers++; // One new timer installed.
  BX_ASSERT(numTimers<BX_MAX_VIRTUAL_TIMERS);

  timer[i].inUse = 1;
  timer[i].period = useconds;
  timer[i].timeToFire = current_virtual_time + (Bit64u)useconds;
  timer[i].active = active;
  timer[i].continuous = continuous;
  timer[i].funct = handler;
  timer[i].this_ptr = this_ptr;
  strncpy(timer[i].id, id, BxMaxTimerIDLen);
  id[BxMaxTimerIDLen]=0; //I like null terminated strings.

  if(period < next_event_time) {
    next_event_time = period;
    //FIXME
  }
}

//unregister a previously registered timer.
unsigned
bx_virt_timer_c::unregisterTimer(int timerID) {
  if(!use_virtual_timers) {
    return bx_pc_system.unregisterTimer(timerID);
  }
  //FIXME: Need error checking.
  timer[timerID].inUse = 0;
}

void
bx_virt_timer_c::start_timers(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.start_timers();
  }
  //FIXME
}

//activate a deactivated but registered timer.
void
bx_virt_timer_c::activate_timer( unsigned timer_index, Bit32u useconds,
		       bx_bool continuous ) {
  if(!use_virtual_timers) {
    return bx_pc_system.activate_timer(timer_index, useconds, continuous);
  }
  //FIXME
}

//deactivate (but don't unregister) a currently registered timer.
void
bx_virt_timer_c::deactivate_timer( unsigned timer_index ) {
  if(!use_virtual_timers) {
    return bx_pc_system.deactivate_timer(timer_index);
  }
  //FIXME: Need error checking.
  timer[timer_index].active = 0;
}

