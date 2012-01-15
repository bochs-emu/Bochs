/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2011  The Bochs Project
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

// Josef Drexler coded the original version of the lowlevel sound support
// for Linux using OSS. The current version also supports OSS on FreeBSD and
// ALSA PCM input/output on Linux.

#include "iodev.h"
#include "soundmod.h"
#include "soundlnx.h"

#if (defined(linux) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)) && BX_SUPPORT_SOUNDLOW

#define LOG_THIS device->

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

bx_sound_linux_c::bx_sound_linux_c(logfunctions *dev)
  :bx_sound_lowlevel_c(dev)
{
#if BX_HAVE_ALSASOUND
  alsa_seq.handle = NULL;
  alsa_pcm[0].handle = NULL;
  alsa_pcm[1].handle = NULL;
  alsa_pcm[0].buffer = NULL;
  alsa_pcm[1].buffer = NULL;
#endif
  midi = NULL;
  wave_device[0] = NULL;
  wave_device[1] = NULL;
  wave_fd[0] = -1;
  wave_fd[1] = -1;
  BX_INFO(("Sound lowlevel module 'linux' initialized"));
}

bx_sound_linux_c::~bx_sound_linux_c()
{
  // nothing for now
}


int bx_sound_linux_c::waveready()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::midiready()
{
  return BX_SOUNDLOW_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_seq_open(const char *alsadev)
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
#endif

int bx_sound_linux_c::openmidioutput(const char *mididev)
{
  if ((mididev == NULL) || (strlen(mididev) < 1))
    return BX_SOUNDLOW_ERR;

#if BX_HAVE_ALSASOUND
  use_alsa_seq = !strncmp(mididev, "alsa:", 5);
  if (use_alsa_seq) {
    return alsa_seq_open(mididev+5);
  }
#endif

  midi = fopen(mididev,"w");

  if (midi == NULL) {
    BX_ERROR(("Couldn't open midi output device %s: %s",
             mididev, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  return BX_SOUNDLOW_OK;
}


#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_seq_output(int delta, int command, int length, Bit8u data[])
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
#endif

int bx_sound_linux_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
#if BX_HAVE_ALSASOUND
  if ((use_alsa_seq) && (alsa_seq.handle != NULL)) {
    return alsa_seq_output(delta, command, length, data);
  }
#endif

  UNUSED(delta);

  fputc(command, midi);
  fwrite(data, 1, length, midi);
  fflush(midi);       // to start playing immediately

  return BX_SOUNDLOW_OK;
}


int bx_sound_linux_c::closemidioutput()
{
#if BX_HAVE_ALSASOUND
  if ((use_alsa_seq) && (alsa_seq.handle != NULL)) {
    snd_seq_close(alsa_seq.handle);
    return BX_SOUNDLOW_OK;
  }
#endif
  fclose(midi);

  return BX_SOUNDLOW_OK;
}


int bx_sound_linux_c::openwaveoutput(const char *wavedev)
{
#if BX_HAVE_ALSASOUND
  use_alsa_pcm = !strcmp(wavedev, "alsa");
  if (use_alsa_pcm) {
    return BX_SOUNDLOW_OK;
  }
#endif
  int length = strlen(wavedev) + 1;

  if (wave_device[0] != NULL)
    delete [] wave_device[0];

  wave_device[0] = new char[length];

  if (wave_device[0] == NULL)
    return BX_SOUNDLOW_ERR;

  strncpy(wave_device[0], wavedev, length);

  return BX_SOUNDLOW_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_pcm_open(bx_bool mode, int frequency, int bits, bx_bool stereo, int format)
{
  int ret;
  snd_pcm_format_t fmt;
  snd_pcm_hw_params_t *params;
  unsigned int size, freq;
  int dir, signeddata = format & 1;

  alsa_pcm[mode].audio_bufsize = 0;

  if (alsa_pcm[mode].handle == NULL) {
    ret = snd_pcm_open(&alsa_pcm[mode].handle, "default", mode ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (ret < 0) {
      return BX_SOUNDLOW_ERR;
    }
    BX_INFO(("ALSA: opened default PCM %s device", mode ? "input":"output"));
  }
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(alsa_pcm[mode].handle, params);
  snd_pcm_hw_params_set_access(alsa_pcm[mode].handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

  if ((frequency == wave_ch[mode].oldfreq) &&
      (bits == wave_ch[mode].oldbits) &&
      (stereo == wave_ch[mode].oldstereo) &&
      (format == wave_ch[mode].oldformat))
    return BX_SOUNDLOW_OK; // nothing to do

  wave_ch[mode].oldfreq = frequency;
  wave_ch[mode].oldbits = bits;
  wave_ch[mode].oldstereo = stereo;
  wave_ch[mode].oldformat = format;

  freq = (unsigned int)frequency;

  if (bits == 16) {
    if (signeddata == 1)
      fmt = SND_PCM_FORMAT_S16_LE;
    else
      fmt = SND_PCM_FORMAT_U16_LE;
    size = 2;
  } else if (bits == 8) {
    if (signeddata == 1)
      fmt = SND_PCM_FORMAT_S8;
    else
      fmt = SND_PCM_FORMAT_U8;
    size = 1;
  } else
    return BX_SOUNDLOW_ERR;

  if (stereo) size *= 2;

  snd_pcm_hw_params_set_format(alsa_pcm[mode].handle, params, fmt);
  snd_pcm_hw_params_set_channels(alsa_pcm[mode].handle, params, (stereo != 0) ? 2 : 1);
  snd_pcm_hw_params_set_rate_near(alsa_pcm[mode].handle, params, &freq, &dir);

  alsa_pcm[mode].frames = 32;
  snd_pcm_hw_params_set_period_size_near(alsa_pcm[mode].handle, params, &alsa_pcm[mode].frames, &dir);

  ret = snd_pcm_hw_params(alsa_pcm[mode].handle, params);
  if (ret < 0) {
    return BX_SOUNDLOW_ERR;
  }
  snd_pcm_hw_params_get_period_size(params, &alsa_pcm[mode].frames, &dir);
  alsa_pcm[mode].alsa_bufsize = alsa_pcm[mode].frames * size;
  BX_DEBUG(("ALSA: buffer size set to %d", alsa_pcm[mode].alsa_bufsize));
  if (alsa_pcm[mode].buffer != NULL) {
    free(alsa_pcm[mode].buffer);
    alsa_pcm[mode].buffer = NULL;
  }

  return BX_SOUNDLOW_OK;
}
#endif

int bx_sound_linux_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  int fmt, ret;
  int signeddata = format & 1;

#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    return alsa_pcm_open(0, frequency, bits, stereo, format);
  }
#endif
  if ((wave_device[0] == NULL) || (strlen(wave_device[0]) < 1))
    return BX_SOUNDLOW_ERR;

  if (wave_fd[0] == -1) {
    wave_fd[0] = open(wave_device[0], O_WRONLY);
    if (wave_fd[0] == -1) {
      return BX_SOUNDLOW_ERR;
    } else {
      BX_INFO(("OSS: opened output device %s", wave_device[0]));
    }
  } else {
    if ((frequency == wave_ch[0].oldfreq) &&
        (bits == wave_ch[0].oldbits) &&
        (stereo == wave_ch[0].oldstereo) &&
        (format == wave_ch[0].oldformat))
      return BX_SOUNDLOW_OK; // nothing to do
  }
  wave_ch[0].oldfreq = frequency;
  wave_ch[0].oldbits = bits;
  wave_ch[0].oldstereo = stereo;
  wave_ch[0].oldformat = format;

  if (bits == 16)
    if (signeddata == 1)
      fmt = AFMT_S16_LE;
    else
      fmt = AFMT_U16_LE;
  else if (bits == 8)
    if (signeddata == 1)
      fmt = AFMT_S8;
    else
      fmt = AFMT_U8;
  else
    return BX_SOUNDLOW_ERR;

      // set frequency etc.
  ret = ioctl(wave_fd[0], SNDCTL_DSP_RESET);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_RESET): %s", strerror(errno)));

  /*
  ret = ioctl(wave_fd[0], SNDCTL_DSP_SETFRAGMENT, &fragment);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_SETFRAGMENT, %d): %s",
             fragment, strerror(errno)));
  */

  ret = ioctl(wave_fd[0], SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0)   // abort if the format is unknown, to avoid playing noise
  {
      BX_DEBUG(("ioctl(SNDCTL_DSP_SETFMT, %d): %s",
               fmt, strerror(errno)));
      return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(wave_fd[0], SNDCTL_DSP_STEREO, &stereo);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_STEREO, %d): %s",
             stereo, strerror(errno)));

  ret = ioctl(wave_fd[0], SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_SPEED, %d): %s",
             frequency, strerror(errno)));

  // ioctl(wave_fd[0], SNDCTL_DSP_GETBLKSIZE, &fragment);
  // BX_DEBUG(("current output block size is %d", fragment));

  return BX_SOUNDLOW_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_pcm_write()
{
  int ret;

  if (alsa_pcm[0].buffer == NULL) {
    alsa_pcm[0].buffer = (char *)malloc(alsa_pcm[0].alsa_bufsize);
  }
  while (alsa_pcm[0].audio_bufsize >= alsa_pcm[0].alsa_bufsize) {
    memcpy(alsa_pcm[0].buffer, audio_buffer[0], alsa_pcm[0].alsa_bufsize);
    ret = snd_pcm_writei(alsa_pcm[0].handle, alsa_pcm[0].buffer, alsa_pcm[0].frames);
    if (ret == -EAGAIN)
      continue;
    if (ret == -EPIPE) {
      /* EPIPE means underrun */
      BX_ERROR(("ALSA: underrun occurred"));
      snd_pcm_prepare(alsa_pcm[0].handle);
    } else if (ret < 0) {
      BX_ERROR(("ALSA: error from writei: %s", snd_strerror(ret)));
    }  else if (ret != (int)alsa_pcm[0].frames) {
      BX_ERROR(("ALSA: short write, write %d frames", ret));
    }
    alsa_pcm[0].audio_bufsize -= alsa_pcm[0].alsa_bufsize;
    memcpy(audio_buffer[0], audio_buffer[0]+alsa_pcm[0].alsa_bufsize, alsa_pcm[0].audio_bufsize);
  }
  if ((alsa_pcm[0].audio_bufsize == 0) && (alsa_pcm[0].buffer != NULL)) {
    free(alsa_pcm[0].buffer);
    alsa_pcm[0].buffer = NULL;
  }

  return BX_SOUNDLOW_OK;
}
#endif

int bx_sound_linux_c::sendwavepacket(int length, Bit8u data[])
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    if ((alsa_pcm[0].audio_bufsize+length) <= BX_SOUND_LINUX_BUFSIZE) {
      memcpy(audio_buffer[0]+alsa_pcm[0].audio_bufsize, data, length);
      alsa_pcm[0].audio_bufsize += length;
    } else {
      BX_ERROR(("ALSA: audio buffer overflow"));
      return BX_SOUNDLOW_ERR;
    }
    if (alsa_pcm[0].audio_bufsize < alsa_pcm[0].alsa_bufsize) {
      return BX_SOUNDLOW_OK;
    } else {
      return alsa_pcm_write();
    }
  }
#endif
  int ret = write(wave_fd[0], data, length);

  if (ret == length) {
    return BX_SOUNDLOW_OK;
  } else {
    BX_ERROR(("OSS: write error"));
    return BX_SOUNDLOW_ERR;
  }
}

int bx_sound_linux_c::stopwaveplayback()
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (alsa_pcm[0].audio_bufsize > 0)) {
    if (alsa_pcm[0].audio_bufsize < alsa_pcm[0].alsa_bufsize) {
      memset(audio_buffer[0]+alsa_pcm[0].audio_bufsize, 0, alsa_pcm[0].alsa_bufsize-alsa_pcm[0].audio_bufsize);
      alsa_pcm[0].audio_bufsize = alsa_pcm[0].alsa_bufsize;
    }
    alsa_pcm_write();
    return BX_SOUNDLOW_OK;
  }
#endif
  // ioctl(wave_fd[0], SNDCTL_DSP_SYNC);
  // close(wave_fd[0]);
  // wave_fd[0] = -1;

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::closewaveoutput()
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (alsa_pcm[0].handle != NULL)) {
    snd_pcm_drain(alsa_pcm[0].handle);
    snd_pcm_close(alsa_pcm[0].handle);
    alsa_pcm[0].handle = NULL;
    return BX_SOUNDLOW_OK;
  }
#endif
  if (wave_device[0] != NULL)
    delete(wave_device[0]);

  if (wave_fd[0] != -1)
  {
      close(wave_fd[0]);
      wave_fd[0] = -1;
  }
  wave_device[0] = NULL;

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "soundlnx");
    // record timer: inactive, continuous, frequency variable
  }
#if BX_HAVE_ALSASOUND
  use_alsa_pcm = !strcmp(wavedev, "alsa");
  if (use_alsa_pcm) {
    return BX_SOUNDLOW_OK;
  }
#endif
  int length = strlen(wavedev) + 1;

  if (wave_device[1] != NULL)
    delete [] wave_device[1];

  wave_device[1] = new char[length];

  if (wave_device[1] == NULL)
    return BX_SOUNDLOW_ERR;

  strncpy(wave_device[1], wavedev, length);

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::startwaverecord(int frequency, int bits, bx_bool stereo, int format)
{
  Bit64u timer_val;
  Bit8u shift = 0;
  int fmt, ret;
  int signeddata = format & 1;

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
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    return alsa_pcm_open(1, frequency, bits, stereo, format);
  }
#endif
  if ((wave_device[1] == NULL) || (strlen(wave_device[1]) < 1))
    return BX_SOUNDLOW_ERR;

  if (wave_fd[1] == -1) {
    wave_fd[1] = open(wave_device[1], O_RDONLY);
    if (wave_fd[1] == -1) {
      return BX_SOUNDLOW_ERR;
    } else {
      BX_INFO(("OSS: opened input device %s", wave_device[1]));
    }
  } else {
    if ((frequency == wave_ch[0].oldfreq) &&
        (bits == wave_ch[0].oldbits) &&
        (stereo == wave_ch[0].oldstereo) &&
        (format == wave_ch[0].oldformat))
      return BX_SOUNDLOW_OK; // nothing to do
  }
  wave_ch[0].oldfreq = frequency;
  wave_ch[0].oldbits = bits;
  wave_ch[0].oldstereo = stereo;
  wave_ch[0].oldformat = format;

  if (bits == 16)
    if (signeddata == 1)
      fmt = AFMT_S16_LE;
    else
      fmt = AFMT_U16_LE;
  else if (bits == 8)
    if (signeddata == 1)
      fmt = AFMT_S8;
    else
      fmt = AFMT_U8;
  else
    return BX_SOUNDLOW_ERR;

      // set frequency etc.
  ret = ioctl(wave_fd[1], SNDCTL_DSP_RESET);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_RESET): %s", strerror(errno)));

  ret = ioctl(wave_fd[1], SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0) {  // abort if the format is unknown, to avoid playing noise
    BX_DEBUG(("ioctl(SNDCTL_DSP_SETFMT, %d): %s",
              fmt, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(wave_fd[1], SNDCTL_DSP_STEREO, &stereo);
  if (ret != 0) {
    BX_DEBUG(("ioctl(SNDCTL_DSP_STEREO, %d): %s",
             stereo, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(wave_fd[1], SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0) {
    BX_DEBUG(("ioctl(SNDCTL_DSP_SPEED, %d): %s",
             frequency, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::getwavepacket(int length, Bit8u data[])
{
  int ret;

#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    if (alsa_pcm[1].buffer == NULL) {
      alsa_pcm[1].buffer = (char *)malloc(alsa_pcm[1].alsa_bufsize);
    }
    while (alsa_pcm[1].audio_bufsize < length) {
      ret = snd_pcm_readi(alsa_pcm[1].handle, alsa_pcm[1].buffer, alsa_pcm[1].frames);
      if (ret == -EAGAIN)
        continue;
      if (ret == -EPIPE) {
        /* EPIPE means overrun */
        BX_ERROR(("overrun occurred"));
        snd_pcm_prepare(alsa_pcm[1].handle);
      } else if (ret < 0) {
        BX_ERROR(("error from read: %s", snd_strerror(ret)));
      } else if (ret != (int)alsa_pcm[1].frames) {
        BX_ERROR(("short read, read %d frames", ret));
      }
      memcpy(audio_buffer[1]+alsa_pcm[1].audio_bufsize, alsa_pcm[1].buffer, alsa_pcm[1].alsa_bufsize);
      alsa_pcm[1].audio_bufsize += alsa_pcm[1].alsa_bufsize;
    }
    memcpy(data, audio_buffer[1], length);
    alsa_pcm[1].audio_bufsize -= length;
    if ((alsa_pcm[1].audio_bufsize <= 0) && (alsa_pcm[1].buffer != NULL)) {
      free(alsa_pcm[1].buffer);
      alsa_pcm[1].buffer = NULL;
    }
    return BX_SOUNDLOW_OK;
  }
#endif
  ret = read(wave_fd[1], data, length);

  if (ret == length) {
    return BX_SOUNDLOW_OK;
  } else {
    BX_ERROR(("OSS: write error"));
    return BX_SOUNDLOW_ERR;
  }
}

int bx_sound_linux_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (alsa_pcm[1].handle != NULL)) {
    snd_pcm_drain(alsa_pcm[1].handle);
    return BX_SOUNDLOW_OK;
  }
#endif
  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::closewaveinput()
{
  stopwaverecord();
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (alsa_pcm[1].handle != NULL)) {
    snd_pcm_drain(alsa_pcm[1].handle);
    snd_pcm_close(alsa_pcm[1].handle);
    alsa_pcm[1].handle = NULL;
    return BX_SOUNDLOW_OK;
  }
#endif
  if (wave_device[1] != NULL)
    delete(wave_device[1]);

  if (wave_fd[1] != -1)
  {
      close(wave_fd[1]);
      wave_fd[1] = -1;
  }
  wave_device[1] = NULL;

  return BX_SOUNDLOW_OK;
}

void bx_sound_linux_c::record_timer_handler(void *this_ptr)
{
  bx_sound_linux_c *class_ptr = (bx_sound_linux_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_sound_linux_c::record_timer(void)
{
  record_handler(this->device, record_packet_size);
}

#endif
