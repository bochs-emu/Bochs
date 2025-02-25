/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025  The Bochs Project
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

#if BX_SUPPORT_GEFORCE

#if BX_USE_GEFORCE_SMF
#  define BX_GEFORCE_SMF  static
#  define BX_GEFORCE_THIS theSvga->
#  define BX_GEFORCE_THIS_PTR theSvga
#else
#  define BX_GEFORCE_SMF
#  define BX_GEFORCE_THIS this->
#  define BX_GEFORCE_THIS_PTR this
#endif // BX_USE_GEFORCE_SMF

// 0x3b4,0x3d4
#define VGA_CRTC_MAX 0x18
#define GEFORCE_CRTC_MAX 0x52

// Size of internal cache memory for bitblt. (must be >= 256 and 4-byte aligned)
#define CIRRUS_BLT_CACHESIZE (2048 * 4)

#define GEFORCE_CHANNEL_COUNT 32
#define GEFORCE_SUBCHANNEL_COUNT 8

class bx_geforce_c : public bx_vgacore_c
{
public:
  bx_geforce_c();
  virtual ~bx_geforce_c();

  virtual bool init_vga_extension(void);
  virtual void reset(unsigned type);
  virtual void redraw_area(Bit32s x0, Bit32s y0,
                           Bit32u width, Bit32u height);
  virtual Bit8u mem_read(bx_phy_address addr);
  virtual void mem_write(bx_phy_address addr, Bit8u value);
  virtual void get_text_snapshot(Bit8u **text_snapshot,
                                 unsigned *txHeight, unsigned *txWidth);
  virtual void register_state(void);
  virtual void after_restore_state(void);

#if BX_SUPPORT_PCI
  virtual void pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#endif
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

protected:
  virtual void update(void);

private:
  static Bit32u svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_CIRRUS_SMF
  Bit32u svga_read(Bit32u address, unsigned io_len);
  void   svga_write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_GEFORCE_SMF void   svga_init_members();

  BX_GEFORCE_SMF void draw_hardware_cursor(unsigned, unsigned, bx_svga_tileinfo_t *);

  // 0x3b4-0x3b5,0x3d4-0x3d5
  BX_GEFORCE_SMF Bit8u svga_read_crtc(Bit32u address, unsigned index);
  BX_GEFORCE_SMF void  svga_write_crtc(Bit32u address, unsigned index, Bit8u value);

  BX_GEFORCE_SMF Bit8u register_read8(Bit32u address);
  BX_GEFORCE_SMF void  register_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF Bit32u register_read32(Bit32u address);
  BX_GEFORCE_SMF void  register_write32(Bit32u address, Bit32u value);

  BX_GEFORCE_SMF Bit32u vram_read32(Bit32u address);
  BX_GEFORCE_SMF void vram_write8(Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void vram_write16(Bit32u address, Bit16u value);
  BX_GEFORCE_SMF void vram_write32(Bit32u address, Bit32u value);
  BX_GEFORCE_SMF Bit32u ramin_read32(Bit32u address);
  BX_GEFORCE_SMF void ramin_write32(Bit32u address, Bit32u value);
  BX_GEFORCE_SMF Bit32u physical_read32(Bit32u address);
  BX_GEFORCE_SMF Bit32u dma_read32(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF void dma_write8(Bit32u object, Bit32u address, Bit8u value);
  BX_GEFORCE_SMF void dma_write16(Bit32u object, Bit32u address, Bit16u value);
  BX_GEFORCE_SMF void dma_write32(Bit32u object, Bit32u address, Bit32u value);
  BX_GEFORCE_SMF Bit32u dma_pt_lookup(Bit32u object, Bit32u address);
  BX_GEFORCE_SMF Bit32u dma_vram_lookup(Bit32u object, Bit32u address);

  BX_GEFORCE_SMF Bit32u ramht_lookup(Bit32u handle, Bit32u chid);
  BX_GEFORCE_SMF void execute_command(Bit32u chid, Bit32u subc, Bit32u method, Bit32u param);
  BX_GEFORCE_SMF Bit32u color_565_to_888(Bit16u value);
  BX_GEFORCE_SMF void fillrect(Bit32u chid);
  BX_GEFORCE_SMF void imageblit(Bit32u chid);

  struct {
    Bit8u index;
    Bit8u reg[GEFORCE_CRTC_MAX+1];
  } crtc; // 0x3b4-5/0x3d4-5

  Bit32u mc_intr;
  Bit32u mc_intr_en;
  Bit32u mc_enable;
  Bit32u bus_debug_1;
  Bit32u bus_12xx[0x40];
  Bit32u fifo_intr;
  Bit32u fifo_intr_en;
  Bit32u fifo_ramht;
  Bit32u fifo_ramfc;
  Bit32u fifo_ramro;
  Bit32u rma_addr;
  Bit32u timer_intr;
  Bit32u timer_intr_en;
  Bit32u timer_num;
  Bit32u timer_den;
  Bit64u timer_inittime1;
  Bit64u timer_inittime2;
  Bit32u timer_alarm;
  Bit32u fb_debug_0;
  Bit32u fb_cfg0;
  Bit32u fb_cfg1;
  Bit32u straps0_primary;
  Bit32u graph_intr;
  Bit32u graph_intr_en;
  Bit32u graph_status;
  Bit32u graph_fifo;
  Bit32u graph_channel_ctx_table;
  Bit32u crtc_intr_en;
  Bit32u crtc_start;
  Bit32u crtc_config;
  Bit32u crtc_cursor_config;
  Bit32u crtc_gpio;
  Bit32u ramdac_cu_start_pos;
  Bit32u nvpll;
  Bit32u mpll;
  Bit32u vpll;
  Bit32u pll_control;
  Bit32u general_control;
  Bit32u test_control;
  Bit32u ramdac_fp_hcrtc;
  Bit32u ramdac_fp_tg_control;

  struct {
    bool initialized;
    Bit32u dma_put;
    Bit32u dma_get;
    Bit32u ref;
    Bit32u pushbuf;
    struct {
      Bit32u mthd;
      Bit32u subc;
      Bit32u mcnt;
      bool ni;
    } dma_state;
    struct {
        Bit32u object;
        Bit8u engine;
    } schs[GEFORCE_SUBCHANNEL_COUNT];

    Bit32u notifier;

    Bit32u image_src;
    Bit32u image_dst;
    Bit32u color_fmt;
    Bit32u color_bytes;
    Bit32u pitch;
    Bit32u offset_src;
    Bit32u offset_dst;

    Bit32u bg_color;
    Bit32u fg_color;
    Bit32u image_wh;
    Bit32u image_xy;
    Bit32u image_words_ptr;
    Bit32u image_words_left;
    Bit32u image_data[1024];

    Bit32u surface;
    Bit32u rect_color;
    Bit32u rect_xy;
    Bit32u rect_wh;
  } chs[GEFORCE_CHANNEL_COUNT];

  Bit32u unk_regs[4*1024*1024]; // temporary

  bool svga_unlock_special;
  bool svga_needs_update_tile;
  bool svga_needs_update_dispentire;
  bool svga_needs_update_mode;
  bool svga_double_width;

  Bit8u devfunc;
  unsigned svga_xres;
  unsigned svga_yres;
  unsigned svga_pitch;
  unsigned svga_bpp;
  unsigned svga_dispbpp;

  Bit32u bank_base[2];
  Bit32u memsize_mask;
  Bit8u *disp_ptr;
  Bit8u ext_latch[4];

  struct {
    Bit32u offset;
    Bit16s x, y;
    Bit8u size;
    bool enabled;
  } hw_cursor;

  struct {
    Bit16u x, y, w, h;
  } redraw;

  bx_ddc_c ddc;

  bool is_unlocked() { return svga_unlock_special; }

#if BX_SUPPORT_PCI
  BX_GEFORCE_SMF void svga_init_pcihandlers(void);

  BX_GEFORCE_SMF bool geforce_mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_GEFORCE_SMF bool geforce_mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
#endif
};

#endif // BX_SUPPORT_GEFORCE
