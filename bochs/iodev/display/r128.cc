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

#define BX_PLUGGABLE

#include "iodev.h"
#include "vgacore.h"
#include "pci.h"
#define BX_USE_BINARY_FWD_ROP
#define BX_USE_BINARY_BKWD_ROP
#include "bitblt.h"
#include "ddc.h"
#include "pxextract.h"
#include "r128.h"
#include "virt_timer.h"

#include "bx_debug/debug.h"

#if BX_SUPPORT_R128

#define LOG_THIS BX_R128_THIS

#if BX_USE_R128_SMF
#define VGA_READ(addr,len)       bx_vgacore_c::read_handler(theSvga,addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write_handler(theSvga,addr,val,len)
#define SVGA_READ(addr,len)      svga_read_handler(theSvga,addr,len)
#define SVGA_WRITE(addr,val,len) svga_write_handler(theSvga,addr,val,len)
#else
#define VGA_READ(addr,len)       bx_vgacore_c::read(addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write(addr,val,len)
#define SVGA_READ(addr,len)      svga_read(addr,len)
#define SVGA_WRITE(addr,val,len) svga_write(addr,val,len)
#endif // BX_USE_R128_SMF

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

static bx_r128_c *theSvga = NULL;

PLUGIN_ENTRY_FOR_MODULE(r128)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_r128_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_R128);
  } else if (mode == PLUGIN_FINI) {
    delete theSvga;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_VGA;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

bx_r128_c::bx_r128_c() : bx_vgacore_c()
{
  // nothing else to do
}

bx_r128_c::~bx_r128_c()
{
  SIM->get_bochs_root()->remove("r128");
  BX_DEBUG(("Exit"));
}

bool bx_r128_c::init_vga_extension(void)
{
  if (!SIM->is_agp_device(BX_PLUGIN_R128)) {
    BX_PANIC(("ATI Rage 128 PRO should be plugged into AGP slot"));
  }

  BX_R128_THIS pci_enabled = true;

  BX_R128_THIS put("R128");
  // initialize SVGA stuffs.
  BX_R128_THIS init_iohandlers(svga_read_handler, svga_write_handler, "r128");
  DEV_register_ioread_handler(this, svga_read_handler, 0x03B4, "r128", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D0, "r128", 6);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D2, "r128", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03C3, "r128", 3);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D0, "r128", 6);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D2, "r128", 2);
  // Register handlers for extended VGA ports (0xe000-0xe007, 0xe016-0xe017) to suppress log messages
  DEV_register_ioread_handler(this, svga_read_handler, 0xe000, "r128", 8);
  DEV_register_iowrite_handler(this, svga_write_handler, 0xe000, "r128", 8);
  DEV_register_ioread_handler(this, svga_read_handler, 0xe016, "r128", 2);
  DEV_register_iowrite_handler(this, svga_write_handler, 0xe016, "r128", 2);
  BX_R128_THIS svga_init_members();
  BX_R128_THIS svga_init_pcihandlers();
  BX_R128_THIS s.CRTC.max_reg = R128_CRTC_MAX;
  BX_R128_THIS s.max_xres = 2048;
  BX_R128_THIS s.max_yres = 1536;
  BX_INFO(("ATI Rage 128 PRO initialized"));
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("r128", this);
#endif
  return 1;
}

#define SETUP_BITBLT(num, name, flags) \
  do { \
    BX_R128_THIS rop_handler[num] = bitblt_rop_fwd_##name; \
    BX_R128_THIS rop_flags[num] = flags; \
  } while (0);

void bx_r128_c::bitblt_init()
{
  for (int i = 0; i < 0x100; i++) {
    SETUP_BITBLT(i, nop, BX_ROP_PATTERN);
  }
  SETUP_BITBLT(0x00, 0, 0);                              // 0
  SETUP_BITBLT(0x05, notsrc_and_notdst, BX_ROP_PATTERN); // PSan
  SETUP_BITBLT(0x0a, notsrc_and_dst, BX_ROP_PATTERN);    // DPna
  SETUP_BITBLT(0x0f, notsrc, BX_ROP_PATTERN);            // Pn
  SETUP_BITBLT(0x11, notsrc_and_notdst, 0);              // DSon
  SETUP_BITBLT(0x22, notsrc_and_dst, 0);                 // DSna
  SETUP_BITBLT(0x33, notsrc, 0);                         // Sn
  SETUP_BITBLT(0x44, src_and_notdst, 0);                 // SDna
  SETUP_BITBLT(0x50, src_and_notdst, 0);                 // PDna
  SETUP_BITBLT(0x55, notdst, 0);                         // Dn
  SETUP_BITBLT(0x5a, src_xor_dst, BX_ROP_PATTERN);       // DPx
  SETUP_BITBLT(0x5f, notsrc_or_notdst, BX_ROP_PATTERN);  // DSan
  SETUP_BITBLT(0x66, src_xor_dst, 0);                    // DSx
  SETUP_BITBLT(0x77, notsrc_or_notdst, 0);               // DSan
  SETUP_BITBLT(0x88, src_and_dst, 0);                    // DSa
  SETUP_BITBLT(0x99, src_notxor_dst, 0);                 // DSxn
  SETUP_BITBLT(0xaa, nop, 0);                            // D
  SETUP_BITBLT(0xad, src_and_dst, BX_ROP_PATTERN);       // DPa
  SETUP_BITBLT(0xaf, notsrc_or_dst, BX_ROP_PATTERN);     // DPno
  SETUP_BITBLT(0xbb, notsrc_or_dst, 0);                  // DSno
  SETUP_BITBLT(0xcc, src, 0);                            // S
  SETUP_BITBLT(0xdd, src_and_notdst, 0);                 // SDna
  SETUP_BITBLT(0xee, src_or_dst, 0);                     // DSo
  SETUP_BITBLT(0xf0, src, BX_ROP_PATTERN);               // P
  SETUP_BITBLT(0xf5, src_or_notdst, BX_ROP_PATTERN);     // PDno
  SETUP_BITBLT(0xfa, src_or_dst, BX_ROP_PATTERN);        // DPo
  SETUP_BITBLT(0xff, 1, 0);                              // 1
}

void bx_r128_c::execute_bitblt()
{
  // ATI Rage 128 PRO 2D acceleration BitBlt execution
  // Based on DP_GUI_MASTER_CNTL, DP_MIX, and trajectory registers
  
  // Extract operation type from DP_GUI_MASTER_CNTL
  Bit32u gmc = BX_R128_THIS dp_gui_master_cntl;
  Bit32u dp_mix_val = BX_R128_THIS dp_mix;
  
  // Extract ROP3 from DP_MIX (bits 7:0) or DP_GUI_MASTER_CNTL (bits 23:16)
  Bit8u rop3 = (gmc >> 16) & 0xFF;
  if (rop3 == 0) {
    rop3 = dp_mix_val & 0xFF;
  }
  
  // Extract source type from DP_GUI_MASTER_CNTL (bits 13:12)
  Bit8u src_type = (gmc >> 12) & 0x3;
  // Extract destination type from DP_GUI_MASTER_CNTL (bits 11:8)
  Bit8u dst_type = (gmc >> 8) & 0xF;
  // Extract source source from DP_GUI_MASTER_CNTL (bits 26:24)
  Bit8u src_source = (gmc >> 24) & 0x7;
  
  // Get destination parameters
  Bit32u dst_off = BX_R128_THIS dst_offset & 0x03FFFFFF;
  Bit32u dst_p = BX_R128_THIS dst_pitch;
  // DST_PITCH is in pixels*8, convert to bytes
  if (dst_type == 2) { // 8 bpp
    dst_p = (dst_p & 0x3FF) << 3;
  } else if (dst_type == 3 || dst_type == 4) { // 16 bpp
    dst_p = (dst_p & 0x3FF) << 4;
  } else if (dst_type == 5) { // 24 bpp
    dst_p = (dst_p & 0x3FF) * 3;
  } else if (dst_type == 6) { // 32 bpp
    dst_p = (dst_p & 0x3FF) << 5;
  } else {
    dst_p = (dst_p & 0x3FF) << 3; // Default to 8 bpp
  }
  
  Bit16s dst_x = (Bit16s)(BX_R128_THIS dst_x & 0x3FFF);
  Bit16s dst_y = (Bit16s)(BX_R128_THIS dst_y & 0x3FFF);
  Bit16u dst_w = (BX_R128_THIS dst_width & 0x1FFF) + 1;
  Bit16u dst_h = (BX_R128_THIS dst_height & 0x1FFF) + 1;
  
  // Extract direction from DP_CNTL
  Bit32u dp_cntl_val = BX_R128_THIS dp_cntl;
  bool x_dir = (dp_cntl_val & 0x01) != 0; // DST_X_DIR
  bool y_dir = (dp_cntl_val & 0x02) != 0; // DST_Y_DIR
  
  // Determine pixel size based on destination type
  Bit8u dst_bpp = 8;
  if (dst_type == 3 || dst_type == 4) dst_bpp = 16;
  else if (dst_type == 5) dst_bpp = 24;
  else if (dst_type == 6) dst_bpp = 32;
  
  Bit8u dst_bytes = dst_bpp / 8;
  
  // Calculate destination address
  Bit32u dst_addr = dst_off + dst_y * dst_p + dst_x * dst_bytes;
  
  // Handle different source types
  if (src_source == 2) {
    // Screen-to-screen BitBlt (source from memory - rectangular trajectory)
    Bit32u src_off = BX_R128_THIS src_offset & 0x03FFFFFF;
    Bit32u src_p = BX_R128_THIS src_pitch;
    // SRC_PITCH is in pixels*8, convert to bytes
    if (src_type == 3) { // Color source
      if (dst_type == 2) src_p = (src_p & 0x3FF) << 3;
      else if (dst_type == 3 || dst_type == 4) src_p = (src_p & 0x3FF) << 4;
      else if (dst_type == 5) src_p = (src_p & 0x3FF) * 3;
      else if (dst_type == 6) src_p = (src_p & 0x3FF) << 5;
      else src_p = (src_p & 0x3FF) << 3;
    } else {
      // Monochrome source
      src_p = (src_p & 0x3FF) << 3;
    }
    
    Bit16s src_x = (Bit16s)(BX_R128_THIS src_x & 0x3FFF);
    Bit16s src_y = (Bit16s)(BX_R128_THIS src_y & 0x3FFF);
    Bit32u src_addr = src_off + src_y * src_p + src_x * dst_bytes;
    
    // Perform screen-to-screen BitBlt
    Bit8u *dst_ptr = BX_R128_THIS s.memory + (dst_addr & (BX_R128_THIS s.memsize - 1));
    Bit8u *src_ptr = BX_R128_THIS s.memory + (src_addr & (BX_R128_THIS s.memsize - 1));
    
    // Handle direction
    int dst_x_inc = x_dir ? dst_bytes : -dst_bytes;
    int src_x_inc = x_dir ? dst_bytes : -dst_bytes;
    int dst_y_inc = y_dir ? dst_p : -dst_p;
    int src_y_inc = y_dir ? src_p : -src_p;
    
    if (!x_dir) {
      dst_ptr += (dst_w - 1) * dst_bytes;
      src_ptr += (dst_w - 1) * dst_bytes;
    }
    if (!y_dir) {
      dst_ptr += (dst_h - 1) * dst_p;
      src_ptr += (dst_h - 1) * src_p;
    }
    
    // Use ROP handler if available (for forward direction only)
    if (x_dir && y_dir && BX_R128_THIS rop_handler[rop3] != NULL && (BX_R128_THIS rop_flags[rop3] & BX_ROP_PATTERN) == 0) {
      // Binary ROP (source and destination) - forward direction
      BX_R128_THIS rop_handler[rop3](dst_ptr, src_ptr, dst_p, src_p, dst_w * dst_bytes, dst_h);
    } else {
      // Handle direction manually or use simple copy
      for (unsigned y = 0; y < dst_h; y++) {
        Bit8u *d = dst_ptr;
        Bit8u *s = src_ptr;
        for (unsigned x = 0; x < dst_w; x++) {
          // Apply ROP operation (simplified - full ROP3 would need pattern support)
          if (rop3 == 0xCC) { // S (source copy)
            memcpy(d, s, dst_bytes);
          } else if (rop3 == 0x00) { // 0 (black)
            memset(d, 0, dst_bytes);
          } else if (rop3 == 0xFF) { // 1 (white)
            memset(d, 0xFF, dst_bytes);
          } else if (rop3 == 0xAA) { // D (destination unchanged)
            // Do nothing
          } else {
            // Default: simple copy
            memcpy(d, s, dst_bytes);
          }
          
          d += dst_x_inc;
          s += src_x_inc;
        }
        dst_ptr += dst_y_inc;
        src_ptr += src_y_inc;
      }
    }
    
    // Mark display area for redraw
    BX_R128_THIS redraw_area(dst_x, dst_y, dst_w, dst_h);
    
  } else if (src_source == 3 || src_source == 4) {
    // Host-to-screen BitBlt (source from HOST_DATA registers)
    // This is typically used for monochrome expansion or solid color fill
    Bit8u *dst_ptr = BX_R128_THIS s.memory + (dst_addr & (BX_R128_THIS s.memsize - 1));
    
    if (src_type == 0 || src_type == 1) {
      // Monochrome source expansion
      Bit32u fg_color = BX_R128_THIS dp_src_frgd_clr;
      // Bit32u bg_color = BX_R128_THIS dp_src_bkgd_clr; // TODO: Use for monochrome expansion
      
      // For now, implement simple solid color fill
      // TODO: Implement proper monochrome expansion from HOST_DATA
      for (unsigned y = 0; y < dst_h; y++) {
        for (unsigned x = 0; x < dst_w; x++) {
          if (dst_bytes == 1) {
            dst_ptr[x] = (Bit8u)(fg_color & 0xFF);
          } else if (dst_bytes == 2) {
            *((Bit16u*)(dst_ptr + x * 2)) = (Bit16u)(fg_color & 0xFFFF);
          } else if (dst_bytes == 3) {
            dst_ptr[x * 3 + 0] = (Bit8u)(fg_color & 0xFF);
            dst_ptr[x * 3 + 1] = (Bit8u)((fg_color >> 8) & 0xFF);
            dst_ptr[x * 3 + 2] = (Bit8u)((fg_color >> 16) & 0xFF);
          } else if (dst_bytes == 4) {
            *((Bit32u*)(dst_ptr + x * 4)) = fg_color;
          }
        }
        dst_ptr += dst_p;
      }
    } else {
      // Color source - solid color fill
      Bit32u fill_color = BX_R128_THIS dp_src_frgd_clr;
      for (unsigned y = 0; y < dst_h; y++) {
        for (unsigned x = 0; x < dst_w; x++) {
          if (dst_bytes == 1) {
            dst_ptr[x] = (Bit8u)(fill_color & 0xFF);
          } else if (dst_bytes == 2) {
            *((Bit16u*)(dst_ptr + x * 2)) = (Bit16u)(fill_color & 0xFFFF);
          } else if (dst_bytes == 3) {
            dst_ptr[x * 3 + 0] = (Bit8u)(fill_color & 0xFF);
            dst_ptr[x * 3 + 1] = (Bit8u)((fill_color >> 8) & 0xFF);
            dst_ptr[x * 3 + 2] = (Bit8u)((fill_color >> 16) & 0xFF);
          } else if (dst_bytes == 4) {
            *((Bit32u*)(dst_ptr + x * 4)) = fill_color;
          }
        }
        dst_ptr += dst_p;
      }
    }
    
    // Mark display area for redraw
    BX_R128_THIS redraw_area(dst_x, dst_y, dst_w, dst_h);
  }
  
  // Update GUI engine status
  BX_R128_THIS gui_eng_status = 0; // Operation complete
  BX_R128_THIS gui_eng_ack = BX_R128_THIS gui_eng_req; // Acknowledge request
}

void bx_r128_c::svga_init_members()
{
  // clear all registers.
  BX_R128_THIS crtc.index = R128_CRTC_MAX + 1;
  for (int i = 0; i <= R128_CRTC_MAX; i++)
    BX_R128_THIS crtc.reg[i] = 0x00;

  // Initialize ATI Rage 128 registers
  memset(BX_R128_THIS regs, 0, sizeof(BX_R128_THIS regs));

  // Configuration registers
  BX_R128_THIS config_cntl = 0;
  BX_R128_THIS config_xstrap = 0;
  BX_R128_THIS config_bonds = 0;
  BX_R128_THIS config_memsize = 64 * 1024 * 1024; // 64MB default
  BX_R128_THIS config_aper_0_base = 0;
  BX_R128_THIS config_aper_1_base = 0;
  BX_R128_THIS config_reg_1_base = 0;
  BX_R128_THIS config_reg_aper_size = R128_REG_APER_SIZE;
  BX_R128_THIS config_aper_size = 0;

  // CRTC registers (ATI Rage 128 PRO addresses)
  // Main CRTC control registers
  BX_R128_THIS crtc_gen_cntl = 0;
  BX_R128_THIS crtc_ext_cntl = 0;
  BX_R128_THIS crtc_status = 0;
  
  // Combined CRTC timing registers (ATI Rage 128 PRO format)
  BX_R128_THIS crtc_h_total_disp = 0;
  BX_R128_THIS crtc_h_sync_strt_wid = 0;
  BX_R128_THIS crtc_v_total_disp = 0;
  BX_R128_THIS crtc_v_sync_strt_wid = 0;
  
  // CRTC display control registers
  BX_R128_THIS crtc_vline_crnt_vline = 0;
  BX_R128_THIS crtc_crnt_frame = 0;
  BX_R128_THIS crtc_gui_trig_vline = 0;
  BX_R128_THIS crtc_debug = 0;
  BX_R128_THIS crtc_offset = 0;
  BX_R128_THIS crtc_offset_cntl = 0;
  BX_R128_THIS crtc_pitch = 0;

  // Hardware cursor
  BX_R128_THIS crtc_cur_offset = 0;
  BX_R128_THIS crtc_cur_hv_pos = 0;
  BX_R128_THIS crtc_cur_hv_offs = 0;
  BX_R128_THIS crtc_cur_color0 = 0;
  BX_R128_THIS crtc_cur_color1 = 0;

  // Clock control and PLL
  BX_R128_THIS clk_cntl_index = 0;
  BX_R128_THIS clk_cntl_data = 0;
  BX_R128_THIS pll_ref_div = 0;
  BX_R128_THIS pll_gen_cntl = 0;
  BX_R128_THIS pll_vclk_cntl = 0;
  BX_R128_THIS pll_vclk_cntl2 = 0;
  BX_R128_THIS pll_vclk_cntl3 = 0;
  BX_R128_THIS pll_vclk_ecp_cntl = 0;

  // DAC registers
  BX_R128_THIS dac_cntl = 0;
  BX_R128_THIS dac_ext_cntl = 0;
  BX_R128_THIS dac_gen_cntl = 0;
  BX_R128_THIS dac_macro_cntl = 0;
  BX_R128_THIS dac_palette_addr = 0;
  BX_R128_THIS dac_palette_data = 0;

  // GPIO registers for DDC communication
  BX_R128_THIS gpio_vga_ddc = 0;
  BX_R128_THIS gpio_dvi_ddc = 0;

  // GUI engine
  BX_R128_THIS gui_eng_cntl = 0;
  BX_R128_THIS gui_eng_status = 0;
  BX_R128_THIS gui_eng_req = 0;
  BX_R128_THIS gui_eng_ack = 0;

  // 2D Engine Destination Trajectory registers
  BX_R128_THIS dst_offset = 0;
  BX_R128_THIS dst_pitch = 0;
  BX_R128_THIS dst_width = 0;
  BX_R128_THIS dst_height = 0;
  BX_R128_THIS src_x = 0;
  BX_R128_THIS src_y = 0;
  BX_R128_THIS dst_x = 0;
  BX_R128_THIS dst_y = 0;
  BX_R128_THIS src_pitch_offset = 0;
  BX_R128_THIS dst_pitch_offset = 0;
  BX_R128_THIS src_y_x = 0;
  BX_R128_THIS dst_y_x = 0;

  // 2D Engine Source Trajectory registers
  BX_R128_THIS src_offset = 0;
  BX_R128_THIS src_pitch = 0;

  // 2D Engine Draw Engine Control registers
  BX_R128_THIS dp_gui_master_cntl = 0;
  BX_R128_THIS dp_cntl = 0;
  BX_R128_THIS dp_datatype = 0;
  BX_R128_THIS dp_mix = 0;
  BX_R128_THIS dp_src_frgd_clr = 0;
  BX_R128_THIS dp_src_bkgd_clr = 0;
  memset(BX_R128_THIS host_data, 0, sizeof(BX_R128_THIS host_data));
  BX_R128_THIS host_data_last = 0;

  // Interrupts
  BX_R128_THIS crt_intr = 0;
  BX_R128_THIS crt_intr_en = 0;

  BX_R128_THIS svga_unlock_special = 0;
  BX_R128_THIS svga_needs_update_tile = 1;
  BX_R128_THIS svga_needs_update_dispentire = 1;
  BX_R128_THIS svga_needs_update_mode = 0;
  BX_R128_THIS svga_double_width = 0;

  BX_R128_THIS svga_xres = 640;
  BX_R128_THIS svga_yres = 480;
  BX_R128_THIS svga_bpp = 8;
  BX_R128_THIS svga_pitch = 640;
  BX_R128_THIS bank_base[0] = 0;
  BX_R128_THIS bank_base[1] = 0;

  BX_R128_THIS hw_cursor.x = 0;
  BX_R128_THIS hw_cursor.y = 0;
  BX_R128_THIS hw_cursor.size = 0;
  BX_R128_THIS hw_cursor.offset = 0;
  BX_R128_THIS hw_cursor.bpp32 = false;
  BX_R128_THIS hw_cursor.enabled = false;

  // ATI Rage 128 PRO defaults to 32MB
  BX_R128_THIS s.memsize = 32 * 1024 * 1024;
  BX_R128_THIS config_memsize = BX_R128_THIS s.memsize;

  // memory allocation.
  if (BX_R128_THIS s.memory == NULL)
    BX_R128_THIS s.memory = new Bit8u[BX_R128_THIS s.memsize];
  memset(BX_R128_THIS s.memory, 0x00, BX_R128_THIS s.memsize);
  BX_R128_THIS disp_ptr = BX_R128_THIS s.memory;

  // VCLK defaults after reset
  BX_R128_THIS s.vclk[0] = 25180000;
  BX_R128_THIS s.vclk[1] = 28325000;
  BX_R128_THIS s.vclk[2] = 41165000;
  BX_R128_THIS s.vclk[3] = 36082000;
}

void bx_r128_c::reset(unsigned type)
{
  // reset VGA stuffs.
  BX_R128_THIS bx_vgacore_c::reset(type);
  // reset SVGA stuffs.
  BX_R128_THIS svga_init_members();
  BX_R128_THIS ddc.init();
  // Disable ROM shadowing to allow clearing of VRAM
  BX_R128_THIS pci_conf[0x50] = 0x00;
}

void bx_r128_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "r128", "Rage 128 PRO State");
  BX_R128_THIS vgacore_register_state(list);
  bx_list_c *crtc = new bx_list_c(list, "crtc");
  new bx_shadow_num_c(crtc, "index", &BX_R128_THIS crtc.index, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_R128_THIS crtc.reg, R128_CRTC_MAX + 1, 1);
  BXRS_PARAM_BOOL(list, svga_unlock_special, BX_R128_THIS svga_unlock_special);
  BXRS_PARAM_BOOL(list, svga_double_width, BX_R128_THIS svga_double_width);
  new bx_shadow_num_c(list, "svga_xres", &BX_R128_THIS svga_xres);
  new bx_shadow_num_c(list, "svga_yres", &BX_R128_THIS svga_yres);
  new bx_shadow_num_c(list, "svga_pitch", &BX_R128_THIS svga_pitch);
  new bx_shadow_num_c(list, "svga_bpp", &BX_R128_THIS svga_bpp);
  new bx_shadow_num_c(list, "svga_dispbpp", &BX_R128_THIS svga_dispbpp);
  new bx_shadow_num_c(list, "bank_base0", &BX_R128_THIS bank_base[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_base1", &BX_R128_THIS bank_base[1], BASE_HEX);
  bx_list_c *cursor = new bx_list_c(list, "hw_cursor");
  new bx_shadow_num_c(cursor, "x", &BX_R128_THIS hw_cursor.x, BASE_HEX);
  new bx_shadow_num_c(cursor, "y", &BX_R128_THIS hw_cursor.y, BASE_HEX);
  new bx_shadow_num_c(cursor, "size", &BX_R128_THIS hw_cursor.size, BASE_HEX);
  register_pci_state(list);
}

void bx_r128_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(r128_mem_read_handler);
  BX_R128_THIS bx_vgacore_c::after_restore_state();
}

void bx_r128_c::redraw_area(unsigned x0, unsigned y0,
                               unsigned width, unsigned height)
{
  redraw_area((Bit32s)x0, (Bit32s)y0, width, height);
}

void bx_r128_c::redraw_area(Bit32u offset, Bit32u width, Bit32u height)
{
  if (BX_R128_THIS svga_pitch != 0) {
    Bit32u redraw_x = offset % BX_R128_THIS svga_pitch / (BX_R128_THIS svga_bpp >> 3);
    Bit32u redraw_y = offset / BX_R128_THIS svga_pitch;
    BX_R128_THIS redraw_area(redraw_x, redraw_y, width, height);
  }
}

void bx_r128_c::redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  if (x0 + (Bit32s)width <= 0 || y0 + (Bit32s)height <= 0)
    return;

  unsigned xti, yti, xt0, xt1, yt0, yt1;

  // Check CRTC_GEN_CNTL bit 24 (CRTC_EXT_DISP_EN): 0=VGA mode, 1=Extended mode
  // In VGA mode, use base VGA core redraw
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24))) {
    BX_R128_THIS bx_vgacore_c::redraw_area(x0, y0, width, height);
    return;
  }

  if (BX_R128_THIS svga_needs_update_mode) {
    return;
  }

  BX_R128_THIS svga_needs_update_tile = 1;

  xt0 = x0 <= 0 ? 0 : x0 / X_TILESIZE;
  yt0 = y0 <= 0 ? 0 : y0 / Y_TILESIZE;
  if (x0 < (Bit32s)BX_R128_THIS svga_xres) {
    xt1 = (x0 + width - 1) / X_TILESIZE;
  } else {
    xt1 = (BX_R128_THIS svga_xres - 1) / X_TILESIZE;
  }
  if (y0 < (Bit32s)BX_R128_THIS svga_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (BX_R128_THIS svga_yres - 1) / Y_TILESIZE;
  }
  if ((x0 + width) > BX_R128_THIS svga_xres) {
    BX_R128_THIS redraw_area(0, y0 + 1, x0 + width - BX_R128_THIS svga_xres, height);
  }
  for (yti=yt0; yti<=yt1; yti++) {
    for (xti=xt0; xti<=xt1; xti++) {
      SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 1);
    }
  }
}

void bx_r128_c::vertical_timer()
{
  bx_vgacore_c::vertical_timer();
  if (BX_R128_THIS vtimer_toggle) {
    BX_R128_THIS crt_intr |= 0x00000001;
    BX_DEBUG(("vertical_timer, crt_intr_en = %d",
      BX_R128_THIS crt_intr_en));
    set_irq_level(true);
  }
}

bool bx_r128_c::r128_mem_read_handler(bx_phy_address addr, unsigned len,
                                        void *data, void *param)
{
  // BAR0: Linear Frame Buffer (LFB) - VRAM access
  if (addr >= BX_R128_THIS pci_bar[0].addr &&
      addr < (BX_R128_THIS pci_bar[0].addr + (64 * 1024 * 1024))) {
    Bit32u offset = (addr - BX_R128_THIS pci_bar[0].addr) & ((64 * 1024 * 1024) - 1);
    Bit8u *data_ptr = (Bit8u *) data;
    for (unsigned i = 0; i < len; i++) {
      data_ptr[i] = BX_R128_THIS s.memory[offset + i];
    }
    return 1;
  }

  // BAR2: Register aperture (16KB)
  if (addr >= BX_R128_THIS pci_bar[2].addr &&
      addr < (BX_R128_THIS pci_bar[2].addr + 0x4000)) {
    Bit32u offset = addr & 0x3FFF;
    if (len == 1) {
      *(Bit8u*)data = register_read8(offset);
    } else if (len == 2) {
      // 16-bit read - read two bytes
      Bit16u value = register_read8(offset) | (register_read8(offset + 1) << 8);
      *((Bit8u*)data + 0) = (value >> 0) & 0xFF;
      *((Bit8u*)data + 1) = (value >> 8) & 0xFF;
    } else if (len == 4) {
      Bit32u value = register_read32(offset);
      *((Bit8u*)data + 0) = (value >> 0) & 0xFF;
      *((Bit8u*)data + 1) = (value >> 8) & 0xFF;
      *((Bit8u*)data + 2) = (value >> 16) & 0xFF;
      *((Bit8u*)data + 3) = (value >> 24) & 0xFF;
    } else if (len == 8) {
      // 64-bit read - read two 32-bit registers
      Bit32u value_low = register_read32(offset);
      Bit32u value_high = register_read32(offset + 4);
      *((Bit8u*)data + 0) = (value_low >> 0) & 0xFF;
      *((Bit8u*)data + 1) = (value_low >> 8) & 0xFF;
      *((Bit8u*)data + 2) = (value_low >> 16) & 0xFF;
      *((Bit8u*)data + 3) = (value_low >> 24) & 0xFF;
      *((Bit8u*)data + 4) = (value_high >> 0) & 0xFF;
      *((Bit8u*)data + 5) = (value_high >> 8) & 0xFF;
      *((Bit8u*)data + 6) = (value_high >> 16) & 0xFF;
      *((Bit8u*)data + 7) = (value_high >> 24) & 0xFF;
    } else {
      // Unsupported length - read byte by byte
      Bit8u *data_ptr = (Bit8u*)data;
      for (unsigned i = 0; i < len; i++) {
        data_ptr[i] = register_read8(offset + i);
      }
    }
    return 1;
  }

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = BX_R128_THIS mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

Bit8u bx_r128_c::mem_read(bx_phy_address addr)
{
  if (BX_R128_THIS pci_rom_size > 0) {
    Bit32u mask = (BX_R128_THIS pci_rom_size - 1);
    if (((Bit32u)addr & ~mask) == BX_R128_THIS pci_rom_address) {
      if (BX_R128_THIS pci_conf[0x30] & 0x01) {
        if (BX_R128_THIS pci_conf[0x50] == 0x00)
          return BX_R128_THIS pci_rom[addr & mask];
        else
          return BX_R128_THIS s.memory[addr & mask];
      } else {
        return 0xff;
      }
    }
  }

  // BAR0 (LFB) is handled by r128_mem_read_handler
  // BAR1 is I/O space, not memory space

  // Check CRTC_GEN_CNTL bit 24 (CRTC_EXT_DISP_EN): 0=VGA mode, 1=Extended mode
  // In VGA mode, use base VGA core memory read
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24)))
    return BX_R128_THIS bx_vgacore_c::mem_read(addr);

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset = addr & 0xffff;
    offset += BX_R128_THIS bank_base[0];
    offset &= (BX_R128_THIS s.memsize - 1);
    return BX_R128_THIS s.memory[offset];
  }

  return 0xFF;
}

bool bx_r128_c::r128_mem_write_handler(bx_phy_address addr, unsigned len,
                                         void *data, void *param)
{
  // BAR0: Linear Frame Buffer (LFB) - VRAM access
  if (addr >= BX_R128_THIS pci_bar[0].addr &&
      addr < (BX_R128_THIS pci_bar[0].addr + (64 * 1024 * 1024))) {
    Bit32u offset = (addr - BX_R128_THIS pci_bar[0].addr) & ((64 * 1024 * 1024) - 1);
    Bit8u *data_ptr = (Bit8u *) data;
    for (unsigned i = 0; i < len; i++) {
      BX_R128_THIS s.memory[offset + i] = data_ptr[i];
    }
    // Mark display as needing update
    BX_R128_THIS svga_needs_update_dispentire = 1;
    return 1;
  }

  // BAR2: Register aperture (16KB)
  if (addr >= BX_R128_THIS pci_bar[2].addr &&
      addr < (BX_R128_THIS pci_bar[2].addr + 0x4000)) {
    Bit32u offset = addr & 0x3FFF;
    if (len == 1) {
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
      register_write8(offset, *(Bit8u*)data);
    } else if (len == 2) {
      // 16-bit write - write two bytes
      Bit16u value =
        (*((Bit8u*)data + 0) << 0) |
        (*((Bit8u*)data + 1) << 8);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%04x (16-bit)", offset, value));
      register_write8(offset, (value >> 0) & 0xFF);
      register_write8(offset + 1, (value >> 8) & 0xFF);
    } else if (len == 4) {
      Bit32u value =
        (*((Bit8u*)data + 0) << 0) |
        (*((Bit8u*)data + 1) << 8) |
        (*((Bit8u*)data + 2) << 16) |
        (*((Bit8u*)data + 3) << 24);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%08x", offset, value));
      register_write32(offset, value);
    } else if (len == 8) {
      Bit64u value =
        ((Bit64u)*((Bit8u*)data + 0) << 0) |
        ((Bit64u)*((Bit8u*)data + 1) << 8) |
        ((Bit64u)*((Bit8u*)data + 2) << 16) |
        ((Bit64u)*((Bit8u*)data + 3) << 24) |
        ((Bit64u)*((Bit8u*)data + 4) << 32) |
        ((Bit64u)*((Bit8u*)data + 5) << 40) |
        ((Bit64u)*((Bit8u*)data + 6) << 48) |
        ((Bit64u)*((Bit8u*)data + 7) << 56);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%016" FMT_64 "x", offset, value));
      register_write32(offset, (Bit32u)value);
      register_write32(offset + 4, value >> 32);
    } else {
      // Unsupported length - write byte by byte
      Bit8u *data_ptr = (Bit8u*)data;
      for (unsigned i = 0; i < len; i++) {
        register_write8(offset + i, data_ptr[i]);
      }
    }
    return 1;
  }

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    BX_R128_THIS mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

void bx_r128_c::mem_write(bx_phy_address addr, Bit8u value)
{
  if ((addr >= BX_R128_THIS pci_bar[0].addr) &&
      (addr < (BX_R128_THIS pci_bar[0].addr + (64 * 1024 * 1024)))) {
    unsigned x, y;
    Bit32u offset = addr & ((64 * 1024 * 1024) - 1);
    BX_R128_THIS s.memory[offset] = value;
    if (BX_R128_THIS svga_pitch != 0) {
      BX_R128_THIS svga_needs_update_tile = 1;
      offset -= (Bit32u)(BX_R128_THIS disp_ptr - BX_R128_THIS s.memory);
      x = (offset % BX_R128_THIS svga_pitch) / (BX_R128_THIS svga_bpp / 8);
      y = offset / BX_R128_THIS svga_pitch;
      SET_TILE_UPDATED(BX_R128_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
    }
    return;
  }

  // Check CRTC_GEN_CNTL bit 24 (CRTC_EXT_DISP_EN): 0=VGA mode, 1=Extended mode
  // In VGA mode, use base VGA core memory write
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24))) {
    BX_R128_THIS bx_vgacore_c::mem_write(addr, value);
    return;
  }

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset;
    unsigned x, y;

    offset = addr & 0xffff;
    // Check CRTC register 0x1C bit 7 (Line Compare bit 8) for split screen
    if (BX_R128_THIS crtc.reg[0x1c] & 0x80) {
      BX_R128_THIS s.memory[offset] = value;
      return;
    }
    offset += BX_R128_THIS bank_base[0];
    offset &= (BX_R128_THIS s.memsize - 1);
    BX_R128_THIS s.memory[offset] = value;
    BX_R128_THIS svga_needs_update_tile = 1;
    x = (offset % BX_R128_THIS svga_pitch) / (BX_R128_THIS svga_bpp / 8);
    y = offset / BX_R128_THIS svga_pitch;
    SET_TILE_UPDATED(BX_R128_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
  }
}

void bx_r128_c::get_text_snapshot(Bit8u **text_snapshot,
                                    unsigned *txHeight, unsigned *txWidth)
{
  BX_R128_THIS bx_vgacore_c::get_text_snapshot(text_snapshot,txHeight,txWidth);
}

Bit32u bx_r128_c::svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_R128_SMF
  bx_r128_c *class_ptr = (bx_r128_c *) this_ptr;

  return class_ptr->svga_read(address, io_len);
}

Bit32u bx_r128_c::svga_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_R128_SMF

  if (address == 0x03C3 && io_len == 2)
    return VGA_READ(address, 1) | VGA_READ(address + 1, 1) << 8;

  // ATI Rage 128 PRO does not have RMA (Register Memory Access) registers
  // Ports 0x03D0-0x03D5 are standard VGA CRTC registers

  // Handle multi-byte I/O operations
  if (io_len == 2 && (address & 1) == 0) {
    // 16-bit read from aligned address - split into two 8-bit reads
    Bit32u value;
    value = (Bit32u)SVGA_READ(address, 1);
    value |= (Bit32u)SVGA_READ(address + 1, 1) << 8;
    return value;
  } else if (io_len == 4 && (address & 3) == 0) {
    // 32-bit read from aligned address - split into four 8-bit reads
    Bit32u value;
    value = (Bit32u)SVGA_READ(address, 1);
    value |= (Bit32u)SVGA_READ(address + 1, 1) << 8;
    value |= (Bit32u)SVGA_READ(address + 2, 1) << 16;
    value |= (Bit32u)SVGA_READ(address + 3, 1) << 24;
    return value;
  } else if (io_len != 1) {
    // Unaligned or unsupported multi-byte read - split into byte reads
    Bit32u value = 0;
    for (unsigned i = 0; i < io_len && i < 4; i++) {
      value |= (Bit32u)SVGA_READ(address + i, 1) << (i * 8);
    }
    return value;
  }

  switch (address) {
    case 0x03b4: /* VGA: CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* VGA: CRTC Index Register (color emulation modes) */
      return BX_R128_THIS crtc.index;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      // ATI Rage 128 PRO supports standard VGA CRTC registers (0x00-0x18)
      // Extended registers beyond VGA_CRTC_MAX are handled by svga_read_crtc
      if (BX_R128_THIS crtc.index > VGA_CRTC_MAX && BX_R128_THIS crtc.index <= R128_CRTC_MAX)
        return BX_R128_THIS svga_read_crtc(address,BX_R128_THIS crtc.index);
      else
        break;
    case 0x03c2: /* Input Status 0 */
      BX_DEBUG(("Input Status 0 read"));
      return 0x10; // Monitor presence detection (DAC Sensing)
    case 0xe000: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe001: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe002: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe003: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe004: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe005: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe006: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe007: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe016: /* Extended VGA port - return 0 to suppress log messages */
    case 0xe017: /* Extended VGA port - return 0 to suppress log messages */
      return 0;
    default:
      break;
  }

  return VGA_READ(address,io_len);
}

void bx_r128_c::svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_R128_SMF
  bx_r128_c *class_ptr = (bx_r128_c *) this_ptr;
  class_ptr->svga_write(address, value, io_len);
}

void bx_r128_c::svga_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_R128_SMF

  // ATI Rage 128 PRO does not have RMA (Register Memory Access) registers
  // Ports 0x03D0-0x03D5 are standard VGA CRTC registers

  // Handle multi-byte I/O operations
  if (io_len == 2 && (address & 1) == 0) {
    // 16-bit write to aligned address - split into two 8-bit writes
    SVGA_WRITE(address, value & 0xff, 1);
    SVGA_WRITE(address + 1, (value >> 8) & 0xff, 1);
    return;
  } else if (io_len == 4 && (address & 3) == 0) {
    // 32-bit write to aligned address - split into four 8-bit writes
    SVGA_WRITE(address, value & 0xff, 1);
    SVGA_WRITE(address + 1, (value >> 8) & 0xff, 1);
    SVGA_WRITE(address + 2, (value >> 16) & 0xff, 1);
    SVGA_WRITE(address + 3, (value >> 24) & 0xff, 1);
    return;
  } else if (io_len != 1) {
    // Unaligned or unsupported multi-byte write - split into byte writes
    for (unsigned i = 0; i < io_len && i < 4; i++) {
      SVGA_WRITE(address + i, (value >> (i * 8)) & 0xff, 1);
    }
    return;
  }

  switch (address) {
    case 0x03b4: /* VGA: CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* VGA: CRTC Index Register (color emulation modes) */
      BX_R128_THIS crtc.index = value;
      break;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      // Standard VGA CRTC registers (0x00-0x18)
      if (BX_R128_THIS crtc.index <= VGA_CRTC_MAX) {
        BX_R128_THIS crtc.reg[BX_R128_THIS crtc.index] = value;
        // ATI Rage 128 PRO standard VGA CRTC registers that trigger mode update
        if (BX_R128_THIS crtc.index == 0x01 ||  // H_DISP_END
            BX_R128_THIS crtc.index == 0x07 ||  // CRTC Overflow
            BX_R128_THIS crtc.index == 0x09 ||  // MAX_ROW_SCAN
            BX_R128_THIS crtc.index == 0x0c ||  // START_ADDR_H
            BX_R128_THIS crtc.index == 0x0d ||  // START_ADDR_L
            BX_R128_THIS crtc.index == 0x12 ||  // V_DISP_END
            BX_R128_THIS crtc.index == 0x13 ||  // OFFSET
            BX_R128_THIS crtc.index == 0x15) {  // UNDERLINE_LOC
          BX_R128_THIS svga_needs_update_mode = 1;
        }
        // Translate VGA CRTC registers to accelerator CRTC registers
        BX_R128_THIS svga_write_crtc(address, BX_R128_THIS crtc.index, value);
      } 
      // Extended ATI Rage 128 PRO CRTC registers (0x19-0x9F)
      else if (BX_R128_THIS crtc.index <= R128_CRTC_MAX) {
        BX_R128_THIS crtc.reg[BX_R128_THIS crtc.index] = value;
        // CRTC 0x19 (MODE_CNTL) - check if extended mode should be enabled
        if (BX_R128_THIS crtc.index == 0x19) {
          // If bit 7 is set, enable extended display mode
          // This is a common pattern for ATI cards to switch from VGA to extended mode
          if (value & 0x80) {
            BX_R128_THIS crtc_gen_cntl |= (1 << 24); // Set CRTC_EXT_DISP_EN
          }
          BX_R128_THIS svga_needs_update_mode = 1;
        }
        BX_R128_THIS svga_write_crtc(address, BX_R128_THIS crtc.index, value);
        return;
      }
      break;
    case 0xe000: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe001: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe002: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe003: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe004: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe005: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe006: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe007: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe016: /* Extended VGA port - ignore writes to suppress log messages */
    case 0xe017: /* Extended VGA port - ignore writes to suppress log messages */
      return;
    default:
      break;
  }

  VGA_WRITE(address,value,io_len);
}

static Bit8u alpha_wrap(int value)
{
  return -(value >> 8) ^ value;
}

void bx_r128_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  // ATI Rage 128 PRO Hardware Cursor Implementation
  // According to datasheet: 2bpp monochrome 64x64 cursor
  // CRTC_GEN_CNTL bit 16 (CRTC_CUR_EN): Hardware Cursor Enable
  // CRTC_GEN_CNTL bits 19:17 (CRTC_CUR_MODE): Hardware Cursor Mode (0 = 2bpp monochrome)
  
  // Check if hardware cursor is enabled
  bool cursor_enabled = (BX_R128_THIS crtc_gen_cntl & (1 << 16)) != 0;
  if (!cursor_enabled) {
    return;
  }
  
  // Check cursor mode (bits 19:17), mode 0 = 2bpp monochrome 64x64
  Bit32u cursor_mode = (BX_R128_THIS crtc_gen_cntl >> 17) & 0x7;
  if (cursor_mode != 0) {
    // Only mode 0 (2bpp monochrome) is supported
    return;
  }
  
  // CUR_HORZ_VERT_POSN: CUR_HORZ_POSN is bits 26:16, CUR_VERT_POSN is bits 10:0
  Bit32u cur_horz_pos = (BX_R128_THIS crtc_cur_hv_pos >> 16) & 0x7FF;
  Bit32u cur_vert_pos = BX_R128_THIS crtc_cur_hv_pos & 0x7FF;
  
  // CUR_HORZ_VERT_OFF: CUR_HORZ_OFF is bits 21:16, CUR_VERT_OFF is bits 5:0
  // Cursor width is always 64 pixels, height is (64 - CUR_VERT_OFF)
  Bit32u cur_horz_off = (BX_R128_THIS crtc_cur_hv_offs >> 16) & 0x3F;
  Bit32u cur_vert_off = BX_R128_THIS crtc_cur_hv_offs & 0x3F;
  Bit32u cursor_width = 64;
  Bit32u cursor_height = 64 - cur_vert_off;
  
  // Cursor position on screen (accounting for offsets)
  Bit16s hwcx = (Bit16s)cur_horz_pos - (Bit16s)cur_horz_off;
  Bit16s hwcy = (Bit16s)cur_vert_pos - (Bit16s)cur_vert_off;
  
  // CUR_OFFSET: Hardware cursor address offset in frame buffer (16-byte aligned)
  // Bits 3:0 are hardwired to ZERO
  Bit32u cursor_offset = BX_R128_THIS crtc_cur_offset & 0xFFFFFFF0;
  
  // CUR_CLR0 and CUR_CLR1: Cursor colors (24-bit RGB)
  Bit8u cur_clr0_r = (BX_R128_THIS crtc_cur_color0 >> 16) & 0xFF;
  Bit8u cur_clr0_g = (BX_R128_THIS crtc_cur_color0 >> 8) & 0xFF;
  Bit8u cur_clr0_b = BX_R128_THIS crtc_cur_color0 & 0xFF;
  Bit8u cur_clr1_r = (BX_R128_THIS crtc_cur_color1 >> 16) & 0xFF;
  Bit8u cur_clr1_g = (BX_R128_THIS crtc_cur_color1 >> 8) & 0xFF;
  Bit8u cur_clr1_b = BX_R128_THIS crtc_cur_color1 & 0xFF;
  
  unsigned w, h;
  Bit8u* tile_ptr;

  if (info->snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    w = BX_R128_THIS svga_xres;
    h = BX_R128_THIS svga_yres;
  } else {
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  }
  
  // Check if cursor overlaps with the tile
  if ((int)xc < hwcx + (int)cursor_width &&
      (int)(xc + w) > hwcx &&
      (int)yc < hwcy + (int)cursor_height &&
      (int)(yc + h) > hwcy) {
    unsigned cx0 = hwcx > (int)xc ? hwcx : xc;
    unsigned cy0 = hwcy > (int)yc ? hwcy : yc;
    unsigned cx1 = (int)(hwcx + cursor_width) < (int)(xc + w) ? hwcx + cursor_width : xc + w;
    unsigned cy1 = (int)(hwcy + cursor_height) < (int)(yc + h) ? hwcy + cursor_height : yc + h;

    Bit8u display_color_bytes = BX_R128_THIS svga_bpp >> 3;
    if (info->bpp == 15) info->bpp = 16;
    tile_ptr += info->pitch * (cy0 - yc) + info->bpp / 8 * (cx0 - xc);
    
    // Cursor data is 2bpp (2 bits per pixel), stored as 64x64 bitmap
    // Each row is 64 pixels = 16 bytes (64 * 2 / 8)
    unsigned cursor_pitch = 16; // 64 pixels * 2 bits / 8 bits per byte
    
    // Calculate cursor data pointer
    Bit32u cursor_row = cy0 - hwcy + cur_vert_off;
    Bit32u cursor_col_start = cx0 - hwcx + cur_horz_off;
    Bit8u* cursor_data = BX_R128_THIS s.memory + cursor_offset + cursor_row * cursor_pitch;
    
    Bit8u* vid_ptr = BX_R128_THIS disp_ptr + BX_R128_THIS svga_pitch * cy0;
    
    for (unsigned cy = cy0; cy < cy1; cy++) {
      Bit8u* tile_ptr2 = tile_ptr;
      Bit8u* cursor_row_data = cursor_data + (cursor_col_start / 4); // 4 pixels per byte (2bpp)
      Bit8u cursor_bit_offset = (cursor_col_start % 4) * 2; // 2 bits per pixel
      Bit8u* vid_ptr2 = vid_ptr + display_color_bytes * cx0;
      
      for (unsigned cx = cx0; cx < cx1; cx++) {
        // Read 2-bit cursor pixel value
        Bit8u cursor_byte = cursor_row_data[0];
        Bit8u cursor_pixel = (cursor_byte >> cursor_bit_offset) & 0x3;
        
        // Cursor pixel encoding: 00=transparent, 01=color0, 10=color1, 11=inverse (XOR)
        if (cursor_pixel != 0) {
          Bit8u cr, cg, cb;
          
          if (cursor_pixel == 1) {
            // Color 0
            cr = cur_clr0_r;
            cg = cur_clr0_g;
            cb = cur_clr0_b;
          } else if (cursor_pixel == 2) {
            // Color 1
            cr = cur_clr1_r;
            cg = cur_clr1_g;
            cb = cur_clr1_b;
          } else { // cursor_pixel == 3
            // Inverse (XOR with background)
            Bit8u dr, dg, db;
            if (display_color_bytes == 1) {
              dr = dg = db = vid_ptr2[0];
            } else if (display_color_bytes == 2) {
              EXTRACT_565_TO_888(vid_ptr2[0] << 0 | vid_ptr2[1] << 8, dr, dg, db);
            } else {
              db = vid_ptr2[0];
              dg = vid_ptr2[1];
              dr = vid_ptr2[2];
            }
            cr = dr ^ 0xFF;
            cg = dg ^ 0xFF;
            cb = db ^ 0xFF;
          }
          
          Bit32u color = cb << 0 | cg << 8 | cr << 16;
          
          if (!info->is_indexed) {
            color = MAKE_COLOUR(
              color, 24, info->red_shift, info->red_mask,
              color, 16, info->green_shift, info->green_mask,
              color, 8, info->blue_shift, info->blue_mask);
            if (info->is_little_endian) {
              for (int i = 0; i < info->bpp; i += 8) {
                *(tile_ptr2++) = (Bit8u)(color >> i);
              }
            } else {
              for (int i = info->bpp - 8; i > -8; i -= 8) {
                *(tile_ptr2++) = (Bit8u)(color >> i);
              }
            }
          } else {
            *(tile_ptr2++) = (Bit8u)color;
          }
        } else {
          // Transparent - skip this pixel (don't modify tile)
          tile_ptr2 += info->bpp / 8;
        }
        
        // Advance cursor bit position
        cursor_bit_offset += 2;
        if (cursor_bit_offset >= 8) {
          cursor_bit_offset = 0;
          cursor_row_data++;
        }
        vid_ptr2 += display_color_bytes;
      }
      tile_ptr += info->pitch;
      cursor_data += cursor_pitch;
      vid_ptr += BX_R128_THIS svga_pitch;
    }
  }
}

Bit16u bx_r128_c::get_crtc_vtotal()
{
  // Check CRTC_EXT_DISP_EN (bit 24): 0=VGA mode, 1=Extended mode
  // If extended mode is not enabled, use VGA CRTC registers
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24))) {
    // Use standard VGA CRTC registers (same as bx_vgacore_c::get_crtc_params)
    return (Bit16u)(BX_R128_THIS crtc.reg[6] +
                     ((BX_R128_THIS crtc.reg[7] & 0x01) << 8) +
                     ((BX_R128_THIS crtc.reg[7] & 0x20) << 4) + 2);
  }
  
  // Use ATI Rage 128 PRO accelerator CRTC registers
  // CRTC_V_TOTAL_DISP: V_TOTAL is bits 10:0, value represents total lines minus 2
  return (Bit16u)(BX_R128_THIS crtc_v_total_disp & 0x7FF) + 2;
}

void bx_r128_c::get_crtc_params(bx_crtc_params_t* crtcp, Bit32u* vclock)
{
  // Check CRTC_EXT_DISP_EN (bit 24): 0=VGA mode, 1=Extended mode
  // If extended mode is not enabled, use VGA mode
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24))) {
    bx_vgacore_c::get_crtc_params(crtcp, vclock);
    return;
  }
  
  // Use ATI Rage 128 PRO accelerator CRTC registers
  // ATI Rage 128 PLL calculation
  Bit32u m = BX_R128_THIS pll_vclk_cntl & 0xFF;
  Bit32u n = BX_R128_THIS pll_vclk_cntl >> 8 & 0xFF;
  Bit32u p = BX_R128_THIS pll_vclk_cntl >> 16 & 0x07;
  Bit32u mb = 1;
  Bit32u nb = 1;
  if (BX_R128_THIS pll_vclk_cntl2 & 0x00000080) {
    mb = BX_R128_THIS pll_vclk_cntl2 >> 4 & 0x7;
    nb = (BX_R128_THIS pll_vclk_cntl2 >> 21) & 0x18 |
         BX_R128_THIS pll_vclk_cntl2 >> 19 & 0x7;
  }
  if (BX_R128_THIS pll_gen_cntl & 0x200 && m && mb) {
    Bit32u crystalFreq = 13500000; // Default crystal frequency for Rage 128
    if (BX_R128_THIS config_xstrap & 0x00000040)
      crystalFreq = 14318000;
    if (BX_R128_THIS config_xstrap & 0x00400000)
      crystalFreq = 27000000;
    *vclock = (Bit32u)((Bit64u)crystalFreq * n * nb / m / mb >> p);
  } else {
    // PLL not configured - fall back to VGA mode
    bx_vgacore_c::get_crtc_params(crtcp, vclock);
    return;
  }
  
  // CRTC_H_TOTAL_DISP: H_TOTAL is bits 8:0, H_DISP is bits 23:16
  // H_TOTAL value is in pixels * 8, represents total
  crtcp->htotal = (BX_R128_THIS crtc_h_total_disp & 0x1FF);
  
  // CRTC_V_TOTAL_DISP: V_TOTAL is bits 10:0, V_DISP is bits 26:16
  // V_TOTAL value represents total - 2
  crtcp->vtotal = get_crtc_vtotal();
  
  // Vertical blank start: CRTC_V_SYNC_STRT + CRTC_V_SYNC_WID
  // CRTC_V_SYNC_STRT_WID: V_SYNC_STRT is bits 10:0, V_SYNC_WID is bits 20:16
  Bit32u v_sync_strt = BX_R128_THIS crtc_v_sync_strt_wid & 0x7FF;
  Bit32u v_sync_wid = (BX_R128_THIS crtc_v_sync_strt_wid >> 16) & 0x1F;
  crtcp->vbstart = v_sync_strt + v_sync_wid;
  
  // Vertical retrace start: CRTC_V_SYNC_STRT
  crtcp->vrstart = v_sync_strt;
}

void bx_r128_c::update(void)
{
  unsigned width, height, pitch;

  // Check CRTC_EXT_DISP_EN (bit 24): 0=VGA mode, 1=Extended mode
  // If extended mode is not enabled, use VGA mode
  if (!(BX_R128_THIS crtc_gen_cntl & (1 << 24))) {
    bx_vgacore_c::update();
    return;
  }

  // Use ATI Rage 128 PRO accelerator CRTC registers
  if (BX_R128_THIS svga_needs_update_mode) {
    // CRTC_OFFSET: Primary graphics display base address in bytes
    Bit32u iTopOffset = BX_R128_THIS crtc_offset;

    // CRTC_PITCH: Display line pitch in (pixels * 8), bits 9:0
    Bit32u iPitch = (BX_R128_THIS crtc_pitch & 0x3FF) << 3;

    // CRTC_PIX_WIDTH: Display pixel width (bits 10:8 of CRTC_GEN_CNTL)
    // 1=4bpp, 2=8bpp, 3=15bpp, 4=16bpp, 5=24bpp, 6=32bpp
    Bit8u pix_width = (BX_R128_THIS crtc_gen_cntl >> 8) & 0x07;
    Bit8u iBpp;
    if (pix_width == 1)
      iBpp = 4;
    else if (pix_width == 2)
      iBpp = 8;
    else if (pix_width == 3)
      iBpp = 15;
    else if (pix_width == 4)
      iBpp = 16;
    else if (pix_width == 5)
      iBpp = 24;
    else if (pix_width == 6)
      iBpp = 32;
    else if (pix_width == 0) {
      // Pixel width 0 means registers not initialized yet - use VGA mode
      BX_DEBUG(("CRTC_PIX_WIDTH is 0, using VGA mode"));
      bx_vgacore_c::update();
      return;
    } else {
      BX_ERROR(("unknown pixel width %d, defaulting to 8bpp", pix_width));
      iBpp = 8;
    }

    // CRTC_H_TOTAL_DISP: H_DISP is bits 23:16
    // Value is in pixels * 8, represents display width - 1
    Bit32u iWidth = ((BX_R128_THIS crtc_h_total_disp >> 16) & 0xFF) + 1;
    iWidth <<= 3; // Convert from characters to pixels
    
    // CRTC_V_TOTAL_DISP: V_DISP is bits 26:16
    // Value represents display height - 1
    Bit32u iHeight = ((BX_R128_THIS crtc_v_total_disp >> 16) & 0x7FF) + 1;

    if ((iWidth != BX_R128_THIS svga_xres) || (iHeight != BX_R128_THIS svga_yres)
        || (iBpp != BX_R128_THIS svga_bpp)) {
      BX_INFO(("switched to %u x %u x %u", iWidth, iHeight, iBpp));
    }

    BX_R128_THIS svga_xres = iWidth;
    BX_R128_THIS svga_yres = iHeight;
    BX_R128_THIS svga_bpp = iBpp;
    BX_R128_THIS svga_dispbpp = iBpp;
    BX_R128_THIS disp_ptr = BX_R128_THIS s.memory + iTopOffset;
    BX_R128_THIS svga_pitch = iPitch;
    // compatibilty settings for VGA core
    BX_R128_THIS s.last_xres = iWidth;
    BX_R128_THIS s.last_yres = iHeight;
    BX_R128_THIS s.last_bpp = iBpp;
    BX_R128_THIS s.last_fh = 0;
  }

  if (BX_R128_THIS svga_dispbpp != 4) {
    width  = BX_R128_THIS svga_xres;
    height = BX_R128_THIS svga_yres;
    pitch = BX_R128_THIS svga_pitch;
    if (BX_R128_THIS svga_needs_update_mode) {
      bx_gui->dimension_update(width, height, 0, 0, BX_R128_THIS svga_dispbpp);
      BX_R128_THIS s.last_bpp = BX_R128_THIS svga_dispbpp;
      BX_R128_THIS svga_needs_update_mode = 0;
      BX_R128_THIS svga_needs_update_dispentire = 1;
    }
  } else {
    BX_R128_THIS determine_screen_dimensions(&height, &width);
    pitch = BX_R128_THIS s.line_offset;
    if ((width != BX_R128_THIS s.last_xres) || (height != BX_R128_THIS s.last_yres) ||
        (BX_R128_THIS s.last_bpp > 8)) {
      bx_gui->dimension_update(width, height);
      BX_R128_THIS s.last_xres = width;
      BX_R128_THIS s.last_yres = height;
      BX_R128_THIS s.last_bpp = 8;
    }
  }

  if (BX_R128_THIS svga_needs_update_dispentire) {
    BX_R128_THIS redraw_area(0,0,width,height);
    BX_R128_THIS svga_needs_update_dispentire = 0;
  }

  if (!BX_R128_THIS svga_needs_update_tile) {
    return;
  }
  BX_R128_THIS svga_needs_update_tile = 0;

  unsigned xc, yc, xti, yti, hp;
  unsigned r, c, w, h, x, y;
  int i;
  Bit8u red, green, blue;
  Bit32u colour, row_addr;
  Bit8u * vid_ptr, * vid_ptr2;
  Bit8u * tile_ptr, * tile_ptr2;
  bx_svga_tileinfo_t info;

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = BX_R128_THIS disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < height; yc++) {
          if (BX_R128_THIS svga_dispbpp != 4) {
            memcpy(tile_ptr, vid_ptr, info.pitch);
            vid_ptr += pitch;
          } else {
            tile_ptr2 = tile_ptr;
            row_addr = BX_R128_THIS s.CRTC.start_addr + (yc * pitch);
            for (xc = 0; xc < width; xc++) {
              *(tile_ptr2++) = BX_R128_THIS get_vga_pixel(xc, yc, row_addr, 0xffff, 0, BX_R128_THIS s.memory);
            }
          }
          tile_ptr += info.pitch;
        }
        draw_hardware_cursor(0, 0, &info);
      }
    } else if (info.is_indexed) {
      switch (BX_R128_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  y = yc + r;
                  if (BX_R128_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_R128_THIS s.CRTC.start_addr + (y * pitch);
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    x = xc + c;
                    *(tile_ptr2++) = BX_R128_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_R128_THIS s.memory);
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
        case 16:
        case 24:
        case 32:
          BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays, svga_dispbpp=%d",
            BX_R128_THIS svga_dispbpp));
          break;
        case 8:
          hp = BX_R128_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    *(tile_ptr2++) = colour;
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
      }
    }
    else {
      switch (BX_R128_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti=0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<Y_TILESIZE; r++) {
                  tile_ptr2 = tile_ptr;
                  y = yc + r;
                  if (BX_R128_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_R128_THIS s.CRTC.start_addr + (y * pitch);
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    colour = BX_R128_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_R128_THIS s.memory);
                    colour = MAKE_COLOUR(
                      BX_R128_THIS s.pel.data[colour].red, 6, info.red_shift, info.red_mask,
                      BX_R128_THIS s.pel.data[colour].green, 6, info.green_shift, info.green_mask,
                      BX_R128_THIS s.pel.data[colour].blue, 6, info.blue_shift, info.blue_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 8:
          hp = BX_R128_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    colour = MAKE_COLOUR(
                      BX_R128_THIS s.pel.data[colour].red, 6, info.red_shift, info.red_mask,
                      BX_R128_THIS s.pel.data[colour].green, 6, info.green_shift, info.green_mask,
                      BX_R128_THIS s.pel.data[colour].blue, 6, info.blue_shift, info.blue_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
          hp = BX_R128_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 2;
                    }
                    colour = MAKE_COLOUR(
                      colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                      colour & 0x03e0, 10, info.green_shift, info.green_mask,
                      colour & 0x7c00, 15, info.red_shift, info.red_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 16:
          hp = BX_R128_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 2;
                    }
                    colour = MAKE_COLOUR(
                      colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                      colour & 0x07e0, 11, info.green_shift, info.green_mask,
                      colour & 0xf800, 16, info.red_shift, info.red_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 24:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + 3 * xc);
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + 3 * xc);
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + 3 * (xc >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    blue = *(vid_ptr2);
                    green = *(vid_ptr2+1);
                    red = *(vid_ptr2+2);
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 3;
                    }
                    colour = MAKE_COLOUR(
                      red, 8, info.red_shift, info.red_mask,
                      green, 8, info.green_shift, info.green_mask,
                      blue, 8, info.blue_shift, info.blue_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 32:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_R128_THIS s.y_doublescan) {
                  vid_ptr = BX_R128_THIS disp_ptr + (yc * pitch + (xc << 2));
                } else {
                  if (!BX_R128_THIS svga_double_width) {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 2));
                  } else {
                    vid_ptr = BX_R128_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    blue = *(vid_ptr2);
                    green = *(vid_ptr2+1);
                    red = *(vid_ptr2+2);
                    if (!BX_R128_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 4;
                    }
                    colour = MAKE_COLOUR(
                      red, 8, info.red_shift, info.red_mask,
                      green, 8, info.green_shift, info.green_mask,
                      blue, 8, info.blue_shift, info.blue_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    } else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_R128_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_R128_THIS, xti, yti, 0);
              }
            }
          }
          break;
      }
    }
  }
  else {
    BX_PANIC(("cannot get svga tile info"));
  }
}

Bit8u bx_r128_c::svga_read_crtc(Bit32u address, unsigned index)
{
  // ATI Rage 128 PRO CRTC registers
  // Standard VGA CRTC registers (0x00-0x18) are handled by base VGA core
  
  if (index <= VGA_CRTC_MAX) {
    // Standard VGA CRTC register - should be handled by base VGA core
    return BX_R128_THIS crtc.reg[index];
  }
  
  // ATI Rage 128 PRO specific CRTC registers
  // CRT1E (0x1E): Graphics Controller Index Decode Register (read-only)
  if (index == 0x1E) {
    // Read back graphics controller index decode
    // Bit 1: GRPH_DEC_RD1
    // Returns graphics controller index decode status
    return BX_R128_THIS crtc.reg[index];
  }
  // CRT1F (0x1F): Graphics Controller Index Decode Register (read-only)
  else if (index == 0x1F) {
    // Read back graphics controller index decode
    // Bits 7:0: GRPH_DEC_RD0
    // Returns graphics controller index decode status
    return BX_R128_THIS crtc.reg[index];
  }
  // CRT22 (0x22): RAM Data Latch Readback Register (read-only)
  else if (index == 0x22) {
    // Read data in Graphics Controller CPU data latches
    // Bits 7:0: GRPH_LATCH_DATA
    // The Graphics Controller Read Map Select register bits 0 and 1 determines which byte is read back
    return BX_R128_THIS crtc.reg[index];
  }
  
  // Shadow CRTC registers (0x40-0x5F) are accessed via bit 6 of CRTC index register
  // Extended registers beyond 0x22 are not documented in ATI Rage 128 PRO datasheet
  if (index > R128_CRTC_MAX) {
    BX_PANIC(("crtc: unknown extended index 0x%02x read", index));
    return 0xff;
  }
  
  // For other extended CRTC registers, return stored value (for compatibility)
  Bit8u value = BX_R128_THIS crtc.reg[index];
  BX_DEBUG(("crtc: extended index 0x%02x read 0x%02x", index, value));
  return value;
}

void bx_r128_c::svga_write_crtc(Bit32u address, unsigned index, Bit8u value)
{
  // ATI Rage 128 PRO CRTC registers
  // Standard VGA CRTC registers (0x00-0x18) need to be translated to accelerator CRTC registers
  
  if (index <= VGA_CRTC_MAX) {
    // Standard VGA CRTC register - store it and translate to accelerator CRTC registers
    BX_R128_THIS crtc.reg[index] = value;
    
    // Translate VGA CRTC registers to ATI Rage 128 PRO accelerator CRTC registers
    // This allows Linux/Windows to set modes via VGA CRTC registers, which are then
    // automatically translated to accelerator CRTC registers when extended mode is enabled
    
    // CRTC 0x00: H_TOTAL (horizontal total - 5)
    // CRTC 0x01: H_DISP_END (horizontal display end)
    // CRTC 0x02: H_BLANK_START
    // CRTC 0x03: H_BLANK_END
    // CRTC 0x04: H_SYNC_START
    // CRTC 0x05: H_SYNC_END
    if (index == 0x00 || index == 0x01 || index == 0x02 || index == 0x03 || 
        index == 0x04 || index == 0x05) {
      // Update CRTC_H_TOTAL_DISP (0x0200)
      // H_TOTAL is in CRTC 0x00 (value + 5), bits 8:0
      // H_DISP is in CRTC 0x01, bits 7:0, needs to be in bits 23:16
      Bit32u h_total = (BX_R128_THIS crtc.reg[0x00] + 5) & 0x1FF;
      Bit32u h_disp = BX_R128_THIS crtc.reg[0x01] & 0xFF;
      BX_R128_THIS crtc_h_total_disp = h_total | (h_disp << 16);
      
      // Update CRTC_H_SYNC_STRT_WID (0x0204)
      // H_SYNC_STRT is in CRTC 0x04, bits 7:0, needs to be in bits 11:3
      // H_SYNC_WID is in CRTC 0x05, bits 4:0, needs to be in bits 21:16
      Bit32u h_sync_strt = (BX_R128_THIS crtc.reg[0x04] & 0xFF) << 3;
      Bit32u h_sync_wid = (BX_R128_THIS crtc.reg[0x05] & 0x1F) << 16;
      BX_R128_THIS crtc_h_sync_strt_wid = h_sync_strt | h_sync_wid;
    }
    
    // CRTC 0x06: V_TOTAL (vertical total - 2, bits 7:0)
    // CRTC 0x07: Overflow (contains V_TOTAL bits 8-9, V_DISP_END bits 8-9, V_SYNC_STRT bits 8-9)
    // CRTC 0x12: V_DISP_END (vertical display end, bits 7:0)
    if (index == 0x06 || index == 0x07 || index == 0x12) {
      // Update CRTC_V_TOTAL_DISP (0x0208)
      // V_TOTAL is in CRTC 0x06 (bits 7:0) + CRTC 0x07 (bits 0, 5), value - 2, bits 10:0
      Bit32u v_total = (BX_R128_THIS crtc.reg[0x06] & 0xFF) |
                       ((BX_R128_THIS crtc.reg[0x07] & 0x01) << 8) |
                       ((BX_R128_THIS crtc.reg[0x07] & 0x20) << 4);
      v_total = (v_total + 2) & 0x7FF; // Add 2, mask to 11 bits
      
      // V_DISP is in CRTC 0x12 (bits 7:0) + CRTC 0x07 (bits 1, 6), value - 1, bits 26:16
      Bit32u v_disp = (BX_R128_THIS crtc.reg[0x12] & 0xFF) |
                      ((BX_R128_THIS crtc.reg[0x07] & 0x02) << 7) |
                      ((BX_R128_THIS crtc.reg[0x07] & 0x40) << 2);
      v_disp = (v_disp + 1) & 0x7FF; // Add 1, mask to 11 bits
      
      BX_R128_THIS crtc_v_total_disp = v_total | (v_disp << 16);
    }
    
    // CRTC 0x10: V_SYNC_STRT (vertical sync start, bits 7:0)
    // CRTC 0x11: V_SYNC_END (vertical sync end, bits 3:0)
    // CRTC 0x07: Overflow (contains V_SYNC_STRT bits 8-9)
    if (index == 0x10 || index == 0x11 || index == 0x07) {
      // Update CRTC_V_SYNC_STRT_WID (0x020C)
      // V_SYNC_STRT is in CRTC 0x10 (bits 7:0) + CRTC 0x07 (bits 2, 7), bits 10:0
      Bit32u v_sync_strt = (BX_R128_THIS crtc.reg[0x10] & 0xFF) |
                           ((BX_R128_THIS crtc.reg[0x07] & 0x04) << 6) |
                           ((BX_R128_THIS crtc.reg[0x07] & 0x80) << 2);
      v_sync_strt &= 0x7FF; // Mask to 11 bits
      
      // V_SYNC_WID is in CRTC 0x11 (bits 3:0), needs to be in bits 20:16
      Bit32u v_sync_wid = (BX_R128_THIS crtc.reg[0x11] & 0x0F) << 16;
      
      BX_R128_THIS crtc_v_sync_strt_wid = v_sync_strt | v_sync_wid;
    }
    
    // CRTC 0x13: OFFSET (line offset/pitch, bits 7:0)
    // CRTC 0x1B: Extended offset (bits 4:0 for bits 12:8 of offset)
    if (index == 0x13) {
      // Update CRTC_PITCH (0x022C)
      // Pitch is in CRTC 0x13 (bits 7:0) + CRTC 0x1B (bits 4:0 for bits 12:8)
      // ATI Rage 128 PRO CRTC_PITCH is in pixels * 8, bits 9:0
      // VGA OFFSET is in characters (8 pixels), so we can use it directly
      Bit32u pitch = BX_R128_THIS crtc.reg[0x13] & 0xFF;
      if (BX_R128_THIS crtc.reg[0x1B] & 0x1F) {
        pitch |= ((BX_R128_THIS crtc.reg[0x1B] & 0x1F) << 8);
      }
      // CRTC_PITCH stores pitch in pixels * 8, so divide by 8 to get pixels, then multiply by 8
      // Actually, VGA OFFSET is already in characters (8 pixels), so it's the same
      BX_R128_THIS crtc_pitch = (pitch & 0x3FF); // Mask to 10 bits
    } else if (index == 0x1B) {
      // Extended offset register - update pitch if CRTC 0x13 is already set
      Bit32u pitch = BX_R128_THIS crtc.reg[0x13] & 0xFF;
      pitch |= ((value & 0x1F) << 8);
      BX_R128_THIS crtc_pitch = (pitch & 0x3FF); // Mask to 10 bits
    }
    
    // CRTC 0x0C: START_ADDR_H (display start address high, bits 7:0)
    // CRTC 0x0D: START_ADDR_L (display start address low, bits 7:0)
    if (index == 0x0C || index == 0x0D) {
      // Update CRTC_OFFSET (0x0224)
      // Display start address is in CRTC 0x0C (bits 7:0) + CRTC 0x0D (bits 7:0)
      // ATI Rage 128 PRO CRTC_OFFSET is in bytes
      Bit32u start_addr = (BX_R128_THIS crtc.reg[0x0C] << 8) | BX_R128_THIS crtc.reg[0x0D];
      // VGA start address is in character units (8 pixels), convert to bytes
      // For 8bpp: 1 character = 8 pixels = 8 bytes
      // For 16bpp: 1 character = 8 pixels = 16 bytes
      // For 32bpp: 1 character = 8 pixels = 32 bytes
      // We need to know the current bpp, but for now assume 8bpp
      // Actually, CRTC_OFFSET should be set based on the actual pixel format
      // For now, just store the address - it will be adjusted based on pixel format
      BX_R128_THIS crtc_offset = start_addr * 8; // Assume 8bpp for now
    }
    
    return;
  }
  
  // ATI Rage 128 PRO specific CRTC registers
  // CRT1E (0x1E): Graphics Controller Index Decode Register (read-only)
  if (index == 0x1E) {
    // Read-only register, writes ignored
    return;
  }
  // CRT1F (0x1F): Graphics Controller Index Decode Register (read-only)
  else if (index == 0x1F) {
    // Read-only register, writes ignored
    return;
  }
  // CRT22 (0x22): RAM Data Latch Readback Register (read-only)
  else if (index == 0x22) {
    // Read-only register, writes ignored
    return;
  }
  
  // Shadow CRTC registers (0x40-0x5F) are accessed via bit 6 of CRTC index register
  // Extended registers beyond 0x22 are not documented in ATI Rage 128 PRO datasheet
  if (index > R128_CRTC_MAX) {
    BX_PANIC(("crtc: unknown extended index 0x%02x write", index));
    return;
  }
  
  // For other extended CRTC registers, store the value (for compatibility)
  BX_DEBUG(("crtc: extended index 0x%02x write 0x%02x", index, (unsigned)value));
  BX_R128_THIS crtc.reg[index] = value;
  
  // Note: Hardware cursor is controlled via memory-mapped CRTC cursor registers (0x0260-0x0270)
  // and CRTC_GEN_CNTL register (0x0050), not via extended CRTC registers
  // Shadow CRTC registers (0x40-0x5F) are accessed via bit 6 of CRTC index register (CRTC_EXT_CNTL bit 6)
}

Bit8u bx_r128_c::register_read8(Bit32u address)
{
  Bit8u value;
  
  // PCI configuration space mirror (0x0F00-0x0FFF)
  // According to ATI Rage 128 PRO datasheet, this is a read-only copy of PCI config space
  if (address >= 0x0F00 && address < 0x1000) {
    Bit32u offset = address - 0x0F00;
    if (offset < 256) {
      value = BX_R128_THIS pci_conf[offset];
    } else {
      value = 0x00;
    }
  }
  // ATI Rage 128 PRO register aperture (0x0000-0x1FFF)
  // VGA registers are NOT memory-mapped - they are accessed through standard I/O ports only
  // For other addresses, read from 32-bit register and extract byte
  else {
    Bit32u value32 = register_read32(address & ~3);
    Bit8u shift = (address & 3) * 8;
    value = (value32 >> shift) & 0xFF;
  }
  
  return value;
}

void bx_r128_c::register_write8(Bit32u address, Bit8u value)
{
  // PCI configuration space mirror (read-only, ignore writes)
  // According to ATI Rage 128 PRO datasheet, this is a read-only copy of PCI config space
  if (address >= 0x0F00 && address < 0x1000) {
    // Read-only mirror, ignore writes
    return;
  }
  // ATI Rage 128 PRO register aperture (0x0000-0x1FFF)
  // VGA registers are NOT memory-mapped - they are accessed through standard I/O ports only
  // For other addresses, read-modify-write 32-bit register
  else {
    Bit32u value32 = register_read32(address & ~3);
    Bit8u shift = (address & 3) * 8;
    value32 = (value32 & ~(0xFF << shift)) | (value << shift);
    register_write32(address & ~3, value32);
  }
}

Bit8u bx_r128_c::vram_read8(Bit32u address)
{
  return BX_R128_THIS s.memory[address];
}

Bit16u bx_r128_c::vram_read16(Bit32u address)
{
  return
    BX_R128_THIS s.memory[address + 0] << 0 |
    BX_R128_THIS s.memory[address + 1] << 8;
}

Bit32u bx_r128_c::vram_read32(Bit32u address)
{
  return
    BX_R128_THIS s.memory[address + 0] << 0 |
    BX_R128_THIS s.memory[address + 1] << 8 |
    BX_R128_THIS s.memory[address + 2] << 16 |
    BX_R128_THIS s.memory[address + 3] << 24;
}

void bx_r128_c::vram_write8(Bit32u address, Bit8u value)
{
  BX_R128_THIS s.memory[address + 0] = value;
}

void bx_r128_c::vram_write16(Bit32u address, Bit16u value)
{
  BX_R128_THIS s.memory[address + 0] = value >> 0 & 0xFF;
  BX_R128_THIS s.memory[address + 1] = value >> 8 & 0xFF;
}

void bx_r128_c::vram_write32(Bit32u address, Bit32u value)
{
  BX_R128_THIS s.memory[address + 0] = value >> 0 & 0xFF;
  BX_R128_THIS s.memory[address + 1] = value >> 8 & 0xFF;
  BX_R128_THIS s.memory[address + 2] = value >> 16 & 0xFF;
  BX_R128_THIS s.memory[address + 3] = value >> 24 & 0xFF;
}

// Removed unused functions: vram_write64, ramin_*, physical_*
// These were NVIDIA-specific and not needed for ATI Rage 128 PRO

// NVIDIA-specific command execution and FIFO processing functions removed
// ATI Rage 128 PRO uses register-based GUI engine, not NVIDIA's channel/FIFO architecture


Bit32u bx_r128_c::get_crt_intr()
{
  Bit32u value = 0x00000000;
  if (BX_R128_THIS crt_intr & BX_R128_THIS crt_intr_en)
    value |= 0x00000001;
  return value;
}

void bx_r128_c::set_irq_level(bool level)
{
  DEV_pci_set_irq(BX_R128_THIS devfunc, BX_R128_THIS pci_conf[0x3d], level);
}

Bit32u bx_r128_c::register_read32(Bit32u address)
{
  Bit32u value = 0;

  // ATI Rage 128 register map
  // Configuration registers (0x0000-0x00FF)
  if (address == 0x00E0) {
    value = BX_R128_THIS config_cntl;
  } else if (address == 0x00E4) {
    value = BX_R128_THIS config_xstrap;
  } else if (address == 0x00E8) {
    value = BX_R128_THIS config_bonds;
  } else if (address == 0x00F8) {
    value = BX_R128_THIS config_memsize;
  } else if (address == 0x0100) {
    value = BX_R128_THIS config_aper_0_base;
  } else if (address == 0x0104) {
    value = BX_R128_THIS config_aper_1_base;
  } else if (address == 0x0108) {
    value = BX_R128_THIS config_reg_1_base;
  } else if (address == 0x010C) {
    value = BX_R128_THIS config_reg_aper_size;
  } else if (address == 0x0110) {
    value = BX_R128_THIS config_aper_size;
  }
  // GPIO registers for DDC communication
  else if (address == 0x0060) {
    // GPIO_VGA_DDC: VGA DDC I2C control register
    // Bits typically control SCL/SDA for VGA DDC communication
    // Reading returns current GPIO state with DDC status
    value = BX_R128_THIS gpio_vga_ddc;
    // Update DDC status bits (typically bits 2:3 for SDA/SCL status)
    Bit8u ddc_status = BX_R128_THIS ddc.read();
    value = (value & ~0x0c) | (ddc_status & 0x0c);
  } else if (address == 0x0064) {
    // GPIO_DVI_DDC: DVI DDC I2C control register
    // Bits typically control SCL/SDA for DVI DDC communication
    // Reading returns current GPIO state with DDC status
    value = BX_R128_THIS gpio_dvi_ddc;
    // Update DDC status bits (typically bits 2:3 for SDA/SCL status)
    Bit8u ddc_status = BX_R128_THIS ddc.read();
    value = (value & ~0x0c) | (ddc_status & 0x0c);
  }
  // CRTC registers (ATI Rage 128 PRO addresses)
  // Main CRTC control registers
  else if (address == 0x0050) {
    value = BX_R128_THIS crtc_gen_cntl;
  } else if (address == 0x0054) {
    value = BX_R128_THIS crtc_ext_cntl;
  } else if (address == 0x005C) {
    value = BX_R128_THIS crtc_status;
  }
  // Combined CRTC timing registers (ATI Rage 128 PRO format)
  else if (address == 0x0200) {
    value = BX_R128_THIS crtc_h_total_disp;
  } else if (address == 0x0204) {
    value = BX_R128_THIS crtc_h_sync_strt_wid;
  } else if (address == 0x0208) {
    value = BX_R128_THIS crtc_v_total_disp;
  } else if (address == 0x020C) {
    value = BX_R128_THIS crtc_v_sync_strt_wid;
  }
  // CRTC display control registers
  else if (address == 0x0210) {
    value = BX_R128_THIS crtc_vline_crnt_vline;
  } else if (address == 0x0214) {
    value = BX_R128_THIS crtc_crnt_frame;
  } else if (address == 0x0218) {
    value = BX_R128_THIS crtc_gui_trig_vline;
  } else if (address == 0x021C) {
    value = BX_R128_THIS crtc_debug;
  } else if (address == 0x0224) {
    value = BX_R128_THIS crtc_offset;
  } else if (address == 0x0228) {
    value = BX_R128_THIS crtc_offset_cntl;
  } else if (address == 0x022C) {
    value = BX_R128_THIS crtc_pitch;
  }
  // Hardware cursor registers (ATI Rage 128 PRO addresses)
  else if (address == 0x0260) {
    value = BX_R128_THIS crtc_cur_offset;
  } else if (address == 0x0264) {
    value = BX_R128_THIS crtc_cur_hv_pos;
  } else if (address == 0x0268) {
    value = BX_R128_THIS crtc_cur_hv_offs;
  } else if (address == 0x026C) {
    value = BX_R128_THIS crtc_cur_color0;
  } else if (address == 0x0270) {
    value = BX_R128_THIS crtc_cur_color1;
  }
  // Clock control and PLL (indirect access via CLK_CNTL_INDEX/DATA)
  else if (address == 0x00C0) {
    value = BX_R128_THIS clk_cntl_index;
  } else if (address == 0x00C4) {
    // Read from indexed PLL register
    Bit8u index = BX_R128_THIS clk_cntl_index & 0x3F;
    if (index == 0x00) value = BX_R128_THIS pll_ref_div;
    else if (index == 0x01) value = BX_R128_THIS pll_gen_cntl;
    else if (index == 0x02) value = BX_R128_THIS pll_vclk_cntl;
    else if (index == 0x03) value = BX_R128_THIS pll_vclk_cntl2;
    else if (index == 0x04) value = BX_R128_THIS pll_vclk_cntl3;
    else if (index == 0x05) value = BX_R128_THIS pll_vclk_ecp_cntl;
    else value = 0;
  } else if (address == 0x00C8) {
    value = BX_R128_THIS pll_ref_div;
  } else if (address == 0x00CC) {
    value = BX_R128_THIS pll_gen_cntl;
  } else if (address == 0x00D0) {
    value = BX_R128_THIS pll_vclk_cntl;
  } else if (address == 0x00D4) {
    value = BX_R128_THIS pll_vclk_cntl2;
  } else if (address == 0x00D8) {
    value = BX_R128_THIS pll_vclk_cntl3;
  } else if (address == 0x00DC) {
    value = BX_R128_THIS pll_vclk_ecp_cntl;
  }
  // DAC registers
  else if (address == 0x00F0) {
    value = BX_R128_THIS dac_palette_addr;
  } else if (address == 0x00F4) {
    value = BX_R128_THIS dac_palette_data;
  }
  // PCI configuration space mirror (0x0F00-0x0FFF)
  else if (address >= 0x0F00 && address < 0x1000) {
    Bit32u offset = address - 0x0F00;
    if (offset < 256) {
      value = 
        (BX_R128_THIS pci_conf[offset + 0] << 0) |
        (BX_R128_THIS pci_conf[offset + 1] << 8) |
        (BX_R128_THIS pci_conf[offset + 2] << 16) |
        (BX_R128_THIS pci_conf[offset + 3] << 24);
    } else {
      value = 0;
    }
  }
  // GUI engine registers (0x1400-0x1FFF)
  else if (address == 0x1400) {
    value = BX_R128_THIS gui_eng_cntl;
  } else if (address == 0x1404) {
    // DST_OFFSET (0x1404) - also used as GUI_ENG_STATUS
    value = BX_R128_THIS dst_offset;
  } else if (address == 0x1408) {
    value = BX_R128_THIS dst_pitch;
  } else if (address == 0x140C) {
    value = BX_R128_THIS dst_width;
  } else if (address == 0x1410) {
    value = BX_R128_THIS dst_height;
  } else if (address == 0x1414) {
    value = BX_R128_THIS src_x;
  } else if (address == 0x1418) {
    value = BX_R128_THIS src_y;
  } else if (address == 0x141C) {
    value = BX_R128_THIS dst_x;
  } else if (address == 0x1420) {
    value = BX_R128_THIS dst_y;
  } else if (address == 0x1428) {
    value = BX_R128_THIS src_pitch_offset;
  } else if (address == 0x142C) {
    value = BX_R128_THIS dst_pitch_offset;
  } else if (address == 0x1434) {
    value = BX_R128_THIS src_y_x;
  } else if (address == 0x1438) {
    value = BX_R128_THIS dst_y_x;
  } else if (address == 0x146C) {
    value = BX_R128_THIS dp_gui_master_cntl;
  } else if (address == 0x15AC) {
    value = BX_R128_THIS src_offset;
  } else if (address == 0x15B0) {
    value = BX_R128_THIS src_pitch;
  } else if (address == 0x15D8) {
    value = BX_R128_THIS dp_src_frgd_clr;
  } else if (address == 0x15DC) {
    value = BX_R128_THIS dp_src_bkgd_clr;
  } else if (address == 0x16C0) {
    value = BX_R128_THIS dp_cntl;
  } else if (address == 0x16C4) {
    value = BX_R128_THIS dp_datatype;
  } else if (address == 0x16C8) {
    value = BX_R128_THIS dp_mix;
  } else if (address >= 0x17C0 && address <= 0x17DC) {
    // HOST_DATA[0-7] registers
    Bit32u index = (address - 0x17C0) / 4;
    if (index < 8) {
      value = BX_R128_THIS host_data[index];
    } else {
      value = 0;
    }
  } else if (address == 0x17E0) {
    value = BX_R128_THIS host_data_last;
  }
  // Interrupt registers
  else if (address == 0x00A0) { // CRT_INTR (approximate location)
    value = BX_R128_THIS crt_intr;
  } else if (address == 0x00A4) { // CRT_INTR_EN (approximate location)
    value = BX_R128_THIS crt_intr_en;
  }
  // Unknown/unimplemented registers - use regs array
  else if (address < R128_REG_APER_SIZE) {
    value = BX_R128_THIS regs[address / 4];
  } else {
    value = 0;
  }
  
  return value;
}

void bx_r128_c::register_write32(Bit32u address, Bit32u value)
{
  // ATI Rage 128 register map
  // Configuration registers
  if (address == 0x00E0) {
    BX_R128_THIS config_cntl = value;
  } else if (address == 0x00E4) {
    BX_R128_THIS config_xstrap = value;
  } else if (address == 0x00E8) {
    BX_R128_THIS config_bonds = value;
  } else if (address == 0x00F8) {
    BX_R128_THIS config_memsize = value;
  } else if (address == 0x0100) {
    BX_R128_THIS config_aper_0_base = value;
  } else if (address == 0x0104) {
    BX_R128_THIS config_aper_1_base = value;
  } else if (address == 0x0108) {
    BX_R128_THIS config_reg_1_base = value;
  } else if (address == 0x010C) {
    BX_R128_THIS config_reg_aper_size = value;
  } else if (address == 0x0110) {
    BX_R128_THIS config_aper_size = value;
  }
  // GPIO registers for DDC communication
  else if (address == 0x0060) {
    // GPIO_VGA_DDC: VGA DDC I2C control register
    // Bits typically control SCL/SDA for VGA DDC communication
    // Bit 0: SCL output enable (1=output, 0=input)
    // Bit 1: SDA output enable (1=output, 0=input)
    // Bit 2: SCL data (when output enabled)
    // Bit 3: SDA data (when output enabled)
    // Bits 4+: Other GPIO functions
    BX_R128_THIS gpio_vga_ddc = value;
    // Extract SCL/SDA from GPIO register and write to DDC
    bool scl_en = (value & 0x01) != 0;
    bool sda_en = (value & 0x02) != 0;
    bool scl = (value & 0x04) != 0;
    bool sda = (value & 0x08) != 0;
    // Only write to DDC if pins are configured as outputs
    if (scl_en && sda_en) {
      BX_R128_THIS ddc.write(scl, sda);
    }
  } else if (address == 0x0064) {
    // GPIO_DVI_DDC: DVI DDC I2C control register
    // Bits typically control SCL/SDA for DVI DDC communication
    // Bit 0: SCL output enable (1=output, 0=input)
    // Bit 1: SDA output enable (1=output, 0=input)
    // Bit 2: SCL data (when output enabled)
    // Bit 3: SDA data (when output enabled)
    // Bits 4+: Other GPIO functions
    BX_R128_THIS gpio_dvi_ddc = value;
    // Extract SCL/SDA from GPIO register and write to DDC
    // Note: DVI DDC typically uses the same DDC module as VGA
    bool scl_en = (value & 0x01) != 0;
    bool sda_en = (value & 0x02) != 0;
    bool scl = (value & 0x04) != 0;
    bool sda = (value & 0x08) != 0;
    // Only write to DDC if pins are configured as outputs
    if (scl_en && sda_en) {
      BX_R128_THIS ddc.write(scl, sda);
    }
  }
  // CRTC registers (ATI Rage 128 PRO addresses)
  // Main CRTC control registers
  else if (address == 0x0050) {
    BX_R128_THIS crtc_gen_cntl = value;
    BX_R128_THIS svga_needs_update_mode = 1;
    // Update cursor enable state (bit 16)
    bool prev_enabled = BX_R128_THIS hw_cursor.enabled;
    BX_R128_THIS hw_cursor.enabled = (value & (1 << 16)) != 0;
    if (prev_enabled != BX_R128_THIS hw_cursor.enabled) {
      // Redraw area when cursor enable state changes
      Bit32u cur_horz_pos = (BX_R128_THIS crtc_cur_hv_pos >> 16) & 0x7FF;
      Bit32u cur_vert_pos = BX_R128_THIS crtc_cur_hv_pos & 0x7FF;
      Bit32u cur_horz_off = (BX_R128_THIS crtc_cur_hv_offs >> 16) & 0x3F;
      Bit32u cur_vert_off = BX_R128_THIS crtc_cur_hv_offs & 0x3F;
      Bit16s hwcx = (Bit16s)cur_horz_pos - (Bit16s)cur_horz_off;
      Bit16s hwcy = (Bit16s)cur_vert_pos - (Bit16s)cur_vert_off;
      Bit32u cursor_height = 64 - cur_vert_off;
      BX_R128_THIS redraw_area(hwcx, hwcy, 64, cursor_height);
    }
  } else if (address == 0x0054) {
    BX_R128_THIS crtc_ext_cntl = value;
  } else if (address == 0x005C) {
    BX_R128_THIS crtc_status = value;
  }
  // Combined CRTC timing registers (ATI Rage 128 PRO format)
  else if (address == 0x0200) {
    BX_R128_THIS crtc_h_total_disp = value;
  } else if (address == 0x0204) {
    BX_R128_THIS crtc_h_sync_strt_wid = value;
  } else if (address == 0x0208) {
    BX_R128_THIS crtc_v_total_disp = value;
  } else if (address == 0x020C) {
    BX_R128_THIS crtc_v_sync_strt_wid = value;
  }
  // CRTC display control registers
  else if (address == 0x0210) {
    BX_R128_THIS crtc_vline_crnt_vline = value;
  } else if (address == 0x0214) {
    BX_R128_THIS crtc_crnt_frame = value;
  } else if (address == 0x0218) {
    BX_R128_THIS crtc_gui_trig_vline = value;
  } else if (address == 0x021C) {
    BX_R128_THIS crtc_debug = value;
  } else if (address == 0x0224) {
    BX_R128_THIS crtc_offset = value;
  } else if (address == 0x0228) {
    BX_R128_THIS crtc_offset_cntl = value;
  } else if (address == 0x022C) {
    BX_R128_THIS crtc_pitch = value;
    // CRTC_PITCH: Display line pitch in (pixels * 8), bits 9:0
    BX_R128_THIS svga_pitch = (value & 0x3FF) << 3;
  }
  // Hardware cursor registers (ATI Rage 128 PRO addresses)
  // CUR_OFFSET (0x0260): Hardware cursor address offset in frame buffer
  else if (address == 0x0260) {
    BX_R128_THIS crtc_cur_offset = value;
    // Bits 3:0 are hardwired to ZERO
    BX_R128_THIS crtc_cur_offset &= 0xFFFFFFF0;
  }
  // CUR_HORZ_VERT_POSN (0x0264): Cursor position
  // CUR_HORZ_POSN is bits 26:16, CUR_VERT_POSN is bits 10:0
  else if (address == 0x0264) {
    BX_R128_THIS crtc_cur_hv_pos = value;
    // Update cursor position (accounting for offsets)
    Bit32u cur_horz_pos = (value >> 16) & 0x7FF;
    Bit32u cur_vert_pos = value & 0x7FF;
    Bit32u cur_horz_off = (BX_R128_THIS crtc_cur_hv_offs >> 16) & 0x3F;
    Bit32u cur_vert_off = BX_R128_THIS crtc_cur_hv_offs & 0x3F;
    BX_R128_THIS hw_cursor.x = (Bit16s)cur_horz_pos - (Bit16s)cur_horz_off;
    BX_R128_THIS hw_cursor.y = (Bit16s)cur_vert_pos - (Bit16s)cur_vert_off;
  }
  // CUR_HORZ_VERT_OFF (0x0268): Cursor offsets
  // CUR_HORZ_OFF is bits 21:16, CUR_VERT_OFF is bits 5:0
  else if (address == 0x0268) {
    BX_R128_THIS crtc_cur_hv_offs = value;
    // Update cursor position (accounting for offsets)
    Bit32u cur_horz_pos = (BX_R128_THIS crtc_cur_hv_pos >> 16) & 0x7FF;
    Bit32u cur_vert_pos = BX_R128_THIS crtc_cur_hv_pos & 0x7FF;
    Bit32u cur_horz_off = (value >> 16) & 0x3F;
    Bit32u cur_vert_off = value & 0x3F;
    BX_R128_THIS hw_cursor.x = (Bit16s)cur_horz_pos - (Bit16s)cur_horz_off;
    BX_R128_THIS hw_cursor.y = (Bit16s)cur_vert_pos - (Bit16s)cur_vert_off;
    // Cursor height is (64 - CUR_VERT_OFF)
    BX_R128_THIS hw_cursor.size = 64 - cur_vert_off;
  }
  // CUR_CLR0 (0x026C): Cursor color 0 (24-bit RGB)
  else if (address == 0x026C) {
    BX_R128_THIS crtc_cur_color0 = value;
  }
  // CUR_CLR1 (0x0270): Cursor color 1 (24-bit RGB)
  else if (address == 0x0270) {
    BX_R128_THIS crtc_cur_color1 = value;
  }
  // Clock control and PLL
  else if (address == 0x00C0) {
    BX_R128_THIS clk_cntl_index = value;
  } else if (address == 0x00C4) {
    // Write to indexed PLL register
    Bit8u index = BX_R128_THIS clk_cntl_index & 0x3F;
    if (index == 0x00) BX_R128_THIS pll_ref_div = value;
    else if (index == 0x01) BX_R128_THIS pll_gen_cntl = value;
    else if (index == 0x02) BX_R128_THIS pll_vclk_cntl = value;
    else if (index == 0x03) BX_R128_THIS pll_vclk_cntl2 = value;
    else if (index == 0x04) BX_R128_THIS pll_vclk_cntl3 = value;
    else if (index == 0x05) BX_R128_THIS pll_vclk_ecp_cntl = value;
    BX_R128_THIS calculate_retrace_timing();
  } else if (address == 0x00C8) {
    BX_R128_THIS pll_ref_div = value;
  } else if (address == 0x00CC) {
    BX_R128_THIS pll_gen_cntl = value;
  } else if (address == 0x00D0) {
    BX_R128_THIS pll_vclk_cntl = value;
    BX_R128_THIS calculate_retrace_timing();
  } else if (address == 0x00D4) {
    BX_R128_THIS pll_vclk_cntl2 = value;
  } else if (address == 0x00D8) {
    BX_R128_THIS pll_vclk_cntl3 = value;
  } else if (address == 0x00DC) {
    BX_R128_THIS pll_vclk_ecp_cntl = value;
  }
  // DAC registers
  else if (address == 0x00F0) {
    BX_R128_THIS dac_palette_addr = value;
  } else if (address == 0x00F4) {
    BX_R128_THIS dac_palette_data = value;
  }
  // PCI configuration space mirror (read-only, writes ignored)
  else if (address >= 0x0F00 && address < 0x1000) {
    // Read-only mirror, ignore writes
  }
  // GUI engine registers (0x1400-0x1FFF)
  else if (address == 0x1400) {
    BX_R128_THIS gui_eng_cntl = value;
  } else if (address == 0x1404) {
    // DST_OFFSET (0x1404) - also used as GUI_ENG_STATUS
    BX_R128_THIS dst_offset = value & 0x03FFFFFF; // Bits 25:0, bits 3:0 hardwired to zero
    BX_R128_THIS dst_offset &= 0xFFFFFFF0;
  } else if (address == 0x1408) {
    // DST_PITCH (0x1408)
    BX_R128_THIS dst_pitch = value;
  } else if (address == 0x140C) {
    // DST_WIDTH (0x140C) - also used as GUI_ENG_ACK
    BX_R128_THIS dst_width = value;
  } else if (address == 0x1410) {
    BX_R128_THIS dst_height = value;
  } else if (address == 0x1414) {
    BX_R128_THIS src_x = value & 0x3FFF; // Bits 13:0
  } else if (address == 0x1418) {
    BX_R128_THIS src_y = value & 0x3FFF; // Bits 13:0
  } else if (address == 0x141C) {
    BX_R128_THIS dst_x = value & 0x3FFF; // Bits 13:0
  } else if (address == 0x1420) {
    BX_R128_THIS dst_y = value & 0x3FFF; // Bits 13:0
  } else if (address == 0x1428) {
    BX_R128_THIS src_pitch_offset = value;
  } else if (address == 0x142C) {
    BX_R128_THIS dst_pitch_offset = value;
  } else if (address == 0x1434) {
    BX_R128_THIS src_y_x = value;
  } else if (address == 0x1438) {
    BX_R128_THIS dst_y_x = value;
  } else if (address == 0x146C) {
    BX_R128_THIS dp_gui_master_cntl = value;
    // Writing DP_GUI_MASTER_CNTL triggers BitBlt execution
    // According to ATI Rage 128 PRO datasheet, writing this register initiates the operation
    BX_R128_THIS execute_bitblt();
  } else if (address == 0x15AC) {
    // SRC_OFFSET (0x15AC)
    BX_R128_THIS src_offset = value & 0x03FFFFFF; // Bits 25:0, bits 3:0 hardwired to zero
    BX_R128_THIS src_offset &= 0xFFFFFFF0;
  } else if (address == 0x15B0) {
    BX_R128_THIS src_pitch = value;
  } else if (address == 0x15D8) {
    BX_R128_THIS dp_src_frgd_clr = value;
  } else if (address == 0x15DC) {
    BX_R128_THIS dp_src_bkgd_clr = value;
  } else if (address == 0x16C0) {
    BX_R128_THIS dp_cntl = value;
  } else if (address == 0x16C4) {
    BX_R128_THIS dp_datatype = value;
  } else if (address == 0x16C8) {
    BX_R128_THIS dp_mix = value;
  } else if (address >= 0x17C0 && address <= 0x17DC) {
    // HOST_DATA[0-7] registers
    Bit32u index = (address - 0x17C0) / 4;
    if (index < 8) {
      BX_R128_THIS host_data[index] = value;
    }
  } else if (address == 0x17E0) {
    BX_R128_THIS host_data_last = value;
    // Writing HOST_DATA_LAST triggers host-to-screen BitBlt if GUI engine is enabled
    if (BX_R128_THIS dp_gui_master_cntl & 0x80000000) {
      BX_R128_THIS execute_bitblt();
    }
  }
  // Interrupt registers
  else if (address == 0x00A0) { // CRT_INTR
    BX_R128_THIS crt_intr &= ~value;
    set_irq_level((BX_R128_THIS crt_intr & BX_R128_THIS crt_intr_en) != 0);
  } else if (address == 0x00A4) { // CRT_INTR_EN
    BX_R128_THIS crt_intr_en = value;
    set_irq_level((BX_R128_THIS crt_intr & BX_R128_THIS crt_intr_en) != 0);
  }
  // Unknown/unimplemented registers - store in regs array
  else if (address < R128_REG_APER_SIZE) {
    BX_R128_THIS regs[address / 4] = value;
  }
}

// I/O port mask for ATI Rage 128 (256 bytes, similar to banshee)
static const Bit8u r128_iomask[256] = {4,0,0,0,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
                                       7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1};

void bx_r128_c::svga_init_pcihandlers(void)
{
  BX_R128_THIS devfunc = 0x00;
  DEV_register_pci_handlers2(BX_R128_THIS_PTR,
      &BX_R128_THIS devfunc, BX_PLUGIN_R128, "ATI Rage 128 PRO AGP", true);
  
  // ATI Rage 128 PRO: Vendor 0x1002, Device 0x5046, Subsystem 0x1002:0x0018
  BX_R128_THIS init_pci_conf(0x1002, 0x5046, 0x00, 0x030000, 0x00, BX_PCI_INTA);

  // BAR0: Linear Frame Buffer (LFB) - Memory-mapped VRAM access
  // 32-bit, prefetchable memory space
  // Size is determined by frame buffer size (CONFIG_MEMSIZE)
  // The BAR will be initialized by init_bar_mem, which sets the proper size encoding
  BX_R128_THIS init_bar_mem(0, (64 * 1024 * 1024), r128_mem_read_handler,
                               r128_mem_write_handler);
  // Set prefetchable bit (bit 3) after init_bar_mem
  BX_R128_THIS pci_conf[0x10] |= 0x08;
  
  // BAR1: I/O ports at b800 [size=256] - I/O bar for register access
  BX_R128_THIS pci_conf[0x14] = 0x01; // I/O space
  BX_R128_THIS init_bar_io(1, 256, svga_read_handler, svga_write_handler, r128_iomask);
  
  // BAR2: Memory at ffdfc000 (32-bit, non-prefetchable) [size=16K] - Register map
  BX_R128_THIS pci_conf[0x18] = 0x00; // Memory space, 32-bit, non-prefetchable
  BX_R128_THIS init_bar_mem(2, 0x4000, r128_mem_read_handler,
                                 r128_mem_write_handler);
  
  BX_R128_THIS pci_rom_address = 0;
  BX_R128_THIS pci_rom_read_handler = r128_mem_read_handler;
  BX_R128_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());

  // Subsystem ID: 0x1002:0x0018 (Rage Fury Pro/Xpert 2000 Pro)
  BX_R128_THIS pci_conf[0x2c] = 0x18;
  BX_R128_THIS pci_conf[0x2d] = 0x00;
  BX_R128_THIS pci_conf[0x2e] = 0x02;
  BX_R128_THIS pci_conf[0x2f] = 0x10;

  // Capabilities pointer
  BX_R128_THIS pci_conf[0x34] = 0x50;

  // Status register
  BX_R128_THIS pci_conf[0x06] = 0xB0; // Cap+ 66MHz+ FastB2B+ ParErr- DEVSEL=medium
  BX_R128_THIS pci_conf[0x07] = 0x02; // FastB2B+

  // AGP Capability (offset 0x50)
  BX_R128_THIS pci_conf[0x50] = 0x02; // AGP capability ID
  BX_R128_THIS pci_conf[0x51] = 0x5C; // Next capability pointer
  BX_R128_THIS pci_conf[0x52] = 0x20; // AGP version 2.0
  BX_R128_THIS pci_conf[0x53] = 0x00;
  BX_R128_THIS pci_conf[0x54] = 0x07; // Status: RQ=32 Iso- ArqSz=0 Cal=0 SBA+ ITACoh- GART64- HTrans- 64bit- FW- AGP3- Rate=x1,x2,x4
  BX_R128_THIS pci_conf[0x55] = 0x20;
  BX_R128_THIS pci_conf[0x56] = 0x00; // Command: RQ=1 ArqSz=0 Cal=0 SBA+ AGP- GART64- 64bit- FW- Rate=<none>
  BX_R128_THIS pci_conf[0x57] = 0x01;

  // Power Management Capability (offset 0x5C)
  BX_R128_THIS pci_conf[0x5C] = 0x01; // PM capability ID
  BX_R128_THIS pci_conf[0x5D] = 0x00; // Next capability pointer
  BX_R128_THIS pci_conf[0x5E] = 0x02; // PM version 2
  BX_R128_THIS pci_conf[0x5F] = 0x00;
  BX_R128_THIS pci_conf[0x60] = 0x13; // PM capabilities: PMEClk- DSI- D1+ D2- AuxCurrent=0mA PME(D0-,D1-,D2-,D3hot-,D3cold-)
  BX_R128_THIS pci_conf[0x61] = 0xC8;
  BX_R128_THIS pci_conf[0x62] = 0x08; // PM status: D0 NoSoftRst- PME-Enable- DSel=0 DScale=0 PME-
  BX_R128_THIS pci_conf[0x63] = 0x00;
}

void bx_r128_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;

  if ((address >= 0x1c) && (address < 0x2c))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = pci_conf[address+i];
    switch (address + i) {
      case 0x06:
      case 0x07:
        value8 = oldval;
        break;
    }
    pci_conf[address+i] = value8;
  }
}

#if BX_DEBUGGER
void bx_r128_c::debug_dump(int argc, char **argv)
{
  bx_vgacore_c::debug_dump(argc, argv);
}
#endif

#endif // BX_SUPPORT_R128
