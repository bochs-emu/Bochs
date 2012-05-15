/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2012  The Bochs Project
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
//

#ifndef BX_IODEV_CMOS_H
#define BX_IODEV_CMOS_H

#if BX_USE_CMOS_SMF
#  define BX_CMOS_SMF  static
#  define BX_CMOS_THIS theCmosDevice->
#else
#  define BX_CMOS_SMF
#  define BX_CMOS_THIS this->
#endif


class bx_cmos_c : public bx_cmos_stub_c {
public:
  bx_cmos_c();
  virtual ~bx_cmos_c();

  virtual void init(void);
  virtual void checksum_cmos(void);
  virtual void reset(unsigned type);
  virtual void save_image(void);
  virtual void register_state(void);
  virtual void after_restore_state(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

  virtual Bit32u get_reg(unsigned reg) {
    return s.reg[reg];
  }
  virtual void set_reg(unsigned reg, Bit32u val) {
    s.reg[reg] = val;
  }
  virtual time_t get_timeval() {
    return s.timeval;
  }

  struct {
    int     periodic_timer_index;
    Bit32u  periodic_interval_usec;
    int     one_second_timer_index;
    int     uip_timer_index;
    time_t  timeval;
    Bit8u   cmos_mem_address;
    bx_bool timeval_change;
    bx_bool rtc_mode_12hour;
    bx_bool rtc_mode_binary;
    bx_bool rtc_sync;

    Bit8u   reg[128];
  } s;  // state information

private:
  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_CMOS_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned len);
#endif

public:
  static void periodic_timer_handler(void *);
  static void one_second_timer_handler(void *);
  static void uip_timer_handler(void *);
  BX_CMOS_SMF void periodic_timer(void);
  BX_CMOS_SMF void one_second_timer(void);
  BX_CMOS_SMF void uip_timer(void);
private:
  BX_CMOS_SMF void update_clock(void);
  BX_CMOS_SMF void update_timeval(void);
  BX_CMOS_SMF void CRA_change(void);
};

#endif
