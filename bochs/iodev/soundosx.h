/////////////////////////////////////////////////////////////////////////
// $Id: soundosx.h,v 1.1 2004-02-09 22:23:48 danielg4 Exp $
/////////////////////////////////////////////////////////////////////////

// This file (SOUNDOSX.H) written and donated by Brian Huffman


#ifdef macintosh

#include "bochs.h"

// Do not include OSX sound headers here;
// They define types Float64 and Float32,
// which are already used elsewhere in bochs.
// All OSX-specific types are hidden inside
// soundosx.cc, where nobody else has to see them.

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
    bx_sound_osx_c(bx_sb16_c *sb16);
    BX_SOUND_VIRTUAL ~bx_sound_osx_c();

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
#ifdef BX_SOUND_OSX_use_converter
    void nextbuffer(int *outDataSize, void **outData);
#endif

private:
    bx_sb16_c *sb16;

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
