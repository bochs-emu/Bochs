//  Copyright (C) 2001  MandrakeSoft S.A.
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

const Bit64u bx_pc_system_c::COUNTER_INTERVAL = 100000;

  // constructor
bx_pc_system_c::bx_pc_system_c(void)
{
  this->put("SYS");

  num_timers = 0;
  // set ticks period and remaining to max Bit32u value
  num_cpu_ticks_in_period = num_cpu_ticks_left = (Bit32u) -1;
  m_ips = 0.0L;

  for (unsigned int i=0; i < 8; i++) {
    DRQ[i] = 0;
    DACK[i] = 0;
    }
  TC = 0;
  HRQ = 0;
  HLDA = 0;

  enable_a20 = 1;
  //set_INTR (0);

#if BX_CPU_LEVEL < 2
  a20_mask   =    0xfffff;
#elif BX_CPU_LEVEL == 2
  a20_mask   =   0xffffff;
#else /* 386+ */
  a20_mask   = 0xffffffff;
#endif

  counter = 0;
  counter_timer_index = register_timer_ticks(this, bx_pc_system_c::counter_timer_handler, COUNTER_INTERVAL, 1, 1);
}

  void
bx_pc_system_c::init_ips(Bit32u ips)
{
  // parameter 'ips' is the processor speed in Instructions-Per-Second
  m_ips = double(ips) / 1000000.0L;
  BX_DEBUG(("ips = %u", (unsigned) ips));
}

  void
bx_pc_system_c::raise_HLDA(void)
{
  HLDA = 1;
  bx_devices.raise_hlda();
  HLDA = 0;
}

  void
bx_pc_system_c::set_DRQ(unsigned channel, Boolean val)
{
  if (channel > 7)
    BX_PANIC(("set_DRQ() channel > 7"));
  DRQ[channel] = val;
  bx_devices.drq(channel, val);
}

  void
bx_pc_system_c::set_HRQ(Boolean val)
{
  HRQ = val;
  if (val)
    BX_CPU(0)->async_event = 1;
  else
    HLDA = 0; // ??? needed?
}

  void
bx_pc_system_c::set_TC(Boolean val)
{
  TC = val;
}

  void
bx_pc_system_c::set_DACK(unsigned channel, Boolean val)
{
  DACK[channel] = val;
}


  void
bx_pc_system_c::dma_write8(Bit32u phy_addr, unsigned channel)
{
  // DMA controlled xfer of byte from I/O to Memory

  Bit8u data_byte;

  UNUSED(channel);
  bx_devices.dma_write8(channel, &data_byte);
  BX_MEM(0)->write_physical(BX_CPU(0), phy_addr, 1, &data_byte);

  BX_DBG_DMA_REPORT(phy_addr, 1, BX_WRITE, data_byte);
}


  void
bx_pc_system_c::dma_read8(Bit32u phy_addr, unsigned channel)
{
  // DMA controlled xfer of byte from Memory to I/O

  Bit8u data_byte;

  UNUSED(channel);
  BX_MEM(0)->read_physical(BX_CPU(0), phy_addr, 1, &data_byte);
  bx_devices.dma_read8(channel, &data_byte);

  BX_DBG_DMA_REPORT(phy_addr, 1, BX_READ, data_byte);
}


#if (BX_NUM_SIMULATORS < 2)
  void
bx_pc_system_c::set_INTR(Boolean value)
{
  if (bx_dbg.interrupts)
    BX_INFO(("pc_system: Setting INTR=%d on bootstrap processor %d", (int)value, BX_BOOTSTRAP_PROCESSOR));
  //INTR = value;
  int cpu = BX_BOOTSTRAP_PROCESSOR;
  BX_CPU(cpu)->set_INTR(value);
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


//
// bochs timer support
//


  void
bx_pc_system_c::timer_handler(void)
{
  Bit64u min;
  unsigned i;
  Bit64u delta;

  //  BX_ERROR(( "Time handler ptime = %d", bx_pc_system.time_ticks() ));

  delta = num_cpu_ticks_in_period - num_cpu_ticks_left;
#if BX_TIMER_DEBUG
  if (num_cpu_ticks_left != 0)
    BX_PANIC(("timer_handler: ticks_left!=0"));
#endif

  for (i=0; i < num_timers; i++) {
    timer[i].triggered = 0;
    if (timer[i].active) {
#if BX_TIMER_DEBUG
      if (timer[i].remaining < delta) {
        BX_PANIC(("timer_handler: remain < delta"));
        }
#endif
      timer[i].remaining -= delta;
      if (timer[i].remaining == 0) {
        timer[i].triggered = 1;
        // reset remaining period for triggered timer
        timer[i].remaining = timer[i].period;

        // if triggered timer is one-shot, deactive
        if (timer[i].continuous==0)
          timer[i].active = 0;
        }
      }
    }

  min = (Bit64u) -1; // max number in Bit64u range
  for (i=0; i < num_timers; i++) {
    if (timer[i].active && (timer[i].remaining < min))
      min = timer[i].remaining;
    }
  num_cpu_ticks_in_period = num_cpu_ticks_left = min;

  for (i=0; i < num_timers; i++) {
    // call requested timer function.  It may request a different
    // timer period or deactivate, all cases handled below
    if (timer[i].triggered) {
      timer[i].funct(timer[i].this_ptr);
      }
    }
}

  void
bx_pc_system_c::expire_ticks(void)
{
  unsigned i;
  Bit64u ticks_delta;

  ticks_delta = num_cpu_ticks_in_period - num_cpu_ticks_left;
  if (ticks_delta == 0) return; // no ticks occurred since
  for (i=0; i<num_timers; i++) {
    if (timer[i].active) {
#if BX_TIMER_DEBUG
      if (timer[i].remaining <= ticks_delta) {
for (unsigned j=0; j<num_timers; j++) {
  BX_INFO(("^^^timer[%u]", j));
  BX_INFO(("^^^remaining = %u, period = %u",
    timer[j].remaining, timer[j].period));
  }
        BX_PANIC(("expire_ticks: i=%u, remain(%u) <= delta(%u)",
          i, timer[i].remaining, (unsigned) ticks_delta));
        }
#endif
      timer[i].remaining -= ticks_delta; // must be >= 1 here
      }
    }

  // set new period to number of ticks left
  num_cpu_ticks_in_period = num_cpu_ticks_left;
}

  int
bx_pc_system_c::register_timer( void *this_ptr, void (*funct)(void *),
  Bit32u useconds, Boolean continuous, Boolean active)
{
  Bit64u instructions;

  if (num_timers >= BX_MAX_TIMERS) {
    BX_PANIC(("register_timer: too many registered timers."));
    }

  if (this_ptr == NULL)
    BX_PANIC(("register_timer: this_ptr is NULL"));
  if (funct == NULL)
    BX_PANIC(("register_timer: funct is NULL"));

  // account for ticks up to now
  expire_ticks();

  // convert useconds to number of instructions
  instructions = (Bit64u) (double(useconds) * m_ips);

  return register_timer_ticks(this_ptr, funct, instructions, continuous, active);
}

  int
bx_pc_system_c::register_timer_ticks(void* this_ptr, bx_timer_handler_t funct, Bit64u instructions, Boolean continuous, Boolean active)
{
  unsigned i;

  if (num_timers >= BX_MAX_TIMERS) {
    BX_PANIC(("register_timer: too many registered timers."));
    }

  if (this_ptr == NULL)
    BX_PANIC(("register_timer: this_ptr is NULL"));
  if (funct == NULL)
    BX_PANIC(("register_timer: funct is NULL"));

  i = num_timers;
  num_timers++;
  timer[i].period    = instructions;
  timer[i].remaining = instructions;
  timer[i].active    = active;
  timer[i].funct     = funct;
  timer[i].continuous = continuous;
  timer[i].this_ptr   = this_ptr;

  if (active) {
    if (num_cpu_ticks_in_period == 0) {
      // no active timers
      num_cpu_ticks_in_period = instructions;
      num_cpu_ticks_left      = instructions;
      }
    else {
      if (instructions < num_cpu_ticks_left) {
        num_cpu_ticks_in_period = instructions;
        num_cpu_ticks_left      = instructions;
        }
      }
    }

  // return timer id
  return(i);
}

  void
bx_pc_system_c::counter_timer_handler(void* this_ptr)
{
      UNUSED(this_ptr);

      bx_pc_system.counter++;
}

#if BX_DEBUGGER
  void
bx_pc_system_c::timebp_handler(void* this_ptr)
{
      BX_CPU(0)->break_point = BREAK_POINT_TIME;
      BX_DEBUG(( "Time breakpoint triggered" ));

      if (timebp_queue_size > 1) {
	    long long new_diff = timebp_queue[1] - bx_pc_system.time_ticks();
	    bx_pc_system.activate_timer_ticks(timebp_timer, new_diff, 1);
      }
      timebp_queue_size--;
      for (int i = 0; i < timebp_queue_size; i++)
	    timebp_queue[i] = timebp_queue[i+1];
}
#endif // BX_DEBUGGER

Bit64u
bx_pc_system_c::time_usec() {
  return (Bit64u) ( (time_ticks()) / m_ips );
  //return (Bit64u) ( double(time_ticks()) / m_ips );
}

  Bit64u
bx_pc_system_c::time_ticks()
{
      return (counter + 1) * COUNTER_INTERVAL 
	    - ticks_remaining(counter_timer_index) 
	    + ((Bit64u)num_cpu_ticks_in_period - (Bit64u)num_cpu_ticks_left);
}

  void
bx_pc_system_c::start_timers(void)
{
}

  void
bx_pc_system_c::activate_timer_ticks (unsigned timer_index, Bit64u instructions, Boolean continuous)
{
  if (timer_index >= num_timers)
    BX_PANIC(("activate_timer(): bad timer index given"));

  // set timer continuity to new value (1=continuous, 0=one-shot)
  timer[timer_index].continuous = continuous;

  timer[timer_index].active = 1;
  timer[timer_index].remaining = instructions;

  if (num_cpu_ticks_in_period == 0) {
    // no active timers
    num_cpu_ticks_in_period = instructions;
    num_cpu_ticks_left      = instructions;
    }
  else {
    if (instructions < num_cpu_ticks_left) {
      num_cpu_ticks_in_period = instructions;
      num_cpu_ticks_left      = instructions;
      }
    }
}

  void
bx_pc_system_c::activate_timer( unsigned timer_index,
    Bit32u useconds, Boolean continuous )
{
  Bit64u instructions;

  if (timer_index >= num_timers)
    BX_PANIC(("activate_timer(): bad timer index given"));

  // account for ticks up to now
  expire_ticks();

  // set timer continuity to new value (1=continuous, 0=one-shot)
  timer[timer_index].continuous = continuous;

  // if useconds = 0, use default stored in period field
  // else set new period from useconds
  if (useconds==0)
    instructions = timer[timer_index].period;
  else {
    // convert useconds to number of instructions
    instructions = (Bit64u) (double(useconds) * m_ips);
    timer[timer_index].period = instructions;
    }

  timer[timer_index].active = 1;
  timer[timer_index].remaining = instructions;

  if (num_cpu_ticks_in_period == 0) {
    // no active timers
    num_cpu_ticks_in_period = instructions;
    num_cpu_ticks_left      = instructions;
    }
  else {
    if (instructions < num_cpu_ticks_left) {
      num_cpu_ticks_in_period = instructions;
      num_cpu_ticks_left      = instructions;
      }
    }
}

  void
bx_pc_system_c::deactivate_timer( unsigned timer_index )
{
  if (timer_index >= num_timers)
    BX_PANIC(("deactivate_timer(): bad timer index given"));

  timer[timer_index].active = 0;
}
