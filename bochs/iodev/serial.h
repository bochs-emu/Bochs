/////////////////////////////////////////////////////////////////////////
// $Id: serial.h,v 1.7 2002-08-27 19:54:46 bdenney Exp $
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
// serial emulation.

#if USE_RAW_SERIAL
#include "serial_raw.h"
#endif // USE_RAW_SERIAL

#if BX_USE_SER_SMF
#  define BX_SER_SMF  static
#  define BX_SER_THIS bx_serial.
#else
#  define BX_SER_SMF
#  define BX_SER_THIS this->
#endif

#define BX_SERIAL_MAXDEV   4
#define BX_SERIAL_CONFDEV  1   /* only 1 serial port currently */

#define  BX_PC_CLOCK_XTL   1843200.0

#define  BX_SER_RXIDLE  0
#define  BX_SER_RXPOLL  1
#define  BX_SER_RXWAIT  2

typedef struct {
  /*
   * UART internal state
   */
  Boolean  rx_empty;
  Boolean  tx_empty;
  Boolean  ls_interrupt;
  Boolean  ms_interrupt;
  Boolean  rx_interrupt;
  Boolean  tx_interrupt;
  Boolean  ls_ipending;
  Boolean  ms_ipending;
  Boolean  rx_ipending;
  Boolean  tx_ipending;

  int  baudrate;
  int  tx_timer_index;

  int  rx_pollstate;
  int  rx_timer_index;

#define RX_CB_SIZE  80
  int  rx_cb_start;
  int  rx_cb_end;
  unsigned char rx_cbuf[RX_CB_SIZE];

  /*
   * Register definitions
   */
  Bit8u     rxbuffer;     /* receiver buffer register (r/o) */
  Bit8u     txbuffer;     /* transmit holding register (w/o) */
  /* Interrupt Enable Register */
  struct {
    Boolean    rxdata_enable;      /* 1=enable receive data interrupts */
    Boolean    txhold_enable;      /* 1=enable tx. holding reg. empty ints */
    Boolean    rxlstat_enable;     /* 1=enable rx line status interrupts */
    Boolean    modstat_enable;     /* 1=enable modem status interrupts */
  } int_enable;
  /* Interrupt Identification Register (r/o) */
  struct {
    Boolean    ipending;           /* 0=interrupt pending */
    Bit8u      int_ID;             /* 3-bit interrupt ID */
    Bit8u      fifo_enabled;       /* 2-bit, set to b11 when FCR0 enabled */
  } int_ident;
  /* FIFO Control Register (w/o) */
  struct {
    Boolean    enable;             /* 1=enable tx and rx FIFOs */
    Boolean    rxreset;            /* 1=clear rx fifo. self-clearing */
    Boolean    txreset;            /* 1=clear tx fifo. self-clearing */
    Boolean    dmamode;            /* 1=DMA mode 1 (unused on PC ?) */
    Bit8u      rxtrigger;          /* 2-bit code for rx fifo trigger level */
  } fifo_cntl;
  /* Line Control Register (r/w) */
  struct {
    Bit8u      wordlen_sel;        /* 2-bit code for char length */
    Boolean    stopbits;           /* select stop bit len */
    Boolean    parity_enable;      /* ... */
    Boolean    evenparity_sel;     /* ... */
    Boolean    stick_parity;       /* ... */
    Boolean    break_cntl;         /* 1=send break signal */
    Boolean    dlab;               /* divisor latch access bit */
  } line_cntl;
  /* MODEM Control Register (r/w) */
  struct {
    Boolean    dtr;                /* DTR output value */
    Boolean    rts;                /* RTS output value */
    Boolean    out1;               /* OUTPUT1 value */
    Boolean    out2;               /* OUTPUT2 value */
    Boolean    local_loopback;     /* 1=loopback mode */
  } modem_cntl;
  /* Line Status Register (r/w) */
  struct {
    Boolean    rxdata_ready;       /* 1=receiver data ready */
    Boolean    overrun_error;      /* 1=receive overrun detected */
    Boolean    parity_error;       /* 1=rx char has a bad parity bit */
    Boolean    framing_error;      /* 1=no stop bit detected for rx char */
    Boolean    break_int;          /* 1=break signal detected */
    Boolean    txhold_empty;       /* 1=tx hold register (or fifo) is empty */
    Boolean    txtransm_empty;     /* 1=shift reg and hold reg empty */
    Boolean    fifo_error;         /* 1=at least 1 err condition in fifo */
  } line_status;
  /* Modem Status Register (r/w) */
  struct {
    Boolean    delta_cts;          /* 1=CTS changed since last read */
    Boolean    delta_dsr;          /* 1=DSR changed since last read */
    Boolean    ri_trailedge;       /* 1=RI moved from low->high */
    Boolean    delta_dcd;          /* 1=CD changed since last read */
    Boolean    cts;                /* CTS input value */
    Boolean    dsr;                /* DSR input value */
    Boolean    ri;                 /* RI input value */
    Boolean    dcd;                /* DCD input value */
  } modem_status;

  Bit8u  scratch;       /* Scratch Register (r/w) */
  Bit8u  divisor_lsb;   /* Divisor latch, least-sig. byte */
  Bit8u  divisor_msb;   /* Divisor latch, most-sig. byte */
} bx_serial_t;



class bx_serial_c : public logfunctions {
public:
  bx_serial_c(void);
  ~bx_serial_c(void);
  BX_SER_SMF void   init(bx_devices_c *);
  BX_SER_SMF void   reset(unsigned type);
#if USE_RAW_SERIAL
  serial_raw* raw;
#endif // USE_RAW_SERIAL

private:
    bx_serial_t s[BX_SERIAL_MAXDEV];

  bx_devices_c *devices;

  static void tx_timer_handler(void *);
  BX_SER_SMF void tx_timer(void);

  static void rx_timer_handler(void *);
  BX_SER_SMF void rx_timer(void);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_SER_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  };


extern bx_serial_c bx_serial;
