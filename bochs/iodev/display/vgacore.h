/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2018  The Bochs Project
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

#ifndef BX_IODEV_VGACORE_H
#define BX_IODEV_VGACORE_H

// Make colour
#define MAKE_COLOUR(red, red_shiftfrom, red_shiftto, red_mask, \
                    green, green_shiftfrom, green_shiftto, green_mask, \
                    blue, blue_shiftfrom, blue_shiftto, blue_mask) \
( \
 ((((red_shiftto) > (red_shiftfrom)) ? \
  (red) << ((red_shiftto) - (red_shiftfrom)) : \
  (red) >> ((red_shiftfrom) - (red_shiftto))) & \
  (red_mask)) | \
 ((((green_shiftto) > (green_shiftfrom)) ? \
  (green) << ((green_shiftto) - (green_shiftfrom)) : \
  (green) >> ((green_shiftfrom) - (green_shiftto))) & \
  (green_mask)) | \
 ((((blue_shiftto) > (blue_shiftfrom)) ? \
  (blue) << ((blue_shiftto) - (blue_shiftfrom)) : \
  (blue) >> ((blue_shiftfrom) - (blue_shiftto))) & \
  (blue_mask)) \
)

#define X_TILESIZE 16
#define Y_TILESIZE 24

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, do nothing.
#define SET_TILE_UPDATED(thisp, xtile, ytile, value)                          \
  do {                                                                        \
    if (((xtile) < thisp s.num_x_tiles) && ((ytile) < thisp s.num_y_tiles))   \
      thisp s.vga_tile_updated[(xtile)+(ytile)* thisp s.num_x_tiles] = value; \
  } while (0)

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, return 0.
#define GET_TILE_UPDATED(xtile,ytile)                        \
  ((((xtile) < s.num_x_tiles) && ((ytile) < s.num_y_tiles))? \
     s.vga_tile_updated[(xtile)+(ytile)* s.num_x_tiles]      \
     : 0)

typedef struct {
  Bit16u htotal;
  Bit16u vtotal;
  Bit16u vrstart;
} bx_crtc_params_t;

#if BX_SUPPORT_PCI
class bx_nonvga_device_c : public bx_pci_device_c {
public:
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height) {}
  virtual void refresh_display(void *this_ptr, bx_bool redraw) {}
  virtual void update(void) {}
};
#endif

class bx_vgacore_c : public bx_vga_stub_c {
public:
  bx_vgacore_c();
  virtual ~bx_vgacore_c();
  virtual void   init(void);
  virtual void   reset(unsigned type) {}
  static bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  static bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  virtual Bit8u  mem_read(bx_phy_address addr);
  virtual void   mem_write(bx_phy_address addr, Bit8u value);
  virtual void   set_override(bx_bool enabled, void *dev);
  virtual void   register_state(bx_list_c *parent);
  virtual void   after_restore_state(void);
#if BX_DEBUGGER
  virtual void   debug_dump(void);
#endif

  virtual void   vga_redraw_area(unsigned x0, unsigned y0, unsigned width,
                                 unsigned height);
  virtual void   redraw_area(unsigned x0, unsigned y0, unsigned width,
                             unsigned height);
  virtual void   refresh_display(void *this_ptr, bx_bool redraw);
  virtual void   get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight,
                                   unsigned *txWidth);
  virtual bx_bool init_vga_extension(void) {return 0;}
  virtual void   get_crtc_params(bx_crtc_params_t *crtcp);

  static void    vga_timer_handler(void *);
  static Bit64s  vga_param_handler(bx_param_c *param, int set, Bit64s val);

protected:
  void init_standard_vga(void);
  void init_gui(void);
  void init_iohandlers(bx_read_handler_t f_read, bx_write_handler_t f_write);
  void init_systemtimer();

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log);

  Bit8u get_vga_pixel(Bit16u x, Bit16u y, Bit16u saddr, Bit16u lc, bx_bool bs, Bit8u **plane);
  virtual void update(void);
  void determine_screen_dimensions(unsigned *piHeight, unsigned *piWidth);
  void calculate_retrace_timing(void);
  bx_bool skip_update(void);

  struct {
    struct {
      bx_bool color_emulation;  // 1=color emulation, base address = 3Dx
                                // 0=mono emulation,  base address = 3Bx
      bx_bool enable_ram;       // enable CPU access to video memory if set
      Bit8u   clock_select;     // 0=25Mhz 1=28Mhz
      bx_bool select_high_bank; // when in odd/even modes, select
                                // high 64k bank if set
      bx_bool horiz_sync_pol;   // bit6: negative if set
      bx_bool vert_sync_pol;    // bit7: negative if set
                                //   bit7,bit6 represent number of lines on display:
                                //   0 = reserved
                                //   1 = 400 lines
                                //   2 = 350 lines
                                //   3 - 480 lines
    } misc_output;

    struct {
      Bit8u   address;
      Bit8u   reg[0x19];
      bx_bool write_protect;
    } CRTC;

    struct {
      bx_bool  flip_flop; /* 0 = address, 1 = data-write */
      unsigned address;  /* register number */
      bx_bool  video_enabled;
      Bit8u    palette_reg[16];
      Bit8u    overscan_color;
      Bit8u    color_plane_enable;
      Bit8u    horiz_pel_panning;
      Bit8u    color_select;
      struct {
        bx_bool graphics_alpha;
        bx_bool display_type;
        bx_bool enable_line_graphics;
        bx_bool blink_intensity;
        bx_bool pixel_panning_compat;
        bx_bool pixel_clock_select;
        bx_bool internal_palette_size;
      } mode_ctrl;
    } attribute_ctrl;

    struct {
      Bit8u write_data_register;
      Bit8u write_data_cycle; /* 0, 1, 2 */
      Bit8u read_data_register;
      Bit8u read_data_cycle; /* 0, 1, 2 */
      Bit8u dac_state;
      struct {
        Bit8u red;
        Bit8u green;
        Bit8u blue;
      } data[256];
      Bit8u mask;
    } pel;

    struct {
      Bit8u   index;
      Bit8u   set_reset;
      Bit8u   enable_set_reset;
      Bit8u   color_compare;
      Bit8u   data_rotate;
      Bit8u   raster_op;
      Bit8u   read_map_select;
      Bit8u   write_mode;
      Bit32u  read_mode;
      bx_bool odd_even;
      bx_bool chain_odd_even;
      Bit8u   shift_reg;
      bx_bool graphics_alpha;
      Bit8u   memory_mapping; /* 0 = use A0000-BFFFF
                               * 1 = use A0000-AFFFF EGA/VGA graphics modes
                               * 2 = use B0000-B7FFF Monochrome modes
                               * 3 = use B8000-BFFFF CGA modes
                               */
      Bit8u   color_dont_care;
      Bit8u   bitmask;
      Bit8u   latch[4];
    } graphics_ctrl;

    struct {
      Bit8u   index;
      Bit8u   map_mask;
      bx_bool reset1;
      bx_bool reset2;
      Bit8u   reg1;
      Bit8u   char_map_select;
      bx_bool extended_mem;
      bx_bool odd_even;
      bx_bool chain_four;
      bx_bool clear_screen;
    } sequencer;

    bx_bool  vga_enabled;
    bx_bool  vga_mem_updated;
    unsigned line_offset;
    unsigned line_compare;
    unsigned vertical_display_end;
    unsigned blink_counter;
    bx_bool  *vga_tile_updated;
    Bit8u *memory;
    Bit32u memsize;
    Bit8u text_snapshot[128 * 1024]; // current text snapshot
    Bit8u tile[X_TILESIZE * Y_TILESIZE * 4]; /**< Currently allocates the tile as large as needed. */
    Bit16u charmap_address;
    bx_bool x_dotclockdiv2;
    bx_bool y_doublescan;
    // h/v retrace timing
    Bit32u vclk[4];
    Bit32u htotal_usec;
    Bit32u hbstart_usec;
    Bit32u hbend_usec;
    Bit32u vtotal_usec;
    Bit32u vblank_usec;
    Bit32u vrstart_usec;
    Bit32u vrend_usec;
    // shift values for extensions
    Bit8u  plane_shift;
    Bit32u plane_offset;
    Bit8u  dac_shift;
    // last active resolution and bpp
    Bit16u last_xres;
    Bit16u last_yres;
    Bit8u last_bpp;
    Bit8u last_fw;
    Bit8u last_fh;
    // maximum resolution and number of tiles
    Bit16u max_xres;
    Bit16u max_yres;
    Bit16u num_x_tiles;
    Bit16u num_y_tiles;
    // vga override mode
    bx_bool vga_override;
#if BX_SUPPORT_PCI
    bx_nonvga_device_c *nvgadev;
#endif
  } s;  // state information

  int timer_id;
  bx_bool update_realtime;
  bx_bool vsync_realtime;
  bx_param_string_c *vgaext;
  bx_bool pci_enabled;
};

#endif
