/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2011  The Bochs Project
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


#ifdef macintosh

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

class bx_sound_osx_c : public bx_sound_lowlevel_c {
public:
    bx_sound_osx_c();
    virtual ~bx_sound_osx_c();

    virtual int get_type() {return BX_SOUNDLOW_OSX;}

    virtual int    waveready();
    virtual int    midiready();

    virtual int    openmidioutput(const char *mididev);
    virtual int    sendmidicommand(int delta, int command, int length, Bit8u data[]);
    virtual int    closemidioutput();

    virtual int    openwaveoutput(const char *wavedev);
    virtual int    startwaveplayback(int frequency, int bits, bx_bool stereo, int format);
    virtual int    sendwavepacket(int length, Bit8u data[]);
    virtual int    stopwaveplayback();
    virtual int    closewaveoutput();
#ifdef BX_SOUND_OSX_use_converter
    void nextbuffer(int *outDataSize, void **outData);
#endif

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(int frequency, int bits, bx_bool stereo, int format);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();
  virtual int closewaveinput();

  static void record_timer_handler(void *);
  void record_timer(void);
private:
    int MidiOpen;
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

#endif  // macintosh
