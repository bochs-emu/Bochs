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

#define BX_SOUND_SDL_BUFSIZE  BX_SOUNDLOW_WAVEPACKETSIZE * 16

static struct {
  int iptr;
  int optr;
  Bit8u data[BX_SOUND_SDL_BUFSIZE];
} audio_buffer;


bx_sound_sdl_c::bx_sound_sdl_c()
    :bx_sound_lowlevel_c()
{
  WaveOpen = 0;
  cb_count = 0;
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
  return BX_SOUNDLOW_OK;
}

Bit32u pcm_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  UNUSED(dev);
  UNUSED(rate);
  int amount = audio_buffer.iptr - audio_buffer.optr;
  if (amount < 0) {
    amount += BX_SOUND_SDL_BUFSIZE;
  }
  if (amount > (int)len) {
    amount = len;
  }
  int tmplen = BX_SOUND_SDL_BUFSIZE - audio_buffer.optr;
  if (amount <= tmplen) {
    memcpy(buffer, &audio_buffer.data[audio_buffer.optr], amount);
    audio_buffer.optr += amount;
  } else {
    memcpy(buffer, &audio_buffer.data[audio_buffer.optr], tmplen);
    memcpy(buffer+tmplen, &audio_buffer.data[0], amount-tmplen);
    audio_buffer.optr = amount-tmplen;
  }
  return amount;
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

void sdl_callback(void *thisptr, Bit8u *stream, int len)
{
  memset(stream, 0, len);
  ((bx_sound_sdl_c*)thisptr)->get_wave_data(stream, len);
}

int bx_sound_sdl_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  int signeddata = format & 1;

  BX_DEBUG(("startwaveplayback(%d, %d, %d, %x)", frequency, bits, stereo, format));
  fmt.freq = frequency;

  if (bits == 16) {
    if (signeddata == 1)
      fmt.format = AUDIO_S16;
    else
      fmt.format = AUDIO_U16;
  } else if (bits == 8) {
    if (signeddata == 1)
      fmt.format = AUDIO_S8;
    else
      fmt.format = AUDIO_U8;
  } else
    return BX_SOUNDLOW_ERR;

  fmt.channels = stereo + 1;
  fmt.samples = frequency / 10;
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
  audio_buffer.iptr = 0;
  audio_buffer.optr = 0;
  SDL_PauseAudio(0);
  return BX_SOUNDLOW_OK;
}

int bx_sound_sdl_c::waveready()
{
  return BX_SOUNDLOW_OK;
}

void bx_sound_sdl_c::convert_wavedata(Bit8u *src, int srcsize, Bit8u *dst, int dstsize, bx_pcm_param_t *param)
{
  int i, j;
  Bit8u xor_val;

  xor_val = (param->format & 1) ? 0x00 : 0x80;
  if (param->bits == 16) {
    if ((param->format & 1) && (param->channels == 2)) {
      memcpy(dst, src, dstsize);
    } else if (param->channels == 2) {
      j = 0;
      for (i = 0; i < srcsize; i+=2) {
        dst[j++] = src[i];
        dst[j++] = src[i+1] ^ xor_val;
      }
    } else {
      j = 0;
      for (i = 0; i < srcsize; i+=2) {
        dst[j++] = src[i];
        dst[j++] = src[i+1] ^ xor_val;
        dst[j++] = src[i];
        dst[j++] = src[i+1] ^ xor_val;
      }
    }
  } else {
    if (param->channels == 2) {
      j = 0;
      for (i = 0; i < srcsize; i++) {
        dst[j++] = 0;
        dst[j++] = src[i] ^ xor_val;
      }
    } else {
      j = 0;
      for (i = 0; i < srcsize; i++) {
        dst[j++] = 0;
        dst[j++] = src[i] ^ xor_val;
        dst[j++] = 0;
        dst[j++] = src[i] ^ xor_val;
      }
    }
  }
}

int bx_sound_sdl_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *param)
{
  int ret = BX_SOUNDLOW_OK;
  int tmpsize;
  int bsize, len2, ssize = 1;

  if (memcmp(param, &pcm_param, sizeof(bx_pcm_param_t)) != 0) {
    startwaveplayback(param->samplerate, 16, 1, 1);
    pcm_param = *param;
  }
  len2 = length;
  if (param->bits == 8) ssize = 2;
  if (param->channels == 1) ssize *= 2;
  len2 = length * ssize;
  SDL_LockAudio();
  if (WaveOpen) {
    bsize = audio_buffer.optr - audio_buffer.iptr;
    if (bsize <= 0) {
      bsize += BX_SOUND_SDL_BUFSIZE;
    }
    if (bsize < len2) {
      BX_ERROR(("SDL: audio buffer overflow: l=%d s=%d", len2, bsize));
      len2 = (bsize > ssize) ? bsize - ssize : 0;
    }
    if (len2 > 0) {
      if ((audio_buffer.iptr + len2) > BX_SOUND_SDL_BUFSIZE) {
        tmpsize = BX_SOUND_SDL_BUFSIZE - audio_buffer.iptr;
        len2 -= tmpsize;
        convert_wavedata(data, tmpsize/ssize, audio_buffer.data+audio_buffer.iptr, tmpsize, param);
        convert_wavedata(data+tmpsize/ssize, len2/ssize, audio_buffer.data, len2, param);
        audio_buffer.iptr = len2;
      } else {
        convert_wavedata(data, length, audio_buffer.data+audio_buffer.iptr, len2, param);
        audio_buffer.iptr += len2;
      }
    }
  } else {
    BX_ERROR(("SDL: audio not open"));
    ret = BX_SOUNDLOW_ERR;
  }
  SDL_UnlockAudio();
  return ret;
}

int bx_sound_sdl_c::stopwaveplayback()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_sdl_c::closewaveoutput()
{
  WaveOpen = 0;
  SDL_CloseAudio();
  return BX_SOUNDLOW_OK;
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
