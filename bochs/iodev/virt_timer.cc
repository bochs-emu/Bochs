#include "bochs.h"

#define BX_USE_VIRTUAL_TIMERS 1
#define BX_VIRTUAL_TIMERS_REALTIME 1

//Important constant #defines:
#define USEC_PER_SECOND (1000000)


// define a macro to convert floating point numbers into 64-bit integers.
// In MSVC++ you can convert a 64-bit float into a 64-bit signed integer,
// but it will not convert a 64-bit float into a 64-bit unsigned integer.
// This macro works around that.
#define F2I(x)  ((Bit64u)(Bit64s) (x))
#define I2F(x)  ((double)(Bit64s) (x))

//CONFIGURATION #defines:


//MAINLINE Configuration (For realtime PIT):

//How much faster than real time we can go:
#define MAX_MULT (1.25)

//Minimum number of emulated useconds per second.
//  Now calculated using BX_MIN_IPS, the minimum number of
//   instructions per second.
#define MIN_USEC_PER_SECOND (((((Bit64u)USEC_PER_SECOND)*((Bit64u)BX_MIN_IPS))/((Bit64u)(bx_options.Oips->get())))+(Bit64u)1)


//DEBUG configuration:

//Debug with printf options.
#define DEBUG_REALTIME_WITH_PRINTF 0

//Use to test execution at multiples of real time.
#define TIME_DIVIDER (1)
#define TIME_MULTIPLIER (1)
#define TIME_HEADSTART (0)


#define GET_VIRT_REALTIME64_USEC() (((bx_get_realtime64_usec()*(Bit64u)TIME_MULTIPLIER/(Bit64u)TIME_DIVIDER)))
//Set up Logging.
#define LOG_THIS bx_virt_timer.

//A single instance.
bx_virt_timer_c bx_virt_timer;


//Generic MAX and MIN Functions
#define BX_MAX(a,b) ( ((a)>(b))?(a):(b) )
#define BX_MIN(a,b) ( ((a)>(b))?(b):(a) )


//USEC_ALPHA is multiplier for the past.
//USEC_ALPHA_B is 1-USEC_ALPHA, or multiplier for the present.
#define USEC_ALPHA ((double)(.8))
#define USEC_ALPHA_B ((double)(((double)1)-USEC_ALPHA))
#define USEC_ALPHA2 ((double)(.5))
#define USEC_ALPHA2_B ((double)(((double)1)-USEC_ALPHA2))
#define ALPHA_LOWER(old,new) ((Bit64u)((old<new)?((USEC_ALPHA*(I2F(old)))+(USEC_ALPHA_B*(I2F(new)))):((USEC_ALPHA2*(I2F(old)))+(USEC_ALPHA2_B*(I2F(new))))))


//Conversion between emulated useconds and optionally realtime ticks.
#define TICKS_TO_USEC(a) ( ((a)*usec_per_second)/ticks_per_second )
#define USEC_TO_TICKS(a) ( ((a)*ticks_per_second)/usec_per_second )

bx_virt_timer_c::bx_virt_timer_c( void )
{
  put("VTIMER");
  settype(VTIMERLOG);

  numTimers = 0;
  current_timers_time = 0;
  timers_next_event_time = BX_MAX_VIRTUAL_TIME;
  last_sequential_time = 0;
  in_timer_handler = 0;
  virtual_next_event_time = BX_MAX_VIRTUAL_TIME;
  current_virtual_time = 0;

  use_virtual_timers = BX_USE_VIRTUAL_TIMERS;
  init_done = 0;
}

bx_virt_timer_c::~bx_virt_timer_c( void )
{
}



const Bit64u bx_virt_timer_c::NullTimerInterval = BX_MAX_VIRTUAL_TIME;

void
bx_virt_timer_c::nullTimer(void* this_ptr) {
  UNUSED(this_ptr);
}

void
bx_virt_timer_c::periodic(Bit64u time_passed) {
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
  unsigned i;
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
	timer[i].funct(timer[i].this_ptr);
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
  next_event_time_update();
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
  BX_ASSERT((!active) || (useconds>0));

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
  timer[i].id[BxMaxTimerIDLen]=0; //I like null terminated strings.

  if(useconds < timers_next_event_time) {
    timers_next_event_time = useconds;
    next_event_time_update();
    //FIXME
  }
  return i;
}

//unregister a previously registered timer.
unsigned
bx_virt_timer_c::unregisterTimer(int timerID) {
  if(!use_virtual_timers) {
    return bx_pc_system.unregisterTimer(timerID);
  }

  BX_ASSERT(timerID >= 0);
  BX_ASSERT(timerID < BX_MAX_VIRTUAL_TIMERS);

  if (timer[timerID].active) {
    BX_PANIC(("unregisterTimer: timer '%s' is still active!", timer[timerID].id));
    return(0); // Fail.
    }


  //No need to prevent doing this to unused timers.
  timer[timerID].inUse = 0;
  return(1);
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
  BX_ASSERT(useconds>0);

  timer[timer_index].period=useconds;
  timer[timer_index].timeToFire = current_timers_time + (Bit64u)useconds;
  timer[timer_index].active=1;
  timer[timer_index].continuous=continuous;

  if(useconds < timers_next_event_time) {
    timers_next_event_time = useconds;
    next_event_time_update();
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
  BX_ASSERT(time_passed <= virtual_next_event_time);

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
  if(init_done) {
    bx_pc_system.deactivate_timer(system_timer_id);
    BX_ASSERT(virtual_next_event_time);
    bx_pc_system.activate_timer(system_timer_id, 
				BX_MAX(1,TICKS_TO_USEC(virtual_next_event_time)),
				0);
  }
}

void
bx_virt_timer_c::init(void) {
  virtual_timers_realtime = bx_options.Orealtime_pit->get ();

  register_timer(this, nullTimer, NullTimerInterval, 1, 1, "Null Timer");

  system_timer_id = bx_pc_system.register_timer(this, pc_system_timer_handler,virtual_next_event_time , 0, 1, "Virtual Timer");

  //Real time variables:
  last_real_time=GET_VIRT_REALTIME64_USEC()+(Bit64u)TIME_HEADSTART*(Bit64u)USEC_PER_SECOND;
  total_real_usec=0;
  last_realtime_delta=0;
  //System time variables:
  last_usec;
  usec_per_second = USEC_PER_SECOND;
  stored_delta=0;
  last_system_usec=0;
  em_last_realtime=0;
  //Virtual timer variables:
  total_ticks=0;
  last_realtime_ticks=0;
  ticks_per_second = USEC_PER_SECOND;

  init_done = 1;
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

  Bit64u usec_delta = bx_pc_system.time_usec()-last_usec;

  if (usec_delta) {
#if BX_HAVE_REALTIME_USEC
    Bit64u ticks_delta = 0;
    Bit64u real_time_delta = GET_VIRT_REALTIME64_USEC() - last_real_time;
    Bit64u real_time_total = real_time_delta + total_real_usec;
    Bit64u system_time_delta = (Bit64u)usec_delta + (Bit64u)stored_delta;
    if(real_time_delta) {
      last_realtime_delta = real_time_delta;
      last_realtime_ticks = total_ticks;
    }
    ticks_per_second = USEC_PER_SECOND;

    //Start out with the number of ticks we would like
    // to have to line up with real time.
    ticks_delta = real_time_total - total_ticks;
    if(real_time_total < total_ticks) {
      //This slows us down if we're already ahead.
      //  probably only an issue on startup, but it solves some problems.
      ticks_delta = 0;
    }
    if(ticks_delta + total_ticks - last_realtime_ticks > (F2I(MAX_MULT * I2F(last_realtime_delta)))) {
      //This keeps us from going too fast in relation to real time.
#if 0
      ticks_delta = (F2I(MAX_MULT * I2F(last_realtime_delta))) + last_realtime_ticks - total_ticks;
#endif
      ticks_per_second = F2I(MAX_MULT * I2F(USEC_PER_SECOND));
    }
    if(ticks_delta > system_time_delta * USEC_PER_SECOND / MIN_USEC_PER_SECOND) {
      //This keeps us from having too few instructions between ticks.
      ticks_delta = system_time_delta * USEC_PER_SECOND / MIN_USEC_PER_SECOND;
    }
    if(ticks_delta > virtual_next_event_time) {
      //This keeps us from missing ticks.
      ticks_delta = virtual_next_event_time;
    }

    if(ticks_delta) {

#  if DEBUG_REALTIME_WITH_PRINTF
      //Every second print some info.
      if(((last_real_time + real_time_delta) / USEC_PER_SECOND) > (last_real_time / USEC_PER_SECOND)) {
	Bit64u temp1, temp2, temp3, temp4;
	temp1 = (Bit64u) total_real_usec;
	temp2 = (total_real_usec);
	temp3 = (Bit64u)total_ticks;
	temp4 = (Bit64u)((total_real_usec) - total_ticks);
	printf("useconds: %llu, ",temp1);
	printf("expect ticks: %llu, ",temp2);
	printf("ticks: %llu, ",temp3);
	printf("diff: %llu\n",temp4);
      }
#  endif

      last_real_time += real_time_delta;
      total_real_usec += real_time_delta;
      last_system_usec += system_time_delta;
      stored_delta = 0;
      total_ticks += ticks_delta;
    } else {
      stored_delta = system_time_delta;
    }


    Bit64u a,b;
    a=(usec_per_second);
    if(real_time_delta) {
      //FIXME
      Bit64u em_realtime_delta = last_system_usec + stored_delta - em_last_realtime;
      b=((Bit64u)USEC_PER_SECOND * em_realtime_delta / real_time_delta);
      em_last_realtime = last_system_usec + stored_delta;
    } else {
      b=a;
    }
    usec_per_second = ALPHA_LOWER(a,b);
#else
    BX_ASSERT(0);
#endif

    advance_virtual_time(ticks_delta);
  }

  last_usec=last_usec + usec_delta;
  bx_pc_system.deactivate_timer(system_timer_id);
  BX_ASSERT(virtual_next_event_time);
  bx_pc_system.activate_timer(system_timer_id, 
			      BX_MAX(1,TICKS_TO_USEC(virtual_next_event_time)),
			      0);

}

void
bx_virt_timer_c::pc_system_timer_handler(void* this_ptr) {
  ((bx_virt_timer_c *)this_ptr)->timer_handler();
}

