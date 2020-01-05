/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2017  The Bochs Project
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


#if BX_HAVE_SOUND_SDL

#include "bochs.h"
#include <SDL_audio.h>

// the waveout class

class bx_soundlow_waveout_sdl_c : public bx_soundlow_waveout_c {
public:
  bx_soundlow_waveout_sdl_c();
  virtual ~bx_soundlow_waveout_sdl_c();

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);

  virtual void unregister_wave_callback(int callback_id);

  virtual void resampler(audio_buffer_t *inbuffer, audio_buffer_t *outbuffer);
  virtual bx_bool mixer_common(Bit8u *buffer, int len);

private:
  bx_bool WaveOutOpen;
  SDL_AudioSpec fmt;
};

#if BX_HAVE_SDL2_AUDIO_CAPTURE
class bx_soundlow_wavein_sdl2_c : public bx_soundlow_wavein_c {
public:
  bx_soundlow_wavein_sdl2_c();
  virtual ~bx_soundlow_wavein_sdl2_c();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(bx_pcm_param_t *param);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();

  static void record_timer_handler(void *);
  void record_timer(void);
private:
  bx_bool WaveInOpen;
  SDL_AudioSpec fmt;
  SDL_AudioDeviceID devID;
};
#endif

class bx_sound_sdl_c : public bx_sound_lowlevel_c {
public:
  bx_sound_sdl_c() : bx_sound_lowlevel_c("sdl") {}
  virtual ~bx_sound_sdl_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
#if BX_HAVE_SDL2_AUDIO_CAPTURE
  virtual bx_soundlow_wavein_c* get_wavein();
#endif
} bx_sound_sdl;

#endif  // BX_HAVE_SOUND_SDL
