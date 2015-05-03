/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003-2014  The Bochs Project
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
// Standard PC gameport
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "gameport.h"

#ifdef __linux__

#include <linux/joystick.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#elif defined(WIN32)

#include <mmsystem.h>
#ifndef JOY_BUTTON1
#define JOY_BUTTON1 1
#define JOY_BUTTON2 2
UINT STDCALL joyGetPos(UINT, LPJOYINFO);
#endif

#define JOYSTICKID1 0

#endif

#define LOG_THIS theGameport->

bx_gameport_c *theGameport = NULL;

int CDECL libgameport_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theGameport = new bx_gameport_c();
  bx_devices.pluginGameport = theGameport;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theGameport, BX_PLUGIN_GAMEPORT);
  return(0); // Success
}

void CDECL libgameport_LTX_plugin_fini(void)
{
  bx_devices.pluginGameport = &bx_devices.stubGameport;
  delete theGameport;
}

bx_gameport_c::bx_gameport_c()
{
  put("gameport", "GAME");
  joyfd = -1;
}

bx_gameport_c::~bx_gameport_c()
{
  if (joyfd >= 0) close(joyfd);
  SIM->get_bochs_root()->remove("gameport");
  BX_DEBUG(("Exit"));
}

void bx_gameport_c::init(void)
{
  // Allocate the gameport IO address range 0x200..0x207
  for (unsigned addr=0x200; addr<0x208; addr++) {
    DEV_register_ioread_handler(this, read_handler, addr, "Gameport", 1);
    DEV_register_iowrite_handler(this, write_handler, addr, "Gameport", 1);
  }

  // always enabled unless controlled by external device
  BX_GAMEPORT_THIS enabled = 1;
  BX_GAMEPORT_THIS port = 0xf0;
  BX_GAMEPORT_THIS write_usec = 0;
  BX_GAMEPORT_THIS timer_x = 0;
  BX_GAMEPORT_THIS timer_y = 0;

#ifdef __linux__
  BX_GAMEPORT_THIS joyfd = open("/dev/input/js0", O_RDONLY);
  if (BX_GAMEPORT_THIS joyfd >= 0) {
    for (unsigned i=0; i<4; i++) poll_joydev();
  }
#elif defined(WIN32)
  JOYINFO joypos;
  if (joyGetPos(JOYSTICKID1, &joypos) == JOYERR_NOERROR) {
    BX_GAMEPORT_THIS joyfd = 1;
  } else {
    BX_GAMEPORT_THIS joyfd = -1;
  }
#else
  BX_GAMEPORT_THIS joyfd = -1;
#endif
}

void bx_gameport_c::reset(unsigned type)
{
  // nothing for now
}

void bx_gameport_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "gameport", "Gameport State");
  BXRS_PARAM_BOOL(list, enabled, BX_GAMEPORT_THIS enabled);
  BXRS_HEX_PARAM_FIELD(list, port, BX_GAMEPORT_THIS port);
  BXRS_DEC_PARAM_FIELD(list, delay_x, BX_GAMEPORT_THIS delay_x);
  BXRS_DEC_PARAM_FIELD(list, delay_y, BX_GAMEPORT_THIS delay_y);
  BXRS_PARAM_BOOL(list, timer_x, BX_GAMEPORT_THIS timer_x);
  BXRS_PARAM_BOOL(list, timer_y, BX_GAMEPORT_THIS timer_y);
  BXRS_DEC_PARAM_FIELD(list, write_usec, BX_GAMEPORT_THIS write_usec);
}

void bx_gameport_c::poll_joydev(void)
{
#ifdef __linux__
  struct js_event e;
  fd_set joyfds;
  struct timeval tv;

  memset(&tv, 0, sizeof(tv));
  FD_ZERO(&joyfds);
  FD_SET(BX_GAMEPORT_THIS joyfd, &joyfds);
  e.type = 0;
  if (select(BX_GAMEPORT_THIS joyfd+1, &joyfds, NULL, NULL, &tv)) {
    read(BX_GAMEPORT_THIS joyfd, &e, sizeof(struct js_event));
    if (e.type & JS_EVENT_BUTTON) {
      if (e.value == 1) {
        BX_GAMEPORT_THIS port &= ~(0x10 << e.number);
      } else {
        BX_GAMEPORT_THIS port |= (0x10 << e.number);
      }
    }
    if (e.type & JS_EVENT_AXIS) {
      if (e.number == 0) {
        BX_GAMEPORT_THIS delay_x = 25 + ((e.value + 0x8000) / 60);
      }
      if (e.number == 1) {
        BX_GAMEPORT_THIS delay_y = 25 + ((e.value + 0x8000) / 62);
      }
    }
  }
#elif defined(WIN32)
  JOYINFO joypos;
  if (joyGetPos(JOYSTICKID1, &joypos) == JOYERR_NOERROR) {
    if (joypos.wButtons & JOY_BUTTON1) {
      BX_GAMEPORT_THIS port &= ~0x10;
    } else {
      BX_GAMEPORT_THIS port |= 0x10;
    }
    if (joypos.wButtons & JOY_BUTTON2) {
      BX_GAMEPORT_THIS port &= ~0x20;
    } else {
      BX_GAMEPORT_THIS port |= 0x20;
    }
    BX_GAMEPORT_THIS delay_x = 25 + (joypos.wXpos / 60);
    BX_GAMEPORT_THIS delay_y = 25 + (joypos.wYpos / 60);
  }
#endif
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_gameport_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_GAMEPORT_SMF
  bx_gameport_c *class_ptr = (bx_gameport_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_gameport_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_GAMEPORT_SMF
  Bit64u usec;

  if (BX_GAMEPORT_THIS enabled) {
    if (BX_GAMEPORT_THIS joyfd >= 0) {
      poll_joydev();
      usec = bx_pc_system.time_usec();
      if (BX_GAMEPORT_THIS timer_x) {
        if ((usec - BX_GAMEPORT_THIS write_usec) >= BX_GAMEPORT_THIS delay_x) {
          BX_GAMEPORT_THIS port &= 0xfe;
          BX_GAMEPORT_THIS timer_x = 0;
        }
      }
      if (BX_GAMEPORT_THIS timer_y) {
        if ((usec - BX_GAMEPORT_THIS write_usec) >= BX_GAMEPORT_THIS delay_y) {
          BX_GAMEPORT_THIS port &= 0xfd;
          BX_GAMEPORT_THIS timer_y = 0;
        }
      }
    } else {
      BX_DEBUG(("read: joystick not present"));
    }
    return BX_GAMEPORT_THIS port;
  } else {
    BX_DEBUG(("read: gameport disabled"));
    return 0xff;
  }
}


// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_gameport_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_GAMEPORT_SMF
  bx_gameport_c *class_ptr = (bx_gameport_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_gameport_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_GAMEPORT_SMF

  if (BX_GAMEPORT_THIS enabled) {
    BX_GAMEPORT_THIS write_usec = bx_pc_system.time_usec();
    BX_GAMEPORT_THIS timer_x = 1;
    BX_GAMEPORT_THIS timer_y = 1;
    BX_GAMEPORT_THIS port |= 0x0f;
  } else {
    BX_DEBUG(("write: gameport disabled"));
  }
}
