/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2015  The Bochs Project
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

// Common sound module code and dummy sound lowlevel functions

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SOUNDLOW

#include "soundmod.h"
#include "soundlow.h"
#include "soundlnx.h"
#include "soundosx.h"
#include "soundwin.h"
#include "soundsdl.h"
#include "soundalsa.h"

#ifndef WIN32
#include <pthread.h>
#endif
#if BX_WITH_SDL || BX_WITH_SDL2
#include <SDL.h>
#endif

#define LOG_THIS theSoundModCtl->

bx_soundmod_ctl_c* theSoundModCtl = NULL;

BX_THREAD_ID(threadID);
BX_MUTEX(beep_mutex);
static int beep_control;
Bit8u *beep_buffer;
unsigned int beep_bufsize;
bx_pcm_param_t beep_param;

Bit32u beep_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len);
BX_THREAD_FUNC(beep_thread, indata);

int CDECL libsoundmod_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theSoundModCtl = new bx_soundmod_ctl_c;
    bx_devices.pluginSoundModCtl = theSoundModCtl;
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libsoundmod_LTX_plugin_fini(void)
{
  delete theSoundModCtl;
}

bx_soundmod_ctl_c::bx_soundmod_ctl_c()
{
  put("soundctl", "SNDCTL");
  soundmod = NULL;
}

bx_soundmod_ctl_c::~bx_soundmod_ctl_c()
{
  beep_active = 0;
  if (beep_callback_id < 0) {
    beep_control = 0;
    while (beep_control >= 0) {
      BX_MSLEEP(1);
    }
  } else {
    soundmod->unregister_wave_callback(beep_callback_id);
  }
  free(beep_buffer);
  if (soundmod != NULL) {
    soundmod->closewaveoutput();
  }
  delete soundmod;
}

void bx_soundmod_ctl_c::init()
{
  static const char default_name[] = BX_SOUND_LOWLEVEL_NAME;
  const char *driver = SIM->get_param_string(BXPN_SOUND_DRIVER)->getptr();
  const char *waveout = SIM->get_param_string(BXPN_SOUND_WAVEOUT)->getptr();
  const char *wavein = SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr();

  if (strcmp(driver, "default") == 0) {
    driver = default_name;
  }

  if (strcmp(driver, "dummy") == 0) {
    soundmod = new bx_sound_lowlevel_c();
#if BX_HAVE_ALSASOUND
  } else if (!strcmp(driver, "alsa")) {
    soundmod = new bx_sound_alsa_c();
#endif
#if BX_WITH_SDL || BX_WITH_SDL2
  } else if (!strcmp(driver, "sdl")) {
    soundmod = new bx_sound_sdl_c();
#endif
#if (defined(linux) || defined(__FreeBSD__) || defined(__FreeBSD_kernel__))
  } else if (!strcmp(driver, "oss")) {
    soundmod = new bx_sound_linux_c();
#endif
#if defined(macintosh)
  } else if (!strcmp(driver, "osx")) {
    soundmod = new bx_sound_osx_c();
#endif
#if defined(WIN32)
  } else if (!strcmp(driver, "win")) {
    soundmod = new bx_sound_windows_c();
#endif
  } else {
    BX_PANIC(("unknown lowlevel sound driver '%s'", driver));
    return;
  }
  if (!strlen(wavein)) {
    SIM->get_param_string(BXPN_SOUND_WAVEIN)->set(waveout);
  }
  int ret = soundmod->openwaveoutput(waveout);
  if (ret != BX_SOUNDLOW_OK) {
    BX_PANIC(("Could not open wave output device"));
  } else {
    beep_active = 0;
    beep_callback_id = soundmod->register_wave_callback(theSoundModCtl, beep_callback);
    beep_cur_freq = 0.0;
    beep_bufsize = 4410;
    beep_buffer = (Bit8u*)malloc(beep_bufsize);
    BX_INIT_MUTEX(beep_mutex);
    if (beep_callback_id < 0) {
      beep_param.samplerate = 44100;
      beep_param.bits = 16;
      beep_param.channels = 2;
      beep_param.format = 1;
      BX_THREAD_CREATE(beep_thread, soundmod, threadID);
    }
    soundmod->startwaveplayback(44100, 16, 1, 1);
  }
}

void* bx_soundmod_ctl_c::get_module()
{
  return soundmod;
}

Bit32u  bx_soundmod_ctl_c::beep_generator(Bit16u rate, Bit8u *buffer, Bit32u len)
{
  Bit32u j = 0;
  Bit16u beep_samples;
  static Bit8u beep_level = 0x40;
  static Bit16u beep_pos = 0;

  BX_LOCK(beep_mutex);
  if (!beep_active) {
    BX_UNLOCK(beep_mutex);
    return 0;
  }
  beep_samples = (Bit32u)((float)rate / beep_cur_freq / 2);
  do {
    buffer[j++] = 0;
    buffer[j++] = beep_level;
    buffer[j++] = 0;
    buffer[j++] = beep_level;
    if ((++beep_pos % beep_samples) == 0) {
      beep_level ^= 0x80;
      beep_pos = 0;
      beep_samples = (Bit32u)((float)rate / beep_cur_freq / 2);
    }
  } while (j < len);
  BX_UNLOCK(beep_mutex);
  return len;
}

Bit32u beep_callback(void *dev, Bit16u rate, Bit8u *buffer, Bit32u len)
{
  return ((bx_soundmod_ctl_c*)dev)->beep_generator(rate, buffer, len);
}

BX_THREAD_FUNC(beep_thread, indata)
{
  Bit32u len = 0;

  bx_sound_lowlevel_c *soundmod = (bx_sound_lowlevel_c*)indata;
  beep_control = 1;
  while (beep_control > 0) {
    len = theSoundModCtl->beep_generator(44100, beep_buffer, beep_bufsize);
    if (len > 0) {
      soundmod->sendwavepacket(beep_bufsize, beep_buffer, &beep_param);
      if (soundmod->get_type() == BX_SOUNDLOW_WIN) {
        BX_MSLEEP(25);
      } else {
        BX_MSLEEP(1);
      }
    } else {
      BX_MSLEEP(25);
    }
  }
  beep_control = -1;
  BX_THREAD_EXIT;
}

bx_bool bx_soundmod_ctl_c::beep_on(float frequency)
{
  if (soundmod != NULL) {
    BX_DEBUG(("Beep ON (frequency=%.2f)",frequency));
    if (frequency != beep_cur_freq) {
      BX_LOCK(beep_mutex);
      beep_cur_freq = frequency;
      beep_active = 1;
      BX_UNLOCK(beep_mutex);
    }
    return 1;
  }
  return 0;
}

bx_bool bx_soundmod_ctl_c::beep_off()
{
  if (soundmod != NULL) {
    BX_DEBUG(("Beep OFF"));
    BX_LOCK(beep_mutex);
    beep_active = 0;
    beep_cur_freq = 0.0;
    BX_UNLOCK(beep_mutex);
    return 1;
  }
  return 0;
}

/* Handlers for the VOC file output */

// Write the header of the VOC file.

void bx_soundmod_ctl_c::VOC_init_file(FILE *stream)
{
  struct {
    char id[20];
    Bit16u headerlen;  // All in LITTLE Endian!
    Bit16u version;
    Bit16u chksum;
  } vocheader =
    { "Creative Voice File",
#ifdef BX_LITTLE_ENDIAN
      0x1a, 0x0114, 0x111f };
#else
      0x1a00, 0x1401, 0x1f11 };
#endif

  vocheader.id[19] = 26;    // Replace string end with 26

  fwrite(&vocheader, 1, sizeof vocheader, stream);
}

// write one block to the VOC file
void bx_soundmod_ctl_c::VOC_write_block(FILE *stream, int block,
                           Bit32u headerlen, Bit8u header[],
                           Bit32u datalen, Bit8u data[])
{
  Bit32u i;

  if (block > 9) {
      BX_ERROR(("VOC Block %d not recognized, ignored.", block));
      return;
  }

  fputc(block, stream);

  i = headerlen + datalen;
#ifdef BX_LITTLE_ENDIAN
  fwrite(&i, 1, 3, stream);       // write the length in 24-bit little endian
#else
  Bit8u lengthbytes[3];
  lengthbytes[0] = i & 0xff; i >>= 8;
  lengthbytes[1] = i & 0xff; i >>= 8;
  lengthbytes[2] = i & 0xff;
  fwrite(lengthbytes, 1, 3, stream);
#endif
  BX_DEBUG(("Voc block %d; Headerlen %d; Datalen %d",
            block, headerlen, datalen));
  if (headerlen > 0)
    fwrite(header, 1, headerlen, stream);
  if (datalen > 0)
    fwrite(data, 1, datalen, stream);
}

void bx_soundmod_ctl_c::pcm_apply_volume(Bit32u datalen, Bit8u data[], Bit16u volume,
                                         Bit8u bits, bx_bool stereo, bx_bool issigned)
{
  unsigned i;
  Bit8u value8u;
  Bit8s value8s;
  Bit16u value16u, tmpvol;
  Bit16s value16s;
  Bit8u volumes[2], channel = 0;

  if (stereo) {
    volumes[0] = (Bit8u)(volume & 0xff);
    volumes[1] = (Bit8u)(volume >> 8);
  } else {
    tmpvol = ((volume & 0xff) + (volume >> 8)) / 2;
    volumes[0] = volumes[1] = (Bit8u)tmpvol;
  }

  if (bits == 16) {
    for (i = 0; i < datalen; i += 2) {
      if (issigned) {
        value16s = (Bit16s)(data[i] | (data[i+1] << 8));
        value16s = (Bit16s)((Bit32s)value16s * volumes[channel] / 256);
        data[i] = (Bit8u)(value16s & 0xff);
        data[i+1] = (Bit8u)(value16s >> 8);
      } else {
        value16u = data[i] | (data[i+1] << 8);
        value16u = (Bit16u)((Bit32u)value16u * volumes[channel] / 256);
        data[i] = (Bit8u)(value16u & 0xff);
        data[i+1] = (Bit8u)(value16u >> 8);
      }
      channel ^= 1;
    }
  } else {
    for (i = 0; i < datalen; i++) {
      if (issigned) {
        value8s = (Bit8s)data[i];
        data[i] = (Bit8u)(((Bit16s)value8s * volumes[channel]) / 256);
      } else {
        value8u = data[i];
        data[i] = (Bit8u)(((Bit16u)value8u * volumes[channel]) / 256);
      }
      channel ^= 1;
    }
  }
}

#endif
