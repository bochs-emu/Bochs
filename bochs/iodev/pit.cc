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
#define LOG_THIS bx_pit.


// NOTES ON THE 8253/8254 PIT MODES

// MODE 0: Interrupt on Terminal Count
// ===================================
// Writing new count action:
//   loaded upon next CLK pulse.  counting doesn't start until GATE=1
// GATE 0..1 transition:
//   ???
// GATE 1..0 transition:
// counter expiration action:
//   wraps to FFFF
// * OUT rises until new count val or new control word for mode 0 written

// MODE 1: Programmable Monoflop
// =============================
// Writing new count action:
//   not effective for current process
// GATE 0..1 transition:
//   loads counter
// counter expiration action:
//   wraps to FFFF
// NOTES:
//   OUT rises until new count val or new control word for mode 0 written

// MODE 2: Rate Generator
// ======================
// Writing new count action:
//   ???
// GATE 0..1 transition:
//   loads initial count val and starts counting
// counter expiration action:
//   reloads after count expires
// NOTES:
// * after control word & initial count val N loaded, PIT starts
//   counting upon next CLK pulse.
// * when counter reaches 1, OUT drops to a low level, for one
//   CLK cycle. (short peak pulse generated)
// * afterwards, the initial count val is automatically reloaded
//   and the PIT restarts the same counting operation again.
// * distance of two OUT pulses is N CLK cycles long.
// * GATE=1 enables, GATE=0 disables counter.
// * if GATE drops to low level during counting operation and rises
//     to high level later, PIT loads initial count value at the
//     rise and starts counting.
// * PIT starts counting after last data byte written if GATE=1
// * if the output is low when the gate goes low, the output is
//   immediately set high.

// MODE 3: Square Wave Generator
// =============================
// Writing new count action:
//   ???
// GATE 0..1 transition:
//   ???
// counter expiration action:
//   reloads after count expires
// NOTES:
// * initially OUT at a high level
// * drop of GATE to a low level while OUT low, raises OUT to a high level
// * a rise from a low to a high level at GATE (trigger pulse),
//   loads the counter with the initial count value and starts
//   counting operation
// * a new count value supplied during the course of an active
//   counting operation doesn't affect the current process.
//   At the end of the current half cycle, the PIT loads the new value
// * if the GATE line goes low, count is temporarily halted until GATE
//   returns high
// * if the OUT line is high when GATE goes low, OUT is forced low.
// ??? different for odd/even counts

// MODE 4: Software Triggered Pulse
// ================================
// Writing new count action:
//   ???
// GATE 0..1 transition:
//   ???
// counter expiration action:
//   wraps to FFFF
// NOTES:

// MODE 5: Hardware Triggered Pulse
// ================================
// Writing new count action:
//   ???
// GATE 0..1 transition:
//   ???
// counter expiration action:
//   wraps to FFFF
// NOTES:



#define BX_PIT_LATCH_MODE_LSB   10
#define BX_PIT_LATCH_MODE_MSB   11
#define BX_PIT_LATCH_MODE_16BIT 12


bx_pit_c bx_pit;
#if BX_USE_PIT_SMF
#define this (&bx_pit)
#endif

#ifdef OUT
#  undef OUT
#endif


bx_pit_c::bx_pit_c( void )
{
  setprefix("PIT");
  settype(PITLOG);
  memset(&s, 0, sizeof(s));

  /* 8254 PIT (Programmable Interval Timer) */

  BX_PIT_THIS s.timer_handle[1] = BX_NULL_TIMER_HANDLE;
  BX_PIT_THIS s.timer_handle[2] = BX_NULL_TIMER_HANDLE;
}

bx_pit_c::~bx_pit_c( void )
{
}


  int
bx_pit_c::init( bx_devices_c *d )
{
  BX_PIT_THIS devices = d;

  BX_PIT_THIS devices->register_irq(0, "8254 PIT");
  BX_PIT_THIS devices->register_io_read_handler(this, read_handler, 0x0040, "8254 PIT");
  BX_PIT_THIS devices->register_io_read_handler(this, read_handler, 0x0041, "8254 PIT");
  BX_PIT_THIS devices->register_io_read_handler(this, read_handler, 0x0042, "8254 PIT");
  BX_PIT_THIS devices->register_io_read_handler(this, read_handler, 0x0043, "8254 PIT");
  BX_PIT_THIS devices->register_io_read_handler(this, read_handler, 0x0061, "8254 PIT");

  BX_PIT_THIS devices->register_io_write_handler(this, write_handler, 0x0040, "8254 PIT");
  BX_PIT_THIS devices->register_io_write_handler(this, write_handler, 0x0041, "8254 PIT");
  BX_PIT_THIS devices->register_io_write_handler(this, write_handler, 0x0042, "8254 PIT");
  BX_PIT_THIS devices->register_io_write_handler(this, write_handler, 0x0043, "8254 PIT");
  BX_PIT_THIS devices->register_io_write_handler(this, write_handler, 0x0061, "8254 PIT");

  BX_PIT_THIS s.speaker_data_on = 0;
  BX_PIT_THIS s.refresh_clock_div2 = 0;

  BX_PIT_THIS s.timer[0].mode        = 3;  /* periodic rate generator */
  BX_PIT_THIS s.timer[0].latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  BX_PIT_THIS s.timer[0].input_latch_value = 0;
  BX_PIT_THIS s.timer[0].input_latch_toggle = 0;
  BX_PIT_THIS s.timer[0].output_latch_value = 0;
  BX_PIT_THIS s.timer[0].output_latch_toggle = 0;
  BX_PIT_THIS s.timer[0].output_latch_full = 0;
  BX_PIT_THIS s.timer[0].counter_max = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[0].counter     = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[0].bcd_mode    = 0;  /* binary counting mode */
  BX_PIT_THIS s.timer[0].GATE        = 1;  /* GATE tied to + logic */
  BX_PIT_THIS s.timer[0].OUT         = 1;
  BX_PIT_THIS s.timer[0].active      = 0;

  BX_PIT_THIS s.timer[1].mode        = 3;  /* periodic rate generator */
  BX_PIT_THIS s.timer[1].latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  BX_PIT_THIS s.timer[1].input_latch_value = 0;
  BX_PIT_THIS s.timer[1].input_latch_toggle = 0;
  BX_PIT_THIS s.timer[1].output_latch_value = 0;
  BX_PIT_THIS s.timer[1].output_latch_toggle = 0;
  BX_PIT_THIS s.timer[1].output_latch_full = 0;
  BX_PIT_THIS s.timer[1].counter_max = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[1].counter     = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[1].bcd_mode    = 0;  /* binary counting mode */
  BX_PIT_THIS s.timer[1].GATE        = 1;  /* GATE tied to + logic */
  BX_PIT_THIS s.timer[1].OUT         = 1;
  BX_PIT_THIS s.timer[1].active      = 0;

  BX_PIT_THIS s.timer[2].mode        = 3;  /* periodic rate generator */
  BX_PIT_THIS s.timer[2].latch_mode  = BX_PIT_LATCH_MODE_16BIT;
  BX_PIT_THIS s.timer[2].input_latch_value = 0;
  BX_PIT_THIS s.timer[2].input_latch_toggle = 0;
  BX_PIT_THIS s.timer[2].output_latch_value = 0;
  BX_PIT_THIS s.timer[2].output_latch_toggle = 0;
  BX_PIT_THIS s.timer[2].output_latch_full = 0;
  BX_PIT_THIS s.timer[2].counter_max = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[2].counter     = 0;  /* 0xFFFF + 1 : (1193182 / 65535 = 18.2Hz) */
  BX_PIT_THIS s.timer[2].bcd_mode    = 0;  /* binary counting mode */
  BX_PIT_THIS s.timer[2].GATE        = 0;  /* timer2 gate controlled by port 61h bit 0 */
  BX_PIT_THIS s.timer[2].OUT         = 1;
  BX_PIT_THIS s.timer[2].active      = 0;

  return(1);
}






  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pit_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit_c *class_ptr = (bx_pit_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pit_c::read( Bit32u   address, unsigned int io_len )
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
      return( read_counter(0) );
      break;

    case 0x42: /* timer 2 read */
      return( read_counter(2) );
      break;

    case 0x61:
      /* AT, port 61h */
      BX_PIT_THIS s.refresh_clock_div2 = !BX_PIT_THIS s.refresh_clock_div2;
      return( (BX_PIT_THIS s.timer[2].OUT<<5) |
              (BX_PIT_THIS s.refresh_clock_div2<<4) |
              (BX_PIT_THIS s.speaker_data_on<<1) |
              (BX_PIT_THIS s.timer[2].GATE) );
      break;

    default:
      BX_PANIC(("pit: unsupported io read from port %04x", address));
    }
  return(0); /* keep compiler happy */
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pit_c::write_handler(void *this_ptr, Bit32u address, Bit32u dvalue, unsigned io_len)
{
#if !BX_USE_PIT_SMF
  bx_pit_c *class_ptr = (bx_pit_c *) this_ptr;

  class_ptr->write(address, dvalue, io_len);
}

  void
bx_pit_c::write( Bit32u   address, Bit32u   dvalue,
                unsigned int io_len )
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_PIT_SMF
  Bit8u    command, mode, bcd_mode;
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
      write_count_reg( value, 0 );
      break;

    case 0x41: /* timer 1: write count register */
      write_count_reg( value, 1 );
      break;

    case 0x42: /* timer 2: write count register */
      write_count_reg( value, 2 );
      break;

    case 0x43: /* timer 0-2 mode control */
      /* |7 6 5 4|3 2 1|0|
       * |-------|-----|-|
       * |command|mode |bcd/binary|
       */
      command  = value >> 4;
      mode     = (value >> 1) & 0x07;
      bcd_mode = value & 0x01;
#if 0
BX_INFO(("timer 0-2 mode control: comm:%02x mode:%02x bcd_mode:%u",
  (unsigned) command, (unsigned) mode, (unsigned) bcd_mode));
#endif

      if ( (mode > 5) || (command > 0x0e) )
        BX_PANIC(("pit: outp(43h)=%02xh out of range", (unsigned) value));
      if (bcd_mode)
        BX_PANIC(("pit: outp(43h)=%02xh: bcd mode unhandled",
          (unsigned) bcd_mode));

      switch (command) {
        case 0x0: /* timer 0: counter latch */
          latch( 0 );
          break;

        case 0x1: /* timer 0: LSB mode */
        case 0x2: /* timer 0: MSB mode */
          BX_PANIC(("pit: outp(43h): command %02xh unhandled",
            (unsigned) command));
          break;
        case 0x3: /* timer 0: 16-bit mode */
          BX_PIT_THIS s.timer[0].mode = mode;
          BX_PIT_THIS s.timer[0].latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          BX_PIT_THIS s.timer[0].input_latch_value = 0;
          BX_PIT_THIS s.timer[0].input_latch_toggle = 0;
          BX_PIT_THIS s.timer[0].bcd_mode    = bcd_mode;
          if ( (mode!=3 && mode!=2 && mode!=0) || bcd_mode!=0 )
            BX_PANIC(("pit: outp(43h): comm 3, mode %02x, bcd %02x unhandled",
              (unsigned) mode, bcd_mode));
          break;
        case 0x4: /* timer 1: counter latch */
          latch( 1 );
          break;

        case 0x5: /* timer 1: LSB mode */
        case 0x6: /* timer 1: MSB mode */
          BX_INFO(("pit: outp(43h): command %02xh unhandled (ignored)",
            (unsigned) command));
          break;
        case 0x7: /* timer 1: 16-bit mode */
          BX_PIT_THIS s.timer[1].mode = mode;
          BX_PIT_THIS s.timer[1].latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          BX_PIT_THIS s.timer[1].input_latch_value = 0;
          BX_PIT_THIS s.timer[1].input_latch_toggle = 0;
          BX_PIT_THIS s.timer[1].bcd_mode    = bcd_mode;
          if ( (mode!=3 && mode!=2 && mode!=0) || bcd_mode!=0 )
            BX_PANIC(("pit: outp(43h): comm 7, mode %02x, bcd %02x unhandled",
              (unsigned) mode, bcd_mode));
          break;
        case 0x8: /* timer 2: counter latch */
          latch( 2 );
          break;

        case 0x9: /* timer 2: LSB mode */
        case 0xa: /* timer 2: MSB mode */
          BX_PANIC(("pit: outp(43h): command %02xh unhandled",
            (unsigned) command));
          break;
        case 0xb: /* timer 2: 16-bit mode */
          BX_PIT_THIS s.timer[2].mode = mode;
          BX_PIT_THIS s.timer[2].latch_mode   = BX_PIT_LATCH_MODE_16BIT;
          BX_PIT_THIS s.timer[2].input_latch_value = 0;
          BX_PIT_THIS s.timer[2].input_latch_toggle = 0;
          BX_PIT_THIS s.timer[2].bcd_mode    = bcd_mode;
          if ( (mode!=3 && mode!=2 && mode!=0) || bcd_mode!=0 )
            BX_PANIC(("pit: outp(43h): comm Bh, mode %02x, bcd %02x unhandled",
              (unsigned) mode, bcd_mode));
          break;
#if 0
        case 0xd: /* general counter latch */
          if (value & 0x08) /* select counter 2 */
            latch( 2 );
          if (value & 0x04) /* select counter 1 */
            latch( 1 );
          if (value & 0x02) /* select counter 0 */
            latch( 0 );
          break;

        case 0xe: /* latch status of timers */
          BX_PANIC(("pit: outp(43h): command %02xh unhandled",
            (unsigned) command);
          break;
#endif
        case 0x0c: case 0x0d: case 0x0e: case 0x0f:
          BX_INFO(("pit: ignoring 8254 command %u", (unsigned) command));
          break;

        default: /* 0xc & 0xf */
          BX_PANIC(("pit: outp(43h) command %1xh unhandled",
            (unsigned) command));
          break;
        }
      break;

    case 0x61:
      BX_PIT_THIS s.speaker_data_on = (value >> 1) & 0x01;
/*??? only on AT+ */
      set_GATE(2, value & 0x01);
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




  void
bx_pit_c::write_count_reg( Bit8u   value, unsigned timerid )
{
  Boolean xfer_complete;

  switch ( BX_PIT_THIS s.timer[timerid].latch_mode ) {
    case BX_PIT_LATCH_MODE_16BIT: /* write1=LSB, write2=MSB */
      if (BX_PIT_THIS s.timer[timerid].input_latch_toggle==0) {
        BX_PIT_THIS s.timer[timerid].input_latch_value = value;
        BX_PIT_THIS s.timer[timerid].input_latch_toggle = 1;
        xfer_complete = 0;
        if (bx_dbg.pit)
          BX_INFO(("pit: BX_PIT_THIS s.timer[timerid] write L = %02x", (unsigned) value));
        }
      else {
        BX_PIT_THIS s.timer[timerid].input_latch_value |= (value << 8);
        BX_PIT_THIS s.timer[timerid].input_latch_toggle = 0;
        xfer_complete = 1;
        if (bx_dbg.pit)
          BX_INFO(("pit: BX_PIT_THIS s.timer[timerid] write H = %02x", (unsigned) value));
        }
      break;

    case BX_PIT_LATCH_MODE_MSB: /* write1=MSB, LSB=0 */
      BX_PIT_THIS s.timer[timerid].input_latch_value = (value << 8);
      xfer_complete = 1;
      if (bx_dbg.pit)
        BX_INFO(("pit: BX_PIT_THIS s.timer[timerid] write H = %02x", (unsigned) value));
      break;

    case BX_PIT_LATCH_MODE_LSB: /* write1=LSB, MSB=0 */
      BX_PIT_THIS s.timer[timerid].input_latch_value = value;
      xfer_complete = 1;
      if (bx_dbg.pit)
        BX_INFO(("pit: BX_PIT_THIS s.timer[timerid] write L = %02x", (unsigned) value));
      break;

    default:
      BX_PANIC(("write_count_reg: latch_mode unknown"));
      xfer_complete = 0;
    }

  if (xfer_complete) {
    BX_PIT_THIS s.timer[timerid].counter_max = BX_PIT_THIS s.timer[timerid].input_latch_value;

    // reprogramming counter clears latch
    BX_PIT_THIS s.timer[timerid].output_latch_full = 0;

    // counter bounds
    // mode      minimum    maximum
    //  0           1          0
    //  1           1          0
    //  2           2          0
    //  3           2          0
    //  4           1          0
    //  5           1          0
    switch (BX_PIT_THIS s.timer[timerid].mode) {
      case 0:
        BX_PIT_THIS s.timer[timerid].counter = BX_PIT_THIS s.timer[timerid].counter_max;
        BX_PIT_THIS s.timer[timerid].active = 1;
        if (BX_PIT_THIS s.timer[timerid].GATE) {
          BX_PIT_THIS s.timer[timerid].OUT = 0; // OUT pin starts low
          start( timerid );
          }
        break;
      case 1:
        BX_PANIC(("pit:write_count_reg(%u): mode1 unsupported",
                 timerid));
        break;
      case 2:
        if ( BX_PIT_THIS s.timer[timerid].counter_max == 1 )
          BX_PANIC(("pit:write_count_reg(%u): mode %u counter_max=1",
                   timerid, (unsigned) BX_PIT_THIS s.timer[timerid].mode));
        if ( BX_PIT_THIS s.timer[timerid].GATE && !BX_PIT_THIS s.timer[timerid].active ) {
          // software triggered
          BX_PIT_THIS s.timer[timerid].counter = BX_PIT_THIS s.timer[timerid].counter_max;
          BX_PIT_THIS s.timer[timerid].active  = 1;
          BX_PIT_THIS s.timer[timerid].OUT     = 1; // initially set high
          start( timerid );
          }
        break;
      case 3:
        if ( BX_PIT_THIS s.timer[timerid].counter_max == 1 )
          BX_PANIC(("pit:write_count_reg(%u): mode %u counter_max=1",
                   timerid, (unsigned) BX_PIT_THIS s.timer[timerid].mode));
        BX_PIT_THIS s.timer[timerid].counter_max = BX_PIT_THIS s.timer[timerid].counter_max & 0xfffe;
        if ( BX_PIT_THIS s.timer[timerid].GATE && !BX_PIT_THIS s.timer[timerid].active ) {
          // software triggered
          BX_PIT_THIS s.timer[timerid].counter = BX_PIT_THIS s.timer[timerid].counter_max;
          BX_PIT_THIS s.timer[timerid].active  = 1;
          BX_PIT_THIS s.timer[timerid].OUT     = 1; // initially set high
          start( timerid );
          }
        break;
      case 4:
        BX_PANIC(("pit:write_count_reg(%u): mode4 unsupported",
                 timerid));
        break;
      case 5:
        BX_PANIC(("pit:write_count_reg(%u): mode5 unsupported",
                 timerid));
        break;
      }
    }
}


  Bit8u
bx_pit_c::read_counter( unsigned timerid )
{
  Bit16u  counter_value;
  Bit8u    retval;

  if (BX_PIT_THIS s.timer[timerid].output_latch_full) { /* latched read */
    counter_value = BX_PIT_THIS s.timer[timerid].output_latch_value;
    }
  else { /* direct unlatched read */
    counter_value = BX_PIT_THIS s.timer[timerid].counter;
BX_INFO(("CV=%04x", (unsigned) BX_PIT_THIS s.timer[timerid].counter));
    }

  switch (BX_PIT_THIS s.timer[timerid].latch_mode) {
    case BX_PIT_LATCH_MODE_LSB:
      retval = (Bit8u  ) counter_value;
      BX_PIT_THIS s.timer[timerid].output_latch_full = 0;
      break;
    case BX_PIT_LATCH_MODE_MSB:
      retval = (Bit8u  ) ( counter_value >> 8 );
      BX_PIT_THIS s.timer[timerid].output_latch_full = 0;
      break;
    case BX_PIT_LATCH_MODE_16BIT:
      if (BX_PIT_THIS s.timer[timerid].output_latch_toggle==0) { /* LSB 1st */
        retval = (Bit8u  ) counter_value;
        }
      else { /* MSB 2nd */
        retval = (Bit8u  ) ( counter_value >> 8 );
        }
      BX_PIT_THIS s.timer[timerid].output_latch_toggle = !BX_PIT_THIS s.timer[timerid].output_latch_toggle;
      if (BX_PIT_THIS s.timer[timerid].output_latch_toggle == 0)
        BX_PIT_THIS s.timer[timerid].output_latch_full = 0;
      break;
    default:
      BX_PANIC(("pit: io read from port 40h: unknown latch mode"));
      retval = 0; /* keep compiler happy */
    }
  return( retval );
}


  void
bx_pit_c::latch( unsigned timerid )
{
  /* subsequent counter latch commands are ignored until value read out */
  if (BX_PIT_THIS s.timer[timerid].output_latch_full) {
    BX_INFO(("pit: pit(%u) latch: output latch full, ignoring",
              timerid));
    return;
    }

  BX_PIT_THIS s.timer[timerid].output_latch_value = BX_PIT_THIS s.timer[timerid].counter;

  if (bx_dbg.pit)
    BX_INFO(("pit: latch_value = %lu", BX_PIT_THIS s.timer[timerid].output_latch_value));
  BX_PIT_THIS s.timer[timerid].output_latch_toggle = 0;
  BX_PIT_THIS s.timer[timerid].output_latch_full   = 1;
}

  void
bx_pit_c::set_GATE(unsigned pit_id, unsigned value)
{
  // GATE's for Timer 0 & Timer 1 are tied high.
  if (pit_id != 2)
    BX_PANIC(("pit:set_GATE: pit_id != 2"));

  value = (value > 0);

  /* if no transition of GATE input line, then nothing to do */
  if (value == BX_PIT_THIS s.timer[2].GATE)
    return;

  if (value) { /* PIT2: GATE transition from 0 to 1 */
    BX_PIT_THIS s.timer[2].GATE  = 1;
    switch ( BX_PIT_THIS s.timer[2].mode ) {
      case 0:
        BX_PIT_THIS s.timer[2].counter = BX_PIT_THIS s.timer[2].counter_max;
        if (BX_PIT_THIS s.timer[2].active) {
          BX_PIT_THIS s.timer[2].OUT = 0;
          }
        start( 2 );
        break;
      case 2:
        // begin counting, reload counter
        BX_PIT_THIS s.timer[2].active = 1;
        BX_PIT_THIS s.timer[2].OUT = 1;
        BX_PIT_THIS s.timer[2].counter = BX_PIT_THIS s.timer[2].counter_max;
        start( 2 );
        break;
      case 3:
        // begin counting, reload counter
        BX_PIT_THIS s.timer[2].active = 1;
        BX_PIT_THIS s.timer[2].OUT = 1;
        BX_PIT_THIS s.timer[2].counter = BX_PIT_THIS s.timer[2].counter_max;
        start( 2 );
        break;
      case 1:
      case 4:
      case 5:
      default:
        BX_PANIC(("bx_pit_c::set_GATE: unhandled timer2 mode %u",
                 (unsigned) BX_PIT_THIS s.timer[2].mode));
      }
    }
  else {       // PIT2: GATE transition from 1 to 0, deactivate
    BX_PIT_THIS s.timer[2].GATE  = 0;
    switch ( BX_PIT_THIS s.timer[2].mode ) {
      case 0:
        break;
      case 2:
        // 1) stops count, 2) OUT goes immediately high
        BX_PIT_THIS s.timer[2].active = 0;
        BX_PIT_THIS s.timer[2].OUT = 1;
        break;
      case 3:
        // 1) stops count, 2) OUT goes immediately high
        BX_PIT_THIS s.timer[2].active = 0;
        BX_PIT_THIS s.timer[2].OUT = 1;
        break;
      case 1:
      case 4:
      case 5:
      default:
        BX_PANIC(("bx_pit_c::set_GATE: unhandled timer2 mode %u",
                 (unsigned) BX_PIT_THIS s.timer[2].mode));
      }
    }
}


  void
bx_pit_c::start(unsigned timerid)
{
  unsigned long period_hz;

  if (BX_PIT_THIS s.timer[timerid].counter_max == 0x0000) {
    period_hz   = 1193182 / 65536;
    }
  else {
    period_hz = 1193182 / BX_PIT_THIS s.timer[timerid].counter_max;
    }
  BX_INFO(("timer%u period set to %lu hz", timerid, period_hz));


  switch (BX_PIT_THIS s.timer[timerid].mode) {
    case 0: /* single timeout */
      break;
    case 1: /* retriggerable one-shot */
      BX_PANIC(("start: mode %u unhandled",
               (unsigned) BX_PIT_THIS s.timer[timerid].mode));
      break;
    case 2: /* rate generator */
      break;
    case 3: /* square wave mode */
      break;
    case 4: /* software triggered strobe */
      BX_PANIC(("start: mode %u unhandled",
               (unsigned) BX_PIT_THIS s.timer[timerid].mode));
      break;
    case 5: /* hardware retriggerable strobe */
      BX_PANIC(("start: mode %u unhandled",
               (unsigned) BX_PIT_THIS s.timer[timerid].mode));
      break;
    default:
      BX_PANIC(("start: timer%u has bad mode",
               (unsigned) BX_PIT_THIS s.timer[timerid].mode));
    }
}




  int
bx_pit_c::SaveState( class state_file *fd )
{
  fd->write_check ("8254 start");
  fd->write (&BX_PIT_THIS s, sizeof (BX_PIT_THIS s));
  fd->write_check ("8254 end");
  return(0);
}


  int
bx_pit_c::LoadState( class state_file *fd )
{
  fd->read_check ("8254 start");
  fd->read (&BX_PIT_THIS s, sizeof (BX_PIT_THIS s));
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
bx_pit_c::periodic( Bit32u   usec_delta )
{
  Boolean prev_timer0_out;

  prev_timer0_out = BX_PIT_THIS s.timer[0].OUT;

  for (unsigned i = 0; i < 3; i++) {
    // is timer enabled and active?
    if ( BX_PIT_THIS s.timer[i].GATE && BX_PIT_THIS s.timer[i].active ) {
      switch ( BX_PIT_THIS s.timer[i].mode ) {
        case 0: // Mode 0: Single Timeout
          // wraps after count expires
          if ( BX_PIT_THIS s.timer[i].counter == 0 ) {
            // counter previously expired, wrap counter
            BX_PIT_THIS s.timer[i].counter = 0xffff;
            }
          else if ( usec_delta >= BX_PIT_THIS s.timer[i].counter ) {
            // counter expired
            BX_PIT_THIS s.timer[i].counter = 0;
            BX_PIT_THIS s.timer[i].OUT     = 1;
            }
          else {
            // decrement counter by elapsed useconds
            BX_PIT_THIS s.timer[i].counter -= (Bit16u ) usec_delta;
            }
          break;

        case 1: // Mode 1: Retriggerable One-Shot
          // wraps after count expires
          BX_PANIC(("bx_pit_c::periodic: bad mode: timer[%u], mode %u",
                        i, (unsigned) BX_PIT_THIS s.timer[i].mode));
          break;

        case 2: // Mode 2: Rate Generator
          // reloads after count expires
          // OUT is low when counter=1, high otherwise
          // min count=2, max count=0
          if ( BX_PIT_THIS s.timer[i].counter == 0 ) {
            // max counter val, just wrap
            BX_PIT_THIS s.timer[i].counter = 0xffff;
            BX_PIT_THIS s.timer[i].OUT     = 1;
            }
          else if ( BX_PIT_THIS s.timer[i].counter == 1 ) {
            // counter previously expired, reload
            BX_PIT_THIS s.timer[i].counter = BX_PIT_THIS s.timer[i].counter_max;
            BX_PIT_THIS s.timer[i].OUT     = 1;
            }
          else if ( (BX_PIT_THIS s.timer[i].counter == 2) ||
                    (usec_delta >= (Bit32u(BX_PIT_THIS s.timer[i].counter) - 1)) ) {
            // in either case, counter will reach 1
            BX_PIT_THIS s.timer[i].counter = 1;
            BX_PIT_THIS s.timer[i].OUT = 0;
            }
          else {
            // decrement counter by elapsed useconds
            BX_PIT_THIS s.timer[i].counter -= (Bit16u ) usec_delta;
            }
          break;

        case 3: // Mode 3: Square Wave Mode
          // reloads after count expires
          // min count=2, max count=0
          if ( BX_PIT_THIS s.timer[i].counter == 0 ) {
            // max count, dec by 2
            BX_PIT_THIS s.timer[i].counter = 0xfffe;
            }
          else if ( (BX_PIT_THIS s.timer[i].counter <= 2) ||
                    ( (usec_delta*2) >= BX_PIT_THIS s.timer[i].counter ) ) {
            // counter expired, reload
            BX_PIT_THIS s.timer[i].counter = BX_PIT_THIS s.timer[i].counter_max;
            BX_PIT_THIS s.timer[i].OUT     = !BX_PIT_THIS s.timer[i].OUT;
            //BX_INFO(("CV: reload t%u to %04x", (unsigned) i, (unsigned)
            //  BX_PIT_THIS s.timer[i].counter));
            }
          else {
            // decrement counter by elapsed useconds
            BX_PIT_THIS s.timer[i].counter -= (Bit16u ) ( 2*usec_delta );
            //BX_INFO(("CV: dec count to %04x",
            //          (unsigned) BX_PIT_THIS s.timer[i].counter));
            }
          break;

        case 4: // Mode 4: Software Triggered Strobe
          // wraps after count expires
          BX_PANIC(("bx_pit_c::periodic: bad mode: timer[%u], mode %u",
                        i, (unsigned) BX_PIT_THIS s.timer[i].mode));
          break;

        case 5: // Mode 5: Hardware Retriggerable Strobe
          // wraps after count expires
          BX_PANIC(("bx_pit_c::periodic: bad mode: timer[%u], mode %u",
                        i, (unsigned) BX_PIT_THIS s.timer[i].mode));
          break;
        default:
          BX_PANIC(("bx_pit_c::periodic: bad mode: timer[%u], mode %u",
                        i, (unsigned) BX_PIT_THIS s.timer[i].mode));
          break;
        } // switch ( BX_PIT_THIS s.tim...
      } // if ( BX_PIT_THIS s.timer[i]...
    } // for (unsigned i...

  // see if there's a rising edge on timer0's output to trigger an IRQ0.
  if ( (prev_timer0_out==0) && (BX_PIT_THIS s.timer[0].OUT==1) )
    return(1); // request IRQ 0
  else
    return(0);
}
