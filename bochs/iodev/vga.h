/////////////////////////////////////////////////////////////////////////
// $Id: vga.h,v 1.14 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
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

#if BX_SUPPORT_VBE
  #define VBE_DISPI_TOTAL_VIDEO_MEMORY_MB 4
  
  #define VBE_DISPI_BANK_ADDRESS          0xA0000
  #define VBE_DISPI_BANK_SIZE_KB          64
  
  #define VBE_DISPI_MAX_XRES              1024
  #define VBE_DISPI_MAX_YRES              768
  
  #define VBE_DISPI_IOPORT_INDEX          0xFF80
  #define VBE_DISPI_IOPORT_DATA           0xFF81
  
  #define VBE_DISPI_INDEX_ID              0x0
  #define VBE_DISPI_INDEX_XRES            0x1
  #define VBE_DISPI_INDEX_YRES            0x2
  #define VBE_DISPI_INDEX_BPP             0x3
  #define VBE_DISPI_INDEX_ENABLE          0x4
  #define VBE_DISPI_INDEX_BANK            0x5
  #define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
  #define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
  #define VBE_DISPI_INDEX_X_OFFSET        0x8
  #define VBE_DISPI_INDEX_Y_OFFSET        0x9
    
  #define VBE_DISPI_ID0                   0xB0C0
  #define VBE_DISPI_ID1                   0xB0C1
    
  #define VBE_DISPI_BPP_8                 0x0
// The following is not support yet, but just for reference available.  
//  #define VBE_DISPI_BPP_RGB565            0x1
//  #define VBE_DISPI_BPP_RGB555            0x2

  #define VBE_DISPI_DISABLED              0x00
  #define VBE_DISPI_ENABLED               0x01
  #define VBE_DISPI_LFB_PHYSICAL_ADDRESS  0xE0000000
  

#define BX_MAX_XRES VBE_DISPI_MAX_XRES
#define BX_MAX_YRES VBE_DISPI_MAX_YRES

#else

#define BX_MAX_XRES 640
#define BX_MAX_YRES 480

#endif //BX_SUPPORT_VBE
#define CGA_TEXT_ADDR(row, column) (0x18000 + ((row)*80 + (column))*2)

#define X_TILESIZE 8
#define Y_TILESIZE 8
#define BX_NUM_X_TILES (BX_MAX_XRES /X_TILESIZE)
#define BX_NUM_Y_TILES (BX_MAX_YRES /Y_TILESIZE)

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
  BX_VGA_SMF void   reset(unsigned type);
  BX_VGA_SMF Bit8u  mem_read(Bit32u addr);
  // Note: either leave value of type Bit8u, or mask it when
  //       used to 8 bits, in memory.cc
  BX_VGA_SMF void   mem_write(Bit32u addr, Bit8u value);

#if BX_SUPPORT_VBE 
  BX_VGA_SMF Bit8u  vbe_mem_read(Bit32u addr);
  BX_VGA_SMF void   vbe_mem_write(Bit32u addr, Bit8u value);  
#endif
  
  BX_VGA_SMF void   redraw_area(unsigned x0, unsigned y0,
                                unsigned width, unsigned height);

private:

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static void   write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

#if BX_SUPPORT_VBE
  static Bit32u vbe_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   vbe_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#endif

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

#if BX_SUPPORT_VBE    
    Bit8u vbe_memory[VBE_DISPI_TOTAL_VIDEO_MEMORY_MB *1024 * 1024];
    Bit16u  vbe_cur_dispi;
    Bit16u  vbe_xres;
    Bit16u  vbe_yres;
    Bit16u  vbe_bpp;
    Bit16u  vbe_bank;
    Boolean vbe_enabled;
    Bit16u  vbe_curindex;
    Bit32u  vbe_visable_screen_size; // in bytes
    Bit16u  vbe_offset_x;
    Bit16u  vbe_offset_y;
    Bit16u  vbe_virtual_xres;
    Bit16u  vbe_virtual_yres;
#endif    
    } s;  // state information


  bx_devices_c *devices;

#if !BX_USE_VGA_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#else
  void write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#endif

#if BX_SUPPORT_VBE
#if !BX_USE_VGA_SMF
  Bit32u vbe_read(Bit32u address, unsigned io_len);
  void   vbe_write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#else
  void vbe_write(Bit32u address, Bit32u value, unsigned io_len, Boolean no_log);
#endif
#endif

  int timer_id;

  public:
  static void   timer_handler(void *);
  BX_VGA_SMF void   timer(void);
  BX_VGA_SMF void set_update_interval (unsigned interval);
  BX_VGA_SMF void  get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight,
                                                            unsigned *txWidth);
  private:
  BX_VGA_SMF void   update(void);
  BX_VGA_SMF void   dump_status(void);
  BX_VGA_SMF void determine_screen_dimensions(unsigned *piHeight,
                                              unsigned *piWidth);
  };

extern bx_vga_c bx_vga;
