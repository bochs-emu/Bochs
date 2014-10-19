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
////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//
//Realtime Algorithm (with gettimeofday)
//  HAVE:
//    Real number of usec.
//    Emulated number of usec.
//  WANT:
//    Number of ticks to use.
//    Number of emulated usec to wait until next try.
//
//  ticks=number of ticks needed to match total real usec.
//  if(desired ticks > max ticks for elapsed real time)
//     ticks = max ticks for elapsed real time.
//  if(desired ticks > max ticks for elapsed emulated usec)
//     ticks = max ticks for emulated usec.
//  next wait ticks = number of ticks until next event.
//  next wait real usec = (current ticks + next wait ticks) * usec per ticks
//  next wait emulated usec = next wait real usec * emulated usec / real usec
//  if(next wait emulated usec < minimum emulated usec for next wait ticks)
//     next wait emulated usec = minimum emulated usec for next wait ticks.
//  if(next wait emulated usec > max emulated usec wait)
//     next wait emulated usec = max emulated usec wait.
//
//  How to calculate elapsed real time:
//    store an unused time value whenever no ticks are used in a given time.
//    add this to the current elapsed time.
//  How to calculate elapsed emulated time:
//    same as above.
//  Above can be done by not updating last_usec and last_sec.
//
//  How to calculate emulated usec/real usec:
//    Each time there are actual ticks:
//      Alpha_product(old emulated usec, emulated usec);
//      Alpha_product(old real usec, real usec);
//    Divide resulting values.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "param_names.h"
#include "virt_timer.h"

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
#define MIN_USEC_PER_SECOND (((((Bit64u)USEC_PER_SECOND)*((Bit64u)BX_MIN_IPS))/((Bit64u)ips))+(Bit64u)1)


//DEBUG configuration:

//Debug with printf options.
#define DEBUG_REALTIME_WITH_PRINTF 0


#define GET_VIRT_REALTIME64_USEC() (bx_get_realtime64_usec())
//Set up Logging.
#define LOG_THIS bx_virt_timer.

//A single instance.
bx_virt_timer_c bx_virt_timer;


//USEC_ALPHA is multiplier for the past.
//USEC_ALPHA_B is 1-USEC_ALPHA, or multiplier for the present.
#define USEC_ALPHA ((double)(.8))
#define USEC_ALPHA_B ((double)(((double)1)-USEC_ALPHA))
#define USEC_ALPHA2 ((double)(.5))
#define USEC_ALPHA2_B ((double)(((double)1)-USEC_ALPHA2))
#define ALPHA_LOWER(old,new) ((Bit64u)((old<new)?((USEC_ALPHA*(I2F(old)))+(USEC_ALPHA_B*(I2F(new)))):((USEC_ALPHA2*(I2F(old)))+(USEC_ALPHA2_B*(I2F(new))))))


//Conversion between emulated useconds and optionally realtime ticks.
#define TICKS_TO_USEC(a) (((a)*usec_per_second)/ticks_per_second)
#define USEC_TO_TICKS(a) (((a)*ticks_per_second)/usec_per_second)

bx_virt_timer_c::bx_virt_timer_c()
{
  put("virt_timer", "VTIMER");

  setup();
}

const Bit64u bx_virt_timer_c::NullTimerInterval = BX_MAX_VIRTUAL_TIME;

void bx_virt_timer_c::nullTimer(void* this_ptr)
{
  UNUSED(this_ptr);
}

void bx_virt_timer_c::periodic(Bit64u time_passed, bx_bool mode)
{
  //Assert that we haven't skipped any events.
  BX_ASSERT (time_passed <= s[mode].timers_next_event_time);
  BX_ASSERT(!in_timer_handler);

  //Update time variables.
  s[mode].timers_next_event_time -= time_passed;
  s[mode].current_timers_time += time_passed;

  //If no events are occurring, just pass the time and we're done.
  if (time_passed < s[mode].timers_next_event_time) return;

  //Starting timer handler calls.
  in_timer_handler = 1;
  //Otherwise, cause any events to occur that should.
  unsigned i;
  for (i=0;i<numTimers;i++) {
    if (timer[i].inUse && timer[i].active) {
      if (timer[i].realtime != mode) continue;
      //Assert that we haven't skipped any timers.
      BX_ASSERT(s[mode].current_timers_time <= timer[i].timeToFire);
      if (timer[i].timeToFire == s[mode].current_timers_time) {
        if (timer[i].continuous) {
          timer[i].timeToFire += timer[i].period;
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
  //s[mode].timers_next_event_time normally contains a cycle count, not a cycle time.
  //  here we use it as a temporary variable that IS a cycle time,
  //  but then convert it back to a cycle count afterwards.
  s[mode].timers_next_event_time = s[mode].current_timers_time + BX_MAX_VIRTUAL_TIME;
  for (i=0;i<numTimers;i++) {
    if (timer[i].inUse && timer[i].active && (timer[i].realtime == mode) &&
        ((timer[i].timeToFire)<s[mode].timers_next_event_time)) {
      s[mode].timers_next_event_time = timer[i].timeToFire;
    }
  }
  s[mode].timers_next_event_time -= s[mode].current_timers_time;
  next_event_time_update(mode);
  //FIXME
}


//Get the current virtual time.
//  This may return the same value on subsequent calls.
Bit64u bx_virt_timer_c::time_usec(bx_bool mode)
{
  //Update the time here only if we're not in a timer handler.
  //If we're in a timer handler we're up-to-date, and otherwise
  // this prevents call stack loops.
  if (!in_timer_handler) {
    timer_handler(mode);
  }
  return s[mode].current_timers_time;
}

//Get the current virtual time.
//  This will return a monotonically increasing value.
// MUST NOT be called from within a timer interrupt.
Bit64u bx_virt_timer_c::time_usec_sequential(bx_bool mode)
{
  //Can't prevent call stack loops here, so this
  // MUST NOT be called from within a timer handler.
  BX_ASSERT(s[mode].timers_next_event_time>0);
  BX_ASSERT(!in_timer_handler);

  if (s[mode].last_sequential_time >= s[mode].current_timers_time) {
    periodic(1, mode);
    s[mode].last_sequential_time = s[mode].current_timers_time;
  }
  return s[mode].current_timers_time;
}


//Register a timer handler to go off after a given interval.
//Register a timer handler to go off with a periodic interval.
int bx_virt_timer_c::register_timer(void *this_ptr, bx_timer_handler_t handler,
                                    Bit32u useconds, bx_bool continuous,
                                    bx_bool active, bx_bool realtime,
                                    const char *id)
{
  //We don't like starting with a zero period timer.
  BX_ASSERT((!active) || (useconds>0));

  //Search for an unused timer.
  unsigned int i;
  for (i=0; i < numTimers; i++) {
    if ((timer[i].inUse == 0) || (i == numTimers))
      break;
  }
  // If we didn't find a free slot, increment the bound, numTimers.
  if (i == numTimers)
    numTimers++; // One new timer installed.
  BX_ASSERT(numTimers<BX_MAX_VIRTUAL_TIMERS);

  timer[i].inUse = 1;
  timer[i].period = useconds;
  timer[i].timeToFire = s[realtime].current_timers_time + (Bit64u)useconds;
  timer[i].active = active;
  timer[i].realtime = realtime;
  timer[i].continuous = continuous;
  timer[i].funct = handler;
  timer[i].this_ptr = this_ptr;
  strncpy(timer[i].id, id, BxMaxTimerIDLen);
  timer[i].id[BxMaxTimerIDLen-1]=0; //I like null terminated strings.

  if (realtime) {
    BX_DEBUG(("Timer #%d ('%s') using realtime synchronisation mode", i, timer[i].id));
  } else {
    BX_DEBUG(("Timer #%d ('%s') using standard mode", i, timer[i].id));
  }
  if (useconds < s[realtime].timers_next_event_time) {
    s[realtime].timers_next_event_time = useconds;
    next_event_time_update(realtime);
    //FIXME
  }
  return i;
}

//unregister a previously registered timer.
bx_bool bx_virt_timer_c::unregisterTimer(unsigned timerID)
{
  BX_ASSERT(timerID < BX_MAX_VIRTUAL_TIMERS);

  if (timer[timerID].active) {
    BX_PANIC(("unregisterTimer: timer '%s' is still active!", timer[timerID].id));
    return(0); // Fail.
  }

  //No need to prevent doing this to unused timers.
  timer[timerID].inUse = 0;
  if (timerID == (numTimers-1)) numTimers--;
  return 1;
}

void bx_virt_timer_c::start_timers(void)
{
  //FIXME
}

//activate a deactivated but registered timer.
void bx_virt_timer_c::activate_timer(unsigned timer_index, Bit32u useconds,
                                     bx_bool continuous)
{
  BX_ASSERT(timer_index < BX_MAX_VIRTUAL_TIMERS);

  BX_ASSERT(timer[timer_index].inUse);
  BX_ASSERT(useconds>0);

  bx_bool realtime = timer[timer_index].realtime;
  timer[timer_index].period = useconds;
  timer[timer_index].timeToFire = s[realtime].current_timers_time + (Bit64u)useconds;
  timer[timer_index].active = 1;
  timer[timer_index].continuous = continuous;

  if (useconds < s[realtime].timers_next_event_time) {
    s[realtime].timers_next_event_time = useconds;
    next_event_time_update(realtime);
    //FIXME
  }
}

//deactivate (but don't unregister) a currently registered timer.
void bx_virt_timer_c::deactivate_timer(unsigned timer_index)
{
  BX_ASSERT(timer_index < BX_MAX_VIRTUAL_TIMERS);

  //No need to prevent doing this to unused/inactive timers.
  timer[timer_index].active = 0;
}

void bx_virt_timer_c::advance_virtual_time(Bit64u time_passed, bx_bool mode)
{
  BX_ASSERT(time_passed <= s[mode].virtual_next_event_time);

  s[mode].current_virtual_time += time_passed;
  s[mode].virtual_next_event_time -= time_passed;

  if (s[mode].current_virtual_time > s[mode].current_timers_time) {
    periodic(s[mode].current_virtual_time - s[mode].current_timers_time, mode);
  }
}

//Called when next_event_time changes.
void bx_virt_timer_c::next_event_time_update(bx_bool mode)
{
  s[mode].virtual_next_event_time = s[mode].timers_next_event_time + s[mode].current_timers_time - s[mode].current_virtual_time;
  if (init_done) {
    bx_pc_system.deactivate_timer(s[mode].system_timer_id);
    BX_ASSERT(s[mode].virtual_next_event_time);
    bx_pc_system.activate_timer(s[mode].system_timer_id,
                                (Bit32u)BX_MIN(0x7FFFFFFF,BX_MAX(1,TICKS_TO_USEC(s[mode].virtual_next_event_time))),
                                0);
  }
}

void bx_virt_timer_c::setup(void)
{
  numTimers = 0;
  in_timer_handler = 0;
  for (unsigned i = 0; i < 2; i++) {
    s[i].current_timers_time = 0;
    s[i].timers_next_event_time = BX_MAX_VIRTUAL_TIME;
    s[i].last_sequential_time = 0;
    s[i].virtual_next_event_time = BX_MAX_VIRTUAL_TIME;
    s[i].current_virtual_time = 0;
  }
  init_done = 0;
}

void bx_virt_timer_c::init(void)
{
  // Local copy of IPS value to avoid reading it frequently in timer handler
  ips = SIM->get_param_num(BXPN_IPS)->get();

  register_timer(this, nullTimer, (Bit32u)NullTimerInterval, 1, 1, 0, "Null Timer #1");
  register_timer(this, nullTimer, (Bit32u)NullTimerInterval, 1, 1, 1, "Null Timer #2");

  s[0].system_timer_id = bx_pc_system.register_timer(this, pc_system_timer_handler_0,
                                                (Bit32u)s[0].virtual_next_event_time, 0, 1, "Virtual Timer #0");
  s[1].system_timer_id = bx_pc_system.register_timer(this, pc_system_timer_handler_1,
                                                (Bit32u)s[1].virtual_next_event_time, 0, 1, "Virtual Timer #1");

  //Real time variables:
#if BX_HAVE_REALTIME_USEC
  last_real_time = GET_VIRT_REALTIME64_USEC();
#endif
  total_real_usec = 0;
  last_realtime_delta = 0;
  real_time_delay = 0;
  //System time variables:
  last_usec = 0;
  usec_per_second = USEC_PER_SECOND;
  stored_delta = 0;
  last_system_usec = 0;
  em_last_realtime = 0;
  //Virtual timer variables:
  total_ticks = 0;
  last_realtime_ticks = 0;
  ticks_per_second = USEC_PER_SECOND;

  init_done = 1;
}

void bx_virt_timer_c::register_state(void)
{
  unsigned i;
  char name[4];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "virt_timer", "Virtual Timer State");
  bx_list_c *vtimers = new bx_list_c(list, "timer");
  for (i = 0; i < numTimers; i++) {
    sprintf(name, "%d", i);
    bx_list_c *bxtimer = new bx_list_c(vtimers, name);
    BXRS_PARAM_BOOL(bxtimer, inUse, timer[i].inUse);
    BXRS_DEC_PARAM_FIELD(bxtimer, period, timer[i].period);
    BXRS_DEC_PARAM_FIELD(bxtimer, timeToFire, timer[i].timeToFire);
    BXRS_PARAM_BOOL(bxtimer, active, timer[i].active);
    BXRS_PARAM_BOOL(bxtimer, continuous, timer[i].continuous);
    BXRS_PARAM_BOOL(bxtimer, realtime, timer[i].realtime);
  }
  bx_list_c *sys = new bx_list_c(list, "s");
  for (i = 0; i < 2; i++) {
    sprintf(name, "%d", i);
    bx_list_c *snum = new bx_list_c(sys, name);
    BXRS_DEC_PARAM_FIELD(snum, current_timers_time, s[i].current_timers_time);
    BXRS_DEC_PARAM_FIELD(snum, timers_next_event_time, s[i].timers_next_event_time);
    BXRS_DEC_PARAM_FIELD(snum, last_sequential_time, s[i].last_sequential_time);
    BXRS_DEC_PARAM_FIELD(snum, virtual_next_event_time, s[i].virtual_next_event_time);
    BXRS_DEC_PARAM_FIELD(snum, current_virtual_time, s[i].current_virtual_time);
  }
  BXRS_DEC_PARAM_SIMPLE(list, last_real_time);
  BXRS_DEC_PARAM_SIMPLE(list, total_real_usec);
  BXRS_DEC_PARAM_SIMPLE(list, last_realtime_delta);
  BXRS_DEC_PARAM_SIMPLE(list, last_usec);
  BXRS_DEC_PARAM_SIMPLE(list, usec_per_second);
  BXRS_DEC_PARAM_SIMPLE(list, stored_delta);
  BXRS_DEC_PARAM_SIMPLE(list, last_system_usec);
  BXRS_DEC_PARAM_SIMPLE(list, em_last_realtime);
  BXRS_DEC_PARAM_SIMPLE(list, total_ticks);
  BXRS_DEC_PARAM_SIMPLE(list, last_realtime_ticks);
  BXRS_DEC_PARAM_SIMPLE(list, ticks_per_second);
}

void bx_virt_timer_c::timer_handler(bx_bool mode)
{
  if (!mode) {
    Bit64u temp_final_time = bx_pc_system.time_usec();
    temp_final_time -= s[0].current_virtual_time;
    while (temp_final_time) {
      if ((temp_final_time)>(s[0].virtual_next_event_time)) {
        temp_final_time -= s[0].virtual_next_event_time;
        advance_virtual_time(s[0].virtual_next_event_time, 0);
      } else {
        advance_virtual_time(temp_final_time, 0);
        temp_final_time -= temp_final_time;
      }
    }
    bx_pc_system.activate_timer(s[0].system_timer_id,
                                (Bit32u)BX_MIN(0x7FFFFFFF,(s[0].virtual_next_event_time>2)?(s[0].virtual_next_event_time-2):1),
                                0);
    return;
  }

  Bit64u usec_delta = bx_pc_system.time_usec()-last_usec;

  if (usec_delta) {
#if BX_HAVE_REALTIME_USEC
    Bit64u ticks_delta = 0;
    Bit64u real_time_delta = GET_VIRT_REALTIME64_USEC() - last_real_time - real_time_delay;
    Bit64u real_time_total = real_time_delta + total_real_usec;
    Bit64u system_time_delta = (Bit64u)usec_delta + (Bit64u)stored_delta;
    if (real_time_delta) {
      last_realtime_delta = real_time_delta;
      last_realtime_ticks = total_ticks;
    }
    ticks_per_second = USEC_PER_SECOND;

    //Start out with the number of ticks we would like
    // to have to line up with real time.
    ticks_delta = real_time_total - total_ticks;
    if (real_time_total < total_ticks) {
      //This slows us down if we're already ahead.
      //  probably only an issue on startup, but it solves some problems.
      ticks_delta = 0;
    }
    if (ticks_delta + total_ticks - last_realtime_ticks > (F2I(MAX_MULT * I2F(last_realtime_delta)))) {
      //This keeps us from going too fast in relation to real time.
#if 0
      ticks_delta = (F2I(MAX_MULT * I2F(last_realtime_delta))) + last_realtime_ticks - total_ticks;
#endif
      ticks_per_second = F2I(MAX_MULT * I2F(USEC_PER_SECOND));
    }
    if (ticks_delta > system_time_delta * USEC_PER_SECOND / MIN_USEC_PER_SECOND) {
      //This keeps us from having too few instructions between ticks.
      ticks_delta = system_time_delta * USEC_PER_SECOND / MIN_USEC_PER_SECOND;
    }
    if (ticks_delta > s[1].virtual_next_event_time) {
      //This keeps us from missing ticks.
      ticks_delta = s[1].virtual_next_event_time;
    }

    if (ticks_delta) {

#  if DEBUG_REALTIME_WITH_PRINTF
      //Every second print some info.
      if (((last_real_time + real_time_delta) / USEC_PER_SECOND) > (last_real_time / USEC_PER_SECOND)) {
        Bit64u temp1, temp2, temp3, temp4;
        temp1 = (Bit64u) total_real_usec;
        temp2 = (total_real_usec);
        temp3 = (Bit64u)total_ticks;
        temp4 = (Bit64u)((total_real_usec) - total_ticks);
        printf("useconds: " FMT_LL "u, ", temp1);
        printf("expect ticks: " FMT_LL "u, ", temp2);
        printf("ticks: " FMT_LL "u, ", temp3);
        printf("diff: "FMT_LL "u\n", temp4);
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

    Bit64u a = usec_per_second, b;
    if (real_time_delta) {
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
#if BX_HAVE_REALTIME_USEC
    advance_virtual_time(ticks_delta, 1);
#endif
  }

  last_usec=last_usec + usec_delta;
  bx_pc_system.deactivate_timer(s[1].system_timer_id);
  BX_ASSERT(s[1].virtual_next_event_time);
  bx_pc_system.activate_timer(s[1].system_timer_id,
                              (Bit32u)BX_MIN(0x7FFFFFFF,BX_MAX(1,TICKS_TO_USEC(s[1].virtual_next_event_time))),
                              0);
}

void bx_virt_timer_c::pc_system_timer_handler_0(void* this_ptr)
{
  ((bx_virt_timer_c *)this_ptr)->timer_handler(0);
}

void bx_virt_timer_c::pc_system_timer_handler_1(void* this_ptr)
{
  ((bx_virt_timer_c *)this_ptr)->timer_handler(1);
}

void bx_virt_timer_c::set_realtime_delay()
{
  real_time_delay = GET_VIRT_REALTIME64_USEC() - last_real_time;
}
