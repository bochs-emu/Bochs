/////////////////////////////////////////////////////////////////////////
// $Id: biosdev.cc,v 1.1.2.1 2002-04-10 05:55:27 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

bx_biosdev_c bx_biosdev;

#if BX_USE_BIOS_SMF
#define this (&bx_biosdev)
#endif

logfunctions  *bioslog;
logfunctions  *vgabioslog;

bx_biosdev_c::bx_biosdev_c(void)
{
  bioslog = new logfunctions();
  bioslog->put("BIOS");
  bioslog->settype(BIOSLOG);
  s.bios_message_i = 0;

  vgabioslog = new logfunctions();
  vgabioslog->put("VBIOS");
  vgabioslog->settype(BIOSLOG);
  s.vgabios_message_i = 0;
}

bx_biosdev_c::~bx_biosdev_c(void)
{
    if ( bioslog != NULL )
    {
        delete bioslog;
        bioslog = NULL;
    }

    if ( vgabioslog != NULL )
    {
        delete vgabioslog;
        vgabioslog = NULL;
    }
}

  void
bx_biosdev_c::init(bx_devices_c *d)
{
  BX_BIOS_THIS devices = d;

  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0400, "Bios Panic Port 1");
  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0401, "Bios Panic Port 2");
  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0xfff0, "Bios Info Port");

  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0501, "VGABios Panic Port 1");
  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0502, "VGABios Panic Port 2");
  BX_BIOS_THIS devices->register_io_write_handler(this, write_handler,
                                      0x0500, "VGABios Info Port");
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_biosdev_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_BIOS_SMF
  bx_biosdev_c *class_ptr = (bx_biosdev_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_biosdev_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_BIOS_SMF
  UNUSED(io_len);


  switch (address) {
    // 0x400-0x401 are used as panic ports for the rombios
    case 0x0401:
      if (BX_BIOS_THIS s.bios_message_i > 0) {
	// if there are bits of message in the buffer, print them as the
	// panic message.  Otherwise fall into the next case.
	if (BX_BIOS_THIS s.bios_message_i >= BX_BIOS_MESSAGE_SIZE)
	  BX_BIOS_THIS s.bios_message_i = BX_BIOS_MESSAGE_SIZE-1;
        BX_BIOS_THIS s.bios_message[ BX_BIOS_THIS s.bios_message_i] = 0;
	BX_BIOS_THIS s.bios_message_i = 0;
        bioslog->panic("%s", BX_BIOS_THIS s.bios_message);
	break;
      }
    case 0x0400:
      bioslog->panic("BIOS panic at rombios.c, line %d", value);
      break;

    // 0xfff0 is used as the message port for the rombios
    case 0xfff0:
      BX_BIOS_THIS s.bios_message[BX_BIOS_THIS s.bios_message_i] =
        (Bit8u) value;
      BX_BIOS_THIS s.bios_message_i ++;
      if ( BX_BIOS_THIS s.bios_message_i >= BX_BIOS_MESSAGE_SIZE ) {
        BX_BIOS_THIS s.bios_message[ BX_BIOS_MESSAGE_SIZE - 1] = 0;
        BX_BIOS_THIS s.bios_message_i = 0;
        bioslog->info("%s", BX_BIOS_THIS s.bios_message);
        }
      else if ((value & 0xff) == '\n') {
        BX_BIOS_THIS s.bios_message[ BX_BIOS_THIS s.bios_message_i - 1 ] = 0;
        BX_BIOS_THIS s.bios_message_i = 0;
        bioslog->info("%s", BX_BIOS_THIS s.bios_message);
        }
      break;

    // 0x501-0x502 are used as panic ports for the vgabios
    case 0x0502:
      if (BX_BIOS_THIS s.vgabios_message_i > 0) {
	// if there are bits of message in the buffer, print them as the
	// panic message.  Otherwise fall into the next case.
	if (BX_BIOS_THIS s.vgabios_message_i >= BX_BIOS_MESSAGE_SIZE)
	  BX_BIOS_THIS s.vgabios_message_i = BX_BIOS_MESSAGE_SIZE-1;
        BX_BIOS_THIS s.vgabios_message[ BX_BIOS_THIS s.vgabios_message_i] = 0;
	BX_BIOS_THIS s.vgabios_message_i = 0;
        vgabioslog->panic("%s", BX_BIOS_THIS s.vgabios_message);
	break;
      }
    case 0x0501:
      vgabioslog->panic("BIOS panic at rombios.c, line %d", value);
      break;

    // 0x0500 is used as the message port for the vgabios
    case 0x0500:
      BX_BIOS_THIS s.vgabios_message[BX_BIOS_THIS s.vgabios_message_i] =
        (Bit8u) value;
      BX_BIOS_THIS s.vgabios_message_i ++;
      if ( BX_BIOS_THIS s.vgabios_message_i >= BX_BIOS_MESSAGE_SIZE ) {
        BX_BIOS_THIS s.vgabios_message[ BX_BIOS_MESSAGE_SIZE - 1] = 0;
        BX_BIOS_THIS s.vgabios_message_i = 0;
        vgabioslog->info("%s", BX_BIOS_THIS s.vgabios_message);
        }
      else if ((value & 0xff) == '\n') {
        BX_BIOS_THIS s.vgabios_message[ BX_BIOS_THIS s.vgabios_message_i - 1 ] = 0;
        BX_BIOS_THIS s.vgabios_message_i = 0;
        vgabioslog->info("%s", BX_BIOS_THIS s.vgabios_message);
        }
      break;

    default:
	    break;
    }
}
