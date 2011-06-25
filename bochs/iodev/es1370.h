/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011  The Bochs Project
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

#ifndef BX_IODEV_ES1370_H
#define BX_IODEV_ES1370_H

#if BX_USE_ES1370_SMF
#  define BX_ES1370_SMF  static
#  define BX_ES1370_THIS theES1370Device->
#  define BX_ES1370_THIS_PTR theES1370Device
#else
#  define BX_ES1370_SMF
#  define BX_ES1370_THIS this->
#  define BX_ES1370_THIS_PTR this
#endif

typedef struct {
    Bit32u shift;
    Bit32u leftover;
    Bit32u scount;
    Bit32u frame_addr;
    Bit32u frame_cnt;
} chan_t;

typedef struct {
  chan_t chan[3];
  Bit32u ctl;
  Bit32u status;
  Bit32u mempage;
  Bit32u codec;
  Bit32u sctl;

  int dac1_timer_index;
  int dac2_timer_index;
  bx_bool dac_outputinit;
  bx_bool adc_inputinit;
  int dac_nr_active;
  Bit16u dac_packet_size[2];

  Bit8u devfunc;
} bx_es1370_t;


// forward definition
class bx_sound_lowlevel_c;

class bx_es1370_c : public bx_devmodel_c, bx_pci_device_stub_c {
public:
  bx_es1370_c();
  virtual ~bx_es1370_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

private:
  bx_es1370_t s;

  BX_ES1370_SMF void set_irq_level(bx_bool level);
  BX_ES1370_SMF void update_status(Bit32u new_status);
  BX_ES1370_SMF void check_lower_irq(Bit32u sctl);
  BX_ES1370_SMF void update_voices(Bit32u ctl, Bit32u sctl, bx_bool force);
  BX_ES1370_SMF void run_channel(unsigned channel, int timer_id, Bit32u buflen);

  static void es1370_timer_handler(void *);
  void es1370_timer(void);

  static Bit32u es1370_adc_handler(void *, Bit32u len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_ES1370_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

  bx_sound_lowlevel_c *soundmod;
};

#endif
