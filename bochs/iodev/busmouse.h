/////////////////////////////////////////////////////////////////////////
// $Id: busmouse.h,v 1.4 2006-05-27 15:54:48 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004  MandrakeSoft S.A.
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


#if BX_SUPPORT_BUSMOUSE

#ifndef _PCBUSM_H
#define _PCBUSM_H

// these keywords should only be used in busmouse.cc
#if BX_USE_BUSM_SMF
#  define BX_BUSM_SMF  static
#  define BX_BUSM_THIS theBusMouse->
#else
#  define BX_BUSM_SMF
#  define BX_BUSM_THIS 
#endif

#define BUS_MOUSE_IRQ  5

#define PORT_CONTROL     0x023C
#define PORT_DATA        0x023D
#define PORT_SIGNATURE   0x023E
#define PORT_CONFIG      0x023F

class bx_busm_c : public bx_busm_stub_c {
public:
  bx_busm_c();
  virtual ~bx_busm_c();

  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void bus_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state);
#if BX_SUPPORT_SAVE_RESTORE
  virtual void register_state(void);
#endif

private:
  static void timer_handler(void *);
  void busm_timer(void);
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_BUSM_SMF
  void     write(Bit32u   address, Bit32u   value, unsigned io_len);
  Bit32u   read(Bit32u   address, unsigned io_len);
#endif

  int   timer_index; // our timer index

  int   mouse_delayed_dx;
  int   mouse_delayed_dy;
  Bit8u current_x, current_y, current_b;

  // signature port
  Bit8u sig_port_sequ;   // bit0: 0 or 1.  A register read rotates between two values? 0xDE and Hardware ID?

  // D7    =  Mode set flag (1 = active)
  // D6,D5 =  Mode selection (port A)
  //          00 = Mode 0 = Basic I/O
  //          01 = Mode 1 = Strobed I/O
  //          10 = Mode 2 = Bi-dir bus
  // D4    =  Port A direction (1 = input)
  // D3    =  Port C (upper 4 bits)
  //          direction. (1 = input)
  // D2    =  Mode selection (port B & C)
  //          0 = Mode 0 = Basic I/O
  //          1 = Mode 1 = Strobed I/O
  // D1    =  Port B direction (1 = input)
  // D0    =  Port C (lower 4 bits)
  //          direction. (1 = input)
  Bit8u control_val;
  struct {
    bx_bool mode_set;
    Bit8u   modeA_select;
    bx_bool portA_dir;       // 1 = input
    bx_bool portC_upper_dir; // 1 = input
    bx_bool modeBC_select;
    bx_bool portB_dir;       // 1 = input
    bx_bool portC_lower_dir; // 1 = input
  } control;


  bx_bool interrupts;    // 0 or 1.  interrupts off or on.
  bx_bool packet_update; // 0 or 1.  allow the mouse to update the packet?

  Bit8u   cur_command;   // current command
  Bit8u   command_val;   // current command val
};


#endif  // #ifndef _PCBUSM_H

#endif  // BX_SUPPORT_BUSMOUSE
