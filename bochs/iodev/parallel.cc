/////////////////////////////////////////////////////////////////////////
// $Id: parallel.cc,v 1.10 2001-11-12 01:45:21 bdenney Exp $
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
//
////////////////////////////////////////////////////////
// This code was just a few stubs until Volker.Ruppert@t-online.de 
// fixed it up in November 2001.


#include "bochs.h"
#define LOG_THIS bx_parallel.

#define OUTPUT BX_PAR_THIS output

bx_parallel_c bx_parallel;

#if BX_USE_PAR_SMF
#define this (&bx_parallel)
#endif



bx_parallel_c::bx_parallel_c(void)
{
	put("PAR");
	settype(PARLOG);
}

bx_parallel_c::~bx_parallel_c(void)
{
  fclose(OUTPUT);
}

  void
bx_parallel_c::init(bx_devices_c *d)
{
  BX_DEBUG(("Init $Id: parallel.cc,v 1.10 2001-11-12 01:45:21 bdenney Exp $"));
  BX_PAR_THIS devices = d;

  /* PARALLEL PORT 1 */

  BX_PAR_THIS devices->register_irq(7, "Parallel Port 1");
  BX_PAR_THIS devices->register_io_read_handler(this,
       read_handler, 0x0379, "Parallel Port 1");
  BX_PAR_THIS devices->register_io_read_handler(this,
       read_handler, 0x037A, "Parallel Port 1");
  BX_PAR_THIS devices->register_io_write_handler(this,
       write_handler, 0x0378, "Parallel Port 1");
  BX_PAR_THIS devices->register_io_write_handler(this,
       write_handler, 0x037A, "Parallel Port 1");

  BX_PAR_THIS s.STATUS.error = 1;
  BX_PAR_THIS s.STATUS.slct  = 1;
  BX_PAR_THIS s.STATUS.pe    = 0;
  BX_PAR_THIS s.STATUS.ack   = 1;
  BX_PAR_THIS s.STATUS.busy  = 1;

  BX_PAR_THIS s.CONTROL.strobe   = 0;
  BX_PAR_THIS s.CONTROL.autofeed = 0;
  BX_PAR_THIS s.CONTROL.init     = 1;
  BX_PAR_THIS s.CONTROL.slct_in  = 1;
  BX_PAR_THIS s.CONTROL.irq      = 0;

  OUTPUT = fopen("parport.out", "w");
}



  void
bx_parallel_c::virtual_printer(void)
{
  fprintf(OUTPUT, "%c", BX_PAR_THIS s.data);
  fflush (OUTPUT);
  if (BX_PAR_THIS s.CONTROL.irq == 1) {
    BX_PAR_THIS devices->pic->trigger_irq(7);
    }
  BX_PAR_THIS s.STATUS.ack = 0;
  BX_PAR_THIS s.STATUS.busy = 1;
}

  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_parallel_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PAR_SMF
  bx_parallel_c *class_ptr = (bx_parallel_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_parallel_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PAR_SMF

  if (io_len == 1) {
    switch (address) {
      case 0x0379:
	{
	  Bit32u retval;
	  retval = ((BX_PAR_THIS s.STATUS.busy  << 7) |
		    (BX_PAR_THIS s.STATUS.ack   << 6) |
		    (BX_PAR_THIS s.STATUS.pe    << 5) |
		    (BX_PAR_THIS s.STATUS.slct  << 4) |
		    (BX_PAR_THIS s.STATUS.error << 3));
	  BX_PAR_THIS s.STATUS.ack = 1;
	  if (BX_PAR_THIS s.CONTROL.irq == 1) {
	    BX_PAR_THIS devices->pic->untrigger_irq(7);
	    }
	  return retval;
	}
	break;
      case 0x037A:
	{
	  return ((BX_PAR_THIS s.CONTROL.irq      << 4) |
		  (BX_PAR_THIS s.CONTROL.slct_in  << 3) |
		  (BX_PAR_THIS s.CONTROL.init     << 2) |
		  (BX_PAR_THIS s.CONTROL.autofeed << 1) |
		  (BX_PAR_THIS s.CONTROL.strobe));
	}
	break;
      }
    }
  /* PARALLEL PORT 1 */
  return(0);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_parallel_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PAR_SMF
  bx_parallel_c *class_ptr = (bx_parallel_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_parallel_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PAR_SMF

  if (io_len == 1) {
    switch (address) {
      case 0x0378:
	{
	  BX_PAR_THIS s.data = (Bit8u)value;
	}
	break;
      case 0x037A:
	{
	  if ((value & 0x01) == 1) {
	    if (BX_PAR_THIS s.CONTROL.strobe == 0) {
	      BX_PAR_THIS s.CONTROL.strobe = 1;
	      virtual_printer(); // data is valid now
	      }
	  } else {
	    if (BX_PAR_THIS s.CONTROL.strobe == 1) {
	      BX_PAR_THIS s.CONTROL.strobe = 0;
	      }
	    }
	  BX_PAR_THIS s.CONTROL.autofeed = ((value & 0x02) == 1);
	  BX_PAR_THIS s.CONTROL.init     = ((value & 0x04) == 1);
	  BX_PAR_THIS s.CONTROL.slct_in  = ((value & 0x08) == 1);
	  BX_PAR_THIS s.STATUS.slct = BX_PAR_THIS s.CONTROL.slct_in;
	  if ((value & 0x10) == 0x10) {
	    BX_PAR_THIS s.CONTROL.irq = 1;
	  } else {
	    BX_PAR_THIS s.CONTROL.irq = 0;
	    }
	}
	break;
      }
    }
  /* PARALLEL PORT 1 */
}
