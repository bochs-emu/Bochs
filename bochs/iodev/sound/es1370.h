/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// ES1370 soundcard support (ported from QEMU)
//
// Copyright (c) 2005  Vassili Karpov (malc)
// Copyright (C) 2011-2013  The Bochs Project
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/////////////////////////////////////////////////////////////////////////

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

#define BX_ES1370_CODEC_REGS 0x1a

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
  Bit8u codec_index;
  Bit8u codec_reg[BX_ES1370_CODEC_REGS];
  Bit16u wave_vol;
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

  // runtime options
  static Bit64s es1370_param_handler(bx_param_c *param, int set, Bit64s val);
  static const char* es1370_param_string_handler(bx_param_string_c *param, int set,
                                                 const char *oldval, const char *val,
                                                 int maxlen);
  static void runtime_config_handler(void *);
  void runtime_config(void);

private:
  bx_es1370_t s;

  BX_ES1370_SMF void set_irq_level(bx_bool level);
  BX_ES1370_SMF void update_status(Bit32u new_status);
  BX_ES1370_SMF void check_lower_irq(Bit32u sctl);
  BX_ES1370_SMF void update_voices(Bit32u ctl, Bit32u sctl, bx_bool force);
  BX_ES1370_SMF void run_channel(unsigned channel, int timer_id, Bit32u buflen);
  BX_ES1370_SMF void sendwavepacket(unsigned channel, Bit32u buflen, Bit8u *buffer);
  BX_ES1370_SMF void closewaveoutput();

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
  int wavemode;
  bx_bool wave_changed;
  FILE *wavefile;
};

#endif
