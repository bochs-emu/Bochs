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

// Sound driver loader code

class bx_sound_lowlevel_c;
class bx_soundlow_waveout_c;
class bx_soundlow_wavein_c;
class bx_soundlow_midiout_c;

// Pseudo device that loads the lowlevel sound module
class BOCHSAPI bx_soundmod_ctl_c : public logfunctions {
public:
  bx_soundmod_ctl_c();
  ~bx_soundmod_ctl_c() {}
  void init(void);
  void exit(void);
  bx_bool register_driver(bx_sound_lowlevel_c *module, int driver_id);
  bx_soundlow_waveout_c* get_waveout(bx_bool using_file);
  bx_soundlow_wavein_c* get_wavein();
  bx_soundlow_midiout_c* get_midiout(bx_bool using_file);

private:
  bx_sound_lowlevel_c* get_driver(int driver_id);
};

BOCHSAPI extern bx_soundmod_ctl_c bx_soundmod_ctl;
