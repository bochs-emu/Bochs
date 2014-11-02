/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003       David N. Welton <davidw@dedasys.com>.
//  Copyright (C) 2003-2014  The Bochs Project
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

#define BX_PLUGGABLE

#include "iodev.h"
#include "speaker.h"

#if BX_SUPPORT_SOUNDLOW
#include "sound/soundmod.h"
#endif

#ifdef __linux__
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>
#endif

#define LOG_THIS theSpeaker->

bx_speaker_c *theSpeaker= NULL;

#define BX_SPK_MODE_NONE   0
#define BX_SPK_MODE_SOUND  1
#define BX_SPK_MODE_SYSTEM 2
#define BX_SPK_MODE_GUI    3

// builtin configuration handling functions

void speaker_init_options(void)
{
  static const char *speaker_mode_list[] = {
#if BX_SUPPORT_SOUNDLOW
    "sound",
#endif
#if defined(__linux__) || defined(WIN32)
    "system",
#endif
    "gui",
    NULL
  };

  bx_list_c *sound = (bx_list_c*)SIM->get_param("sound");
  bx_list_c *menu = new bx_list_c(sound, "speaker", "PC speaker output configuration");
  menu->set_options(menu->SERIES_ASK);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu, "enabled", "Enable speaker output",
      "Enables the PC speaker output", 1);
  bx_param_enum_c *mode = new bx_param_enum_c(menu, "mode", "Speaker output mode",
      "The mode can be one these: 'sound', 'system' or 'gui'",
      speaker_mode_list, 0, 0);
  mode->set_ask_format("Select speker output mode [%s] ");
  bx_list_c *deplist = new bx_list_c(NULL);
  deplist->add(mode);
  enabled->set_dependent_list(deplist);
}

Bit32s speaker_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "speaker")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SPEAKER);
    for (int i=1; i<num_params; i++) {
      if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for speaker ignored.", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s speaker_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_SOUND_SPEAKER), NULL, 0);
}

// device plugin entry points

int CDECL libspeaker_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theSpeaker = new bx_speaker_c();
  bx_devices.pluginSpeaker = theSpeaker;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSpeaker, BX_PLUGIN_SPEAKER);
  // add new configuration parameters for the config interface
  speaker_init_options();
  // register add-on options for bochsrc and command line
  SIM->register_addon_option("speaker", speaker_options_parser, speaker_options_save);
  return(0); // Success
}

void CDECL libspeaker_LTX_plugin_fini(void)
{
  bx_devices.pluginSpeaker = &bx_devices.stubSpeaker;
  delete theSpeaker;
  SIM->unregister_addon_option("speaker");
  ((bx_list_c*)SIM->get_param("sound"))->remove("speaker");
}

// the device object

bx_speaker_c::bx_speaker_c()
{
  put("speaker", "PCSPK");

  beep_frequency = 0.0; // Off
#ifdef __linux__
  consolefd = -1;
#endif
}

bx_speaker_c::~bx_speaker_c()
{
  beep_off();
#ifdef __linux__
  if (consolefd >= 0) {
    close(consolefd);
  }
#endif
  BX_DEBUG(("Exit"));
}

void bx_speaker_c::init(void)
{
  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_SOUND_SPEAKER);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("PC speaker output disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("sb16"))->set(0);
    return;
  }
  const char *mode = SIM->get_param_enum("mode", base)->get_selected();
  if (!strcmp(mode, "sound")) {
    output_mode = BX_SPK_MODE_SOUND;
    outputinit = 0;
#if BX_SUPPORT_SOUNDLOW
    if (DEV_soundmod_beep_off()) {
      BX_INFO(("Using lowlevel sound support for output"));
      outputinit = 1;
    } else {
      BX_ERROR(("Failed to use lowlevel sound support for output"));
    }
#endif
  } else if (!strcmp(mode, "system")) {
    output_mode = BX_SPK_MODE_SYSTEM;
#ifdef __linux__
    consolefd = open("/dev/console", O_WRONLY);
    if (consolefd != -1) {
      BX_INFO(("Using /dev/console for output"));
    } else {
      BX_ERROR(("Failed to open /dev/console: %s", strerror(errno)));
      BX_ERROR(("Deactivating beep on console"));
    }
#elif defined(WIN32)
    BX_INFO(("Using system beep for output"));
#endif
  } else if (!strcmp(mode, "gui")) {
    output_mode = BX_SPK_MODE_GUI;
    BX_INFO(("Forwarding beep to gui"));
  } else {
    output_mode = BX_SPK_MODE_NONE;
  }
}

void bx_speaker_c::reset(unsigned type)
{
  beep_off();
}

void bx_speaker_c::beep_on(float frequency)
{
#if defined(WIN32)
  if ((output_mode == BX_SPK_MODE_SYSTEM) && (beep_frequency != 0.0)) {
    beep_off();
  }
#endif
  beep_frequency = frequency;

  if (output_mode == BX_SPK_MODE_SOUND) {
#if BX_SUPPORT_SOUNDLOW
    if (outputinit) {
      DEV_soundmod_beep_on(frequency);
    }
#endif
  } else if (output_mode == BX_SPK_MODE_SYSTEM) {
#ifdef __linux__
    if (consolefd != -1) {
      BX_DEBUG(("PC speaker on with frequency %f", frequency));
      ioctl(consolefd, KIOCSOUND, (int)(clock_tick_rate/frequency));
    }
#elif defined(WIN32)
    usec_start = bx_pc_system.time_usec();
#endif
  } else if (output_mode == BX_SPK_MODE_GUI) {
    // give the gui a chance to signal beep on
    bx_gui->beep_on(frequency);
  }
}

#if defined(WIN32)

static struct {
  DWORD frequency;
  DWORD msec;
} beep_info;

DWORD WINAPI BeepThread(LPVOID)
{
  static BOOL threadActive = FALSE;

  while (threadActive) Sleep(10);
  threadActive = TRUE;
  Beep(beep_info.frequency, beep_info.msec);
  threadActive = FALSE;
  return 0;
}

#endif

void bx_speaker_c::beep_off()
{
  if (output_mode == BX_SPK_MODE_SOUND) {
#if BX_SUPPORT_SOUNDLOW
    if (outputinit) {
      DEV_soundmod_beep_off();
    }
#endif
  } else if (output_mode == BX_SPK_MODE_SYSTEM) {
    if (beep_frequency != 0.0) {
#ifdef __linux__
      if (consolefd != -1) {
        ioctl(consolefd, KIOCSOUND, 0);
      }
#elif defined(WIN32)
      // FIXME: sound should start at beep_on() and end here
      DWORD threadID;
      beep_info.msec = (DWORD)((bx_pc_system.time_usec() - usec_start) / 1000);
      beep_info.frequency = (DWORD)beep_frequency;
      CreateThread(NULL, 0, BeepThread, NULL, 0, &threadID);
#endif
    }
  } else if (output_mode == BX_SPK_MODE_GUI) {
    // give the gui a chance to signal beep off
    bx_gui->beep_off();
  }

  beep_frequency = 0.0;
}
