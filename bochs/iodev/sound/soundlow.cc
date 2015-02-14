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
BX_MUTEX(mixer_mutex);

BX_THREAD_FUNC(mixer_thread, indata)
{
  int len;

  bx_soundlow_waveout_c *waveout = (bx_soundlow_waveout_c*)indata;
  Bit8u *mixbuffer = new Bit8u[BX_SOUNDLOW_WAVEPACKETSIZE];
  mixer_control = 1;
  while (mixer_control > 0) {
    len = waveout->get_packetsize();
    memset(mixbuffer, 0, len);
    if (waveout->mixer_common(mixbuffer, len)) {
      waveout->output(len, mixbuffer);
    } else {
      BX_MSLEEP(25);
    }
  }
  delete [] mixbuffer;
  mixer_control = -1;
  BX_THREAD_EXIT;
}

// bx_soundlow_waveout_c class implemenzation
// The dummy output methods don't do anything.

bx_soundlow_waveout_c::bx_soundlow_waveout_c()
{
  put("waveout", "WAVOUT");
  real_pcm_param = default_pcm_param;
  emu_pcm_param = default_pcm_param;
  cb_count = 0;
  pcm_callback_id = -1;
  mixer_control = 0;
  set_pcm_params(&real_pcm_param);
}

bx_soundlow_waveout_c::~bx_soundlow_waveout_c()
{
  unregister_wave_callback(pcm_callback_id);
  if (mixer_control > 0) {
    mixer_control = 0;
    while (mixer_control >= 0) {
      BX_MSLEEP(1);
    }
  }
  BX_FINI_MUTEX(mixer_mutex);
}

int bx_soundlow_waveout_c::openwaveoutput(const char *wavedev)
{
  UNUSED(wavedev);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::set_pcm_params(bx_pcm_param_t *param)
{
  UNUSED(param);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param)
{
  int len2;

  if (memcmp(src_param, &emu_pcm_param, sizeof(bx_pcm_param_t)) != 0) {
    emu_pcm_param = *src_param;
    cvt_mult = (src_param->bits == 8) ? 2 : 1;
    if (src_param->channels == 1) cvt_mult <<= 1;
    if (src_param->samplerate != real_pcm_param.samplerate) {
      real_pcm_param.samplerate = src_param->samplerate;
      set_pcm_params(&real_pcm_param);
    }
  }
  len2 = length * cvt_mult;
  BX_LOCK(mixer_mutex);
  audio_buffer_t *newbuffer = new_audio_buffer(len2);
  convert_pcm_data(data, length, newbuffer->data, len2, src_param);
  BX_UNLOCK(mixer_mutex);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::get_packetsize()
{
  return (real_pcm_param.samplerate * 4 / 10);
}

int bx_soundlow_waveout_c::output(int length, Bit8u data[])
{
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::register_wave_callback(void *arg, get_wave_cb_t wd_cb)
{
  if (cb_count < BX_MAX_WAVE_CALLBACKS) {
    get_wave[cb_count].device = arg;
    get_wave[cb_count].cb = wd_cb;
    return cb_count++;
  }
  return -1;
}

void bx_soundlow_waveout_c::unregister_wave_callback(int callback_id)
{
  BX_LOCK(mixer_mutex);
  if ((callback_id >= 0) && (callback_id < BX_MAX_WAVE_CALLBACKS)) {
    get_wave[callback_id].device = NULL;
    get_wave[callback_id].cb = NULL;
  }
  BX_UNLOCK(mixer_mutex);
}

bx_bool bx_soundlow_waveout_c::mixer_common(Bit8u *buffer, int len)
{
  Bit32u count, len2 = 0, len3 = 0;
  Bit16s src1, src2, dst_val;
  Bit32s tmp_val;
  Bit8u *src, *dst;

  Bit8u *tmpbuffer = new Bit8u[len];
  BX_LOCK(mixer_mutex);
  for (int i = 0; i < cb_count; i++) {
    if (get_wave[i].cb != NULL) {
      memset(tmpbuffer, 0, len);
      len2 = get_wave[i].cb(get_wave[i].device, real_pcm_param.samplerate, tmpbuffer, len);
      if (len2 > 0) {
        src = tmpbuffer;
        dst = buffer;
        count = len / 2;
        while (count--) {
          src1 = (src[0] | (src[1] << 8));
          src2 = (dst[0] | (dst[1] << 8));
          tmp_val = (Bit32s)src1 + (Bit32s)src2;
          if (tmp_val > BX_MAX_BIT16S) {
            tmp_val = BX_MAX_BIT16S;
          } else if (tmp_val < BX_MIN_BIT16S) {
            tmp_val = BX_MIN_BIT16S;
          }
          dst_val = (Bit16s)tmp_val;
          dst[0] = dst_val & 0xff;
          dst[1] = (Bit8u)(dst_val >> 8);
          src += 2;
          dst += 2;
        }
        if (len3 < len2) len3 = len2;
      }
    }
  }
  BX_UNLOCK(mixer_mutex);
  delete [] tmpbuffer;
  return (len3 > 0);
}

void bx_soundlow_waveout_c::convert_pcm_data(Bit8u *src, int srcsize, Bit8u *dst, int dstsize, bx_pcm_param_t *param)
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

void bx_soundlow_waveout_c::start_mixer_thread()
{
  BX_THREAD_ID(threadID);

  BX_THREAD_CREATE(mixer_thread, this, threadID);
}

// bx_soundlow_wavein_c class implemenzation
// The dummy input method returns silence.

bx_soundlow_wavein_c::bx_soundlow_wavein_c()
{
  put("wavein", "WAVEIN");
  record_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_soundlow_wavein_c::~bx_soundlow_wavein_c()
{
  stopwaverecord();
}

int bx_soundlow_wavein_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "wavein");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::startwaverecord(bx_pcm_param_t *param)
{
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
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::getwavepacket(int length, Bit8u data[])
{
  memset(data, 0, length);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_c *class_ptr = (bx_soundlow_wavein_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

// bx_sound_lowlevel_c class implemenzation
// This is the base class of the sound lowlevel support.

bx_sound_lowlevel_c::bx_sound_lowlevel_c()
{
  put("soundlow", "SNDLOW");
  waveout = NULL;
  wavein = NULL;
}

bx_sound_lowlevel_c::~bx_sound_lowlevel_c()
{
  if (waveout != NULL) {
    delete waveout;
  }
  if (wavein != NULL) {
    delete wavein;
  }
}

bx_soundlow_waveout_c* bx_sound_lowlevel_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_c();
  }
  return waveout;
}

bx_soundlow_wavein_c* bx_sound_lowlevel_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_c();
  }
  return wavein;
}

int bx_sound_lowlevel_c::openmidioutput(const char *mididev)
{
  UNUSED(mididev);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::midiready()
{
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

#endif
