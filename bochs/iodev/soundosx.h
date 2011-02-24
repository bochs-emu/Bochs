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
#endif

#define BX_SOUND_OSX_NBUF     8   // number of buffers for digital output

class bx_sound_osx_c : public bx_sound_output_c {
public:
    bx_sound_osx_c(logfunctions *dev);
    virtual ~bx_sound_osx_c();

    virtual int    waveready();
    virtual int    midiready();

    virtual int    openmidioutput(const char *mididev);
    virtual int    sendmidicommand(int delta, int command, int length, Bit8u data[]);
    virtual int    closemidioutput();

    virtual int    openwaveoutput(const char *wavedev);
    virtual int    startwaveplayback(int frequency, int bits, int stereo, int format);
    virtual int    sendwavepacket(int length, Bit8u data[]);
    virtual int    stopwaveplayback();
    virtual int    closewaveoutput();
#ifdef BX_SOUND_OSX_use_converter
    void nextbuffer(int *outDataSize, void **outData);
#endif

private:
    int MidiOpen;
    int WaveOpen;

    Bit8u WaveData[BX_SOUND_OSX_NBUF][BX_SOUND_OUTPUT_WAVEPACKETSIZE];
    int WaveLength[BX_SOUND_OSX_NBUF];
    int head, tail;  // buffer pointers

#ifdef BX_SOUND_OSX_use_converter
    int WavePlaying;
#endif
};

#endif  // macintosh
