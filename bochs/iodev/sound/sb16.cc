/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2020  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// The original version of the SB16 support written and donated by Josef Drexler

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SB16

#include "soundlow.h"
#include "soundmod.h"
#include "sb16.h"
#include "opl.h"

#include <math.h>

#define LOG_THIS theSB16Device->

bx_sb16_c *theSB16Device = NULL;

Bit32u fmopl_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len);

// builtin configuration handling functions

void sb16_init_options(void)
{
  static const char *sb16_mode_list[] = {
    "0",
    "1",
    "2",
    "3",
    NULL
  };

  bx_param_c *sound = SIM->get_param("sound");
  bx_list_c *menu = new bx_list_c(sound, "sb16", "SB16 Configuration");
  menu->set_options(menu->SHOW_PARENT);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable SB16 emulation",
    "Enables the SB16 emulation",
    1);

  bx_param_enum_c *midimode = new bx_param_enum_c(menu,
    "midimode",
    "Midi mode",
    "Controls the MIDI output switches.",
    sb16_mode_list,
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
    sb16_mode_list,
    0, 0);
  bx_param_filename_c *wavefile = new bx_param_filename_c(menu,
    "wavefile",
    "Wave file",
    "This is the file where the wave output is stored",
    "", BX_PATHNAME_LEN);
  bx_param_num_c *loglevel = new bx_param_num_c(menu,
    "loglevel",
    "Log level",
    "Controls how verbose the SB16 emulation is (0 = no log, 5 = all errors and infos).",
    0, 5,
    0);
  bx_param_filename_c *logfile = new bx_param_filename_c(menu,
    "log",
    "Log file",
    "The file to write the SB16 emulator messages to.",
    "", BX_PATHNAME_LEN);
  logfile->set_extension("log");
  bx_param_num_c *dmatimer = new bx_param_num_c(menu,
    "dmatimer",
    "DMA timer",
    "Microseconds per second for a DMA cycle.",
    0, BX_MAX_BIT32U,
    0);

  bx_list_c *deplist = new bx_list_c(NULL);
  deplist->add(midimode);
  deplist->add(wavemode);
  deplist->add(loglevel);
  deplist->add(dmatimer);
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
  deplist = new bx_list_c(NULL);
  deplist->add(logfile);
  loglevel->set_dependent_list(deplist);
  loglevel->set_options(loglevel->USE_SPIN_CONTROL);
}

Bit32s sb16_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "sb16")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
    int enable = 1;
    SIM->get_param_bool("enabled", base)->set(1);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool("enabled", base)->parse_param(&params[i][8]);
        enable = SIM->get_param_bool("enabled", base)->get();
      } else if (!strncmp(params[i], "midi=", 5)) {
        SIM->get_param_string("midifile", base)->set(&params[i][5]);
      } else if (!strncmp(params[i], "wave=", 5)) {
        SIM->get_param_string("wavefile", base)->set(&params[i][5]);
      } else if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for sb16 ignored.", context));
      }
    }
    if ((enable != 0) && (SIM->get_param_num("dmatimer", base)->get() == 0)) {
      SIM->get_param_bool("enabled", base)->set(0);
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s sb16_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16), NULL, 0);
}

// device plugin entry points

int CDECL libsb16_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
{
  theSB16Device = new bx_sb16_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSB16Device, BX_PLUGIN_SB16);
  // add new configuration parameter for the config interface
  sb16_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("sb16", sb16_options_parser, sb16_options_save);
  return(0); // Success
}

void CDECL libsb16_LTX_plugin_fini(void)
{
  delete theSB16Device;
  SIM->unregister_addon_option("sb16");
  ((bx_list_c*)SIM->get_param("sound"))->remove("sb16");
}

// some shortcuts to save typing
#define LOGFILE         BX_SB16_THIS logfile
#define MPU             BX_SB16_THIS mpu401.d
#define MPU_B           BX_SB16_THIS mpu401.b
#define DSP             BX_SB16_THIS dsp.d
#define DSP_B           BX_SB16_THIS dsp.b
#define MIXER           BX_SB16_THIS mixer
#define EMUL            BX_SB16_THIS emuldata
#define OPL             BX_SB16_THIS opl

#define BX_SB16_WAVEOUT1 BX_SB16_THIS waveout[0]
#define BX_SB16_WAVEOUT2 BX_SB16_THIS waveout[1]
#define BX_SB16_WAVEIN   BX_SB16_THIS wavein
#define BX_SB16_MIDIOUT1 BX_SB16_THIS midiout[0]
#define BX_SB16_MIDIOUT2 BX_SB16_THIS midiout[1]

// here's a safe way to print out null pointeres
#define MIGHT_BE_NULL(x)  ((x==NULL)? "(null)" : x)

// the device object

bx_sb16_c::bx_sb16_c(void)
{
  put("SB16");
  memset(&mpu401.d, 0, sizeof(mpu401.d));
  memset(&dsp.d, 0, sizeof(dsp.d));
  memset(&opl, 0, sizeof(opl));
  currentdma8 = 0;
  currentdma16 = 0;
  mpu401.d.timer_handle = BX_NULL_TIMER_HANDLE;
  dsp.d.timer_handle = BX_NULL_TIMER_HANDLE;
  opl.timer_handle = BX_NULL_TIMER_HANDLE;
  waveout[0] = NULL;
  waveout[1] = NULL;
  wavein = NULL;
  midiout[0] = NULL;
  midiout[1] = NULL;
  wavemode = 0;
  midimode = 0;
  loglevel = 0;
  logfile = NULL;
  rt_conf_id = -1;
}

bx_sb16_c::~bx_sb16_c(void)
{
  SIM->unregister_runtime_config_handler(rt_conf_id);

  closemidioutput();

  if (BX_SB16_WAVEOUT1 != NULL) {
    BX_SB16_WAVEOUT1->unregister_wave_callback(fmopl_callback_id);
  }
  closewaveoutput();

  delete [] DSP.dma.chunk;

  if (LOGFILE != NULL)
    fclose(LOGFILE);

  SIM->get_bochs_root()->remove("sb16");
  bx_list_c *misc_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_MISC);
  misc_rt->remove("sb16");
  BX_DEBUG(("Exit"));
}

void bx_sb16_c::init(void)
{
  unsigned addr, i;

  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("SB16 disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("sb16"))->set(0);
    return;
  }
  create_logfile();
  BX_SB16_THIS midimode = SIM->get_param_enum("midimode", base)->get();
  BX_SB16_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
  BX_SB16_THIS dmatimer = SIM->get_param_num("dmatimer", base)->get();
  BX_SB16_THIS loglevel = SIM->get_param_num("loglevel", base)->get();

  // always initialize lowlevel driver
  BX_SB16_WAVEOUT1 = DEV_sound_get_waveout(0);
  if (BX_SB16_WAVEOUT1 == NULL) {
    BX_PANIC(("Couldn't initialize waveout driver"));
    BX_SB16_THIS wavemode &= ~1;
  } else {
    BX_SB16_THIS fmopl_callback_id = BX_SB16_WAVEOUT1->register_wave_callback(BX_SB16_THISP, fmopl_callback);
  }
  if (BX_SB16_THIS wavemode & 2) {
    BX_SB16_WAVEOUT2 = DEV_sound_get_waveout(1);
    if (BX_SB16_WAVEOUT2 == NULL) {
      BX_PANIC(("Couldn't initialize wave file driver"));
    }
  }
  BX_SB16_WAVEIN = DEV_sound_get_wavein();
  if (BX_SB16_WAVEIN == NULL) {
    BX_PANIC(("Couldn't initialize wavein driver"));
  }
  BX_SB16_MIDIOUT1 = DEV_sound_get_midiout(0);
  if (BX_SB16_MIDIOUT1 == NULL) {
    BX_PANIC(("Couldn't initialize midiout driver"));
  }
  if (BX_SB16_THIS midimode & 2) {
    BX_SB16_MIDIOUT2 = DEV_sound_get_midiout(1);
    if (BX_SB16_MIDIOUT2 == NULL) {
      BX_PANIC(("Couldn't initialize midi file driver"));
    }
  }

  DSP.dma.chunk = new Bit8u[BX_SOUNDLOW_WAVEPACKETSIZE];
  DSP.dma.chunkindex = 0;

  DSP.outputinit = (BX_SB16_THIS wavemode & 1);
  DSP.inputinit = 0;
  MPU.outputinit = 0;

  if (DSP.dma.chunk == NULL)
  {
    writelog(WAVELOG(2), "Couldn't allocate wave buffer - wave output disabled.");
    BX_SB16_THIS wavemode = 0;
  }

  BX_INFO(("midi=%d,'%s'  wave=%d,'%s'  log=%d,'%s'  dmatimer=%d",
    BX_SB16_THIS midimode, MIGHT_BE_NULL(SIM->get_param_string("midifile", base)->getptr()),
    BX_SB16_THIS wavemode, MIGHT_BE_NULL(SIM->get_param_string("wavefile", base)->getptr()),
    BX_SB16_THIS loglevel, MIGHT_BE_NULL(SIM->get_param_string("log", base)->getptr()),
    BX_SB16_THIS dmatimer));

  // allocate the FIFO buffers - except for the MPUMIDICMD buffer
  // these sizes are generous, 16 or 8 would probably be sufficient
  MPU_B.datain.init((int) 64);           // the MPU input
  MPU_B.dataout.init((int) 64);          // and output
  MPU_B.cmd.init((int) 64);              // and command buffers
  MPU_B.midicmd.init((int) 256);         // and the midi command buffer (note- large SYSEX'es have to fit!)
  DSP_B.datain.init((int) 64);           // the DSP input
  DSP_B.dataout.init((int) 64);          // and output buffers
  EMUL.datain.init((int) 64);            // the emulator ports
  EMUL.dataout.init((int) 64);           // for changing emulator settings

  // reset all parts of the hardware by
  // triggering their reset functions

  // reset the Emulator port
  emul_write(0x00);

  // reset the MPU401
  mpu_command(0xff);
  MPU.last_delta_time = 0xffffffff;

  // reset the DSP
  DSP.dma.highspeed = 0;
  DSP.dma.mode = 0;
  DSP.irqpending = 0;
  DSP.midiuartmode = 0;
  DSP.nondma_mode = 0;
  DSP.nondma_count = 0;
  DSP.samplebyte = 0;
  DSP.resetport = 1;  // so that one call to dsp_reset is sufficient
  dsp_reset(0);       // (reset is 1 to 0 transition)
  DSP.testreg = 0;

  BX_SB16_IRQ = -1; // will be initialized later by the mixer reset

  for (i=0; i<BX_SB16_MIX_REG; i++)
    MIXER.reg[i] = 0xff;
  MIXER.reg[0x00] = 0;  // reset register
  MIXER.reg[0x80] = 2;  // IRQ 5
  MIXER.reg[0x81] = 2;  // 8-bit DMA 1, no 16-bit DMA
  MIXER.reg[0x82] = 2 << 5;  // no IRQ pending
  MIXER.reg[0xfd] = 16; // ???
  MIXER.reg[0xfe] = 6;  // ???
  set_irq_dma();        // set the IRQ and DMA

  // call the mixer reset
  mixer_writeregister(0x00);
  mixer_writedata(0x00);

  // reset the FM emulation
  OPL.timer_running = 0;
  for (i=0; i<2; i++) {
    OPL.tmask[i] = 0;
    OPL.tflag[i] = 0;
  }
  for (i=0; i<4; i++) {
    OPL.timer[i] = 0;
    OPL.timerinit[i] = 0;
  }
  adlib_init(44100);

  // csp
  memset(&BX_SB16_THIS csp_reg[0], 0, sizeof(BX_SB16_THIS csp_reg));
  BX_SB16_THIS csp_reg[5] = 0x01;
  BX_SB16_THIS csp_reg[9] = 0xf8;

  // Allocate the IO addresses, 2x0..2xf, 3x0..3x4 and 388..38b
  for (addr=BX_SB16_IO; addr<BX_SB16_IO+BX_SB16_IOLEN; addr++) {
    DEV_register_ioread_handler(this, &read_handler, addr, "SB16", 1);
    DEV_register_iowrite_handler(this, &write_handler, addr, "SB16", 1);
  }
  for (addr=BX_SB16_IOMPU; addr<BX_SB16_IOMPU+BX_SB16_IOMPULEN; addr++) {
    DEV_register_ioread_handler(this, &read_handler, addr, "SB16", 1);
    DEV_register_iowrite_handler(this, &write_handler, addr, "SB16", 1);
  }
  for (addr=BX_SB16_IOADLIB; addr<BX_SB16_IOADLIB+BX_SB16_IOADLIBLEN; addr++) {
    DEV_register_ioread_handler(this, read_handler, addr, "SB16", 1);
    DEV_register_iowrite_handler(this, write_handler, addr, "SB16", 1);
  }

  writelog(BOTHLOG(1),
          "SB16 emulation initialised, IRQ %d, IO %03x/%03x/%03x, DMA %d/%d",
          BX_SB16_IRQ, BX_SB16_IO, BX_SB16_IOMPU, BX_SB16_IOADLIB,
          BX_SB16_DMAL, BX_SB16_DMAH);

  // initialize the timers
  if (MPU.timer_handle == BX_NULL_TIMER_HANDLE) {
    MPU.timer_handle = DEV_register_timer
      (BX_SB16_THISP, mpu_timer, 500000 / 384, 1, 1, "sb16.mpu");
    // midi timer: active, continuous, 500000 / 384 seconds (384 = delta time, 500000 = sec per beat at 120 bpm. Don't change this!)
  }

  if (DSP.timer_handle == BX_NULL_TIMER_HANDLE) {
    DSP.timer_handle = DEV_register_timer
      (BX_SB16_THISP, dsp_dmatimer_handler, 1, 1, 0, "sb16.dsp");
    // dma timer: inactive, continuous, frequency variable
  }

  if (OPL.timer_handle == BX_NULL_TIMER_HANDLE) {
    OPL.timer_handle = DEV_register_timer
      (BX_SB16_THISP, opl_timer, 80, 1, 0, "sb16.opl");
    // opl timer: inactive, continuous, frequency 80us
  }

  writelog(MIDILOG(4), "Timers initialized, midi %d, dma %d, opl %d",
          MPU.timer_handle, DSP.timer_handle, OPL.timer_handle);
  MPU.current_timer = 0;

  // init runtime parameters
  bx_list_c *misc_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_MISC);
  bx_list_c *menu = new bx_list_c(misc_rt, "sb16", "SB16 Runtime Options");
  menu->set_options(menu->SHOW_PARENT | menu->USE_BOX_TITLE);

  menu->add(SIM->get_param("midimode", base));
  menu->add(SIM->get_param("midifile", base));
  menu->add(SIM->get_param("wavemode", base));
  menu->add(SIM->get_param("wavefile", base));
  menu->add(SIM->get_param("loglevel", base));
  menu->add(SIM->get_param("log", base));
  menu->add(SIM->get_param("dmatimer", base));
  SIM->get_param_enum("wavemode", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("wavefile", base)->set_handler(sb16_param_string_handler);
  SIM->get_param_num("midimode", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("midifile", base)->set_handler(sb16_param_string_handler);
  SIM->get_param_num("dmatimer", base)->set_handler(sb16_param_handler);
  SIM->get_param_num("loglevel", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("log", base)->set_handler(sb16_param_string_handler);
  // register handler for correct sb16 parameter handling after runtime config
  BX_SB16_THIS rt_conf_id = SIM->register_runtime_config_handler(this, runtime_config_handler);
  BX_SB16_THIS midi_changed = 0;
  BX_SB16_THIS wave_changed = 0;
}

void bx_sb16_c::reset(unsigned type)
{
}

void bx_sb16_c::register_state(void)
{
  unsigned i;
  char name[8];
  bx_list_c *chip, *ins_map, *patch;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "sb16", "SB16 State");
  bx_list_c *mpu = new bx_list_c(list, "mpu");
  new bx_shadow_bool_c(mpu, "uartmode", &MPU.uartmode);
  new bx_shadow_bool_c(mpu, "irqpending", &MPU.irqpending);
  new bx_shadow_bool_c(mpu, "forceuartmode", &MPU.forceuartmode);
  new bx_shadow_bool_c(mpu, "singlecommand", &MPU.singlecommand);
  new bx_shadow_num_c(mpu, "current_timer", &MPU.current_timer);
  new bx_shadow_num_c(mpu, "last_delta_time", &MPU.last_delta_time);
  bx_list_c *patchtbl = new bx_list_c(mpu, "patchtable");
  for (i=0; i<16; i++) {
    sprintf(name, "0x%02x", i);
    patch = new bx_list_c(patchtbl, name);
    new bx_shadow_num_c(patch, "banklsb", &MPU.banklsb[i]);
    new bx_shadow_num_c(patch, "bankmsb", &MPU.bankmsb[i]);
    new bx_shadow_num_c(patch, "program", &MPU.program[i]);
  }
  bx_list_c *dsp = new bx_list_c(list, "dsp");
  new bx_shadow_num_c(dsp, "resetport", &DSP.resetport, BASE_HEX);
  new bx_shadow_num_c(dsp, "speaker", &DSP.speaker, BASE_HEX);
  new bx_shadow_num_c(dsp, "prostereo", &DSP.prostereo, BASE_HEX);
  new bx_shadow_bool_c(dsp, "irqpending", &DSP.irqpending);
  new bx_shadow_bool_c(dsp, "midiuartmode", &DSP.midiuartmode);
  new bx_shadow_bool_c(dsp, "nondma_mode", &DSP.nondma_mode);
  new bx_shadow_num_c(dsp, "nondma_count", &DSP.nondma_count);
  new bx_shadow_num_c(dsp, "samplebyte", &DSP.samplebyte, BASE_HEX);
  new bx_shadow_num_c(dsp, "testreg", &DSP.testreg, BASE_HEX);
  bx_list_c *dma = new bx_list_c(dsp, "dma");
  new bx_shadow_num_c(dma, "mode", &DSP.dma.mode);
  new bx_shadow_num_c(dma, "bps", &DSP.dma.bps);
  new bx_shadow_num_c(dma, "timer", &DSP.dma.timer);
  new bx_shadow_bool_c(dma, "fifo", &DSP.dma.fifo);
  new bx_shadow_bool_c(dma, "output", &DSP.dma.output);
  new bx_shadow_bool_c(dma, "highspeed", &DSP.dma.highspeed);
  new bx_shadow_num_c(dma, "count", &DSP.dma.count);
  new bx_shadow_num_c(dma, "chunkindex", &DSP.dma.chunkindex);
  new bx_shadow_num_c(dma, "chunkcount", &DSP.dma.chunkcount);
  new bx_shadow_num_c(dma, "timeconstant", &DSP.dma.timeconstant);
  new bx_shadow_num_c(dma, "blocklength", &DSP.dma.blocklength);
  new bx_shadow_num_c(dma, "samplerate", &DSP.dma.param.samplerate);
  new bx_shadow_num_c(dma, "bits", &DSP.dma.param.bits);
  new bx_shadow_num_c(dma, "channels", &DSP.dma.param.channels);
  new bx_shadow_num_c(dma, "format", &DSP.dma.param.format);
  new bx_shadow_num_c(dma, "volume", &DSP.dma.param.volume);
  new bx_shadow_num_c(list, "fm_volume", &fm_volume);
  new bx_shadow_data_c(list, "chunk", DSP.dma.chunk, BX_SOUNDLOW_WAVEPACKETSIZE);
  new bx_shadow_data_c(list, "csp_reg", BX_SB16_THIS csp_reg, 256, 1);
  bx_list_c *opl = new bx_list_c(list, "opl");
  new bx_shadow_num_c(opl, "timer_running", &OPL.timer_running);
  for (i=0; i<2; i++) {
    sprintf(name, "chip%d", i+1);
    chip = new bx_list_c(opl, name);
    new bx_shadow_num_c(chip, "index", &OPL.index[i]);
    new bx_shadow_num_c(chip, "timer1", &OPL.timer[i*2]);
    new bx_shadow_num_c(chip, "timer2", &OPL.timer[i*2+1]);
    new bx_shadow_num_c(chip, "timerinit1", &OPL.timerinit[i*2]);
    new bx_shadow_num_c(chip, "timerinit2", &OPL.timerinit[i*2+1]);
    new bx_shadow_num_c(chip, "tmask", &OPL.tmask[i]);
    new bx_shadow_num_c(chip, "tflag", &OPL.tflag[i]);
  }
  new bx_shadow_num_c(list, "mixer_regindex", &MIXER.regindex, BASE_HEX);
  new bx_shadow_data_c(list, "mixer_reg", MIXER.reg, BX_SB16_MIX_REG, 1);
  bx_list_c *emul = new bx_list_c(list, "emul");
  new bx_shadow_num_c(emul, "remaps", &EMUL.remaps);
  bx_list_c *remap = new bx_list_c(emul, "remaplist");
  for (i=0; i<BX_SB16_MAX_REMAPS; i++) {
    sprintf(name, "0x%02x", i);
    ins_map = new bx_list_c(remap, name);
    new bx_shadow_num_c(ins_map, "oldbankmsb", &EMUL.remaplist[i].oldbankmsb);
    new bx_shadow_num_c(ins_map, "oldbanklsb", &EMUL.remaplist[i].oldbanklsb);
    new bx_shadow_num_c(ins_map, "oldprogch", &EMUL.remaplist[i].oldprogch);
    new bx_shadow_num_c(ins_map, "newbankmsb", &EMUL.remaplist[i].newbankmsb);
    new bx_shadow_num_c(ins_map, "newbanklsb", &EMUL.remaplist[i].newbanklsb);
    new bx_shadow_num_c(ins_map, "newprogch", &EMUL.remaplist[i].newprogch);
  }
  adlib_register_state(list);
}

void bx_sb16_c::after_restore_state(void)
{
  set_irq_dma();
  adlib_after_restore_state();
}

void bx_sb16_c::runtime_config_handler(void *this_ptr)
{
  bx_sb16_c *class_ptr = (bx_sb16_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_sb16_c::runtime_config(void)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  if (BX_SB16_THIS midi_changed != 0) {
    BX_SB16_THIS closemidioutput();
    if (BX_SB16_THIS midi_changed & 1) {
      BX_SB16_THIS midimode = SIM->get_param_num("midimode", base)->get();
      if (BX_SB16_THIS midimode & 2) {
        BX_SB16_MIDIOUT2 = DEV_sound_get_midiout(1);
        if (BX_SB16_MIDIOUT2 == NULL) {
          BX_PANIC(("Couldn't initialize midi file driver"));
        }
      }
    }
    // writemidicommand() re-opens the output device / file on demand
    BX_SB16_THIS midi_changed = 0;
  }
  if (BX_SB16_THIS wave_changed != 0) {
    if (BX_SB16_THIS wavemode & 2) {
      BX_SB16_THIS closewaveoutput();
    }
    if (BX_SB16_THIS wave_changed & 1) {
      BX_SB16_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
      DSP.outputinit = (BX_SB16_THIS wavemode & 1);
      if (BX_SB16_THIS wavemode & 2) {
        BX_SB16_WAVEOUT2 = DEV_sound_get_waveout(1);
        if (BX_SB16_WAVEOUT2 == NULL) {
          BX_PANIC(("Couldn't initialize wave file driver"));
        }
      }
    }
    // dsp_dma() re-opens the output file on demand
    BX_SB16_THIS wave_changed = 0;
  }
}

// the timer functions
void bx_sb16_c::mpu_timer (void *this_ptr)
{
  ((bx_sb16_c *) this_ptr)->mpu401.d.current_timer++;
}

void bx_sb16_c::dsp_dmatimer_handler(void *this_ptr)
{
  ((bx_sb16_c *) this_ptr)->dsp_dmatimer();
}

void bx_sb16_c::dsp_dmatimer(void)
{
  // raise the DRQ line. It is then lowered by the dma read / write functions
  // when the next byte has been sent / received.
  // However, don't do this if the next byte/word will fill up the
  // output buffer and the output functions are not ready yet
  // or if buffer is empty in input mode.

  if (!DSP.nondma_mode) {
    if ((DSP.dma.chunkindex + 1 < BX_SOUNDLOW_WAVEPACKETSIZE) &&
        (DSP.dma.count > 0)) {
      if (((DSP.dma.output == 0) && (DSP.dma.chunkcount > 0)) ||
          (DSP.dma.output == 1)) {
        if ((DSP.dma.param.bits == 8) || (BX_SB16_DMAH == 0)) {
          DEV_dma_set_drq(BX_SB16_DMAL, 1);
        } else {
          DEV_dma_set_drq(BX_SB16_DMAH, 1);
        }
      }
    }
  } else {
    dsp_getsamplebyte(0);
    dsp_getsamplebyte(DSP.samplebyte);
    dsp_getsamplebyte(0);
    dsp_getsamplebyte(DSP.samplebyte);
  }
}

void bx_sb16_c::opl_timer (void *this_ptr)
{
  ((bx_sb16_c *) this_ptr)->opl_timerevent();
}

// the various IO handlers

// The DSP/FM music part

// dsp_reset() resets the DSP after the sequence 1/0. Returns
// 0xaa on the data port
void bx_sb16_c::dsp_reset(Bit32u value)
{
  writelog(WAVELOG(4), "DSP Reset port write value %x", value);

  dsp_disable_nondma();

  // just abort high speed mode if it is set
  if (DSP.dma.highspeed != 0)
  {
      DSP.dma.highspeed = 0;
      writelog(WAVELOG(4), "High speed mode aborted");
      return;
  }

  if ((DSP.resetport == 1) && (value == 0))
  {
      // 1-0 sequences to reset port, do one of the following:
      // if in UART MIDI mode, abort it, don't reset
      // if in Highspeed mode (not SB16!), abort it, don't reset
      // otherwise reset

      if (DSP.midiuartmode != 0)
      {  // abort UART MIDI mode
         DSP.midiuartmode = 0;
         writelog(MIDILOG(4), "DSP UART MIDI mode aborted");
         return;
      }

      // do the reset
      writelog(WAVELOG(4), "DSP resetting...");

      if (DSP.irqpending != 0)
      {
         DEV_pic_lower_irq(BX_SB16_IRQ);
         writelog(WAVELOG(4), "DSP reset: IRQ untriggered");
      }
      if (DSP.dma.mode != 0)
      {
         writelog(WAVELOG(4), "DSP reset: DMA aborted");
         DSP.dma.mode = 1;  // no auto init anymore
         dsp_dmadone();
      }

      DSP.resetport = 0;
      DSP.speaker = 0;
      DSP.irqpending = 0;
      DSP.midiuartmode = 0;
      DSP.prostereo = 0;

      DSP.dma.mode = 0;
      DSP.dma.fifo = 0;
      DSP.dma.output = 0;
      DSP.dma.param.channels = 1;
      DSP.dma.count = 0;
      DSP.dma.highspeed = 0;
      DSP.dma.chunkindex = 0;

      DSP_B.dataout.reset();    // clear the buffers
      DSP_B.datain.reset();

      DSP_B.dataout.put(0xaa);  // acknowledge the reset
  }
  else
    DSP.resetport = value;
}

// dsp_dataread() reads the data port of the DSP
Bit32u bx_sb16_c::dsp_dataread()
{
  Bit8u value = 0xff;

  // if we are in MIDI UART mode, call the mpu401 part instead
  if (DSP.midiuartmode != 0)
    value = mpu_dataread();
  else
  {
    // default behaviour: if none available, return last byte again
    //  if (DSP_B.dataout.empty() == 0)
    DSP_B.dataout.get(&value);
  }

  writelog(WAVELOG(4), "DSP Data port read, result = %x", value);

  return(value);
}

// dsp_datawrite() writes a command or data byte to the data port
void bx_sb16_c::dsp_datawrite(Bit32u value)
{
  int bytesneeded;
  Bit8u index = 0, mode = 0, value8 = 0;
  Bit16u length = 0;

  writelog(WAVELOG(4), "DSP Data port write, value %x", value);

  // in high speed mode, any data passed to DSP is a sample
  if (DSP.dma.highspeed != 0)
  {
    dsp_getsamplebyte(value);
    return;
  }

  // route information to mpu401 part if in MIDI UART mode
  if (DSP.midiuartmode != 0)
  {
    mpu_datawrite(value);
    return;
  }

  if (DSP_B.datain.hascommand() == 1) // already a command pending, add to argument list
  {
    if (DSP_B.datain.put(value) == 0)
    {
       writelog(WAVELOG(3), "DSP command buffer overflow for command %02x",
                 DSP_B.datain.currentcommand());
    }
  }
  else // no command pending, set one up
  {
    bytesneeded = 0;   // find out how many arguments the command takes
    switch (value)
    {   // all fallbacks intended!
       case 0x04:
       case 0x0f:
       case 0x10:
       case 0x40:
       case 0x38:
       case 0xe0:
       case 0xe2:
       case 0xe4:
       case 0xf9:
         bytesneeded = 1;
         break;
       case 0x05:
       case 0x0e:
       case 0x14:
       case 0x16:
       case 0x17:
       case 0x41:
       case 0x42:
       case 0x48:
       case 0x74:
       case 0x75:
       case 0x76:
       case 0x77:
       case 0x80:
         bytesneeded = 2;
         break;
        // 0xb0 ... 0xbf:
       case 0xb0:
       case 0xb1:
       case 0xb2:
       case 0xb3:
       case 0xb4:
       case 0xb5:
       case 0xb6:
       case 0xb7:
       case 0xb8:
       case 0xb9:
       case 0xba:
       case 0xbb:
       case 0xbc:
       case 0xbd:
       case 0xbe:
       case 0xbf:

        // 0xc0 ... 0xcf:
       case 0xc0:
       case 0xc1:
       case 0xc2:
       case 0xc3:
       case 0xc4:
       case 0xc5:
       case 0xc6:
       case 0xc7:
       case 0xc8:
       case 0xc9:
       case 0xca:
       case 0xcb:
       case 0xcc:
       case 0xcd:
       case 0xce:
       case 0xcf:
         bytesneeded = 3;
         break;
    }
    DSP_B.datain.newcommand(value, bytesneeded);
  }

  if (DSP_B.datain.commanddone() == 1) // command is complete, process it
  {
      writelog(WAVELOG(4), "DSP command %x with %d arg bytes",
              DSP_B.datain.currentcommand(), DSP_B.datain.bytes());

      switch (DSP_B.datain.currentcommand())
      {
         // DSP commands - comments are the parameters for
         // this command, and/or the output

         // ASP commands (Advanced Signal Processor)
         // undocumented (?), just from looking what an SB16 does
       case 0x04:
         DSP_B.datain.get(&value8);
         break;

       case 0x05:
         DSP_B.datain.get(&value8);
         DSP_B.datain.get(&value8);
         break;

       case 0x0e:
         DSP_B.datain.get(&index);
         DSP_B.datain.get(&value8);
         BX_SB16_THIS csp_reg[index] = value;
         break;

       case 0x0f:
         DSP_B.datain.get(&index);
         DSP_B.dataout.put(BX_SB16_THIS csp_reg[index]);
         break;

         // direct mode DAC
       case 0x10:
         // 1: 8bit sample
         if (!DSP.nondma_mode) {
           DSP.dma.param.samplerate = 22050;
           DSP.dma.param.bits = 16;
           DSP.dma.param.channels = 2;
           DSP.dma.param.format = 1;
           DSP.dma.chunkcount = 8820;
           DSP.dma.chunkindex = 0;
           bx_pc_system.activate_timer(DSP.timer_handle, 45, 1);
           DSP.nondma_mode = 1;
           DSP.nondma_count = 0;
         }
         DSP_B.datain.get(&DSP.samplebyte);
         DSP.nondma_count++;
         break;

         // uncomp'd, normal DAC DMA
       case 0x14:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 0);
         break;

          // 2-bit comp'd, normal DAC DMA, no ref byte
       case 0x16:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 2);
         break;

         // 2-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x17:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 2|8);
         break;

         // uncomp'd, auto DAC DMA
       case 0x1c:
         // none
         dsp_dma(0xc4, 0x00, DSP.dma.blocklength, 0);
         break;

         // 2-bit comp'd, auto DAC DMA, 1 ref byte
       case 0x1f:
         // none
         dsp_dma(0xc4, 0x00, DSP.dma.blocklength, 2|8);
         break;

         // direct mode ADC
       case 0x20:
         // o1: 8bit sample
         DSP_B.dataout.put(0x80); // put a silence, for now.
         break;

         // uncomp'd, normal ADC DMA
       case 0x24:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc8, 0x00, length, 0);
         break;

         // uncomp'd, auto ADC DMA
       case 0x2c:
         // none
         dsp_dma(0xcc, 0x00, DSP.dma.blocklength, 0);
         break;

         // ? polling mode MIDI input
       case 0x30:
         break;

         // ? interrupt mode MIDI input
       case 0x31:
         break;

         // 0x34..0x37: UART mode MIDI output
       case 0x34:

         // UART mode MIDI input/output
       case 0x35:

         // UART polling mode MIDI IO with time stamp
       case 0x36:

         // UART interrupt mode MIDI IO with time stamp
       case 0x37:
         // Fallbacks intended - all set the midi uart mode
         DSP.midiuartmode = 1;
         break;

         // MIDI output
       case 0x38:
         DSP_B.datain.get(&value8);
         // route to mpu401 part
         mpu_datawrite(value8);
         break;

         // set time constant
       case 0x40:
         // 1: timeconstant
         DSP_B.datain.get(&value8);
         DSP.dma.timeconstant = value8 <<  8;
         DSP.dma.param.samplerate = (Bit32u) 256000000L / ((Bit32u) 65536L - (Bit32u) DSP.dma.timeconstant);
           break;

         // set samplerate for input
       case 0x41:
         // (fallback intended)

         // set samplerate for output
       case 0x42:
         // 1,2: hi(frq) lo(frq)
         DSP_B.datain.getw1(&(DSP.dma.param.samplerate));
         DSP.dma.timeconstant = 65536 - (Bit32u) 256000000 / (Bit32u) DSP.dma.param.samplerate;
         break;

         // set block length
       case 0x48:
         // 1,2: lo(blk len) hi(blk len)
         DSP_B.datain.getw(&(DSP.dma.blocklength));
         break;

         // 4-bit comp'd, normal DAC DMA, no ref byte
       case 0x74:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 4);
         break;

         // 4-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x75:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 4|8);
         break;

         // 3-bit comp'd, normal DAC DMA, no ref byte
       case 0x76:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 3);
         break;

         // 3-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x77:
         // 1,2: lo(length) hi(length)
         DSP_B.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 3|8);
         break;

         // 4-bit comp'd, auto DAC DMA, 1 ref byte
       case 0x7d:
         // none
         dsp_dma(0xc4, 0x00, DSP.dma.blocklength, 4|8);
         break;

         // 3-bit comp'd, auto DAC DMA, 1 ref byte
       case 0x7f:
         // none
         dsp_dma(0xc4, 0x00, DSP.dma.blocklength, 3|8);
         break;

         // silence period
       case 0x80:
         // 1,2: lo(silence) hi(silence) (len in samples)
         DSP_B.datain.getw(&length);
         // TODO
         break;

          // 8-bit auto DAC DMA, highspeed
       case 0x90:
         //none
         dsp_dma(0xc4, 0x00, DSP.dma.blocklength, 16);
         break;

         // 8-bit normal DAC DMA, highspeed
       case 0x91:
         //none
         dsp_dma(0xc0, 0x00, DSP.dma.blocklength, 16);
         break;

         // 8-bit auto ADC DMA, highspeed
       case 0x98:
         //none
         dsp_dma(0xcc, 0x00, DSP.dma.blocklength, 16);
         break;

       case 0x99: // 8-bit normal DMA
         //none
         dsp_dma(0xc8, 0x00, DSP.dma.blocklength, 16);
         break;

         // switch to mono for SBPro DAC/ADC
       case 0xa0:
         // none
         DSP.prostereo = 1;
         break;

         // switch to stereo for SBPro DAC/ADC
       case 0xa8:
         //// none
         DSP.prostereo = 2;
         break;

        // 0xb0 ... 0xbf:
        // 16 bit DAC/ADC DMA, general commands
        // fallback intended
        case 0xb0:
        case 0xb1:
        case 0xb2:
        case 0xb3:
        case 0xb4:
        case 0xb5:
        case 0xb6:
        case 0xb7:
        case 0xb8:
        case 0xb9:
        case 0xba:
        case 0xbb:
        case 0xbc:
        case 0xbd:
        case 0xbe:
        case 0xbf:

        // 0xc0 ... 0xcf:
        // 8 bit DAC/ADC DMA, general commands
        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
        case 0xc8:
        case 0xc9:
        case 0xca:
        case 0xcb:
        case 0xcc:
        case 0xcd:
        case 0xce:
        case 0xcf:
         DSP_B.datain.get(&mode);
         DSP_B.datain.getw(&length);
         dsp_dma(DSP_B.datain.currentcommand(), mode, length, 0);
         break;

         // pause 8 bit DMA transfer
       case 0xd0:
         // none
         if (DSP.dma.mode != 0)
           dsp_disabledma();
         break;

         // speaker on
       case 0xd1:
         // none
         DSP.speaker = 1;
         break;

         // speaker off
       case 0xd3:
         // none
         DSP.speaker = 0;
         break;

         // continue 8 bit DMA, see 0xd0
       case 0xd4:
         // none
         if (DSP.dma.mode != 0)
           dsp_enabledma();
         break;

          // pause 16 bit DMA
       case 0xd5:
         // none
         if (DSP.dma.mode != 0)
           dsp_disabledma();
         break;

         // continue 16 bit DMA, see 0xd5
       case 0xd6:
         // none
         if (DSP.dma.mode != 0)
           dsp_enabledma();
         break;

         // read speaker on/off (out ff=on, 00=off)
       case 0xd8:
         // none, o1: speaker; ff/00
         DSP_B.dataout.put((DSP.speaker == 1)?0xff:0x00);
         break;

         // stop 16 bit auto DMA
       case 0xd9:
         // none
         if (DSP.dma.mode != 0)
           {
             DSP.dma.mode = 1;  // no auto init anymore
             dsp_dmadone();
           }
         break;

         // stop 8 bit auto DMA
       case 0xda:
         // none
         if (DSP.dma.mode != 0)
         {
             DSP.dma.mode = 1;  // no auto init anymore
             dsp_dmadone();
         }
         break;

         // DSP identification
       case 0xe0:
         DSP_B.datain.get(&value8);
         DSP_B.dataout.put(~value8);
         break;

         // get version, out 2 bytes (major, minor)
       case 0xe1:
         // none, o1/2: version major.minor
         DSP_B.dataout.put(4);
         if (DSP_B.dataout.put(5) == 0)
         {
             writelog(WAVELOG(3), "DSP version couldn't be written - buffer overflow");
         }
         break;

       case 0xe2:
         DSP_B.datain.get(&value8);
         // TODO
         writelog(WAVELOG(3), "undocumented DSP command %x ignored (value = 0x%02x)",
                 DSP_B.datain.currentcommand(), value8);
         break;

       case 0xe3:
         // none, output: Copyright string
         // the Windows driver needs the exact text, otherwise it
         // won't load. Same for diagnose.exe
         DSP_B.dataout.puts("COPYRIGHT (C) CREATIVE TECHNOLOGY LTD, 1992.");
         DSP_B.dataout.put(0);    // need extra string end
         break;

         // write test register
       case 0xe4:
         DSP_B.datain.get(&DSP.testreg);
         break;

         // read test register
       case 0xe8:
         DSP_B.dataout.put(DSP.testreg);
         break;

         // Trigger 8-bit IRQ
       case 0xf2:
         DSP_B.dataout.put(0xaa);
         DSP.irqpending = 1;
         MIXER.reg[0x82] |= 1; // reg 82 shows the kind of IRQ
         DEV_pic_raise_irq(BX_SB16_IRQ);
         break;

         // ??? - Win98 needs this
        case 0xf9:
          DSP_B.datain.get(&value8);
          switch (value8) {
            case 0x0e:
              DSP_B.dataout.put(0xff);
              break;
            case 0x0f:
              DSP_B.dataout.put(0x07);
              break;
            case 0x37:
              DSP_B.dataout.put(0x38);
              break;
            default:
              DSP_B.dataout.put(0x00);
          }
          break;

         // unknown command
       default:
         writelog(WAVELOG(3), "unknown DSP command %x, ignored",
                 DSP_B.datain.currentcommand());
         break;
      }
      DSP_B.datain.clearcommand();
      DSP_B.datain.flush();
   }
}

// dsp_dma() initiates all kinds of dma transfers
void bx_sb16_c::dsp_dma(Bit8u command, Bit8u mode, Bit16u length, Bit8u comp)
{
  int ret;
  bx_list_c *base;
  bx_bool issigned;

  // command: 8bit, 16bit, in/out, single/auto, fifo
  // mode: mono/stereo, signed/unsigned
  //   (for info on command and mode see sound blaster programmer's manual,
  //    cmds bx and cx)
  // length: number of samples - not number of bytes
  // comp: bit-coded are: type of compression; ref-byte; highspeed
  //       D0..D2: 0=none, 2,3,4 bits ADPCM
  //       D3: ref-byte
  //       D6: highspeed

  writelog(WAVELOG(4), "DMA initialized. Cmd %02x, mode %02x, length %d, comp %d",
           command, mode, length, comp);

  dsp_disable_nondma();

  if ((command >> 4) == 0xb)  // 0xb? = 16 bit DMA
  {
    DSP.dma.param.bits = 16;
    DSP.dma.bps = 2;
  }
  else                        // 0xc? = 8 bit DMA
  {
    DSP.dma.param.bits = 8;
    DSP.dma.bps = 1;
  }

  // Prevent division by zero in some instances
  if (DSP.dma.param.samplerate == 0)
    DSP.dma.param.samplerate = 10752;
  command &= 0x0f;
  DSP.dma.output = 1 - (command >> 3);       // 1=output, 0=input
  DSP.dma.mode = 1 + ((command >> 2) & 1);  // 0=none, 1=normal, 2=auto
  DSP.dma.fifo = (command >> 1) & 1;         // ? not sure what this is

  DSP.dma.param.channels = ((mode >> 5) & 1) + 1;

  if (DSP.dma.param.channels == 2)
    DSP.dma.bps *= 2;

  DSP.dma.blocklength = length;
  issigned = (mode >> 4) & 1;
  DSP.dma.highspeed = (comp >> 4) & 1;

  DSP.dma.chunkindex = 0;
  DSP.dma.chunkcount = 0;

  Bit32u sampledatarate = (Bit32u) DSP.dma.param.samplerate * (Bit32u) DSP.dma.bps;
  if (DSP.dma.param.bits == 8 || (DSP.dma.param.bits == 16 && BX_SB16_DMAH != 0)) {
    DSP.dma.count = DSP.dma.blocklength;
  } else {
    DSP.dma.count = ((DSP.dma.blocklength + 1) << 1) - 1;
  }
  DSP.dma.timer = BX_SB16_THIS dmatimer * BX_DMA_BUFFER_SIZE / sampledatarate;

  writelog(WAVELOG(5), "DMA is %db, %dHz, %s, %s, mode %d, %s, %s, %d bps, %d usec/DMA",
           DSP.dma.param.bits, DSP.dma.param.samplerate,
           (DSP.dma.param.channels == 2)?"stereo":"mono",
           (DSP.dma.output == 1)?"output":"input", DSP.dma.mode,
           (issigned == 1)?"signed":"unsigned",
           (DSP.dma.highspeed == 1)?"highspeed":"normal speed",
           sampledatarate, DSP.dma.timer);

  DSP.dma.param.format = issigned | ((comp & 7) << 1) | ((comp & 8) << 4);

  // write the output to the device/file
  if (DSP.dma.output == 1) {
    if (BX_SB16_THIS wavemode & 2) {
      if ((DSP.outputinit & 2) == 0) {
        base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
        bx_param_string_c *waveparam = SIM->get_param_string("wavefile", base);
        if (BX_SB16_WAVEOUT2->openwaveoutput(waveparam->getptr()) == BX_SOUNDLOW_OK)
          DSP.outputinit |= 2;
        else
          DSP.outputinit &= ~2;
        if (((DSP.outputinit & BX_SB16_THIS wavemode) & 2) == 0) {
          writelog(WAVELOG(2), "Error opening file %s. Wave file output disabled.",
                   waveparam->getptr());
          BX_SB16_THIS wavemode = DSP.outputinit;
        }
      }
    }
    DSP.dma.chunkcount = sampledatarate / 10; // 0.1 sec
    if (DSP.dma.chunkcount > BX_SOUNDLOW_WAVEPACKETSIZE) {
      DSP.dma.chunkcount = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
  } else {
    if (DSP.inputinit == 0) {
      ret = BX_SB16_WAVEIN->openwaveinput(SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr(), sb16_adc_handler);
      if (ret != BX_SOUNDLOW_OK) {
        writelog(WAVELOG(2), "Error: Could not open wave input device.");
      } else {
        DSP.inputinit = 1;
      }
    }
    if (DSP.inputinit == 1) {
      ret = BX_SB16_WAVEIN->startwaverecord(&DSP.dma.param);
      if (ret != BX_SOUNDLOW_OK) {
        writelog(WAVELOG(2), "Error: Could not start wave record.");
      }
    }
    DSP.dma.chunkcount = 0;
  }

  dsp_enabledma();
}

Bit32u bx_sb16_c::sb16_adc_handler(void *this_ptr, Bit32u buflen)
{
  bx_sb16_c *class_ptr = (bx_sb16_c*)this_ptr;
  class_ptr->dsp_adc_handler(buflen);
  return 0;
}

Bit32u bx_sb16_c::dsp_adc_handler(Bit32u buflen)
{
  Bit32u len;

  len = DSP.dma.chunkcount - DSP.dma.chunkindex;
  if (len > 0) {
    memmove(DSP.dma.chunk, DSP.dma.chunk+DSP.dma.chunkindex, len);
    DSP.dma.chunkcount = len;
  }
  DSP.dma.chunkindex = 0;
  if ((DSP.dma.chunkcount + buflen) > BX_SOUNDLOW_WAVEPACKETSIZE) {
    DSP.dma.chunkcount = BX_SOUNDLOW_WAVEPACKETSIZE;
    len = DSP.dma.chunkcount + buflen - BX_SOUNDLOW_WAVEPACKETSIZE;
    BX_DEBUG(("dsp_adc_handler(): unhandled len=%d", len));
  } else {
    DSP.dma.chunkcount += buflen;
    len = 0;
  }
  BX_SB16_WAVEIN->getwavepacket(DSP.dma.chunkcount, DSP.dma.chunk);
  return len;
}

// dsp_enabledma(): Start the DMA timer and thus the transfer

void bx_sb16_c::dsp_enabledma()
{
  bx_pc_system.activate_timer(DSP.timer_handle, DSP.dma.timer, 1);
}

// dsp_disabledma(): Stop the DMA timer and thus the transfer, but don't abort it
void bx_sb16_c::dsp_disabledma()
{
  bx_pc_system.deactivate_timer(DSP.timer_handle);
}

void bx_sb16_c::dsp_disable_nondma()
{
  if (DSP.nondma_mode) {
    bx_pc_system.deactivate_timer(DSP.timer_handle);
    DSP.nondma_mode = 0;
  }
}

// dsp_bufferstatus() checks if the DSP is ready for data/commands
Bit32u bx_sb16_c::dsp_bufferstatus()
{
  Bit32u result = 0x7f;

  // MSB set -> not ready for commands
  if (DSP_B.datain.full() == 1) result |= 0x80;

  writelog(WAVELOG(4), "DSP Buffer status read, result %x", result);

  return(result);
}

// dsp_status() checks if the DSP is ready to send data
Bit32u bx_sb16_c::dsp_status()
{
  Bit32u result = 0x7f;

  // read might be to acknowledge IRQ
  if (DSP.irqpending != 0)
  {
      MIXER.reg[0x82] &= (~0x01);
      writelog(WAVELOG(4), "8-bit DMA or SBMIDI IRQ acknowledged");
      if ((MIXER.reg[0x82] & 0x07) == 0) {
        DSP.irqpending = 0;
        DEV_pic_lower_irq(BX_SB16_IRQ);
      }
  }

  // if buffer is not empty, there is data to be read
  if (DSP_B.dataout.empty() == 0) result |= 0x80;

  writelog(WAVELOG(4), "DSP output status read, result %x", result);

  return(result);
}

// dsp_irq16ack() notifies that the 16bit DMA IRQ has been acknowledged
Bit32u bx_sb16_c::dsp_irq16ack()
{
  Bit32u result = 0xff;

  if (DSP.irqpending != 0)
  {
      MIXER.reg[0x82] &= (~0x02);
      if ((MIXER.reg[0x82] & 0x07) == 0) {
        DSP.irqpending = 0;
        DEV_pic_lower_irq(BX_SB16_IRQ);
      }
      writelog(WAVELOG(4), "16-bit DMA IRQ acknowledged");
  }
  else
    writelog(WAVELOG(3), "16-bit DMA IRQ acknowledged but not active!");

  return result;
}


// the DMA handlers

// highlevel input and output handlers - rerouting to/from file,device

// write a wave packet to the output device
void bx_sb16_c::dsp_sendwavepacket()
{
  if (DSP.nondma_mode) {
    if (DSP.nondma_count == 0) {
      dsp_disable_nondma();
      return;
    }
    DSP.nondma_count = 0;
  }

  if (DSP.dma.chunkindex == 0)
    return;

  if (BX_SB16_THIS wavemode & 1) {
    BX_SB16_WAVEOUT1->sendwavepacket(DSP.dma.chunkindex, DSP.dma.chunk, &DSP.dma.param);
  }
  if (BX_SB16_THIS wavemode & 2) {
    BX_SB16_WAVEOUT2->sendwavepacket(DSP.dma.chunkindex, DSP.dma.chunk, &DSP.dma.param);
  }
  DSP.dma.chunkindex = 0;
}

// put a sample byte into the output buffer
void bx_sb16_c::dsp_getsamplebyte(Bit8u value)
{
  if (DSP.dma.chunkindex < DSP.dma.chunkcount)
    DSP.dma.chunk[DSP.dma.chunkindex++] = value;

  if (DSP.dma.chunkindex >= DSP.dma.chunkcount)
    dsp_sendwavepacket();
}

// read a sample byte from the input buffer
Bit8u bx_sb16_c::dsp_putsamplebyte()
{
  Bit8u value = DSP.dma.chunk[DSP.dma.chunkindex++];

  if (DSP.dma.chunkindex >= DSP.dma.chunkcount) {
    DSP.dma.chunkcount = 0;
    DSP.dma.chunkindex = 0;
  }

  return value;
}

// called when the last byte of a DMA transfer has been received/sent
void bx_sb16_c::dsp_dmadone()
{
  writelog(WAVELOG(4), "DMA transfer done, triggering IRQ");

  if ((DSP.dma.output == 1) && (DSP.dma.mode != 2)) {
    dsp_sendwavepacket();  // flush the output
  } else if ((DSP.dma.output == 0) && (DSP.dma.mode != 2)) {
    BX_SB16_WAVEIN->stopwaverecord();
  }

  // generate the appropriate IRQ
  if (DSP.dma.param.bits == 8)
    MIXER.reg[0x82] |= 1;
  else
    MIXER.reg[0x82] |= 2;

  DEV_pic_raise_irq(BX_SB16_IRQ);
  DSP.irqpending = 1;

  // if auto-DMA, reinitialize
  if (DSP.dma.mode == 2)
  {
      if (DSP.dma.param.bits == 8 || (DSP.dma.param.bits == 16 && BX_SB16_DMAH != 0)) {
        DSP.dma.count = DSP.dma.blocklength;
      } else {
        DSP.dma.count = ((DSP.dma.blocklength + 1) << 1) - 1;
      }
      writelog(WAVELOG(4), "auto-DMA reinitializing to length %d", DSP.dma.count);
  }
  else
  {
      DSP.dma.mode = 0;
      dsp_disabledma();
  }
}

// now the actual transfer routines, called by the DMA controller
// note that read = from application to soundcard (output),
// and write = from soundcard to application (input)
Bit16u bx_sb16_c::dma_read8(Bit8u *buffer, Bit16u maxlen)
{
  Bit16u len = 0;

  DEV_dma_set_drq(BX_SB16_DMAL, 0);  // the timer will raise it again

  writelog(WAVELOG(5), "Received 8-bit DMA: 0x%02x, %d remaining ",
           buffer[0], DSP.dma.count);

  do {
    dsp_getsamplebyte(buffer[len++]);
    DSP.dma.count--;
  } while ((len < maxlen) && (DSP.dma.count != 0xffff));

  if (DSP.dma.count == 0xffff) // last byte received
    dsp_dmadone();

  return len;
}

Bit16u bx_sb16_c::dma_write8(Bit8u *buffer, Bit16u maxlen)
{
  Bit16u len = 0;

  DEV_dma_set_drq(BX_SB16_DMAL, 0);  // the timer will raise it again

  do {
    buffer[len++] = dsp_putsamplebyte();
    DSP.dma.count--;
  } while ((len < maxlen) && (DSP.dma.count != 0xffff));

  writelog(WAVELOG(5), "Sent 8-bit DMA: 0x%02x, %d remaining ",
           buffer[0], DSP.dma.count);

  if (DSP.dma.count == 0xffff) // last byte sent
    dsp_dmadone();

  return len;
}

Bit16u bx_sb16_c::dma_read16(Bit16u *buffer, Bit16u maxlen)
{
  Bit16u len = 0;
  Bit8u *buf8;

  DEV_dma_set_drq(BX_SB16_DMAH, 0);  // the timer will raise it again

  writelog(WAVELOG(5), "Received 16-bit DMA: 0x%04x, %d remaining ",
           buffer[0], DSP.dma.count);

  do {
    buf8 = (Bit8u*)(buffer+len);
    dsp_getsamplebyte(buf8[0]);
    dsp_getsamplebyte(buf8[1]);
    len++;
    DSP.dma.count--;
  } while ((len < maxlen) && (DSP.dma.count != 0xffff));

  if (DSP.dma.count == 0xffff) // last word received
    dsp_dmadone();

  return len;
}

Bit16u bx_sb16_c::dma_write16(Bit16u *buffer, Bit16u maxlen)
{
  Bit16u len = 0;
  Bit8u *buf8;

  DEV_dma_set_drq(BX_SB16_DMAH, 0);  // the timer will raise it again

  do {
    buf8 = (Bit8u*)(buffer+len);
    buf8[0] = dsp_putsamplebyte();
    buf8[1] = dsp_putsamplebyte();
    len++;
    DSP.dma.count--;
  } while ((len < maxlen) && (DSP.dma.count != 0xffff));

  writelog(WAVELOG(5), "Sent 16-bit DMA: 0x%4x, %d remaining ",
           buffer[0], DSP.dma.count);

  if (DSP.dma.count == 0xffff) // last word sent
    dsp_dmadone();

  return len;
}

Bit16u bx_sb16_c::calc_output_volume(Bit8u reg1, Bit8u reg2, bx_bool shift)
{
  Bit8u vol1, vol2;
  float fvol1, fvol2;
  Bit16u result;

  vol1 = (MIXER.reg[reg1] >> 3);
  vol2 = (MIXER.reg[reg2] >> 3);
  fvol1 = pow(10.0f, (float)(31-vol1)*-0.065f);
  fvol2 = pow(10.0f, (float)(31-vol2)*-0.065f);
  result = (Bit8u)(255 * fvol1 * fvol2);
  if (shift) result <<= 8;
  return result;
}

// the mixer, supported type is CT1745 (as in an SB16)
void bx_sb16_c::mixer_writedata(Bit32u value)
{
  int i;
  Bit8u set_output_vol = 0;

  // do some action depending on what register was written
  switch (MIXER.regindex)
  {
    case 0:  // initialize mixer
      writelog(BOTHLOG(4), "Initializing mixer...");
      MIXER.reg[0x04] = 0xcc;
      MIXER.reg[0x0a] = 0x00;
      MIXER.reg[0x22] = 0xcc;
      MIXER.reg[0x26] = 0xcc;
      MIXER.reg[0x28] = 0x00;
      MIXER.reg[0x2e] = 0x00;
      MIXER.reg[0x3c] = 0x1f;
      MIXER.reg[0x3d] = 0x15;
      MIXER.reg[0x3e] = 0x0b;
      for (i=0x30; i<=0x35; i++)
        MIXER.reg[i] = 0xc0;
      for (i=0x36; i<=0x3b; i++)
        MIXER.reg[i] = 0x00;
      for (i=0x3f; i<=0x43; i++)
        MIXER.reg[i] = 0x00;
      for (i=0x44; i<=0x47; i++)
        MIXER.reg[i] = 0x80;

      MIXER.regindex = 0;   // next mixer register read is register 0
      set_output_vol = 3;
      break;

    case 0x04: // DAC level
      MIXER.reg[0x32] = (value & 0xf0) | 0x08;
      MIXER.reg[0x33] = ((value & 0x0f) << 4) | 0x08;
      set_output_vol = 1;
      break;

    case 0x0a: // microphone level
      MIXER.reg[0x3a] = (value << 5) | 0x18;
      break;

    case 0x22: // master volume
      MIXER.reg[0x30] = (value & 0xf0) | 0x08;
      MIXER.reg[0x31] = ((value & 0x0f) << 4) | 0x08;
      set_output_vol = 3;
      break;

    case 0x26: // FM level
      MIXER.reg[0x34] = (value & 0xf0) | 0x08;
      MIXER.reg[0x35] = ((value & 0x0f) << 4) | 0x08;
      set_output_vol = 2;
      break;

    case 0x28: // CD audio level
      MIXER.reg[0x36] = (value & 0xf0) | 0x08;
      MIXER.reg[0x37] = ((value & 0x0f) << 4) | 0x08;
      break;

    case 0x2e: // line in level
      MIXER.reg[0x38] = (value & 0xf0) | 0x08;
      MIXER.reg[0x39] = ((value & 0x0f) << 4) | 0x08;
      break;

    case 0x30: // master volume left
      MIXER.reg[0x22] &= 0x0f;
      MIXER.reg[0x22] |= (value & 0xf0);
      set_output_vol = 3;
      break;

    case 0x31: // master volume right
      MIXER.reg[0x22] &= 0xf0;
      MIXER.reg[0x22] |= (value >> 4);
      set_output_vol = 3;
      break;

    case 0x32: // DAC level left
      MIXER.reg[0x04] &= 0x0f;
      MIXER.reg[0x04] |= (value & 0xf0);
      set_output_vol = 1;
      break;

    case 0x33: // DAC level right
      MIXER.reg[0x04] &= 0xf0;
      MIXER.reg[0x04] |= (value >> 4);
      set_output_vol = 1;
      break;

    case 0x34: // FM level left
      MIXER.reg[0x26] &= 0x0f;
      MIXER.reg[0x26] |= (value & 0xf0);
      set_output_vol = 2;
      break;

    case 0x35: // FM level right
      MIXER.reg[0x26] &= 0xf0;
      MIXER.reg[0x26] |= (value >> 4);
      set_output_vol = 2;
      break;

    case 0x36: // CD audio level left
      MIXER.reg[0x28] &= 0x0f;
      MIXER.reg[0x28] |= (value & 0xf0);
      break;

    case 0x37: // CD audio level right
      MIXER.reg[0x28] &= 0xf0;
      MIXER.reg[0x28] |= (value >> 4);
      break;

    case 0x38: // line in level left
      MIXER.reg[0x2e] &= 0x0f;
      MIXER.reg[0x2e] |= (value & 0xf0);
      break;

    case 0x39: // line in level right
      MIXER.reg[0x2e] &= 0xf0;
      MIXER.reg[0x2e] |= (value >> 4);
      break;

    case 0x3a: // microphone level
      MIXER.reg[0x0a] = (value >> 5);
      break;

    case 0x3b:
    case 0x3c:
    case 0x3d:
    case 0x3e:
    case 0x3f:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
      break;

    case 0x80: // IRQ mask
    case 0x81: // DMA mask
      MIXER.reg[MIXER.regindex] = value;
      set_irq_dma(); // both 0x80 and 0x81 handled
      return;

    default: // ignore read-only registers
      return;
  }

  // store the value
  if (MIXER.regindex != 0) {
    MIXER.reg[MIXER.regindex] = value;
  }

  if (set_output_vol & 1) {
    DSP.dma.param.volume = calc_output_volume(0x30, 0x32, 0);
    DSP.dma.param.volume |= calc_output_volume(0x31, 0x33, 1);
  }
  if (set_output_vol & 2) {
    BX_SB16_THIS fm_volume = calc_output_volume(0x30, 0x34, 0);
    BX_SB16_THIS fm_volume |= calc_output_volume(0x31, 0x35, 1);
  }

  writelog(BOTHLOG(4), "mixer register %02x set to %02x",
          MIXER.regindex, MIXER.reg[MIXER.regindex]);
}

Bit32u bx_sb16_c::mixer_readdata()
{
  writelog(BOTHLOG(4), "read from mixer register %02x returns %02x",
          MIXER.regindex, MIXER.reg[MIXER.regindex]);
  return(MIXER.reg[MIXER.regindex]);
}

void bx_sb16_c::mixer_writeregister(Bit32u value)
{
  MIXER.regindex = value;
}

void bx_sb16_c::set_irq_dma()
{
  static bx_bool isInitialized=0;
  int newirq;
  int oldDMA8, oldDMA16;

  // set the IRQ according to the value in mixer register 0x80
  switch (MIXER.reg[0x80])
  {
    case 1:
      newirq = 2;
      break;
    case 2:
      newirq = 5;
      break;
    case 4:
      newirq = 7;
      break;
    case 8:
      newirq = 10;
      break;
    default:
      newirq = 5;
      writelog(BOTHLOG(3), "Bad value %02x in mixer register 0x80. IRQ set to %d",
              MIXER.reg[0x80], newirq);
      MIXER.reg[0x80] = 2;
  }
  if (newirq != BX_SB16_IRQ)   // a different IRQ was set
  {
      if (BX_SB16_IRQ > 0)
        DEV_unregister_irq(BX_SB16_IRQ, "SB16");

      BX_SB16_IRQ = newirq;
      DEV_register_irq(BX_SB16_IRQ, "SB16");
  }

  // set the 8 bit DMA
  oldDMA8=BX_SB16_DMAL;
  switch (MIXER.reg[0x81] & 0x0f)
  {
    case 1:
      BX_SB16_DMAL = 0;
      break;
    case 2:
      BX_SB16_DMAL = 1;
      break;
    case 8:
      BX_SB16_DMAL = 3;
      break;
    default:
      BX_SB16_DMAL = 1;
      writelog(BOTHLOG(3), "Bad value %02x in mixer register 0x81. DMA8 set to %d",
              MIXER.reg[0x81], BX_SB16_DMAL);
      MIXER.reg[0x81] &= (~0x0f);
      MIXER.reg[0x81] |= (1 << BX_SB16_DMAL);
  }

  // Unregister the previous DMA if initialized
  if ((isInitialized) && (oldDMA8 != BX_SB16_DMAL))
    DEV_dma_unregister_channel(oldDMA8);

  // And register the new 8bits DMA Channel
  if ((!isInitialized) || (oldDMA8 != BX_SB16_DMAL))
    DEV_dma_register_8bit_channel(BX_SB16_DMAL, dma_read8, dma_write8, "SB16");

  // and the 16 bit DMA
  oldDMA16=BX_SB16_DMAH;
  switch (MIXER.reg[0x81] >> 4)
  {
    case 0:
      BX_SB16_DMAH = 0;    // no 16-bit DMA
      break;
    case 2:
      BX_SB16_DMAH = 5;
      break;
    case 4:
      BX_SB16_DMAH = 6;
      break;
    case 8:
      BX_SB16_DMAH = 7;
      break;
    default:
      BX_SB16_DMAH = 0;
      writelog(BOTHLOG(3), "Bad value %02x in mixer register 0x81. DMA16 set to %d",
              MIXER.reg[0x81], BX_SB16_DMAH);
      MIXER.reg[0x81] &= (~0xf0);
      // MIXER.reg[0x81] |= (1 << BX_SB16_DMAH);
      // no default 16 bit channel!
  }

  // Unregister the previous DMA if initialized
  if ((isInitialized) && (oldDMA16 != 0) && (oldDMA16 != BX_SB16_DMAH))
    DEV_dma_unregister_channel(oldDMA16);

  // And register the new 16bits DMA Channel
  if ((BX_SB16_DMAH != 0) && (oldDMA16 != BX_SB16_DMAH))
    DEV_dma_register_16bit_channel(BX_SB16_DMAH, dma_read16, dma_write16, "SB16");

  // If not already initialized
  if(!isInitialized) {
    isInitialized=1;
  } else {
    writelog(BOTHLOG(1), "Resources set to I%d D%d H%d",
             BX_SB16_IRQ, BX_SB16_DMAL, BX_SB16_DMAH);
  }
}


// now the MPU 401 part

// the MPU 401 status port shows if input or output are ready
// Note that the bits are inverse to their meaning

Bit32u bx_sb16_c::mpu_status()
{
  Bit32u result = 0;

  if ((MPU_B.datain.full() == 1) ||
       ((BX_SB16_THIS midimode & 1) &&
        (BX_SB16_MIDIOUT1->midiready() == BX_SOUNDLOW_ERR)))
    result |= 0x40;       // output not ready
  if (MPU_B.dataout.empty() == 1)
    result |= 0x80;       // no input available

  writelog(MIDILOG(4), "MPU status port, result %02x", result);

  return(result);
}

// the MPU 401 command port

void bx_sb16_c::mpu_command(Bit32u value)
{
  int i;
  int bytesneeded;

  if (MPU_B.cmd.hascommand() == 1) // already a command pending, abort that one
  {
      if ((MPU_B.cmd.currentcommand() != value) ||
          (MPU_B.cmd.commanddone() == 0))
            // it's a different command, or the old one isn't done yet, abort it
       {
         MPU_B.cmd.clearcommand();
         MPU_B.cmd.flush();
       }

       // if it's the same one, and we just completed the argument list,
       // we leave it as it is and process it here
  }

  if (MPU_B.cmd.hascommand() == 0)  // no command pending, set one up
  {
      bytesneeded = 0;
      if ((value >> 4) == 14) bytesneeded = 1;
      MPU_B.cmd.newcommand(value, bytesneeded);
  }

  if (MPU_B.cmd.commanddone() == 1) // command is complete, process it
  {
     switch (MPU_B.cmd.currentcommand())
     {
        case 0x3f:
          writelog(MIDILOG(5), "MPU cmd: UART mode on");
          MPU.uartmode=1;
          MPU.irqpending=1;
          MPU.singlecommand=0;
          if (BX_SB16_IRQMPU != -1) {
             MIXER.reg[0x82] |= 4;
             DEV_pic_raise_irq(BX_SB16_IRQMPU);
          }
          break;

        case 0xff:
          writelog(MIDILOG(4), "MPU cmd: Master reset of device");
          MPU.uartmode=MPU.forceuartmode;
          MPU.singlecommand=0;
          for (i=0; i<16; i++)
          {
             MPU.banklsb[i] = 0;
             MPU.bankmsb[i] = 0;
             MPU.program[i] = 0;
          }
          MPU_B.cmd.reset();
          MPU_B.dataout.reset();
          MPU_B.datain.reset();
          MPU_B.midicmd.reset();
          break;
       case 0xd0:  // d0 and df: prefix for midi command
       case 0xdf:  // like uart mode, but only a single command
          MPU.singlecommand = 1;
          writelog(MIDILOG(4), "MPU: prefix %02x received",
                  MPU_B.cmd.currentcommand());
          break;
       default:
          writelog(MIDILOG(3), "MPU cmd: unknown command %02x ignored",
                  MPU_B.cmd.currentcommand());
          break;
     }

     // Need to put an MPU_ACK into the data port if command successful
     // we'll fake it even if we didn't process the command, so as to
     // allow detection of the MPU 401.
     if (MPU_B.dataout.put(0xfe) == 0)
        writelog(MIDILOG(3), "MPU_ACK error - output buffer full");
     MPU_B.cmd.clearcommand(); // clear the command from the buffer
  }
}


// MPU 401 data port/read: contains an MPU_ACK after receiving a command
// Will contain other data as well when other than UART mode is supported

Bit32u bx_sb16_c::mpu_dataread()
{
  Bit8u res8bit;
  Bit32u result;

  // also acknowledge IRQ?
  if (MPU.irqpending != 0)
  {
     MPU.irqpending = 0;
     MIXER.reg[0x82] &= (~4);
     if ((MIXER.reg[0x82] & 0x07) == 0)
       DEV_pic_lower_irq(BX_SB16_IRQMPU);
     writelog(MIDILOG(4), "MPU IRQ acknowledged");
  }

  if (MPU_B.dataout.get(&res8bit) == 0) {
     writelog(MIDILOG(3), "MPU data port not ready - no data in buffer");
     result = 0xff;
  }
  else
    result = (Bit32u) res8bit;

  writelog(MIDILOG(4), "MPU data port, result %02x", result);

  return(result);
}


// MPU 401 data port/write: This is where the midi stream comes from,
// as well as arguments to any pending command

void bx_sb16_c::mpu_datawrite(Bit32u value)
{
  writelog(MIDILOG(4), "write to MPU data port, value %02x", value);

  if (MPU_B.cmd.hascommand() == 1)
  { // there is a command pending, add arguments to it
      if (MPU_B.cmd.put(value) == 0)
       writelog(MIDILOG(3), "MPU Command arguments too long - buffer full");
      if (MPU_B.cmd.commanddone() == 1)
       BX_SB16_THIS mpu_command(MPU_B.cmd.currentcommand());
  }
  else if ((MPU.uartmode == 0) && (MPU.singlecommand == 0))
  {
      // Hm? No UART mode, but still data? Maybe should send it
      // to the command port...  Only SBMPU401.EXE does this...
      writelog(MIDILOG(4), "MPU Data %02x received but no UART mode. Assuming it's a command.", value);
      mpu_command(value);
      return;
  }
  else // no MPU command pending, in UART mode, this has to be midi data
    mpu_mididata(value);
}

// A byte of midi data has been received
void bx_sb16_c::mpu_mididata(Bit32u value)
{
  // first, find out if it is a midi command or midi data
  bx_bool ismidicommand = 0;
  if (value >= 0x80)
  {  // bit 8 usually denotes a midi command...
      ismidicommand = 1;
      if ((value == 0xf7) && (MPU_B.midicmd.currentcommand() == 0xf0))
      // ...except if it is a continuing SYSEX message, then it just
      // denotes the end of a SYSEX chunk, not the start of a message
      {
         ismidicommand = 0;     // first, it's not a command
         MPU_B.midicmd.newcommand(MPU_B.midicmd.currentcommand(),
                             MPU_B.midicmd.bytes());
         // Then, set needed bytes to current buffer
         // because we didn't know the length before
      }
  }

  if (ismidicommand == 1)
  {  // this is a command, check if an old one is pending
      if (MPU_B.midicmd.hascommand() == 1)
      {
         writelog(MIDILOG(3), "Midi command %02x incomplete, has %d of %d bytes.",
                 MPU_B.midicmd.currentcommand(), MPU_B.midicmd.bytes(),
                 MPU_B.midicmd.commandbytes());
         // write as much as we can. Should we do this?
         processmidicommand(0);
         // clear the pending command
         MPU_B.midicmd.clearcommand();
         MPU_B.midicmd.flush();
      }

      // find the number of arguments to the command
      static const signed eventlength[] = { 2, 2, 2, 2, 1, 1, 2, 255};
      // note - length 255 commands have unknown length
      MPU_B.midicmd.newcommand(value, eventlength[(value & 0x70) >> 4]);
  }
  else  // no command, just arguments to the old command
  {
      if (MPU_B.midicmd.hascommand() == 0)
      {  // no command pending, ignore the data
         writelog(MIDILOG(3), "Midi data %02x received, but no command pending?", value);
         return;
      }

      // just some data to the command
      if (MPU_B.midicmd.put(value) == 0)
       writelog(MIDILOG(3), "Midi buffer overflow!");
      if (MPU_B.midicmd.commanddone() == 1)
      {
         // the command is complete, process it
         writelog(MIDILOG(5), "Midi command %02x complete, has %d bytes.",
                 MPU_B.midicmd.currentcommand(), MPU_B.midicmd.bytes());
         processmidicommand(0);
         // and remove the command from the buffer
         MPU_B.midicmd.clearcommand();
         MPU_B.midicmd.flush();
      }
  }
}

// The emulator port/read: See if commands were successful

Bit32u bx_sb16_c::emul_read()
{
  Bit8u res8bit;
  Bit32u result;

  if (EMUL.datain.get(&res8bit) == 0)
  {
     writelog(3, "emulator port not ready - no data in buffer");
     result = 0x00;
  }
  else result = (Bit32u) res8bit;

  writelog(4, "emulator port, result %02x", result);

  return(result);
}

// Emulator port/write: Changing instrument mapping etc.

void bx_sb16_c::emul_write(Bit32u value)
{
  Bit8u value8 = 0;

  writelog(4, "write to emulator port, value %02x", value);

  if (EMUL.dataout.hascommand() == 0)   // no command pending, set it up
  {
    static signed char cmdlength[] = { 0, 0, 4, 2, 6, 1, 0, 0, 1, 1, 0, 1};
    if (value > 11)
    {
       writelog(3, "emulator command %02x unknown, ignored.", value);
       return;
    }
    writelog(5, "emulator command %02x, needs %d arguments",
              value, cmdlength[value]);
    EMUL.dataout.newcommand(value, cmdlength[value]);
    EMUL.datain.reset();
    EMUL.datain.put(0xfe);
  }
  else
    EMUL.dataout.put(value);    // otherwise just add data

  if (EMUL.dataout.commanddone() == 1)
  {  // process the command
     writelog(4, "executing emulator command %02x with %d arguments",
              EMUL.dataout.currentcommand(), EMUL.dataout.bytes());
     switch (EMUL.dataout.currentcommand())
     {
       case 0: // reinit of emulator
         writelog(4, "Emulator reinitialized");
         EMUL.remaps = 0;
         EMUL.dataout.reset();
         EMUL.datain.reset();
         EMUL.datain.put(0xfe);
         break;
       case 1: // dummy command to reset state of emulator port
               // just give a few times to end any commands
         break;
       case 2: // map bank
         if (EMUL.remaps >= BX_SB16_MAX_REMAPS) break;
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldbankmsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldbanklsb));
         EMUL.remaplist[EMUL.remaps].oldprogch = 0xff;
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newbankmsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newbanklsb));
         EMUL.remaplist[EMUL.remaps].newprogch = 0xff;
         EMUL.datain.put(4);
         writelog(4, "Map bank command received, from %d %d to %d %d",
                 EMUL.remaplist[EMUL.remaps].oldbankmsb,
                 EMUL.remaplist[EMUL.remaps].oldbanklsb,
                 EMUL.remaplist[EMUL.remaps].newbankmsb,
                 EMUL.remaplist[EMUL.remaps].newbanklsb);
         EMUL.remaps++;
         break;
       case 3: // map program change
         if (EMUL.remaps >= BX_SB16_MAX_REMAPS) break;
         EMUL.remaplist[EMUL.remaps].oldbankmsb = 0xff;
         EMUL.remaplist[EMUL.remaps].oldbanklsb = 0xff;
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldprogch));
         EMUL.remaplist[EMUL.remaps].newbankmsb = 0xff;
         EMUL.remaplist[EMUL.remaps].newbanklsb = 0xff;
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newprogch));
         EMUL.datain.put(2);
         writelog(4, "Map program change received, from %d to %d",
                 EMUL.remaplist[EMUL.remaps].oldprogch,
                 EMUL.remaplist[EMUL.remaps].newprogch);
         EMUL.remaps++;
         break;
       case 4: // map bank and program change
         if (EMUL.remaps >= BX_SB16_MAX_REMAPS) break;
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldbankmsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldbanklsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].oldprogch));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newbankmsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newbanklsb));
         EMUL.dataout.get (& (EMUL.remaplist[EMUL.remaps].newprogch));
         EMUL.datain.put(6);
         writelog(4, "Complete remap received, from %d %d %d to %d %d %d",
                 EMUL.remaplist[EMUL.remaps].oldbankmsb,
                 EMUL.remaplist[EMUL.remaps].oldbanklsb,
                 EMUL.remaplist[EMUL.remaps].oldprogch,
                 EMUL.remaplist[EMUL.remaps].newbankmsb,
                 EMUL.remaplist[EMUL.remaps].newbanklsb,
                 EMUL.remaplist[EMUL.remaps].newprogch);

         EMUL.remaps++;
         break;
       case 5: EMUL.dataout.get(&value8); // dump emulator state
         switch (value8)
         {
           case 0:
             EMUL.datain.puts("SB16 Emulator for Bochs\n");
             break;
           case 1:
             EMUL.datain.puts("UART mode=%d (force=%d)\n",
                            MPU.uartmode, MPU.forceuartmode);
             break;
           case 2:
             EMUL.datain.puts("timer=%d\n", MPU.current_timer);
             break;
           case 3:
             EMUL.datain.puts("%d remappings active\n", EMUL.remaps);
             break;
           case 4:
             EMUL.datain.puts("Resources are A%3x I%d D%d H%d T%d P%3x; Adlib at %3x\n",
                            BX_SB16_IO, BX_SB16_IRQ, BX_SB16_DMAL,
                            BX_SB16_DMAH, 6, BX_SB16_IOMPU, BX_SB16_IOADLIB);
             break;
           default:
             EMUL.datain.puts("no info. Only slots 0..4 have values.\n");
             break;
         }
         break;
      case 6: // close midi and wave files and/or output
        BX_SB16_THIS closemidioutput();
        BX_SB16_THIS midimode = 0;
        BX_SB16_THIS closewaveoutput();
        BX_SB16_THIS wavemode = 0;
        break;
      case 7: // clear bank/program mappings
        EMUL.remaps = 0;
        writelog(4, "Bank/program mappings cleared.");
        break;
      case 8: // set force uart mode on/off
        EMUL.dataout.get(&value8);
        MPU.forceuartmode = value8;
        if (value8 != 0)
          MPU.uartmode = MPU.forceuartmode;
        writelog(4, "Force UART mode = %d", MPU.forceuartmode);
        break;
      case 9: // enter specific OPL2/3 mode
        // this feature has been removed
        break;
      case 10: // check emulator present
        EMUL.datain.put(0x55);
        break;
      case 11: // send data to midi device
        EMUL.dataout.get(&value8);
        mpu_mididata(value8);
    }
    EMUL.dataout.clearcommand();
    EMUL.dataout.flush();
  }
}

// and finally the OPL (FM emulation) part

// this is called whenever one of the timer elapses
void bx_sb16_c::opl_timerevent()
{
  Bit16u mask;

  for (int i=0; i<4; i++) {
    if ((OPL.tmask[i/2] & (1 << (i % 2))) != 0) { // only running timers
      if ((i % 2) == 0) {
        mask = 0xff;
      } else {
        mask = 0x3ff;
      }
      if (((++OPL.timer[i]) & mask) == 0) { // overflow occurred, set flags accordingly
        OPL.timer[i] = OPL.timerinit[i];      // reset the counter
        if ((OPL.tmask[i/2] >> (6 - (i % 2))) == 0) { // set flags only if unmasked
          writelog(MIDILOG(5), "OPL Timer Interrupt: Chip %d, Timer %d", i/2, 1 << (i % 2));
          OPL.tflag[i/2] |= 1 << (6 - (i % 2));   // set the overflow flag
          OPL.tflag[i/2] |= 1 << 7;             // set the IRQ flag
        }
      }
    }
  }
}

// return the status of one of the OPL2's, or the
// base status of the OPL3
Bit32u bx_sb16_c::opl_status(int chipid)
{
  Bit32u status = OPL.tflag[chipid];
  writelog(MIDILOG(5), "OPL status of chip %d is %02x", chipid, status);
  return status;
}

// write to the data port
void bx_sb16_c::opl_data(Bit32u value, int chipid)
{
  int index = OPL.index[chipid];

  writelog(MIDILOG(4), "Write to OPL(%d) register %02x: %02x",
           chipid, index, value);

  switch (index & 0xff) {
    // the two timer counts
    case 0x02:
      OPL.timerinit[chipid * 2] = OPL.timer[chipid * 2] = value;
      break;
    case 0x03:
      OPL.timerinit[chipid * 2 + 1] = OPL.timer[chipid * 2 + 1] = (value << 2);
      break;

    // the timer masks
    case 0x04:
      if (chipid == 0) {
        opl_settimermask(value, chipid);
      }
      break;
  }
}

// called for a write to port 4 of either chip
void bx_sb16_c::opl_settimermask(int value, int chipid)
{
  if ((value & 0x80) != 0) {  // reset IRQ and timer flags
                              // all other bits ignored!
      writelog(MIDILOG(5), "IRQ Reset called");
      OPL.tflag[chipid] = 0;
      return;
  }

  OPL.tmask[chipid] = value & 0x63;
  writelog(MIDILOG(5), "New timer mask for chip %d is %02x",
           chipid, OPL.tmask[chipid]);

  // do we have to activate or deactivate the timer?
  if (((value & 0x03) != 0) ^ (OPL.timer_running != 0)) {
    if ((value & 0x03) != 0) {  // yes, it's different. Start or stop?
       writelog(MIDILOG(5), "Starting timers");
       bx_pc_system.activate_timer(OPL.timer_handle, 80, 1);
       OPL.timer_running = 1;
    } else {
       writelog(MIDILOG(5), "Stopping timers");
       bx_pc_system.deactivate_timer(OPL.timer_handle);
       OPL.timer_running = 0;
    }
  }
}

Bit32u bx_sb16_c::fmopl_generator(Bit16u rate, Bit8u *buffer, Bit32u len)
{
  bx_bool ret = adlib_getsample(rate, (Bit16s*)buffer, len / 4, BX_SB16_THIS fm_volume);
  return ret ? len : 0;
}

Bit32u fmopl_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  return ((bx_sb16_c*)dev)->fmopl_generator(rate, buffer, len);
}

/* Handlers for the midi commands/midi file output */

// write the midi command to the midi file

void bx_sb16_c::writemidicommand(int command, int length, Bit8u data[])
{
  bx_param_string_c *midiparam;

  /* We need to determine the time elapsed since the last MIDI command */
  int deltatime = currentdeltatime();

  /* Initialize output device/file if necessary and not done yet */
  if (BX_SB16_THIS midimode > 0) {
    if ((MPU.outputinit & BX_SB16_THIS midimode) != BX_SB16_THIS midimode) {
      writelog(MIDILOG(4), "Initializing Midi output.");
      if (BX_SB16_THIS midimode & 1) {
        midiparam = SIM->get_param_string(BXPN_SOUND_MIDIOUT);
        if (BX_SB16_MIDIOUT1->openmidioutput(midiparam->getptr()) == BX_SOUNDLOW_OK)
          MPU.outputinit |= 1;
        else
          MPU.outputinit &= ~1;
      }
      if (BX_SB16_THIS midimode & 2) {
        bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
        midiparam = SIM->get_param_string("midifile", base);
        if (BX_SB16_MIDIOUT2->openmidioutput(midiparam->getptr()) == BX_SOUNDLOW_OK)
          MPU.outputinit |= 2;
        else
          MPU.outputinit &= ~2;
      }
      if ((MPU.outputinit & BX_SB16_THIS midimode) != BX_SB16_THIS midimode) {
        writelog(MIDILOG(2), "Error: Couldn't open midi output. Midi disabled.");
        BX_SB16_THIS midimode = MPU.outputinit;
        return;
      }
    }
    if (BX_SB16_THIS midimode & 1) {
      BX_SB16_MIDIOUT1->sendmidicommand(deltatime, command, length, data);
    }
    if (BX_SB16_THIS midimode & 2) {
      BX_SB16_MIDIOUT2->sendmidicommand(deltatime, command, length, data);
    }
  }
}

// determine how many delta times have passed since
// this function was called last

int bx_sb16_c::currentdeltatime()
{
  int deltatime;

  // counting starts at first access
  if (MPU.last_delta_time == 0xffffffff)
    MPU.last_delta_time = MPU.current_timer;

  deltatime = MPU.current_timer - MPU.last_delta_time;
  MPU.last_delta_time = MPU.current_timer;

  return deltatime;
}

// process the midi command stored in MPU_B.midicmd.to the midi driver

void bx_sb16_c::processmidicommand(bx_bool force)
{
  int i, channel;
  Bit8u value;
  bx_bool needremap = 0;

  channel = MPU_B.midicmd.currentcommand() & 0xf;

  // we need to log bank changes and program changes
  if ((MPU_B.midicmd.currentcommand() >> 4) == 0xc)
  {   // a program change
      value = MPU_B.midicmd.peek(0);
      writelog(MIDILOG(1), "* ProgramChange channel %d to %d",
              channel, value);
      MPU.program[channel] = value;
      needremap = 1;
  }
  else if ((MPU_B.midicmd.currentcommand() >> 4) == 0xb)
  {   // a control change, could be a bank change
      if (MPU_B.midicmd.peek(0) == 0)
      {  // bank select MSB
         value = MPU_B.midicmd.peek(1);
         writelog(MIDILOG(1), "* BankSelectMSB (%x %x %x) channel %d to %d",
                 MPU_B.midicmd.peek(0), MPU_B.midicmd.peek(1), MPU_B.midicmd.peek(2),
                 channel, value);
         MPU.bankmsb[channel] = value;
         needremap = 1;
      }
      else if (MPU_B.midicmd.peek(0) == 32)
      {  // bank select LSB
         value = MPU_B.midicmd.peek(1);
         writelog(MIDILOG(1), "* BankSelectLSB channel %d to %d",
                 channel, value);
         MPU.banklsb[channel] = value;
         needremap = 1;
      }
  }

  Bit8u temparray[256];
  i = 0;
  while (MPU_B.midicmd.empty() == 0)
    MPU_B.midicmd.get(&(temparray[i++]));

  writemidicommand(MPU_B.midicmd.currentcommand(), i, temparray);

  // if single command, revert to command mode
  if (MPU.singlecommand != 0)
  {
      MPU.singlecommand = 0;
  }

  if ((force == 0) && (needremap == 1))
    // have to check the remap lists, and remap program change if necessary
    midiremapprogram(channel);
}

// check if a program change has to be remapped, and do it if necessary

void bx_sb16_c::midiremapprogram(int channel)
{
  int bankmsb,banklsb,program;
  Bit8u commandbytes[2];

  bankmsb = MPU.bankmsb[channel];
  banklsb = MPU.banklsb[channel];
  program = MPU.program[channel];

  for(int i = 0; i < EMUL.remaps; i++)
  {
     if (((EMUL.remaplist[i].oldbankmsb == bankmsb) ||
          (EMUL.remaplist[i].oldbankmsb == 0xff)) &&
         ((EMUL.remaplist[i].oldbanklsb == banklsb) ||
          (EMUL.remaplist[i].oldbanklsb == 0xff)) &&
         ((EMUL.remaplist[i].oldprogch == program) ||
          (EMUL.remaplist[i].oldprogch == 0xff)))
     {
         writelog(5, "Remapping instrument for channel %d", channel);
         if ((EMUL.remaplist[i].newbankmsb != bankmsb) &&
             (EMUL.remaplist[i].newbankmsb != 0xff))
         {   // write control change bank msb
             MPU.bankmsb[channel] = EMUL.remaplist[i].newbankmsb;
             commandbytes[0] = 0;
             commandbytes[1] = EMUL.remaplist[i].newbankmsb;
             writemidicommand(0xb0 | channel, 2, commandbytes);
         }
         if ((EMUL.remaplist[i].newbanklsb != banklsb) &&
             (EMUL.remaplist[i].newbanklsb != 0xff))
         {   // write control change bank lsb
             MPU.banklsb[channel] = EMUL.remaplist[i].newbanklsb;
             commandbytes[0] = 32;
             commandbytes[1] = EMUL.remaplist[i].newbanklsb;
             writemidicommand(0xb0 | channel, 2, commandbytes);
         }
         if ((EMUL.remaplist[i].newprogch != program) &&
             (EMUL.remaplist[i].newprogch != 0xff))
         {   // write program change
             MPU.program[channel] = EMUL.remaplist[i].newprogch;
             commandbytes[0] = EMUL.remaplist[i].newprogch;
             writemidicommand(0xc0 | channel, 1, commandbytes);
         }
     }
  }
}

void bx_sb16_c::closemidioutput()
{
  if (BX_SB16_THIS midimode > 0) {
    if (MPU.outputinit & 1) {
      BX_SB16_MIDIOUT1->closemidioutput();
      MPU.outputinit &= ~1;
    }
    if (MPU.outputinit & 2) {
      BX_SB16_MIDIOUT2->closemidioutput();
      MPU.outputinit &= ~2;
    }
  }
}

void bx_sb16_c::closewaveoutput()
{
  if (BX_SB16_THIS wavemode > 0) {
    if (DSP.outputinit & 2) {
      BX_SB16_WAVEOUT2->closewaveoutput();
      DSP.outputinit &= ~2;
    }
  }
}


// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_sb16_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_SB16_SMF
  bx_sb16_c *class_ptr = (bx_sb16_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_sb16_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SB16_SMF

  bx_pc_system.isa_bus_delay();

  switch (address) {
    // 2x0: FM Music Status Port
    // 2x8 and 388 are aliases
    case BX_SB16_IO + 0x00:
    case BX_SB16_IO + 0x08:
    case BX_SB16_IOADLIB + 0x00:
      return opl_status(0);

    // 2x1: reserved (w: FM Music Data Port)
    // 2x9 and 389 are aliases
    case BX_SB16_IO + 0x01:
    case BX_SB16_IO + 0x09:
    case BX_SB16_IOADLIB + 0x01:
      break;

    // 2x2: Advanced Music Status Port
    //      or (for SBPro1) FM Music Status Port 2
    // 38a is an alias
    case BX_SB16_IO + 0x02:
    case BX_SB16_IOADLIB + 0x02:
      return opl_status(1);

    // 2x3: reserved (w: Adv. FM Music Data Port)
    //      or (for SBPro1) FM Music Data Port 2
    // 38b is an alias
    case BX_SB16_IO + 0x03:
    case BX_SB16_IOADLIB + 0x03:
      break;

    // 2x4: reserved (w: Mixer Register Port)
    case BX_SB16_IO + 0x04:
      break;

    // 2x5: Mixer Data Port
    case BX_SB16_IO + 0x05:
      return mixer_readdata();

    // 2x6: reserved (w: DSP Reset)
    case BX_SB16_IO + 0x06:
      break;

    // 2x7: reserved
    case BX_SB16_IO + 0x07:
      break;

    // 2x8: FM Music Status Port (OPL-2)
    // handled above

    // 2x9: reserved (w: FM Music Data Port)
    // handled above

    // 2xa: DSP Read Data Port
    case BX_SB16_IO + 0x0a:
      return dsp_dataread();

    // 2xb: reserved
    case BX_SB16_IO + 0x0b:
      break;

    // 2xc: DSP Buffer Status Port
    case BX_SB16_IO + 0x0c:
      return dsp_bufferstatus();

    // 2xd: reserved
    case BX_SB16_IO + 0x0d:
      break;

    // 2xe: DSP Data Status Port
    case BX_SB16_IO + 0x0e:
      return dsp_status();

    // 2xf: DSP Acknowledge 16bit DMA IRQ
    case BX_SB16_IO + 0x0f:
       return dsp_irq16ack();

     // 3x0: MPU Data Port Read
    case BX_SB16_IOMPU + 0x00:
      return mpu_dataread();

     // 3x1: MPU Status Port
    case BX_SB16_IOMPU + 0x01:
      return mpu_status();

    // 3x2: reserved
    case BX_SB16_IOMPU + 0x02:
      break;

    // 3x3: *Emulator* Port
    case BX_SB16_IOMPU + 0x03:
      return emul_read();

  }

  // If we get here, the port wasn't valid
  writelog(3, "Read access to 0x%04x: unsupported port!", address);

  return(0xff);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_sb16_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_SB16_SMF
  bx_sb16_c *class_ptr = (bx_sb16_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_sb16_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // !BX_USE_SB16_SMF

  bx_pc_system.isa_bus_delay();

  switch (address) {
    // 2x0: FM Music Register Port
    // 2x8 and 388 are aliases
    case BX_SB16_IO + 0x00:
    case BX_SB16_IO + 0x08:
    case BX_SB16_IOADLIB + 0x00:
      OPL.index[0] = value;
      adlib_write_index(address, value);
      return;

    // 2x1: FM Music Data Port
    // 2x9 and 389 are aliases
    case BX_SB16_IO + 0x01:
    case BX_SB16_IO + 0x09:
    case BX_SB16_IOADLIB + 0x01:
      opl_data(value, 0);
      adlib_write(opl_index, value);
      return;

    // 2x2: Advanced FM Music Register Port
    //      or (for SBPro1) FM Music Register Port 2
    // 38a is an alias
    case BX_SB16_IO + 0x02:
    case BX_SB16_IOADLIB + 0x02:
      OPL.index[1] = value;
      adlib_write_index(address, value);
      return;

    // 2x3: Advanced FM Music Data Port
    //      or (for SBPro1) FM Music Data Port 2
    // 38b is an alias
    case BX_SB16_IO + 0x03:
    case BX_SB16_IOADLIB + 0x03:
      opl_data(value, 1);
      adlib_write(opl_index, value);
      return;

    // 2x4: Mixer Register Port
    case BX_SB16_IO + 0x04:
      mixer_writeregister(value);
      return;

    // 2x5: Mixer Data Portr,
    case BX_SB16_IO + 0x05:
      mixer_writedata(value);
      return;

    // 2x6: DSP Reset
    case BX_SB16_IO + 0x06:
      dsp_reset(value);
      return;

    // 2x7: reserved
    case BX_SB16_IO + 0x07:
      break;

    // 2x8: FM Music Register Port (OPL-2)
    // handled above

    // 2x9: FM Music Data Port
    // handled above

    // 2xa: reserved (r: DSP Data Port)
    case BX_SB16_IO + 0x0a:
      break;

    // 2xb: reserved
    case BX_SB16_IO + 0x0b:
      break;

    // 2xc: DSP Write Command/Data
    case BX_SB16_IO + 0x0c:
      dsp_datawrite(value);
      return;

        // 2xd: reserved
    case BX_SB16_IO + 0x0d:
      break;

    // 2xe: reserved (r: DSP Buffer Status)
    case BX_SB16_IO + 0x0e:
      break;

    // 2xf: reserved
    case BX_SB16_IO + 0x0f:
      break;

    // 3x0: MPU Command Port
    case BX_SB16_IOMPU + 0x00:
      mpu_datawrite(value);
      return;

    // 3x1: MPU Data Port
    case BX_SB16_IOMPU + 0x01:
      mpu_command(value);
      return;

    // 3x2: reserved
    case BX_SB16_IOMPU + 0x02:
      break;

    // 3x3: *Emulator* Port
    case BX_SB16_IOMPU + 0x03:
      emul_write(value);
      return;
  }

  // if we arrive here, the port is unsupported
  writelog(3, "Write access to 0x%04x (value = 0x%02x): unsupported port!",
          address, value);
}

void bx_sb16_c::create_logfile(void)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  bx_param_string_c *logfile = SIM->get_param_string("log", base);

  if (logfile->isempty()) {
    SIM->get_param_num("loglevel", base)->set(0);
    return;
  }

  if (SIM->get_param_num("loglevel", base)->get() > 0) {
    LOGFILE = fopen(logfile->getptr(),"w"); // logfile for errors etc.
    if (LOGFILE == NULL) {
      BX_ERROR(("Error opening file %s. Logging disabled.", logfile->getptr()));
      SIM->get_param_num("loglevel", base)->set(0);
    }
  }
}

void bx_sb16_c::writelog(int loglev, const char *str, ...)
{
  if ((LOGFILE == NULL) && (BX_SB16_THIS loglevel != 0)) {
    create_logfile();
  }
  // append a line to the log file, if desired
  if (BX_SB16_THIS loglevel >= loglev) {
    fprintf(LOGFILE, FMT_TICK, bx_pc_system.time_ticks());
    fprintf(LOGFILE, " (%d) ", loglev);
    va_list ap;
    va_start(ap, str);
    vfprintf(LOGFILE, str, ap);
    va_end(ap);
    fprintf(LOGFILE, "\n");
    fflush(LOGFILE);
  }
}

// the round-robin FIFO buffers of the SB16
bx_sb16_buffer::bx_sb16_buffer()
{
  length = 0;          // total bytes in buffer
  head = 0;            // pointer to next slot available for new data
  tail = 0;            // pointer to next slot to be read from
  buffer = NULL;       // pointer to the actual data
}

void bx_sb16_buffer::init(int bufferlen)
{
  if (buffer != NULL)       // Was it initialized before?
    delete buffer;

  length = bufferlen;
  buffer = new Bit8u[length];
  if (buffer == NULL)
    length = 0;              // This will be checked later

  reset();
}

void bx_sb16_buffer::reset()
{
  head = 0;             // Reset the pointers
  tail = 0;

  clearcommand();       // no current command set
}

bx_sb16_buffer::~bx_sb16_buffer()
{
  if (buffer != NULL)
    delete [] buffer;

  buffer = NULL;
  length = 0;
}

// Report how many bytes are available
int bx_sb16_buffer::bytes(void)
{
  if (empty() != 0)
    return 0;   // empty / not initialized

  int bytes = head - tail;
  if (bytes < 0) bytes += length;
  return (bytes);
}

// This puts one byte into the buffer
bx_bool bx_sb16_buffer::put(Bit8u data)
{
  if (full() != 0)
    return 0;       // buffer full

  buffer[head++] = data;       // Write data, and increase write pointer
  head %= length;              // wrap it around so it stays inside the data

  return 1;       // put was successful
}

// This writes a formatted string to the buffer
bx_bool bx_sb16_buffer::puts(const char *data, ...)
{
  if (data == NULL)
    return 0;  // invalid string

//char string[length];
  char *string;
  int index = 0;

  string = new char[length];

  va_list ap;
  va_start(ap, data);
  vsprintf(string, data, ap);
  va_end(ap);

  if ((int) strlen(string) >= length)
    BX_PANIC(("bx_sb16_buffer: puts() too long!"));

  while (string[index] != 0)
  {
    if (put((Bit8u) string[index]) == 0)
    {
      delete [] string;
      return 0;  // buffer full
    }
    index++;
  }
  delete [] string;
  return 1;
}

// This returns if the buffer is full, i.e. if a put will fail
bx_bool bx_sb16_buffer::full(void)
{
  if (length == 0)
    return 1;   // not initialized

  if (((head + 1) % length) == tail)
    return 1;   // buffer full

  return 0;       // buffer has some space left
}

// This reads the next available byte from the buffer
bx_bool bx_sb16_buffer::get(Bit8u *data)
{
  if (empty() != 0)
  {
      // Buffer is empty. Still, if it was initialized, return
      // the last byte again.
      if (length > 0)
        (*data) = buffer[ (tail - 1) % length ];
      return 0;       // buffer empty
  }

  (*data) = buffer[tail++];       // read data and increase read pointer
  tail %= length;              // and wrap it around to stay inside the data

  return 1;       // get was successful
}

// Read a word in lo/hi order
bx_bool bx_sb16_buffer::getw(Bit16u *data)
{
  Bit8u dummy;
  if (bytes() < 2)
  {
      if (bytes() == 1)
      {
         get(&dummy);
         *data = (Bit16u) dummy;
      }
      else
         dummy = 0;
      return 0;
  }
  get(&dummy);
  *data = (Bit16u) dummy;
  get(&dummy);
  *data |= ((Bit16u) dummy) << 8;
  return 1;
}

// Read a word in hi/lo order
bx_bool bx_sb16_buffer::getw1(Bit16u *data)
{
  Bit8u dummy;
  if (bytes() < 2)
  {
      if (bytes() == 1)
      {
         get(&dummy);
         *data = ((Bit16u) dummy) << 8;
      }
      else
         dummy = 0;
      return 0;
  }
  get(&dummy);
  *data = ((Bit16u) dummy) << 8;
  get(&dummy);
  *data |= (Bit16u) dummy;
  return 1;
}

// This returns if the buffer is empty, i.e. if a get will fail
bx_bool bx_sb16_buffer::empty(void)
{
  if (length == 0)
    return 1;   // not inialized

  if (head == tail)
    return 1;   // buffer empty

  return 0;       // buffer contains data
}

// Flushes the buffer
void bx_sb16_buffer::flush(void)
{
  tail = head;
  return;
}

// Peeks ahead in the buffer
// Warning: No checking if result is valid. Must call bytes() to check that!
Bit8u bx_sb16_buffer::peek(int offset)
{
  return buffer[(tail + offset) % length];
}

// Set a new active command
void bx_sb16_buffer::newcommand(Bit8u newcmd, int bytes)
{
  command = newcmd;
  havecommand = 1;
  bytesneeded = bytes;
}

// Return the currently active command
Bit8u bx_sb16_buffer::currentcommand(void)
{
  return command;
}

// Clear the active command
void bx_sb16_buffer::clearcommand(void)
{
  command = 0;
  havecommand = 0;
  bytesneeded = 0;
}

// return if the command has received all necessary bytes
bx_bool bx_sb16_buffer::commanddone(void)
{
  if (hascommand() == 0)
    return 0;  // no command pending - not done then

  if (bytes() >= bytesneeded)
    return 1;  // yes, it's done

  return 0;    // no, it's not
}

// return if there is a command pending
bx_bool bx_sb16_buffer::hascommand(void)
{
  return havecommand;
}

int bx_sb16_buffer::commandbytes(void)
{
  return bytesneeded;
}

// runtime parameter handlers
Bit64s bx_sb16_c::sb16_param_handler(bx_param_c *param, int set, Bit64s val)
{
  if (set) {
    const char *pname = param->get_name();
    if (!strcmp(pname, "dmatimer")) {
      BX_SB16_THIS dmatimer = (Bit32u)val;
    } else if (!strcmp(pname, "loglevel")) {
      BX_SB16_THIS loglevel = (int)val;
    } else if (!strcmp(pname, "midimode")) {
      if (val != BX_SB16_THIS midimode) {
        BX_SB16_THIS midi_changed |= 1;
      }
    } else if (!strcmp(pname, "wavemode")) {
      if (val != BX_SB16_THIS wavemode) {
        BX_SB16_THIS wave_changed |= 1;
      }
    } else {
      BX_PANIC(("sb16_param_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

const char* bx_sb16_c::sb16_param_string_handler(bx_param_string_c *param, int set,
                                                 const char *oldval, const char *val,
                                                 int maxlen)
{
  if ((set) && (strcmp(val, oldval))) {
    const char *pname = param->get_name();
    if (!strcmp(pname, "wavefile")) {
      BX_SB16_THIS wave_changed |= 2;
    } else if (!strcmp(pname, "midifile")) {
      BX_SB16_THIS midi_changed |= 2;
    } else if (!strcmp(pname, "log")) {
      if (LOGFILE != NULL) {
        fclose(LOGFILE);
        LOGFILE = NULL;
      }
      // writelog() re-opens the log file on demand
    } else {
      BX_PANIC(("sb16_param_string_handler called with unexpected parameter '%s'", pname));
    }
  }
  return val;
}

#endif /* if BX_SUPPORT_SB16 */
