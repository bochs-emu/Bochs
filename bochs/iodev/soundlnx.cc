/////////////////////////////////////////////////////////////////////////
// $Id: soundlnx.cc,v 1.20 2009-02-08 09:05:52 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
// ALSA PCM output on Linux.

#define NO_DEVICE_INCLUDES
#include "iodev.h"
#define BX_SOUNDLOW
#include "sb16.h"

#if (defined(linux) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)) && BX_SUPPORT_SB16

#define LOG_THIS sb16->

#include "soundlnx.h"

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

bx_sound_linux_c::bx_sound_linux_c(bx_sb16_c *sb16)
  :bx_sound_output_c(sb16)
{
  this->sb16 = sb16;
#if BX_HAVE_ALSASOUND
  alsa_seq.handle = NULL;
  alsa_pcm.handle = NULL;
  alsa_buffer = NULL;
#endif
  midi = NULL;
  wavedevice = NULL;
  wave = -1;
}

bx_sound_linux_c::~bx_sound_linux_c()
{
  // nothing for now
}


int bx_sound_linux_c::waveready()
{
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_linux_c::midiready()
{
  return BX_SOUND_OUTPUT_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_seq_open(char *device)
{
  char *mididev, *ptr;
  int client, port, ret = 0;
  int length = strlen(device) + 1;

  mididev = new char[length];

  if (mididev == NULL)
    return BX_SOUND_OUTPUT_ERR;

  strcpy(mididev, device);
  ptr = strtok(mididev, ":");
  if (ptr == NULL) {
    WRITELOG(MIDILOG(2), "ALSA sequencer setup: missing client parameters");
    return BX_SOUND_OUTPUT_ERR;
  }
  client = atoi(ptr);
  ptr = strtok(NULL, ":");
  if (ptr == NULL) {
    WRITELOG(MIDILOG(2), "ALSA sequencer setup: missing port parameter");
    return BX_SOUND_OUTPUT_ERR;
  }
  port = atoi(ptr);

  delete(mididev);

  if (snd_seq_open(&alsa_seq.handle, "default", SND_SEQ_OPEN_OUTPUT, 0) < 0) {
    WRITELOG(MIDILOG(2), "Couldn't open ALSA sequencer for midi output");
    return BX_SOUND_OUTPUT_ERR;
  }
  ret = snd_seq_create_simple_port(alsa_seq.handle, NULL,
    SND_SEQ_PORT_CAP_WRITE |
    SND_SEQ_PORT_CAP_SUBS_WRITE |
    SND_SEQ_PORT_CAP_READ,
    SND_SEQ_PORT_TYPE_MIDI_GENERIC);
  if (ret < 0) {
    WRITELOG(MIDILOG(2), "ALSA sequencer: error creating port %s\n", snd_strerror(errno));
  } else {
    alsa_seq.source_port = ret;
    ret = snd_seq_connect_to(alsa_seq.handle, alsa_seq.source_port, client, port);
    if (ret < 0) {
      WRITELOG(MIDILOG(2), "ALSA sequencer: could not connect to port %d:%d\n", client, port);
    }
  }
  if (ret < 0) {
    snd_seq_close(alsa_seq.handle);
    return BX_SOUND_OUTPUT_ERR;
  } else {
    return BX_SOUND_OUTPUT_OK;
  }
}
#endif

int bx_sound_linux_c::openmidioutput(char *device)
{
  if ((device == NULL) || (strlen(device) < 1))
    return BX_SOUND_OUTPUT_ERR;

#if BX_HAVE_ALSASOUND
  use_alsa_seq = !strncmp(device, "alsa:", 5);
  if (use_alsa_seq) {
    return alsa_seq_open(device+5);
  }
#endif

  midi = fopen(device,"w");

  if (midi == NULL)
  {
      WRITELOG(MIDILOG(2), "Couldn't open midi output device %s: %s.",
                device, strerror(errno));
      return BX_SOUND_OUTPUT_ERR;
  }

  return BX_SOUND_OUTPUT_OK;
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
      WRITELOG(MIDILOG(3), "alsa_seq_output(): SYSEX not implemented, length=%d", length);
      return BX_SOUND_OUTPUT_ERR;
    default:
      WRITELOG(MIDILOG(3), "alsa_seq_output(): unknown command 0x%02x, length=%d", command, length);
      return BX_SOUND_OUTPUT_ERR;
  }
  snd_seq_event_output(alsa_seq.handle, &ev);
  snd_seq_drain_output(alsa_seq.handle);
  return BX_SOUND_OUTPUT_OK;
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

  return BX_SOUND_OUTPUT_OK;
}


int bx_sound_linux_c::closemidioutput()
{
#if BX_HAVE_ALSASOUND
  if ((use_alsa_seq) && (alsa_seq.handle != NULL)) {
    snd_seq_close(alsa_seq.handle);
    return BX_SOUND_OUTPUT_OK;
  }
#endif
  fclose(midi);

  return BX_SOUND_OUTPUT_OK;
}


int bx_sound_linux_c::openwaveoutput(char *device)
{
#if BX_HAVE_ALSASOUND
  use_alsa_pcm = !strcmp(device, "alsa");
  if (use_alsa_pcm) {
    return BX_SOUND_OUTPUT_OK;
  }
#endif
  int length = strlen(device) + 1;

  if (wavedevice != NULL)
    delete(wavedevice);

  wavedevice = new char[length];

  if (wavedevice == NULL)
    return BX_SOUND_OUTPUT_ERR;

  strncpy(wavedevice, device, length);

  return BX_SOUND_OUTPUT_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_pcm_open(int frequency, int bits, int stereo, int format)
{
  int ret;
  snd_pcm_format_t fmt;
  snd_pcm_hw_params_t *params;
  unsigned int size, freq;
  int signeddata = format & 1;

  audio_bufsize = 0;

  if (alsa_pcm.handle == NULL) {
    ret = snd_pcm_open(&alsa_pcm.handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
      return BX_SOUND_OUTPUT_ERR;
    }
    WRITELOG(WAVELOG(1), "ALSA: opened default PCM output device");
  }
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(alsa_pcm.handle, params);
  snd_pcm_hw_params_set_access(alsa_pcm.handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

  if ((frequency == oldfreq) &&
      (bits == oldbits) &&
      (stereo == oldstereo) &&
      (format == oldformat))
    return BX_SOUND_OUTPUT_OK; // nothing to do

  oldfreq = frequency;
  oldbits = bits;
  oldstereo = stereo;
  oldformat = format;

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
    return BX_SOUND_OUTPUT_ERR;

  if (stereo) size *= 2;

  snd_pcm_hw_params_set_format(alsa_pcm.handle, params, fmt);
  snd_pcm_hw_params_set_channels(alsa_pcm.handle, params, (stereo != 0) ? 2 : 1);
  snd_pcm_hw_params_set_rate_near(alsa_pcm.handle, params, &freq, &dir);

  alsa_pcm.frames = 32;
  snd_pcm_hw_params_set_period_size_near(alsa_pcm.handle, params, &alsa_pcm.frames, &dir);

  ret = snd_pcm_hw_params(alsa_pcm.handle, params);
  if (ret < 0) {
    return BX_SOUND_OUTPUT_ERR;
  }
  snd_pcm_hw_params_get_period_size(params, &alsa_pcm.frames, &dir);
  alsa_bufsize = alsa_pcm.frames * size;
  WRITELOG(WAVELOG(4), "ALSA: buffer size set to %d", alsa_bufsize);
  if (alsa_buffer != NULL) {
    free(alsa_buffer);
    alsa_buffer = NULL;
  }

  return BX_SOUND_OUTPUT_OK;
}
#endif

int bx_sound_linux_c::startwaveplayback(int frequency, int bits, int stereo, int format)
{
  int fmt, ret;
  int signeddata = format & 1;

#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    return alsa_pcm_open(frequency, bits, stereo, format);
  }
#endif
  if ((wavedevice == NULL) || (strlen(wavedevice) < 1))
    return BX_SOUND_OUTPUT_ERR;

  if (wave == -1) {
    wave = open(wavedevice, O_WRONLY);
    if (wave == -1) {
      return BX_SOUND_OUTPUT_ERR;
    } else {
      WRITELOG(WAVELOG(1), "OSS: opened output device %s", wavedevice);
    }
  } else {
    if ((frequency == oldfreq) &&
        (bits == oldbits) &&
        (stereo == oldstereo) &&
        (format == oldformat))
      return BX_SOUND_OUTPUT_OK; // nothing to do
  }
  oldfreq = frequency;
  oldbits = bits;
  oldstereo = stereo;
  oldformat = format;

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
    return BX_SOUND_OUTPUT_ERR;

      // set frequency etc.
  ret = ioctl(wave, SNDCTL_DSP_RESET);
  if (ret != 0)
    WRITELOG(WAVELOG(4), "ioctl(SNDCTL_DSP_RESET): %s", strerror(errno));

  /*
  ret = ioctl(wave, SNDCTL_DSP_SETFRAGMENT, &fragment);
  if (ret != 0)
    WRITELOG(WAVELOG(4), "ioctl(SNDCTL_DSP_SETFRAGMENT, %d): %s",
             fragment, strerror(errno));
  */

  ret = ioctl(wave, SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0)   // abort if the format is unknown, to avoid playing noise
  {
      WRITELOG(WAVELOG(4), "ioctl(SNDCTL_DSP_SETFMT, %d): %s",
               fmt, strerror(errno));
      return BX_SOUND_OUTPUT_ERR;
  }

  ret = ioctl(wave, SNDCTL_DSP_STEREO, &stereo);
  if (ret != 0)
    WRITELOG(WAVELOG(4), "ioctl(SNDCTL_DSP_STEREO, %d): %s",
             stereo, strerror(errno));

  ret = ioctl(wave, SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0)
    WRITELOG(WAVELOG(4), "ioctl(SNDCTL_DSP_SPEED, %d): %s",
             frequency, strerror(errno));

  // ioctl(wave, SNDCTL_DSP_GETBLKSIZE, &fragment);
  // WRITELOG(WAVELOG(4), "current output block size is %d", fragment);

  return BX_SOUND_OUTPUT_OK;
}

#if BX_HAVE_ALSASOUND
int bx_sound_linux_c::alsa_pcm_write()
{
  int ret;

  if (alsa_buffer == NULL) {
    alsa_buffer = (char *)malloc(alsa_bufsize);
  }
  while (audio_bufsize >= alsa_bufsize) {
    memcpy(alsa_buffer, audio_buffer, alsa_bufsize);
    ret = snd_pcm_writei(alsa_pcm.handle, alsa_buffer, alsa_pcm.frames);
    if (ret == -EPIPE) {
      /* EPIPE means underrun */
      WRITELOG(WAVELOG(3), "ALSA: underrun occurred");
      snd_pcm_prepare(alsa_pcm.handle);
    } else if (ret < 0) {
      WRITELOG(WAVELOG(3), "ALSA: error from writei: %s", snd_strerror(ret));
    }  else if (ret != (int)alsa_pcm.frames) {
      WRITELOG(WAVELOG(3), "ALSA: short write, write %d frames", ret);
    }
    audio_bufsize -= alsa_bufsize;
    memcpy(audio_buffer, audio_buffer+alsa_bufsize, audio_bufsize);
  }
  if ((audio_bufsize == 0) && (alsa_buffer != NULL)) {
    free(alsa_buffer);
    alsa_buffer = NULL;
  }

  return BX_SOUND_OUTPUT_OK;
}
#endif

int bx_sound_linux_c::sendwavepacket(int length, Bit8u data[])
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm) {
    if ((audio_bufsize+length) <= BX_SOUND_LINUX_BUFSIZE) {
      memcpy(audio_buffer+audio_bufsize, data, length);
      audio_bufsize += length;
    } else {
      WRITELOG(WAVELOG(3), "ALSA: audio buffer overflow");
      return BX_SOUND_OUTPUT_ERR;
    }
    if (audio_bufsize < alsa_bufsize) {
      return BX_SOUND_OUTPUT_OK;
    } else {
      return alsa_pcm_write();
    }
  }
#endif
  int ret = write(wave, data, length);

  if (ret == length) {
    return BX_SOUND_OUTPUT_OK;
  } else {
    WRITELOG(WAVELOG(3), "OSS: write error");
    return BX_SOUND_OUTPUT_ERR;
  }
}

int bx_sound_linux_c::stopwaveplayback()
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (audio_bufsize > 0)) {
    if (audio_bufsize < alsa_bufsize) {
      memset(audio_buffer+audio_bufsize, 0, alsa_bufsize-audio_bufsize);
      audio_bufsize = alsa_bufsize;
    }
    alsa_pcm_write();
  }
#endif
  // ioctl(wave, SNDCTL_DSP_SYNC);
  // close(wave);
  // wave = -1;

  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_linux_c::closewaveoutput()
{
#if BX_HAVE_ALSASOUND
  if (use_alsa_pcm && (alsa_pcm.handle != NULL)) {
    snd_pcm_drain(alsa_pcm.handle);
    snd_pcm_close(alsa_pcm.handle);
    alsa_pcm.handle = NULL;
  }
#endif
  if (wavedevice != NULL)
    delete(wavedevice);

  if (wave != -1)
  {
      close(wave);
      wave = -1;
  }
  wavedevice = NULL;

  return BX_SOUND_OUTPUT_OK;
}

#endif
