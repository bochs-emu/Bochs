/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2003       David N. Welton <davidw@dedasys.com>.
//  Copyright (C) 2003-2025  The Bochs Project
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

 /*
 * This file defines a class to deal with the speaker.
 * This class also tries to play beeps on the console
 * (linux only) and also forwards beeps to the gui
 * so we have the possiblity to signal the beep
 */

#ifndef BX_PC_SPEAKER_H
#define BX_PC_SPEAKER_H

#if BX_SUPPORT_SOUNDLOW
#define DSP_EVENT_BUFSIZE 4800
#endif

class bx_soundlow_waveout_c;

class bx_speaker_c : public bx_speaker_stub_c {
public:
    bx_speaker_c();
    virtual ~bx_speaker_c();

    virtual void init(void);
    virtual void reset(unsigned int);

    void beep_on(float frequency);
    void beep_off();
    void set_line(bool level);
#if BX_SUPPORT_SOUNDLOW
    Bit32u beep_generator(Bit16u rate, Bit8u *buffer, Bit32u len);
#if BX_HAVE_REALTIME_USEC
    Bit32u dsp_generator(Bit16u rate, Bit8u *buffer, Bit32u len);
#endif
    void set_volume(Bit8u volume) { beep_volume = volume; }
    static Bit64s speaker_param_handler(bx_param_c *param, bool set, Bit64s val);
#endif

private:
    float beep_frequency;  // 0 : beep is off
    unsigned output_mode;
#ifdef __linux__
    /* Do we have access?  If not, just skip everything else. */
    signed int consolefd;
    const static unsigned int clock_tick_rate = 1193180;
#elif defined(WIN32)
    Bit64u usec_start;
#endif
#if BX_SUPPORT_SOUNDLOW
  bx_soundlow_waveout_c *waveout;
  int beep_callback_id;
  bool beep_active;
  Bit16s beep_level;
  Bit8u beep_volume;
#if BX_HAVE_REALTIME_USEC
  bool dsp_active;
  Bit64u dsp_start_usec;
  Bit64u dsp_cb_usec;
  Bit32u dsp_count;
  Bit64u *dsp_event_buffer;
#endif
#endif
};

#endif
