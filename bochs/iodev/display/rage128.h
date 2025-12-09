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

#if BX_SUPPORT_RAGE128

#if BX_USE_RAGE128_SMF
#  define BX_RAGE128_SMF  static
#  define BX_RAGE128_THIS theSvga->
#  define BX_RAGE128_THIS_PTR theSvga
#else
#  define BX_RAGE128_SMF
#  define BX_RAGE128_THIS this->
#  define BX_RAGE128_THIS_PTR this
#endif // BX_USE_RAGE128_SMF

// 0x3b4,0x3d4
#define VGA_CRTC_MAX 0x18
#define RAGE128_CRTC_MAX 0x9F

// ATI Rage 128 register aperture size (8KB = 0x2000)
#define RAGE128_REG_APER_SIZE 0x2000

#define BX_ROP_PATTERN 0x01

class bx_rage128_c : public bx_vgacore_c
{
public:
  bx_rage128_c();
  virtual ~bx_rage128_c();

  virtual bool init_vga_extension(void);
  Bit16u get_crtc_vtotal();
  virtual void get_crtc_params(bx_crtc_params_t* crtcp, Bit32u* vclock);
  virtual void reset(unsigned type);
  virtual void redraw_area(unsigned x0, unsigned y0,
                           unsigned width, unsigned height);
  void redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height);
  void redraw_area(Bit32u offset, Bit32u width, Bit32u height);
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
#if !BX_USE_RAGE128_SMF
  Bit32u svga_read(Bit32u address, unsigned io_len);
  void   svga_write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_RAGE128_SMF void   svga_init_members();
  BX_RAGE128_SMF void   bitblt_init();
  BX_RAGE128_SMF void   execute_bitblt();

  BX_RAGE128_SMF void draw_hardware_cursor(unsigned, unsigned, bx_svga_tileinfo_t *);

  // 0x3b4-0x3b5,0x3d4-0x3d5
  BX_RAGE128_SMF Bit8u svga_read_crtc(Bit32u address, unsigned index);
  BX_RAGE128_SMF void  svga_write_crtc(Bit32u address, unsigned index, Bit8u value);

  BX_RAGE128_SMF void set_irq_level(bool level);
  BX_RAGE128_SMF Bit32u get_crt_intr();

  BX_RAGE128_SMF Bit8u register_read8(Bit32u address);
  BX_RAGE128_SMF void  register_write8(Bit32u address, Bit8u value);
  BX_RAGE128_SMF Bit32u register_read32(Bit32u address);
  BX_RAGE128_SMF void  register_write32(Bit32u address, Bit32u value);

  BX_RAGE128_SMF Bit8u vram_read8(Bit32u address);
  BX_RAGE128_SMF Bit16u vram_read16(Bit32u address);
  BX_RAGE128_SMF Bit32u vram_read32(Bit32u address);
  BX_RAGE128_SMF void vram_write8(Bit32u address, Bit8u value);
  BX_RAGE128_SMF void vram_write16(Bit32u address, Bit16u value);
  BX_RAGE128_SMF void vram_write32(Bit32u address, Bit32u value);

  struct {
    Bit8u index;
    Bit8u reg[RAGE128_CRTC_MAX+1];
  } crtc; // 0x3b4-5/0x3d4-5

  // ATI Rage 128 register state
  // Configuration registers (0x0000-0x00FF)
  Bit32u config_cntl;           // 0x00E0
  Bit32u config_xstrap;         // 0x00E4
  Bit32u config_bonds;          // 0x00E8
  Bit32u config_memsize;        // 0x00F8
  Bit32u config_aper_0_base;     // 0x0100
  Bit32u config_aper_1_base;    // 0x0104
  Bit32u config_reg_1_base;     // 0x0108
  Bit32u config_reg_aper_size;  // 0x010C
  Bit32u config_aper_size;      // 0x0110

  // CRTC registers (accelerator CRTC, not VGA CRTC) - ATI Rage 128 PRO addresses
  // Main CRTC control registers
  Bit32u crtc_gen_cntl;         // 0x0050 - CRTC_GEN_CNTL
  Bit32u crtc_ext_cntl;         // 0x0054 - CRTC_EXT_CNTL
  Bit32u crtc_status;           // 0x005C - CRTC_STATUS
  
  // Combined CRTC timing registers (ATI Rage 128 PRO format)
  Bit32u crtc_h_total_disp;     // 0x0200 - CRTC_H_TOTAL_DISP (H_TOTAL bits 8:0, H_DISP bits 23:16)
  Bit32u crtc_h_sync_strt_wid;  // 0x0204 - CRTC_H_SYNC_STRT_WID (H_SYNC_STRT bits 11:3, H_SYNC_WID bits 21:16)
  Bit32u crtc_v_total_disp;     // 0x0208 - CRTC_V_TOTAL_DISP (V_TOTAL bits 10:0, V_DISP bits 26:16)
  Bit32u crtc_v_sync_strt_wid;  // 0x020C - CRTC_V_SYNC_STRT_WID (V_SYNC_STRT bits 10:0, V_SYNC_WID bits 20:16)
  
  // CRTC display control registers
  Bit32u crtc_vline_crnt_vline; // 0x0210 - CRTC_VLINE_CRNT_VLINE
  Bit32u crtc_crnt_frame;       // 0x0214 - CRTC_CRNT_FRAME
  Bit32u crtc_gui_trig_vline;   // 0x0218 - CRTC_GUI_TRIG_VLINE
  Bit32u crtc_debug;            // 0x021C - CRTC_DEBUG
  Bit32u crtc_offset;           // 0x0224 - CRTC_OFFSET
  Bit32u crtc_offset_cntl;      // 0x0228 - CRTC_OFFSET_CNTL
  Bit32u crtc_pitch;            // 0x022C - CRTC_PITCH

  // Hardware cursor registers (ATI Rage 128 PRO addresses)
  Bit32u crtc_cur_offset;       // 0x0260 - CUR_OFFSET
  Bit32u crtc_cur_hv_pos;       // 0x0264 - CUR_HORZ_VERT_POSN
  Bit32u crtc_cur_hv_offs;      // 0x0268 - CUR_HORZ_VERT_OFF
  Bit32u crtc_cur_color0;       // 0x026C - CUR_CLR0
  Bit32u crtc_cur_color1;       // 0x0270 - CUR_CLR1

  // Clock control and PLL registers
  Bit32u clk_cntl_index;        // 0x00C0
  Bit32u clk_cntl_data;         // 0x00C4
  Bit32u pll_ref_div;           // 0x00C8
  Bit32u pll_gen_cntl;          // 0x00CC
  Bit32u pll_vclk_cntl;        // 0x00D0
  Bit32u pll_vclk_cntl2;       // 0x00D4
  Bit32u pll_vclk_cntl3;       // 0x00D8
  Bit32u pll_vclk_ecp_cntl;    // 0x00DC

  // DAC registers
  Bit32u dac_cntl;              // 0x00E0
  Bit32u dac_ext_cntl;         // 0x00E4
  Bit32u dac_gen_cntl;         // 0x00E8
  Bit32u dac_macro_cntl;       // 0x00EC
  Bit32u dac_palette_addr;     // 0x00F0
  Bit32u dac_palette_data;      // 0x00F4

  // GUI engine registers (0x1400-0x1FFF)
  Bit32u gui_eng_cntl;          // 0x1400
  Bit32u gui_eng_status;        // 0x1404
  Bit32u gui_eng_req;           // 0x1408
  Bit32u gui_eng_ack;           // 0x140C

  // 2D Engine Destination Trajectory registers
  Bit32u dst_offset;            // 0x1404 - DST_OFFSET
  Bit32u dst_pitch;             // 0x1408 - DST_PITCH
  Bit32u dst_width;             // 0x140C - DST_WIDTH
  Bit32u dst_height;            // 0x1410 - DST_HEIGHT
  Bit32u src_x;                 // 0x1414 - SRC_X
  Bit32u src_y;                 // 0x1418 - SRC_Y
  Bit32u dst_x;                 // 0x141C - DST_X
  Bit32u dst_y;                 // 0x1420 - DST_Y
  Bit32u src_pitch_offset;      // 0x1428 - SRC_PITCH_OFFSET
  Bit32u dst_pitch_offset;      // 0x142C - DST_PITCH_OFFSET
  Bit32u src_y_x;               // 0x1434 - SRC_Y_X
  Bit32u dst_y_x;               // 0x1438 - DST_Y_X

  // 2D Engine Source Trajectory registers
  Bit32u src_offset;            // 0x15AC - SRC_OFFSET
  Bit32u src_pitch;             // 0x15B0 - SRC_PITCH

  // 2D Engine Draw Engine Control registers
  Bit32u dp_gui_master_cntl;    // 0x146C - DP_GUI_MASTER_CNTL
  Bit32u dp_cntl;               // 0x16C0 - DP_CNTL
  Bit32u dp_datatype;           // 0x16C4 - DP_DATATYPE
  Bit32u dp_mix;                // 0x16C8 - DP_MIX
  Bit32u dp_src_frgd_clr;      // 0x15D8 - DP_SRC_FRGD_CLR
  Bit32u dp_src_bkgd_clr;      // 0x15DC - DP_SRC_BKGD_CLR
  Bit32u host_data[8];          // 0x17C0-0x17DC - HOST_DATA[0-7]
  Bit32u host_data_last;        // 0x17E0 - HOST_DATA_LAST

  // GPIO registers for DDC communication
  Bit32u gpio_vga_ddc;          // 0x0060 - GPIO_VGA_DDC (VGA DDC I2C control)
  Bit32u gpio_dvi_ddc;          // 0x0064 - GPIO_DVI_DDC (DVI DDC I2C control)

  // Interrupt registers
  Bit32u crt_intr;              // CRT interrupt
  Bit32u crt_intr_en;           // CRT interrupt enable

  bx_bitblt_rop_t rop_handler[0x100];
  Bit8u  rop_flags[0x100];

  // Register storage for unknown/unimplemented registers
  Bit32u regs[RAGE128_REG_APER_SIZE / 4];

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

  Bit8u *disp_ptr;
  Bit32u bank_base[2];

  struct {
    Bit32u offset;
    Bit16s x, y;
    Bit8u size;
    bool bpp32;
    bool enabled;
  } hw_cursor;

  struct {
    Bit16u x, y, w, h;
  } redraw;

  bx_ddc_c ddc;

  bool is_unlocked() { return svga_unlock_special; }

  BX_RAGE128_SMF void svga_init_pcihandlers(void);

  void vertical_timer();

  BX_RAGE128_SMF bool rage128_mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param);
  BX_RAGE128_SMF bool rage128_mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param);
};

#endif // BX_SUPPORT_RAGE128
