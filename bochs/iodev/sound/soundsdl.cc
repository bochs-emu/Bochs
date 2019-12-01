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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "soundlow.h"
#include "soundmod.h"
#include "soundsdl.h"

#if BX_HAVE_SOUND_SDL && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#include <SDL.h>

// sound driver plugin entry points

int CDECL libsdl_sound_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL libsdl_sound_plugin_fini(void)
{
  // Nothing here yet
}

// SDL audio callback

void sdl_callback(void *thisptr, Bit8u *stream, int len)
{
  memset(stream, 0, len);
  ((bx_soundlow_waveout_sdl_c*)thisptr)->mixer_common(stream, len);
}

// bx_soundlow_waveout_sdl_c class implementation

bx_soundlow_waveout_sdl_c::bx_soundlow_waveout_sdl_c()
    :bx_soundlow_waveout_c()
{
  WaveOutOpen = 0;
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
  WaveOutOpen = 0;
  mix_thread_start = 0;
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
  if (WaveOutOpen) {
    SDL_CloseAudio();
  } else {
    pcm_callback_id = register_wave_callback(this, pcm_callback);
  }
  if (SDL_OpenAudio(&fmt, NULL) < 0) {
    BX_PANIC(("SDL_OpenAudio() failed"));
    WaveOutOpen = 0;
    return BX_SOUNDLOW_ERR;
  } else {
    if (fmt.freq != param->samplerate) {
      param->samplerate = fmt.freq;
      BX_INFO(("changed sample rate to %d", fmt.freq));
    }
    WaveOutOpen = 1;
    mix_thread_start = 1;
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
  if (WaveOutOpen) {
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

  Bit8u *tmpbuffer = new Bit8u[len];
  for (int i = 0; i < cb_count; i++) {
    if (get_wave[i].cb != NULL) {
      memset(tmpbuffer, 0, len);
      len2 = get_wave[i].cb(get_wave[i].device, fmt.freq, tmpbuffer, len);
      if (len2 > 0) {
        SDL_MixAudio(buffer, tmpbuffer, len2, SDL_MIX_MAXVOLUME);
      }
    }
  }
  delete [] tmpbuffer;
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

// bx_soundlow_wavein_sdl2_c class implementation

#if BX_HAVE_SDL2_AUDIO_CAPTURE
bx_soundlow_wavein_sdl2_c::bx_soundlow_wavein_sdl2_c()
    :bx_soundlow_wavein_c()
{
  WaveInOpen = 0;
  devID = 0;
}

bx_soundlow_wavein_sdl2_c::~bx_soundlow_wavein_sdl2_c()
{
  if (WaveInOpen) {
    SDL_CloseAudioDevice(devID);
  }
}

int bx_soundlow_wavein_sdl2_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = DEV_register_timer(this, record_timer_handler, 1, 1, 0, "wavein");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_sdl2_c::startwaverecord(bx_pcm_param_t *param)
{
  int signeddata = param->format & 1;
  Bit64u timer_val;
  Bit8u shift = 0;

  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (param->bits == 16) shift++;
    if (param->channels == 2) shift++;
    record_packet_size = (param->samplerate / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (param->samplerate << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
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
  fmt.callback = NULL;
  fmt.userdata = NULL;
  if (WaveInOpen) {
    SDL_CloseAudioDevice(devID);
  }
  devID = SDL_OpenAudioDevice(NULL, 1, &fmt, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
  if (devID <= 0) {
    BX_PANIC(("SDL_OpenAudioDevive() failed"));
    WaveInOpen = 0;
    return BX_SOUNDLOW_ERR;
  } else {
    if (fmt.freq != param->samplerate) {
      param->samplerate = fmt.freq;
      BX_INFO(("changed sample rate to %d", fmt.freq));
    }
    WaveInOpen = 1;
  }
  SDL_PauseAudioDevice(devID, 0);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_sdl2_c::getwavepacket(int length, Bit8u data[])
{
  SDL_DequeueAudio(devID, data, length);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_sdl2_c::stopwaverecord()
{
  SDL_PauseAudioDevice(devID, 1);
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_sdl2_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_sdl2_c *class_ptr = (bx_soundlow_wavein_sdl2_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_sdl2_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}
#endif

// bx_sound_sdl_c class implementation

bx_soundlow_waveout_c* bx_sound_sdl_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_sdl_c();
  }
  return waveout;
}

#if BX_HAVE_SDL2_AUDIO_CAPTURE
bx_soundlow_wavein_c* bx_sound_sdl_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_sdl2_c();
  }
  return wavein;
}
#endif

#endif  // BX_HAVE_SOUND_SDL
