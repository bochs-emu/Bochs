/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2024  The Bochs Project
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

// PCM playback/recording support using PulseAudio simple API

#include "bochs.h"
#include "plugin.h"

#if BX_HAVE_SOUND_PULSE && BX_SUPPORT_SOUNDLOW

#include "pc_system.h"
#include "soundlow.h"
#include "soundpulse.h"

#define LOG_THIS

PLUGIN_ENTRY_FOR_SND_MODULE(pulse)
{
  if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_SND;
  }
  return 0; // Success
}

// bx_sound_pulse_c class implementation

bx_soundlow_waveout_c* bx_sound_pulse_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_pulse_c();
  }
  return waveout;
}

bx_soundlow_wavein_c* bx_sound_pulse_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_pulse_c();
  }
  return wavein;
}

// bx_sound_waveout_pulse_c class implementation

int bx_soundlow_waveout_pulse_c::openwaveoutput(const char *wavedev)
{
  set_pcm_params(&real_pcm_param);
  pcm_callback_id = register_wave_callback(this, pcm_callback);
  start_resampler_thread();
  start_mixer_thread();
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_pulse_c::set_pcm_params(bx_pcm_param_t *param)
{
  pa_sample_spec spec;
  int signeddata = param->format & 1;
  int pa_error;

  if ((param->bits == 16) && (signeddata == 1)) {
    spec.format = PA_SAMPLE_S16LE;
  } else if ((param->bits == 8) && (signeddata == 0)) {
    spec.format = PA_SAMPLE_U8;
  } else {
    return BX_SOUNDLOW_ERR;
  }
  spec.channels = param->channels;
  spec.rate = param->samplerate;
  if (!(s = pa_simple_new(NULL, "bochs", PA_STREAM_PLAYBACK, NULL, "playback", &spec, NULL, NULL, &pa_error))) {
    BX_ERROR(("pa_simple_new() failed: %s", pa_strerror(pa_error)));
    return BX_SOUNDLOW_ERR;
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_pulse_c::get_packetsize()
{
  return BX_SOUND_PULSE_BUFSIZE;
}

int bx_soundlow_waveout_pulse_c::output(int length, Bit8u data[])
{
  int pa_error;

  if (pa_simple_write(s, data, (size_t)length, &pa_error) < 0) {
    BX_ERROR(("pa_simple_write() failed: %s", pa_strerror(pa_error)));
    return BX_SOUNDLOW_ERR;
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_pulse_c::closewaveoutput()
{
  int pa_error;

  if (s) {
    if (pa_simple_drain(s, &pa_error) < 0) {
      BX_ERROR(("pa_simple_drain() failed: %s", pa_strerror(pa_error)));
      return BX_SOUNDLOW_ERR;
    }
    pa_simple_free(s);
  }
  return BX_SOUNDLOW_OK;
}

// bx_sound_wavein_pulse_c class implementation

bx_soundlow_wavein_pulse_c::bx_soundlow_wavein_pulse_c()
    :bx_soundlow_wavein_c()
{
  s = NULL;
}

bx_soundlow_wavein_pulse_c::~bx_soundlow_wavein_pulse_c()
{
  if (s) {
    pa_simple_free(s);
    s = NULL;
  }
}

int bx_soundlow_wavein_pulse_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = DEV_register_timer(this, record_timer_handler, 1, 1, 0, "wavein");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_pulse_c::startwaverecord(bx_pcm_param_t *param)
{
  pa_sample_spec spec;
  int signeddata = param->format & 1;
  int pa_error;
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
  if (memcmp(param, &wavein_param, sizeof(bx_pcm_param_t)) != 0) {
    wavein_param = *param;

    if (s) {
      pa_simple_free(s);
      s = NULL;
    }
    if ((param->bits == 16) && (signeddata == 1)) {
      spec.format = PA_SAMPLE_S16LE;
    } else if ((param->bits == 8) && (signeddata == 0)) {
      spec.format = PA_SAMPLE_U8;
    } else {
      return BX_SOUNDLOW_ERR;
    }
    spec.channels = param->channels;
    spec.rate = param->samplerate;
    if (!(s = pa_simple_new(NULL, "bochs", PA_STREAM_RECORD, NULL, "record", &spec, NULL, NULL, &pa_error))) {
      BX_ERROR(("pa_simple_new() failed: %s", pa_strerror(pa_error)));
      return BX_SOUNDLOW_ERR;
    }
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_pulse_c::getwavepacket(int length, Bit8u data[])
{
  int pa_error;

  if (s) {
    if (pa_simple_read(s, data, length, &pa_error) < 0) {
      BX_ERROR(("pa_simple_read() failed: %s", pa_strerror(pa_error)));
      return BX_SOUNDLOW_ERR;
    }
  } else {
    memset(data, 0, length);
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_pulse_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_pulse_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_pulse_c *class_ptr = (bx_soundlow_wavein_pulse_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_pulse_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

#endif // BX_HAVE_SOUND_PULSE
