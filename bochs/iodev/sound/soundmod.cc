/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011-2014  The Bochs Project
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
#include "soundlnx.h"
#include "soundosx.h"
#include "soundwin.h"
#include "soundsdl.h"
#include "soundalsa.h"

#ifndef WIN32
#include <pthread.h>
#endif
#if BX_WITH_SDL
#include <SDL.h>
#endif

#define LOG_THIS theSoundModCtl->

bx_soundmod_ctl_c* theSoundModCtl = NULL;

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
  if (soundmod != NULL) {
    soundmod->closewaveoutput();
  }
  delete soundmod;
}

void bx_soundmod_ctl_c::init()
{
  const char *driver = SIM->get_param_string(BXPN_SOUND_DRIVER)->getptr();
  const char *waveout = SIM->get_param_string(BXPN_SOUND_WAVEOUT)->getptr();
  const char *wavein = SIM->get_param_string(BXPN_SOUND_WAVEIN)->getptr();
  if (!strcmp(driver, "default")) {
    soundmod = new BX_SOUND_LOWLEVEL_C();
#if BX_HAVE_ALSASOUND
  } else if (!strcmp(driver, "alsa")) {
    soundmod = new bx_sound_alsa_c();
#endif
#if BX_WITH_SDL
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
  } else if (!strcmp(driver, "dummy")) {
    soundmod = new bx_sound_lowlevel_c();
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
  }
}

void* bx_soundmod_ctl_c::get_module()
{
  return soundmod;
}

#ifndef WIN32
pthread_t thread;
#endif
Bit8u *beep_buffer;
unsigned int beep_bytes, beep_bufsize;
Bit8u beep_control = 0;

#ifdef WIN32
DWORD WINAPI beep_thread(LPVOID indata)
#else
void beep_thread(void *indata)
#endif
{
  Bit8u level;
  unsigned int i, j;
  int ret;

  bx_sound_lowlevel_c *soundmod = (bx_sound_lowlevel_c*)indata;
  level = 0x40;
  i = 0;
  while (beep_control == 2) {
    j = 0;
    do {
      beep_buffer[j++] = level;
      if ((++i % beep_bytes) == 0) level ^= 0x40;
    } while (j < beep_bufsize);
    ret = soundmod->sendwavepacket(beep_bufsize, beep_buffer);
    if (ret == BX_SOUNDLOW_ERR) break;
    if (soundmod->get_type() == BX_SOUNDLOW_WIN) {
#ifdef WIN32
      Sleep(100);
#endif
    } else if (soundmod->get_type() == BX_SOUNDLOW_SDL) {
#if BX_WITH_SDL
      SDL_Delay(100);
#endif
    }
  }
  soundmod->stopwaveplayback();
  free(beep_buffer);
  beep_control = 0;
#ifdef WIN32
  return 0;
#else
  pthread_exit(NULL);
#endif
}

bx_bool bx_soundmod_ctl_c::beep_on(float frequency)
{
  if (soundmod != NULL) {
    BX_DEBUG(("Beep ON (frequency=%.2f)",frequency));
    if (beep_control > 0) {
      beep_off();
    }
    if (soundmod->startwaveplayback(44100, 8, 0, 0) == BX_SOUNDLOW_ERR) {
      return 0;
    }
    beep_bytes = (int)(44100.0 / frequency / 2);
    beep_bufsize = 4410;
    beep_buffer = (Bit8u*)malloc(beep_bufsize);
    beep_control = 2;
#ifdef WIN32
    DWORD threadID;
    CreateThread(NULL, 0, beep_thread, soundmod, 0, &threadID);
#else
    pthread_create(&thread, NULL, (void *(*)(void *))&beep_thread, soundmod);
#endif
    return 1;
  }
  return 0;
}

bx_bool bx_soundmod_ctl_c::beep_off()
{
  if (soundmod != NULL) {
    BX_DEBUG(("Beep OFF"));
    if (beep_control > 0) {
      beep_control = 1;
#ifdef WIN32
      while (beep_control > 0) Sleep(1);
#else
      pthread_join(thread, NULL);
#endif
    }
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

// The dummy sound lowlevel functions. They don't do anything.
bx_sound_lowlevel_c::bx_sound_lowlevel_c()
{
  put("soundlow", "SNDLOW");
  record_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_sound_lowlevel_c::~bx_sound_lowlevel_c()
{
}

int bx_sound_lowlevel_c::waveready()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::midiready()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openmidioutput(const char *mididev)
{
  UNUSED(mididev);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);
  UNUSED(command);
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closemidioutput()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openwaveoutput(const char *wavedev)
{
  UNUSED(wavedev);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
  UNUSED(frequency);
  UNUSED(bits);
  UNUSED(stereo);
  UNUSED(format);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::sendwavepacket(int length, Bit8u data[])
{
  UNUSED(length);
  UNUSED(data);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::stopwaveplayback()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closewaveoutput()
{
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "soundmod");
    // record timer: inactive, continuous, frequency variable
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::startwaverecord(int frequency, int bits, bx_bool stereo, int format)
{
  Bit64u timer_val;
  Bit8u shift = 0;

  UNUSED(format);
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (bits == 16) shift++;
    if (stereo) shift++;
    record_packet_size = (frequency / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (frequency << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::getwavepacket(int length, Bit8u data[])
{
  memset(data, 0, length);
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_lowlevel_c::closewaveinput()
{
  stopwaverecord();
  return BX_SOUNDLOW_OK;
}

void bx_sound_lowlevel_c::record_timer_handler(void *this_ptr)
{
  bx_sound_lowlevel_c *class_ptr = (bx_sound_lowlevel_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_sound_lowlevel_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

#endif
