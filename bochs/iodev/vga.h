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


#define CGA_TEXT_ADDR(row, column) (0x18000 + ((row)*80 + (column))*2)

#define X_TILESIZE 16
#define Y_TILESIZE 16
#define BX_NUM_X_TILES (640/X_TILESIZE)
#define BX_NUM_Y_TILES (((65536 / (640/8)) / Y_TILESIZE) + 1)
// #define BX_NUM_Y_TILES (480/Y_TILESIZE)

// Support varying number of rows of text.  This used to
// be limited to only 25 lines.
#define BX_MAX_TEXT_LINES 260

#if BX_USE_VGA_SMF
#  define BX_VGA_SMF  static
#  define BX_VGA_THIS bx_vga.
#else
#  define BX_VGA_SMF
#  define BX_VGA_THIS this->
#endif

class bx_vga_c : public logfunctions {
public:

  bx_vga_c(void);
  ~bx_vga_c(void);
  BX_VGA_SMF void   init(bx_devices_c *, bx_cmos_c *cmos);
  BX_VGA_SMF Bit8u  mem_read(Bit32u addr);
  // Note: either leave value of type Bit8u, or mask it when
  //       used to 8 bits, in memory.cc
  BX_VGA_SMF void   mem_write(Bit32u addr, Bit8u value);
  BX_VGA_SMF void   redraw_area(unsigned x0, unsigned y0,
                                unsigned width, unsigned height);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static void   write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

  struct {
    struct {
      Boolean color_emulation;  // 1=color emulation, base address = 3Dx
                                // 0=mono emulation,  base address = 3Bx
      Boolean enable_ram;       // enable CPU access to video memory if set
      Bit8u   clock_select;     // 0=25Mhz 1=28Mhz
      Boolean select_high_bank; // when in odd/even modes, select
                                // high 64k bank if set
      Boolean horiz_sync_pol;   // bit6: negative if set
      Boolean vert_sync_pol;    // bit7: negative if set
                                //   bit7,bit6 represent number of lines on display:
                                //   0 = reserved
                                //   1 = 400 lines
                                //   2 = 350 lines
                                //   3 - 480 lines
      } misc_output;

    struct {
      Bit8u   address;
      Bit8u   reg[0x19];
      } CRTC;

    struct {
      Boolean  flip_flop; /* 0 = address, 1 = data-write */
      unsigned address;  /* register number */
      Boolean  video_enabled;
      Bit8u    palette_reg[16];
      Bit8u    overscan_color;
      Bit8u    color_plane_enable;
      Bit8u    horiz_pel_panning;
      Bit8u    color_select;
      struct {
        Boolean graphics_alpha;
        Boolean display_type;
        Boolean enable_line_graphics;
        Boolean blink_intensity;
        Boolean pixel_panning_compat;
        Boolean pixel_clock_select;
        Boolean internal_palette_size;
        } mode_ctrl;
      } attribute_ctrl;

    struct {
      Bit8u write_data_register;
      Bit8u write_data_cycle; /* 0, 1, 2 */
      Bit8u read_data_register;
      Bit8u read_data_cycle; /* 0, 1, 2 */
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
      Boolean read_mode;
      Boolean odd_even;
      Boolean chain_odd_even;
      Bit8u   shift_reg;
      Boolean graphics_alpha;
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
      Boolean map_mask_bit[4];
      Boolean bit0;
      Boolean bit1;
      Bit8u   reg1;
      Bit8u   char_map_select;
      Boolean extended_mem;
      Boolean odd_even;
      Boolean chain_four;
      } sequencer;

    Boolean  vga_mem_updated;
    unsigned x_tilesize;
    unsigned y_tilesize;
    unsigned scan_bits;
    Boolean  vga_tile_updated[BX_NUM_X_TILES][BX_NUM_Y_TILES];
    Bit8u vga_memory[256 * 1024];
    Bit8u text_snapshot[2 * 80 * BX_MAX_TEXT_LINES]; // current text snapshot
    unsigned horiz_tick;
    unsigned vert_tick;
    Bit8u rgb[3 * 256];
    Bit8u tile[X_TILESIZE * Y_TILESIZE];
    } s;  // state information


  bx_devices_c *devices;

#if !BX_USE_VGA_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#else
  void write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#endif
  int timer_id;

  public:
  static void   timer_handler(void *);
  BX_VGA_SMF void   timer(void);
  BX_VGA_SMF void set_update_interval (unsigned interval);
  private:
  BX_VGA_SMF void   update(void);
  BX_VGA_SMF void   dump_status(void);
  BX_VGA_SMF void determine_screen_dimensions(unsigned *piHeight,
                                              unsigned *piWidth);
  };

extern bx_vga_c bx_vga;
