/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2017  The Bochs Project
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

// Sound driver loader code

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundmod.h"
#include "soundlow.h"

#if BX_WITH_SDL || BX_WITH_SDL2
#include <SDL.h>
#endif

#define LOG_THIS bx_soundmod_ctl.

bx_soundmod_ctl_c bx_soundmod_ctl;


bx_soundmod_ctl_c::bx_soundmod_ctl_c()
{
  put("soundctl", "SNDCTL");
}

void bx_soundmod_ctl_c::init()
{
  const char *pwaveout = SIM->get_param_string(BXPN_SOUND_WAVEOUT)->getptr();
  const char *pwavein = SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr();
  int ret;

  bx_soundlow_waveout_c *waveout = get_waveout(0);
  if (waveout != NULL) {
    if (!strlen(pwavein)) {
      SIM->get_param_string(BXPN_SOUND_WAVEIN)->set(pwaveout);
    }
    ret = waveout->openwaveoutput(pwaveout);
    if (ret != BX_SOUNDLOW_OK) {
      BX_PANIC(("Could not open wave output device"));
    }
  } else {
    BX_PANIC(("no waveout support present"));
  }
}

void bx_soundmod_ctl_c::exit()
{
  bx_sound_lowlevel_c::cleanup();
}

bx_sound_lowlevel_c* bx_soundmod_ctl_c::get_driver(int driver_id)
{
  const char *modname = sound_driver_names[driver_id];
  if (!bx_sound_lowlevel_c::module_present(modname)) {
#if BX_PLUGINS
    PLUG_load_snd_plugin(modname);
#else
    BX_PANIC(("could not find sound driver '%s'", modname));
#endif
  }
  return bx_sound_lowlevel_c::get_module(modname);
}

bx_soundlow_waveout_c* bx_soundmod_ctl_c::get_waveout(bx_bool using_file)
{
  bx_sound_lowlevel_c *module = NULL;

  if (!using_file) {
    int driver_id = SIM->get_param_enum(BXPN_SOUND_WAVEOUT_DRV)->get();
    module = get_driver(driver_id);
  } else {
    module = get_driver(BX_SOUNDDRV_FILE);
  }
  if (module != NULL) {
    return module->get_waveout();
  } else {
    return NULL;
  }
}

bx_soundlow_wavein_c* bx_soundmod_ctl_c::get_wavein()
{
  bx_sound_lowlevel_c *module = NULL;
  bx_soundlow_wavein_c *wavein = NULL;

  int driver_id = SIM->get_param_enum(BXPN_SOUND_WAVEIN_DRV)->get();
  module = get_driver(driver_id);
  if (module != NULL) {
    wavein = module->get_wavein();
    if (wavein == NULL) {
      BX_ERROR(("sound service 'wavein' not available - using dummy driver"));
      module = get_driver(BX_SOUNDDRV_DUMMY);
      if (module != NULL) {
        wavein = module->get_wavein();
      }
    }
  }
  return wavein;
}

bx_soundlow_midiout_c* bx_soundmod_ctl_c::get_midiout(bx_bool using_file)
{
  bx_sound_lowlevel_c *module = NULL;
  bx_soundlow_midiout_c *midiout = NULL;

  if (!using_file) {
    int driver_id = SIM->get_param_enum(BXPN_SOUND_MIDIOUT_DRV)->get();
    module = get_driver(driver_id);
  } else {
    module = get_driver(BX_SOUNDDRV_FILE);
  }
  if (module != NULL) {
    midiout = module->get_midiout();
    if (midiout == NULL) {
      BX_ERROR(("sound service 'midiout' not available - using dummy driver"));
      module = get_driver(BX_SOUNDDRV_DUMMY);
      if (module != NULL) {
        midiout = module->get_midiout();
      }
    }
  }
  return midiout;
}

#endif
