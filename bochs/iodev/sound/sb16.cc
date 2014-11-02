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
/////////////////////////////////////////////////////////////////////////

// The original version of the SB16 support written and donated by Josef Drexler

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SB16

#include "sb16.h"
#include "soundmod.h"

#include <math.h>

#define LOG_THIS theSB16Device->

bx_sb16_c *theSB16Device = NULL;

// builtin configuration handling functions

void sb16_init_options(void)
{
  static const char *sb16_wavemode_list[] = {
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
  bx_param_num_c *midimode = new bx_param_num_c(menu,
    "midimode",
    "Midi mode",
    "Controls the MIDI output format.",
    0, 3,
    0);
  bx_param_filename_c *midifile = new bx_param_filename_c(menu,
    "midi",
    "MIDI file",
    "The filename is where the MIDI data is sent. This can be device or just a file.",
    "", BX_PATHNAME_LEN);
  bx_param_enum_c *wavemode = new bx_param_enum_c(menu,
    "wavemode",
    "Wave mode",
    "Controls the wave output format.",
    sb16_wavemode_list,
    0, 0);
  bx_param_filename_c *wavefile = new bx_param_filename_c(menu,
    "wave",
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

  midimode->set_options(midimode->USE_SPIN_CONTROL);
  loglevel->set_options(loglevel->USE_SPIN_CONTROL);
  bx_list_c *deplist = new bx_list_c(NULL);
  deplist->add(midimode);
  deplist->add(wavemode);
  deplist->add(loglevel);
  deplist->add(dmatimer);
  enabled->set_dependent_list(deplist);
  deplist = new bx_list_c(NULL);
  deplist->add(midifile);
  midimode->set_dependent_list(deplist);
  deplist = new bx_list_c(NULL);
  deplist->add(wavefile);
  wavemode->set_dependent_list(deplist, 0);
  wavemode->set_dependent_bitmap(2, 0x1);
  wavemode->set_dependent_bitmap(3, 0x1);
  deplist = new bx_list_c(NULL);
  deplist->add(logfile);
  loglevel->set_dependent_list(deplist);
}

Bit32s sb16_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "sb16")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
    int enable = 1;
    SIM->get_param_bool("enabled", base)->set(1);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        enable = atol(&params[i][8]);
        SIM->get_param_bool("enabled", base)->set(enable);
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

int CDECL libsb16_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
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
#define MIDIDATA        BX_SB16_THIS midifile
#define WAVEDATA        BX_SB16_THIS wavefile
#define MPU             BX_SB16_THIS mpu401
#define DSP             BX_SB16_THIS dsp
#define MIXER           BX_SB16_THIS mixer
#define EMUL            BX_SB16_THIS emuldata
#define OPL             BX_SB16_THIS opl

#define BX_SB16_OUTPUT  BX_SB16_THIS soundmod

// here's a safe way to print out null pointeres
#define MIGHT_BE_NULL(x)  ((x==NULL)? "(null)" : x)

// the device object

bx_sb16_c::bx_sb16_c(void)
{
  put("SB16");
  memset(&mpu401, 0, sizeof(mpu401));
  memset(&dsp, 0, sizeof(dsp));
  memset(&opl, 0, sizeof(opl));
  currentdma8 = 0;
  currentdma16 = 0;
  mpu401.timer_handle = BX_NULL_TIMER_HANDLE;
  dsp.timer_handle = BX_NULL_TIMER_HANDLE;
  opl.timer_handle = BX_NULL_TIMER_HANDLE;
  soundmod = NULL;
  midimode = 0;
  midifile = NULL;
  wavemode = 0;
  wavefile = NULL;
  loglevel = 0;
  logfile = NULL;
}

bx_sb16_c::~bx_sb16_c(void)
{
  closemidioutput();

  if (DSP.inputinit != 0) {
    BX_SB16_OUTPUT->closewaveinput();
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
  BX_SB16_THIS midimode = SIM->get_param_num("midimode", base)->get();
  BX_SB16_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
  BX_SB16_THIS dmatimer = SIM->get_param_num("dmatimer", base)->get();
  BX_SB16_THIS loglevel = SIM->get_param_num("loglevel", base)->get();

  // always initialize lowlevel driver
  BX_SB16_OUTPUT = DEV_sound_get_module();

  if (BX_SB16_OUTPUT == NULL) {
    BX_PANIC(("Couldn't initialize lowlevel driver"));
  }

  DSP.dma.chunk = new Bit8u[BX_SOUNDLOW_WAVEPACKETSIZE];
  DSP.dma.chunkindex = 0;

  DSP.outputinit = 1;
  DSP.inputinit = 0;
  MPU.outputinit = 0;

  if (DSP.dma.chunk == NULL)
  {
    writelog(WAVELOG(2), "Couldn't allocate wave buffer - wave output disabled.");
    BX_SB16_THIS wavemode = 0;
  }

  BX_INFO(("midi=%d,%s  wave=%d,%s  log=%d,%s  dmatimer=%d",
    BX_SB16_THIS midimode, MIGHT_BE_NULL(SIM->get_param_string("midi", base)->getptr()),
    BX_SB16_THIS wavemode, MIGHT_BE_NULL(SIM->get_param_string("wave", base)->getptr()),
    BX_SB16_THIS loglevel, MIGHT_BE_NULL(SIM->get_param_string("log", base)->getptr()),
    BX_SB16_THIS dmatimer));

  // allocate the FIFO buffers - except for the MPUMIDICMD buffer
  // these sizes are generous, 16 or 8 would probably be sufficient
  MPU.datain.init((int) 64);               // the input
  MPU.dataout.init((int) 64);               // and output
  MPU.cmd.init((int) 64);              // and command buffers
  MPU.midicmd.init((int) 256);         // and the midi command buffer (note- large SYSEX'es have to fit!)
  DSP.datain.init((int) 64);           // the DSP input
  DSP.dataout.init((int) 64);          // and output buffers
  EMUL.datain.init((int) 64);          // the emulator ports
  EMUL.dataout.init((int) 64);         // for changing emulator settings

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
  OPL.mode = fminit;
  OPL.timer_running = 0;
  opl_entermode(single);

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
    MPU.timer_handle = bx_pc_system.register_timer
      (BX_SB16_THISP, mpu_timer, 500000 / 384, 1, 1, "sb16.mpu");
    // midi timer: active, continuous, 500000 / 384 seconds (384 = delta time, 500000 = sec per beat at 120 bpm. Don't change this!)
  }

  if (DSP.timer_handle == BX_NULL_TIMER_HANDLE) {
    DSP.timer_handle = bx_pc_system.register_timer
      (BX_SB16_THISP, dsp_dmatimer, 1, 1, 0, "sb16.dsp");
    // dma timer: inactive, continuous, frequency variable
  }

  if (OPL.timer_handle == BX_NULL_TIMER_HANDLE) {
    OPL.timer_handle = bx_pc_system.register_timer
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
  menu->add(SIM->get_param("midi", base));
  menu->add(SIM->get_param("wavemode", base));
  menu->add(SIM->get_param("wave", base));
  menu->add(SIM->get_param("loglevel", base));
  menu->add(SIM->get_param("log", base));
  menu->add(SIM->get_param("dmatimer", base));
  SIM->get_param_enum("wavemode", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("wave", base)->set_handler(sb16_param_string_handler);
  SIM->get_param_num("midimode", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("midi", base)->set_handler(sb16_param_string_handler);
  SIM->get_param_num("dmatimer", base)->set_handler(sb16_param_handler);
  SIM->get_param_num("loglevel", base)->set_handler(sb16_param_handler);
  SIM->get_param_string("log", base)->set_handler(sb16_param_string_handler);
  // register handler for correct sb16 parameter handling after runtime config
  SIM->register_runtime_config_handler(this, runtime_config_handler);
  BX_SB16_THIS midi_changed = 0;
  BX_SB16_THIS wave_changed = 0;
}

void bx_sb16_c::reset(unsigned type)
{
}

void bx_sb16_c::register_state(void)
{
  unsigned i, j;
  char name[8];
  bx_list_c *chip, *ins_map, *item, *patch;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "sb16", "SB16 State");
  bx_list_c *mpu = new bx_list_c(list, "mpu");
  new bx_shadow_bool_c(mpu, "uartmode", &MPU.uartmode);
  new bx_shadow_bool_c(mpu, "irqpending", &MPU.irqpending);
  new bx_shadow_bool_c(mpu, "forceuartmode", &MPU.forceuartmode);
  new bx_shadow_bool_c(mpu, "singlecommand", &MPU.singlecommand);
  new bx_shadow_bool_c(mpu, "outputinit", &MPU.outputinit);
  new bx_shadow_num_c(mpu, "current_timer", &MPU.current_timer);
  new bx_shadow_num_c(mpu, "last_delta_time", &MPU.last_delta_time);
  bx_list_c *patchtbl = new bx_list_c(mpu, "patchtable");
  for (i=0; i<BX_SB16_PATCHTABLESIZE; i++) {
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
  new bx_shadow_num_c(dsp, "testreg", &DSP.testreg, BASE_HEX);
  bx_list_c *dma = new bx_list_c(dsp, "dma");
  new bx_shadow_num_c(dma, "mode", &DSP.dma.mode);
  new bx_shadow_num_c(dma, "bits", &DSP.dma.bits);
  new bx_shadow_num_c(dma, "bps", &DSP.dma.bps);
  new bx_shadow_num_c(dma, "format", &DSP.dma.format);
  new bx_shadow_num_c(dma, "timer", &DSP.dma.timer);
  new bx_shadow_bool_c(dma, "fifo", &DSP.dma.fifo);
  new bx_shadow_bool_c(dma, "output", &DSP.dma.output);
  new bx_shadow_bool_c(dma, "stereo", &DSP.dma.stereo);
  new bx_shadow_bool_c(dma, "issigned", &DSP.dma.issigned);
  new bx_shadow_bool_c(dma, "highspeed", &DSP.dma.highspeed);
  new bx_shadow_num_c(dma, "count", &DSP.dma.count);
  new bx_shadow_num_c(dma, "chunkindex", &DSP.dma.chunkindex);
  new bx_shadow_num_c(dma, "chunkcount", &DSP.dma.chunkcount);
  new bx_shadow_num_c(dma, "timeconstant", &DSP.dma.timeconstant);
  new bx_shadow_num_c(dma, "blocklength", &DSP.dma.blocklength);
  new bx_shadow_num_c(dma, "samplerate", &DSP.dma.samplerate);
  new bx_shadow_bool_c(dsp, "outputinit", &DSP.outputinit);
  new bx_shadow_bool_c(dsp, "inputinit", &DSP.inputinit);
  new bx_shadow_data_c(list, "chunk", DSP.dma.chunk, BX_SOUNDLOW_WAVEPACKETSIZE);
  bx_list_c *csp = new bx_list_c(list, "csp_reg");
  for (i=0; i<256; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(csp, name, &BX_SB16_THIS csp_reg[i], BASE_HEX);
  }
  bx_list_c *opl = new bx_list_c(list, "opl");
  new bx_shadow_num_c(opl, "mode", (Bit8u*)&OPL.mode);
  new bx_shadow_num_c(opl, "timer_running", &OPL.timer_running);
  new bx_shadow_num_c(opl, "midichannels", &OPL.midichannels);
  new bx_shadow_num_c(opl, "drumchannel", &OPL.drumchannel);
  for (i=0; i<2; i++) {
    sprintf(name, "chip%d", i+1);
    chip = new bx_list_c(opl, name);
    new bx_shadow_num_c(chip, "index", &OPL.index[i]);
    new bx_shadow_num_c(chip, "wsenable", &OPL.wsenable[i]);
    new bx_shadow_num_c(chip, "timer1", &OPL.timer[i*2]);
    new bx_shadow_num_c(chip, "timer2", &OPL.timer[i*2+1]);
    new bx_shadow_num_c(chip, "timerinit1", &OPL.timerinit[i*2]);
    new bx_shadow_num_c(chip, "timerinit2", &OPL.timerinit[i*2+1]);
    new bx_shadow_num_c(chip, "tmask", &OPL.tmask[i]);
    new bx_shadow_num_c(chip, "tflag", &OPL.tflag[i]);
    new bx_shadow_num_c(chip, "percmode", &OPL.percmode[i]);
    new bx_shadow_num_c(chip, "cyhhnote", &OPL.cyhhnote[i]);
    new bx_shadow_num_c(chip, "cyhhon", &OPL.cyhhon[i]);
  }
  bx_list_c *oper = new bx_list_c(opl, "oper");
  for (i=0; i<BX_SB16_FM_NOP; i++) {
    sprintf(name, "%d", i);
    item = new bx_list_c(oper, name);
    for (j=0; j<BX_SB16_FM_OPB; j++) {
      sprintf(name, "%d", j);
      new bx_shadow_num_c(item, name, &OPL.oper[i][j]);
    }
  }
  bx_list_c *chan = new bx_list_c(opl, "chan");
  for (i=0; i<BX_SB16_FM_NCH; i++) {
    sprintf(name, "%d", i);
    item = new bx_list_c(chan, name);
    new bx_shadow_num_c(item, "nop", &OPL.chan[i].nop);
    new bx_shadow_num_c(item, "ncarr", &OPL.chan[i].ncarr);
    new bx_shadow_num_c(item, "opnum1", &OPL.chan[i].opnum[0]);
    new bx_shadow_num_c(item, "opnum2", &OPL.chan[i].opnum[1]);
    new bx_shadow_num_c(item, "opnum3", &OPL.chan[i].opnum[2]);
    new bx_shadow_num_c(item, "opnum4", &OPL.chan[i].opnum[3]);
    new bx_shadow_num_c(item, "freq", &OPL.chan[i].freq);
    new bx_shadow_num_c(item, "afreq", &OPL.chan[i].afreq);
    new bx_shadow_num_c(item, "midichan", &OPL.chan[i].midichan);
    new bx_shadow_bool_c(item, "needprogch", &OPL.chan[i].needprogch);
    new bx_shadow_num_c(item, "midinote", &OPL.chan[i].midinote);
    new bx_shadow_bool_c(item, "midion", &OPL.chan[i].midion);
    new bx_shadow_num_c(item, "midibend", &OPL.chan[i].midibend);
    new bx_shadow_num_c(item, "outputlevel1", &OPL.chan[i].outputlevel[0]);
    new bx_shadow_num_c(item, "outputlevel2", &OPL.chan[i].outputlevel[1]);
    new bx_shadow_num_c(item, "outputlevel3", &OPL.chan[i].outputlevel[2]);
    new bx_shadow_num_c(item, "outputlevel4", &OPL.chan[i].outputlevel[3]);
    new bx_shadow_num_c(item, "midivol", &OPL.chan[i].midivol);
  }
  new bx_shadow_num_c(list, "mixer_regindex", &MIXER.regindex, BASE_HEX);
  bx_list_c *mixer = new bx_list_c(list, "mixer_reg");
  for (i=0; i<BX_SB16_MIX_REG; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(mixer, name, &MIXER.reg[i], BASE_HEX);
  }
  bx_list_c *emul = new bx_list_c(list, "emul");
  new bx_shadow_num_c(emul, "remaps", &EMUL.remaps);
  bx_list_c *remap = new bx_list_c(emul, "remaplist");
  for (i=0; i<EMUL.remaps; i++) {
    sprintf(name, "0x%02x", i);
    ins_map = new bx_list_c(remap, name);
    new bx_shadow_num_c(ins_map, "oldbankmsb", &EMUL.remaplist[i].oldbankmsb);
    new bx_shadow_num_c(ins_map, "oldbanklsb", &EMUL.remaplist[i].oldbanklsb);
    new bx_shadow_num_c(ins_map, "oldprogch", &EMUL.remaplist[i].oldprogch);
    new bx_shadow_num_c(ins_map, "newbankmsb", &EMUL.remaplist[i].newbankmsb);
    new bx_shadow_num_c(ins_map, "newbanklsb", &EMUL.remaplist[i].newbanklsb);
    new bx_shadow_num_c(ins_map, "newprogch", &EMUL.remaplist[i].newprogch);
  }
}

void bx_sb16_c::after_restore_state(void)
{
  set_irq_dma();
}

void bx_sb16_c::runtime_config_handler(void *this_ptr)
{
  bx_sb16_c *class_ptr = (bx_sb16_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_sb16_c::runtime_config(void)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  if (BX_SB16_THIS midi_changed) {
    BX_SB16_THIS closemidioutput();
    BX_SB16_THIS midimode = SIM->get_param_num("midimode", base)->get();
    // writemidicommand() re-opens the output device / file on demand
    BX_SB16_THIS midi_changed = 0;
  }
  if (BX_SB16_THIS wave_changed) {
    BX_SB16_THIS closewaveoutput();
    BX_SB16_THIS wavemode = SIM->get_param_enum("wavemode", base)->get();
    // dsp_dma() re-opens the output file on demand
    BX_SB16_THIS wave_changed = 0;
  }
}

// the timer functions
void bx_sb16_c::mpu_timer (void *this_ptr)
{
  ((bx_sb16_c *) this_ptr)->mpu401.current_timer++;
}

void bx_sb16_c::dsp_dmatimer(void *this_ptr)
{
  bx_sb16_c *This = (bx_sb16_c *) this_ptr;

  // raise the DRQ line. It is then lowered by the dma read / write functions
  // when the next byte has been sent / received.
  // However, don't do this if the next byte/word will fill up the
  // output buffer and the output functions are not ready yet
  // or if buffer is empty in input mode.

  if ((BX_SB16_THIS wavemode != 1) ||
       ((This->dsp.dma.chunkindex + 1 < BX_SOUNDLOW_WAVEPACKETSIZE) &&
        (This->dsp.dma.count > 0)) ||
       (BX_SB16_OUTPUT->waveready() == BX_SOUNDLOW_OK)) {
    if (((This->dsp.dma.output == 0) && (This->dsp.dma.chunkcount > 0)) ||
        (This->dsp.dma.output == 1)) {
      if ((DSP.dma.bits == 8) || (BX_SB16_DMAH == 0)) {
        DEV_dma_set_drq(BX_SB16_DMAL, 1);
      } else {
        DEV_dma_set_drq(BX_SB16_DMAH, 1);
      }
    }
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
      DSP.dma.stereo = 0;
      DSP.dma.issigned = 0;
      DSP.dma.count = 0;
      DSP.dma.highspeed = 0;
      DSP.dma.chunkindex = 0;

      DSP.dataout.reset();    // clear the buffers
      DSP.datain.reset();

      DSP.dataout.put(0xaa);  // acknowledge the reset
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
    //  if (DSP.dataout.empty() == 0)
    DSP.dataout.get(&value);
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

  if (DSP.datain.hascommand() == 1) // already a command pending, add to argument list
  {
    if (DSP.datain.put(value) == 0)
    {
       writelog(WAVELOG(3), "DSP command buffer overflow for command %02x",
                 DSP.datain.currentcommand());
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
    DSP.datain.newcommand(value, bytesneeded);
  }

  if (DSP.datain.commanddone() == 1) // command is complete, process it
  {
      writelog(WAVELOG(4), "DSP command %x with %d arg bytes",
              DSP.datain.currentcommand(), DSP.datain.bytes());

      switch (DSP.datain.currentcommand())
      {
         // DSP commands - comments are the parameters for
         // this command, and/or the output

         // ASP commands (Advanced Signal Processor)
         // undocumented (?), just from looking what an SB16 does
       case 0x04:
         DSP.datain.get(&value8);
         break;

       case 0x05:
         DSP.datain.get(&value8);
         DSP.datain.get(&value8);
         break;

       case 0x0e:
         DSP.datain.get(&index);
         DSP.datain.get(&value8);
         BX_SB16_THIS csp_reg[index] = value;
         break;

       case 0x0f:
         DSP.datain.get(&index);
         DSP.dataout.put(BX_SB16_THIS csp_reg[index]);
         break;

         // direct mode DAC
       case 0x10:
         // 1: 8bit sample
         DSP.datain.get(&value8);   // sample is ignored
         break;

         // uncomp'd, normal DAC DMA
       case 0x14:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 0);
         break;

          // 2-bit comp'd, normal DAC DMA, no ref byte
       case 0x16:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 2);
         break;

         // 2-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x17:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
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
         DSP.dataout.put(0x80); // put a silence, for now.
         break;

         // uncomp'd, normal ADC DMA
       case 0x24:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
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
         DSP.datain.get(&value8);
         // route to mpu401 part
         mpu_datawrite(value8);
         break;

         // set time constant
       case 0x40:
         // 1: timeconstant
         DSP.datain.get(&value8);
         DSP.dma.timeconstant = value8 <<  8;
         DSP.dma.samplerate = (Bit32u) 256000000L / ((Bit32u) 65536L - (Bit32u) DSP.dma.timeconstant);
           break;

         // set samplerate for input
       case 0x41:
         // (fallback intended)

         // set samplerate for output
       case 0x42:
         // 1,2: hi(frq) lo(frq)
         DSP.datain.getw1(&(DSP.dma.samplerate));
         DSP.dma.timeconstant = 65536 - (Bit32u) 256000000 / (Bit32u) DSP.dma.samplerate;
         break;

         // set block length
       case 0x48:
         // 1,2: lo(blk len) hi(blk len)
         DSP.datain.getw(&(DSP.dma.blocklength));
         break;

         // 4-bit comp'd, normal DAC DMA, no ref byte
       case 0x74:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 4);
         break;

         // 4-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x75:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 4|8);
         break;

         // 3-bit comp'd, normal DAC DMA, no ref byte
       case 0x76:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
         dsp_dma(0xc0, 0x00, length, 3);
         break;

         // 3-bit comp'd, normal DAC DMA, 1 ref byte
       case 0x77:
         // 1,2: lo(length) hi(length)
         DSP.datain.getw(&length);
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
         DSP.datain.getw(&length);
         // only handled for VOC output so far
         if (BX_SB16_THIS wavemode == 2)
         {
            Bit8u temparray[3] = { (Bit8u)(length & 0xff), (Bit8u)(length >> 8), (Bit8u)(DSP.dma.timeconstant >> 8) };
            DEV_soundmod_VOC_write_block(WAVEDATA, 3, 3, temparray, 0, NULL);
         }
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
         DSP.datain.get(&mode);
         DSP.datain.getw(&length);
         dsp_dma(DSP.datain.currentcommand(), mode, length, 0);
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
         DSP.dataout.put((DSP.speaker == 1)?0xff:0x00);
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
         DSP.datain.get(&value8);
         DSP.dataout.put(~value8);
         break;

         // get version, out 2 bytes (major, minor)
       case 0xe1:
         // none, o1/2: version major.minor
         DSP.dataout.put(4);
         if (DSP.dataout.put(5) == 0)
         {
             writelog(WAVELOG(3), "DSP version couldn't be written - buffer overflow");
         }
         break;

       case 0xe3:
         // none, output: Copyright string
         // the Windows driver needs the exact text, otherwise it
         // won't load. Same for diagnose.exe
         DSP.dataout.puts("COPYRIGHT (C) CREATIVE TECHNOLOGY LTD, 1992.");
         DSP.dataout.put(0);    // need extra string end
         break;

         // write test register
       case 0xe4:
         DSP.datain.get(&DSP.testreg);
         break;

         // read test register
       case 0xe8:
         DSP.dataout.put(DSP.testreg);
         break;

         // Trigger 8-bit IRQ
       case 0xf2:
         DSP.dataout.put(0xaa);
         DSP.irqpending = 1;
         MIXER.reg[0x82] |= 1; // reg 82 shows the kind of IRQ
         DEV_pic_raise_irq(BX_SB16_IRQ);
         break;

         // ??? - Win98 needs this
        case 0xf9:
          DSP.datain.get(&value8);
          switch (value8) {
            case 0x0e:
              DSP.dataout.put(0xff);
              break;
            case 0x0f:
              DSP.dataout.put(0x07);
              break;
            case 0x37:
              DSP.dataout.put(0x38);
              break;
            default:
              DSP.dataout.put(0x00);
          }
          break;

         // unknown command
       default:
         writelog(WAVELOG(3), "unknown DSP command %x, ignored",
                 DSP.datain.currentcommand());
         break;
      }
      DSP.datain.clearcommand();
      DSP.datain.flush();
   }
}

// dsp_dma() initiates all kinds of dma transfers
void bx_sb16_c::dsp_dma(Bit8u command, Bit8u mode, Bit16u length, Bit8u comp)
{
  int ret;
  bx_list_c *base;

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

  if ((command >> 4) == 0xb)  // 0xb? = 16 bit DMA
  {
    DSP.dma.bits = 16;
    DSP.dma.bps = 2;
  }
  else                        // 0xc? = 8 bit DMA
  {
    DSP.dma.bits = 8;
    DSP.dma.bps = 1;
  }

  // Prevent division by zero in some instances
  if (DSP.dma.samplerate == 0)
    DSP.dma.samplerate= 10752;
  command &= 0x0f;
  DSP.dma.output = 1 - (command >> 3);       // 1=output, 0=input
  DSP.dma.mode = 1 + ((command >> 2) & 1);  // 0=none, 1=normal, 2=auto
  DSP.dma.fifo = (command >> 1) & 1;         // ? not sure what this is

  DSP.dma.stereo = (mode >> 5) & 1;

  if (DSP.dma.stereo != 0)
    DSP.dma.bps *= 2;

  DSP.dma.blocklength = length;
  DSP.dma.issigned = (mode >> 4) & 1;
  DSP.dma.highspeed = (comp >> 4) & 1;

  DSP.dma.chunkindex = 0;
  DSP.dma.chunkcount = 0;

  Bit32u sampledatarate = (Bit32u) DSP.dma.samplerate * (Bit32u) DSP.dma.bps;
  if (DSP.dma.bits == 8 || (DSP.dma.bits == 16 && BX_SB16_DMAH != 0)) {
    DSP.dma.count = DSP.dma.blocklength;
  } else {
    DSP.dma.count = ((DSP.dma.blocklength + 1) << 1) - 1;
  }
  DSP.dma.timer = BX_SB16_THIS dmatimer * BX_DMA_BUFFER_SIZE / sampledatarate;

  writelog(WAVELOG(5), "DMA is %db, %dHz, %s, %s, mode %d, %s, %s, %d bps, %d usec/DMA",
           DSP.dma.bits, DSP.dma.samplerate, (DSP.dma.stereo != 0)?"stereo":"mono",
           (DSP.dma.output == 1)?"output":"input", DSP.dma.mode,
           (DSP.dma.issigned == 1)?"signed":"unsigned",
           (DSP.dma.highspeed == 1)?"highspeed":"normal speed",
           sampledatarate, DSP.dma.timer);

  DSP.dma.format = DSP.dma.issigned | ((comp & 7) << 1) | ((comp & 8) << 4);

  // write the output to the device/file
  base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  if (DSP.dma.output == 1) {
    if (BX_SB16_THIS wavemode == 1) {
      if (DSP.outputinit == 1) {
        ret = BX_SB16_OUTPUT->startwaveplayback(DSP.dma.samplerate, DSP.dma.bits, DSP.dma.stereo, DSP.dma.format);
        if (ret != BX_SOUNDLOW_OK) {
          BX_SB16_THIS wavemode = 0;
          writelog(WAVELOG(2), "Error: Could not start wave playback.");
        }
      }
    } else if ((BX_SB16_THIS wavemode == 2) ||
               (BX_SB16_THIS wavemode == 3)) {
      bx_param_string_c *waveparam = SIM->get_param_string("wave", base);
      if ((WAVEDATA == NULL) && (!waveparam->isempty())) {
        WAVEDATA = fopen(waveparam->getptr(), "wb");
        if (WAVEDATA == NULL) {
          writelog(WAVELOG(2), "Error opening file %s. Wavemode disabled.",
                   waveparam->getptr());
          BX_SB16_THIS wavemode = 0;
        } else if (BX_SB16_THIS wavemode == 2) {
          DEV_soundmod_VOC_init_file(WAVEDATA);
        }
      }
    }
    DSP.dma.chunkcount = sampledatarate / 10; // 0.1 sec
    if (DSP.dma.chunkcount > BX_SOUNDLOW_WAVEPACKETSIZE) {
      DSP.dma.chunkcount = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
  } else {
    if (DSP.inputinit == 0) {
      ret = BX_SB16_OUTPUT->openwaveinput(SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr(), sb16_adc_handler);
      if (ret != BX_SOUNDLOW_OK) {
        BX_SB16_THIS wavemode = 0;
        writelog(WAVELOG(2), "Error: Could not open wave input device.");
      } else {
        DSP.inputinit = 1;
      }
    }
    if (DSP.inputinit == 1) {
      ret = BX_SB16_OUTPUT->startwaverecord(DSP.dma.samplerate, DSP.dma.bits, DSP.dma.stereo, DSP.dma.format);
      if (ret != BX_SOUNDLOW_OK) {
        BX_SB16_THIS wavemode = 0;
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
    memcpy(DSP.dma.chunk, DSP.dma.chunk+DSP.dma.chunkindex, len);
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
  BX_SB16_OUTPUT->getwavepacket(DSP.dma.chunkcount, DSP.dma.chunk);
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

// dsp_bufferstatus() checks if the DSP is ready for data/commands
Bit32u bx_sb16_c::dsp_bufferstatus()
{
  Bit32u result = 0x7f;

  // MSB set -> not ready for commands
  if (DSP.datain.full() == 1) result |= 0x80;

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
  if (DSP.dataout.empty() == 0) result |= 0x80;

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
  if (DSP.dma.chunkindex == 0)
    return;

  // apply wave volume
  if (BX_SB16_THIS wave_vol != 0xffff) {
    DEV_soundmod_pcm_apply_volume(DSP.dma.chunkindex, DSP.dma.chunk, BX_SB16_THIS wave_vol,
                                  DSP.dma.bits, DSP.dma.stereo, DSP.dma.format & 1);
  }

  switch (BX_SB16_THIS wavemode) {
    case 1:
      BX_SB16_OUTPUT->sendwavepacket(DSP.dma.chunkindex, DSP.dma.chunk);
      break;
    case 3:
      fwrite(DSP.dma.chunk, 1, DSP.dma.chunkindex, WAVEDATA);
      break;
    case 2:
      Bit8u temparray[12] =
       { (Bit8u)(DSP.dma.samplerate & 0xff), (Bit8u)(DSP.dma.samplerate >> 8), 0, 0,
         (Bit8u)DSP.dma.bits, (Bit8u)(DSP.dma.stereo + 1), 0, 0, 0, 0, 0, 0 };
      switch ((DSP.dma.format >> 1) & 7) {
       case 2:
         temparray[6] = 3;
         break;
       case 3:
         temparray[6] = 2;
         break;
       case 4:
         temparray[6] = 1;
         break;
      }
      if (DSP.dma.bits == 16)
         temparray[6] = 4;

      DEV_soundmod_VOC_write_block(WAVEDATA, 9, 12, temparray, DSP.dma.chunkindex, DSP.dma.chunk);
      break;
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

    if (BX_SB16_THIS wavemode == 1) {
      BX_SB16_OUTPUT->stopwaveplayback();
    } else if (BX_SB16_THIS wavemode != 0) {
      fflush(WAVEDATA);
    }
  } else if ((DSP.dma.output == 0) && (DSP.dma.mode != 2)) {
    if (BX_SB16_THIS wavemode == 1) {
      BX_SB16_OUTPUT->stopwaverecord();
    }
  }

  // generate the appropriate IRQ
  if (DSP.dma.bits == 8)
    MIXER.reg[0x82] |= 1;
  else
    MIXER.reg[0x82] |= 2;

  DEV_pic_raise_irq(BX_SB16_IRQ);
  DSP.irqpending = 1;

  // if auto-DMA, reinitialize
  if (DSP.dma.mode == 2)
  {
      if (DSP.dma.bits == 8 || (DSP.dma.bits == 16 && BX_SB16_DMAH != 0)) {
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

// the mixer, supported type is CT1745 (as in an SB16)
void bx_sb16_c::mixer_writedata(Bit32u value)
{
  int i;
  bx_bool set_wave_vol = 0;
  Bit8u master_vol, dac_vol;
  float tmp_vol;

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
      set_wave_vol = 1;
      return;

    case 0x04: // DAC level
      MIXER.reg[0x32] = (value & 0xf0) | 0x08;
      MIXER.reg[0x33] = ((value & 0x0f) << 4) | 0x08;
      set_wave_vol = 1;
      break;

    case 0x0a: // microphone level
      MIXER.reg[0x3a] = (value << 5) | 0x18;
      break;

    case 0x22: // master volume
      MIXER.reg[0x30] = (value & 0xf0) | 0x08;
      MIXER.reg[0x31] = ((value & 0x0f) << 4) | 0x08;
      set_wave_vol = 1;
      break;

    case 0x26: // FM level
      MIXER.reg[0x34] = (value & 0xf0) | 0x08;
      MIXER.reg[0x35] = ((value & 0x0f) << 4) | 0x08;
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
      set_wave_vol = 1;
      break;

    case 0x31: // master volume right
      MIXER.reg[0x22] &= 0xf0;
      MIXER.reg[0x22] |= (value >> 4);
      set_wave_vol = 1;
      break;

    case 0x32: // DAC level left
      MIXER.reg[0x04] &= 0x0f;
      MIXER.reg[0x04] |= (value & 0xf0);
      set_wave_vol = 1;
      break;

    case 0x33: // DAC level right
      MIXER.reg[0x04] &= 0xf0;
      MIXER.reg[0x04] |= (value >> 4);
      set_wave_vol = 1;
      break;

    case 0x34: // FM level left
      MIXER.reg[0x26] &= 0x0f;
      MIXER.reg[0x26] |= (value & 0xf0);
      break;

    case 0x35: // FM level right
      MIXER.reg[0x26] &= 0xf0;
      MIXER.reg[0x26] |= (value >> 4);
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
  MIXER.reg[MIXER.regindex] = value;

  if (set_wave_vol) {
    master_vol = (MIXER.reg[0x30] >> 3);
    dac_vol = (MIXER.reg[0x32] >> 3);
    tmp_vol = (float)master_vol/31.0f*pow(10.0f, (float)(31-dac_vol)*-0.065f);
    BX_SB16_THIS wave_vol = (Bit8u)(255 * tmp_vol);
    master_vol = (MIXER.reg[0x31] >> 3);
    dac_vol = (MIXER.reg[0x33] >> 3);
    tmp_vol = (float)master_vol/31.0f*pow(10.0f, (float)(31-dac_vol)*-0.065f);
    BX_SB16_THIS wave_vol |= ((Bit8u)(255 * tmp_vol) << 8);
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

  if ((MPU.datain.full() == 1) ||
       ((BX_SB16_THIS midimode == 1) &&
        (BX_SB16_OUTPUT->midiready() == BX_SOUNDLOW_ERR)))
    result |= 0x40;       // output not ready
  if (MPU.dataout.empty() == 1)
    result |= 0x80;       // no input available

  writelog(MIDILOG(4), "MPU status port, result %02x", result);

  return(result);
}

// the MPU 401 command port

void bx_sb16_c::mpu_command(Bit32u value)
{
  int i;
  int bytesneeded;

  if (MPU.cmd.hascommand() == 1) // already a command pending, abort that one
  {
      if ((MPU.cmd.currentcommand() != value) ||
          (MPU.cmd.commanddone() == 0))
            // it's a different command, or the old one isn't done yet, abort it
       {
         MPU.cmd.clearcommand();
         MPU.cmd.flush();
       }

       // if it's the same one, and we just completed the argument list,
       // we leave it as it is and process it here
  }

  if (MPU.cmd.hascommand() == 0)  // no command pending, set one up
  {
      bytesneeded = 0;
      if ((value >> 4) == 14) bytesneeded = 1;
      MPU.cmd.newcommand(value, bytesneeded);
  }

  if (MPU.cmd.commanddone() == 1) // command is complete, process it
  {
     switch (MPU.cmd.currentcommand())
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
          MPU.cmd.reset();
          MPU.dataout.reset();
          MPU.datain.reset();
          MPU.midicmd.reset();

          /*
          if (BX_SB16_IRQ != -1) {
             MIXER.reg[0x82] |= 4;
             BX_SB16_THIS devices->pic->trigger_irq(BX_SB16_IRQ);
          }
          */
          break;
       case 0xd0:  // d0 and df: prefix for midi command
       case 0xdf:  // like uart mode, but only a single command
          MPU.singlecommand = 1;
          writelog(MIDILOG(4), "MPU: prefix %02x received",
                  MPU.cmd.currentcommand());
          break;
       default:
          writelog(MIDILOG(3), "MPU cmd: unknown command %02x ignored",
                  MPU.cmd.currentcommand());
          break;
     }

     // Need to put an MPU_ACK into the data port if command successful
     // we'll fake it even if we didn't process the command, so as to
     // allow detection of the MPU 401.
     if (MPU.dataout.put(0xfe) == 0)
        writelog(MIDILOG(3), "MPU_ACK error - output buffer full");
     MPU.cmd.clearcommand(); // clear the command from the buffer
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

  if (MPU.dataout.get(&res8bit) == 0) {
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

  if (MPU.cmd.hascommand() == 1)
  { // there is a command pending, add arguments to it
      if (MPU.cmd.put(value) == 0)
       writelog(MIDILOG(3), "MPU Command arguments too long - buffer full");
      if (MPU.cmd.commanddone() == 1)
       BX_SB16_THIS mpu_command(MPU.cmd.currentcommand());
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
      if ((value == 0xf7) && (MPU.midicmd.currentcommand() == 0xf0))
      // ...except if it is a continuing SYSEX message, then it just
      // denotes the end of a SYSEX chunk, not the start of a message
      {
         ismidicommand = 0;     // first, it's not a command
         MPU.midicmd.newcommand(MPU.midicmd.currentcommand(),
                             MPU.midicmd.bytes());
         // Then, set needed bytes to current buffer
         // because we didn't know the length before
      }
  }

  if (ismidicommand == 1)
  {  // this is a command, check if an old one is pending
      if (MPU.midicmd.hascommand() == 1)
      {
         writelog(MIDILOG(3), "Midi command %02x incomplete, has %d of %d bytes.",
                 MPU.midicmd.currentcommand(), MPU.midicmd.bytes(),
                 MPU.midicmd.commandbytes());
         // write as much as we can. Should we do this?
         processmidicommand(0);
         // clear the pending command
         MPU.midicmd.clearcommand();
         MPU.midicmd.flush();
      }

      // find the number of arguments to the command
      static const signed eventlength[] = { 2, 2, 2, 2, 1, 1, 2, 255};
      // note - length 255 commands have unknown length
      MPU.midicmd.newcommand(value, eventlength[(value & 0x70) >> 4]);
  }
  else  // no command, just arguments to the old command
  {
      if (MPU.midicmd.hascommand() == 0)
      {  // no command pending, ignore the data
         writelog(MIDILOG(3), "Midi data %02x received, but no command pending?", value);
         return;
      }

      // just some data to the command
      if (MPU.midicmd.put(value) == 0)
       writelog(MIDILOG(3), "Midi buffer overflow!");
      if (MPU.midicmd.commanddone() == 1)
      {
         // the command is complete, process it
         writelog(MIDILOG(5), "Midi command %02x complete, has %d bytes.",
                 MPU.midicmd.currentcommand(), MPU.midicmd.bytes());
         processmidicommand(0);
         // and remove the command from the buffer
         MPU.midicmd.clearcommand();
         MPU.midicmd.flush();
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
         if (EMUL.remaps >= BX_SB16_PATCHTABLESIZE) break;
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
         if (EMUL.remaps >= BX_SB16_PATCHTABLESIZE) break;
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
         if (EMUL.remaps >= BX_SB16_PATCHTABLESIZE) break;
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
           case 5:
             EMUL.datain.puts("Current OPL2/3 mode: %s",
                            // ok, I admit that this is a bit ugly...
                            (OPL.mode == single)?"single OPL2 (OPL3 disabled)\n":
                            (OPL.mode == adlib)?"single OPL2 (no OPL3)\n":
                            (OPL.mode == dual)?"double OPL2\n":
                            (OPL.mode == opl3)?"OPL3\n":
                            "unknown");
             break;
           default:
             EMUL.datain.puts("no info. Only slots 0..5 have values.\n");
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
        EMUL.dataout.get(&value8);
        writelog(4, "Entering OPL2/3 mode %d", value8);
        opl_entermode((bx_sb16_fm_mode) value8);
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

// select a new operational mode for the FM part
// this also serves as reset for the OPL chip
void bx_sb16_c::opl_entermode(bx_sb16_fm_mode newmode)
{
  int i, j;

  // do nothing if the mode is unchanged
  if (OPL.mode == newmode)
    return;

  // if the old mode was 0, and the new mode is 3, then
  // no reset is necessary, just set the flag
  if ((OPL.mode == single) && (newmode == opl3))
  {
      writelog(MIDILOG(4), "OPL3 mode enabled");
      OPL.mode = newmode;
      return;
  }

  writelog(MIDILOG(4), "Switching to OPL mode %d from %d", newmode, OPL.mode);

  for (i=0; i<BX_SB16_FM_NCH; i++)
    opl_keyonoff(i, 0);

  OPL.mode = newmode;

  if (OPL.timer_running != 0)
  {
      bx_pc_system.deactivate_timer(OPL.timer_handle);
      OPL.timer_running = 0;
  }

  OPL.drumchannel = 10;

  OPL.midichannels = 0xffff;    // all channels but the drum channel available
  OPL.midichannels &= ~(1 << OPL.drumchannel);

  for (i=0; i<2; i++) {
      OPL.wsenable[i] = 0;
      OPL.tmask[i] = 0;
      OPL.tflag[i] = 0;
      OPL.percmode[i] = 0;
  }

  for (i=0; i<4; i++) {
      OPL.timer[i] = 0;
      OPL.timerinit[i] = 0;
  }

  // initialize the operators
  for (i=0; i<BX_SB16_FM_NOP; i++)
    for (j=0; j<BX_SB16_FM_OPB; j++)
      OPL.oper[i][j] = 0;

  // TESTING for array bounds - compiler should bark if too high
  OPL.oper[BX_SB16_FM_NOP-1][BX_SB16_FM_OPB-1] = 0;

  // initialize the channels

  // first zero all values
  for (i=0; i<BX_SB16_FM_NCH; i++)
  {
      OPL.chan[i].nop = 0;
      for (j=0; j<4; j++) {
         OPL.chan[i].opnum[j] = 0;
         OPL.chan[i].outputlevel[j] = 0;
      }
      OPL.chan[i].freq = 0;
      OPL.chan[i].afreq = 0;
      OPL.chan[i].midichan = 0xff;
      OPL.chan[i].needprogch = 0;
      OPL.chan[i].midion = 0;
      OPL.chan[i].midinote = 0;
      OPL.chan[i].midibend = 0;
      OPL.chan[i].midivol = 0;
  }

  // assign the operators
  for (i=0; i<BX_SB16_FM_NCH; i++)
  {
      OPL.chan[i].nop = 2;
      // who invented this absolutely insane operator grouping??
      // it's like this: (ch 9...17 as 0...8 but higher operators)
      // ch:   0   1   2   3   4   5   6   7   8
      // op1:  0   1   2   6   7   8  12  13  14
      // op2:  3   4   5   9  10  11  15  16  17
      OPL.chan[i].opnum[0] = i + ((int) (i / 3)) * 3;
      OPL.chan[i].opnum[1] = OPL.chan[i].opnum[0] + 3;
  }

  // assign 4-op operators to the appropriate channels
  // note- they are not used unless .nop == 4
  for (i=0; i<6; i++) {
      j = i + (i /3) * 6;
      OPL.chan[j].opnum[2] = OPL.chan[j + 3].opnum[0];
      OPL.chan[j].opnum[3] = OPL.chan[j + 3].opnum[1];
  }
}

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
      if (((OPL.timer[i]++) & mask) == 0) { // overflow occured, set flags accordingly
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

// set the register index for one of the OPL2's or the
// base or advanced register index for the OPL3
void bx_sb16_c::opl_index(Bit32u value, int chipid)
{
  OPL.index[chipid] = value;
}

// write to the data port
void bx_sb16_c::opl_data(Bit32u value, int chipid)
{
  int index = OPL.index[chipid];
  int opernum = -1;      // OPL3 operator number; 0..35
  int channum = -1;      // OPL3 channel number; 0..17
  int subopnum = -1;     // channel operator; 0..nop-1

  writelog(MIDILOG(4), "Write to OPL(%d) register %02x: %02x",
           chipid, index, value);

  // first find out operator and/or channel numbers
  // case 0x20 ... 0x95:  includes too many ports, but that is harmless
  // case 0xe0 ... 0xf5:
  if (((index>=0x20) && (index<=0x95)) ||
      ((index>=0xe0) && (index<=0xf5))) {
      // operator access
      // find the operator number. 0..17 on chip 1, 18..35 on chip 2

      // note, the numbers are not continuous (again...), so we need
      // this rather weird calculation
      opernum = index & 0x07;
      if (opernum > 5)   // invalid register, has no operator associated
      {
         opernum = -1;
         goto break_here;
      }

      opernum += ((index & 0x18) >> 3) * 6;
      if (opernum > 17)     // Operators 18+ have to be accessed on other address set
      {
         opernum = -1;
         goto break_here;
      }

      if (chipid == 1)
         opernum += BX_SB16_FM_NOP / 2;

      // find out the channel number, and which of the channel's operators this is
      channum = opernum % 3 + ((int) (opernum / 6)) * 3;
      subopnum = 0;

      if ((opernum % 6) > 2) // second operator
         subopnum = 1;

      // if (channel - 3) is in a four-operator mode, that is really
      // what this operator belongs to
      if (channum >= 3) {
         if (OPL.chan[channum - 3].nop == 4)
         {
           channum -= 3;
           subopnum += 2;
         }
      }
      writelog(MIDILOG(5), "Is Channel %d, Oper %d, Subop %d",
              channum, opernum, subopnum);
  }
  else if ((index>=0xa0) && (index<=0xc8)) {
      // channel access
      channum = index & 0x0f;
      if (OPL.chan[channum].nop == 0)
         channum = -1; // the channel is disabled
      writelog(MIDILOG(5), "Is channel %d", channum);
  }

break_here:

  switch (index & 0xff)
  {
    // WSEnable and Test Register
    case 0x01:
      OPL.wsenable[chipid] = (value >> 5) & 1;
      if ((value & 0x1f) != 0)
        writelog(MIDILOG(3), "Warning: Test Register set to %02x", value & 0x1f);
      break;

    // the two timer counts
    case 0x02:
      OPL.timerinit[chipid * 2] = OPL.timer[chipid * 2] = value;
      break;
    case 0x03:
      OPL.timerinit[chipid * 2 + 1] = OPL.timer[chipid * 2 + 1] = (value << 2);
      break;

    // if OPL2: timer masks
    // if OPL3: 4-operator modes
    case 0x04:
      if ((chipid == 0) || (OPL.mode == dual))
        opl_settimermask(value, chipid);
      else
        opl_set4opmode(value & 0x3f);
      break;

    // only OPL3: OPL3 enable
    case 0x05:
      if (chipid == 1)
      {
         if ((value & 1) != 0)
           opl_entermode(opl3);
         else
           opl_entermode(single);
      }
      break;

    // Composite Sine Wave and Note-sel (ignored)
    case 0x08:
      if (value != 0)
        writelog(MIDILOG(3),
                "Warning: write of %02x to CSW/Note-sel ignored", value);
      break;

    // most importantly the percussion part
    case 0xbd:
      opl_setpercussion(value, chipid);
      break;

    // the operator registers
    // case 0x20 ... 0x35:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2a:
    case 0x2b:
    case 0x2c:
    case 0x2d:
    case 0x2e:
    case 0x2f:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    // case 0x60 ... 0x75:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6a:
    case 0x6b:
    case 0x6c:
    case 0x6d:
    case 0x6e:
    case 0x6f:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    // case 0x80 ... 0x95:
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8a:
    case 0x8b:
    case 0x8c:
    case 0x8d:
    case 0x8e:
    case 0x8f:
    case 0x90:
    case 0x91:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0x95:
      if (opernum != -1)
      {
         opl_changeop(channum, opernum, (index / 0x20) - 1, value);
         break;
      }
      // else let default: catch it

    // case 0x40 ... 0x55:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
      if (opernum != -1)
      {
         opl_changeop(channum, opernum, 1, value & 0xc0);
         if (subopnum != -1)
           opl_setvolume(channum, subopnum, value & 0x3f);
         break;
      }
      // else let default: catch it

    // case 0xe0 ... 0xf5:
    case 0xe0:
    case 0xe1:
    case 0xe2:
    case 0xe3:
    case 0xe4:
    case 0xe5:
    case 0xe6:
    case 0xe7:
    case 0xe8:
    case 0xe9:
    case 0xea:
    case 0xeb:
    case 0xec:
    case 0xed:
    case 0xee:
    case 0xef:
    case 0xf0:
    case 0xf1:
    case 0xf2:
    case 0xf3:
    case 0xf4:
    case 0xf5:
      if (opernum != -1)
      {
         opl_changeop(channum, opernum, 5, value & 0x07);
         break;
      }
      // else let default: catch it

    // and the channel registers
    // case 0xa0 ... 0xa8:
    case 0xa0:
    case 0xa1:
    case 0xa2:
    case 0xa3:
    case 0xa4:
    case 0xa5:
    case 0xa6:
    case 0xa7:
    case 0xa8:
      if (channum != -1)
      {
         if (value != (Bit32u)(OPL.chan[channum].freq & 0xff)) {
           OPL.chan[channum].freq &= 0xff00;
           OPL.chan[channum].freq |= value;
           opl_setfreq(channum);
         }
         break;
      }
      // else let default: catch it

    // case 0xb0 ... 0xb8:
    case 0xb0:
    case 0xb1:
    case 0xb2:
    case 0xb3:
    case 0xb4:
    case 0xb5:
    case 0xb6:
    case 0xb7:
    case 0xb8:
      if (channum != -1)
      {
         if ((value & 0x1f) != ((Bit32u)(OPL.chan[channum].freq >> 8) & 0x1f)) {
           OPL.chan[channum].freq &= 0x00ff;
           OPL.chan[channum].freq |= (value & 0x1f) << 8;
           opl_setfreq(channum);
         }
         opl_keyonoff(channum, (value >> 5) & 1);
         break;
      }
      // else let default: catch it


      // this is a channel access, but it belongs to the instrument
      // definition, so put it into value [4] of the channel's first operator
    // case 0xc0 ... 0xc8:
    case 0xc0:
    case 0xc1:
    case 0xc2:
    case 0xc3:
    case 0xc4:
    case 0xc5:
    case 0xc6:
    case 0xc7:
    case 0xc8:
      if (channum != -1)
      {
         int needchange = 0;
         if ((OPL.oper[OPL.chan[channum].opnum[0]][4] & 1) != (int)(value & 1))
           needchange = 1;

         opl_changeop(channum, OPL.chan[channum].opnum[0], 4, value & 0x3f);

         if (needchange == 1)
           opl_setmodulation(channum);
         break;
      }
      // else let default: catch it

    default:
      writelog(MIDILOG(3), "Attempt to write %02x to unknown OPL(%d) register %02x",
              value, chipid, index);
      break;
  }
}

// change a value of an operator
void bx_sb16_c::opl_changeop(int channum, int opernum, int byte, int value)
{
  if (OPL.oper[opernum][byte] != value) {
      OPL.oper[opernum][byte] = value;
      OPL.chan[channum].needprogch = 1;
  }
}

// called for a write to the 4-operator mode register
void bx_sb16_c::opl_set4opmode(int new4opmode)
{
  int i, channel1, channel2;

  writelog(MIDILOG(4), "Switching to 4-op mode %02x", new4opmode);

  // every bit switches a 4-op channel-pairing on or off
  // 4-op mode is two channels combined into the first one
  for (i = 0; i<6; i++)
  {
      channel1 = i + (i / 3) * 6;
      channel2 = channel1 + 3;

      if (((new4opmode >> i) & 1) != 0)
      {   // enable 4-op mode
         opl_keyonoff(channel1, 0);
         opl_keyonoff(channel2, 0);

         OPL.chan[channel1].nop = 4;
         OPL.chan[channel2].nop = 0;

         OPL.chan[channel1].needprogch = 1;
      }
      else
      {   // disable 4-op mode
         opl_keyonoff(channel1, 0);

         OPL.chan[channel1].nop = 2;
         OPL.chan[channel2].nop = 2;

         OPL.chan[channel1].needprogch = 1;
         OPL.chan[channel2].needprogch = 1;
      }
  }
}

// called for a write to port 4 of either chip
void bx_sb16_c::opl_settimermask(int value, int chipid)
{
  if ((value & 0x80) != 0)     // reset IRQ and timer flags
  {                           // all other bits ignored!
      writelog(MIDILOG(5), "IRQ Reset called");
      OPL.tflag[chipid] = 0;
      return;
  }

  OPL.tmask[chipid] = value & 0x63;
  writelog(MIDILOG(5), "New timer mask for chip %d is %02x",
           chipid, OPL.tmask[chipid]);

  // do we have to activate or deactivate the timer?
  if (((value & 0x03) != 0) ^ (OPL.timer_running != 0))
  {
    if ((value & 0x03) != 0)    // yes, it's different. Start or stop?
    {
       writelog(MIDILOG(5), "Starting timers");
       bx_pc_system.activate_timer(OPL.timer_handle, 80, 1);
       OPL.timer_running = 1;
    }
    else
    {
       writelog(MIDILOG(5), "Stopping timers");
       bx_pc_system.deactivate_timer(OPL.timer_handle);
       OPL.timer_running = 0;
    }
  }
}

// called when the modulation mode of a channel changes
void bx_sb16_c::opl_setmodulation(int channel)
{
  int opernum = OPL.chan[channel].opnum[0];

  if ((OPL.chan[channel].nop == 0) &&
      (channel >= 3) &&
      (OPL.chan[channel].nop == 4)) channel -= 3;

  if (OPL.chan[channel].nop == 2)
  {
      OPL.chan[channel].ncarr = (OPL.oper[opernum][4] & 1) + 1;
      OPL.chan[channel].needprogch = 1;
  }
  else if (OPL.chan[channel].nop == 4)
  {
      int opernum2 = OPL.chan[channel].opnum[2];
      int modmode = (OPL.oper[opernum][4] & 1) |
                   ((OPL.oper[opernum2][4] & 1) >> 1);
      OPL.chan[channel].ncarr = modmode + 1 - (modmode / 2);
      OPL.chan[channel].needprogch = 1;
  }
}

// called for a write to register 0xbd, the percussion register
void bx_sb16_c::opl_setpercussion(Bit8u value, int chipid)
{
  UNUSED(value);
  UNUSED(chipid);
}

// called when a channel volume changes
// opnum is which of the channel's operators had the change, not
// the actual operator number. Thus, it's from 0..3.
void bx_sb16_c::opl_setvolume(int channel, int opnum, int outlevel)
{
  UNUSED(opnum);
  UNUSED(outlevel);

  OPL.chan[channel].midivol = 127;
}


// called when a frequency change is complete, to find out the
// corresponding midi key and pitch bender values
void bx_sb16_c::opl_setfreq(int channel)
{
  int block,fnum;

  // definition:
  // low-byte of freq:  8 bit F-Number, LSB's
  // high-byte of freq: [2 reserved][KEY-ON][3 block][2 F-Number MSB's]
  // [KEY-ON] is ignored by this function
  //
  // the definition of the F-number is
  // F-Number = Frequency * 2**(20-block) / (49716 Hz)
  //
  // Thus, the frequency can be calculated as
  // Frequency = F-Number / 2**(20-block) * 49716 Hz
  //
  // (But remember that afreq is in 10^-3 Hz!)
  //

  fnum = OPL.chan[channel].freq & 0x3ff;
  block = (OPL.chan[channel].freq >> 10) & 0x07;

  writelog(MIDILOG(5), "F-Num is %d, block is %d", fnum, block);

  Bit32u realfreq;
  const Bit32u freqbase = 49716000; // const is better than #define if type is important

  // this is a bit messy to preserve accuracy as much as possible,
  // otherwise we might either lose precision, or the higher bits.
  realfreq = ((freqbase >> 4) * fnum) >> (16 - block);

  OPL.chan[channel].afreq = realfreq;

  // now find out what MIDI key this corresponds to, and with what
  // pitch bender value... (the latter not implemented yet)
  int octave=0;          // 0: Octave from 523.2511 Hz; pos=higher, neg=lower
  int keynum=0;          // 0=C; 1=C#; 2=D; ...; 11=B

  if (realfreq > 8175) {    // 8.175 is smallest possible frequency
    const Bit32u freqC = 523251;    // Midi note 72; "C": 523.251 Hz
    Bit32u keyfreq;           // Frequency scaled to the octave from freqC to 2*freqC

    if (realfreq > freqC) {
      while ((realfreq >> (++octave)) > freqC);
      keyfreq = realfreq >> (--octave);
    } else {
      while ((realfreq << (++octave)) < freqC);
      keyfreq = realfreq << octave;
      octave = -octave;
    }

    // this is a reasonable approximation for keyfreq /= 1.059463
    // (that value is 2**(1/12), which is the difference between two keys)
    while ((keyfreq -= ((keyfreq * 1000) / 17817)) > freqC)
      keynum++;
  } else {
    octave = -6;
    keynum = 0;
  }

  OPL.chan[channel].midinote = (octave + 6) * 12 + keynum;

  writelog(MIDILOG(5), "New frequency %.3f is key %d in octave %d; midi note %d",
           (float) realfreq/1000.0, keynum, octave, OPL.chan[channel].midinote);
}

// called when a note is possibly turned on or off
void bx_sb16_c::opl_keyonoff(int channel, bx_bool onoff)
{
  int i;
  Bit8u commandbytes[3];

  if (OPL.mode == fminit)
    return;

  // first check if there really is a change in the state
  if (onoff == OPL.chan[channel].midion)
    return;

  OPL.chan[channel].midion = onoff;

  // check if we have a midi channel, otherwise allocate one if possible
  if (OPL.chan[channel].midichan == 0xff) {
    for (i=0; i<16; i++)
      if (((OPL.midichannels >> i) & 1) != 0) {
        OPL.chan[channel].midichan = i;
        OPL.midichannels &= ~(1 << i); // mark channel as used
        OPL.chan[channel].needprogch = 1;
      }
    if (OPL.chan[channel].midichan == 0xff)
      return;
  }

  if (OPL.chan[channel].needprogch != 0)
    opl_midichannelinit(channel);

  commandbytes[0] = OPL.chan[channel].midichan;
  commandbytes[1] = OPL.chan[channel].midinote;
  commandbytes[2] = 0;

  if (onoff == 0) {
    commandbytes[0] |= 0x80;  // turn it off
  } else {
    commandbytes[0] |= 0x90;  // turn it on
    commandbytes[2] = OPL.chan[channel].midivol;
  }

  writemidicommand(commandbytes[0], 2, & (commandbytes[1]));
}

// setup a midi channel
void bx_sb16_c::opl_midichannelinit(int channel)
{
  UNUSED(channel);
}

/* Handlers for the midi commands/midi file output */

// Write the header of the midi file. Track length is 0x7fffffff
// until we know how long it's really going to be

void bx_sb16_c::initmidifile()
{
  struct {
    Bit8u chunk[4];
    Bit32u chunklen;  // all values in BIG Endian!
    Bit16u smftype;
    Bit16u tracknum;
    Bit16u timecode;  // 0x80 + deltatimesperquarter << 8
  } midiheader =
#ifdef BX_LITTLE_ENDIAN
      { "MTh", 0x06000000, 0, 0x0100, 0x8001 };
#else
      { "MTh", 6, 0, 1, 0x180 };
#endif
  midiheader.chunk[3] = 'd';

  struct {
    Bit8u chunk[4];
    Bit32u chunklen;
    Bit8u data[15];
  } trackheader =
#ifdef BX_LITTLE_ENDIAN
      { "MTr", 0xffffff7f,
#else
      { "MTr", 0x7fffffff,
#endif
       { 0x00,0xff,0x51,3,0x07,0xa1,0x20,    // set tempo 120 (0x7a120 us per quarter)
         0x00,0xff,0x58,4,4,2,0x18,0x08 }};  // time sig 4/4
  trackheader.chunk[3] = 'k';

  fwrite(&midiheader, 1, 14, MIDIDATA);
  fwrite(&trackheader, 1, 23, MIDIDATA);
}

// write the midi command to the midi file

void bx_sb16_c::writemidicommand(int command, int length, Bit8u data[])
{
  bx_list_c *base;
  /* We need to determine the time elapsed since the last MIDI command */
  int deltatime = currentdeltatime();

  /* Initialize output device if necessary and not done yet */
  base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SB16);
  bx_param_string_c *midiparam = SIM->get_param_string("midi", base);
  if (BX_SB16_THIS midimode == 1) {
    if (MPU.outputinit != 1) {
      writelog(MIDILOG(4), "Initializing Midi output.");
      if (BX_SB16_OUTPUT->openmidioutput(midiparam->getptr()) == BX_SOUNDLOW_OK)
        MPU.outputinit = 1;
      else
        MPU.outputinit = 0;
      if (MPU.outputinit != 1) {
        writelog(MIDILOG(2), "Error: Couldn't open midi output. Midi disabled.");
        BX_SB16_THIS midimode = 0;
        return;
      }
    }
    BX_SB16_OUTPUT->sendmidicommand(deltatime, command, length, data);
    return;
  } else if ((BX_SB16_THIS midimode == 2) ||
             (BX_SB16_THIS midimode == 3)) {
    if ((MIDIDATA == NULL) && (!midiparam->isempty())) {
      MIDIDATA = fopen(midiparam->getptr(),"wb");
      if (MIDIDATA == NULL) {
        writelog (MIDILOG(2), "Error opening file %s. Midimode disabled.",
          midiparam->getptr());
        BX_SB16_THIS midimode = 0;
      } else if (BX_SB16_THIS midimode == 2) {
        initmidifile();
      }
    }
  }

  if (BX_SB16_THIS midimode < 2)
    return;

  if (BX_SB16_THIS midimode == 2)
    writedeltatime(deltatime);

  fputc(command, MIDIDATA);
  if ((command == 0xf0) ||
      (command == 0xf7))    // write event length for sysex/meta events
    writedeltatime(length);

  fwrite(data, 1, length, MIDIDATA);
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

// process the midi command stored in MPU.midicmd.to the midi driver

void bx_sb16_c::processmidicommand(bx_bool force)
{
  int i, channel;
  Bit8u value;
  bx_bool needremap = 0;

  channel = MPU.midicmd.currentcommand() & 0xf;

  // we need to log bank changes and program changes
  if ((MPU.midicmd.currentcommand() >> 4) == 0xc)
  {   // a program change
      value = MPU.midicmd.peek(0);
      writelog(MIDILOG(1), "* ProgramChange channel %d to %d",
              channel, value);
      MPU.program[channel] = value;
      needremap = 1;
  }
  else if ((MPU.midicmd.currentcommand() >> 4) == 0xb)
  {   // a control change, could be a bank change
      if (MPU.midicmd.peek(0) == 0)
      {  // bank select MSB
         value = MPU.midicmd.peek(1);
         writelog(MIDILOG(1), "* BankSelectMSB (%x %x %x) channel %d to %d",
                 MPU.midicmd.peek(0), MPU.midicmd.peek(1), MPU.midicmd.peek(2),
                 channel, value);
         MPU.bankmsb[channel] = value;
         needremap = 1;
      }
      else if (MPU.midicmd.peek(0) == 32)
      {  // bank select LSB
         value = MPU.midicmd.peek(1);
         writelog(MIDILOG(1), "* BankSelectLSB channel %d to %d",
                 channel, value);
         MPU.banklsb[channel] = value;
         needremap = 1;
      }
  }

  Bit8u temparray[256];
  i = 0;
  while (MPU.midicmd.empty() == 0)
    MPU.midicmd.get(&(temparray[i++]));

  writemidicommand(MPU.midicmd.currentcommand(), i, temparray);

  // if single command, revert to command mode
  if (MPU.singlecommand != 0)
  {
      MPU.singlecommand = 0;
      // and trigger IRQ?
      //      MPU.irqpending = 1;
      //      BX_SB16_THIS devices->pic->trigger_irq(BX_SB16_IRQMPU);
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

// convert a number into a delta time coded value
int bx_sb16_c::converttodeltatime(Bit32u deltatime, Bit8u value[4])
{
  int i, count;
  Bit8u outbytes[4];

  count = 0;

  if (deltatime <= 0)
  {
      count = 1;
      value[0] = 0;
  }
  else
  {
      while ((deltatime > 0) && (count < 4))   // split into parts
      {                                        // of seven bits
         outbytes[count++] = deltatime & 0x7f;
         deltatime >>= 7;
      }
      for (i=0; i<count; i++)                      // reverse order and
         value[i] = outbytes[count - i - 1] | 0x80; // set eighth bit on
      value[count - 1] &= 0x7f;                    // all but last byte
  }
  return count;
}

// write a delta time coded value to the midi file
void bx_sb16_c::writedeltatime(Bit32u deltatime)
{
  Bit8u outbytes[4];

  int count = converttodeltatime(deltatime, outbytes);

  for (int i=0; i<count; i++)
    fputc(outbytes[i], MIDIDATA);
}


// close the midi file, and set the track length accordingly

void bx_sb16_c::finishmidifile()
{
  struct {
    Bit8u delta, statusbyte, metaevent, length;
  } metatrackend = { 0, 0xff, 0x2f, 0 };

   // Meta event track end (0xff 0x2f 0x00) plus leading delta time
  fwrite(&metatrackend, 1, sizeof(metatrackend), MIDIDATA);

  Bit32u tracklen = ftell(MIDIDATA);
  if (tracklen < 0)
    BX_PANIC (("ftell failed in finishmidifile"));
  if (tracklen < 22)
    BX_PANIC (("finishmidifile with track length too short"));
  tracklen -= 22;    // subtract the midi file and track header
  fseek(MIDIDATA, 22 - 4, SEEK_SET);
  // value has to be in big endian
#ifdef BX_LITTLE_ENDIAN
  tracklen = bx_bswap32(tracklen);
#endif
  fwrite(&tracklen, 4, 1, MIDIDATA);
}

void bx_sb16_c::closemidioutput()
{
  switch (BX_SB16_THIS midimode) {
    case 1:
      if (MPU.outputinit != 0) {
        BX_SB16_OUTPUT->closemidioutput();
        MPU.outputinit = 0;
      }
      break;
    case 2:
      if (MIDIDATA != NULL)
        finishmidifile();
    case 3:
      if (MIDIDATA != NULL)
        fclose(MIDIDATA);
      MIDIDATA = NULL;
      break;
  }
}

void bx_sb16_c::closewaveoutput()
{
  switch (BX_SB16_THIS wavemode) {
    case 1:
      // nothing to do here yet
      break;
    case 2:
      if (WAVEDATA != NULL)
        finishvocfile();
    case 3:
      if (WAVEDATA != NULL)
        fclose(WAVEDATA);
      WAVEDATA = NULL;
      break;
  }
}


/* Handler for the voc file output */

// close the voc file
void bx_sb16_c::finishvocfile()
{
  fputc(0, WAVEDATA);  // blocktype 0: end block
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

  switch (address)
  {
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

  switch (address)
  {
    // 2x0: FM Music Register Port
    // 2x8 and 388 are aliases
    case BX_SB16_IO + 0x00:
    case BX_SB16_IO + 0x08:
    case BX_SB16_IOADLIB + 0x00:
      opl_index(value, 0);
      return;

    // 2x1: FM Music Data Port
    // 2x9 and 389 are aliases
    case BX_SB16_IO + 0x01:
    case BX_SB16_IO + 0x09:
    case BX_SB16_IOADLIB + 0x01:
      opl_data(value, 0);
      return;

    // 2x2: Advanced FM Music Register Port
    //      or (for SBPro1) FM Music Register Port 2
    // 38a is an alias
    case BX_SB16_IO + 0x02:
    case BX_SB16_IOADLIB + 0x02:
      opl_index(value, 1);
      return;

    // 2x3: Advanced FM Music Data Port
    //      or (for SBPro1) FM Music Data Port 2
    // 38b is an alias
    case BX_SB16_IO + 0x03:
    case BX_SB16_IOADLIB + 0x03:
      opl_data(value, 1);
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

  string = (char *) malloc(length);

  va_list ap;
  va_start(ap, data);
  vsprintf(string, data, ap);
  va_end(ap);

  if ((int) strlen(string) >= length)
    BX_PANIC(("bx_sb16_buffer: puts() too long!"));

  while (string[index] != 0)
  {
    if (put((Bit8u) string[index]) == 0)
       return 0;  // buffer full
    index++;
  }
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
        BX_SB16_THIS midi_changed = 1;
      }
    } else if (!strcmp(pname, "wavemode")) {
      if (val != BX_SB16_THIS wavemode) {
        BX_SB16_THIS wave_changed = 1;
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
    if (!strcmp(pname, "wave")) {
      BX_SB16_THIS wave_changed = 1;
    } else if (!strcmp(pname, "midi")) {
      BX_SB16_THIS midi_changed = 1;
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
