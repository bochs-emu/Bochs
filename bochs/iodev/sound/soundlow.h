/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2017  The Bochs Project
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

// Common code for sound lowlevel modules

#include "bxthread.h"

#if BX_HAVE_LIBSAMPLERATE
#include <samplerate.h>
#elif BX_HAVE_SOXR_LSR
#include <soxr-lsr.h>
#endif

// This is the maximum size of a wave data packet.
// It should be large enough for 0.1 seconds of playback or recording.
#define BX_SOUNDLOW_WAVEPACKETSIZE  19200

// Definitions for the output functions
#define BX_SOUNDLOW_OK   0
#define BX_SOUNDLOW_ERR  1

#define BX_MAX_WAVE_CALLBACKS 3

typedef struct {
  Bit16u samplerate;
  Bit8u  bits;
  Bit8u  channels;
  Bit8u  format;
  Bit16u volume;
} bx_pcm_param_t;

const bx_pcm_param_t default_pcm_param = { 44100, 16, 2, 1, 0xffff };

typedef Bit32u (*sound_record_handler_t)(void *arg, Bit32u len);
typedef Bit32u (*get_wave_cb_t)(void *arg, Bit16u rate, Bit8u *buffer, Bit32u len);

// audio buffer support

#define BUFTYPE_FLOAT 0
#define BUFTYPE_UCHAR 1

typedef struct _audio_buffer_t
{
  Bit32u size, pos;
  union {
    Bit8u *data;
    float *fdata;
  };
  bx_pcm_param_t param;
  struct _audio_buffer_t *next;
} audio_buffer_t;

class bx_audio_buffer_c {
public:
  bx_audio_buffer_c(Bit8u format);
  ~bx_audio_buffer_c();

  audio_buffer_t *new_buffer(Bit32u size);
  audio_buffer_t *get_buffer();
  void delete_buffer();
private:
  Bit8u format;
  audio_buffer_t *root;
};

extern bx_audio_buffer_c *audio_buffers[2];
void convert_float_to_s16le(float *src, unsigned srcsize, Bit8u *dst);
BOCHSAPI_MSVCONLY Bit32u pcm_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len);

extern BX_MUTEX(resampler_mutex);
#ifndef ANDROID
extern BX_MUTEX(mixer_mutex);
#endif

// the waveout class

class BOCHSAPI_MSVCONLY bx_soundlow_waveout_c : public logfunctions {
public:
  bx_soundlow_waveout_c();
  virtual ~bx_soundlow_waveout_c();

  virtual int openwaveoutput(const char *wavedev);
  virtual int set_pcm_params(bx_pcm_param_t *param);
  virtual int sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param);
  virtual int get_packetsize();
  virtual int output(int length, Bit8u data[]);
  virtual int closewaveoutput();

  virtual int register_wave_callback(void *, get_wave_cb_t wd_cb);
  virtual void unregister_wave_callback(int callback_id);

  virtual void resampler(audio_buffer_t *inbuffer, audio_buffer_t *outbuffer);

  virtual bx_bool mixer_common(Bit8u *buffer, int len);

  bx_bool resampler_running() {return res_thread_start;}
  bx_bool mixer_running() {return mix_thread_start;}

protected:
  void start_resampler_thread(void);
  void start_mixer_thread(void);
  Bit32u resampler_common(audio_buffer_t *inbuffer, float **fbuffer);

  bx_pcm_param_t real_pcm_param;
  bx_bool res_thread_start;
  bx_bool mix_thread_start;
  BX_THREAD_VAR(res_thread_var);
  BX_THREAD_VAR(mix_thread_var);
#if BX_HAVE_LIBSAMPLERATE || BX_HAVE_SOXR_LSR
  SRC_STATE *src_state;
#endif

  int cb_count;
  struct {
    void *device;
    get_wave_cb_t cb;
  } get_wave[BX_MAX_WAVE_CALLBACKS];
  int pcm_callback_id;
};

// the wavein class

class BOCHSAPI_MSVCONLY bx_soundlow_wavein_c : public logfunctions {
public:
  bx_soundlow_wavein_c();
  virtual ~bx_soundlow_wavein_c();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(bx_pcm_param_t *param);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();

  static void record_timer_handler(void *);
  void record_timer(void);
protected:
  int record_timer_index;
  int record_packet_size;
  sound_record_handler_t record_handler;
};

// the midiout class

class BOCHSAPI_MSVCONLY bx_soundlow_midiout_c : public logfunctions {
public:
  bx_soundlow_midiout_c();
  virtual ~bx_soundlow_midiout_c();

  virtual int openmidioutput(const char *mididev);
  virtual int midiready();
  virtual int sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int closemidioutput();
};

// This is the base class of the sound lowlevel support.
// the lowlevel sound driver class returns pointers to the child objects

class BOCHSAPI_MSVCONLY bx_sound_lowlevel_c : public logfunctions {
public:
  static bx_bool module_present(const char *type);
  static bx_sound_lowlevel_c* get_module(const char *type);
  static void cleanup();

  virtual bx_soundlow_waveout_c* get_waveout() {return NULL;}
  virtual bx_soundlow_wavein_c* get_wavein() {return NULL;}
  virtual bx_soundlow_midiout_c* get_midiout() {return NULL;}

protected:
  bx_sound_lowlevel_c(const char *type);
  virtual ~bx_sound_lowlevel_c();

  bx_soundlow_waveout_c *waveout;
  bx_soundlow_wavein_c *wavein;
  bx_soundlow_midiout_c *midiout;
private:
  static bx_sound_lowlevel_c *all;
  bx_sound_lowlevel_c *next;
  const char *type;
};
