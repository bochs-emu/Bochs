//  Copyright (C) 2001  MandrakeSoft S.A.
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

#ifndef _BX_PIT_H
#define _BX_PIT_H


#if BX_USE_PIT_SMF
#  define BX_PIT_SMF  static
#  define BX_PIT_THIS bx_pit.
#else
#  define BX_PIT_SMF
#  define BX_PIT_THIS this->
#endif

#ifdef OUT
#  undef OUT
#endif


typedef struct {
  Bit8u      mode;
  Bit8u      latch_mode;
  Bit16u     input_latch_value;
  Boolean    input_latch_toggle;
  Bit16u     output_latch_value;
  Boolean    output_latch_toggle;
  Boolean    output_latch_full;
  Bit16u     counter_max;
  Bit16u     counter;
  Boolean    bcd_mode;
  Boolean    active;
  Boolean    GATE;     // GATE input  pin
  Boolean    OUT;      // OUT  output pin
  } bx_pit_t;




class bx_pit_c : public logfunctions {
public:
  bx_pit_c( void );
  ~bx_pit_c( void );
  BX_PIT_SMF int init( bx_devices_c *);
  BX_PIT_SMF Boolean periodic( Bit32u   usec_delta );

  BX_PIT_SMF int SaveState( class state_file *fd );
  BX_PIT_SMF int LoadState( class state_file *fd );

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PIT_SMF
  Bit32u   read( Bit32u   addr, unsigned int len );
  void write( Bit32u   addr, Bit32u   Value, unsigned int len );
#endif

  struct {
    bx_pit_t timer[3];
    Bit8u   speaker_data_on;
    Boolean refresh_clock_div2;
    int  timer_handle[3];
    } s;

  bx_devices_c *devices;

  BX_PIT_SMF void  write_count_reg( Bit8u   value, unsigned timerid );
  BX_PIT_SMF Bit8u read_counter( unsigned timerid );
  BX_PIT_SMF void  latch( unsigned timerid );
  BX_PIT_SMF void  set_GATE(unsigned pit_id, unsigned value);
  BX_PIT_SMF void  start(unsigned timerid);
  };

extern bx_pit_c bx_pit;

#endif  // #ifndef _BX_PIT_H
