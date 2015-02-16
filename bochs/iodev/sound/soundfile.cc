/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2015  The Bochs Project
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

// Support for sound output to files (based on SB16 code)

#include "iodev.h"
#include "soundlow.h"
#include "soundfile.h"

#if BX_SUPPORT_SOUNDLOW

#ifndef WIN32
#include <pthread.h>
#endif

#define LOG_THIS

// bx_soundlow_waveout_file_c class implemenzation

bx_soundlow_waveout_file_c::bx_soundlow_waveout_file_c()
    :bx_soundlow_waveout_c()
{
  wavefile = NULL;
}

bx_soundlow_waveout_file_c::~bx_soundlow_waveout_file_c()
{
  if (wavefile != NULL) {
    fputc(0, wavefile);
    fclose(wavefile);
    wavefile = NULL;
  }
}

int bx_soundlow_waveout_file_c::openwaveoutput(const char *wavedev)
{
  if ((wavefile == NULL) && (strlen(wavedev) > 0)) {
    wavefile = fopen(wavedev, "wb");
    if (wavefile == NULL) {
      BX_ERROR(("Failed to open WAVE output file %s.", wavedev));
    } else {
      DEV_soundmod_VOC_init_file(wavefile);
    }
  }
  set_pcm_params(&real_pcm_param);
  pcm_callback_id = register_wave_callback(this, pcm_callback);
  BX_INIT_MUTEX(mixer_mutex);
  start_mixer_thread();
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_file_c::set_pcm_params(bx_pcm_param_t *param)
{
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_file_c::output(int length, Bit8u data[])
{
  Bit8u temparray[12] =
   { (Bit8u)(real_pcm_param.samplerate & 0xff), (Bit8u)(real_pcm_param.samplerate >> 8), 0, 0,
     (Bit8u)real_pcm_param.bits, real_pcm_param.channels, 0, 0, 0, 0, 0, 0 };

  if (wavefile != NULL) {
    switch ((real_pcm_param.format >> 1) & 7) {
      case 2:
        temparray[6] = 3;
        break;
      case 3:
        temparray[6] = 2;
        break;
      case 4:
        temparray[6] = 1;
        break;
    }
    if (real_pcm_param.bits == 16)
      temparray[6] = 4;

    DEV_soundmod_VOC_write_block(wavefile, 9, 12, temparray, length, data);
    BX_MSLEEP(100);
  }
  return BX_SOUNDLOW_OK;
}

// bx_soundlow_midiout_file_c class implemenzation

bx_soundlow_midiout_file_c::bx_soundlow_midiout_file_c()
    :bx_soundlow_midiout_c()
{
  midifile = NULL;
}

bx_soundlow_midiout_file_c::~bx_soundlow_midiout_file_c()
{
  struct {
    Bit8u delta, statusbyte, metaevent, length;
  } metatrackend = { 0, 0xff, 0x2f, 0 };

  if (midifile != NULL) {
     // Meta event track end (0xff 0x2f 0x00) plus leading delta time
    fwrite(&metatrackend, 1, sizeof(metatrackend), midifile);

    Bit32u tracklen = ftell(midifile);
    if (tracklen < 0)
      BX_PANIC (("ftell failed in finishmidifile"));
    if (tracklen < 22)
      BX_PANIC (("MIDI track length too short"));
    tracklen -= 22;    // subtract the midi file and track header
    fseek(midifile, 22 - 4, SEEK_SET);
    // value has to be in big endian
#ifdef BX_LITTLE_ENDIAN
    tracklen = bx_bswap32(tracklen);
#endif
    fwrite(&tracklen, 4, 1, midifile);
    fclose(midifile);
  }
}

int bx_soundlow_midiout_file_c::openmidioutput(const char *mididev)
{
  struct {
    Bit8u chunk[4];
    Bit32u chunklen;  // all values in BIG Endian!
    Bit16u smftype;
    Bit16u tracknum;
    Bit16u timecode;  // 0x80 + deltatimesperquarter << 8
  } midiheader =
#ifdef BX_LITTLE_ENDIAN
      { "MTh", 0x06000000, 0, 0x0100, 0x8001 };
#else
      { "MTh", 6, 0, 1, 0x180 };
#endif
  midiheader.chunk[3] = 'd';

  struct {
    Bit8u chunk[4];
    Bit32u chunklen;
    Bit8u data[15];
  } trackheader =
#ifdef BX_LITTLE_ENDIAN
      { "MTr", 0xffffff7f,
#else
      { "MTr", 0x7fffffff,
#endif
       { 0x00,0xff,0x51,3,0x07,0xa1,0x20,    // set tempo 120 (0x7a120 us per quarter)
         0x00,0xff,0x58,4,4,2,0x18,0x08 }};  // time sig 4/4
  trackheader.chunk[3] = 'k';

  if ((midifile == NULL) && (strlen(mididev) > 0)) {
    midifile = fopen(mididev, "wb");
    if (midifile == NULL) {
      BX_ERROR(("Failed to open MIDI output file %s.", mididev));
      return BX_SOUNDLOW_ERR;
    } else {
      fwrite(&midiheader, 1, 14, midifile);
      fwrite(&trackheader, 1, 23, midifile);
    }
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_file_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  if (midifile != NULL) {
    writedeltatime(delta);
    fputc(command, midifile);
    if ((command == 0xf0) ||
        (command == 0xf7))    // write event length for sysex/meta events
      writedeltatime(length);

    fwrite(data, 1, length, midifile);
  }
  return BX_SOUNDLOW_OK;
}

void bx_soundlow_midiout_file_c::writedeltatime(Bit32u deltatime)
{
  int i, count = 0;
  Bit8u outbytes[4], value[4];

  if (deltatime == 0) {
    count = 1;
    value[0] = 0;
  } else {
    while ((deltatime > 0) && (count < 4)) { // split into parts of seven bits
     outbytes[count++] = deltatime & 0x7f;
     deltatime >>= 7;
    }
    for (i=0; i<count; i++)                     // reverse order and
     value[i] = outbytes[count - i - 1] | 0x80; // set eighth bit on
    value[count - 1] &= 0x7f;                   // all but last byte
  }
  for (int i=0; i<count; i++)
    fputc(value[i], midifile);
}

// bx_sound_oss_c class implemenzation

bx_sound_file_c::bx_sound_file_c()
    :bx_sound_lowlevel_c()
{
  BX_INFO(("Sound lowlevel module 'file' initialized"));
}

bx_soundlow_waveout_c* bx_sound_file_c::get_waveout()
{
  if (waveout == NULL) {
    waveout = new bx_soundlow_waveout_file_c();
  }
  return waveout;
}

bx_soundlow_midiout_c* bx_sound_file_c::get_midiout()
{
  if (midiout == NULL) {
    midiout = new bx_soundlow_midiout_file_c();
  }
  return midiout;
}

#endif
