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
#define BX_USE_BINARY_ROP
#define BX_USE_TERNARY_ROP
#include "bitblt.h"
#include "ddc.h"
#include "geforce.h"
#include "virt_timer.h"

#include "bx_debug/debug.h"

#if BX_SUPPORT_GEFORCE

#define LOG_THIS BX_GEFORCE_THIS

#if BX_USE_GEFORCE_SMF
#define VGA_READ(addr,len)       bx_vgacore_c::read_handler(theSvga,addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write_handler(theSvga,addr,val,len)
#define SVGA_READ(addr,len)      svga_read_handler(theSvga,addr,len)
#define SVGA_WRITE(addr,val,len) svga_write_handler(theSvga,addr,val,len)
#else
#define VGA_READ(addr,len)       bx_vgacore_c::read(addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write(addr,val,len)
#define SVGA_READ(addr,len)      svga_read(addr,len)
#define SVGA_WRITE(addr,val,len) svga_write(addr,val,len)
#endif // BX_USE_GEFORCE_SMF

#define GEFORCE_PNPMMIO_SIZE        0x1000000

#define ALIGN(x, a) (((x) + (a) - 1) & ~((a) - 1))

#define EXTRACT_565_TO_888(val, a, b, c)          \
  (a) = (((val) >> 8) & 0xf8) | (((val) >> 13) & 0x07); \
  (b) = (((val) >> 3) & 0xfc) | (((val) >> 9) & 0x03);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);

#define EXTRACT_x555_TO_888(val, a, b, c)         \
  (a) = (((val) >> 7) & 0xf8) | (((val) >> 12) & 0x07); \
  (b) = (((val) >> 2) & 0xf8) | (((val) >> 7) & 0x07);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);

#define EXTRACT_1555_TO_8888(val, a, b, c, d)       \
  (a) = ((Bit16s)(val) >> 15) & 0xff;           \
  EXTRACT_x555_TO_888(val, b, c, d)

static bx_geforce_c *theSvga = NULL;

/* enumeration specifying which model of GeForce we are emulating */
enum
{
    GEFORCE_3,
    GEFORCE_FX_5900,
    GEFORCE_6800,
    MAX_GEFORCE_TYPES
};


void geforce_init_options(void)
{
  static const char* geforce_model_list[] = {
    "geforce3",
    "geforcefx5900",
    "geforce6800",
    NULL
  };

  bx_param_c *display = SIM->get_param("display");
  bx_list_c *menu = new bx_list_c(display, "geforce", "GeForce");
  menu->set_options(menu->SHOW_PARENT);
  new bx_param_enum_c(menu,
    "model",
    "GeForce model",
    "Selects the GeForce model to emulate.",
    geforce_model_list,
    GEFORCE_3, GEFORCE_3);
}

Bit32s geforce_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "geforce")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_GEFORCE);
    for (int i = 1; i < num_params; i++) {
      if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for geforce ignored.", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s geforce_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_GEFORCE), NULL, 0);
}

PLUGIN_ENTRY_FOR_MODULE(geforce)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_geforce_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_GEFORCE);
    // add new configuration parameter for the config interface
    geforce_init_options();
    // register add-on option for bochsrc and command line
    SIM->register_addon_option("geforce", geforce_options_parser, geforce_options_save);
  } else if (mode == PLUGIN_FINI) {
    SIM->unregister_addon_option("geforce");
    bx_list_c *menu = (bx_list_c*)SIM->get_param("display");
    menu->remove("geforce");
    delete theSvga;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_VGA;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

bx_geforce_c::bx_geforce_c() : bx_vgacore_c()
{
  // nothing else to do
}

bx_geforce_c::~bx_geforce_c()
{
  SIM->get_bochs_root()->remove("geforce");
  BX_DEBUG(("Exit"));
}

bool bx_geforce_c::init_vga_extension(void)
{
  // Read in values from config interface
  bx_list_c* base = (bx_list_c*)SIM->get_param(BXPN_GEFORCE);
  Bit32s model_enum = (Bit8u)SIM->get_param_enum("model", base)->get();

  const char* model_string;
  if (model_enum == GEFORCE_3) {
    BX_GEFORCE_THIS card_type = 0x20;
    model_string = "GeForce3 Ti 500";
  } else if (model_enum == GEFORCE_FX_5900) {
    BX_GEFORCE_THIS card_type = 0x35;
    model_string = "GeForce FX 5900";
  } else if (model_enum == GEFORCE_6800) {
    BX_GEFORCE_THIS card_type = 0x40;
    model_string = "GeForce 6800 GT";
  } else {
    model_string = nullptr;
    BX_GEFORCE_THIS card_type = 0x00;
    BX_PANIC(("Unknown card type"));
  }

  if (!SIM->is_agp_device(BX_PLUGIN_GEFORCE)) {
    BX_PANIC(("%s should be plugged into AGP slot", model_string));
  }

  BX_GEFORCE_THIS pci_enabled = true;

  BX_GEFORCE_THIS put("GEFORCE");
  // initialize SVGA stuffs.
  BX_GEFORCE_THIS init_iohandlers(svga_read_handler, svga_write_handler, "geforce");
  DEV_register_ioread_handler(this, svga_read_handler, 0x03B4, "geforce", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D0, "geforce", 7);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D2, "geforce", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03C3, "geforce", 3);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D0, "geforce", 7);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D2, "geforce", 2);
  BX_GEFORCE_THIS svga_init_members();
  BX_GEFORCE_THIS svga_init_pcihandlers();
  BX_GEFORCE_THIS bitblt_init();
  BX_GEFORCE_THIS s.CRTC.max_reg = GEFORCE_CRTC_MAX;
  BX_GEFORCE_THIS s.max_xres = 2048;
  BX_GEFORCE_THIS s.max_yres = 1536;
  BX_INFO(("%s initialized", model_string));
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("geforce", this);
#endif
  return 1;
}

#define SETUP_BITBLT(num, name, flags) \
  do { \
    BX_GEFORCE_THIS rop_handler[num] = bitblt_rop_fwd_##name; \
    BX_GEFORCE_THIS rop_flags[num] = flags; \
  } while (0);

void bx_geforce_c::bitblt_init()
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

void bx_geforce_c::svga_init_members()
{
  // clear all registers.
  BX_GEFORCE_THIS crtc.index = GEFORCE_CRTC_MAX + 1;
  for (int i = 0; i <= GEFORCE_CRTC_MAX; i++)
    BX_GEFORCE_THIS crtc.reg[i] = 0x00;

  BX_GEFORCE_THIS mc_intr_en = 0;
  BX_GEFORCE_THIS mc_enable = 0;
  BX_GEFORCE_THIS bus_intr = 0;
  BX_GEFORCE_THIS bus_intr_en = 0;
  BX_GEFORCE_THIS fifo_intr = 0;
  BX_GEFORCE_THIS fifo_intr_en = 0;
  BX_GEFORCE_THIS fifo_ramht = 0;
  BX_GEFORCE_THIS fifo_ramfc = 0;
  BX_GEFORCE_THIS fifo_ramro = 0;
  BX_GEFORCE_THIS fifo_mode = 0;
  BX_GEFORCE_THIS fifo_cache1_push1 = 0;
  BX_GEFORCE_THIS fifo_cache1_put = 0;
  BX_GEFORCE_THIS fifo_cache1_dma_push = 0;
  BX_GEFORCE_THIS fifo_cache1_dma_instance = 0;
  BX_GEFORCE_THIS fifo_cache1_dma_put = 0;
  BX_GEFORCE_THIS fifo_cache1_dma_get = 0;
  BX_GEFORCE_THIS fifo_cache1_ref_cnt = 0;
  BX_GEFORCE_THIS fifo_cache1_pull0 = 0;
  BX_GEFORCE_THIS fifo_cache1_semaphore = 0;
  BX_GEFORCE_THIS fifo_cache1_get = 0;
  BX_GEFORCE_THIS fifo_grctx_instance = 0;
  for (int i = 0; i < GEFORCE_CACHE1_SIZE; i++) {
    BX_GEFORCE_THIS fifo_cache1_method[i] = 0;
    BX_GEFORCE_THIS fifo_cache1_data[i] = 0;
  }
  BX_GEFORCE_THIS rma_addr = 0;
  BX_GEFORCE_THIS timer_intr = 0;
  BX_GEFORCE_THIS timer_intr_en = 0;
  BX_GEFORCE_THIS timer_num = 0;
  BX_GEFORCE_THIS timer_den = 0;
  BX_GEFORCE_THIS timer_inittime1 = 0;
  BX_GEFORCE_THIS timer_inittime2 = 0;
  BX_GEFORCE_THIS timer_alarm = 0;
  BX_GEFORCE_THIS graph_intr = 0;
  BX_GEFORCE_THIS graph_nsource = 0;
  BX_GEFORCE_THIS graph_intr_en = 0;
  BX_GEFORCE_THIS graph_ctx_switch1 = 0;
  BX_GEFORCE_THIS graph_ctx_switch2 = 0;
  BX_GEFORCE_THIS graph_ctx_switch4 = 0;
  BX_GEFORCE_THIS graph_ctxctl_cur = 0;
  BX_GEFORCE_THIS graph_status = 0;
  BX_GEFORCE_THIS graph_trapped_addr = 0;
  BX_GEFORCE_THIS graph_trapped_data = 0;
  BX_GEFORCE_THIS graph_notify = 0;
  BX_GEFORCE_THIS graph_fifo = 0;
  BX_GEFORCE_THIS graph_channel_ctx_table = 0;
  BX_GEFORCE_THIS crtc_intr = 0;
  BX_GEFORCE_THIS crtc_intr_en = 0;
  BX_GEFORCE_THIS crtc_start = 0;
  BX_GEFORCE_THIS crtc_config = 0;
  BX_GEFORCE_THIS crtc_cursor_offset = 0;
  BX_GEFORCE_THIS crtc_cursor_config = 0;
  BX_GEFORCE_THIS ramdac_cu_start_pos = 0;
  BX_GEFORCE_THIS ramdac_vpll = 0;
  BX_GEFORCE_THIS ramdac_vpll_b = 0;
  BX_GEFORCE_THIS ramdac_pll_select = 0;
  BX_GEFORCE_THIS ramdac_general_control = 0;

  BX_GEFORCE_THIS acquire_active = false;

  for (int i = 0; i < GEFORCE_CHANNEL_COUNT; i++) {
    BX_GEFORCE_THIS chs[i].subr_return = 0;
    BX_GEFORCE_THIS chs[i].subr_active = false;
    BX_GEFORCE_THIS chs[i].dma_state.mthd = 0;
    BX_GEFORCE_THIS chs[i].dma_state.subc = 0;
    BX_GEFORCE_THIS chs[i].dma_state.mcnt = 0;
    BX_GEFORCE_THIS chs[i].dma_state.ni = false;
    for (int j = 0; j < GEFORCE_SUBCHANNEL_COUNT; j++) {
      BX_GEFORCE_THIS chs[i].schs[j].object = 0;
      BX_GEFORCE_THIS chs[i].schs[j].engine = 0x00;
      BX_GEFORCE_THIS chs[i].schs[j].notifier = 0;
    }

    BX_GEFORCE_THIS chs[i].notify_pending = false;
    BX_GEFORCE_THIS chs[i].notify_type = 0;
    BX_GEFORCE_THIS chs[i].s2d_img_src = 0;
    BX_GEFORCE_THIS chs[i].s2d_img_dst = 0;
    BX_GEFORCE_THIS chs[i].s2d_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].s2d_color_bytes = 1;
    BX_GEFORCE_THIS chs[i].s2d_pitch = 0;
    BX_GEFORCE_THIS chs[i].s2d_ofs_src = 0;
    BX_GEFORCE_THIS chs[i].s2d_ofs_dst = 0;

    BX_GEFORCE_THIS chs[i].ifc_color_key_enable = false;
    BX_GEFORCE_THIS chs[i].ifc_operation = 0;
    BX_GEFORCE_THIS chs[i].ifc_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].ifc_color_bytes = 0;
    BX_GEFORCE_THIS chs[i].ifc_yx = 0;
    BX_GEFORCE_THIS chs[i].ifc_dhw = 0;
    BX_GEFORCE_THIS chs[i].ifc_shw = 0;
    BX_GEFORCE_THIS chs[i].ifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[i].ifc_words_left = 0;
    BX_GEFORCE_THIS chs[i].ifc_words = nullptr;

    BX_GEFORCE_THIS chs[i].iifc_palette = 0;
    BX_GEFORCE_THIS chs[i].iifc_palette_ofs = 0;
    BX_GEFORCE_THIS chs[i].iifc_operation = 0;
    BX_GEFORCE_THIS chs[i].iifc_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].iifc_color_bytes = 0;
    BX_GEFORCE_THIS chs[i].iifc_bpp4 = 0;
    BX_GEFORCE_THIS chs[i].iifc_yx = 0;
    BX_GEFORCE_THIS chs[i].iifc_dhw = 0;
    BX_GEFORCE_THIS chs[i].iifc_shw = 0;
    BX_GEFORCE_THIS chs[i].iifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[i].iifc_words_left = 0;
    BX_GEFORCE_THIS chs[i].iifc_words = nullptr;

    BX_GEFORCE_THIS chs[i].sifc_operation = 0;
    BX_GEFORCE_THIS chs[i].sifc_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].sifc_color_bytes = 0;
    BX_GEFORCE_THIS chs[i].sifc_shw = 0;
    BX_GEFORCE_THIS chs[i].sifc_dxds = 0;
    BX_GEFORCE_THIS chs[i].sifc_dydt = 0;
    BX_GEFORCE_THIS chs[i].sifc_clip_yx = 0;
    BX_GEFORCE_THIS chs[i].sifc_clip_hw = 0;
    BX_GEFORCE_THIS chs[i].sifc_syx = 0;
    BX_GEFORCE_THIS chs[i].sifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[i].sifc_words_left = 0;
    BX_GEFORCE_THIS chs[i].sifc_words = nullptr;

    BX_GEFORCE_THIS chs[i].blit_color_key_enable = false;
    BX_GEFORCE_THIS chs[i].blit_operation = 0;
    BX_GEFORCE_THIS chs[i].blit_syx = 0;
    BX_GEFORCE_THIS chs[i].blit_dyx = 0;
    BX_GEFORCE_THIS chs[i].blit_hw = 0;

    BX_GEFORCE_THIS chs[i].sifm_src = 0;
    BX_GEFORCE_THIS chs[i].sifm_operation = 0;
    BX_GEFORCE_THIS chs[i].sifm_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].sifm_color_bytes = 0;
    BX_GEFORCE_THIS chs[i].sifm_syx = 0;
    BX_GEFORCE_THIS chs[i].sifm_dyx = 0;
    BX_GEFORCE_THIS chs[i].sifm_shw = 0;
    BX_GEFORCE_THIS chs[i].sifm_dhw = 0;
    BX_GEFORCE_THIS chs[i].sifm_dudx = 0;
    BX_GEFORCE_THIS chs[i].sifm_dvdy = 0;
    BX_GEFORCE_THIS chs[i].sifm_sfmt = 0;
    BX_GEFORCE_THIS chs[i].sifm_sofs = 0;

    BX_GEFORCE_THIS chs[i].m2mf_src = 0;
    BX_GEFORCE_THIS chs[i].m2mf_dst = 0;
    BX_GEFORCE_THIS chs[i].m2mf_src_offset = 0;
    BX_GEFORCE_THIS chs[i].m2mf_dst_offset = 0;
    BX_GEFORCE_THIS chs[i].m2mf_src_pitch = 0;
    BX_GEFORCE_THIS chs[i].m2mf_dst_pitch = 0;
    BX_GEFORCE_THIS chs[i].m2mf_line_length = 0;
    BX_GEFORCE_THIS chs[i].m2mf_line_count = 0;
    BX_GEFORCE_THIS chs[i].m2mf_format = 0;
    BX_GEFORCE_THIS chs[i].m2mf_buffer_notify = 0;

    BX_GEFORCE_THIS chs[i].d3d_semaphore_obj = 0;
    BX_GEFORCE_THIS chs[i].d3d_semaphore_offset = 0;

    BX_GEFORCE_THIS chs[i].rop = 0;

    BX_GEFORCE_THIS chs[i].beta = 0;

    BX_GEFORCE_THIS chs[i].clip_yx = 0;
    BX_GEFORCE_THIS chs[i].clip_hw = 0;

    BX_GEFORCE_THIS chs[i].chroma_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].chroma_color = 0;

    BX_GEFORCE_THIS chs[i].patt_shape = 0;
    BX_GEFORCE_THIS chs[i].patt_type = 0;
    BX_GEFORCE_THIS chs[i].patt_bg_color = 0;
    BX_GEFORCE_THIS chs[i].patt_fg_color = 0;
    for (int j = 0; j < 64; j++) {
      BX_GEFORCE_THIS chs[i].patt_data_mono[j] = false;
      BX_GEFORCE_THIS chs[i].patt_data_color[j] = 0;
    }

    BX_GEFORCE_THIS chs[i].gdi_operation = 0;
    BX_GEFORCE_THIS chs[i].gdi_color_fmt = 0;
    BX_GEFORCE_THIS chs[i].gdi_clip_yx0 = 0;
    BX_GEFORCE_THIS chs[i].gdi_clip_yx1 = 0;
    BX_GEFORCE_THIS chs[i].gdi_rect_color = 0;
    BX_GEFORCE_THIS chs[i].gdi_rect_xy = 0;
    BX_GEFORCE_THIS chs[i].gdi_rect_yx0 = 0;
    BX_GEFORCE_THIS chs[i].gdi_rect_yx1 = 0;
    BX_GEFORCE_THIS chs[i].gdi_rect_wh = 0;
    BX_GEFORCE_THIS chs[i].gdi_bg_color = 0;
    BX_GEFORCE_THIS chs[i].gdi_fg_color = 0;
    BX_GEFORCE_THIS chs[i].gdi_image_swh = 0;
    BX_GEFORCE_THIS chs[i].gdi_image_dwh = 0;
    BX_GEFORCE_THIS chs[i].gdi_image_xy = 0;
    BX_GEFORCE_THIS chs[i].gdi_words_ptr = 0;
    BX_GEFORCE_THIS chs[i].gdi_words_left = 0;
    BX_GEFORCE_THIS chs[i].gdi_words = nullptr;
  }

  for (int i = 0; i < 4 * 1024 * 1024; i++)
    BX_GEFORCE_THIS unk_regs[i] = 0;

  BX_GEFORCE_THIS svga_unlock_special = 0;
  BX_GEFORCE_THIS svga_needs_update_tile = 1;
  BX_GEFORCE_THIS svga_needs_update_dispentire = 1;
  BX_GEFORCE_THIS svga_needs_update_mode = 0;
  BX_GEFORCE_THIS svga_double_width = 0;

  BX_GEFORCE_THIS svga_xres = 640;
  BX_GEFORCE_THIS svga_yres = 480;
  BX_GEFORCE_THIS svga_bpp = 8;
  BX_GEFORCE_THIS svga_pitch = 640;
  BX_GEFORCE_THIS bank_base[0] = 0;
  BX_GEFORCE_THIS bank_base[1] = 0;

  BX_GEFORCE_THIS hw_cursor.x = 0;
  BX_GEFORCE_THIS hw_cursor.y = 0;
  BX_GEFORCE_THIS hw_cursor.size = 0;
  BX_GEFORCE_THIS hw_cursor.offset = 0;
  BX_GEFORCE_THIS hw_cursor.bpp32 = false;
  BX_GEFORCE_THIS hw_cursor.enabled = false;

  if (BX_GEFORCE_THIS card_type == 0x20) {
    BX_GEFORCE_THIS s.memsize = 64 * 1024 * 1024;
    BX_GEFORCE_THIS bar2_size = 0x00080000;
    // Matches real hardware with exception of disabled TV out
    BX_GEFORCE_THIS straps0_primary_original = (0x7FF86C6B | 0x00000180);
  } else {
    if (BX_GEFORCE_THIS card_type == 0x35)
      BX_GEFORCE_THIS s.memsize = 128 * 1024 * 1024;
    else
      BX_GEFORCE_THIS s.memsize = 256 * 1024 * 1024;
    BX_GEFORCE_THIS bar2_size = 0x01000000;
    // Guess
    BX_GEFORCE_THIS straps0_primary_original = (0x7FF86C4B | 0x00000180);
  }
  BX_GEFORCE_THIS straps0_primary = BX_GEFORCE_THIS straps0_primary_original;
  BX_GEFORCE_THIS ramin_flip = BX_GEFORCE_THIS s.memsize - 64;
  BX_GEFORCE_THIS memsize_mask = BX_GEFORCE_THIS s.memsize - 1;

  // memory allocation.
  if (BX_GEFORCE_THIS s.memory == NULL)
    BX_GEFORCE_THIS s.memory = new Bit8u[BX_GEFORCE_THIS s.memsize];
  memset(BX_GEFORCE_THIS s.memory, 0x00, BX_GEFORCE_THIS s.memsize);
  BX_GEFORCE_THIS disp_ptr = BX_GEFORCE_THIS s.memory;

  // VCLK defaults after reset
  BX_GEFORCE_THIS s.vclk[0] = 25180000;
  BX_GEFORCE_THIS s.vclk[1] = 28325000;
  BX_GEFORCE_THIS s.vclk[2] = 41165000;
  BX_GEFORCE_THIS s.vclk[3] = 36082000;
}

void bx_geforce_c::reset(unsigned type)
{
  // reset VGA stuffs.
  BX_GEFORCE_THIS bx_vgacore_c::reset(type);
  // reset SVGA stuffs.
  BX_GEFORCE_THIS svga_init_members();
  BX_GEFORCE_THIS ddc.init();
  // Disable ROM shadowing to allow clearing of VRAM
  BX_GEFORCE_THIS pci_conf[0x50] = 0x00;
}

void bx_geforce_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "geforce", "GeForce State");
  BX_GEFORCE_THIS vgacore_register_state(list);
  bx_list_c *crtc = new bx_list_c(list, "crtc");
  new bx_shadow_num_c(crtc, "index", &BX_GEFORCE_THIS crtc.index, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_GEFORCE_THIS crtc.reg, GEFORCE_CRTC_MAX + 1, 1);
  BXRS_PARAM_BOOL(list, svga_unlock_special, BX_GEFORCE_THIS svga_unlock_special);
  BXRS_PARAM_BOOL(list, svga_double_width, BX_GEFORCE_THIS svga_double_width);
  new bx_shadow_num_c(list, "svga_xres", &BX_GEFORCE_THIS svga_xres);
  new bx_shadow_num_c(list, "svga_yres", &BX_GEFORCE_THIS svga_yres);
  new bx_shadow_num_c(list, "svga_pitch", &BX_GEFORCE_THIS svga_pitch);
  new bx_shadow_num_c(list, "svga_bpp", &BX_GEFORCE_THIS svga_bpp);
  new bx_shadow_num_c(list, "svga_dispbpp", &BX_GEFORCE_THIS svga_dispbpp);
  new bx_shadow_num_c(list, "bank_base0", &BX_GEFORCE_THIS bank_base[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_base1", &BX_GEFORCE_THIS bank_base[1], BASE_HEX);
  bx_list_c *cursor = new bx_list_c(list, "hw_cursor");
  new bx_shadow_num_c(cursor, "x", &BX_GEFORCE_THIS hw_cursor.x, BASE_HEX);
  new bx_shadow_num_c(cursor, "y", &BX_GEFORCE_THIS hw_cursor.y, BASE_HEX);
  new bx_shadow_num_c(cursor, "size", &BX_GEFORCE_THIS hw_cursor.size, BASE_HEX);
  register_pci_state(list);
}

void bx_geforce_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(geforce_mem_read_handler);
  BX_GEFORCE_THIS bx_vgacore_c::after_restore_state();
}

void bx_geforce_c::redraw_area(unsigned x0, unsigned y0,
                               unsigned width, unsigned height)
{
  redraw_area((Bit32s)x0, (Bit32s)y0, width, height);
}

void bx_geforce_c::redraw_area(Bit32u offset, Bit32u width, Bit32u height)
{
  if (BX_GEFORCE_THIS svga_pitch != 0) {
    Bit32u redraw_x = offset % BX_GEFORCE_THIS svga_pitch / (BX_GEFORCE_THIS svga_bpp >> 3);
    Bit32u redraw_y = offset / BX_GEFORCE_THIS svga_pitch;
    BX_GEFORCE_THIS redraw_area(redraw_x, redraw_y, width, height);
  }
}

void bx_geforce_c::redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  if (x0 + (Bit32s)width <= 0 || y0 + (Bit32s)height <= 0)
    return;

  if (!BX_GEFORCE_THIS crtc.reg[0x28]) {
    BX_GEFORCE_THIS bx_vgacore_c::redraw_area(x0, y0, width, height);
    return;
  }

  if (BX_GEFORCE_THIS svga_needs_update_mode)
    return;

  if (s.y_doublescan) {
    y0 <<= 1;
    height <<= 1;
  }
  if (BX_GEFORCE_THIS svga_double_width) {
    x0 <<= 1;
    width <<= 1;
  }

  BX_GEFORCE_THIS svga_needs_update_tile = 1;

  unsigned xti, yti, xt0, xt1, yt0, yt1;
  xt0 = x0 <= 0 ? 0 : x0 / X_TILESIZE;
  yt0 = y0 <= 0 ? 0 : y0 / Y_TILESIZE;
  if (x0 < (Bit32s)BX_GEFORCE_THIS svga_xres) {
    xt1 = (x0 + width - 1) / X_TILESIZE;
  } else {
    xt1 = (BX_GEFORCE_THIS svga_xres - 1) / X_TILESIZE;
  }
  if (y0 < (Bit32s)BX_GEFORCE_THIS svga_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (BX_GEFORCE_THIS svga_yres - 1) / Y_TILESIZE;
  }
  if ((x0 + width) > BX_GEFORCE_THIS svga_xres) {
    BX_GEFORCE_THIS redraw_area(0, y0 + 1, x0 + width - BX_GEFORCE_THIS svga_xres, height);
  }
  for (yti=yt0; yti<=yt1; yti++) {
    for (xti=xt0; xti<=xt1; xti++) {
      SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 1);
    }
  }
}

void bx_geforce_c::vertical_timer()
{
  bx_vgacore_c::vertical_timer();
  if (BX_GEFORCE_THIS vtimer_toggle) {
    BX_GEFORCE_THIS crtc_intr |= 0x00000001;
    BX_DEBUG(("vertical_timer, crtc_intr_en = %d, mc_intr_en = %d",
      BX_GEFORCE_THIS crtc_intr_en, BX_GEFORCE_THIS mc_intr_en));
    update_irq_level();
  }
  if (BX_GEFORCE_THIS acquire_active) {
    BX_GEFORCE_THIS acquire_active = false;
    for (int i = 0; i < GEFORCE_CHANNEL_COUNT; i++)
      fifo_process(i);
  }
}

bool bx_geforce_c::geforce_mem_read_handler(bx_phy_address addr, unsigned len,
                                        void *data, void *param)
{
  if (addr >= BX_GEFORCE_THIS pci_bar[0].addr &&
      addr < (BX_GEFORCE_THIS pci_bar[0].addr + GEFORCE_PNPMMIO_SIZE)) {
    Bit32u offset = addr & (GEFORCE_PNPMMIO_SIZE - 1);
    if (len == 1) {
      *(Bit8u*)data = register_read8(offset);
      BX_DEBUG(("MMIO read from 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
    } else if (len == 4) {
      Bit32u value = register_read32(offset);
      BX_DEBUG(("MMIO read from 0x%08x, value 0x%08x", offset, value));
      *((Bit8u*)data + 0) = (value >> 0) & 0xFF;
      *((Bit8u*)data + 1) = (value >> 8) & 0xFF;
      *((Bit8u*)data + 2) = (value >> 16) & 0xFF;
      *((Bit8u*)data + 3) = (value >> 24) & 0xFF;
    } else {
      BX_PANIC(("MMIO read len %d", len));
    }
    return 1;
  }

  if (BX_GEFORCE_THIS card_type != 0x35 && addr >= BX_GEFORCE_THIS pci_bar[2].addr &&
      addr < (BX_GEFORCE_THIS pci_bar[2].addr + BX_GEFORCE_THIS bar2_size)) {
    Bit32u offset = addr & (BX_GEFORCE_THIS bar2_size - 1);
    if (BX_GEFORCE_THIS card_type >= 0x40) {
      if (len == 1) {
        *(Bit8u*)data = ramin_read8(offset);
        BX_DEBUG(("RAMIN read from 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
      } else if (len == 4) {
        Bit32u value = ramin_read32(offset);
        BX_DEBUG(("RAMIN read from 0x%08x, value 0x%08x", offset, value));
        *((Bit8u*)data + 0) = (value >> 0) & 0xFF;
        *((Bit8u*)data + 1) = (value >> 8) & 0xFF;
        *((Bit8u*)data + 2) = (value >> 16) & 0xFF;
        *((Bit8u*)data + 3) = (value >> 24) & 0xFF;
      } else {
        BX_PANIC(("RAMIN read len %d", len));
      }
      return 1;
    } else {
      BX_ERROR(("BAR2 read from 0x%08x", offset));
    }
  }

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = BX_GEFORCE_THIS mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

Bit8u bx_geforce_c::mem_read(bx_phy_address addr)
{
  if (BX_GEFORCE_THIS pci_rom_size > 0) {
    Bit32u mask = (BX_GEFORCE_THIS pci_rom_size - 1);
    if (((Bit32u)addr & ~mask) == BX_GEFORCE_THIS pci_rom_address) {
      if (BX_GEFORCE_THIS pci_conf[0x30] & 0x01) {
        if (BX_GEFORCE_THIS pci_conf[0x50] == 0x00)
          return BX_GEFORCE_THIS pci_rom[addr & mask];
        else
          return BX_GEFORCE_THIS s.memory[addr & mask ^ BX_GEFORCE_THIS ramin_flip];
      } else {
        return 0xff;
      }
    }
  }

  if ((addr >= BX_GEFORCE_THIS pci_bar[1].addr) &&
      (addr < (BX_GEFORCE_THIS pci_bar[1].addr + BX_GEFORCE_THIS s.memsize))) {
    Bit32u offset = addr & BX_GEFORCE_THIS memsize_mask;
    return BX_GEFORCE_THIS s.memory[offset];
  }

  if (!BX_GEFORCE_THIS crtc.reg[0x28])
    return BX_GEFORCE_THIS bx_vgacore_c::mem_read(addr);

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset = addr & 0xffff;
    offset += BX_GEFORCE_THIS bank_base[0];
    offset &= BX_GEFORCE_THIS memsize_mask;
    return BX_GEFORCE_THIS s.memory[offset];
  }

  return 0xFF;
}

bool bx_geforce_c::geforce_mem_write_handler(bx_phy_address addr, unsigned len,
                                         void *data, void *param)
{
  if (addr >= BX_GEFORCE_THIS pci_bar[0].addr &&
      addr < (BX_GEFORCE_THIS pci_bar[0].addr + GEFORCE_PNPMMIO_SIZE)) {
    Bit32u offset = addr & (GEFORCE_PNPMMIO_SIZE - 1);
    if (len == 1) {
      BX_DEBUG(("MMIO write to 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
      register_write8(offset, *(Bit8u*)data);
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
      BX_PANIC(("MMIO write len %d", len));
    }
    return 1;
  }

  if (BX_GEFORCE_THIS card_type != 0x35 && addr >= BX_GEFORCE_THIS pci_bar[2].addr &&
      addr < (BX_GEFORCE_THIS pci_bar[2].addr + BX_GEFORCE_THIS bar2_size)) {
    Bit32u offset = addr & (BX_GEFORCE_THIS bar2_size - 1);
    if (BX_GEFORCE_THIS card_type >= 0x40) {
      if (len == 1) {
        BX_DEBUG(("RAMIN write to 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
        ramin_write8(offset, *(Bit8u*)data);
      } else if (len == 4) {
        Bit32u value =
          (*((Bit8u*)data + 0) << 0) |
          (*((Bit8u*)data + 1) << 8) |
          (*((Bit8u*)data + 2) << 16) |
          (*((Bit8u*)data + 3) << 24);
        BX_DEBUG(("RAMIN write to 0x%08x, value 0x%08x", offset, value));
        ramin_write32(offset, value);
      } else {
        BX_PANIC(("RAMIN write len %d", len));
      }
      return 1;
    } else {
      BX_PANIC(("BAR2 write to 0x%08x", offset));
    }
  }

  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    BX_GEFORCE_THIS mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

void bx_geforce_c::mem_write(bx_phy_address addr, Bit8u value)
{
  if ((addr >= BX_GEFORCE_THIS pci_bar[1].addr) &&
      (addr < (BX_GEFORCE_THIS pci_bar[1].addr + BX_GEFORCE_THIS s.memsize))) {
    unsigned x, y;
    Bit32u offset = addr & BX_GEFORCE_THIS memsize_mask;
    BX_GEFORCE_THIS s.memory[offset] = value;
    if (BX_GEFORCE_THIS svga_pitch != 0) {
      BX_GEFORCE_THIS svga_needs_update_tile = 1;
      offset -= (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
      x = (offset % BX_GEFORCE_THIS svga_pitch) / (BX_GEFORCE_THIS svga_bpp / 8);
      y = offset / BX_GEFORCE_THIS svga_pitch;
      if (BX_GEFORCE_THIS s.y_doublescan)
        y <<= 1;
      if (BX_GEFORCE_THIS svga_double_width)
        x <<= 1;
      SET_TILE_UPDATED(BX_GEFORCE_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
    }
    return;
  }

  if (!BX_GEFORCE_THIS crtc.reg[0x28]) {
    BX_GEFORCE_THIS bx_vgacore_c::mem_write(addr, value);
    return;
  }

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u offset;
    unsigned x, y;

    offset = addr & 0xffff;
    if (BX_GEFORCE_THIS crtc.reg[0x1c] & 0x80) {
      BX_GEFORCE_THIS s.memory[offset ^ BX_GEFORCE_THIS ramin_flip] = value;
      return;
    }
    offset += BX_GEFORCE_THIS bank_base[0];
    offset &= BX_GEFORCE_THIS memsize_mask;
    BX_GEFORCE_THIS s.memory[offset] = value;
    BX_GEFORCE_THIS svga_needs_update_tile = 1;
    x = (offset % BX_GEFORCE_THIS svga_pitch) / (BX_GEFORCE_THIS svga_bpp / 8);
    y = offset / BX_GEFORCE_THIS svga_pitch;
    SET_TILE_UPDATED(BX_GEFORCE_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
  }
}

void bx_geforce_c::get_text_snapshot(Bit8u **text_snapshot,
                                    unsigned *txHeight, unsigned *txWidth)
{
  BX_GEFORCE_THIS bx_vgacore_c::get_text_snapshot(text_snapshot,txHeight,txWidth);
}

Bit32u bx_geforce_c::svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_GEFORCE_SMF
  bx_geforce_c *class_ptr = (bx_geforce_c *) this_ptr;

  return class_ptr->svga_read(address, io_len);
}

Bit32u bx_geforce_c::svga_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_GEFORCE_SMF

  if (address == 0x03C3 && io_len == 2)
    return VGA_READ(address, 1) | VGA_READ(address + 1, 1) << 8;

  if (address == 0x03d0 || address == 0x03d2) // RMA_ACCESS
  {
      if (io_len == 1) {
        BX_ERROR(("port 0x3d0 access with io_len = 1"));
        return 0;
      }
      Bit8u crtc38 = BX_GEFORCE_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable) {
        BX_ERROR(("port 0x3d0 access is disabled"));
        return 0;
      }
      int rma_index = crtc38 >> 1;
      if (rma_index == 1) {
        if (address == 0x03d0) {
          //if (io_len == 2)
          //  BX_DEBUG(("rma: read address 0x%08x (lo)", BX_GEFORCE_THIS rma_addr));
          //else
          //  BX_DEBUG(("rma: read address 0x%08x", BX_GEFORCE_THIS rma_addr));
          return BX_GEFORCE_THIS rma_addr;
        } else {
          //BX_DEBUG(("rma: read address 0x%08x (hi)", BX_GEFORCE_THIS rma_addr));
          return BX_GEFORCE_THIS rma_addr >> 16;
        }
      }
      else if (rma_index == 2) {
        bool vram = false;
        Bit32u offset = BX_GEFORCE_THIS rma_addr;
        if (BX_GEFORCE_THIS rma_addr & 0x80000000) {
          vram = true;
          offset &= ~0x80000000;
        } 
        if (!vram && offset < GEFORCE_PNPMMIO_SIZE ||
            vram && offset < BX_GEFORCE_THIS s.memsize) {
          Bit32u value = vram ? vram_read32(offset) : register_read32(offset);
          if (address == 0x03d0) {
            if (io_len == 2)
              BX_DEBUG(("rma: read from 0x%08x value 0x????%04x", BX_GEFORCE_THIS rma_addr, value & 0xFFFF));
            else
              BX_DEBUG(("rma: read from 0x%08x value 0x%08x", BX_GEFORCE_THIS rma_addr, value));
            return value;
          } else {
            BX_DEBUG(("rma: read from 0x%08x value 0x%04x????", BX_GEFORCE_THIS rma_addr, value >> 16));
            return value >> 16;
          }
        } else {
          BX_ERROR(("rma: oob read from 0x%08x ignored", BX_GEFORCE_THIS rma_addr));
          return 0xFFFFFFFF;
        }
      }
      else if (rma_index == 3)
        BX_ERROR(("rma: read index 3"));
      else
        BX_PANIC(("rma: read unknown index %d", rma_index));
      return 0;
  }

  if ((io_len == 2) && ((address & 1) == 0)) {
    Bit32u value;
    value = (Bit32u)SVGA_READ(address,1);
    value |= (Bit32u)SVGA_READ(address+1,1) << 8;
    return value;
  }

  if (io_len != 1) {
    BX_PANIC(("SVGA read: io_len != 1"));
  }

  switch (address) {
    case 0x03b4: /* VGA: CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* VGA: CRTC Index Register (color emulation modes) */
      return BX_GEFORCE_THIS crtc.index;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_GEFORCE_THIS crtc.index > VGA_CRTC_MAX)
        return BX_GEFORCE_THIS svga_read_crtc(address,BX_GEFORCE_THIS crtc.index);
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

void bx_geforce_c::svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_GEFORCE_SMF
  bx_geforce_c *class_ptr = (bx_geforce_c *) this_ptr;
  class_ptr->svga_write(address, value, io_len);
}

void bx_geforce_c::svga_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_GEFORCE_SMF

  if (address == 0x03d0 || address == 0x03d2) // RMA_ACCESS
  {
      if (io_len == 1) {
        BX_DEBUG(("port 0x3d0 access with io_len = 1"));
        return;
      }
      Bit8u crtc38 = BX_GEFORCE_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable) {
        BX_ERROR(("port 0x3d0 access is disabled"));
        return;
      }
      int rma_index = crtc38 >> 1;
      if (rma_index == 1) {
        if (address == 0x03d0) {
          if (io_len == 2) {
            BX_GEFORCE_THIS rma_addr &= 0xFFFF0000;
            BX_GEFORCE_THIS rma_addr |= value;
            //BX_DEBUG(("rma: address set to 0x%08x (lo)", BX_GEFORCE_THIS rma_addr));
          } else {
            BX_GEFORCE_THIS rma_addr = value;
            //BX_DEBUG(("rma: address set to 0x%08x", BX_GEFORCE_THIS rma_addr));
          }
        } else {
          BX_GEFORCE_THIS rma_addr &= 0x0000FFFF;
          BX_GEFORCE_THIS rma_addr |= value << 16;
          //BX_DEBUG(("rma: address set to 0x%08x (hi)", BX_GEFORCE_THIS rma_addr));
        }
      } else if (rma_index == 2) {
        BX_ERROR(("rma: write index 2"));
      } else if (rma_index == 3) {
        bool vram = false;
        Bit32u offset = BX_GEFORCE_THIS rma_addr;
        if (BX_GEFORCE_THIS rma_addr & 0x80000000) {
          vram = true;
          offset &= ~0x80000000;
        }
        if (!vram && offset < GEFORCE_PNPMMIO_SIZE ||
            vram && offset < BX_GEFORCE_THIS s.memsize) {
          if (address == 0x03d0) {
            if (io_len == 2) {
              Bit32u value32 = vram ? vram_read32(offset) : register_read32(offset);
              value32 &= 0xFFFF0000;
              value32 |= value;
              if (vram)
                vram_write32(offset, value32);
              else
                register_write32(offset, value32);
              BX_DEBUG(("rma: write to 0x%08x value 0x????%04x", BX_GEFORCE_THIS rma_addr, value));
            } else {
              if (vram)
                vram_write32(offset, value);
              else
                register_write32(offset, value);
              BX_DEBUG(("rma: write to 0x%08x value 0x%08x", BX_GEFORCE_THIS rma_addr, value));
            }
          } else {
            Bit32u value32 = vram ? vram_read32(offset) : register_read32(offset);
            value32 &= 0x0000FFFF;
            value32 |= value << 16;
            if (vram)
              vram_write32(offset, value32);
            else
              register_write32(offset, value32);
            BX_DEBUG(("rma: write to 0x%08x value 0x%04x????", BX_GEFORCE_THIS rma_addr, value));
          }
        } else {
          BX_ERROR(("rma: oob write to 0x%08x ignored", BX_GEFORCE_THIS rma_addr));
        }
      } else {
        BX_PANIC(("rma: write unknown index %d", rma_index));
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
      BX_GEFORCE_THIS crtc.index = value;
      break;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_GEFORCE_THIS crtc.index == 0x01 ||
          BX_GEFORCE_THIS crtc.index == 0x07 ||
          BX_GEFORCE_THIS crtc.index == 0x09 ||
          BX_GEFORCE_THIS crtc.index == 0x0c ||
          BX_GEFORCE_THIS crtc.index == 0x0d ||
          BX_GEFORCE_THIS crtc.index == 0x12 ||
          BX_GEFORCE_THIS crtc.index == 0x13 ||
          BX_GEFORCE_THIS crtc.index == 0x15 ||
          BX_GEFORCE_THIS crtc.index == 0x19 ||
          BX_GEFORCE_THIS crtc.index == 0x25 ||
          BX_GEFORCE_THIS crtc.index == 0x28 ||
          BX_GEFORCE_THIS crtc.index == 0x2D ||
          BX_GEFORCE_THIS crtc.index == 0x41 ||
          BX_GEFORCE_THIS crtc.index == 0x42) {
        BX_GEFORCE_THIS svga_needs_update_mode = 1;
      }
      if (BX_GEFORCE_THIS crtc.index <= VGA_CRTC_MAX) {
        BX_GEFORCE_THIS crtc.reg[BX_GEFORCE_THIS crtc.index] = value;
      } else {
        BX_GEFORCE_THIS svga_write_crtc(address, BX_GEFORCE_THIS crtc.index, value);
        if (BX_GEFORCE_THIS crtc.index == 0x25 ||
            BX_GEFORCE_THIS crtc.index == 0x2D ||
            BX_GEFORCE_THIS crtc.index == 0x41) {
          BX_GEFORCE_THIS calculate_retrace_timing();
        }
        return;
      }
      break;
    default:
      break;
  }

  VGA_WRITE(address,value,io_len);
}

Bit8u alpha_wrap(int value)
{
  return -(value >> 8) ^ value;
}

void bx_geforce_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  Bit16s hwcx = BX_GEFORCE_THIS hw_cursor.x;
  Bit16s hwcy = BX_GEFORCE_THIS hw_cursor.y;
  Bit8u size = BX_GEFORCE_THIS hw_cursor.size;

  if (BX_GEFORCE_THIS svga_double_width) {
    hwcx <<= 1;
    hwcy <<= 1;
    size <<= 1;
  }

  unsigned w, h;
  Bit8u* tile_ptr;
  if (info->snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    w = BX_GEFORCE_THIS svga_xres;
    h = BX_GEFORCE_THIS svga_yres;
  } else {
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  }
  if (BX_GEFORCE_THIS hw_cursor.enabled &&
      (int)xc < hwcx + size &&
      (int)(xc + w) > hwcx &&
      (int)yc < hwcy + size &&
      (int)(yc + h) > hwcy) {
    unsigned cx0 = hwcx > (int)xc ? hwcx : xc;
    unsigned cy0 = hwcy > (int)yc ? hwcy : yc;
    unsigned cx1 = hwcx + size < (int)(xc + w) ? hwcx + size : xc + w;
    unsigned cy1 = hwcy + size < (int)(yc + h) ? hwcy + size : yc + h;

    Bit8u display_color_bytes = BX_GEFORCE_THIS svga_bpp >> 3;
    Bit8u cursor_color_bytes = BX_GEFORCE_THIS hw_cursor.bpp32 ? 4 : 2;
    if (info->bpp == 15) info->bpp = 16;
    tile_ptr += info->pitch * (cy0 - yc) + info->bpp / 8 * (cx0 - xc);
    unsigned pitch = BX_GEFORCE_THIS hw_cursor.size * cursor_color_bytes;
    Bit8u* cursor_ptr = BX_GEFORCE_THIS s.memory + BX_GEFORCE_THIS hw_cursor.offset +
      (pitch * (cy0 - hwcy) >> (int)BX_GEFORCE_THIS s.y_doublescan);
    Bit8u* vid_ptr = BX_GEFORCE_THIS disp_ptr + (BX_GEFORCE_THIS svga_pitch * cy0 >>
      (int)BX_GEFORCE_THIS s.y_doublescan);
    for (unsigned cy = cy0; cy < cy1; cy++) {
      Bit8u* tile_ptr2 = tile_ptr;
      Bit8u* cursor_ptr2 = cursor_ptr + (cursor_color_bytes * (cx0 - hwcx) >>
        (int)BX_GEFORCE_THIS svga_double_width);
      Bit8u* vid_ptr2 = vid_ptr + (display_color_bytes * cx0 >>
        (int)BX_GEFORCE_THIS svga_double_width);
      for (unsigned cx = cx0; cx < cx1; cx++) {
        Bit8u dr, dg, db;
        if (display_color_bytes == 1) {
          if (info->is_indexed) {
            dr = dg = db = vid_ptr2[0];
          } else {
            Bit8u index = vid_ptr2[0];
            dr = (BX_GEFORCE_THIS s.pel.data[index].red << BX_GEFORCE_THIS s.dac_shift);
            dg = (BX_GEFORCE_THIS s.pel.data[index].green << BX_GEFORCE_THIS s.dac_shift);
            db = (BX_GEFORCE_THIS s.pel.data[index].blue << BX_GEFORCE_THIS s.dac_shift);
          }
        } else if (display_color_bytes == 2) {
          EXTRACT_565_TO_888(vid_ptr2[0] << 0 | vid_ptr2[1] << 8, dr, dg, db);
        } else {
          db = vid_ptr2[0];
          dg = vid_ptr2[1];
          dr = vid_ptr2[2];
        }
        Bit32u color;
        if (BX_GEFORCE_THIS hw_cursor.bpp32) {
          if (*(Bit32u*)cursor_ptr2) {
            Bit8u ica = 0xFF - cursor_ptr2[3];
            Bit8u b = alpha_wrap(db * ica / 0xFF + cursor_ptr2[0]);
            Bit8u g = alpha_wrap(dg * ica / 0xFF + cursor_ptr2[1]);
            Bit8u r = alpha_wrap(dr * ica / 0xFF + cursor_ptr2[2]);
            color = b << 0 | g << 8 | r << 16;
          } else {
            color = db << 0 | dg << 8 | dr << 16;
          }
        } else {
          Bit8u alpha, cr, cg, cb;
          EXTRACT_1555_TO_8888(cursor_ptr2[0] << 0 | cursor_ptr2[1] << 8, alpha, cr, cg, cb);
          if (alpha) {
            color = cb << 0 | cg << 8 | cr << 16;
          } else {
            Bit8u b = db ^ cb;
            Bit8u g = dg ^ cg;
            Bit8u r = dr ^ cr;
            color = b << 0 | g << 8 | r << 16;
          }
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
        if (!BX_GEFORCE_THIS svga_double_width || (cx & 1)) {
          cursor_ptr2 += cursor_color_bytes;
          vid_ptr2 += display_color_bytes;
        }
      }
      tile_ptr += info->pitch;
      if (!BX_GEFORCE_THIS s.y_doublescan || (cy & 1)) {
        cursor_ptr += pitch;
        vid_ptr += BX_GEFORCE_THIS svga_pitch;
      }
    }
  }
}

Bit16u bx_geforce_c::get_crtc_vtotal()
{
  return BX_GEFORCE_THIS crtc.reg[6] +
    ((BX_GEFORCE_THIS crtc.reg[7] & 0x01) << 8) +
    ((BX_GEFORCE_THIS crtc.reg[7] & 0x20) << 4) +
    ((BX_GEFORCE_THIS crtc.reg[0x25] & 1) << 10) +
    ((BX_GEFORCE_THIS crtc.reg[0x41] & 1) << 11) + 2;
}

void bx_geforce_c::get_crtc_params(bx_crtc_params_t* crtcp, Bit32u* vclock)
{
  Bit32u m = BX_GEFORCE_THIS ramdac_vpll & 0xFF;
  Bit32u n = BX_GEFORCE_THIS ramdac_vpll >> 8 & 0xFF;
  Bit32u p = BX_GEFORCE_THIS ramdac_vpll >> 16 & 0x07;
  Bit32u mb = 1;
  Bit32u nb = 1;
  if (BX_GEFORCE_THIS card_type == 0x35) {
    m &= 0xF;
    if (BX_GEFORCE_THIS ramdac_vpll & 0x00000080) {
      mb = BX_GEFORCE_THIS ramdac_vpll >> 4 & 0x7;
      nb = BX_GEFORCE_THIS ramdac_vpll >> 21 & 0x18 |
           BX_GEFORCE_THIS ramdac_vpll >> 19 & 0x7;
    }
  } else if (BX_GEFORCE_THIS card_type >= 0x40 && BX_GEFORCE_THIS ramdac_vpll_b & 0x80000000) {
    mb = BX_GEFORCE_THIS ramdac_vpll_b & 0xFF;
    nb = BX_GEFORCE_THIS ramdac_vpll_b >> 8 & 0xFF;
  }
  if (BX_GEFORCE_THIS ramdac_pll_select & 0x200 && m && mb) {
    Bit32u crystalFreq = 13500000;
    if (BX_GEFORCE_THIS straps0_primary & 0x00000040)
      crystalFreq = 14318000;
    if (BX_GEFORCE_THIS card_type > 0x20 && BX_GEFORCE_THIS straps0_primary & 0x00400000)
      crystalFreq = 27000000;
    *vclock = (Bit32u)((Bit64u)crystalFreq * n * nb / m / mb >> p);
    crtcp->htotal = BX_GEFORCE_THIS crtc.reg[0] +
                    ((BX_GEFORCE_THIS crtc.reg[0x2D] & 1) << 8) + 5;
    crtcp->vtotal = get_crtc_vtotal();
    crtcp->vbstart = BX_GEFORCE_THIS crtc.reg[21] +
                     ((BX_GEFORCE_THIS crtc.reg[7] & 0x08) << 5) +
                     ((BX_GEFORCE_THIS crtc.reg[9] & 0x20) << 4) +
                     ((BX_GEFORCE_THIS crtc.reg[0x25] & 0x08) << 7) +
                     ((BX_GEFORCE_THIS crtc.reg[0x41] & 0x40) << 5);
    crtcp->vrstart = BX_GEFORCE_THIS crtc.reg[16] +
                     ((BX_GEFORCE_THIS crtc.reg[7] & 0x04) << 6) +
                     ((BX_GEFORCE_THIS crtc.reg[7] & 0x80) << 2) +
                     ((BX_GEFORCE_THIS crtc.reg[0x25] & 0x04) << 8) +
                     ((BX_GEFORCE_THIS crtc.reg[0x41] & 0x10) << 7);
  } else {
    bx_vgacore_c::get_crtc_params(crtcp, vclock);
  }
}

void bx_geforce_c::update(void)
{
  unsigned width, height, pitch;

  Bit8u crtc28 = BX_GEFORCE_THIS crtc.reg[0x28];
  if (!crtc28) {
    BX_GEFORCE_THIS bx_vgacore_c::update();
    return;
  }

  if (BX_GEFORCE_THIS svga_needs_update_mode) {
    Bit32u iTopOffset =
      BX_GEFORCE_THIS crtc.reg[0x0d] |
      BX_GEFORCE_THIS crtc.reg[0x0c] << 8 |
      (BX_GEFORCE_THIS crtc.reg[0x19] & 0x1F) << 16;
    iTopOffset <<= 2;
    iTopOffset += BX_GEFORCE_THIS crtc_start;

    Bit32u iPitch =
      BX_GEFORCE_THIS crtc.reg[0x13] |
      BX_GEFORCE_THIS crtc.reg[0x19] >> 5 << 8 |
      (BX_GEFORCE_THIS crtc.reg[0x42] >> 6 & 1) << 11;
    iPitch <<= 3;

    if (crtc28 & 0x80) {
      crtc28 &= 0x7F;
      BX_ERROR(("Slaved mode activated"));
    }

    Bit8u iBpp;
    if (crtc28 == 0x01)
      iBpp = 8;
    else if (crtc28 == 0x02)
      iBpp = 16;
    else if (crtc28 == 0x03)
      iBpp = 32;
    else
      BX_PANIC(("unknown bpp"));

    Bit32u iWidth =
      (BX_GEFORCE_THIS crtc.reg[1] +
      ((BX_GEFORCE_THIS crtc.reg[0x2D] & 0x02) << 7) + 1) * 8;
    Bit32u iHeight =
      (BX_GEFORCE_THIS crtc.reg[18] |
      ((BX_GEFORCE_THIS crtc.reg[7] & 0x02) << 7) |
      ((BX_GEFORCE_THIS crtc.reg[7] & 0x40) << 3) |
      ((BX_GEFORCE_THIS crtc.reg[0x25] & 0x02) << 9) |
      ((BX_GEFORCE_THIS crtc.reg[0x41] & 0x04) << 9)) + 1;

    if (BX_GEFORCE_THIS s.y_doublescan && iHeight > iWidth) {
      iWidth <<= 1;
      BX_GEFORCE_THIS svga_double_width = true;
    } else {
      BX_GEFORCE_THIS svga_double_width = false;
    }

    if (iWidth != BX_GEFORCE_THIS svga_xres ||
        iHeight != BX_GEFORCE_THIS svga_yres ||
        iBpp != BX_GEFORCE_THIS svga_bpp) {
      BX_INFO(("switched to %u x %u x %u", iWidth, iHeight, iBpp));
    }

    BX_GEFORCE_THIS svga_xres = iWidth;
    BX_GEFORCE_THIS svga_yres = iHeight;
    BX_GEFORCE_THIS svga_bpp = iBpp;
    BX_GEFORCE_THIS svga_dispbpp = iBpp;
    BX_GEFORCE_THIS disp_ptr = BX_GEFORCE_THIS s.memory + iTopOffset;
    BX_GEFORCE_THIS svga_pitch = iPitch;
    // compatibilty settings for VGA core
    BX_GEFORCE_THIS s.last_xres = iWidth;
    BX_GEFORCE_THIS s.last_yres = iHeight;
    BX_GEFORCE_THIS s.last_bpp = iBpp;
    BX_GEFORCE_THIS s.last_fh = 0;
  }

  if (BX_GEFORCE_THIS svga_dispbpp != 4) {
    width  = BX_GEFORCE_THIS svga_xres;
    height = BX_GEFORCE_THIS svga_yres;
    pitch = BX_GEFORCE_THIS svga_pitch;
    if (BX_GEFORCE_THIS svga_needs_update_mode) {
      bx_gui->dimension_update(width, height, 0, 0, BX_GEFORCE_THIS svga_dispbpp);
      BX_GEFORCE_THIS s.last_bpp = BX_GEFORCE_THIS svga_dispbpp;
      BX_GEFORCE_THIS svga_needs_update_mode = 0;
      BX_GEFORCE_THIS svga_needs_update_dispentire = 1;
    }
  } else {
    BX_GEFORCE_THIS determine_screen_dimensions(&height, &width);
    pitch = BX_GEFORCE_THIS s.line_offset;
    if ((width != BX_GEFORCE_THIS s.last_xres) || (height != BX_GEFORCE_THIS s.last_yres) ||
        (BX_GEFORCE_THIS s.last_bpp > 8)) {
      bx_gui->dimension_update(width, height);
      BX_GEFORCE_THIS s.last_xres = width;
      BX_GEFORCE_THIS s.last_yres = height;
      BX_GEFORCE_THIS s.last_bpp = 8;
    }
  }

  if (BX_GEFORCE_THIS svga_needs_update_dispentire) {
    BX_GEFORCE_THIS redraw_area(0, 0, width, height);
    BX_GEFORCE_THIS svga_needs_update_dispentire = 0;
  }

  if (!BX_GEFORCE_THIS svga_needs_update_tile) {
    return;
  }
  BX_GEFORCE_THIS svga_needs_update_tile = 0;

  unsigned xc, yc, xti, yti, hp;
  unsigned r, c, w, h, x, y;
  int i;
  Bit8u red, green, blue;
  Bit32u colour, row_addr;
  Bit8u * vid_ptr, * vid_ptr2;
  Bit8u * tile_ptr, * tile_ptr2;
  bx_svga_tileinfo_t info;
  Bit8u dac_size = (BX_GEFORCE_THIS s.dac_shift == 0) ? 8 : 6;

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = BX_GEFORCE_THIS disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < height; yc++) {
          vid_ptr2  = vid_ptr;
          tile_ptr2 = tile_ptr;
          if (BX_GEFORCE_THIS svga_dispbpp != 4) {
            for (xc = 0; xc < width; xc++) {
              memcpy(tile_ptr2, vid_ptr2, (BX_GEFORCE_THIS svga_bpp >> 3));
              if (!BX_GEFORCE_THIS svga_double_width || (xc & 1)) {
                vid_ptr2 += (BX_GEFORCE_THIS svga_bpp >> 3);
              }
              tile_ptr2 += ((info.bpp + 1) >> 3);
            }
            if (!BX_GEFORCE_THIS s.y_doublescan || (yc & 1)) {
              vid_ptr += pitch;
            }
          } else {
            row_addr = BX_GEFORCE_THIS s.CRTC.start_addr + (yc * pitch);
            for (xc = 0; xc < width; xc++) {
              *(tile_ptr2++) = BX_GEFORCE_THIS get_vga_pixel(xc, yc, row_addr, 0xffff, 0, BX_GEFORCE_THIS s.memory);
            }
          }
          tile_ptr += info.pitch;
        }
        draw_hardware_cursor(0, 0, &info);
      }
    } else if (info.is_indexed) {
      switch (BX_GEFORCE_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  y = yc + r;
                  if (BX_GEFORCE_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_GEFORCE_THIS s.CRTC.start_addr + (y * pitch);
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    x = xc + c;
                    *(tile_ptr2++) = BX_GEFORCE_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_GEFORCE_THIS s.memory);
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
        case 16:
        case 24:
        case 32:
          BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays, svga_dispbpp=%d",
            BX_GEFORCE_THIS svga_dispbpp));
          break;
        case 8:
          hp = BX_GEFORCE_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    *(tile_ptr2++) = colour;
                  }
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
      }
    }
    else {
      switch (BX_GEFORCE_THIS svga_dispbpp) {
        case 4:
          for (yc=0, yti=0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<Y_TILESIZE; r++) {
                  tile_ptr2 = tile_ptr;
                  y = yc + r;
                  if (BX_GEFORCE_THIS s.y_doublescan) y >>= 1;
                  row_addr = BX_GEFORCE_THIS s.CRTC.start_addr + (y * pitch);
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    colour = BX_GEFORCE_THIS get_vga_pixel(x, y, row_addr, 0xffff, 0, BX_GEFORCE_THIS s.memory);
                    colour = MAKE_COLOUR(
                      BX_GEFORCE_THIS s.pel.data[colour].red, 6, info.red_shift, info.red_mask,
                      BX_GEFORCE_THIS s.pel.data[colour].green, 6, info.green_shift, info.green_mask,
                      BX_GEFORCE_THIS s.pel.data[colour].blue, 6, info.blue_shift, info.blue_mask);
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
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 8:
          hp = BX_GEFORCE_THIS s.attribute_ctrl.horiz_pel_panning & 0x07;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + xc + hp);
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + xc + hp);
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) >> 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
                      vid_ptr2++;
                    }
                    colour = MAKE_COLOUR(
                      BX_GEFORCE_THIS s.pel.data[colour].red, dac_size, info.red_shift, info.red_mask,
                      BX_GEFORCE_THIS s.pel.data[colour].green, dac_size, info.green_shift, info.green_mask,
                      BX_GEFORCE_THIS s.pel.data[colour].blue, dac_size, info.blue_shift, info.blue_mask);
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
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
          hp = BX_GEFORCE_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
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
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 16:
          hp = BX_GEFORCE_THIS s.attribute_ctrl.horiz_pel_panning & 0x01;
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + ((xc + hp) << 1));
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + ((xc + hp) << 1));
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + xc + (hp << 1));
                  }
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2);
                    colour |= *(vid_ptr2+1) << 8;
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
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
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 24:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + 3 * xc);
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + 3 * xc);
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + 3 * (xc >> 1));
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
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
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
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 32:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_GEFORCE_THIS s.y_doublescan) {
                  vid_ptr = BX_GEFORCE_THIS disp_ptr + (yc * pitch + (xc << 2));
                } else {
                  if (!BX_GEFORCE_THIS svga_double_width) {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 2));
                  } else {
                    vid_ptr = BX_GEFORCE_THIS disp_ptr + ((yc >> 1) * pitch + (xc << 1));
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
                    if (!BX_GEFORCE_THIS svga_double_width || (c & 1)) {
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
                  if (!BX_GEFORCE_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr  += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 0);
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

Bit8u bx_geforce_c::svga_read_crtc(Bit32u address, unsigned index)
{
  if (index <= GEFORCE_CRTC_MAX) {
    Bit8u value = BX_GEFORCE_THIS crtc.reg[index];
    BX_DEBUG(("crtc: index 0x%02x read 0x%02x", index, value));
    return value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x read", index));

  return 0xff;
}

void bx_geforce_c::svga_write_crtc(Bit32u address, unsigned index, Bit8u value)
{
  if (index == 0x40) {
    if (BX_GEFORCE_THIS crtc.reg[index] != value)
      BX_DEBUG(("crtc: index 0x%02x write 0x%02x [no duplicates]", index, (unsigned)value));
  } else
    BX_DEBUG(("crtc: index 0x%02x write 0x%02x", index, (unsigned)value));

  bool update_cursor_addr = false;

  if (index == 0x1c) {
    if (!(BX_GEFORCE_THIS crtc.reg[index] & 0x80) && value & 0x80) {
      // Without clearing this register, Windows 95 hangs after reboot
      BX_GEFORCE_THIS crtc_intr_en = 0x00000000;
      update_irq_level();
    }
  } else if (index == 0x1d || index == 0x1e)
    BX_GEFORCE_THIS bank_base[index - 0x1d] = value * 0x8000;
  else if (index == 0x2f || index == 0x30 || index == 0x31)
    update_cursor_addr = true;
  else if (index == 0x37 || index == 0x3f || index == 0x51) {
    bool scl = value & 0x20;
    bool sda = value & 0x10;
    if (index == 0x3f) {
      BX_GEFORCE_THIS ddc.write(scl, sda);
      BX_GEFORCE_THIS crtc.reg[0x3e] = BX_GEFORCE_THIS ddc.read() & 0x0c;
    } else {
      BX_GEFORCE_THIS crtc.reg[index - 1] = sda << 3 | scl << 2;
    }
  } else if (index == 0x58) {
    // Combined with 0x57, this register makes pair which allows
    //   to access 16 bytes of head-specific variables.
    // Until visible side effects appear, however, it is better to
    //   just disable writes to it which makes reads to return zeroes.
    return;
  }

  if (index <= GEFORCE_CRTC_MAX) {
    BX_GEFORCE_THIS crtc.reg[index] = value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x write", index));

  if (update_cursor_addr) {
    bool prev_enabled = BX_GEFORCE_THIS hw_cursor.enabled;
    BX_GEFORCE_THIS hw_cursor.enabled =
      BX_GEFORCE_THIS crtc.reg[0x31] & 0x01 ||
      BX_GEFORCE_THIS crtc_cursor_config & 0x00000001;
    BX_GEFORCE_THIS hw_cursor.offset =
      BX_GEFORCE_THIS crtc.reg[0x31] >> 2 << 11 |
      (BX_GEFORCE_THIS crtc.reg[0x30] & 0x7F) << 17 |
      BX_GEFORCE_THIS crtc.reg[0x2f] << 24;
    BX_GEFORCE_THIS hw_cursor.offset += BX_GEFORCE_THIS crtc_cursor_offset;
    if (prev_enabled != BX_GEFORCE_THIS hw_cursor.enabled) {
      BX_GEFORCE_THIS redraw_area(BX_GEFORCE_THIS hw_cursor.x, BX_GEFORCE_THIS hw_cursor.y,
        BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
    }
  }
}

Bit8u bx_geforce_c::register_read8(Bit32u address)
{
  Bit8u value;
  if (address >= 0x1800 && address < 0x1900) {
    value = BX_GEFORCE_THIS pci_conf[address - 0x1800];
  } else if (address >= 0x300000 && address < 0x310000) {
    if (BX_GEFORCE_THIS pci_conf[0x50] == 0x00)
      value = BX_GEFORCE_THIS pci_rom[address - 0x300000];
    else
      value = 0x00;
  } else if (address >= 0xc0300 && address < 0xc0400 ||
             address >= 0xc2300 && address < 0xc2400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3c3 ||
        offset == 0x3c4 || offset == 0x3c5 ||
        offset == 0x3cc || offset == 0x3cf) {
      if (!head)
        value = SVGA_READ(offset, 1);
      else
        value = 0x00;
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x601300 && address < 0x601400 ||
             address >= 0x603300 && address < 0x603400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3b4 || offset == 0x3b5 ||
        offset == 0x3c0 || offset == 0x3c1 ||
        offset == 0x3c2 || offset == 0x3d4 ||
        offset == 0x3d5 || offset == 0x3d8 ||
        offset == 0x3da) {
      if (!head)
        value = SVGA_READ(offset, 1);
      else
        value = 0x00;
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x681300 && address < 0x681400 ||
             address >= 0x683300 && address < 0x683400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset >= 0x3c6 && offset <= 0x3c9) {
      if (!head)
        value = SVGA_READ(offset, 1);
      else
        value = 0x00;
    } else {
      value = 0xFF;
      BX_PANIC(("Unknown register 0x%08x read", address));
    }
  } else if (address >= 0x700000 && address < 0x800000) {
    value = BX_GEFORCE_THIS s.memory[address - 0x700000 ^ BX_GEFORCE_THIS ramin_flip];
  } else {
    value = register_read32(address);
  }
  return value;
}

void bx_geforce_c::register_write8(Bit32u address, Bit8u value)
{
  if (address >= 0xc0300 && address < 0xc0400 ||
      address >= 0xc2300 && address < 0xc2400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3c2 || offset == 0x3c3 ||
        offset == 0x3c4 || offset == 0x3c5 ||
        offset == 0x3ce || offset == 0x3cf) {
      if (!head)
        SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x601300 && address < 0x601400 ||
             address >= 0x603300 && address < 0x603400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset == 0x3b4 || offset == 0x3b5 ||
        offset == 0x3c0 || offset == 0x3c1 ||
        offset == 0x3c2 || offset == 0x3d4 ||
        offset == 0x3d5 || offset == 0x3da) {
      if (!head)
        SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x681300 && address < 0x681400 ||
             address >= 0x683300 && address < 0x683400) {
    Bit32u head = address >> 13 & 1;
    Bit32u offset = address & 0x00000fff;
    if (offset >= 0x3c6 && offset <= 0x3c9) {
      if (!head)
        SVGA_WRITE(offset, value, 1);
    } else {
      BX_PANIC(("Unknown register 0x%08x write", address));
    }
  } else if (address >= 0x700000 && address < 0x800000) {
    BX_GEFORCE_THIS s.memory[address - 0x700000 ^ BX_GEFORCE_THIS ramin_flip] = value;
  } else {
    register_write32(address, register_read32(address) & ~0xFF | value);
  }
}

Bit8u bx_geforce_c::vram_read8(Bit32u address)
{
  return BX_GEFORCE_THIS s.memory[address];
}

Bit16u bx_geforce_c::vram_read16(Bit32u address)
{
  return
    BX_GEFORCE_THIS s.memory[address + 0] << 0 |
    BX_GEFORCE_THIS s.memory[address + 1] << 8;
}

Bit32u bx_geforce_c::vram_read32(Bit32u address)
{
  return
    BX_GEFORCE_THIS s.memory[address + 0] << 0 |
    BX_GEFORCE_THIS s.memory[address + 1] << 8 |
    BX_GEFORCE_THIS s.memory[address + 2] << 16 |
    BX_GEFORCE_THIS s.memory[address + 3] << 24;
}

void bx_geforce_c::vram_write8(Bit32u address, Bit8u value)
{
  BX_GEFORCE_THIS s.memory[address + 0] = value;
}

void bx_geforce_c::vram_write16(Bit32u address, Bit16u value)
{
  BX_GEFORCE_THIS s.memory[address + 0] = value >> 0 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 1] = value >> 8 & 0xFF;
}

void bx_geforce_c::vram_write32(Bit32u address, Bit32u value)
{
  BX_GEFORCE_THIS s.memory[address + 0] = value >> 0 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 1] = value >> 8 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 2] = value >> 16 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 3] = value >> 24 & 0xFF;
}

void bx_geforce_c::vram_write64(Bit32u address, Bit64u value)
{
  BX_GEFORCE_THIS s.memory[address + 0] = value >> 0 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 1] = value >> 8 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 2] = value >> 16 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 3] = value >> 24 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 4] = value >> 32 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 5] = value >> 40 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 6] = value >> 48 & 0xFF;
  BX_GEFORCE_THIS s.memory[address + 7] = value >> 56 & 0xFF;
}

Bit8u bx_geforce_c::ramin_read8(Bit32u address)
{
  return vram_read8(address ^ BX_GEFORCE_THIS ramin_flip);
}

Bit32u bx_geforce_c::ramin_read32(Bit32u address)
{
  return vram_read32(address ^ BX_GEFORCE_THIS ramin_flip);
}

void bx_geforce_c::ramin_write8(Bit32u address, Bit8u value)
{
  vram_write8(address ^ BX_GEFORCE_THIS ramin_flip, value);
}

void bx_geforce_c::ramin_write32(Bit32u address, Bit32u value)
{
  vram_write32(address ^ BX_GEFORCE_THIS ramin_flip, value);
}

Bit8u bx_geforce_c::physical_read8(Bit32u address)
{
  Bit8u data;
  DEV_MEM_READ_PHYSICAL(address, 1, &data);
  return data;
}

Bit16u bx_geforce_c::physical_read16(Bit32u address)
{
  Bit8u data[2];
  DEV_MEM_READ_PHYSICAL(address, 2, data);
  return data[0] << 0 | data[1] << 8;
}

Bit32u bx_geforce_c::physical_read32(Bit32u address)
{
  Bit8u data[4];
  DEV_MEM_READ_PHYSICAL(address, 4, data);
  return data[0] << 0 | data[1] << 8 | data[2] << 16 | data[3] << 24;
}

void bx_geforce_c::physical_write8(Bit32u address, Bit8u value)
{
  DEV_MEM_WRITE_PHYSICAL(address, 1, &value);
}

void bx_geforce_c::physical_write16(Bit32u address, Bit16u value)
{
  Bit8u data[2];
  data[0] = value >> 0 & 0xFF;
  data[1] = value >> 8 & 0xFF;
  DEV_MEM_WRITE_PHYSICAL(address, 2, data);
}

void bx_geforce_c::physical_write32(Bit32u address, Bit32u value)
{
  Bit8u data[4];
  data[0] = value >> 0 & 0xFF;
  data[1] = value >> 8 & 0xFF;
  data[2] = value >> 16 & 0xFF;
  data[3] = value >> 24 & 0xFF;
  DEV_MEM_WRITE_PHYSICAL(address, 4, data);
}

void bx_geforce_c::physical_write64(Bit32u address, Bit64u value)
{
  Bit8u data[8];
  data[0] = value >> 0 & 0xFF;
  data[1] = value >> 8 & 0xFF;
  data[2] = value >> 16 & 0xFF;
  data[3] = value >> 24 & 0xFF;
  data[4] = value >> 32 & 0xFF;
  data[5] = value >> 40 & 0xFF;
  data[6] = value >> 48 & 0xFF;
  data[7] = value >> 56 & 0xFF;
  DEV_MEM_WRITE_PHYSICAL(address, 8, data);
}

Bit32u bx_geforce_c::dma_pt_lookup(Bit32u object, Bit32u address)
{
  Bit32u address_adj = address + (ramin_read32(object) >> 20);
  Bit32u page_offset = address_adj & 0xFFF;
  Bit32u page_index = address_adj >> 12;
  Bit32u page = ramin_read32(object + 8 + page_index * 4) & 0xFFFFF000;
  return page | page_offset;
}

Bit32u bx_geforce_c::dma_lin_lookup(Bit32u object, Bit32u address)
{
  Bit32u adjust = ramin_read32(object) >> 20;
  Bit32u base = ramin_read32(object + 8) & 0xFFFFF000;
  return base + adjust + address;
}

Bit8u bx_geforce_c::dma_read8(Bit32u object, Bit32u address)
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
    return vram_read8(addr_abs);
}

Bit16u bx_geforce_c::dma_read16(Bit32u object, Bit32u address)
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
    return vram_read16(addr_abs);
}

Bit32u bx_geforce_c::dma_read32(Bit32u object, Bit32u address)
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
    return vram_read32(addr_abs);
}

void bx_geforce_c::dma_write8(Bit32u object, Bit32u address, Bit8u value)
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

void bx_geforce_c::dma_write16(Bit32u object, Bit32u address, Bit16u value)
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

void bx_geforce_c::dma_write32(Bit32u object, Bit32u address, Bit32u value)
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

void bx_geforce_c::dma_write64(Bit32u object, Bit32u address, Bit64u value)
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

void bx_geforce_c::dma_copy(Bit32u dst_obj, Bit32u dst_addr,
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
      memcpy(buffer, BX_GEFORCE_THIS s.memory + src_addr_abs, chunk_bytes);
    if (dst_flags & 0x00020000)
      DEV_MEM_WRITE_PHYSICAL(dst_addr_abs, chunk_bytes, buffer);
    else
      memcpy(BX_GEFORCE_THIS s.memory + dst_addr_abs, buffer, chunk_bytes);
    dst_addr += chunk_bytes;
    src_addr += chunk_bytes;
    bytes_left -= chunk_bytes;
  }
}

Bit32u bx_geforce_c::ramfc_address(Bit32u chid, Bit32u offset)
{
  Bit32u ramfc;
  if (BX_GEFORCE_THIS card_type < 0x40)
    ramfc = (BX_GEFORCE_THIS fifo_ramfc & 0xFFF) << 8;
  else
    ramfc = (BX_GEFORCE_THIS fifo_ramfc & 0xFFF) << 16;
  Bit32u ramfc_ch_size = BX_GEFORCE_THIS card_type < 0x40 ? 0x40 : 0x80;
  return ramfc + chid * ramfc_ch_size + offset;
}

void bx_geforce_c::ramfc_write32(Bit32u chid, Bit32u offset, Bit32u value)
{
  ramin_write32(ramfc_address(chid, offset), value);
}

Bit32u bx_geforce_c::ramfc_read32(Bit32u chid, Bit32u offset)
{
  return ramin_read32(ramfc_address(chid, offset));
}

void bx_geforce_c::ramht_lookup(Bit32u handle, Bit32u chid, Bit32u* object, Bit8u* engine)
{
  Bit32u ramht_addr = (BX_GEFORCE_THIS fifo_ramht & 0xFFF) << 8;
  Bit32u ramht_bits = (BX_GEFORCE_THIS fifo_ramht >> 16 & 0xFF) + 9;
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
  Bit32u steps = 1; // for debugging
  do {
    if (ramin_read32(ramht_addr + it) == handle) {
      Bit32u context = ramin_read32(ramht_addr + it + 4);
      Bit32u ctx_chid;
      if (BX_GEFORCE_THIS card_type < 0x40)
        ctx_chid = context >> 24 & 0x1F;
      else
        ctx_chid = context >> 23 & 0x1F;
      if (chid == ctx_chid) {
        BX_DEBUG(("ramht_lookup: 0x%08x -> 0x%08x, steps: %d", handle, context, steps));
        if (object) {
          if (BX_GEFORCE_THIS card_type < 0x40)
            *object = (context & 0xFFFF) << 4;
          else
            *object = (context & 0xFFFFF) << 4;
        }
        if (engine) {
          if (BX_GEFORCE_THIS card_type < 0x40)
            *engine = context >> 16 & 0xFF;
          else
            *engine = context >> 20 & 0x7;
        }
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

Bit32u color_565_to_888(Bit16u value)
{
  Bit8u r, g, b;
  EXTRACT_565_TO_888(value, r, g, b);
  return r << 16 | g << 8 | b;
}

Bit16u color_888_to_565(Bit32u value)
{
  return ((value >> 19 & 0x1F) << 11) | ((value >> 10 & 0x3F) << 5) | (value >> 3 & 0x1F);
}


Bit32u bx_geforce_c::get_pixel(Bit32u obj, Bit32u ofs, Bit32u x, Bit32u cb)
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

void bx_geforce_c::put_pixel(Bit32u chid, Bit32u ofs, Bit32u x, Bit32u value)
{
  if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 1)
    dma_write8(BX_GEFORCE_THIS chs[chid].s2d_img_dst, ofs + x, value);
  else if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
    dma_write16(BX_GEFORCE_THIS chs[chid].s2d_img_dst, ofs + x * 2, value);
  else if (BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 6)
    dma_write32(BX_GEFORCE_THIS chs[chid].s2d_img_dst, ofs + x * 4, value & 0x00FFFFFF);
  else
    dma_write32(BX_GEFORCE_THIS chs[chid].s2d_img_dst, ofs + x * 4, value);
}

void bx_geforce_c::pixel_operation(Bit32u chid, Bit32u op,
  Bit32u* dstcolor, const Bit32u* srccolor, Bit32u cb, Bit32u px, Bit32u py)
{
  if (op == 1) {
    Bit8u rop = BX_GEFORCE_THIS chs[chid].rop;
    if (BX_GEFORCE_THIS rop_flags[rop]) {
      Bit32u i = py % 8 * 8 + px % 8;
      Bit32u patt_color = BX_GEFORCE_THIS chs[chid].patt_type == 1 ?
        BX_GEFORCE_THIS chs[chid].patt_data_mono[i] ?
          BX_GEFORCE_THIS chs[chid].patt_fg_color : BX_GEFORCE_THIS chs[chid].patt_bg_color :
        BX_GEFORCE_THIS chs[chid].patt_data_color[i];
      bx_ternary_rop(rop, (Bit8u*)dstcolor, (Bit8u*)srccolor, (Bit8u*)&patt_color, cb);
    } else {
      BX_GEFORCE_THIS rop_handler[rop]((Bit8u*)dstcolor, (Bit8u*)srccolor,
        0, 0, cb, 1);
    }
  } else if (op == 5) {
    if (cb == 4) {
      if (*srccolor) {
        Bit8u sb = *srccolor;
        Bit8u sg = *srccolor >> 8;
        Bit8u sr = *srccolor >> 16;
        Bit8u sa = *srccolor >> 24;
        Bit32u beta = BX_GEFORCE_THIS chs[chid].beta;
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
        Bit8u b = alpha_wrap(db * isa / 0xFF + sb);
        Bit8u g = alpha_wrap(dg * isa / 0xFF + sg);
        Bit8u r = alpha_wrap(dr * isa / 0xFF + sr);
        Bit8u a = alpha_wrap(da * isa / 0xFF + sa);
        *dstcolor = b << 0 | g << 8 | r << 16 | a << 24;
      }
    } else {
      Bit32u beta = BX_GEFORCE_THIS chs[chid].beta;
      Bit8u bb = beta;
      Bit8u bg = beta >> 8;
      Bit8u br = beta >> 16;
      Bit8u iba = 0xFF - (beta >> 24);
      Bit8u sb = *srccolor & 0x1F;
      Bit8u sg = *srccolor >> 5 & 0x3F;
      Bit8u sr = *srccolor >> 11 & 0x1F;
      Bit8u db = *dstcolor & 0x1F;
      Bit8u dg = *dstcolor >> 5 & 0x3F;
      Bit8u dr = *dstcolor >> 11 & 0x1F;
      Bit8u b = (db * iba + sb * bb) / 0xFF;
      Bit8u g = (dg * iba + sg * bg) / 0xFF;
      Bit8u r = (dr * iba + sr * br) / 0xFF;
      *dstcolor = b << 0 | g << 5 | r << 11;
    }
  } else {
    *dstcolor = *srccolor;
  }
}

void bx_geforce_c::gdi_fillrect(Bit32u chid, bool clipped)
{
  Bit16s clipx0;
  Bit16s clipy0;
  Bit16s clipx1;
  Bit16s clipy1;
  if (clipped) {
    clipx0 = BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 & 0xFFFF;
    clipy0 = BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 >> 16;
    clipx1 = BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 & 0xFFFF;
    clipy1 = BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 >> 16;
  }
  Bit16s dx;
  Bit16s dy;
  if (clipped) {
    dx = BX_GEFORCE_THIS chs[chid].gdi_rect_yx0 & 0xFFFF;
    dy = BX_GEFORCE_THIS chs[chid].gdi_rect_yx0 >> 16;
    clipx0 -= dx;
    clipy0 -= dy;
    clipx1 -= dx;
    clipy1 -= dy;
  } else {
    dx = BX_GEFORCE_THIS chs[chid].gdi_rect_xy >> 16;
    dy = BX_GEFORCE_THIS chs[chid].gdi_rect_xy & 0xFFFF;
  }
  Bit16u width;
  Bit16u height;
  if (clipped) {
    width = (BX_GEFORCE_THIS chs[chid].gdi_rect_yx1 & 0xFFFF) - dx;
    height = (BX_GEFORCE_THIS chs[chid].gdi_rect_yx1 >> 16) - dy;
  } else {
    width = BX_GEFORCE_THIS chs[chid].gdi_rect_wh >> 16;
    height = BX_GEFORCE_THIS chs[chid].gdi_rect_wh & 0xFFFF;
  }
  Bit32u pitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u srccolor = BX_GEFORCE_THIS chs[chid].gdi_rect_color;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * pitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < width; x++) {
      if (!clipped || x >= clipx0 && x < clipx1 && y >= clipy0 && y < clipy1) {
        Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
          draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
        pixel_operation(chid, BX_GEFORCE_THIS chs[chid].gdi_operation,
          &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].s2d_color_bytes, dx + x, dy + y);
        put_pixel(chid, draw_offset, x, dstcolor);
      }
    }
    draw_offset += pitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, width, height);
}

void bx_geforce_c::gdi_blit(Bit32u chid, Bit32u type)
{
  Bit16s dx = BX_GEFORCE_THIS chs[chid].gdi_image_xy & 0xFFFF;
  Bit16s dy = BX_GEFORCE_THIS chs[chid].gdi_image_xy >> 16;
  Bit16s clipx0 = (BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 & 0xFFFF) - dx;
  Bit16s clipy0 = (BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 >> 16) - dy;
  Bit16s clipx1 = (BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 & 0xFFFF) - dx;
  Bit16s clipy1 = (BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 >> 16) - dy;
  Bit32u swidth = BX_GEFORCE_THIS chs[chid].gdi_image_swh & 0xFFFF;
  Bit32u dwidth = type ? BX_GEFORCE_THIS chs[chid].gdi_image_dwh & 0xFFFF : swidth;
  Bit32u height = BX_GEFORCE_THIS chs[chid].gdi_image_swh >> 16;
  Bit32u pitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u bg_color = BX_GEFORCE_THIS chs[chid].gdi_bg_color;
  Bit32u fg_color = BX_GEFORCE_THIS chs[chid].gdi_fg_color;
  if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 4 && BX_GEFORCE_THIS chs[chid].gdi_color_fmt != 3) {
    bg_color = color_565_to_888(bg_color);
    fg_color = color_565_to_888(fg_color);
  }
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * pitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  Bit32u bit_index = 0;
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < dwidth; x++) {
      if (x >= clipx0 && x < clipx1 && y >= clipy0 && y < clipy1) {
        Bit32u word_offset = bit_index / 32;
        Bit32u bit_offset = bit_index % 32;
        bit_offset = bit_offset & 24 | 7 - (bit_offset & 7);
        bool pixel = BX_GEFORCE_THIS chs[chid].gdi_words[word_offset] >> bit_offset & 1;
        if (type || !type && pixel) {
          Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
            draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
          Bit32u srccolor = pixel ? fg_color : bg_color;
          pixel_operation(chid, BX_GEFORCE_THIS chs[chid].gdi_operation,
            &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].s2d_color_bytes, dx + x, dy + y);
          put_pixel(chid, draw_offset, x, dstcolor);
        }
      }
      bit_index++;
    }
    bit_index += swidth - dwidth;
    draw_offset += pitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, dwidth, height);
}

void bx_geforce_c::ifc(Bit32u chid)
{
  Bit16u dx = BX_GEFORCE_THIS chs[chid].ifc_yx & 0xFFFF;
  Bit16u dy = BX_GEFORCE_THIS chs[chid].ifc_yx >> 16;
  Bit32u swidth = BX_GEFORCE_THIS chs[chid].ifc_shw & 0xFFFF;
  Bit32u dwidth = BX_GEFORCE_THIS chs[chid].ifc_dhw & 0xFFFF;
  Bit32u height = BX_GEFORCE_THIS chs[chid].ifc_dhw >> 16;
  Bit32u pitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * pitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  Bit32u word_offset = 0;
  Bit32u chromacolor;
  if (BX_GEFORCE_THIS chs[chid].ifc_color_key_enable) {
    if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 4)
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x00FFFFFF;
    else if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 2)
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x0000FFFF;
    else
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x000000FF;
  }
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < dwidth; x++) {
      Bit32u srccolor;
      if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 4) {
        srccolor = BX_GEFORCE_THIS chs[chid].ifc_words[word_offset];
      } else if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 2) {
        Bit16u *ifc_words16 = (Bit16u*)BX_GEFORCE_THIS chs[chid].ifc_words;
        srccolor = ifc_words16[word_offset];
      } else {
        Bit8u *ifc_words8 = (Bit8u*)BX_GEFORCE_THIS chs[chid].ifc_words;
        srccolor = ifc_words8[word_offset];
      }
      if (!BX_GEFORCE_THIS chs[chid].ifc_color_key_enable || srccolor != chromacolor) {
        Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
          draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
        if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 4 &&
            BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
          dstcolor = color_565_to_888(dstcolor);
        pixel_operation(chid, BX_GEFORCE_THIS chs[chid].ifc_operation,
          &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].ifc_color_bytes, dx + x, dy + y);
        if (BX_GEFORCE_THIS chs[chid].ifc_color_bytes == 4 &&
            BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
          dstcolor = color_888_to_565(dstcolor);
        put_pixel(chid, draw_offset, x, dstcolor);
      }
      word_offset++;
    }
    word_offset += swidth - dwidth;
    draw_offset += pitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, dwidth, height);
}

void bx_geforce_c::iifc(Bit32u chid)
{
  Bit16s dx = BX_GEFORCE_THIS chs[chid].iifc_yx & 0xFFFF;
  Bit16s dy = BX_GEFORCE_THIS chs[chid].iifc_yx >> 16;
  Bit16s clipx0 = (BX_GEFORCE_THIS chs[chid].clip_yx & 0xFFFF) - dx;
  Bit16s clipy0 = (BX_GEFORCE_THIS chs[chid].clip_yx >> 16) - dy;
  Bit16s clipx1 = clipx0 + (BX_GEFORCE_THIS chs[chid].clip_hw & 0xFFFF);
  Bit16s clipy1 = clipy0 + (BX_GEFORCE_THIS chs[chid].clip_hw >> 16);
  Bit32u swidth = BX_GEFORCE_THIS chs[chid].iifc_shw & 0xFFFF;
  Bit32u dwidth = BX_GEFORCE_THIS chs[chid].iifc_dhw & 0xFFFF;
  Bit32u height = BX_GEFORCE_THIS chs[chid].iifc_dhw >> 16;
  Bit32u pitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * pitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  Bit32u symbol_index = 0;
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < dwidth; x++) {
      if (x >= clipx0 && x < clipx1 && y >= clipy0 && y < clipy1) {
        Bit8u symbol;
        if (BX_GEFORCE_THIS chs[chid].iifc_bpp4) {
          Bit32u word_offset = symbol_index / 8;
          Bit32u symbol_offset = (symbol_index % 8 ^ 1) * 4;
          symbol = BX_GEFORCE_THIS chs[chid].iifc_words[word_offset] >> symbol_offset & 0xF;
        } else {
          Bit32u word_offset = symbol_index / 4;
          Bit32u symbol_offset = symbol_index % 4 * 8;
          symbol = BX_GEFORCE_THIS chs[chid].iifc_words[word_offset] >> symbol_offset & 0xFF;
        }
        Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
          draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
        if (BX_GEFORCE_THIS chs[chid].iifc_color_bytes == 4) {
          Bit32u srccolor = dma_read32(BX_GEFORCE_THIS chs[chid].iifc_palette,
            BX_GEFORCE_THIS chs[chid].iifc_palette_ofs + symbol * 4);
          if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
            dstcolor = color_565_to_888(dstcolor);
          pixel_operation(chid, BX_GEFORCE_THIS chs[chid].iifc_operation,
            &dstcolor, &srccolor, 4, dx + x, dy + y);
          if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
            dstcolor = color_888_to_565(dstcolor);
        } else if (BX_GEFORCE_THIS chs[chid].iifc_color_bytes == 2) {
          Bit32u srccolor = dma_read16(BX_GEFORCE_THIS chs[chid].iifc_palette,
            BX_GEFORCE_THIS chs[chid].iifc_palette_ofs + symbol * 2);
          pixel_operation(chid, BX_GEFORCE_THIS chs[chid].iifc_operation,
            &dstcolor, &srccolor, 2, dx + x, dy + y);
        }
        put_pixel(chid, draw_offset, x, dstcolor);
      }
      symbol_index++;
    }
    symbol_index += swidth - dwidth;
    draw_offset += pitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, dwidth, height);
}

void bx_geforce_c::sifc(Bit32u chid)
{
  Bit16u dx = BX_GEFORCE_THIS chs[chid].sifc_clip_yx & 0xFFFF;
  Bit16u dy = BX_GEFORCE_THIS chs[chid].sifc_clip_yx >> 16;
  Bit32u dsdx = (Bit32u)(BX_CONST64(1099511627776) / BX_GEFORCE_THIS chs[chid].sifc_dxds);
  Bit32u dtdy = (Bit32u)(BX_CONST64(1099511627776) / BX_GEFORCE_THIS chs[chid].sifc_dydt);
  Bit32u swidth = BX_GEFORCE_THIS chs[chid].sifc_shw & 0xFFFF;
  Bit32u dwidth = BX_GEFORCE_THIS chs[chid].sifc_clip_hw & 0xFFFF;
  Bit32u height = BX_GEFORCE_THIS chs[chid].sifc_clip_hw >> 16;
  Bit32u pitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * pitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  Bit32s sx0 = ((BX_GEFORCE_THIS chs[chid].sifc_syx & 0xFFFF) << 16) - (dx << 20) - 0x80000;
  Bit32s sy = (BX_GEFORCE_THIS chs[chid].sifc_syx & 0xFFFF0000) - (dy << 20) - 0x80000;
  if (sx0 < 0)
    sx0 = 0;
  if (sy < 0)
    sy = 0;
  Bit32u symbol_offset_y = 0;
  for (Bit16u y = 0; y < height; y++) {
    Bit32u sx = sx0;
    for (Bit16u x = 0; x < dwidth; x++) {
      Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
        draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
      Bit32u srccolor;
      Bit32u symbol_offset = symbol_offset_y + (sx >> 20);
      if (BX_GEFORCE_THIS chs[chid].sifc_color_bytes == 4) {
        srccolor = BX_GEFORCE_THIS chs[chid].sifc_words[symbol_offset];
      } else if (BX_GEFORCE_THIS chs[chid].sifc_color_bytes == 2) {
        Bit16u *sifc_words16 = (Bit16u*)BX_GEFORCE_THIS chs[chid].sifc_words;
        srccolor = sifc_words16[symbol_offset];
      } else {
        Bit8u *sifc_words8 = (Bit8u*)BX_GEFORCE_THIS chs[chid].sifc_words;
        srccolor = sifc_words8[symbol_offset];
      }
      if (BX_GEFORCE_THIS chs[chid].sifc_color_bytes == 4 &&
          BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
        dstcolor = color_565_to_888(dstcolor);
      pixel_operation(chid, BX_GEFORCE_THIS chs[chid].sifc_operation,
        &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].sifc_color_bytes, dx + x, dy + y);
      if (BX_GEFORCE_THIS chs[chid].sifc_color_bytes == 4 &&
          BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
        dstcolor = color_888_to_565(dstcolor);
      put_pixel(chid, draw_offset, x, dstcolor);
      sx += dsdx;
    }
    sy += dtdy;
    symbol_offset_y = (sy >> 20) * swidth;
    draw_offset += pitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, dwidth, height);
}

void bx_geforce_c::copyarea(Bit32u chid)
{
  Bit16u sx = BX_GEFORCE_THIS chs[chid].blit_syx & 0xFFFF;
  Bit16u sy = BX_GEFORCE_THIS chs[chid].blit_syx >> 16;
  Bit16u dx = BX_GEFORCE_THIS chs[chid].blit_dyx & 0xFFFF;
  Bit16u dy = BX_GEFORCE_THIS chs[chid].blit_dyx >> 16;
  Bit16u width = BX_GEFORCE_THIS chs[chid].blit_hw & 0xFFFF;
  Bit16u height = BX_GEFORCE_THIS chs[chid].blit_hw >> 16;
  Bit32u spitch = BX_GEFORCE_THIS chs[chid].s2d_pitch & 0xFFFF;
  Bit32u dpitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u src_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_src;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst;
  bool xdir = dx > sx;
  bool ydir = dy > sy;
  src_offset += (sy + ydir * (height - 1)) * spitch + sx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) +
    dy * dpitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  draw_offset += (dy + ydir * (height - 1)) * dpitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u chromacolor;
  if (BX_GEFORCE_THIS chs[chid].blit_color_key_enable) {
    if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 4)
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x00FFFFFF;
    else if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 2)
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x0000FFFF;
    else
      chromacolor = BX_GEFORCE_THIS chs[chid].chroma_color & 0x000000FF;
  }
  for (Bit16u y = 0; y < height; y++) {
    for (Bit16u x = 0; x < width; x++) {
      Bit16u xa = xdir ? width - x - 1 : x;
      Bit32u srccolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_src,
        src_offset, xa, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
      if (!BX_GEFORCE_THIS chs[chid].blit_color_key_enable || srccolor != chromacolor) {
        Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
          draw_offset, xa, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
        pixel_operation(chid, BX_GEFORCE_THIS chs[chid].blit_operation,
          &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].s2d_color_bytes, dx + x, dy + y);
        put_pixel(chid, draw_offset, xa, dstcolor);
      }
    }
    src_offset += spitch * (1 - 2 * ydir);
    draw_offset += dpitch * (1 - 2 * ydir);
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, width, height);
}

void bx_geforce_c::m2mf(Bit32u chid)
{
  Bit32u src_offset = BX_GEFORCE_THIS chs[chid].m2mf_src_offset;
  Bit32u dst_offset = BX_GEFORCE_THIS chs[chid].m2mf_dst_offset;
  for (Bit16u y = 0; y < BX_GEFORCE_THIS chs[chid].m2mf_line_count; y++) {
    dma_copy(
      BX_GEFORCE_THIS chs[chid].m2mf_dst, dst_offset,
      BX_GEFORCE_THIS chs[chid].m2mf_src, src_offset,
      BX_GEFORCE_THIS chs[chid].m2mf_line_length);
    src_offset += BX_GEFORCE_THIS chs[chid].m2mf_src_pitch;
    dst_offset += BX_GEFORCE_THIS chs[chid].m2mf_dst_pitch;
  }
  Bit32u dma_target = ramin_read32(BX_GEFORCE_THIS chs[chid].m2mf_dst) >> 12 & 0xFF;
  if (dma_target == 0x03 || dma_target == 0x0b) {
    Bit32u redraw_offset = dma_lin_lookup(
      BX_GEFORCE_THIS chs[chid].m2mf_dst, BX_GEFORCE_THIS chs[chid].m2mf_dst_offset);
    Bit32u width = BX_GEFORCE_THIS chs[chid].m2mf_line_length / (BX_GEFORCE_THIS svga_bpp >> 3);
    BX_GEFORCE_THIS redraw_area(redraw_offset, width, BX_GEFORCE_THIS chs[chid].m2mf_line_count);
  }
}

void bx_geforce_c::sifm(Bit32u chid)
{
  Bit16u sx = (BX_GEFORCE_THIS chs[chid].sifm_syx & 0xFFFF) >> 4;
  Bit16u sy = (BX_GEFORCE_THIS chs[chid].sifm_syx >> 16) >> 4;
  Bit16u dx = BX_GEFORCE_THIS chs[chid].sifm_dyx & 0xFFFF;
  Bit16u dy = BX_GEFORCE_THIS chs[chid].sifm_dyx >> 16;
  Bit16u swidth = BX_GEFORCE_THIS chs[chid].sifm_shw & 0xFFFF;
  Bit16u sheight = BX_GEFORCE_THIS chs[chid].sifm_shw >> 16;
  Bit16u dwidth = BX_GEFORCE_THIS chs[chid].sifm_dhw & 0xFFFF;
  Bit16u dheight = BX_GEFORCE_THIS chs[chid].sifm_dhw >> 16;
  Bit32u spitch = BX_GEFORCE_THIS chs[chid].sifm_sfmt & 0xFFFF;
  Bit32u dpitch = BX_GEFORCE_THIS chs[chid].s2d_pitch >> 16;
  Bit32u src_offset = BX_GEFORCE_THIS chs[chid].sifm_sofs +
    sy * spitch + sx * BX_GEFORCE_THIS chs[chid].sifm_color_bytes;
  Bit32u draw_offset = BX_GEFORCE_THIS chs[chid].s2d_ofs_dst +
    dy * dpitch + dx * BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
  Bit32u redraw_offset = dma_lin_lookup(BX_GEFORCE_THIS chs[chid].s2d_img_dst, draw_offset) -
    (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
  if (BX_GEFORCE_THIS chs[chid].sifm_dudx != 0x00100000 ||
      BX_GEFORCE_THIS chs[chid].sifm_dvdy != 0x00100000)
    BX_ERROR(("SIFM scaling is not implemented"));
  for (Bit16u y = 0; y < dheight; y++) {
    for (Bit16u x = 0; x < dwidth; x++) {
      Bit32u dstcolor = get_pixel(BX_GEFORCE_THIS chs[chid].s2d_img_dst,
        draw_offset, x, BX_GEFORCE_THIS chs[chid].s2d_color_bytes);
      Bit32u srccolor = get_pixel(BX_GEFORCE_THIS chs[chid].sifm_src,
        src_offset, x, BX_GEFORCE_THIS chs[chid].sifm_color_bytes);
      if (BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 4)
        srccolor |= 0xFF000000;
      pixel_operation(chid, BX_GEFORCE_THIS chs[chid].sifm_operation,
        &dstcolor, &srccolor, BX_GEFORCE_THIS chs[chid].s2d_color_bytes, dx + x, dy + y);
      put_pixel(chid, draw_offset, x, dstcolor);
    }
    src_offset += spitch;
    draw_offset += dpitch;
  }
  BX_GEFORCE_THIS redraw_area(redraw_offset, dwidth, dheight);
}

void bx_geforce_c::execute_clip(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].clip_yx = param;
  else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].clip_hw = param;
}

void bx_geforce_c::execute_m2mf(Bit32u chid, Bit32u subc, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].m2mf_src = param;
  else if (method == 0x062)
    BX_GEFORCE_THIS chs[chid].m2mf_dst = param;
  else if (method == 0x0c3)
    BX_GEFORCE_THIS chs[chid].m2mf_src_offset = param;
  else if (method == 0x0c4)
    BX_GEFORCE_THIS chs[chid].m2mf_dst_offset = param;
  else if (method == 0x0c5)
    BX_GEFORCE_THIS chs[chid].m2mf_src_pitch = param;
  else if (method == 0x0c6)
    BX_GEFORCE_THIS chs[chid].m2mf_dst_pitch = param;
  else if (method == 0x0c7)
    BX_GEFORCE_THIS chs[chid].m2mf_line_length = param;
  else if (method == 0x0c8)
    BX_GEFORCE_THIS chs[chid].m2mf_line_count = param;
  else if (method == 0x0c9)
    BX_GEFORCE_THIS chs[chid].m2mf_format = param;
  else if (method == 0x0ca) {
    BX_GEFORCE_THIS chs[chid].m2mf_buffer_notify = param;
    m2mf(chid);
    if ((ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier) & 0xFF) == 0x30) {
      BX_DEBUG(("M2MF notify skipped"));
    } else {
      BX_DEBUG(("M2MF notify 0x%08x",
        BX_GEFORCE_THIS chs[chid].schs[subc].notifier));
      dma_write64(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0x10 + 0x0, get_current_time());
      dma_write32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0x10 + 0x8, 0);
      dma_write32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0x10 + 0xC, 0);
    }
  }
}

void bx_geforce_c::execute_rop(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].rop = param;
}

void bx_geforce_c::execute_patt(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0c2)
    BX_GEFORCE_THIS chs[chid].patt_shape = param;
  else if (method == 0x0c3)
    BX_GEFORCE_THIS chs[chid].patt_type = param;
  else if (method == 0x0c4)
    BX_GEFORCE_THIS chs[chid].patt_bg_color = param;
  else if (method == 0x0c5)
    BX_GEFORCE_THIS chs[chid].patt_fg_color = param;
  else if (method == 0x0c6 || method == 0x0c7) {
    for (Bit32u i = 0; i < 32; i++)
      BX_GEFORCE_THIS chs[chid].patt_data_mono[i + (method & 1) * 32] = 1 << (i ^ 7) & param;
  } else if (method >= 0x100 && method < 0x110) {
    Bit32u i = (method - 0x100) * 4;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i] = param & 0xFF;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i + 1] = param >> 8 & 0xFF;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i + 2] = param >> 16 & 0xFF;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i + 3] = param >> 24;
  } else if (method >= 0x140 && method < 0x160) {
    Bit32u i = (method - 0x140) * 2;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i] = param & 0xFFFF;
    BX_GEFORCE_THIS chs[chid].patt_data_color[i + 1] = param >> 16;
  } else if (method >= 0x1c0 && method < 0x200)
    BX_GEFORCE_THIS chs[chid].patt_data_color[method - 0x1c0] = param;
}

void bx_geforce_c::execute_gdi(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0bf)
    BX_GEFORCE_THIS chs[chid].gdi_operation = param;
  else if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].gdi_color_fmt = param;
  else if (method == 0x0ff)
    BX_GEFORCE_THIS chs[chid].gdi_rect_color = param;
  else if (method >= 0x100 && method < 0x140) {
    if (method & 1) {
      BX_GEFORCE_THIS chs[chid].gdi_rect_wh = param;
      gdi_fillrect(chid, false);
    } else
      BX_GEFORCE_THIS chs[chid].gdi_rect_xy = param;
  } else if (method == 0x17d)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 = param;
  else if (method == 0x17e)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 = param;
  else if (method == 0x17f)
    BX_GEFORCE_THIS chs[chid].gdi_rect_color = param;
  else if (method >= 0x180 && method < 0x1c0) {
    if (method & 1) {
      BX_GEFORCE_THIS chs[chid].gdi_rect_yx1 = param;
      gdi_fillrect(chid, true);
    } else
      BX_GEFORCE_THIS chs[chid].gdi_rect_yx0 = param;
  } else if (method == 0x1fd)
    BX_GEFORCE_THIS chs[chid].gdi_fg_color = param;
  else if (method == 0x1fe)
    BX_GEFORCE_THIS chs[chid].gdi_image_swh = param;
  else if (method == 0x1ff) {
    BX_GEFORCE_THIS chs[chid].gdi_image_xy = param;
    Bit32u width = BX_GEFORCE_THIS chs[chid].gdi_image_swh & 0xFFFF;
    Bit32u height = BX_GEFORCE_THIS chs[chid].gdi_image_swh >> 16;
    Bit32u wordCount = ALIGN(width * height, 32) >> 5;
    if (BX_GEFORCE_THIS chs[chid].gdi_words != nullptr)
      delete[] BX_GEFORCE_THIS chs[chid].gdi_words;
    BX_GEFORCE_THIS chs[chid].gdi_words_ptr = 0;
    BX_GEFORCE_THIS chs[chid].gdi_words_left = wordCount;
    BX_GEFORCE_THIS chs[chid].gdi_words = new Bit32u[wordCount];
  } else if (method >= 0x200 && method < 0x280) {
    BX_GEFORCE_THIS chs[chid].gdi_words[
      BX_GEFORCE_THIS chs[chid].gdi_words_ptr++] = param;
    BX_GEFORCE_THIS chs[chid].gdi_words_left--;
    if (!BX_GEFORCE_THIS chs[chid].gdi_words_left) {
      gdi_blit(chid, 0);
      delete[] BX_GEFORCE_THIS chs[chid].gdi_words;
      BX_GEFORCE_THIS chs[chid].gdi_words = nullptr;
    }
  } else if (method == 0x2f9)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 = param;
  else if (method == 0x2fa)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 = param;
  else if (method == 0x2fb)
    BX_GEFORCE_THIS chs[chid].gdi_bg_color = param;
  else if (method == 0x2fc)
    BX_GEFORCE_THIS chs[chid].gdi_fg_color = param;
  else if (method == 0x2fd)
    BX_GEFORCE_THIS chs[chid].gdi_image_swh = param;
  else if (method == 0x2fe)
    BX_GEFORCE_THIS chs[chid].gdi_image_dwh = param;
  else if (method == 0x2ff) {
    BX_GEFORCE_THIS chs[chid].gdi_image_xy = param;
    Bit32u width = BX_GEFORCE_THIS chs[chid].gdi_image_swh & 0xFFFF;
    Bit32u height = BX_GEFORCE_THIS chs[chid].gdi_image_swh >> 16;
    Bit32u wordCount = ALIGN(width * height, 32) >> 5;
    if (BX_GEFORCE_THIS chs[chid].gdi_words != nullptr)
      delete[] BX_GEFORCE_THIS chs[chid].gdi_words;
    BX_GEFORCE_THIS chs[chid].gdi_words_ptr = 0;
    BX_GEFORCE_THIS chs[chid].gdi_words_left = wordCount;
    BX_GEFORCE_THIS chs[chid].gdi_words = new Bit32u[wordCount];
  } else if (method >= 0x300 && method < 0x380) {
    BX_GEFORCE_THIS chs[chid].gdi_words[
      BX_GEFORCE_THIS chs[chid].gdi_words_ptr++] = param;
    BX_GEFORCE_THIS chs[chid].gdi_words_left--;
    if (!BX_GEFORCE_THIS chs[chid].gdi_words_left) {
      gdi_blit(chid, 1);
      delete[] BX_GEFORCE_THIS chs[chid].gdi_words;
      BX_GEFORCE_THIS chs[chid].gdi_words = nullptr;
    }
  } else if (method == 0x3fd)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx0 = param;
  else if (method == 0x3fe)
    BX_GEFORCE_THIS chs[chid].gdi_clip_yx1 = param;
  else if (method == 0x3ff)
    BX_GEFORCE_THIS chs[chid].gdi_fg_color = param;
}

void bx_geforce_c::execute_chroma(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].chroma_color_fmt = param;
  else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].chroma_color = param;
}

void bx_geforce_c::execute_imageblit(Bit32u chid, Bit8u cls, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].blit_color_key_enable = (ramin_read32(param) & 0xFF) != 0x30;
  else if (method == 0x0bf)
    BX_GEFORCE_THIS chs[chid].blit_operation = param;
  else if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].blit_syx = param;
  else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].blit_dyx = param;
  else if (method == 0x0c2) {
    BX_GEFORCE_THIS chs[chid].blit_hw = param;
    copyarea(chid);
  }
}

void bx_geforce_c::execute_ifc(Bit32u chid, Bit8u cls, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].ifc_color_key_enable = (ramin_read32(param) & 0xFF) != 0x30;
  else if (method == 0x0bf)
    BX_GEFORCE_THIS chs[chid].ifc_operation = param;
  else if (method == 0x0c0) {
    BX_GEFORCE_THIS chs[chid].ifc_color_fmt = param;
    update_color_bytes_ifc(chid);
  } else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].ifc_yx = param;
  else if (method == 0x0c2)
    BX_GEFORCE_THIS chs[chid].ifc_dhw = param;
  else if (method == 0x0c3) {
    BX_GEFORCE_THIS chs[chid].ifc_shw = param;
    Bit32u width = BX_GEFORCE_THIS chs[chid].ifc_shw & 0xFFFF;
    Bit32u height = BX_GEFORCE_THIS chs[chid].ifc_shw >> 16;
    Bit32u wordCount = ALIGN(width * height *
      BX_GEFORCE_THIS chs[chid].ifc_color_bytes, 4) >> 2;
    if (BX_GEFORCE_THIS chs[chid].ifc_words != nullptr)
      delete[] BX_GEFORCE_THIS chs[chid].ifc_words;
    BX_GEFORCE_THIS chs[chid].ifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[chid].ifc_words_left = wordCount;
    BX_GEFORCE_THIS chs[chid].ifc_words = new Bit32u[wordCount];
  }
  else if (method >= 0x100 && method < 0x800) {
    BX_GEFORCE_THIS chs[chid].ifc_words[
      BX_GEFORCE_THIS chs[chid].ifc_words_ptr++] = param;
    BX_GEFORCE_THIS chs[chid].ifc_words_left--;
    if (!BX_GEFORCE_THIS chs[chid].ifc_words_left) {
      ifc(chid);
      delete[] BX_GEFORCE_THIS chs[chid].ifc_words;
      BX_GEFORCE_THIS chs[chid].ifc_words = nullptr;
    }
  }
}

void bx_geforce_c::execute_surf2d(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].s2d_img_src = param;
  else if (method == 0x062)
    BX_GEFORCE_THIS chs[chid].s2d_img_dst = param;
  else if (method == 0x0c0) {
    BX_GEFORCE_THIS chs[chid].s2d_color_fmt = param;
    Bit32u s2d_color_bytes_prev = BX_GEFORCE_THIS chs[chid].s2d_color_bytes;
    if (BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 1) // Y8
      BX_GEFORCE_THIS chs[chid].s2d_color_bytes = 1;
    else if (BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 4) // R5G6B5
      BX_GEFORCE_THIS chs[chid].s2d_color_bytes = 2;
    else if (BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 0x6 || // X8R8G8B8_Z8R8G8B8
             BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 0xA || // A8R8G8B8
             BX_GEFORCE_THIS chs[chid].s2d_color_fmt == 0xB)   // Y32
      BX_GEFORCE_THIS chs[chid].s2d_color_bytes = 4;
    else {
      BX_ERROR(("unknown 2d surface color format: 0x%02x",
        BX_GEFORCE_THIS chs[chid].s2d_color_fmt));
    }
    if (BX_GEFORCE_THIS chs[chid].s2d_color_bytes != s2d_color_bytes_prev &&
        (BX_GEFORCE_THIS chs[chid].s2d_color_bytes == 1 || s2d_color_bytes_prev == 1)) {
      update_color_bytes_ifc(chid);
      update_color_bytes_sifc(chid);
    }
  } else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].s2d_pitch = param;
  else if (method == 0x0c2)
    BX_GEFORCE_THIS chs[chid].s2d_ofs_src = param;
  else if (method == 0x0c3)
    BX_GEFORCE_THIS chs[chid].s2d_ofs_dst = param;
}

void bx_geforce_c::update_color_bytes_ifc(Bit32u chid)
{
  BX_GEFORCE_THIS update_color_bytes(
    BX_GEFORCE_THIS chs[chid].s2d_color_fmt,
    BX_GEFORCE_THIS chs[chid].ifc_color_fmt,
    &BX_GEFORCE_THIS chs[chid].ifc_color_bytes);
}

void bx_geforce_c::update_color_bytes_sifc(Bit32u chid)
{
  BX_GEFORCE_THIS update_color_bytes(
    BX_GEFORCE_THIS chs[chid].s2d_color_fmt,
    BX_GEFORCE_THIS chs[chid].sifc_color_fmt,
    &BX_GEFORCE_THIS chs[chid].sifc_color_bytes);
}

void bx_geforce_c::update_color_bytes_iifc(Bit32u chid)
{
  BX_GEFORCE_THIS update_color_bytes(0,
    BX_GEFORCE_THIS chs[chid].iifc_color_fmt,
    &BX_GEFORCE_THIS chs[chid].iifc_color_bytes);
}

void bx_geforce_c::update_color_bytes(Bit32u s2d_color_fmt, Bit32u color_fmt, Bit32u* color_bytes)
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
  else {
    BX_ERROR(("unknown color format: 0x%02x", color_fmt));
  }
}

void bx_geforce_c::execute_iifc(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].iifc_palette = param;
  else if (method == 0x0f9)
    BX_GEFORCE_THIS chs[chid].iifc_operation = param;
  else if (method == 0x0fa) {
    BX_GEFORCE_THIS chs[chid].iifc_color_fmt = param;
    update_color_bytes_iifc(chid);
  } else if (method == 0x0fb)
    BX_GEFORCE_THIS chs[chid].iifc_bpp4 = param;
  else if (method == 0x0fc)
    BX_GEFORCE_THIS chs[chid].iifc_palette_ofs = param;
  else if (method == 0x0fd)
    BX_GEFORCE_THIS chs[chid].iifc_yx = param;
  else if (method == 0x0fe)
    BX_GEFORCE_THIS chs[chid].iifc_dhw = param;
  else if (method == 0x0ff) {
    BX_GEFORCE_THIS chs[chid].iifc_shw = param;
    Bit32u width = BX_GEFORCE_THIS chs[chid].iifc_shw & 0xFFFF;
    Bit32u height = BX_GEFORCE_THIS chs[chid].iifc_shw >> 16;
    Bit32u wordCount = ALIGN(width * height *
      (BX_GEFORCE_THIS chs[chid].iifc_bpp4 ? 4 : 8), 32) >> 5;
    if (BX_GEFORCE_THIS chs[chid].iifc_words != nullptr)
      delete[] BX_GEFORCE_THIS chs[chid].iifc_words;
    BX_GEFORCE_THIS chs[chid].iifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[chid].iifc_words_left = wordCount;
    BX_GEFORCE_THIS chs[chid].iifc_words = new Bit32u[wordCount];
  }
  else if (method >= 0x100 && method < 0x800) {
    BX_GEFORCE_THIS chs[chid].iifc_words[
      BX_GEFORCE_THIS chs[chid].iifc_words_ptr++] = param;
    BX_GEFORCE_THIS chs[chid].iifc_words_left--;
    if (!BX_GEFORCE_THIS chs[chid].iifc_words_left) {
      iifc(chid);
      delete[] BX_GEFORCE_THIS chs[chid].iifc_words;
      BX_GEFORCE_THIS chs[chid].iifc_words = nullptr;
    }
  }
}

void bx_geforce_c::execute_sifc(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0bf)
    BX_GEFORCE_THIS chs[chid].sifc_operation = param;
  else if (method == 0x0c0) {
    BX_GEFORCE_THIS chs[chid].sifc_color_fmt = param;
    update_color_bytes_sifc(chid);
  } else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].sifc_shw = param;
  else if (method == 0x0c2)
    BX_GEFORCE_THIS chs[chid].sifc_dxds = param;
  else if (method == 0x0c3)
    BX_GEFORCE_THIS chs[chid].sifc_dydt = param;
  else if (method == 0x0c4)
    BX_GEFORCE_THIS chs[chid].sifc_clip_yx = param;
  else if (method == 0x0c5)
    BX_GEFORCE_THIS chs[chid].sifc_clip_hw = param;
  else if (method == 0x0c6) {
    BX_GEFORCE_THIS chs[chid].sifc_syx = param;
    Bit32u width = BX_GEFORCE_THIS chs[chid].sifc_shw & 0xFFFF;
    Bit32u height = BX_GEFORCE_THIS chs[chid].sifc_shw >> 16;
    Bit32u wordCount = ALIGN(width * height *
      BX_GEFORCE_THIS chs[chid].sifc_color_bytes, 4) >> 2;
    if (BX_GEFORCE_THIS chs[chid].sifc_words != nullptr)
      delete[] BX_GEFORCE_THIS chs[chid].sifc_words;
    BX_GEFORCE_THIS chs[chid].sifc_words_ptr = 0;
    BX_GEFORCE_THIS chs[chid].sifc_words_left = wordCount;
    BX_GEFORCE_THIS chs[chid].sifc_words = new Bit32u[wordCount];
  }
  else if (method >= 0x100 && method < 0x800) {
    BX_GEFORCE_THIS chs[chid].sifc_words[
      BX_GEFORCE_THIS chs[chid].sifc_words_ptr++] = param;
    BX_GEFORCE_THIS chs[chid].sifc_words_left--;
    if (!BX_GEFORCE_THIS chs[chid].sifc_words_left) {
      sifc(chid);
      delete[] BX_GEFORCE_THIS chs[chid].sifc_words;
      BX_GEFORCE_THIS chs[chid].sifc_words = nullptr;
    }
  }
}

void bx_geforce_c::execute_beta(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x0c0)
    BX_GEFORCE_THIS chs[chid].beta = param;
}

void bx_geforce_c::execute_sifm(Bit32u chid, Bit32u method, Bit32u param)
{
  if (method == 0x061)
    BX_GEFORCE_THIS chs[chid].sifm_src = param;
  else if (method == 0x0c0) {
    BX_GEFORCE_THIS chs[chid].sifm_color_fmt = param;
    if (BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 8)        // ???
      BX_GEFORCE_THIS chs[chid].sifm_color_bytes = 1;
    else if (BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 1 || // A1R5G5B5
             BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 2 || // X1R5G5B5
             BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 7)   // R5G6B5
      BX_GEFORCE_THIS chs[chid].sifm_color_bytes = 2;
    else if (BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 3 || // A8R8G8B8
             BX_GEFORCE_THIS chs[chid].sifm_color_fmt == 4)   // X8R8G8B8
      BX_GEFORCE_THIS chs[chid].sifm_color_bytes = 4;
    else {
      BX_ERROR(("unknown sifm color format: 0x%02x",
        BX_GEFORCE_THIS chs[chid].sifm_color_fmt));
    }
  } else if (method == 0x0c1)
    BX_GEFORCE_THIS chs[chid].sifm_operation = param;
  else if (method == 0x0c4)
    BX_GEFORCE_THIS chs[chid].sifm_dyx = param;
  else if (method == 0x0c5)
    BX_GEFORCE_THIS chs[chid].sifm_dhw = param;
  else if (method == 0x0c6)
    BX_GEFORCE_THIS chs[chid].sifm_dudx = param;
  else if (method == 0x0c7)
    BX_GEFORCE_THIS chs[chid].sifm_dvdy = param;
  else if (method == 0x100)
    BX_GEFORCE_THIS chs[chid].sifm_shw = param;
  else if (method == 0x101)
    BX_GEFORCE_THIS chs[chid].sifm_sfmt = param;
  else if (method == 0x102)
    BX_GEFORCE_THIS chs[chid].sifm_sofs = param;
  else if (method == 0x103) {
    BX_GEFORCE_THIS chs[chid].sifm_syx = param;
    sifm(chid);
  }
}

void bx_geforce_c::execute_d3d(Bit32u chid, Bit8u cls, Bit32u method, Bit32u param)
{
  if (method == 0x069)
    BX_GEFORCE_THIS chs[chid].d3d_semaphore_obj = param;
  else if (method == 0x75b)
    BX_GEFORCE_THIS chs[chid].d3d_semaphore_offset = param;
  else if (method == 0x75c) {
    dma_write32(BX_GEFORCE_THIS chs[chid].d3d_semaphore_obj,
      BX_GEFORCE_THIS chs[chid].d3d_semaphore_offset, param);
  }
}

bool bx_geforce_c::execute_command(Bit32u chid, Bit32u subc, Bit32u method, Bit32u param)
{
  bool software_method = false;
  BX_DEBUG(("execute_command: chid 0x%02x, subc 0x%02x, method 0x%03x, param 0x%08x",
    chid, subc, method, param));
  if (method == 0x000) {
    if (BX_GEFORCE_THIS chs[chid].schs[subc].engine == 0x01) {
      Bit32u word1 = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x4);
      if (BX_GEFORCE_THIS card_type < 0x40)
        word1 = word1 & 0x0000FFFF | BX_GEFORCE_THIS chs[chid].schs[subc].notifier >> 4 << 16;
      else
        word1 = word1 & 0xFFF00000 | BX_GEFORCE_THIS chs[chid].schs[subc].notifier >> 4;
      Bit32u word0 = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object);
      Bit8u cls = word0;
      if (cls == 0x62) {
        if (BX_GEFORCE_THIS card_type < 0x40) {
          ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8,
            BX_GEFORCE_THIS chs[chid].s2d_img_src >> 4 |
            BX_GEFORCE_THIS chs[chid].s2d_img_dst >> 4 << 16);
        } else {
          ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8,
            BX_GEFORCE_THIS chs[chid].s2d_img_src >> 4);
          ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0xC,
            BX_GEFORCE_THIS chs[chid].s2d_img_dst >> 4);
        }
      } else if (cls == 0x64) {
        ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8,
          BX_GEFORCE_THIS chs[chid].iifc_palette >> 4);
        if (BX_GEFORCE_THIS card_type < 0x40)
          word0 = word0 & 0xFFFC7FFF | BX_GEFORCE_THIS chs[chid].iifc_operation << 15;
        else
          word0 = word0 & 0xFFC7FFFF | BX_GEFORCE_THIS chs[chid].iifc_operation << 19;
        ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object, word0);
        if (BX_GEFORCE_THIS card_type < 0x40) {
          word1 = word1 & 0xFFFF00FF | BX_GEFORCE_THIS chs[chid].iifc_color_fmt + 9 << 8;
        } else {
          // should be stored somewhere else
          ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x10,
            BX_GEFORCE_THIS chs[chid].iifc_color_fmt);
        }
      }
      ramin_write32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x4, word1);
    }
    ramht_lookup(param, chid,
      &BX_GEFORCE_THIS chs[chid].schs[subc].object,
      &BX_GEFORCE_THIS chs[chid].schs[subc].engine);
    if (BX_GEFORCE_THIS chs[chid].schs[subc].engine == 0x01) {
      Bit32u word1 = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x4);
      if (BX_GEFORCE_THIS card_type < 0x40)
        BX_GEFORCE_THIS chs[chid].schs[subc].notifier = word1 >> 16 << 4;
      else
        BX_GEFORCE_THIS chs[chid].schs[subc].notifier = (word1 & 0xFFFFF) << 4;
      Bit32u word0 = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object);
      Bit8u cls = word0;
      if (cls == 0x62) {
        if (BX_GEFORCE_THIS card_type < 0x40) {
          Bit32u srcdst = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8);
          BX_GEFORCE_THIS chs[chid].s2d_img_src = (srcdst & 0xFFFF) << 4;
          BX_GEFORCE_THIS chs[chid].s2d_img_dst = srcdst >> 16 << 4;
        } else {
          BX_GEFORCE_THIS chs[chid].s2d_img_src =
            ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8) << 4;
          BX_GEFORCE_THIS chs[chid].s2d_img_dst =
            ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0xC) << 4;
        }
      } else if (cls == 0x64) {
        BX_GEFORCE_THIS chs[chid].iifc_palette =
          ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x8) << 4;
        Bit32u shift = BX_GEFORCE_THIS card_type < 0x40 ? 15 : 19;
        BX_GEFORCE_THIS chs[chid].iifc_operation = word0 >> shift & 7;
        if (BX_GEFORCE_THIS card_type < 0x40) {
          BX_GEFORCE_THIS chs[chid].iifc_color_fmt = (word1 >> 8 & 0xFF) - 9;
        } else {
          // should be stored somewhere else
          BX_GEFORCE_THIS chs[chid].iifc_color_fmt =
            ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object + 0x10);
          if (BX_GEFORCE_THIS chs[chid].iifc_color_fmt == 0)
            BX_GEFORCE_THIS chs[chid].iifc_color_fmt = 1;
        }
        update_color_bytes_iifc(chid);
      }
    } else if (BX_GEFORCE_THIS chs[chid].schs[subc].engine == 0x00) {
      software_method = true;
    } else {
      BX_DEBUG(("execute_command: unknown engine %d", BX_GEFORCE_THIS chs[chid].schs[subc].engine));
    }
  } else if (method == 0x014) {
    BX_GEFORCE_THIS fifo_cache1_ref_cnt = param;
  } else if (method == 0x018) {
    Bit32u semaphore_obj;
    ramht_lookup(param, chid, &semaphore_obj, nullptr);
    BX_GEFORCE_THIS fifo_cache1_semaphore = semaphore_obj >> 4;
  } else if (method == 0x019) {
    BX_GEFORCE_THIS fifo_cache1_semaphore &= 0x000FFFFF;
    BX_GEFORCE_THIS fifo_cache1_semaphore |= param << 20;
  } else if (method == 0x01a || method == 0x01b) {
    Bit32u semaphore_obj = (BX_GEFORCE_THIS fifo_cache1_semaphore & 0x000FFFFF) << 4;
    Bit32u semaphore_offset = BX_GEFORCE_THIS fifo_cache1_semaphore >> 20;
    if (method == 0x01a) {
      if (dma_read32(semaphore_obj, semaphore_offset) != param) {
        BX_GEFORCE_THIS acquire_active = true;
        return false;
      }
    } else {
      dma_write32(semaphore_obj, semaphore_offset, param);
    }
  } else if (method >= 0x040) {
    if (BX_GEFORCE_THIS chs[chid].schs[subc].engine == 0x01) {
      if (method >= 0x060 && method < 0x080)
        ramht_lookup(param, chid, &param, nullptr);
      if (method == 0x041) {
        BX_GEFORCE_THIS chs[chid].notify_pending = true;
        BX_GEFORCE_THIS chs[chid].notify_type = param;
      } else if (method == 0x060) {
        BX_GEFORCE_THIS chs[chid].schs[subc].notifier = param;
      } else {
        Bit8u cls = ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].object);
        BX_DEBUG(("execute_command: obj 0x%08x, class 0x%02x, method 0x%03x, param 0x%08x",
          BX_GEFORCE_THIS chs[chid].schs[subc].object, cls, method, param));
        if (cls == 0x19)
          execute_clip(chid, method, param);
        else if (cls == 0x39)
          execute_m2mf(chid, subc, method, param);
        else if (cls == 0x43)
          execute_rop(chid, method, param);
        else if (cls == 0x44)
          execute_patt(chid, method, param);
        else if (cls == 0x4a)
          execute_gdi(chid, method, param);
        else if (cls == 0x57)
          execute_chroma(chid, method, param);
        else if (cls == 0x5f || cls == 0x9f)
          execute_imageblit(chid, cls, method, param);
        else if (cls == 0x61 || cls == 0x65 || cls == 0x8a)
          execute_ifc(chid, cls, method, param);
        else if (cls == 0x62)
          execute_surf2d(chid, method, param);
        else if (cls == 0x64)
          execute_iifc(chid, method, param);
        else if (cls == 0x66)
          execute_sifc(chid, method, param);
        else if (cls == 0x72)
          execute_beta(chid, method, param);
        else if (cls == 0x89)
          execute_sifm(chid, method, param);
        else if (cls == 0x97)
          execute_d3d(chid, cls, method, param);
        if (BX_GEFORCE_THIS chs[chid].notify_pending) {
          BX_GEFORCE_THIS chs[chid].notify_pending = false;
          if ((ramin_read32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier) & 0xFF) == 0x30) {
            BX_DEBUG(("execute_command: DMA notify skipped"));
          } else {
            BX_DEBUG(("execute_command: DMA notify 0x%08x",
              BX_GEFORCE_THIS chs[chid].schs[subc].notifier));
            dma_write64(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0x0, get_current_time());
            dma_write32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0x8, 0);
            dma_write32(BX_GEFORCE_THIS chs[chid].schs[subc].notifier, 0xC, 0);
          }
          if (BX_GEFORCE_THIS chs[chid].notify_type) {
            BX_GEFORCE_THIS graph_intr |= 0x00000001;
            update_irq_level();
            BX_GEFORCE_THIS graph_nsource |= 0x00000001;
            BX_GEFORCE_THIS graph_notify = 0x00110000;
            Bit32u notifier = BX_GEFORCE_THIS chs[chid].schs[subc].notifier >> 4;
            if (BX_GEFORCE_THIS card_type < 0x40)
              BX_GEFORCE_THIS graph_ctx_switch2 = notifier << 16;
            else
              BX_GEFORCE_THIS graph_ctx_switch1 = notifier;
            BX_GEFORCE_THIS graph_ctx_switch4 =
              BX_GEFORCE_THIS chs[chid].schs[subc].object >> 4;
            if (BX_GEFORCE_THIS card_type >= 0x40)
              BX_GEFORCE_THIS graph_ctxctl_cur = BX_GEFORCE_THIS fifo_grctx_instance;
            BX_GEFORCE_THIS graph_trapped_addr = method << 2 | subc << 16 | chid << 20;
            BX_GEFORCE_THIS graph_trapped_data = param;
            BX_DEBUG(("execute_command: notify interrupt triggered"));
          }
        }
      }
    } else if (BX_GEFORCE_THIS chs[chid].schs[subc].engine == 0x00) {
      software_method = true;
    } else {
      BX_DEBUG(("execute_command: unknown engine %d", BX_GEFORCE_THIS chs[chid].schs[subc].engine));
    }
  }
  if (software_method) {
    BX_GEFORCE_THIS fifo_intr |= 0x00000001;
    update_irq_level();
    BX_GEFORCE_THIS fifo_cache1_pull0 |= 0x00000100;
    BX_GEFORCE_THIS fifo_cache1_method[BX_GEFORCE_THIS fifo_cache1_put / 4] = method << 2 | subc << 13;
    BX_GEFORCE_THIS fifo_cache1_data[BX_GEFORCE_THIS fifo_cache1_put / 4] = param;
    BX_GEFORCE_THIS fifo_cache1_put += 4;
    if (BX_GEFORCE_THIS fifo_cache1_put == GEFORCE_CACHE1_SIZE * 4)
      BX_GEFORCE_THIS fifo_cache1_put = 0;
    BX_DEBUG(("execute_command: software method"));
  }
  return true;
}

void bx_geforce_c::fifo_process(Bit32u chid)
{
  Bit32u oldchid = BX_GEFORCE_THIS fifo_cache1_push1 & 0x1F;
  if (oldchid == chid) {
    if (BX_GEFORCE_THIS fifo_cache1_dma_put == BX_GEFORCE_THIS fifo_cache1_dma_get)
      return;
  } else {
    if (ramfc_read32(chid, 0x0) == ramfc_read32(chid, 0x4))
      return;
  }
  if (oldchid != chid) {
    Bit32u sro = BX_GEFORCE_THIS card_type < 0x40 ? 0x2C : 0x30;
    ramfc_write32(oldchid, 0x0, BX_GEFORCE_THIS fifo_cache1_dma_put);
    ramfc_write32(oldchid, 0x4, BX_GEFORCE_THIS fifo_cache1_dma_get);
    ramfc_write32(oldchid, 0x8, BX_GEFORCE_THIS fifo_cache1_ref_cnt);
    ramfc_write32(oldchid, 0xC, BX_GEFORCE_THIS fifo_cache1_dma_instance);
    ramfc_write32(oldchid, sro, BX_GEFORCE_THIS fifo_cache1_semaphore);
    if (BX_GEFORCE_THIS card_type >= 0x40)
      ramfc_write32(oldchid, 0x38, BX_GEFORCE_THIS fifo_grctx_instance);
    BX_GEFORCE_THIS fifo_cache1_dma_put = ramfc_read32(chid, 0x0);
    BX_GEFORCE_THIS fifo_cache1_dma_get = ramfc_read32(chid, 0x4);
    BX_GEFORCE_THIS fifo_cache1_ref_cnt = ramfc_read32(chid, 0x8);
    BX_GEFORCE_THIS fifo_cache1_dma_instance = ramfc_read32(chid, 0xC);
    BX_GEFORCE_THIS fifo_cache1_semaphore = ramfc_read32(chid, sro);
    if (BX_GEFORCE_THIS card_type >= 0x40)
      BX_GEFORCE_THIS fifo_grctx_instance = ramfc_read32(chid, 0x38);
    BX_GEFORCE_THIS fifo_cache1_push1 = BX_GEFORCE_THIS fifo_cache1_push1 & ~0x1F | chid;
  }
  BX_GEFORCE_THIS fifo_cache1_dma_push |= 0x100;
  while (BX_GEFORCE_THIS fifo_cache1_dma_get != BX_GEFORCE_THIS fifo_cache1_dma_put) {
    BX_DEBUG(("fifo: processing at 0x%08x", BX_GEFORCE_THIS fifo_cache1_dma_get));
    Bit32u word = dma_read32(
      BX_GEFORCE_THIS fifo_cache1_dma_instance << 4,
      BX_GEFORCE_THIS fifo_cache1_dma_get);
    BX_GEFORCE_THIS fifo_cache1_dma_get += 4;
    if (BX_GEFORCE_THIS chs[chid].dma_state.mcnt) {
      if (!execute_command(chid,
          BX_GEFORCE_THIS chs[chid].dma_state.subc,
          BX_GEFORCE_THIS chs[chid].dma_state.mthd,
          word)) {
        BX_GEFORCE_THIS fifo_cache1_dma_get -= 4;
        break;
      }
      if (!BX_GEFORCE_THIS chs[chid].dma_state.ni)
        BX_GEFORCE_THIS chs[chid].dma_state.mthd++;
      BX_GEFORCE_THIS chs[chid].dma_state.mcnt--;
    } else {
      if ((word & 0xe0000003) == 0x20000000) {
        // old jump
        BX_DEBUG(("fifo: jump to 0x%08x", word & 0x1fffffff));
        BX_GEFORCE_THIS fifo_cache1_dma_get = word & 0x1fffffff;
      } else if ((word & 3) == 2) {
        // call
        if (BX_GEFORCE_THIS chs[chid].subr_active)
          BX_PANIC(("fifo: call with subroutine active"));
        BX_DEBUG(("fifo: call 0x%08x", word & 0xfffffffc));
        BX_GEFORCE_THIS chs[chid].subr_return = BX_GEFORCE_THIS fifo_cache1_dma_get;
        BX_GEFORCE_THIS chs[chid].subr_active = true;
        BX_GEFORCE_THIS fifo_cache1_dma_get = word & 0xfffffffc;
      } else if (word == 0x00020000) {
        // return
        if (!BX_GEFORCE_THIS chs[chid].subr_active)
          BX_PANIC(("fifo: return with subroutine inactive"));
        BX_DEBUG(("fifo: return to 0x%08x", BX_GEFORCE_THIS chs[chid].subr_return));
        BX_GEFORCE_THIS fifo_cache1_dma_get = BX_GEFORCE_THIS chs[chid].subr_return;
        BX_GEFORCE_THIS chs[chid].subr_active = false;
      } else if ((word & 0xa0030003) == 0) {
        BX_GEFORCE_THIS chs[chid].dma_state.mthd = (word >> 2) & 0x7ff;
        BX_GEFORCE_THIS chs[chid].dma_state.subc = (word >> 13) & 7;
        BX_GEFORCE_THIS chs[chid].dma_state.mcnt = (word >> 18) & 0x7ff;
        BX_GEFORCE_THIS chs[chid].dma_state.ni = word & 0x40000000;
      } else {
        BX_PANIC(("fifo: unexpected word 0x%08x", word));
      }
    }
  }
}

Bit64u bx_geforce_c::get_current_time()
{
  return BX_GEFORCE_THIS timer_inittime1 +
    bx_pc_system.time_nsec() - BX_GEFORCE_THIS timer_inittime2 & ~BX_CONST64(0x1F);
}

void bx_geforce_c::set_irq_level(bool level)
{
  DEV_pci_set_irq(BX_GEFORCE_THIS devfunc, BX_GEFORCE_THIS pci_conf[0x3d], level);
}

Bit32u bx_geforce_c::get_mc_intr()
{
  Bit32u value = 0x00000000;
  if (BX_GEFORCE_THIS bus_intr & BX_GEFORCE_THIS bus_intr_en)
    value |= 0x10000000;
  if (BX_GEFORCE_THIS fifo_intr & BX_GEFORCE_THIS fifo_intr_en)
    value |= 0x00000100;
  if (BX_GEFORCE_THIS graph_intr & BX_GEFORCE_THIS graph_intr_en)
    value |= 0x00001000;
  if (BX_GEFORCE_THIS crtc_intr & BX_GEFORCE_THIS crtc_intr_en)
    value |= 0x01000000;
  return value;
}

void bx_geforce_c::update_irq_level()
{
  set_irq_level(get_mc_intr() && BX_GEFORCE_THIS mc_intr_en & 1);
}


Bit32u bx_geforce_c::register_read32(Bit32u address)
{
  Bit32u value;

  if (address == 0x0) {
    if (BX_GEFORCE_THIS card_type == 0x20)
      value = 0x020200A5;
    else
      value = BX_GEFORCE_THIS card_type << 20;
  } else if (address == 0x100) {
    value = get_mc_intr();
  } else if (address == 0x140)
    value = BX_GEFORCE_THIS mc_intr_en;
  else if (address == 0x200)
    value = BX_GEFORCE_THIS mc_enable;
  else if (address == 0x1100)
    value = BX_GEFORCE_THIS bus_intr;
  else if (address == 0x1140)
    value = BX_GEFORCE_THIS bus_intr_en;
  else if (address >= 0x1800 && address < 0x1900) {
    Bit32u offset = address - 0x1800;
    value = 
      (BX_GEFORCE_THIS pci_conf[offset + 0] << 0) |
      (BX_GEFORCE_THIS pci_conf[offset + 1] << 8) |
      (BX_GEFORCE_THIS pci_conf[offset + 2] << 16) |
      (BX_GEFORCE_THIS pci_conf[offset + 3] << 24);
  } else if (address == 0x2100) {
    value = BX_GEFORCE_THIS fifo_intr;
  } else if (address == 0x2140) {
    value = BX_GEFORCE_THIS fifo_intr_en;
  } else if (address == 0x2210) {
    value = BX_GEFORCE_THIS fifo_ramht;
  } else if (address == 0x2214 && BX_GEFORCE_THIS card_type < 0x40) {
    value = BX_GEFORCE_THIS fifo_ramfc;
  } else if (address == 0x2218) {
    value = BX_GEFORCE_THIS fifo_ramro;
  } else if (address == 0x2220 && BX_GEFORCE_THIS card_type >= 0x40) {
    value = BX_GEFORCE_THIS fifo_ramfc;
  } else if (address == 0x2400) { // PFIFO_RUNOUT_STATUS
    value = 0x00000010;
    if (BX_GEFORCE_THIS fifo_cache1_get != BX_GEFORCE_THIS fifo_cache1_put)
      value = 0x00000000;
  } else if (address == 0x2504) {
    value = BX_GEFORCE_THIS fifo_mode;
  } else if (address == 0x3204) {
    value = BX_GEFORCE_THIS fifo_cache1_push1;
  } else if (address == 0x3210) {
    value = BX_GEFORCE_THIS fifo_cache1_put;
  } else if (address == 0x3214) { // PFIFO_CACHE1_STATUS
    value = 0x00000010;
    if (BX_GEFORCE_THIS fifo_cache1_get != BX_GEFORCE_THIS fifo_cache1_put)
      value = 0x00000000;
  } else if (address == 0x3220) {
    value = BX_GEFORCE_THIS fifo_cache1_dma_push;
  } else if (address == 0x322c) {
    value = BX_GEFORCE_THIS fifo_cache1_dma_instance;
  } else if (address == 0x3230) { // PFIFO_CACHE1_DMA_CTL
    value = 0x80000000;
  } else if (address == 0x3240) {
    value = BX_GEFORCE_THIS fifo_cache1_dma_put;
  } else if (address == 0x3244) {
    value = BX_GEFORCE_THIS fifo_cache1_dma_get;
  } else if (address == 0x3248) {
    value = BX_GEFORCE_THIS fifo_cache1_ref_cnt;
  } else if (address == 0x3250) {
    if (BX_GEFORCE_THIS fifo_cache1_get != BX_GEFORCE_THIS fifo_cache1_put)
      BX_GEFORCE_THIS fifo_cache1_pull0 |= 0x00000100;
    value = BX_GEFORCE_THIS fifo_cache1_pull0;
  } else if (address == 0x3270) {
    value = BX_GEFORCE_THIS fifo_cache1_get;
  } else if (address == 0x32e0) {
    value = BX_GEFORCE_THIS fifo_grctx_instance;
  } else if (address == 0x3304) {
    value = 0x00000001;
  } else if (address >= 0x3800 && address < 0x4000 && BX_GEFORCE_THIS card_type < 0x40 ||
             address >= 0x90000 && address < 0x92000 && BX_GEFORCE_THIS card_type >= 0x40) {
    Bit32u offset;
    if (BX_GEFORCE_THIS card_type < 0x40)
      offset = address - 0x3800;
    else
      offset = address - 0x90000;
    Bit32u index = offset / 8;
    if (offset % 8 == 0)
      value = BX_GEFORCE_THIS fifo_cache1_method[index];
    else
      value = BX_GEFORCE_THIS fifo_cache1_data[index];
  } else if (address == 0x9100) {
    value = BX_GEFORCE_THIS timer_intr;
  } else if (address == 0x9140) {
    value = BX_GEFORCE_THIS timer_intr_en;
  } else if (address == 0x9200)
    value = BX_GEFORCE_THIS timer_num;
  else if (address == 0x9210)
    value = BX_GEFORCE_THIS timer_den;
  else if (address == 0x9400)
    value = (Bit32u)get_current_time();
  else if (address == 0x9410)
    value = get_current_time() >> 32;
  else if (address == 0x9420)
    value = BX_GEFORCE_THIS timer_alarm;
  else if (address >= 0xc0300 && address < 0xc0400 ||
           address >= 0xc2300 && address < 0xc2400)
    value = register_read8(address);
  else if (address == 0x10020c)
    value = BX_GEFORCE_THIS s.memsize;
  else if (address == 0x100320) { // PFB_ZCOMP_SIZE
    if (BX_GEFORCE_THIS card_type == 0x20)
      value = 0x00007fff;
    else if (BX_GEFORCE_THIS card_type == 0x35)
      value = 0x0005c7ff;
    else
      value = 0x0002e3ff;
  } else if (address == 0x101000)
    value = BX_GEFORCE_THIS straps0_primary;
  else if (address >= 0x300000 && address < 0x310000) {
    Bit32u offset = address - 0x300000;
    if (BX_GEFORCE_THIS pci_conf[0x50] == 0x00) {
      value = 
        (BX_GEFORCE_THIS pci_rom[offset + 0] << 0) |
        (BX_GEFORCE_THIS pci_rom[offset + 1] << 8) |
        (BX_GEFORCE_THIS pci_rom[offset + 2] << 16) |
        (BX_GEFORCE_THIS pci_rom[offset + 3] << 24);
    } else {
      value = 0x00000000;
    }
  } else if (address == 0x400100) {
    value = BX_GEFORCE_THIS graph_intr;
  } else if (address == 0x400108) {
    value = BX_GEFORCE_THIS graph_nsource;
  } else if (address == 0x40013C && BX_GEFORCE_THIS card_type >= 0x40 ||
             address == 0x400140 && BX_GEFORCE_THIS card_type < 0x40) {
    value = BX_GEFORCE_THIS graph_intr_en;
  } else if (address == 0x40014C) {
    value = BX_GEFORCE_THIS graph_ctx_switch1;
  } else if (address == 0x400150) {
    value = BX_GEFORCE_THIS graph_ctx_switch2;
  } else if (address == 0x400158) {
    value = BX_GEFORCE_THIS graph_ctx_switch4;
  } else if (address == 0x40032c) {
    value = BX_GEFORCE_THIS graph_ctxctl_cur;
  } else if (address == 0x400700) {
    value = BX_GEFORCE_THIS graph_status;
  } else if (address == 0x400704) {
    value = BX_GEFORCE_THIS graph_trapped_addr;
  } else if (address == 0x400708) {
    value = BX_GEFORCE_THIS graph_trapped_data;
  } else if (address == 0x400718) {
    value = BX_GEFORCE_THIS graph_notify;
  } else if (address == 0x400720) {
    value = BX_GEFORCE_THIS graph_fifo;
  } else if (address == 0x400780) {
    value = BX_GEFORCE_THIS graph_channel_ctx_table;
  } else if (address == 0x600100) {
    value = BX_GEFORCE_THIS crtc_intr;
  } else if (address == 0x600140) {
    value = BX_GEFORCE_THIS crtc_intr_en;
  } else if (address == 0x600800) {
    value = BX_GEFORCE_THIS crtc_start;
  } else if (address == 0x600804) {
    value = BX_GEFORCE_THIS crtc_config;
  } else if (address == 0x600808) {
    value = VGA_READ(0x03da, 1) << 13; // hack
  } else if (address == 0x60080c) {
    value = BX_GEFORCE_THIS crtc_cursor_offset;
  } else if (address == 0x600810) {
    value = BX_GEFORCE_THIS crtc_cursor_config;
  } else if (address == 0x600868) {
    Bit64u display_usec =
      bx_virt_timer.time_usec(BX_GEFORCE_THIS vsync_realtime) - BX_GEFORCE_THIS s.display_start_usec;
    display_usec = display_usec % BX_GEFORCE_THIS s.vtotal_usec;
    value = (Bit32u)(BX_GEFORCE_THIS get_crtc_vtotal() * display_usec / BX_GEFORCE_THIS s.vtotal_usec);
  } else if (address >= 0x601300 && address < 0x601400 ||
             address >= 0x603300 && address < 0x603400) {
    value = register_read8(address);
  } else if (address == 0x680300) {
    value = BX_GEFORCE_THIS ramdac_cu_start_pos;
  } else if (address == 0x680404) { // RAMDAC_NV10_CURSYNC
    value = 0x00000000;
  } else if (address == 0x680508) {
    value = BX_GEFORCE_THIS ramdac_vpll;
  } else if (address == 0x68050c) {
    value = BX_GEFORCE_THIS ramdac_pll_select;
  } else if (address == 0x680578) {
    value = BX_GEFORCE_THIS ramdac_vpll_b;
  } else if (address == 0x680600) {
    value = BX_GEFORCE_THIS ramdac_general_control;
  } else if (address == 0x680828) { // PRAMDAC_FP_HCRTC
    value = 0x00000000; // Second monitor is disconnected
  } else if (address >= 0x681300 && address < 0x681400 ||
             address >= 0x683300 && address < 0x683400) {
    value = register_read8(address);
  } else if (address >= 0x700000 && address < 0x800000) {
    value = ramin_read32(address - 0x700000);
  } else if (address >= 0x800000 && address < 0xA00000 ||
             address >= 0xC00000 && address < 0xE00000) {
    Bit32u chid;
    Bit32u offset;
    if (address >= 0x800000 && address < 0xA00000) {
      chid = address >> 16 & 0x1F;
      offset = address & 0x1FFF;
    } else {
      chid = address >> 12 & 0x1FF;
      if (chid >= GEFORCE_CHANNEL_COUNT) {
        BX_PANIC(("Channel id >= 32"));
        chid = 0;
      }
      offset = address & 0x1FF;
    }
    value = 0x00000000;
    if (offset >= 0x40 && offset <= 0x48) {
      Bit32u curchid = BX_GEFORCE_THIS fifo_cache1_push1 & 0x1F;
      if (curchid == chid) {
        if (offset == 0x40)
          value = BX_GEFORCE_THIS fifo_cache1_dma_put;
        else if (offset == 0x44)
          value = BX_GEFORCE_THIS fifo_cache1_dma_get;
        else if (offset == 0x48)
          value = BX_GEFORCE_THIS fifo_cache1_ref_cnt;
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
    value = BX_GEFORCE_THIS unk_regs[address / 4];
  }
  return value;
}

void bx_geforce_c::register_write32(Bit32u address, Bit32u value)
{
  if (address == 0x140) {
    BX_GEFORCE_THIS mc_intr_en = value;
    update_irq_level();
  } else if (address == 0x200) {
    BX_GEFORCE_THIS mc_enable = value;
  } else if (address >= 0x1800 && address < 0x1900) {
    BX_GEFORCE_THIS pci_write_handler(address - 0x1800, value, 4);
  } else if (address == 0x1100) {
    BX_GEFORCE_THIS bus_intr &= ~value;
    update_irq_level();
  } else if (address == 0x1140) {
    BX_GEFORCE_THIS bus_intr_en = value;
    update_irq_level();
  } else if (address == 0x2100) {
    BX_GEFORCE_THIS fifo_intr &= ~value;
    update_irq_level();
  } else if (address == 0x2140) {
    BX_GEFORCE_THIS fifo_intr_en = value;
    update_irq_level();
  } else if (address == 0x2210) {
    BX_GEFORCE_THIS fifo_ramht = value;
  } else if (address == 0x2214 && BX_GEFORCE_THIS card_type < 0x40) {
    BX_GEFORCE_THIS fifo_ramfc = value;
  } else if (address == 0x2218) {
    BX_GEFORCE_THIS fifo_ramro = value;
  } else if (address == 0x2220 && BX_GEFORCE_THIS card_type >= 0x40) {
    BX_GEFORCE_THIS fifo_ramfc = value;
  } else if (address == 0x2504) {
    BX_GEFORCE_THIS fifo_mode = value;
  } else if (address == 0x3204) {
    BX_GEFORCE_THIS fifo_cache1_push1 = value;
  } else if (address == 0x3210) {
    BX_GEFORCE_THIS fifo_cache1_put = value;
  } else if (address == 0x3220) {
    BX_GEFORCE_THIS fifo_cache1_dma_push = value;
  } else if (address == 0x322c) {
    BX_GEFORCE_THIS fifo_cache1_dma_instance = value;
  } else if (address == 0x3240) {
    BX_GEFORCE_THIS fifo_cache1_dma_put = value;
  } else if (address == 0x3244) {
    BX_GEFORCE_THIS fifo_cache1_dma_get = value;
  } else if (address == 0x3248) {
    BX_GEFORCE_THIS fifo_cache1_ref_cnt = value;
  } else if (address == 0x3250) {
    BX_GEFORCE_THIS fifo_cache1_pull0 = value;
  } else if (address == 0x3270) {
    BX_GEFORCE_THIS fifo_cache1_get = value & (GEFORCE_CACHE1_SIZE * 4 - 1);
    if (BX_GEFORCE_THIS fifo_cache1_get != BX_GEFORCE_THIS fifo_cache1_put) {
      BX_GEFORCE_THIS fifo_intr |= 0x00000001;
    } else {
      BX_GEFORCE_THIS fifo_intr &= ~0x00000001;
      BX_GEFORCE_THIS fifo_cache1_pull0 &= ~0x00000100;
    }
    update_irq_level();
  } else if (address == 0x32e0) {
    BX_GEFORCE_THIS fifo_grctx_instance = value;
  } else if (address == 0x9100) {
    BX_GEFORCE_THIS timer_intr &= ~value;
  } else if (address == 0x9140) {
    BX_GEFORCE_THIS timer_intr_en = value;
  } else if (address == 0x9200) {
    BX_GEFORCE_THIS timer_num = value;
  } else if (address == 0x9210) {
    BX_GEFORCE_THIS timer_den = value;
  } else if (address == 0x9400 || address == 0x9410) {
    BX_GEFORCE_THIS timer_inittime2 = bx_pc_system.time_nsec();
    if (address == 0x9400) {
      BX_GEFORCE_THIS timer_inittime1 = 
        (BX_GEFORCE_THIS timer_inittime1 & 0xFFFFFFFF00000000ULL) | value;
    } else {
      BX_GEFORCE_THIS timer_inittime1 =
        (BX_GEFORCE_THIS timer_inittime1 & 0x00000000FFFFFFFFULL) | ((Bit64u)value << 32);
    }
  } else if (address == 0x9420) {
    BX_GEFORCE_THIS timer_alarm = value;
  } else if (address >= 0xc0300 && address < 0xc0400 ||
             address >= 0xc2300 && address < 0xc2400) {
    register_write8(address, value);
  } else if (address == 0x101000) {
    if (value >> 31)
      BX_GEFORCE_THIS straps0_primary = value;
    else
      BX_GEFORCE_THIS straps0_primary = BX_GEFORCE_THIS straps0_primary_original;
  } else if (address == 0x400100) {
    BX_GEFORCE_THIS graph_intr &= ~value;
    update_irq_level();
  } else if (address == 0x400108) {
    BX_GEFORCE_THIS graph_nsource = value;
  } else if (address == 0x40013C && BX_GEFORCE_THIS card_type >= 0x40 ||
             address == 0x400140 && BX_GEFORCE_THIS card_type < 0x40) {
    BX_GEFORCE_THIS graph_intr_en = value;
    update_irq_level();
  } else if (address == 0x40014C) {
    BX_GEFORCE_THIS graph_ctx_switch1 = value;
  } else if (address == 0x400150) {
    BX_GEFORCE_THIS graph_ctx_switch2 = value;
  } else if (address == 0x400158) {
    BX_GEFORCE_THIS graph_ctx_switch4 = value;
  } else if (address == 0x40032c) {
    BX_GEFORCE_THIS graph_ctxctl_cur = value;
  } else if (address == 0x400700) {
    BX_GEFORCE_THIS graph_status = value;
  } else if (address == 0x400704) {
    BX_GEFORCE_THIS graph_trapped_addr = value;
  } else if (address == 0x400708) {
    BX_GEFORCE_THIS graph_trapped_data = value;
  } else if (address == 0x400718) {
    BX_GEFORCE_THIS graph_notify = value;
  } else if (address == 0x400720) {
    BX_GEFORCE_THIS graph_fifo = value;
  } else if (address == 0x400780) {
    BX_GEFORCE_THIS graph_channel_ctx_table = value;
  } else if (address == 0x600100) {
    BX_GEFORCE_THIS crtc_intr &= ~value;
    update_irq_level();
  } else if (address == 0x600140) {
    BX_GEFORCE_THIS crtc_intr_en = value;
    update_irq_level();
  } else if (address == 0x600800) {
    BX_GEFORCE_THIS crtc_start = value;
    BX_GEFORCE_THIS svga_needs_update_mode = 1;
  } else if (address == 0x600804) {
    BX_GEFORCE_THIS crtc_config = value;
  } else if (address == 0x60080c) {
    BX_GEFORCE_THIS crtc_cursor_offset = value;
    BX_GEFORCE_THIS hw_cursor.offset = BX_GEFORCE_THIS crtc_cursor_offset;
  } else if (address == 0x600810) {
    BX_GEFORCE_THIS crtc_cursor_config = value;
    BX_GEFORCE_THIS hw_cursor.enabled =
      BX_GEFORCE_THIS crtc.reg[0x31] & 0x01 || value & 0x00000001;
    BX_GEFORCE_THIS hw_cursor.size = value & 0x00010000 ? 64 : 32;
    BX_GEFORCE_THIS hw_cursor.bpp32 = value & 0x00001000;
  } else if (address >= 0x601300 && address < 0x601400 ||
             address >= 0x603300 && address < 0x603400) {
    register_write8(address, value);
  } else if (address == 0x680300) {
    Bit16s prevx = BX_GEFORCE_THIS hw_cursor.x;
    Bit16s prevy = BX_GEFORCE_THIS hw_cursor.y;
    BX_GEFORCE_THIS ramdac_cu_start_pos = value;
    BX_GEFORCE_THIS hw_cursor.x = (Bit32s)BX_GEFORCE_THIS ramdac_cu_start_pos << 20 >> 20;
    BX_GEFORCE_THIS hw_cursor.y = (Bit32s)BX_GEFORCE_THIS ramdac_cu_start_pos << 4 >> 20;
    if (BX_GEFORCE_THIS hw_cursor.size != 0) {
      BX_GEFORCE_THIS redraw_area(prevx, prevy,
        BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
      BX_GEFORCE_THIS redraw_area(BX_GEFORCE_THIS hw_cursor.x, BX_GEFORCE_THIS hw_cursor.y,
        BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
    }
  } else if (address == 0x680508) {
    BX_GEFORCE_THIS ramdac_vpll = value;
    BX_GEFORCE_THIS calculate_retrace_timing();
  } else if (address == 0x68050c) {
    BX_GEFORCE_THIS ramdac_pll_select = value;
    BX_GEFORCE_THIS calculate_retrace_timing();
  } else if (address == 0x680578) {
    BX_GEFORCE_THIS ramdac_vpll_b = value;
    BX_GEFORCE_THIS calculate_retrace_timing();
  } else if (address == 0x680600) {
    BX_GEFORCE_THIS ramdac_general_control = value;
    BX_GEFORCE_THIS s.dac_shift = value >> 20 & 1 ? 0 : 2;
  } else if (address >= 0x681300 && address < 0x681400 ||
             address >= 0x683300 && address < 0x683400) {
    register_write8(address, value);
  } else if (address >= 0x700000 && address < 0x800000) {
    ramin_write32(address - 0x700000, value);
  } else if (address >= 0x800000 && address < 0xA00000 ||
             address >= 0xC00000 && address < 0xE00000) {
    Bit32u chid;
    Bit32u offset;
    if (address >= 0x800000 && address < 0xA00000) {
      chid = address >> 16 & 0x1F;
      offset = address & 0x1FFF;
    } else {
      chid = address >> 12 & 0x1FF;
      if (chid >= GEFORCE_CHANNEL_COUNT) {
        BX_PANIC(("Channel id >= 32"));
        chid = 0;
      }
      offset = address & 0x1FF;
    }
    if (offset == 0x40) {
      Bit32u curchid = BX_GEFORCE_THIS fifo_cache1_push1 & 0x1F;
      if (curchid == chid)
        BX_GEFORCE_THIS fifo_cache1_dma_put = value;
      else
        ramfc_write32(chid, 0x0, value);
      fifo_process(chid);
    }
  } else {
    BX_GEFORCE_THIS unk_regs[address / 4] = value;
  }
}

void bx_geforce_c::svga_init_pcihandlers(void)
{
  BX_GEFORCE_THIS devfunc = 0x00;
  DEV_register_pci_handlers2(BX_GEFORCE_THIS_PTR,
      &BX_GEFORCE_THIS devfunc, BX_PLUGIN_GEFORCE, "GeForce AGP", true);
  Bit16u devid = 0x0000;
  Bit8u revid = 0x00;
  if (BX_GEFORCE_THIS card_type == 0x20) {
    devid = 0x0202;
    revid = 0xA3;
  } else if (BX_GEFORCE_THIS card_type == 0x35) {
    devid = 0x0331;
  } else if (BX_GEFORCE_THIS card_type == 0x40) {
    devid = 0x0045;
  }
  BX_GEFORCE_THIS init_pci_conf(0x10DE, devid, revid, 0x030000, 0x00, BX_PCI_INTA);

  BX_GEFORCE_THIS init_bar_mem(0, GEFORCE_PNPMMIO_SIZE, geforce_mem_read_handler,
                               geforce_mem_write_handler);
  BX_GEFORCE_THIS pci_conf[0x14] = 0x08;
  BX_GEFORCE_THIS init_bar_mem(1, BX_GEFORCE_THIS s.memsize, geforce_mem_read_handler,
                               geforce_mem_write_handler);
  if (BX_GEFORCE_THIS card_type != 0x35) {
    if (BX_GEFORCE_THIS card_type == 0x20)
      BX_GEFORCE_THIS pci_conf[0x18] = 0x08;
    BX_GEFORCE_THIS init_bar_mem(2, BX_GEFORCE_THIS bar2_size, geforce_mem_read_handler,
                                 geforce_mem_write_handler);
  }
  BX_GEFORCE_THIS pci_rom_address = 0;
  BX_GEFORCE_THIS pci_rom_read_handler = geforce_mem_read_handler;
  BX_GEFORCE_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());

  BX_GEFORCE_THIS pci_conf[0x2c] = 0x7D;
  BX_GEFORCE_THIS pci_conf[0x2d] = 0x10;
  if (BX_GEFORCE_THIS card_type == 0x20) {
    BX_GEFORCE_THIS pci_conf[0x2e] = 0x63;
    BX_GEFORCE_THIS pci_conf[0x2f] = 0x28;
  } else if (BX_GEFORCE_THIS card_type == 0x35) {
    BX_GEFORCE_THIS pci_conf[0x2e] = 0x7B;
    BX_GEFORCE_THIS pci_conf[0x2f] = 0x29;
  } else if (BX_GEFORCE_THIS card_type == 0x40) {
    BX_GEFORCE_THIS pci_conf[0x2e] = 0x96;
    BX_GEFORCE_THIS pci_conf[0x2f] = 0x29;
  }
  BX_GEFORCE_THIS pci_conf[0x40] = BX_GEFORCE_THIS pci_conf[0x2c];
  BX_GEFORCE_THIS pci_conf[0x41] = BX_GEFORCE_THIS pci_conf[0x2d];
  BX_GEFORCE_THIS pci_conf[0x42] = BX_GEFORCE_THIS pci_conf[0x2e];
  BX_GEFORCE_THIS pci_conf[0x43] = BX_GEFORCE_THIS pci_conf[0x2f];

  BX_GEFORCE_THIS pci_conf[0x06] = 0xB0;
  BX_GEFORCE_THIS pci_conf[0x07] = 0x02;
  BX_GEFORCE_THIS pci_conf[0x34] = 0x60;
  BX_GEFORCE_THIS pci_conf[0x44] = 0x02;
  BX_GEFORCE_THIS pci_conf[0x45] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x46] = 0x20;
  BX_GEFORCE_THIS pci_conf[0x47] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x48] = 0x07;
  BX_GEFORCE_THIS pci_conf[0x49] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x4a] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x4b] = 0x1F;
  BX_GEFORCE_THIS pci_conf[0x54] = 0x01;
  BX_GEFORCE_THIS pci_conf[0x55] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x56] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x57] = 0x00;
  BX_GEFORCE_THIS pci_conf[0x60] = 0x01;
  BX_GEFORCE_THIS pci_conf[0x61] = 0x44;
  BX_GEFORCE_THIS pci_conf[0x62] = 0x02;
  BX_GEFORCE_THIS pci_conf[0x63] = 0x00;
}

void bx_geforce_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
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
void bx_geforce_c::debug_dump(int argc, char **argv)
{
  bx_vgacore_c::debug_dump(argc, argv);
}
#endif

#endif // BX_SUPPORT_GEFORCE
