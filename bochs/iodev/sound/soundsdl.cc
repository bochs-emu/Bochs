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

#include "iodev.h"
#include "soundlow.h"
#include "soundsdl.h"

#if BX_HAVE_SOUND_SDL && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#include <SDL.h>

// SDL audio callback

void sdl_callback(void *thisptr, Bit8u *stream, int len)
{
  memset(stream, 0, len);
  ((bx_soundlow_waveout_sdl_c*)thisptr)->mixer_common(stream, len);
}

// bx_soundlow_waveout_sdl_c class implemenzation

bx_soundlow_waveout_sdl_c::bx_soundlow_waveout_sdl_c()
    :bx_soundlow_waveout_c()
{
  WaveOpen = 0;
  if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    BX_PANIC(("Initialization of sound lowlevel module 'sdl' failed"));
  } else {
    BX_INFO(("Sound lowlevel module 'sdl' initialized"));
  }
}

bx_soundlow_waveout_sdl_c::~bx_soundlow_waveout_sdl_c()
{
  if (pcm_callback_id >= 0) {
    unregister_wave_callback(pcm_callback_id);
    pcm_callback_id = -1;
  }
  WaveOpen = 0;
  mixer_control = 0;
  SDL_CloseAudio();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int bx_soundlow_waveout_sdl_c::openwaveoutput(const char *wavedev)
{
  set_pcm_params(&real_pcm_param);
  start_resampler_thread();
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_sdl_c::set_pcm_params(bx_pcm_param_t *param)
{
  int signeddata = param->format & 1;

  BX_DEBUG(("set_pcm_params(): %u, %u, %u, %02x", param->samplerate, param->bits,
            param->channels, param->format));
  fmt.freq = param->samplerate;

  if (param->bits == 16) {
    if (signeddata == 1)
      fmt.format = AUDIO_S16;
    else
      fmt.format = AUDIO_U16;
  } else if (param->bits == 8) {
    if (signeddata == 1)
      fmt.format = AUDIO_S8;
    else
      fmt.format = AUDIO_U8;
  } else
    return BX_SOUNDLOW_ERR;

  fmt.channels = param->channels;
  fmt.samples = fmt.freq / 10;
  fmt.callback = sdl_callback;
  fmt.userdata = this;
  if (WaveOpen) {
    SDL_CloseAudio();
  } else {
    pcm_callback_id = register_wave_callback(this, pcm_callback);
  }
  if (SDL_OpenAudio(&fmt, NULL) < 0) {
    BX_PANIC(("SDL_OpenAudio() failed"));
    WaveOpen = 0;
    return BX_SOUNDLOW_ERR;
  } else {
    if (fmt.freq != param->samplerate) {
      param->samplerate = fmt.freq;
      BX_INFO(("changed sample rate to %d", fmt.freq));
    }
    WaveOpen = 1;
    mixer_control = 1;
  }
  SDL_PauseAudio(0);
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_waveout_sdl_c::resampler(audio_buffer_t *inbuffer, audio_buffer_t *outbuffer)
{
  Bit32u fcount;
  float *fbuffer = NULL;

  UNUSED(outbuffer);
  fcount = resampler_common(inbuffer, &fbuffer);
  SDL_LockAudio();
  if (WaveOpen) {
    audio_buffer_t *newbuffer = audio_buffers[1]->new_buffer(fcount << 1);
    convert_float_to_s16le(fbuffer, fcount, newbuffer->data);
  }
  SDL_UnlockAudio();
  if (fbuffer != NULL) {
    delete [] fbuffer;
  }
}

bx_bool bx_soundlow_waveout_sdl_c::mixer_common(Bit8u *buffer, int len)
{
  Bit32u len2 = 0;

  Bit8u *tmpbuffer = (Bit8u*)malloc(len);
  for (int i = 0; i < cb_count; i++) {
    if (get_wave[i].cb != NULL) {
      memset(tmpbuffer, 0, len);
      len2 = get_wave[i].cb(get_wave[i].device, fmt.freq, tmpbuffer, len);
      if (len2 > 0) {
        SDL_MixAudio(buffer, tmpbuffer, len2, SDL_MIX_MAXVOLUME);
      }
    }
  }
  free(tmpbuffer);
  return 1;
}

void bx_soundlow_waveout_sdl_c::unregister_wave_callback(int callback_id)
{
  SDL_LockAudio();
  if ((callback_id >= 0) && (callback_id < BX_MAX_WAVE_CALLBACKS)) {
    get_wave[callback_id].device = NULL;
    get_wave[callback_id].cb = NULL;
  }
  SDL_UnlockAudio();
}

// bx_sound_sdl_c class implemenzation

bx_sound_sdl_c::bx_sound_sdl_c()
    :bx_sound_lowlevel_c()
{
  // nothing here yet
}

bx_soundlow_waveout_c* bx_sound_sdl_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_sdl_c();
  }
  return waveout;
}

#endif  // BX_HAVE_SOUND_SDL
