/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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
 * Emulator of an Intel 8254/82C54 Programmable Interval Timer.
 * Greg Alexander <yakovlev@usa.com>
 */

#ifndef _PIT_82C54_H_
#define _PIT_82C54_H_ 1

typedef void (*out_handler_t)(bx_bool value);

class pit_82C54 : public logfunctions {
public:
  //Please do not use these.  They are public because they have to be
  // to compile on some platforms.  They are not to be used by other
  // classes.

  enum rw_status {
    LSByte=0,
    MSByte=1,
    LSByte_multiple=2,
    MSByte_multiple=3
  };

private:

  enum {
    MAX_COUNTER=2,
    MAX_ADDRESS=3,
    CONTROL_ADDRESS=3,
    MAX_MODE=5
  };

  enum real_RW_status {
    LSB_real=1,
    MSB_real=2,
    BOTH_real=3
  };

  enum problem_type {
    UNL_2P_READ=1
  };

  struct counter_type {
    //Chip IOs;
    bx_bool GATE; //GATE Input value at end of cycle
    bx_bool OUTpin; //OUT output this cycle

    //Architected state;
    Bit32u count; //Counter value this cycle
    Bit16u outlatch; //Output latch this cycle
    Bit16u inlatch; //Input latch this cycle
    Bit8u status_latch;

    //Status Register data;
    Bit8u rw_mode; //2-bit R/W mode from command word register.
    Bit8u mode; //3-bit mode from command word register.
    bx_bool bcd_mode; //1-bit BCD vs. Binary setting.
    bx_bool null_count; //Null count bit of status register.

    //Latch status data;
    bx_bool count_LSB_latched;
    bx_bool count_MSB_latched;
    bx_bool status_latched;

    //Miscelaneous State;
    Bit32u count_binary; //Value of the count in binary.
    bx_bool triggerGATE; //Whether we saw GATE rise this cycle.
    rw_status write_state; //Read state this cycle
    rw_status read_state; //Read state this cycle
    bx_bool count_written; //Whether a count written since programmed
    bx_bool first_pass; //Whether or not this is the first loaded count.
    bx_bool state_bit_1; //Miscelaneous state bits.
    bx_bool state_bit_2;
    Bit32u next_change_time; //Next time something besides count changes.
                             //0 means never.
    out_handler_t out_handler; // OUT pin callback (for IRQ0)
  };

  counter_type counter[3];

  Bit8u controlword;

  int seen_problems;

  void latch_counter(counter_type & thisctr);

  void set_OUT (counter_type & thisctr, bx_bool data);

  void set_count (counter_type & thisctr, Bit32u data) BX_CPP_AttrRegparmN(2);

  void set_count_to_binary (counter_type & thisctr) BX_CPP_AttrRegparmN(1);

  void set_binary_to_count (counter_type & thisctr) BX_CPP_AttrRegparmN(1);

  void decrement (counter_type & thisctr) BX_CPP_AttrRegparmN(1);

  void decrement_multiple(counter_type & thisctr, Bit32u cycles) BX_CPP_AttrRegparmN(2);

  void clock(Bit8u cnum) BX_CPP_AttrRegparmN(1);

  void print_counter(counter_type & thisctr);

public:
  pit_82C54 (void);
  void init (void);
  void reset (unsigned type);
  void register_state(bx_param_c *parent);

  void clock_all(Bit32u cycles);
  void clock_multiple(Bit8u cnum, Bit32u cycles);

  Bit8u read(Bit8u address);
  void write(Bit8u address, Bit8u data);

  void set_GATE(Bit8u cnum, bx_bool data);
  bx_bool read_GATE(Bit8u cnum);

  bx_bool read_OUT(Bit8u cnum);
  void set_OUT_handler(Bit8u cnum, out_handler_t outh);

  Bit32u get_clock_event_time(Bit8u cnum);
  Bit32u get_next_event_time(void);
  Bit16u get_inlatch(int countnum);
  bx_bool new_count_ready(int countnum);

  void print_cnum(Bit8u cnum);
};

#endif
