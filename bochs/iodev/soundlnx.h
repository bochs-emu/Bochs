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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

// This file (SOUNDLNX.H) written and donated by Josef Drexler


#if defined(linux)

#include "bochs.h"

#define BX_SOUND_LINUX_BUFSIZE   BX_SOUND_OUTPUT_WAVEPACKETSIZE

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
  bx_sb16_c *sb16;
  FILE *midi;
  char *wavedevice;
  int wave;
  int bufferpos;
  Bit8u audio_buffer[BX_SOUND_LINUX_BUFSIZE];
  int oldfreq,oldbits,oldstereo,oldformat;
};

#endif  // defined(linux)
