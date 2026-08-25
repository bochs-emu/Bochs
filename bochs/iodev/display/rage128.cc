/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation: device shell, PCI configuration, register
//  file dispatch, VGA passthrough and the display block (PLL / CRTC / DAC /
//  palette / hardware cursor / scanout).
//
//  Modelled after the 86Box Rage 128 Pro emulation (vid_ati_rage128*.c).
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
#if BX_SUPPORT_PCI && BX_SUPPORT_RAGE128

#include "vgacore.h"
#include "pci.h"
#include "ddc.h"
#include "rage128_regs.h"
#include "rage128.h"
#include "virt_timer.h"
#include "bx_debug/debug.h"

#define LOG_THIS BX_RAGE128_THIS

static bx_rage128_c *theSvga = NULL;

// ---------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------

static const char *rage128_memsize_list[] = {
  "16",
  "32",
  NULL
};

static int rage128_cfg_threads = 1;

static void rage128_init_options(void)
{
  bx_param_enum_c *model = SIM->get_param_enum(BXPN_VGA_EXT_MODEL);
  model->set_enabled(1);
  model->set_label("Rage 128 memory size (MB)");
  model->set_description("Size of the Rage 128 PRO video memory in MB");
  model->set_choices(rage128_memsize_list, 1, 0);
}

static void rage128_cleanup_options(void)
{
  bx_param_enum_c *model = SIM->get_param_enum(BXPN_VGA_EXT_MODEL);
  model->set_choices(NULL, 0, 0);
  model->set_label("Model");
  model->set_enabled(0);
}

Bit32s bx_rage128_c::options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "rage128")) {
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "threads=", 8)) {
        int n = atoi(&params[i][8]);
        if (n < 1) n = 1;
        if (n > RAGE128_RASTER_MAX_WORKERS) n = RAGE128_RASTER_MAX_WORKERS;
        rage128_cfg_threads = n;
      } else if (!strncmp(params[i], "memory=", 7)) {
        SIM->get_param_enum(BXPN_VGA_EXT_MODEL)->set_by_name(&params[i][7]);
      } else {
        if (theSvga != NULL)
          theSvga->error("%s: unknown parameter '%s' for rage128 ignored.", context, params[i]);
      }
    }
  }
  return 0;
}

Bit32s bx_rage128_c::options_save(FILE *fp)
{
  return fprintf(fp, "rage128: threads=%d\n", rage128_cfg_threads);
}

// ---------------------------------------------------------------------
// Plugin entry point
// ---------------------------------------------------------------------

PLUGIN_ENTRY_FOR_MODULE(rage128)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_rage128_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_RAGE128);
    rage128_init_options();
    SIM->register_addon_option("rage128", bx_rage128_c::options_parser, bx_rage128_c::options_save);
  } else if (mode == PLUGIN_FINI) {
    rage128_cleanup_options();
    SIM->unregister_addon_option("rage128");
    delete theSvga;
    theSvga = NULL;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_VGA;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

// ---------------------------------------------------------------------
// Construction / initialisation
// ---------------------------------------------------------------------

bx_rage128_c::bx_rage128_c() : bx_vgacore_c()
{
  put("RAGE128");
  cce_fifo = NULL;
  cce_fifo_rptr = NULL;
  cce_fifo_tag = NULL;
  cce_pl = NULL;
  ind_pl = NULL;
  cce_thread_started = false;
  cce_thread_run = false;
  raster_run = false;
  memset(&rb, 0, sizeof(rb));
  memset(&tex_stage, 0, sizeof(tex_stage));
  memset(&vtx_cache, 0, sizeof(vtx_cache));
  memset(&z_stage, 0, sizeof(z_stage));
  memset(&c_stage, 0, sizeof(c_stage));
  memset(&s2d_dst, 0, sizeof(s2d_dst));
  memset(&s2d_src, 0, sizeof(s2d_src));
  vline_timer_id = BX_NULL_TIMER_HANDLE;
  pump_timer_id = BX_NULL_TIMER_HANDLE;
  in_indirect = false;
}

bx_rage128_c::~bx_rage128_c()
{
  pm4_thread_close();
  raster_close();
  if (tex_stage.arena != NULL) free(tex_stage.arena);
  if (z_stage.arena != NULL) free(z_stage.arena);
  if (c_stage.arena != NULL) free(c_stage.arena);
  if (s2d_dst.arena != NULL) free(s2d_dst.arena);
  if (s2d_src.arena != NULL) free(s2d_src.arena);
  if (vtx_cache.v != NULL) free(vtx_cache.v);
  if (vtx_cache.gen != NULL) free(vtx_cache.gen);
  if (BX_RAGE128_THIS s.memory != NULL) {
    delete [] BX_RAGE128_THIS s.memory;
    BX_RAGE128_THIS s.memory = NULL;
  }
  if (SIM->get_bochs_root())
    SIM->get_bochs_root()->remove("rage128");
  BX_DEBUG(("Exit"));
}

bool bx_rage128_c::init_vga_extension(void)
{
  is_agp = SIM->is_agp_device(BX_PLUGIN_RAGE128);
  if (!SIM->is_pci_device(BX_PLUGIN_RAGE128) && !is_agp) {
    BX_PANIC(("ATI Rage 128 PRO must be assigned to a PCI or AGP slot"));
  }
  BX_RAGE128_THIS pci_enabled = true;

  BX_RAGE128_THIS init_iohandlers(svga_read_handler, svga_write_handler, "rage128");

  // Video memory: 16 or 32 MB (the model parameter of the vga extension)
  vram_size = atoi(SIM->get_param_enum(BXPN_VGA_EXT_MODEL)->get_selected()) << 20;
  if ((vram_size != (16 << 20)) && (vram_size != (32 << 20)))
    vram_size = 32 << 20;
  vram_mask = vram_size - 1;
  BX_RAGE128_THIS s.memsize = vram_size;
  if (BX_RAGE128_THIS s.memory == NULL)
    BX_RAGE128_THIS s.memory = new Bit8u[vram_size + 16];
  memset(BX_RAGE128_THIS s.memory, 0, vram_size + 16);

  BX_RAGE128_THIS s.max_xres = 2048;
  BX_RAGE128_THIS s.max_yres = 1536;
  BX_RAGE128_THIS s.CRTC.max_reg = 0x18;

  render_threads = rage128_cfg_threads;
  if (is_agp) {
    pci_device_id = RAGE128_PCI_DEVICE_PF;
    pci_subsys_id = RAGE128_SUBSYS_ID_PF;
    ref_freq_hz = RAGE128_REF_FREQ_AGP_HZ;
  } else {
    pci_device_id = RAGE128_PCI_DEVICE_TR;
    pci_subsys_id = RAGE128_SUBSYS_ID_TR;
    ref_freq_hz = RAGE128_REF_FREQ_PCI_HZ;
  }
  vga_disabled = false;

  if (vline_timer_id == BX_NULL_TIMER_HANDLE) {
    vline_timer_id = bx_virt_timer.register_timer(this, vline_timer_handler, 1000, 0, 0, 0, "rage128 vline");
  }
  if (pump_timer_id == BX_NULL_TIMER_HANDLE) {
    pump_timer_id = bx_virt_timer.register_timer(this, pump_timer_handler, 100, 0, 0, 0, "rage128 cce pump");
  }

  init_pcihandlers();
  pm4_thread_init();
  raster_init();
  init_members();
  ddc.init();
  chip_reset();

  BX_INFO(("ATI Rage 128 PRO (%s, %u MB, %d render thread%s) initialized",
           is_agp ? "AGP" : "PCI", vram_size >> 20, render_threads,
           (render_threads == 1) ? "" : "s"));
#if BX_DEBUGGER
  bx_dbg_register_debug_info("rage128", this);
#endif
  return 1;
}

void bx_rage128_c::init_members(void)
{
  mm_index = 0;
  memset(bios_scratch, 0, sizeof(bios_scratch));
  memset(pll_regs, 0, sizeof(pll_regs));
  memset(ppll_work, 0, sizeof(ppll_work));
  memset(ext_crtc, 0, sizeof(ext_crtc));
  disp_ext = false;
  disp_xres = 640;
  disp_yres = 480;
  disp_bpp = 8;
  disp_pitch = 640;
  disp_base = 0;
  disp_dblscan = false;
  disp_blank = false;
  disp_dac_const = false;
  disp_dac_const_color = 0;
  needs_update_mode = true;
  needs_update_tile = true;
  needs_update_dispentire = true;
  ext_last = false;
  gui_idle_event = 0;
  irq_dirty = false;
  timing_dirty = false;
  palette_dirty = false;
  hostdata_active = false;
  hostdata_ndw = 0;
  memset(scl_palette, 0, sizeof(scl_palette));
  memset(gui_scratch, 0, sizeof(gui_scratch));
  memset(brush_data, 0, sizeof(brush_data));
}

// Power-on defaults for the chip core registers
void bx_rage128_c::chip_reset(void)
{
  pm4_drain_wait();

  mm_index = 0;
  timing_valid = false;
  gui_xres = gui_yres = gui_bpp = 0;
  memset(bios_scratch, 0, sizeof(bios_scratch));
  // System-BIOS compatibility seed for BIOS_1_SCRATCH (hardware resets it
  // to 0). The Rage 128 video BIOS never reads its BARs at runtime: at
  // POST its rom_init stores the PCI bus/devfn the system BIOS passes in
  // AX, asks PCI BIOS INT 1A/B109 for BAR1 and writes {ROM segment, that
  // AX word} to BIOS_1_SCRATCH+0/+2 at the real IO base; every later
  // register access (INT 10h mode sets, the VBE memory size, the driver
  // hand-off) first checks that stored base and otherwise scans
  // 'in ax, XX16' for XX = 0xff..0x01 looking for the signature word.
  // The Bochs ROM BIOS (rom_scan) does not implement the PCI Firmware
  // Specification convention and calls the init entry with AX = 0xF000
  // (its own segment), so the B109 query hits bus 0xF0, the ROM adopts a
  // dead IO base and its locator never finds BAR1: all register reads then
  // return 0xFF (VBE reports "-64 KB", no modes, no LFB, the hardware
  // init never runs). Pre-loading the register with exactly what rom_init
  // would have written at the true base (segment 0xC000, signature
  // 0xF000) makes the locator scan land on BAR1 instead. A BIOS that does
  // pass the device address (SeaBIOS, real firmware) makes the ROM
  // overwrite this value before its first use, so it is inert there.
  bios_scratch[1] = 0xf000c000;
  memset(t3d.fog_table, 0xff, sizeof(t3d.fog_table));
  fog_table_wr_index = 0;
  tex_pal_wr_index = 0;

  gen_int_cntl = 0;
  gen_int_status = RAGE128_GIS_GUI_IDLE;
  gui_idle_event = 0;
  gen_int_update();

  bus_cntl = RAGE128_BUS_CNTL_DEFAULT;
  bus_cntl1 = 0;
  config_cntl = vga_disabled ? RAGE128_CFG_VGA_IO_DIS : 0;
  gen_reset_cntl = 0;
  config_memsize = vram_size & RAGE128_CONFIG_MEMSIZE_MASK;
  test_debug_cntl = 0;
  test_debug_mux = 0;
  hw_debug = 0;
  host_path_cntl = RAGE128_HOST_PATH_CNTL_DEFAULT;
  mem_cntl = RAGE128_MEM_CNTL_DEFAULT;
  ext_mem_cntl = RAGE128_EXT_MEM_CNTL_DEFAULT;
  if (is_agp)
    mem_addr_config = (vram_size == 0x02000000) ? 0x0003012c : 0x00000000;
  else
    mem_addr_config = 0x0002002c;
  mem_intf_cntl = 0;
  mem_str_cntl = 0;
  mem_init_lat_timer = RAGE128_MEM_INIT_LAT_DEFAULT;
  mem_sdram_mode_reg = RAGE128_MEM_SDRAM_MODE_DEFAULT;
  pad_ctlr_strength = RAGE128_PAD_CTLR_STRENGTH_DEFAULT;
  pc_misc_ctl = 0;
  videomux_cntl = RAGE128_VIDEOMUX_CNTL_DEFAULT;
  surface_delay = RAGE128_SURFACE_DELAY_DEFAULT;
  memset(surf_lower, 0, sizeof(surf_lower));
  memset(surf_upper, 0, sizeof(surf_upper));
  memset(surf_info, 0, sizeof(surf_info));
  surf_xlate_on = false;
  agp_base = 0;
  agp_cntl = RAGE128_AGP_CNTL_DEFAULT;
  agp_cntl_b = 0;
  gui_debug0 = 0;
  pc_gui_mode = 0;
  bm_chunk_val[0] = bm_chunk_val[1] = 0;
  pm4_vc_debug_config = 0;
  gpio_monid = 0;
  ddc.write(1, 1);

  display_reset();
  ov0_reset();
  r2d_reset();
  pm4_reset();
  r3d_reset();
  update_vga_decode();
}

void bx_rage128_c::reset(unsigned type)
{
  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x04, 0x00 }, { 0x05, 0x00 },
    { 0x0c, 0x00 }, { 0x0d, 0x00 },
    { 0x3c, 0xff },
    { 0x58, 0x00 }, { 0x59, 0x00 }, { 0x5a, 0x00 }, { 0x5b, 0x00 },
    { 0x60, 0x00 },
  };
  for (unsigned i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
    pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }
  BX_RAGE128_THIS bx_vgacore_c::reset(type);
  chip_reset();
  needs_update_mode = true;
  needs_update_dispentire = true;
}

// ---------------------------------------------------------------------
// PCI configuration space
// ---------------------------------------------------------------------

static const Bit8u rage128_iomask[256] = {
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
  7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1
};

void bx_rage128_c::init_pcihandlers(void)
{
  devfunc = 0x00;
  DEV_register_pci_handlers2(BX_RAGE128_THIS_PTR, &devfunc, BX_PLUGIN_RAGE128,
                             is_agp ? "ATI Rage 128 PRO AGP" : "ATI Rage 128 PRO PCI", is_agp);

  init_pci_conf(RAGE128_PCI_VENDOR, pci_device_id, 0x00, 0x030000, 0x00, BX_PCI_INTA);

  // BAR0: 64 MB prefetchable framebuffer aperture
  pci_conf[0x10] = 0x08;
  init_bar_mem(0, RAGE128_LFB_SIZE, mem_read_handler, mem_write_handler);
  // BAR1: 256 byte IO block
  init_bar_io(1, RAGE128_IO_SIZE, io_read_handler, io_write_handler, rage128_iomask);
  // BAR2: 16 KB register aperture
  init_bar_mem(2, RAGE128_MMIO_SIZE, mem_read_handler, mem_write_handler);

  load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr(), mem_read_handler);

  // Status: capabilities list, 66 MHz (AGP board), fast back-to-back, medium DEVSEL
  pci_conf[0x06] = is_agp ? 0xb0 : 0x90;
  pci_conf[0x07] = 0x02;
  // Subsystem ID
  pci_conf[0x2c] = RAGE128_SUBSYS_VENDOR & 0xff;
  pci_conf[0x2d] = RAGE128_SUBSYS_VENDOR >> 8;
  pci_conf[0x2e] = pci_subsys_id & 0xff;
  pci_conf[0x2f] = pci_subsys_id >> 8;
  pci_conf[0x34] = is_agp ? RAGE128_PCI_CAP_PTR : RAGE128_AGP_NEXT_PTR;
  pci_conf[0x3e] = 0x08; // min grant
  pci_conf[0x3f] = 0x00; // max latency
  if (is_agp) {
    pci_conf[0x50] = RAGE128_AGP_CAP_ID;
    pci_conf[0x51] = RAGE128_AGP_NEXT_PTR;
    pci_conf[0x52] = RAGE128_AGP_REV;
    pci_conf[0x53] = 0x00;
    pci_conf[0x54] = RAGE128_AGP_STATUS & 0xff;
    pci_conf[0x55] = (RAGE128_AGP_STATUS >> 8) & 0xff;
    pci_conf[0x56] = (RAGE128_AGP_STATUS >> 16) & 0xff;
    pci_conf[0x57] = (RAGE128_AGP_STATUS >> 24) & 0xff;
  }
  // Power management capability
  pci_conf[0x5c] = RAGE128_PMI_CAP_ID;
  pci_conf[0x5d] = 0x00;
  pci_conf[0x5e] = RAGE128_PMI_PMC & 0xff;
  pci_conf[0x5f] = RAGE128_PMI_PMC >> 8;
}

Bit32u bx_rage128_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = bx_pci_device_c::pci_read_handler(address, io_len);
  // AGP_COMMAND SBA_EN [9] reads as 1 (AGP board)
  if (is_agp && (address <= 0x59) && ((address + io_len) > 0x59)) {
    value |= (RAGE128_AGP_COMMAND_SBA_EN >> 8) << ((0x59 - address) * 8);
  }
  return value;
}

void bx_rage128_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;

  if ((address >= 0x1c) && (address < 0x2c))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i = 0; i < io_len; i++) {
    value8 = (value >> (i * 8)) & 0xff;
    oldval = pci_conf[address + i];
    switch (address + i) {
      case 0x04:
        value8 &= 0x27;
        // Enabling bus mastering resumes a ring armed while blocked
        if ((value8 ^ oldval) & 0x04) {
          pci_conf[0x04] = value8;
          if ((value8 & 0x04) && cce_thread_started)
            pm4_kick();
        }
        break;
      case 0x05:
        value8 = oldval;
        break;
      case 0x06:
      case 0x07:
        value8 = oldval;
        break;
      case 0x0c:
      case 0x0d:
        break;
      case 0x58: case 0x59: case 0x5a: case 0x5b:
        if (is_agp)
          value8 &= (RAGE128_AGP_COMMAND_MASK >> (((address + i) - 0x58) * 8)) & 0xff;
        else
          value8 = oldval;
        break;
      case 0x60:
        value8 &= 0x03;
        if (value8 == 0x02) value8 = oldval;
        break;
      default:
        value8 = oldval;
    }
    pci_conf[address + i] = value8;
  }
}

// ---------------------------------------------------------------------
// Interrupts (GEN_INT_CNTL / GEN_INT_STATUS): INTA is level-triggered
// while any enabled source has its latched status bit set.
// ---------------------------------------------------------------------

void bx_rage128_c::set_irq_level(bool level)
{
  DEV_pci_set_irq(devfunc, pci_conf[0x3d], level);
}

void bx_rage128_c::gen_int_update(void)
{
  // INTA is driven from the CPU thread only: a change made by a ring packet
  // on the CCE thread is folded in by the next display update / vertical timer
  if (on_cce_thread()) {
    irq_dirty = true;
    return;
  }
  set_irq_level((gen_int_cntl & gen_int_status & RAGE128_GIC_SUPPORTED) != 0);
}

// Re-evaluate deferred CPU-thread side effects of CCE-thread register writes
void bx_rage128_c::fold_deferred(void)
{
  if (irq_dirty) {
    irq_dirty = false;
    set_irq_level((gen_int_cntl & gen_int_status & RAGE128_GIC_SUPPORTED) != 0);
  }
  if (timing_dirty) {
    timing_dirty = false;
    timing_recalc();
  }
  if (palette_dirty) {
    palette_dirty = false;
    for (unsigned i = 0; i < 256; i++) {
      bx_gui->palette_change_common(i, BX_RAGE128_THIS s.pel.data[i].red << BX_RAGE128_THIS s.dac_shift,
                                    BX_RAGE128_THIS s.pel.data[i].green << BX_RAGE128_THIS s.dac_shift,
                                    BX_RAGE128_THIS s.pel.data[i].blue << BX_RAGE128_THIS s.dac_shift);
    }
    needs_update_dispentire = true;
  }
}

void bx_rage128_c::timing_update(void)
{
  if (on_cce_thread())
    timing_dirty = true;
  else
    timing_recalc();
}

// Hand the CRTC timing to the VGA core only when it actually changed: the
// core's calculate_retrace_timing() restarts its vertical timer, so doing
// it on every CRTC_GEN_CNTL/PLL write (cursor enable toggles, mode-set
// housekeeping) would starve the vertical blank events (cursor latch,
// VBLANK/VSYNC interrupts, page flips) and stall the frame counter.
void bx_rage128_c::timing_recalc(void)
{
  bx_crtc_params_t p;
  Bit32u vclock = 0;

  get_crtc_params(&p, &vclock);
  if (timing_valid && (vclock == timing_vclock) &&
      (p.htotal == timing_last.htotal) && (p.vtotal == timing_last.vtotal) &&
      (p.vbstart == timing_last.vbstart) && (p.vrstart == timing_last.vrstart))
    return;
  timing_valid = true;
  timing_last = p;
  timing_vclock = vclock;
  bx_vgacore_c::calculate_retrace_timing();
}

// Fold the engine's busy->idle event into GEN_INT_STATUS[19] (CPU thread only)
void bx_rage128_c::gen_int_fold_gui_idle(void)
{
  if (gui_idle_event) {
    gui_idle_event = 0;
    gen_int_status |= RAGE128_GIS_GUI_IDLE;
    gen_int_update();
  }
}

// ---------------------------------------------------------------------
// Timers
// ---------------------------------------------------------------------

void bx_rage128_c::vline_timer_handler(void *this_ptr)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  class_ptr->gen_int_status |= RAGE128_GIS_VLINE;
  class_ptr->gen_int_update();
}

void bx_rage128_c::pump_timer_handler(void *this_ptr)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  class_ptr->pm4_pump();
}

// Microseconds into the current frame (0 = display start)
Bit32u bx_rage128_c::get_display_usec(void)
{
  Bit64u display_usec = bx_virt_timer.time_usec(BX_RAGE128_THIS vsync_realtime) - BX_RAGE128_THIS s.display_start_usec;
  if (BX_RAGE128_THIS s.vtotal_usec == 0) return 0;
  return (Bit32u)(display_usec % BX_RAGE128_THIS s.vtotal_usec);
}

Bit32u bx_rage128_c::get_current_line(void)
{
  if (BX_RAGE128_THIS s.htotal_usec == 0) return 0;
  return get_display_usec() / BX_RAGE128_THIS s.htotal_usec;
}

bool bx_rage128_c::in_vblank(void)
{
  return get_display_usec() >= BX_RAGE128_THIS s.vblank_usec;
}

// The VGA core's vertical timer fires twice per frame: once at the end of
// the vertical retrace (toggle = 1) and once at display start (toggle = 0).
void bx_rage128_c::vertical_timer(void)
{
  bx_vgacore_c::vertical_timer();
  fold_deferred();
  if (BX_RAGE128_THIS vtimer_toggle) {
    // vertical blank / sync started
    vblank_save = true;
    frame_count = (frame_count + 1) & 0x1fffff;
    gen_int_fold_gui_idle();
    gen_int_status |= RAGE128_GIS_VBLANK | RAGE128_GIS_VSYNC;
    gen_int_update();

    // Deferred PPLL atomic update
    if (ppll_update_pending) {
      ppll_commit();
      ppll_update_pending = false;
      timing_recalc();
    }
    // Page flip: a pending CRTC_OFFSET takes effect at vertical blank
    if (crtc_offset_pending && !crtc_offset_lock) {
      latch_crtc_offset();
      pm4_flip_notify();
    }
    // OV0 / subpicture register latch happens once per frame
    ov0_frame_latch();
    // Hardware cursor tuple is picked up whole at vertical blank
    cursor_frame_latch();
  } else {
    // Display start: schedule the CRTC_VLINE compare-line event
    Bit32u cmp = crtc_vline & 0x7ff;
    Bit32u vtotal = disp_ext ? ((crtc_v_total_disp & 0x7ff) + 1) : 0;
    if (disp_ext && (cmp < vtotal) && (BX_RAGE128_THIS s.htotal_usec > 0)) {
      Bit32u usec = cmp * BX_RAGE128_THIS s.htotal_usec;
      if (usec == 0) usec = 1;
      bx_virt_timer.activate_timer(vline_timer_id, usec, 0);
    }
  }
}

// ---------------------------------------------------------------------
// Save / restore
// ---------------------------------------------------------------------

void bx_rage128_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "rage128", "Rage 128 PRO State");
  BX_RAGE128_THIS vgacore_register_state(list);
  new bx_shadow_data_c(list, "vram", BX_RAGE128_THIS s.memory, vram_size);
  new bx_shadow_num_c(list, "mm_index", &mm_index, BASE_HEX);
  new bx_shadow_data_c(list, "bios_scratch", (Bit8u*)bios_scratch, sizeof(bios_scratch));
  new bx_shadow_num_c(list, "bus_cntl", &bus_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "bus_cntl1", &bus_cntl1, BASE_HEX);
  new bx_shadow_num_c(list, "config_cntl", &config_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "gen_reset_cntl", &gen_reset_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "config_memsize", &config_memsize, BASE_HEX);
  new bx_shadow_num_c(list, "host_path_cntl", &host_path_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "mem_cntl", &mem_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "ext_mem_cntl", &ext_mem_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "mem_addr_config", &mem_addr_config, BASE_HEX);
  new bx_shadow_num_c(list, "agp_base", &agp_base, BASE_HEX);
  new bx_shadow_num_c(list, "agp_cntl", &agp_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "gpio_monid", &gpio_monid, BASE_HEX);
  new bx_shadow_num_c(list, "gen_int_cntl", &gen_int_cntl, BASE_HEX);
  new bx_shadow_num_c(list, "gen_int_status", &gen_int_status, BASE_HEX);
  new bx_shadow_data_c(list, "surf_lower", (Bit8u*)surf_lower, sizeof(surf_lower));
  new bx_shadow_data_c(list, "surf_upper", (Bit8u*)surf_upper, sizeof(surf_upper));
  new bx_shadow_data_c(list, "surf_info", (Bit8u*)surf_info, sizeof(surf_info));

  bx_list_c *disp = new bx_list_c(list, "display");
  new bx_shadow_data_c(disp, "pll_regs", (Bit8u*)pll_regs, sizeof(pll_regs));
  new bx_shadow_data_c(disp, "ppll_work", (Bit8u*)ppll_work, sizeof(ppll_work));
  new bx_shadow_num_c(disp, "clock_cntl_index", &clock_cntl_index, BASE_HEX);
  BXRS_PARAM_BOOL(disp, ppll_update_pending, ppll_update_pending);
  new bx_shadow_num_c(disp, "crtc_gen_cntl", &crtc_gen_cntl, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_ext_cntl", &crtc_ext_cntl, BASE_HEX);
  new bx_shadow_num_c(disp, "dac_cntl", &dac_cntl, BASE_HEX);
  new bx_shadow_num_c(disp, "dac_mask_prog", &dac_mask_prog, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_h_total_disp", &crtc_h_total_disp, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_h_sync_strt_wid", &crtc_h_sync_strt_wid, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_v_total_disp", &crtc_v_total_disp, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_v_disp_active", &crtc_v_disp_active, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_v_sync_strt_wid", &crtc_v_sync_strt_wid, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_vline", &crtc_vline, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_gui_trig_vline", &crtc_gui_trig_vline, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_offset", &crtc_offset, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_offset_latched", &crtc_offset_latched, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_offset_cntl", &crtc_offset_cntl, BASE_HEX);
  new bx_shadow_num_c(disp, "crtc_pitch", &crtc_pitch, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_offset", &cur_offset, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_horz_vert_posn", &cur_horz_vert_posn, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_horz_vert_off", &cur_horz_vert_off, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_offset_act", &cur_offset_act, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_posn_act", &cur_posn_act, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_hvoff_act", &cur_hvoff_act, BASE_HEX);
  BXRS_PARAM_BOOL(disp, cur_lock, cur_lock);
  new bx_shadow_num_c(disp, "cur_clr0", &cur_clr0, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_clr1", &cur_clr1, BASE_HEX);
  BXRS_PARAM_BOOL(disp, cur_lat_en, cur_lat_en);
  new bx_shadow_num_c(disp, "cur_lat_offset", &cur_lat_offset, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_lat_posn", &cur_lat_posn, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_lat_hvoff", &cur_lat_hvoff, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_lat_clr0", &cur_lat_clr0, BASE_HEX);
  new bx_shadow_num_c(disp, "cur_lat_clr1", &cur_lat_clr1, BASE_HEX);
  new bx_shadow_num_c(disp, "ovr_clr", &ovr_clr, BASE_HEX);
  new bx_shadow_num_c(disp, "dac_ext_cntl", &dac_ext_cntl, BASE_HEX);
  new bx_shadow_num_c(disp, "mem_vga_wp_sel", &mem_vga_wp_sel, BASE_HEX);
  new bx_shadow_num_c(disp, "mem_vga_rp_sel", &mem_vga_rp_sel, BASE_HEX);
  new bx_shadow_num_c(disp, "palette_index", &palette_index, BASE_HEX);
  new bx_shadow_num_c(disp, "frame_count", &frame_count);
  BXRS_PARAM_BOOL(disp, crtc_offset_pending, crtc_offset_pending);
  BXRS_PARAM_BOOL(disp, crtc_offset_lock, crtc_offset_lock);
  new bx_shadow_data_c(disp, "ext_crtc", ext_crtc, sizeof(ext_crtc));

  bx_list_c *eng = new bx_list_c(list, "gui");
  new bx_shadow_num_c(eng, "dp_gui_master_cntl", &dp_gui_master_cntl, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_brush_frgd_clr", &dp_brush_frgd_clr, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_brush_bkgd_clr", &dp_brush_bkgd_clr, BASE_HEX);
  new bx_shadow_data_c(eng, "brush_data", (Bit8u*)brush_data, sizeof(brush_data));
  new bx_shadow_num_c(eng, "dp_src_frgd_clr", &dp_src_frgd_clr, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_src_bkgd_clr", &dp_src_bkgd_clr, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_cntl", &dp_cntl, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_datatype", &dp_datatype, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_mix", &dp_mix, BASE_HEX);
  new bx_shadow_num_c(eng, "dp_write_mask", &dp_write_mask, BASE_HEX);
  new bx_shadow_num_c(eng, "clr_cmp_clr_src", &clr_cmp_clr_src, BASE_HEX);
  new bx_shadow_num_c(eng, "clr_cmp_clr_dst", &clr_cmp_clr_dst, BASE_HEX);
  new bx_shadow_num_c(eng, "clr_cmp_cntl", &clr_cmp_cntl, BASE_HEX);
  new bx_shadow_num_c(eng, "clr_cmp_mask", &clr_cmp_mask, BASE_HEX);
  new bx_shadow_num_c(eng, "aux_sc_cntl", &aux_sc_cntl, BASE_HEX);
  new bx_shadow_data_c(eng, "aux_sc_rect", (Bit8u*)aux_sc_rect, sizeof(aux_sc_rect));
  new bx_shadow_num_c(eng, "default_offset", &default_offset, BASE_HEX);
  new bx_shadow_num_c(eng, "default_pitch", &default_pitch, BASE_HEX);
  new bx_shadow_num_c(eng, "default_sc_bottom_right", &default_sc_bottom_right, BASE_HEX);
  new bx_shadow_num_c(eng, "sc_top_left", &sc_top_left, BASE_HEX);
  new bx_shadow_num_c(eng, "sc_bottom_right", &sc_bottom_right, BASE_HEX);
  new bx_shadow_num_c(eng, "src_offset", &src_offset, BASE_HEX);
  new bx_shadow_num_c(eng, "src_pitch", &src_pitch, BASE_HEX);
  new bx_shadow_num_c(eng, "dst_offset", &dst_offset, BASE_HEX);
  new bx_shadow_num_c(eng, "dst_pitch", &dst_pitch, BASE_HEX);
  new bx_shadow_num_c(eng, "src_pitch_reg", &src_pitch_reg, BASE_HEX);
  new bx_shadow_num_c(eng, "dst_pitch_reg", &dst_pitch_reg, BASE_HEX);
  new bx_shadow_num_c(eng, "gui_dst_x", &gui_dst_x);
  new bx_shadow_num_c(eng, "gui_dst_y", &gui_dst_y);
  new bx_shadow_num_c(eng, "gui_src_x", &gui_src_x);
  new bx_shadow_num_c(eng, "gui_src_y", &gui_src_y);
  new bx_shadow_num_c(eng, "gui_dst_w", &gui_dst_w);
  new bx_shadow_num_c(eng, "gui_dst_h", &gui_dst_h);
  new bx_shadow_data_c(eng, "scl_palette", (Bit8u*)scl_palette, sizeof(scl_palette));

  bx_list_c *pm4 = new bx_list_c(list, "pm4");
  new bx_shadow_num_c(pm4, "buffer_offset", &pm4_buffer_offset, BASE_HEX);
  new bx_shadow_num_c(pm4, "buffer_cntl", &pm4_buffer_cntl, BASE_HEX);
  new bx_shadow_num_c(pm4, "wm_cntl", &pm4_wm_cntl, BASE_HEX);
  new bx_shadow_num_c(pm4, "rptr_addr", &pm4_rptr_addr, BASE_HEX);
  new bx_shadow_num_c(pm4, "rptr", &pm4_rptr, BASE_HEX);
  new bx_shadow_num_c(pm4, "wptr", &pm4_wptr, BASE_HEX);
  new bx_shadow_num_c(pm4, "micro_cntl", &pm4_micro_cntl, BASE_HEX);
  new bx_shadow_num_c(pm4, "iw_indoff", &pm4_iw_indoff, BASE_HEX);
  new bx_shadow_num_c(pm4, "iw_indsize", &pm4_iw_indsize, BASE_HEX);
  new bx_shadow_num_c(pm4, "pci_gart_page", &pci_gart_page, BASE_HEX);

  bx_list_c *t3 = new bx_list_c(list, "t3d");
  new bx_shadow_data_c(t3, "ctx", (Bit8u*)&t3d, sizeof(t3d));

  bx_list_c *ov = new bx_list_c(list, "ov0");
  new bx_shadow_data_c(ov, "shadow", (Bit8u*)ov0.shadow, sizeof(ov0.shadow));
  new bx_shadow_data_c(ov, "active", (Bit8u*)ov0.active, sizeof(ov0.active));
  new bx_shadow_num_c(ov, "reg_load_cntl", &ov0_reg_load_cntl, BASE_HEX);
  new bx_shadow_data_c(ov, "subpic_regs", (Bit8u*)subpic.regs, sizeof(subpic.regs));
  new bx_shadow_data_c(ov, "subpic_pal", (Bit8u*)subpic.pal, sizeof(subpic.pal));
  register_pci_state(list);
}

void bx_rage128_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state();
  BX_RAGE128_THIS bx_vgacore_c::after_restore_state();
  update_banking();
  update_vga_decode();
  surf_xlate_on = false;
  for (int n = 0; n < 4; n++)
    if (surf_info[n] & RAGE128_SURF_INFO_MASK) surf_xlate_on = true;
  ov0_update();
  cursor_publish();
  timing_valid = false;
  timing_recalc();
  gui_xres = gui_yres = gui_bpp = 0;
  needs_update_mode = true;
  needs_update_dispentire = true;
  gen_int_update();
  if (!cce_thread_started) {
    pm4_thread_init();
  }
}

// ---------------------------------------------------------------------
// Legacy VGA I/O passthrough
// ---------------------------------------------------------------------

Bit32u bx_rage128_c::svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  return class_ptr->svga_read(address, io_len);
}

void bx_rage128_c::svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  class_ptr->svga_write(address, value, io_len);
}

Bit32u bx_rage128_c::svga_read(Bit32u address, unsigned io_len)
{
  if (io_len == 2) {
    Bit32u v = svga_read(address, 1);
    v |= svga_read(address + 1, 1) << 8;
    return v;
  }
  if (vga_disabled || (config_cntl & RAGE128_CFG_VGA_IO_DIS))
    return 0xff;

  // The VGA I/O DAC addresses are gated in extended modes
  if ((address >= 0x3c6) && (address <= 0x3c9) && disp_ext && !(dac_cntl & RAGE128_DAC_VGA_ADR_EN))
    return 0xff;

  switch (address) {
    case 0x03c2: // Input Status 0: monitor sense asserted
      return bx_vgacore_c::read(address, 1) | 0x10;
    case 0x03c6:
      return dac_mask_prog;
    case 0x03b5:
    case 0x03d5:
      // CRTC indices above 0x20 read 0xff; 0x19-0x20 are plain storage
      if (BX_RAGE128_THIS s.CRTC.address > 0x18) {
        if (BX_RAGE128_THIS s.CRTC.address <= 0x20)
          return ext_crtc[BX_RAGE128_THIS s.CRTC.address];
        return 0xff;
      }
      break;
    default:
      break;
  }
  return bx_vgacore_c::read(address, io_len);
}

void bx_rage128_c::svga_write(Bit32u address, Bit32u value, unsigned io_len)
{
  if (io_len == 2) {
    svga_write(address, value & 0xff, 1);
    svga_write(address + 1, (value >> 8) & 0xff, 1);
    return;
  }
  if (vga_disabled || (config_cntl & RAGE128_CFG_VGA_IO_DIS))
    return;
  if ((address >= 0x3c6) && (address <= 0x3c9) && disp_ext && !(dac_cntl & RAGE128_DAC_VGA_ADR_EN))
    return;

  switch (address) {
    case 0x03c6:
      dac_mask_prog = (Bit8u)value;
      break;
    case 0x03c9:
      if (disp_ext) needs_update_dispentire = true;
      break;
    case 0x03b4:
    case 0x03d4:
      BX_RAGE128_THIS s.CRTC.address = value & 0x3f;
      if (BX_RAGE128_THIS s.CRTC.address > 0x18)
        return;
      break;
    case 0x03b5:
    case 0x03d5:
      if (BX_RAGE128_THIS s.CRTC.address > 0x18) {
        if (BX_RAGE128_THIS s.CRTC.address <= 0x20)
          ext_crtc[BX_RAGE128_THIS s.CRTC.address] = (Bit8u)value;
        return;
      }
      // CRTC[0..7] write protect
      if ((BX_RAGE128_THIS s.CRTC.address < 7) && (BX_RAGE128_THIS s.CRTC.reg[0x11] & 0x80))
        return;
      break;
    default:
      break;
  }
  bx_vgacore_c::write(address, value, io_len, 0);

  // VGA vertical-display-end writes through to the CRTC_V_TOTAL_DISP readback
  if (((address == 0x3d5) || (address == 0x3b5)) &&
      ((BX_RAGE128_THIS s.CRTC.address == 0x07) || (BX_RAGE128_THIS s.CRTC.address == 0x12))) {
    Bit32u vdisp = BX_RAGE128_THIS s.CRTC.reg[0x12] |
                   ((BX_RAGE128_THIS s.CRTC.reg[7] & 0x02) << 7) |
                   ((BX_RAGE128_THIS s.CRTC.reg[7] & 0x40) << 3);
    crtc_v_total_disp = (crtc_v_total_disp & ~0x07ff0000) | (vdisp << 16);
  }
}

// IO BAR1: register file offsets 0x00-0xff
Bit32u bx_rage128_c::io_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  Bit32u off = (address - class_ptr->pci_bar[1].addr) & 0xff;
  Bit32u v = class_ptr->reg_read32(off & 0xfc) >> ((off & 3) * 8);
  if (io_len == 1) v &= 0xff;
  else if (io_len == 2) v &= 0xffff;
  return v;
}

void bx_rage128_c::io_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)this_ptr;
  Bit32u off = (address - class_ptr->pci_bar[1].addr) & 0xff;
  Bit32u sh = (off & 3) * 8;
  Bit32u mask = (io_len == 1) ? 0xff : (io_len == 2) ? 0xffff : 0xffffffff;
  class_ptr->reg_write(off & 0xfc, value << sh, mask << sh);
}

// ---------------------------------------------------------------------
// Memory handlers: BAR0 (LFB + AGP image), BAR2 (registers), ROM,
// legacy VGA window
// ---------------------------------------------------------------------

bool bx_rage128_c::agp_image_read(Bit32u off, Bit8u *dst, Bit32u len)
{
  if (!pm4_bus_master_ok() || !agp_base)
    return false;
  phys_read_chunked(agp_base + (off & 0x01ffffff), dst, len);
  return true;
}

bool bx_rage128_c::agp_image_write(Bit32u off, const Bit8u *src, Bit32u len)
{
  if (!pm4_bus_master_ok() || !agp_base)
    return false;
  phys_write_chunked(agp_base + (off & 0x01ffffff), src, len);
  return true;
}

// SURFACE0-3 CPU tiling translation windows
static const Bit16u rage128_surf_pitch[32] = {
     0,   64,  128,  256,  512, 1024, 2048, 4096,
   640, 1280, 2560, 5120, 1600, 3200, 6400,  832,
  1664, 3328, 1920, 3840,    0,    0,    0,    0,
     0,    0,    0,    0,    0,    0,    0,    0
};

Bit32u bx_rage128_c::surf_xlate(Bit32u addr)
{
  for (int n = 0; n < 4; n++) {
    Bit32u pitch = rage128_surf_pitch[surf_info[n] & RAGE128_SURF_INFO_MASK];
    if (!pitch || (addr < surf_lower[n]) || (addr >= surf_upper[n]))
      continue;
    Bit32u off = addr - surf_lower[n];
    return (surf_lower[n] + r128_tile_off(off % pitch, off / pitch, pitch)) & vram_mask;
  }
  return addr;
}

// Mark the display tiles covering VRAM bytes [addr, addr+len) as dirty
void bx_rage128_c::vram_dirty(Bit32u addr, Bit32u len)
{
  // A rewrite of the 64x64x2bpp cursor image (1 KB at the latched
  // CUR_OFFSET) changes the cursor shape without any register write
  if (cur_lat_en) {
    Bit32u ca = addr & vram_mask;
    if ((ca < cur_lat_offset + 64 * 16) && (ca + len > cur_lat_offset))
      cur_bitmap_dirty = true;
  }
  if (!disp_ext || (disp_pitch == 0)) {
    BX_RAGE128_THIS s.vga_mem_updated |= 1;
    bx_vgacore_c::vga_redraw_area(0, 0, BX_RAGE128_THIS s.last_xres, BX_RAGE128_THIS s.last_yres);
    return;
  }
  addr &= vram_mask;
  Bit32u end = addr + len;
  Bit32u dend = disp_base + disp_pitch * (disp_dblscan ? (disp_yres + 1) / 2 : disp_yres);
  if ((end <= disp_base) || (addr >= dend))
    return;
  if (addr < disp_base) addr = disp_base;
  if (end > dend) end = dend;
  Bit32u y0 = (addr - disp_base) / disp_pitch;
  Bit32u y1 = (end - 1 - disp_base) / disp_pitch;
  if (disp_dblscan) { y0 <<= 1; y1 = (y1 << 1) + 1; }
  Bit32u yt0 = y0 / Y_TILESIZE;
  Bit32u yt1 = y1 / Y_TILESIZE;
  Bit32u xt1 = (disp_xres - 1) / X_TILESIZE;
  for (Bit32u yti = yt0; yti <= yt1; yti++)
    for (Bit32u xti = 0; xti <= xt1; xti++)
      SET_TILE_UPDATED(BX_RAGE128_THIS, xti, yti, 1);
  needs_update_tile = true;
}

bool bx_rage128_c::lfb_read(bx_phy_address addr, unsigned len, Bit8u *data)
{
  Bit32u off = (Bit32u)(addr - pci_bar[0].addr) & (RAGE128_LFB_SIZE - 1);
  Bit32u endian = config_cntl & 3;
  if (off & R128_CARD_AGP_HALF) {
    endian = (config_cntl >> 2) & 3;
    if ((endian == 0) || (len == 1)) {
      Bit32u a = off;
      if (endian == 1) a ^= 1; else if (endian == 2) a ^= 3;
      if (!agp_image_read(a, data, len))
        memset(data, 0xff, len);
    } else {
      for (unsigned i = 0; i < len; i++) {
        Bit32u a = off + i;
        if (endian == 1) a ^= 1; else a ^= 3;
        if (!agp_image_read(a, &data[i], 1)) data[i] = 0xff;
      }
    }
    return true;
  }
  for (unsigned i = 0; i < len; i++) {
    Bit32u a = (off + i) & vram_mask;
    if (endian == 1) a ^= 1; else if (endian == 2) a ^= 3;
    if (surf_xlate_on) a = surf_xlate(a);
    data[i] = BX_RAGE128_THIS s.memory[a & vram_mask];
  }
  return true;
}

bool bx_rage128_c::lfb_write(bx_phy_address addr, unsigned len, Bit8u *data)
{
  Bit32u off = (Bit32u)(addr - pci_bar[0].addr) & (RAGE128_LFB_SIZE - 1);
  Bit32u endian = config_cntl & 3;
  if (off & R128_CARD_AGP_HALF) {
    endian = (config_cntl >> 2) & 3;
    if ((endian == 0) || (len == 1)) {
      Bit32u a = off;
      if (endian == 1) a ^= 1; else if (endian == 2) a ^= 3;
      agp_image_write(a, data, len);
    } else {
      for (unsigned i = 0; i < len; i++) {
        Bit32u a = off + i;
        if (endian == 1) a ^= 1; else a ^= 3;
        agp_image_write(a, &data[i], 1);
      }
    }
    return true;
  }
  Bit32u lo = 0xffffffff, hi = 0;
  for (unsigned i = 0; i < len; i++) {
    Bit32u a = (off + i) & vram_mask;
    if (endian == 1) a ^= 1; else if (endian == 2) a ^= 3;
    if (surf_xlate_on) a = surf_xlate(a);
    a &= vram_mask;
    BX_RAGE128_THIS s.memory[a] = data[i];
    if (a < lo) lo = a;
    if (a > hi) hi = a;
  }
  if (hi >= lo) vram_dirty(lo, hi - lo + 1);
  return true;
}

void bx_rage128_c::mmio_read(Bit32u off, unsigned len, Bit8u *data)
{
  bool swap = (config_cntl & (1 << 4)) != 0;
  Bit32u a = off & RAGE128_REG_APER_MASK;
  if (len == 4) {
    Bit32u v = reg_read32(a & 0x1ffc);
    if (swap) v = bx_bswap32(v);
    WriteHostDWordToLittleEndian((Bit32u*)data, v);
  } else if (len == 2) {
    if (swap) a ^= 2;
    Bit16u v = (Bit16u)(reg_read32(a & 0x1ffc) >> ((a & 2) * 8));
    if (swap) v = bx_bswap16(v);
    WriteHostWordToLittleEndian((Bit16u*)data, v);
  } else if (len == 1) {
    if (swap) a ^= 3;
    data[0] = (Bit8u)(reg_read32(a & 0x1ffc) >> ((a & 3) * 8));
  } else {
    for (unsigned i = 0; i < len; i += 4) {
      Bit32u v = reg_read32((a + i) & 0x1ffc);
      if (swap) v = bx_bswap32(v);
      WriteHostDWordToLittleEndian((Bit32u*)(data + i), v);
    }
  }
}

void bx_rage128_c::mmio_write(Bit32u off, unsigned len, Bit8u *data)
{
  bool swap = (config_cntl & (1 << 4)) != 0;
  Bit32u a = off & RAGE128_REG_APER_MASK;
  if (len == 4) {
    Bit32u v = ReadHostDWordFromLittleEndian((Bit32u*)data);
    if (swap) v = bx_bswap32(v);
    reg_write(a & 0x1ffc, v, 0xffffffff);
  } else if (len == 2) {
    Bit16u v = ReadHostWordFromLittleEndian((Bit16u*)data);
    if (swap) { a ^= 2; v = bx_bswap16(v); }
    Bit32u sh = (a & 2) * 8;
    reg_write(a & 0x1ffc, (Bit32u)v << sh, 0xffff << sh);
  } else if (len == 1) {
    if (swap) a ^= 3;
    Bit32u sh = (a & 3) * 8;
    reg_write(a & 0x1ffc, (Bit32u)data[0] << sh, 0xff << sh);
  } else {
    for (unsigned i = 0; i < len; i += 4) {
      Bit32u v = ReadHostDWordFromLittleEndian((Bit32u*)(data + i));
      if (swap) v = bx_bswap32(v);
      reg_write((a + i) & 0x1ffc, v, 0xffffffff);
    }
  }
}

bool bx_rage128_c::mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)param;
  Bit8u *data_ptr = (Bit8u *)data;

  // Expansion ROM
  if (class_ptr->pci_bar[PCI_ROM_BAR].size > 0) {
    Bit32u mask = class_ptr->pci_bar[PCI_ROM_BAR].size - 1;
    if (((Bit32u)addr & ~mask) == class_ptr->pci_bar[PCI_ROM_BAR].addr) {
      for (unsigned i = 0; i < len; i++) {
        if (class_ptr->pci_conf[0x30] & 0x01)
          data_ptr[i] = class_ptr->pci_rom[(addr + i) & mask];
        else
          data_ptr[i] = 0xff;
      }
      return true;
    }
  }
  // BAR2: register aperture
  if ((class_ptr->pci_bar[2].addr != 0) && (addr >= class_ptr->pci_bar[2].addr) &&
      (addr < (class_ptr->pci_bar[2].addr + RAGE128_MMIO_SIZE))) {
    class_ptr->mmio_read((Bit32u)(addr - class_ptr->pci_bar[2].addr), len, data_ptr);
    return true;
  }
  // BAR0: framebuffer aperture
  if ((class_ptr->pci_bar[0].addr != 0) && (addr >= class_ptr->pci_bar[0].addr) &&
      (addr < (class_ptr->pci_bar[0].addr + RAGE128_LFB_SIZE))) {
    return class_ptr->lfb_read(addr, len, data_ptr);
  }
  // Legacy VGA window
  for (unsigned i = 0; i < len; i++) {
    data_ptr[i] = class_ptr->mem_read(addr + i);
  }
  return true;
}

bool bx_rage128_c::mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_rage128_c *class_ptr = (bx_rage128_c *)param;
  Bit8u *data_ptr = (Bit8u *)data;

  if ((class_ptr->pci_bar[2].addr != 0) && (addr >= class_ptr->pci_bar[2].addr) &&
      (addr < (class_ptr->pci_bar[2].addr + RAGE128_MMIO_SIZE))) {
    class_ptr->mmio_write((Bit32u)(addr - class_ptr->pci_bar[2].addr), len, data_ptr);
    return true;
  }
  if ((class_ptr->pci_bar[0].addr != 0) && (addr >= class_ptr->pci_bar[0].addr) &&
      (addr < (class_ptr->pci_bar[0].addr + RAGE128_LFB_SIZE))) {
    return class_ptr->lfb_write(addr, len, data_ptr);
  }
  for (unsigned i = 0; i < len; i++) {
    class_ptr->mem_write(addr + i, data_ptr[i]);
  }
  return true;
}

// Legacy VGA aperture. The A0000 window is banked through MEM_VGA_WP/RP_SEL
// whenever the extended display path is enabled (or VGA_MEM_PS_EN is set);
// otherwise the VGA core owns it.
Bit8u bx_rage128_c::mem_read(bx_phy_address addr)
{
  if (vga_disabled)
    return 0xff;
  bool paged = disp_ext || (crtc_ext_cntl & RAGE128_VGA_MEM_PS_EN);
  if (paged && (addr >= 0xa0000) && (addr <= 0xaffff)) {
    Bit32u off = ((Bit32u)addr & 0x7fff) + bank_r[((Bit32u)addr >> 15) & 1];
    return BX_RAGE128_THIS s.memory[off & vram_mask];
  }
  return bx_vgacore_c::mem_read(addr);
}

void bx_rage128_c::mem_write(bx_phy_address addr, Bit8u value)
{
  if (vga_disabled)
    return;
  bool paged = disp_ext || (crtc_ext_cntl & RAGE128_VGA_MEM_PS_EN);
  if (paged && (addr >= 0xa0000) && (addr <= 0xaffff)) {
    Bit32u off = (((Bit32u)addr & 0x7fff) + bank_w[((Bit32u)addr >> 15) & 1]) & vram_mask;
    BX_RAGE128_THIS s.memory[off] = value;
    vram_dirty(off, 1);
    return;
  }
  bx_vgacore_c::mem_write(addr, value);
}

void bx_rage128_c::get_text_snapshot(Bit8u **text_snapshot, unsigned *txHeight, unsigned *txWidth)
{
  BX_RAGE128_THIS bx_vgacore_c::get_text_snapshot(text_snapshot, txHeight, txWidth);
}

// Legacy VGA I/O decode: enabled only with CFG_VGA_IO_DIS clear
void bx_rage128_c::update_vga_decode(void)
{
  // The I/O handlers stay registered; svga_read/svga_write gate on the bit.
}

// VGA aperture banking (MEM_VGA_WP/RP_SEL page the A0000 window)
void bx_rage128_c::update_banking(void)
{
  bool paged = disp_ext || (crtc_ext_cntl & RAGE128_VGA_MEM_PS_EN);
  Bit32u unit = (crtc_ext_cntl & RAGE128_CRTC_VGA_ATI_LINEAR) ? (RAGE128_VGA_PAGE_SIZE * 4) : RAGE128_VGA_PAGE_SIZE;
  if (paged) {
    bank_w[0] = (mem_vga_wp_sel & 0x3ff) * unit;
    bank_w[1] = ((mem_vga_wp_sel >> 16) & 0x3ff) * unit;
    bank_r[0] = (mem_vga_rp_sel & 0x3ff) * unit;
    bank_r[1] = ((mem_vga_rp_sel >> 16) & 0x3ff) * unit;
  } else {
    bank_w[0] = bank_r[0] = 0x0000;
    bank_w[1] = bank_r[1] = 0x8000;
  }
}

// ---------------------------------------------------------------------
// Central register file
// ---------------------------------------------------------------------

Bit32u bx_rage128_c::reg_read32(Bit32u off)
{
  Bit32u v;

  off &= 0x3ffc;
  switch (off) {
    case RAGE128_MM_INDEX:
      return mm_index;
    case RAGE128_MM_DATA:
      if (mm_index & RAGE128_MM_INDEX_MM_APER) {
        Bit32u a = mm_index & RAGE128_MM_INDEX_MM_ADDR;
        if (r128_card_is_agp(a)) {
          Bit32u val = 0xffffffff;
          agp_image_read(a, (Bit8u*)&val, 4);
          return val;
        }
        a &= vram_mask;
        return ReadHostDWordFromLittleEndian((Bit32u*)&BX_RAGE128_THIS s.memory[a]);
      }
      if ((mm_index & 0x3ffc) == RAGE128_MM_DATA)
        return 0;
      return reg_read32(mm_index);
    case RAGE128_BIOS_0_SCRATCH:
    case RAGE128_BIOS_1_SCRATCH:
    case RAGE128_BIOS_2_SCRATCH:
    case RAGE128_BIOS_3_SCRATCH:
      return bios_scratch[(off - RAGE128_BIOS_0_SCRATCH) >> 2];
    case RAGE128_GEN_INT_CNTL:
      return gen_int_cntl;
    case RAGE128_GEN_INT_STATUS:
      gen_int_fold_gui_idle();
      return gen_int_status;
    case RAGE128_BUS_CNTL:
      return bus_cntl;
    case RAGE128_BUS_CNTL1:
      return bus_cntl1;
    case RAGE128_GPIO_MONID: {
      // latched pad state with the two DDC input lines overlaid live
      v = gpio_monid & ~(RAGE128_GPIO_MONID_Y_DDC_CLK | RAGE128_GPIO_MONID_Y_DDC_DAT);
      // bx_ddc_c::read(): bit 2 = clock line, bit 3 = data line (host AND monitor)
      Bit8u ddc_state = ddc.read();
      if (ddc_state & 0x04) v |= RAGE128_GPIO_MONID_Y_DDC_CLK;
      if (ddc_state & 0x08) v |= RAGE128_GPIO_MONID_Y_DDC_DAT;
      return v;
    }
    case RAGE128_CONFIG_CNTL:
      return (config_cntl & ~0x000f0000) |
             ((pci_device_id == RAGE128_PCI_DEVICE_TR) ? RAGE128_CFG_ATI_REV_TR : RAGE128_CFG_ATI_REV_PF);
    case RAGE128_CONFIG_APER_0_BASE:
      return pci_bar[0].addr & RAGE128_APER_0_BASE_MASK;
    case RAGE128_CONFIG_APER_1_BASE:
      return (pci_bar[0].addr & RAGE128_APER_1_BASE_MASK) | R128_CARD_AGP_HALF;
    case RAGE128_CONFIG_APER_SIZE:
      return RAGE128_CONFIG_APER_SIZE_VAL;
    case RAGE128_CONFIG_REG_1_BASE:
      return (pci_bar[2].addr & RAGE128_REG_1_BASE_MASK) | (RAGE128_REG_APER_MASK + 1);
    case RAGE128_CONFIG_REG_APER_SIZE:
      return 0x00002000;
    case RAGE128_CONFIG_MEMSIZE_EMB:
      return 0;
    case RAGE128_BM_CHUNK_0_VAL:
      return bm_chunk_val[0];
    case RAGE128_BM_CHUNK_1_VAL:
      return bm_chunk_val[1];
    case RAGE128_BM_QUEUE_FREE_STATUS:
      return RAGE128_BM_QUEUE_FREE_IDLE;
    case RAGE128_CONFIG_XSTRAP:
      return vga_disabled ? 0x00000001 : 0x00000000;
    case RAGE128_CONFIG_BONDS:
      return 0;
    case RAGE128_GEN_RESET_CNTL:
      return gen_reset_cntl;
    case RAGE128_GEN_STATUS:
      return 0;
    case RAGE128_CONFIG_MEMSIZE:
      return config_memsize;
    case RAGE128_TEST_DEBUG_CNTL:
      return test_debug_cntl;
    case RAGE128_TEST_DEBUG_MUX:
      return test_debug_mux;
    case RAGE128_HW_DEBUG:
      return hw_debug;
    case RAGE128_HOST_PATH_CNTL:
      return host_path_cntl;
    case RAGE128_MEM_CNTL:
      return mem_cntl & ~RAGE128_MEM_CNTL_RO_MASK;
    case RAGE128_EXT_MEM_CNTL:
      return ext_mem_cntl;
    case RAGE128_MEM_ADDR_CONFIG:
      return mem_addr_config;
    case RAGE128_MEM_INTF_CNTL:
      return mem_intf_cntl;
    case RAGE128_MEM_STR_CNTL:
      return mem_str_cntl;
    case RAGE128_MEM_INIT_LAT_TIMER:
      return mem_init_lat_timer;
    case RAGE128_MEM_SDRAM_MODE_REG:
      return mem_sdram_mode_reg;
    case RAGE128_PAD_CTLR_STRENGTH:
      return pad_ctlr_strength;
    case RAGE128_PC_MISC_CTL:
      return pc_misc_ctl;
    case RAGE128_VIDEOMUX_CNTL:
      return videomux_cntl;
    case RAGE128_SURFACE_DELAY:
      return surface_delay;
    case RAGE128_AGP_BASE:
      return agp_base;
    case RAGE128_AGP_CNTL:
      return agp_cntl;
    case RAGE128_AGP_APER_OFFSET:
      return R128_CARD_AGP_HALF;
    case RAGE128_AGP_CNTL_B:
      return agp_cntl_b;
    case RAGE128_GUI_DEBUG0:
      return gui_debug0;
    case RAGE128_GUI_STAT:
      return pm4_active() ? RAGE128_GUI_STAT_BUSY : RAGE128_GUI_STAT_IDLE;
    case RAGE128_PC_GUI_MODE:
      return pc_gui_mode;
    default:
      break;
  }

  if ((off >= RAGE128_SURFACE0_LOWER_BOUND) && (off <= RAGE128_SURFACE3_INFO)) {
    Bit32u n = (off - 0x0b00) >> 4;
    switch (off & 0x0c) {
      case 0x04: return surf_lower[n];
      case 0x08: return surf_upper[n];
      case 0x0c: return surf_info[n];
      default: break;
    }
  }

  if (display_reg_read(off, &v)) return v;
  if (ov0_reg_read(off, &v)) return v;
  if (subpic_reg_read(off, &v)) return v;
  if (r2d_reg_read(off, &v)) return v;
  if (pm4_reg_read(off, &v)) return v;
  if (r3d_reg_read(off, &v)) return v;

  if ((off >= RAGE128_CONFIG_MIRROR_BASE) && (off <= RAGE128_CONFIG_MIRROR_END)) {
    Bit32u base = off - RAGE128_CONFIG_MIRROR_BASE;
    return pci_read_handler((Bit8u)base, 4);
  }
  return 0;
}

void bx_rage128_c::reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
  off &= 0x3ffc;
  val &= mask;

#define MERGE(field) ((field) = ((field) & ~mask) | val)
  switch (off) {
    case RAGE128_AGP_BASE:
      agp_base = ((agp_base & ~mask) | val) & RAGE128_AGP_BASE_MASK;
      return;
    case RAGE128_AGP_CNTL:
      agp_cntl = ((agp_cntl & ~mask) | val) & RAGE128_AGP_CNTL_MASK;
      return;
    case RAGE128_AGP_APER_OFFSET:
      return;
    case RAGE128_MM_INDEX:
      MERGE(mm_index);
      return;
    case RAGE128_MM_DATA:
      if (mm_index & RAGE128_MM_INDEX_MM_APER) {
        Bit32u a = mm_index & RAGE128_MM_INDEX_MM_ADDR;
        if (r128_card_is_agp(a)) {
          for (Bit32u i = 0; i < 4; i++) {
            if (mask & (0xff << (i * 8))) {
              Bit8u b = (Bit8u)(val >> (i * 8));
              agp_image_write(a + i, &b, 1);
            }
          }
          return;
        }
        a &= vram_mask;
        Bit32u *p = (Bit32u*)&BX_RAGE128_THIS s.memory[a];
        Bit32u cur = ReadHostDWordFromLittleEndian(p);
        WriteHostDWordToLittleEndian(p, (cur & ~mask) | val);
        vram_dirty(a, 4);
        return;
      }
      if ((mm_index & 0x3ffc) == RAGE128_MM_DATA)
        return;
      reg_write(mm_index, val, mask);
      return;
    case RAGE128_BIOS_0_SCRATCH:
    case RAGE128_BIOS_1_SCRATCH:
    case RAGE128_BIOS_2_SCRATCH:
    case RAGE128_BIOS_3_SCRATCH:
      MERGE(bios_scratch[(off - RAGE128_BIOS_0_SCRATCH) >> 2]);
      return;
    case RAGE128_BUS_CNTL:
      MERGE(bus_cntl);
      bus_cntl &= ~RAGE128_BUS_CNTL_WO_MASK;
      if (cce_thread_started && !on_cce_thread())
        pm4_kick();
      return;
    case RAGE128_BUS_CNTL1:
      MERGE(bus_cntl1);
      return;
    case RAGE128_GEN_INT_CNTL:
      MERGE(gen_int_cntl);
      gen_int_update();
      return;
    case RAGE128_GEN_INT_STATUS:
      gen_int_status &= ~(val & RAGE128_GIS_ACK_MASK);
      gen_int_update();
      return;
    case RAGE128_GPIO_MONID:
      MERGE(gpio_monid);
      // open-drain pads: a pad pulls its line low only when output-enabled with A low
      ddc.write(!((gpio_monid & RAGE128_GPIO_MONID_EN_DDC_CLK) && !(gpio_monid & RAGE128_GPIO_MONID_A_DDC_CLK)),
                !((gpio_monid & RAGE128_GPIO_MONID_EN_DDC_DAT) && !(gpio_monid & RAGE128_GPIO_MONID_A_DDC_DAT)));
      return;
    case RAGE128_CONFIG_CNTL:
      MERGE(config_cntl);
      config_cntl &= ~0x000f0000;
      update_vga_decode();
      return;
    case RAGE128_CONFIG_XSTRAP:
    case RAGE128_CONFIG_BONDS:
    case RAGE128_GEN_STATUS:
      return;
    case RAGE128_GEN_RESET_CNTL: {
      Bit32u old = gen_reset_cntl;
      MERGE(gen_reset_cntl);
      if (!(old & RAGE128_GEN_SOFT_RESET_GUI) && (gen_reset_cntl & RAGE128_GEN_SOFT_RESET_GUI)) {
        pm4_gui_reset();
        raster_abandon();
        hostdata_active = false;
        hostdata_ndw = 0;
        pm4_vc_debug_config = 0;
      }
      return;
    }
    case RAGE128_CONFIG_MEMSIZE:
      MERGE(config_memsize);
      config_memsize &= RAGE128_CONFIG_MEMSIZE_MASK;
      return;
    case RAGE128_TEST_DEBUG_CNTL:
      MERGE(test_debug_cntl);
      return;
    case RAGE128_TEST_DEBUG_MUX:
      MERGE(test_debug_mux);
      test_debug_mux &= 0x00009f3f;
      return;
    case RAGE128_HW_DEBUG:
      MERGE(hw_debug);
      return;
    case RAGE128_HOST_PATH_CNTL:
      MERGE(host_path_cntl);
      return;
    case RAGE128_MEM_CNTL:
      MERGE(mem_cntl);
      return;
    case RAGE128_EXT_MEM_CNTL:
      MERGE(ext_mem_cntl);
      return;
    case RAGE128_MEM_ADDR_CONFIG:
      MERGE(mem_addr_config);
      return;
    case RAGE128_MEM_INTF_CNTL:
      MERGE(mem_intf_cntl);
      return;
    case RAGE128_MEM_STR_CNTL:
      MERGE(mem_str_cntl);
      return;
    case RAGE128_MEM_INIT_LAT_TIMER:
      MERGE(mem_init_lat_timer);
      return;
    case RAGE128_MEM_SDRAM_MODE_REG:
      MERGE(mem_sdram_mode_reg);
      return;
    case RAGE128_PAD_CTLR_STRENGTH:
      MERGE(pad_ctlr_strength);
      pad_ctlr_strength &= ~0x000600ff;
      return;
    case RAGE128_PC_MISC_CTL:
      MERGE(pc_misc_ctl);
      return;
    case RAGE128_VIDEOMUX_CNTL:
      MERGE(videomux_cntl);
      return;
    case RAGE128_SURFACE_DELAY:
      MERGE(surface_delay);
      return;
    case RAGE128_AGP_CNTL_B:
      MERGE(agp_cntl_b);
      return;
    case RAGE128_GUI_DEBUG0:
      MERGE(gui_debug0);
      return;
    case RAGE128_GUI_STAT:
      return;
    case RAGE128_PC_GUI_MODE:
      MERGE(pc_gui_mode);
      return;
    case RAGE128_BM_CHUNK_0_VAL:
      MERGE(bm_chunk_val[0]);
      return;
    case RAGE128_BM_CHUNK_1_VAL:
      MERGE(bm_chunk_val[1]);
      return;
    case RAGE128_CONFIG_APER_0_BASE:
    case RAGE128_CONFIG_APER_1_BASE:
    case RAGE128_CONFIG_APER_SIZE:
    case RAGE128_CONFIG_REG_1_BASE:
    case RAGE128_CONFIG_REG_APER_SIZE:
    case RAGE128_CONFIG_MEMSIZE_EMB:
    case RAGE128_BM_QUEUE_FREE_STATUS:
      return;
    default:
      break;
  }
#undef MERGE

  if ((off >= RAGE128_SURFACE0_LOWER_BOUND) && (off <= RAGE128_SURFACE3_INFO) && ((off & 0x0c) != 0)) {
    Bit32u n = (off - 0x0b00) >> 4;
    Bit32u *r;
    Bit32u m;
    switch (off & 0x0c) {
      case 0x04: r = &surf_lower[n]; m = RAGE128_SURF_BOUND_MASK; break;
      case 0x08: r = &surf_upper[n]; m = RAGE128_SURF_BOUND_MASK; break;
      default:   r = &surf_info[n];  m = RAGE128_SURF_INFO_MASK;  break;
    }
    *r = ((*r & ~mask) | val) & m;
    surf_xlate_on = false;
    for (n = 0; n < 4; n++)
      if (rage128_surf_pitch[surf_info[n] & RAGE128_SURF_INFO_MASK])
        surf_xlate_on = true;
    return;
  }

  // Direct CPU writes into the GUI engine block apply synchronously: drain
  // pending ring/CCE work first. WAIT_UNTIL queues behind the engine.
  if ((off >= 0x1400) && (off < 0x2000) && !on_cce_thread() && cce_thread_started) {
    if ((off == RAGE128_WAIT_UNTIL) && (mask == 0xffffffff) && pm4_enqueue_write(off, val))
      return;
    pm4_drain_wait();
  }

  if (display_reg_write(off, val, mask)) return;
  if (ov0_reg_write(off, val, mask)) return;
  if (subpic_reg_write(off, val, mask)) return;
  if (r2d_reg_write(off, val, mask)) return;
  if (pm4_reg_write(off, val, mask)) return;
  if (r3d_reg_write(off, val, mask)) return;
}

// =====================================================================
// Display block: PLL register file, CRTC, DAC / palette, cursor
// =====================================================================

static const int rage128_ppll_post_div[8] = { 1, 2, 4, 8, 3, 1, 6, 12 };

void bx_rage128_c::ppll_commit(void)
{
  for (int i = 0; i < 5; i++)
    ppll_work[i] = pll_regs[RAGE128_PLL_PPLL_REF_DIV + i];
}

double bx_rage128_c::ppll_vco_hz(int sel)
{
  Bit32u div = ppll_work[1 + (sel & 3)];
  Bit32u m = ppll_work[0] & RAGE128_PPLL_REF_DIV_MASK;
  Bit32u n = div & RAGE128_PPLL_FB_DIV_MASK;
  if (m < 2) m = 2;
  if (n < 4) n = 4;
  return ref_freq_hz * (double)n / (double)m;
}

double bx_rage128_c::dot_clock_hz(void)
{
  int sel;
  if ((pll_regs[RAGE128_PLL_VCLK_ECP_CNTL] & RAGE128_VCLK_SRC_SEL_MASK) != RAGE128_VCLK_SRC_PPLL)
    return ref_freq_hz;
  if (crtc_gen_cntl & RAGE128_CRTC_EXT_DISP_EN)
    sel = (clock_cntl_index >> RAGE128_PPLL_DIV_SEL_SHIFT) & 3;
  else
    sel = BX_RAGE128_THIS s.misc_output.clock_select & 3;
  Bit32u div = ppll_work[1 + sel];
  return ppll_vco_hz(sel) / rage128_ppll_post_div[(div >> RAGE128_PPLL_POST_DIV_SHIFT) & 7];
}

double bx_rage128_c::xpll_hz(void)
{
  Bit32u r = pll_regs[RAGE128_PLL_X_MPLL_REF_FB_DIV];
  Bit32u m = r & 0xff, n = (r >> 8) & 0xff;
  if (m < 2) m = 2;
  if (n < 2) n = 2;
  return 2.0 * ref_freq_hz * (double)n / (double)m;
}

double bx_rage128_c::mpll_hz(void)
{
  Bit32u r = pll_regs[RAGE128_PLL_X_MPLL_REF_FB_DIV];
  Bit32u m = r & 0xff, n = (r >> 16) & 0xff;
  if (m < 2) m = 2;
  if (n < 2) n = 2;
  return 2.0 * ref_freq_hz * (double)n / (double)m;
}

#define RAGE128_PCICLK_HZ 33333333.0

double bx_rage128_c::test_clock_hz(void)
{
  int sel = (test_debug_mux >> RAGE128_TEST_DEBUG_CLK_SHIFT) & 0x1f;
  int ppll_sel = (clock_cntl_index >> RAGE128_PPLL_DIV_SEL_SHIFT) & 3;
  Bit32u m;
  switch (sel) {
    case 0x01: return ref_freq_hz;
    case 0x02: return ppll_vco_hz(ppll_sel) / 2.0;
    case 0x03:
      m = ppll_work[0] & RAGE128_PPLL_REF_DIV_MASK;
      return ref_freq_hz / (double)((m < 2) ? 2 : m);
    case 0x05:
    case 0x06: return ppll_vco_hz(ppll_sel);
    case 0x0b: return xpll_hz();
    case 0x0c: return xpll_hz() / 2.0;
    case 0x0f:
      switch (pll_regs[RAGE128_PLL_XCLK_CNTL] & 0xf) {
        case 0: return RAGE128_PCICLK_HZ;
        case 1: return xpll_hz();
        case 2: return xpll_hz() / 2.0;
        case 3: return xpll_hz() / 4.0;
        case 4: return xpll_hz() / 8.0;
        default: return 0.0;
      }
    case 0x13: return mpll_hz();
    case 0x14: return mpll_hz() / 2.0;
    case 0x16:
      switch (pll_regs[RAGE128_PLL_MCLK_CNTL] & 0x7) {
        case 0: return RAGE128_PCICLK_HZ;
        case 1: return mpll_hz();
        case 2: return mpll_hz() / 2.0;
        case 3: return mpll_hz() / 4.0;
        case 4: return mpll_hz() / 8.0;
        case 7: return ref_freq_hz;
        default: return 0.0;
      }
    default: return 0.0;
  }
}

#define RAGE128_PLL_TEST_ACCESS_S (16.0 / 27000000.0)

Bit32u bx_rage128_c::pll_read(void)
{
  int idx = clock_cntl_index & RAGE128_PLL_ADDR_MASK;
  Bit32u v = pll_regs[idx];

  switch (idx) {
    case RAGE128_PLL_PPLL_REF_DIV:
    case RAGE128_PLL_PPLL_DIV_0:
    case RAGE128_PLL_PPLL_DIV_1:
    case RAGE128_PLL_PPLL_DIV_2:
    case RAGE128_PLL_PPLL_DIV_3:
      v = (v & ~RAGE128_PPLL_ATOMIC_UPDATE) | (ppll_update_pending ? RAGE128_PPLL_ATOMIC_UPDATE : 0);
      break;
    case RAGE128_PLL_TEST_CNTL: {
      // TEST_COUNT [31:24]: free-running counter of the test-mux clock
      Bit8u count = pll_test_count_base;
      if (!(pll_regs[RAGE128_PLL_TEST_CNTL] & (1 << 9))) {
        double hz = test_clock_hz();
        if (hz > 0.0) {
          pll_test_acc += RAGE128_PLL_TEST_ACCESS_S * hz;
          double elapsed_us = (double)(bx_virt_timer.time_usec(0) - pll_test_zero_usec);
          Bit64u ticks = (Bit64u)(elapsed_us * hz / 1000000.0 + pll_test_acc);
          count = (Bit8u)(pll_test_count_base + ticks);
        }
      }
      v = (v & 0x00ffffff) | ((Bit32u)count << 24);
      break;
    }
    default:
      break;
  }
  return v;
}

void bx_rage128_c::pll_write(Bit32u val, Bit32u mask)
{
  int idx = clock_cntl_index & RAGE128_PLL_ADDR_MASK;
  Bit32u merged;

  if (!(clock_cntl_index & RAGE128_PLL_WR_EN))
    return;
  merged = (pll_regs[idx] & ~mask) | (val & mask);
  switch (idx) {
    case RAGE128_PLL_PPLL_REF_DIV:
    case RAGE128_PLL_PPLL_DIV_0:
    case RAGE128_PLL_PPLL_DIV_1:
    case RAGE128_PLL_PPLL_DIV_2:
    case RAGE128_PLL_PPLL_DIV_3: {
      pll_regs[idx] = merged & ~RAGE128_PPLL_ATOMIC_UPDATE;
      Bit32u cntl = pll_regs[RAGE128_PLL_PPLL_CNTL];
      bool atomic = (cntl & (RAGE128_PPLL_ATOMIC_UPDATE_EN | RAGE128_PPLL_VGA_ATOMIC_UPDATE_EN)) != 0;
      bool requested = (mask & RAGE128_PPLL_ATOMIC_UPDATE) && (merged & RAGE128_PPLL_ATOMIC_UPDATE);
      if (atomic && requested && (cntl & RAGE128_PPLL_ATOMIC_UPDATE_SYNC)) {
        ppll_update_pending = true;
        return;
      }
      if (atomic && !requested)
        return;
      ppll_commit();
      ppll_update_pending = false;
      timing_update();
      return;
    }
    case RAGE128_PLL_TEST_CNTL:
      if (mask & 0xff000000) {
        pll_test_count_base = merged >> 24;
        pll_test_zero_usec = bx_virt_timer.time_usec(0);
        pll_test_acc = 0;
      }
      pll_regs[idx] = merged & 0x00ffffff;
      return;
    case RAGE128_PLL_VCLK_ECP_CNTL:
    case RAGE128_PLL_HTOTAL_CNTL:
      pll_regs[idx] = merged;
      timing_update();
      return;
    default:
      pll_regs[idx] = merged;
      return;
  }
}

// Palette dword path (PALETTE_INDEX/PALETTE_DATA)
void bx_rage128_c::palette_data_write(Bit32u val, Bit32u mask)
{
  int idx = palette_index & 0xff;
  Bit32u cur, merged;
  Bit8u r, g, b;

  if (dac_cntl & RAGE128_DAC_8BIT_EN)
    cur = BX_RAGE128_THIS s.pel.data[idx].blue | (BX_RAGE128_THIS s.pel.data[idx].green << 8) |
          (BX_RAGE128_THIS s.pel.data[idx].red << 16);
  else
    cur = (BX_RAGE128_THIS s.pel.data[idx].blue & 0x3f) | ((BX_RAGE128_THIS s.pel.data[idx].green & 0x3f) << 8) |
          ((BX_RAGE128_THIS s.pel.data[idx].red & 0x3f) << 16);
  merged = (cur & ~mask) | (val & mask);
  b = merged & 0xff;
  g = (merged >> 8) & 0xff;
  r = (merged >> 16) & 0xff;
  if (!(dac_cntl & RAGE128_DAC_8BIT_EN)) {
    b &= 0x3f; g &= 0x3f; r &= 0x3f;
  }
  BX_RAGE128_THIS s.pel.data[idx].red = r;
  BX_RAGE128_THIS s.pel.data[idx].green = g;
  BX_RAGE128_THIS s.pel.data[idx].blue = b;
  if (on_cce_thread()) {
    palette_dirty = true;
  } else {
    bx_gui->palette_change_common(idx, r << BX_RAGE128_THIS s.dac_shift, g << BX_RAGE128_THIS s.dac_shift,
                                  b << BX_RAGE128_THIS s.dac_shift);
  }
  needs_update_dispentire = true;
  palette_index = (palette_index & ~0xff) | ((idx + 1) & 0xff);
}

Bit32u bx_rage128_c::palette_data_read(void)
{
  int idx = (palette_index >> 16) & 0xff;
  Bit32u v = BX_RAGE128_THIS s.pel.data[idx].blue | (BX_RAGE128_THIS s.pel.data[idx].green << 8) |
             (BX_RAGE128_THIS s.pel.data[idx].red << 16);
  palette_index = (palette_index & ~0xff0000) | (((idx + 1) & 0xff) << 16);
  return v;
}

void bx_rage128_c::latch_crtc_offset(void)
{
  crtc_offset_latched = crtc_offset;
  crtc_offset_pending = false;
  needs_update_mode = true;
  needs_update_dispentire = true;
}

// CUR_LOCK is one flag surfaced as bit 31 of the three geometry registers:
// while locked, writes land only in the programmed images; the write that
// clears the lock publishes the whole tuple. The published tuple is picked
// up by scanout at the next vertical blank (cursor_frame_latch).
void bx_rage128_c::cursor_publish(void)
{
  if (cur_lock)
    return;
  cur_offset_act = cur_offset;
  cur_posn_act = cur_horz_vert_posn;
  cur_hvoff_act = cur_horz_vert_off;
}

// Screen rectangle covered by a cursor tuple (the image is 64x64, the
// HORZ/VERT_OFF fields clip its top-left corner)
void bx_rage128_c::cursor_rect(Bit32u posn, Bit32u hvoff, int *x0, int *y0, int *x1, int *y1)
{
  *x0 = (posn >> 16) & 0x7ff;
  *y0 = posn & 0x7ff;
  *x1 = *x0 + 64 - (int)((hvoff >> 16) & 0x3f);
  *y1 = *y0 + 64 - (int)(hvoff & 0x3f);
}

// Vertical blank: latch the cursor tuple the CRTC composites for the next
// frame. Only the tiles under the old and the new cursor are invalidated
// and repainted right away, so cursor motion runs at the frame rate and
// costs a few tiles rather than a full-screen refresh at the (much lower)
// VGA update rate.
void bx_rage128_c::cursor_frame_latch(void)
{
  bool en = disp_ext && ((crtc_gen_cntl & RAGE128_CRTC_CUR_EN) != 0);
  bool changed = (en != cur_lat_en);
  if (en && !changed) {
    changed = (cur_offset_act != cur_lat_offset) || (cur_posn_act != cur_lat_posn) ||
              (cur_hvoff_act != cur_lat_hvoff) || (cur_clr0 != cur_lat_clr0) ||
              (cur_clr1 != cur_lat_clr1) || cur_bitmap_dirty;
  }
  if (!changed) {
    cur_bitmap_dirty = false;
    return;
  }
  int ox0, oy0, ox1, oy1, nx0, ny0, nx1, ny1;
  bool old_en = cur_lat_en;
  cursor_rect(cur_lat_posn, cur_lat_hvoff, &ox0, &oy0, &ox1, &oy1);
  cur_lat_en = en;
  cur_lat_offset = cur_offset_act & 0x01fffff0;
  cur_lat_posn = cur_posn_act;
  cur_lat_hvoff = cur_hvoff_act;
  cur_lat_clr0 = cur_clr0;
  cur_lat_clr1 = cur_clr1;
  cur_bitmap_dirty = false;
  cursor_rect(cur_lat_posn, cur_lat_hvoff, &nx0, &ny0, &nx1, &ny1);
  if (!disp_ext)
    return;

  int bx0 = 0x7fffffff, by0 = 0x7fffffff, bx1 = 0, by1 = 0;
  if (old_en && (ox1 > ox0) && (oy1 > oy0)) {
    redraw_area(ox0, oy0, ox1 - ox0, oy1 - oy0);
    if (ox0 < bx0) bx0 = ox0;
    if (oy0 < by0) by0 = oy0;
    if (ox1 > bx1) bx1 = ox1;
    if (oy1 > by1) by1 = oy1;
  }
  if (en && (nx1 > nx0) && (ny1 > ny0)) {
    redraw_area(nx0, ny0, nx1 - nx0, ny1 - ny0);
    if (nx0 < bx0) bx0 = nx0;
    if (ny0 < by0) by0 = ny0;
    if (nx1 > bx1) bx1 = nx1;
    if (ny1 > by1) by1 = ny1;
  }
  // A pending mode change (or a mode not yet shown) is handled by update()
  if (needs_update_mode || needs_update_dispentire || !ext_last)
    return;
  if ((bx1 > bx0) && (by1 > by0))
    paint_tiles_in(bx0, by0, bx1, by1);
}

// Paint the dirty display tiles intersecting [x0,x1) x [y0,y1) now
void bx_rage128_c::paint_tiles_in(int x0, int y0, int x1, int y1)
{
  bx_svga_tileinfo_t info;
  Bit8u pel8[256 * 3];
  unsigned w, h;

  if (!bx_gui->graphics_tile_info_common(&info))
    return;
  if (info.snapshot_mode)
    return;
  if (info.is_indexed && (disp_bpp != 8) && (disp_bpp != 4))
    return;
  if (x0 < 0) x0 = 0;
  if (y0 < 0) y0 = 0;
  if (x1 > (int)disp_xres) x1 = disp_xres;
  if (y1 > (int)disp_yres) y1 = disp_yres;
  if ((x0 >= x1) || (y0 >= y1))
    return;
  for (int i = 0; i < 256; i++) {
    pel8[i * 3 + 0] = BX_RAGE128_THIS s.pel.data[i].red;
    pel8[i * 3 + 1] = BX_RAGE128_THIS s.pel.data[i].green;
    pel8[i * 3 + 2] = BX_RAGE128_THIS s.pel.data[i].blue;
  }
  unsigned xt0 = x0 / X_TILESIZE, xt1 = (x1 - 1) / X_TILESIZE;
  unsigned yt0 = y0 / Y_TILESIZE, yt1 = (y1 - 1) / Y_TILESIZE;
  for (unsigned yti = yt0; yti <= yt1; yti++) {
    for (unsigned xti = xt0; xti <= xt1; xti++) {
      if (!GET_TILE_UPDATED(xti, yti))
        continue;
      unsigned xc = xti * X_TILESIZE, yc = yti * Y_TILESIZE;
      paint_tile(xc, yc, &info, pel8);
      bx_gui->graphics_tile_get(xc, yc, &w, &h);
      bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
      SET_TILE_UPDATED(BX_RAGE128_THIS, xti, yti, 0);
    }
  }
  bx_gui->flush();
}

void bx_rage128_c::snapshot_take(void)
{
  Bit32u hcount = 0;
  if (BX_RAGE128_THIS s.htotal_usec > 0) {
    Bit32u htot = (crtc_h_total_disp & 0x1ff) + 1;
    Bit32u line_usec = get_display_usec() % BX_RAGE128_THIS s.htotal_usec;
    hcount = (line_usec * htot / BX_RAGE128_THIS s.htotal_usec) & 0x1ff;
  }
  snapshot_vh_counts = hcount | ((get_current_line() & 0x7ff) << 16);
  snapshot_f_count = frame_count & 0x1fffff;
  gen_int_status |= RAGE128_GIS_SNAPSHOT;
  gen_int_update();
}

bool bx_rage128_c::display_reg_read(Bit32u off, Bit32u *val)
{
  switch (off) {
    case RAGE128_CLOCK_CNTL_INDEX: *val = clock_cntl_index; return true;
    case RAGE128_CLOCK_CNTL_DATA:  *val = pll_read(); return true;
    case RAGE128_CRTC_GEN_CNTL:    *val = crtc_gen_cntl; return true;
    case RAGE128_CRTC_EXT_CNTL:    *val = crtc_ext_cntl; return true;
    case RAGE128_DAC_CNTL:
      *val = (dac_cntl & 0x00ffff7f) | RAGE128_DAC_CMP_OUTPUT | ((Bit32u)dac_mask_prog << 24);
      return true;
    case RAGE128_CRTC_STATUS:
      *val = RAGE128_CRTC_STATUS_DEFAULT | (in_vblank() ? 0x1 : 0x0) | (vblank_save ? 0x2 : 0x0) |
             ((get_current_line() & 1) << 2) | ((frame_count & 1) << 3);
      return true;
    case RAGE128_MEM_VGA_WP_SEL: *val = mem_vga_wp_sel; return true;
    case RAGE128_MEM_VGA_RP_SEL: *val = mem_vga_rp_sel; return true;
    case RAGE128_PALETTE_INDEX:  *val = palette_index; return true;
    case RAGE128_PALETTE_DATA:   *val = palette_data_read(); return true;
    case RAGE128_CRTC_H_TOTAL_DISP:    *val = crtc_h_total_disp; return true;
    case RAGE128_CRTC_H_SYNC_STRT_WID: *val = crtc_h_sync_strt_wid; return true;
    case RAGE128_CRTC_V_TOTAL_DISP:    *val = crtc_v_total_disp; return true;
    case RAGE128_CRTC_V_SYNC_STRT_WID: *val = crtc_v_sync_strt_wid; return true;
    case RAGE128_CRTC_VLINE_CRNT_VLINE:
      *val = (crtc_vline & 0x7ff) | ((get_current_line() & 0x7ff) << 16);
      return true;
    case RAGE128_CRTC_CRNT_FRAME:    *val = frame_count; return true;
    case RAGE128_SNAPSHOT_VH_COUNTS: *val = snapshot_vh_counts; return true;
    case RAGE128_SNAPSHOT_F_COUNT:   *val = snapshot_f_count; return true;
    case RAGE128_N_VIF_COUNT:        *val = n_vif_count; return true;
    case RAGE128_SNAPSHOT_VIF_COUNT: *val = snapshot_vif_cntl; return true;
    case RAGE128_CRTC_GUI_TRIG_VLINE: {
      Bit32u strt = crtc_gui_trig_vline & 0x7ff;
      Bit32u end = (crtc_gui_trig_vline >> 16) & 0x7ff;
      Bit32u line = get_current_line();
      *val = (crtc_gui_trig_vline & 0x07ff07ff) | (((line >= strt) && (line <= end)) ? 0x80000000 : 0);
      return true;
    }
    case RAGE128_CRTC_DEBUG: *val = crtc_debug; return true;
    case RAGE128_CRTC_OFFSET:
      *val = (crtc_offset & 0x01ffffff) | (crtc_offset_pending ? 0x40000000 : 0) | (crtc_offset_lock ? 0x80000000 : 0);
      return true;
    case RAGE128_CRTC_OFFSET_CNTL:
      *val = (crtc_offset_cntl & 0x3fffffff) | (crtc_offset_pending ? 0x40000000 : 0) | (crtc_offset_lock ? 0x80000000 : 0);
      return true;
    case RAGE128_CRTC_PITCH: *val = crtc_pitch; return true;
    case RAGE128_CUR_OFFSET: *val = cur_offset | (cur_lock ? 0x80000000 : 0); return true;
    case RAGE128_CUR_HORZ_VERT_POSN: *val = cur_horz_vert_posn | (cur_lock ? 0x80000000 : 0); return true;
    case RAGE128_CUR_HORZ_VERT_OFF:  *val = cur_horz_vert_off | (cur_lock ? 0x80000000 : 0); return true;
    case RAGE128_CUR_CLR0: *val = cur_clr0; return true;
    case RAGE128_CUR_CLR1: *val = cur_clr1; return true;
    case RAGE128_OVR_CLR: *val = ovr_clr; return true;
    case RAGE128_OVR_WID_LEFT_RIGHT: *val = ovr_wid_left_right; return true;
    case RAGE128_OVR_WID_TOP_BOTTOM: *val = ovr_wid_top_bottom; return true;
    case RAGE128_DAC_EXT_CNTL: *val = dac_ext_cntl; return true;
    case RAGE128_DAC_CRC_SIG:  *val = dac_crc_sig; return true;
    case RAGE128_DDA_CONFIG:   *val = dda_config; return true;
    case RAGE128_DDA_ON_OFF:   *val = dda_on_off; return true;
    case RAGE128_VGA_DDA_CONFIG: *val = vga_dda_config; return true;
    case RAGE128_VGA_DDA_ON_OFF: *val = vga_dda_on_off; return true;
    default:
      return false;
  }
}

bool bx_rage128_c::display_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
#define MERGE(field) ((field) = ((field) & ~mask) | (val & mask))
  switch (off) {
    case RAGE128_CLOCK_CNTL_INDEX:
      MERGE(clock_cntl_index);
      clock_cntl_index &= 0x3ff;
      timing_update();
      return true;
    case RAGE128_CLOCK_CNTL_DATA:
      pll_write(val, mask);
      return true;
    case RAGE128_CRTC_GEN_CNTL: {
      Bit32u was_ext = crtc_gen_cntl & RAGE128_CRTC_EXT_DISP_EN;
      Bit32u gen_old = crtc_gen_cntl;
      MERGE(crtc_gen_cntl);
      if (!was_ext && (crtc_gen_cntl & RAGE128_CRTC_EXT_DISP_EN))
        crtc_v_disp_active = (crtc_v_total_disp >> 16) & 0x7ff;
      disp_ext = (crtc_gen_cntl & RAGE128_CRTC_EXT_DISP_EN) != 0;
      // CRTC_CUR_EN [16] and CRTC_CUR_MODE [22:20] only feed the cursor,
      // which is latched at vertical blank; drivers toggle them on every
      // pointer show/hide, so they must not re-derive the mode.
      if ((gen_old ^ crtc_gen_cntl) & ~(RAGE128_CRTC_CUR_EN | 0x00700000)) {
        update_banking();
        needs_update_mode = true;
        needs_update_dispentire = true;
        timing_update();
      }
      return true;
    }
    case RAGE128_CRTC_EXT_CNTL: {
      Bit32u ext_old = crtc_ext_cntl;
      MERGE(crtc_ext_cntl);
      if (ext_old != crtc_ext_cntl) {
        update_banking();
        needs_update_mode = true;
        needs_update_dispentire = true;
      }
      return true;
    }
    case RAGE128_DAC_CNTL: {
      Bit32u dac_old = dac_cntl;
      MERGE(dac_cntl);
      dac_cntl &= 0x00ffff7f;
      if ((dac_old ^ dac_cntl) & (RAGE128_DAC_4BPP_PIX_ORDER | RAGE128_DAC_PDWN | RAGE128_DAC_8BIT_EN)) {
        needs_update_mode = true;
        needs_update_dispentire = true;
      }
      if (mask & 0xff000000) {
        Bit8u m = (Bit8u)(mask >> 24);
        dac_mask_prog = (Bit8u)((dac_mask_prog & ~m) | ((val >> 24) & m));
      }
      BX_RAGE128_THIS s.dac_shift = (dac_cntl & RAGE128_DAC_8BIT_EN) ? 0 : 2;
      return true;
    }
    case RAGE128_CRTC_STATUS:
      if ((mask & 0x2) && (val & 0x2))
        vblank_save = false;
      return true;
    case RAGE128_MEM_VGA_WP_SEL:
      MERGE(mem_vga_wp_sel);
      mem_vga_wp_sel &= 0x03ff03ff;
      update_banking();
      return true;
    case RAGE128_MEM_VGA_RP_SEL:
      MERGE(mem_vga_rp_sel);
      mem_vga_rp_sel &= 0x03ff03ff;
      update_banking();
      return true;
    case RAGE128_PALETTE_INDEX:
      MERGE(palette_index);
      palette_index &= 0x00ff00ff;
      return true;
    case RAGE128_PALETTE_DATA:
      palette_data_write(val, mask);
      return true;
    case RAGE128_CRTC_H_TOTAL_DISP: {
      Bit32u old = crtc_h_total_disp;
      MERGE(crtc_h_total_disp);
      if (old != crtc_h_total_disp) {
        needs_update_mode = true;
        timing_update();
      }
      return true;
    }
    case RAGE128_CRTC_H_SYNC_STRT_WID:
      MERGE(crtc_h_sync_strt_wid);
      timing_update();
      return true;
    case RAGE128_CRTC_V_TOTAL_DISP: {
      Bit32u old = crtc_v_total_disp, old_active = crtc_v_disp_active;
      MERGE(crtc_v_total_disp);
      if (mask & 0x07ff0000)
        crtc_v_disp_active = (crtc_v_total_disp >> 16) & 0x7ff;
      if ((old != crtc_v_total_disp) || (old_active != crtc_v_disp_active)) {
        needs_update_mode = true;
        timing_update();
      }
      return true;
    }
    case RAGE128_CRTC_V_SYNC_STRT_WID:
      MERGE(crtc_v_sync_strt_wid);
      timing_update();
      return true;
    case RAGE128_CRTC_VLINE_CRNT_VLINE:
      MERGE(crtc_vline);
      crtc_vline &= 0x7ff;
      return true;
    case RAGE128_CRTC_CRNT_FRAME:
    case RAGE128_SNAPSHOT_VH_COUNTS:
    case RAGE128_SNAPSHOT_F_COUNT:
      return true;
    case RAGE128_N_VIF_COUNT:
      MERGE(n_vif_count);
      n_vif_count &= 0x800003ff;
      return true;
    case RAGE128_SNAPSHOT_VIF_COUNT:
      MERGE(snapshot_vif_cntl);
      snapshot_vif_cntl &= 0x03000000;
      if (val & mask & 0x02000000)
        snapshot_take();
      return true;
    case RAGE128_CRTC_GUI_TRIG_VLINE:
      MERGE(crtc_gui_trig_vline);
      crtc_gui_trig_vline &= 0x07ff07ff;
      return true;
    case RAGE128_CRTC_DEBUG:
      MERGE(crtc_debug);
      return true;
    case RAGE128_CRTC_OFFSET: {
      Bit32u merged = (crtc_offset & ~mask) | (val & mask);
      crtc_offset = merged & 0x01fffff8;
      if (mask & 0x80000000)
        crtc_offset_lock = (val & 0x80000000) != 0;
      if (mask & 0x01fffff8) {
        // The target buffer must hold the finished frame before scanout
        // latches it: complete any deferred 3D batch now.
        if (on_cce_thread())
          raster_flush();
        else
          pm4_drain_wait();
        crtc_offset_pending = true;
        if (crtc_offset_cntl & RAGE128_CRTC_OFFSET_FLIP_CNTL) {
          // latch at the next horizontal blank: modelled as immediate
          if (!crtc_offset_lock) {
            latch_crtc_offset();
            pm4_flip_notify();
          }
        }
      }
      return true;
    }
    case RAGE128_CRTC_OFFSET_CNTL:
      MERGE(crtc_offset_cntl);
      crtc_offset_cntl &= 0x3fffffff;
      if (mask & 0x80000000)
        crtc_offset_lock = (val & 0x80000000) != 0;
      needs_update_mode = true;
      return true;
    case RAGE128_CRTC_PITCH:
      MERGE(crtc_pitch);
      crtc_pitch &= 0x3ff;
      needs_update_mode = true;
      return true;
    case RAGE128_CUR_OFFSET:
      MERGE(cur_offset);
      if (mask & 0x80000000)
        cur_lock = (cur_offset & 0x80000000) != 0;
      cur_offset &= 0x01fffff0;
      cursor_publish();
      return true;
    case RAGE128_CUR_HORZ_VERT_POSN:
      MERGE(cur_horz_vert_posn);
      if (mask & 0x80000000)
        cur_lock = (cur_horz_vert_posn & 0x80000000) != 0;
      cur_horz_vert_posn &= 0x07ff07ff;
      cursor_publish();
      return true;
    case RAGE128_CUR_HORZ_VERT_OFF:
      MERGE(cur_horz_vert_off);
      if (mask & 0x80000000)
        cur_lock = (cur_horz_vert_off & 0x80000000) != 0;
      cur_horz_vert_off &= 0x003f003f;
      cursor_publish();
      return true;
    case RAGE128_CUR_CLR0:
      MERGE(cur_clr0);
      cur_clr0 &= 0x00ffffff;
      return true;
    case RAGE128_CUR_CLR1:
      MERGE(cur_clr1);
      cur_clr1 &= 0x00ffffff;
      return true;
    case RAGE128_OVR_CLR:
      MERGE(ovr_clr);
      return true;
    case RAGE128_OVR_WID_LEFT_RIGHT:
      MERGE(ovr_wid_left_right);
      return true;
    case RAGE128_OVR_WID_TOP_BOTTOM:
      MERGE(ovr_wid_top_bottom);
      return true;
    case RAGE128_DAC_EXT_CNTL: {
      Bit32u ext_old = dac_ext_cntl;
      MERGE(dac_ext_cntl);
      if ((ext_old ^ dac_ext_cntl) & 0x0000fff0) {
        needs_update_mode = true;
        needs_update_dispentire = true;
      }
      return true;
    }
    case RAGE128_DAC_CRC_SIG:
      return true;
    case RAGE128_DDA_CONFIG:     MERGE(dda_config); return true;
    case RAGE128_DDA_ON_OFF:     MERGE(dda_on_off); return true;
    case RAGE128_VGA_DDA_CONFIG: MERGE(vga_dda_config); return true;
    case RAGE128_VGA_DDA_ON_OFF: MERGE(vga_dda_on_off); return true;
    default:
      return false;
  }
#undef MERGE
}

void bx_rage128_c::display_reset(void)
{
  memset(pll_regs, 0, sizeof(pll_regs));
  pll_regs[RAGE128_PLL_CLK_PIN_CNTL] = 0x000000ff;
  pll_regs[RAGE128_PLL_PPLL_CNTL]    = 0x0000cc03;
  pll_regs[RAGE128_PLL_XPLL_CNTL]    = 0x0000cc03;
  pll_regs[RAGE128_PLL_XDLL_CNTL]    = 0x000b000b;
  pll_regs[RAGE128_PLL_MPLL_CNTL]    = 0x0000cc03;
  pll_regs[RAGE128_PLL_AGP_PLL_CNTL] = 0x7a770000;
  pll_regs[RAGE128_PLL_FCP_CNTL]     = 0x00000404;
  pll_regs[RAGE128_PLL_TEST_CNTL]    = 0x00000200;
  pll_test_count_base = 0;
  pll_test_zero_usec = bx_virt_timer.time_usec(0);
  pll_test_acc = 0;
  clock_cntl_index = 0;
  ppll_update_pending = false;
  ppll_commit();

  crtc_gen_cntl = RAGE128_CRTC_GEN_CNTL_DEFAULT;
  crtc_ext_cntl = RAGE128_CRTC_EXT_CNTL_DEFAULT;
  dac_cntl = RAGE128_DAC_CNTL_DEFAULT & 0x00ffff7f;
  dac_mask_prog = 0xff;
  BX_RAGE128_THIS s.dac_shift = 2;
  crtc_h_total_disp = 0;
  crtc_h_sync_strt_wid = 0;
  crtc_v_total_disp = 0;
  crtc_v_disp_active = 0;
  crtc_v_sync_strt_wid = 0;
  crtc_vline = 0;
  crtc_gui_trig_vline = 0;
  crtc_debug = 0;
  crtc_offset = 0;
  crtc_offset_latched = 0;
  crtc_offset_cntl = 0;
  crtc_pitch = 0;
  cur_offset = 0;
  cur_horz_vert_posn = 0;
  cur_horz_vert_off = 0;
  cur_offset_act = 0;
  cur_posn_act = 0;
  cur_hvoff_act = 0;
  cur_lock = false;
  cur_clr0 = 0;
  cur_clr1 = 0;
  cur_lat_en = false;
  cur_lat_offset = 0;
  cur_lat_posn = 0;
  cur_lat_hvoff = 0;
  cur_lat_clr0 = 0;
  cur_lat_clr1 = 0;
  cur_bitmap_dirty = false;
  ovr_clr = 0;
  ovr_wid_left_right = 0;
  ovr_wid_top_bottom = 0;
  dac_ext_cntl = 0;
  dac_crc_sig = 0;
  dda_config = 0;
  dda_on_off = 0;
  vga_dda_config = 0;
  vga_dda_on_off = 0;
  palette_index = 0;
  frame_count = 0;
  snapshot_vh_counts = 0;
  snapshot_f_count = 0;
  n_vif_count = 0;
  snapshot_vif_cntl = 0;
  crtc_offset_pending = false;
  crtc_offset_lock = false;
  vblank_save = false;
  mem_vga_wp_sel = 0;
  mem_vga_rp_sel = 0;
  disp_ext = false;
  update_banking();
  needs_update_mode = true;
  needs_update_dispentire = true;
}

// ---------------------------------------------------------------------
// Timing for the VGA core: vertical timer parameters from the extended CRTC
// ---------------------------------------------------------------------

void bx_rage128_c::get_crtc_params(bx_crtc_params_t *crtcp, Bit32u *vclock)
{
  double dot_hz;

  if (!disp_ext) {
    bx_vgacore_c::get_crtc_params(crtcp, vclock);
    // The programmable PPLL drives VGA scanout too once the VCLK mux selects it
    if ((pll_regs[RAGE128_PLL_VCLK_ECP_CNTL] & RAGE128_VCLK_SRC_SEL_MASK) == RAGE128_VCLK_SRC_PPLL) {
      dot_hz = dot_clock_hz();
      if (dot_hz >= 1000000.0) {
        *vclock = (Bit32u)dot_hz;
        if (BX_RAGE128_THIS s.x_dotclockdiv2) *vclock >>= 1;
      }
    }
    return;
  }
  dot_hz = dot_clock_hz();
  if (dot_hz < 1000000.0)
    dot_hz = ref_freq_hz;
  *vclock = (Bit32u)dot_hz;

  // Horizontal fields are in 8-pixel character clocks; the VGA core
  // multiplies htotal by the sequencer character width (8 or 9).
  Bit32u htotal = (crtc_h_total_disp & 0x1ff) + 1;
  Bit32u cwidth = ((BX_RAGE128_THIS s.sequencer.reg1 & 0x01) == 1) ? 8 : 9;
  if (cwidth == 9)
    htotal = (htotal * 8 + 4) / 9;
  Bit32u vtotal = (crtc_v_total_disp & 0x7ff) + 1;
  Bit32u vdisp = (crtc_v_disp_active & 0x7ff) + 1;
  Bit32u vsync = (crtc_v_sync_strt_wid & 0x7ff) + 1;
  if (crtc_gen_cntl & RAGE128_CRTC_INTERLACE_EN) {
    vtotal = (vtotal + 1) >> 1;
    vdisp = (vdisp + 1) >> 1;
    vsync = (vsync + 1) >> 1;
  }
  if (htotal < 8) htotal = 8;
  if (vtotal < 8) vtotal = 8;
  crtcp->htotal = (Bit16u)htotal;
  crtcp->vtotal = (Bit16u)vtotal;
  crtcp->vbstart = (Bit16u)vdisp;
  crtcp->vrstart = (Bit16u)vsync;
}

// ---------------------------------------------------------------------
// Scanout
// ---------------------------------------------------------------------

void bx_rage128_c::redraw_area(unsigned x0, unsigned y0, unsigned width, unsigned height)
{
  redraw_area((Bit32s)x0, (Bit32s)y0, width, height);
}

void bx_rage128_c::redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1;

  if (!disp_ext) {
    bx_vgacore_c::redraw_area(x0, y0, width, height);
    return;
  }
  if ((x0 + (Bit32s)width <= 0) || (y0 + (Bit32s)height <= 0))
    return;
  needs_update_tile = true;
  xt0 = x0 <= 0 ? 0 : x0 / X_TILESIZE;
  yt0 = y0 <= 0 ? 0 : y0 / Y_TILESIZE;
  if (x0 < (Bit32s)disp_xres) {
    xt1 = (x0 + width - 1) / X_TILESIZE;
  } else {
    xt1 = (disp_xres - 1) / X_TILESIZE;
  }
  if (y0 < (Bit32s)disp_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (disp_yres - 1) / Y_TILESIZE;
  }
  for (yti = yt0; yti <= yt1; yti++) {
    for (xti = xt0; xti <= xt1; xti++) {
      SET_TILE_UPDATED(BX_RAGE128_THIS, xti, yti, 1);
    }
  }
}

// Derive the scanout geometry from the extended CRTC registers
void bx_rage128_c::update_mode(void)
{
  unsigned pix_width = (crtc_gen_cntl >> RAGE128_CRTC_PIX_WIDTH_SHIFT) & 7;
  unsigned bpp;

  switch (pix_width) {
    case 1: bpp = 4; break;
    case 2: bpp = 8; break;
    case 3: bpp = 15; break;
    case 4: bpp = 16; break;
    case 5: bpp = 24; break;
    case 6: bpp = 32; break;
    default: bpp = 0; break;
  }
  unsigned xres = (((crtc_h_total_disp >> 16) & 0xff) + 1) * 8;
  unsigned yres = (crtc_v_disp_active & 0x7ff) + 1;
  Bit32u pitch = (crtc_pitch & 0x3ff) * 8;
  switch (bpp) {
    case 4:  pitch >>= 1; break;
    case 8:  break;
    case 15:
    case 16: pitch <<= 1; break;
    case 24: pitch *= 3; break;
    case 32: pitch <<= 2; break;
    default: break;
  }
  disp_dblscan = (crtc_gen_cntl & RAGE128_CRTC_DBL_SCAN_EN) != 0;
  disp_blank = (bpp == 0) || !(crtc_gen_cntl & RAGE128_CRTC_EN) ||
               (crtc_gen_cntl & RAGE128_CRTC_DISP_REQ_EN_B) ||
               ((crtc_ext_cntl & RAGE128_CRTC_DISPLAY_DIS) && !(dac_ext_cntl & RAGE128_DAC_FORCE_BLANK_OFF_EN));
  disp_dac_const = false;
  disp_dac_const_color = 0;
  if (dac_cntl & RAGE128_DAC_PDWN) {
    disp_dac_const = true;
  } else if ((dac_ext_cntl & RAGE128_DAC_FORCE_DATA_EN) && !disp_blank) {
    Bit8u v = (Bit8u)(dac_ext_cntl >> RAGE128_DAC_FORCE_DATA_SHIFT);
    disp_dac_const = true;
    switch ((dac_ext_cntl >> RAGE128_DAC_FORCE_DATA_SEL_SHIFT) & 3) {
      case 0: disp_dac_const_color = v << 16; break;
      case 1: disp_dac_const_color = v << 8; break;
      case 2: disp_dac_const_color = v; break;
      default: disp_dac_const_color = (v << 16) | (v << 8) | v; break;
    }
  }
  if (bpp == 0) bpp = 8;
  if (xres < 8) xres = 8;
  if (yres < 1) yres = 1;
  if (xres > BX_RAGE128_THIS s.max_xres) xres = BX_RAGE128_THIS s.max_xres;
  if (yres > BX_RAGE128_THIS s.max_yres) yres = BX_RAGE128_THIS s.max_yres;
  if (pitch == 0) pitch = xres * ((bpp + 7) / 8);

  if ((xres != disp_xres) || (yres != disp_yres) || (bpp != disp_bpp)) {
    BX_INFO(("switched to %u x %u x %u", xres, yres, bpp));
  }
  disp_xres = xres;
  disp_yres = yres;
  disp_bpp = bpp;
  disp_pitch = pitch;
  disp_base = crtc_offset_latched & 0x01fffff8;
  BX_RAGE128_THIS s.last_xres = xres;
  BX_RAGE128_THIS s.last_yres = yres;
  BX_RAGE128_THIS s.last_bpp = (bpp == 15) ? 16 : bpp;
  BX_RAGE128_THIS s.last_fh = 0;
  ov0_update();
}

// Hardware cursor: 64x64, 16 bytes per line = 8 byte AND mask then 8 byte
// XOR mask, MSB first. AND=0 draws CLR0/CLR1 by the XOR bit; AND=1 is
// transparent (XOR=0) or inverts the screen (XOR=1).
void bx_rage128_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  // Composited from the tuple latched at the last vertical blank
  if (!cur_lat_en || disp_dac_const || disp_blank)
    return;

  int posx = (cur_lat_posn >> 16) & 0x7ff;
  int posy = cur_lat_posn & 0x7ff;
  int xoff = (cur_lat_hvoff >> 16) & 0x3f;
  int yoff = cur_lat_hvoff & 0x3f;
  int cw = 64 - xoff;
  int ch = 64 - yoff;
  if ((cw <= 0) || (ch <= 0))
    return;
  Bit32u base = cur_lat_offset;
  Bit32u col0 = cur_lat_clr0;
  Bit32u col1 = cur_lat_clr1;

  unsigned w, h;
  Bit8u *tile_ptr;
  if (info->snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    w = disp_xres;
    h = disp_yres;
  } else {
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  }
  int tx0 = (int)xc, ty0 = (int)yc, tx1 = (int)(xc + w), ty1 = (int)(yc + h);
  int cx0 = posx, cy0 = posy, cx1 = posx + cw, cy1 = posy + ch;
  if (cx0 < tx0) cx0 = tx0;
  if (cy0 < ty0) cy0 = ty0;
  if (cx1 > tx1) cx1 = tx1;
  if (cy1 > ty1) cy1 = ty1;
  if ((cx0 >= cx1) || (cy0 >= cy1))
    return;
  if (info->bpp == 15) info->bpp = 16;
  unsigned tbytes = info->bpp / 8;

  for (int y = cy0; y < cy1; y++) {
    int row = (y - posy) + yoff;
    Bit32u laddr = base + row * 16;
    Bit8u *tp = tile_ptr + info->pitch * (y - ty0) + tbytes * (cx0 - tx0);
    for (int x = cx0; x < cx1; x++) {
      int xx = (x - posx) + xoff;
      int a = (BX_RAGE128_THIS s.memory[(laddr + (xx >> 3)) & vram_mask] >> (7 - (xx & 7))) & 1;
      int sbit = (BX_RAGE128_THIS s.memory[(laddr + 8 + (xx >> 3)) & vram_mask] >> (7 - (xx & 7))) & 1;
      if (!a || sbit) {
        Bit32u colour;
        if (!a) {
          colour = sbit ? col1 : col0;
        } else {
          // invert the pixel currently in the tile
          Bit32u cur = 0;
          if (info->is_little_endian) {
            for (unsigned i = 0; i < tbytes; i++) cur |= (Bit32u)tp[i] << (i * 8);
          } else {
            for (unsigned i = 0; i < tbytes; i++) cur = (cur << 8) | tp[i];
          }
          colour = ~cur;
          if (info->is_indexed) {
            tp[0] = (Bit8u)colour;
            tp += tbytes;
            continue;
          }
          // write back the inverted host pixel directly
          if (info->is_little_endian) {
            for (unsigned i = 0; i < tbytes; i++) *(tp++) = (Bit8u)(colour >> (i * 8));
          } else {
            for (int i = (int)tbytes - 1; i >= 0; i--) *(tp++) = (Bit8u)(colour >> (i * 8));
          }
          continue;
        }
        if (!info->is_indexed) {
          colour = MAKE_COLOUR(
            colour, 24, info->red_shift, info->red_mask,
            colour, 16, info->green_shift, info->green_mask,
            colour, 8, info->blue_shift, info->blue_mask);
          if (info->is_little_endian) {
            for (int i = 0; i < info->bpp; i += 8) *(tp++) = (Bit8u)(colour >> i);
          } else {
            for (int i = info->bpp - 8; i > -8; i -= 8) *(tp++) = (Bit8u)(colour >> i);
          }
        } else {
          *(tp++) = (Bit8u)colour;
        }
      } else {
        tp += tbytes;
      }
    }
  }
}

// Fetch one framebuffer pixel as 0x00RRGGBB
static BX_CPP_INLINE Bit32u rage128_fetch_pixel(const Bit8u *p, unsigned bpp, const bx_rage128_c *dev, unsigned x, bool lsb_nibble, Bit8u *pel8, Bit8u dac_shift)
{
  UNUSED(dev);
  Bit32u v;
  switch (bpp) {
    case 4: {
      Bit8u b = p[0];
      Bit8u idx = ((x & 1) ^ (lsb_nibble ? 1 : 0)) ? (b & 0x0f) : (b >> 4);
      Bit8u *pe = pel8 + idx * 3;
      return ((Bit32u)(pe[0] << dac_shift) << 16) | ((Bit32u)(pe[1] << dac_shift) << 8) | (Bit32u)(pe[2] << dac_shift);
    }
    case 8: {
      Bit8u *pe = pel8 + p[0] * 3;
      return ((Bit32u)(pe[0] << dac_shift) << 16) | ((Bit32u)(pe[1] << dac_shift) << 8) | (Bit32u)(pe[2] << dac_shift);
    }
    case 15:
      v = p[0] | (p[1] << 8);
      return (((v >> 10) & 0x1f) << 19) | (((v >> 10) & 0x1c) << 14) |
             (((v >> 5) & 0x1f) << 11) | (((v >> 5) & 0x1c) << 6) |
             ((v & 0x1f) << 3) | ((v & 0x1c) >> 2);
    case 16:
      v = p[0] | (p[1] << 8);
      return (((v >> 11) & 0x1f) << 19) | (((v >> 11) & 0x1c) << 14) |
             (((v >> 5) & 0x3f) << 10) | (((v >> 5) & 0x30) << 4) |
             ((v & 0x1f) << 3) | ((v & 0x1c) >> 2);
    case 24:
      return p[0] | (p[1] << 8) | (p[2] << 16);
    default:
      return p[0] | (p[1] << 8) | (p[2] << 16);
  }
}

void bx_rage128_c::update(void)
{
  unsigned width, height;

  fold_deferred();
  if (!disp_ext) {
    if (ext_last) {
      ext_last = false;
      gui_xres = gui_yres = gui_bpp = 0;  // the VGA core re-reports its geometry
      BX_RAGE128_THIS s.last_xres = 0;
      BX_RAGE128_THIS s.last_yres = 0;
      BX_RAGE128_THIS s.vga_mem_updated |= 1;
      bx_vgacore_c::redraw_area(0, 0, 4096, 4096);
    }
    bx_vgacore_c::update();
    return;
  }
  ext_last = true;

  if (needs_update_mode) {
    update_mode();
    unsigned bpp = (disp_bpp == 15) ? 16 : ((disp_bpp == 4) ? 8 : disp_bpp);
    if ((disp_xres != gui_xres) || (disp_yres != gui_yres) || (bpp != gui_bpp)) {
      bx_gui->dimension_update(disp_xres, disp_yres, 0, 0, bpp);
      gui_xres = disp_xres;
      gui_yres = disp_yres;
      gui_bpp = bpp;
    }
    needs_update_mode = false;
    needs_update_dispentire = true;
  }
  width = disp_xres;
  height = disp_yres;

  if (needs_update_dispentire) {
    redraw_area(0, 0, width, height);
    needs_update_dispentire = false;
  }
  if (!needs_update_tile)
    return;
  needs_update_tile = false;

  unsigned xc, yc, xti, yti, w, h;
  int i;
  Bit32u colour;
  Bit8u *tile_ptr, *tile_ptr2;
  bx_svga_tileinfo_t info;
  Bit8u dac_shift = BX_RAGE128_THIS s.dac_shift;
  Bit8u pel8[256 * 3];
  for (i = 0; i < 256; i++) {
    pel8[i * 3 + 0] = BX_RAGE128_THIS s.pel.data[i].red;
    pel8[i * 3 + 1] = BX_RAGE128_THIS s.pel.data[i].green;
    pel8[i * 3 + 2] = BX_RAGE128_THIS s.pel.data[i].blue;
  }
  unsigned pxbytes = (disp_bpp + 7) / 8;
  bool lsb_nibble = (dac_cntl & RAGE128_DAC_4BPP_PIX_ORDER) != 0;
  Bit8u *vram = BX_RAGE128_THIS s.memory;

  if (!bx_gui->graphics_tile_info_common(&info)) {
    BX_PANIC(("cannot get svga tile info"));
    return;
  }
  if (info.snapshot_mode) {
    tile_ptr = bx_gui->get_snapshot_buffer();
    if (tile_ptr == NULL)
      return;
    for (yc = 0; yc < height; yc++) {
      unsigned sy = disp_dblscan ? (yc >> 1) : yc;
      tile_ptr2 = tile_ptr;
      for (xc = 0; xc < width; xc++) {
        Bit32u addr = disp_base + sy * disp_pitch + ((disp_bpp == 4) ? (xc >> 1) : xc * pxbytes);
        if (disp_blank) colour = 0;
        else if (disp_dac_const) colour = disp_dac_const_color;
        else colour = rage128_fetch_pixel(&vram[addr & vram_mask], disp_bpp, this, xc, lsb_nibble, pel8, dac_shift);
        if (!info.is_indexed) {
          colour = MAKE_COLOUR(colour, 24, info.red_shift, info.red_mask,
                               colour, 16, info.green_shift, info.green_mask,
                               colour, 8, info.blue_shift, info.blue_mask);
          if (info.is_little_endian) {
            for (i = 0; i < info.bpp; i += 8) *(tile_ptr2++) = (Bit8u)(colour >> i);
          } else {
            for (i = info.bpp - 8; i > -8; i -= 8) *(tile_ptr2++) = (Bit8u)(colour >> i);
          }
        } else {
          *(tile_ptr2++) = (disp_bpp == 8) ? vram[addr & vram_mask] : (Bit8u)colour;
        }
      }
      tile_ptr += info.pitch;
    }
    draw_overlay(0, 0, &info);
    draw_hardware_cursor(0, 0, &info);
    return;
  }

  if (info.is_indexed && (disp_bpp != 8) && (disp_bpp != 4)) {
    BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays, bpp=%d", disp_bpp));
    return;
  }

  for (yc = 0, yti = 0; yc < height; yc += Y_TILESIZE, yti++) {
    for (xc = 0, xti = 0; xc < width; xc += X_TILESIZE, xti++) {
      if (!GET_TILE_UPDATED(xti, yti))
        continue;
      paint_tile(xc, yc, &info, pel8);
      bx_gui->graphics_tile_get(xc, yc, &w, &h);
      bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
      SET_TILE_UPDATED(BX_RAGE128_THIS, xti, yti, 0);
    }
  }
}

// Render one display tile (framebuffer, then overlay and cursor on top)
// into the GUI tile buffer
void bx_rage128_c::paint_tile(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info, Bit8u *pel8)
{
  unsigned w, h, r, c;
  int i;
  Bit32u colour;
  Bit8u *tile_ptr, *tile_ptr2;
  Bit8u dac_shift = BX_RAGE128_THIS s.dac_shift;
  unsigned pxbytes = (disp_bpp + 7) / 8;
  bool lsb_nibble = (dac_cntl & RAGE128_DAC_4BPP_PIX_ORDER) != 0;
  Bit8u *vram = BX_RAGE128_THIS s.memory;

  tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
  for (r = 0; r < h; r++) {
    unsigned y = yc + r;
    unsigned sy = disp_dblscan ? (y >> 1) : y;
    Bit32u rowaddr = disp_base + sy * disp_pitch;
    tile_ptr2 = tile_ptr;
    for (c = 0; c < w; c++) {
      unsigned x = xc + c;
      Bit32u addr = rowaddr + ((disp_bpp == 4) ? (x >> 1) : x * pxbytes);
      if (disp_blank) colour = 0;
      else if (disp_dac_const) colour = disp_dac_const_color;
      else colour = rage128_fetch_pixel(&vram[addr & vram_mask], disp_bpp, this, x, lsb_nibble, pel8, dac_shift);
      if (info->is_indexed) {
        *(tile_ptr2++) = (disp_bpp == 8) ? vram[addr & vram_mask] : (Bit8u)colour;
      } else {
        colour = MAKE_COLOUR(colour, 24, info->red_shift, info->red_mask,
                             colour, 16, info->green_shift, info->green_mask,
                             colour, 8, info->blue_shift, info->blue_mask);
        if (info->is_little_endian) {
          for (i = 0; i < info->bpp; i += 8) *(tile_ptr2++) = (Bit8u)(colour >> i);
        } else {
          for (i = info->bpp - 8; i > -8; i -= 8) *(tile_ptr2++) = (Bit8u)(colour >> i);
        }
      }
    }
    tile_ptr += info->pitch;
  }
  draw_overlay(xc, yc, info);
  draw_hardware_cursor(xc, yc, info);
}

#if BX_DEBUGGER
void bx_rage128_c::debug_dump(int argc, char **argv)
{
  dbg_printf("ATI Rage 128 PRO (%s, %u MB)\n", is_agp ? "AGP" : "PCI", vram_size >> 20);
  if (disp_ext) {
    dbg_printf("extended mode: %u x %u x %u, pitch %u, base 0x%08x\n", disp_xres, disp_yres, disp_bpp, disp_pitch, disp_base);
    dbg_printf("CRTC_GEN_CNTL=0x%08x CRTC_EXT_CNTL=0x%08x DAC_CNTL=0x%08x\n", crtc_gen_cntl, crtc_ext_cntl, dac_cntl);
    dbg_printf("H_TOTAL_DISP=0x%08x V_TOTAL_DISP=0x%08x dot clock %.2f MHz\n", crtc_h_total_disp, crtc_v_total_disp, dot_clock_hz() / 1e6);
  } else {
    dbg_printf("VGA mode\n");
  }
  dbg_printf("GEN_INT_CNTL=0x%08x GEN_INT_STATUS=0x%08x\n", gen_int_cntl, gen_int_status);
  dbg_printf("PM4: BUFFER_CNTL=0x%08x OFFSET=0x%08x RPTR=0x%08x WPTR=0x%08x GART=0x%08x AGP_BASE=0x%08x\n",
             pm4_buffer_cntl, pm4_buffer_offset, pm4_rptr, pm4_wptr, pci_gart_page, agp_base);
  dbg_printf("2D: GMC=0x%08x DST_OFFSET=0x%08x DST_PITCH=%u SRC_OFFSET=0x%08x SRC_PITCH=%u DP_DATATYPE=0x%08x DP_MIX=0x%08x\n",
             dp_gui_master_cntl, dst_offset, dst_pitch, src_offset, src_pitch, dp_datatype, dp_mix);
  dbg_printf("3D: TEX_CNTL_C=0x%08x Z_OFFSET=0x%08x Z_PITCH=0x%08x MISC=0x%08x SETUP=0x%08x\n",
             t3d.tex_cntl, t3d.z_offset, t3d.z_pitch, t3d.misc_3d_state_cntl, t3d.setup_cntl);

  // "info device rage128 vram <file>": write the whole VRAM image to a file
  if ((argc >= 2) && !strcmp(argv[0], "vram")) {
    FILE *fp = fopen(argv[1], "wb");
    if (fp == NULL) {
      dbg_printf("cannot create '%s'\n", argv[1]);
    } else {
      size_t n = fwrite(BX_RAGE128_THIS s.memory, 1, vram_size, fp);
      fclose(fp);
      dbg_printf("wrote %u bytes of VRAM to '%s'\n", (unsigned)n, argv[1]);
    }
    return;
  }

  // "info device rage128 fifo,<file>": the CCE command FIFO (the last
  // RAGE128_CCE_FIFO_DWORDS dwords the executor was handed, ring order),
  // followed by the tag bytes; the read/write indices are printed.
  if ((argc >= 2) && !strcmp(argv[0], "fifo")) {
    FILE *fp = fopen(argv[1], "wb");
    if ((fp == NULL) || (cce_fifo == NULL)) {
      dbg_printf("cannot create '%s' or no FIFO\n", argv[1]);
      if (fp) fclose(fp);
    } else {
      fwrite(cce_fifo, sizeof(Bit32u), RAGE128_CCE_FIFO_DWORDS, fp);
      fwrite(cce_fifo_tag, 1, RAGE128_CCE_FIFO_DWORDS, fp);
      fclose(fp);
      dbg_printf("wrote FIFO (%u dwords + tags) to '%s': rd=%u wr=%u retire=0x%08x\n",
                 RAGE128_CCE_FIFO_DWORDS, argv[1], cce_fifo_rd, cce_fifo_wr, cce_retire_rptr);
    }
    return;
  }

  // "info device rage128 3d": the complete engine/texture context
  if ((argc >= 1) && !strcmp(argv[0], "3d")) {
    dbg_printf("scanout: CRTC_OFFSET=0x%08x latched=0x%08x pending=%d CRTC_PITCH=0x%08x\n",
               crtc_offset, crtc_offset_latched, crtc_offset_pending, crtc_pitch);
    dbg_printf("2D: DEFAULT_OFFSET=0x%08x DEFAULT_PITCH=0x%08x DST_PITCH_REG=0x%08x SC_TL=0x%08x SC_BR=0x%08x\n",
               default_offset, default_pitch, dst_pitch_reg, sc_top_left, sc_bottom_right);
    dbg_printf("2D: CLR_CMP_CNTL=0x%08x CLR_SRC=0x%08x MASK=0x%08x WRITE_MASK=0x%08x AUX_SC=0x%08x\n",
               clr_cmp_cntl, clr_cmp_clr_src, clr_cmp_mask, dp_write_mask, aux_sc_cntl);
    dbg_printf("PM4: RPTR_ADDR=0x%08x RETIRE=0x%08x INDOFF=0x%08x INDSIZE=0x%08x MICRO_CNTL=0x%08x VC_DEBUG=0x%08x\n",
               pm4_rptr_addr, cce_retire_rptr, pm4_iw_indoff, pm4_iw_indsize, pm4_micro_cntl, pm4_vc_debug_config);
    dbg_printf("PM4: fifo rd=%u wr=%u executing=%d ind_busy=%d batch_pending=%d\n",
               cce_fifo_rd, cce_fifo_wr, (int)cce_executing, (int)pm4_ind_busy, (int)cce_batch_pending);
    dbg_printf("3D: FPU_SETUP=0x%08x SETUP_CNTL_PM4=0x%08x WINDOW_XY=0x%08x Z_STEN=0x%08x STEN_REF=0x%08x PLANE_MASK=0x%08x\n",
               t3d.fpu_setup, t3d.setup_cntl_pm4, t3d.window_xy_offset, t3d.z_sten_cntl, t3d.sten_ref_mask, t3d.plane_3d_mask);
    dbg_printf("3D: PRIM_TEX_CNTL=0x%08x PRIM_COMBINE=0x%08x TEX_SIZE_PITCH=0x%08x SEC_TEX_CNTL=0x%08x SEC_COMBINE=0x%08x\n",
               t3d.prim_tex_cntl, t3d.prim_tex_combine_cntl, t3d.tex_size_pitch, t3d.sec_tex_cntl, t3d.sec_tex_combine_cntl);
    dbg_printf("3D: SCALE_3D_CNTL=0x%08x CONST_COLOR=0x%08x FOG=0x%08x CLR_CMP_3D=0x%08x/0x%08x TEX_CK=0x%08x/0x%08x\n",
               t3d.scale_3d_cntl, t3d.constant_color, t3d.fog_color, t3d.clr_cmp_clr_3d, t3d.clr_cmp_msk_3d,
               t3d.tex_clr_cmp_clr, t3d.tex_clr_cmp_msk);
    dbg_printf("3D: PRIM_TEX_OFFSET[0..10]:");
    for (int i = 0; i < 11; i++) dbg_printf(" %08x", t3d.prim_tex_offset[i]);
    dbg_printf("\n3D: SEC_TEX_OFFSET[0..10]: ");
    for (int i = 0; i < 11; i++) dbg_printf(" %08x", t3d.sec_tex_offset[i]);
    dbg_printf("\n");
    dbg_printf("OV0: enabled=%d SCALE_CNTL=0x%08x Y_X_START=0x%08x Y_X_END=0x%08x LOAD_CNTL=0x%08x\n",
               (int)ov0_enabled, ov0.active[RAGE128_OV0_REG(RAGE128_OV0_SCALE_CNTL)],
               ov0.active[RAGE128_OV0_REG(RAGE128_OV0_Y_X_START)],
               ov0.active[RAGE128_OV0_REG(RAGE128_OV0_Y_X_END)], ov0_reg_load_cntl);
    dbg_printf("cursor: latched en=%d offset=0x%08x posn=0x%08x hvoff=0x%08x lock=%d\n",
               (int)cur_lat_en, cur_lat_offset, cur_lat_posn, cur_lat_hvoff, (int)cur_lock);
    dbg_printf("surfaces:");
    for (int n = 0; n < 4; n++)
      dbg_printf(" [%d: lo=%08x hi=%08x info=%08x]", n, surf_lower[n], surf_upper[n], surf_info[n]);
    dbg_printf(" xlate=%d\n", (int)surf_xlate_on);
    return;
  }
  if (argc > 0) {
    bx_vgacore_c::debug_dump(argc, argv);
  }
}
#endif

#endif // BX_SUPPORT_PCI && BX_SUPPORT_RAGE128
