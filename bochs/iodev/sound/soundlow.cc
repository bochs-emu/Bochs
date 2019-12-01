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

// Common sound module code and base classes for sound lowlevel functions

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundlow.h"

#define LOG_THIS

// audio buffer support

bx_audio_buffer_c *audio_buffers[2];

bx_audio_buffer_c::bx_audio_buffer_c(Bit8u _format)
{
  format = _format;
  root = NULL;
}

bx_audio_buffer_c::~bx_audio_buffer_c()
{
  while (root != NULL) {
    delete_buffer();
  }
}

audio_buffer_t* bx_audio_buffer_c::new_buffer(Bit32u size)
{
  audio_buffer_t *newbuffer = new audio_buffer_t;
  if (format == BUFTYPE_FLOAT) {
    newbuffer->fdata = new float[size];
  } else {
    newbuffer->data = new Bit8u[size];
  }
  newbuffer->size = size;
  newbuffer->pos = 0;
  newbuffer->next = NULL;

  if (root == NULL) {
    root = newbuffer;
  } else {
    audio_buffer_t *temp = root;

    while (temp->next)
      temp = temp->next;

    temp->next = newbuffer;
  }
  return newbuffer;
}

audio_buffer_t* bx_audio_buffer_c::get_buffer()
{
  return root;
}

void bx_audio_buffer_c::delete_buffer()
{
  audio_buffer_t *tmpbuffer = root;
  root = tmpbuffer->next;
  if (tmpbuffer->size > 0) {
    if (format == BUFTYPE_FLOAT) {
      delete [] tmpbuffer->fdata;
    } else {
      delete [] tmpbuffer->data;
    }
  }
  delete tmpbuffer;
}

// convert to float format for resampler

static void convert_to_float(Bit8u *src, unsigned srcsize, audio_buffer_t *audiobuf)
{
  unsigned i, j;
  bx_pcm_param_t *param = &audiobuf->param;
  bx_bool issigned = (param->format & 1);
  bx_bool setvol = (param->volume != BX_MAX_BIT16U);
  Bit16s val16s;
  Bit16u val16u;
  float volume[2];

  float *dst = audiobuf->fdata;
  if (setvol) {
    volume[0] = ((float)(param->volume & 0xff)) / 255.0F;
    volume[1] = ((float)(param->volume >> 8)) / 255.0F;
  }
  if (param->bits == 8) {
    if (issigned) {
      for (i = 0; i < srcsize; i++) {
        dst[i] = ((float)src[i]) / 128.0F;
        if (setvol) {
          dst[i] *= volume[i & 1];
        }
      }
    } else {
      for (i = 0; i < srcsize; i++) {
        dst[i] = (((float)src[i]) - 128.0F) / 128.0F;
        if (setvol) {
          dst[i] *= volume[i & 1];
        }
      }
    }
  } else {
    j = 0;
    if (issigned) {
      for (i = 0; i < srcsize; i += 2) {
        val16s = (Bit16s)(src[i] | (src[i+1] << 8));
        dst[j] = ((float)val16s) / 32768.0F;
        if (setvol) {
          dst[j] *= volume[i & 1];
        }
        j++;
      }
    } else {
      for (i = 0; i < srcsize; i += 2) {
        val16u = (Bit16u)(src[i] | (src[i+1] << 8));
        dst[j] = (((float)val16u) - 32768.0F) / 32768.0F;
        if (setvol) {
          dst[j] *= volume[i & 1];
        }
        j++;
      }
    }
  }
}

// convert from float format for output

void convert_float_to_s16le(float *src, unsigned srcsize, Bit8u *dst)
{
  Bit16s val16s;
  unsigned i, j = 0;

  for (i = 0; i < srcsize; i++) {
    val16s = (Bit16s)(src[i] * 32768.0F);
    dst[j++] =(Bit8u)(val16s & 0xff);
    dst[j++] = (Bit8u)(val16s >> 8);
  }
}

// audio buffer callback function

Bit32u pcm_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  Bit32u copied = 0;
  UNUSED(dev);
  UNUSED(rate);

  while (len > 0) {
    audio_buffer_t *curbuffer = audio_buffers[1]->get_buffer();
    if (curbuffer == NULL)
      break;
    Bit32u tmplen = curbuffer->size - curbuffer->pos;
    if (tmplen > len) {
      tmplen = len;
    }
    if (tmplen > 0) {
      memcpy(buffer+copied, curbuffer->data+curbuffer->pos, tmplen);
      curbuffer->pos += tmplen;
      copied += tmplen;
      len -= tmplen;
    }
    if (curbuffer->pos >= curbuffer->size) {
      audio_buffers[1]->delete_buffer();
    }
  }
  return copied;
}

// resampler & mixer thread support

BX_MUTEX(resampler_mutex);
BX_MUTEX(mixer_mutex);

BX_THREAD_FUNC(resampler_thread, indata)
{
  bx_soundlow_waveout_c *waveout = (bx_soundlow_waveout_c*)indata;
  while (waveout->resampler_running()) {
    BX_LOCK(resampler_mutex);
    audio_buffer_t *curbuffer = audio_buffers[0]->get_buffer();
    BX_UNLOCK(resampler_mutex);
    if (curbuffer != NULL) {
      waveout->resampler(curbuffer, NULL);
      BX_LOCK(resampler_mutex);
      audio_buffers[0]->delete_buffer();
      BX_UNLOCK(resampler_mutex);
    } else {
      BX_MSLEEP(20);
    }
  }
  BX_THREAD_EXIT;
}

BX_THREAD_FUNC(mixer_thread, indata)
{
  int len;

  bx_soundlow_waveout_c *waveout = (bx_soundlow_waveout_c*)indata;
  Bit8u *mixbuffer = new Bit8u[BX_SOUNDLOW_WAVEPACKETSIZE];
  while (waveout->mixer_running()) {
    len = waveout->get_packetsize();
    memset(mixbuffer, 0, len);
    if (waveout->mixer_common(mixbuffer, len)) {
      waveout->output(len, mixbuffer);
    } else {
      BX_MSLEEP(25);
    }
  }
  delete [] mixbuffer;
  waveout->closewaveoutput();
  BX_THREAD_EXIT;
}

// bx_soundlow_waveout_c class implementation
// The dummy output methods don't do anything.

bx_soundlow_waveout_c::bx_soundlow_waveout_c()
{
  put("waveout", "WAVOUT");
  if (audio_buffers[0] == NULL) {
    audio_buffers[0] = new bx_audio_buffer_c(BUFTYPE_FLOAT);
    audio_buffers[1] = new bx_audio_buffer_c(BUFTYPE_UCHAR);
  }
  real_pcm_param = default_pcm_param;
  cb_count = 0;
  pcm_callback_id = -1;
  res_thread_start = 0;
  mix_thread_start = 0;
#if BX_HAVE_LIBSAMPLERATE || BX_HAVE_SOXR_LSR
  int ret = 0;
  src_state = src_new(SRC_SINC_MEDIUM_QUALITY, 2, &ret);
#endif
}

bx_soundlow_waveout_c::~bx_soundlow_waveout_c()
{
  if (pcm_callback_id >= 0) {
#if BX_HAVE_LIBSAMPLERATE || BX_HAVE_SOXR_LSR
    src_delete(src_state);
#endif
    unregister_wave_callback(pcm_callback_id);
    if (res_thread_start) {
      res_thread_start = 0;
      BX_MSLEEP(20);
      BX_FINI_MUTEX(resampler_mutex);
    }
    if (mix_thread_start) {
      mix_thread_start = 0;
      BX_MSLEEP(25);
      BX_FINI_MUTEX(mixer_mutex);
    }
    if (audio_buffers[0] != NULL) {
      delete audio_buffers[0];
      delete audio_buffers[1];
      audio_buffers[0] = NULL;
    }
  }
}

int bx_soundlow_waveout_c::openwaveoutput(const char *wavedev)
{
  UNUSED(wavedev);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::set_pcm_params(bx_pcm_param_t *param)
{
  UNUSED(param);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *src_param)
{
  unsigned len1 = length;

  if (src_param->bits == 16) len1 >>= 1;
  if (pcm_callback_id >= 0) {
    BX_LOCK(resampler_mutex);
    audio_buffer_t *inbuffer = audio_buffers[0]->new_buffer(len1);
    memcpy(&inbuffer->param, src_param, sizeof(bx_pcm_param_t));
    convert_to_float(data, length, inbuffer);
    BX_UNLOCK(resampler_mutex);
  } else {
    audio_buffer_t *inbuffer = new audio_buffer_t;
    inbuffer->fdata = new float[len1];
    inbuffer->size = len1;
    memcpy(&inbuffer->param, src_param, sizeof(bx_pcm_param_t));
    audio_buffer_t *outbuffer = new audio_buffer_t;
    memset(outbuffer, 0, sizeof(audio_buffer_t));
    convert_to_float(data, length, inbuffer);
    resampler(inbuffer, outbuffer);
    output(outbuffer->size, outbuffer->data);
    delete outbuffer;
    delete inbuffer;
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::get_packetsize()
{
  return (real_pcm_param.samplerate * 4 / 10);
}

int bx_soundlow_waveout_c::output(int length, Bit8u data[])
{
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::closewaveoutput()
{
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_c::register_wave_callback(void *arg, get_wave_cb_t wd_cb)
{
  if (cb_count < BX_MAX_WAVE_CALLBACKS) {
    get_wave[cb_count].device = arg;
    get_wave[cb_count].cb = wd_cb;
    return cb_count++;
  }
  return -1;
}

void bx_soundlow_waveout_c::unregister_wave_callback(int callback_id)
{
  BX_LOCK(mixer_mutex);
  if ((callback_id >= 0) && (callback_id < BX_MAX_WAVE_CALLBACKS)) {
    get_wave[callback_id].device = NULL;
    get_wave[callback_id].cb = NULL;
  }
  BX_UNLOCK(mixer_mutex);
}

bx_bool bx_soundlow_waveout_c::mixer_common(Bit8u *buffer, int len)
{
  Bit32u count, len2 = 0, len3 = 0;
  Bit16s src1, src2, dst_val;
  Bit32s tmp_val;
  Bit8u *src, *dst;

  Bit8u *tmpbuffer = new Bit8u[len];
  BX_LOCK(mixer_mutex);
  for (int i = 0; i < cb_count; i++) {
    if (get_wave[i].cb != NULL) {
      memset(tmpbuffer, 0, len);
      len2 = get_wave[i].cb(get_wave[i].device, real_pcm_param.samplerate, tmpbuffer, len);
      if (len2 > 0) {
        src = tmpbuffer;
        dst = buffer;
        count = len / 2;
        while (count--) {
          src1 = (src[0] | (src[1] << 8));
          src2 = (dst[0] | (dst[1] << 8));
          tmp_val = (Bit32s)src1 + (Bit32s)src2;
          if (tmp_val > BX_MAX_BIT16S) {
            tmp_val = BX_MAX_BIT16S;
          } else if (tmp_val < BX_MIN_BIT16S) {
            tmp_val = BX_MIN_BIT16S;
          }
          dst_val = (Bit16s)tmp_val;
          dst[0] = dst_val & 0xff;
          dst[1] = (Bit8u)(dst_val >> 8);
          src += 2;
          dst += 2;
        }
        if (len3 < len2) len3 = len2;
      }
    }
  }
  BX_UNLOCK(mixer_mutex);
  delete [] tmpbuffer;
  return (len3 > 0);
}

void bx_soundlow_waveout_c::resampler(audio_buffer_t *inbuffer, audio_buffer_t *outbuffer)
{
  Bit32u fcount;
  float *fbuffer = NULL;

  fcount = resampler_common(inbuffer, &fbuffer);
  if (outbuffer == NULL) {
    BX_LOCK(mixer_mutex);
    audio_buffer_t *newbuffer = audio_buffers[1]->new_buffer(fcount << 1);
    convert_float_to_s16le(fbuffer, fcount, newbuffer->data);
    BX_UNLOCK(mixer_mutex);
  } else {
    outbuffer->data = new Bit8u[fcount << 1];
    outbuffer->size = (fcount << 1);
    convert_float_to_s16le(fbuffer, fcount, outbuffer->data);
  }
  if (fbuffer != NULL) {
    delete [] fbuffer;
  }
}

Bit32u bx_soundlow_waveout_c::resampler_common(audio_buffer_t *inbuffer, float **fbuffer)
{
  unsigned i, j, fcount = 0;
  bx_pcm_param_t param = inbuffer->param;

  if (param.channels != real_pcm_param.channels) {
    if (param.channels == 1) {
      float *temp = new float[inbuffer->size * 2];
      j = 0;
      for (i = 0; i < inbuffer->size; i++) {
        temp[j++] = inbuffer->fdata[i];
        temp[j++] = inbuffer->fdata[i];
      }
      delete [] inbuffer->fdata;
      inbuffer->fdata = temp;
      inbuffer->size <<= 1;
    } else {
      BX_ERROR(("conversion from stereo to mono not implemented"));
    }
  }
#if BX_HAVE_LIBSAMPLERATE || BX_HAVE_SOXR_LSR
  if (param.samplerate != real_pcm_param.samplerate) {
    SRC_DATA data;
    double irate = (double)param.samplerate;
    double orate = (double)real_pcm_param.samplerate;
    size_t ilen = inbuffer->size / 2;
    size_t olen = (size_t)(ilen * orate / irate + 0.5);
    *fbuffer  = new float[olen * 2];
    fcount = olen * 2;
    int ret = 0;

    data.data_in = inbuffer->fdata;
    data.data_out = *fbuffer;
    data.input_frames = (int)ilen;
    data.output_frames = (int)olen;
    data.src_ratio = orate / irate;
    data.end_of_input = 0;
    ret = src_process(src_state, &data);
    if (ret != 0) {
      BX_ERROR(("resampling error: %s", src_strerror(ret)));
    }
  } else {
    *fbuffer = new float[inbuffer->size];
    fcount = inbuffer->size;
    memcpy(*fbuffer, inbuffer->data, sizeof(float) * fcount);
  }
#else
  if (param.samplerate != real_pcm_param.samplerate) {
    real_pcm_param.samplerate = param.samplerate;
    set_pcm_params(&real_pcm_param);
  }
  *fbuffer = new float[inbuffer->size];
  fcount = inbuffer->size;
  memcpy(*fbuffer, inbuffer->data, sizeof(float) * fcount);
#endif
  return fcount;
}

void bx_soundlow_waveout_c::start_resampler_thread()
{
  BX_INIT_MUTEX(resampler_mutex);
  res_thread_start = 1;
  BX_THREAD_CREATE(resampler_thread, this, res_thread_var);
}

void bx_soundlow_waveout_c::start_mixer_thread()
{
  BX_INIT_MUTEX(mixer_mutex);
  mix_thread_start = 1;
  BX_THREAD_CREATE(mixer_thread, this, mix_thread_var);
}

// bx_soundlow_wavein_c class implementation
// The dummy input method returns silence.

bx_soundlow_wavein_c::bx_soundlow_wavein_c()
{
  put("wavein", "WAVEIN");
  record_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_soundlow_wavein_c::~bx_soundlow_wavein_c()
{
  stopwaverecord();
}

int bx_soundlow_wavein_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = DEV_register_timer(this, record_timer_handler, 1, 1, 0, "wavein");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::startwaverecord(bx_pcm_param_t *param)
{
  Bit64u timer_val;
  Bit8u shift = 0;

  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (param->bits == 16) shift++;
    if (param->channels == 2) shift++;
    record_packet_size = (param->samplerate / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (param->samplerate << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::getwavepacket(int length, Bit8u data[])
{
  memset(data, 0, length);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_wavein_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_wavein_c::record_timer_handler(void *this_ptr)
{
  bx_soundlow_wavein_c *class_ptr = (bx_soundlow_wavein_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_soundlow_wavein_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

// bx_soundlow_midiout_c class implementation
// The dummy output methods don't do anything.

bx_soundlow_midiout_c::bx_soundlow_midiout_c()
{
  put("midiout", "MIDI");
}

bx_soundlow_midiout_c::~bx_soundlow_midiout_c()
{
}

int bx_soundlow_midiout_c::openmidioutput(const char *mididev)
{
  UNUSED(mididev);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_c::midiready()
{
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);
  UNUSED(command);
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_c::closemidioutput()
{
  return BX_SOUNDLOW_OK;
}

bx_sound_lowlevel_c *bx_sound_lowlevel_c::all;

// bx_sound_lowlevel_c class implementation

bx_sound_lowlevel_c::bx_sound_lowlevel_c(const char *type)
{
  put("soundlow", "SNDLOW");
  waveout = NULL;
  wavein = NULL;
  midiout = NULL;
  // self-registering static objects ported from the network code
  next = all;
  all  = this;
  this->type = type;
}

bx_sound_lowlevel_c::~bx_sound_lowlevel_c()
{
  bx_sound_lowlevel_c *ptr = 0;

  if (waveout != NULL) {
    delete waveout;
  }
  if (wavein != NULL) {
    delete wavein;
  }
  if (midiout != NULL) {
    delete midiout;
  }
  // unregister sound driver
  if (this == all) {
    all = all->next;
  } else {
    ptr = all;
    while (ptr != NULL) {
      if (ptr->next != this) {
        ptr = ptr->next;
      } else {
        break;
      }
    }
  }
  if (ptr) {
    ptr->next = this->next;
  }
}

bx_bool bx_sound_lowlevel_c::module_present(const char *type)
{
  bx_sound_lowlevel_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return 1;
  }
  return 0;
}

bx_sound_lowlevel_c* bx_sound_lowlevel_c::get_module(const char *type)
{
  bx_sound_lowlevel_c *ptr = 0;

  for (ptr = all; ptr != NULL; ptr = ptr->next) {
    if (strcmp(type, ptr->type) == 0)
      return ptr;
  }
  return NULL;
}

void bx_sound_lowlevel_c::cleanup()
{
#if BX_PLUGINS
  while (all != NULL) {
    PLUG_unload_snd_plugin(all->type);
  }
#endif
}

#endif
