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
/////////////////////////////////////////////////////////////////////////

#ifndef BX_IODEV_VGA_H
#define BX_IODEV_VGA_H

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

// Bochs VBE definitions

#define VBE_DISPI_TOTAL_VIDEO_MEMORY_MB  16
#define VBE_DISPI_4BPP_PLANE_SHIFT       22

#define VBE_DISPI_BANK_ADDRESS           0xA0000
#define VBE_DISPI_BANK_SIZE_KB           64

#define VBE_DISPI_MAX_XRES               2560
#define VBE_DISPI_MAX_YRES               1600
#define VBE_DISPI_MAX_BPP                32

#define VBE_DISPI_IOPORT_INDEX           0x01CE
#define VBE_DISPI_IOPORT_DATA            0x01CF

#define VBE_DISPI_INDEX_ID               0x0
#define VBE_DISPI_INDEX_XRES             0x1
#define VBE_DISPI_INDEX_YRES             0x2
#define VBE_DISPI_INDEX_BPP              0x3
#define VBE_DISPI_INDEX_ENABLE           0x4
#define VBE_DISPI_INDEX_BANK             0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH       0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT      0x7
#define VBE_DISPI_INDEX_X_OFFSET         0x8
#define VBE_DISPI_INDEX_Y_OFFSET         0x9
#define VBE_DISPI_INDEX_VIDEO_MEMORY_64K 0xa

#define VBE_DISPI_ID0                    0xB0C0
#define VBE_DISPI_ID1                    0xB0C1
#define VBE_DISPI_ID2                    0xB0C2
#define VBE_DISPI_ID3                    0xB0C3
#define VBE_DISPI_ID4                    0xB0C4
#define VBE_DISPI_ID5                    0xB0C5

#define VBE_DISPI_BPP_4                  0x04
#define VBE_DISPI_BPP_8                  0x08
#define VBE_DISPI_BPP_15                 0x0F
#define VBE_DISPI_BPP_16                 0x10
#define VBE_DISPI_BPP_24                 0x18
#define VBE_DISPI_BPP_32                 0x20

#define VBE_DISPI_DISABLED               0x00
#define VBE_DISPI_ENABLED                0x01
#define VBE_DISPI_GETCAPS                0x02
#define VBE_DISPI_8BIT_DAC               0x20
#define VBE_DISPI_LFB_ENABLED            0x40
#define VBE_DISPI_NOCLEARMEM             0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS   0xE0000000

#define VBE_DISPI_TOTAL_VIDEO_MEMORY_KB  (VBE_DISPI_TOTAL_VIDEO_MEMORY_MB * 1024)
#define VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB * 1024)

// End Bochs VBE definitions

#define X_TILESIZE 16
#define Y_TILESIZE 24

#if BX_USE_VGA_SMF
#  define BX_VGA_SMF  static
#  define BX_VGA_THIS theVga->
#  define BX_VGA_THIS_PTR theVga
#else
#  define BX_VGA_SMF
#  define BX_VGA_THIS this->
#  define BX_VGA_THIS_PTR this
#endif

class bx_vga_c : public bx_vga_stub_c {
public:
  bx_vga_c();
  virtual ~bx_vga_c();
  virtual void   init(void);
  virtual void   reset(unsigned type) {}
  BX_VGA_SMF bx_bool mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_VGA_SMF bx_bool mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  virtual Bit8u  mem_read(bx_phy_address addr);
  virtual void   mem_write(bx_phy_address addr, Bit8u value);
  virtual void   trigger_timer(void *this_ptr);
  virtual void   register_state(void);
  virtual void   after_restore_state(void);
#if BX_DEBUGGER
  virtual void   debug_dump(void);
#endif

  virtual void   redraw_area(unsigned x0, unsigned y0,
                             unsigned width, unsigned height);

  virtual int    get_snapshot_mode(void);
  virtual void   get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight,
                                   unsigned *txWidth);
  virtual Bit32u get_gfx_snapshot(Bit8u **snapshot_ptr, Bit8u **palette_ptr,
                                  unsigned *iHeight, unsigned *iWidth, unsigned *iDepth);
  virtual void   init_vga_extension(void);

  static void     timer_handler(void *);
#if BX_USE_VGA_SMF == 0
  BX_VGA_SMF void timer(void);
#endif
  static Bit64s   vga_param_handler(bx_param_c *param, int set, Bit64s val);

protected:
  void init_standard_vga(void);
  void init_iohandlers(bx_read_handler_t f_read, bx_write_handler_t f_write);
  void init_systemtimer(bx_timer_handler_t f_timer, param_event_handler f_param);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if BX_USE_VGA_SMF
  static void   write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#endif

#if BX_USE_VGA_SMF == 0
  Bit32u read(Bit32u address, unsigned io_len);
#endif
  void  write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log);

  BX_VGA_SMF Bit8u get_vga_pixel(Bit16u x, Bit16u y, Bit16u saddr, Bit16u lc, Bit8u **plane);
  BX_VGA_SMF bx_bool get_dac_palette(Bit8u **palette_ptr, Bit8u shift);
  BX_VGA_SMF void update(void);
  BX_VGA_SMF void determine_screen_dimensions(unsigned *piHeight, unsigned *piWidth);

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
      bx_bool read_mode;
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
    Bit16u last_xres;
    Bit16u last_yres;
    Bit8u last_bpp;
    Bit16u max_xres;
    Bit16u max_yres;
    Bit16u num_x_tiles;
    Bit16u num_y_tiles;
  } s;  // state information

  int timer_id;
  bx_bool extension_init;
  bx_bool pci_enabled;

  // Bochs VBE section
  virtual bx_bool vbe_set_base_addr(Bit32u *addr, Bit8u *pci_conf);

  BX_VGA_SMF Bit8u vbe_mem_read(bx_phy_address addr) BX_CPP_AttrRegparmN(1);
  BX_VGA_SMF void  vbe_mem_write(bx_phy_address addr, Bit8u value) BX_CPP_AttrRegparmN(2);

  static Bit32u vbe_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   vbe_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

#if BX_USE_VGA_SMF == 0
  Bit32u vbe_read(Bit32u address, unsigned io_len);
  void  vbe_write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log);
#endif

private:
  struct {
    Bit16u  cur_dispi;
    Bit32u  base_address;
    Bit16u  xres;
    Bit16u  yres;
    Bit16u  bpp;
    Bit16u  max_xres;
    Bit16u  max_yres;
    Bit16u  max_bpp;
    Bit16u  bank;
    bx_bool enabled;
    Bit16u  curindex;
    Bit32u  visible_screen_size; /**< in bytes */
    Bit16u  offset_x;            /**< Virtual screen x start (in pixels) */
    Bit16u  offset_y;            /**< Virtual screen y start (in pixels) */
    Bit16u  virtual_xres;
    Bit16u  virtual_yres;
    Bit32u  virtual_start;   /**< For dealing with bpp>8, this is where the virtual screen starts. */
    Bit8u   bpp_multiplier;  /**< We have to save this b/c sometimes we need to recalculate stuff with it. */
    bx_bool lfb_enabled;
    bx_bool get_capabilities;
    bx_bool dac_8bit;
  } vbe;  // VBE state information
};

#if BX_SUPPORT_CLGD54XX
void bx_vga_set_smf_pointer(bx_vga_c *theVga_ptr);
#endif

#endif
