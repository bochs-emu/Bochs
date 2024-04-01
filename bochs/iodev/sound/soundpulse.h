/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2024  The Bochs Project
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

// PCM playback/recording support using PulseAudio simple API

#if BX_HAVE_SOUND_PULSE

#define BX_SOUND_PULSE_BUFSIZE   BX_SOUNDLOW_WAVEPACKETSIZE

#include <pulse/simple.h>
#include <pulse/error.h>

class bx_sound_pulse_c : public bx_sound_lowlevel_c {
public:
  bx_sound_pulse_c() : bx_sound_lowlevel_c("pulse") {}
  virtual ~bx_sound_pulse_c() {}

  virtual bx_soundlow_waveout_c* get_waveout();
  virtual bx_soundlow_wavein_c* get_wavein();
} bx_sound_pulse;

// the pulse waveout class

class bx_soundlow_waveout_pulse_c : public bx_soundlow_waveout_c {
public:
  bx_soundlow_waveout_pulse_c() : bx_soundlow_waveout_c() {}
  virtual ~bx_soundlow_waveout_pulse_c() {}

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);
  virtual int get_packetsize();
  virtual int output(int length, Bit8u data[]);
  virtual int closewaveoutput();
private:
  pa_simple *s;
};

// the pulse waveoin class

class bx_soundlow_wavein_pulse_c : public bx_soundlow_wavein_c {
public:
  bx_soundlow_wavein_pulse_c();
  virtual ~bx_soundlow_wavein_pulse_c();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(bx_pcm_param_t *param);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();

  static void record_timer_handler(void *);
  void record_timer(void);
private:
  bx_pcm_param_t wavein_param;
  pa_simple *s;
};

#endif // BX_HAVE_SOUND_PULSE
