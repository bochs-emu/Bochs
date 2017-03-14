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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

// The dummy sound driver contains stubs for all features

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundlow.h"

int CDECL libdummy_sound_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL libdummy_sound_plugin_fini(void)
{
  // Nothing here yet
}

class bx_sound_dummy_c : public bx_sound_lowlevel_c {
public:
  bx_sound_dummy_c() : bx_sound_lowlevel_c("dummy") {}
  virtual ~bx_sound_dummy_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
  virtual bx_soundlow_wavein_c* get_wavein();
  virtual bx_soundlow_midiout_c* get_midiout();
} bx_sound_dummy;

// bx_sound_dummy_c class implementation

bx_soundlow_waveout_c* bx_sound_dummy_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_c();
  }
  return waveout;
}

bx_soundlow_wavein_c* bx_sound_dummy_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_c();
  }
  return wavein;
}

bx_soundlow_midiout_c* bx_sound_dummy_c::get_midiout()
{
  if (midiout == NULL) {
    midiout = new bx_soundlow_midiout_c();
  }
  return midiout;
}

#endif
