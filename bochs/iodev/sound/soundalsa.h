/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// ALSA PCM input/output and MIDI output support written by Volker Ruppert

#if BX_HAVE_ALSASOUND

#define BX_SOUND_LINUX_BUFSIZE   BX_SOUNDLOW_WAVEPACKETSIZE * 2

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

class bx_sound_alsa_c : public bx_sound_lowlevel_c {
public:
  bx_sound_alsa_c();
  virtual ~bx_sound_alsa_c();

  virtual int get_type() {return BX_SOUNDLOW_ALSA;}

  virtual int waveready();
  virtual int midiready();

  virtual int openmidioutput(const char *mididev);
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();

  virtual int openwaveoutput(const char *wavedev);
  virtual int startwaveplayback(int frequency, int bits, bx_bool stereo, int format);
  virtual int sendwavepacket(int length, Bit8u data[]);
  virtual int stopwaveplayback();
  virtual int closewaveoutput();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(int frequency, int bits, bx_bool stereo, int format);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();
  virtual int closewaveinput();

  static void record_timer_handler(void *);
  void record_timer(void);
private:
  int alsa_seq_open(const char *alsadev);
  int alsa_seq_output(int delta, int command, int length, Bit8u data[]);
  int alsa_pcm_open(bx_bool input, int frequency, int bits, bx_bool stereo, int format);
  int alsa_pcm_write();

  struct {
    snd_seq_t *handle;
    int source_port;
  } alsa_seq;
  struct {
    snd_pcm_t *handle;
    snd_pcm_uframes_t frames;
    int alsa_bufsize, audio_bufsize;
    char *buffer;
  } alsa_pcm[2];
  struct {
    int oldfreq, oldbits, oldformat;
    bx_bool oldstereo;
  } wave_ch[2];
  Bit8u audio_buffer[2][BX_SOUND_LINUX_BUFSIZE];
};

#endif
