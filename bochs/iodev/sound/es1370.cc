/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// ES1370 soundcard support (ported from QEMU)
//
// Copyright (c) 2005  Vassili Karpov (malc)
// Copyright (C) 2011-2018  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_ES1370

#include "soundlow.h"
#include "soundmod.h"
#include "pci.h"
#include "es1370.h"

#include <math.h>

#define LOG_THIS theES1370Device->

bx_es1370_c* theES1370Device = NULL;

const Bit8u es1370_iomask[64] = {7, 1, 3, 1, 7, 1, 3, 1, 1, 3, 1, 0, 7, 0, 0, 0,
                                 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                 7, 1, 3, 1, 6, 0, 2, 0, 6, 0, 2, 0, 6, 0, 2, 0,
                                 4, 0, 0, 0, 6, 0, 2, 0, 4, 0, 0, 0, 6, 0, 2, 0};

static const Bit8u midi_eventlength[] = { 2, 2, 2, 2, 1, 1, 2, 255};

#define ES1370_CTL            0x00
#define ES1370_STATUS         0x04
#define ES1370_UART_DATA      0x08
#define ES1370_UART_STATUS    0x09
#define ES1370_UART_CTL       0x09
#define ES1370_UART_TEST      0x0a
#define ES1370_MEMPAGE        0x0c
#define ES1370_CODEC          0x10
#define ES1370_SCTL           0x20
#define ES1370_DAC1_SCOUNT    0x24
#define ES1370_DAC2_SCOUNT    0x28
#define ES1370_ADC_SCOUNT     0x2c

#define ES1370_DAC1_FRAMEADR  0xc30
#define ES1370_DAC1_FRAMECNT  0xc34
#define ES1370_DAC2_FRAMEADR  0xc38
#define ES1370_DAC2_FRAMECNT  0xc3c
#define ES1370_ADC_FRAMEADR   0xd30
#define ES1370_ADC_FRAMECNT   0xd34
#define ES1370_PHA_FRAMEADR   0xd38
#define ES1370_PHA_FRAMECNT   0xd3c

#define STAT_INTR       0x80000000
#define STAT_DAC1       0x00000004
#define STAT_DAC2       0x00000002
#define STAT_ADC        0x00000001

#define SCTRL_R1INTEN     0x00000400
#define SCTRL_P2INTEN     0x00000200
#define SCTRL_P1INTEN     0x00000100

#define DAC1_CHANNEL 0
#define DAC2_CHANNEL 1
#define ADC_CHANNEL  2

#define BX_ES1370_WAVEOUT1 BX_ES1370_THIS waveout[0]
#define BX_ES1370_WAVEOUT2 BX_ES1370_THIS waveout[1]
#define BX_ES1370_WAVEIN   BX_ES1370_THIS wavein
#define BX_ES1370_MIDIOUT1 BX_ES1370_THIS midiout[0]
#define BX_ES1370_MIDIOUT2 BX_ES1370_THIS midiout[1]

const char chan_name[3][5] = {"DAC1", "DAC2", "ADC"};
const Bit16u dac1_freq[4] = {5512, 11025, 22050, 44100};
const Bit16u ctl_ch_en[3] = {0x0040, 0x0020, 0x0010};
const Bit16u sctl_ch_pause[3] = {0x0800, 0x1000, 0x0000};
const Bit16u sctl_loop_sel[3] = {0x2000, 0x4000, 0x8000};

// builtin configuration handling functions

void es1370_init_options(void)
{
  static const char *es1370_mode_list[] = {
    "0",
    "1",
    "2",
    "3",
    NULL
  };

  bx_param_c *sound = SIM->get_param("sound");
  bx_list_c *menu = new bx_list_c(sound, "es1370", "ES1370 Configuration");
  menu->set_options(menu->SHOW_PARENT);
  menu->set_enabled(BX_SUPPORT_ES1370);

  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable ES1370 emulation",
    "Enables the ES1370 emulation",
    1);
  enabled->set_enabled(BX_SUPPORT_ES1370);

  bx_param_enum_c *midimode = new bx_param_enum_c(menu,
    "midimode",
    "Midi mode",
    "Controls the MIDI output switches.",
    es1370_mode_list,
    0, 0);
  bx_param_filename_c *midifile = new bx_param_filename_c(menu,
    "midifile",
    "MIDI file",
    "The filename is where the MIDI data is sent to in mode 2 or 3.",
    "", BX_PATHNAME_LEN);

  bx_param_enum_c *wavemode = new bx_param_enum_c(menu,
    "wavemode",
    "Wave mode",
    "Controls the wave output switches.",
    es1370_mode_list,
    0, 0);
  bx_param_filename_c *wavefile = new bx_param_filename_c(menu,
    "wavefile",
    "Wave file",
    "This is the file where the wave output is stored",
    "", BX_PATHNAME_LEN);

  bx_list_c *deplist = new bx_list_c(NULL);
  deplist->add(midimode);
  deplist->add(wavemode);
  enabled->set_dependent_list(deplist);
  deplist = new bx_list_c(NULL);
  deplist->add(midifile);
  midimode->set_dependent_list(deplist, 0);
  midimode->set_dependent_bitmap(2, 0x1);
  midimode->set_dependent_bitmap(3, 0x1);
  deplist = new bx_list_c(NULL);
  deplist->add(wavefile);
  wavemode->set_dependent_list(deplist, 0);
  wavemode->set_dependent_bitmap(2, 0x1);
  wavemode->set_dependent_bitmap(3, 0x1);
}

Bit32s es1370_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "es1370")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "wavedev=", 8)) {
        BX_ERROR(("%s: wave device now specified with the 'sound' option.", context));
      } else if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for es1370 ignored.", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s es1370_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370), NULL, 0);
}

// device plugin entry points

int CDECL libes1370_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
{
  theES1370Device = new bx_es1370_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theES1370Device, BX_PLUGIN_ES1370);
  // add new configuration parameter for the config interface
  es1370_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("es1370", es1370_options_parser, es1370_options_save);
  return 0; // Success
}

void CDECL libes1370_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("es1370");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("sound");
  menu->remove("es1370");
  delete theES1370Device;
}

// the device object

bx_es1370_c::bx_es1370_c()
{
  put("ES1370");
  memset(&s, 0, sizeof(bx_es1370_t));
  s.dac1_timer_index = BX_NULL_TIMER_HANDLE;
  s.dac2_timer_index = BX_NULL_TIMER_HANDLE;
  s.mpu_timer_index = BX_NULL_TIMER_HANDLE;
  waveout[0] = NULL;
  waveout[1] = NULL;
  wavein = NULL;
  midiout[0] = NULL;
  midiout[1] = NULL;
  wavemode = 0;
  midimode = 0;
  s.rt_conf_id = -1;
}

bx_es1370_c::~bx_es1370_c()
{
  closemidioutput();
  closewaveoutput();

  SIM->unregister_runtime_config_handler(s.rt_conf_id);
  SIM->get_bochs_root()->remove("es1370");
  bx_list_c *misc_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_MISC);
  misc_rt->remove("es1370");
  BX_DEBUG(("Exit"));
}

void bx_es1370_c::init(void)
{
  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("ES1370 disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("es1370"))->set(0);
    return;
  }
  BX_ES1370_THIS s.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_ES1370_THIS s.devfunc, BX_PLUGIN_ES1370,
                            "ES1370 soundcard");

  // initialize readonly registers
  init_pci_conf(0x1274, 0x5000, 0x00, 0x040100, 0x00, BX_PCI_INTA);

  BX_ES1370_THIS init_bar_io(0, 64, read_handler, write_handler, &es1370_iomask[0]);

  BX_ES1370_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
  BX_ES1370_THIS midimode = SIM->get_param_enum("midimode", base)->get();

  // always initialize lowlevel driver
  BX_ES1370_WAVEOUT1 = DEV_sound_get_waveout(0);
  if (BX_ES1370_WAVEOUT1 == NULL) {
    BX_PANIC(("Couldn't initialize waveout driver"));
  }
  if (BX_ES1370_THIS wavemode & 2) {
    BX_ES1370_WAVEOUT2 = DEV_sound_get_waveout(1);
    if (BX_ES1370_WAVEOUT2 == NULL) {
      BX_PANIC(("Couldn't initialize wave file driver"));
    }
  }
  BX_ES1370_WAVEIN = DEV_sound_get_wavein();
  if (BX_ES1370_WAVEIN == NULL) {
    BX_PANIC(("Couldn't initialize wavein driver"));
  }
  BX_ES1370_MIDIOUT1 = DEV_sound_get_midiout(0);
  if (BX_ES1370_MIDIOUT1 == NULL) {
    BX_PANIC(("Couldn't initialize midiout driver"));
  }
  if (BX_ES1370_THIS midimode & 2) {
    BX_ES1370_MIDIOUT2 = DEV_sound_get_midiout(1);
    if (BX_ES1370_MIDIOUT2 == NULL) {
      BX_PANIC(("Couldn't initialize midi file driver"));
    }
  }

  BX_ES1370_THIS s.dac_outputinit = (BX_ES1370_THIS wavemode & 1);
  BX_ES1370_THIS s.dac_nr_active = -1;
  BX_ES1370_THIS s.adc_inputinit = 0;
  BX_ES1370_THIS s.mpu_outputinit = (BX_ES1370_THIS midimode & 1);

  if (BX_ES1370_THIS s.dac1_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ES1370_THIS s.dac1_timer_index = DEV_register_timer
      (BX_ES1370_THIS_PTR, es1370_timer_handler, 1, 1, 0, "es1370.dac1");
    // DAC1 timer: inactive, continuous, frequency variable
    bx_pc_system.setTimerParam(BX_ES1370_THIS s.dac1_timer_index, 0);
  }
  if (BX_ES1370_THIS s.dac2_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ES1370_THIS s.dac2_timer_index = DEV_register_timer
      (BX_ES1370_THIS_PTR, es1370_timer_handler, 1, 1, 0, "es1370.dac2");
    // DAC2 timer: inactive, continuous, frequency variable
    bx_pc_system.setTimerParam(BX_ES1370_THIS s.dac2_timer_index, 1);
  }
  if (BX_ES1370_THIS s.mpu_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ES1370_THIS s.mpu_timer_index = DEV_register_timer
      (BX_ES1370_THIS_PTR, mpu_timer_handler, 500000 / 384, 1, 1, "es1370.mpu");
    // midi timer: active, continuous, 500000 / 384 seconds (384 = delta time, 500000 = sec per beat at 120 bpm. Don't change this!)
  }
  BX_ES1370_THIS s.mpu_current_timer = 0;
  BX_ES1370_THIS s.last_delta_time = 0xffffffff;
  BX_ES1370_THIS s.midi_command = 0x00;
  BX_ES1370_THIS s.midicmd_len = 0;
  BX_ES1370_THIS s.midicmd_index = 0;

  // init runtime parameters
  bx_list_c *misc_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_MISC);
  bx_list_c *menu = new bx_list_c(misc_rt, "es1370", "ES1370 Runtime Options");
  menu->set_options(menu->SHOW_PARENT | menu->USE_BOX_TITLE);

  menu->add(SIM->get_param("wavemode", base));
  menu->add(SIM->get_param("wavefile", base));
  menu->add(SIM->get_param("midimode", base));
  menu->add(SIM->get_param("midifile", base));
  SIM->get_param_enum("wavemode", base)->set_handler(es1370_param_handler);
  SIM->get_param_string("wavefile", base)->set_handler(es1370_param_string_handler);
  SIM->get_param_num("midimode", base)->set_handler(es1370_param_handler);
  SIM->get_param_string("midifile", base)->set_handler(es1370_param_string_handler);
  // register handler for correct es1370 parameter handling after runtime config
  BX_ES1370_THIS s.rt_conf_id = SIM->register_runtime_config_handler(this, runtime_config_handler);
  BX_ES1370_THIS wave_changed = 0;
  BX_ES1370_THIS midi_changed = 0;

  BX_INFO(("ES1370 initialized"));
}

void bx_es1370_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x04, 0x05 }, { 0x05, 0x00 }, // command_io
    { 0x06, 0x00 }, { 0x07, 0x04 }, // status
    // address space 0x10 - 0x13
    { 0x10, 0x01 }, { 0x11, 0x00 },
    { 0x12, 0x00 }, { 0x13, 0x00 },
    { 0x2c, 0x42 }, { 0x2d, 0x49 }, // subsystem vendor
    { 0x2e, 0x4c }, { 0x2f, 0x4c }, // subsystem id
    { 0x3c, 0x00 },                 // IRQ
    { 0x3e, 0x0c },                 // min_gnt
    { 0x3f, 0x80 },                 // max_lat

  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_ES1370_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  BX_ES1370_THIS s.ctl = 1;
  BX_ES1370_THIS s.status = 0x60;
  BX_ES1370_THIS s.mempage = 0;
  BX_ES1370_THIS s.codec_index = 0;
  for (i = 0; i < BX_ES1370_CODEC_REGS; i++) {
    BX_ES1370_THIS s.codec_reg[i] = 0;
  }
  BX_ES1370_THIS s.wave_vol = 0;
  BX_ES1370_THIS s.sctl = 0;
  BX_ES1370_THIS s.legacy1B = 0;
  for (i = 0; i < 3; i++) {
    BX_ES1370_THIS s.chan[i].scount = 0;
    BX_ES1370_THIS s.chan[i].leftover = 0;
  }

  #ifndef ANDROID
  // Gameport is unsupported on Android
  DEV_gameport_set_enabled(0);
  #endif

  // Deassert IRQ
  set_irq_level(0);
}

void bx_es1370_c::register_state(void)
{
  unsigned i;
  char pname[6];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "es1370", "ES1370 State");
  for (i = 0; i < 3; i++) {
    sprintf(pname, "chan%d", i);
    bx_list_c *chan = new bx_list_c(list, pname, "");
    BXRS_HEX_PARAM_FIELD(chan, shift, BX_ES1370_THIS s.chan[i].shift);
    BXRS_HEX_PARAM_FIELD(chan, leftover, BX_ES1370_THIS s.chan[i].leftover);
    BXRS_HEX_PARAM_FIELD(chan, scount, BX_ES1370_THIS s.chan[i].scount);
    BXRS_HEX_PARAM_FIELD(chan, frame_addr, BX_ES1370_THIS s.chan[i].frame_addr);
    BXRS_HEX_PARAM_FIELD(chan, frame_cnt, BX_ES1370_THIS s.chan[i].frame_cnt);
  }
  BXRS_HEX_PARAM_FIELD(list, ctl, BX_ES1370_THIS s.ctl);
  BXRS_HEX_PARAM_FIELD(list, status, BX_ES1370_THIS s.status);
  BXRS_HEX_PARAM_FIELD(list, mempage, BX_ES1370_THIS s.mempage);
  BXRS_HEX_PARAM_FIELD(list, codec_index, BX_ES1370_THIS s.codec_index);
  new bx_shadow_data_c(list, "codec_regs", BX_ES1370_THIS s.codec_reg,
                       BX_ES1370_CODEC_REGS, 1);
  BXRS_HEX_PARAM_FIELD(list, sctl, BX_ES1370_THIS s.sctl);
  BXRS_HEX_PARAM_FIELD(list, legacy1B, BX_ES1370_THIS s.legacy1B);
  BXRS_HEX_PARAM_FIELD(list, wave_vol, BX_ES1370_THIS s.wave_vol);
  BXRS_DEC_PARAM_FIELD(list, mpu_current_timer, BX_ES1370_THIS s.mpu_current_timer);
  BXRS_DEC_PARAM_FIELD(list, last_delta_time, BX_ES1370_THIS s.last_delta_time);
  BXRS_DEC_PARAM_FIELD(list, midi_command, BX_ES1370_THIS s.midi_command);
  BXRS_DEC_PARAM_FIELD(list, midicmd_len, BX_ES1370_THIS s.midicmd_len);
  BXRS_DEC_PARAM_FIELD(list, midicmd_index, BX_ES1370_THIS s.midicmd_index);
  new bx_shadow_data_c(list, "midi_buffer", BX_ES1370_THIS s.midi_buffer, 256);

  register_pci_state(list);
}

void bx_es1370_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(NULL);
  BX_ES1370_THIS check_lower_irq(BX_ES1370_THIS s.sctl);
  BX_ES1370_THIS s.adc_inputinit = 0;
  BX_ES1370_THIS s.dac_nr_active = -1;
  BX_ES1370_THIS update_voices(BX_ES1370_THIS s.ctl, BX_ES1370_THIS s.sctl, 1);
}

void bx_es1370_c::runtime_config_handler(void *this_ptr)
{
  bx_es1370_c *class_ptr = (bx_es1370_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_es1370_c::runtime_config(void)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
  if (BX_ES1370_THIS wave_changed != 0) {
    if (BX_ES1370_THIS wavemode & 2) {
      BX_ES1370_THIS closewaveoutput();
    }
    if (BX_ES1370_THIS wave_changed & 1) {
      BX_ES1370_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
      BX_ES1370_THIS s.dac_outputinit = (BX_ES1370_THIS wavemode & 1);
      if (BX_ES1370_THIS wavemode & 2) {
        BX_ES1370_WAVEOUT2 = DEV_sound_get_waveout(1);
        if (BX_ES1370_WAVEOUT2 == NULL) {
          BX_PANIC(("Couldn't initialize wave file driver"));
        }
      }
    }
    // update_voices() re-opens the output file on demand
    BX_ES1370_THIS wave_changed = 0;
  }
  if (BX_ES1370_THIS midi_changed != 0) {
    BX_ES1370_THIS closemidioutput();
    if (BX_ES1370_THIS midi_changed & 1) {
      BX_ES1370_THIS midimode = SIM->get_param_num("midimode", base)->get();
      if (BX_ES1370_THIS midimode & 2) {
        BX_ES1370_MIDIOUT2 = DEV_sound_get_midiout(1);
        if (BX_ES1370_MIDIOUT2 == NULL) {
          BX_PANIC(("Couldn't initialize midi file driver"));
        }
      }
    }
    // writemidicommand() re-opens the output device / file on demand
    BX_ES1370_THIS midi_changed = 0;
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_es1370_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_ES1370_SMF
  bx_es1370_c *class_ptr = (bx_es1370_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_es1370_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_ES1370_SMF
  Bit32u val = 0x0, shift;
  Bit16u offset;
  Bit8u index;
  unsigned i;

  BX_DEBUG(("register read from address 0x%04x - ", address));

  offset = address - BX_ES1370_THIS pci_bar[0].addr;
  if (offset >= 0x30) {
    offset |= (BX_ES1370_THIS s.mempage << 8);
  }
  shift = (offset & 3) << 3;

  switch (offset & ~3) {
    case ES1370_CTL:
      val = BX_ES1370_THIS s.ctl >> shift;
      break;
    case ES1370_STATUS:
      val = BX_ES1370_THIS s.status >> shift;
      break;
    case ES1370_UART_DATA:
    case ES1370_UART_STATUS:
    case ES1370_UART_TEST:
      if (offset == ES1370_UART_DATA) {
        BX_ERROR(("reading from UART data register not supported yet"));
      } else if (offset == ES1370_UART_STATUS) {
        BX_DEBUG(("reading from UART status register"));
        val = 0x03;
      } else {
        BX_INFO(("reading from UART test register"));
      }
      break;
    case ES1370_MEMPAGE:
      val = BX_ES1370_THIS s.mempage;
      break;
    case ES1370_CODEC:
      index = BX_ES1370_THIS s.codec_index;
      val = BX_ES1370_THIS s.codec_reg[index] | (index << 8);
      break;
    case ES1370_SCTL:
      val = BX_ES1370_THIS s.sctl >> shift;
      break;
    case ES1370_DAC1_SCOUNT:
    case ES1370_DAC2_SCOUNT:
    case ES1370_ADC_SCOUNT:
      i = (offset - ES1370_DAC1_SCOUNT) >> 2;
      val = BX_ES1370_THIS s.chan[i].scount >> shift;
      break;
    case ES1370_DAC1_FRAMEADR:
      val = BX_ES1370_THIS s.chan[0].frame_addr;
      break;
    case ES1370_DAC2_FRAMEADR:
      val = BX_ES1370_THIS s.chan[1].frame_addr;
      break;
    case ES1370_ADC_FRAMEADR:
      val = BX_ES1370_THIS s.chan[2].frame_addr;
      break;
    case ES1370_DAC1_FRAMECNT:
      val = BX_ES1370_THIS s.chan[0].frame_cnt >> shift;
      break;
    case ES1370_DAC2_FRAMECNT:
      val = BX_ES1370_THIS s.chan[1].frame_cnt >> shift;
      break;
    case ES1370_ADC_FRAMECNT:
      val = BX_ES1370_THIS s.chan[2].frame_cnt >> shift;
      break;
    case ES1370_PHA_FRAMEADR:
      BX_ERROR(("reading from phantom frame address"));
      val = ~0U;
      break;
    case ES1370_PHA_FRAMECNT:
      BX_ERROR(("reading from phantom frame count"));
      val = ~0U;
      break;
    default:
      if (offset == 0x1b) {
        BX_ERROR(("reading from legacy register 0x1b"));
        val = BX_ES1370_THIS s.legacy1B;
      } else if (offset >= 0x30) {
        val = ~0U; // keep compiler happy
        BX_ERROR(("unsupported read from memory offset=0x%02x!",
                  (BX_ES1370_THIS s.mempage << 4) | (offset & 0x0f)));
      } else {
        val = ~0U; // keep compiler happy
        BX_ERROR(("unsupported io read from offset=0x%04x!", offset));
      }
      break;
  }

  BX_DEBUG(("val =  0x%08x", val));

  return(val);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_es1370_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_ES1370_SMF
  bx_es1370_c *class_ptr = (bx_es1370_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_es1370_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_ES1370_SMF
  Bit16u  offset;
  Bit32u shift, mask;
  Bit8u index;
  bx_bool set_wave_vol = 0;
  chan_t *d = &BX_ES1370_THIS s.chan[0];
  unsigned i;

  BX_DEBUG(("register write to address 0x%04x - value = 0x%08x", address, value));

  offset = address - BX_ES1370_THIS pci_bar[0].addr;
  if (offset >= 0x30) {
    offset |= (BX_ES1370_THIS s.mempage << 8);
  }
  shift = (offset & 3) << 3;

  switch (offset & ~3) {
    case ES1370_CTL:
      mask = (0xffffffff >> ((4 - io_len) << 3)) << shift;
      value = (BX_ES1370_THIS s.ctl & ~mask) | ((value << shift) & mask);
      if ((value ^ BX_ES1370_THIS s.ctl) & 0x04) {
        #ifndef ANDROID
        // Gameport is unsupported on Android
        DEV_gameport_set_enabled((value & 0x04) != 0);
        #endif
      }
      BX_ES1370_THIS update_voices(value, BX_ES1370_THIS s.sctl, 0);
      break;
    case ES1370_UART_DATA:
    case ES1370_UART_CTL:
    case ES1370_UART_TEST:
      if (offset == ES1370_UART_DATA) {
        if (value > 0x80) {
          if (BX_ES1370_THIS s.midi_command != 0x00) {
            BX_ERROR(("received new MIDI command while another one is pending"));
          }
          BX_ES1370_THIS s.midi_command = (Bit8u)value;
          BX_ES1370_THIS s.midicmd_len = midi_eventlength[(value & 0x70) >> 4];
          BX_ES1370_THIS s.midicmd_index = 0;
        } else {
          if (BX_ES1370_THIS s.midi_command != 0x00) {
            BX_ES1370_THIS s.midi_buffer[BX_ES1370_THIS s.midicmd_index++] = (Bit8u)value;
            if (BX_ES1370_THIS s.midicmd_index >= BX_ES1370_THIS s.midicmd_len) {
              BX_ES1370_THIS writemidicommand(BX_ES1370_THIS s.midi_command,
                                              BX_ES1370_THIS s.midicmd_len,
                                              BX_ES1370_THIS s.midi_buffer);
              BX_ES1370_THIS s.midi_command = 0x00;
            }
          } else {
            BX_ERROR(("ignoring MIDI data without command pending"));
          }
        }
      } else if (offset == ES1370_UART_CTL) {
        BX_ERROR(("writing to UART control register not supported yet (value=0x%02x)", value & 0xff));
      } else {
        BX_ERROR(("writing to UART test register not supported yet (value=0x%02x)", value & 0xff));
      }
      break;
    case ES1370_MEMPAGE:
      BX_ES1370_THIS s.mempage = value & 0x0f;
      break;
    case ES1370_CODEC:
      index = (value >> 8) & 0xff;
      BX_ES1370_THIS s.codec_index = index;
      if (index < BX_ES1370_CODEC_REGS) {
        BX_ES1370_THIS s.codec_reg[index] = value & 0xff;
        if ((index >= 0) && (index <= 3)) {
          set_wave_vol = 1;
        }
        BX_DEBUG(("writing to CODEC register 0x%02x, value = 0x%02x", index, value & 0xff));
      }
      break;
    case ES1370_SCTL:
      mask = (0xffffffff >> ((4 - io_len) << 3)) << shift;
      value = (BX_ES1370_THIS s.sctl & ~mask) | ((value << shift) & mask);
      BX_ES1370_THIS check_lower_irq(value);
      BX_ES1370_THIS update_voices(BX_ES1370_THIS s.ctl, value, 0);
      break;
    case ES1370_DAC1_SCOUNT:
    case ES1370_DAC2_SCOUNT:
    case ES1370_ADC_SCOUNT:
      i = (offset - ES1370_DAC1_SCOUNT) >> 2;
      value &= 0xffff;
      BX_ES1370_THIS s.chan[i].scount = value | (value << 16);
      break;
    case ES1370_ADC_FRAMEADR:
      d++;
    case ES1370_DAC2_FRAMEADR:
      d++;
    case ES1370_DAC1_FRAMEADR:
      d->frame_addr = value;
      break;
    case ES1370_ADC_FRAMECNT:
      d++;
    case ES1370_DAC2_FRAMECNT:
      d++;
    case ES1370_DAC1_FRAMECNT:
      if ((offset & 3) == 0) {
        d->frame_cnt = value;
        d->leftover = 0;
      }
      break;
    case ES1370_PHA_FRAMEADR:
      BX_ERROR(("writing to phantom frame address"));
      break;
    case ES1370_PHA_FRAMECNT:
      BX_ERROR(("writing to phantom frame count"));
      break;
    default:
      if (offset == 0x1b) {
        BX_ERROR(("writing to legacy register 0x1b (value = 0x%02x)", value & 0xff));
        BX_ES1370_THIS s.legacy1B = (Bit8u)(value & 0xff);
        set_irq_level(BX_ES1370_THIS s.legacy1B & 0x01);
      } else if (offset >= 0x30) {
        BX_ERROR(("unsupported write to memory offset=0x%02x!",
                  (BX_ES1370_THIS s.mempage << 4) | (offset & 0x0f)));
      } else {
        BX_ERROR(("unsupported io write to offset=0x%04x!", offset));
      }
      break;
  }

  if (set_wave_vol) {
    BX_ES1370_THIS s.wave_vol = calc_output_volume(0x00, 0x02, 0);
    BX_ES1370_THIS s.wave_vol |= calc_output_volume(0x01, 0x03, 1);
  }
}

Bit16u bx_es1370_c::calc_output_volume(Bit8u reg1, Bit8u reg2, bx_bool shift)
{
  Bit8u vol1, vol2;
  float fvol1, fvol2;
  Bit16u result;

  vol1 = (0x1f - (BX_ES1370_THIS s.codec_reg[reg1] & 0x1f));
  vol2 = (0x1f - (BX_ES1370_THIS s.codec_reg[reg2] & 0x1f));
  fvol1 = pow(10.0f, (float)(31-vol1)*-0.065f);
  fvol2 = pow(10.0f, (float)(31-vol2)*-0.065f);
  result = (Bit8u)(255 * fvol1 * fvol2);
  if (shift) result <<= 8;
  return result;
}

void bx_es1370_c::es1370_timer_handler(void *this_ptr)
{
  bx_es1370_c *class_ptr = (bx_es1370_c *) this_ptr;
  class_ptr->es1370_timer();
}

void bx_es1370_c::es1370_timer(void)
{
  int timer_id = bx_pc_system.triggeredTimerID();
  unsigned i = bx_pc_system.triggeredTimerParam();
  Bit32u ret = run_channel(i, timer_id, BX_ES1370_THIS s.dac_packet_size[i]);
  if (ret > 0) {
    Bit64u timer_val = (Bit64u)BX_ES1370_THIS s.dac_timer_val[i] * ret / BX_ES1370_THIS s.dac_packet_size[i];
    bx_pc_system.activate_timer(timer_id, (Bit32u)timer_val, 1);
  }
}

void bx_es1370_c::mpu_timer_handler(void *this_ptr)
{
  ((bx_es1370_c *) this_ptr)->s.mpu_current_timer++;
}

Bit32u bx_es1370_c::run_channel(unsigned chan, int timer_id, Bit32u buflen)
{
  Bit32u new_status = BX_ES1370_THIS s.status;
  Bit32u addr, sc, csc_bytes, cnt, size, left, transfered, temp;
  Bit8u tmpbuf[BX_SOUNDLOW_WAVEPACKETSIZE];
  bx_bool irq = 0;

  chan_t *d = &BX_ES1370_THIS s.chan[chan];

  if (!(BX_ES1370_THIS s.ctl & ctl_ch_en[chan]) || (BX_ES1370_THIS s.sctl & sctl_ch_pause[chan])) {
    if (chan == ADC_CHANNEL) {
      BX_ES1370_WAVEIN->stopwaverecord();
    } else {
      bx_pc_system.deactivate_timer(timer_id);
    }
    return 0;
  }

  addr = d->frame_addr;
  sc = d->scount & 0xffff;
  csc_bytes = ((d->scount >> 16) + 1) << d->shift;
  cnt = d->frame_cnt >> 16;
  size = d->frame_cnt & 0xffff;
  left = ((size - cnt + 1) << 2) + d->leftover;
  transfered = 0;
  temp = BX_MIN(buflen, BX_MIN(left, csc_bytes));
  addr += (cnt << 2) + d->leftover;

  if (chan == ADC_CHANNEL) {
    BX_ES1370_WAVEIN->getwavepacket(temp, tmpbuf);
    DEV_MEM_WRITE_PHYSICAL_DMA(addr, temp, tmpbuf);
    transfered = temp;
  } else {
    DEV_MEM_READ_PHYSICAL_DMA(addr, temp, tmpbuf);
    if ((int)chan == BX_ES1370_THIS s.dac_nr_active) {
      BX_ES1370_THIS sendwavepacket(chan, temp, tmpbuf);
    }
    transfered = temp;
  }

  if (csc_bytes == transfered) {
    irq = 1;
    d->scount = sc | (sc << 16);
    BX_DEBUG(("%s: all samples played/recorded - signalling IRQ (if enabled)", chan_name[chan]));
  } else {
    irq = 0;
    d->scount = sc | (((csc_bytes - transfered - 1) >> d->shift) << 16);
  }

  cnt += (transfered + d->leftover) >> 2;

  if (BX_ES1370_THIS s.sctl & sctl_loop_sel[chan]) {
    BX_ERROR(("%s: non looping mode not supported", chan_name[chan]));
  } else {
    d->frame_cnt = size;
    if (cnt <= d->frame_cnt) {
      d->frame_cnt |= cnt << 16;
    }
  }

  d->leftover = (transfered + d->leftover) & 3;

  if (irq) {
    if (BX_ES1370_THIS s.sctl & (1 << (8 + chan))) {
      new_status |= (4 >> chan);
    }
  }
  if (new_status != BX_ES1370_THIS s.status) {
    update_status(new_status);
  }
  return transfered;
}

Bit32u bx_es1370_c::es1370_adc_handler(void *this_ptr, Bit32u buflen)
{
  bx_es1370_c *class_ptr = (bx_es1370_c *) this_ptr;
  class_ptr->run_channel(ADC_CHANNEL, 0, buflen);
  return 0;
}

void bx_es1370_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_ES1370_THIS s.devfunc, BX_ES1370_THIS pci_conf[0x3d], level);
}

void bx_es1370_c::update_status(Bit32u new_status)
{
  Bit32u level = new_status & (STAT_DAC1 | STAT_DAC2 | STAT_ADC);

  if (level) {
    BX_ES1370_THIS s.status = new_status | STAT_INTR;
  } else {
    BX_ES1370_THIS s.status = new_status & ~STAT_INTR;
  }
  set_irq_level(level != 0);
}

void bx_es1370_c::check_lower_irq(Bit32u sctl)
{
  Bit32u new_status = BX_ES1370_THIS s.status;

  if (!(sctl & SCTRL_P1INTEN) && (BX_ES1370_THIS s.sctl & SCTRL_P1INTEN)) {
    new_status &= ~STAT_DAC1;
  }
  if (!(sctl & SCTRL_P2INTEN) && (BX_ES1370_THIS s.sctl & SCTRL_P2INTEN)) {
    new_status &= ~STAT_DAC2;
  }
  if (!(sctl & SCTRL_R1INTEN) && (BX_ES1370_THIS s.sctl & SCTRL_R1INTEN)) {
    new_status &= ~STAT_ADC;
  }
  if (new_status != BX_ES1370_THIS s.status) {
    update_status(new_status);
  }
}

void bx_es1370_c::update_voices(Bit32u ctl, Bit32u sctl, bx_bool force)
{
  unsigned i;
  Bit32u old_freq, new_freq, old_fmt, new_fmt;
  int ret, timer_id;
  bx_pcm_param_t param;

  for (i = 0; i < 3; ++i) {
    chan_t *d = &BX_ES1370_THIS s.chan[i];

    old_fmt = (BX_ES1370_THIS s.sctl >> (i << 1)) & 3;
    new_fmt = (sctl >> (i << 1)) & 3;

    if (i == DAC1_CHANNEL) {
      old_freq = dac1_freq[(BX_ES1370_THIS s.ctl >> 12) & 3];
      new_freq = dac1_freq[(ctl >> 12) & 3];
    } else {
      old_freq = 1411200 / (((BX_ES1370_THIS s.ctl >> 16) & 0x1fff) + 2);
      new_freq = 1411200 / (((ctl >> 16) & 0x1fff) + 2);
    }

    if ((old_fmt != new_fmt) || (old_freq != new_freq) || force) {
      d->shift = (new_fmt & 1) + (new_fmt >> 1);
      if (new_freq) {
        if (i == ADC_CHANNEL) {
          if (!BX_ES1370_THIS s.adc_inputinit) {
            ret = BX_ES1370_WAVEIN->openwaveinput(SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr(),
                                                         es1370_adc_handler);
            if (ret != BX_SOUNDLOW_OK) {
              BX_ERROR(("could not open wave input device"));
            } else {
              BX_ES1370_THIS s.adc_inputinit = 1;
            }
          }
        }
      }
    }
    if (((ctl ^ BX_ES1370_THIS s.ctl) & ctl_ch_en[i]) ||
        ((sctl ^ BX_ES1370_THIS s.sctl) & sctl_ch_pause[i]) || force) {
      bx_bool on = ((ctl & ctl_ch_en[i]) && !(sctl & sctl_ch_pause[i]));

      if (i == DAC1_CHANNEL) {
        timer_id = BX_ES1370_THIS s.dac1_timer_index;
      } else {
        timer_id = BX_ES1370_THIS s.dac2_timer_index;
      }
      if (on) {
        BX_INFO(("%s: freq = %d, nchannels %d, fmt %d, shift %d",
                 chan_name[i], new_freq, 1 << (new_fmt & 1), (new_fmt & 2) ? 16 : 8, d->shift));
        if (i == ADC_CHANNEL) {
          if (BX_ES1370_THIS s.adc_inputinit) {
            memset(&param, 0, sizeof(bx_pcm_param_t));
            param.samplerate = new_freq;
            param.bits = (new_fmt >> 1) ? 16 : 8;
            param.channels = (new_fmt & 1) + 1;
            param.format = (new_fmt >> 1);
            ret = BX_ES1370_WAVEIN->startwaverecord(&param);
            if (ret != BX_SOUNDLOW_OK) {
              BX_ES1370_THIS s.adc_inputinit = 0;
              BX_ERROR(("could not start wave record"));
            }
          }
        } else {
          if (BX_ES1370_THIS s.dac_nr_active == -1) {
            if (BX_ES1370_THIS wavemode & 2) {
              if ((BX_ES1370_THIS s.dac_outputinit & 2) == 0) {
                bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
                bx_param_string_c *waveparam = SIM->get_param_string("wavefile", base);
                if (BX_ES1370_WAVEOUT2->openwaveoutput(waveparam->getptr()) == BX_SOUNDLOW_OK)
                  BX_ES1370_THIS s.dac_outputinit |= 2;
                else
                  BX_ES1370_THIS s.dac_outputinit &= ~2;
                if (((BX_ES1370_THIS s.dac_outputinit & BX_ES1370_THIS wavemode) & 2) == 0) {
                  BX_ERROR(("Error opening file '%s' - wave output disabled",
                            waveparam->getptr()));
                  BX_ES1370_THIS wavemode = BX_ES1370_THIS s.dac_outputinit;
                }
              }
            }
            BX_ES1370_THIS s.dac_nr_active = i;
          } else {
            BX_ERROR(("%s: %s already active - dual output not supported yet", chan_name[i],
                      chan_name[BX_ES1370_THIS s.dac_nr_active]));
          }
          BX_ES1370_THIS s.dac_packet_size[i] = (new_freq / 10) << d->shift; // 0.1 sec
          if (BX_ES1370_THIS s.dac_packet_size[i] > BX_SOUNDLOW_WAVEPACKETSIZE) {
            BX_ES1370_THIS s.dac_packet_size[i] = BX_SOUNDLOW_WAVEPACKETSIZE;
          }
          BX_ES1370_THIS s.dac_timer_val[i] =
            (Bit32u)((Bit64u)BX_ES1370_THIS s.dac_packet_size[i] * 1000000 / (new_freq << d->shift));
          bx_pc_system.activate_timer(timer_id, BX_ES1370_THIS s.dac_timer_val[i], 1);
        }
      } else {
        if (i == ADC_CHANNEL) {
          if (BX_ES1370_THIS s.adc_inputinit) {
            BX_ES1370_WAVEIN->stopwaverecord();
          }
        } else {
          BX_ES1370_THIS s.dac_nr_active = -1;
          bx_pc_system.deactivate_timer(timer_id);
        }
      }
    }
  }
  BX_ES1370_THIS s.ctl = ctl;
  BX_ES1370_THIS s.sctl = sctl;
}

void bx_es1370_c::sendwavepacket(unsigned channel, Bit32u buflen, Bit8u *buffer)
{
  bx_pcm_param_t param;
  Bit8u format;

  if (channel == DAC1_CHANNEL) {
    param.samplerate = dac1_freq[(BX_ES1370_THIS s.ctl >> 12) & 3];
  } else {
    param.samplerate = 1411200 / (((BX_ES1370_THIS s.ctl >> 16) & 0x1fff) + 2);
  }
  format = (BX_ES1370_THIS s.sctl >> (channel << 1)) & 3;
  param.bits = (format >> 1) ? 16 : 8;
  param.channels = (format & 1) + 1;
  param.format = (format >> 1) & 1;
  param.volume = BX_ES1370_THIS s.wave_vol;

  if (BX_ES1370_THIS wavemode & 1) {
    BX_ES1370_WAVEOUT1->sendwavepacket(buflen, buffer, &param);
  }
  if (BX_ES1370_THIS wavemode & 2) {
    BX_ES1370_WAVEOUT2->sendwavepacket(buflen, buffer, &param);
  }
}

void bx_es1370_c::closewaveoutput()
{
  if (BX_ES1370_THIS wavemode > 0) {
    if (BX_ES1370_THIS s.dac_outputinit & 2) {
      BX_ES1370_WAVEOUT2->closewaveoutput();
      BX_ES1370_THIS s.dac_outputinit &= ~2;
    }
  }
}

void bx_es1370_c::writemidicommand(int command, int length, Bit8u data[])
{
  bx_param_string_c *midiparam;

  int deltatime = currentdeltatime();

  if (BX_ES1370_THIS midimode > 0) {
    if ((BX_ES1370_THIS s.mpu_outputinit & BX_ES1370_THIS midimode) != BX_ES1370_THIS midimode) {
      BX_DEBUG(("Initializing Midi output"));
      if (BX_ES1370_THIS midimode & 1) {
        midiparam = SIM->get_param_string(BXPN_SOUND_MIDIOUT);
        if (BX_ES1370_MIDIOUT1->openmidioutput(midiparam->getptr()) == BX_SOUNDLOW_OK)
          BX_ES1370_THIS s.mpu_outputinit |= 1;
        else
          BX_ES1370_THIS s.mpu_outputinit &= ~1;
      }
      if (BX_ES1370_THIS midimode & 2) {
        bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
        midiparam = SIM->get_param_string("midifile", base);
        if (BX_ES1370_MIDIOUT2->openmidioutput(midiparam->getptr()) == BX_SOUNDLOW_OK)
          BX_ES1370_THIS s.mpu_outputinit |= 2;
        else
          BX_ES1370_THIS s.mpu_outputinit &= ~2;
      }
      if ((BX_ES1370_THIS s.mpu_outputinit & BX_ES1370_THIS midimode) != BX_ES1370_THIS midimode) {
        BX_ERROR(("Couldn't open midi output. Midi disabled"));
        BX_ES1370_THIS midimode = BX_ES1370_THIS s.mpu_outputinit;
        return;
      }
    }
    if (BX_ES1370_THIS midimode & 1) {
      BX_ES1370_MIDIOUT1->sendmidicommand(deltatime, command, length, data);
    }
    if (BX_ES1370_THIS midimode & 2) {
      BX_ES1370_MIDIOUT2->sendmidicommand(deltatime, command, length, data);
    }
  }
}

int bx_es1370_c::currentdeltatime()
{
  int deltatime;

  // counting starts at first access
  if (BX_ES1370_THIS s.last_delta_time == 0xffffffff)
    BX_ES1370_THIS s.last_delta_time = BX_ES1370_THIS s.mpu_current_timer;

  deltatime = BX_ES1370_THIS s.mpu_current_timer - BX_ES1370_THIS s.last_delta_time;
  BX_ES1370_THIS s.last_delta_time = BX_ES1370_THIS s.mpu_current_timer;

  return deltatime;
}

void bx_es1370_c::closemidioutput()
{
  if (BX_ES1370_THIS midimode > 0) {
    if (BX_ES1370_THIS s.mpu_outputinit & 1) {
      BX_ES1370_MIDIOUT1->closemidioutput();
      BX_ES1370_THIS s.mpu_outputinit &= ~1;
    }
    if (BX_ES1370_THIS s.mpu_outputinit & 2) {
      BX_ES1370_MIDIOUT2->closemidioutput();
      BX_ES1370_THIS s.mpu_outputinit &= ~2;
    }
  }
}


// pci configuration space write callback handler
void bx_es1370_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  if ((address >= 0x14) && (address < 0x34))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    Bit8u value8 = (value >> (i*8)) & 0xFF;
//    Bit8u oldval = BX_ES1370_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x05;
        BX_ES1370_THIS pci_conf[address+i] = value8;
        break;
      case 0x05:
        value8 &= 0x01;
        BX_ES1370_THIS pci_conf[address+i] = value8;
        break;
      case 0x3d: //
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      default:
        BX_ES1370_THIS pci_conf[address+i] = value8;
    }
  }
}

// runtime parameter handlers
Bit64s bx_es1370_c::es1370_param_handler(bx_param_c *param, int set, Bit64s val)
{
  if (set) {
    const char *pname = param->get_name();
    if (!strcmp(pname, "wavemode")) {
      if (val != BX_ES1370_THIS wavemode) {
        BX_ES1370_THIS wave_changed |= 1;
      }
    } else if (!strcmp(pname, "midimode")) {
      if (val != BX_ES1370_THIS midimode) {
        BX_ES1370_THIS midi_changed |= 1;
      }
    } else {
      BX_PANIC(("es1370_param_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

const char* bx_es1370_c::es1370_param_string_handler(bx_param_string_c *param, int set,
                                                 const char *oldval, const char *val,
                                                 int maxlen)
{
  if ((set) && (strcmp(val, oldval))) {
    const char *pname = param->get_name();
    if (!strcmp(pname, "wavefile")) {
      BX_ES1370_THIS wave_changed |= 2;
    } else if (!strcmp(pname, "midifile")) {
      BX_ES1370_THIS midi_changed |= 2;
    } else {
      BX_PANIC(("es1370_param_string_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_ES1370
