/////////////////////////////////////////////////////////////////////////
// $Id: serial.h,v 1.9 2002-10-25 11:44:41 bdenney Exp $
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
#  define BX_SER_THIS theSerialDevice->
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
  bx_bool  rx_empty;
  bx_bool  tx_empty;
  bx_bool  ls_interrupt;
  bx_bool  ms_interrupt;
  bx_bool  rx_interrupt;
  bx_bool  tx_interrupt;
  bx_bool  ls_ipending;
  bx_bool  ms_ipending;
  bx_bool  rx_ipending;
  bx_bool  tx_ipending;

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
    bx_bool    rxdata_enable;      /* 1=enable receive data interrupts */
    bx_bool    txhold_enable;      /* 1=enable tx. holding reg. empty ints */
    bx_bool    rxlstat_enable;     /* 1=enable rx line status interrupts */
    bx_bool    modstat_enable;     /* 1=enable modem status interrupts */
  } int_enable;
  /* Interrupt Identification Register (r/o) */
  struct {
    bx_bool    ipending;           /* 0=interrupt pending */
    Bit8u      int_ID;             /* 3-bit interrupt ID */
    Bit8u      fifo_enabled;       /* 2-bit, set to b11 when FCR0 enabled */
  } int_ident;
  /* FIFO Control Register (w/o) */
  struct {
    bx_bool    enable;             /* 1=enable tx and rx FIFOs */
    bx_bool    rxreset;            /* 1=clear rx fifo. self-clearing */
    bx_bool    txreset;            /* 1=clear tx fifo. self-clearing */
    bx_bool    dmamode;            /* 1=DMA mode 1 (unused on PC ?) */
    Bit8u      rxtrigger;          /* 2-bit code for rx fifo trigger level */
  } fifo_cntl;
  /* Line Control Register (r/w) */
  struct {
    Bit8u      wordlen_sel;        /* 2-bit code for char length */
    bx_bool    stopbits;           /* select stop bit len */
    bx_bool    parity_enable;      /* ... */
    bx_bool    evenparity_sel;     /* ... */
    bx_bool    stick_parity;       /* ... */
    bx_bool    break_cntl;         /* 1=send break signal */
    bx_bool    dlab;               /* divisor latch access bit */
  } line_cntl;
  /* MODEM Control Register (r/w) */
  struct {
    bx_bool    dtr;                /* DTR output value */
    bx_bool    rts;                /* RTS output value */
    bx_bool    out1;               /* OUTPUT1 value */
    bx_bool    out2;               /* OUTPUT2 value */
    bx_bool    local_loopback;     /* 1=loopback mode */
  } modem_cntl;
  /* Line Status Register (r/w) */
  struct {
    bx_bool    rxdata_ready;       /* 1=receiver data ready */
    bx_bool    overrun_error;      /* 1=receive overrun detected */
    bx_bool    parity_error;       /* 1=rx char has a bad parity bit */
    bx_bool    framing_error;      /* 1=no stop bit detected for rx char */
    bx_bool    break_int;          /* 1=break signal detected */
    bx_bool    txhold_empty;       /* 1=tx hold register (or fifo) is empty */
    bx_bool    txtransm_empty;     /* 1=shift reg and hold reg empty */
    bx_bool    fifo_error;         /* 1=at least 1 err condition in fifo */
  } line_status;
  /* Modem Status Register (r/w) */
  struct {
    bx_bool    delta_cts;          /* 1=CTS changed since last read */
    bx_bool    delta_dsr;          /* 1=DSR changed since last read */
    bx_bool    ri_trailedge;       /* 1=RI moved from low->high */
    bx_bool    delta_dcd;          /* 1=CD changed since last read */
    bx_bool    cts;                /* CTS input value */
    bx_bool    dsr;                /* DSR input value */
    bx_bool    ri;                 /* RI input value */
    bx_bool    dcd;                /* DCD input value */
  } modem_status;

  Bit8u  scratch;       /* Scratch Register (r/w) */
  Bit8u  divisor_lsb;   /* Divisor latch, least-sig. byte */
  Bit8u  divisor_msb;   /* Divisor latch, most-sig. byte */
} bx_serial_t;



class bx_serial_c : public bx_devmodel_c {
public:
  bx_serial_c(void);
  ~bx_serial_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);
#if USE_RAW_SERIAL
  serial_raw* raw;
#endif // USE_RAW_SERIAL

private:
    bx_serial_t s[BX_SERIAL_MAXDEV];

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

