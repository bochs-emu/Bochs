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

#define BX_SOUND_SDL_BUFSIZE  BX_SOUNDLOW_WAVEPACKETSIZE * 8

static struct {
  int iptr;
  int optr;
  Bit8u data[BX_SOUND_SDL_BUFSIZE];
} audio_buffer;


bx_sound_sdl_c::bx_sound_sdl_c()
    :bx_sound_lowlevel_c()
{
  WaveOpen = 0;
  wo_cb = NULL;
  if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
    BX_PANIC(("Initialization of sound lowlevel module 'sdl' failed"));
  } else {
    BX_INFO(("Sound lowlevel module 'sdl' initialized"));
  }
}

bx_sound_sdl_c::~bx_sound_sdl_c()
{
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


int bx_sound_sdl_c::openwaveoutput(const char *wavedev)
{
  return BX_SOUNDLOW_OK;
}

Bit32u bx_sound_sdl_c::get_wave_data(Bit8u *stream, int len)
{
  if (wo_cb == NULL) {
    return 0;
  }
  Bit8u *tmpbuffer = (Bit8u*)malloc(len);
  memset(tmpbuffer, 0, len);
  Bit32u len2 = wo_cb(dev, 44100, tmpbuffer, len);
  if (len2 > 0) {
    SDL_MixAudio(stream, tmpbuffer, len2, SDL_MIX_MAXVOLUME);
  }
  free(tmpbuffer);
  return len2;
}

void sdl_callback(void *thisptr, Bit8u *stream, int len)
{
  memset(stream, 0, len);
  // FIXME: mix wave data
  if (((bx_sound_sdl_c*)thisptr)->get_wave_data(stream, len) > 0) {
    return;
  }
  int amount = audio_buffer.iptr - audio_buffer.optr;
  if (amount < 0) {
    amount += BX_SOUND_SDL_BUFSIZE;
  }
  if (amount > len) {
    amount = len;
  }
  int tmplen = BX_SOUND_SDL_BUFSIZE - audio_buffer.optr;
  if (amount <= tmplen) {
    SDL_MixAudio(stream, &audio_buffer.data[audio_buffer.optr], amount, SDL_MIX_MAXVOLUME);
    audio_buffer.optr += amount;
  } else {
    SDL_MixAudio(stream, &audio_buffer.data[audio_buffer.optr], tmplen, SDL_MIX_MAXVOLUME);
    amount -= tmplen;
    SDL_MixAudio(stream, &audio_buffer.data[0], amount, SDL_MIX_MAXVOLUME);
    audio_buffer.optr = amount;
  }
}

int bx_sound_sdl_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  SDL_AudioSpec fmt;
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
  int size;

  if (WaveOpen) {
    size = audio_buffer.optr - audio_buffer.iptr;
    if (size <= 0) {
      size += BX_SOUND_SDL_BUFSIZE;
    }
    if (size < BX_SOUNDLOW_WAVEPACKETSIZE) {
      return BX_SOUNDLOW_ERR;
    } else {
      return BX_SOUNDLOW_OK;
    }
  } else {
    return BX_SOUNDLOW_ERR;
  }
}

int bx_sound_sdl_c::sendwavepacket(int length, Bit8u data[])
{
  int ret = BX_SOUNDLOW_OK;
  int tmpsize;

  SDL_PauseAudio(1);
  if (waveready() == BX_SOUNDLOW_OK) {
    if ((audio_buffer.iptr + length) > BX_SOUND_SDL_BUFSIZE) {
      tmpsize = BX_SOUND_SDL_BUFSIZE - audio_buffer.iptr;
      memcpy(audio_buffer.data+audio_buffer.iptr, data, tmpsize);
      memcpy(audio_buffer.data, data+tmpsize, length-tmpsize);
      audio_buffer.iptr = length-tmpsize;
    } else {
      memcpy(audio_buffer.data+audio_buffer.iptr, data, length);
      audio_buffer.iptr += length;
    }
  } else {
    BX_ERROR(("SDL: audio buffer overflow"));
    ret = BX_SOUNDLOW_ERR;
  }
  SDL_PauseAudio(0);
  return ret;
}

int bx_sound_sdl_c::stopwaveplayback()
{
  while (audio_buffer.optr != audio_buffer.iptr) {
    SDL_Delay(1);
  }
  SDL_CloseAudio();
  WaveOpen = 0;
  return BX_SOUNDLOW_OK;
}

int bx_sound_sdl_c::closewaveoutput()
{
  return BX_SOUNDLOW_OK;
}

bx_bool bx_sound_sdl_c::set_waveout_callback(void *arg, waveout_callback_t wocb)
{
  dev = arg;
  wo_cb = wocb;
  return 1;
}

#endif  // BX_WITH_SDL || BX_WITH_SDL2
