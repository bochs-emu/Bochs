/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2017  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "soundlow.h"
#include "soundmod.h"
#include "soundfile.h"

#if BX_SUPPORT_SOUNDLOW

#define BX_SOUNDFILE_RAW 0
#define BX_SOUNDFILE_VOC 1
#define BX_SOUNDFILE_WAV 2
#define BX_SOUNDFILE_MID 3

#define LOG_THIS

// sound driver plugin entry points

int CDECL libfile_sound_plugin_init(plugin_t *plugin, plugintype_t type)
{
  // Nothing here yet
  return 0; // Success
}

void CDECL libfile_sound_plugin_fini(void)
{
  // Nothing here yet
}

// bx_soundlow_waveout_file_c class implementation

bx_soundlow_waveout_file_c::bx_soundlow_waveout_file_c()
    :bx_soundlow_waveout_c()
{
  wavefile = NULL;
  type = BX_SOUNDFILE_RAW;
}

void bx_soundlow_waveout_file_c::initwavfile()
{
  Bit8u waveheader[44] =
    {0x52, 0x49, 0x46, 0x46, 0, 0, 0, 0, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d,
     0x74, 0x20, 0x10, 0, 0, 0, 0x1, 0, 0x2, 0, 0x44, 0xac, 0, 0, 0x10, 0xb1,
     0x2, 0, 0x4, 0, 0x10, 0, 0x64, 0x61, 0x74, 0x61, 0, 0, 0, 0};

  fwrite(waveheader, 1, 44, wavefile);
}

void bx_soundlow_waveout_file_c::write_32bit(Bit32u pos, Bit32u value)
{
  Bit8u size[4] = {(Bit8u)(value & 0xff), (Bit8u)(value >> 8),
                   (Bit8u)(value >> 16), (Bit8u)(value >> 24)};

  fseek(wavefile, pos, SEEK_SET);
  fwrite(size, 1, 4, wavefile);
}

int bx_soundlow_waveout_file_c::openwaveoutput(const char *wavedev)
{
  size_t len = strlen(wavedev);
  char ext[4];

  if ((wavefile == NULL) && (len > 0)) {
    if (len > 4) {
      if (wavedev[len-4] == '.') {
        strcpy(ext, wavedev+len-3);
        if (!stricmp(ext, "voc")) {
          type = BX_SOUNDFILE_VOC;
        } else if (!stricmp(ext, "wav")) {
          type = BX_SOUNDFILE_WAV;
        }
      }
    }
    wavefile = fopen(wavedev, "wb");
    if (wavefile == NULL) {
      BX_ERROR(("Failed to open WAVE output file %s.", wavedev));
    } else if (type == BX_SOUNDFILE_VOC) {
      VOC_init_file();
    } else if (type == BX_SOUNDFILE_WAV) {
      initwavfile();
    }
    set_pcm_params(&real_pcm_param);
    if (!res_thread_start) {
      start_resampler_thread();
    }
    if (!mix_thread_start) {
      pcm_callback_id = register_wave_callback(this, pcm_callback);
      start_mixer_thread();
    }
    return BX_SOUNDLOW_OK;
  } else {
    return BX_SOUNDLOW_ERR;
  }
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
    if (type == BX_SOUNDFILE_VOC) {
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

      VOC_write_block(9, 12, temparray, length, data);
    } else {
      fwrite(data, 1, length, wavefile);
    }
    if (pcm_callback_id >= 0) {
      BX_MSLEEP(100);
    }
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_waveout_file_c::closewaveoutput()
{
  if (wavefile != NULL) {
    if (type == BX_SOUNDFILE_VOC) {
      fputc(0, wavefile);
    } else if (type == BX_SOUNDFILE_WAV) {
      Bit32u tracklen = ftell(wavefile);
      write_32bit(4, tracklen - 8);
      write_32bit(24, real_pcm_param.samplerate);
      write_32bit(28, (Bit32u)real_pcm_param.samplerate * 4);
      write_32bit(40, tracklen - 44);
    }
    fclose(wavefile);
    wavefile = NULL;
  }
  return BX_SOUNDLOW_OK;
}

/* Handlers for the VOC file output */

// Write the header of the VOC file.

void bx_soundlow_waveout_file_c::VOC_init_file()
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

  fwrite(&vocheader, 1, sizeof vocheader, wavefile);
}

// write one block to the VOC file
void bx_soundlow_waveout_file_c::VOC_write_block(int block, Bit32u headerlen,
                                Bit8u header[], Bit32u datalen, Bit8u data[])
{
  Bit32u i;

  if (block > 9) {
      BX_ERROR(("VOC Block %d not recognized, ignored.", block));
      return;
  }

  fputc(block, wavefile);

  i = headerlen + datalen;
#ifdef BX_LITTLE_ENDIAN
  fwrite(&i, 1, 3, wavefile); // write the length in 24-bit little endian
#else
  Bit8u lengthbytes[3];
  lengthbytes[0] = i & 0xff; i >>= 8;
  lengthbytes[1] = i & 0xff; i >>= 8;
  lengthbytes[2] = i & 0xff;
  fwrite(lengthbytes, 1, 3, wavefile);
#endif
  BX_DEBUG(("Voc block %d; Headerlen %d; Datalen %d",
            block, headerlen, datalen));
  if (headerlen > 0)
    fwrite(header, 1, headerlen, wavefile);
  if (datalen > 0)
    fwrite(data, 1, datalen, wavefile);
}

// bx_soundlow_midiout_file_c class implementation

bx_soundlow_midiout_file_c::bx_soundlow_midiout_file_c()
    :bx_soundlow_midiout_c()
{
  midifile = NULL;
  type = BX_SOUNDFILE_RAW;
}

bx_soundlow_midiout_file_c::~bx_soundlow_midiout_file_c()
{
  closemidioutput();
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

  size_t len = strlen(mididev);
  char ext[4];

  if ((midifile == NULL) && (len > 0)) {
    if (len > 4) {
      if (mididev[len-4] == '.') {
        strcpy(ext, mididev+len-3);
        if (!stricmp(ext, "mid")) {
          type = BX_SOUNDFILE_MID;
        }
      }
    }
    midifile = fopen(mididev, "wb");
    if (midifile == NULL) {
      BX_ERROR(("Failed to open MIDI output file %s.", mididev));
      return BX_SOUNDLOW_ERR;
    } else if (type == BX_SOUNDFILE_MID) {
      fwrite(&midiheader, 1, 14, midifile);
      fwrite(&trackheader, 1, 23, midifile);
    }
    return BX_SOUNDLOW_OK;
  } else {
    return BX_SOUNDLOW_ERR;
  }
}

int bx_soundlow_midiout_file_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  if (midifile != NULL) {
    if (type == BX_SOUNDFILE_MID) {
      writedeltatime(delta);
    }
    fputc(command, midifile);
    if ((command == 0xf0) ||
        (command == 0xf7))    // write event length for sysex/meta events
      writedeltatime(length);

    fwrite(data, 1, length, midifile);
  }
  return BX_SOUNDLOW_OK;
}

int bx_soundlow_midiout_file_c::closemidioutput()
{
  struct {
    Bit8u delta, statusbyte, metaevent, length;
  } metatrackend = { 0, 0xff, 0x2f, 0 };

  if (midifile != NULL) {
    if (type == BX_SOUNDFILE_MID) {
      // Meta event track end (0xff 0x2f 0x00) plus leading delta time
      fwrite(&metatrackend, 1, sizeof(metatrackend), midifile);

      int trlen = ftell(midifile);
      if (trlen < 0)
        BX_PANIC (("ftell failed in closemidioutput()"));
      Bit32u tracklen = (Bit32u)trlen;
      if (tracklen < 22)
        BX_PANIC (("MIDI track length too short"));
      tracklen -= 22;    // subtract the midi file and track header
      fseek(midifile, 22 - 4, SEEK_SET);
      // value has to be in big endian
#ifdef BX_LITTLE_ENDIAN
      tracklen = bx_bswap32(tracklen);
#endif
      fwrite(&tracklen, 4, 1, midifile);
    }
    fclose(midifile);
    midifile = NULL;
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

// bx_sound_oss_c class implementation

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
