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

#include "iodev.h"
#include "soundlow.h"
#include "soundsdl.h"

#if (BX_WITH_SDL || BX_WITH_SDL2) && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#include <SDL.h>

// SDL audio callback

void sdl_callback(void *thisptr, Bit8u *stream, int len)
{
  memset(stream, 0, len);
  ((bx_sound_sdl_c*)thisptr)->get_wave_data(stream, len);
}

// bx_sound_sdl_c class implemenzation

bx_sound_sdl_c::bx_sound_sdl_c()
    :bx_sound_lowlevel_c()
{
  WaveOpen = 0;
  pcm_callback_id = -1;
  if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    BX_PANIC(("Initialization of sound lowlevel module 'sdl' failed"));
  } else {
    BX_INFO(("Sound lowlevel module 'sdl' initialized"));
  }
}

bx_sound_sdl_c::~bx_sound_sdl_c()
{
  unregister_wave_callback(pcm_callback_id);
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

int bx_sound_sdl_c::openwaveoutput(const char *wavedev)
{
  set_pcm_params(real_pcm_param);
  return BX_SOUNDLOW_OK;
}

int bx_sound_sdl_c::set_pcm_params(bx_pcm_param_t param)
{
  int signeddata = param.format & 1;

  BX_DEBUG(("set_pcm_params(): %u, %u, %u, %02x", param.samplerate, param.bits,
            param.channels, param.format));
  fmt.freq = param.samplerate;

  if (param.bits == 16) {
    if (signeddata == 1)
      fmt.format = AUDIO_S16;
    else
      fmt.format = AUDIO_U16;
  } else if (param.bits == 8) {
    if (signeddata == 1)
      fmt.format = AUDIO_S8;
    else
      fmt.format = AUDIO_U8;
  } else
    return BX_SOUNDLOW_ERR;

  fmt.channels = param.channels;
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
    WaveOpen = 1;
  }
  SDL_PauseAudio(0);
  return BX_SOUNDLOW_OK;
}

int bx_sound_sdl_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param)
{
  int ret = BX_SOUNDLOW_OK;
  int len2;

  if (memcmp(src_param, &emu_pcm_param, sizeof(bx_pcm_param_t)) != 0) {
    emu_pcm_param = *src_param;
    cvt_mult = (src_param->bits == 8) ? 2 : 1;
    if (src_param->channels == 1) cvt_mult <<= 1;
    if (src_param->samplerate != real_pcm_param.samplerate) {
      real_pcm_param.samplerate = src_param->samplerate;
      set_pcm_params(real_pcm_param);
    }
  }
  len2 = length * cvt_mult;
  SDL_LockAudio();
  if (WaveOpen) {
    audio_buffer_t *newbuffer = new_audio_buffer(len2);
    convert_pcm_data(data, length, newbuffer->data, len2, src_param);
  } else {
    BX_ERROR(("SDL: audio not open"));
    ret = BX_SOUNDLOW_ERR;
  }
  SDL_UnlockAudio();
  return ret;
}

int bx_sound_sdl_c::closewaveoutput()
{
  WaveOpen = 0;
  SDL_CloseAudio();
  return BX_SOUNDLOW_OK;
}

void bx_sound_sdl_c::get_wave_data(Bit8u *stream, int len)
{
  Bit32u len2 = 0;

  Bit8u *tmpbuffer = (Bit8u*)malloc(len);
  for (int i = 0; i < cb_count; i++) {
    if (get_wave[i].cb != NULL) {
      memset(tmpbuffer, 0, len);
      len2 = get_wave[i].cb(get_wave[i].device, fmt.freq, tmpbuffer, len);
      if (len2 > 0) {
        SDL_MixAudio(stream, tmpbuffer, len2, SDL_MIX_MAXVOLUME);
      }
    }
  }
  free(tmpbuffer);
}

int bx_sound_sdl_c::register_wave_callback(void *arg, get_wave_cb_t wd_cb)
{
  if (cb_count < BX_MAX_WAVE_CALLBACKS) {
    get_wave[cb_count].device = arg;
    get_wave[cb_count].cb = wd_cb;
    return cb_count++;
  }
  return -1;
}

void bx_sound_sdl_c::unregister_wave_callback(int callback_id)
{
  SDL_LockAudio();
  if ((callback_id >= 0) && (callback_id < BX_MAX_WAVE_CALLBACKS)) {
    get_wave[callback_id].device = NULL;
    get_wave[callback_id].cb = NULL;
  }
  SDL_UnlockAudio();
}

#endif  // BX_WITH_SDL || BX_WITH_SDL2
