/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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

// NVIDIA RIVA TNT2 M64 (NV6) emulation

#define BX_PLUGGABLE

#include "iodev.h"
#include "vgacore.h"
#include "pci.h"
#define BX_USE_BINARY_FWD_ROP
#define BX_USE_TERNARY_ROP
#include "bitblt.h"
#include "ddc.h"
#include "pxextract.h"
#include "nvriva.h"
#include "virt_timer.h"

#include "bx_debug/debug.h"

#include <math.h>

#if BX_SUPPORT_NVRIVA

#define LOG_THIS BX_NVRIVA_THIS

#if BX_USE_NVRIVA_SMF
#define VGA_READ(addr,len)       bx_vgacore_c::read_handler(theSvga,addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write_handler(theSvga,addr,val,len)
#define SVGA_READ(addr,len)      svga_read_handler(theSvga,addr,len)
#define SVGA_WRITE(addr,val,len) svga_write_handler(theSvga,addr,val,len)
#else
#define VGA_READ(addr,len)       bx_vgacore_c::read(addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write(addr,val,len)
#define SVGA_READ(addr,len)      svga_read(addr,len)
#define SVGA_WRITE(addr,val,len) svga_write(addr,val,len)
#endif // BX_USE_NVRIVA_SMF

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

static bx_nvriva_c *theSvga = NULL;

PLUGIN_ENTRY_FOR_MODULE(nvriva)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_nvriva_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_NVRIVA);
  } else if (mode == PLUGIN_FINI) {
    delete theSvga;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_VGA;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

bx_nvriva_c::bx_nvriva_c() : bx_vgacore_c()
{
  // nothing else to do
}

bx_nvriva_c::~bx_nvriva_c()
{
  SIM->get_bochs_root()->remove("nvriva");
  BX_DEBUG(("Exit"));
}

bool bx_nvriva_c::init_vga_extension(void)
{
  if (!SIM->is_agp_device(BX_PLUGIN_NVRIVA)) {
    BX_PANIC(("RIVA TNT2 M64 should be plugged into AGP slot"));
  }

  BX_NVRIVA_THIS pci_enabled = true;

  BX_NVRIVA_THIS put("NVRIVA");
  // initialize SVGA stuffs.
  BX_NVRIVA_THIS init_iohandlers(svga_read_handler, svga_write_handler, "nvriva");
  DEV_register_ioread_handler(this, svga_read_handler, 0x03B4, "nvriva", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D0, "nvriva", 7);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D2, "nvriva", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03C3, "nvriva", 3);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D0, "nvriva", 7);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D2, "nvriva", 2);
  BX_NVRIVA_THIS svga_init_members();
  BX_NVRIVA_THIS svga_init_pcihandlers();
  BX_NVRIVA_THIS bitblt_init();
  BX_NVRIVA_THIS s.CRTC.max_reg = NVRIVA_CRTC_MAX;
  BX_NVRIVA_THIS s.max_xres = 1600;
  BX_NVRIVA_THIS s.max_yres = 1200;
  BX_INFO(("RIVA TNT2 M64 initialized"));
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("nvriva", this);
#endif
  return 1;
}

#define SETUP_BITBLT(num, name, flags) \
  do { \
    BX_NVRIVA_THIS rop_handler[num] = bitblt_rop_fwd_##name; \
    BX_NVRIVA_THIS rop_flags[num] = flags; \
  } while (0);

void bx_nvriva_c::bitblt_init()
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

void bx_nvriva_c::svga_init_members()
{
  // clear all registers.
  BX_NVRIVA_THIS crtc.index = NVRIVA_CRTC_MAX + 1;
  for (int i = 0; i <= NVRIVA_CRTC_MAX; i++)
    BX_NVRIVA_THIS crtc.reg[i] = 0x00;

  BX_NVRIVA_THIS mc_soft_intr = false;
  BX_NVRIVA_THIS mc_intr_en = 0;
  BX_NVRIVA_THIS mc_enable = 0;
  BX_NVRIVA_THIS bus_intr = 0;
  BX_NVRIVA_THIS bus_intr_en = 0;
  BX_NVRIVA_THIS fifo_wait = false;
  BX_NVRIVA_THIS fifo_wait_soft = false;
  BX_NVRIVA_THIS fifo_wait_notify = false;
  BX_NVRIVA_THIS fifo_wait_flip = false;
  BX_NVRIVA_THIS fifo_wait_acquire = false;
  BX_NVRIVA_THIS fifo_intr = 0;
  BX_NVRIVA_THIS fifo_intr_en = 0;
  BX_NVRIVA_THIS fifo_ramht = 0;
  BX_NVRIVA_THIS fifo_ramfc = 0;
  BX_NVRIVA_THIS fifo_ramro = 0;
  BX_NVRIVA_THIS fifo_mode = 0;
  BX_NVRIVA_THIS fifo_cache1_push0 = 0;
  BX_NVRIVA_THIS fifo_cache1_push1 = 0;
  BX_NVRIVA_THIS fifo_cache1_put = 0;
  BX_NVRIVA_THIS fifo_cache1_dma_push = 0;
  BX_NVRIVA_THIS fifo_cache1_dma_instance = 0;
  BX_NVRIVA_THIS fifo_cache1_dma_put = 0;
  BX_NVRIVA_THIS fifo_cache1_dma_get = 0;
  BX_NVRIVA_THIS fifo_cache1_ref_cnt = 0;
  BX_NVRIVA_THIS fifo_cache1_pull0 = 0;
  BX_NVRIVA_THIS fifo_cache1_get = 0;
  for (int i = 0; i < NVRIVA_CACHE1_SIZE; i++) {
    BX_NVRIVA_THIS fifo_cache1_method[i] = 0;
    BX_NVRIVA_THIS fifo_cache1_data[i] = 0;
  }
  BX_NVRIVA_THIS rma_addr = 0;
  BX_NVRIVA_THIS timer_intr = 0;
  BX_NVRIVA_THIS timer_intr_en = 0;
  BX_NVRIVA_THIS timer_num = 0;
  BX_NVRIVA_THIS timer_den = 0;
  BX_NVRIVA_THIS timer_inittime1 = 0;
  BX_NVRIVA_THIS timer_inittime2 = 0;
  BX_NVRIVA_THIS timer_alarm = 0;
  BX_NVRIVA_THIS graph_intr = 0;
  BX_NVRIVA_THIS graph_nsource = 0;
  BX_NVRIVA_THIS graph_intr_en = 0;
  BX_NVRIVA_THIS graph_ctx_switch1 = 0;
  BX_NVRIVA_THIS graph_ctx_switch2 = 0;
  BX_NVRIVA_THIS graph_ctx_switch4 = 0;
  BX_NVRIVA_THIS graph_status = 0;
  BX_NVRIVA_THIS graph_trapped_addr = 0;
  BX_NVRIVA_THIS graph_trapped_data = 0;
  BX_NVRIVA_THIS graph_flip_read = 0;
  BX_NVRIVA_THIS graph_flip_write = 0;
  BX_NVRIVA_THIS graph_flip_modulo = 0;
  BX_NVRIVA_THIS graph_notify = 0;
  BX_NVRIVA_THIS graph_fifo = 0;
  BX_NVRIVA_THIS graph_bpixel = 0;
  BX_NVRIVA_THIS graph_channel_ctx_table = 0;
  BX_NVRIVA_THIS graph_offset0 = 0;
  BX_NVRIVA_THIS graph_pitch0 = 0;
  BX_NVRIVA_THIS crtc_intr = 0;
  BX_NVRIVA_THIS crtc_intr_en = 0;
  BX_NVRIVA_THIS crtc_start = 0;
  BX_NVRIVA_THIS crtc_config = 0;
  BX_NVRIVA_THIS crtc_raster_pos = 0;
  BX_NVRIVA_THIS crtc_cursor_offset = 0;
  BX_NVRIVA_THIS crtc_cursor_config = 0;
  BX_NVRIVA_THIS crtc_gpio_ext = 0;
  BX_NVRIVA_THIS ramdac_cu_start_pos = 0;
  BX_NVRIVA_THIS ramdac_vpll = 0;
  BX_NVRIVA_THIS ramdac_pll_select = 0;
  BX_NVRIVA_THIS ramdac_general_control = 0;

  memset(BX_NVRIVA_THIS chs, 0x00, sizeof(BX_NVRIVA_THIS chs));
  for (int i = 0; i < NVRIVA_CHANNEL_COUNT; i++) {
    BX_NVRIVA_THIS chs[i].s2d_color_bytes = 1;
  }

  for (int i = 0; i < 4 * 1024 * 1024; i++)
    BX_NVRIVA_THIS unk_regs[i] = 0;

  BX_NVRIVA_THIS svga_unlock_special = 0;
  BX_NVRIVA_THIS svga_needs_update_tile = 1;
  BX_NVRIVA_THIS svga_needs_update_dispentire = 1;
  BX_NVRIVA_THIS svga_needs_update_mode = 0;
  BX_NVRIVA_THIS svga_double_width = 0;

  BX_NVRIVA_THIS svga_xres = 640;
  BX_NVRIVA_THIS svga_yres = 480;
  BX_NVRIVA_THIS svga_bpp = 8;
  BX_NVRIVA_THIS svga_pitch = 640;
  BX_NVRIVA_THIS bank_base[0] = 0;
  BX_NVRIVA_THIS bank_base[1] = 0;

  BX_NVRIVA_THIS hw_cursor.x = 0;
  BX_NVRIVA_THIS hw_cursor.y = 0;
  BX_NVRIVA_THIS hw_cursor.size = 32;
  BX_NVRIVA_THIS hw_cursor.offset = 0;
  BX_NVRIVA_THIS hw_cursor.bpp32 = false;
  BX_NVRIVA_THIS hw_cursor.enabled = false;

  // NV5 RIVA TNT2 M64: 32MB VRAM
  BX_NVRIVA_THIS s.memsize = 32 * 1024 * 1024;
  // Straps: bit 6 set = 14.318 MHz crystal
  BX_NVRIVA_THIS straps0_primary_original = 0x00000040;
  BX_NVRIVA_THIS straps0_primary = BX_NVRIVA_THIS straps0_primary_original;
  BX_NVRIVA_THIS memsize_mask = BX_NVRIVA_THIS s.memsize - 1;
  BX_NVRIVA_THIS ramin_flip = BX_NVRIVA_THIS s.memsize - 64;

  // memory allocation.
  if (BX_NVRIVA_THIS s.memory == NULL)
    BX_NVRIVA_THIS s.memory = new Bit8u[BX_NVRIVA_THIS s.memsize];
  memset(BX_NVRIVA_THIS s.memory, 0x00, BX_NVRIVA_THIS s.memsize);
  BX_NVRIVA_THIS disp_ptr = BX_NVRIVA_THIS s.memory;
  BX_NVRIVA_THIS disp_offset = 0;
  BX_NVRIVA_THIS disp_end_offset = 0;

  // VCLK defaults after reset
  BX_NVRIVA_THIS s.vclk[0] = 25180000;
  BX_NVRIVA_THIS s.vclk[1] = 28325000;
  BX_NVRIVA_THIS s.vclk[2] = 41165000;
  BX_NVRIVA_THIS s.vclk[3] = 36082000;
}

void bx_nvriva_c::reset(unsigned type)
{
  // reset VGA stuffs.
  BX_NVRIVA_THIS bx_vgacore_c::reset(type);
  // reset SVGA stuffs.
  BX_NVRIVA_THIS svga_init_members();
  BX_NVRIVA_THIS ddc.init();
  // Disable ROM shadowing to allow clearing of VRAM
  BX_NVRIVA_THIS pci_conf[0x50] = 0x00;
}

void bx_nvriva_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "nvriva", "RIVA TNT2 M64 State");
  BX_NVRIVA_THIS vgacore_register_state(list);
  bx_list_c *crtc = new bx_list_c(list, "crtc");
  new bx_shadow_num_c(crtc, "index", &BX_NVRIVA_THIS crtc.index, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_NVRIVA_THIS crtc.reg, NVRIVA_CRTC_MAX + 1, 1);
  BXRS_PARAM_BOOL(list, svga_unlock_special, BX_NVRIVA_THIS svga_unlock_special);
  BXRS_PARAM_BOOL(list, svga_double_width, BX_NVRIVA_THIS svga_double_width);
  new bx_shadow_num_c(list, "svga_xres", &BX_NVRIVA_THIS svga_xres);
  new bx_shadow_num_c(list, "svga_yres", &BX_NVRIVA_THIS svga_yres);
  new bx_shadow_num_c(list, "svga_pitch", &BX_NVRIVA_THIS svga_pitch);
  new bx_shadow_num_c(list, "svga_bpp", &BX_NVRIVA_THIS svga_bpp);
  new bx_shadow_num_c(list, "svga_dispbpp", &BX_NVRIVA_THIS svga_dispbpp);
  new bx_shadow_num_c(list, "bank_base0", &BX_NVRIVA_THIS bank_base[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_base1", &BX_NVRIVA_THIS bank_base[1], BASE_HEX);
  bx_list_c *cursor = new bx_list_c(list, "hw_cursor");
  BXRS_PARAM_BOOL(cursor, enabled, BX_NVRIVA_THIS hw_cursor.enabled);
  BXRS_PARAM_BOOL(cursor, bpp32, BX_NVRIVA_THIS hw_cursor.bpp32);
  BXRS_PARAM_BOOL(cursor, vram, BX_NVRIVA_THIS hw_cursor.vram);
  new bx_shadow_num_c(cursor, "x", &BX_NVRIVA_THIS hw_cursor.x, BASE_HEX);
  new bx_shadow_num_c(cursor, "y", &BX_NVRIVA_THIS hw_cursor.y, BASE_HEX);
  new bx_shadow_num_c(cursor, "size", &BX_NVRIVA_THIS hw_cursor.size, BASE_HEX);
  new bx_shadow_num_c(cursor, "offset", &BX_NVRIVA_THIS hw_cursor.offset, BASE_HEX);
  BXRS_PARAM_BOOL(list, mc_soft_intr, BX_NVRIVA_THIS mc_soft_intr);
  new bx_shadow_num_c(list, "mc_intr_en", &BX_NVRIVA_THIS mc_intr_en, BASE_HEX);
  new bx_shadow_num_c(list, "mc_enable", &BX_NVRIVA_THIS mc_enable, BASE_HEX);
  new bx_shadow_num_c(list, "bus_intr", &BX_NVRIVA_THIS bus_intr, BASE_HEX);
  new bx_shadow_num_c(list, "bus_intr_en", &BX_NVRIVA_THIS bus_intr_en, BASE_HEX);
  BXRS_PARAM_BOOL(list, fifo_wait, BX_NVRIVA_THIS fifo_wait);
  BXRS_PARAM_BOOL(list, fifo_wait_soft, BX_NVRIVA_THIS fifo_wait_soft);
  BXRS_PARAM_BOOL(list, fifo_wait_notify, BX_NVRIVA_THIS fifo_wait_notify);
  BXRS_PARAM_BOOL(list, fifo_wait_flip, BX_NVRIVA_THIS fifo_wait_flip);
  BXRS_PARAM_BOOL(list, fifo_wait_acquire, BX_NVRIVA_THIS fifo_wait_acquire);
  new bx_shadow_num_c(list, "fifo_intr", &BX_NVRIVA_THIS fifo_intr, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_intr_en", &BX_NVRIVA_THIS fifo_intr_en, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_ramht", &BX_NVRIVA_THIS fifo_ramht, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_ramfc", &BX_NVRIVA_THIS fifo_ramfc, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_ramro", &BX_NVRIVA_THIS fifo_ramro, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_mode", &BX_NVRIVA_THIS fifo_mode, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_push0", &BX_NVRIVA_THIS fifo_cache1_push0, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_push1", &BX_NVRIVA_THIS fifo_cache1_push1, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_put", &BX_NVRIVA_THIS fifo_cache1_put, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_dma_push", &BX_NVRIVA_THIS fifo_cache1_dma_push, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_dma_instance", &BX_NVRIVA_THIS fifo_cache1_dma_instance, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_dma_put", &BX_NVRIVA_THIS fifo_cache1_dma_put, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_dma_get", &BX_NVRIVA_THIS fifo_cache1_dma_get, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_ref_cnt", &BX_NVRIVA_THIS fifo_cache1_ref_cnt, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_pull0", &BX_NVRIVA_THIS fifo_cache1_pull0, BASE_HEX);
  new bx_shadow_num_c(list, "fifo_cache1_get", &BX_NVRIVA_THIS fifo_cache1_get, BASE_HEX);
  new bx_shadow_data_c(list, "fifo_cache1_method", (Bit8u*)BX_NVRIVA_THIS fifo_cache1_method, NVRIVA_CACHE1_SIZE << 2, 1);
  new bx_shadow_data_c(list, "fifo_cache1_data", (Bit8u*)BX_NVRIVA_THIS fifo_cache1_data, NVRIVA_CACHE1_SIZE << 2, 1);
  new bx_shadow_num_c(list, "rma_addr", &BX_NVRIVA_THIS rma_addr, BASE_HEX);
  new bx_shadow_num_c(list, "timer_intr", &BX_NVRIVA_THIS timer_intr, BASE_HEX);
  new bx_shadow_num_c(list, "timer_intr_en", &BX_NVRIVA_THIS timer_intr_en, BASE_HEX);
  new bx_shadow_num_c(list, "timer_num", &BX_NVRIVA_THIS timer_num, BASE_HEX);
  new bx_shadow_num_c(list, "timer_den", &BX_NVRIVA_THIS timer_den, BASE_HEX);
  new bx_shadow_num_c(list, "timer_inittime1", &BX_NVRIVA_THIS timer_inittime1, BASE_HEX);
  new bx_shadow_num_c(list, "timer_inittime2", &BX_NVRIVA_THIS timer_inittime2, BASE_HEX);
  new bx_shadow_num_c(list, "timer_alarm", &BX_NVRIVA_THIS timer_alarm, BASE_HEX);
  new bx_shadow_num_c(list, "straps0_primary", &BX_NVRIVA_THIS straps0_primary, BASE_HEX);
  new bx_shadow_num_c(list, "straps0_primary_original", &BX_NVRIVA_THIS straps0_primary_original, BASE_HEX);
  new bx_shadow_num_c(list, "graph_intr", &BX_NVRIVA_THIS graph_intr, BASE_HEX);
  new bx_shadow_num_c(list, "graph_nsource", &BX_NVRIVA_THIS graph_nsource, BASE_HEX);
  new bx_shadow_num_c(list, "graph_intr_en", &BX_NVRIVA_THIS graph_intr_en, BASE_HEX);
  new bx_shadow_num_c(list, "graph_ctx_switch1", &BX_NVRIVA_THIS graph_ctx_switch1, BASE_HEX);
  new bx_shadow_num_c(list, "graph_ctx_switch2", &BX_NVRIVA_THIS graph_ctx_switch2, BASE_HEX);
  new bx_shadow_num_c(list, "graph_ctx_switch4", &BX_NVRIVA_THIS graph_ctx_switch4, BASE_HEX);
  new bx_shadow_num_c(list, "graph_status", &BX_NVRIVA_THIS graph_status, BASE_HEX);
  new bx_shadow_num_c(list, "graph_trapped_addr", &BX_NVRIVA_THIS graph_trapped_addr, BASE_HEX);
  new bx_shadow_num_c(list, "graph_trapped_data", &BX_NVRIVA_THIS graph_trapped_data, BASE_HEX);
  new bx_shadow_num_c(list, "graph_flip_read", &BX_NVRIVA_THIS graph_flip_read, BASE_HEX);
  new bx_shadow_num_c(list, "graph_flip_write", &BX_NVRIVA_THIS graph_flip_write, BASE_HEX);
  new bx_shadow_num_c(list, "graph_flip_modulo", &BX_NVRIVA_THIS graph_flip_modulo, BASE_HEX);
  new bx_shadow_num_c(list, "graph_notify", &BX_NVRIVA_THIS graph_notify, BASE_HEX);
  new bx_shadow_num_c(list, "graph_fifo", &BX_NVRIVA_THIS graph_fifo, BASE_HEX);
  new bx_shadow_num_c(list, "graph_bpixel", &BX_NVRIVA_THIS graph_bpixel, BASE_HEX);
  new bx_shadow_num_c(list, "graph_channel_ctx_table", &BX_NVRIVA_THIS graph_channel_ctx_table, BASE_HEX);
  new bx_shadow_num_c(list, "graph_offset0", &BX_NVRIVA_THIS graph_offset0, BASE_HEX);
  new bx_shadow_num_c(list, "graph_pitch0", &BX_NVRIVA_THIS graph_pitch0, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_intr", &BX_NVRIVA_THIS crtc_intr, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_intr_en", &BX_NVRIVA_THIS crtc_intr_en, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_start", &BX_NVRIVA_THIS crtc_start, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_config", &BX_NVRIVA_THIS crtc_config, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_raster_pos", &BX_NVRIVA_THIS crtc_raster_pos, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_cursor_offset", &BX_NVRIVA_THIS crtc_cursor_offset, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_cursor_config", &BX_NVRIVA_THIS crtc_cursor_config, BASE_HEX);
  new bx_shadow_num_c(list, "crtc_gpio_ext", &BX_NVRIVA_THIS crtc_gpio_ext, BASE_HEX);
  new bx_shadow_num_c(list, "ramdac_cu_start_pos", &BX_NVRIVA_THIS ramdac_cu_start_pos, BASE_HEX);
  new bx_shadow_num_c(list, "ramdac_vpll", &BX_NVRIVA_THIS ramdac_vpll, BASE_HEX);
  new bx_shadow_num_c(list, "ramdac_pll_select", &BX_NVRIVA_THIS ramdac_pll_select, BASE_HEX);
  new bx_shadow_num_c(list, "ramdac_general_control", &BX_NVRIVA_THIS ramdac_general_control, BASE_HEX);
  new bx_shadow_data_c(list, "chs", (Bit8u*)BX_NVRIVA_THIS chs, sizeof(chs));
  register_pci_state(list);
}

void bx_nvriva_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state();
  if (BX_NVRIVA_THIS crtc.reg[0x28] == 0x00) {
    BX_NVRIVA_THIS bx_vgacore_c::after_restore_state();
  } else {
    BX_NVRIVA_THIS svga_needs_update_mode = 1;
    BX_NVRIVA_THIS update();
  }
}

void bx_nvriva_c::redraw_area(unsigned x0, unsigned y0,
                               unsigned width, unsigned height)
{
  redraw_area_d((Bit32s)x0, (Bit32s)y0, width, height);
}

void bx_nvriva_c::redraw_area_nd(Bit32u offset, Bit32u width, Bit32u height)
{
  if (BX_NVRIVA_THIS svga_pitch != 0) {
    Bit32u redraw_x = offset % BX_NVRIVA_THIS svga_pitch / (BX_NVRIVA_THIS svga_bpp >> 3);
    Bit32u redraw_y = offset / BX_NVRIVA_THIS svga_pitch;
    BX_NVRIVA_THIS redraw_area_nd(redraw_x, redraw_y, width, height);
  }
}

void bx_nvriva_c::redraw_area_nd(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  if (s.y_doublescan) {
    y0 <<= 1;
    height <<= 1;
  }
  if (BX_NVRIVA_THIS svga_double_width) {
    x0 <<= 1;
    width <<= 1;
  }
  BX_NVRIVA_THIS redraw_area_d(x0, y0, width, height);
}

void bx_nvriva_c::redraw_area_d(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  if (x0 + (Bit32s)width <= 0 || y0 + (Bit32s)height <= 0)
    return;

  if (!BX_NVRIVA_THIS crtc.reg[0x28]) {
    BX_NVRIVA_THIS bx_vgacore_c::redraw_area(x0, y0, width, height);
    return;
  }

  if (BX_NVRIVA_THIS svga_needs_update_mode)
    return;

  BX_NVRIVA_THIS svga_needs_update_tile = 1;

  unsigned xti, yti, xt0, xt1, yt0, yt1;
  xt0 = x0 <= 0 ? 0 : x0 / X_TILESIZE;
  yt0 = y0 <= 0 ? 0 : y0 / Y_TILESIZE;
  if (x0 < (Bit32s)BX_NVRIVA_THIS svga_xres) {
    xt1 = (x0 + width - 1) / X_TILESIZE;
  } else {
    xt1 = (BX_NVRIVA_THIS svga_xres - 1) / X_TILESIZE;
  }
  if (y0 < (Bit32s)BX_NVRIVA_THIS svga_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (BX_NVRIVA_THIS svga_yres - 1) / Y_TILESIZE;
  }
  if ((x0 + width) > BX_NVRIVA_THIS svga_xres) {
    BX_NVRIVA_THIS redraw_area_d(0, y0 + 1, x0 + width - BX_NVRIVA_THIS svga_xres, height);
  }
  for (yti=yt0; yti<=yt1; yti++) {
    for (xti=xt0; xti<=xt1; xti++) {
      SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 1);
    }
  }
}

void bx_nvriva_c::vertical_timer()
{
  bx_vgacore_c::vertical_timer();
  if (BX_NVRIVA_THIS vtimer_toggle) {
    BX_NVRIVA_THIS crtc_intr |= 0x00000001;
    BX_DEBUG(("vertical_timer, crtc_intr_en = %d, mc_intr_en = %d",
      BX_NVRIVA_THIS crtc_intr_en, BX_NVRIVA_THIS mc_intr_en));
    update_irq_level();
  }
  if (BX_NVRIVA_THIS fifo_wait_acquire) {
    BX_NVRIVA_THIS fifo_wait_acquire = false;
    update_fifo_wait();
    fifo_process();
  }
}

// Memory handlers

bool bx_nvriva_c::nvriva_mem_read_handler(bx_phy_address addr, unsigned len,
                                            void *data, void *param)
{
  // BAR0: MMIO
  if (addr >= BX_NVRIVA_THIS pci_bar[0].addr &&
      addr < (BX_NVRIVA_THIS pci_bar[0].addr + NVRIVA_PNPMMIO_SIZE)) {
    Bit32u offset = addr & (NVRIVA_PNPMMIO_SIZE - 1);
    if (len == 1) {
      *(Bit8u*)data = register_read8(offset);
      BX_DEBUG(("MMIO read from 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
    } else if (len == 2) {
      Bit32u aligned = offset & ~3;
      Bit32u shift = (offset & 3) * 8;
      Bit16u value = (Bit16u)(register_read32(aligned) >> shift);
      BX_DEBUG(("MMIO read from 0x%08x, value 0x%04x", offset, value));
      *((Bit16u*)data) = value;
    } else if (len == 4) {
      Bit32u value = register_read32(offset);
      BX_DEBUG(("MMIO read from 0x%08x, value 0x%08x", offset, value));
      *((Bit32u*)data) = value;
    } else {
      BX_PANIC(("MMIO read len %d", len));
    }
    return true;
  }

  // No BAR2 on NV5 - RAMIN is accessed through BAR0 at 0x700000

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = BX_NVRIVA_THIS mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return true;
}

Bit8u bx_nvriva_c::mem_read(bx_phy_address addr)
{
  if (BX_NVRIVA_THIS pci_bar[PCI_ROM_BAR].size > 0) {
    Bit32u mask = (BX_NVRIVA_THIS pci_bar[PCI_ROM_BAR].size - 1);
    if (((Bit32u)addr & ~mask) == BX_NVRIVA_THIS pci_bar[PCI_ROM_BAR].addr) {
      if (BX_NVRIVA_THIS pci_conf[0x30] & 0x01) {
        if (BX_NVRIVA_THIS pci_conf[0x50] == 0x00)
          return BX_NVRIVA_THIS pci_rom[addr & mask];
        else
          return BX_NVRIVA_THIS s.memory[addr & mask];
      } else {
        return 0xff;
      }
    }
  }

  if ((addr >= BX_NVRIVA_THIS pci_bar[1].addr) &&
      (addr < (BX_NVRIVA_THIS pci_bar[1].addr + BX_NVRIVA_THIS s.memsize))) {
    Bit32u offset = addr & BX_NVRIVA_THIS memsize_mask;
    return BX_NVRIVA_THIS s.memory[offset];
  }

  if (!BX_NVRIVA_THIS crtc.reg[0x28])
    return BX_NVRIVA_THIS bx_vgacore_c::mem_read(addr);

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset = addr & 0xffff;
    offset += BX_NVRIVA_THIS bank_base[0];
    offset &= BX_NVRIVA_THIS memsize_mask;
    return BX_NVRIVA_THIS s.memory[offset];
  }

  return 0xff;
}

bool bx_nvriva_c::nvriva_mem_write_handler(bx_phy_address addr, unsigned len,
                                             void *data, void *param)
{
  // BAR0: MMIO
  if (addr >= BX_NVRIVA_THIS pci_bar[0].addr &&
      addr < (BX_NVRIVA_THIS pci_bar[0].addr + NVRIVA_PNPMMIO_SIZE)) {
    Bit32u offset = addr & (NVRIVA_PNPMMIO_SIZE - 1);
    if (len == 1) {
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
      register_write8(offset, *(Bit8u*)data);
    } else if (len == 2) {
      Bit16u value = *((Bit16u*)data);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%04x", offset, value));
      Bit32u value32 = register_read32(offset & ~3);
      if (offset & 2)
        value32 = (value32 & 0x0000FFFF) | ((Bit32u)value << 16);
      else
        value32 = (value32 & 0xFFFF0000) | value;
      register_write32(offset & ~3, value32);
    } else if (len == 4) {
      Bit32u value = *((Bit32u*)data);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%08x", offset, value));
      register_write32(offset, value);
    } else if (len == 8) {
      Bit64u value = *((Bit64u*)data);
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%016" FMT_64 "x", offset, value));
      register_write32(offset, (Bit32u)value);
      register_write32(offset + 4, value >> 32);
    } else {
      BX_PANIC(("MMIO write len %d", len));
    }
    return true;
  }

  // No BAR2 on NV5

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    BX_NVRIVA_THIS mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return true;
}

void bx_nvriva_c::mem_write(bx_phy_address addr, Bit8u value)
{
  if ((addr >= BX_NVRIVA_THIS pci_bar[1].addr) &&
      (addr < (BX_NVRIVA_THIS pci_bar[1].addr + BX_NVRIVA_THIS s.memsize))) {
    Bit32u offset = addr & BX_NVRIVA_THIS memsize_mask;
    BX_NVRIVA_THIS s.memory[offset] = value;
    if (offset < BX_NVRIVA_THIS disp_end_offset &&
        offset >= BX_NVRIVA_THIS disp_offset) {
      BX_NVRIVA_THIS svga_needs_update_tile = 1;
      offset -= BX_NVRIVA_THIS disp_offset;
      Bit32u x = (offset % BX_NVRIVA_THIS svga_pitch) / (BX_NVRIVA_THIS svga_bpp / 8);
      Bit32u y = offset / BX_NVRIVA_THIS svga_pitch;
      if (BX_NVRIVA_THIS s.y_doublescan)
        y <<= 1;
      if (BX_NVRIVA_THIS svga_double_width)
        x <<= 1;
      SET_TILE_UPDATED(BX_NVRIVA_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
    }
    return;
  }

  if (!BX_NVRIVA_THIS crtc.reg[0x28]) {
    BX_NVRIVA_THIS bx_vgacore_c::mem_write(addr, value);
    return;
  }

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset = addr & 0xffff;
    if (BX_NVRIVA_THIS crtc.reg[0x1c] & 0x80) {
      // NV4/NV5: direct RAMIN access via A0000 window
      BX_NVRIVA_THIS s.memory[offset] = value;
      return;
    }
    offset += BX_NVRIVA_THIS bank_base[0];
    offset &= BX_NVRIVA_THIS memsize_mask;
    BX_NVRIVA_THIS s.memory[offset] = value;
    BX_NVRIVA_THIS svga_needs_update_tile = 1;
    unsigned x = (offset % BX_NVRIVA_THIS svga_pitch) / (BX_NVRIVA_THIS svga_bpp / 8);
    unsigned y = offset / BX_NVRIVA_THIS svga_pitch;
    if (BX_NVRIVA_THIS s.y_doublescan)
      y <<= 1;
    if (BX_NVRIVA_THIS svga_double_width)
      x <<= 1;
    SET_TILE_UPDATED(BX_NVRIVA_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
  }
}

void bx_nvriva_c::get_text_snapshot(Bit8u **text_snapshot,
                                    unsigned *txHeight, unsigned *txWidth)
{
  BX_NVRIVA_THIS bx_vgacore_c::get_text_snapshot(text_snapshot,txHeight,txWidth);
}

// VGA I/O handlers

Bit32u bx_nvriva_c::svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_NVRIVA_SMF
  bx_nvriva_c *class_ptr = (bx_nvriva_c *) this_ptr;

  return class_ptr->svga_read(address, io_len);
}

Bit32u bx_nvriva_c::svga_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_NVRIVA_SMF

  if (address == 0x03C3 && io_len == 2)
    return VGA_READ(address, 1) | VGA_READ(address + 1, 1) << 8;

  if (address == 0x03d0 || address == 0x03d2) // RMA_ACCESS
  {
      if (io_len == 1) {
        BX_ERROR(("port 0x3d0 access with io_len = 1"));
        return 0;
      }
      Bit8u crtc38 = BX_NVRIVA_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable) {
        BX_ERROR(("port 0x3d0 access is disabled"));
        return 0;
      }
      int rma_index = (crtc38 >> 1) & 7;
      if (rma_index == 0) {
        // SIG: signature register, always reads 0x2b16d065
        if (address == 0x03d0)
          return 0x2b16d065;
        else
          return 0x2b16d065 >> 16;
      }
      else if (rma_index == 1) {
        if (address == 0x03d0)
          return BX_NVRIVA_THIS rma_addr;
        else
          return BX_NVRIVA_THIS rma_addr >> 16;
      }
      else if (rma_index == 2 || rma_index == 3) {
        // DATA (index 2) and DATA_PARTIAL (index 3) both read from rma_addr
        bool vram = false;
        Bit32u offset = BX_NVRIVA_THIS rma_addr;
        if (BX_NVRIVA_THIS rma_addr & 0x80000000) {
          vram = true;
          offset &= ~0x80000000;
        }
        if ((!vram && offset < NVRIVA_PNPMMIO_SIZE) ||
             (vram && offset < BX_NVRIVA_THIS s.memsize)) {
          Bit32u value = vram ? vram_read32(offset) : register_read32(offset);
          if (address == 0x03d0) {
            if (io_len == 2)
              BX_DEBUG(("rma: read from 0x%08x value 0x????%04x", BX_NVRIVA_THIS rma_addr, value & 0xFFFF));
            else
              BX_DEBUG(("rma: read from 0x%08x value 0x%08x", BX_NVRIVA_THIS rma_addr, value));
            return value;
          } else {
            BX_DEBUG(("rma: read from 0x%08x value 0x%04x????", BX_NVRIVA_THIS rma_addr, value >> 16));
            return value >> 16;
          }
        } else {
          BX_ERROR(("rma: oob read from 0x%08x ignored", BX_NVRIVA_THIS rma_addr));
          return 0xFFFFFFFF;
        }
      }
      else
        BX_DEBUG(("rma: read unknown index %d", rma_index));
      return 0;
  }

  if ((io_len == 2) && ((address & 1) == 0)) {
    Bit32u value  = (Bit32u)SVGA_READ(address,1);
           value |= (Bit32u)SVGA_READ(address+1,1) << 8;
    return value;
  }

  if (io_len != 1) {
    BX_PANIC(("SVGA read: io_len != 1"));
  }

  switch (address) {
    case 0x03b4: /* VGA: CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* VGA: CRTC Index Register (color emulation modes) */
      return BX_NVRIVA_THIS crtc.index;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_NVRIVA_THIS crtc.index > VGA_CRTC_MAX)
        return BX_NVRIVA_THIS svga_read_crtc(address,BX_NVRIVA_THIS crtc.index);
      else
        break;
    case 0x03c2: /* Input Status 0 */
      BX_DEBUG(("Input Status 0 read"));
      return 0x10; // Monitor presence detection (DAC Sensing)
    default:
      break;
  }

  return VGA_READ(address,io_len);
}

void bx_nvriva_c::svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_NVRIVA_SMF
  bx_nvriva_c *class_ptr = (bx_nvriva_c *) this_ptr;
  class_ptr->svga_write(address, value, io_len);
}

void bx_nvriva_c::svga_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_NVRIVA_SMF

  if (address == 0x03d0 || address == 0x03d2) // RMA_ACCESS
  {
      if (io_len == 1) {
        BX_DEBUG(("port 0x3d0 access with io_len = 1"));
        return;
      }
      Bit8u crtc38 = BX_NVRIVA_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable) {
        BX_ERROR(("port 0x3d0 access is disabled"));
        return;
      }
      int rma_index = (crtc38 >> 1) & 7;
      if (rma_index == 0) {
        // SIG: read-only signature, writes ignored
      } else if (rma_index == 1) {
        if (address == 0x03d0) {
          if (io_len == 2) {
            BX_NVRIVA_THIS rma_addr &= 0xFFFF0000;
            BX_NVRIVA_THIS rma_addr |= value;
          } else {
            BX_NVRIVA_THIS rma_addr = value;
          }
        } else {
          BX_NVRIVA_THIS rma_addr &= 0x0000FFFF;
          BX_NVRIVA_THIS rma_addr |= value << 16;
        }
      } else if (rma_index == 2 || rma_index == 3) {
        // DATA (index 2) and DATA_PARTIAL (index 3) both write to rma_addr
        bool vram = false;
        Bit32u offset = BX_NVRIVA_THIS rma_addr & ~3;
        if (BX_NVRIVA_THIS rma_addr & 0x80000000) {
          vram = true;
          offset &= ~0x80000000;
        }
        if ((!vram && offset < NVRIVA_PNPMMIO_SIZE) ||
             (vram && offset < BX_NVRIVA_THIS s.memsize)) {
          if (address == 0x03d0) {
            if (io_len == 2) {
              Bit32u value32 = vram ? vram_read32(offset) : register_read32(offset);
              value32 &= 0xFFFF0000;
              value32 |= value;
              if (vram)
                vram_write32(offset, value32);
              else
                register_write32(offset, value32);
              BX_DEBUG(("rma: write to 0x%08x value 0x????%04x", BX_NVRIVA_THIS rma_addr, value));
            } else {
              if (vram)
                vram_write32(offset, value);
              else
                register_write32(offset, value);
              BX_DEBUG(("rma: write to 0x%08x value 0x%08x", BX_NVRIVA_THIS rma_addr, value));
            }
          } else {
            Bit32u value32 = vram ? vram_read32(offset) : register_read32(offset);
            value32 &= 0x0000FFFF;
            value32 |= value << 16;
            if (vram)
              vram_write32(offset, value32);
            else
              register_write32(offset, value32);
            BX_DEBUG(("rma: write to 0x%08x value 0x%04x????", BX_NVRIVA_THIS rma_addr, value));
          }
        } else {
          BX_ERROR(("rma: oob write to 0x%08x ignored", BX_NVRIVA_THIS rma_addr));
        }
      } else {
        BX_DEBUG(("rma: write unknown index %d", rma_index));
      }
      return;
  }

  if ((io_len == 2) && ((address & 1) == 0)) {
    SVGA_WRITE(address,value & 0xff,1);
    SVGA_WRITE(address+1,value >> 8,1);
    return;
  }

  if (io_len != 1) {
    BX_PANIC(("SVGA write: io_len != 1"));
  }

  switch (address) {
    case 0x03b4: /* VGA: CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* VGA: CRTC Index Register (color emulation modes) */
      BX_NVRIVA_THIS crtc.index = value;
      break;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_NVRIVA_THIS crtc.index == 0x01 ||
          BX_NVRIVA_THIS crtc.index == 0x07 ||
          BX_NVRIVA_THIS crtc.index == 0x09 ||
          BX_NVRIVA_THIS crtc.index == 0x0c ||
          BX_NVRIVA_THIS crtc.index == 0x0d ||
          BX_NVRIVA_THIS crtc.index == 0x12 ||
          BX_NVRIVA_THIS crtc.index == 0x13 ||
          BX_NVRIVA_THIS crtc.index == 0x15 ||
          BX_NVRIVA_THIS crtc.index == 0x19 ||
          BX_NVRIVA_THIS crtc.index == 0x25 ||
          BX_NVRIVA_THIS crtc.index == 0x28 ||
          BX_NVRIVA_THIS crtc.index == 0x2D) {
        // No CR41/CR42 on NV4/NV5
        BX_NVRIVA_THIS svga_needs_update_mode = 1;
      }
      if (BX_NVRIVA_THIS crtc.index <= VGA_CRTC_MAX) {
        BX_NVRIVA_THIS crtc.reg[BX_NVRIVA_THIS crtc.index] = value;
      } else {
        BX_NVRIVA_THIS svga_write_crtc(address, BX_NVRIVA_THIS crtc.index, value);
        if (BX_NVRIVA_THIS crtc.index == 0x25 ||
            BX_NVRIVA_THIS crtc.index == 0x2D) {
          BX_NVRIVA_THIS calculate_retrace_timing();
        }
        return;
      }
      break;
    default:
      break;
  }

  VGA_WRITE(address,value,io_len);
}

// Hardware cursor

Bit8u nv4_alpha_wrap(int value)
{
  return -(value >> 8) ^ value;
}

Bit16u bx_nvriva_c::cursor_read16(Bit32u address)
{
  if (BX_NVRIVA_THIS hw_cursor.vram)
    return vram_read16(address);
  else
    return ramin_read16(address);
}

Bit32u bx_nvriva_c::cursor_read32(Bit32u address)
{
  if (BX_NVRIVA_THIS hw_cursor.vram)
    return vram_read32(address);
  else
    return ramin_read32(address);
}

void bx_nvriva_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  Bit16s hwcx = BX_NVRIVA_THIS hw_cursor.x;
  Bit16s hwcy = BX_NVRIVA_THIS hw_cursor.y;
  Bit8u size = BX_NVRIVA_THIS hw_cursor.size;

  if (BX_NVRIVA_THIS svga_double_width) {
    hwcx <<= 1;
    hwcy <<= 1;
    size <<= 1;
  }

  unsigned w, h;
  Bit8u* tile_ptr;
  if (info->snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    w = BX_NVRIVA_THIS svga_xres;
    h = BX_NVRIVA_THIS svga_yres;
  } else {
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  }
  if (BX_NVRIVA_THIS hw_cursor.enabled &&
      (int)xc < hwcx + size &&
      (int)(xc + w) > hwcx &&
      (int)yc < hwcy + size &&
      (int)(yc + h) > hwcy) {
    unsigned cx0 = hwcx > (int)xc ? hwcx : xc;
    unsigned cy0 = hwcy > (int)yc ? hwcy : yc;
    unsigned cx1 = hwcx + size < (int)(xc + w) ? hwcx + size : xc + w;
    unsigned cy1 = hwcy + size < (int)(yc + h) ? hwcy + size : yc + h;

    Bit8u display_color_bytes = BX_NVRIVA_THIS svga_bpp >> 3;
    Bit8u cursor_color_bytes = BX_NVRIVA_THIS hw_cursor.bpp32 ? 4 : 2;
    if (info->bpp == 15) info->bpp = 16;
    tile_ptr += info->pitch * (cy0 - yc) + info->bpp / 8 * (cx0 - xc);
    unsigned pitch = BX_NVRIVA_THIS hw_cursor.size * cursor_color_bytes;
    Bit32u cursor_ofs = BX_NVRIVA_THIS hw_cursor.offset +
      (pitch * (cy0 - hwcy) >> (int)BX_NVRIVA_THIS s.y_doublescan);
    Bit8u* vid_ptr = BX_NVRIVA_THIS disp_ptr + (BX_NVRIVA_THIS svga_pitch * cy0 >>
      (int)BX_NVRIVA_THIS s.y_doublescan);
    for (unsigned cy = cy0; cy < cy1; cy++) {
      Bit8u* tile_ptr2 = tile_ptr;
      Bit32u cursor_ofs2 = cursor_ofs + (cursor_color_bytes * (cx0 - hwcx) >>
        (int)BX_NVRIVA_THIS svga_double_width);
      Bit8u* vid_ptr2 = vid_ptr + (display_color_bytes * cx0 >>
        (int)BX_NVRIVA_THIS svga_double_width);
      for (unsigned cx = cx0; cx < cx1; cx++) {
        Bit8u dr, dg, db;
        if (display_color_bytes == 1) {
          if (info->is_indexed) {
            dr = dg = db = vid_ptr2[0];
          } else {
            Bit8u index = vid_ptr2[0];
            dr = (BX_NVRIVA_THIS s.pel.data[index].red << BX_NVRIVA_THIS s.dac_shift);
            dg = (BX_NVRIVA_THIS s.pel.data[index].green << BX_NVRIVA_THIS s.dac_shift);
            db = (BX_NVRIVA_THIS s.pel.data[index].blue << BX_NVRIVA_THIS s.dac_shift);
          }
        } else if (display_color_bytes == 2) {
          EXTRACT_565_TO_888(vid_ptr2[0] << 0 | vid_ptr2[1] << 8, dr, dg, db);
        } else {
          db = vid_ptr2[0];
          dg = vid_ptr2[1];
          dr = vid_ptr2[2];
        }
        Bit8u b, g, r;
        if (BX_NVRIVA_THIS hw_cursor.bpp32) {
          Bit32u cursor_color = cursor_read32(cursor_ofs2);
          if (cursor_color != 0) {
            Bit8u alpha, cr, cg, cb;
            EXTRACT_8888_TO_8888(cursor_color, alpha, cr, cg, cb);
            Bit8u ica = 0xFF - alpha;
            b = nv4_alpha_wrap(db * ica / 0xFF + cb);
            g = nv4_alpha_wrap(dg * ica / 0xFF + cg);
            r = nv4_alpha_wrap(dr * ica / 0xFF + cr);
          } else {
            b = db;
            g = dg;
            r = dr;
          }
        } else {
          Bit8u alpha, cr, cg, cb;
          EXTRACT_1555_TO_8888(cursor_read16(cursor_ofs2), alpha, cr, cg, cb);
          if (alpha) {
            b = cb;
            g = cg;
            r = cr;
          } else {
            b = db ^ cb;
            g = dg ^ cg;
            r = dr ^ cr;
          }
        }
        Bit32u color;
        if (display_color_bytes == 1) {
          color = b << 0 | g << 8 | r << 16;
        } else {
          color =
            BX_NVRIVA_THIS s.pel.data[b].blue << 0 |
            BX_NVRIVA_THIS s.pel.data[g].green << 8 |
            BX_NVRIVA_THIS s.pel.data[r].red << 16;
        }
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
        if (!BX_NVRIVA_THIS svga_double_width || (cx & 1)) {
          cursor_ofs2 += cursor_color_bytes;
          vid_ptr2 += display_color_bytes;
        }
      }
      tile_ptr += info->pitch;
      if (!BX_NVRIVA_THIS s.y_doublescan || (cy & 1)) {
        cursor_ofs += pitch;
        vid_ptr += BX_NVRIVA_THIS svga_pitch;
      }
    }
  }
}

// CRTC timing

Bit16u bx_nvriva_c::get_crtc_vtotal()
{
  // NV4/NV5: no CR41 extension bit
  return BX_NVRIVA_THIS crtc.reg[6] +
    ((BX_NVRIVA_THIS crtc.reg[7] & 0x01) << 8) +
    ((BX_NVRIVA_THIS crtc.reg[7] & 0x20) << 4) +
    ((BX_NVRIVA_THIS crtc.reg[0x25] & 1) << 10) + 2;
}

void bx_nvriva_c::get_crtc_params(bx_crtc_params_t* crtcp, Bit32u* vclock)
{
  Bit32u m = BX_NVRIVA_THIS ramdac_vpll & 0xFF;
  Bit32u n = (BX_NVRIVA_THIS ramdac_vpll >> 8) & 0xFF;
  Bit32u p = (BX_NVRIVA_THIS ramdac_vpll >> 16) & 0x07;
  // NV4/NV5: simple PLL formula, no vpll_b
  if ((BX_NVRIVA_THIS ramdac_pll_select & 0x200) != 0 && m) {
    Bit32u crystalFreq = 13500000;
    if (BX_NVRIVA_THIS straps0_primary & 0x00000040)
      crystalFreq = 14318000;
    *vclock = (Bit32u)((Bit64u)crystalFreq * n / m >> p);
    crtcp->htotal = BX_NVRIVA_THIS crtc.reg[0] +
                    ((BX_NVRIVA_THIS crtc.reg[0x2D] & 1) << 8) + 5;
    crtcp->vtotal = get_crtc_vtotal();
    // NV4/NV5: no CR41 extension bits
    crtcp->vbstart = BX_NVRIVA_THIS crtc.reg[21] +
                     ((BX_NVRIVA_THIS crtc.reg[7] & 0x08) << 5) +
                     ((BX_NVRIVA_THIS crtc.reg[9] & 0x20) << 4) +
                     ((BX_NVRIVA_THIS crtc.reg[0x25] & 0x08) << 7);
    crtcp->vrstart = BX_NVRIVA_THIS crtc.reg[16] +
                     ((BX_NVRIVA_THIS crtc.reg[7] & 0x04) << 6) +
                     ((BX_NVRIVA_THIS crtc.reg[7] & 0x80) << 2) +
                     ((BX_NVRIVA_THIS crtc.reg[0x25] & 0x04) << 8);
  } else {
    bx_vgacore_c::get_crtc_params(crtcp, vclock);
  }
}

// Display update

void bx_nvriva_c::update(void)
{
  Bit8u crtc28 = BX_NVRIVA_THIS crtc.reg[0x28];

  if (crtc28 & 0x80) {
    crtc28 &= 0x7F;
    BX_DEBUG(("Slaved mode activated"));
  }

  if (crtc28 == 0x00) {
    BX_NVRIVA_THIS bx_vgacore_c::update();
    return;
  }

  if (BX_NVRIVA_THIS svga_needs_update_mode) {
    Bit32u iTopOffset =
       BX_NVRIVA_THIS crtc.reg[0x0d] |
      (BX_NVRIVA_THIS crtc.reg[0x0c] << 8) |
      (BX_NVRIVA_THIS crtc.reg[0x19] & 0x1F) << 16;
    iTopOffset <<= 2;
    iTopOffset += BX_NVRIVA_THIS crtc_start;

    // NV4/NV5: no CR42 extension bit for pitch
    Bit32u iPitch =
       BX_NVRIVA_THIS crtc.reg[0x13] |
      (BX_NVRIVA_THIS crtc.reg[0x19] >> 5 << 8);
    iPitch <<= 3;

    Bit8u iBpp = 0;
    if (crtc28 == 0x01)
      iBpp = 8;
    else if (crtc28 == 0x02)
      iBpp = 16;
    else if (crtc28 == 0x03)
      iBpp = 32;
    else
      BX_PANIC(("unknown bpp"));

    // NV4/NV5: no CR41 extension bit for width/height
    Bit32u iWidth =
      (BX_NVRIVA_THIS crtc.reg[1] +
      ((BX_NVRIVA_THIS crtc.reg[0x2D] & 0x02) << 7) + 1) * 8;
    Bit32u iHeight =
      (BX_NVRIVA_THIS crtc.reg[18] |
      ((BX_NVRIVA_THIS crtc.reg[7] & 0x02) << 7) |
      ((BX_NVRIVA_THIS crtc.reg[7] & 0x40) << 3) |
      ((BX_NVRIVA_THIS crtc.reg[0x25] & 0x02) << 9)) + 1;

    if (BX_NVRIVA_THIS s.y_doublescan && iHeight > iWidth) {
      iWidth <<= 1;
      BX_NVRIVA_THIS svga_double_width = true;
    } else {
      BX_NVRIVA_THIS svga_double_width = false;
    }

    if (iWidth != BX_NVRIVA_THIS svga_xres ||
        iHeight != BX_NVRIVA_THIS svga_yres ||
        iBpp != BX_NVRIVA_THIS svga_bpp) {
      BX_INFO(("switched to %u x %u x %u", iWidth, iHeight, iBpp));
    }

    BX_NVRIVA_THIS svga_xres = iWidth;
    BX_NVRIVA_THIS svga_yres = iHeight;
    BX_NVRIVA_THIS svga_bpp = iBpp;
    BX_NVRIVA_THIS svga_dispbpp = iBpp;
    BX_NVRIVA_THIS disp_ptr = BX_NVRIVA_THIS s.memory + iTopOffset;
    BX_NVRIVA_THIS disp_offset = iTopOffset;
    BX_NVRIVA_THIS disp_end_offset = iTopOffset + iPitch * iHeight;
    BX_NVRIVA_THIS svga_pitch = iPitch;
    // compatibility settings for VGA core
    BX_NVRIVA_THIS s.last_xres = iWidth;
    BX_NVRIVA_THIS s.last_yres = iHeight;
    BX_NVRIVA_THIS s.last_bpp = iBpp;
    BX_NVRIVA_THIS s.last_fh = 0;
  }

  unsigned width, height, pitch;

  if (BX_NVRIVA_THIS svga_dispbpp != 4) {
    width  = BX_NVRIVA_THIS svga_xres;
    height = BX_NVRIVA_THIS svga_yres;
    pitch = BX_NVRIVA_THIS svga_pitch;
    if (BX_NVRIVA_THIS svga_needs_update_mode) {
      bx_gui->dimension_update(width, height, 0, 0, BX_NVRIVA_THIS svga_dispbpp);
      BX_NVRIVA_THIS s.last_bpp = BX_NVRIVA_THIS svga_dispbpp;
      BX_NVRIVA_THIS svga_needs_update_mode = 0;
      BX_NVRIVA_THIS svga_needs_update_dispentire = 1;
    }
  } else {
    BX_NVRIVA_THIS determine_screen_dimensions(&height, &width);
    pitch = BX_NVRIVA_THIS s.line_offset;
    if ((width != BX_NVRIVA_THIS s.last_xres) || (height != BX_NVRIVA_THIS s.last_yres) ||
        (BX_NVRIVA_THIS s.last_bpp > 8)) {
      bx_gui->dimension_update(width, height);
      BX_NVRIVA_THIS s.last_xres = width;
      BX_NVRIVA_THIS s.last_yres = height;
      BX_NVRIVA_THIS s.last_bpp = 8;
    }
  }

  if (BX_NVRIVA_THIS svga_needs_update_dispentire) {
    BX_NVRIVA_THIS redraw_area_d(0, 0, width, height);
    BX_NVRIVA_THIS svga_needs_update_dispentire = 0;
  }

  if (!BX_NVRIVA_THIS svga_needs_update_tile)
    return;

  BX_NVRIVA_THIS svga_needs_update_tile = 0;

  unsigned xc, yc, xti, yti, hp;
  unsigned r, c, w, h, x, y;
  int i;
  Bit8u red, green, blue;
  Bit32u colour, row_addr;
  Bit8u * vid_ptr, * vid_ptr2;
  Bit8u * tile_ptr, * tile_ptr2;
  bx_svga_tileinfo_t info;
  Bit8u dac_size = (BX_NVRIVA_THIS s.dac_shift == 0) ? 8 : 6;

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = BX_NVRIVA_THIS disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < height; yc++) {
          vid_ptr2  = vid_ptr;
          tile_ptr2 = tile_ptr;
          if (BX_NVRIVA_THIS svga_dispbpp != 4) {
            for (xc = 0; xc < width; xc++) {
              memcpy(tile_ptr2, vid_ptr2, (BX_NVRIVA_THIS svga_bpp >> 3));
              if (!BX_NVRIVA_THIS svga_double_width || (xc & 1)) {
                vid_ptr2 += (BX_NVRIVA_THIS svga_bpp >> 3);
              }
              tile_ptr2 += ((info.bpp + 1) >> 3);
            }
            if (!BX_NVRIVA_THIS s.y_doublescan || (yc & 1)) {
              vid_ptr += pitch;
            }
          } else {
            row_addr = BX_NVRIVA_THIS s.CRTC.start_addr + (yc * pitch);
            for (xc = 0; xc < width; xc++) {
              *(tile_ptr2++) = BX_NVRIVA_THIS get_vga_pixel(xc, yc, row_addr, 0xffff, 0, BX_NVRIVA_THIS s.memory);
            }
          }
          tile_ptr += info.pitch;
        }
        draw_hardware_cursor(0, 0, &info);
      }
    } else if (info.is_indexed) {
      switch (BX_NVRIVA_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  y = yc + r;
                  if (BX_NVRIVA_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_NVRIVA_THIS s.CRTC.start_addr + (y * pitch);
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    x = xc + c;
                    *(tile_ptr2++) = BX_NVRIVA_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_NVRIVA_THIS s.memory);
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
        case 16:
        case 32:
          BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays, svga_dispbpp=%d",
            BX_NVRIVA_THIS svga_dispbpp));
          break;
        case 8:
          hp = BX_NVRIVA_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_NVRIVA_THIS s.y_doublescan) {
                  vid_ptr = BX_NVRIVA_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_NVRIVA_THIS svga_double_width) {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_NVRIVA_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    *(tile_ptr2++) = colour;
                  }
                  if (!BX_NVRIVA_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
      }
    }
    else {
      switch (BX_NVRIVA_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti=0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<Y_TILESIZE; r++) {
                  tile_ptr2 = tile_ptr;
                  y = yc + r;
                  if (BX_NVRIVA_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_NVRIVA_THIS s.CRTC.start_addr + (y * pitch);
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    colour = BX_NVRIVA_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_NVRIVA_THIS s.memory);
                    colour = MAKE_COLOUR(
                      BX_NVRIVA_THIS s.pel.data[colour].red, 6, info.red_shift, info.red_mask,
                      BX_NVRIVA_THIS s.pel.data[colour].green, 6, info.green_shift, info.green_mask,
                      BX_NVRIVA_THIS s.pel.data[colour].blue, 6, info.blue_shift, info.blue_mask);
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
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 8:
          hp = BX_NVRIVA_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_NVRIVA_THIS s.y_doublescan) {
                  vid_ptr = BX_NVRIVA_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_NVRIVA_THIS svga_double_width) {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_NVRIVA_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    colour = MAKE_COLOUR(
                      BX_NVRIVA_THIS s.pel.data[colour].red, dac_size, info.red_shift, info.red_mask,
                      BX_NVRIVA_THIS s.pel.data[colour].green, dac_size, info.green_shift, info.green_mask,
                      BX_NVRIVA_THIS s.pel.data[colour].blue, dac_size, info.blue_shift, info.blue_mask);
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
                  if (!BX_NVRIVA_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
          hp = BX_NVRIVA_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_NVRIVA_THIS s.y_doublescan) {
                  vid_ptr = BX_NVRIVA_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_NVRIVA_THIS svga_double_width) {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_NVRIVA_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 2;
                    }
                    EXTRACT_x555_TO_888(colour, red, green, blue);
                    if (info.bpp >= 24) {
                      colour =
                        (BX_NVRIVA_THIS s.pel.data[red].red << 16) |
                        (BX_NVRIVA_THIS s.pel.data[green].green << 8) |
                        BX_NVRIVA_THIS s.pel.data[blue].blue;
                    } else {
                      colour = MAKE_COLOUR(
                        BX_NVRIVA_THIS s.pel.data[blue].blue, 8, info.blue_shift, info.blue_mask,
                        BX_NVRIVA_THIS s.pel.data[green].green, 8, info.green_shift, info.green_mask,
                        BX_NVRIVA_THIS s.pel.data[red].red, 8, info.red_shift, info.red_mask);
                    }
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
                  if (!BX_NVRIVA_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 16:
          hp = BX_NVRIVA_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_NVRIVA_THIS s.y_doublescan) {
                  vid_ptr = BX_NVRIVA_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_NVRIVA_THIS svga_double_width) {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_NVRIVA_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 2;
                    }
                    EXTRACT_565_TO_888(colour, red, green, blue);
                    if (info.bpp >= 24) {
                      colour =
                        (BX_NVRIVA_THIS s.pel.data[red].red << 16) |
                        (BX_NVRIVA_THIS s.pel.data[green].green << 8) |
                        BX_NVRIVA_THIS s.pel.data[blue].blue;
                    } else {
                      colour = MAKE_COLOUR(
                        BX_NVRIVA_THIS s.pel.data[blue].blue, 8, info.blue_shift, info.blue_mask,
                        BX_NVRIVA_THIS s.pel.data[green].green, 8, info.green_shift, info.green_mask,
                        BX_NVRIVA_THIS s.pel.data[red].red, 8, info.red_shift, info.red_mask);
                    }
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
                  if (!BX_NVRIVA_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 32:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_NVRIVA_THIS s.y_doublescan) {
                  vid_ptr = BX_NVRIVA_THIS disp_ptr + (yc * pitch + (xc << 2));
                } else {
                  if (!BX_NVRIVA_THIS svga_double_width) {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 2));
                  } else {
                    vid_ptr = BX_NVRIVA_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 1));
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
                    if (!BX_NVRIVA_THIS svga_double_width || (c & 1)) {
                      vid_ptr2 += 4;
                    }
                    colour = MAKE_COLOUR(
                      BX_NVRIVA_THIS s.pel.data[red].red, 8, info.red_shift, info.red_mask,
                      BX_NVRIVA_THIS s.pel.data[green].green, 8, info.green_shift, info.green_mask,
                      BX_NVRIVA_THIS s.pel.data[blue].blue, 8, info.blue_shift, info.blue_mask);
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
                  if (!BX_NVRIVA_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_NVRIVA_THIS, xti, yti, 0);
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

// Extended CRTC registers

Bit8u bx_nvriva_c::svga_read_crtc(Bit32u address, unsigned index)
{
  if (index <= NVRIVA_CRTC_MAX) {
    Bit8u value = BX_NVRIVA_THIS crtc.reg[index];
    BX_DEBUG(("crtc: index 0x%02x read 0x%02x", index, value));
    return value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x read", index));

  return 0xff;
}

void bx_nvriva_c::svga_write_crtc(Bit32u address, unsigned index, Bit8u value)
{
  BX_DEBUG(("crtc: index 0x%02x write 0x%02x", index, (unsigned)value));

  bool update_cursor_addr = false;

  if (index == 0x1c) {
    if (!(BX_NVRIVA_THIS crtc.reg[index] & 0x80) && (value & 0x80) != 0) {
      // Without clearing this register, Windows 95 hangs after reboot
      BX_NVRIVA_THIS crtc_intr_en = 0x00000000;
      update_irq_level();
    }
  } else if (index == 0x1d || index == 0x1e)
    BX_NVRIVA_THIS bank_base[index - 0x1d] = value * 0x8000;
  else if (index == 0x2f || index == 0x30 || index == 0x31)
    update_cursor_addr = true;
  else if (index == 0x3f) {
    bool scl = value & 0x20;
    bool sda = value & 0x10;
    BX_NVRIVA_THIS ddc.write(scl, sda);
    BX_NVRIVA_THIS crtc.reg[0x3e] = BX_NVRIVA_THIS ddc.read() & 0x0c;
  }

  if (index <= NVRIVA_CRTC_MAX) {
    BX_NVRIVA_THIS crtc.reg[index] = value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x write", index));

  if (update_cursor_addr) {
    bool prev_enabled = BX_NVRIVA_THIS hw_cursor.enabled;
    BX_NVRIVA_THIS hw_cursor.enabled =
      (BX_NVRIVA_THIS crtc.reg[0x31] & 0x01) ||
      (BX_NVRIVA_THIS crtc_cursor_config & 0x00000001);
    BX_NVRIVA_THIS hw_cursor.vram =
      (BX_NVRIVA_THIS crtc.reg[0x30] & 0x80) ||
      (BX_NVRIVA_THIS crtc_cursor_config & 0x00000100);
    BX_NVRIVA_THIS hw_cursor.offset =
      (BX_NVRIVA_THIS crtc.reg[0x31] >> 2 << 11) |
      (BX_NVRIVA_THIS crtc.reg[0x30] & 0x7F) << 17 |
       BX_NVRIVA_THIS crtc.reg[0x2f] << 24;
    BX_NVRIVA_THIS hw_cursor.offset += BX_NVRIVA_THIS crtc_cursor_offset;
    if (prev_enabled != BX_NVRIVA_THIS hw_cursor.enabled) {
      BX_NVRIVA_THIS redraw_area_nd(BX_NVRIVA_THIS hw_cursor.x, BX_NVRIVA_THIS hw_cursor.y,
        BX_NVRIVA_THIS hw_cursor.size, BX_NVRIVA_THIS hw_cursor.size);
    }
  }
}

// Register 8-bit read/write (VGA I/O remapped in MMIO)

Bit8u bx_nvriva_c::register_read8(Bit32u address)
{
  Bit8u value;
  if (address >= 0x1800 && address < 0x1900) {
    value = BX_NVRIVA_THIS pci_conf[address - 0x1800];
  } else if (address >= 0x300000 && address < 0x310000) {
    if (BX_NVRIVA_THIS pci_conf[0x50] == 0x00)
      value = BX_NVRIVA_THIS pci_rom[address - 0x300000];
    else
      value = 0x00;
  } else if (address >= 0xc0300 && address < 0xc0400) {
    // PRMVIO: single head only on NV4/NV5
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3c3 ||
        offset == 0x3c4 || offset == 0x3c5 ||
        offset == 0x3cc || offset == 0x3cf) {
      value = SVGA_READ(offset, 1);
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x601300 && address < 0x601400) {
    // PRMCIO: single head only
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3b4 || offset == 0x3b5 ||
        offset == 0x3c0 || offset == 0x3c1 ||
        offset == 0x3c2 || offset == 0x3d4 ||
        offset == 0x3d5 || offset == 0x3d8 ||
        offset == 0x3da) {
      value = SVGA_READ(offset, 1);
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x681300 && address < 0x681400) {
    // PRMDIO: single head only
    Bit32u offset = address & 0x00000fff;
    if (offset >= 0x3c6 && offset <= 0x3c9) {
      value = SVGA_READ(offset, 1);
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x700000 && address < 0x800000) {
    // NV4/NV5: RAMIN access, XOR with ramin_flip to map to top of VRAM
    value = BX_NVRIVA_THIS s.memory[(address - 0x700000) ^ BX_NVRIVA_THIS ramin_flip];
  } else {
    Bit32u aligned = address & ~3;
    Bit32u shift = (address & 3) * 8;
    value = register_read32(aligned) >> shift;
  }
  return value;
}

void bx_nvriva_c::register_write8(Bit32u address, Bit8u value)
{
  if (address >= 0xc0300 && address < 0xc0400) {
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3c2 || offset == 0x3c3 ||
        offset == 0x3c4 || offset == 0x3c5 ||
        offset == 0x3ce || offset == 0x3cf) {
      SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x601300 && address < 0x601400) {
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3b4 || offset == 0x3b5 ||
        offset == 0x3c0 || offset == 0x3c1 ||
        offset == 0x3c2 || offset == 0x3d4 ||
        offset == 0x3d5 || offset == 0x3da) {
      SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x681300 && address < 0x681400) {
    Bit32u offset = address & 0x00000fff;
    if (offset >= 0x3c6 && offset <= 0x3c9) {
      SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x700000 && address < 0x800000) {
    // NV4/NV5: RAMIN access, XOR with ramin_flip to map to top of VRAM
    BX_NVRIVA_THIS s.memory[(address - 0x700000) ^ BX_NVRIVA_THIS ramin_flip] = value;
  } else {
    Bit32u aligned = address & ~3;
    Bit32u shift = (address & 3) * 8;
    Bit32u mask = 0xFF << shift;
    Bit32u value32 = register_read32(aligned);
    value32 = (value32 & ~mask) | ((Bit32u)value << shift);
    register_write32(aligned, value32);
  }
}

// VRAM access functions

Bit8u bx_nvriva_c::vram_read8(Bit32u address)
{
  return BX_NVRIVA_THIS s.memory[address];
}

Bit16u bx_nvriva_c::vram_read16(Bit32u address)
{
  return
    BX_NVRIVA_THIS s.memory[address + 0] << 0 |
    BX_NVRIVA_THIS s.memory[address + 1] << 8;
}

Bit32u bx_nvriva_c::vram_read32(Bit32u address)
{
  return
    BX_NVRIVA_THIS s.memory[address + 0] << 0 |
    BX_NVRIVA_THIS s.memory[address + 1] << 8 |
    BX_NVRIVA_THIS s.memory[address + 2] << 16 |
    BX_NVRIVA_THIS s.memory[address + 3] << 24;
}

Bit64u bx_nvriva_c::vram_read64(Bit32u address)
{
  return
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 0] << 0 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 1] << 8 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 2] << 16 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 3] << 24 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 4] << 32 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 5] << 40 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 6] << 48 |
    (Bit64u)BX_NVRIVA_THIS s.memory[address + 7] << 56;
}

void bx_nvriva_c::vram_write8(Bit32u address, Bit8u value)
{
  BX_NVRIVA_THIS s.memory[address + 0] = value;
}

void bx_nvriva_c::vram_write16(Bit32u address, Bit16u value)
{
  BX_NVRIVA_THIS s.memory[address + 0] = (value >> 0) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 1] = (value >> 8) & 0xFF;
}

void bx_nvriva_c::vram_write32(Bit32u address, Bit32u value)
{
  BX_NVRIVA_THIS s.memory[address + 0] = (value >> 0) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 1] = (value >> 8) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 2] = (value >> 16) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 3] = (value >> 24) & 0xFF;
}

void bx_nvriva_c::vram_write64(Bit32u address, Bit64u value)
{
  BX_NVRIVA_THIS s.memory[address + 0] = (value >> 0) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 1] = (value >> 8) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 2] = (value >> 16) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 3] = (value >> 24) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 4] = (value >> 32) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 5] = (value >> 40) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 6] = (value >> 48) & 0xFF;
  BX_NVRIVA_THIS s.memory[address + 7] = (value >> 56) & 0xFF;
}

// RAMIN access - XOR with ramin_flip to map to top of VRAM

Bit8u bx_nvriva_c::ramin_read8(Bit32u address)
{
  return vram_read8(address ^ BX_NVRIVA_THIS ramin_flip);
}

Bit16u bx_nvriva_c::ramin_read16(Bit32u address)
{
  return vram_read16(address ^ BX_NVRIVA_THIS ramin_flip);
}

Bit32u bx_nvriva_c::ramin_read32(Bit32u address)
{
  return vram_read32(address ^ BX_NVRIVA_THIS ramin_flip);
}

void bx_nvriva_c::ramin_write8(Bit32u address, Bit8u value)
{
  vram_write8(address ^ BX_NVRIVA_THIS ramin_flip, value);
}

void bx_nvriva_c::ramin_write32(Bit32u address, Bit32u value)
{
  vram_write32(address ^ BX_NVRIVA_THIS ramin_flip, value);
}

// Physical memory access

Bit8u bx_nvriva_c::physical_read8(Bit32u address)
{
  Bit8u data;
  DEV_MEM_READ_PHYSICAL(address, 1, &data);
  return data;
}

Bit16u bx_nvriva_c::physical_read16(Bit32u address)
{
  Bit8u data[2];
  DEV_MEM_READ_PHYSICAL(address, 2, data);
  return data[0] << 0 | data[1] << 8;
}

Bit32u bx_nvriva_c::physical_read32(Bit32u address)
{
  Bit8u data[4];
  DEV_MEM_READ_PHYSICAL(address, 4, data);
  return data[0] << 0 | data[1] << 8 | data[2] << 16 | data[3] << 24;
}

Bit64u bx_nvriva_c::physical_read64(Bit32u address)
{
  Bit8u data[8];
  DEV_MEM_READ_PHYSICAL(address, 8, data);
  return (Bit64u)data[0] << 0 | (Bit64u)data[1] << 8 | (Bit64u)data[2] << 16 |
    (Bit64u)data[3] << 24 | (Bit64u)data[4] << 32 | (Bit64u)data[5] << 40 |
    (Bit64u)data[6] << 48 | (Bit64u)data[7] << 56;
}

void bx_nvriva_c::physical_write8(Bit32u address, Bit8u value)
{
  DEV_MEM_WRITE_PHYSICAL(address, 1, &value);
}

void bx_nvriva_c::physical_write16(Bit32u address, Bit16u value)
{
#ifndef BX_LITTLE_ENDIAN
  Bit8u data[2];
  data[0] = (value >> 0) & 0xFF;
  data[1] = (value >> 8) & 0xFF;
#else
  Bit8u *data = (Bit8u *)(&value);
#endif
  DEV_MEM_WRITE_PHYSICAL(address, 2, data);
}

void bx_nvriva_c::physical_write32(Bit32u address, Bit32u value)
{
#ifndef BX_LITTLE_ENDIAN
  Bit8u data[4];
  data[0] = (value >> 0) & 0xFF;
  data[1] = (value >> 8) & 0xFF;
  data[2] = (value >> 16) & 0xFF;
  data[3] = (value >> 24) & 0xFF;
#else
  Bit8u *data = (Bit8u *)(&value);
#endif
  DEV_MEM_WRITE_PHYSICAL(address, 4, data);
}

void bx_nvriva_c::physical_write64(Bit32u address, Bit64u value)
{
#ifndef BX_LITTLE_ENDIAN
  Bit8u data[8];
  data[0] = (value >> 0) & 0xFF;
  data[1] = (value >> 8) & 0xFF;
  data[2] = (value >> 16) & 0xFF;
  data[3] = (value >> 24) & 0xFF;
  data[4] = (value >> 32) & 0xFF;
  data[5] = (value >> 40) & 0xFF;
  data[6] = (value >> 48) & 0xFF;
  data[7] = (value >> 56) & 0xFF;
#else
  Bit8u *data = (Bit8u *)(&value);
#endif
  DEV_MEM_WRITE_PHYSICAL(address, 8, data);
}

// DMA access

Bit32u bx_nvriva_c::dma_pt_lookup(Bit32u object, Bit32u address)
{
  Bit32u address_adj = address + (ramin_read32(object) >> 20);
  Bit32u page_offset = address_adj & 0xFFF;
  Bit32u page_index = address_adj >> 12;
  Bit32u page = ramin_read32(object + 8 + page_index * 4) & 0xFFFFF000;
  return page | page_offset;
}

Bit32u bx_nvriva_c::dma_lin_lookup(Bit32u object, Bit32u address)
{
  Bit32u adjust = ramin_read32(object) >> 20;
  Bit32u base = ramin_read32(object + 8) & 0xFFFFF000;
  return base + adjust + address;
}

Bit8u bx_nvriva_c::dma_read8(Bit32u object, Bit32u address)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    return physical_read8(addr_abs);
  else
    return vram_read8(addr_abs & BX_NVRIVA_THIS memsize_mask);
}

Bit16u bx_nvriva_c::dma_read16(Bit32u object, Bit32u address)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    return physical_read16(addr_abs);
  else
    return vram_read16(addr_abs & BX_NVRIVA_THIS memsize_mask);
}

Bit32u bx_nvriva_c::dma_read32(Bit32u object, Bit32u address)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    return physical_read32(addr_abs);
  else
    return vram_read32(addr_abs & BX_NVRIVA_THIS memsize_mask);
}

Bit64u bx_nvriva_c::dma_read64(Bit32u object, Bit32u address)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    return physical_read64(addr_abs);
  else
    return vram_read64(addr_abs & BX_NVRIVA_THIS memsize_mask);
}

void bx_nvriva_c::dma_write8(Bit32u object, Bit32u address, Bit8u value)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    physical_write8(addr_abs, value);
  else
    vram_write8(addr_abs, value);
}

void bx_nvriva_c::dma_write16(Bit32u object, Bit32u address, Bit16u value)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    physical_write16(addr_abs, value);
  else
    vram_write16(addr_abs, value);
}

void bx_nvriva_c::dma_write32(Bit32u object, Bit32u address, Bit32u value)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    physical_write32(addr_abs, value);
  else
    vram_write32(addr_abs, value);
}

void bx_nvriva_c::dma_write64(Bit32u object, Bit32u address, Bit64u value)
{
  Bit32u flags = ramin_read32(object);
  Bit32u addr_abs;
  if (flags & 0x00002000)
    addr_abs = dma_lin_lookup(object, address);
  else
    addr_abs = dma_pt_lookup(object, address);
  if (flags & 0x00020000)
    physical_write64(addr_abs, value);
  else
    vram_write64(addr_abs, value);
}

void bx_nvriva_c::dma_copy(Bit32u dst_obj, Bit32u dst_addr,
  Bit32u src_obj, Bit32u src_addr, Bit32u byte_count)
{
  Bit32u dst_flags = ramin_read32(dst_obj);
  Bit32u src_flags = ramin_read32(src_obj);
  Bit8u buffer[0x1000];
  Bit32u bytes_left = byte_count;
  while (bytes_left) {
    Bit32u dst_addr_abs;
    Bit32u src_addr_abs;
    if (dst_flags & 0x00002000)
      dst_addr_abs = dma_lin_lookup(dst_obj, dst_addr);
    else
      dst_addr_abs = dma_pt_lookup(dst_obj, dst_addr);
    if (src_flags & 0x00002000)
      src_addr_abs = dma_lin_lookup(src_obj, src_addr);
    else
      src_addr_abs = dma_pt_lookup(src_obj, src_addr);
    Bit32u chunk_bytes = BX_MIN(bytes_left, BX_MIN(
      0x1000 - (dst_addr_abs & 0xFFF),
      0x1000 - (src_addr_abs & 0xFFF)));
    if (src_flags & 0x00020000)
      DEV_MEM_READ_PHYSICAL(src_addr_abs, chunk_bytes, buffer);
    else
      memcpy(buffer, BX_NVRIVA_THIS s.memory + src_addr_abs, chunk_bytes);
    if (dst_flags & 0x00020000)
      DEV_MEM_WRITE_PHYSICAL(dst_addr_abs, chunk_bytes, buffer);
    else
      memcpy(BX_NVRIVA_THIS s.memory + dst_addr_abs, buffer, chunk_bytes);
    dst_addr += chunk_bytes;
    src_addr += chunk_bytes;
    bytes_left -= chunk_bytes;
  }
}

// RAMFC/RAMHT - NV4/NV5 style

Bit32u bx_nvriva_c::ramfc_address(Bit32u chid, Bit32u offset)
{
  // NV4/NV5: 0x20 byte per channel RAMFC
  Bit32u ramfc = (BX_NVRIVA_THIS fifo_ramfc & 0xFFF) << 8;
  return ramfc + chid * 0x20 + offset;
}

void bx_nvriva_c::ramfc_write32(Bit32u chid, Bit32u offset, Bit32u value)
{
  ramin_write32(ramfc_address(chid, offset), value);
}

Bit32u bx_nvriva_c::ramfc_read32(Bit32u chid, Bit32u offset)
{
  return ramin_read32(ramfc_address(chid, offset));
}

void bx_nvriva_c::ramht_lookup(Bit32u handle, Bit32u chid, Bit32u* object, Bit8u* engine)
{
  Bit32u ramht_addr = (BX_NVRIVA_THIS fifo_ramht & 0xFFF) << 8;
  Bit32u ramht_bits = ((BX_NVRIVA_THIS fifo_ramht >> 16) & 0xFF) + 9;
  Bit32u ramht_size = 1 << ramht_bits << 3;

  Bit32u hash = 0;
  Bit32u x = handle;
  while (x) {
    hash ^= (x & ((1 << ramht_bits) - 1));
    x >>= ramht_bits;
  }
  hash ^= (chid & 0xF) << (ramht_bits - 4);
  hash = hash << 3;

  Bit32u it = hash;
  Bit32u steps = 1;
  do {
    if (ramin_read32(ramht_addr + it) == handle) {
      Bit32u context = ramin_read32(ramht_addr + it + 4);
      // NV4/NV5 context format
      Bit32u ctx_chid = (context >> 24) & 0xF;
      if (chid == ctx_chid) {
        BX_DEBUG(("ramht_lookup: 0x%08x -> 0x%08x, steps: %d", handle, context, steps));
        if (object)
          *object = (context & 0xFFFF) << 4;
        if (engine)
          *engine = (context >> 16) & 0xFF;
        return;
      }
    }
    steps++;
    it += 8;
    if (it >= ramht_size)
      it = 0;
  } while (it != hash);

  BX_PANIC(("ramht_lookup failed for 0x%08x", handle));
}

// Color conversion helpers

static Bit32u color_565_to_888(Bit16u value)
{
  Bit8u r, g, b;
  EXTRACT_565_TO_888(value, r, g, b);
  return r << 16 | g << 8 | b;
}

static Bit16u color_888_to_565(Bit32u value)
{
  return (((value >> 19) & 0x1F) << 11) | (((value >> 10) & 0x3F) << 5) | ((value >> 3) & 0x1F);
}

// 2D pixel operations

Bit32u bx_nvriva_c::get_pixel(Bit32u obj, Bit32u ofs, Bit32u x, Bit32u cb)
{
  Bit32u result;
  if (cb == 1)
    result = dma_read8(obj, ofs + x);
  else if (cb == 2)
    result = dma_read16(obj, ofs + x * 2);
  else
    result = dma_read32(obj, ofs + x * 4);
  return result;
}

void bx_nvriva_c::put_pixel(nv_channel* ch, Bit32u ofs, Bit32u x, Bit32u value)
{
  if (ch->s2d_color_bytes == 1)
    dma_write8(ch->s2d_img_dst, ofs + x, value);
  else if (ch->s2d_color_bytes == 2)
    dma_write16(ch->s2d_img_dst, ofs + x * 2, value);
  else if (ch->s2d_color_fmt == 6)
    dma_write32(ch->s2d_img_dst, ofs + x * 4, value & 0x00FFFFFF);
  else
    dma_write32(ch->s2d_img_dst, ofs + x * 4, value);
}

void bx_nvriva_c::pixel_operation(nv_channel* ch, Bit32u op,
  Bit32u* dstcolor, const Bit32u* srccolor, Bit32u cb, Bit32u px, Bit32u py)
{
  if (op == 1) {
    Bit8u rop = ch->rop;
    if (BX_NVRIVA_THIS rop_flags[rop]) {
      Bit32u i = py % 8 * 8 + px % 8;
      Bit32u patt_color;
      if (ch->patt_type_color)
        patt_color = ch->patt_data_color[i];
      else
        patt_color = ch->patt_data_mono[i] ? ch->patt_fg_color : ch->patt_bg_color;
      bx_ternary_rop(rop, (Bit8u*)dstcolor, (Bit8u*)srccolor, (Bit8u*)&patt_color, cb);
    } else {
      BX_NVRIVA_THIS rop_handler[rop]((Bit8u*)dstcolor, (Bit8u*)srccolor, 0, 0, cb, 1);
    }
  } else if (op == 5) {
    if (cb == 4) {
      if (*srccolor) {
        Bit8u sb = *srccolor;
        Bit8u sg = *srccolor >> 8;
        Bit8u sr = *srccolor >> 16;
        Bit8u sa = *srccolor >> 24;
        Bit32u beta = ch->beta;
        if (beta != 0xFFFFFFFF) {
          Bit8u bb = beta;
          Bit8u bg = beta >> 8;
          Bit8u br = beta >> 16;
          Bit8u ba = beta >> 24;
          sb = sb * bb / 0xFF;
          sg = sg * bg / 0xFF;
          sr = sr * br / 0xFF;
          sa = sa * ba / 0xFF;
        }
        Bit8u db = *dstcolor;
        Bit8u dg = *dstcolor >> 8;
        Bit8u dr = *dstcolor >> 16;
        Bit8u da = *dstcolor >> 24;
        Bit8u isa = 0xFF - sa;
        Bit8u b = nv4_alpha_wrap(db * isa / 0xFF + sb);
        Bit8u g = nv4_alpha_wrap(dg * isa / 0xFF + sg);
        Bit8u r = nv4_alpha_wrap(dr * isa / 0xFF + sr);
        Bit8u a = nv4_alpha_wrap(da * isa / 0xFF + sa);
        *dstcolor = b << 0 | g << 8 | r << 16 | a << 24;
      }
    } else {
      Bit32u beta = ch->beta;
      Bit8u bb = beta;
      Bit8u bg = beta >> 8;
      Bit8u br = beta >> 16;
      Bit8u iba = 0xFF - (beta >> 24);
      Bit8u sb = *srccolor & 0x1F;
      Bit8u sg = (*srccolor >> 5) & 0x3F;
      Bit8u sr = (*srccolor >> 11) & 0x1F;
      Bit8u db = *dstcolor & 0x1F;
      Bit8u dg = (*dstcolor >> 5) & 0x3F;
      Bit8u dr = (*dstcolor >> 11) & 0x1F;
      Bit8u b = (db * iba + sb * bb) / 0xFF;
      Bit8u g = (dg * iba + sg * bg) / 0xFF;
      Bit8u r = (dr * iba + sr * br) / 0xFF;
      *dstcolor = b << 0 | g << 5 | r << 11;
    }
  } else {
    *dstcolor = *srccolor;
  }
}

// 2D drawing operations

void bx_nvriva_c::rect(nv_channel* ch)
{
  Bit16s dx = ch->rect_yx & 0xFFFF;
  Bit16s dy = ch->rect_yx >> 16;
  Bit16u width = ch->rect_hw & 0xFFFF;
  Bit16u height = ch->rect_hw >> 16;
  Bit32u pitch = ch->s2d_pitch_dst;
  Bit32u srccolor = ch->rect_color;
  Bit32u draw_offset = ch->s2d_ofs_dst +
    dy * pitch + dx * ch->s2d_color_bytes;
  Bit32u rect_dst_lin = dma_lin_lookup(ch->s2d_img_dst, draw_offset);
  BX_INFO(("rect: %d,%d %dx%d color=0x%08x op=%d dst_lin=0x%08x",
    dx, dy, width, height, srccolor, ch->rect_operation, rect_dst_lin));
  Bit32u redraw_offset = rect_dst_lin - BX_NVRIVA_THIS disp_offset;
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < width; x++) {
      Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
        draw_offset, x, ch->s2d_color_bytes);
      pixel_operation(ch, ch->rect_operation,
        &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
      put_pixel(ch, draw_offset, x, dstcolor);
    }
    draw_offset += pitch;
  }
  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, width, height);
}

void bx_nvriva_c::ifc(nv_channel* ch, Bit32u word)
{
  Bit32u chromacolor;
  bool chroma_enabled = false;
  if (ch->ifc_color_key_enable) {
    if (ch->ifc_color_bytes == 4) {
      chromacolor = ch->chroma_color & 0x00FFFFFF;
      chroma_enabled = ch->chroma_color & 0xFF000000;
    } else if (ch->ifc_color_bytes == 2) {
      chromacolor = ch->chroma_color & 0x0000FFFF;
      chroma_enabled = ch->chroma_color & 0xFFFF0000;
    } else {
      chromacolor = ch->chroma_color & 0x000000FF;
      chroma_enabled = ch->chroma_color & 0xFFFFFF00;
    }
  }
  for (Bit32u i = 0; i < ch->ifc_pixels_per_word; i++) {
    if (ch->ifc_x >= ch->ifc_clip_x0 && ch->ifc_x < ch->ifc_clip_x1 &&
        ch->ifc_y >= ch->ifc_clip_y0 && ch->ifc_y < ch->ifc_clip_y1) {
      Bit32u srccolor;
      if (ch->ifc_color_bytes == 4)
        srccolor = word;
      else if (ch->ifc_color_bytes == 2)
        srccolor = i == 0 ? word & 0xffff : word >> 16;
      else
        srccolor = (word >> (i * 8)) & 0xff;
      if (ch->ifc_operation == 5 && ch->ifc_x < 4 && ch->ifc_y == 0)
        BX_INFO(("ifc_pixel: x=%d y=%d src=0x%08x cfmt=%d",
          ch->ifc_x, ch->ifc_y, srccolor, ch->ifc_color_fmt));
      if (!chroma_enabled || srccolor != chromacolor) {
        Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
          ch->ifc_draw_offset, ch->ifc_x, ch->s2d_color_bytes);
        if (ch->ifc_color_bytes == 4 &&
            ch->s2d_color_bytes == 2)
          dstcolor = color_565_to_888(dstcolor);
        pixel_operation(ch, ch->ifc_operation, &dstcolor, &srccolor,
          ch->ifc_color_bytes, ch->ifc_ofs_x + ch->ifc_x, ch->ifc_ofs_y + ch->ifc_y);
        if (ch->ifc_color_bytes == 4 &&
            ch->s2d_color_bytes == 2)
          dstcolor = color_888_to_565(dstcolor);
        put_pixel(ch, ch->ifc_draw_offset, ch->ifc_x, dstcolor);
      }
    }
    ch->ifc_x++;
    if (ch->ifc_x >= ch->ifc_src_width) {
      BX_NVRIVA_THIS redraw_area_nd(ch->ifc_redraw_offset, ch->ifc_dst_width, 1);
      ch->ifc_draw_offset += ch->s2d_pitch_dst;
      ch->ifc_redraw_offset += ch->s2d_pitch_dst;
      ch->ifc_x = 0;
      ch->ifc_y++;
    }
  }
}

void bx_nvriva_c::copyarea(nv_channel* ch)
{
  Bit16u sx = ch->blit_syx & 0xFFFF;
  Bit16u sy = ch->blit_syx >> 16;
  Bit16u dx = ch->blit_dyx & 0xFFFF;
  Bit16u dy = ch->blit_dyx >> 16;
  Bit16u width = ch->blit_hw & 0xFFFF;
  Bit16u height = ch->blit_hw >> 16;
  Bit32u spitch = ch->s2d_pitch_src;
  Bit32u dpitch = ch->s2d_pitch_dst;
  Bit32u src_offset = ch->s2d_ofs_src;
  Bit32u draw_offset = ch->s2d_ofs_dst;
  BX_INFO(("copyarea: src=%d,%d dst=%d,%d wh=%dx%d src_lin=0x%08x dst_lin=0x%08x",
    sx, sy, dx, dy, width, height,
    dma_lin_lookup(ch->s2d_img_src, src_offset), dma_lin_lookup(ch->s2d_img_dst, draw_offset)));
  bool xdir = dx > sx;
  bool ydir = dy > sy;
  src_offset += (sy + ydir * (height - 1)) * spitch + sx * ch->s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(ch->s2d_img_dst, draw_offset) +
    dy * dpitch + dx * ch->s2d_color_bytes - BX_NVRIVA_THIS disp_offset;
  draw_offset += (dy + ydir * (height - 1)) * dpitch + dx * ch->s2d_color_bytes;
  Bit32u chromacolor;
  bool chroma_enabled = false;
  if (ch->blit_color_key_enable) {
    if (ch->s2d_color_bytes == 4) {
      chromacolor = ch->chroma_color & 0x00FFFFFF;
      chroma_enabled = ch->chroma_color & 0xFF000000;
    } else if (ch->s2d_color_bytes == 2) {
      chromacolor = ch->chroma_color & 0x0000FFFF;
      chroma_enabled = ch->chroma_color & 0xFFFF0000;
    } else {
      chromacolor = ch->chroma_color & 0x000000FF;
      chroma_enabled = ch->chroma_color & 0xFFFFFF00;
    }
  }
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < width; x++) {
      Bit16u xa = xdir ? width - x - 1 : x;
      Bit32u srccolor = get_pixel(ch->s2d_img_src,
        src_offset, xa, ch->s2d_color_bytes);
      if (!chroma_enabled || srccolor != chromacolor) {
        Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
          draw_offset, xa, ch->s2d_color_bytes);
        pixel_operation(ch, ch->blit_operation,
          &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
        put_pixel(ch, draw_offset, xa, dstcolor);
      }
    }
    src_offset += spitch * (1 - 2 * ydir);
    draw_offset += dpitch * (1 - 2 * ydir);
  }
  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, width, height);
}

void bx_nvriva_c::m2mf(nv_channel* ch)
{
  Bit32u src_offset = ch->m2mf_src_offset;
  Bit32u dst_offset = ch->m2mf_dst_offset;
  for (Bit16u y = 0; y < ch->m2mf_line_count; y++) {
    dma_copy(
      ch->m2mf_dst, dst_offset,
      ch->m2mf_src, src_offset,
      ch->m2mf_line_length);
    src_offset += ch->m2mf_src_pitch;
    dst_offset += ch->m2mf_dst_pitch;
  }
  Bit32u dma_target = ramin_read32(ch->m2mf_dst) >> 12 & 0xFF;
  if (dma_target == 0x03 || dma_target == 0x0b) {
    Bit32u redraw_offset = dma_lin_lookup(ch->m2mf_dst, ch->m2mf_dst_offset) -
      BX_NVRIVA_THIS disp_offset;
    Bit32u width = ch->m2mf_line_length / (BX_NVRIVA_THIS svga_bpp >> 3);
    BX_NVRIVA_THIS redraw_area_nd(redraw_offset, width, ch->m2mf_line_count);
  }
}

// 2D engine execute functions

void bx_nvriva_c::execute_clip(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0c0) {
    ch->clip_x = (Bit16u)param;
    ch->clip_y = param >> 16;
  } else if (method == 0x0c1) {
    ch->clip_width = (Bit16u)param;
    ch->clip_height = param >> 16;
  }
}

void bx_nvriva_c::execute_m2mf(nv_channel* ch, Bit32u subc, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    ch->m2mf_src = param;
  else if (method == 0x062)
    ch->m2mf_dst = param;
  else if (method == 0x0c3)
    ch->m2mf_src_offset = param;
  else if (method == 0x0c4)
    ch->m2mf_dst_offset = param;
  else if (method == 0x0c5)
    ch->m2mf_src_pitch = param;
  else if (method == 0x0c6)
    ch->m2mf_dst_pitch = param;
  else if (method == 0x0c7)
    ch->m2mf_line_length = param;
  else if (method == 0x0c8)
    ch->m2mf_line_count = param;
  else if (method == 0x0c9)
    ch->m2mf_format = param;
  else if (method == 0x0ca) {
    ch->m2mf_buffer_notify = param;
    m2mf(ch);
    if ((ramin_read32(ch->schs[subc].notifier) & 0xFF) == 0x30) {
      BX_DEBUG(("M2MF notify skipped"));
    } else {
      BX_DEBUG(("M2MF notify 0x%08x",
        ch->schs[subc].notifier));
      dma_write64(ch->schs[subc].notifier, 0x10 + 0x0, get_current_time());
      dma_write32(ch->schs[subc].notifier, 0x10 + 0x8, 0);
      dma_write32(ch->schs[subc].notifier, 0x10 + 0xC, 0);
    }
  }
}

void bx_nvriva_c::execute_rop(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    ch->rop = param;
}

void bx_nvriva_c::execute_patt(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0c2)
    ch->patt_shape = param;
  else if (method == 0x0c3)
    ch->patt_type_color = param == 2;
  else if (method == 0x0c4)
    ch->patt_bg_color = param;
  else if (method == 0x0c5)
    ch->patt_fg_color = param;
  else if (method == 0x0c6 || method == 0x0c7) {
    for (Bit32u i = 0; i < 32; i++)
      ch->patt_data_mono[i + (method & 1) * 32] = 1 << (i ^ 7) & param;
  } else if (method >= 0x100 && method < 0x110) {
    Bit32u i = (method - 0x100) * 4;
    ch->patt_data_color[i] = param & 0xFF;
    ch->patt_data_color[i + 1] = (param >> 8) & 0xFF;
    ch->patt_data_color[i + 2] = (param >> 16) & 0xFF;
    ch->patt_data_color[i + 3] = param >> 24;
  } else if (method >= 0x140 && method < 0x160) {
    Bit32u i = (method - 0x140) * 2;
    ch->patt_data_color[i] = param & 0xFFFF;
    ch->patt_data_color[i + 1] = param >> 16;
  } else if (method >= 0x1c0 && method < 0x200)
    ch->patt_data_color[method - 0x1c0] = param;
}

void bx_nvriva_c::execute_chroma(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    ch->chroma_color_fmt = param;
  else if (method == 0x0c1)
    ch->chroma_color = param;
}

void bx_nvriva_c::execute_rect(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0bf)
    ch->rect_operation = param;
  else if (method == 0x0c0)
    ch->rect_color_fmt = param;
  else if (method == 0x0c1)
    ch->rect_color = param;
  else if (method >= 0x100 && method < 0x120) {
    if (method & 1) {
      ch->rect_hw = param;
      rect(ch);
    } else
      ch->rect_yx = param;
  }
}

void bx_nvriva_c::execute_imageblit(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    ch->blit_color_key_enable = (ramin_read32(param) & 0xFF) != 0x30;
  else if (method == 0x0bf)
    ch->blit_operation = param;
  else if (method == 0x0c0)
    ch->blit_syx = param;
  else if (method == 0x0c1)
    ch->blit_dyx = param;
  else if (method == 0x0c2) {
    ch->blit_hw = param;
    copyarea(ch);
  }
}

void bx_nvriva_c::execute_ifc(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    ch->ifc_color_key_enable = (ramin_read32(param) & 0xFF) != 0x30;
  else if (method == 0x062)
    ch->ifc_clip_enable = (ramin_read32(param) & 0xFF) != 0x30;
  else if (method == 0x0bf)
    ch->ifc_operation = param;
  else if (method == 0x0c0) {
    ch->ifc_color_fmt = param;
    update_color_bytes_ifc(ch);
    ch->ifc_pixels_per_word = 4 / ch->ifc_color_bytes;
  } else if (method == 0x0c1) {
    ch->ifc_x = 0;
    ch->ifc_y = 0;
    ch->ifc_ofs_x = param & 0xFFFF;
    ch->ifc_ofs_y = param >> 16;
    ch->ifc_draw_offset = ch->s2d_ofs_dst +
      ch->ifc_ofs_y * ch->s2d_pitch_dst + ch->ifc_ofs_x * ch->s2d_color_bytes;
    ch->ifc_redraw_offset = dma_lin_lookup(ch->s2d_img_dst,
      ch->ifc_draw_offset) - BX_NVRIVA_THIS disp_offset;
  } else if (method == 0x0c2) {
    ch->ifc_dst_width = param & 0xFFFF;
    ch->ifc_dst_height = param >> 16;
    ch->ifc_clip_x0 = 0;
    ch->ifc_clip_y0 = 0;
    ch->ifc_clip_x1 = ch->ifc_dst_width;
    ch->ifc_clip_y1 = ch->ifc_dst_height;
    if (ch->ifc_clip_enable) {
      Bit32s clipx0 = ch->clip_x - ch->ifc_ofs_x;
      Bit32s clipy0 = ch->clip_y - ch->ifc_ofs_y;
      Bit32s clipx1 = clipx0 + ch->clip_width;
      Bit32s clipy1 = clipy0 + ch->clip_height;
      ch->ifc_clip_x0 = BX_MAX((Bit32s)ch->ifc_clip_x0, clipx0);
      ch->ifc_clip_y0 = BX_MAX((Bit32s)ch->ifc_clip_y0, clipy0);
      ch->ifc_clip_x1 = BX_MIN((Bit32s)ch->ifc_clip_x1, clipx1);
      ch->ifc_clip_y1 = BX_MIN((Bit32s)ch->ifc_clip_y1, clipy1);
    }
  } else if (method == 0x0c3) {
    ch->ifc_src_width = param & 0xFFFF;
    ch->ifc_src_height = param >> 16;
    BX_INFO(("ifc_setup: pos=%d,%d src=%dx%d dst=%dx%d cfmt=%d op=%d dst_lin=0x%08x",
      ch->ifc_ofs_x, ch->ifc_ofs_y, ch->ifc_src_width, ch->ifc_src_height,
      ch->ifc_dst_width, ch->ifc_dst_height, ch->ifc_color_fmt, ch->ifc_operation,
      dma_lin_lookup(ch->s2d_img_dst, ch->ifc_draw_offset)));
  } else if (method >= 0x100 && method < 0x800) {
    ifc(ch, param);
  }
}

void bx_nvriva_c::execute_surf2d(nv_channel* ch, Bit32u method, Bit32u param)
{
  ch->s2d_locked = true;
  if (method == 0x061)
    ch->s2d_img_src = param;
  else if (method == 0x062)
    ch->s2d_img_dst = param;
  else if (method == 0x0c0) {
    ch->s2d_color_fmt = param;
    Bit32u s2d_color_bytes_prev = ch->s2d_color_bytes;
    update_color_bytes_s2d(ch);
    if (ch->s2d_color_bytes != s2d_color_bytes_prev &&
        (ch->s2d_color_bytes == 1 || s2d_color_bytes_prev == 1)) {
      update_color_bytes_ifc(ch);
      update_color_bytes_sifc(ch);
    }
  } else if (method == 0x0c1) {
    ch->s2d_pitch_src = param & 0xFFFF;
    ch->s2d_pitch_dst = param >> 16;
  } else if (method == 0x0c2)
    ch->s2d_ofs_src = param;
  else if (method == 0x0c3)
    ch->s2d_ofs_dst = param;
}

void bx_nvriva_c::update_color_bytes_s2d(nv_channel* ch)
{
  if (ch->s2d_color_fmt == 0x1)        // Y8
    ch->s2d_color_bytes = 1;
  else if (ch->s2d_color_fmt == 0x2 || // X1R5G5B5_Z1R5G5B5
           ch->s2d_color_fmt == 0x3 || // X1R5G5B5_X1R5G5B5
           ch->s2d_color_fmt == 0x4 || // R5G6B5
           ch->s2d_color_fmt == 0x5)   // Y16
    ch->s2d_color_bytes = 2;
  else if (ch->s2d_color_fmt == 0x6 || // X8R8G8B8_Z8R8G8B8
           ch->s2d_color_fmt == 0x7 || // X8R8G8B8_X8R8G8B8
           ch->s2d_color_fmt == 0x8 || // X1A7R8G8B8_Z1A7R8G8B8
           ch->s2d_color_fmt == 0x9 || // X1A7R8G8B8_X1A7R8G8B8
           ch->s2d_color_fmt == 0xA || // A8R8G8B8
           ch->s2d_color_fmt == 0xB || // Y32
           ch->s2d_color_fmt == 0xC || // PGRAPH A8R8G8B8
           ch->s2d_color_fmt == 0xD)   // PGRAPH Y32
    ch->s2d_color_bytes = 4;
  else {
    BX_ERROR(("unknown 2d surface color format: 0x%02x",
      ch->s2d_color_fmt));
  }
}

void bx_nvriva_c::update_color_bytes_ifc(nv_channel* ch)
{
  BX_NVRIVA_THIS update_color_bytes(
    ch->s2d_color_fmt, ch->ifc_color_fmt,
    &ch->ifc_color_bytes);
}

void bx_nvriva_c::update_color_bytes_sifc(nv_channel* ch)
{
  BX_NVRIVA_THIS update_color_bytes(
    ch->s2d_color_fmt, ch->sifc_color_fmt,
    &ch->sifc_color_bytes);
}

void bx_nvriva_c::update_color_bytes(Bit32u s2d_color_fmt, Bit32u color_fmt, Bit32u* color_bytes)
{
  if (s2d_color_fmt == 1) // Y8
    *color_bytes = 1; // hack
  else if (color_fmt == 1 || // R5G6B5
           color_fmt == 2 || // A1R5G5B5
           color_fmt == 3)   // X1R5G5B5
    *color_bytes = 2;
  else if (color_fmt == 4 || // A8R8G8B8
           color_fmt == 5)   // X8R8G8B8
    *color_bytes = 4;
  else if (color_fmt == 0) {
    // IFC color format not yet programmed, derive from s2d format
    if (s2d_color_fmt == 0x4 || s2d_color_fmt == 0x2 ||
        s2d_color_fmt == 0x3 || s2d_color_fmt == 0x5)
      *color_bytes = 2;
    else if (s2d_color_fmt >= 0x6)
      *color_bytes = 4;
    else
      *color_bytes = 1;
  } else
    BX_ERROR(("unknown color format: 0x%02x", color_fmt));
}

void bx_nvriva_c::execute_beta(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    ch->beta = param;
}

void bx_nvriva_c::execute_sifm(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    ch->sifm_src = param;
  else if (method == 0x0c0) {
    ch->sifm_color_fmt = param;
    if (ch->sifm_color_fmt == 1 ||       // A1R5G5B5
        ch->sifm_color_fmt == 2 ||       // X1R5G5B5
        ch->sifm_color_fmt == 7)          // R5G6B5
      ch->sifm_color_bytes = 2;
    else if (ch->sifm_color_fmt == 3 ||   // A8R8G8B8
             ch->sifm_color_fmt == 4)     // X8R8G8B8
      ch->sifm_color_bytes = 4;
    else if (ch->sifm_color_fmt == 8)
      ch->sifm_color_bytes = 1;
    else {
      BX_ERROR(("unknown sifm color format: 0x%02x",
        ch->sifm_color_fmt));
    }
  } else if (method == 0x0c1)
    ch->sifm_operation = param;
  else if (method == 0x0c4)
    ch->sifm_dyx = param;
  else if (method == 0x0c5)
    ch->sifm_dhw = param;
  else if (method == 0x0c6)
    ch->sifm_dudx = param;
  else if (method == 0x0c7)
    ch->sifm_dvdy = param;
  else if (method == 0x100)
    ch->sifm_shw = param;
  else if (method == 0x101)
    ch->sifm_sfmt = param;
  else if (method == 0x102)
    ch->sifm_sofs = param;
  else if (method == 0x103) {
    ch->sifm_syx = param;
    sifm(ch);
  }
}

void bx_nvriva_c::sifm(nv_channel* ch)
{
  Bit16u dx = ch->sifm_dyx & 0xFFFF;
  Bit16u dy = ch->sifm_dyx >> 16;
  Bit16u dwidth = ch->sifm_dhw & 0xFFFF;
  Bit16u dheight = ch->sifm_dhw >> 16;
  Bit32u spitch = ch->sifm_sfmt & 0xFFFF;
  Bit32u sifm_src_lin = dma_lin_lookup(ch->sifm_src, ch->sifm_sofs);
  BX_INFO(("sifm: dst=%d,%d %dx%d src_obj=0x%08x sofs=0x%08x src_lin=0x%08x spitch=%d cfmt=%d op=%d",
    dx, dy, dwidth, dheight, ch->sifm_src, ch->sifm_sofs, sifm_src_lin, spitch,
    ch->sifm_color_fmt, ch->sifm_operation));
  // SIFM without scaling is used frequently in some operating systems
  if (ch->sifm_dudx == 0x00100000 && ch->sifm_dvdy == 0x00100000) {
    Bit16u sx = (ch->sifm_syx & 0xFFFF) >> 4;
    Bit16u sy = (ch->sifm_syx >> 16) >> 4;
    Bit32u src_offset = ch->sifm_sofs + sy * spitch + sx * ch->sifm_color_bytes;
    Bit32u dpitch = ch->s2d_pitch_dst;
    Bit32u draw_offset = ch->s2d_ofs_dst +
      dy * dpitch + dx * ch->s2d_color_bytes;
    Bit32u redraw_offset = dma_lin_lookup(ch->s2d_img_dst, draw_offset) -
      BX_NVRIVA_THIS disp_offset;
    if (ch->sifm_operation == 5) {
      Bit32u p0 = get_pixel(ch->sifm_src, src_offset, 0, ch->sifm_color_bytes);
      Bit32u p1 = get_pixel(ch->sifm_src, src_offset, 1, ch->sifm_color_bytes);
      Bit32u p2 = get_pixel(ch->sifm_src, src_offset, dwidth/2, ch->sifm_color_bytes);
      Bit32u p3 = get_pixel(ch->sifm_src, src_offset, dwidth-1, ch->sifm_color_bytes);
      BX_INFO(("sifm_pixels: [0]=0x%08x [1]=0x%08x [%d]=0x%08x [%d]=0x%08x",
        p0, p1, dwidth/2, p2, dwidth-1, p3));
    }
    for (Bit16u y = 0; y < dheight; y++) {
      for (Bit16u x = 0; x < dwidth; x++) {
        Bit32u dstcolor = get_pixel(ch->s2d_img_dst, draw_offset, x, ch->s2d_color_bytes);
        Bit32u srccolor = get_pixel(ch->sifm_src, src_offset, x, ch->sifm_color_bytes);
        if (ch->sifm_color_fmt == 4)
          srccolor |= 0xFF000000;
        pixel_operation(ch, ch->sifm_operation,
          &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
        put_pixel(ch, draw_offset, x, dstcolor);
      }
      src_offset += spitch;
      draw_offset += dpitch;
    }
    BX_NVRIVA_THIS redraw_area_nd(redraw_offset, dwidth, dheight);
  } else {
    Bit32s sx0 = ((ch->sifm_syx & 0xFFFF) << 16) - 0x80000;
    Bit32s sy = (ch->sifm_syx & 0xFFFF0000) - 0x80000;
    if (sx0 < 0)
      sx0 = 0;
    if (sy < 0)
      sy = 0;
    Bit32u dpitch = ch->s2d_pitch_dst;
    Bit32u draw_offset = ch->s2d_ofs_dst +
      dy * dpitch + dx * ch->s2d_color_bytes;
    Bit32u redraw_offset = dma_lin_lookup(ch->s2d_img_dst, draw_offset) -
      BX_NVRIVA_THIS disp_offset;
    for (Bit16u y = 0; y < dheight; y++) {
      Bit32u sx = sx0;
      Bit32u src_offset = ch->sifm_sofs + (sy >> 20) * spitch;
      for (Bit16u x = 0; x < dwidth; x++) {
        Bit32u dstcolor = get_pixel(ch->s2d_img_dst, draw_offset, x, ch->s2d_color_bytes);
        Bit32u srccolor = get_pixel(ch->sifm_src, src_offset, sx >> 20, ch->sifm_color_bytes);
        if (ch->sifm_color_fmt == 4)
          srccolor |= 0xFF000000;
        pixel_operation(ch, ch->sifm_operation,
          &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
        put_pixel(ch, draw_offset, x, dstcolor);
        sx += ch->sifm_dudx;
      }
      sy += ch->sifm_dvdy;
      draw_offset += dpitch;
    }
    BX_NVRIVA_THIS redraw_area_nd(redraw_offset, dwidth, dheight);
  }
}

void bx_nvriva_c::execute_sifc(nv_channel* ch, Bit32u method, Bit32u param)
{
  if (method == 0x0bf)
    ch->sifc_operation = param;
  else if (method == 0x0c0) {
    ch->sifc_color_fmt = param;
    update_color_bytes_sifc(ch);
  } else if (method == 0x0c1)
    ch->sifc_shw = param;
  else if (method == 0x0c2)
    ch->sifc_dxds = param;
  else if (method == 0x0c3)
    ch->sifc_dydt = param;
  else if (method == 0x0c4)
    ch->sifc_clip_yx = param;
  else if (method == 0x0c5)
    ch->sifc_clip_hw = param;
  else if (method == 0x0c6) {
    ch->sifc_syx = param;
    Bit32u width = ch->sifc_shw & 0xFFFF;
    Bit32u height = ch->sifc_shw >> 16;
    Bit32u wordCount = ALIGN(width * height * ch->sifc_color_bytes, 4) >> 2;
    if (ch->sifc_words != nullptr)
      delete[] ch->sifc_words;
    ch->sifc_words_ptr = 0;
    ch->sifc_words_left = wordCount;
    ch->sifc_words = new Bit32u[wordCount];
  }
  else if (method >= 0x100 && method < 0x800) {
    ch->sifc_words[ch->sifc_words_ptr++] = param;
    ch->sifc_words_left--;
    if (!ch->sifc_words_left) {
      sifc(ch);
      delete[] ch->sifc_words;
      ch->sifc_words = nullptr;
    }
  }
}

void bx_nvriva_c::sifc(nv_channel* ch)
{
  Bit16u dx = ch->sifc_clip_yx & 0xFFFF;
  Bit16u dy = ch->sifc_clip_yx >> 16;
  Bit32u dsdx = (Bit32u)(BX_CONST64(1099511627776) / ch->sifc_dxds);
  Bit32u dtdy = (Bit32u)(BX_CONST64(1099511627776) / ch->sifc_dydt);
  Bit32u swidth = ch->sifc_shw & 0xFFFF;
  Bit32u dwidth = ch->sifc_clip_hw & 0xFFFF;
  Bit32u height = ch->sifc_clip_hw >> 16;
  Bit32u pitch = ch->s2d_pitch_dst;
  Bit32u draw_offset = ch->s2d_ofs_dst + dy * pitch + dx * ch->s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(ch->s2d_img_dst, draw_offset) -
    BX_NVRIVA_THIS disp_offset;
  Bit32s sx0 = ((ch->sifc_syx & 0xFFFF) << 16) - (dx << 20) - 0x80000;
  Bit32s sy = (ch->sifc_syx & 0xFFFF0000) - (dy << 20) - 0x80000;
  if (sx0 < 0)
    sx0 = 0;
  if (sy < 0)
    sy = 0;
  Bit32u symbol_offset_y = 0;
  for (Bit16u y = 0; y < height; y++) {
    Bit32u sx = sx0;
    for (Bit16u x = 0; x < dwidth; x++) {
      Bit32u dstcolor = get_pixel(ch->s2d_img_dst, draw_offset, x, ch->s2d_color_bytes);
      Bit32u srccolor;
      Bit32u symbol_offset = symbol_offset_y + (sx >> 20);
      if (ch->sifc_color_bytes == 4) {
        srccolor = ch->sifc_words[symbol_offset];
      } else if (ch->sifc_color_bytes == 2) {
        Bit16u *sifc_words16 = (Bit16u*)ch->sifc_words;
        srccolor = sifc_words16[symbol_offset];
      } else {
        Bit8u *sifc_words8 = (Bit8u*)ch->sifc_words;
        srccolor = sifc_words8[symbol_offset];
      }
      if (ch->sifc_color_bytes == 4 &&
          ch->s2d_color_bytes == 2)
        dstcolor = color_565_to_888(dstcolor);
      pixel_operation(ch, ch->sifc_operation,
        &dstcolor, &srccolor, ch->sifc_color_bytes, dx + x, dy + y);
      if (ch->sifc_color_bytes == 4 &&
          ch->s2d_color_bytes == 2)
        dstcolor = color_888_to_565(dstcolor);
      put_pixel(ch, draw_offset, x, dstcolor);
      sx += dsdx;
    }
    sy += dtdy;
    symbol_offset_y = (sy >> 20) * swidth;
    draw_offset += pitch;
  }
  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, dwidth, height);
}

void bx_nvriva_c::gdi_fillrect(nv_channel* ch, bool clipped)
{
  Bit16s clipx0;
  Bit16s clipy0;
  Bit16s clipx1;
  Bit16s clipy1;
  if (clipped) {
    clipx0 = ch->gdi_clip_yx0 & 0xFFFF;
    clipy0 = ch->gdi_clip_yx0 >> 16;
    clipx1 = ch->gdi_clip_yx1 & 0xFFFF;
    clipy1 = ch->gdi_clip_yx1 >> 16;
  }
  Bit16s dx;
  Bit16s dy;
  if (clipped) {
    dx = ch->gdi_rect_yx0 & 0xFFFF;
    dy = ch->gdi_rect_yx0 >> 16;
    clipx0 -= dx;
    clipy0 -= dy;
    clipx1 -= dx;
    clipy1 -= dy;
  } else {
    dx = ch->gdi_rect_xy >> 16;
    dy = ch->gdi_rect_xy & 0xFFFF;
  }
  Bit16u width;
  Bit16u height;
  if (clipped) {
    width = (ch->gdi_rect_yx1 & 0xFFFF) - dx;
    height = (ch->gdi_rect_yx1 >> 16) - dy;
  } else {
    width = ch->gdi_rect_wh >> 16;
    height = ch->gdi_rect_wh & 0xFFFF;
  }
  Bit32u pitch = ch->s2d_pitch_dst;
  Bit32u srccolor = ch->gdi_rect_color;
  Bit32u draw_offset = ch->s2d_ofs_dst +
    dy * pitch + dx * ch->s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(ch->s2d_img_dst, draw_offset) -
    BX_NVRIVA_THIS disp_offset;
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < width; x++) {
      if (!clipped || (x >= clipx0 && x < clipx1 && y >= clipy0 && y < clipy1)) {
        Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
          draw_offset, x, ch->s2d_color_bytes);
        pixel_operation(ch, ch->gdi_operation,
          &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
        put_pixel(ch, draw_offset, x, dstcolor);
      }
    }
    draw_offset += pitch;
  }
  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, width, height);
}

void bx_nvriva_c::gdi_blit(nv_channel* ch, Bit32u type)
{
  Bit16s dx = ch->gdi_image_xy & 0xFFFF;
  Bit16s dy = ch->gdi_image_xy >> 16;
  Bit16s clipx0 = (ch->gdi_clip_yx0 & 0xFFFF) - dx;
  Bit16s clipy0 = (ch->gdi_clip_yx0 >> 16) - dy;
  Bit16s clipx1 = (ch->gdi_clip_yx1 & 0xFFFF) - dx;
  Bit16s clipy1 = (ch->gdi_clip_yx1 >> 16) - dy;
  BX_INFO(("gdi_blit: type=%d xy=%d,%d fg=0x%08x bg=0x%08x op=%d s2d_dst=0x%08x s2d_ofs=0x%08x pitch=%d",
    type, dx, dy, ch->gdi_fg_color, ch->gdi_bg_color, ch->gdi_operation,
    ch->s2d_img_dst, ch->s2d_ofs_dst, ch->s2d_pitch_dst));
  Bit32u swidth = ch->gdi_image_swh & 0xFFFF;
  Bit32u dwidth = type ? ch->gdi_image_dwh & 0xFFFF : swidth;
  Bit32u height = ch->gdi_image_swh >> 16;
  Bit32u pitch = ch->s2d_pitch_dst;
  Bit32u bg_color = ch->gdi_bg_color;
  Bit32u fg_color = ch->gdi_fg_color;
  if (ch->s2d_color_bytes == 4 && ch->gdi_color_fmt != 3) {
    bg_color = color_565_to_888(bg_color);
    fg_color = color_565_to_888(fg_color);
  }
  Bit32u draw_offset = ch->s2d_ofs_dst +
    dy * pitch + dx * ch->s2d_color_bytes;
  Bit32u gdi_dst_lin = dma_lin_lookup(ch->s2d_img_dst, draw_offset);
  BX_INFO(("gdi_blit: dst_lin=0x%08x draw_ofs=0x%08x cb=%d",
    gdi_dst_lin, draw_offset, ch->s2d_color_bytes));
  Bit32u redraw_offset = gdi_dst_lin - BX_NVRIVA_THIS disp_offset;
  Bit32u bit_index = 0;
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < dwidth; x++) {
      if (x >= clipx0 && x < clipx1 && y >= clipy0 && y < clipy1) {
        Bit32u word_offset = bit_index / 32;
        Bit32u bit_offset = bit_index % 32;
        if (ch->gdi_mono_fmt == 1)
          bit_offset ^= 7;
        bool pixel = (ch->gdi_words[word_offset] >> bit_offset) & 1;
        if (type || (!type && pixel)) {
          Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
            draw_offset, x, ch->s2d_color_bytes);
          Bit32u srccolor = pixel ? fg_color : bg_color;
          pixel_operation(ch, ch->gdi_operation,
            &dstcolor, &srccolor, ch->s2d_color_bytes, dx + x, dy + y);
          put_pixel(ch, draw_offset, x, dstcolor);
        }
      }
      bit_index++;
    }
    bit_index += swidth - dwidth;
    draw_offset += pitch;
  }
  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, dwidth, height);
}

void bx_nvriva_c::gdi_font_render(nv_channel* ch, Bit16s dx, Bit16s dy, Bit32u glyph_index)
{
  Bit32u glyph_stride = 1 << ch->gdi_font_pitch;
  Bit32u glyph_addr = ch->gdi_font_offset + glyph_index * glyph_stride;

  // Read size header: width (low 16), height (high 16)
  Bit32u size_word = dma_read32(ch->gdi_dma_fonts, glyph_addr);
  Bit32u glyph_width = size_word & 0xFFFF;
  Bit32u glyph_height = size_word >> 16;

  if (glyph_width == 0 || glyph_height == 0)
    return;

  // Clip in glyph-local coordinates
  Bit16s clipx0 = (Bit16s)(ch->gdi_font_clip_yx0 & 0xFFFF) - dx;
  Bit16s clipy0 = (Bit16s)(ch->gdi_font_clip_yx0 >> 16) - dy;
  Bit16s clipx1 = (Bit16s)(ch->gdi_font_clip_yx1 & 0xFFFF) - dx;
  Bit16s clipy1 = (Bit16s)(ch->gdi_font_clip_yx1 >> 16) - dy;

  Bit32u fg_color = ch->gdi_font_color;
  if (ch->s2d_color_bytes == 4 && ch->gdi_color_fmt != 3)
    fg_color = color_565_to_888(fg_color);

  Bit32u pitch = ch->s2d_pitch_dst;
  Bit32u draw_offset = ch->s2d_ofs_dst +
    dy * pitch + dx * ch->s2d_color_bytes;
  Bit32u gdi_dst_lin = dma_lin_lookup(ch->s2d_img_dst, draw_offset);
  Bit32u redraw_offset = gdi_dst_lin - BX_NVRIVA_THIS disp_offset;

  // Monochrome data starts at glyph_addr + 4
  Bit32u mono_base = glyph_addr + 4;
  Bit32u bit_index = 0;
  Bit32u cached_word = 0;
  Bit32u cached_word_index = 0xFFFFFFFF;

  for (Bit32u y = 0; y < glyph_height; y++) {
    for (Bit32u x = 0; x < glyph_width; x++) {
      if ((Bit16s)x >= clipx0 && (Bit16s)x < clipx1 &&
          (Bit16s)y >= clipy0 && (Bit16s)y < clipy1) {
        Bit32u word_index = bit_index / 32;
        Bit32u bit_offset = bit_index % 32;
        if (ch->gdi_mono_fmt == 1)
          bit_offset ^= 7;
        if (word_index != cached_word_index) {
          cached_word = dma_read32(ch->gdi_dma_fonts, mono_base + word_index * 4);
          cached_word_index = word_index;
        }
        bool pixel = (cached_word >> bit_offset) & 1;
        if (pixel) {
          Bit32u dstcolor = get_pixel(ch->s2d_img_dst,
            draw_offset, x, ch->s2d_color_bytes);
          pixel_operation(ch, ch->gdi_operation,
            &dstcolor, &fg_color, ch->s2d_color_bytes, dx + x, dy + y);
          put_pixel(ch, draw_offset, x, dstcolor);
        }
      }
      bit_index++;
    }
    draw_offset += pitch;
  }

  BX_NVRIVA_THIS redraw_area_nd(redraw_offset, glyph_width, glyph_height);
}

void bx_nvriva_c::execute_gdi(nv_channel* ch, Bit32u cls, Bit32u method, Bit32u param)
{
  if (method == 0x061) {
    ch->gdi_dma_fonts = param;
    return;
  } else if (method == 0x0bf)
    ch->gdi_operation = param;
  else if (method == 0x0c0)
    ch->gdi_color_fmt = param;
  else if (method == 0x0c1)
    ch->gdi_mono_fmt = param;
  else if (method == 0x0ff)
    ch->gdi_rect_color = param;
  else if (method >= 0x100 && method < 0x140) {
    if (method & 1) {
      ch->gdi_rect_wh = param;
      gdi_fillrect(ch, false);
    } else
      ch->gdi_rect_xy = param;
  } else if (method == 0x17d)
    ch->gdi_clip_yx0 = param;
  else if (method == 0x17e)
    ch->gdi_clip_yx1 = param;
  else if (method == 0x17f)
    ch->gdi_rect_color = param;
  else if (method >= 0x180 && method < 0x1c0) {
    if (method & 1) {
      ch->gdi_rect_yx1 = param;
      gdi_fillrect(ch, true);
    } else
      ch->gdi_rect_yx0 = param;
  } else if ((method == 0x1fb && cls == 0x004a) ||
             (method == 0x2fb && cls == 0x004b))
    ch->gdi_clip_yx0 = param;
  else if ((method == 0x1fc && cls == 0x004a) ||
           (method == 0x2fc && cls == 0x004b))
    ch->gdi_clip_yx1 = param;
  else if ((method == 0x1fd && cls == 0x004a) ||
           (method == 0x2fd && cls == 0x004b))
    ch->gdi_fg_color = param;
  else if ((method == 0x1fe && cls == 0x004a) ||
           (method == 0x2fe && cls == 0x004b))
    ch->gdi_image_swh = param;
  else if ((method == 0x1ff && cls == 0x004a) ||
           (method == 0x2ff && cls == 0x004b)) {
    ch->gdi_image_xy = param;
    Bit32u width = ch->gdi_image_swh & 0xFFFF;
    Bit32u height = ch->gdi_image_swh >> 16;
    Bit32u wordCount = ALIGN(width * height, 32) >> 5;
    if (ch->gdi_words != nullptr)
      delete[] ch->gdi_words;
    ch->gdi_words_ptr = 0;
    ch->gdi_words_left = wordCount;
    ch->gdi_words = new Bit32u[wordCount];
  } else if ((method >= 0x200 && method < 0x280 && cls == 0x004a) ||
             (method >= 0x300 && method < 0x380 && cls == 0x004b)) {
    ch->gdi_words[ch->gdi_words_ptr++] = param;
    ch->gdi_words_left--;
    if (!ch->gdi_words_left) {
      gdi_blit(ch, 0);
      delete[] ch->gdi_words;
      ch->gdi_words = nullptr;
    }
  } else if ((method == 0x2f9 && cls == 0x004a) ||
             (method == 0x4f9 && cls == 0x004b))
    ch->gdi_clip_yx0 = param;
  else if ((method == 0x2fa && cls == 0x004a) ||
           (method == 0x4fa && cls == 0x004b))
    ch->gdi_clip_yx1 = param;
  else if ((method == 0x2fb && cls == 0x004a) ||
           (method == 0x4fb && cls == 0x004b))
    ch->gdi_bg_color = param;
  else if ((method == 0x2fc && cls == 0x004a) ||
           (method == 0x4fc && cls == 0x004b))
    ch->gdi_fg_color = param;
  else if ((method == 0x2fd && cls == 0x004a) ||
           (method == 0x4fd && cls == 0x004b))
    ch->gdi_image_swh = param;
  else if ((method == 0x2fe && cls == 0x004a) ||
           (method == 0x4fe && cls == 0x004b))
    ch->gdi_image_dwh = param;
  else if ((method == 0x2ff && cls == 0x004a) ||
           (method == 0x4ff && cls == 0x004b)) {
    ch->gdi_image_xy = param;
    Bit32u width = ch->gdi_image_swh & 0xFFFF;
    Bit32u height = ch->gdi_image_swh >> 16;
    Bit32u wordCount = ALIGN(width * height, 32) >> 5;
    if (ch->gdi_words != nullptr)
      delete[] ch->gdi_words;
    ch->gdi_words_ptr = 0;
    ch->gdi_words_left = wordCount;
    ch->gdi_words = new Bit32u[wordCount];
  } else if ((method >= 0x300 && method < 0x380 && cls == 0x004a) ||
             (method >= 0x500 && method < 0x580 && cls == 0x004b)) {
    ch->gdi_words[ch->gdi_words_ptr++] = param;
    ch->gdi_words_left--;
    if (!ch->gdi_words_left) {
      gdi_blit(ch, 1);
      delete[] ch->gdi_words;
      ch->gdi_words = nullptr;
    }
  } else if (method == 0x3fd && cls == 0x004b)
    ch->gdi_clip_yx0 = param;
  else if (method == 0x3fe && cls == 0x004b)
    ch->gdi_clip_yx1 = param;
  else if (method == 0x3ff && cls == 0x004b)
    ch->gdi_fg_color = param;
  // Font F (NV4_GDI class 0x4a only)
  else if (method == 0x3fc && cls == 0x004a) {
    ch->gdi_font_offset = param & 0x0FFFFFFF;
    ch->gdi_font_pitch = (param >> 28) & 0xF;
  } else if (method == 0x3fd && cls == 0x004a)
    ch->gdi_font_clip_yx0 = param;
  else if (method == 0x3fe && cls == 0x004a)
    ch->gdi_font_clip_yx1 = param;
  else if (method == 0x3ff && cls == 0x004a)
    ch->gdi_font_color = param;
  else if (method >= 0x400 && method < 0x500 && cls == 0x004a) {
    Bit32u index = param & 0xFF;
    Bit16s cx = (Bit32s)(param << 12) >> 20;
    Bit16s cy = (Bit32s)param >> 20;
    gdi_font_render(ch, cx, cy, index);
  }
  // Font G (NV4_GDI class 0x4a only)
  else if (method == 0x5fc && cls == 0x004a) {
    ch->gdi_font_offset = param & 0x0FFFFFFF;
    ch->gdi_font_pitch = (param >> 28) & 0xF;
  } else if (method == 0x5fd && cls == 0x004a)
    ch->gdi_font_clip_yx0 = param;
  else if (method == 0x5fe && cls == 0x004a)
    ch->gdi_font_clip_yx1 = param;
  else if (method == 0x5ff && cls == 0x004a)
    ch->gdi_font_color = param;
  else if (method >= 0x600 && method < 0x800 && cls == 0x004a) {
    if (method & 1) {
      Bit16s gx = ch->gdi_font_g_xy & 0xFFFF;
      Bit16s gy = ch->gdi_font_g_xy >> 16;
      gdi_font_render(ch, gx, gy, param & 0xFFFF);
    } else
      ch->gdi_font_g_xy = param;
  }
}

// FIFO command execution

int bx_nvriva_c::execute_command(Bit32u chid, Bit32u subc, Bit32u method, Bit32u param)
{
  int result = 0;
  bool software_method = false;
  BX_DEBUG(("execute_command: chid 0x%02x, subc 0x%02x, method 0x%03x, param 0x%08x",
    chid, subc, method, param));
  nv_channel* ch = &BX_NVRIVA_THIS chs[chid];
  if (method == 0x000) {
    if (ch->schs[subc].engine == 0x01) {
      Bit32u word1 = ramin_read32(ch->schs[subc].object + 0x4);
      // NV4/NV5 context format
      word1 = (word1 & 0x0000FFFF) | (ch->schs[subc].notifier >> 4 << 16);
      Bit32u word0 = ramin_read32(ch->schs[subc].object);
      Bit8u cls8 = word0;
      if (cls8 == 0x4a || cls8 == 0x4b) {
        word0 = (word0 & 0xFFFC7FFF) | (ch->gdi_operation << 15);
        word1 = (word1 & 0xFFFFFFFC) | ch->gdi_mono_fmt;
        ramin_write32(ch->schs[subc].object, word0);
      } else if (cls8 == 0x62) {
        ramin_write32(ch->schs[subc].object + 0x8,
          (ch->s2d_img_src >> 4) |
          (ch->s2d_img_dst >> 4 << 16));
      }
      ramin_write32(ch->schs[subc].object + 0x4, word1);
    }
    ramht_lookup(param, chid,
      &ch->schs[subc].object,
      &ch->schs[subc].engine);
    if (ch->schs[subc].engine == 0x01) {
      Bit32u word1 = ramin_read32(ch->schs[subc].object + 0x4);
      ch->schs[subc].notifier = word1 >> 16 << 4;
      Bit32u word0 = ramin_read32(ch->schs[subc].object);
      Bit8u cls8 = word0;
      if (cls8 == 0x48 || cls8 == 0x42) {
        // Hack: restore s2d from context
        if (!ch->s2d_locked) {
          Bit32u srcdst = ramin_read32(ch->schs[subc].object + 0x8);
          ch->s2d_img_src = (srcdst & 0xFFFF) << 4;
          ch->s2d_img_dst = srcdst >> 16 << 4;
          // Convert PGRAPH SURF_FORMAT to object s2d format
          Bit32u bpixel = BX_NVRIVA_THIS graph_bpixel & 0xf;
          Bit32u s2d_fmt;
          if (bpixel == 0x1)
            s2d_fmt = 0x1; // Y8
          else if (bpixel >= 0x2 && bpixel <= 0x4)
            s2d_fmt = 0x2; // X1R5G5B5
          else if (bpixel == 0x5)
            s2d_fmt = 0x4; // R5G6B5
          else if (bpixel == 0x6)
            s2d_fmt = 0x5; // Y16
          else if (bpixel >= 0x7 && bpixel <= 0xb)
            s2d_fmt = 0x7; // X8R8G8B8
          else if (bpixel >= 0xc && bpixel <= 0xd)
            s2d_fmt = 0x7; // X8R8G8B8 (PGRAPH A8R8G8B8/Y32)
          else {
            // Derive from display bpp (CR28) as fallback
            Bit8u crtc28 = BX_NVRIVA_THIS crtc.reg[0x28] & 0x7F;
            if (crtc28 == 0x03)
              s2d_fmt = 0x7; // X8R8G8B8
            else if (crtc28 == 0x02)
              s2d_fmt = 0x4; // R5G6B5
            else
              s2d_fmt = 0x1; // Y8
          }
          ch->s2d_color_fmt = s2d_fmt;
          update_color_bytes_s2d(ch);
          ch->s2d_pitch_src = BX_NVRIVA_THIS graph_pitch0 & 0xffff;
          ch->s2d_pitch_dst = ch->s2d_pitch_src;
          ch->s2d_ofs_src = BX_NVRIVA_THIS graph_offset0;
          ch->s2d_ofs_dst = BX_NVRIVA_THIS graph_offset0;
        }
      } else if (cls8 == 0x4a || cls8 == 0x4b) {
        ch->gdi_operation = (word0 >> 15) & 7;
        ch->gdi_mono_fmt = word1 & 3;
      } else if (cls8 == 0x62) {
        Bit32u srcdst = ramin_read32(ch->schs[subc].object + 0x8);
        ch->s2d_img_src = (srcdst & 0xFFFF) << 4;
        ch->s2d_img_dst = srcdst >> 16 << 4;
      }
    } else if (ch->schs[subc].engine == 0x00) {
      software_method = true;
    } else {
      BX_DEBUG(("execute_command: unknown engine %d", ch->schs[subc].engine));
    }
  } else if (method == 0x014) {
    BX_NVRIVA_THIS fifo_cache1_ref_cnt = param;
  } else if (method >= 0x040) {
    if (ch->schs[subc].engine == 0x01) {
      if (method >= 0x060 && method < 0x080)
        ramht_lookup(param, chid, &param, nullptr);
      Bit32u cls = ramin_read32(
        ch->schs[subc].object) & 0x00000FFF;
      Bit8u cls8 = cls;
      BX_DEBUG(("execute_command: obj 0x%08x, class 0x%04x, method 0x%03x, param 0x%08x",
        ch->schs[subc].object, cls, method, param));
      // NV4/NV5 class objects only
      switch (cls8) {
        case 0x19:
          execute_clip(ch, method, param);
          break;
        case 0x39:
          execute_m2mf(ch, subc, method, param);
          break;
        case 0x42: // NV4 surface 2D
          execute_surf2d(ch, method, param);
          break;
        case 0x43:
          execute_rop(ch, method, param);
          break;
        case 0x44:
        case 0x18:
          execute_patt(ch, method, param);
          break;
        case 0x57:
          execute_chroma(ch, method, param);
          break;
        case 0x4a:
        case 0x4b:
          execute_gdi(ch, cls, method, param);
          break;
        case 0x5e:
          execute_rect(ch, method, param);
          break;
        case 0x5f:
          execute_imageblit(ch, method, param);
          break;
        case 0x65: // NV5_IFC
        case 0x61:
        case 0x21:
          execute_ifc(ch, method, param);
          break;
        case 0x62:
          execute_surf2d(ch, method, param);
          break;
        case 0x72:
          execute_beta(ch, method, param);
          break;
        case 0x37: // NV3_SIFM
        case 0x77: // NV4_SIFM
          execute_sifm(ch, method, param);
          break;
        case 0x76: // NV4_SIFC
        case 0x66: // NV5_SIFC
          execute_sifc(ch, method, param);
          break;
        default:
          BX_INFO(("execute_command: UNHANDLED class 0x%02x, method 0x%03x, param 0x%08x",
            cls8, method, param));
          break;
      }
      if (ch->notify_pending) {
        ch->notify_pending = false;
        if ((ramin_read32(ch->schs[subc].notifier) & 0xFF) == 0x30) {
          BX_DEBUG(("execute_command: DMA notify skipped"));
        } else {
          BX_DEBUG(("execute_command: DMA notify 0x%08x",
            ch->schs[subc].notifier));
          dma_write64(ch->schs[subc].notifier, 0x0, get_current_time());
          dma_write32(ch->schs[subc].notifier, 0x8, 0);
          dma_write32(ch->schs[subc].notifier, 0xC, 0);
        }
        if (ch->notify_type) {
          BX_NVRIVA_THIS graph_intr |= 0x00000001;
          update_irq_level();
          BX_NVRIVA_THIS graph_nsource |= 0x00000001;
          BX_NVRIVA_THIS graph_notify = 0x00110000;
          Bit32u notifier = ch->schs[subc].notifier >> 4;
          BX_NVRIVA_THIS graph_ctx_switch2 = notifier << 16;
          BX_NVRIVA_THIS graph_ctx_switch4 = ch->schs[subc].object >> 4;
          BX_NVRIVA_THIS graph_trapped_addr = (method << 2) | (subc << 16) | (chid << 20);
          BX_NVRIVA_THIS graph_trapped_data = param;
          BX_NVRIVA_THIS fifo_wait_notify = true;
          BX_NVRIVA_THIS fifo_wait = true;
          BX_DEBUG(("execute_command: notify interrupt triggered"));
        }
      }
      if (method == 0x041) {
        ch->notify_pending = true;
        ch->notify_type = param;
      } else if (method == 0x060)
        ch->schs[subc].notifier = param;
    } else if (ch->schs[subc].engine == 0x00) {
      software_method = true;
    } else {
      BX_DEBUG(("execute_command: unknown engine %d", ch->schs[subc].engine));
    }
  }
  if (software_method) {
    BX_NVRIVA_THIS fifo_wait_soft = true;
    BX_NVRIVA_THIS fifo_wait = true;
    BX_NVRIVA_THIS fifo_intr |= 0x00000001;
    update_irq_level();
    BX_NVRIVA_THIS fifo_cache1_pull0 |= 0x00000100;
    BX_NVRIVA_THIS fifo_cache1_method[BX_NVRIVA_THIS fifo_cache1_put / 4] = (method << 2) | (subc << 13);
    BX_NVRIVA_THIS fifo_cache1_data[BX_NVRIVA_THIS fifo_cache1_put / 4] = param;
    BX_NVRIVA_THIS fifo_cache1_put += 4;
    if (BX_NVRIVA_THIS fifo_cache1_put == NVRIVA_CACHE1_SIZE * 4)
      BX_NVRIVA_THIS fifo_cache1_put = 0;
    BX_DEBUG(("execute_command: software method"));
    result = 1;
  }
  return result;
}

// FIFO processing

void bx_nvriva_c::update_fifo_wait()
{
  BX_NVRIVA_THIS fifo_wait =
    BX_NVRIVA_THIS fifo_wait_soft ||
    BX_NVRIVA_THIS fifo_wait_notify ||
    BX_NVRIVA_THIS fifo_wait_flip ||
    BX_NVRIVA_THIS fifo_wait_acquire;
}

void bx_nvriva_c::fifo_process()
{
  Bit32u offset = (BX_NVRIVA_THIS fifo_cache1_push1 & 0xf) + 1;
  for (Bit32u i = 0; i < NVRIVA_CHANNEL_COUNT; i++)
    fifo_process((i + offset) & 0xf);
}

void bx_nvriva_c::fifo_process(Bit32u chid)
{
  if (BX_NVRIVA_THIS fifo_wait)
    return;
  if ((BX_NVRIVA_THIS fifo_mode & (1 << chid)) == 0)
    return;
  if ((BX_NVRIVA_THIS fifo_cache1_push0 & 1) == 0)
    return;
  if ((BX_NVRIVA_THIS fifo_cache1_pull0 & 1) == 0)
    return;
  Bit32u oldchid = BX_NVRIVA_THIS fifo_cache1_push1 & 0xF;
  if (oldchid == chid) {
    if (BX_NVRIVA_THIS fifo_cache1_dma_put == BX_NVRIVA_THIS fifo_cache1_dma_get)
      return;
  } else {
    if (ramfc_read32(chid, 0x0) == ramfc_read32(chid, 0x4))
      return;
  }
  if (oldchid != chid) {
    // NV4/NV5: save/restore 4 RAMFC words (0x20 byte channels)
    ramfc_write32(oldchid, 0x0, BX_NVRIVA_THIS fifo_cache1_dma_put);
    ramfc_write32(oldchid, 0x4, BX_NVRIVA_THIS fifo_cache1_dma_get);
    ramfc_write32(oldchid, 0x8, BX_NVRIVA_THIS fifo_cache1_ref_cnt);
    ramfc_write32(oldchid, 0xC, BX_NVRIVA_THIS fifo_cache1_dma_instance);
    BX_NVRIVA_THIS fifo_cache1_dma_put = ramfc_read32(chid, 0x0);
    BX_NVRIVA_THIS fifo_cache1_dma_get = ramfc_read32(chid, 0x4);
    BX_NVRIVA_THIS fifo_cache1_ref_cnt = ramfc_read32(chid, 0x8);
    BX_NVRIVA_THIS fifo_cache1_dma_instance = ramfc_read32(chid, 0xC);
    BX_NVRIVA_THIS fifo_cache1_push1 = (BX_NVRIVA_THIS fifo_cache1_push1 & ~0xF) | chid;
  }
  BX_NVRIVA_THIS fifo_cache1_dma_push |= 0x100;
  if (BX_NVRIVA_THIS fifo_cache1_dma_instance == 0) {
    BX_DEBUG(("fifo: DMA instance = 0, skipping"));
    return;
  }
  nv_channel* ch = &BX_NVRIVA_THIS chs[chid];
  while (BX_NVRIVA_THIS fifo_cache1_dma_get != BX_NVRIVA_THIS fifo_cache1_dma_put) {
    BX_DEBUG(("fifo: processing at 0x%08x", BX_NVRIVA_THIS fifo_cache1_dma_get));
    Bit32u word = dma_read32(
      BX_NVRIVA_THIS fifo_cache1_dma_instance << 4,
      BX_NVRIVA_THIS fifo_cache1_dma_get);
    BX_NVRIVA_THIS fifo_cache1_dma_get += 4;
    if (ch->dma_state.mcnt) {
      int cmd_result = execute_command(chid,
        ch->dma_state.subc, ch->dma_state.mthd, word);
      if (cmd_result <= 1) {
        if (!ch->dma_state.ni)
          ch->dma_state.mthd++;
        ch->dma_state.mcnt--;
      } else {
        BX_NVRIVA_THIS fifo_cache1_dma_get -= 4;
      }
      if (cmd_result != 0)
        break;
    } else {
      if ((word & 0xe0000003) == 0x20000000) {
        // old jump
        BX_NVRIVA_THIS fifo_cache1_dma_get = word & 0x1fffffff;
        BX_DEBUG(("fifo: jump to 0x%08x", BX_NVRIVA_THIS fifo_cache1_dma_get));
      } else if ((word & 3) == 1) {
        // jump
        BX_NVRIVA_THIS fifo_cache1_dma_get = word & 0xfffffffc;
        BX_DEBUG(("fifo: jump to 0x%08x", BX_NVRIVA_THIS fifo_cache1_dma_get));
      } else if ((word & 3) == 2) {
        // call
        if (ch->subr_active)
          BX_PANIC(("fifo: call with subroutine active"));
        ch->subr_return = BX_NVRIVA_THIS fifo_cache1_dma_get;
        ch->subr_active = true;
        BX_NVRIVA_THIS fifo_cache1_dma_get = word & 0xfffffffc;
        BX_DEBUG(("fifo: call 0x%08x", BX_NVRIVA_THIS fifo_cache1_dma_get));
      } else if (word == 0x00020000) {
        // return
        if (!ch->subr_active)
          BX_PANIC(("fifo: return with subroutine inactive"));
        BX_DEBUG(("fifo: return to 0x%08x", ch->subr_return));
        BX_NVRIVA_THIS fifo_cache1_dma_get = ch->subr_return;
        ch->subr_active = false;
      } else if ((word & 0xa0030003) == 0) {
        ch->dma_state.mthd = (word >> 2) & 0x7ff;
        ch->dma_state.subc = (word >> 13) & 7;
        ch->dma_state.mcnt = (word >> 18) & 0x7ff;
        ch->dma_state.ni = word & 0x40000000;
      } else {
        BX_PANIC(("fifo: unexpected word 0x%08x", word));
      }
    }
  }
}

// Timing and IRQ

Bit64u bx_nvriva_c::get_current_time()
{
  return (BX_NVRIVA_THIS timer_inittime1 +
    bx_pc_system.time_nsec() - BX_NVRIVA_THIS timer_inittime2) & ~BX_CONST64(0x1F);
}

void bx_nvriva_c::set_irq_level(bool level)
{
  DEV_pci_set_irq(BX_NVRIVA_THIS devfunc, BX_NVRIVA_THIS pci_conf[0x3d], level);
}

Bit32u bx_nvriva_c::get_mc_intr()
{
  Bit32u value = 0x00000000;
  if (BX_NVRIVA_THIS bus_intr & BX_NVRIVA_THIS bus_intr_en)
    value |= 0x10000000;
  if (BX_NVRIVA_THIS fifo_intr & BX_NVRIVA_THIS fifo_intr_en)
    value |= 0x00000100;
  if (BX_NVRIVA_THIS graph_intr & BX_NVRIVA_THIS graph_intr_en)
    value |= 0x00001000;
  if (BX_NVRIVA_THIS crtc_intr & BX_NVRIVA_THIS crtc_intr_en)
    value |= 0x01000000;
  return value;
}

void bx_nvriva_c::update_irq_level()
{
  set_irq_level((get_mc_intr() && BX_NVRIVA_THIS mc_intr_en & 1) ||
    (BX_NVRIVA_THIS mc_soft_intr && BX_NVRIVA_THIS mc_intr_en & 2));
}

// MMIO register read (32-bit)

Bit32u bx_nvriva_c::register_read32(Bit32u address)
{
  Bit32u value;

  if (address == 0x0) {
    // PMC_BOOT_0: NV5 identification
    value = 0x20154000;
  } else if (address == 0x100) {
    value = get_mc_intr();
    if (BX_NVRIVA_THIS mc_soft_intr)
      value |= 0x80000000;
  } else if (address == 0x140)
    value = BX_NVRIVA_THIS mc_intr_en;
  else if (address == 0x200)
    value = BX_NVRIVA_THIS mc_enable;
  else if (address == 0x1100)
    value = BX_NVRIVA_THIS bus_intr;
  else if (address == 0x1140)
    value = BX_NVRIVA_THIS bus_intr_en;
  else if (address >= 0x1800 && address < 0x1900) {
    Bit32u offset = address - 0x1800;
    value =
      (BX_NVRIVA_THIS pci_conf[offset + 0] << 0) |
      (BX_NVRIVA_THIS pci_conf[offset + 1] << 8) |
      (BX_NVRIVA_THIS pci_conf[offset + 2] << 16) |
      (BX_NVRIVA_THIS pci_conf[offset + 3] << 24);
  } else if (address == 0x2100) {
    value = BX_NVRIVA_THIS fifo_intr;
  } else if (address == 0x2140) {
    value = BX_NVRIVA_THIS fifo_intr_en;
  } else if (address == 0x2210) {
    value = BX_NVRIVA_THIS fifo_ramht;
  } else if (address == 0x2214) {
    value = BX_NVRIVA_THIS fifo_ramfc;
  } else if (address == 0x2218) {
    value = BX_NVRIVA_THIS fifo_ramro;
  } else if (address == 0x2400) { // PFIFO_RUNOUT_STATUS
    value = 0x00000010;
    if (BX_NVRIVA_THIS fifo_cache1_get != BX_NVRIVA_THIS fifo_cache1_put)
      value = 0x00000000;
  } else if (address == 0x2504) {
    value = BX_NVRIVA_THIS fifo_mode;
  } else if (address == 0x3200) {
    value = BX_NVRIVA_THIS fifo_cache1_push0;
  } else if (address == 0x3204) {
    value = BX_NVRIVA_THIS fifo_cache1_push1;
  } else if (address == 0x3210) {
    value = BX_NVRIVA_THIS fifo_cache1_put;
  } else if (address == 0x3214) { // PFIFO_CACHE1_STATUS
    value = 0x00000010;
    if (BX_NVRIVA_THIS fifo_cache1_get != BX_NVRIVA_THIS fifo_cache1_put)
      value = 0x00000000;
  } else if (address == 0x3220) {
    value = BX_NVRIVA_THIS fifo_cache1_dma_push;
  } else if (address == 0x322c) {
    value = BX_NVRIVA_THIS fifo_cache1_dma_instance;
  } else if (address == 0x3230) { // PFIFO_CACHE1_DMA_CTL
    value = 0x80000000;
  } else if (address == 0x3240) {
    value = BX_NVRIVA_THIS fifo_cache1_dma_put;
  } else if (address == 0x3244) {
    value = BX_NVRIVA_THIS fifo_cache1_dma_get;
  } else if (address == 0x3248) {
    value = BX_NVRIVA_THIS fifo_cache1_ref_cnt;
  } else if (address == 0x3250) {
    if (BX_NVRIVA_THIS fifo_cache1_get != BX_NVRIVA_THIS fifo_cache1_put)
      BX_NVRIVA_THIS fifo_cache1_pull0 |= 0x00000100;
    value = BX_NVRIVA_THIS fifo_cache1_pull0;
  } else if (address == 0x3270) {
    value = BX_NVRIVA_THIS fifo_cache1_get;
  } else if (address == 0x3304) {
    value = 0x00000001;
  } else if (address >= 0x3800 && address < 0x4000) {
    // NV4 CACHE1 method/data
    Bit32u offset = address - 0x3800;
    Bit32u index = offset / 8;
    if (offset % 8 == 0)
      value = BX_NVRIVA_THIS fifo_cache1_method[index];
    else
      value = BX_NVRIVA_THIS fifo_cache1_data[index];
  } else if (address == 0x9100) {
    value = BX_NVRIVA_THIS timer_intr;
  } else if (address == 0x9140) {
    value = BX_NVRIVA_THIS timer_intr_en;
  } else if (address == 0x9200)
    value = BX_NVRIVA_THIS timer_num;
  else if (address == 0x9210)
    value = BX_NVRIVA_THIS timer_den;
  else if (address == 0x9400)
    value = (Bit32u)get_current_time();
  else if (address == 0x9410)
    value = get_current_time() >> 32;
  else if (address == 0x9420)
    value = BX_NVRIVA_THIS timer_alarm;
  else if (address >= 0xc0300 && address < 0xc0400)
    value = register_read8(address);
  else if (address == 0x100000) {
    // PFB_BOOT_0: NV4/NV5 VRAM config
    // Bit[1:0] = RAM_AMOUNT: 0=32MB, 1=4MB, 2=8MB, 3=16MB
    // Bit[2]   = RAM_WIDTH_128: 0=64-bit, 1=128-bit
    // TNT2 M64 = 32MB SDRAM, 64-bit bus
    value = 0x00000000;
  } else if (address == 0x10020c) {
    // PFB memory size
    value = BX_NVRIVA_THIS s.memsize;
  } else if (address == 0x101000)
    value = BX_NVRIVA_THIS straps0_primary;
  else if (address >= 0x300000 && address < 0x310000) {
    Bit32u offset = address - 0x300000;
    if (BX_NVRIVA_THIS pci_conf[0x50] == 0x00) {
      value =
        (BX_NVRIVA_THIS pci_rom[offset + 0] << 0) |
        (BX_NVRIVA_THIS pci_rom[offset + 1] << 8) |
        (BX_NVRIVA_THIS pci_rom[offset + 2] << 16) |
        (BX_NVRIVA_THIS pci_rom[offset + 3] << 24);
    } else {
      value = 0x00000000;
    }
  } else if (address == 0x400100) {
    value = BX_NVRIVA_THIS graph_intr;
  } else if (address == 0x400108) {
    value = BX_NVRIVA_THIS graph_nsource;
  } else if (address == 0x400140) {
    // NV4/NV5: graph_intr_en at 0x400140
    value = BX_NVRIVA_THIS graph_intr_en;
  } else if (address == 0x40014C) {
    value = BX_NVRIVA_THIS graph_ctx_switch1;
  } else if (address == 0x400150) {
    value = BX_NVRIVA_THIS graph_ctx_switch2;
  } else if (address == 0x400158) {
    value = BX_NVRIVA_THIS graph_ctx_switch4;
  } else if (address == 0x400700) {
    value = BX_NVRIVA_THIS graph_status;
  } else if (address == 0x400704) {
    value = BX_NVRIVA_THIS graph_trapped_addr;
  } else if (address == 0x400708) {
    value = BX_NVRIVA_THIS graph_trapped_data;
  } else if (address == 0x400718) {
    value = BX_NVRIVA_THIS graph_notify;
  } else if (address == 0x400720) {
    value = BX_NVRIVA_THIS graph_fifo;
  } else if (address == 0x400724) {
    value = BX_NVRIVA_THIS graph_bpixel;
  } else if (address == 0x400780) {
    value = BX_NVRIVA_THIS graph_channel_ctx_table;
  } else if (address == 0x400640) {
    // NV4/NV5: offset at 0x400640
    value = BX_NVRIVA_THIS graph_offset0;
  } else if (address == 0x400670) {
    // NV4/NV5: pitch at 0x400670
    value = BX_NVRIVA_THIS graph_pitch0;
  } else if (address == 0x600100) {
    value = BX_NVRIVA_THIS crtc_intr;
  } else if (address == 0x600140) {
    value = BX_NVRIVA_THIS crtc_intr_en;
  } else if (address == 0x600800) {
    value = BX_NVRIVA_THIS crtc_start;
  } else if (address == 0x600804) {
    value = BX_NVRIVA_THIS crtc_config;
  } else if (address == 0x600808) {
    BX_NVRIVA_THIS crtc_raster_pos ^= 1; // fake
    value = (VGA_READ(0x03da, 1) << 13) | BX_NVRIVA_THIS crtc_raster_pos;
  } else if (address == 0x60080c) {
    value = BX_NVRIVA_THIS crtc_cursor_offset;
  } else if (address == 0x600810) {
    value = BX_NVRIVA_THIS crtc_cursor_config;
  } else if (address == 0x60081c) {
    value = BX_NVRIVA_THIS crtc_gpio_ext;
  } else if (address == 0x600868) {
    Bit64u display_usec =
      bx_virt_timer.time_usec(BX_NVRIVA_THIS vsync_realtime) - BX_NVRIVA_THIS s.display_start_usec;
    display_usec = display_usec % BX_NVRIVA_THIS s.vtotal_usec;
    value = (Bit32u)(BX_NVRIVA_THIS get_crtc_vtotal() * display_usec / BX_NVRIVA_THIS s.vtotal_usec);
  } else if (address >= 0x601300 && address < 0x601400) {
    value = register_read8(address);
  } else if (address == 0x680300) {
    value = BX_NVRIVA_THIS ramdac_cu_start_pos;
  } else if (address == 0x680508) {
    value = BX_NVRIVA_THIS ramdac_vpll;
  } else if (address == 0x68050c) {
    value = BX_NVRIVA_THIS ramdac_pll_select;
  } else if (address == 0x680600) {
    value = BX_NVRIVA_THIS ramdac_general_control;
  } else if (address >= 0x681300 && address < 0x681400) {
    value = register_read8(address);
  } else if (address >= 0x700000 && address < 0x800000) {
    // NV4/NV5: direct RAMIN at BAR0+0x700000
    Bit32u offset = address & 0x000fffff;
    if (offset & 3) {
      value =
        ramin_read8(offset + 0) << 0 |
        ramin_read8(offset + 1) << 8 |
        ramin_read8(offset + 2) << 16 |
        ramin_read8(offset + 3) << 24;
    } else {
      value = ramin_read32(offset);
    }
  } else if (address >= 0x800000 && address < 0x900000) {
    // NV4-style channel FIFO: 0x800000 + chid*0x10000 + subc*0x2000
    // 16 channels (chid 0-15), 8 subchannels
    Bit32u chid = (address >> 16) & 0xF;
    Bit32u offset = address & 0x1FFF;
    value = 0x00000000;
    Bit32u curchid = BX_NVRIVA_THIS fifo_cache1_push1 & 0xF;
    if (offset == 0x10) {
      value = 0xffff;
    } else if (offset >= 0x40 && offset <= 0x48) {
      if (curchid == chid) {
        if (offset == 0x40)
          value = BX_NVRIVA_THIS fifo_cache1_dma_put;
        else if (offset == 0x44)
          value = BX_NVRIVA_THIS fifo_cache1_dma_get;
        else if (offset == 0x48)
          value = BX_NVRIVA_THIS fifo_cache1_ref_cnt;
      } else {
        if (offset == 0x40)
          value = ramfc_read32(chid, 0x0);
        else if (offset == 0x44)
          value = ramfc_read32(chid, 0x4);
        else if (offset == 0x48)
          value = ramfc_read32(chid, 0x8);
      }
    } else {
      BX_ERROR(("Unknown FIFO offset 0x%08x", offset));
    }
  } else {
    value = BX_NVRIVA_THIS unk_regs[address / 4];
  }
  return value;
}

// MMIO register write (32-bit)

void bx_nvriva_c::register_write32(Bit32u address, Bit32u value)
{
  if (address == 0x100) {
    BX_NVRIVA_THIS mc_soft_intr = (bool)(value >> 31);
    update_irq_level();
  } else if (address == 0x140) {
    BX_NVRIVA_THIS mc_intr_en = value;
    update_irq_level();
  } else if (address == 0x200) {
    BX_NVRIVA_THIS mc_enable = value;
  } else if (address >= 0x1800 && address < 0x1900) {
    BX_NVRIVA_THIS pci_write_handler(address - 0x1800, value, 4);
  } else if (address == 0x1100) {
    BX_NVRIVA_THIS bus_intr &= ~value;
    update_irq_level();
  } else if (address == 0x1140) {
    BX_NVRIVA_THIS bus_intr_en = value;
    update_irq_level();
  } else if (address == 0x2100) {
    BX_NVRIVA_THIS fifo_intr &= ~value;
    update_irq_level();
  } else if (address == 0x2140) {
    BX_NVRIVA_THIS fifo_intr_en = value;
    update_irq_level();
  } else if (address == 0x2210) {
    BX_NVRIVA_THIS fifo_ramht = value;
  } else if (address == 0x2214) {
    BX_NVRIVA_THIS fifo_ramfc = value;
  } else if (address == 0x2218) {
    BX_NVRIVA_THIS fifo_ramro = value;
  } else if (address == 0x2504) {
    bool process = (BX_NVRIVA_THIS fifo_mode | value) != BX_NVRIVA_THIS fifo_mode;
    BX_NVRIVA_THIS fifo_mode = value;
    if (process)
      fifo_process();
  } else if (address == 0x3200) {
    BX_NVRIVA_THIS fifo_cache1_push0 = value;
    if ((BX_NVRIVA_THIS fifo_cache1_push0 & 1) != 0)
      fifo_process();
  } else if (address == 0x3204) {
    BX_NVRIVA_THIS fifo_cache1_push1 = value;
  } else if (address == 0x3210) {
    BX_NVRIVA_THIS fifo_cache1_put = value;
  } else if (address == 0x3220) {
    BX_NVRIVA_THIS fifo_cache1_dma_push = value;
  } else if (address == 0x322c) {
    BX_NVRIVA_THIS fifo_cache1_dma_instance = value;
  } else if (address == 0x3240) {
    BX_NVRIVA_THIS fifo_cache1_dma_put = value;
  } else if (address == 0x3244) {
    BX_NVRIVA_THIS fifo_cache1_dma_get = value;
  } else if (address == 0x3248) {
    BX_NVRIVA_THIS fifo_cache1_ref_cnt = value;
  } else if (address == 0x3250) {
    BX_NVRIVA_THIS fifo_cache1_pull0 = value;
    if ((BX_NVRIVA_THIS fifo_cache1_pull0 & 1) != 0)
      fifo_process();
  } else if (address == 0x3270) {
    BX_NVRIVA_THIS fifo_cache1_get = value & (NVRIVA_CACHE1_SIZE * 4 - 1);
    if (BX_NVRIVA_THIS fifo_cache1_get != BX_NVRIVA_THIS fifo_cache1_put) {
      BX_NVRIVA_THIS fifo_intr |= 0x00000001;
    } else {
      BX_NVRIVA_THIS fifo_intr &= ~0x00000001;
      BX_NVRIVA_THIS fifo_cache1_pull0 &= ~0x00000100;
      if (BX_NVRIVA_THIS fifo_wait_soft) {
        BX_NVRIVA_THIS fifo_wait_soft = false;
        update_fifo_wait();
        fifo_process();
      }
    }
    update_irq_level();
  } else if (address == 0x9100) {
    BX_NVRIVA_THIS timer_intr &= ~value;
  } else if (address == 0x9140) {
    BX_NVRIVA_THIS timer_intr_en = value;
  } else if (address == 0x9200) {
    BX_NVRIVA_THIS timer_num = value;
  } else if (address == 0x9210) {
    BX_NVRIVA_THIS timer_den = value;
  } else if (address == 0x9400 || address == 0x9410) {
    BX_NVRIVA_THIS timer_inittime2 = bx_pc_system.time_nsec();
    if (address == 0x9400) {
      BX_NVRIVA_THIS timer_inittime1 =
        (BX_NVRIVA_THIS timer_inittime1 & BX_CONST64(0xFFFFFFFF00000000)) | value;
    } else {
      BX_NVRIVA_THIS timer_inittime1 =
        (BX_NVRIVA_THIS timer_inittime1 & BX_CONST64(0x00000000FFFFFFFF)) | ((Bit64u)value << 32);
    }
  } else if (address == 0x9420) {
    BX_NVRIVA_THIS timer_alarm = value;
  } else if (address >= 0xc0300 && address < 0xc0400) {
    register_write8(address, value);
  } else if (address == 0x101000) {
    if (value >> 31)
      BX_NVRIVA_THIS straps0_primary = value;
    else
      BX_NVRIVA_THIS straps0_primary = BX_NVRIVA_THIS straps0_primary_original;
  } else if (address == 0x400100) {
    BX_NVRIVA_THIS graph_intr &= ~value;
    update_irq_level();
    if (BX_NVRIVA_THIS fifo_wait_notify && BX_NVRIVA_THIS graph_intr == 0) {
      BX_NVRIVA_THIS fifo_wait_notify = false;
      update_fifo_wait();
      fifo_process();
    }
  } else if (address == 0x400108) {
    BX_NVRIVA_THIS graph_nsource = value;
  } else if (address == 0x400140) {
    // NV4/NV5: graph_intr_en at 0x400140
    BX_NVRIVA_THIS graph_intr_en = value;
    update_irq_level();
  } else if (address == 0x40014C) {
    BX_NVRIVA_THIS graph_ctx_switch1 = value;
  } else if (address == 0x400150) {
    BX_NVRIVA_THIS graph_ctx_switch2 = value;
  } else if (address == 0x400158) {
    BX_NVRIVA_THIS graph_ctx_switch4 = value;
  } else if (address == 0x400700) {
    BX_NVRIVA_THIS graph_status = value;
  } else if (address == 0x400704) {
    BX_NVRIVA_THIS graph_trapped_addr = value;
  } else if (address == 0x400708) {
    BX_NVRIVA_THIS graph_trapped_data = value;
  } else if (address == 0x400718) {
    BX_NVRIVA_THIS graph_notify = value;
  } else if (address == 0x40071c) {
    if ((value & 0x00000002) != 0) {
      BX_NVRIVA_THIS graph_flip_read++;
      BX_NVRIVA_THIS graph_flip_read %= BX_NVRIVA_THIS graph_flip_modulo;
      if (BX_NVRIVA_THIS fifo_wait_flip &&
          BX_NVRIVA_THIS graph_flip_read != BX_NVRIVA_THIS graph_flip_write) {
        BX_NVRIVA_THIS fifo_wait_flip = false;
        update_fifo_wait();
        fifo_process();
      }
    }
  } else if (address == 0x400720) {
    BX_NVRIVA_THIS graph_fifo = value;
  } else if (address == 0x400724) {
    BX_NVRIVA_THIS graph_bpixel = value;
  } else if (address == 0x400780) {
    BX_NVRIVA_THIS graph_channel_ctx_table = value;
  } else if (address == 0x400640) {
    BX_NVRIVA_THIS graph_offset0 = value;
  } else if (address == 0x400670) {
    BX_NVRIVA_THIS graph_pitch0 = value;
  } else if (address == 0x600100) {
    BX_NVRIVA_THIS crtc_intr &= ~value;
    update_irq_level();
  } else if (address == 0x600140) {
    BX_NVRIVA_THIS crtc_intr_en = value;
    update_irq_level();
  } else if (address == 0x600800) {
    BX_NVRIVA_THIS crtc_start = value;
    BX_NVRIVA_THIS svga_needs_update_mode = 1;
  } else if (address == 0x600804) {
    BX_NVRIVA_THIS crtc_config = value;
  } else if (address == 0x60080c) {
    BX_NVRIVA_THIS crtc_cursor_offset = value;
    BX_NVRIVA_THIS hw_cursor.offset = BX_NVRIVA_THIS crtc_cursor_offset;
  } else if (address == 0x600810) {
    BX_NVRIVA_THIS crtc_cursor_config = value;
    BX_NVRIVA_THIS hw_cursor.enabled =
      (BX_NVRIVA_THIS crtc.reg[0x31] & 0x01) || (value & 0x00000001);
    BX_NVRIVA_THIS hw_cursor.vram =
      (BX_NVRIVA_THIS crtc.reg[0x30] & 0x80) || (value & 0x00000100);
    BX_NVRIVA_THIS hw_cursor.size = value & 0x00010000 ? 64 : 32;
    BX_NVRIVA_THIS hw_cursor.bpp32 = value & 0x00001000;
  } else if (address == 0x60081c) {
    BX_NVRIVA_THIS crtc_gpio_ext = value;
  } else if (address >= 0x601300 && address < 0x601400) {
    register_write8(address, value);
  } else if (address == 0x680300) {
    Bit16s prevx = BX_NVRIVA_THIS hw_cursor.x;
    Bit16s prevy = BX_NVRIVA_THIS hw_cursor.y;
    BX_NVRIVA_THIS ramdac_cu_start_pos = value;
    BX_NVRIVA_THIS hw_cursor.x = (Bit32s)BX_NVRIVA_THIS ramdac_cu_start_pos << 20 >> 20;
    BX_NVRIVA_THIS hw_cursor.y = (Bit32s)BX_NVRIVA_THIS ramdac_cu_start_pos << 4 >> 20;
    BX_NVRIVA_THIS redraw_area_nd(prevx, prevy,
      BX_NVRIVA_THIS hw_cursor.size, BX_NVRIVA_THIS hw_cursor.size);
    BX_NVRIVA_THIS redraw_area_nd(BX_NVRIVA_THIS hw_cursor.x, BX_NVRIVA_THIS hw_cursor.y,
      BX_NVRIVA_THIS hw_cursor.size, BX_NVRIVA_THIS hw_cursor.size);
  } else if (address == 0x680508) {
    BX_NVRIVA_THIS ramdac_vpll = value;
    BX_NVRIVA_THIS calculate_retrace_timing();
  } else if (address == 0x68050c) {
    BX_NVRIVA_THIS ramdac_pll_select = value;
    BX_NVRIVA_THIS calculate_retrace_timing();
  } else if (address == 0x680600) {
    BX_NVRIVA_THIS ramdac_general_control = value;
    BX_NVRIVA_THIS s.dac_shift = (value >> 20) & 1 ? 0 : 2;
  } else if (address >= 0x681300 && address < 0x681400) {
    register_write8(address, value);
  } else if (address >= 0x700000 && address < 0x800000) {
    ramin_write32(address - 0x700000, value);
  } else if (address >= 0x800000 && address < 0x900000) {
    Bit32u chid = (address >> 16) & 0xF;
    Bit32u offset = address & 0x1FFF;
    if ((BX_NVRIVA_THIS fifo_mode & (1 << chid)) != 0) {
      if (offset == 0x40) {
        Bit32u curchid = BX_NVRIVA_THIS fifo_cache1_push1 & 0xF;
        if (curchid == chid)
          BX_NVRIVA_THIS fifo_cache1_dma_put = value;
        else
          ramfc_write32(chid, 0x0, value);
        fifo_process(chid);
      }
    } else {
      Bit32u subc = (address >> 13) & 7;
      execute_command(chid, subc, offset / 4, value);
    }
  } else {
    BX_NVRIVA_THIS unk_regs[address / 4] = value;
  }
}

// PCI configuration

void bx_nvriva_c::svga_init_pcihandlers(void)
{
  BX_NVRIVA_THIS devfunc = 0x00;
  DEV_register_pci_handlers2(BX_NVRIVA_THIS_PTR,
      &BX_NVRIVA_THIS devfunc, BX_PLUGIN_NVRIVA, "RIVA TNT2 M64 AGP", true);

  BX_NVRIVA_THIS init_pci_conf(0x10DE, 0x002D, 0x15, 0x030000, 0x00, BX_PCI_INTA);

  BX_NVRIVA_THIS init_bar_mem(0, NVRIVA_PNPMMIO_SIZE, nvriva_mem_read_handler, nvriva_mem_write_handler);

  BX_NVRIVA_THIS pci_conf[0x14] = 0x08;
  BX_NVRIVA_THIS init_bar_mem(1, BX_NVRIVA_THIS s.memsize, nvriva_mem_read_handler, nvriva_mem_write_handler);

  BX_NVRIVA_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr(),
                               nvriva_mem_read_handler);

  // Subsystem IDs
  BX_NVRIVA_THIS pci_conf[0x2c] = 0x7D;
  BX_NVRIVA_THIS pci_conf[0x2d] = 0x10;
  BX_NVRIVA_THIS pci_conf[0x2e] = 0x37;
  BX_NVRIVA_THIS pci_conf[0x2f] = 0x21;
  BX_NVRIVA_THIS pci_conf[0x40] = BX_NVRIVA_THIS pci_conf[0x2c];
  BX_NVRIVA_THIS pci_conf[0x41] = BX_NVRIVA_THIS pci_conf[0x2d];
  BX_NVRIVA_THIS pci_conf[0x42] = BX_NVRIVA_THIS pci_conf[0x2e];
  BX_NVRIVA_THIS pci_conf[0x43] = BX_NVRIVA_THIS pci_conf[0x2f];


  BX_NVRIVA_THIS pci_conf[0x06] = 0xB0;
  BX_NVRIVA_THIS pci_conf[0x07] = 0x02;

  BX_NVRIVA_THIS pci_conf[0x34] = 0x60;

  BX_NVRIVA_THIS pci_conf[0x44] = 0x02;
  BX_NVRIVA_THIS pci_conf[0x45] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x46] = 0x20;
  BX_NVRIVA_THIS pci_conf[0x47] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x48] = 0x07;
  BX_NVRIVA_THIS pci_conf[0x49] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x4a] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x4b] = 0x1F;
  BX_NVRIVA_THIS pci_conf[0x54] = 0x01;
  BX_NVRIVA_THIS pci_conf[0x55] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x56] = 0x00;
  BX_NVRIVA_THIS pci_conf[0x57] = 0x00;

  BX_NVRIVA_THIS pci_conf[0x60] = 0x01;
  BX_NVRIVA_THIS pci_conf[0x61] = 0x44;
  BX_NVRIVA_THIS pci_conf[0x62] = 0x02;
  BX_NVRIVA_THIS pci_conf[0x63] = 0x00;
}

void bx_nvriva_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
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
void bx_nvriva_c::debug_dump(int argc, char **argv)
{
  bx_vgacore_c::debug_dump(argc, argv);
}
#endif

#endif // BX_SUPPORT_NVRIVA