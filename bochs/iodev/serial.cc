/////////////////////////////////////////////////////////////////////////
// $Id: serial.cc,v 1.17 2002-01-29 17:20:12 vruppert Exp $
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


// Peter Grehan (grehan@iprg.nokia.com) coded most of this
// serial emulation.  Here's some notes from his implementation:

// New files. Implement a single 8250, and allow terminal input/output
// to stdout on FreeBSD. 16550/16550a/16552 should be easy: just add code
// to handle a FIFO. I only did what was needed to get console output.


// define USE_TTY_HACK to connect an xterm or similar (depends on tty.c)
// to the serial port /AM

#include "bochs.h"
#define LOG_THIS bx_serial.

#if USE_RAW_SERIAL
#include <signal.h>
#endif

#ifdef WIN32
#ifndef __MINGW32__
// +++
//#include <winsock2.h>
#include <winsock.h>
#endif
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__)
// #define SERIAL_ENABLE
#endif

#ifdef SERIAL_ENABLE
extern "C" {
#include <termios.h>
};
#endif

bx_serial_c bx_serial;
#if BX_USE_SER_SMF
#define this (&bx_serial)
#endif

#ifdef SERIAL_ENABLE
static struct termios term_orig, term_new;
#endif

static int tty_id;

bx_serial_c::bx_serial_c(void)
{
  put("SER");
  settype(SERLOG);
#ifdef SERIAL_ENABLE
  tty_id = open("/dev/ttyqf",O_RDWR|O_NONBLOCK,600);
  BX_DEBUG(("tty_id: %d",tty_id));
  tcgetattr(tty_id, &term_orig);
  bcopy((caddr_t) &term_orig, (caddr_t) &term_new, sizeof(struct termios));
  cfmakeraw(&term_new);
  term_new.c_oflag |= OPOST | ONLCR;  // Enable NL to CR-NL translation
#ifndef TRUE_CTLC
  // ctl-C will exit Bochs, or trap to the debugger
  term_new.c_iflag &= ~IGNBRK;
  term_new.c_iflag |= BRKINT;
  term_new.c_lflag |= ISIG;
#else
  // ctl-C will be delivered to the serial port
  term_new.c_iflag |= IGNBRK;
  term_new.c_iflag &= ~BRKINT;
#endif    /* !def TRUE_CTLC */
  //term_new.c_iflag |= IXOFF;
  //tcsetattr(tty_id, TCSAFLUSH, &term_new);
#endif   /* def SERIAL_ENABLE */
  // nothing for now
#if USE_RAW_SERIAL
  this->raw = new serial_raw("/dev/cua0", SIGUSR1);
#endif // USE_RAW_SERIAL
}

bx_serial_c::~bx_serial_c(void)
{
#ifdef SERIAL_ENABLE
  tcsetattr(tty_id, TCSAFLUSH, &term_orig);
#endif
  // nothing for now
}


  void
bx_serial_c::init(bx_devices_c *d)
{
  BX_SER_THIS devices = d;

  BX_SER_THIS devices->register_irq(4, "Serial Port 1");

#if defined (USE_TTY_HACK)
  tty_id = tty_alloc("Bx Serial Console, Your Window to the 8250");
  if (tty_id > 0)
    BX_INFO(("TTY allocated fd = %d", tty_get_fd(tty_id)));
  else
    BX_INFO(("TTY allocation failed"));
#else
  //BX_INFO(("TTY not used, serial port is not connected"));
#endif

  /*
   * Put the UART registers into their RESET state
   */
  for (unsigned i=0; i<BX_SERIAL_CONFDEV; i++) {
    /* internal state */
    BX_SER_THIS s[i].rx_empty = 1;
    BX_SER_THIS s[i].tx_empty = 1;
    BX_SER_THIS s[i].ls_ipending = 0;
    BX_SER_THIS s[i].ms_ipending = 0;
    BX_SER_THIS s[i].rx_ipending = 0;
    BX_SER_THIS s[i].tx_ipending = 0;
    BX_SER_THIS s[i].ls_interrupt = 0;
    BX_SER_THIS s[i].ms_interrupt = 0;
    BX_SER_THIS s[i].rx_interrupt = 0;
    BX_SER_THIS s[i].tx_interrupt = 0;

    BX_SER_THIS s[i].tx_timer_index =
      bx_pc_system.register_timer(this, tx_timer_handler, 0,
				  0,0); // one-shot, inactive

    BX_SER_THIS s[i].rx_timer_index =
      bx_pc_system.register_timer(this, rx_timer_handler, 0,
				  0,0); // one-shot, inactive
    BX_SER_THIS s[i].rx_pollstate = BX_SER_RXIDLE;

    /* int enable: b0000 0000 */
    BX_SER_THIS s[i].int_enable.rxdata_enable = 0;
    BX_SER_THIS s[i].int_enable.txhold_enable = 0;
    BX_SER_THIS s[i].int_enable.rxlstat_enable = 0;
    BX_SER_THIS s[i].int_enable.modstat_enable = 0;

    /* int ID: b0000 0001 */
    BX_SER_THIS s[i].int_ident.ipending = 1;
    BX_SER_THIS s[i].int_ident.int_ID = 0;
    BX_SER_THIS s[i].int_ident.fifo_enabled = 0;

    /* FIFO control: b0000 0000 */
    BX_SER_THIS s[i].fifo_cntl.enable = 0;
    BX_SER_THIS s[i].fifo_cntl.rxreset = 0;
    BX_SER_THIS s[i].fifo_cntl.txreset = 0;
    BX_SER_THIS s[i].fifo_cntl.dmamode = 0;
    BX_SER_THIS s[i].fifo_cntl.rxtrigger = 0;

    /* Line Control reg: b0000 0000 */
    BX_SER_THIS s[i].line_cntl.wordlen_sel = 0;
    BX_SER_THIS s[i].line_cntl.stopbits = 0;
    BX_SER_THIS s[i].line_cntl.parity_enable = 0;
    BX_SER_THIS s[i].line_cntl.evenparity_sel = 0;
    BX_SER_THIS s[i].line_cntl.stick_parity = 0;
    BX_SER_THIS s[i].line_cntl.break_cntl = 0;
    BX_SER_THIS s[i].line_cntl.dlab = 0;

    /* Modem Control reg: b0000 0000 */
    BX_SER_THIS s[i].modem_cntl.dtr = 0;
    BX_SER_THIS s[i].modem_cntl.rts = 0;
    BX_SER_THIS s[i].modem_cntl.out1 = 0;
    BX_SER_THIS s[i].modem_cntl.out2 = 0;
    BX_SER_THIS s[i].modem_cntl.local_loopback = 0;

    /* Line Status register: b0110 0000 */
    BX_SER_THIS s[i].line_status.rxdata_ready = 0;
    BX_SER_THIS s[i].line_status.overrun_error = 0;
    BX_SER_THIS s[i].line_status.parity_error = 0;
    BX_SER_THIS s[i].line_status.framing_error = 0;
    BX_SER_THIS s[i].line_status.break_int = 0;
    BX_SER_THIS s[i].line_status.txhold_empty = 1;
    BX_SER_THIS s[i].line_status.txtransm_empty = 1;
    BX_SER_THIS s[i].line_status.fifo_error = 0;

    /* Modem Status register: bXXXX 0000 */
    BX_SER_THIS s[i].modem_status.delta_cts = 0;
    BX_SER_THIS s[i].modem_status.delta_dsr = 0;
    BX_SER_THIS s[i].modem_status.ri_trailedge = 0;
    BX_SER_THIS s[i].modem_status.delta_dcd = 0;
    BX_SER_THIS s[i].modem_status.cts = 0;
    BX_SER_THIS s[i].modem_status.dsr = 0;
    BX_SER_THIS s[i].modem_status.ri = 0;
    BX_SER_THIS s[i].modem_status.dcd = 0;

    BX_SER_THIS s[i].scratch = 0;      /* scratch register */
    BX_SER_THIS s[i].divisor_lsb = 0;  /* divisor-lsb register */
    BX_SER_THIS s[i].divisor_msb = 0;  /* divisor-msb register */
  }

  for (unsigned addr=0x03F8; addr<=0x03FF; addr++) {
	BX_DEBUG(("register read/write: 0x%x",addr));
    BX_SER_THIS devices->register_io_read_handler(this,
       read_handler,
       addr, "Serial Port 1");
    BX_SER_THIS devices->register_io_write_handler(this,
       write_handler,
       addr, "Serial Port 1");
    }

  BX_INFO(( "com0 at 0x3f8/8 irq 4" ));

}



  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_serial_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_SER_SMF
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_serial_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SER_SMF
  //UNUSED(address);
  Bit8u val;

  /* SERIAL PORT 1 */

  if (io_len > 1)
    BX_PANIC(("io read from port %04x, bad len=%u",
	     (unsigned) address,
             (unsigned) io_len));

  BX_DEBUG(("register read from address 0x%x - ", (unsigned) address));

  switch (address) {
    case 0x03F8: /* receive buffer, or divisor latch LSB if DLAB set */
      if (BX_SER_THIS s[0].line_cntl.dlab) {
        val = BX_SER_THIS s[0].divisor_lsb;
      } else {
        val = BX_SER_THIS s[0].rxbuffer;
        BX_SER_THIS s[0].line_status.rxdata_ready = 0;
        BX_SER_THIS s[0].rx_empty = 1;

        /* If there are no more ints pending, clear the irq */
        if ((BX_SER_THIS s[0].tx_interrupt == 0) &&
            (BX_SER_THIS s[0].ls_interrupt == 0) &&
            (BX_SER_THIS s[0].ms_interrupt == 0)) {
          BX_SER_THIS devices->pic->lower_irq(4);
        }
        BX_SER_THIS s[0].rx_interrupt = 0;
        BX_SER_THIS s[0].rx_ipending = 0;
      }
      break;

    case 0x03F9: /* interrupt enable register, or div. latch MSB */
      if (BX_SER_THIS s[0].line_cntl.dlab) {
        val = BX_SER_THIS s[0].divisor_msb;
      } else {
        val = BX_SER_THIS s[0].int_enable.rxdata_enable |
              (BX_SER_THIS s[0].int_enable.txhold_enable  << 1) |
              (BX_SER_THIS s[0].int_enable.rxlstat_enable << 2) |
              (BX_SER_THIS s[0].int_enable.modstat_enable << 3);
      }
      break;

    case 0x03FA: /* interrupt ID register */
      /*
       * Set the interrupt ID based on interrupt source
       */
      if (BX_SER_THIS s[0].ls_interrupt) {
        BX_SER_THIS s[0].int_ident.int_ID = 0x3;
        BX_SER_THIS s[0].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[0].rx_interrupt) {
        BX_SER_THIS s[0].int_ident.int_ID = 0x2;
        BX_SER_THIS s[0].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[0].tx_interrupt) {
        BX_SER_THIS s[0].int_ident.int_ID = 0x1;
        BX_SER_THIS s[0].int_ident.ipending = 0;
      } else if (BX_SER_THIS s[0].ms_interrupt) {
        BX_SER_THIS s[0].int_ident.int_ID = 0x0;
        BX_SER_THIS s[0].int_ident.ipending = 0;
      } else {
        BX_SER_THIS s[0].int_ident.int_ID = 0x0;
        BX_SER_THIS s[0].int_ident.ipending = 1;
      }

      /* If there are no more ints pending, clear the irq */
      if ((BX_SER_THIS s[0].rx_interrupt == 0) &&
          (BX_SER_THIS s[0].ls_interrupt == 0) &&
          (BX_SER_THIS s[0].ms_interrupt == 0)) {
        BX_SER_THIS devices->pic->lower_irq(4);
      }

      BX_SER_THIS s[0].tx_interrupt = 0;
      BX_SER_THIS s[0].tx_ipending = 0;

      val = BX_SER_THIS s[0].int_ident.ipending  |
            (BX_SER_THIS s[0].int_ident.int_ID << 1) |
            (BX_SER_THIS s[0].int_ident.fifo_enabled << 6);
      break;

    case 0x03FB: /* Line control register */
      val = BX_SER_THIS s[0].line_cntl.wordlen_sel       |
	(BX_SER_THIS s[0].line_cntl.stopbits       << 2) |
	(BX_SER_THIS s[0].line_cntl.parity_enable  << 3) |
	(BX_SER_THIS s[0].line_cntl.evenparity_sel << 4) |
	(BX_SER_THIS s[0].line_cntl.stick_parity   << 5) |
	(BX_SER_THIS s[0].line_cntl.break_cntl     << 6) |
	(BX_SER_THIS s[0].line_cntl.dlab           << 7);
      break;

    case 0x03FC: /* MODEM control register */
      val = BX_SER_THIS s[0].modem_cntl.dtr |
            (BX_SER_THIS s[0].modem_cntl.rts << 1) |
            (BX_SER_THIS s[0].modem_cntl.out1 << 2) |
            (BX_SER_THIS s[0].modem_cntl.out2 << 3) |
            (BX_SER_THIS s[0].modem_cntl.local_loopback << 4);
      break;

    case 0x03FD: /* Line status register */
      val = BX_SER_THIS s[0].line_status.rxdata_ready     |
	(BX_SER_THIS s[0].line_status.overrun_error  << 1) |
	(BX_SER_THIS s[0].line_status.parity_error   << 2) |
	(BX_SER_THIS s[0].line_status.framing_error  << 3) |
	(BX_SER_THIS s[0].line_status.break_int      << 4) |
	(BX_SER_THIS s[0].line_status.txhold_empty   << 5) |
	(BX_SER_THIS s[0].line_status.txtransm_empty << 6) |
	(BX_SER_THIS s[0].line_status.fifo_error     << 7);
      BX_SER_THIS s[0].line_status.overrun_error = 0;
      BX_SER_THIS s[0].line_status.break_int = 0;
      /* If there are no more ints pending, clear the irq */
      if ((BX_SER_THIS s[0].rx_interrupt == 0) &&
          (BX_SER_THIS s[0].tx_interrupt == 0) &&
          (BX_SER_THIS s[0].ms_interrupt == 0)) {
        BX_SER_THIS devices->pic->lower_irq(4);
      }
      BX_SER_THIS s[0].ls_interrupt = 0;
      BX_SER_THIS s[0].ls_ipending = 0;
      break;

    case 0x03FE: /* MODEM status register */
      val = BX_SER_THIS s[0].modem_status.delta_cts       |
	(BX_SER_THIS s[0].modem_status.delta_dsr    << 1) |
	(BX_SER_THIS s[0].modem_status.ri_trailedge << 2) |
	(BX_SER_THIS s[0].modem_status.delta_dcd    << 3) |
	(BX_SER_THIS s[0].modem_status.cts          << 4) |
	(BX_SER_THIS s[0].modem_status.dsr          << 5) |
	(BX_SER_THIS s[0].modem_status.ri           << 6) |
	(BX_SER_THIS s[0].modem_status.dcd          << 7);
      BX_SER_THIS s[0].modem_status.delta_cts = 0;
      BX_SER_THIS s[0].modem_status.delta_dsr = 0;
      BX_SER_THIS s[0].modem_status.ri_trailedge = 0;
      BX_SER_THIS s[0].modem_status.delta_dcd = 0;
      /* If there are no more ints pending, clear the irq */
      if ((BX_SER_THIS s[0].rx_interrupt == 0) &&
          (BX_SER_THIS s[0].tx_interrupt == 0) &&
          (BX_SER_THIS s[0].ls_interrupt == 0)) {
        BX_SER_THIS devices->pic->lower_irq(4);
      }
      BX_SER_THIS s[0].ms_interrupt = 0;
      BX_SER_THIS s[0].ms_ipending = 0;
      break;

    case 0x03FF: /* scratch register */
      val = BX_SER_THIS s[0].scratch;
      break;

    default:
      val = 0; // keep compiler happy
      BX_PANIC(("unsupported io read from address=%0x%x!",
        (unsigned) address));
      break;
  }

  BX_DEBUG(("val =  0x%x", (unsigned) val));

  return(val);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

void
bx_serial_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_SER_SMF
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void
bx_serial_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SER_SMF
  Boolean prev_cts, prev_dsr, prev_ri, prev_dcd;
  Boolean gen_int = 0;

  BX_DEBUG(("write: 0x%x <- %d",address,value));

  /* SERIAL PORT 1 */

  if (io_len > 1)
    BX_PANIC(("io write to address %08x len=%u",
             (unsigned) address, (unsigned) io_len));

  BX_DEBUG(("write to address: 0x%x = 0x%x",
	      (unsigned) address, (unsigned) value));

  switch (address) {
    case 0x03F8: /* transmit buffer, or divisor latch LSB if DLAB set */
      if (BX_SER_THIS s[0].line_cntl.dlab) {
	BX_SER_THIS s[0].divisor_lsb = value;

	if (value != 0) {
	  BX_SER_THIS s[0].baudrate = (int) (BX_PC_CLOCK_XTL /
			  (16 * ((BX_SER_THIS s[0].divisor_msb << 8) |
				 BX_SER_THIS s[0].divisor_lsb)));
#if USE_RAW_SERIAL
	  BX_SER_THIS raw->set_baudrate(BX_SER_THIS s[0].baudrate);
#endif // USE_RAW_SERIAL
	}
      } else {
        if (BX_SER_THIS s[0].tx_empty) {
          BX_SER_THIS s[0].tx_empty = 0;
          BX_SER_THIS s[0].line_status.txtransm_empty = 0;
          BX_SER_THIS s[0].line_status.txhold_empty = 0;
          /* If there are no more ints pending, clear the irq */
          if ((BX_SER_THIS s[0].rx_interrupt == 0) &&
              (BX_SER_THIS s[0].ls_interrupt == 0) &&
              (BX_SER_THIS s[0].ms_interrupt == 0)) {
            BX_SER_THIS devices->pic->lower_irq(4);
          }
          BX_SER_THIS s[0].tx_interrupt = 0;
          BX_SER_THIS s[0].tx_ipending = 0;
          Bit8u bitmask = 0xff >> (3 - BX_SER_THIS s[0].line_cntl.wordlen_sel);
          BX_SER_THIS s[0].txbuffer = value & bitmask;
          bx_pc_system.activate_timer(BX_SER_THIS s[0].tx_timer_index,
                                      (int) (1000000.0 / (BX_SER_THIS s[0].baudrate / 8)),
                                      0); /* not continuous */
        } else {
          BX_ERROR(("write to tx hold register when not empty"));
        }
      }
      break;

    case 0x03F9: /* interrupt enable register, or div. latch MSB */
      if (BX_SER_THIS s[0].line_cntl.dlab) {
	BX_SER_THIS s[0].divisor_msb = value;

	if (BX_SER_THIS s[0].divisor_msb != 0 && value != 0) {
	  BX_SER_THIS s[0].baudrate = (int) (BX_PC_CLOCK_XTL /
		       (16 * ((BX_SER_THIS s[0].divisor_msb << 8) |
			      BX_SER_THIS s[0].divisor_lsb)));
#if USE_RAW_SERIAL
	  BX_SER_THIS raw->set_baudrate(BX_SER_THIS s[0].baudrate);
#endif // USE_RAW_SERIAL
	}
      } else {
	BX_SER_THIS s[0].int_enable.rxdata_enable  = value & 0x01;
	BX_SER_THIS s[0].int_enable.txhold_enable  = (value & 0x02) >> 1;
	BX_SER_THIS s[0].int_enable.rxlstat_enable = (value & 0x04) >> 2;
	BX_SER_THIS s[0].int_enable.modstat_enable = (value & 0x08) >> 3;
        if ((BX_SER_THIS s[0].ms_ipending == 1) &&
            (BX_SER_THIS s[0].int_enable.modstat_enable == 1)) {
          BX_SER_THIS s[0].ms_interrupt = 1;
          BX_SER_THIS s[0].ms_ipending = 0;
          gen_int = 1;
        }
        if ((BX_SER_THIS s[0].tx_ipending == 1) &&
            (BX_SER_THIS s[0].int_enable.txhold_enable == 1)) {
          BX_SER_THIS s[0].tx_interrupt = 1;
          BX_SER_THIS s[0].tx_ipending = 0;
          gen_int = 1;
        }
        if ((BX_SER_THIS s[0].rx_ipending == 1) &&
            (BX_SER_THIS s[0].int_enable.rxdata_enable == 1)) {
          BX_SER_THIS s[0].rx_interrupt = 1;
          BX_SER_THIS s[0].rx_ipending = 0;
          gen_int = 1;
        if ((BX_SER_THIS s[0].ls_ipending == 1) &&
            (BX_SER_THIS s[0].int_enable.rxlstat_enable == 1)) {
          BX_SER_THIS s[0].ls_interrupt = 1;
          BX_SER_THIS s[0].ls_ipending = 0;
          gen_int = 1;
        }
        }
        if (gen_int == 1)
          BX_SER_THIS devices->pic->raise_irq(4);
      }
      break;

    case 0x03FA: /* FIFO control register */
      /* Ignore until FIFO mode is supported... */
      break;

    case 0x03FB: /* Line control register */
#if !USE_RAW_SERIAL
      if ((value & 0x3) != 0x3) {
	/* ignore this: this is set by FreeBSD when the console
	   code wants to set DLAB */
      }
#endif // !USE_RAW_SERIAL
#if USE_RAW_SERIAL
      if (BX_SER_THIS s[0].line_cntl.wordlen_sel != (value & 0x3)) {
	    BX_SER_THIS raw->set_data_bits((value & 0x3) + 5);
      }
      if (BX_SER_THIS s[0].line_cntl.stopbits != (value & 0x4) >> 2) {
	    BX_SER_THIS raw->set_stop_bits((value & 0x4 >> 2) ? 2 : 1);
      }
      if (BX_SER_THIS s[0].line_cntl.parity_enable != (value & 0x8) >> 3 ||
	  BX_SER_THIS s[0].line_cntl.evenparity_sel != (value & 0x10) >> 4 ||
	  BX_SER_THIS s[0].line_cntl.stick_parity != (value & 0x20) >> 5) {
	    if (((value & 0x20) >> 5) &&
		((value & 0x8) >> 3))
		  BX_PANIC(("sticky parity set and parity enabled"));
	    BX_SER_THIS raw->set_parity_mode(((value & 0x8) >> 3),
					     ((value & 0x10) >> 4) ? P_EVEN : P_ODD);
      }
      if (BX_SER_THIS s[0].line_cntl.break_cntl && !((value & 0x40) >> 6)) {
	    BX_SER_THIS raw->transmit(C_BREAK);
      }
#endif // USE_RAW_SERIAL

      BX_SER_THIS s[0].line_cntl.wordlen_sel = value & 0x3;
      /* These are ignored, but set them up so they can be read back */
      BX_SER_THIS s[0].line_cntl.stopbits = (value & 0x4) >> 2;
      BX_SER_THIS s[0].line_cntl.parity_enable = (value & 0x8) >> 3;
      BX_SER_THIS s[0].line_cntl.evenparity_sel = (value & 0x10) >> 4;
      BX_SER_THIS s[0].line_cntl.stick_parity = (value & 0x20) >> 5;
      BX_SER_THIS s[0].line_cntl.break_cntl = (value & 0x40) >> 6;
      /* used when doing future writes */
      if (BX_SER_THIS s[0].line_cntl.dlab &&
	  !((value & 0x80) >> 7)) {
	// Start the receive polling process if not already started
	// and there is a valid baudrate. Poll every 4 bit times
	if (BX_SER_THIS s[0].rx_pollstate == BX_SER_RXIDLE &&
	    BX_SER_THIS s[0].baudrate != 0) {
	  BX_SER_THIS s[0].rx_pollstate = BX_SER_RXPOLL;
	  bx_pc_system.activate_timer(BX_SER_THIS s[0].rx_timer_index,
		      (int) (1000000.0 / (BX_SER_THIS s[0].baudrate / 4)),
				      0); /* not continuous */
	}
	BX_DEBUG(("baud rate set - %d", BX_SER_THIS s[0].baudrate));
      }
      BX_SER_THIS s[0].line_cntl.dlab = (value & 0x80) >> 7;
      break;

    case 0x03FC: /* MODEM control register */
      if ((value & 0x01) == 0) {
#if USE_RAW_SERIAL
	BX_SER_THIS raw->send_hangup();
#endif
      }

      BX_SER_THIS s[0].modem_cntl.dtr  = value & 0x01;
      BX_SER_THIS s[0].modem_cntl.rts  = (value & 0x02) >> 1;
      BX_SER_THIS s[0].modem_cntl.out1 = (value & 0x04) >> 2;
      BX_SER_THIS s[0].modem_cntl.out2 = (value & 0x08) >> 3;
      BX_SER_THIS s[0].modem_cntl.local_loopback = (value & 0x10) >> 4;

      if (BX_SER_THIS s[0].modem_cntl.local_loopback) {
        prev_cts = BX_SER_THIS s[0].modem_status.cts;
        prev_dsr = BX_SER_THIS s[0].modem_status.dsr;
        prev_ri  = BX_SER_THIS s[0].modem_status.ri;
        prev_dcd = BX_SER_THIS s[0].modem_status.dcd;
        BX_SER_THIS s[0].modem_status.cts = BX_SER_THIS s[0].modem_cntl.rts;
        BX_SER_THIS s[0].modem_status.dsr = BX_SER_THIS s[0].modem_cntl.dtr;
        BX_SER_THIS s[0].modem_status.ri  = BX_SER_THIS s[0].modem_cntl.out1;
        BX_SER_THIS s[0].modem_status.dcd = BX_SER_THIS s[0].modem_cntl.out2;
        if (BX_SER_THIS s[0].modem_status.cts != prev_cts) {
          BX_SER_THIS s[0].modem_status.delta_cts = 1;
          BX_SER_THIS s[0].ms_ipending = 1;
        }
        if (BX_SER_THIS s[0].modem_status.dsr != prev_dsr) {
          BX_SER_THIS s[0].modem_status.delta_dsr = 1;
          BX_SER_THIS s[0].ms_ipending = 1;
        }
        if (BX_SER_THIS s[0].modem_status.ri != prev_ri)
          BX_SER_THIS s[0].ms_ipending = 1;
        if ((BX_SER_THIS s[0].modem_status.ri == 0) && (prev_ri == 1))
          BX_SER_THIS s[0].modem_status.ri_trailedge = 1;
        if (BX_SER_THIS s[0].modem_status.dcd != prev_dcd) {
          BX_SER_THIS s[0].modem_status.delta_dcd = 1;
          BX_SER_THIS s[0].ms_ipending = 1;
        }
        if ((BX_SER_THIS s[0].ms_ipending == 1) &&
            (BX_SER_THIS s[0].int_enable.modstat_enable == 1)) {
          BX_SER_THIS s[0].ms_interrupt = 1;
          BX_SER_THIS s[0].ms_ipending = 0;
        }
      } else {
        /* set these to 0 for the time being */
        BX_SER_THIS s[0].modem_status.cts = 0;
        BX_SER_THIS s[0].modem_status.dsr = 0;
        BX_SER_THIS s[0].modem_status.ri  = 0;
        BX_SER_THIS s[0].modem_status.dcd = 0;
      }
      break;

    case 0x03FD: /* Line status register */
      BX_ERROR(("write to line status register ignored"));
      break;

    case 0x03FE: /* MODEM status register */
      BX_ERROR(("write to MODEM status register ignored"));
      break;

    case 0x03FF: /* scratch register */
      BX_SER_THIS s[0].scratch = value;
      break;

    default:
      BX_PANIC(("unsupported io write to address=0x%x, value = 0x%x!",
        (unsigned) address, (unsigned) value));
      break;
  }
}

void
bx_serial_c::tx_timer_handler(void *this_ptr)
{
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->tx_timer();
}


void
bx_serial_c::tx_timer(void)
{
  int gen_int = 0;

  BX_SER_THIS s[0].tx_empty = 1;
  BX_SER_THIS s[0].line_status.txtransm_empty = 1;
  BX_SER_THIS s[0].line_status.txhold_empty = 1;
  if (BX_SER_THIS s[0].int_enable.txhold_enable) {
    gen_int = 1;
    BX_SER_THIS s[0].tx_interrupt = 1;
  } else {
    BX_SER_THIS s[0].tx_ipending = 1;
  }

  if (BX_SER_THIS s[0].modem_cntl.local_loopback) {
    if (BX_SER_THIS s[0].rx_empty == 0)
      BX_SER_THIS s[0].line_status.overrun_error = 1;
    BX_SER_THIS s[0].rxbuffer = BX_SER_THIS s[0].txbuffer;
    BX_SER_THIS s[0].line_status.rxdata_ready = 1;
    BX_SER_THIS s[0].rx_empty = 0;
    if (BX_SER_THIS s[0].modem_cntl.out2) {
      if (BX_SER_THIS s[0].int_enable.rxdata_enable) {
        gen_int = 1;
        BX_SER_THIS s[0].rx_interrupt = 1;
      } else {
        BX_SER_THIS s[0].rx_ipending = 1;
      }
      if (BX_SER_THIS s[0].int_enable.rxlstat_enable) {
        gen_int = 1;
        BX_SER_THIS s[0].ls_interrupt = 1;
      } else {
        BX_SER_THIS s[0].ls_ipending = 1;
      }
    }
  } else {
#if defined (USE_TTY_HACK)
    tty(tty_id, 0, & BX_SER_THIS s[0].txbuffer);
#elif USE_RAW_SERIAL
    if (!BX_SER_THIS raw->ready_transmit())
	  BX_PANIC(("Not ready to transmit"));
    BX_SER_THIS raw->transmit(BX_SER_THIS s[0].txbuffer);
#endif
#if defined(SERIAL_ENABLE)
    { char *s = (char *)(BX_SER_THIS s[0].txbuffer);
	BX_DEBUG(("write: '%c'",(bx_ptr_t) & s));
	}
    write(tty_id, (bx_ptr_t) & BX_SER_THIS s[0].txbuffer, 1);
#endif
  }

  if (gen_int) {
    BX_SER_THIS devices->pic->raise_irq(4);
  }
}


void
bx_serial_c::rx_timer_handler(void *this_ptr)
{
  bx_serial_c *class_ptr = (bx_serial_c *) this_ptr;

  class_ptr->rx_timer();
}


void
bx_serial_c::rx_timer(void)
{
#if BX_HAVE_SELECT
#if BX_WITH_BEOS == 0
  struct timeval tval;
  fd_set fds;
#endif
#endif
  int bdrate = BX_SER_THIS s[0].baudrate / 8;
  unsigned char chbuf = 0;

#if BX_HAVE_SELECT
#if BX_WITH_BEOS == 0
  tval.tv_sec  = 0;
  tval.tv_usec = 0;

// MacOS: I'm not sure what to do with this, since I don't know
// what an fd_set is or what FD_SET() or select() do. They aren't
// declared in the CodeWarrior standard library headers. I'm just
// leaving it commented out for the moment.

  FD_SET(tty_id, &fds);

  if (BX_SER_THIS s[0].line_status.rxdata_ready == 0) {
#if defined (USE_TTY_HACK)
    if (tty_prefetch_char(tty_id)) {
      tty(tty_id, 1, &chbuf);
#elif USE_RAW_SERIAL
    Boolean rdy;
    uint16 data;
    if ((rdy = BX_SER_THIS raw->ready_receive())) {
      data = BX_SER_THIS raw->receive();
      if (data == C_BREAK) {
	    BX_DEBUG(("got BREAK"));
	    BX_SER_THIS s[0].line_status.break_int = 1;
	    rdy = 0;
      }
    }
    if (rdy) {
	  chbuf = data;
#elif defined(SERIAL_ENABLE)
    if (select(1, &fds, NULL, NULL, &tval) == 1) {
      (void) read(tty_id, &chbuf, 1);
	  BX_DEBUG(("read: '%c'",chbuf));
#else
    if (0) {
#endif
      if (!BX_SER_THIS s[0].modem_cntl.local_loopback) {

        BX_SER_THIS s[0].rxbuffer = chbuf;
        BX_SER_THIS s[0].line_status.rxdata_ready = 1;
        BX_SER_THIS s[0].rx_empty = 0;
        if (BX_SER_THIS s[0].int_enable.rxdata_enable) {
          BX_SER_THIS s[0].rx_interrupt = 1;
          BX_SER_THIS devices->pic->raise_irq(4);
        } else {
          BX_SER_THIS s[0].rx_ipending = 1;
        }
      }
    } else {
      bdrate = (int) (1000000.0 / 100); // Poll frequency is 100ms
    }
  } else {
    // Poll at 4x baud rate to see if the next-char can
    // be read
    bdrate *= 4;
  }
#endif
#endif

  bx_pc_system.activate_timer(BX_SER_THIS s[0].rx_timer_index,
			      (int) (1000000.0 / bdrate),
			      0); /* not continuous */
}
