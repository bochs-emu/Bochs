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

// Common code for sound lowlevel modules

#define BX_MAX_SOUND_DRIVERS 4

class bx_sound_lowlevel_c;
class bx_soundlow_waveout_c;

// Pseudo device that loads the lowlevel sound module
class bx_soundmod_ctl_c : public bx_soundmod_ctl_stub_c {
public:
  bx_soundmod_ctl_c();
  virtual ~bx_soundmod_ctl_c();
  virtual void init(void);
  virtual bx_soundlow_waveout_c* get_waveout(bx_bool using_file);
  virtual bx_soundlow_wavein_c* get_wavein();
  virtual bx_soundlow_midiout_c* get_midiout(bx_bool using_file);

private:
  bx_sound_lowlevel_c* get_driver(int driver_id);

  struct {
    int drv_id;
    bx_sound_lowlevel_c *module;
  } soundmod[BX_MAX_SOUND_DRIVERS];
  unsigned n_sound_drivers;

  bx_soundlow_waveout_c *waveout;
};
