/////////////////////////////////////////////////////////////////////////
// $Id: soundmod.h,v 1.2 2011-02-10 22:58:22 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011  The Bochs Project
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


// this is the size of a DMA chunk sent to output
// it should not be too large to avoid lag, and not too
// small to avoid unnecessary overhead.
#define BX_SOUND_OUTPUT_WAVEPACKETSIZE  8192

/* Definitions for the output functions */
#define BX_SOUND_OUTPUT_OK   0
#define BX_SOUND_OUTPUT_ERR  1

// The class with the output functions
class bx_sound_output_c : public logfunctions {
public:

  /*
  These functions are the sound output functions, sending
  the music/sound to the OS specific driver.
  They are in a different file (soundxxx.cc) because they are
  non-portable, while everything in sb16.cc is portable
  */

  bx_sound_output_c(logfunctions *dev);
  virtual ~bx_sound_output_c();

  virtual int waveready();
  virtual int midiready();

  virtual int openmidioutput(char *mididev);
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();

  virtual int openwaveoutput(char *wavedev);
  virtual int startwaveplayback(int frequency, int bits, int stereo, int format);
  virtual int sendwavepacket(int length, Bit8u data[]);
  virtual int stopwaveplayback();
  virtual int closewaveoutput();
protected:
  logfunctions *device;
};
