/////////////////////////////////////////////////////////////////////////
// $Id: pit82c54.h,v 1.9 2001-10-03 13:10:38 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
/*
 * Emulator of an Intel 8254/82C54 Programmable Interval Timer.
 * Greg Alexander <yakovlev@usa.com>
 *
 * This code is not yet linked into Bochs, but has been included so
 * that you can experiment with it.  (bbd)
 */

#ifndef _PIT_82C54_H_
#define _PIT_82C54_H_ 1

#include "bochs.h"


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
    bool GATE; //GATE Input value at end of cycle
    bool OUTpin; //OUT output this cycle

    //Architected state;
    Bit32u count; //Counter value this cycle
    Bit16u outlatch; //Output latch this cycle
    Bit16u inlatch; //Input latch this cycle
    Bit8u status_latch;

    //Status Register data;
    Bit8u rw_mode; //2-bit R/W mode from command word register.
    Bit8u mode; //3-bit mode from command word register.
    bool bcd_mode; //1-bit BCD vs. Binary setting.
    bool null_count; //Null count bit of status register.

    //Latch status data;
    bool count_LSB_latched;
    bool count_MSB_latched;
    bool status_latched;

    //Miscelaneous State;
    Bit32u count_binary; //Value of the count in binary.
    bool triggerGATE; //Whether we saw GATE rise this cycle.
    rw_status write_state; //Read state this cycle
    rw_status read_state; //Read state this cycle
    bool count_written; //Whether a count written since programmed
    bool first_pass; //Whether or not this is the first loaded count.
    bool state_bit_1; //Miscelaneous state bits.
    bool state_bit_2;
    Bit32u next_change_time; //Next time something besides count changes.
                             //0 means never.
  };

  counter_type counter[3];

  Bit8u controlword;

  int seen_problems;

  void latch_counter(counter_type & thisctr);

  void set_OUT (counter_type & thisctr, bool data);

  void set_count (counter_type & thisctr, Bit32u data);

  void set_count_to_binary (counter_type & thisctr);

  void set_binary_to_count (counter_type & thisctr);

  void decrement (counter_type & thisctr);

  void decrement_multiple(counter_type & thisctr, Bit32u cycles);

  void clock(Bit8u cnum);

  void print_counter(counter_type & thisctr);

public:
  void init (void);
  pit_82C54 (void);

  void clock_all(Bit32u cycles);
  void clock_multiple(Bit8u cnum, Bit32u cycles);

  Bit8u read(Bit8u address);
  void write(Bit8u address, Bit8u data);

  void set_GATE(Bit8u cnum, bool data);
  bool read_GATE(Bit8u cnum);

  bool read_OUT(Bit8u cnum);

  Bit32u get_clock_event_time(Bit8u cnum);
  Bit32u get_next_event_time(void);

};

#endif
