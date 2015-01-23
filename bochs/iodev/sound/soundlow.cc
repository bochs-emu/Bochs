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

#include "soundlow.h"


// The dummy sound lowlevel functions. They don't do anything.
bx_sound_lowlevel_c::bx_sound_lowlevel_c()
{
  put("soundlow", "SNDLOW");
  record_timer_index = BX_NULL_TIMER_HANDLE;
  cb_count = 0;
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

int bx_sound_lowlevel_c::sendwavepacket(int length, Bit8u data[], bx_pcm_param_t *param)
{
  if (memcmp(param, &pcm_param, sizeof(bx_pcm_param_t)) != 0) {
    startwaveplayback(param->samplerate, param->bits, param->channels == 2,
                      param->format);
    pcm_param = *param;
  }
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
