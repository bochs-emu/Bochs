#include "bochs.h"

bx_virt_timer_c bx_virt_timer;

const Bit64u bx_pc_system_c::NullTimerInterval = BX_MAX_VIRTUAL_TIME;

static void
bx_virt_timer_c::nullTimer(void* this_ptr) {
  UNUSED(this_ptr);
}

void periodic(Bit64u time_passed) {
  //Assert that we haven't skipped any events.
  BX_ASSERT (time_passed <= timers_next_event_time);
  BX_ASSERT(!in_timer_handler);

  //Update time variables.
  timers_next_event_time -= time_passed;
  current_timers_time += time_passed;

  //If no events are occurring, just pass the time and we're done.
  if( time_passed < timers_next_event_time ) {
    return;
  }
  //Starting timer handler calls.
  in_timer_handler = 1;
  //Otherwise, cause any events to occur that should.
  for(i=0;i<numTimers;i++) {
    if( timer[i].inUse && timer[i].active ) {
      //Assert that we haven't skipped any timers.
      BX_ASSERT(current_timers_time <= timer[i].timeToFire);
      if(timer[i].timeToFire == current_timers_time) {
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
  //Finished timer handler calls.
  in_timer_handler = 0;
  //Use a second FOR loop so that a timer function call can
  //  change the behavior of another timer.
  //timers_next_event_time normally contains a cycle count, not a cycle time.
  //  here we use it as a temporary variable that IS a cycle time,
  //  but then convert it back to a cycle count afterwards.
  timers_next_event_time = current_timers_time + BX_MAX_VIRTUAL_TIME;
  for(i=0;i<numTimers;i++) {
    if( timer[i].inUse && timer[i].active && ((timer[i].timeToFire)<timers_next_event_time) ) {
      timers_next_event_time = timer[i].timeToFire;
    }
  }
  timers_next_event_time-=current_timers_time;
  //FIXME
}


//Get the current virtual time.
//  This may return the same value on subsequent calls.
Bit64u
bx_virt_timer_c::time_usec(void) {
  if(!use_virtual_timers) {
    return bx_pc_system.time_usec();
  }

  //I might choose to update the time here, but this is
  // safer, since it prevents call stack loops.
  return current_timers_time;
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
  // MUST NOT be called from within a timer handler.
  BX_ASSERT(timers_next_event_time>0);
  BX_ASSERT(!in_timer_handler);

  if(last_sequential_time >= current_timers_time) {
    periodic(1);
    last_sequential_time = current_timers_time;
  }
  return current_timers_time;
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
  timer[i].timeToFire = current_timers_time + (Bit64u)useconds;
  timer[i].active = active;
  timer[i].continuous = continuous;
  timer[i].funct = handler;
  timer[i].this_ptr = this_ptr;
  strncpy(timer[i].id, id, BxMaxTimerIDLen);
  id[BxMaxTimerIDLen]=0; //I like null terminated strings.

  if(period < timers_next_event_time) {
    timers_next_event_time = period;
    //FIXME
  }
}

//unregister a previously registered timer.
unsigned
bx_virt_timer_c::unregisterTimer(int timerID) {
  if(!use_virtual_timers) {
    return bx_pc_system.unregisterTimer(timerID);
  }

  BX_ASSERT(timerID >= 0);
  BX_ASSERT(timerID < BX_MAX_VIRTUAL_TIMERS);

  //No need to prevent doing this to unused timers.
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

  BX_ASSERT(timer_index >= 0);
  BX_ASSERT(timer_index < BX_MAX_VIRTUAL_TIMERS);

  BX_ASSERT(timer[timer_index].inUse);

  timer[timer_index].period=useconds;
  timer[timer_index].timeToFire = current_timers_time + (Bit64u)useconds;
  timer[timer_index].active=1;
  timer[timer_index].continuous=continuous;

  if(period < timers_next_event_time) {
    timers_next_event_time = period;
    //FIXME
  }
}

//deactivate (but don't unregister) a currently registered timer.
void
bx_virt_timer_c::deactivate_timer( unsigned timer_index ) {
  if(!use_virtual_timers) {
    return bx_pc_system.deactivate_timer(timer_index);
  }

  BX_ASSERT(timer_index >= 0);
  BX_ASSERT(timer_index < BX_MAX_VIRTUAL_TIMERS);

  //No need to prevent doing this to unused/inactive timers.
  timer[timer_index].active = 0;
}

void
bx_virt_timer_c::advance_virtual_time(Bit64u time_passed) {
  ASSERT(time_passed <= virtual_next_event_time);

  current_virtual_time += time_passed;
  virtual_next_event_time -= time_passed;

  if(current_virtual_time > current_timers_time) {
    periodic(current_virtual_time - current_timers_time);
  }
}

//Called when next_event_time changes.
void
bx_virt_timer_c::next_event_time_update(void) {
  virtual_next_event_time = timers_next_event_time + current_timers_time - current_virtual_time;
}

void
bx_virt_timer_c::init(void) {
  numTimers = 0;
  current_timers_time = 0;
  timers_next_event_time = BX_MAX_VIRTUAL_TIME;
  last_sequential_time = 0;
  in_timer_handler = 0;
  virtual_next_event_time = 0;
  current_virtual_time = 0;

  use_virtual_timers = BX_USE_VIRTUAL_TIMERS;
  virtual_timers_realtime = BX_VIRTUAL_TIMERS_REALTIME;

  register_timer(this, nullTimer, NullTimerInterval, 1, 1, "Null Timer");

  system_timer_id = bx_pc_system.register_timer(this, pc_system_timer_handler, BX_MAX_VIRTUAL_TIME, 0, 1, "Virtual Timer");
}

void
bx_virt_timer_c::timer_handler(void) {
  if(!virtual_timers_realtime) {
    Bit64u temp_final_time = bx_pc_system.time_usec();
    temp_final_time-=current_virtual_time;
    while(temp_final_time) {
      if((temp_final_time)>(virtual_next_event_time)) {
	temp_final_time-=virtual_next_event_time;
	advance_virtual_time(virtual_next_event_time);
      } else {
	advance_virtual_time(temp_final_time);
	temp_final_time-=temp_final_time;
      }
    }
    bx_pc_system.activate_timer(system_timer_id,
				(virtual_next_event_time>2)?(virtual_next_event_time-2):1,
				0);
    return;
  }

  
}

static void
bx_virt_timer_c::pc_system_timer_handler(void* this_ptr) {
  ((bx_virt_timer_c *)this_ptr)->timer_handler();
}

