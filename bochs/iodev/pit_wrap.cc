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
#include "pit82c54.h"
#include "pit_wrap.h"
#define LOG_THIS bx_pit2.

bx_pit2_c bx_pit2;
#if BX_USE_PIT_SMF
#define this (&bx_pit2)
#endif

#ifdef OUT
#  undef OUT
#endif

bx_pit2_c::bx_pit2_c( void )
{
  put("PIT");
  settype(PITLOG);
  s.speaker_data_on=0;

  /* 8254 PIT (Programmable Interval Timer) */

  BX_PIT2_THIS s.timer_handle[1] = BX_NULL_TIMER_HANDLE;
  BX_PIT2_THIS s.timer_handle[2] = BX_NULL_TIMER_HANDLE;
}

bx_pit2_c::~bx_pit2_c( void )
{
}

  int
bx_pit2_c::init( bx_devices_c *d )
{
  BX_PIT2_THIS devices = d;

  BX_PIT2_THIS devices->register_irq(0, "8254 PIT");
  BX_PIT2_THIS devices->register_io_read_handler(this, read_handler, 0x0040, "8254 PIT");
  BX_PIT2_THIS devices->register_io_read_handler(this, read_handler, 0x0041, "8254 PIT");
  BX_PIT2_THIS devices->register_io_read_handler(this, read_handler, 0x0042, "8254 PIT");
  BX_PIT2_THIS devices->register_io_read_handler(this, read_handler, 0x0043, "8254 PIT");
  BX_PIT2_THIS devices->register_io_read_handler(this, read_handler, 0x0061, "8254 PIT");

  BX_PIT2_THIS devices->register_io_write_handler(this, write_handler, 0x0040, "8254 PIT");
  BX_PIT2_THIS devices->register_io_write_handler(this, write_handler, 0x0041, "8254 PIT");
  BX_PIT2_THIS devices->register_io_write_handler(this, write_handler, 0x0042, "8254 PIT");
  BX_PIT2_THIS devices->register_io_write_handler(this, write_handler, 0x0043, "8254 PIT");
  BX_PIT2_THIS devices->register_io_write_handler(this, write_handler, 0x0061, "8254 PIT");

  BX_PIT2_THIS s.speaker_data_on = 0;
  BX_PIT2_THIS s.refresh_clock_div2 = 0;

  BX_PIT2_THIS s.timer.init();

  return(1);
}



  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pit2_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit2_c *class_ptr = (bx_pit2_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pit2_c::read( Bit32u   address, unsigned int io_len )
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PIT_SMF
  if (io_len > 1)
    BX_PANIC(("pit: io read from port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  if (bx_dbg.pit)
    BX_INFO(("pit: io read from port %04x", (unsigned) address));

  switch (address) {

    case 0x40: /* timer 0 - system ticks */
      return(BX_PIT2_THIS s.timer.read(0));
      break;
    case 0x41: /* timer 1 read */
      return(BX_PIT2_THIS s.timer.read(1));
      break;
    case 0x42: /* timer 2 read */
      return(BX_PIT2_THIS s.timer.read(2));
      break;
    case 0x43: /* timer 1 read */
      return(BX_PIT2_THIS s.timer.read(3));
      break;

    case 0x61:
      /* AT, port 61h */
      BX_PIT2_THIS s.refresh_clock_div2 = !BX_PIT2_THIS s.refresh_clock_div2;
      return( (BX_PIT2_THIS s.timer.read_OUT(2)<<5) |
              (BX_PIT2_THIS s.refresh_clock_div2<<4) |
              (BX_PIT2_THIS s.speaker_data_on<<1) |
              (BX_PIT2_THIS s.timer.read_GATE(2)) );
      break;

    default:
      BX_PANIC(("pit: unsupported io read from port %04x", address));
    }
  return(0); /* keep compiler happy */
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pit2_c::write_handler(void *this_ptr, Bit32u address, Bit32u dvalue, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit2_c *class_ptr = (bx_pit2_c *) this_ptr;

  class_ptr->write(address, dvalue, io_len);
}

  void
bx_pit2_c::write( Bit32u   address, Bit32u   dvalue,
                unsigned int io_len )
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PIT_SMF
  Bit8u   value;

  value = (Bit8u  ) dvalue;

  if (io_len > 1)
    BX_PANIC(("pit: io write to port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  if (bx_dbg.pit)
    BX_INFO(("pit: write to port %04x = %02x",
      (unsigned) address, (unsigned) value));

  switch (address) {
    case 0x40: /* timer 0: write count register */
      BX_PIT2_THIS s.timer.write(0,value);
      break;

    case 0x41: /* timer 1: write count register */
      BX_PIT2_THIS s.timer.write( 1,value );
      break;

    case 0x42: /* timer 2: write count register */
      BX_PIT2_THIS s.timer.write( 2,value );
      break;

    case 0x43: /* timer 0-2 mode control */
      BX_PIT2_THIS s.timer.write( 3,value );

    case 0x61:
      BX_PIT2_THIS s.speaker_data_on = (value >> 1) & 0x01;
/*??? only on AT+ */
      BX_PIT2_THIS s.timer.set_GATE(2, value & 0x01);
#if BX_CPU_LEVEL < 2
      /* ??? XT: */
      bx_kbd_port61h_write(value);
#endif
      break;

    default:
      BX_PANIC(("pit: unsupported io write to port %04x = %02x",
        (unsigned) address, (unsigned) value));
    }
}




  int
bx_pit2_c::SaveState( class state_file *fd )
{
  fd->write_check ("8254 start");
  fd->write (&BX_PIT2_THIS s, sizeof (BX_PIT2_THIS s));
  fd->write_check ("8254 end");
  return(0);
}


  int
bx_pit2_c::LoadState( class state_file *fd )
{
  fd->read_check ("8254 start");
  fd->read (&BX_PIT2_THIS s, sizeof (BX_PIT2_THIS s));
  fd->read_check ("8254 end");
  return(0);
}


#if 0
  void
bx_kbd_port61h_write(Bit8u   value)
{
//  PcError("KBD_PORT61H_WRITE(): not implemented yet");
  UNUSED( value );
}
#endif


  Boolean
bx_pit2_c::periodic( Bit32u   usec_delta )
{
  Bit32u i;
  Boolean prev_timer0_out = BX_PIT2_THIS s.timer.read_OUT(0);
  Boolean want_interrupt = 0;

  for(i=0; i<usec_delta; i++) {
    BX_PIT2_THIS s.timer.clock_all();
    if ( (prev_timer0_out==0) && ((BX_PIT2_THIS s.timer.read_OUT(0))==1) ) {
      want_interrupt=1;
    }
    prev_timer0_out=BX_PIT2_THIS s.timer.read_OUT(0);
  }

  return(want_interrupt);
}
