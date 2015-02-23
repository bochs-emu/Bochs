/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2015  The Bochs Project
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

// Common sound module code and dummy sound lowlevel functions

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundmod.h"
#include "soundlow.h"
#include "soundlnx.h"
#include "soundosx.h"
#include "soundwin.h"
#include "soundsdl.h"
#include "soundalsa.h"
#include "soundfile.h"

#if BX_WITH_SDL || BX_WITH_SDL2
#include <SDL.h>
#endif

#define LOG_THIS theSoundModCtl->

bx_soundmod_ctl_c* theSoundModCtl = NULL;

int CDECL libsoundmod_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theSoundModCtl = new bx_soundmod_ctl_c;
    bx_devices.pluginSoundModCtl = theSoundModCtl;
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libsoundmod_LTX_plugin_fini(void)
{
  delete theSoundModCtl;
}

bx_soundmod_ctl_c::bx_soundmod_ctl_c()
{
  put("soundctl", "SNDCTL");
  n_sound_drivers = 0;
  soundmod[0].module = NULL;
  waveout = NULL;
}

bx_soundmod_ctl_c::~bx_soundmod_ctl_c()
{
  for (unsigned i = 0; i < n_sound_drivers; i++) {
    delete soundmod[i].module;
  }
}

void bx_soundmod_ctl_c::init()
{
  bx_param_enum_c *driver = SIM->get_param_enum(BXPN_SOUND_DRIVER);
  int ret;
  const char *pwaveout = SIM->get_param_string(BXPN_SOUND_WAVEOUT)->getptr();
  const char *pwavein = SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr();

  if (get_driver(driver->get()) != NULL) {
    if (!strlen(pwavein)) {
      SIM->get_param_string(BXPN_SOUND_WAVEIN)->set(pwaveout);
    }
    waveout = soundmod[0].module->get_waveout();
    if (waveout != NULL) {
      ret = waveout->openwaveoutput(pwaveout);
      if (ret != BX_SOUNDLOW_OK) {
        BX_PANIC(("Could not open wave output device"));
      }
    } else {
      BX_PANIC(("no waveout support in sound driver '%s'", driver->get_selected());
    }
  }
}

bx_sound_lowlevel_c* bx_soundmod_ctl_c::get_driver(int driver_id)
{
  bx_sound_lowlevel_c *driver = NULL;
  unsigned i;

  for (i = 0; i < n_sound_drivers; i++) {
    if (driver_id == soundmod[i].drv_id)) {
      return soundmod[i].module;
    }
  }
  if (i == BX_MAX_SOUND_DRIVERS) {
    BX_PANIC(("Too many sound drivers!"));
    return NULL;
  }
  if (driver_id == BX_SOUNDDRV_DUMMY) {
    driver = new bx_sound_lowlevel_c();
  } else if (driver_id == BX_SOUNDDRV_FILE) {
    driver = new bx_sound_file_c();
#if BX_HAVE_SOUND_ALSA
  } else if (driver_id == BX_SOUNDDRV_ALSA) {
    driver = new bx_sound_alsa_c();
#endif
#if BX_HAVE_SOUND_SDL
  } else if (driver_id == BX_SOUNDDRV_SDL) {
    driver = new bx_sound_sdl_c();
#endif
#if BX_HAVE_SOUND_OSS
  } else if (driver_id == BX_SOUNDDRV_OSS) {
    driver = new bx_sound_oss_c();
#endif
#if BX_HAVE_SOUND_OSX
  } else if (driver_id == BX_SOUNDDRV_OSX) {
    driver = new bx_sound_osx_c();
#endif
#if BX_HAVE_SOUND_WIN
  } else if (driver_id == BX_SOUNDDRV_WIN) {
    driver = new bx_sound_windows_c();
#endif
  } else {
    BX_PANIC(("unknown lowlevel sound driver id %d", driver_id));
  }
  if (driver != NULL) {
    BX_INFO(("Installed sound driver '%s' at index #%d",
             sound_driver_names[driver_id], i));
    soundmod[i].drv_id = driver_id;
    soundmod[i].module = driver;
    n_sound_drivers++;
  }
  return driver;
}

bx_soundlow_waveout_c* bx_soundmod_ctl_c::get_waveout(const char *driver)
{
  bx_sound_lowlevel_c *module = NULL;

  if (!strcmp(driver, "default")) {
    module = soundmod[0].module;
  } else if (!strcmp(driver, "file")) {
    module = get_driver(BX_SOUNDDRV_FILE);
  }
  if (module != NULL) {
    return module->get_waveout();
  } else {
    return NULL;
  }
}

bx_soundlow_wavein_c* bx_soundmod_ctl_c::get_wavein(const char *driver)
{
  bx_sound_lowlevel_c *module = NULL;

  if (!strcmp(driver, "default")) {
    module = soundmod[0].module;
  }
  if (module != NULL) {
    return module->get_wavein();
  } else {
    return NULL;
  }
}

bx_soundlow_midiout_c* bx_soundmod_ctl_c::get_midiout(const char *driver)
{
  bx_sound_lowlevel_c *module = NULL;

  if (!strcmp(driver, "default")) {
    module = soundmod[0].module;
  } else if (!strcmp(driver, "file")) {
    module = get_driver(BX_SOUNDDRV_FILE);
  }
  if (module != NULL) {
    return module->get_midiout();
  } else {
    return NULL;
  }
}

#endif
