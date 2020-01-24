/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2020  The Bochs Project
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
#define BX_VVGA_THIS theVoodooVga->
#define BX_VVGA_THIS_PTR theVoodooVga

typedef struct {
  Bit8u model;
  struct {
    Bit32u width;
    Bit32u height;
    Bit64u htotal_usec;
    Bit64u vtotal_usec;
    Bit64u hsync_usec;
    Bit64u vsync_usec;
    double htime_to_pixel;
    Bit64u frame_start;
    bx_bool clock_enabled;
    bx_bool output_on;
    bx_bool override_on;
    bx_bool screen_update_pending;
    bx_bool gui_update_pending;
  } vdraw;
  int mode_change_timer_id;
  int vertical_timer_id;
  Bit8u devfunc;
  Bit16u max_xres;
  Bit16u max_yres;
  Bit16u num_x_tiles;
  Bit16u num_y_tiles;
  bx_bool  *vga_tile_updated;
} bx_voodoo_t;


class bx_voodoo_base_c : public bx_nonvga_device_c {
public:
  bx_voodoo_base_c();
  virtual ~bx_voodoo_base_c();
  virtual void init(void);
  virtual void init_model(void) {}
  virtual void register_state(void) {}

  virtual void refresh_display(void *this_ptr, bx_bool redraw);
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height);
  virtual void update(void);
  virtual bx_bool update_timing(void) {return 0;}
  virtual Bit32u get_retrace(bx_bool hv) {return 0;}

  virtual void output_enable(bx_bool enabled) {}
  virtual void update_screen_start(void) {}

  virtual void reg_write(Bit32u reg, Bit32u value);
  virtual void blt_reg_write(Bit8u reg, Bit32u value) {}
  virtual void mem_write_linear(Bit32u offset, Bit32u value, unsigned len) {}
  virtual void draw_hwcursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info) {}
  virtual void set_tile_updated(unsigned xti, unsigned yti, bx_bool flag) {}

  static void vertical_timer_handler(void *);

protected:
  bx_voodoo_t s;

  void register_state(bx_list_c *parent);
  void set_irq_level(bx_bool level);
  void vertical_timer(void);
};

class bx_voodoo_1_2_c : public bx_voodoo_base_c {
public:
  bx_voodoo_1_2_c();
  virtual ~bx_voodoo_1_2_c();
  virtual void init_model(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual bx_bool update_timing(void);
  virtual Bit32u get_retrace(bx_bool hv);

  virtual void output_enable(bx_bool enabled);
  virtual void update_screen_start(void);

  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

private:
  static bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);

  static void mode_change_timer_handler(void *);
  void mode_change_timer(void);
};

class bx_banshee_c : public bx_voodoo_base_c {
public:
  bx_banshee_c();
  virtual ~bx_banshee_c();
  virtual void init_model(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual bx_bool update_timing(void);
  virtual Bit32u get_retrace(bx_bool hv);

  virtual void reg_write(Bit32u reg, Bit32u value);
  virtual void blt_reg_write(Bit8u reg, Bit32u value);
  virtual void mem_write_linear(Bit32u offset, Bit32u value, unsigned len);
  virtual void draw_hwcursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info);
  virtual void set_tile_updated(unsigned xti, unsigned yti, bx_bool flag);

  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  Bit32u blt_reg_read(Bit8u reg);

private:
  static bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);

  void mem_read(bx_phy_address addr, unsigned len, void *data);
  void mem_write(bx_phy_address addr, unsigned len, void *data);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);

  Bit32u agp_reg_read(Bit8u reg);
  void   agp_reg_write(Bit8u reg, Bit32u value);

  void   blt_launch_area_setup(void);
  void   blt_launch_area_write(Bit32u value);
  void   blt_execute(void);
  void   blt_complete(void);
  bx_bool blt_apply_clipwindow(int *x0, int *y0, int *x1, int *y1, int *w, int *h);

  void   blt_rectangle_fill(void);
  void   blt_pattern_fill_mono(void);
  void   blt_pattern_fill_color(void);
  void   blt_screen_to_screen(void);
  void   blt_screen_to_screen_pattern(void);
  void   blt_screen_to_screen_stretch(void);
  void   blt_host_to_screen(void);
  void   blt_host_to_screen_pattern(void);
  void   blt_line(bx_bool pline);

  bx_ddc_c ddc;
  bx_bool  is_agp;
};

class bx_voodoo_vga_c : public bx_vgacore_c {
public:
  bx_voodoo_vga_c();
  virtual ~bx_voodoo_vga_c();

  virtual void   reset(unsigned type);
  virtual void   register_state(void);
  virtual void   after_restore_state(void);

  virtual Bit8u  mem_read(bx_phy_address addr);
  virtual void   mem_write(bx_phy_address addr, Bit8u value);

  virtual void   redraw_area(unsigned x0, unsigned y0,
                             unsigned width, unsigned height);

  virtual bx_bool init_vga_extension(void);
  virtual void   get_crtc_params(bx_crtc_params_t *crtcp);
  Bit32u get_retrace(void);

  void banshee_update_mode(void);
  void banshee_set_dac_mode(bx_bool mode);
  void banshee_set_vclk3(Bit32u value);

  static Bit32u banshee_vga_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   banshee_vga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

protected:
  virtual void  update(void);
};

extern bx_voodoo_base_c* theVoodooDevice;
extern bx_voodoo_vga_c* theVoodooVga;

#endif
