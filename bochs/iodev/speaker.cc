/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003       David N. Welton <davidw@dedasys.com>.
//  Copyright (C) 2003-2020  The Bochs Project
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
#include "sound/soundlow.h"
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

#if defined(__linux__) || defined(WIN32)
#define BX_SYSTEM_SPEAKER
#endif

enum {
  BX_SPK_MODE_NONE,
#if BX_SUPPORT_SOUNDLOW
  BX_SPK_MODE_SOUND,
#endif
#ifdef BX_SYSTEM_SPEAKER
  BX_SPK_MODE_SYSTEM,
#endif
  BX_SPK_MODE_GUI
};

static const char *speaker_mode_list[] = {
  "none",
#if BX_SUPPORT_SOUNDLOW
  "sound",
#endif
#ifdef BX_SYSTEM_SPEAKER
  "system",
#endif
  "gui",
  NULL
};

#if BX_SUPPORT_SOUNDLOW
BX_MUTEX(beep_mutex);

Bit32u beep_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len);
#endif

// builtin configuration handling functions

void speaker_init_options(void)
{
  bx_list_c *deplist;

  bx_list_c *sound = (bx_list_c*)SIM->get_param("sound");
  bx_list_c *menu = new bx_list_c(sound, "speaker", "PC speaker output configuration");
  menu->set_options(menu->SERIES_ASK);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu, "enabled", "Enable speaker output",
      "Enables the PC speaker output", 1);
  bx_param_enum_c *mode = new bx_param_enum_c(menu, "mode", "Speaker output mode",
      "The mode can be one these: 'none', 'sound', 'system' or 'gui'",
      speaker_mode_list, 1, BX_SPK_MODE_NONE);
  mode->set_ask_format("Select speker output mode [%s] ");
#if BX_SUPPORT_SOUNDLOW
  bx_param_num_c *volume = new bx_param_num_c(menu, "volume", "Speaker volume",
      "Set the PC speaker volume", 0, 15, 15);
#endif
  deplist = new bx_list_c(NULL);
  deplist->add(mode);
  enabled->set_dependent_list(deplist);
#if BX_SUPPORT_SOUNDLOW
  deplist = new bx_list_c(NULL);
  deplist->add(volume);
  mode->set_dependent_list(deplist, 0);
  mode->set_dependent_bitmap(BX_SPK_MODE_SOUND, 1);
#endif
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

int CDECL libspeaker_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
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
#if BX_SUPPORT_SOUNDLOW
  waveout = NULL;
#endif
}

bx_speaker_c::~bx_speaker_c()
{
  beep_off();
  switch (output_mode) {
#if BX_SUPPORT_SOUNDLOW
    case BX_SPK_MODE_SOUND:
      beep_active = 0;
      if (waveout != NULL) {
        if (beep_callback_id >= 0) {
          waveout->unregister_wave_callback(beep_callback_id);
        }
      }
      break;
#endif
#ifdef __linux__
    case BX_SPK_MODE_SYSTEM:
      if (consolefd >= 0) {
        close(consolefd);
      }
      break;
#endif
  }
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
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("speaker"))->set(0);
    return;
  }
  output_mode = SIM->get_param_enum("mode", base)->get();
  switch (output_mode) {
#if BX_SUPPORT_SOUNDLOW
    case BX_SPK_MODE_SOUND:
      waveout = DEV_sound_get_waveout(0);
      if (waveout != NULL) {
        beep_active = 0;
        beep_volume = SIM->get_param_num("volume", base)->get();
#if BX_HAVE_REALTIME_USEC
        dsp_active = 0;
        dsp_start_usec = bx_get_realtime64_usec();
        dsp_cb_usec = 0;
        dsp_count = 0;
#endif
        BX_INIT_MUTEX(beep_mutex);
        beep_callback_id = waveout->register_wave_callback(theSpeaker, beep_callback);
        BX_INFO(("Using lowlevel sound support for output"));
      } else {
        BX_ERROR(("Failed to use lowlevel sound support for output"));
        output_mode = BX_SPK_MODE_NONE;
      }
      break;
#endif
#ifdef BX_SYSTEM_SPEAKER
    case BX_SPK_MODE_SYSTEM:
#ifdef __linux__
      consolefd = open("/dev/console", O_WRONLY);
      if (consolefd != -1) {
        BX_INFO(("Using /dev/console for output"));
      } else {
        BX_ERROR(("Failed to open /dev/console: %s", strerror(errno)));
        BX_ERROR(("Deactivating beep on console"));
        output_mode = BX_SPK_MODE_NONE;
      }
#elif defined(WIN32)
      BX_INFO(("Using system beep for output"));
#endif
      break;
#endif
    case BX_SPK_MODE_GUI:
      BX_INFO(("Forwarding beep to gui"));
      break;
  }
}

void bx_speaker_c::reset(unsigned type)
{
  beep_off();
}

#if BX_SUPPORT_SOUNDLOW
Bit32u bx_speaker_c::beep_generator(Bit16u rate, Bit8u *buffer, Bit32u len)
{
  Bit32u j = 0, ret = 0;
  Bit16u beep_samples;
  static Bit16u beep_pos = 0;

  BX_LOCK(beep_mutex);
  if (!beep_active) {
    beep_samples = 0;
  } else {
    beep_samples = (Bit32u)((float)rate / beep_frequency / 2);
  }
  if (beep_samples == 0) {
#if BX_SUPPORT_SOUNDLOW && BX_HAVE_REALTIME_USEC
    if (dsp_active) {
      ret = dsp_generator(rate, buffer, len);
    }
#endif
    BX_UNLOCK(beep_mutex);
    return ret;
  }
  do {
    buffer[j++] = (Bit8u)beep_level;
    buffer[j++] = (Bit8u)(beep_level >> 8);
    buffer[j++] = (Bit8u)beep_level;
    buffer[j++] = (Bit8u)(beep_level >> 8);
    if ((++beep_pos % beep_samples) == 0) {
      beep_level *= -1;
      beep_pos = 0;
      beep_samples = (Bit32u)((float)rate / beep_frequency / 2);
      if (beep_samples == 0) break;
    }
  } while (j < len);
  BX_UNLOCK(beep_mutex);
  return len;
}

#if BX_SUPPORT_SOUNDLOW && BX_HAVE_REALTIME_USEC
Bit32u bx_speaker_c::dsp_generator(Bit16u rate, Bit8u *buffer, Bit32u len)
{
  Bit32u i = 0, j = 0;
  double tmp_dsp_usec, step_usec;

  Bit64u new_dsp_cb_usec = bx_get_realtime64_usec() - dsp_start_usec;
  if (dsp_cb_usec == 0) {
    dsp_cb_usec = new_dsp_cb_usec - 25000;
  }
  tmp_dsp_usec = (double)dsp_cb_usec;
  step_usec = 1000000.0 / (double)rate;
  do {
    if ((i < dsp_count) && (dsp_event_buffer[i] < (Bit64u)tmp_dsp_usec)) {
      beep_level *= -1;
      i++;
    }
    buffer[j++] = (Bit8u)beep_level;
    buffer[j++] = (Bit8u)(beep_level >> 8);
    buffer[j++] = (Bit8u)beep_level;
    buffer[j++] = (Bit8u)(beep_level >> 8);
    tmp_dsp_usec += step_usec;
  } while (j < len);
  dsp_active = 0;
  dsp_count = 0;
  dsp_cb_usec = new_dsp_cb_usec;
  return len;
}
#endif

Bit32u beep_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  return ((bx_speaker_c*)dev)->beep_generator(rate, buffer, len);
}
#endif

void bx_speaker_c::beep_on(float frequency)
{
#if defined(WIN32)
  if ((output_mode == BX_SPK_MODE_SYSTEM) && (beep_frequency != 0.0)) {
    beep_off();
  }
#endif

  switch (output_mode) {
#if BX_SUPPORT_SOUNDLOW
    case BX_SPK_MODE_SOUND:
      if ((waveout != NULL) && (frequency != beep_frequency)) {
        BX_LOCK(beep_mutex);
        beep_frequency = frequency;
        if (!beep_active) {
          beep_level = (Bit16s)(0x4000 * (beep_volume / 15.0f));
        }
        beep_active = 1;
        BX_UNLOCK(beep_mutex);
      }
      break;
#endif
#ifdef BX_SYSTEM_SPEAKER
    case BX_SPK_MODE_SYSTEM:
#ifdef __linux__
      if (consolefd != -1) {
        BX_DEBUG(("PC speaker on with frequency %f", frequency));
        ioctl(consolefd, KIOCSOUND, (int)(clock_tick_rate/frequency));
      }
#elif defined(WIN32)
      usec_start = bx_pc_system.time_usec();
#endif
      break;
#endif
    case BX_SPK_MODE_GUI:
      // give the gui a chance to signal beep on
      bx_gui->beep_on(frequency);
      break;
  }
  beep_frequency = frequency;
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
  switch (output_mode) {
#if BX_SUPPORT_SOUNDLOW
    case BX_SPK_MODE_SOUND:
      if (waveout != NULL) {
        BX_LOCK(beep_mutex);
        beep_active = 0;
        beep_frequency = 0.0;
        BX_UNLOCK(beep_mutex);
      }
      break;
#endif
#ifdef BX_SYSTEM_SPEAKER
    case BX_SPK_MODE_SYSTEM:
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
        CloseHandle(CreateThread(NULL, 0, BeepThread, NULL, 0, &threadID));
#endif
      }
      break;
#endif
    case BX_SPK_MODE_GUI:
      // give the gui a chance to signal beep off
      bx_gui->beep_off();
      break;
  }
  beep_frequency = 0.0;
}

void bx_speaker_c::set_line(bx_bool level)
{
#if BX_SUPPORT_SOUNDLOW && BX_HAVE_REALTIME_USEC
  if (output_mode == BX_SPK_MODE_SOUND) {
    BX_LOCK(beep_mutex);
    Bit64u timestamp = bx_get_realtime64_usec() - dsp_start_usec;
    dsp_active = 1;
    if (dsp_count < 500) {
      dsp_event_buffer[dsp_count++] = timestamp;
    } else {
      BX_ERROR(("DSP event buffer full"));
    }
    BX_UNLOCK(beep_mutex);
  }
#else
  BX_DEBUG(("setting speaker line to %d", level));
#endif
}
