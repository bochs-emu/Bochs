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

// Lowlevel sound output support using PulseAudio simple API

#include "bochs.h"
#include "plugin.h"

#if BX_SUPPORT_SOUNDLOW

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

#endif // BX_HAVE_SOUND_PULSE
