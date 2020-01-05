/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2017  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// ALSA PCM input/output and MIDI output support written by Volker Ruppert

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "soundlow.h"
#include "soundmod.h"
#include "soundalsa.h"

#if BX_HAVE_SOUND_ALSA && BX_SUPPORT_SOUNDLOW

#define LOG_THIS log->

// sound driver plugin entry points

int CDECL libalsa_sound_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL libalsa_sound_plugin_fini(void)
{
  // Nothing here yet
}

// helper function for wavein / waveout

int alsa_pcm_open(bx_bool mode, alsa_pcm_t *alsa_pcm, bx_pcm_param_t *param, logfunctions *log)
{
  snd_pcm_format_t fmt;
  snd_pcm_hw_params_t *hwparams;
  unsigned int size, freq;
  int dir, ret, signeddata = param->format & 1;

  alsa_pcm->audio_bufsize = 0;

  if (alsa_pcm->handle == NULL) {
    ret = snd_pcm_open(&alsa_pcm->handle, "default", mode ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
      return BX_SOUNDLOW_ERR;
    }
    BX_INFO(("ALSA: opened default PCM %s device", mode ? "input":"output"));
  }
  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(alsa_pcm->handle, hwparams);
  snd_pcm_hw_params_set_access(alsa_pcm->handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

  freq = (unsigned int)param->samplerate;

  if (param->bits == 16) {
    if (signeddata == 1)
      fmt = SND_PCM_FORMAT_S16_LE;
    else
      fmt = SND_PCM_FORMAT_U16_LE;
    size = 2;
  } else if (param->bits == 8) {
    if (signeddata == 1)
      fmt = SND_PCM_FORMAT_S8;
    else
      fmt = SND_PCM_FORMAT_U8;
    size = 1;
  } else
    return BX_SOUNDLOW_ERR;

  if (param->channels == 2) size *= 2;

  ret = snd_pcm_hw_params_set_format(alsa_pcm->handle, hwparams, fmt);
  if (ret < 0)
    return BX_SOUNDLOW_ERR;
  ret = snd_pcm_hw_params_set_channels(alsa_pcm->handle, hwparams, param->channels);
  if (ret < 0)
    return BX_SOUNDLOW_ERR;
#if BX_HAVE_LIBSAMPLERATE || BX_HAVE_SOXR_LSR
  snd_pcm_hw_params_set_rate_resample(alsa_pcm->handle, hwparams, 0);
#endif
  ret =snd_pcm_hw_params_set_rate_near(alsa_pcm->handle, hwparams, &freq, &dir);
  if (ret < 0)
    return BX_SOUNDLOW_ERR;
  if (freq != param->samplerate) {
    param->samplerate = freq;
    BX_INFO(("changed sample rate to %d", freq));
  }

  alsa_pcm->frames = 32;
  snd_pcm_hw_params_set_period_size_near(alsa_pcm->handle, hwparams, &alsa_pcm->frames, &dir);

  ret = snd_pcm_hw_params(alsa_pcm->handle, hwparams);
  if (ret < 0) {
    return BX_SOUNDLOW_ERR;
  }
  snd_pcm_hw_params_get_period_size(hwparams, &alsa_pcm->frames, &dir);
  alsa_pcm->alsa_bufsize = alsa_pcm->frames * size;
  BX_DEBUG(("ALSA: buffer size set to %d", alsa_pcm->alsa_bufsize));
  if (alsa_pcm->buffer != NULL) {
    free(alsa_pcm->buffer);
    alsa_pcm->buffer = NULL;
  }
  return BX_SOUNDLOW_OK;
}

#undef LOG_THIS
#define LOG_THIS

// bx_soundlow_waveout_alsa_c class implementation

bx_soundlow_waveout_alsa_c::bx_soundlow_waveout_alsa_c()
    :bx_soundlow_waveout_c()
{
  alsa_waveout.handle = NULL;
  alsa_waveout.buffer = NULL;
}

int bx_soundlow_waveout_alsa_c::openwaveoutput(const char *wavedev)
{
  set_pcm_params(&real_pcm_param);
  pcm_callback_id = register_wave_callback(this, pcm_callback);
  start_resampler_thread();
  start_mixer_thread();
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_alsa_c::set_pcm_params(bx_pcm_param_t *param)
{
  return alsa_pcm_open(0, &alsa_waveout, param, this);
}

int bx_soundlow_waveout_alsa_c::get_packetsize()
{
  return alsa_waveout.alsa_bufsize;
}

int bx_soundlow_waveout_alsa_c::output(int length, Bit8u data[])
{
  if (!alsa_waveout.handle || (length > alsa_waveout.alsa_bufsize)) {
    return BX_SOUNDLOW_ERR;
  }
  int ret = snd_pcm_writei(alsa_waveout.handle, data, alsa_waveout.frames);
  if (ret == -EPIPE) {
    /* EPIPE means underrun */
    BX_ERROR(("ALSA: underrun occurred"));
    snd_pcm_prepare(alsa_waveout.handle);
  } else if (ret < 0) {
    BX_ERROR(("ALSA: error from writei: %s", snd_strerror(ret)));
  }  else if (ret != (int)alsa_waveout.frames) {
    BX_ERROR(("ALSA: short write, write %d frames", ret));
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_alsa_c::closewaveoutput()
{
  if (alsa_waveout.handle != NULL) {
    snd_pcm_drain(alsa_waveout.handle);
    snd_pcm_close(alsa_waveout.handle);
    alsa_waveout.handle = NULL;
  }
  return BX_SOUNDLOW_OK;
}

// bx_soundlow_wavein_alsa_c class implementation

bx_soundlow_wavein_alsa_c::bx_soundlow_wavein_alsa_c()
    :bx_soundlow_wavein_c()
{
  alsa_wavein.handle = NULL;
  alsa_wavein.buffer = NULL;
}

bx_soundlow_wavein_alsa_c::~bx_soundlow_wavein_alsa_c()
{
  if (alsa_wavein.handle != NULL) {
    snd_pcm_drain(alsa_wavein.handle);
    snd_pcm_close(alsa_wavein.handle);
    alsa_wavein.handle = NULL;
  }
}

int bx_soundlow_wavein_alsa_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = DEV_register_timer(this, record_timer_handler, 1, 1, 0, "wavein");
    // record timer: inactive, continuous, frequency variable
  }
  wavein_param.samplerate = 0;

  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_alsa_c::startwaverecord(bx_pcm_param_t *param)
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
  if (memcmp(param, &wavein_param, sizeof(bx_pcm_param_t)) == 0) {
    return BX_SOUNDLOW_OK;
  } else {
    wavein_param = *param;
    return alsa_pcm_open(1, &alsa_wavein, param, this);
  }
}

int bx_soundlow_wavein_alsa_c::getwavepacket(int length, Bit8u data[])
{
  int ret;

  if (alsa_wavein.buffer == NULL) {
    alsa_wavein.buffer = new char[alsa_wavein.alsa_bufsize];
  }
  while (alsa_wavein.audio_bufsize < length) {
    ret = snd_pcm_readi(alsa_wavein.handle, alsa_wavein.buffer, alsa_wavein.frames);
    if (ret == -EAGAIN)
      continue;
    if (ret == -EPIPE) {
      /* EPIPE means overrun */
      BX_ERROR(("overrun occurred"));
      snd_pcm_prepare(alsa_wavein.handle);
    } else if (ret < 0) {
      BX_ERROR(("error from read: %s", snd_strerror(ret)));
    } else if (ret != (int)alsa_wavein.frames) {
      BX_ERROR(("short read, read %d frames", ret));
    }
    memcpy(audio_buffer+alsa_wavein.audio_bufsize, alsa_wavein.buffer, alsa_wavein.alsa_bufsize);
    alsa_wavein.audio_bufsize += alsa_wavein.alsa_bufsize;
  }
  memcpy(data, audio_buffer, length);
  alsa_wavein.audio_bufsize -= length;
  if ((alsa_wavein.audio_bufsize <= 0) && (alsa_wavein.buffer != NULL)) {
    delete [] alsa_wavein.buffer;
    alsa_wavein.buffer = NULL;
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_alsa_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  if (alsa_wavein.handle != NULL) {
    snd_pcm_drain(alsa_wavein.handle);
  }

  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_alsa_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_alsa_c *class_ptr = (bx_soundlow_wavein_alsa_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_alsa_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

// bx_soundlow_midiout_alsa_c class implementation

bx_soundlow_midiout_alsa_c::bx_soundlow_midiout_alsa_c()
    :bx_soundlow_midiout_c()
{
  alsa_seq.handle = NULL;
}

bx_soundlow_midiout_alsa_c::~bx_soundlow_midiout_alsa_c()
{
  closemidioutput();
}

int bx_soundlow_midiout_alsa_c::alsa_seq_open(const char *alsadev)
{
  char *mididev, *ptr;
  int client, port, ret = 0;
  int length = strlen(alsadev) + 1;

  mididev = new char[length];

  if (mididev == NULL)
    return BX_SOUNDLOW_ERR;

  strcpy(mididev, alsadev);
  ptr = strtok(mididev, ":");
  if (ptr == NULL) {
    BX_ERROR(("ALSA sequencer setup: missing client parameters"));
    return BX_SOUNDLOW_ERR;
  }
  client = atoi(ptr);
  ptr = strtok(NULL, ":");
  if (ptr == NULL) {
    BX_ERROR(("ALSA sequencer setup: missing port parameter"));
    return BX_SOUNDLOW_ERR;
  }
  port = atoi(ptr);

  delete(mididev);

  if (snd_seq_open(&alsa_seq.handle, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
    BX_ERROR(("Couldn't open ALSA sequencer for midi output"));
    return BX_SOUNDLOW_ERR;
  }
  ret = snd_seq_create_simple_port(alsa_seq.handle, NULL,
    SND_SEQ_PORT_CAP_WRITE |
    SND_SEQ_PORT_CAP_SUBS_WRITE |
    SND_SEQ_PORT_CAP_READ,
    SND_SEQ_PORT_TYPE_MIDI_GENERIC);
  if (ret < 0) {
    BX_ERROR(("ALSA sequencer: error creating port %s", snd_strerror(errno)));
  } else {
    alsa_seq.source_port = ret;
    ret = snd_seq_connect_to(alsa_seq.handle, alsa_seq.source_port, client, port);
    if (ret < 0) {
      BX_ERROR(("ALSA sequencer: could not connect to port %d:%d", client, port));
    }
  }
  if (ret < 0) {
    snd_seq_close(alsa_seq.handle);
    return BX_SOUNDLOW_ERR;
  } else {
    return BX_SOUNDLOW_OK;
  }
}

int bx_soundlow_midiout_alsa_c::openmidioutput(const char *mididev)
{
  if ((mididev == NULL) || (strlen(mididev) < 1))
    return BX_SOUNDLOW_ERR;

  return alsa_seq_open(mididev);
}

int bx_soundlow_midiout_alsa_c::alsa_seq_output(int delta, int command, int length, Bit8u data[])
{
  int cmd, chan, value;
  snd_seq_event_t ev;

  snd_seq_ev_clear(&ev);
  snd_seq_ev_set_source(&ev, alsa_seq.source_port);
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_direct(&ev);
  cmd = command & 0xf0;
  chan = command & 0x0f;
  switch (cmd) {
    case 0x80:
      ev.type = SND_SEQ_EVENT_NOTEOFF;
      ev.data.note.channel = chan;
      ev.data.note.note = data[0];
      ev.data.note.velocity = data[1];
      ev.data.note.duration = delta;
      break;
    case 0x90:
      ev.type = SND_SEQ_EVENT_NOTEON;
      ev.data.note.channel = chan;
      ev.data.note.note = data[0];
      ev.data.note.velocity = data[1];
      ev.data.note.duration = 0;
      break;
    case 0xa0:
      ev.type = SND_SEQ_EVENT_KEYPRESS;
      ev.data.control.channel = chan;
      ev.data.control.param = data[0];
      ev.data.control.value = data[1];
      break;
    case 0xb0:
      ev.type = SND_SEQ_EVENT_CONTROLLER;
      ev.data.control.channel = chan;
      ev.data.control.param = data[0];
      ev.data.control.value = data[1];
      break;
    case 0xc0:
      ev.type = SND_SEQ_EVENT_PGMCHANGE;
      ev.data.control.channel = chan;
      ev.data.control.value = data[0];
      break;
    case 0xd0:
      ev.type = SND_SEQ_EVENT_CHANPRESS;
      ev.data.control.channel = chan;
      ev.data.control.value = data[0];
      break;
    case 0xe0:
      ev.type = SND_SEQ_EVENT_PITCHBEND;
      ev.data.control.channel = chan;
      value = data[0] | (data[1] << 7);
      value -= 0x2000;
      ev.data.control.value = value;
      break;
    case 0xf0:
      BX_ERROR(("alsa_seq_output(): SYSEX not implemented, length=%d", length));
      return BX_SOUNDLOW_ERR;
    default:
      BX_ERROR(("alsa_seq_output(): unknown command 0x%02x, length=%d", command, length));
      return BX_SOUNDLOW_ERR;
  }
  snd_seq_event_output(alsa_seq.handle, &ev);
  snd_seq_drain_output(alsa_seq.handle);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_alsa_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  if (alsa_seq.handle != NULL) {
    return alsa_seq_output(delta, command, length, data);
  }

  return BX_SOUNDLOW_ERR;
}

int bx_soundlow_midiout_alsa_c::closemidioutput()
{
  if (alsa_seq.handle != NULL) {
    snd_seq_close(alsa_seq.handle);
    alsa_seq.handle = NULL;
  }
  return BX_SOUNDLOW_OK;
}

// bx_sound_alsa_c class implementation

bx_soundlow_waveout_c* bx_sound_alsa_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_alsa_c();
  }
  return waveout;
}

bx_soundlow_wavein_c* bx_sound_alsa_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_alsa_c();
  }
  return wavein;
}

bx_soundlow_midiout_c* bx_sound_alsa_c::get_midiout()
{
  if (midiout == NULL) {
    midiout = new bx_soundlow_midiout_alsa_c();
  }
  return midiout;
}

#endif
