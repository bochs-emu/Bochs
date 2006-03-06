/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.10 2006-03-06 22:32:03 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//

#ifndef BX_IODEV_SLOWDOWN_TIMER_H
#define BX_IODEV_SLOWDOWN_TIMER_H

class bx_slowdown_timer_c : public logfunctions {
private:
  struct {
    Bit64u start_time;
    Bit64u start_emulated_time;
    Bit64u lasttime;

    int timer_handle;

    float MAXmultiplier;
    Bit64u Q; // sleep rate in usec
  } s;

public:
  bx_slowdown_timer_c();

  void init(void);

  static void timer_handler(void * this_ptr);

  void handle_timer();

};

extern bx_slowdown_timer_c bx_slowdown_timer;

#endif
