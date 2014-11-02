///////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "pit.h"
#include "virt_timer.h"
#include "speaker.h"


#define LOG_THIS thePit->

bx_pit_c *thePit = NULL;

int CDECL libpit_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    thePit = new bx_pit_c();
    bx_devices.pluginPitDevice = thePit;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePit, BX_PLUGIN_PIT);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libpit_LTX_plugin_fini(void)
{
  delete thePit;
}

//Important constant #defines:
#define USEC_PER_SECOND (1000000)
//1.193181MHz Clock
#define TICKS_PER_SECOND (1193181)


// define a macro to convert floating point numbers into 64-bit integers.
// In MSVC++ you can convert a 64-bit float into a 64-bit signed integer,
// but it will not convert a 64-bit float into a 64-bit unsigned integer.
// This macro works around that.
#define F2I(x)  ((Bit64u)(Bit64s) (x))
#define I2F(x)  ((double)(Bit64s) (x))


//USEC_ALPHA is multiplier for the past.
//USEC_ALPHA_B is 1-USEC_ALPHA, or multiplier for the present.
#define USEC_ALPHA ((double)(.8))
#define USEC_ALPHA_B ((double)(((double)1)-USEC_ALPHA))
#define USEC_ALPHA2 ((double)(.5))
#define USEC_ALPHA2_B ((double)(((double)1)-USEC_ALPHA2))
#define ALPHA_LOWER(old,new) ((Bit64u)((old<new)?((USEC_ALPHA*(I2F(old)))+(USEC_ALPHA_B*(I2F(new)))):((USEC_ALPHA2*(I2F(old)))+(USEC_ALPHA2_B*(I2F(new))))))


//PIT tick to usec conversion functions:
//Direct conversions:
#define TICKS_TO_USEC(a) (((a)*USEC_PER_SECOND)/TICKS_PER_SECOND)
#define USEC_TO_TICKS(a) (((a)*TICKS_PER_SECOND)/USEC_PER_SECOND)

bx_pit_c::bx_pit_c()
{
  put("PIT");

  /* 8254 PIT (Programmable Interval Timer) */

  s.timer_handle[1] = BX_NULL_TIMER_HANDLE;
  s.timer_handle[2] = BX_NULL_TIMER_HANDLE;
  s.timer_handle[0] = BX_NULL_TIMER_HANDLE;
}

bx_pit_c::~bx_pit_c()
{
  SIM->get_bochs_root()->remove("pit");
  BX_DEBUG(("Exit"));
}

void bx_pit_c::init(void)
{
  int clock_mode = SIM->get_param_enum(BXPN_CLOCK_SYNC)->get();
  BX_PIT_THIS is_realtime = (clock_mode == BX_CLOCK_SYNC_REALTIME) ||
      (clock_mode == BX_CLOCK_SYNC_BOTH);

  DEV_register_irq(0, "8254 PIT");
  DEV_register_ioread_handler(this, read_handler, 0x0040, "8254 PIT", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0041, "8254 PIT", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0042, "8254 PIT", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0043, "8254 PIT", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0061, "8254 PIT", 1);

  DEV_register_iowrite_handler(this, write_handler, 0x0040, "8254 PIT", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0041, "8254 PIT", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0042, "8254 PIT", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0043, "8254 PIT", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0061, "8254 PIT", 1);

  BX_DEBUG(("starting init"));

  BX_PIT_THIS s.speaker_data_on = 0;
  BX_PIT_THIS s.speaker_active = 0;

  BX_PIT_THIS s.timer.init();
  BX_PIT_THIS s.timer.set_OUT_handler(0, irq_handler);

  Bit64u my_time_usec = bx_virt_timer.time_usec(BX_PIT_THIS is_realtime);

  if (BX_PIT_THIS s.timer_handle[0] == BX_NULL_TIMER_HANDLE) {
    BX_PIT_THIS s.timer_handle[0] = bx_virt_timer.register_timer(this, timer_handler,
                                                                 (unsigned) 100 , 1, 1, BX_PIT_THIS is_realtime, "pit");
    if (BX_PIT_THIS is_realtime) {
      BX_INFO(("PIT using realtime synchronisation method"));
    }
  }
  BX_DEBUG(("RESETting timer."));
  bx_virt_timer.deactivate_timer(BX_PIT_THIS s.timer_handle[0]);
  BX_DEBUG(("deactivated timer."));
  if (BX_PIT_THIS s.timer.get_next_event_time()) {
    bx_virt_timer.activate_timer(BX_PIT_THIS s.timer_handle[0],
                                 (Bit32u)BX_MAX(1,TICKS_TO_USEC(BX_PIT_THIS s.timer.get_next_event_time())),
                                 0);
    BX_DEBUG(("activated timer."));
  }
  BX_PIT_THIS s.last_next_event_time = BX_PIT_THIS s.timer.get_next_event_time();
  BX_PIT_THIS s.last_usec = my_time_usec;

  BX_PIT_THIS s.total_ticks = 0;
  BX_PIT_THIS s.total_usec = 0;

  BX_DEBUG(("finished init"));

  BX_DEBUG(("s.last_usec="FMT_LL"d",BX_PIT_THIS s.last_usec));
  BX_DEBUG(("s.timer_id=%d",BX_PIT_THIS s.timer_handle[0]));
  BX_DEBUG(("s.timer.get_next_event_time=%d", BX_PIT_THIS s.timer.get_next_event_time()));
  BX_DEBUG(("s.last_next_event_time=%d", BX_PIT_THIS s.last_next_event_time));

#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("pit", this);
#endif
}

void bx_pit_c::reset(unsigned type)
{
  BX_PIT_THIS s.timer.reset(type);
}

void bx_pit_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pit", "8254 PIT State");
  new bx_shadow_bool_c(list, "speaker_data_on", &BX_PIT_THIS s.speaker_data_on);
  new bx_shadow_bool_c(list, "speaker_active", &BX_PIT_THIS s.speaker_active);
  new bx_shadow_num_c(list, "last_usec", &BX_PIT_THIS s.last_usec);
  new bx_shadow_num_c(list, "last_next_event_time", &BX_PIT_THIS s.last_next_event_time);
  new bx_shadow_num_c(list, "total_ticks", &BX_PIT_THIS s.total_ticks);
  new bx_shadow_num_c(list, "total_usec", &BX_PIT_THIS s.total_usec);
  bx_list_c *counter = new bx_list_c(list, "counter");
  BX_PIT_THIS s.timer.register_state(counter);
}

void bx_pit_c::after_restore_state(void)
{
  if (BX_PIT_THIS s.speaker_active) {
    Bit32u value32 = BX_PIT_THIS get_timer(2);
    if (value32 == 0) value32 = 0x10000;
    DEV_speaker_beep_on((float)(1193180.0 / value32));
  }
}

void bx_pit_c::timer_handler(void *this_ptr)
{
  bx_pit_c * class_ptr = (bx_pit_c *) this_ptr;
  class_ptr->handle_timer();
}

void bx_pit_c::handle_timer()
{
  Bit64u my_time_usec = bx_virt_timer.time_usec(BX_PIT_THIS is_realtime);
  Bit64u time_passed = my_time_usec-BX_PIT_THIS s.last_usec;
  Bit32u time_passed32 = (Bit32u)time_passed;

  BX_DEBUG(("entering timer handler"));

  if(time_passed32) {
    periodic(time_passed32);
  }
  BX_PIT_THIS s.last_usec = BX_PIT_THIS s.last_usec + time_passed;
  if (time_passed || (BX_PIT_THIS s.last_next_event_time != BX_PIT_THIS s.timer.get_next_event_time())) {
    BX_DEBUG(("RESETting timer"));
    bx_virt_timer.deactivate_timer(BX_PIT_THIS s.timer_handle[0]);
    BX_DEBUG(("deactivated timer"));
    if(BX_PIT_THIS s.timer.get_next_event_time()) {
      bx_virt_timer.activate_timer(BX_PIT_THIS s.timer_handle[0],
                                   (Bit32u)BX_MAX(1,TICKS_TO_USEC(BX_PIT_THIS s.timer.get_next_event_time())),
                                   0);
      BX_DEBUG(("activated timer"));
    }
    BX_PIT_THIS s.last_next_event_time = BX_PIT_THIS s.timer.get_next_event_time();
  }
  BX_DEBUG(("s.last_usec="FMT_LL"d", BX_PIT_THIS s.last_usec));
  BX_DEBUG(("s.timer_id=%d", BX_PIT_THIS s.timer_handle[0]));
  BX_DEBUG(("s.timer.get_next_event_time=%x", BX_PIT_THIS s.timer.get_next_event_time()));
  BX_DEBUG(("s.last_next_event_time=%d", BX_PIT_THIS s.last_next_event_time));
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_pit_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit_c *class_ptr = (bx_pit_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_pit_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PIT_SMF
  bx_bool refresh_clock_div2;
  Bit8u value = 0;

  handle_timer();

  switch (address) {

    case 0x40: /* timer 0 - system ticks */
      value = BX_PIT_THIS s.timer.read(0);
      break;
    case 0x41: /* timer 1 read */
      value = BX_PIT_THIS s.timer.read(1);
      break;
    case 0x42: /* timer 2 read */
      value = BX_PIT_THIS s.timer.read(2);
      break;
    case 0x43: /* timer 1 read */
      value = BX_PIT_THIS s.timer.read(3);
      break;

    case 0x61:
      /* AT, port 61h */
      refresh_clock_div2 = (bx_bool)((bx_virt_timer.time_usec(BX_PIT_THIS is_realtime) / 15) & 1);
      value = (BX_PIT_THIS s.timer.read_OUT(2)  << 5) |
              (refresh_clock_div2               << 4) |
              (BX_PIT_THIS s.speaker_data_on    << 1) |
              (BX_PIT_THIS s.timer.read_GATE(2) ? 1 : 0);
      break;

    default:
      BX_PANIC(("unsupported io read from port 0x%04x", address));
  }

  BX_DEBUG(("read from port 0x%04x, value = 0x%02x", address, value));
  return value;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_pit_c::write_handler(void *this_ptr, Bit32u address, Bit32u dvalue, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit_c *class_ptr = (bx_pit_c *) this_ptr;
  class_ptr->write(address, dvalue, io_len);
}

void bx_pit_c::write(Bit32u address, Bit32u dvalue, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PIT_SMF
  Bit8u   value;
  Bit64u my_time_usec = bx_virt_timer.time_usec(BX_PIT_THIS is_realtime);
  Bit64u time_passed = my_time_usec-BX_PIT_THIS s.last_usec;
  Bit32u value32, time_passed32 = (Bit32u)time_passed;
  bx_bool new_speaker_active;

  if (time_passed32) {
    periodic(time_passed32);
  }
  BX_PIT_THIS s.last_usec = BX_PIT_THIS s.last_usec + time_passed;

  value = (Bit8u) dvalue;

  BX_DEBUG(("write to port 0x%04x, value = 0x%02x", address, value));

  switch (address) {
    case 0x40: /* timer 0: write count register */
      BX_PIT_THIS s.timer.write(0, value);
      break;

    case 0x41: /* timer 1: write count register */
      BX_PIT_THIS s.timer.write(1, value);
      break;

    case 0x42: /* timer 2: write count register */
      BX_PIT_THIS s.timer.write(2, value);
      if (BX_PIT_THIS s.speaker_active && BX_PIT_THIS new_timer_count(2)) {
        value32 = BX_PIT_THIS get_timer(2);
        if (value32 == 0) value32 = 0x10000;
        DEV_speaker_beep_on((float)(1193180.0 / value32));
      }
      break;

    case 0x43: /* timer 0-2 mode control */
      BX_PIT_THIS s.timer.write(3, value);
      break;

    case 0x61:
      BX_PIT_THIS s.timer.set_GATE(2, value & 0x01);
      BX_PIT_THIS s.speaker_data_on = (value >> 1) & 0x01;
      new_speaker_active = ((value & 3) == 3);
      if (BX_PIT_THIS s.speaker_active != new_speaker_active) {
        if (new_speaker_active) {
          value32 = BX_PIT_THIS get_timer(2);
          if (value32 == 0) value32 = 0x10000;
          DEV_speaker_beep_on((float)(1193180.0 / value32));
        } else {
          DEV_speaker_beep_off();
        }
        BX_PIT_THIS s.speaker_active = new_speaker_active;
      }
      break;

    default:
      BX_PANIC(("unsupported io write to port 0x%04x = 0x%02x", address, value));
  }

  if (time_passed || (BX_PIT_THIS s.last_next_event_time != BX_PIT_THIS s.timer.get_next_event_time())) {
    BX_DEBUG(("RESETting timer"));
    bx_virt_timer.deactivate_timer(BX_PIT_THIS s.timer_handle[0]);
    BX_DEBUG(("deactivated timer"));
    if(BX_PIT_THIS s.timer.get_next_event_time()) {
      bx_virt_timer.activate_timer(BX_PIT_THIS s.timer_handle[0],
                                   (Bit32u)BX_MAX(1,TICKS_TO_USEC(BX_PIT_THIS s.timer.get_next_event_time())),
                                   0);
      BX_DEBUG(("activated timer"));
    }
    BX_PIT_THIS s.last_next_event_time = BX_PIT_THIS s.timer.get_next_event_time();
  }
  BX_DEBUG(("s.last_usec="FMT_LL"d", BX_PIT_THIS s.last_usec));
  BX_DEBUG(("s.timer_id=%d", BX_PIT_THIS s.timer_handle[0]));
  BX_DEBUG(("s.timer.get_next_event_time=%x", BX_PIT_THIS s.timer.get_next_event_time()));
  BX_DEBUG(("s.last_next_event_time=%d", BX_PIT_THIS s.last_next_event_time));

}

bx_bool bx_pit_c::periodic(Bit32u usec_delta)
{
  Bit32u ticks_delta = 0;

  BX_PIT_THIS s.total_usec += usec_delta;
  ticks_delta = (Bit32u)((USEC_TO_TICKS((Bit64u)(BX_PIT_THIS s.total_usec)))-BX_PIT_THIS s.total_ticks);
  BX_PIT_THIS s.total_ticks += ticks_delta;

  while ((BX_PIT_THIS s.total_ticks >= TICKS_PER_SECOND) && (BX_PIT_THIS s.total_usec >= USEC_PER_SECOND)) {
    BX_PIT_THIS s.total_ticks -= TICKS_PER_SECOND;
    BX_PIT_THIS s.total_usec  -= USEC_PER_SECOND;
  }

  while(ticks_delta>0) {
    Bit32u maxchange = BX_PIT_THIS s.timer.get_next_event_time();
    Bit32u timedelta = maxchange;
    if((maxchange == 0) || (maxchange>ticks_delta)) {
      timedelta = ticks_delta;
    }
    BX_PIT_THIS s.timer.clock_all(timedelta);
    ticks_delta -= timedelta;
  }

  return 0;
}

void bx_pit_c::irq_handler(bx_bool value)
{
  if (value == 1) {
    DEV_pic_raise_irq(0);
  } else {
    DEV_pic_lower_irq(0);
  }
}

Bit16u bx_pit_c::get_timer(int Timer)
{
  return BX_PIT_THIS s.timer.get_inlatch(Timer);
}

Bit16u bx_pit_c::new_timer_count(int Timer)
{
  return BX_PIT_THIS s.timer.new_count_ready(Timer);
}

#if BX_DEBUGGER
void bx_pit_c::debug_dump(int argc, char **argv)
{
  Bit32u value;
  int counter = -1;

  dbg_printf("82C54 PIT\n\n");
  dbg_printf("GATE #2 = %d\n", BX_PIT_THIS s.timer.read_GATE(2));
  dbg_printf("Speaker = %d\n\n", BX_PIT_THIS s.speaker_data_on);
  if (argc == 0) {
    for (int i = 0; i < 3; i++) {
      value = BX_PIT_THIS get_timer(i);
      if (value == 0) value = 0x10000;
      dbg_printf("counter #%d: freq=%.3f, OUT=%d\n", i, (float)(1193180.0 / value),
                 BX_PIT_THIS s.timer.read_OUT(i));
    }
    dbg_printf("\nSupported options:\n");
    dbg_printf("info device 'pit' 'counter=N' - show status of counter N\n");
  } else {
    for (int arg = 0; arg < argc; arg++) {
      if (!strncmp(argv[arg], "counter=", 8) && isdigit(argv[arg][8])) {
        counter = atoi(&argv[arg][8]);
      } else {
        dbg_printf("\nUnknown option: '%s'\n", argv[arg]);
        return;
      }
    }
    if ((counter >= 0) && (counter < 3)) {
      value = BX_PIT_THIS get_timer(counter);
      if (value == 0) value = 0x10000;
      dbg_printf("counter #%d: freq=%.3f\n", counter, (float)(1193180.0 / value));
      BX_PIT_THIS s.timer.print_cnum(counter);
    } else {
      dbg_printf("\nInvalid PIT counter number: %d\n", counter);
    }
  }
}
#endif
