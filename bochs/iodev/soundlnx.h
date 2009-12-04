/////////////////////////////////////////////////////////////////////////
// $Id: soundlnx.h,v 1.14 2009-12-04 19:50:29 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
//
/////////////////////////////////////////////////////////////////////////

// Josef Drexler coded the original version of the lowlevel sound support
// for Linux using OSS. The current version also supports OSS on FreeBSD and
// ALSA PCM output on Linux.

#if (defined(linux) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))

#define BX_SOUND_LINUX_BUFSIZE   BX_SOUND_OUTPUT_WAVEPACKETSIZE * 2

#if BX_HAVE_ALSASOUND
#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#endif

class bx_sound_linux_c : public bx_sound_output_c {
public:
  bx_sound_linux_c(bx_sb16_c *sb16);
  BX_SOUND_VIRTUAL ~bx_sound_linux_c();

  // if virtual functions are used, we have to override them
  // and define our own. Otherwise this file will just implement
  // the original functions
#ifdef BX_USE_SOUND_VIRTUAL
  BX_SOUND_VIRTUAL int    waveready();
  BX_SOUND_VIRTUAL int    midiready();

  BX_SOUND_VIRTUAL int    openmidioutput(char *device);
  BX_SOUND_VIRTUAL int    sendmidicommand(int delta, int command, int length, Bit8u data[]);
  BX_SOUND_VIRTUAL int    closemidioutput();

  BX_SOUND_VIRTUAL int    openwaveoutput(char *device);
  BX_SOUND_VIRTUAL int    startwaveplayback(int frequency, int bits, int stereo, int format);
  BX_SOUND_VIRTUAL int    sendwavepacket(int length, Bit8u data[]);
  BX_SOUND_VIRTUAL int    stopwaveplayback();
  BX_SOUND_VIRTUAL int    closewaveoutput();
#endif

private:
#if BX_HAVE_ALSASOUND
  int alsa_seq_open(char *device);
  int alsa_seq_output(int delta, int command, int length, Bit8u data[]);
  int alsa_pcm_open(int frequency, int bits, int stereo, int format);
  int alsa_pcm_write();
#endif
  bx_sb16_c *sb16;
#if BX_HAVE_ALSASOUND
  bx_bool use_alsa_seq;
  bx_bool use_alsa_pcm;
  struct {
    snd_seq_t *handle;
    int source_port;
  } alsa_seq;
  struct {
    snd_pcm_t *handle;
    snd_pcm_uframes_t frames;
  } alsa_pcm;
  int dir, alsa_bufsize, audio_bufsize;
  char *alsa_buffer;
#endif
  FILE *midi;
  char *wavedevice;
  int wave;
  Bit8u audio_buffer[BX_SOUND_LINUX_BUFSIZE];
  int oldfreq,oldbits,oldstereo,oldformat;
};

#endif
