/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  High Precision Event Timer emulation ported from Qemu
//
//  Copyright (c) 2007 Alexander Graf
//  Copyright (c) 2008 IBM Corporation
//
//  Authors: Beth Kon <bkon@us.ibm.com>
//
//  Copyright (C) 2017-2021  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_IODEV_HPET_H
#define BX_IODEV_HPET_H

#if BX_SUPPORT_PCI

/* HPET will set up timers to fire after a certain period of time.
 * These values can be used to clamp this period to reasonable/supported values.
 * The values are in ticks.
 */
#define HPET_MAX_ALLOWED_PERIOD 0x0400000000000000ull // Must not overflow when multiplied by HPET_CLK_PERIOD
#define HPET_MIN_ALLOWED_PERIOD 1

#define RTC_ISA_IRQ 8

#define HPET_BASE               0xfed00000
#define HPET_LEN                0x400
#define HPET_CLK_PERIOD         10 // 10 ns
#define HPET_ROUTING_CAP        0xffffffull // allowed irq routing

#define FS_PER_NS 1000000       /* 1000000 femtosectons == 1 ns */
#define HPET_MIN_TIMERS         3
#define HPET_MAX_TIMERS         32

#define HPET_CFG_ENABLE 0x001
#define HPET_CFG_LEGACY 0x002

#define HPET_ID         0x000
#define HPET_PERIOD     0x004
#define HPET_CFG        0x010
#define HPET_STATUS     0x020
#define HPET_COUNTER    0x0f0
#define HPET_TN_CFG     0x000
#define HPET_TN_CMP     0x008
#define HPET_TN_ROUTE   0x010
#define HPET_CFG_WRITE_MASK  0x3

#define HPET_TN_TYPE_LEVEL       0x002
#define HPET_TN_ENABLE           0x004
#define HPET_TN_PERIODIC         0x008
#define HPET_TN_PERIODIC_CAP     0x010
#define HPET_TN_SIZE_CAP         0x020
#define HPET_TN_SETVAL           0x040
#define HPET_TN_32BIT            0x100
#define HPET_TN_INT_ROUTE_MASK  0x3e00
#define HPET_TN_FSB_ENABLE      0x4000
#define HPET_TN_CFG_WRITE_MASK  0x7f4e
#define HPET_TN_INT_ROUTE_SHIFT      9

typedef struct {
  Bit8u  tn;
  int    timer_id;
  Bit64u config;
  Bit64u cmp;
  Bit64u fsb;
  Bit64u period;
  Bit64u last_checked;
} HPETTimer;

class bx_hpet_c : public bx_devmodel_c {
public:
  bx_hpet_c();
  virtual ~bx_hpet_c();
  virtual void init();
  virtual void reset(unsigned type);
  virtual void register_state(void);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

  Bit32u read_aligned(bx_phy_address address);
  void write_aligned(bx_phy_address address, Bit32u data, bool trailing_write);

private:
  Bit32u hpet_in_legacy_mode(void) {return s.config & HPET_CFG_LEGACY;}
  Bit32u timer_int_route(HPETTimer *timer)
  {
    return (timer->config & HPET_TN_INT_ROUTE_MASK) >> HPET_TN_INT_ROUTE_SHIFT;
  }
  Bit32u timer_fsb_route(HPETTimer *t) {return t->config & HPET_TN_FSB_ENABLE;}
  Bit32u hpet_enabled(void) {return s.config & HPET_CFG_ENABLE;}
  Bit32u timer_is_periodic(HPETTimer *t) {return t->config & HPET_TN_PERIODIC;}
  Bit32u timer_enabled(HPETTimer *t) {return t->config & HPET_TN_ENABLE;}
  Bit64u hpet_get_ticks(void);
  Bit64u hpet_calculate_diff(HPETTimer *t, Bit64u current);
  void   update_irq(HPETTimer *timer, bool set);
  void   hpet_set_timer(HPETTimer *t);
  void   hpet_del_timer(HPETTimer *t);

  static void timer_handler(void *);
  void   hpet_timer(void);

  struct {
    Bit8u  num_timers;
    Bit64u hpet_reference_value;
    Bit64u hpet_reference_time;
    Bit64u capability;
    Bit64u config;
    Bit64u isr;
    Bit64u hpet_counter;
    HPETTimer timer[HPET_MAX_TIMERS];
  } s;
};

#endif

#endif
