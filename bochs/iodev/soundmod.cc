/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2011  The Bochs Project
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

// Common sound module code and dummy sound output functions

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_SB16

#include "soundmod.h"
#include "soundlnx.h"
#include "soundosx.h"
#include "soundwin.h"

#define LOG_THIS dev->

bx_soundmod_ctl_c* theSoundModCtl = NULL;

int libsoundmod_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theSoundModCtl = new bx_soundmod_ctl_c;
  bx_devices.pluginSoundModCtl = theSoundModCtl;
  return(0); // Success
}

void libsoundmod_LTX_plugin_fini(void)
{
  delete theSoundModCtl;
}

int bx_soundmod_ctl_c::init_module(const char *type, void **module, logfunctions *dev)
{
  bx_sound_output_c **soundmod = (bx_sound_output_c**)module;

  if (!strcmp(type, "default")) {
    *soundmod = new BX_SOUND_OUTPUT_C(dev);
  } else if (!strcmp(type, "dummy")) {
    *soundmod = new bx_sound_output_c(dev);
  } else {
    BX_PANIC(("unknown sound module type '%s'", type));
    *soundmod = NULL;
  }
  return 0;
}

// The dummy sound output functions. They don't do anything.
bx_sound_output_c::bx_sound_output_c(logfunctions *dev)
{
  device = dev;
}

bx_sound_output_c::~bx_sound_output_c()
{
}

int bx_sound_output_c::waveready()
{
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::midiready()
{
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::openmidioutput(const char *mididev)
{
  UNUSED(mididev);
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UNUSED(delta);
  UNUSED(command);
  UNUSED(length);
  UNUSED(data);
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::closemidioutput()
{
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::openwaveoutput(const char *wavedev)
{
  UNUSED(wavedev);
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::startwaveplayback(int frequency, int bits, int stereo, int format)
{
  UNUSED(frequency);
  UNUSED(bits);
  UNUSED(stereo);
  UNUSED(format);
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::sendwavepacket(int length, Bit8u data[])
{
  UNUSED(length);
  UNUSED(data);
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::stopwaveplayback()
{
  return BX_SOUND_OUTPUT_OK;
}

int bx_sound_output_c::closewaveoutput()
{
  return BX_SOUND_OUTPUT_OK;
}

#endif
