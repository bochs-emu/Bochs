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
#define LOG_THIS  bx_g2h.

bx_g2h_c bx_g2h;

bx_g2h_c::bx_g2h_c(void)
{
  put("G2H");
  settype(G2HLOG);
  unsigned i;

  for (i=0; i<BX_MAX_G2H_CHANNELS; i++) {
    s.callback[i].f = NULL;
    s.callback[i].used = 0;
    }
}

bx_g2h_c::~bx_g2h_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}

  void
bx_g2h_c::init(bx_devices_c *d)
{
  BX_DEBUG(("Init $Id: guest2host.cc,v 1.8 2001-06-27 20:27:49 fries Exp $"));
  // Reserve a dword port for this interface
  for (Bit32u addr=BX_G2H_PORT; addr<=(BX_G2H_PORT+3); addr++) {
    d->register_io_read_handler(&bx_g2h,
      inp_handler, addr, "g2h");
    d->register_io_write_handler(&bx_g2h,
      outp_handler, addr, "g2h");
    }
  memset(&bx_g2h.s, 0, sizeof(bx_g2h.s));
}


  unsigned
bx_g2h_c::aquire_channel(bx_g2h_callback_t f)
{
  unsigned i;

  for (i=0; i<BX_MAX_G2H_CHANNELS; i++) {
    if (bx_g2h.s.callback[i].used==0) {
      bx_g2h.s.callback[i].f = f;
      bx_g2h.s.callback[i].used = 1;
      return(i);
      }
    }

  BX_INFO(("g2h: attempt to aquire channel: maxed out");
  return(BX_G2H_ERROR); // No more free channels
}

  unsigned
bx_g2h_c::deaquire_channel(unsigned channel)
{
  if ( (channel >= BX_MAX_G2H_CHANNELS) ||
       (bx_g2h.s.callback[channel].used==0) ) {
    BX_PANIC(("g2h: attempt to deaquire channel %u: not aquired",
      channel));
    }
  bx_g2h.s.callback[channel].used = 0;
  bx_g2h.s.callback[channel].f = NULL;
  return(0);
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_g2h_c::inp_handler(void *this_ptr, Bit32u addr, unsigned io_len)
{
  UNUSED(this_ptr);

  if (addr != BX_G2H_PORT)
    BX_PANIC(("g2h: IO read not aligned on dword boundary."));
  if (io_len != 4)
    BX_PANIC(("g2h: IO read not dword."));

  BX_PANIC(("g2h: IO read not complete."));
  return(0);
}



  void
bx_g2h_c::outp_handler(void *this_ptr, Bit32u addr,
                               Bit32u val32, unsigned io_len)
{
  UNUSED(this_ptr);

  if (addr != BX_G2H_PORT)
    BX_PANIC(("g2h: IO write not aligned on dword boundary."));
  if (io_len != 4)
    BX_PANIC(("g2h: IO write not dword."));

  if ( (bx_g2h.s.packet_count==0) && (val32!=BX_G2H_MAGIC) ) {
    BX_INFO(("g2h: IO W: Not magic header.");
    return;
    }
  bx_g2h.s.guest_packet[bx_g2h.s.packet_count++] = val32;
  if (bx_g2h.s.packet_count >= BX_G2H_PACKET_SIZE) {
    unsigned channel;

    // Full packet received from guest.  Pass on to the host code.
    channel = bx_g2h.s.guest_packet[1];
    if (channel >= BX_MAX_G2H_CHANNELS) {
      BX_PANIC(("g2h: channel (%u) out of bounds", channel));
      }
    if (bx_g2h.s.callback[channel].used==0) {
      BX_PANIC(("g2h: channel (%u) not active", channel));
      }
    bx_g2h.s.callback[channel].f(&bx_g2h.s.guest_packet);
    bx_g2h.s.packet_count = 0; // Ready for next packet
    }
}
