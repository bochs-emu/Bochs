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
#define LOG_THIS bx_parallel.


bx_parallel_c bx_parallel;

#if BX_USE_PAR_SMF
#define this (&bx_parallel)
#endif



bx_parallel_c::bx_parallel_c(void)
{
  put("PAR");
  settype(PARLOG);
  // nothing for now
}

bx_parallel_c::~bx_parallel_c(void)
{
  // nothing for now
}

  void
bx_parallel_c::init(bx_devices_c *d)
{
  BX_PAR_THIS devices = d;

#if 0
  /* PARALLEL PORT 1 */

  BX_PAR_THIS devices->register_irq(7, "Parallel Port 1");
  for (unsigned addr=0x03BC; addr<=0x03BE; addr++) {
    BX_PAR_THIS devices->register_io_read_handler(this,
       read_handler,
       addr, "Parallel Port 1");
    BX_PAR_THIS devices->register_io_write_handler(this,
       write_handler,
       addr, "Parallel Port 1");
    }
#endif
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
  UNUSED(address);
  UNUSED(io_len);

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
  UNUSED(address);
  UNUSED(value);
  UNUSED(io_len);
  /* PARALLEL PORT 1 */
}
