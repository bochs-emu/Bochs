/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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

#include "iodev.h"
#include "soundmod.h"
#include "soundlnx.h"

#if (defined(linux) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__)) && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

bx_sound_linux_c::bx_sound_linux_c()
  :bx_sound_lowlevel_c()
{
  midi = NULL;
  wave_device[0] = NULL;
  wave_device[1] = NULL;
  wave_fd[0] = -1;
  wave_fd[1] = -1;
  BX_INFO(("Sound lowlevel module 'oss' initialized"));
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

int bx_sound_linux_c::openmidioutput(const char *mididev)
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


int bx_sound_linux_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);

  fputc(command, midi);
  fwrite(data, 1, length, midi);
  fflush(midi);       // to start playing immediately

  return BX_SOUNDLOW_OK;
}


int bx_sound_linux_c::closemidioutput()
{
  fclose(midi);

  return BX_SOUNDLOW_OK;
}


int bx_sound_linux_c::openwaveoutput(const char *wavedev)
{
  int length = strlen(wavedev) + 1;

  if (wave_device[0] != NULL)
    delete [] wave_device[0];

  wave_device[0] = new char[length];

  if (wave_device[0] == NULL)
    return BX_SOUNDLOW_ERR;

  strncpy(wave_device[0], wavedev, length);

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  int fmt, ret;
  int signeddata = format & 1;

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

int bx_sound_linux_c::sendwavepacket(int length, Bit8u data[])
{
  if (wave_fd[0] == -1) {
    return BX_SOUNDLOW_ERR;
  }

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
  // ioctl(wave_fd[0], SNDCTL_DSP_SYNC);
  // close(wave_fd[0]);
  // wave_fd[0] = -1;

  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::closewaveoutput()
{
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
  return BX_SOUNDLOW_OK;
}

int bx_sound_linux_c::closewaveinput()
{
  stopwaverecord();
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
  record_handler(this, record_packet_size);
}

#endif
