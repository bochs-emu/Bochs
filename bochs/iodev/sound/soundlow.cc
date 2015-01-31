/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2015  The Bochs Project
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

// Common sound module code and dummy sound lowlevel functions

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundlow.h"

#ifndef WIN32
#include <pthread.h>
#endif

// audio buffer support

static audio_buffer_t *audio_buffers = NULL;

audio_buffer_t* new_audio_buffer(Bit32u size)
{
  audio_buffer_t *newbuffer = new audio_buffer_t;
  newbuffer->data = new Bit8u[size];
  newbuffer->size = size;
  newbuffer->pos = 0;
  newbuffer->next = NULL;

  if (audio_buffers == NULL) {
    audio_buffers = newbuffer;
  } else {
    audio_buffer_t *temp = audio_buffers;

    while (temp->next)
      temp = temp->next;

    temp->next = newbuffer;
  }
  return newbuffer;
}

audio_buffer_t* get_current_buffer()
{
  return audio_buffers;
}

void delete_audio_buffer()
{
  audio_buffer_t *tmpbuffer = audio_buffers;
  audio_buffers = tmpbuffer->next;
  delete [] tmpbuffer->data;
  delete tmpbuffer;
}

// audio buffer callback function

Bit32u pcm_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  Bit32u copied = 0;
  UNUSED(dev);
  UNUSED(rate);

  while (len > 0) {
    audio_buffer_t *curbuffer = get_current_buffer();
    if (curbuffer == NULL)
      break;
    Bit32u tmplen = curbuffer->size - curbuffer->pos;
    if (tmplen > len) {
      tmplen = len;
    }
    if (tmplen > 0) {
      memcpy(buffer+copied, curbuffer->data+curbuffer->pos, tmplen);
      curbuffer->pos += tmplen;
      copied += tmplen;
      len -= tmplen;
    }
    if (curbuffer->pos >= curbuffer->size) {
      delete_audio_buffer();
    }
  }
  return copied;
}

// mixer thread support

static int mixer_control;

BX_THREAD_FUNC(mixer_thread, indata)
{
  bx_sound_lowlevel_c *soundmod = (bx_sound_lowlevel_c*)indata;
  mixer_control = 1;
  while (mixer_control > 0) {
    if (!soundmod->mixer_common()) {
      BX_MSLEEP(25);
    }
  }
  mixer_control = -1;
  BX_THREAD_EXIT;
}

// The dummy sound lowlevel functions. They don't do anything.

bx_sound_lowlevel_c::bx_sound_lowlevel_c()
{
  put("soundlow", "SNDLOW");
  record_timer_index = BX_NULL_TIMER_HANDLE;
  cb_count = 0;
  emu_pcm_param.samplerate = 0;
  mixer_control = 0;
}

bx_sound_lowlevel_c::~bx_sound_lowlevel_c()
{
  if (mixer_control > 0) {
    mixer_control = 0;
    while (mixer_control >= 0) {
      BX_MSLEEP(1);
    }
  }
}

void bx_sound_lowlevel_c::start_mixer_thread()
{
  BX_THREAD_ID(threadID);

  BX_THREAD_CREATE(mixer_thread, this, threadID);
}

bx_bool bx_sound_lowlevel_c::mixer_common()
{
  // TODO
  return 0;
}

int bx_sound_lowlevel_c::waveready()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::midiready()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openmidioutput(const char *mididev)
{
  UNUSED(mididev);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);
  UNUSED(command);
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closemidioutput()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openwaveoutput(const char *wavedev)
{
  UNUSED(wavedev);
  real_pcm_param = default_pcm_param;
  set_pcm_params(real_pcm_param);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::set_pcm_params(bx_pcm_param_t param)
{
  UNUSED(param);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  UNUSED(frequency);
  UNUSED(bits);
  UNUSED(stereo);
  UNUSED(format);
  return BX_SOUNDLOW_OK;
}

void bx_sound_lowlevel_c::convert_pcm_data(Bit8u *src, int srcsize, Bit8u *dst, int dstsize, bx_pcm_param_t *param)
{
  int i, j;
  Bit8u xor_val;
  Bit16s value16s;
  Bit8u volumes[2], channel = 0;

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
  if (param->volume != 0xffff) {
    volumes[0] = (Bit8u)(param->volume & 0xff);
    volumes[1] = (Bit8u)(param->volume >> 8);
    for (i = 0; i < dstsize; i+=2) {
      value16s = (Bit16s)(dst[i] | (dst[i+1] << 8));
      value16s = (Bit16s)((Bit32s)value16s * volumes[channel] / 256);
      dst[i] = (Bit8u)(value16s & 0xff);
      dst[i+1] = (Bit8u)(value16s >> 8);
      channel ^= 1;
    }
  }
}

int bx_sound_lowlevel_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param)
{
  if (memcmp(src_param, &emu_pcm_param, sizeof(bx_pcm_param_t)) != 0) {
    emu_pcm_param = *src_param;
    if (src_param->samplerate != real_pcm_param.samplerate) {
      real_pcm_param.samplerate = src_param->samplerate;
      set_pcm_params(real_pcm_param);
    }
  }
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::stopwaveplayback()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closewaveoutput()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "soundmod");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::startwaverecord(int frequency, int bits, bx_bool stereo, int format)
{
  Bit64u timer_val;
  Bit8u shift = 0;

  UNUSED(format);
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (bits == 16) shift++;
    if (stereo) shift++;
    record_packet_size = (frequency / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (frequency << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::getwavepacket(int length, Bit8u data[])
{
  memset(data, 0, length);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closewaveinput()
{
  stopwaverecord();
  return BX_SOUNDLOW_OK;
}

void bx_sound_lowlevel_c::record_timer_handler(void *this_ptr)
{
  bx_sound_lowlevel_c *class_ptr = (bx_sound_lowlevel_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_sound_lowlevel_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

#endif
