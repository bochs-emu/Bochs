/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
// ES1370 soundcard support (ported from QEMU)
//
// Copyright (c) 2005  Vassili Karpov (malc)
// Copyright (C) 2011-2014  The Bochs Project
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

#include "pci.h"
#include "es1370.h"
#include "soundmod.h"

#include <math.h>

#define LOG_THIS theES1370Device->

bx_es1370_c* theES1370Device = NULL;

const Bit8u es1370_iomask[64] = {7, 1, 3, 1, 7, 1, 3, 1, 1, 3, 1, 0, 7, 0, 0, 0,
                                 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
                                 7, 1, 3, 1, 6, 0, 2, 0, 6, 0, 2, 0, 6, 0, 2, 0,
                                 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0};

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

const char chan_name[3][5] = {"DAC1", "DAC2", "ADC"};
const Bit16u dac1_freq[4] = {5512, 11025, 22050, 44100};
const Bit16u ctl_ch_en[3] = {0x0040, 0x0020, 0x0010};
const Bit16u sctl_ch_pause[3] = {0x0800, 0x1000, 0x0000};
const Bit16u sctl_loop_sel[3] = {0x2000, 0x4000, 0x8000};

// builtin configuration handling functions

void es1370_init_options(void)
{
  static const char *es1370_wavemode_list[] = {
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

  bx_param_enum_c *wavemode = new bx_param_enum_c(menu,
    "wavemode",
    "Wave mode",
    "Controls the wave output format.",
    es1370_wavemode_list,
    0, 0);
  bx_param_filename_c *wavefile = new bx_param_filename_c(menu,
    "wavefile",
    "Wave file",
    "This is the file where the wave output is stored",
    "", BX_PATHNAME_LEN);
  bx_list_c *deplist = new bx_list_c(NULL);
  deplist->add(wavemode);
  enabled->set_dependent_list(deplist);
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

int CDECL libes1370_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
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
  soundmod = NULL;
  wavefile = NULL;
}

bx_es1370_c::~bx_es1370_c()
{
  if (s.adc_inputinit) {
    soundmod->closewaveinput();
  }
  closewaveoutput();

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
                            "Experimental ES1370 soundcard");

  // initialize readonly registers
  init_pci_conf(0x1274, 0x5000, 0x00, 0x040100, 0x00);
  BX_ES1370_THIS pci_conf[0x3d] = BX_PCI_INTA;

  BX_ES1370_THIS pci_base_address[0] = 0;

  BX_ES1370_THIS soundmod = DEV_sound_get_module();
  BX_ES1370_THIS s.dac_outputinit = 1;
  BX_ES1370_THIS s.adc_inputinit = 0;
  BX_ES1370_THIS s.dac_nr_active = -1;

  BX_ES1370_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();

  if (BX_ES1370_THIS s.dac1_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ES1370_THIS s.dac1_timer_index = bx_pc_system.register_timer
      (BX_ES1370_THIS_PTR, es1370_timer_handler, 1, 1, 0, "es1370.dac1");
    // DAC1 timer: inactive, continuous, frequency variable
    bx_pc_system.setTimerParam(BX_ES1370_THIS s.dac1_timer_index, 0);
  }
  if (BX_ES1370_THIS s.dac2_timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ES1370_THIS s.dac2_timer_index = bx_pc_system.register_timer
      (BX_ES1370_THIS_PTR, es1370_timer_handler, 1, 1, 0, "es1370.dac2");
    // DAC2 timer: inactive, continuous, frequency variable
    bx_pc_system.setTimerParam(BX_ES1370_THIS s.dac2_timer_index, 1);
  }

  // init runtime parameters
  bx_list_c *misc_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_MISC);
  bx_list_c *menu = new bx_list_c(misc_rt, "es1370", "ES1370 Runtime Options");
  menu->set_options(menu->SHOW_PARENT | menu->USE_BOX_TITLE);

  menu->add(SIM->get_param("wavemode", base));
  menu->add(SIM->get_param("wavefile", base));
  SIM->get_param_enum("wavemode", base)->set_handler(es1370_param_handler);
  SIM->get_param_string("wavefile", base)->set_handler(es1370_param_string_handler);
  // register handler for correct es1370 parameter handling after runtime config
  SIM->register_runtime_config_handler(this, runtime_config_handler);
  BX_ES1370_THIS wave_changed = 0;

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
  for (i = 0; i < 3; i++) {
    BX_ES1370_THIS s.chan[i].scount = 0;
    BX_ES1370_THIS s.chan[i].leftover = 0;
  }

  DEV_gameport_set_enabled(0);

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
  bx_list_c *codec_regs = new bx_list_c(list, "codec_regs", "");
  for (i = 0; i < BX_ES1370_CODEC_REGS; i++) {
    sprintf(pname, "0x%02x", i);
    new bx_shadow_num_c(codec_regs, pname, &BX_ES1370_THIS s.codec_reg[i], BASE_HEX);
  }
  BXRS_HEX_PARAM_FIELD(list, sctl, BX_ES1370_THIS s.sctl);

  register_pci_state(list);
}

void bx_es1370_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_ES1370_THIS_PTR, read_handler, write_handler,
                          &BX_ES1370_THIS pci_base_address[0],
                          &BX_ES1370_THIS pci_conf[0x10],
                          64, &es1370_iomask[0], "ES1370")) {
    BX_INFO(("new base address: 0x%04x", BX_ES1370_THIS pci_base_address[0]));
  }
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
  if (BX_ES1370_THIS wave_changed) {
    BX_ES1370_THIS closewaveoutput();
    BX_ES1370_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
    // update_voices() re-opens the output file on demand
    BX_ES1370_THIS wave_changed = 0;
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

  offset = address - BX_ES1370_THIS pci_base_address[0];
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
      BX_ERROR(("reading from UART not supported yet"));
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
      i = (offset - ES1370_DAC1_SCOUNT) / 4;
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
      val = BX_ES1370_THIS s.chan[0].frame_cnt;
      break;
    case ES1370_DAC2_FRAMECNT:
      val = BX_ES1370_THIS s.chan[1].frame_cnt;
      break;
    case ES1370_ADC_FRAMECNT:
      val = BX_ES1370_THIS s.chan[2].frame_cnt;
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
      val = ~0U; // keep compiler happy
      BX_ERROR(("unsupported io read from offset=0x%04x!", offset));
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
  Bit8u index, master_vol, dac_vol;
  bx_bool set_wave_vol = 0;
  unsigned i;
  float tmp_vol;

  BX_DEBUG(("register write to address 0x%04x - value = 0x%08x", address, value));

  offset = address - BX_ES1370_THIS pci_base_address[0];
  if (offset >= 0x30) {
    offset |= (BX_ES1370_THIS s.mempage << 8);
  }
  shift = (offset & 3) << 3;

  switch (offset & ~3) {
    case ES1370_CTL:
      mask = (0xffffffff >> ((4 - io_len) << 3)) << shift;
      value = (BX_ES1370_THIS s.ctl & ~mask) | ((value << shift) & mask);
      if ((value ^ BX_ES1370_THIS s.ctl) & 0x04) {
        DEV_gameport_set_enabled((value & 0x04) != 0);
      }
      BX_ES1370_THIS update_voices(value, BX_ES1370_THIS s.sctl, 0);
      break;
    case ES1370_UART_DATA:
    case ES1370_UART_CTL:
    case ES1370_UART_TEST:
      BX_ERROR(("writing to UART not supported yet"));
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
      i = (offset - ES1370_DAC1_SCOUNT) / 4;
      value &= 0xffff;
      BX_ES1370_THIS s.chan[i].scount = value | (value << 16);
      break;
    case ES1370_DAC1_FRAMEADR:
      BX_ES1370_THIS s.chan[0].frame_addr = value;
      break;
    case ES1370_DAC2_FRAMEADR:
      BX_ES1370_THIS s.chan[1].frame_addr = value;
      break;
    case ES1370_ADC_FRAMEADR:
      BX_ES1370_THIS s.chan[2].frame_addr = value;
      break;
    case ES1370_DAC1_FRAMECNT:
      BX_ES1370_THIS s.chan[0].frame_cnt = value;
      break;
    case ES1370_DAC2_FRAMECNT:
      BX_ES1370_THIS s.chan[1].frame_cnt = value;
      break;
    case ES1370_ADC_FRAMECNT:
      BX_ES1370_THIS s.chan[2].frame_cnt = value;
      break;
    case ES1370_PHA_FRAMEADR:
      BX_ERROR(("writing to phantom frame address"));
      break;
    case ES1370_PHA_FRAMECNT:
      BX_ERROR(("writing to phantom frame count"));
      break;
    default:
      BX_ERROR(("unsupported io write to offset=0x%04x!", offset));
      break;
  }

  if (set_wave_vol) {
    master_vol = (0x1f - (BX_ES1370_THIS s.codec_reg[0] & 0x1f));
    dac_vol = (0x1f - (BX_ES1370_THIS s.codec_reg[2] & 0x1f));
    tmp_vol = (float)master_vol/31.0f*pow(10.0f, (float)(31-dac_vol)*-0.065f);
    BX_ES1370_THIS s.wave_vol = (Bit8u)(255 * tmp_vol);
    master_vol = (0x1f - (BX_ES1370_THIS s.codec_reg[1] & 0x1f));
    dac_vol = (0x1f - (BX_ES1370_THIS s.codec_reg[3] & 0x1f));
    tmp_vol = (float)master_vol/31.0f*pow(10.0f, (float)(31-dac_vol)*-0.065f);
    BX_ES1370_THIS s.wave_vol |= ((Bit8u)(255 * tmp_vol) << 8);
  }
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
  run_channel(i, timer_id, BX_ES1370_THIS s.dac_packet_size[i]);
}

void bx_es1370_c::run_channel(unsigned chan, int timer_id, Bit32u buflen)
{
  Bit32u new_status = BX_ES1370_THIS s.status;
  Bit32u addr, sc, csc_bytes, cnt, size, left, transfered, temp;
  Bit8u tmpbuf[BX_SOUNDLOW_WAVEPACKETSIZE];
  bx_bool irq = 0;

  chan_t *d = &BX_ES1370_THIS s.chan[chan];

  if (!(BX_ES1370_THIS s.ctl & ctl_ch_en[chan]) || (BX_ES1370_THIS s.sctl & sctl_ch_pause[chan])) {
    if (chan == ADC_CHANNEL) {
      BX_ES1370_THIS soundmod->stopwaverecord();
    } else {
      bx_pc_system.deactivate_timer(timer_id);
    }
    return;
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
    BX_ES1370_THIS soundmod->getwavepacket(temp, tmpbuf);
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
  Bit64u timer_val;

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
            ret = BX_ES1370_THIS soundmod->openwaveinput(SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr(),
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
            ret = BX_ES1370_THIS soundmod->startwaverecord(new_freq, (new_fmt >> 1) ? 16 : 8, (new_fmt & 1), (new_fmt >> 1));
            if (ret != BX_SOUNDLOW_OK) {
              BX_ES1370_THIS soundmod->closewaveinput();
              BX_ES1370_THIS s.adc_inputinit = 0;
              BX_ERROR(("could not start wave record"));
            }
          }
        } else {
          if ((BX_ES1370_THIS s.dac_nr_active == -1) && BX_ES1370_THIS s.dac_outputinit) {
            if (BX_ES1370_THIS wavemode == 1) {
              ret = BX_ES1370_THIS soundmod->startwaveplayback(new_freq, (new_fmt >> 1) ? 16 : 8, (new_fmt & 1), (new_fmt >> 1));
              if (ret != BX_SOUNDLOW_OK) {
                BX_ES1370_THIS soundmod->closewaveoutput();
                BX_ES1370_THIS s.dac_outputinit = 0;
                BX_ERROR(("could not start wave playback"));
              } else {
                BX_ES1370_THIS s.dac_nr_active = i;
              }
            } else if ((BX_ES1370_THIS wavemode == 2) ||
                       (BX_ES1370_THIS wavemode == 3)) {
              bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_ES1370);
              bx_param_string_c *waveparam = SIM->get_param_string("wavefile", base);
              if ((BX_ES1370_THIS wavefile == NULL) && (!waveparam->isempty())) {
                BX_ES1370_THIS wavefile = fopen(waveparam->getptr(), "wb");
                if (BX_ES1370_THIS wavefile == NULL) {
                  BX_ERROR(("Error opening file '%s' - wave output disabled",
                            waveparam->getptr()));
                  BX_ES1370_THIS wavemode = 0;
                } else if (BX_ES1370_THIS wavemode == 2) {
                  DEV_soundmod_VOC_init_file(BX_ES1370_THIS wavefile);
                }
              }
              BX_ES1370_THIS s.dac_nr_active = i;
            } else {
              BX_ES1370_THIS s.dac_nr_active = i;
            }
          }
          BX_ES1370_THIS s.dac_packet_size[i] = (new_freq / 10) << d->shift; // 0.1 sec
          if (BX_ES1370_THIS s.dac_packet_size[i] > BX_SOUNDLOW_WAVEPACKETSIZE) {
            BX_ES1370_THIS s.dac_packet_size[i] = BX_SOUNDLOW_WAVEPACKETSIZE;
          }
          timer_val = (Bit64u)BX_ES1370_THIS s.dac_packet_size[i] * 1000000 / (new_freq << d->shift);
          bx_pc_system.activate_timer(timer_id, (Bit32u)timer_val, 1);
        }
      } else {
        if (i == ADC_CHANNEL) {
          if (BX_ES1370_THIS s.adc_inputinit) {
            BX_ES1370_THIS soundmod->stopwaverecord();
          }
        } else {
          if (((int)i == BX_ES1370_THIS s.dac_nr_active) && BX_ES1370_THIS s.dac_outputinit) {
            if (BX_ES1370_THIS wavemode == 1) {
              BX_ES1370_THIS soundmod->stopwaveplayback();
            }
            BX_ES1370_THIS s.dac_nr_active = -1;
          }
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
  Bit8u bits, format;
  Bit16u samplerate;
  bx_bool stereo, issigned;

  format = (BX_ES1370_THIS s.sctl >> (channel << 1)) & 3;
  bits = (format >> 1) ? 16 : 8;
  stereo = format & 1;
  issigned = (format >> 1) & 1;

  // apply wave volume
  if (BX_ES1370_THIS s.wave_vol != 0xffff) {
    DEV_soundmod_pcm_apply_volume(buflen, buffer, BX_ES1370_THIS s.wave_vol,
                                  bits, stereo, issigned);
  }

  switch (BX_ES1370_THIS wavemode) {
    case 1:
      if (BX_ES1370_THIS s.dac_outputinit) {
        BX_ES1370_THIS soundmod->sendwavepacket(buflen, buffer);
      }
      break;
    case 3:
      fwrite(buffer, 1, buflen, BX_ES1370_THIS wavefile);
      break;
    case 2:
      if (channel == DAC1_CHANNEL) {
        samplerate = dac1_freq[(BX_ES1370_THIS s.ctl >> 12) & 3];
      } else {
        samplerate = 1411200 / (((BX_ES1370_THIS s.ctl >> 16) & 0x1fff) + 2);
      }
      Bit8u temparray[12] =
       { (Bit8u)(samplerate & 0xff), (Bit8u)(samplerate >> 8), 0, 0,
         bits, (Bit8u)(stereo + 1), 0, 0, 0, 0, 0, 0 };
      if (issigned)
         temparray[6] = 4;

      DEV_soundmod_VOC_write_block(BX_ES1370_THIS wavefile, 9, 12, temparray, buflen, buffer);
      break;
  }
}

void bx_es1370_c::closewaveoutput()
{
  switch (BX_ES1370_THIS wavemode) {
    case 1:
      // nothing to do here yet
      break;
    case 2:
      if (BX_ES1370_THIS wavefile != NULL) {
        fputc(0, BX_ES1370_THIS wavefile);
      }
    case 3:
      if (BX_ES1370_THIS wavefile != NULL)
        fclose(BX_ES1370_THIS wavefile);
      BX_ES1370_THIS wavefile = NULL;
      break;
  }
}

// pci configuration space read callback handler
Bit32u bx_es1370_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_ES1370_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%08x", address, value));

  return value;
}


// pci configuration space write callback handler
void bx_es1370_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if ((address >= 0x14) && (address < 0x34))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_ES1370_THIS pci_conf[address+i];
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
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
          BX_ES1370_THIS pci_conf[address+i] = value8;
        }
        break;
      case 0x10:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x11:
      case 0x12:
      case 0x13:
        baseaddr_change |= (value8 != oldval);
      default:
        BX_ES1370_THIS pci_conf[address+i] = value8;
    }
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_io(BX_ES1370_THIS_PTR, read_handler, write_handler,
                            &BX_ES1370_THIS pci_base_address[0],
                            &BX_ES1370_THIS pci_conf[0x10],
                            64, &es1370_iomask[0], "ES1370")) {
      BX_INFO(("new base address: 0x%04x", BX_ES1370_THIS pci_base_address[0]));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

// runtime parameter handlers
Bit64s bx_es1370_c::es1370_param_handler(bx_param_c *param, int set, Bit64s val)
{
  if (set) {
    const char *pname = param->get_name();
    if (!strcmp(pname, "wavemode")) {
      if (val != BX_ES1370_THIS wavemode) {
        BX_ES1370_THIS wave_changed = 1;
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
      BX_ES1370_THIS wave_changed = 1;
    } else {
      BX_PANIC(("es1370_param_string_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_ES1370
