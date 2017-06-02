/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2015  The Bochs Project
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

// Support for sound output to files (based on SB16 code)


class bx_soundlow_waveout_file_c : public bx_soundlow_waveout_c {
public:
  bx_soundlow_waveout_file_c();
  virtual ~bx_soundlow_waveout_file_c() {}

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);
  virtual int output(int length, Bit8u data[]);
  virtual int closewaveoutput();

private:
  void initwavfile();
  void write_32bit(Bit32u pos, Bit32u value);

  void VOC_init_file();
  void VOC_write_block(int block, Bit32u headerlen, Bit8u header[],
                       Bit32u datalen, Bit8u data[]);

  FILE *wavefile;
  unsigned type;
};

class bx_soundlow_midiout_file_c : public bx_soundlow_midiout_c {
public:
  bx_soundlow_midiout_file_c();
  virtual ~bx_soundlow_midiout_file_c();

  virtual int openmidioutput(const char *mididev);
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();

private:
  void writedeltatime(Bit32u deltatime);

  FILE *midifile;
  unsigned type;
};

class bx_sound_file_c : public bx_sound_lowlevel_c {
public:
  bx_sound_file_c() : bx_sound_lowlevel_c("file") {}
  virtual ~bx_sound_file_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
  virtual bx_soundlow_midiout_c* get_midiout();
} bx_sound_file;
