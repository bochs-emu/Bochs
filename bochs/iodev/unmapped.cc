/////////////////////////////////////////////////////////////////////////
// $Id: unmapped.cc,v 1.17 2002-07-30 08:48:03 cbothamy Exp $
/////////////////////////////////////////////////////////////////////////
//
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
#define LOG_THIS bx_unmapped.


bx_unmapped_c bx_unmapped;
#if BX_USE_UM_SMF
#define this (&bx_unmapped)
#endif

bx_unmapped_c::bx_unmapped_c(void)
{
  put("UNMP");
  settype(UNMAPLOG);
  s.port80 = 0x00;
  s.port8e = 0x00;
  s.shutdown = 0;
}

bx_unmapped_c::~bx_unmapped_c(void)
{
  // Nothing yet
}

  void
bx_unmapped_c::init(bx_devices_c *d)
{
  BX_UM_THIS devices = d;

  for (Bit32u addr=0; addr<0x10000; addr++) {
    BX_UM_THIS devices->register_io_read_handler(this, read_handler,
                                      addr, "Unmapped");
    BX_UM_THIS devices->register_io_write_handler(this, write_handler,
                                      addr, "Unmapped");
    }
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_unmapped_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_UM_SMF
  bx_unmapped_c *class_ptr = (bx_unmapped_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}

  Bit32u
bx_unmapped_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_UM_SMF
  UNUSED(io_len);

  Bit32u retval;

  // This function gets called for access to any IO ports which
  // are not mapped to any device handler.  Reads return 0

  if (address >= 0x02e0 && address <= 0x02ef) {
	retval = 0;
	goto return_from_read;
  }

  switch (address) {
    case 0x80:
	  retval = BX_UM_THIS s.port80;
	  break;
    case 0x8e:
	  retval = BX_UM_THIS s.port8e;
	  break;
#if BX_PORT_E9_HACK
    // Unused port on ISA - this can be used by the emulated code
    // to detect it is running inside Bochs and that the debugging
    // features are available (write 0xFF or something on unused
    // port 0x80, then read from 0xe9, if value is 0xe9, debug
    // output is available) (see write() for that) -- Andreas and Emmanuel
    case 0xe9:
	  retval = 0xe9;
	  break;
#endif
    case 0x03df:
	  retval = 0xffffffff;
      BX_DEBUG(("unsupported IO read from port %04x (CGA)", address));
      break;
    case 0x023a:
    case 0x02f8: /* UART */
    case 0x02f9: /* UART */
    case 0x02fb: /* UART */
    case 0x02fc: /* UART */
    case 0x02fd: /* UART */
    case 0x02ea:
    case 0x02eb:
    case 0x03e8:
    case 0x03e9:
    case 0x03ea:
    case 0x03eb:
    case 0x03ec:
    case 0x03ed:
    case 0x03f8: /* UART */
    case 0x03f9: /* UART */
    case 0x03fb: /* UART */
    case 0x03fc: /* UART */
    case 0x03fd: /* UART */
    case 0x17c6:
	  retval = 0xffffffff;
      BX_DEBUG(("unsupported IO read from port %04x", address));
      break;
    default:
	  retval = 0xffffffff;
    }

  return_from_read:
  if (bx_dbg.unsupported_io)
	  switch (io_len) {
	  case 1:
		  retval = (Bit8u)retval;
		  BX_DEBUG(("unmapped: 8-bit read from %04x = %02x", address, retval));
		  break;
	  case 2:
		  retval = (Bit16u)retval;
		  BX_DEBUG(("unmapped: 16-bit read from %04x = %04x", address, retval));
		  break;
	  case 4:
		  BX_DEBUG(("unmapped: 32-bit read from %04x = %08x", address, retval));
		  break;
	  default:
		  BX_DEBUG(("unmapped: ??-bit read from %04x = %x", address, retval));
	  }
  return retval;
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_unmapped_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_UM_SMF
  bx_unmapped_c *class_ptr = (bx_unmapped_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_unmapped_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_UM_SMF
  UNUSED(io_len);


  // This function gets called for access to any IO ports which
  // are not mapped to any device handler.  Writes to an unmapped
  // IO port are ignored.

// ???


  if (address >= 0x02e0 && address <= 0x02ef)
	goto return_from_write;

  switch (address) {
    case 0x80: // diagnostic test port to display progress of POST
      //BX_DEBUG(("Diagnostic port 80h: write = %02xh", (unsigned) value));
      BX_UM_THIS s.port80 = value;
      break;

    case 0x8e: // ???
      BX_UM_THIS s.port8e = value;
      break;

#if BX_PORT_E9_HACK
    // This port doesn't exist on normal ISA architecture. However,
    // we define a convention here, to display on the console of the
    // system running Bochs, anything that is written to it. The
    // idea is to provide debug output very early when writing
    // BIOS or OS code for example, without having to bother with
    // properly setting up a serial port or anything.
    //
    // Idea by Andreas Beck (andreas.beck@ggi-project.org)

    case 0xe9:
      putchar(value);
      fflush(stdout);
      break;
#endif
    case 0xed: // Dummy port used as I/O delay
	  break;
    case 0xee: // ???
	  break;

    case 0x2f2:
    case 0x2f3:
    case 0x2f4:
    case 0x2f5:
    case 0x2f6:
    case 0x2f7:
    case 0x3e8:
    case 0x3e9:
    case 0x3eb:
    case 0x3ec:
    case 0x3ed:
	    // BX_DEBUG(("unsupported IO write to port %04x of %02x",
	    // address, value));
      break;
    
    case 0x8900: // Shutdown port, could be moved in a PM device
                 // or a host <-> guest communication device 
      switch (value) {
        case 'S': if (BX_UM_THIS s.shutdown == 0) BX_UM_THIS s.shutdown = 1; break;
        case 'h': if (BX_UM_THIS s.shutdown == 1) BX_UM_THIS s.shutdown = 2; break;
        case 'u': if (BX_UM_THIS s.shutdown == 2) BX_UM_THIS s.shutdown = 3; break;
        case 't': if (BX_UM_THIS s.shutdown == 3) BX_UM_THIS s.shutdown = 4; break;
        case 'd': if (BX_UM_THIS s.shutdown == 4) BX_UM_THIS s.shutdown = 5; break;
        case 'o': if (BX_UM_THIS s.shutdown == 5) BX_UM_THIS s.shutdown = 6; break;
        case 'w': if (BX_UM_THIS s.shutdown == 6) BX_UM_THIS s.shutdown = 7; break;
        case 'n': if (BX_UM_THIS s.shutdown == 7) BX_UM_THIS s.shutdown = 8; break;
	default :  BX_UM_THIS s.shutdown = 0; break;
        }
      if (BX_UM_THIS s.shutdown == 8) {
        BX_INFO(("Shutdown port: shutdown requested"));
        BX_CPU(0)->kill_bochs_request = 2;
        }
      break;

    case 0xfedc:
      bx_dbg.debugger = (value > 0);
		BX_DEBUG(( "DEBUGGER = %u", (unsigned) bx_dbg.debugger));
      break;

    default:
	    break;
    }
  return_from_write:
  if (bx_dbg.unsupported_io)
	  switch (io_len) {
	  case 1:
		  BX_INFO(("unmapped: 8-bit write to %04x = %02x", address, value));
		  break;
	  case 2:
		  BX_INFO(("unmapped: 16-bit write to %04x = %04x", address, value));
		  break;
	  case 4:
		  BX_INFO(("unmapped: 32-bit write to %04x = %08x", address, value));
		  break;
	  default:
		  BX_INFO(("unmapped: ??-bit write to %04x = %x", address, value));
		  break;
	  }
}
