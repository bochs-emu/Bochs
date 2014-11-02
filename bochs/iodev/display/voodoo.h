/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2014  The Bochs Project
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

#ifndef BX_IODEV_VOODOO_H
#define BX_IODEV_VOODOO_H

#define BX_VOODOO_THIS theVoodooDevice->
#define BX_VOODOO_THIS_PTR theVoodooDevice

typedef struct {
  struct {
    Bit32u width;
    Bit32u height;
    Bit64u htotal_usec;
    Bit64u vtotal_usec;
    Bit64u vsync_usec;
    Bit64u frame_start;
    bx_bool clock_enabled;
    bx_bool output_on;
    bx_bool override_on;
    bx_bool screen_update_pending;
  } vdraw;
  int mode_change_timer_id;
  int update_timer_id;
  Bit8u devfunc;
} bx_voodoo_t;


class bx_voodoo_c : public bx_nonvga_device_c, bx_pci_device_stub_c {
public:
  bx_voodoo_c();
  virtual ~bx_voodoo_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual void refresh_display(void *this_ptr, bx_bool redraw);
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static Bit16u get_retrace(void);
  static void output_enable(bx_bool enabled);
  static void update_screen_start(void);
  static bx_bool update_timing(void);

private:
  bx_voodoo_t s;

  static void    set_irq_level(bx_bool level);

  static bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);

  static void mode_change_timer_handler(void *);
  static void update_timer_handler(void *);
  static void update(void);
};

#endif
