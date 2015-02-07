/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2015  The Bochs Project
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

// Lowlevel sound output support for SDL written by Volker Ruppert


#if BX_WITH_SDL || BX_WITH_SDL2

#include "bochs.h"
#include <SDL_audio.h>

class bx_sound_sdl_c : public bx_sound_lowlevel_c {
public:
  bx_sound_sdl_c();
  virtual ~bx_sound_sdl_c();

  virtual int get_type() {return BX_SOUNDLOW_SDL;}

  virtual int    openwaveoutput(const char *wavedev);
  virtual int    set_pcm_params(bx_pcm_param_t param);
  virtual int    sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param);
  virtual int    closewaveoutput();

  virtual void unregister_wave_callback(int callback_id);
  virtual bx_bool mixer_common(Bit8u *buffer, int len);
private:
  bx_bool WaveOpen;
  SDL_AudioSpec fmt;
};

#endif  // BX_WITH_SDL || BX_WITH_SDL2
