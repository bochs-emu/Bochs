/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2015  The Bochs Project
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

// This file (SOUNDOSX.H) written and donated by Brian Huffman


#if BX_HAVE_SOUND_OSX

#include "bochs.h"

// uncomment one of these two:
#if BX_WITH_MACOS
#define BX_SOUND_OSX_use_quicktime
#else
#define BX_SOUND_OSX_use_converter
//#define BX_SOUND_OSX_use_quicktime
#include <CoreServices/CoreServices.h>
#endif

#define BX_SOUND_OSX_NBUF     8   // number of buffers for digital output

class bx_soundlow_waveout_osx_c : public bx_soundlow_waveout_c {
public:
  bx_soundlow_waveout_osx_c();
  virtual ~bx_soundlow_waveout_osx_c();

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);
  virtual int output(int length, Bit8u data[]);
#ifdef BX_SOUND_OSX_use_converter
  void nextbuffer(int *outDataSize, void **outData);
#endif
private:
  int WaveOpen;

  Bit8u WaveData[BX_SOUND_OSX_NBUF][BX_SOUNDLOW_WAVEPACKETSIZE];
  int WaveLength[BX_SOUND_OSX_NBUF];
  int head, tail;  // buffer pointers

#ifdef BX_SOUND_OSX_use_converter
  int WavePlaying;

  OSStatus core_audio_pause();
  OSStatus core_audio_resume();
#endif
};

class bx_soundlow_midiout_osx_c : public bx_soundlow_midiout_c {
public:
  bx_soundlow_midiout_osx_c();
  virtual ~bx_soundlow_midiout_osx_c();

  virtual int openmidioutput(const char *mididev);
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();

private:
  int MidiOpen;
};

class bx_sound_osx_c : public bx_sound_lowlevel_c {
public:
  bx_sound_osx_c() : bx_sound_lowlevel_c("osx") {}
  virtual ~bx_sound_osx_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
  virtual bx_soundlow_midiout_c* get_midiout();
} bx_sound_osx;

#endif  // macintosh
