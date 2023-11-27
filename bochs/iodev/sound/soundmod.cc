/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2023  The Bochs Project
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

#include "bochs.h"
#include "plugin.h"
#include "gui/siminterface.h"
#include "param_names.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundmod.h"
#include "soundlow.h"

#if BX_WITH_SDL || BX_WITH_SDL2
#include <SDL.h>
#endif

#define LOG_THIS bx_soundmod_ctl.

bx_soundmod_ctl_c bx_soundmod_ctl;

const char **sound_driver_names;

bx_soundmod_ctl_c::bx_soundmod_ctl_c()
{
  put("soundctl", "SNDCTL");
}

bx_soundmod_ctl_c::~bx_soundmod_ctl_c()
{
  free(sound_driver_names);
}

void bx_soundmod_ctl_c::init()
{
  Bit8u i, count = 0;

  count = PLUG_get_plugins_count(PLUGTYPE_SND);
  sound_driver_names = (const char**) malloc((count + 1) * sizeof(char*));
  for (i = 0; i < count; i++) {
    sound_driver_names[i] = PLUG_get_plugin_name(PLUGTYPE_SND, i);
  }
  sound_driver_names[count] = NULL;
  // move 'dummy' module to the top of the list
  if (strcmp(sound_driver_names[0], "dummy")) {
    for (i = 1; i < count; i++) {
      if (!strcmp(sound_driver_names[i], "dummy")) {
        sound_driver_names[i] = sound_driver_names[0];
        sound_driver_names[0] = "dummy";
        break;
      }
    }
  }
}

void bx_soundmod_ctl_c::exit()
{
  bx_sound_lowlevel_c::cleanup();
}

const char **bx_soundmod_ctl_c::get_driver_names(void)
{
  return sound_driver_names;
}

void bx_soundmod_ctl_c::list_modules(void)
{
  char list[60];
  Bit8u i = 0;
  size_t len = 0, len1;

  BX_INFO(("Sound drivers"));
  list[0] = 0;
  while (sound_driver_names[i] != NULL) {
    len1 = strlen(sound_driver_names[i]);
    if ((len + len1 + 1) > 58) {
      BX_INFO((" %s", list));
      list[0] = 0;
      len = 0;
    }
    strcat(list, " ");
    strcat(list, sound_driver_names[i]);
    len = strlen(list);
    i++;
  }
  if (len > 0) {
    BX_INFO((" %s", list));
  }
}

void bx_soundmod_ctl_c::open_output()
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

bx_sound_lowlevel_c* bx_soundmod_ctl_c::get_driver(const char *modname)
{
  if (!bx_sound_lowlevel_c::module_present(modname)) {
#if BX_PLUGINS
    PLUG_load_plugin_var(modname, PLUGTYPE_SND);
#else
    BX_PANIC(("could not find sound driver '%s'", modname));
#endif
  }
  return bx_sound_lowlevel_c::get_module(modname);
}

bx_soundlow_waveout_c* bx_soundmod_ctl_c::get_waveout(bool using_file)
{
  bx_sound_lowlevel_c *module = NULL;

  if (!using_file) {
    module = get_driver(SIM->get_param_enum(BXPN_SOUND_WAVEOUT_DRV)->get_selected());
  } else {
    module = get_driver("file");
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

  module = get_driver(SIM->get_param_enum(BXPN_SOUND_WAVEIN_DRV)->get_selected());
  if (module != NULL) {
    wavein = module->get_wavein();
    if (wavein == NULL) {
      BX_ERROR(("sound service 'wavein' not available - using dummy driver"));
      module = get_driver("dummy");
      if (module != NULL) {
        wavein = module->get_wavein();
      }
    }
  }
  return wavein;
}

bx_soundlow_midiout_c* bx_soundmod_ctl_c::get_midiout(bool using_file)
{
  bx_sound_lowlevel_c *module = NULL;
  bx_soundlow_midiout_c *midiout = NULL;

  if (!using_file) {
    module = get_driver(SIM->get_param_enum(BXPN_SOUND_MIDIOUT_DRV)->get_selected());
  } else {
    module = get_driver("file");
  }
  if (module != NULL) {
    midiout = module->get_midiout();
    if (midiout == NULL) {
      BX_ERROR(("sound service 'midiout' not available - using dummy driver"));
      module = get_driver("dummy");
      if (module != NULL) {
        midiout = module->get_midiout();
      }
    }
  }
  return midiout;
}

#endif
