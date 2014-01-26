/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef BX_IODEV_SERIAL_H
#define BX_IODEV_SERIAL_H

// Peter Grehan (grehan@iprg.nokia.com) coded most of this
// serial emulation.

#if BX_USE_SER_SMF
#  define BX_SER_SMF  static
#  define BX_SER_THIS theSerialDevice->
#else
#  define BX_SER_SMF
#  define BX_SER_THIS this->
#endif

#ifdef BX_SER_WIN32
#define SERIAL_ENABLE
#endif

#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__APPLE__) || defined(__sun__) || defined(__CYGWIN__)
#define SERIAL_ENABLE
extern "C" {
#include <termios.h>
};
#endif

#define BX_SERIAL_MAXDEV   4

#define  BX_PC_CLOCK_XTL   1843200.0

#define  BX_SER_RXIDLE  0
#define  BX_SER_RXPOLL  1
#define  BX_SER_RXWAIT  2

#define BX_SER_THR  0
#define BX_SER_RBR  0
#define BX_SER_IER  1
#define BX_SER_IIR  2
#define BX_SER_FCR  2
#define BX_SER_LCR  3
#define BX_SER_MCR  4
#define BX_SER_LSR  5
#define BX_SER_MSR  6
#define BX_SER_SCR  7

#define BX_SER_MODE_NULL          0
#define BX_SER_MODE_FILE          1
#define BX_SER_MODE_TERM          2
#define BX_SER_MODE_RAW           3
#define BX_SER_MODE_MOUSE         4
#define BX_SER_MODE_SOCKET_CLIENT 5
#define BX_SER_MODE_SOCKET_SERVER 6
#define BX_SER_MODE_PIPE_CLIENT   7
#define BX_SER_MODE_PIPE_SERVER   8

enum {
  BX_SER_INT_IER,
  BX_SER_INT_RXDATA,
  BX_SER_INT_TXHOLD,
  BX_SER_INT_RXLSTAT,
  BX_SER_INT_MODSTAT,
  BX_SER_INT_FIFO
};

#if USE_RAW_SERIAL
class serial_raw;
#endif

typedef struct {
  /*
   * UART internal state
   */
  bx_bool  ls_interrupt;
  bx_bool  ms_interrupt;
  bx_bool  rx_interrupt;
  bx_bool  tx_interrupt;
  bx_bool  fifo_interrupt;
  bx_bool  ls_ipending;
  bx_bool  ms_ipending;
  bx_bool  rx_ipending;
  bx_bool  fifo_ipending;

  Bit8u IRQ;

  Bit8u rx_fifo_end;
  Bit8u tx_fifo_end;

  int  baudrate;
  Bit32u databyte_usec;

  int  rx_timer_index;
  int  tx_timer_index;
  int  fifo_timer_index;

  int io_mode;
  int tty_id;
  SOCKET socket_id;
  FILE *output;
#ifdef BX_SER_WIN32
  HANDLE pipe;
#endif

#if USE_RAW_SERIAL
  serial_raw* raw;
#endif
#if defined(SERIAL_ENABLE) && !defined(BX_SER_WIN32)
  struct termios term_orig, term_new;
#endif

  /*
   * Register definitions
   */
  Bit8u     rxbuffer;     /* receiver buffer register (r/o) */
  Bit8u     thrbuffer;    /* transmit holding register (w/o) */
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
  } int_ident;
  /* FIFO Control Register (w/o) */
  struct {
    bx_bool    enable;             /* 1=enable tx and rx FIFOs */
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
    bx_bool    thr_empty;          /* 1=tx hold register (or fifo) is empty */
    bx_bool    tsr_empty;          /* 1=shift reg and hold reg empty */
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
  Bit8u  tsrbuffer;     /* transmit shift register (internal) */
  Bit8u  rx_fifo[16];   /* receive FIFO (internal) */
  Bit8u  tx_fifo[16];   /* transmit FIFO (internal) */
  Bit8u  divisor_lsb;   /* Divisor latch, least-sig. byte */
  Bit8u  divisor_msb;   /* Divisor latch, most-sig. byte */
} bx_serial_t;



class bx_serial_c : public bx_devmodel_c {
public:
  bx_serial_c();
  virtual ~bx_serial_c();
  virtual void init(void);
  virtual void reset(unsigned type) {}
  virtual void register_state(void);

private:
  bx_serial_t s[BX_SERIAL_MAXDEV];

  int   detect_mouse;
  int   mouse_port;
  int   mouse_type;
  int   mouse_delayed_dx;
  int   mouse_delayed_dy;
  int   mouse_delayed_dz;
  Bit8u mouse_buttons;
  bx_bool mouse_update;
  struct {
    int     num_elements;
    Bit8u   buffer[BX_MOUSE_BUFF_SIZE];
    int     head;
  } mouse_internal_buffer;

  static void lower_interrupt(Bit8u port);
  static void raise_interrupt(Bit8u port, int type);

  static void rx_fifo_enq(Bit8u port, Bit8u data);

  static void tx_timer_handler(void *);
  BX_SER_SMF void tx_timer(void);

  static void rx_timer_handler(void *);
  BX_SER_SMF void rx_timer(void);

  static void fifo_timer_handler(void *);
  BX_SER_SMF void fifo_timer(void);

  static void mouse_enq_static(void *dev, int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);
  void mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state, bx_bool absxy);
  void update_mouse_data(void);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_SER_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
