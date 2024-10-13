/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2024  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "param_names.h"
#include "vgacore.h"
#include "virt_timer.h"

#define BX_VGA_THIS this->
#define BX_VGA_THIS_PTR this

#define LOG_THIS

static const Bit16u charmap_offset[8] = {
  0x0000, 0x4000, 0x8000, 0xc000,
  0x2000, 0x6000, 0xa000, 0xe000
};

static const Bit32u text_snap_size[4] = {
  0x20000, 0x10000, 0x8000, 0x8000
};

const Bit8u ccdat[16][4] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0xff, 0x00, 0x00, 0x00 },
  { 0x00, 0xff, 0x00, 0x00 },
  { 0xff, 0xff, 0x00, 0x00 },
  { 0x00, 0x00, 0xff, 0x00 },
  { 0xff, 0x00, 0xff, 0x00 },
  { 0x00, 0xff, 0xff, 0x00 },
  { 0xff, 0xff, 0xff, 0x00 },
  { 0x00, 0x00, 0x00, 0xff },
  { 0xff, 0x00, 0x00, 0xff },
  { 0x00, 0xff, 0x00, 0xff },
  { 0xff, 0xff, 0x00, 0xff },
  { 0x00, 0x00, 0xff, 0xff },
  { 0xff, 0x00, 0xff, 0xff },
  { 0x00, 0xff, 0xff, 0xff },
  { 0xff, 0xff, 0xff, 0xff },
};


bx_vgacore_c::bx_vgacore_c()
{
  memset(&s, 0, sizeof(s));
  update_timer_id = BX_NULL_TIMER_HANDLE;
  vga_vtimer_id = BX_NULL_TIMER_HANDLE;
}

bx_vgacore_c::~bx_vgacore_c()
{
  if (s.memory != NULL) {
    delete [] s.memory;
    s.memory = NULL;
  }
  if (s.text_buffer != NULL) {
    delete [] s.text_buffer;
    s.text_buffer = NULL;
  }
  if (s.text_snapshot != NULL) {
    delete [] s.text_snapshot;
    s.text_snapshot = NULL;
  }
  if (s.vga_tile_updated != NULL) {
    delete [] s.vga_tile_updated;
    s.vga_tile_updated = NULL;
  }
  SIM->get_param_num(BXPN_VGA_UPDATE_FREQUENCY)->set_handler(NULL);
}

void bx_vgacore_c::init(void)
{
  unsigned x,y;

  BX_VGA_THIS vga_ext = SIM->get_param_enum(BXPN_VGA_EXTENSION);
  BX_VGA_THIS pci_enabled = 0;

  BX_VGA_THIS init_standard_vga();
  if (!BX_VGA_THIS init_vga_extension()) {
    // VGA memory not yet initialized
    BX_VGA_THIS s.memsize = 0x40000;
    if (BX_VGA_THIS s.memory == NULL)
      BX_VGA_THIS s.memory = new Bit8u[BX_VGA_THIS s.memsize];
    memset(BX_VGA_THIS s.memory, 0, BX_VGA_THIS s.memsize);
  }
  BX_VGA_THIS s.vgamem_mask = 0x3ffff;
  BX_VGA_THIS init_gui();

  BX_VGA_THIS s.num_x_tiles = BX_VGA_THIS s.max_xres / X_TILESIZE +
                              ((BX_VGA_THIS s.max_xres % X_TILESIZE) > 0);
  BX_VGA_THIS s.num_y_tiles = BX_VGA_THIS s.max_yres / Y_TILESIZE +
                              ((BX_VGA_THIS s.max_yres % Y_TILESIZE) > 0);
  BX_VGA_THIS s.vga_tile_updated = new bool[BX_VGA_THIS s.num_x_tiles * BX_VGA_THIS s.num_y_tiles];
  for (y = 0; y < BX_VGA_THIS s.num_y_tiles; y++)
    for (x = 0; x < BX_VGA_THIS s.num_x_tiles; x++)
      SET_TILE_UPDATED(BX_VGA_THIS, x, y, 0);

  if (!BX_VGA_THIS pci_enabled) {
    BX_MEM(0)->load_ROM(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr(), 0xc0000, 1);
  }
}

void bx_vgacore_c::init_standard_vga(void)
{
  // initialize VGA controllers and other internal stuff
  BX_VGA_THIS s.vga_enabled = 1;
  BX_VGA_THIS s.misc_output.color_emulation  = 1;
  BX_VGA_THIS s.misc_output.enable_ram  = 1;
  BX_VGA_THIS s.misc_output.horiz_sync_pol   = 1;
  BX_VGA_THIS s.misc_output.vert_sync_pol    = 1;

  BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics = 1;
  BX_VGA_THIS s.line_offset=80;
  BX_VGA_THIS s.line_compare=1023;
  BX_VGA_THIS s.vertical_display_end=399;

  BX_VGA_THIS s.attribute_ctrl.video_enabled = 1;
  BX_VGA_THIS s.attribute_ctrl.color_plane_enable = 0x0f;
  BX_VGA_THIS s.pel.dac_state = 0x01;
  BX_VGA_THIS s.pel.mask = 0xff;
  BX_VGA_THIS s.graphics_ctrl.memory_mapping = 2; // monochrome text mode

  BX_VGA_THIS s.sequencer.reset1 = 1;
  BX_VGA_THIS s.sequencer.reset2 = 1;
  BX_VGA_THIS s.sequencer.extended_mem = 1; // display mem greater than 64K
  BX_VGA_THIS s.sequencer.odd_even_dis = 1; // use sequential addressing mode

  BX_VGA_THIS s.dac_shift = 2;
  BX_VGA_THIS s.last_bpp = 8;
  BX_VGA_THIS s.vclk[0] = 25175000;
  BX_VGA_THIS s.vclk[1] = 28322000;
  BX_VGA_THIS s.htotal_usec = 31;
  BX_VGA_THIS s.vtotal_usec = 14268;
  BX_VGA_THIS s.vrend_usec = 13155;

  BX_VGA_THIS s.max_xres = 800;
  BX_VGA_THIS s.max_yres = 600;

  BX_VGA_THIS s.vga_override = 0;

  if (BX_VGA_THIS s.text_buffer == NULL)
    BX_VGA_THIS s.text_buffer = new Bit8u[0x20000];
  if (BX_VGA_THIS s.text_snapshot == NULL)
    BX_VGA_THIS s.text_snapshot = new Bit8u[0x20000];

  // initialize memory handlers, timer and CMOS
  DEV_register_memory_handlers(BX_VGA_THIS_PTR, mem_read_handler, mem_write_handler,
                               0xa0000, 0xbffff);

  BX_VGA_THIS init_systemtimer();

  // video card with BIOS ROM
  DEV_cmos_set_reg(0x14, (DEV_cmos_get_reg(0x14) & 0xcf) | 0x00);
}

void bx_vgacore_c::init_gui(void)
{
  int argc, i;
  char *argv[16];
  const char *options;

  // set up display library options and start gui
  memset(argv, 0, sizeof(argv));
  argc = 1;
  argv[0] = (char *)"bochs";
  options = SIM->get_param_string(BXPN_DISPLAYLIB_OPTIONS)->getptr();
  argc = SIM->split_option_list("Display library options", options, &argv[1], 15) + 1;
  bx_gui->init(argc, argv, BX_VGA_THIS s.max_xres, BX_VGA_THIS s.max_yres,
               X_TILESIZE, Y_TILESIZE);
  for (i = 1; i < argc; i++) {
    if (argv[i] != NULL) {
        free(argv[i]);
        argv[i] = NULL;
    }
  }
}

void bx_vgacore_c::init_iohandlers(bx_read_handler_t f_read, bx_write_handler_t f_write)
{
  unsigned addr, i;
  Bit8u io_mask[16] = {3, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1};
  for (addr=0x03B4; addr<=0x03B5; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 1);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
  }

  for (addr=0x03BA; addr<=0x03BA; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 1);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
  }

  i = 0;
  for (addr=0x03C0; addr<=0x03CF; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", io_mask[i++]);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
  }

  for (addr=0x03D4; addr<=0x03D5; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 3);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
  }

  for (addr=0x03DA; addr<=0x03DA; addr++) {
    DEV_register_ioread_handler(this, f_read, addr, "vga video", 3);
    DEV_register_iowrite_handler(this, f_write, addr, "vga video", 3);
  }
}

void bx_vgacore_c::init_systemtimer(void)
{
  BX_VGA_THIS update_realtime = SIM->get_param_bool(BXPN_VGA_REALTIME)->get();
  BX_VGA_THIS vsync_realtime = (SIM->get_param_enum(BXPN_CLOCK_SYNC)->get() & BX_CLOCK_SYNC_REALTIME) > 0;
  bx_param_num_c *vga_update_freq = SIM->get_param_num(BXPN_VGA_UPDATE_FREQUENCY);
  Bit32u update_interval = 100000;
  if (vga_update_freq->get() > 0) {
    update_interval = (Bit32u)(1000000 / vga_update_freq->get());
    BX_INFO(("interval=%u, mode=%s", update_interval, BX_VGA_THIS update_realtime ? "realtime":"standard"));
    BX_VGA_THIS update_mode_vsync = false;
  } else {
    BX_INFO(("VGA update interval uses VSYNC, mode=%s", BX_VGA_THIS update_realtime ? "realtime":"standard"));
    BX_VGA_THIS update_mode_vsync = true;
  }
  if (BX_VGA_THIS update_timer_id == BX_NULL_TIMER_HANDLE) {
    BX_VGA_THIS update_timer_id = bx_virt_timer.register_timer(this, vga_timer_handler,
      update_interval, 1, 1, BX_VGA_THIS update_realtime, "vga update");
    if (!BX_VGA_THIS update_mode_vsync) {
      vga_update_freq->set_range(1, 75);
      vga_update_freq->set_handler(vga_param_handler);
      vga_update_freq->set_device_param(this);
    } else {
      vga_update_freq->set_runtime_param(0);
    }
  }
  if (BX_VGA_THIS vga_vtimer_id == BX_NULL_TIMER_HANDLE) {
    BX_VGA_THIS vga_vtimer_id = bx_virt_timer.register_timer(this, vertical_timer_handler,
       100000, 1, 1, BX_VGA_THIS vsync_realtime, "vga vsync");
  }
  BX_VGA_THIS set_update_timer(update_interval);
  BX_INFO(("VSYNC using %s mode", BX_VGA_THIS vsync_realtime ? "realtime":"standard"));
  BX_VGA_THIS start_vertical_timer();
}

void bx_vgacore_c::vgacore_register_state(bx_list_c *parent)
{
  char name[4];

  bx_list_c *list = new bx_list_c(parent, "vgacore", "VGA Core State");
  bx_list_c *misc = new bx_list_c(list, "misc_output");
  BXRS_PARAM_BOOL(misc, color_emulation, BX_VGA_THIS s.misc_output.color_emulation);
  BXRS_PARAM_BOOL(misc, enable_ram, BX_VGA_THIS s.misc_output.enable_ram);
  new bx_shadow_num_c(misc, "clock_select", &BX_VGA_THIS s.misc_output.clock_select);
  BXRS_PARAM_BOOL(misc, select_high_bank, BX_VGA_THIS s.misc_output.select_high_bank);
  BXRS_PARAM_BOOL(misc, horiz_sync_pol, BX_VGA_THIS s.misc_output.horiz_sync_pol);
  BXRS_PARAM_BOOL(misc, vert_sync_pol, BX_VGA_THIS s.misc_output.vert_sync_pol);
  bx_list_c *crtc = new bx_list_c(list, "CRTC");
  new bx_shadow_num_c(crtc, "address", &BX_VGA_THIS s.CRTC.address, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_VGA_THIS s.CRTC.reg, 25, 1);
  BXRS_PARAM_BOOL(crtc, write_protect, BX_VGA_THIS s.CRTC.write_protect);
  bx_list_c *actl = new bx_list_c(list, "attribute_ctrl");
  BXRS_PARAM_BOOL(actl, flip_flop, BX_VGA_THIS s.attribute_ctrl.flip_flop);
  new bx_shadow_num_c(actl, "address", &BX_VGA_THIS s.attribute_ctrl.address, BASE_HEX);
  BXRS_PARAM_BOOL(actl, video_enabled, BX_VGA_THIS s.attribute_ctrl.video_enabled);
  new bx_shadow_data_c(actl, "palette_reg", BX_VGA_THIS s.attribute_ctrl.palette_reg, 16, 1);
  new bx_shadow_num_c(actl, "overscan_color", &BX_VGA_THIS s.attribute_ctrl.overscan_color, BASE_HEX);
  new bx_shadow_num_c(actl, "color_plane_enable", &BX_VGA_THIS s.attribute_ctrl.color_plane_enable, BASE_HEX);
  new bx_shadow_num_c(actl, "horiz_pel_panning", &BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning, BASE_HEX);
  new bx_shadow_num_c(actl, "color_select", &BX_VGA_THIS s.attribute_ctrl.color_select, BASE_HEX);
  bx_list_c *mode = new bx_list_c(actl, "mode_ctrl");
  BXRS_PARAM_BOOL(mode, graphics_alpha, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha);
  BXRS_PARAM_BOOL(mode, display_type, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type);
  BXRS_PARAM_BOOL(mode, enable_line_graphics, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics);
  BXRS_PARAM_BOOL(mode, blink_intensity, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity);
  BXRS_PARAM_BOOL(mode, pixel_panning_compat, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat);
  BXRS_PARAM_BOOL(mode, pixel_clock_select, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select);
  BXRS_PARAM_BOOL(mode, internal_palette_size, BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size);
  bx_list_c *pel = new bx_list_c(list, "pel");
  new bx_shadow_num_c(pel, "write_data_register", &BX_VGA_THIS s.pel.write_data_register, BASE_HEX);
  new bx_shadow_num_c(pel, "write_data_cycle", &BX_VGA_THIS s.pel.write_data_cycle);
  new bx_shadow_num_c(pel, "read_data_register", &BX_VGA_THIS s.pel.read_data_register, BASE_HEX);
  new bx_shadow_num_c(pel, "read_data_cycle", &BX_VGA_THIS s.pel.read_data_cycle);
  new bx_shadow_num_c(pel, "dac_state", &BX_VGA_THIS s.pel.dac_state);
  new bx_shadow_num_c(pel, "mask", &BX_VGA_THIS s.pel.mask, BASE_HEX);
  new bx_shadow_data_c(list, "pel_data", &BX_VGA_THIS s.pel.data[0].red, sizeof(BX_VGA_THIS s.pel.data));
  bx_list_c *gfxc = new bx_list_c(list, "graphics_ctrl");
  new bx_shadow_num_c(gfxc, "index", &BX_VGA_THIS s.graphics_ctrl.index);
  new bx_shadow_num_c(gfxc, "set_reset", &BX_VGA_THIS s.graphics_ctrl.set_reset);
  new bx_shadow_num_c(gfxc, "enable_set_reset", &BX_VGA_THIS s.graphics_ctrl.enable_set_reset);
  new bx_shadow_num_c(gfxc, "color_compare", &BX_VGA_THIS s.graphics_ctrl.color_compare);
  new bx_shadow_num_c(gfxc, "data_rotate", &BX_VGA_THIS s.graphics_ctrl.data_rotate);
  new bx_shadow_num_c(gfxc, "raster_op", &BX_VGA_THIS s.graphics_ctrl.raster_op);
  new bx_shadow_num_c(gfxc, "read_map_select", &BX_VGA_THIS s.graphics_ctrl.read_map_select);
  new bx_shadow_num_c(gfxc, "write_mode", &BX_VGA_THIS s.graphics_ctrl.write_mode);
  new bx_shadow_num_c(gfxc, "read_mode", &BX_VGA_THIS s.graphics_ctrl.read_mode);
  BXRS_PARAM_BOOL(gfxc, odd_even, BX_VGA_THIS s.graphics_ctrl.odd_even);
  BXRS_PARAM_BOOL(gfxc, chain_odd_even, BX_VGA_THIS s.graphics_ctrl.chain_odd_even);
  new bx_shadow_num_c(gfxc, "shift_reg", &BX_VGA_THIS s.graphics_ctrl.shift_reg);
  BXRS_PARAM_BOOL(gfxc, graphics_alpha, BX_VGA_THIS s.graphics_ctrl.graphics_alpha);
  new bx_shadow_num_c(gfxc, "memory_mapping", &BX_VGA_THIS s.graphics_ctrl.memory_mapping);
  new bx_shadow_num_c(gfxc, "color_dont_care", &BX_VGA_THIS s.graphics_ctrl.color_dont_care, BASE_HEX);
  new bx_shadow_num_c(gfxc, "bitmask", &BX_VGA_THIS s.graphics_ctrl.bitmask, BASE_HEX);
  new bx_shadow_num_c(gfxc, "latch0", &BX_VGA_THIS s.graphics_ctrl.latch[0], BASE_HEX);
  new bx_shadow_num_c(gfxc, "latch1", &BX_VGA_THIS s.graphics_ctrl.latch[1], BASE_HEX);
  new bx_shadow_num_c(gfxc, "latch2", &BX_VGA_THIS s.graphics_ctrl.latch[2], BASE_HEX);
  new bx_shadow_num_c(gfxc, "latch3", &BX_VGA_THIS s.graphics_ctrl.latch[3], BASE_HEX);
  bx_list_c *sequ = new bx_list_c(list, "sequencer");
  new bx_shadow_num_c(sequ, "index", &BX_VGA_THIS s.sequencer.index);
  new bx_shadow_num_c(sequ, "map_mask", &BX_VGA_THIS s.sequencer.map_mask);
  BXRS_PARAM_BOOL(sequ, reset1, BX_VGA_THIS s.sequencer.reset1);
  BXRS_PARAM_BOOL(sequ, reset2, BX_VGA_THIS s.sequencer.reset2);
  new bx_shadow_num_c(sequ, "reg1", &BX_VGA_THIS s.sequencer.reg1, BASE_HEX);
  new bx_shadow_num_c(sequ, "char_map_select", &BX_VGA_THIS s.sequencer.char_map_select);
  BXRS_PARAM_BOOL(sequ, extended_mem, BX_VGA_THIS s.sequencer.extended_mem);
  BXRS_PARAM_BOOL(sequ, odd_even_dis, BX_VGA_THIS s.sequencer.odd_even_dis);
  BXRS_PARAM_BOOL(sequ, chain_four, BX_VGA_THIS s.sequencer.chain_four);
  BXRS_PARAM_BOOL(list, enabled, BX_VGA_THIS s.vga_enabled);
  new bx_shadow_num_c(list, "line_offset", &BX_VGA_THIS s.line_offset);
  new bx_shadow_num_c(list, "line_compare", &BX_VGA_THIS s.line_compare);
  new bx_shadow_num_c(list, "vertical_display_end", &BX_VGA_THIS s.vertical_display_end);
  new bx_shadow_num_c(list, "charmap_address1", &BX_VGA_THIS s.charmap_address1);
  new bx_shadow_num_c(list, "charmap_address2", &BX_VGA_THIS s.charmap_address2);
  BXRS_PARAM_BOOL(list, x_dotclockdiv2, BX_VGA_THIS s.x_dotclockdiv2);
  BXRS_PARAM_BOOL(list, y_doublescan, BX_VGA_THIS s.y_doublescan);
  bx_list_c *vclk = new bx_list_c(list, "vclk");
  for (int i = 0; i < 4; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(vclk, name, &BX_VGA_THIS s.vclk[i]);
  }
  new bx_shadow_num_c(list, "dac_shift", &BX_VGA_THIS s.dac_shift);
  new bx_shadow_num_c(list, "ext_offset", &BX_VGA_THIS s.ext_offset);
  new bx_shadow_num_c(list, "ext_start_addr", &BX_VGA_THIS s.ext_start_addr);
  BXRS_PARAM_BOOL(list, ext_y_dblsize, BX_VGA_THIS s.ext_y_dblsize);
  new bx_shadow_num_c(list, "last_xres", &BX_VGA_THIS s.last_xres);
  new bx_shadow_num_c(list, "last_yres", &BX_VGA_THIS s.last_yres);
  new bx_shadow_num_c(list, "last_bpp", &BX_VGA_THIS s.last_bpp);
  new bx_shadow_num_c(list, "last_fw", &BX_VGA_THIS s.last_fw);
  new bx_shadow_num_c(list, "last_fh", &BX_VGA_THIS s.last_fh);
  new bx_shadow_num_c(list, "vgamem_mask", &BX_VGA_THIS s.vgamem_mask);
  BXRS_PARAM_BOOL(list, vga_override, BX_VGA_THIS s.vga_override);
  new bx_shadow_data_c(list, "memory", BX_VGA_THIS s.memory, BX_VGA_THIS s.memsize);
}

void bx_vgacore_c::after_restore_state(void)
{
  for (unsigned i=0; i<256; i++) {
    bx_gui->palette_change_common(i, BX_VGA_THIS s.pel.data[i].red   << BX_VGA_THIS s.dac_shift,
                                  BX_VGA_THIS s.pel.data[i].green << BX_VGA_THIS s.dac_shift,
                                  BX_VGA_THIS s.pel.data[i].blue  << BX_VGA_THIS s.dac_shift);
  }
  BX_VGA_THIS calculate_retrace_timing();
  BX_VGA_THIS s.text_buffer_update = true;
  if (!BX_VGA_THIS s.vga_override) {
    BX_VGA_THIS s.last_xres = BX_VGA_THIS s.max_xres;
    BX_VGA_THIS s.last_yres = BX_VGA_THIS s.max_yres;
    BX_VGA_THIS vga_redraw_area(0, 0, BX_VGA_THIS s.max_xres, BX_VGA_THIS s.max_yres);
  }
}

void bx_vgacore_c::determine_screen_dimensions(unsigned *piHeight, unsigned *piWidth)
{
  int h, v, vb;
#define AI BX_VGA_THIS s.CRTC.reg

  h = (AI[1] + 1) * 8;
  v = (AI[18] | ((AI[7] & 0x02) << 7) | ((AI[7] & 0x40) << 3)) + 1;
  vb = (AI[21] | ((AI[7] & 0x08) << 5) | ((AI[9] & 0x20) << 4)) + 1;
#undef AI

  if (vb < v) v = vb;
  if (BX_VGA_THIS s.x_dotclockdiv2) h <<= 1;
  if (BX_VGA_THIS s.ext_y_dblsize) v <<= 1;
  *piWidth = h;
  *piHeight = v;
}

void bx_vgacore_c::get_crtc_params(bx_crtc_params_t *crtcp, Bit32u *vclock)
{
  *vclock = BX_VGA_THIS s.vclk[BX_VGA_THIS s.misc_output.clock_select];
  if (BX_VGA_THIS s.x_dotclockdiv2) *vclock >>= 1;
  crtcp->htotal = BX_VGA_THIS s.CRTC.reg[0] + 5;
  crtcp->vtotal = BX_VGA_THIS s.CRTC.reg[6] +
                  ((BX_VGA_THIS s.CRTC.reg[7] & 0x01) << 8) +
                  ((BX_VGA_THIS s.CRTC.reg[7] & 0x20) << 4) + 2;
  crtcp->vbstart = BX_VGA_THIS s.CRTC.reg[21] +
                   ((BX_VGA_THIS s.CRTC.reg[7] & 0x08) << 5) +
                   ((BX_VGA_THIS s.CRTC.reg[9] & 0x20) << 4);
  crtcp->vrstart = BX_VGA_THIS s.CRTC.reg[16] +
                   ((BX_VGA_THIS s.CRTC.reg[7] & 0x04) << 6) +
                   ((BX_VGA_THIS s.CRTC.reg[7] & 0x80) << 2);
}

void bx_vgacore_c::calculate_retrace_timing()
{
  Bit32u hbstart, hbend, vclock = 0, cwidth, vrend;
  bx_crtc_params_t crtcp;
  float f_htotal_usec, hfreq, vfreq;

  BX_VGA_THIS get_crtc_params(&crtcp, &vclock);
  if (vclock == 0) {
    BX_ERROR(("Ignoring invalid video clock setting"));
    return;
  } else {
    BX_DEBUG(("Using video clock %.3f MHz", (float)vclock / 1000000.0f));
  }
  cwidth = ((BX_VGA_THIS s.sequencer.reg1 & 0x01) == 1) ? 8 : 9;
  hfreq = (float)vclock / (crtcp.htotal * cwidth);
  f_htotal_usec = 1000000.0f / hfreq;
  BX_VGA_THIS s.htotal_usec = (Bit32u)f_htotal_usec;
  hbstart = BX_VGA_THIS s.CRTC.reg[2];
  BX_VGA_THIS s.hbstart_usec = (Bit32u)((1000000.0 * hbstart * cwidth) / vclock);
  hbend = (BX_VGA_THIS s.CRTC.reg[3] & 0x1f) + ((BX_VGA_THIS s.CRTC.reg[5] & 0x80) >> 2);
  hbend = hbstart + ((hbend - hbstart) & 0x3f);
  BX_VGA_THIS s.hbend_usec = (Bit32u)((1000000.0 * hbend * cwidth) / vclock);
  vrend = ((BX_VGA_THIS s.CRTC.reg[17] & 0x0f) - crtcp.vrstart) & 0x0f;
  vrend += crtcp.vrstart;
  vfreq = hfreq / crtcp.vtotal;
  BX_VGA_THIS s.vtotal_usec = (Bit32u)(1000000.0 / vfreq);
  BX_VGA_THIS s.vblank_usec = (Bit32u)(f_htotal_usec * crtcp.vbstart);
  BX_VGA_THIS s.vrstart_usec = (Bit32u)(f_htotal_usec * crtcp.vrstart);
  BX_VGA_THIS s.vrend_usec = (Bit32u)(f_htotal_usec * vrend);
  BX_DEBUG(("hfreq = %.1f kHz / vfreq = %.1f Hz", (hfreq / 1000), vfreq));
  if (BX_VGA_THIS s.vtotal_usec < 8000) {
    BX_VGA_THIS s.vtotal_usec = 14268;
  }
  if (BX_VGA_THIS s.vrend_usec < 7000) {
    BX_VGA_THIS s.vrend_usec = BX_VGA_THIS s.vtotal_usec - 1113;
  }
  BX_VGA_THIS start_vertical_timer();
  if (BX_VGA_THIS update_mode_vsync) {
    BX_VGA_THIS set_update_timer(0);
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_vgacore_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  bx_vgacore_c *class_ptr = (bx_vgacore_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_vgacore_c::read(Bit32u address, unsigned io_len)
{
  Bit64u display_usec, line_usec;
  Bit16u ret16;
  Bit8u retval;
  Bit32u ret = 0;
#define RETURN(x) do { ret = (x); goto read_return; } while (0)

  if (io_len == 2) {
    ret16 = bx_vgacore_c::read(address, 1);
    ret16 |= (bx_vgacore_c::read(address+1, 1)) << 8;
    RETURN(ret16);
  }

  if ((address >= 0x03b0) && (address <= 0x03bf) &&
      (BX_VGA_THIS s.misc_output.color_emulation)) {
        RETURN(0xff);
  }
  if ((address >= 0x03d0) && (address <= 0x03df) &&
      (BX_VGA_THIS s.misc_output.color_emulation==0)) {
        RETURN(0xff);
  }

  switch (address) {
    case 0x03ba: /* Input Status 1 (monochrome emulation modes) */
    case 0x03ca: /* Feature Control ??? */
    case 0x03da: /* Input Status 1 (color emulation modes) */
      // bit3: Vertical Retrace
      //       0 = display is in the display mode
      //       1 = display is in the vertical retrace mode
      // bit0: Display Enable
      //       0 = display is in the display mode
      //       1 = display is not in the display mode; either the
      //           horizontal or vertical blanking period is active

      retval = 0;
      display_usec = bx_virt_timer.time_usec(BX_VGA_THIS vsync_realtime) - BX_VGA_THIS s.display_start_usec;
      display_usec = display_usec % BX_VGA_THIS s.vtotal_usec;
      if ((display_usec >= BX_VGA_THIS s.vrstart_usec) &&
          (display_usec <= BX_VGA_THIS s.vrend_usec)) {
        retval |= 0x08;
      }
      if (display_usec >= BX_VGA_THIS s.vblank_usec) {
        retval |= 0x01;
      } else {
        line_usec = display_usec % BX_VGA_THIS s.htotal_usec;
        if ((line_usec >= BX_VGA_THIS s.hbstart_usec) &&
            (line_usec <= BX_VGA_THIS s.hbend_usec)) {
          retval |= 0x01;
        }
      }

      /* reading this port resets the flip-flop to address mode */
      BX_VGA_THIS s.attribute_ctrl.flip_flop = 0;
      RETURN(retval);
      break;

    case 0x03c0: /* */
      if (BX_VGA_THIS s.attribute_ctrl.flip_flop == 0) {
        retval =
          (BX_VGA_THIS s.attribute_ctrl.video_enabled << 5) |
          BX_VGA_THIS s.attribute_ctrl.address;
        RETURN(retval);
      } else {
        BX_ERROR(("io read: 0x3c0: flip_flop != 0"));
        return(0);
      }
      break;

    case 0x03c1: /* */
      switch (BX_VGA_THIS s.attribute_ctrl.address) {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b:
        case 0x0c: case 0x0d: case 0x0e: case 0x0f:
          retval = BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address];
          RETURN(retval);
          break;
        case 0x10: /* mode control register */
          retval =
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha << 0) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type << 1) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics << 2) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity << 3) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat << 5) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select << 6) |
            (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size << 7);
          RETURN(retval);
          break;
        case 0x11: /* overscan color register */
          RETURN(BX_VGA_THIS s.attribute_ctrl.overscan_color);
          break;
        case 0x12: /* color plane enable */
          RETURN(BX_VGA_THIS s.attribute_ctrl.color_plane_enable);
          break;
        case 0x13: /* horizontal PEL panning register */
          RETURN(BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning);
          break;
        case 0x14: /* color select register */
          RETURN(BX_VGA_THIS s.attribute_ctrl.color_select);
          break;
        default:
          BX_INFO(("io read: 0x3c1: unknown register 0x%02x",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.address));
          RETURN(0);
      }
      break;

    case 0x03c2: /* Input Status 0 */
      BX_DEBUG(("io read 0x3c2: input status #0: ignoring"));
      RETURN(0);
      break;

    case 0x03c3: /* VGA Enable Register */
      RETURN(BX_VGA_THIS s.vga_enabled);
      break;

    case 0x03c4: /* Sequencer Index Register */
      RETURN(BX_VGA_THIS s.sequencer.index);
      break;

    case 0x03c5: /* Sequencer Registers 00..04 */
      switch (BX_VGA_THIS s.sequencer.index) {
        case 0: /* sequencer: reset */
          BX_DEBUG(("io read 0x3c5: sequencer reset"));
          RETURN((Bit8u)BX_VGA_THIS s.sequencer.reset1 | (BX_VGA_THIS s.sequencer.reset2<<1));
          break;
        case 1: /* sequencer: clocking mode */
          BX_DEBUG(("io read 0x3c5: sequencer clocking mode"));
          RETURN(BX_VGA_THIS s.sequencer.reg1);
          break;
        case 2: /* sequencer: map mask register */
          RETURN(BX_VGA_THIS s.sequencer.map_mask);
          break;
        case 3: /* sequencer: character map select register */
          RETURN(BX_VGA_THIS s.sequencer.char_map_select);
          break;
        case 4: /* sequencer: memory mode register */
          retval =
            (BX_VGA_THIS s.sequencer.extended_mem   << 1) |
            (BX_VGA_THIS s.sequencer.odd_even_dis   << 2) |
            (BX_VGA_THIS s.sequencer.chain_four     << 3);
          RETURN(retval);
          break;

        default:
          BX_DEBUG(("io read 0x3c5: index %u unhandled",
            (unsigned) BX_VGA_THIS s.sequencer.index));
          RETURN(0);
      }
      break;

    case 0x03c6: /* PEL mask ??? */
      RETURN(BX_VGA_THIS s.pel.mask);
      break;

    case 0x03c7: /* DAC state, read = 11b, write = 00b */
      RETURN(BX_VGA_THIS s.pel.dac_state);
      break;

    case 0x03c8: /* PEL address write mode */
      RETURN(BX_VGA_THIS s.pel.write_data_register);
      break;

    case 0x03c9: /* PEL Data Register, colors 00..FF */
      if (BX_VGA_THIS s.pel.dac_state == 0x03) {
        switch (BX_VGA_THIS s.pel.read_data_cycle) {
          case 0:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].red;
            break;
          case 1:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].green;
            break;
          case 2:
            retval = BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.read_data_register].blue;
            break;
          default:
            retval = 0; // keep compiler happy
        }
        BX_VGA_THIS s.pel.read_data_cycle++;
        if (BX_VGA_THIS s.pel.read_data_cycle >= 3) {
          BX_VGA_THIS s.pel.read_data_cycle = 0;
          BX_VGA_THIS s.pel.read_data_register++;
        }
      }
      else {
        retval = 0x3f;
      }
      RETURN(retval);
      break;

    case 0x03cc: /* Miscellaneous Output / Graphics 1 Position ??? */
      retval =
        ((BX_VGA_THIS s.misc_output.color_emulation  & 0x01) << 0) |
        ((BX_VGA_THIS s.misc_output.enable_ram       & 0x01) << 1) |
        ((BX_VGA_THIS s.misc_output.clock_select     & 0x03) << 2) |
        ((BX_VGA_THIS s.misc_output.select_high_bank & 0x01) << 5) |
        ((BX_VGA_THIS s.misc_output.horiz_sync_pol   & 0x01) << 6) |
        ((BX_VGA_THIS s.misc_output.vert_sync_pol    & 0x01) << 7);
      RETURN(retval);
      break;

    case 0x03ce: /* Graphics Controller Index Register */
      RETURN(BX_VGA_THIS s.graphics_ctrl.index);
      break;

    case 0x03cd: /* ??? */
      BX_DEBUG(("io read from 03cd"));
      RETURN(0x00);
      break;

    case 0x03cf: /* Graphics Controller Registers 00..08 */
      switch (BX_VGA_THIS s.graphics_ctrl.index) {
        case 0: /* Set/Reset */
          RETURN(BX_VGA_THIS s.graphics_ctrl.set_reset);
          break;
        case 1: /* Enable Set/Reset */
          RETURN(BX_VGA_THIS s.graphics_ctrl.enable_set_reset);
          break;
        case 2: /* Color Compare */
          RETURN(BX_VGA_THIS s.graphics_ctrl.color_compare);
          break;
        case 3: /* Data Rotate */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.raster_op & 0x03) << 3) |
            ((BX_VGA_THIS s.graphics_ctrl.data_rotate & 0x07) << 0);
          RETURN(retval);
          break;
        case 4: /* Read Map Select */
          RETURN(BX_VGA_THIS s.graphics_ctrl.read_map_select);
          break;
        case 5: /* Mode */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.shift_reg & 0x03) << 5) |
            ((BX_VGA_THIS s.graphics_ctrl.odd_even & 0x01) << 4) |
            ((BX_VGA_THIS s.graphics_ctrl.read_mode & 0x01) << 3) |
            ((BX_VGA_THIS s.graphics_ctrl.write_mode & 0x03) << 0);

          if (BX_VGA_THIS s.graphics_ctrl.odd_even ||
              BX_VGA_THIS s.graphics_ctrl.shift_reg)
            BX_DEBUG(("io read 0x3cf: reg 05 = 0x%02x", (unsigned) retval));
          RETURN(retval);
          break;
        case 6: /* Miscellaneous */
          retval =
            ((BX_VGA_THIS s.graphics_ctrl.memory_mapping & 0x03) << 2) |
            ((BX_VGA_THIS s.graphics_ctrl.odd_even & 0x01) << 1) |
            ((BX_VGA_THIS s.graphics_ctrl.graphics_alpha & 0x01) << 0);
          RETURN(retval);
          break;
        case 7: /* Color Don't Care */
          RETURN(BX_VGA_THIS s.graphics_ctrl.color_dont_care);
          break;
        case 8: /* Bit Mask */
          RETURN(BX_VGA_THIS s.graphics_ctrl.bitmask);
          break;
        default:
          /* ??? */
          BX_DEBUG(("io read: 0x3cf: index %u unhandled",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.index));
          RETURN(0);
      }
      break;

    case 0x03d4: /* CRTC Index Register (color emulation modes) */
      RETURN(BX_VGA_THIS s.CRTC.address);
      break;

    case 0x03b5: /* CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* CRTC Registers (color emulation modes) */
      if (BX_VGA_THIS s.CRTC.address == 0x22) {
        return BX_VGA_THIS s.graphics_ctrl.latch[BX_VGA_THIS s.graphics_ctrl.read_map_select];
      }
      if (BX_VGA_THIS s.CRTC.address > 0x18) {
        BX_DEBUG(("io read: invalid CRTC register 0x%02x", BX_VGA_THIS s.CRTC.address));
        RETURN(0);
      }
      RETURN(BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address]);
      break;

    case 0x03db: /* Ignore this address (16-bit read from 0x03da) */
      RETURN(0); /* keep compiler happy */
      break;

    case 0x03b4: /* CRTC Index Register (monochrome emulation modes) */
    case 0x03cb: /* not sure but OpenBSD reads it a lot */
    default:
      BX_INFO(("io read from vga port 0x%04x", (unsigned) address));
      RETURN(0); /* keep compiler happy */
  }

read_return:
  if (io_len == 1) {
    BX_DEBUG(("8-bit read from 0x%04x = 0x%02x", (unsigned) address, ret));
  } else {
    BX_DEBUG(("16-bit read from 0x%04x = 0x%04x", (unsigned) address, ret));
  }
  return ret;
}

#undef RETURN

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_vgacore_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  bx_vgacore_c *class_ptr = (bx_vgacore_c *) this_ptr;
  class_ptr->write(address, value, io_len, 0);
}

void bx_vgacore_c::write(Bit32u address, Bit32u value, unsigned io_len, bool no_log)
{
  Bit8u charmap1, charmap2, prev_memory_mapping;
  bool prev_video_enabled, prev_line_graphics, prev_int_pal_size, prev_graphics_alpha;
  bool needs_update = 0;

  if (!no_log)
    switch (io_len) {
      case 1:
        BX_DEBUG(("8-bit write to 0x%04x = 0x%02x", (Bit16u)address, (Bit8u)value));
        break;
      case 2:
        BX_DEBUG(("16-bit write to 0x%04x = 0x%04x", (Bit16u)address, (Bit16u)value));
        break;
      default:
        BX_PANIC(("Weird VGA write size"));
  }

  if (io_len == 2) {
    bx_vgacore_c::write(address, value & 0xff, 1, 1);
    bx_vgacore_c::write(address+1, (value >> 8) & 0xff, 1, 1);
    return;
  }

  if ((address >= 0x03b0) && (address <= 0x03bf) &&
      (BX_VGA_THIS s.misc_output.color_emulation))
    return;
  if ((address >= 0x03d0) && (address <= 0x03df) &&
      (BX_VGA_THIS s.misc_output.color_emulation==0))
    return;

  switch (address) {
    case 0x03ba: /* Feature Control (monochrome emulation modes) */
      break;

    case 0x03c0: /* Attribute Controller */
      if (BX_VGA_THIS s.attribute_ctrl.flip_flop == 0) { /* address mode */
        prev_video_enabled = BX_VGA_THIS s.attribute_ctrl.video_enabled;
        BX_VGA_THIS s.attribute_ctrl.video_enabled = (value >> 5) & 0x01;
        if (BX_VGA_THIS s.attribute_ctrl.video_enabled == 0)
          bx_gui->clear_screen();
        else if (!prev_video_enabled) {
          needs_update = 1;
        }
        value &= 0x1f; /* address = bits 0..4 */
        BX_VGA_THIS s.attribute_ctrl.address = value;
        switch (value) {
          case 0x00: case 0x01: case 0x02: case 0x03:
          case 0x04: case 0x05: case 0x06: case 0x07:
          case 0x08: case 0x09: case 0x0a: case 0x0b:
          case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            break;

          default:
            BX_DEBUG(("io write 0x3c0: address mode reg=0x%02x",
              (unsigned) value));
        }
      }
      else { /* data-write mode */
        switch (BX_VGA_THIS s.attribute_ctrl.address) {
          case 0x00: case 0x01: case 0x02: case 0x03:
          case 0x04: case 0x05: case 0x06: case 0x07:
          case 0x08: case 0x09: case 0x0a: case 0x0b:
          case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            if (value != BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address]) {
              BX_VGA_THIS s.attribute_ctrl.palette_reg[BX_VGA_THIS s.attribute_ctrl.address] =
                value;
              needs_update = 1;
            }
            break;
          case 0x10: // mode control register
            prev_line_graphics = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics;
            prev_int_pal_size = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha =
              (value >> 0) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type =
              (value >> 1) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics =
              (value >> 2) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity =
              (value >> 3) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat =
              (value >> 5) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select =
              (value >> 6) & 0x01;
            BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size =
              (value >> 7) & 0x01;
            if (((value >> 2) & 0x01) != prev_line_graphics) {
              BX_VGA_THIS s.vga_mem_updated |= 4;
            }
            if (((value >> 7) & 0x01) != prev_int_pal_size) {
              needs_update = 1;
            }
            break;
          case 0x11: // Overscan Color Register
            BX_VGA_THIS s.attribute_ctrl.overscan_color = (value & 0x3f);
            break;
          case 0x12: // Color Plane Enable Register
            BX_VGA_THIS s.attribute_ctrl.color_plane_enable = (value & 0x0f);
            needs_update = 1;
            break;
          case 0x13: // Horizontal Pixel Panning Register
            BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning = (value & 0x0f);
            needs_update = 1;
            break;
          case 0x14: // Color Select Register
            BX_VGA_THIS s.attribute_ctrl.color_select = (value & 0x0f);
            needs_update = 1;
            break;
          default:
            BX_DEBUG(("io write 0x3c0: data-write mode 0x%02x", BX_VGA_THIS s.attribute_ctrl.address));
        }
      }
      BX_VGA_THIS s.attribute_ctrl.flip_flop = !BX_VGA_THIS s.attribute_ctrl.flip_flop;
      break;

    case 0x03c2: // Miscellaneous Output Register
      BX_VGA_THIS s.misc_output.color_emulation  = (value >> 0) & 0x01;
      BX_VGA_THIS s.misc_output.enable_ram       = (value >> 1) & 0x01;
      BX_VGA_THIS s.misc_output.clock_select     = (value >> 2) & 0x03;
      BX_VGA_THIS s.misc_output.select_high_bank = (value >> 5) & 0x01;
      BX_VGA_THIS s.misc_output.horiz_sync_pol   = (value >> 6) & 0x01;
      BX_VGA_THIS s.misc_output.vert_sync_pol    = (value >> 7) & 0x01;
      calculate_retrace_timing();
      break;

    case 0x03c3: // VGA enable
      // bit0: enables VGA display if set
      BX_VGA_THIS s.vga_enabled = value & 0x01;
      break;

    case 0x03c4: /* Sequencer Index Register */
      if (value > 4) {
        BX_DEBUG(("io write 3c4: value > 4"));
      }
      BX_VGA_THIS s.sequencer.index = value;
      break;

    case 0x03c5: /* Sequencer Registers 00..04 */
      switch (BX_VGA_THIS s.sequencer.index) {
        case 0: /* sequencer: reset */
          if (BX_VGA_THIS s.sequencer.reset1 && ((value & 0x01) == 0)) {
            BX_VGA_THIS s.sequencer.char_map_select = 0;
            BX_VGA_THIS s.charmap_address1 = 0;
            BX_VGA_THIS s.charmap_address2 = 0;
            BX_VGA_THIS s.vga_mem_updated |= 4;
          }
          BX_VGA_THIS s.sequencer.reset1 = (value >> 0) & 0x01;
          BX_VGA_THIS s.sequencer.reset2 = (value >> 1) & 0x01;
          break;
        case 1: /* sequencer: clocking mode */
          if ((value ^ BX_VGA_THIS s.sequencer.reg1) & 0x29) {
            BX_VGA_THIS s.x_dotclockdiv2 = ((value & 0x08) > 0);
            BX_VGA_THIS s.sequencer.clear_screen = ((value & 0x20) > 0);
            calculate_retrace_timing();
            needs_update = 1;
          }
          BX_VGA_THIS s.sequencer.reg1 = value & 0x3d;
          break;
        case 2: /* sequencer: map mask register */
          BX_VGA_THIS s.sequencer.map_mask = (value & 0x0f);
          break;
        case 3: /* sequencer: character map select register */
          BX_VGA_THIS s.sequencer.char_map_select = value & 0x3f;
          charmap1 = value & 0x13;
          if (charmap1 > 3) charmap1 = (charmap1 & 3) + 4;
          charmap2 = (value & 0x2C) >> 2;
          if (charmap2 > 3) charmap2 = (charmap2 & 3) + 4;
          if (BX_VGA_THIS s.CRTC.reg[0x09] > 0) {
            BX_VGA_THIS s.charmap_address1 = charmap_offset[charmap1];
            BX_VGA_THIS s.charmap_address2 = charmap_offset[charmap2];
            BX_VGA_THIS s.vga_mem_updated |= 4;
          }
          break;
        case 4: /* sequencer: memory mode register */
          BX_VGA_THIS s.sequencer.extended_mem   = (value >> 1) & 0x01;
          BX_VGA_THIS s.sequencer.odd_even_dis   = (value >> 2) & 0x01;
          BX_VGA_THIS s.sequencer.chain_four     = (value >> 3) & 0x01;

          break;
        default:
          BX_DEBUG(("io write 0x3c5: index 0x%02x unhandled",
            (unsigned) BX_VGA_THIS s.sequencer.index));
      }
      break;

    case 0x03c6: /* PEL mask */
      if (value != BX_VGA_THIS s.pel.mask) {
        BX_VGA_THIS s.pel.mask = value;
        needs_update = 1;
      }
      break;

    case 0x03c7: // PEL address, read mode
      BX_VGA_THIS s.pel.read_data_register = value;
      BX_VGA_THIS s.pel.read_data_cycle    = 0;
      BX_VGA_THIS s.pel.dac_state = 0x03;
      break;

    case 0x03c8: /* PEL address write mode */
      BX_VGA_THIS s.pel.write_data_register = value;
      BX_VGA_THIS s.pel.write_data_cycle    = 0;
      BX_VGA_THIS s.pel.dac_state = 0x00;
      break;

    case 0x03c9: /* PEL Data Register, colors 00..FF */
      switch (BX_VGA_THIS s.pel.write_data_cycle) {
        case 0:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red = value;
          break;
        case 1:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green = value;
          break;
        case 2:
          BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue = value;

          needs_update |= bx_gui->palette_change_common(BX_VGA_THIS s.pel.write_data_register,
            BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].red   << BX_VGA_THIS s.dac_shift,
            BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].green << BX_VGA_THIS s.dac_shift,
            BX_VGA_THIS s.pel.data[BX_VGA_THIS s.pel.write_data_register].blue  << BX_VGA_THIS s.dac_shift);
          break;
      }

      BX_VGA_THIS s.pel.write_data_cycle++;
      if (BX_VGA_THIS s.pel.write_data_cycle >= 3) {
        BX_VGA_THIS s.pel.write_data_cycle = 0;
        BX_VGA_THIS s.pel.write_data_register++;
      }
      break;

    case 0x03ca: /* Graphics 2 Position (EGA) */
      // ignore, EGA only???
      break;

    case 0x03cc: /* Graphics 1 Position (EGA) */
      // ignore, EGA only???
      break;

    case 0x03cd: /* ??? */
      BX_DEBUG(("io write to 0x3cd = 0x%02x", (unsigned) value));
      break;

    case 0x03ce: /* Graphics Controller Index Register */
      if (value > 0x08) /* ??? */
        BX_DEBUG(("io write: 0x3ce: value > 8"));
      BX_VGA_THIS s.graphics_ctrl.index = value;
      break;

    case 0x03cf: /* Graphics Controller Registers 00..08 */
      switch (BX_VGA_THIS s.graphics_ctrl.index) {
        case 0: /* Set/Reset */
          BX_VGA_THIS s.graphics_ctrl.set_reset = value & 0x0f;
          break;
        case 1: /* Enable Set/Reset */
          BX_VGA_THIS s.graphics_ctrl.enable_set_reset = value & 0x0f;
          break;
        case 2: /* Color Compare */
          BX_VGA_THIS s.graphics_ctrl.color_compare = value & 0x0f;
          break;
        case 3: /* Data Rotate */
          BX_VGA_THIS s.graphics_ctrl.data_rotate = value & 0x07;
          BX_VGA_THIS s.graphics_ctrl.raster_op    = (value >> 3) & 0x03;
          break;
        case 4: /* Read Map Select */
          BX_VGA_THIS s.graphics_ctrl.read_map_select = value & 0x03;
          break;
        case 5: /* Mode */
          BX_VGA_THIS s.graphics_ctrl.write_mode        = value & 0x03;
          BX_VGA_THIS s.graphics_ctrl.read_mode         = (value >> 3) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.odd_even          = (value >> 4) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.shift_reg         = (value >> 5) & 0x03;

          if (BX_VGA_THIS s.graphics_ctrl.odd_even)
            BX_DEBUG(("io write: 0x3cf: mode reg: value = 0x%02x",
              (unsigned) value));
          if (BX_VGA_THIS s.graphics_ctrl.shift_reg)
            BX_DEBUG(("io write: 0x3cf: mode reg: value = 0x%02x",
              (unsigned) value));
          break;
        case 6: /* Miscellaneous */
          prev_graphics_alpha = BX_VGA_THIS s.graphics_ctrl.graphics_alpha;
          prev_memory_mapping = BX_VGA_THIS s.graphics_ctrl.memory_mapping;

          BX_VGA_THIS s.graphics_ctrl.graphics_alpha = value & 0x01;
          BX_VGA_THIS s.graphics_ctrl.chain_odd_even = (value >> 1) & 0x01;
          BX_VGA_THIS s.graphics_ctrl.memory_mapping = (value >> 2) & 0x03;
          if (prev_memory_mapping != BX_VGA_THIS s.graphics_ctrl.memory_mapping)
            needs_update = 1;
          if (prev_graphics_alpha != BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
            needs_update = 1;
            BX_VGA_THIS s.text_buffer_update = true;
            BX_VGA_THIS s.last_yres = 0;
          }
          break;
        case 7: /* Color Don't Care */
          BX_VGA_THIS s.graphics_ctrl.color_dont_care = value & 0x0f;
          break;
        case 8: /* Bit Mask */
          BX_VGA_THIS s.graphics_ctrl.bitmask = value;
          break;
        default:
          /* ??? */
          BX_DEBUG(("io write: 0x3cf: index %u unhandled",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.index));
      }
      break;

    case 0x03b4: /* CRTC Index Register (monochrome emulation modes) */
    case 0x03d4: /* CRTC Index Register (color emulation modes) */
      BX_VGA_THIS s.CRTC.address = value & 0x3f;
      if (BX_VGA_THIS s.CRTC.address > 0x18)
        BX_DEBUG(("write: invalid CRTC register 0x%02x selected",
          (unsigned) BX_VGA_THIS s.CRTC.address));
      break;

    case 0x03b5: /* CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* CRTC Registers (color emulation modes) */
      if (BX_VGA_THIS s.CRTC.address > 0x18) {
        BX_DEBUG(("write: invalid CRTC register 0x%02x ignored",
          (unsigned) BX_VGA_THIS s.CRTC.address));
        return;
      }
      if (BX_VGA_THIS s.CRTC.write_protect && (BX_VGA_THIS s.CRTC.address < 0x08)) {
        if (BX_VGA_THIS s.CRTC.address == 0x07) {
          BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] &= ~0x10;
          BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] |= (value & 0x10);
          BX_VGA_THIS s.line_compare &= 0x2ff;
          if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x10) BX_VGA_THIS s.line_compare |= 0x100;
          needs_update = 1;
          break;
        } else {
          return;
        }
      }
      if (value != BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address]) {
        BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] = value;
        switch (BX_VGA_THIS s.CRTC.address) {
          case 0x00:
          case 0x02:
          case 0x03:
          case 0x05:
          case 0x06:
          case 0x10:
            if ((BX_VGA_THIS s.CRTC.reg[0x03] & 0x60) > 0) {
              BX_ERROR(("CRTC: display enable skew not supported"));
            }
            calculate_retrace_timing();
            break;
          case 0x07:
            BX_VGA_THIS s.vertical_display_end &= 0xff;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x02) BX_VGA_THIS s.vertical_display_end |= 0x100;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x40) BX_VGA_THIS s.vertical_display_end |= 0x200;
            BX_VGA_THIS s.line_compare &= 0x2ff;
            if (BX_VGA_THIS s.CRTC.reg[0x07] & 0x10) BX_VGA_THIS s.line_compare |= 0x100;
            calculate_retrace_timing();
            needs_update = 1;
            break;
          case 0x08:
            // Vertical pel panning change
            if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha && ((BX_VGA_THIS s.CRTC.reg[0x08] & 0x1f) > 0)) {
              BX_ERROR(("CRTC: vertical pel panning in graphics mode not supported"));
            }
            if ((BX_VGA_THIS s.CRTC.reg[0x08] & 0x60) > 0) {
              BX_ERROR(("CRTC: byte panning not implemented yet"));
            }
            needs_update = 1;
            break;
          case 0x09:
            BX_VGA_THIS s.y_doublescan = ((value & 0x9f) > 0);
            BX_VGA_THIS s.line_compare &= 0x1ff;
            if (BX_VGA_THIS s.CRTC.reg[0x09] & 0x40) BX_VGA_THIS s.line_compare |= 0x200;
            BX_VGA_THIS s.vga_mem_updated |= 4;
            needs_update = 1;
            break;
          case 0x0A:
          case 0x0B:
          case 0x0E:
          case 0x0F:
            // Cursor size / location change
            BX_VGA_THIS s.vga_mem_updated |= 1;
            break;
          case 0x0C:
          case 0x0D:
            // Start address change
            if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
              needs_update = 1;
            } else {
              BX_VGA_THIS s.vga_mem_updated |= 1;
            }
            break;
          case 0x11:
            BX_VGA_THIS s.CRTC.write_protect = ((BX_VGA_THIS s.CRTC.reg[0x11] & 0x80) > 0);
            calculate_retrace_timing();
            break;
          case 0x12:
            BX_VGA_THIS s.vertical_display_end &= 0x300;
            BX_VGA_THIS s.vertical_display_end |= BX_VGA_THIS s.CRTC.reg[0x12];
            calculate_retrace_timing();
            break;
          case 0x13:
          case 0x14:
          case 0x17:
            // Line offset change
            BX_VGA_THIS s.line_offset = BX_VGA_THIS s.CRTC.reg[0x13] << 1;
            if (BX_VGA_THIS s.CRTC.reg[0x14] & 0x40) BX_VGA_THIS s.line_offset <<= 2;
            else if ((BX_VGA_THIS s.CRTC.reg[0x17] & 0x40) == 0) BX_VGA_THIS s.line_offset <<= 1;
            needs_update = 1;
            break;
          case 0x18:
            BX_VGA_THIS s.line_compare &= 0x300;
            BX_VGA_THIS s.line_compare |= BX_VGA_THIS s.CRTC.reg[0x18];
            needs_update = 1;
            break;
        }
      }
      break;

    case 0x03da: /* Feature Control (color emulation modes) */
      BX_DEBUG(("io write: 3da: ignoring: feature ctrl & vert sync"));
      break;

    case 0x03c1: /* */
    default:
      BX_ERROR(("unsupported io write to port 0x%04x, val=0x%02x",
        (unsigned) address, (unsigned) value));
  }

  if (needs_update) {
    // Mark all video as updated so the changes will go through
    BX_VGA_THIS vga_redraw_area(0, 0, BX_VGA_THIS s.last_xres, BX_VGA_THIS s.last_yres);
  }
}

void bx_vgacore_c::set_override(bool enabled, void *dev)
{
  BX_VGA_THIS s.vga_override = enabled;
#if BX_SUPPORT_PCI
  BX_VGA_THIS s.nvgadev = (bx_nonvga_device_c*)dev;
#endif
  if (!enabled) {
    bx_gui->dimension_update(BX_VGA_THIS s.last_xres, BX_VGA_THIS s.last_yres,
                             BX_VGA_THIS s.last_fh, BX_VGA_THIS s.last_fw, BX_VGA_THIS s.last_bpp);
    BX_VGA_THIS vga_redraw_area(0, 0, BX_VGA_THIS s.last_xres, BX_VGA_THIS s.last_yres);
    BX_VGA_THIS start_vertical_timer();
  } else {
    bx_virt_timer.deactivate_timer(BX_VGA_THIS vga_vtimer_id);
  }
  if (BX_VGA_THIS update_mode_vsync) {
    BX_VGA_THIS set_update_timer(0);
  }
}

Bit8u bx_vgacore_c::get_vga_pixel(Bit16u x, Bit16u y, Bit32u raddr, Bit16u lc, bool bs, Bit8u *vgamem_ptr)
{
  Bit8u attribute, bit_no, palette_reg_val, DAC_regno;
  Bit32u byte_offset;

  if (BX_VGA_THIS s.x_dotclockdiv2) x >>= 1;
  if ((y <= lc) || !BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat) {
    x += BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning;
  }
  bit_no = 7 - (x % 8);
  byte_offset = ((raddr + (x / 8)) << 2) & BX_VGA_THIS s.vgamem_mask;
  attribute =
    (((vgamem_ptr[byte_offset] >> bit_no) & 0x01) << 0) |
    (((vgamem_ptr[byte_offset + 1] >> bit_no) & 0x01) << 1) |
    (((vgamem_ptr[byte_offset + 2] >> bit_no) & 0x01) << 2) |
    (((vgamem_ptr[byte_offset + 3] >> bit_no) & 0x01) << 3);

  attribute &= BX_VGA_THIS s.attribute_ctrl.color_plane_enable;
  // undocumented feature ???: colors 0..7 high intensity, colors 8..15 blinking
  if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity) {
    if (bs) {
      attribute |= 0x08;
    } else {
      attribute ^= 0x08;
    }
  }
  palette_reg_val = BX_VGA_THIS s.attribute_ctrl.palette_reg[attribute];
  if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size) {
    // use 4 lower bits from palette register
    // use 4 higher bits from color select register
    // 16 banks of 16-color registers
    DAC_regno = (palette_reg_val & 0x0f) |
                (BX_VGA_THIS s.attribute_ctrl.color_select << 4);
  } else {
    // use 6 lower bits from palette register
    // use 2 higher bits from color select register
    // 4 banks of 64-color registers
    DAC_regno = (palette_reg_val & 0x3f) |
                ((BX_VGA_THIS s.attribute_ctrl.color_select & 0x0c) << 4);
  }
  DAC_regno &= BX_VGA_THIS s.pel.mask;
  return DAC_regno;
}

bool bx_vgacore_c::skip_update(void)
{
  Bit64u display_usec;

  /* handle clear screen request from the sequencer */
  if (BX_VGA_THIS s.sequencer.clear_screen) {
    bx_gui->clear_screen();
    BX_VGA_THIS s.sequencer.clear_screen = 0;
  }

  /* skip screen update when vga/video is disabled or the sequencer is in reset mode */
  if (!BX_VGA_THIS s.vga_enabled || !BX_VGA_THIS s.attribute_ctrl.video_enabled
      || (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha != BX_VGA_THIS s.graphics_ctrl.graphics_alpha)
      || !BX_VGA_THIS s.sequencer.reset2 || !BX_VGA_THIS s.sequencer.reset1
      || (BX_VGA_THIS s.sequencer.reg1 & 0x20))
    return 1;

  /* skip screen update if the vertical retrace is in progress */
  if (!BX_VGA_THIS update_mode_vsync) {
    display_usec = bx_virt_timer.time_usec(BX_VGA_THIS vsync_realtime) % BX_VGA_THIS s.vtotal_usec;
    if ((display_usec > BX_VGA_THIS s.vrstart_usec) &&
        (display_usec < BX_VGA_THIS s.vrend_usec)) {
      return 1;
    }
  }
  return 0;
}

void bx_vgacore_c::update_charmap(void)
{
  Bit8u charmap[0x2000];
  Bit32u addr;

  addr = (BX_VGA_THIS s.charmap_address1 << 2);
  for (int i = 0; i < 0x2000; i++) {
    charmap[i] = BX_VGA_THIS s.memory[addr + 2];
    addr += 4;
  }
  bx_gui->set_text_charmap(0, charmap);
  if (BX_VGA_THIS s.charmap_address2 != BX_VGA_THIS s.charmap_address1) {
    addr = (BX_VGA_THIS s.charmap_address2 << 2) + 2;
    for (int i = 0; i < 0x2000; i++) {
      charmap[i] = BX_VGA_THIS s.memory[addr];
      addr += 4;
    }
  }
  bx_gui->set_text_charmap(1, charmap);
}

void bx_vgacore_c::update(void)
{
  unsigned iHeight, iWidth;
  static unsigned cs_counter = 1;
  static bool cs_visible = 0;
  bool cs_toggle = 0;

  cs_counter--;
  /* no screen update necessary */
  if ((BX_VGA_THIS s.vga_mem_updated == 0) && (cs_counter > 0))
    return;
  if (cs_counter == 0) {
    cs_counter = BX_VGA_THIS s.blink_counter;
    if ((!BX_VGA_THIS s.graphics_ctrl.graphics_alpha) ||
        (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity)) {
      cs_toggle = 1;
      cs_visible = !cs_visible;
    } else {
      if (BX_VGA_THIS s.vga_mem_updated == 0)
        return;
      cs_toggle = 0;
      cs_visible = 0;
    }
  }

  // fields that effect the way video memory is serialized into screen output:
  // GRAPHICS CONTROLLER:
  //   BX_VGA_THIS s.graphics_ctrl.shift_reg:
  //     0: output data in standard VGA format or CGA-compatible 640x200 2 color
  //        graphics mode (mode 6)
  //     1: output data in CGA-compatible 320x200 4 color graphics mode
  //        (modes 4 & 5)
  //     2: output data 8 bits at a time from the 4 bit planes
  //        (mode 13 and variants like modeX)

  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    // Graphics mode
    Bit8u color;
    Bit32u x, y, line_offset;
    Bit32u start_addr;
    unsigned r, c;
    unsigned long byte_offset;
    unsigned xc, yc, xti, yti;

    start_addr = BX_VGA_THIS s.CRTC.start_addr + BX_VGA_THIS s.ext_start_addr;

    determine_screen_dimensions(&iHeight, &iWidth);
    if((iWidth != BX_VGA_THIS s.last_xres) || (iHeight != BX_VGA_THIS s.last_yres) ||
        (BX_VGA_THIS s.last_bpp > 8))
    {
      bx_gui->dimension_update(iWidth, iHeight);
      BX_VGA_THIS s.last_xres = iWidth;
      BX_VGA_THIS s.last_yres = iHeight;
      BX_VGA_THIS s.last_bpp = 8;
    }

    if (skip_update()) return;

    switch (BX_VGA_THIS s.graphics_ctrl.shift_reg) {
      case 0: // interleaved shift
        Bit8u attribute, palette_reg_val, DAC_regno;
        Bit16u line_compare;
        Bit32u row_addr;
        line_compare = BX_VGA_THIS s.line_compare;
        if (BX_VGA_THIS s.y_doublescan) line_compare >>= 1;

        if ((BX_VGA_THIS s.CRTC.reg[0x17] & 1) == 0) { // MAP13 (used for CGA 640x200x2)

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  /* 0 or 0x2000 */
                  row_addr = (start_addr & 0xdfff) + ((y & 1) << 13);
                  /* to the start of the line */
                  row_addr += (320 / 4) * (y / 2);
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    BX_VGA_THIS s.tile[r * X_TILESIZE + c] =
                      BX_VGA_THIS get_vga_pixel(x, y, row_addr, line_compare, cs_visible, BX_VGA_THIS s.memory);
                  }
                }
                SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
                bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        } else { // output data in serial fashion with each display plane
                 // output on its associated serial output.  Standard EGA/VGA format

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (cs_toggle || GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  if (y > line_compare) {
                    row_addr = (y - line_compare - 1) * BX_VGA_THIS s.line_offset;
                  } else {
                    row_addr = start_addr + (y * BX_VGA_THIS s.line_offset);
                  }
                  for (c=0; c<X_TILESIZE; c++) {
                    x = xc + c;
                    BX_VGA_THIS s.tile[r * X_TILESIZE + c] =
                      BX_VGA_THIS get_vga_pixel(x, y, row_addr, line_compare, cs_visible, BX_VGA_THIS s.memory);
                  }
                }
                SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
                bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        }
        break; // case 0

      case 1: // output the data in a CGA-compatible 320x200 4 color graphics
              // mode.  (planar shift, modes 4 & 5)

        /* CGA 320x200x4 start */

        start_addr <<= 1;
        for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
          for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
            if (GET_TILE_UPDATED (xti, yti)) {
              for (r=0; r<Y_TILESIZE; r++) {
                y = yc + r;
                if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                for (c=0; c<X_TILESIZE; c++) {

                  x = xc + c;
                  if (BX_VGA_THIS s.x_dotclockdiv2) x >>= 1;
                  /* to the start of the line */
                  byte_offset = start_addr + (320 / 4) * (y / 2);
                  /* to the byte start */
                  byte_offset += (x / 4);
                  byte_offset &= 0x1fff;
                  /* 0 or 0x2000 */
                  byte_offset += ((y & 1) << 13);

                  attribute = 6 - 2 * (x % 4);

                  byte_offset = ((byte_offset & ~1) << 2) | (byte_offset & 1);
                  palette_reg_val = (BX_VGA_THIS s.memory[byte_offset]) >> attribute;
                  palette_reg_val &= 3;
                  DAC_regno = BX_VGA_THIS s.attribute_ctrl.palette_reg[palette_reg_val];
                  BX_VGA_THIS s.tile[r*X_TILESIZE + c] = DAC_regno;
                }
              }
              SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
              bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
            }
          }
        }
        /* CGA 320x200x4 end */

        break; // case 1

      case 2: // output the data eight bits at a time from the 4 bit plane
              // (format for VGA mode 13 hex)
      case 3: // FIXME: is this really the same ???

        line_compare = BX_VGA_THIS s.line_compare;
        if (BX_VGA_THIS s.y_doublescan) line_compare >>= 1;

        if (BX_VGA_THIS s.CRTC.reg[0x14] & 0x40) { // DW set: doubleword mode

          if (BX_VGA_THIS s.misc_output.select_high_bank != 1)
            BX_PANIC(("update: select_high_bank != 1"));

          line_offset = BX_VGA_THIS s.line_offset;
          start_addr <<= 2;

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  if (y > line_compare) {
                    row_addr = (y - line_compare - 1) * line_offset;
                  } else {
                    row_addr = start_addr + (y * line_offset);
                  }
                  for (c=0; c<X_TILESIZE; c++) {
                    x = (xc + c) >> 1;
                    byte_offset = (row_addr + x) & 0xffff;
                    color = BX_VGA_THIS s.memory[byte_offset];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = color;
                  }
                }
                SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
                bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        } else if (BX_VGA_THIS s.CRTC.reg[0x17] & 0x40) { // B/W set: byte mode, modeX
          Bit8u h_panning;

          line_offset = BX_VGA_THIS s.line_offset << 2;
          start_addr <<= 2;

          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  h_panning = BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning / 2;
                  if (y > line_compare) {
                    row_addr = (y - line_compare - 1) * line_offset;
                    if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat) {
                      h_panning = 0;
                    }
                  } else {
                    row_addr = start_addr + (y * line_offset);
                  }
                  for (c=0; c<X_TILESIZE; c++) {
                    x = (xc + c) >> 1;
                    x += h_panning;
                    byte_offset = (row_addr + x) & 0x3ffff;
                    color = BX_VGA_THIS s.memory[byte_offset];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = color;
                  }
                }
                SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
                bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        } else { // word mode

          BX_INFO(("256-color word mode untested"));
          for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                for (r=0; r<Y_TILESIZE; r++) {
                  y = yc + r;
                  if (BX_VGA_THIS s.y_doublescan) y >>= 1;
                  row_addr = start_addr + (y * BX_VGA_THIS s.line_offset);
                  for (c=0; c<X_TILESIZE; c++) {
                    x = (xc + c) >> 1;
                    byte_offset = (row_addr + ((x & ~1) << 1)) | (x & 1);
                    color = BX_VGA_THIS s.memory[start_addr + byte_offset];
                    BX_VGA_THIS s.tile[r*X_TILESIZE + c] = color;
                  }
                }
                SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 0);
                bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
              }
            }
          }
        }
        break; // case 2

      default:
        BX_PANIC(("update: shift_reg == %u", (unsigned)
          BX_VGA_THIS s.graphics_ctrl.shift_reg));
    }

    BX_VGA_THIS s.vga_mem_updated = 0;
    return;
  } else {
    // text mode
    Bit16u cursor_address;
    bx_vga_tminfo_t tm_info;
    unsigned VDE, cols, rows, cWidth;
    Bit8u MSL;

    if ((BX_VGA_THIS s.vga_mem_updated & 4) > 0) {
      update_charmap();
    }
    tm_info.start_address = (BX_VGA_THIS s.CRTC.start_addr << 1);
    if ((BX_VGA_THIS s.CRTC.reg[0x08] & 0x60) > 0) {
      BX_ERROR(("byte panning not implemented yet"));
    }
    tm_info.cs_start = BX_VGA_THIS s.CRTC.reg[0x0a] & 0x3f;
    tm_info.cs_end = BX_VGA_THIS s.CRTC.reg[0x0b] & 0x1f;
    tm_info.line_offset = BX_VGA_THIS s.line_offset;
    tm_info.line_compare = BX_VGA_THIS s.line_compare;
    tm_info.h_panning = BX_VGA_THIS s.attribute_ctrl.horiz_pel_panning & 0x0f;
    tm_info.v_panning = BX_VGA_THIS s.CRTC.reg[0x08] & 0x1f;
    tm_info.line_graphics = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.enable_line_graphics;
    tm_info.split_hpanning = BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_panning_compat;
    tm_info.blink_flags = 0;
    if (BX_VGA_THIS s.attribute_ctrl.mode_ctrl.blink_intensity) {
      tm_info.blink_flags |= BX_TEXT_BLINK_MODE;
      if (cs_toggle)
        tm_info.blink_flags |= BX_TEXT_BLINK_TOGGLE;
    }
    if (cs_visible)
      tm_info.blink_flags |= BX_TEXT_BLINK_STATE;
    if ((BX_VGA_THIS s.sequencer.reg1 & 0x01) == 0) {
      if (tm_info.h_panning >= 8)
        tm_info.h_panning = 0;
      else
        tm_info.h_panning++;
    } else {
      tm_info.h_panning &= 0x07;
    }
    for (int index = 0; index < 16; index++) {
      tm_info.actl_palette[index] =
        BX_VGA_THIS s.attribute_ctrl.palette_reg[index] & BX_VGA_THIS s.pel.mask;
    }

    // Vertical Display End: find out how many lines are displayed
    VDE = BX_VGA_THIS s.vertical_display_end;
    // Maximum Scan Line: height of character cell
    MSL = BX_VGA_THIS s.CRTC.reg[0x09] & 0x1f;
    cols = BX_VGA_THIS s.CRTC.reg[1] + 1;
    // workaround for update() calls before VGABIOS init
    if ((cols == 1) || (MSL == 0)) {
      cols = 80;
      MSL = 15;
    }
    if ((MSL == 1) && (VDE == 399)) {
      // emulated CGA graphics mode 160x100x16 colors
      MSL = 3;
    }
    rows = (VDE+1)/(MSL+1);
    if ((rows * tm_info.line_offset) > (1 << 17)) {
      BX_ERROR(("update(): text mode: out of memory"));
      return;
    }
    cWidth = ((BX_VGA_THIS s.sequencer.reg1 & 0x01) == 1) ? 8 : 9;
    if (BX_VGA_THIS s.x_dotclockdiv2) cWidth <<= 1;
    iWidth = cWidth * cols;
    iHeight = VDE+1;
    if ((iWidth != BX_VGA_THIS s.last_xres) || (iHeight != BX_VGA_THIS s.last_yres) ||
        (cWidth != BX_VGA_THIS s.last_fw) ||((MSL+1) != BX_VGA_THIS s.last_fh) ||
        (BX_VGA_THIS s.last_bpp > 8))
    {
      bx_gui->dimension_update(iWidth, iHeight, MSL+1, cWidth);
      BX_VGA_THIS s.last_xres = iWidth;
      BX_VGA_THIS s.last_yres = iHeight;
      BX_VGA_THIS s.last_fw = cWidth;
      BX_VGA_THIS s.last_fh = MSL+1;
      BX_VGA_THIS s.last_bpp = 8;
    }
    if (skip_update()) return;

    // pass old text snapshot & new VGA memory contents
    cursor_address = 2*((BX_VGA_THIS s.CRTC.reg[0x0e] << 8) +
                     BX_VGA_THIS s.CRTC.reg[0x0f]);
    if ((cursor_address < tm_info.start_address) ||
        (cursor_address > (tm_info.start_address + tm_info.line_offset * rows))) {
      cursor_address = 0x7fff;
    }
    if (BX_VGA_THIS s.text_buffer_update) {
      int size = text_snap_size[BX_VGA_THIS s.graphics_ctrl.memory_mapping];
      for (int i = 0; i < size; i += 2) {
        BX_VGA_THIS s.text_buffer[i] = BX_VGA_THIS s.memory[i * 4];
        BX_VGA_THIS s.text_buffer[i + 1] = BX_VGA_THIS s.memory[i * 4 + 1];
      }
      BX_VGA_THIS s.text_buffer_update = false;
    }
    bx_gui->text_update_common(BX_VGA_THIS s.text_snapshot, BX_VGA_THIS s.text_buffer,
                               cursor_address, &tm_info);
    if (BX_VGA_THIS s.vga_mem_updated > 0) {
      // screen updated, copy new VGA memory contents into text snapshot
      memcpy(BX_VGA_THIS s.text_snapshot, BX_VGA_THIS s.text_buffer,
             tm_info.line_offset * rows + tm_info.start_address);
      BX_VGA_THIS s.vga_mem_updated = 0;
    }
  }
}

bool bx_vgacore_c::mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_vgacore_c *class_ptr = (bx_vgacore_c *) param;
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = class_ptr->mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

Bit8u bx_vgacore_c::mem_read(bx_phy_address addr)
{
  Bit32u offset;
  Bit8u read_map_select = BX_VGA_THIS s.graphics_ctrl.read_map_select;

  if (addr >= 0xA0000) {
    switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
      case 1: // 0xA0000 .. 0xAFFFF
        if (addr > 0xAFFFF) return 0xff;
        offset = addr & 0xFFFF;
        break;
      case 2: // 0xB0000 .. 0xB7FFF
        if ((addr < 0xB0000) || (addr > 0xB7FFF)) return 0xff;
        offset = addr & 0x7FFF;
        break;
      case 3: // 0xB8000 .. 0xBFFFF
        if (addr < 0xB8000) return 0xff;
        offset = addr & 0x7FFF;
        break;
      default: // 0xA0000 .. 0xBFFFF
        offset = addr & 0x1FFFF;
    }
  } else {
    offset = (Bit32u)addr;
  }

  if (BX_VGA_THIS s.sequencer.chain_four) {
    // Mode 13h: 320 x 200 256 color mode: chained pixel representation
    return BX_VGA_THIS s.memory[offset];
  } else if (!BX_VGA_THIS s.sequencer.odd_even_dis) {
    Bit8u plane = (read_map_select & 2) | (offset & 1);
    return BX_VGA_THIS s.memory[((offset & ~1) << 2) | plane];
  }

  offset += BX_VGA_THIS s.ext_offset;

  switch (BX_VGA_THIS s.graphics_ctrl.read_mode) {
    case 0: /* read mode 0 */
      BX_VGA_THIS s.graphics_ctrl.latch[0] = BX_VGA_THIS s.memory[offset << 2];
      BX_VGA_THIS s.graphics_ctrl.latch[1] = BX_VGA_THIS s.memory[(offset << 2) + 1];
      BX_VGA_THIS s.graphics_ctrl.latch[2] = BX_VGA_THIS s.memory[(offset << 2) + 2];
      BX_VGA_THIS s.graphics_ctrl.latch[3] = BX_VGA_THIS s.memory[(offset << 2) + 3];
      return BX_VGA_THIS s.graphics_ctrl.latch[read_map_select];

    case 1: /* read mode 1 */
      {
      Bit8u color_compare, color_dont_care;
      Bit8u latch0, latch1, latch2, latch3, retval;

      color_compare   = BX_VGA_THIS s.graphics_ctrl.color_compare & 0x0f;
      color_dont_care = BX_VGA_THIS s.graphics_ctrl.color_dont_care & 0x0f;
      latch0 = BX_VGA_THIS s.graphics_ctrl.latch[0] = BX_VGA_THIS s.memory[offset << 2];
      latch1 = BX_VGA_THIS s.graphics_ctrl.latch[1] = BX_VGA_THIS s.memory[(offset << 2) + 1];
      latch2 = BX_VGA_THIS s.graphics_ctrl.latch[2] = BX_VGA_THIS s.memory[(offset << 2) + 2];
      latch3 = BX_VGA_THIS s.graphics_ctrl.latch[3] = BX_VGA_THIS s.memory[(offset << 2) + 3];

      latch0 ^= ccdat[color_compare][0];
      latch1 ^= ccdat[color_compare][1];
      latch2 ^= ccdat[color_compare][2];
      latch3 ^= ccdat[color_compare][3];

      latch0 &= ccdat[color_dont_care][0];
      latch1 &= ccdat[color_dont_care][1];
      latch2 &= ccdat[color_dont_care][2];
      latch3 &= ccdat[color_dont_care][3];

      retval = ~(latch0 | latch1 | latch2 | latch3);

      return retval;
      }
    default:
      return 0;
  }
}

bool bx_vgacore_c::mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_vgacore_c *class_ptr = (bx_vgacore_c *) param;
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    class_ptr->mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

void bx_vgacore_c::mem_write(bx_phy_address addr, Bit8u value)
{
  Bit32u offset;
  Bit8u new_val[4] = {0,0,0,0};
  unsigned start_addr;
  Bit8u sequ_map_mask = BX_VGA_THIS s.sequencer.map_mask & 0x0f;

  if (addr >= 0xA0000) {
    switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
      case 1: // 0xA0000 .. 0xAFFFF
        if ((addr < 0xA0000) || (addr > 0xAFFFF)) return;
        offset = addr & 0xFFFF;
        break;
      case 2: // 0xB0000 .. 0xB7FFF
        if ((addr < 0xB0000) || (addr > 0xB7FFF)) return;
        offset = addr & 0x7FFF;
        break;
      case 3: // 0xB8000 .. 0xBFFFF
        if ((addr < 0xB8000) || (addr > 0xBFFFF)) return;
        offset = addr & 0x7FFF;
        break;
      default: // 0xA0000 .. 0xBFFFF
        if ((addr < 0xA0000) || (addr > 0xBFFFF)) return;
        offset = addr & 0x1FFFF;
    }
  } else {
    offset = (Bit32u)addr;
  }

  start_addr = BX_VGA_THIS s.CRTC.start_addr;

  if (BX_VGA_THIS s.sequencer.chain_four) {
    // 320 x 200 256 color mode: chained pixel representation
    BX_VGA_THIS s.memory[offset] = value;
    BX_VGA_THIS s.vga_mem_updated |= (1 << (offset % 4));
    if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
      unsigned x_tileno, y_tileno;

      if (BX_VGA_THIS s.CRTC.reg[0x14] & 0x40) {
        start_addr <<= 2;
      }
      if (BX_VGA_THIS s.line_offset > 0) {
        if (BX_VGA_THIS s.line_compare < BX_VGA_THIS s.vertical_display_end) {
          x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 2);
          if (BX_VGA_THIS s.y_doublescan) {
            y_tileno = ((offset / BX_VGA_THIS s.line_offset) + BX_VGA_THIS s.line_compare + 1) / Y_TILESIZE;
          } else {
            y_tileno = ((offset / BX_VGA_THIS s.line_offset) + BX_VGA_THIS s.line_compare + 1) / (Y_TILESIZE / 2);
          }
          SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
        }
        if (offset >= start_addr) {
          offset -= start_addr;
          x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 2);
          if (BX_VGA_THIS s.y_doublescan) {
            y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE / 2);
          } else {
            y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
          }
          SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
        }
      }
    }
    return;
  } else if (!BX_VGA_THIS s.sequencer.odd_even_dis) {
    Bit8u plane = offset & 1;
    Bit8u mask = sequ_map_mask & (0x05 << plane);
    if (mask > 0) {
      if (mask & 0x03) {
        BX_VGA_THIS s.memory[((offset & ~1) << 2) | plane] = value;
        BX_VGA_THIS s.vga_mem_updated |= (1 << plane);
      }
      if (mask & 0x0c) {
        BX_VGA_THIS s.memory[((offset & ~1) << 2) | (plane + 2)] = value;
        BX_VGA_THIS s.vga_mem_updated |= (4 << plane);
      }
      if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
        unsigned x_tileno, y_tileno;
        if ((BX_VGA_THIS s.CRTC.reg[0x17] & 1) == 0) { // MAP13 (CGA 320x200x4
          unsigned xc, yc;

          if ((BX_VGA_THIS s.CRTC.reg[0x17] & 0x40) == 0) {
            start_addr <<= 1;
          }
          offset -= start_addr;
          if (offset >= 0x2000) {
            yc = (((offset - 0x2000) / (320 / 4)) << 1) + 1;
            xc = ((offset - 0x2000) % (320 / 4)) << 2;
          } else {
            yc = (offset / (320 / 4)) << 1;
            xc = (offset % (320 / 4)) << 2;
          }
          if ((BX_VGA_THIS s.graphics_ctrl.shift_reg == 0) || BX_VGA_THIS s.x_dotclockdiv2) {
            xc <<= 1;
          }
          x_tileno = xc / X_TILESIZE;
          if (BX_VGA_THIS s.y_doublescan) {
            y_tileno = yc / (Y_TILESIZE / 2);
          } else {
            y_tileno = yc / Y_TILESIZE;
          }
          SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
        }
      } else {
        // Write to text buffer in legacy format (simplifies text update)
        Bit32u mem_mask = text_snap_size[BX_VGA_THIS s.graphics_ctrl.memory_mapping] - 1;
        BX_VGA_THIS s.text_buffer[offset & mem_mask] = value;
      }
    }
    return;
  }

  offset += BX_VGA_THIS s.ext_offset;

  switch (BX_VGA_THIS s.graphics_ctrl.write_mode) {
    unsigned i;

    case 0: /* write mode 0 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask;
        const Bit8u set_reset = BX_VGA_THIS s.graphics_ctrl.set_reset;
        const Bit8u enable_set_reset = BX_VGA_THIS s.graphics_ctrl.enable_set_reset;
        /* perform rotate on CPU data in case its needed */
        if (BX_VGA_THIS s.graphics_ctrl.data_rotate) {
          value = (value >> BX_VGA_THIS s.graphics_ctrl.data_rotate) |
                  (value << (8 - BX_VGA_THIS s.graphics_ctrl.data_rotate));
        }
        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;
        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // replace
            new_val[0] |= ((enable_set_reset & 1)
                           ? ((set_reset & 1) ? bitmask : 0)
                           : (value & bitmask));
            new_val[1] |= ((enable_set_reset & 2)
                           ? ((set_reset & 2) ? bitmask : 0)
                           : (value & bitmask));
            new_val[2] |= ((enable_set_reset & 4)
                           ? ((set_reset & 4) ? bitmask : 0)
                           : (value & bitmask));
            new_val[3] |= ((enable_set_reset & 8)
                           ? ((set_reset & 8) ? bitmask : 0)
                           : (value & bitmask));
            break;
          case 1: // AND
            new_val[0] |= ((enable_set_reset & 1)
                           ? ((set_reset & 1)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask);
            new_val[1] |= ((enable_set_reset & 2)
                           ? ((set_reset & 2)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask);
            new_val[2] |= ((enable_set_reset & 4)
                           ? ((set_reset & 4)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask);
            new_val[3] |= ((enable_set_reset & 8)
                           ? ((set_reset & 8)
                              ? (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
                              : 0)
                           : (value & BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask);
            break;
          case 2: // OR
            new_val[0]
              |= ((enable_set_reset & 1)
                  ? ((set_reset & 1)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask));
            new_val[1]
              |= ((enable_set_reset & 2)
                  ? ((set_reset & 2)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask));
            new_val[2]
              |= ((enable_set_reset & 4)
                  ? ((set_reset & 4)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask));
            new_val[3]
              |= ((enable_set_reset & 8)
                  ? ((set_reset & 8)
                     ? bitmask
                     : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask))
                  : ((value | BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask));
            break;
          case 3: // XOR
            new_val[0]
              |= ((enable_set_reset & 1)
                 ? ((set_reset & 1)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[0]) & bitmask);
            new_val[1]
              |= ((enable_set_reset & 2)
                 ? ((set_reset & 2)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[1]) & bitmask);
            new_val[2]
              |= ((enable_set_reset & 4)
                 ? ((set_reset & 4)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[2]) & bitmask);
            new_val[3]
              |= ((enable_set_reset & 8)
                 ? ((set_reset & 8)
                    ? (~BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
                    : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask))
                 : (value ^ BX_VGA_THIS s.graphics_ctrl.latch[3]) & bitmask);
            break;
          default:
            BX_PANIC(("vga_mem_write: write mode 0: op = %u",
                      (unsigned) BX_VGA_THIS s.graphics_ctrl.raster_op));
        }
      }
      break;

    case 1: /* write mode 1 */
      for (i=0; i<4; i++) {
        new_val[i] = BX_VGA_THIS s.graphics_ctrl.latch[i];
      }
      break;

    case 2: /* write mode 2 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask;

        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;
        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // write
            new_val[0] |= (value & 1) ? bitmask : 0;
            new_val[1] |= (value & 2) ? bitmask : 0;
            new_val[2] |= (value & 4) ? bitmask : 0;
            new_val[3] |= (value & 8) ? bitmask : 0;
            break;
          case 1: // AND
            new_val[0] |= (value & 1)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
              : 0;
            new_val[1] |= (value & 2)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
              : 0;
            new_val[2] |= (value & 4)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
              : 0;
            new_val[3] |= (value & 8)
              ? (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
              : 0;
            break;
          case 2: // OR
            new_val[0] |= (value & 1)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask);
            new_val[1] |= (value & 2)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask);
            new_val[2] |= (value & 4)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask);
            new_val[3] |= (value & 8)
              ? bitmask
              : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask);
            break;
          case 3: // XOR
            new_val[0] |= (value & 1)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[0] & bitmask);
            new_val[1] |= (value & 2)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[1] & bitmask);
            new_val[2] |= (value & 4)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[2] & bitmask);
            new_val[3] |= (value & 8)
              ? (~BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask)
              : (BX_VGA_THIS s.graphics_ctrl.latch[3] & bitmask);
            break;
        }
      }
      break;

    case 3: /* write mode 3 */
      {
        const Bit8u bitmask = BX_VGA_THIS s.graphics_ctrl.bitmask & value;
        const Bit8u set_reset = BX_VGA_THIS s.graphics_ctrl.set_reset;

        /* perform rotate on CPU data */
        if (BX_VGA_THIS s.graphics_ctrl.data_rotate) {
          value = (value >> BX_VGA_THIS s.graphics_ctrl.data_rotate) |
                  (value << (8 - BX_VGA_THIS s.graphics_ctrl.data_rotate));
        }
        new_val[0] = BX_VGA_THIS s.graphics_ctrl.latch[0] & ~bitmask;
        new_val[1] = BX_VGA_THIS s.graphics_ctrl.latch[1] & ~bitmask;
        new_val[2] = BX_VGA_THIS s.graphics_ctrl.latch[2] & ~bitmask;
        new_val[3] = BX_VGA_THIS s.graphics_ctrl.latch[3] & ~bitmask;

        value &= bitmask;

        switch (BX_VGA_THIS s.graphics_ctrl.raster_op) {
          case 0: // write
            new_val[0] |= (set_reset & 1) ? value : 0;
            new_val[1] |= (set_reset & 2) ? value : 0;
            new_val[2] |= (set_reset & 4) ? value : 0;
            new_val[3] |= (set_reset & 8) ? value : 0;
            break;
          case 1: // AND
            new_val[0] |= ((set_reset & 1) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              & BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
          case 2: // OR
            new_val[0] |= ((set_reset & 1) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              | BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
          case 3: // XOR
            new_val[0] |= ((set_reset & 1) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[0];
            new_val[1] |= ((set_reset & 2) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[1];
            new_val[2] |= ((set_reset & 4) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[2];
            new_val[3] |= ((set_reset & 8) ? value : 0)
              ^ BX_VGA_THIS s.graphics_ctrl.latch[3];
            break;
        }
      }
      break;

    default:
      BX_PANIC(("vga_mem_write: write mode %u ?",
        (unsigned) BX_VGA_THIS s.graphics_ctrl.write_mode));
  }

  if (sequ_map_mask & 0x0f) {
    BX_VGA_THIS s.vga_mem_updated |= (sequ_map_mask & 0x0f);
    if (sequ_map_mask & 0x01)
      BX_VGA_THIS s.memory[offset << 2] = new_val[0];
    if (sequ_map_mask & 0x02)
      BX_VGA_THIS s.memory[(offset << 2) + 1] = new_val[1];
    if (sequ_map_mask & 0x04)
      BX_VGA_THIS s.memory[(offset << 2) + 2] = new_val[2];
    if (sequ_map_mask & 0x08)
      BX_VGA_THIS s.memory[(offset << 2) + 3] = new_val[3];

    if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
      unsigned x_tileno, y_tileno;
      if ((BX_VGA_THIS s.CRTC.reg[0x17] & 1) == 0) { // MAP13 (CGA 320x200x4 / 640x200x2)
        unsigned xc, yc;

        if ((BX_VGA_THIS s.CRTC.reg[0x17] & 0x40) == 0) {
          start_addr <<= 1;
        }
        offset -= start_addr;
        if (offset >= 0x2000) {
          yc = (((offset - 0x2000) / (320 / 4)) << 1) + 1;
          xc = ((offset - 0x2000) % (320 / 4)) << 2;
        } else {
          yc = (offset / (320 / 4)) << 1;
          xc = (offset % (320 / 4)) << 2;
        }
        if ((BX_VGA_THIS s.graphics_ctrl.shift_reg == 0) || BX_VGA_THIS s.x_dotclockdiv2) {
          xc <<= 1;
        }
        x_tileno = xc / X_TILESIZE;
        if (BX_VGA_THIS s.y_doublescan) {
          y_tileno = yc / (Y_TILESIZE / 2);
        } else {
          y_tileno = yc / Y_TILESIZE;
        }
        SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
      } else if (BX_VGA_THIS s.graphics_ctrl.shift_reg == 2) {
        offset -= start_addr;
        x_tileno = (offset % BX_VGA_THIS s.line_offset) * 4 / (X_TILESIZE / 2);
        if (BX_VGA_THIS s.y_doublescan) {
          y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE / 2);
        } else {
          y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
        }
        SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
      } else {
        if (BX_VGA_THIS s.line_offset > 0) {
          if (BX_VGA_THIS s.line_compare < BX_VGA_THIS s.vertical_display_end) {
            if (BX_VGA_THIS s.x_dotclockdiv2) {
              x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 16);
            } else {
              x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 8);
            }
            if (BX_VGA_THIS s.y_doublescan) {
              y_tileno = ((offset / BX_VGA_THIS s.line_offset) * 2 + BX_VGA_THIS s.line_compare + 1) / Y_TILESIZE;
            } else {
              y_tileno = ((offset / BX_VGA_THIS s.line_offset) + BX_VGA_THIS s.line_compare + 1) / Y_TILESIZE;
            }
            SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
          }
          if (offset >= start_addr) {
            offset -= start_addr;
            if (BX_VGA_THIS s.x_dotclockdiv2) {
              x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 16);
            } else {
              x_tileno = (offset % BX_VGA_THIS s.line_offset) / (X_TILESIZE / 8);
            }
            if (BX_VGA_THIS s.y_doublescan) {
              y_tileno = (offset / BX_VGA_THIS s.line_offset) / (Y_TILESIZE / 2);
            } else {
              y_tileno = (offset / BX_VGA_THIS s.line_offset) / Y_TILESIZE;
            }
            SET_TILE_UPDATED(BX_VGA_THIS, x_tileno, y_tileno, 1);
          }
        }
      }
    }
  }
}

void bx_vgacore_c::get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight,
                                                   unsigned *txWidth)
{
  unsigned VDE, MSL;

  if (!BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    *text_snapshot = &BX_VGA_THIS s.text_snapshot[0];
    VDE = BX_VGA_THIS s.vertical_display_end;
    MSL = BX_VGA_THIS s.CRTC.reg[0x09] & 0x1f;
    *txHeight = (VDE+1)/(MSL+1);
    *txWidth = BX_VGA_THIS s.CRTC.reg[1] + 1;
  } else {
    *txHeight = 0;
    *txWidth = 0;
  }
}

#if BX_DEBUGGER
void bx_vgacore_c::debug_dump(int argc, char **argv)
{
  dbg_printf("Standard VGA adapter\n\n");
  dbg_printf("s.misc_output.color_emulation = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.color_emulation);
  dbg_printf("s.misc_output.enable_ram = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.enable_ram);
  dbg_printf("s.misc_output.clock_select = %u ",
            (unsigned) BX_VGA_THIS s.misc_output.clock_select);
  if (BX_VGA_THIS s.misc_output.clock_select == 0)
    dbg_printf("(25Mhz 640 horiz pixel clock)\n");
  else
    dbg_printf("(28Mhz 720 horiz pixel clock)\n");
  dbg_printf("s.misc_output.select_high_bank = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.select_high_bank);
  dbg_printf("s.misc_output.horiz_sync_pol = %u\n",
            (unsigned) BX_VGA_THIS s.misc_output.horiz_sync_pol);
  dbg_printf("s.misc_output.vert_sync_pol = %u ",
            (unsigned) BX_VGA_THIS s.misc_output.vert_sync_pol);
  switch (((int)BX_VGA_THIS s.misc_output.vert_sync_pol << 1) |
           (int)BX_VGA_THIS s.misc_output.horiz_sync_pol) {
    case 1: dbg_printf("(400 lines)\n"); break;
    case 2: dbg_printf("(350 lines)\n"); break;
    case 3: dbg_printf("(480 lines)\n"); break;
    default: dbg_printf("(reserved)\n");
  }

  dbg_printf("s.graphics_ctrl.odd_even = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.odd_even);
  dbg_printf("s.graphics_ctrl.chain_odd_even = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.chain_odd_even);
  dbg_printf("s.graphics_ctrl.shift_reg = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.shift_reg);
  dbg_printf("s.graphics_ctrl.graphics_alpha = %u\n",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.graphics_alpha);
  dbg_printf("s.graphics_ctrl.memory_mapping = %u ",
            (unsigned) BX_VGA_THIS s.graphics_ctrl.memory_mapping);
  switch (BX_VGA_THIS s.graphics_ctrl.memory_mapping) {
    case 1: dbg_printf("(A0000-AFFFF)\n"); break;
    case 2: dbg_printf("(B0000-B7FFF)\n"); break;
    case 3: dbg_printf("(B8000-BFFFF)\n"); break;
    default: dbg_printf("(A0000-BFFFF)\n"); break;
  }

  dbg_printf("s.sequencer.extended_mem = %u\n",
            (unsigned) BX_VGA_THIS s.sequencer.extended_mem);
  dbg_printf("s.sequencer.odd_even_dis = %u (inverted)\n",
            (unsigned) BX_VGA_THIS s.sequencer.odd_even_dis);
  dbg_printf("s.sequencer.chain_four = %u\n",
            (unsigned) BX_VGA_THIS s.sequencer.chain_four);

  dbg_printf("s.attribute_ctrl.video_enabled = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.video_enabled);
  dbg_printf("s.attribute_ctrl.mode_ctrl.graphics_alpha = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.graphics_alpha);
  dbg_printf("s.attribute_ctrl.mode_ctrl.display_type = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.display_type);
  dbg_printf("s.attribute_ctrl.mode_ctrl.internal_palette_size = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.internal_palette_size);
  dbg_printf("s.attribute_ctrl.mode_ctrl.pixel_clock_select = %u\n",
            (unsigned) BX_VGA_THIS s.attribute_ctrl.mode_ctrl.pixel_clock_select);

  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif

void bx_vgacore_c::vga_redraw_area(unsigned x0, unsigned y0, unsigned width,
                                   unsigned height)
{
  if (width == 0 || height == 0) {
    return;
  }
#if BX_SUPPORT_PCI
  if (BX_VGA_THIS s.vga_override && (BX_VGA_THIS s.nvgadev != NULL)) {
    BX_VGA_THIS s.nvgadev->redraw_area(x0, y0, width, height);
    return;
  }
#endif
  redraw_area(x0, y0, width, height);
}

void bx_vgacore_c::redraw_area(unsigned x0, unsigned y0, unsigned width, unsigned height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1, xmax, ymax;

  BX_VGA_THIS s.vga_mem_updated |= 0x07;

  if (BX_VGA_THIS s.graphics_ctrl.graphics_alpha) {
    // graphics mode
    xmax = BX_VGA_THIS s.last_xres;
    ymax = BX_VGA_THIS s.last_yres;
    if ((xmax == 0) || (ymax == 0))
      return;
    xt0 = x0 / X_TILESIZE;
    yt0 = y0 / Y_TILESIZE;
    if (x0 < xmax) {
      xt1 = (x0 + width  - 1) / X_TILESIZE;
    } else {
      xt1 = (xmax - 1) / X_TILESIZE;
    }
    if (y0 < ymax) {
      yt1 = (y0 + height - 1) / Y_TILESIZE;
    } else {
      yt1 = (ymax - 1) / Y_TILESIZE;
    }
    for (yti=yt0; yti<=yt1; yti++) {
      for (xti=xt0; xti<=xt1; xti++) {
        SET_TILE_UPDATED(BX_VGA_THIS, xti, yti, 1);
      }
    }

  } else {
    // text mode
    memset(BX_VGA_THIS s.text_snapshot, 0,
           text_snap_size[BX_VGA_THIS s.graphics_ctrl.memory_mapping]);
  }
}

void bx_vgacore_c::refresh_display(void *this_ptr, bool redraw)
{
  bx_vgacore_c *vgadev = (bx_vgacore_c *) this_ptr;
#if BX_SUPPORT_PCI
  if (vgadev->s.vga_override && (vgadev->s.nvgadev != NULL)) {
    vgadev->s.nvgadev->refresh_display(vgadev->s.nvgadev, redraw);
    return;
  }
#endif
  if (redraw) {
    vga_redraw_area(0, 0, vgadev->s.last_xres, vgadev->s.last_yres);
  }
  vga_timer_handler(vgadev);
}

void bx_vgacore_c::vga_timer_handler(void *this_ptr)
{
  bx_vgacore_c *vgadev = (bx_vgacore_c *) this_ptr;
#if BX_SUPPORT_PCI
  if (vgadev->s.vga_override && (vgadev->s.nvgadev != NULL)) {
    vgadev->s.nvgadev->update();
  }
  else
#endif
  {
    vgadev->update();
  }
  bx_gui->flush();
}

void bx_vgacore_c::vertical_timer_handler(void *this_ptr)
{
  ((bx_vgacore_c *)this_ptr)->vertical_timer();
}

void bx_vgacore_c::vertical_timer(void)
{
  BX_VGA_THIS vtimer_toggle ^= 1;
  bx_virt_timer.activate_timer(BX_VGA_THIS vga_vtimer_id,
    BX_VGA_THIS vtimer_interval[BX_VGA_THIS vtimer_toggle], 0);
  if (BX_VGA_THIS vtimer_toggle) {
    BX_VGA_THIS s.CRTC.start_addr = (BX_VGA_THIS s.CRTC.reg[0x0c] << 8) | BX_VGA_THIS s.CRTC.reg[0x0d];
  } else {
    BX_VGA_THIS s.display_start_usec = bx_virt_timer.time_usec(BX_VGA_THIS vsync_realtime);
  }
}

void bx_vgacore_c::set_update_timer(Bit32u usec)
{
  if (BX_VGA_THIS update_mode_vsync) {
#if BX_SUPPORT_PCI
    if (BX_VGA_THIS s.vga_override && (BX_VGA_THIS s.nvgadev != NULL)) {
      usec = BX_VGA_THIS s.nvgadev->get_vtotal_usec();
    } else
#endif
    {
      usec = BX_VGA_THIS s.vtotal_usec;
    }
    if ((usec < 8000) || (usec > 200000)) {
      usec = 100000;
    }
  }
  if (usec != BX_VGA_THIS vga_update_interval) {
    BX_INFO(("Setting VGA update interval to %d (%.1f Hz)", usec, 1000000.0 / (float)usec));
    bx_virt_timer.activate_timer(BX_VGA_THIS update_timer_id, usec, 1);
    // VGA text mode cursor blink frequency 1.875 Hz
    if (usec < 266666) {
      BX_VGA_THIS s.blink_counter = 266666 / (unsigned)usec;
    } else {
      BX_VGA_THIS s.blink_counter = 1;
    }
    BX_VGA_THIS vga_update_interval = usec;
  }
}

void bx_vgacore_c::start_vertical_timer(void)
{
  BX_VGA_THIS vtimer_toggle = 0;
  BX_VGA_THIS vtimer_interval[0] = BX_VGA_THIS s.vrend_usec;
  BX_VGA_THIS vtimer_interval[1] = BX_VGA_THIS s.vtotal_usec - BX_VGA_THIS s.vrend_usec;
  bx_virt_timer.activate_timer(BX_VGA_THIS vga_vtimer_id, vtimer_interval[0], 0);
}

#undef LOG_THIS
#define LOG_THIS vgadev->

Bit64s bx_vgacore_c::vga_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  // handler for runtime parameter 'vga: update_freq'
  if (set) {
    Bit32u update_interval;
    if (val > 0) {
      update_interval = (Bit32u)(1000000 / val);
    } else {
      update_interval = 0;
    }
    bx_vgacore_c *vgadev = (bx_vgacore_c *)param->get_device_param();
    vgadev->set_update_timer(update_interval);
  }
  return val;
}
