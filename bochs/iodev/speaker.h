/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright 2003 by David N. Welton <davidw@dedasys.com>.
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

class bx_speaker_c : public bx_speaker_stub_c {
public:
    bx_speaker_c();
    virtual ~bx_speaker_c();

    virtual void init(void);
    virtual void reset(unsigned int);

    void beep_on(float frequency);
    void beep_off();
private:
    float beep_frequency;  // 0 : beep is off
    bx_bool outputinit;
    unsigned output_mode;
#ifdef __linux__
    /* Do we have access?  If not, just skip everything else. */
    signed int consolefd;
    const static unsigned int clock_tick_rate = 1193180;
#elif defined(WIN32)
    Bit64u usec_start;
#endif
};

#endif
