/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2017  The Bochs Project
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
// for Linux using OSS. The current version also supports OSS on FreeBSD.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "soundlow.h"
#include "soundmod.h"
#include "soundoss.h"

#if BX_HAVE_SOUND_OSS && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

// sound driver plugin entry points

int CDECL liboss_sound_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL liboss_sound_plugin_fini(void)
{
  // Nothing here yet
}

// bx_soundlow_waveout_oss_c class implementation

bx_soundlow_waveout_oss_c::bx_soundlow_waveout_oss_c()
    :bx_soundlow_waveout_c()
{
  waveout_fd = -1;
}

bx_soundlow_waveout_oss_c::~bx_soundlow_waveout_oss_c()
{
  if (waveout_fd != -1) {
    close(waveout_fd);
    waveout_fd = -1;
  }
}

int bx_soundlow_waveout_oss_c::openwaveoutput(const char *wavedev)
{
  if (waveout_fd == -1) {
    waveout_fd = open(wavedev, O_WRONLY);
    if (waveout_fd == -1) {
      return BX_SOUNDLOW_ERR;
    } else {
      BX_INFO(("OSS: opened output device %s", wavedev));
    }
  }
  set_pcm_params(&real_pcm_param);
  pcm_callback_id = register_wave_callback(this, pcm_callback);
  start_resampler_thread();
  start_mixer_thread();
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_oss_c::set_pcm_params(bx_pcm_param_t *param)
{
  int fmt, ret;
  int frequency = param->samplerate;
  int channels = param->channels;
  int signeddata = param->format & 1;

  BX_DEBUG(("set_pcm_params(): %u, %u, %u, %02x", param->samplerate, param->bits,
            param->channels, param->format));

  if (waveout_fd == -1) {
    return BX_SOUNDLOW_ERR;
  }
  if (param->bits == 16) {
    if (signeddata == 1) {
      fmt = AFMT_S16_LE;
    } else {
      fmt = AFMT_U16_LE;
    }
  } else if (param->bits == 8) {
    if (signeddata == 1) {
      fmt = AFMT_S8;
    } else {
      fmt = AFMT_U8;
    }
  } else {
    return BX_SOUNDLOW_ERR;
  }
  // set frequency etc.
  ret = ioctl(waveout_fd, SNDCTL_DSP_RESET);
  if (ret != 0)
    BX_ERROR(("ioctl(SNDCTL_DSP_RESET): %s", strerror(errno)));

  /*
  ret = ioctl(waveout_fd], SNDCTL_DSP_SETFRAGMENT, &fragment);
  if (ret != 0)
    BX_DEBUG(("ioctl(SNDCTL_DSP_SETFRAGMENT, %d): %s",
             fragment, strerror(errno)));
  */

  ret = ioctl(waveout_fd, SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0) { // abort if the format is unknown, to avoid playing noise
    BX_ERROR(("ioctl(SNDCTL_DSP_SETFMT, %d): %s",
              fmt, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(waveout_fd, SNDCTL_DSP_CHANNELS, &channels);
  if (ret != 0)
    BX_ERROR(("ioctl(SNDCTL_DSP_CHANNELS, %d): %s",
              channels, strerror(errno)));

  ret = ioctl(waveout_fd, SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0)
    BX_ERROR(("ioctl(SNDCTL_DSP_SPEED, %d): %s",
              frequency, strerror(errno)));

  // ioctl(wave_fd[0], SNDCTL_DSP_GETBLKSIZE, &fragment);
  // BX_DEBUG(("current output block size is %d", fragment));

  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_oss_c::output(int length, Bit8u data[])
{
  int odelay, delay;

  if (waveout_fd == -1) {
    return BX_SOUNDLOW_ERR;
  }
  if (write(waveout_fd, data, length) == length) {
    ioctl(waveout_fd, SNDCTL_DSP_GETODELAY, &odelay);
    delay = odelay * 1000 / (real_pcm_param.samplerate * 4);
    BX_MSLEEP(delay);
    return BX_SOUNDLOW_OK;
  } else {
    return BX_SOUNDLOW_ERR;
  }
}

// bx_soundlow_wavein_oss_c class implementation

bx_soundlow_wavein_oss_c::bx_soundlow_wavein_oss_c()
    :bx_soundlow_wavein_c()
{
  wavein_fd = -1;
}

bx_soundlow_wavein_oss_c::~bx_soundlow_wavein_oss_c()
{
  if (wavein_fd != -1) {
    close(wavein_fd);
    wavein_fd = -1;
  }
}

int bx_soundlow_wavein_oss_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = DEV_register_timer(this, record_timer_handler, 1, 1, 0, "soundlnx");
    // record timer: inactive, continuous, frequency variable
  }
  if (wavein_fd == -1) {
    wavein_fd = open(wavedev, O_RDONLY);
    if (wavein_fd == -1) {
      return BX_SOUNDLOW_ERR;
    } else {
      BX_INFO(("OSS: opened input device %s", wavedev));
    }
  }
  wavein_param.samplerate = 0;
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_oss_c::startwaverecord(bx_pcm_param_t *param)
{
  Bit64u timer_val;
  Bit8u shift = 0;
  int fmt, ret;
  int frequency = param->samplerate;
  int channels = param->channels;
  int signeddata = param->format & 1;

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
  if (wavein_fd == -1) {
    return BX_SOUNDLOW_ERR;
  } else {
    if (memcmp(param, &wavein_param, sizeof(bx_pcm_param_t)) == 0) {
      return BX_SOUNDLOW_OK; // nothing to do
    }
    wavein_param = *param;
  }

  if (param->bits == 16) {
    if (signeddata == 1) {
      fmt = AFMT_S16_LE;
    } else {
      fmt = AFMT_U16_LE;
    }
  } else if (param->bits == 8) {
    if (signeddata == 1) {
      fmt = AFMT_S8;
    } else {
      fmt = AFMT_U8;
    }
  } else {
    return BX_SOUNDLOW_ERR;
  }

      // set frequency etc.
  ret = ioctl(wavein_fd, SNDCTL_DSP_RESET);
  if (ret != 0)
    BX_ERROR(("ioctl(SNDCTL_DSP_RESET): %s", strerror(errno)));

  ret = ioctl(wavein_fd, SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0) {  // abort if the format is unknown, to avoid playing noise
    BX_ERROR(("ioctl(SNDCTL_DSP_SETFMT, %d): %s",
              fmt, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(wavein_fd, SNDCTL_DSP_CHANNELS, &channels);
  if (ret != 0) {
    BX_ERROR(("ioctl(SNDCTL_DSP_CHANNELS, %d): %s",
              channels, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  ret = ioctl(wavein_fd, SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0) {
    BX_ERROR(("ioctl(SNDCTL_DSP_SPEED, %d): %s",
              frequency, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_oss_c::getwavepacket(int length, Bit8u data[])
{
  int ret;

  ret = read(wavein_fd, data, length);

  if (ret == length) {
    return BX_SOUNDLOW_OK;
  } else {
    BX_ERROR(("OSS: write error"));
    return BX_SOUNDLOW_ERR;
  }
}

int bx_soundlow_wavein_oss_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_oss_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_oss_c *class_ptr = (bx_soundlow_wavein_oss_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_oss_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

// bx_soundlow_midiout_oss_c class implementation

bx_soundlow_midiout_oss_c::bx_soundlow_midiout_oss_c()
    :bx_soundlow_midiout_c()
{
  midi = NULL;
}

bx_soundlow_midiout_oss_c::~bx_soundlow_midiout_oss_c()
{
  closemidioutput();
}

int bx_soundlow_midiout_oss_c::openmidioutput(const char *mididev)
{
  if ((mididev == NULL) || (strlen(mididev) < 1))
    return BX_SOUNDLOW_ERR;

  midi = fopen(mididev,"w");

  if (midi == NULL) {
    BX_ERROR(("Couldn't open midi output device %s: %s",
             mididev, strerror(errno)));
    return BX_SOUNDLOW_ERR;
  }

  return BX_SOUNDLOW_OK;
}


int bx_soundlow_midiout_oss_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);

  fputc(command, midi);
  fwrite(data, 1, length, midi);
  fflush(midi);       // to start playing immediately

  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_oss_c::closemidioutput()
{
  if (midi != NULL) {
    fclose(midi);
    midi = NULL;
  }
  return BX_SOUNDLOW_OK;
}

// bx_sound_oss_c class implementation

bx_soundlow_waveout_c* bx_sound_oss_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_oss_c();
  }
  return waveout;
}

bx_soundlow_wavein_c* bx_sound_oss_c::get_wavein()
{
  if (wavein == NULL) {
    wavein = new bx_soundlow_wavein_oss_c();
  }
  return wavein;
}

bx_soundlow_midiout_c* bx_sound_oss_c::get_midiout()
{
  if (midiout == NULL) {
    midiout = new bx_soundlow_midiout_oss_c();
  }
  return midiout;
}

#endif
