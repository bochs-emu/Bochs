/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2025  The Bochs Project
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

#ifndef BX_IODEV_PIC_H
#define BX_IODEV_PIC_H

#if BX_USE_PIC_SMF
#  define BX_PIC_SMF  static
#  define BX_PIC_THIS thePic->
#else
#  define BX_PIC_SMF
#  define BX_PIC_THIS this->
#endif

typedef struct {
  bool master;
  Bit8u interrupt_offset;  /* programmable interrupt vector offset */
  Bit8u sfnm;              /* specially fully nested mode: 0=no, 1=yes*/
  Bit8u buffered_mode;     /* 0=no buffered mode, 1=buffered mode */
  Bit8u master_slave;      /* master/slave: 0=slave PIC, 1=master PIC */
  Bit8u auto_eoi;          /* 0=manual EOI, 1=automatic EOI */
  Bit8u imr;               /* interrupt mask register, 1=masked */
  Bit8u isr;               /* in service register */
  Bit8u irr;               /* interrupt request register */
  Bit8u read_reg_select;   /* 0=IRR, 1=ISR */
  Bit8u irq;               /* current IRQ number */
  Bit8u lowest_priority;   /* current lowest priority irq */
  bool INT;                /* INT request pin of PIC */
  Bit8u IRQ_in[8];         /* IRQ pins of PIC */
  struct {
    bool  in_init;
    bool  requires_4;
    Bit8u byte_expected;
  } init;
  bool special_mask;
  bool polled;             /* Set when poll command is issued. */
  bool rotate_on_autoeoi;  /* Set when should rotate in auto-eoi mode. */
  Bit8u edge_level; /* bitmap for irq mode (0=edge, 1=level) */
} bx_pic_t;


class bx_pic_c : public bx_pic_stub_c {
public:
  bx_pic_c();
  virtual ~bx_pic_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void lower_irq(unsigned irq_no, Bit8u irq_type);
  virtual void raise_irq(unsigned irq_no, Bit8u irq_type);
  virtual void set_irq_level(unsigned irq_no, Bit8u irq_type, bool level);
  virtual void set_mode(bool ma_sl, Bit8u mode);
  virtual Bit8u IAC(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif
  virtual void register_state(void);

private:
  struct {
    bx_pic_t master_pic;
    bx_pic_t slave_pic;
  } s;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PIC_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  BX_PIC_SMF void   pic_service(bx_pic_t *pic);
  BX_PIC_SMF void   clear_highest_interrupt(bx_pic_t *pic);
};

#endif
