//  Copyright (C) 2000  MandrakeSoft S.A.
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// This file (SOUNDLNX.CC) written and donated by Josef Drexler

#if defined(linux)

#include "bochs.h"

#include <errno.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

bx_sound_linux_c::bx_sound_linux_c(bx_sb16_c *sb16)
  :bx_sound_output_c(sb16)
{
  this->sb16 = sb16;
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

int bx_sound_linux_c::openmidioutput(char *device)
{
  if ( (device == NULL) || (strlen(device) < 1) )
    return BX_SOUND_OUTPUT_ERR;

  midi = fopen(device,"w");

  if (midi == NULL)
    {
      WRITELOG( MIDILOG(2), "Couldn't open midi output device %s: %s.",
		device, strerror(errno));
      return BX_SOUND_OUTPUT_ERR;
    }

  return BX_SOUND_OUTPUT_OK;
}


int bx_sound_linux_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);
  //  bx_panic("Sendmidicommand!!");

  fputc(command, midi);
  fwrite(data, 1, length, midi);
  fflush(midi);       // to start playing immediately

  return BX_SOUND_OUTPUT_OK;
}


int bx_sound_linux_c::closemidioutput()
{
  fclose(midi);

  return BX_SOUND_OUTPUT_OK;
}


int bx_sound_linux_c::openwaveoutput(char *device)
{
  int length = strlen(device) + 1;

  if (wavedevice != NULL)
    delete(wavedevice);

  wavedevice = new char[length];

  if (wavedevice == NULL)
    return BX_SOUND_OUTPUT_ERR;

  strncpy(wavedevice, device, length);

  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_linux_c::startwaveplayback(int frequency, int bits, int stereo, int format)
{
  int fmt, ret;
  int signeddata = format & 1;

  if ( (wavedevice == NULL) || (strlen(wavedevice) < 1) )
    return BX_SOUND_OUTPUT_ERR;

  if (wave == -1)
    wave = open(wavedevice, O_WRONLY);
  else
    if ( (frequency == oldfreq) &&
	 (bits == oldbits) &&
	 (stereo == oldstereo) &&
	 (format == oldformat) )
      return BX_SOUND_OUTPUT_OK;    // nothing to do

  oldfreq = frequency;
  oldbits = bits;
  oldstereo = stereo;
  oldformat = format;

  if (wave == -1)
    return BX_SOUND_OUTPUT_ERR;

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
    WRITELOG( WAVELOG(4), "ioctl(SNDCTL_DSP_RESET): %s", strerror(errno));

  /*
  ret = ioctl(wave, SNDCTL_DSP_SETFRAGMENT, &fragment);
  if (ret != 0)
    WRITELOG( WAVELOG(4), "ioctl(SNDCTL_DSP_SETFRAGMENT, %d): %s",
	      fragment, strerror(errno));
  */

  ret = ioctl(wave, SNDCTL_DSP_SETFMT, &fmt);
  if (ret != 0)   // abort if the format is unknown, to avoid playing noise
    {
      WRITELOG( WAVELOG(4), "ioctl(SNDCTL_DSP_SETFMT, %d): %s",
		fmt, strerror(errno));
      return BX_SOUND_OUTPUT_ERR;
    }

  ret = ioctl(wave, SNDCTL_DSP_STEREO, &stereo);
  if (ret != 0)
    WRITELOG( WAVELOG(4), "ioctl(SNDCTL_DSP_STEREO, %d): %s", 
	      stereo, strerror(errno));

  ret = ioctl(wave, SNDCTL_DSP_SPEED, &frequency);
  if (ret != 0)
    WRITELOG( WAVELOG(4), "ioctl(SNDCTL_DSP_SPEED, %d): %s", 
	      frequency, strerror(errno));

  //  ioctl(wave, SNDCTL_DSP_GETBLKSIZE, &fragment);
  //  WRITELOG( WAVELOG(4), "current output block size is %d", fragment);

  return BX_SOUND_OUTPUT_OK;
}
			 
int bx_sound_linux_c::sendwavepacket(int length, Bit8u data[])
{
  int ret;

  ret = write(wave, data, length);

  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_linux_c::stopwaveplayback()
{
  //  ioctl(wave, SNDCTL_DSP_SYNC);
  //  close(wave);
  //  wave = -1;

  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_linux_c::closewaveoutput()
{
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

#endif  // defined(linux)
