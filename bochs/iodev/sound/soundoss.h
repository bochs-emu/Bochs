/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2017 The Bochs Project
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
// for Linux using OSS. The current version also supports OSS on FreeBSD.

#if BX_HAVE_SOUND_OSS

class bx_soundlow_waveout_oss_c : public bx_soundlow_waveout_c {
public:
  bx_soundlow_waveout_oss_c();
  virtual ~bx_soundlow_waveout_oss_c();

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);
  virtual int output(int length, Bit8u data[]);
private:
  int waveout_fd;
};

class bx_soundlow_wavein_oss_c : public bx_soundlow_wavein_c {
public:
  bx_soundlow_wavein_oss_c();
  virtual ~bx_soundlow_wavein_oss_c();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(bx_pcm_param_t *param);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();

  static void record_timer_handler(void *);
  void record_timer(void);
private:
  int wavein_fd;
  bx_pcm_param_t wavein_param;
};

class bx_soundlow_midiout_oss_c : public bx_soundlow_midiout_c {
public:
  bx_soundlow_midiout_oss_c();
  virtual ~bx_soundlow_midiout_oss_c();

  virtual int openmidioutput(const char *mididev);
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();

private:
  FILE *midi;
};

class bx_sound_oss_c : public bx_sound_lowlevel_c {
public:
  bx_sound_oss_c() : bx_sound_lowlevel_c("oss") {}
  virtual ~bx_sound_oss_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
  virtual bx_soundlow_wavein_c* get_wavein();
  virtual bx_soundlow_midiout_c* get_midiout();
} bx_sound_oss;

#endif
