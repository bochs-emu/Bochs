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

// PCI 0x00: vendor, 0x02: device
#define PCI_VENDOR_NVIDIA             0x10DE
#define PCI_DEVICE_GEFORCE3TI500      0x0202
// PCI 0x04: command(word), 0x06(word): status
#define PCI_COMMAND_IOACCESS                0x0001
#define PCI_COMMAND_MEMACCESS               0x0002
#define PCI_COMMAND_BUSMASTER               0x0004
#define PCI_COMMAND_SPECIALCYCLE            0x0008
#define PCI_COMMAND_MEMWRITEINVALID         0x0010
#define PCI_COMMAND_PALETTESNOOPING         0x0020
#define PCI_COMMAND_PARITYDETECTION         0x0040
#define PCI_COMMAND_ADDRESSDATASTEPPING     0x0080
#define PCI_COMMAND_SERR                    0x0100
#define PCI_COMMAND_BACKTOBACKTRANS         0x0200
// PCI 0x08, 0xff000000 (0x09-0x0b:class,0x08:rev)
#define PCI_CLASS_BASE_DISPLAY        0x03
// PCI 0x08, 0x00ff0000
#define PCI_CLASS_SUB_VGA             0x00
// PCI 0x0c, 0x00ff0000 (0x0c:cacheline,0x0d:latency,0x0e:headertype,0x0f:Built-in self test)
#define PCI_CLASS_HEADERTYPE_00h  0x00
// 0x10-0x3f (headertype 00h)
// PCI 0x10,0x14,0x18,0x1c,0x20,0x24: base address mapping registers
//   0x10: MEMBASE, 0x14: IOBASE(hard-coded in XFree86 3.x)
#define PCI_MAP_MEM                 0x0
#define PCI_MAP_IO                  0x1
#define PCI_MAP_MEM_ADDR_MASK       (~0xf)
#define PCI_MAP_IO_ADDR_MASK        (~0x3)
#define PCI_MAP_MEMFLAGS_32BIT      0x0
#define PCI_MAP_MEMFLAGS_32BIT_1M   0x1
#define PCI_MAP_MEMFLAGS_64BIT      0x4
#define PCI_MAP_MEMFLAGS_CACHEABLE  0x8
// PCI 0x28: cardbus CIS pointer
// PCI 0x2c: subsystem vendor id, 0x2e: subsystem id
// PCI 0x30: expansion ROM base address
// PCI 0x34: 0xffffff00=reserved, 0x000000ff=capabilities pointer
// PCI 0x38: reserved
// PCI 0x3c: 0x3c=int-line, 0x3d=int-pin, 0x3e=min-gnt, 0x3f=maax-lat
// PCI 0x40-0xff: device dependent fields

// default PnP memory and memory-mapped I/O sizes
#define GEFORCE_PNPMMIO_SIZE        0x1000000
#define GEFORCE_BAR2_SIZE           0x80000

#define GEFORCE_VIDEO_MEMORY_MB     64

#define GEFORCE_VIDEO_MEMORY_KB     (GEFORCE_VIDEO_MEMORY_MB * 1024)
#define GEFORCE_VIDEO_MEMORY_BYTES  (GEFORCE_VIDEO_MEMORY_KB * 1024)

#define RAMIN_OFFSET                (GEFORCE_VIDEO_MEMORY_BYTES - 0x100000)


#define STRAPS0_PRIMARY             (0xFFF86C6B | 0x00000180) // disable TV out

static bx_geforce_c *theSvga = NULL;

PLUGIN_ENTRY_FOR_MODULE(geforce)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_geforce_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_GEFORCE);
  } else if (mode == PLUGIN_FINI) {
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
  const char* model = "GeForce 3 Ti 500";
  if (!SIM->is_agp_device(BX_PLUGIN_GEFORCE)) {
    BX_PANIC(("%s should be plugged into AGP slot", model));
  }

  BX_GEFORCE_THIS pci_enabled = true;

  BX_GEFORCE_THIS put("GEFORCE");
  // initialize SVGA stuffs.
  DEV_register_ioread_handler(this, svga_read_handler, 0x03B4, "geforce", 2);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D0, "geforce", 6);
  DEV_register_ioread_handler(this, svga_read_handler, 0x03D2, "geforce", 2);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D0, "geforce", 6);
  DEV_register_iowrite_handler(this, svga_write_handler, 0x03D2, "geforce", 2);
  BX_GEFORCE_THIS init_iohandlers(svga_read_handler, svga_write_handler);
  BX_GEFORCE_THIS svga_init_members();
  BX_GEFORCE_THIS svga_init_pcihandlers();
  BX_INFO(("%s initialized", model));
  BX_GEFORCE_THIS s.max_xres = 1600;
  BX_GEFORCE_THIS s.max_yres = 1200;
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("geforce", this);
#endif
  return 1;
}

void bx_geforce_c::svga_init_members()
{
  unsigned i;

  // clear all registers.
  BX_GEFORCE_THIS sequencer.index = CIRRUS_SEQUENCER_MAX + 1;
  for (i = 0; i <= CIRRUS_SEQUENCER_MAX; i++)
    BX_GEFORCE_THIS sequencer.reg[i] = 0x00;
  BX_GEFORCE_THIS control.index = CIRRUS_CONTROL_MAX + 1;
  for (i = 0; i <= CIRRUS_CONTROL_MAX; i++)
    BX_GEFORCE_THIS control.reg[i] = 0x00;
  BX_GEFORCE_THIS control.shadow_reg0 = 0x00;
  BX_GEFORCE_THIS control.shadow_reg1 = 0x00;
  BX_GEFORCE_THIS crtc.index = GEFORCE_CRTC_MAX + 1;
  for (i = 0; i <= GEFORCE_CRTC_MAX; i++)
    BX_GEFORCE_THIS crtc.reg[i] = 0x00;
  BX_GEFORCE_THIS hidden_dac.lockindex = 0;
  BX_GEFORCE_THIS hidden_dac.data = 0x00;

  BX_GEFORCE_THIS mc_intr = 0x00000000;
  BX_GEFORCE_THIS mc_intr_en = 0x00000000;
  BX_GEFORCE_THIS mc_enable = 0x00000000;
  BX_GEFORCE_THIS bus_debug_1 = 0x00000000;
  for (i = 0; i < 0x40; i++)
    BX_GEFORCE_THIS bus_12xx[i] = 0x00000000;
  BX_GEFORCE_THIS fifo_intr = 0x00000000;
  BX_GEFORCE_THIS fifo_intr_en = 0x00000000;
  BX_GEFORCE_THIS rma_addr = 0x00000000;
  BX_GEFORCE_THIS timer_intr = 0x00000000;
  BX_GEFORCE_THIS timer_intr_en = 0x00000000;
  BX_GEFORCE_THIS timer_num = 0x00000000;
  BX_GEFORCE_THIS timer_den = 0x00000000;
  BX_GEFORCE_THIS timer_time = 0x0000000000000000;
  BX_GEFORCE_THIS timer_alarm = 0x00000000;
  BX_GEFORCE_THIS fb_debug_0 = 0x00000000;
  BX_GEFORCE_THIS fb_cfg0 = 0x00000000;
  BX_GEFORCE_THIS fb_cfg1 = 0x00000000;
  BX_GEFORCE_THIS straps0_primary = STRAPS0_PRIMARY;
  BX_GEFORCE_THIS graph_intr = 0x00000000;
  BX_GEFORCE_THIS graph_intr_en = 0x00000000;
  BX_GEFORCE_THIS graph_status = 0x00000000;
  BX_GEFORCE_THIS graph_fifo = 0x00000000;
  BX_GEFORCE_THIS graph_channel_ctx_table = 0x00000000;
  BX_GEFORCE_THIS crtc_intr_en = 0x00000000;
  BX_GEFORCE_THIS crtc_start = 0x00000000;
  BX_GEFORCE_THIS crtc_config = 0x00000000;
  BX_GEFORCE_THIS crtc_cursor_config = 0x00000000;
  BX_GEFORCE_THIS crtc_gpio = 0x00000000;
  BX_GEFORCE_THIS ramdac_cu_start_pos = 0x00000000;
  BX_GEFORCE_THIS nvpll = 0x00000000;
  BX_GEFORCE_THIS mpll = 0x00000000;
  BX_GEFORCE_THIS vpll = 0x00000000;
  BX_GEFORCE_THIS pll_control = 0x00000000;
  BX_GEFORCE_THIS general_control = 0x00000000;
  BX_GEFORCE_THIS test_control = 0x00000000;
  BX_GEFORCE_THIS ramdac_fp_hcrtc = 0x00000000;
  BX_GEFORCE_THIS ramdac_fp_tg_control = 0x00000000;

  for (i = 0; i < 4*1024*1024; i++)
    BX_GEFORCE_THIS unk_regs[i] = 0x00000000;

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
  BX_GEFORCE_THIS hw_cursor.offset = 0x00000000;
  BX_GEFORCE_THIS hw_cursor.enabled = false;

  // memory allocation.
  if (BX_GEFORCE_THIS s.memory == NULL)
    BX_GEFORCE_THIS s.memory = new Bit8u[GEFORCE_VIDEO_MEMORY_BYTES];

  BX_GEFORCE_THIS s.memsize = GEFORCE_VIDEO_MEMORY_BYTES;

  BX_GEFORCE_THIS hidden_dac.lockindex = 5;
  BX_GEFORCE_THIS hidden_dac.data = 0;

  memset(BX_GEFORCE_THIS s.memory, 0x00, GEFORCE_VIDEO_MEMORY_BYTES);
  BX_GEFORCE_THIS disp_ptr = BX_GEFORCE_THIS s.memory;
  BX_GEFORCE_THIS memsize_mask = BX_GEFORCE_THIS s.memsize - 1;

  // VCLK defaults after reset
  BX_GEFORCE_THIS sequencer.reg[0x0b] = 0x66;
  BX_GEFORCE_THIS sequencer.reg[0x0c] = 0x5b;
  BX_GEFORCE_THIS sequencer.reg[0x0d] = 0x45;
  BX_GEFORCE_THIS sequencer.reg[0x0e] = 0x7e;
  BX_GEFORCE_THIS sequencer.reg[0x1b] = 0x3b;
  BX_GEFORCE_THIS sequencer.reg[0x1c] = 0x2f;
  BX_GEFORCE_THIS sequencer.reg[0x1d] = 0x30;
  BX_GEFORCE_THIS sequencer.reg[0x1e] = 0x33;
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
}

void bx_geforce_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "geforce", "GeForce State");
  BX_GEFORCE_THIS vgacore_register_state(list);
  bx_list_c *crtc = new bx_list_c(list, "crtc");
  new bx_shadow_num_c(crtc, "index", &BX_GEFORCE_THIS crtc.index, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_GEFORCE_THIS crtc.reg, GEFORCE_CRTC_MAX + 1, 1);
  bx_list_c *sequ = new bx_list_c(list, "sequencer");
  new bx_shadow_num_c(sequ, "index", &BX_GEFORCE_THIS sequencer.index, BASE_HEX);
  new bx_shadow_data_c(sequ, "reg", BX_GEFORCE_THIS sequencer.reg, CIRRUS_SEQUENCER_MAX + 1, 1);
  bx_list_c *ctrl = new bx_list_c(list, "control");
  new bx_shadow_num_c(ctrl, "index", &BX_GEFORCE_THIS control.index, BASE_HEX);
  new bx_shadow_data_c(ctrl, "reg", BX_GEFORCE_THIS control.reg, CIRRUS_CONTROL_MAX + 1, 1);
  new bx_shadow_num_c(ctrl, "shadow_reg0", &BX_GEFORCE_THIS control.shadow_reg0, BASE_HEX);
  new bx_shadow_num_c(ctrl, "shadow_reg1", &BX_GEFORCE_THIS control.shadow_reg1, BASE_HEX);
  bx_list_c *hdac = new bx_list_c(list, "hidden_dac");
  new bx_shadow_num_c(hdac, "lockindex", &BX_GEFORCE_THIS hidden_dac.lockindex, BASE_HEX);
  new bx_shadow_num_c(hdac, "data", &BX_GEFORCE_THIS hidden_dac.data, BASE_HEX);
  new bx_shadow_data_c(hdac, "palette", BX_GEFORCE_THIS hidden_dac.palette, 48, 1);
  BXRS_PARAM_BOOL(list, svga_unlock_special, BX_GEFORCE_THIS svga_unlock_special);
  BXRS_PARAM_BOOL(list, svga_double_width, BX_GEFORCE_THIS svga_double_width);
  new bx_shadow_num_c(list, "svga_xres", &BX_GEFORCE_THIS svga_xres);
  new bx_shadow_num_c(list, "svga_yres", &BX_GEFORCE_THIS svga_yres);
  new bx_shadow_num_c(list, "svga_pitch", &BX_GEFORCE_THIS svga_pitch);
  new bx_shadow_num_c(list, "svga_bpp", &BX_GEFORCE_THIS svga_bpp);
  new bx_shadow_num_c(list, "svga_dispbpp", &BX_GEFORCE_THIS svga_dispbpp);
  new bx_shadow_num_c(list, "bank_base0", &BX_GEFORCE_THIS bank_base[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_base1", &BX_GEFORCE_THIS bank_base[1], BASE_HEX);
  new bx_shadow_num_c(list, "ext_latch0", &BX_GEFORCE_THIS ext_latch[0], BASE_HEX);
  new bx_shadow_num_c(list, "ext_latch1", &BX_GEFORCE_THIS ext_latch[1], BASE_HEX);
  new bx_shadow_num_c(list, "ext_latch2", &BX_GEFORCE_THIS ext_latch[2], BASE_HEX);
  new bx_shadow_num_c(list, "ext_latch3", &BX_GEFORCE_THIS ext_latch[3], BASE_HEX);
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

void bx_geforce_c::redraw_area(Bit32s x0, Bit32s y0, Bit32u width, Bit32u height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1;

  if (!BX_GEFORCE_THIS crtc.reg[0x28]) {
    BX_GEFORCE_THIS bx_vgacore_c::redraw_area(x0, y0, width, height);
    return;
  }

  if (BX_GEFORCE_THIS svga_needs_update_mode) {
    return;
  }

  BX_GEFORCE_THIS svga_needs_update_tile = 1;

  xt0 = x0 <= 0 ? 0 : x0 / X_TILESIZE;
  yt0 = y0 <= 0 ? 0 : y0 / Y_TILESIZE;
  if (x0 < BX_GEFORCE_THIS svga_xres) {
    xt1 = (x0 + width - 1) / X_TILESIZE;
  } else {
    xt1 = (BX_GEFORCE_THIS svga_xres - 1) / X_TILESIZE;
  }
  if (y0 < BX_GEFORCE_THIS svga_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (BX_GEFORCE_THIS svga_yres - 1) / Y_TILESIZE;
  }
  if ((x0 + width) > svga_xres) {
    BX_GEFORCE_THIS redraw_area(0, y0 + 1, x0 + width - svga_xres, height);
  }
  for (yti=yt0; yti<=yt1; yti++) {
    for (xti=xt0; xti<=xt1; xti++) {
      SET_TILE_UPDATED(BX_GEFORCE_THIS, xti, yti, 1);
    }
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
      BX_ERROR(("MMIO read from 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
    } else if (len == 4) {
      Bit32u value = register_read32(offset);
      BX_ERROR(("MMIO read from 0x%08x, value 0x%08x", offset, value));
      *((Bit8u*)data + 0) = (value >> 0) & 0xFF;
      *((Bit8u*)data + 1) = (value >> 8) & 0xFF;
      *((Bit8u*)data + 2) = (value >> 16) & 0xFF;
      *((Bit8u*)data + 3) = (value >> 24) & 0xFF;
    } else {
      BX_PANIC(("MMIO read len %d", len));
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
          return BX_GEFORCE_THIS s.memory[(addr & mask) + RAMIN_OFFSET];
      } else {
        return 0xff;
      }
    }
  }

  if ((addr >= BX_GEFORCE_THIS pci_bar[1].addr) &&
      (addr < (BX_GEFORCE_THIS pci_bar[1].addr + GEFORCE_VIDEO_MEMORY_BYTES))) {
    Bit32u offset = addr & BX_GEFORCE_THIS memsize_mask;
    return BX_GEFORCE_THIS s.memory[offset];
  }

  if ((addr >= BX_GEFORCE_THIS pci_bar[2].addr) &&
      (addr < (BX_GEFORCE_THIS pci_bar[2].addr + GEFORCE_BAR2_SIZE))) {
    Bit32u offset = addr & (GEFORCE_BAR2_SIZE - 1);
    BX_PANIC(("BAR2 read from 0x%08x", offset));
    return 0xFF;
  }

  if (!BX_GEFORCE_THIS crtc.reg[0x28])
    return BX_GEFORCE_THIS bx_vgacore_c::mem_read(addr);

  return 0xFF;
}

bool bx_geforce_c::geforce_mem_write_handler(bx_phy_address addr, unsigned len,
                                         void *data, void *param)
{
  if (addr >= BX_GEFORCE_THIS pci_bar[0].addr &&
      addr < (BX_GEFORCE_THIS pci_bar[0].addr + GEFORCE_PNPMMIO_SIZE)) {
    Bit32u offset = addr & (GEFORCE_PNPMMIO_SIZE - 1);
    if (len == 1) {
      BX_ERROR(("MMIO write to 0x%08x, value 0x%02x", offset, *(Bit8u*)data));
      register_write8(offset, *(Bit8u*)data);
    } else if (len == 4) {
      Bit32u value =
        (*((Bit8u*)data + 0) << 0) |
        (*((Bit8u*)data + 1) << 8) |
        (*((Bit8u*)data + 2) << 16) |
        (*((Bit8u*)data + 3) << 24);
      BX_ERROR(("MMIO write to 0x%08x, value 0x%08x", offset, value));
      register_write32(offset, value);
    } else {
      BX_PANIC(("MMIO write len %d", len));
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
      (addr < (BX_GEFORCE_THIS pci_bar[1].addr + GEFORCE_VIDEO_MEMORY_BYTES))) {
    unsigned x, y;
    Bit32u offset = addr & BX_GEFORCE_THIS memsize_mask;
    BX_GEFORCE_THIS s.memory[offset] = value;
    BX_GEFORCE_THIS svga_needs_update_tile = 1;
    offset -= (Bit32u)(BX_GEFORCE_THIS disp_ptr - BX_GEFORCE_THIS s.memory);
    x = (offset % BX_GEFORCE_THIS svga_pitch) / (BX_GEFORCE_THIS svga_bpp / 8);
    y = offset / BX_GEFORCE_THIS svga_pitch;
    SET_TILE_UPDATED(BX_GEFORCE_THIS, x / X_TILESIZE, y / Y_TILESIZE, 1);
    return;
  }

  if ((addr >= BX_GEFORCE_THIS pci_bar[2].addr) &&
      (addr < (BX_GEFORCE_THIS pci_bar[2].addr + GEFORCE_BAR2_SIZE))) {
    Bit32u offset = addr & (GEFORCE_BAR2_SIZE - 1);
    BX_PANIC(("BAR2 write to 0x%08x, value = 0x%02x", offset, value));
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
      BX_GEFORCE_THIS s.memory[offset + RAMIN_OFFSET] = value;
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

  if (address == 0x03d0 || address == 0x03d2) // RMA_ACCESS
  {
      Bit8u crtc38 = BX_GEFORCE_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable)
          BX_PANIC(("port 0x3d0 access is disabled"));
      int rma_index = crtc38 >> 1;
      if (rma_index == 1) {
        if (address == 0x03d0) {
          if (io_len == 2)
            BX_DEBUG(("rma: read address 0x%08x (lo)", BX_GEFORCE_THIS rma_addr));
          else
            BX_DEBUG(("rma: read address 0x%08x", BX_GEFORCE_THIS rma_addr));
          return BX_GEFORCE_THIS rma_addr;
        } else {
          BX_DEBUG(("rma: read address 0x%08x (hi)", BX_GEFORCE_THIS rma_addr));
          return BX_GEFORCE_THIS rma_addr >> 16;
        }
      }
      else if (rma_index == 2) {
        if (BX_GEFORCE_THIS rma_addr < 0x1000000) {
          Bit32u value = register_read32(BX_GEFORCE_THIS rma_addr);
          if (address == 0x03d0) {
            if (io_len == 2)
              BX_ERROR(("rma: read from 0x%08x value 0x????%04x", BX_GEFORCE_THIS rma_addr, value & 0xFFFF));
            else
              BX_ERROR(("rma: read from 0x%08x value 0x%08x", BX_GEFORCE_THIS rma_addr, value));
            return value;
          } else {
            BX_ERROR(("rma: read from 0x%08x value 0x%04x????", BX_GEFORCE_THIS rma_addr, value >> 16));
            return value >> 16;
          }
        } else {
          BX_ERROR(("rma: oob read from 0x%08x ignored", BX_GEFORCE_THIS rma_addr));
          return 0x00000000;
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
      return 0x10; // Monitor presence detection (DAC Sensing)
    case 0x03c4: /* VGA: Sequencer Index Register */
      return BX_GEFORCE_THIS sequencer.index;
    case 0x03c5: /* VGA: Sequencer Registers */
      if (BX_GEFORCE_THIS sequencer.index > VGA_SEQENCER_MAX)
        BX_PANIC(("sequencer: unknown index 0x%02x read", BX_GEFORCE_THIS sequencer.index));
      break;
    case 0x03c6: /* Hidden DAC */
      break;
    case 0x03c8: /* PEL write address */
      BX_GEFORCE_THIS hidden_dac.lockindex = 0;
      break;
    case 0x03c9: /* PEL Data Register, hiddem pel colors 00..0F */
      break;
    case 0x03ce: /* VGA: Graphics Controller Index Register */
      return BX_GEFORCE_THIS control.index;
    case 0x03cf: /* VGA: Graphics Controller Registers */
      if (BX_GEFORCE_THIS control.index > VGA_CONTROL_MAX)
        BX_PANIC(("control: unknown index 0x%02x read", BX_GEFORCE_THIS control.index));
      break;
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
      Bit8u crtc38 = BX_GEFORCE_THIS crtc.reg[0x38];
      bool rma_enable = crtc38 & 1;
      if (!rma_enable)
        BX_PANIC(("port 0x3d0 access is disabled"));
      int rma_index = crtc38 >> 1;
      if (rma_index == 1) {
        if (address == 0x03d0) {
          if (io_len == 2) {
            BX_GEFORCE_THIS rma_addr &= 0xFFFF0000;
            BX_GEFORCE_THIS rma_addr |= value;
            BX_DEBUG(("rma: address set to 0x%08x (lo)", BX_GEFORCE_THIS rma_addr));
          } else {
            BX_GEFORCE_THIS rma_addr = value;
            BX_DEBUG(("rma: address set to 0x%08x", BX_GEFORCE_THIS rma_addr));
          }
        } else {
          BX_GEFORCE_THIS rma_addr &= 0x0000FFFF;
          BX_GEFORCE_THIS rma_addr |= value << 16;
          BX_DEBUG(("rma: address set to 0x%08x (hi)", BX_GEFORCE_THIS rma_addr));
        }
      } else if (rma_index == 2) {
        BX_PANIC(("rma: write index 2"));
      } else if (rma_index == 3) {
        if (BX_GEFORCE_THIS rma_addr < 0x1000000) {
          if (address == 0x03d0) {
            if (io_len == 2) {
              Bit32u value32 = register_read32(BX_GEFORCE_THIS rma_addr);
              value32 &= 0xFFFF0000;
              value32 |= value;
              register_write32(BX_GEFORCE_THIS rma_addr, value32);
              BX_ERROR(("rma: write to 0x%08x value 0x????%04x", BX_GEFORCE_THIS rma_addr, value));
            } else {
              register_write32(BX_GEFORCE_THIS rma_addr, value);
              BX_ERROR(("rma: write to 0x%08x value 0x%08x", BX_GEFORCE_THIS rma_addr, value));
            }
          } else {
            Bit32u value32 = register_read32(BX_GEFORCE_THIS rma_addr);
            value32 &= 0x0000FFFF;
            value32 |= value << 16;
            register_write32(BX_GEFORCE_THIS rma_addr, value32);
            BX_ERROR(("rma: write to 0x%08x value 0x%04x????", BX_GEFORCE_THIS rma_addr, value));
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
          BX_GEFORCE_THIS crtc.index == 0x28 ||
          BX_GEFORCE_THIS crtc.index == 0x42) {
        BX_GEFORCE_THIS svga_needs_update_mode = 1;
      }
      if (BX_GEFORCE_THIS crtc.index <= VGA_CRTC_MAX) {
        BX_GEFORCE_THIS crtc.reg[BX_GEFORCE_THIS crtc.index] = value;
      } else {
        BX_GEFORCE_THIS svga_write_crtc(address, BX_GEFORCE_THIS crtc.index, value);
        return;
      }
      break;
    case 0x03c4: /* VGA: Sequencer Index Register */
      BX_GEFORCE_THIS sequencer.index = value;
      break;
    case 0x03c5: /* VGA: Sequencer Registers */
      if (BX_GEFORCE_THIS sequencer.index > VGA_SEQENCER_MAX)
        BX_PANIC(("sequencer: unknown index 0x%02x write", BX_GEFORCE_THIS sequencer.index));
      break;
    case 0x03c6: /* Hidden DAC */
      break;
    case 0x03c9: /* PEL Data Register, hidden pel colors 00..0F */
      break;
    case 0x03ce: /* VGA: Graphics Controller Index Register */
      BX_GEFORCE_THIS control.index = value;
      break;
    case 0x03cf: /* VGA: Graphics Controller Registers */
      if (BX_GEFORCE_THIS control.index > VGA_CONTROL_MAX)
        BX_PANIC(("control: unknown index 0x%02x write", BX_GEFORCE_THIS control.index));
      break;
    default:
      break;
  }

  VGA_WRITE(address,value,io_len);
}

void bx_geforce_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  Bit16s hwcx = BX_GEFORCE_THIS hw_cursor.x;
  Bit16s hwcy = BX_GEFORCE_THIS hw_cursor.y;
  Bit8u size = BX_GEFORCE_THIS hw_cursor.size;

  if (BX_GEFORCE_THIS hw_cursor.enabled &&
      (int)xc < hwcx + size &&
      (int)xc + X_TILESIZE > hwcx &&
      (int)yc < hwcy + size &&
      (int)yc + Y_TILESIZE > hwcy) {
    unsigned cx0 = hwcx > (int)xc ? hwcx : xc;
    unsigned cy0 = hwcy > (int)yc ? hwcy : yc;
    unsigned cx1 = hwcx + size < (int)xc + X_TILESIZE ? hwcx + size : xc + X_TILESIZE;
    unsigned cy1 = hwcy + size < (int)yc + Y_TILESIZE ? hwcy + size : yc + Y_TILESIZE;

    unsigned w, h;
    if (info->bpp == 15) info->bpp = 16;
    Bit8u* tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h) +
               info->pitch * (cy0 - yc) + info->bpp / 8 * (cx0 - xc);
    unsigned pitch = BX_GEFORCE_THIS hw_cursor.size * 4;
    Bit8u* cursor_ptr = BX_GEFORCE_THIS s.memory +
      BX_GEFORCE_THIS hw_cursor.offset + pitch * (cy0 - hwcy);
    Bit8u* vid_ptr = BX_GEFORCE_THIS disp_ptr + BX_GEFORCE_THIS svga_pitch * cy0;
    for (unsigned cy = cy0; cy < cy1; cy++) {
      Bit8u* tile_ptr2 = tile_ptr;
      Bit8u* cursor_ptr2 = cursor_ptr + 4 * (cx0 - hwcx);
      Bit8u* vid_ptr2 = vid_ptr + 4 * cx0;
      for (unsigned cx = cx0; cx < cx1; cx++) {
        Bit8u alpha = cursor_ptr2[3];
        Bit32u color;
        if (alpha) {
          Bit8u b = (vid_ptr2[0] * (0xFF - alpha) >> 8) + cursor_ptr2[0];
          Bit8u g = (vid_ptr2[1] * (0xFF - alpha) >> 8) + cursor_ptr2[1];
          Bit8u r = (vid_ptr2[2] * (0xFF - alpha) >> 8) + cursor_ptr2[2];
          color = b << 0 | g << 8 | r << 16;
        } else {
          color = vid_ptr2[0] << 0 | vid_ptr2[1] << 8 | vid_ptr2[2] << 16;
        }
        if (info->is_little_endian) {
          for (int i = 0; i < info->bpp; i += 8) {
            *(tile_ptr2++) = (Bit8u)(color >> i);
          }
        } else {
          for (int i = info->bpp - 8; i > -8; i -= 8) {
            *(tile_ptr2++) = (Bit8u)(color >> i);
          }
        }
        cursor_ptr2 += 4;
        vid_ptr2 += 4;
      }
      tile_ptr += info->pitch;
      cursor_ptr += pitch;
      vid_ptr += BX_GEFORCE_THIS svga_pitch;
    }
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

    Bit8u iBpp;
    if (crtc28 == 0x01)
      iBpp = 8;
    else if (crtc28 == 0x02)
      iBpp = 16;
    else if (crtc28 == 0x03)
      iBpp = 32;
    else
      BX_PANIC(("unknown bpp"));

    Bit32u iHeight, iWidth;
    determine_screen_dimensions(&iHeight, &iWidth);

    if ((iWidth != BX_GEFORCE_THIS svga_xres) || (iHeight != BX_GEFORCE_THIS svga_yres)
        || (iBpp != BX_GEFORCE_THIS svga_bpp)) {
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
    BX_GEFORCE_THIS redraw_area(0,0,width,height);
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

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = BX_GEFORCE_THIS disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < height; yc++) {
          if (BX_GEFORCE_THIS svga_dispbpp != 4) {
            memcpy(tile_ptr, vid_ptr, info.pitch);
            vid_ptr += pitch;
          } else {
            tile_ptr2 = tile_ptr;
            row_addr = BX_GEFORCE_THIS s.CRTC.start_addr + (yc * pitch);
            for (xc = 0; xc < width; xc++) {
              *(tile_ptr2++) = BX_GEFORCE_THIS get_vga_pixel(xc, yc, row_addr, 0xffff, 0, BX_GEFORCE_THIS s.memory);
            }
          }
          tile_ptr += info.pitch;
        }
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
  if (index == 0x36)
    BX_GEFORCE_THIS crtc.reg[index] = BX_GEFORCE_THIS ddc.read();
  else if (index == 0x3e)
    BX_GEFORCE_THIS crtc.reg[index] = 0xFF;

  if (index <= GEFORCE_CRTC_MAX) {
    Bit8u value = BX_GEFORCE_THIS crtc.reg[index];
    BX_ERROR(("crtc: index 0x%02x read 0x%02x", index, value));
    return value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x read", index));

  return 0xff;
}

void bx_geforce_c::svga_write_crtc(Bit32u address, unsigned index, Bit8u value)
{
  BX_ERROR(("crtc: index 0x%02x write 0x%02x", index, (unsigned)value));

  bool update_cursor_addr = false;

  if (index == 0x1d || index == 0x1e)
    BX_GEFORCE_THIS bank_base[index - 0x1d] = value * 0x8000;
  else if (index == 0x2f || index == 0x30 || index == 0x31)
    update_cursor_addr = true;
  else if (index == 0x37)
    BX_GEFORCE_THIS ddc.write((value >> 5) & 1, (value >> 4) & 1);
  else if (index == 0x3f)
    value = 0xFF;

  if (index <= GEFORCE_CRTC_MAX) {
    BX_GEFORCE_THIS crtc.reg[index] = value;
  }
  else
    BX_PANIC(("crtc: unknown index 0x%02x write", index));

  if (update_cursor_addr) {
    bool prev_enabled = BX_GEFORCE_THIS hw_cursor.enabled;
    BX_GEFORCE_THIS hw_cursor.enabled = BX_GEFORCE_THIS crtc.reg[0x31] & 1;
    BX_GEFORCE_THIS hw_cursor.offset =
      BX_GEFORCE_THIS crtc.reg[0x31] >> 2 << 11 |
      (BX_GEFORCE_THIS crtc.reg[0x30] & 0x7F) << 17 |
      BX_GEFORCE_THIS crtc.reg[0x2f] << 24;
    if (prev_enabled != BX_GEFORCE_THIS hw_cursor.enabled) {
      BX_GEFORCE_THIS redraw_area(BX_GEFORCE_THIS hw_cursor.x, BX_GEFORCE_THIS hw_cursor.y,
        BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
    }
  }
}

Bit8u bx_geforce_c::register_read8(Bit32u address)
{
  Bit8u value = 0xFF;
  if (address >= 0x300000 && address < 0x310000) {
    if (BX_GEFORCE_THIS pci_conf[0x50] == 0x00)
      value = BX_GEFORCE_THIS pci_rom[address - 0x300000];
    else
      value = 0x00;
  } else if (address == 0xc03c4 || address == 0xc03c5 ||
             address == 0xc03cc || address == 0xc03cf) {
    value = SVGA_READ(address - 0xc0000, 1);
  } else if (address == 0x6013c0 || address == 0x6013c1 ||
             address == 0x6013c2 || address == 0x6013d4 ||
             address == 0x6013d5 || address == 0x6013da) {
    value = SVGA_READ(address - 0x601000, 1);
  } else if (address >= 0x6813c6 && address <= 0x6813c9) {
    value = SVGA_READ(address - 0x681000, 1);
  } else if (address >= 0x700000 && address < 0x800000) {
    value = BX_GEFORCE_THIS s.memory[address - 0x700000 + RAMIN_OFFSET];
  }
  else
    BX_PANIC(("Unknown 8 bit register 0x%08x read", address));
  return value;
}

void bx_geforce_c::register_write8(Bit32u address, Bit8u value)
{
  if (address == 0xc03c2 || address == 0xc03c4 ||
      address == 0xc03c5 || address == 0xc03ce ||
      address == 0xc03cf) {
    SVGA_WRITE(address - 0xc0000, value, 1);
  } else if (address == 0x6013c0 || address == 0x6013c1 ||
             address == 0x6013c2 || address == 0x6013d4 ||
             address == 0x6013d5 || address == 0x6013da) {
    SVGA_WRITE(address - 0x601000, value, 1);
  } else if (address >= 0x6813c6 && address <= 0x6813c9) {
    SVGA_WRITE(address - 0x681000, value, 1);
  } else if (address >= 0x700000 && address < 0x800000) {
    BX_GEFORCE_THIS s.memory[address - 0x700000 + RAMIN_OFFSET] = value;
  }
  else
    BX_PANIC(("Unknown 8 bit register 0x%08x write", address));
}

Bit32u bx_geforce_c::register_read32(Bit32u address)
{
  Bit32u value = 0;

  if (address == 0x0)
    value = 0x020200A5; // PMC_ID
  else if (address == 0x100)
    value = BX_GEFORCE_THIS mc_intr;
  else if (address == 0x140)
    value = BX_GEFORCE_THIS mc_intr_en;
  else if (address == 0x200)
    value = BX_GEFORCE_THIS mc_enable;
  else if (address == 0x1084)
    value = BX_GEFORCE_THIS bus_debug_1;
  else if (address >= 0x1200 && address < 0x1300)
    value = BX_GEFORCE_THIS bus_12xx[(address - 0x1200) / 4];
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
  } else if (address == 0x9100) {
    value = BX_GEFORCE_THIS timer_intr;
  } else if (address == 0x9140) {
    value = BX_GEFORCE_THIS timer_intr_en;
  } else if (address == 0x9200)
    value = BX_GEFORCE_THIS timer_num;
  else if (address == 0x9210)
    value = BX_GEFORCE_THIS timer_den;
  else if (address == 0x9400) {
    BX_GEFORCE_THIS timer_time += 0x10000;
    value = (Bit32u)BX_GEFORCE_THIS timer_time;
  } else if (address == 0x9410)
    value = BX_GEFORCE_THIS timer_time >> 32;
  else if (address == 0x9420)
    value = BX_GEFORCE_THIS timer_alarm;
  else if (address == 0x100080)
    value = BX_GEFORCE_THIS fb_debug_0;
  else if (address == 0x100200)
    value = BX_GEFORCE_THIS fb_cfg0;
  else if (address == 0x100204)
    value = BX_GEFORCE_THIS fb_cfg1;
  else if (address == 0x10020c)
    value = GEFORCE_VIDEO_MEMORY_BYTES;
  else if (address == 0x101000)
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
  } else if (address == 0x400140) {
    value = BX_GEFORCE_THIS graph_intr_en;
  } else if (address == 0x400700) {
    value = BX_GEFORCE_THIS graph_status;
  } else if (address == 0x400720) {
    value = BX_GEFORCE_THIS graph_fifo;
  } else if (address == 0x400780) {
    value = BX_GEFORCE_THIS graph_channel_ctx_table;
  } else if (address == 0x600140) {
    value = BX_GEFORCE_THIS crtc_intr_en;
  } else if (address == 0x600800) {
    value = BX_GEFORCE_THIS crtc_start;
  } else if (address == 0x600804) {
    value = BX_GEFORCE_THIS crtc_config;
  } else if (address == 0x600810) {
    value = BX_GEFORCE_THIS crtc_cursor_config;
  } else if (address == 0x600818) {
    value = BX_GEFORCE_THIS crtc_gpio;
  } else if (address == 0x6013da) {
    value = register_read8(address);
  } else if (address == 0x680300) {
    value = BX_GEFORCE_THIS ramdac_cu_start_pos;
  } else if (address == 0x680500) {
    value = BX_GEFORCE_THIS nvpll;
  } else if (address == 0x680504) {
    value = BX_GEFORCE_THIS mpll;
  } else if (address == 0x680508) {
    value = BX_GEFORCE_THIS vpll;
  } else if (address == 0x68050C) {
    value = BX_GEFORCE_THIS pll_control;
  } else if (address == 0x680600) {
    value = BX_GEFORCE_THIS general_control;
  } else if (address == 0x680608) {
    value = BX_GEFORCE_THIS test_control;
  } else if (address == 0x680828) {
    //value = BX_GEFORCE_THIS ramdac_fp_hcrtc;
  } else if (address == 0x680848) {
    value = BX_GEFORCE_THIS ramdac_fp_tg_control;
  } else if (address >= 0x700000 && address < 0x800000) {
    Bit32u offset = address - 0x700000 + RAMIN_OFFSET;
    value = 
      (BX_GEFORCE_THIS s.memory[offset + 0] << 0) |
      (BX_GEFORCE_THIS s.memory[offset + 1] << 8) |
      (BX_GEFORCE_THIS s.memory[offset + 2] << 16) |
      (BX_GEFORCE_THIS s.memory[offset + 3] << 24);
  } else {
    value = BX_GEFORCE_THIS unk_regs[address / 4];
  }
  return value;
}

void bx_geforce_c::register_write32(Bit32u address, Bit32u value)
{
  if (address == 0x100) {
    BX_GEFORCE_THIS mc_intr &= ~value;
  } else if (address == 0x140) {
    BX_GEFORCE_THIS mc_intr_en = value;
  } else if (address == 0x200) {
    BX_GEFORCE_THIS mc_enable = value;
  } else if (address == 0x1084) {
    BX_GEFORCE_THIS bus_debug_1 = value;
  } else if (address >= 0x1200 && address < 0x1300) {
    BX_GEFORCE_THIS bus_12xx[(address - 0x1200) / 4] = value;
  } else if (address >= 0x1800 && address < 0x1900) {
    BX_GEFORCE_THIS pci_write_handler(address - 0x1800, value, 4);
  } else if (address == 0x2100) {
    BX_GEFORCE_THIS fifo_intr &= ~value;
  } else if (address == 0x2140) {
    BX_GEFORCE_THIS fifo_intr_en = value;
  } else if (address == 0x9100) {
    BX_GEFORCE_THIS timer_intr &= ~value;
  } else if (address == 0x9140) {
    BX_GEFORCE_THIS timer_intr_en = value;
  } else if (address == 0x9200) {
    BX_GEFORCE_THIS timer_num = value;
  } else if (address == 0x9210) {
    BX_GEFORCE_THIS timer_den = value;
  } else if (address == 0x9400) {
    BX_GEFORCE_THIS timer_time = (BX_GEFORCE_THIS timer_time & 0xFFFFFFFF00000000ULL) | value;
  } else if (address == 0x9410) {
    BX_GEFORCE_THIS timer_time = (BX_GEFORCE_THIS timer_time & 0x00000000FFFFFFFFULL) | ((Bit64u)value << 32);
  } else if (address == 0x9420) {
    BX_GEFORCE_THIS timer_alarm = value;
  } else if (address == 0x100080) {
    BX_GEFORCE_THIS fb_debug_0 = value;
  } else if (address == 0x100200) {
    BX_GEFORCE_THIS fb_cfg0 = value;
  } else if (address == 0x100204) {
    BX_GEFORCE_THIS fb_cfg1 = value;
  } else if (address == 0x101000) {
    if (value >> 31)
      BX_GEFORCE_THIS straps0_primary = value;
    else
      BX_GEFORCE_THIS straps0_primary = STRAPS0_PRIMARY;
  } else if (address == 0x400100) {
    BX_GEFORCE_THIS graph_intr &= ~value;
  } else if (address == 0x400140) {
    BX_GEFORCE_THIS graph_intr_en = value;
  } else if (address == 0x400700) {
    BX_GEFORCE_THIS graph_status = value;
  } else if (address == 0x400720) {
    BX_GEFORCE_THIS graph_fifo = value;
  } else if (address == 0x400780) {
    BX_GEFORCE_THIS graph_channel_ctx_table = value;
  } else if (address == 0x600140) {
    BX_GEFORCE_THIS crtc_intr_en = value;
  } else if (address == 0x600800) {
    BX_GEFORCE_THIS crtc_start = value;
    BX_GEFORCE_THIS svga_needs_update_mode = 1;
  } else if (address == 0x600804) {
    BX_GEFORCE_THIS crtc_config = value;
  } else if (address == 0x600810) {
    BX_GEFORCE_THIS crtc_cursor_config = value;
    BX_GEFORCE_THIS hw_cursor.size = value & 0x00010000 ? 64 : 32;
  } else if (address == 0x600818) {
    BX_GEFORCE_THIS crtc_gpio = value;
  } else if (address == 0x680300) {
    Bit16s prevx = BX_GEFORCE_THIS hw_cursor.x;
    Bit16s prevy = BX_GEFORCE_THIS hw_cursor.y;
    BX_GEFORCE_THIS ramdac_cu_start_pos = value;
    BX_GEFORCE_THIS hw_cursor.x = BX_GEFORCE_THIS ramdac_cu_start_pos & 0xFFFF;
    BX_GEFORCE_THIS hw_cursor.y = BX_GEFORCE_THIS ramdac_cu_start_pos >> 16;
    BX_GEFORCE_THIS redraw_area(prevx, prevy,
      BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
    BX_GEFORCE_THIS redraw_area(BX_GEFORCE_THIS hw_cursor.x, BX_GEFORCE_THIS hw_cursor.y,
      BX_GEFORCE_THIS hw_cursor.size, BX_GEFORCE_THIS hw_cursor.size);
  } else if (address == 0x680500) {
    BX_GEFORCE_THIS nvpll = value;
  } else if (address == 0x680504) {
    BX_GEFORCE_THIS mpll = value;
  } else if (address == 0x680508) {
    BX_GEFORCE_THIS vpll = value;
  } else if (address == 0x68050C) {
    BX_GEFORCE_THIS pll_control = value;
  } else if (address == 0x680600) {
    BX_GEFORCE_THIS general_control = value;
  } else if (address == 0x680608) {
    BX_GEFORCE_THIS test_control = value;
  } else if (address == 0x680828) {
    BX_GEFORCE_THIS ramdac_fp_hcrtc = value;
  } else if (address == 0x680848) {
    BX_GEFORCE_THIS ramdac_fp_tg_control = value;
  } else if (address >= 0x700000 && address < 0x800000) {
    Bit32u offset = address - 0x700000 + RAMIN_OFFSET;
    BX_GEFORCE_THIS s.memory[offset + 0] = (value >> 0) & 0xFF;
    BX_GEFORCE_THIS s.memory[offset + 1] = (value >> 8) & 0xFF;
    BX_GEFORCE_THIS s.memory[offset + 2] = (value >> 16) & 0xFF;
    BX_GEFORCE_THIS s.memory[offset + 3] = (value >> 24) & 0xFF;
  } else {
    BX_GEFORCE_THIS unk_regs[address / 4] = value;
  }
}


/////////////////////////////////////////////////////////////////////////
//
// PCI support
//
/////////////////////////////////////////////////////////////////////////

void bx_geforce_c::svga_init_pcihandlers(void)
{
    BX_GEFORCE_THIS devfunc = 0x00;
    DEV_register_pci_handlers2(BX_GEFORCE_THIS_PTR,
        &BX_GEFORCE_THIS devfunc, BX_PLUGIN_GEFORCE, "GeForce AGP", true);
    BX_GEFORCE_THIS init_pci_conf(PCI_VENDOR_NVIDIA, PCI_DEVICE_GEFORCE3TI500, 0xA3,
      (PCI_CLASS_BASE_DISPLAY << 16) | (PCI_CLASS_SUB_VGA << 8),
      PCI_CLASS_HEADERTYPE_00h, BX_PCI_INTA);

    BX_GEFORCE_THIS pci_conf[0x14] = 0x08;
    BX_GEFORCE_THIS pci_conf[0x18] = 0x08;
    BX_GEFORCE_THIS init_bar_mem(0, GEFORCE_PNPMMIO_SIZE, geforce_mem_read_handler,
                                 geforce_mem_write_handler);
    BX_GEFORCE_THIS init_bar_mem(1, GEFORCE_VIDEO_MEMORY_BYTES, geforce_mem_read_handler,
                                geforce_mem_write_handler);
    BX_GEFORCE_THIS init_bar_mem(2, GEFORCE_BAR2_SIZE, geforce_mem_read_handler,
                                geforce_mem_write_handler);
    BX_GEFORCE_THIS pci_rom_address = 0;
    BX_GEFORCE_THIS pci_rom_read_handler = geforce_mem_read_handler;
    BX_GEFORCE_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());

    BX_GEFORCE_THIS pci_conf[0x2c] = 0x7D;
    BX_GEFORCE_THIS pci_conf[0x2d] = 0x10;
    BX_GEFORCE_THIS pci_conf[0x2e] = 0x63;
    BX_GEFORCE_THIS pci_conf[0x2f] = 0x28;
    BX_GEFORCE_THIS pci_conf[0x40] = BX_GEFORCE_THIS pci_conf[0x2c];
    BX_GEFORCE_THIS pci_conf[0x41] = BX_GEFORCE_THIS pci_conf[0x2d];
    BX_GEFORCE_THIS pci_conf[0x42] = BX_GEFORCE_THIS pci_conf[0x2e];
    BX_GEFORCE_THIS pci_conf[0x43] = BX_GEFORCE_THIS pci_conf[0x2f];
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
