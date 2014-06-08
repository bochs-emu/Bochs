/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
//  PCI VGA dummy adapter Copyright (C) 2002,2003  Mike Nordell
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

// Standard VGA emulation + Bochs VBE support + PCI VGA interface

// NOTE from the original pcivga code:
// This "driver" was created for the SOLE PURPOSE of getting BeOS
// to boot. It currently does NOTHING more than presenting a generic VGA
// device on the PCI bus. ALL gfx in/out-put is still handled by the VGA code.
// Furthermore, almost all of the PCI registers are currently acting like RAM.

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "vgacore.h"
#include "vga.h"
#include "virt_timer.h"

#define LOG_THIS theVga->

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, do nothing.
#define SET_TILE_UPDATED(xtile, ytile, value)                            \
  do {                                                                   \
    if (((xtile) < BX_VGA_THIS s.num_x_tiles) && ((ytile) < BX_VGA_THIS s.num_y_tiles)) \
      BX_VGA_THIS s.vga_tile_updated[(xtile)+(ytile)*BX_VGA_THIS s.num_x_tiles] = value; \
  } while (0)

// Only reference the array if the tile numbers are within the bounds
// of the array.  If out of bounds, return 0.
#define GET_TILE_UPDATED(xtile,ytile)                                    \
  ((((xtile) < BX_VGA_THIS s.num_x_tiles) && ((ytile) < BX_VGA_THIS s.num_y_tiles))? \
     BX_VGA_THIS s.vga_tile_updated[(xtile)+(ytile)*BX_VGA_THIS s.num_x_tiles] \
     : 0)

bx_vga_c *theVga = NULL;

int CDECL libvga_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    theVga = new bx_vga_c();
    bx_devices.pluginVgaDevice = theVga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theVga, BX_PLUGIN_VGA);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libvga_LTX_plugin_fini(void)
{
  delete theVga;
}

bx_vga_c::bx_vga_c() : bx_vgacore_c()
{
  put("VGA");
}

bx_vga_c::~bx_vga_c()
{
  SIM->get_bochs_root()->remove("vga");
  BX_DEBUG(("Exit"));
}

void bx_vga_c::init_vga_extension(void)
{
  unsigned addr;
  Bit16u max_xres, max_yres, max_bpp;

  BX_VGA_THIS init_iohandlers(read_handler, write_handler);
  BX_VGA_THIS init_systemtimer(timer_handler, vga_param_handler);
  BX_VGA_THIS pci_enabled = SIM->is_pci_device("pcivga");

  // The following is for the VBE display extension
  BX_VGA_THIS vbe_present = 0;
  BX_VGA_THIS vbe.enabled = 0;
  BX_VGA_THIS vbe.dac_8bit = 0;
  BX_VGA_THIS vbe.base_address = 0x0000;
  if (!strcmp(SIM->get_param_string(BXPN_VGA_EXTENSION)->getptr(), "vbe")) {
    BX_VGA_THIS put("BXVGA");
    for (addr=VBE_DISPI_IOPORT_INDEX; addr<=VBE_DISPI_IOPORT_DATA; addr++) {
      DEV_register_ioread_handler(this, vbe_read_handler, addr, "vga video", 7);
      DEV_register_iowrite_handler(this, vbe_write_handler, addr, "vga video", 7);
    }
    if (!BX_VGA_THIS pci_enabled) {
      BX_VGA_THIS vbe.base_address = VBE_DISPI_LFB_PHYSICAL_ADDRESS;
      DEV_register_memory_handlers(theVga, mem_read_handler, mem_write_handler,
                                   BX_VGA_THIS vbe.base_address,
                                   BX_VGA_THIS vbe.base_address + VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES - 1);
    }
    if (BX_VGA_THIS s.memory == NULL)
      BX_VGA_THIS s.memory = new Bit8u[VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES];
    memset(BX_VGA_THIS s.memory, 0, VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES);
    BX_VGA_THIS s.memsize = VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES;
    BX_VGA_THIS vbe.cur_dispi=VBE_DISPI_ID0;
    BX_VGA_THIS vbe.xres=640;
    BX_VGA_THIS vbe.yres=480;
    BX_VGA_THIS vbe.bpp=8;
    BX_VGA_THIS vbe.bank=0;
    BX_VGA_THIS vbe.curindex=0;
    BX_VGA_THIS vbe.offset_x=0;
    BX_VGA_THIS vbe.offset_y=0;
    BX_VGA_THIS vbe.virtual_xres=640;
    BX_VGA_THIS vbe.virtual_yres=480;
    BX_VGA_THIS vbe.bpp_multiplier=1;
    BX_VGA_THIS vbe.virtual_start=0;
    BX_VGA_THIS vbe.lfb_enabled=0;
    BX_VGA_THIS vbe.get_capabilities=0;
    bx_gui->get_capabilities(&max_xres, &max_yres, &max_bpp);
    if (max_xres > VBE_DISPI_MAX_XRES) {
      BX_VGA_THIS vbe.max_xres=VBE_DISPI_MAX_XRES;
    } else {
      BX_VGA_THIS vbe.max_xres=max_xres;
    }
    if (max_yres > VBE_DISPI_MAX_YRES) {
      BX_VGA_THIS vbe.max_yres=VBE_DISPI_MAX_YRES;
    } else {
      BX_VGA_THIS vbe.max_yres=max_yres;
    }
    if (max_bpp > VBE_DISPI_MAX_BPP) {
      BX_VGA_THIS vbe.max_bpp=VBE_DISPI_MAX_BPP;
    } else {
      BX_VGA_THIS vbe.max_bpp=max_bpp;
    }
    BX_VGA_THIS s.max_xres = BX_VGA_THIS vbe.max_xres;
    BX_VGA_THIS s.max_yres = BX_VGA_THIS vbe.max_yres;
    BX_VGA_THIS vbe_present = 1;
    BX_VGA_THIS extension_init = 1;

    BX_INFO(("VBE Bochs Display Extension Enabled"));
  }
#if BX_SUPPORT_PCI
  Bit8u devfunc = 0x00;

  if (BX_VGA_THIS pci_enabled) {
    DEV_register_pci_handlers(this, &devfunc, "pcivga", "Experimental PCI VGA");

    // initialize readonly registers
    // Note that the values for vendor and device id are selected at random!
    // There might actually be "real" values for "experimental" vendor and
    // device that should be used!
    init_pci_conf(0x1234, 0x1111, 0x00, 0x030000, 0x00);

    if (BX_VGA_THIS vbe_present) {
      BX_VGA_THIS pci_conf[0x10] = 0x08;
      BX_VGA_THIS pci_base_address[0] = 0;
    }
    BX_VGA_THIS pci_rom_address = 0;
    BX_VGA_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());
  }
#endif
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("vga", this);
#endif
}

void bx_vga_c::reset(unsigned type)
{
#if BX_SUPPORT_PCI
  if (BX_VGA_THIS pci_enabled) {
    static const struct reset_vals_t {
      unsigned      addr;
      unsigned char val;
    } reset_vals[] = {
        { 0x04, 0x03 }, { 0x05, 0x00 }, // command_io + command_mem
        { 0x06, 0x00 }, { 0x07, 0x02 }  // status_devsel_medium
    };
    for (unsigned i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_VGA_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }
#endif
}

void bx_vga_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "vga", "VGA Adapter State");
  bx_vgacore_c::register_state(list);
#if BX_SUPPORT_PCI
  if (BX_VGA_THIS pci_enabled) {
    register_pci_state(list);
  }
#endif
  // register state for Bochs VBE
  if (BX_VGA_THIS vbe_present) {
    bx_list_c *vbe = new bx_list_c(list, "vbe");
    new bx_shadow_num_c(vbe, "cur_dispi", &BX_VGA_THIS vbe.cur_dispi, BASE_HEX);
    new bx_shadow_num_c(vbe, "xres", &BX_VGA_THIS vbe.xres);
    new bx_shadow_num_c(vbe, "yres", &BX_VGA_THIS vbe.yres);
    new bx_shadow_num_c(vbe, "bpp", &BX_VGA_THIS vbe.bpp);
    new bx_shadow_num_c(vbe, "bank", &BX_VGA_THIS vbe.bank);
    new bx_shadow_bool_c(vbe, "enabled", &BX_VGA_THIS vbe.enabled);
    new bx_shadow_num_c(vbe, "curindex", &BX_VGA_THIS vbe.curindex);
    new bx_shadow_num_c(vbe, "visible_screen_size", &BX_VGA_THIS vbe.visible_screen_size);
    new bx_shadow_num_c(vbe, "offset_x", &BX_VGA_THIS vbe.offset_x);
    new bx_shadow_num_c(vbe, "offset_y", &BX_VGA_THIS vbe.offset_y);
    new bx_shadow_num_c(vbe, "virtual_xres", &BX_VGA_THIS vbe.virtual_xres);
    new bx_shadow_num_c(vbe, "virtual_yres", &BX_VGA_THIS vbe.virtual_yres);
    new bx_shadow_num_c(vbe, "virtual_start", &BX_VGA_THIS vbe.virtual_start);
    new bx_shadow_num_c(vbe, "bpp_multiplier", &BX_VGA_THIS vbe.bpp_multiplier);
    new bx_shadow_bool_c(vbe, "lfb_enabled", &BX_VGA_THIS vbe.lfb_enabled);
    new bx_shadow_bool_c(vbe, "get_capabilities", &BX_VGA_THIS vbe.get_capabilities);
    new bx_shadow_bool_c(vbe, "dac_8bit", &BX_VGA_THIS vbe.dac_8bit);
  }
}

void bx_vga_c::after_restore_state(void)
{
#if BX_SUPPORT_PCI
  if (BX_VGA_THIS pci_enabled) {
    if (BX_VGA_THIS vbe_present) {
      if (BX_VGA_THIS vbe_set_base_addr(&BX_VGA_THIS pci_base_address[0],
                                        &BX_VGA_THIS pci_conf[0x10])) {
        BX_INFO(("new base address: 0x%08x", BX_VGA_THIS pci_base_address[0]));
      }
    }
    if (DEV_pci_set_base_mem(this, mem_read_handler, mem_write_handler,
                             &BX_VGA_THIS pci_rom_address,
                             &BX_VGA_THIS pci_conf[0x30],
                             BX_VGA_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_VGA_THIS pci_rom_address));
    }
  }
#endif
  if (BX_VGA_THIS vbe.enabled) {
    bx_gui->dimension_update(BX_VGA_THIS vbe.xres, BX_VGA_THIS vbe.yres, 0, 0,
                             BX_VGA_THIS vbe.bpp);
  }
  bx_vgacore_c::after_restore_state();
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_vga_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if BX_USE_VGA_SMF == 0
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;
  class_ptr->write(address, value, io_len, 0);
#else
  UNUSED(this_ptr);
  theVga->write(address, value, io_len, 0);
#endif
}

#if BX_USE_VGA_SMF
void bx_vga_c::write_handler_no_log(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  UNUSED(this_ptr);
  theVga->write(address, value, io_len, 1);
}
#endif

void bx_vga_c::write(Bit32u address, Bit32u value, unsigned io_len, bx_bool no_log)
{
  if (io_len == 2) {
#if BX_USE_VGA_SMF
    bx_vga_c::write_handler_no_log(0, address, value & 0xff, 1);
    bx_vga_c::write_handler_no_log(0, address+1, (value >> 8) & 0xff, 1);
#else
    bx_vga_c::write(address, value & 0xff, 1, 1);
    bx_vga_c::write(address+1, (value >> 8) & 0xff, 1, 1);
#endif
    return;
  }

  if ((address >= 0x03b0) && (address <= 0x03bf) &&
      (BX_VGA_THIS s.misc_output.color_emulation))
    return;
  if ((address >= 0x03d0) && (address <= 0x03df) &&
      (BX_VGA_THIS s.misc_output.color_emulation==0))
    return;

  switch (address) {
    case 0x03b5: /* CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* CRTC Registers (color emulation modes) */
      if (BX_VGA_THIS s.CRTC.address > 0x18) {
        BX_DEBUG(("write: invalid CRTC register 0x%02x ignored",
          (unsigned) BX_VGA_THIS s.CRTC.address));
        return;
      }
      if (value != BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address]) {
        switch (BX_VGA_THIS s.CRTC.address) {
          case 0x13:
          case 0x14:
          case 0x17:
            if (!BX_VGA_THIS vbe.enabled || (BX_VGA_THIS vbe.bpp == VBE_DISPI_BPP_4)) {
              // Line offset change
              bx_vgacore_c::write(address, value, io_len, no_log);
            } else {
              BX_VGA_THIS s.CRTC.reg[BX_VGA_THIS s.CRTC.address] = value;
            }
            break;
          default:
            bx_vgacore_c::write(address, value, io_len, no_log);
        }
      }
      break;
    default:
      bx_vgacore_c::write(address, value, io_len, no_log);
  }
}

Bit64s bx_vga_c::vga_param_handler(bx_param_c *param, int set, Bit64s val)
{
  // handler for runtime parameter 'vga: update_freq'
  if (set) {
    BX_VGA_THIS update_interval = (Bit32u)(1000000 / val);
    BX_INFO(("Changing timer interval to %d", BX_VGA_THIS update_interval));
    BX_VGA_THIS timer_handler(theVga);
    bx_virt_timer.activate_timer(BX_VGA_THIS timer_id, BX_VGA_THIS update_interval, 1);
    if (BX_VGA_THIS update_interval < 300000) {
      BX_VGA_THIS s.blink_counter = 300000 / (unsigned)BX_VGA_THIS update_interval;
    } else {
      BX_VGA_THIS s.blink_counter = 1;
    }
  }
  return val;
}

void bx_vga_c::refresh_display(void *this_ptr, bx_bool redraw)
{
  if (BX_VGA_THIS s.vga_override && (BX_VGA_THIS s.nvgadev != NULL)) {
    BX_VGA_THIS s.nvgadev->refresh_display(BX_VGA_THIS s.nvgadev, redraw);
  } else {
    if (redraw) {
      redraw_area(0, 0, BX_VGA_THIS s.last_xres, BX_VGA_THIS s.last_yres);
    }
    timer_handler(this_ptr);
  }
}

void bx_vga_c::timer_handler(void *this_ptr)
{
#if BX_USE_VGA_SMF == 0
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;
  class_ptr->timer();
}

void bx_vga_c::timer(void)
{
#else
  UNUSED(this_ptr);
#endif

  update();
  bx_gui->flush();
}

void bx_vga_c::update(void)
{
  unsigned iHeight, iWidth;

  if (BX_VGA_THIS vbe.enabled) {
    /* no screen update necessary */
    if ((BX_VGA_THIS s.vga_mem_updated==0) && BX_VGA_THIS s.graphics_ctrl.graphics_alpha)
      return;

    /* skip screen update when vga/video is disabled or the sequencer is in reset mode */
    if (!BX_VGA_THIS s.vga_enabled || !BX_VGA_THIS s.attribute_ctrl.video_enabled
        || !BX_VGA_THIS s.sequencer.reset2 || !BX_VGA_THIS s.sequencer.reset1
        || (BX_VGA_THIS s.sequencer.reg1 & 0x20))
      return;

    /* skip screen update if the vertical retrace is in progress
       (using 72 Hz vertical frequency) */
    if ((bx_pc_system.time_usec() % 13888) < 70)
      return;

    if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
      // specific VBE code display update code
      unsigned pitch;
      unsigned xc, yc, xti, yti;
      unsigned r, c, w, h;
      int i;
      unsigned long red, green, blue, colour;
      Bit8u * vid_ptr, * vid_ptr2;
      Bit8u * tile_ptr, * tile_ptr2;
      bx_svga_tileinfo_t info;
      Bit8u dac_size = BX_VGA_THIS vbe.dac_8bit ? 8 : 6;

      iWidth=BX_VGA_THIS vbe.xres;
      iHeight=BX_VGA_THIS vbe.yres;
      pitch = BX_VGA_THIS s.line_offset;
      Bit8u *disp_ptr = &BX_VGA_THIS s.memory[BX_VGA_THIS vbe.virtual_start];

      if (bx_gui->graphics_tile_info_common(&info)) {
        if (info.snapshot_mode) {
          vid_ptr = disp_ptr;
          tile_ptr = bx_gui->get_snapshot_buffer();
          if (tile_ptr != NULL) {
            for (yc = 0; yc < iHeight; yc++) {
              memcpy(tile_ptr, vid_ptr, info.pitch);
              vid_ptr += pitch;
              tile_ptr += info.pitch;
            }
          }
        } else if (info.is_indexed) {
          switch (BX_VGA_THIS vbe.bpp) {
            case 4:
            case 15:
            case 16:
            case 24:
            case 32:
              BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays"));
              break;
            case 8:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + xc);
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        colour = 0;
                        for (i=0; i<(int)BX_VGA_THIS vbe.bpp; i+=8) {
                          colour |= *(vid_ptr2++) << i;
                        }
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
          }
        } else {
          switch (BX_VGA_THIS vbe.bpp) {
            case 4:
              BX_ERROR(("cannot draw 4bpp SVGA"));
              break;
            case 8:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + xc);
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        colour = *(vid_ptr2++);
                        colour = MAKE_COLOUR(
                          BX_VGA_THIS s.pel.data[colour].red, dac_size, info.red_shift, info.red_mask,
                          BX_VGA_THIS s.pel.data[colour].green, dac_size, info.green_shift, info.green_mask,
                          BX_VGA_THIS s.pel.data[colour].blue, dac_size, info.blue_shift, info.blue_mask);
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
            case 15:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + (xc<<1));
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        colour = *(vid_ptr2++);
                        colour |= *(vid_ptr2++) << 8;
                        colour = MAKE_COLOUR(
                          colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                          colour & 0x03e0, 10, info.green_shift, info.green_mask,
                          colour & 0x7c00, 15, info.red_shift, info.red_mask);
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
            case 16:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + (xc<<1));
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        colour = *(vid_ptr2++);
                        colour |= *(vid_ptr2++) << 8;
                        colour = MAKE_COLOUR(
                          colour & 0x001f, 5, info.blue_shift, info.blue_mask,
                          colour & 0x07e0, 11, info.green_shift, info.green_mask,
                          colour & 0xf800, 16, info.red_shift, info.red_mask);
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
            case 24:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + 3*xc);
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        blue = *(vid_ptr2++);
                        green = *(vid_ptr2++);
                        red = *(vid_ptr2++);
                        colour = MAKE_COLOUR(
                          red, 8, info.red_shift, info.red_mask,
                          green, 8, info.green_shift, info.green_mask,
                          blue, 8, info.blue_shift, info.blue_mask);
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
            case 32:
              for (yc=0, yti = 0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
                for (xc=0, xti = 0; xc<iWidth; xc+=X_TILESIZE, xti++) {
                  if (GET_TILE_UPDATED (xti, yti)) {
                    vid_ptr = disp_ptr + (yc * pitch + (xc<<2));
                    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                    for (r=0; r<h; r++) {
                      vid_ptr2  = vid_ptr;
                      tile_ptr2 = tile_ptr;
                      for (c=0; c<w; c++) {
                        blue = *(vid_ptr2++);
                        green = *(vid_ptr2++);
                        red = *(vid_ptr2++);
                        vid_ptr2++;
                        colour = MAKE_COLOUR(
                          red, 8, info.red_shift, info.red_mask,
                          green, 8, info.green_shift, info.green_mask,
                          blue, 8, info.blue_shift, info.blue_mask);
                        if (info.is_little_endian) {
                          for (i=0; i<info.bpp; i+=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        } else {
                          for (i=info.bpp-8; i>-8; i-=8) {
                            *(tile_ptr2++) = (Bit8u)(colour >> i);
                          }
                        }
                      }
                      vid_ptr  += pitch;
                      tile_ptr += info.pitch;
                    }
                    bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                    SET_TILE_UPDATED (xti, yti, 0);
                  }
                }
              }
              break;
          }
        }
        BX_VGA_THIS s.last_xres = iWidth;
        BX_VGA_THIS s.last_yres = iHeight;
        BX_VGA_THIS s.vga_mem_updated = 0;
      } else {
        BX_PANIC(("cannot get svga tile info"));
      }
    } else {
      unsigned r, c, x, y;
      unsigned xc, yc, xti, yti;
      Bit8u *plane[4];

      BX_VGA_THIS determine_screen_dimensions(&iHeight, &iWidth);
      if ((iWidth != BX_VGA_THIS s.last_xres) || (iHeight != BX_VGA_THIS s.last_yres) ||
           (BX_VGA_THIS s.last_bpp > 8)) {
        bx_gui->dimension_update(iWidth, iHeight);
        BX_VGA_THIS s.last_xres = iWidth;
        BX_VGA_THIS s.last_yres = iHeight;
        BX_VGA_THIS s.last_bpp = 8;
      }

      plane[0] = &BX_VGA_THIS s.memory[0<<VBE_DISPI_4BPP_PLANE_SHIFT];
      plane[1] = &BX_VGA_THIS s.memory[1<<VBE_DISPI_4BPP_PLANE_SHIFT];
      plane[2] = &BX_VGA_THIS s.memory[2<<VBE_DISPI_4BPP_PLANE_SHIFT];
      plane[3] = &BX_VGA_THIS s.memory[3<<VBE_DISPI_4BPP_PLANE_SHIFT];

      for (yc=0, yti=0; yc<iHeight; yc+=Y_TILESIZE, yti++) {
        for (xc=0, xti=0; xc<iWidth; xc+=X_TILESIZE, xti++) {
          if (GET_TILE_UPDATED (xti, yti)) {
            for (r=0; r<Y_TILESIZE; r++) {
              y = yc + r;
              if (BX_VGA_THIS s.y_doublescan) y >>= 1;
              for (c=0; c<X_TILESIZE; c++) {
                x = xc + c;
                BX_VGA_THIS s.tile[r*X_TILESIZE + c] =
                  BX_VGA_THIS get_vga_pixel(x, y, BX_VGA_THIS vbe.virtual_start, 0xffff, 0, plane);
              }
            }
            SET_TILE_UPDATED (xti, yti, 0);
            bx_gui->graphics_tile_update_common(BX_VGA_THIS s.tile, xc, yc);
          }
        }
      }
    }
  } else {
    BX_VGA_THIS bx_vgacore_c::update();
  }
}

bx_bool bx_vga_c::mem_read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = theVga->mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

Bit8u bx_vga_c::mem_read(bx_phy_address addr)
{
#if BX_SUPPORT_PCI
  if ((BX_VGA_THIS pci_enabled) && (BX_VGA_THIS pci_rom_size > 0)) {
    Bit32u mask = (BX_VGA_THIS pci_rom_size - 1);
    if ((addr & ~mask) == BX_VGA_THIS pci_rom_address) {
      if (BX_VGA_THIS pci_conf[0x30] & 0x01) {
        return BX_VGA_THIS pci_rom[addr & mask];
      } else {
        return 0xff;
      }
    }
  }
#endif
  // if in a vbe enabled mode, read from the vbe_memory
  if ((BX_VGA_THIS vbe.enabled) && (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4)) {
    return vbe_mem_read(addr);
  } else if ((BX_VGA_THIS vbe.base_address != 0) && (addr >= BX_VGA_THIS vbe.base_address)) {
    return 0xff;
  }

  return bx_vgacore_c::mem_read(addr);
}

bx_bool bx_vga_c::mem_write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    theVga->mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}

void bx_vga_c::mem_write(bx_phy_address addr, Bit8u value)
{
  // if in a vbe enabled mode, write to the vbe_memory
  if ((BX_VGA_THIS vbe.enabled) && (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4)) {
    vbe_mem_write(addr, value);
    return;
  } else if ((BX_VGA_THIS vbe.base_address != 0) && (addr >= BX_VGA_THIS vbe.base_address)) {
    return;
  }

  bx_vgacore_c::mem_write(addr, value);
}

void bx_vga_c::redraw_area(unsigned x0, unsigned y0, unsigned width,
                      unsigned height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1, xmax, ymax;

  if (width == 0 || height == 0) {
    return;
  }
  if (BX_VGA_THIS s.vga_override && (BX_VGA_THIS s.nvgadev != NULL)) {
    BX_VGA_THIS s.nvgadev->redraw_area(x0, y0, width, height);
    return;
  }

  if (BX_VGA_THIS vbe.enabled) {
    BX_VGA_THIS s.vga_mem_updated = 1;
    xmax = BX_VGA_THIS vbe.xres;
    ymax = BX_VGA_THIS vbe.yres;
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
        SET_TILE_UPDATED(xti, yti, 1);
      }
    }

  } else {
    bx_vgacore_c::redraw_area(x0, y0, width, height);
  }
}

#if BX_SUPPORT_PCI
bx_bool bx_vga_c::vbe_set_base_addr(Bit32u *addr, Bit8u *pci_conf)
{
  if (DEV_pci_set_base_mem(BX_VGA_THIS_PTR, mem_read_handler,
                           mem_write_handler,
                           addr, pci_conf, VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)) {
    BX_VGA_THIS vbe.base_address = *addr;
    return 1;
  }
  return 0;
}
#endif

  Bit8u  BX_CPP_AttrRegparmN(1)
bx_vga_c::vbe_mem_read(bx_phy_address addr)
{
  Bit32u offset;

  if (addr >= BX_VGA_THIS vbe.base_address) {
    // LFB read
    offset = (Bit32u)(addr - BX_VGA_THIS vbe.base_address);
  }
  else {
    // banked mode read
    offset = (Bit32u)(BX_VGA_THIS vbe.bank*65536 + addr - 0xA0000);
  }

  // check for out of memory read
  if (offset > VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)
    return 0;

  return (BX_VGA_THIS s.memory[offset]);
}

  void BX_CPP_AttrRegparmN(2)
bx_vga_c::vbe_mem_write(bx_phy_address addr, Bit8u value)
{
  Bit32u offset;
  unsigned x_tileno, y_tileno;

  if (BX_VGA_THIS vbe.lfb_enabled)
  {
    if (addr >= BX_VGA_THIS vbe.base_address) {
      // LFB write
      offset = (Bit32u)(addr - BX_VGA_THIS vbe.base_address);
    }
    else {
      // banked mode write while in LFB mode -> ignore
      return;
    }
  }
  else
  {
    if (addr < BX_VGA_THIS vbe.base_address) {
      // banked mode write
      offset = (Bit32u)(BX_VGA_THIS vbe.bank*65536 + (addr - 0xA0000));
    }
    else {
      // LFB write while in banked mode -> ignore
      return;
    }
  }

  // check for out of memory write
  if (offset < VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)
  {
    BX_VGA_THIS s.memory[offset]=value;
  }
  else
  {
    // make sure we don't flood the logfile
    static int count=0;
    if (count<100)
    {
      count ++;
      BX_INFO(("VBE_mem_write out of video memory write at %x",offset));
    }
  }

  offset-=BX_VGA_THIS vbe.virtual_start;

  // only update the UI when writing 'onscreen'
  if (offset < BX_VGA_THIS vbe.visible_screen_size)
  {
    y_tileno = ((offset / BX_VGA_THIS vbe.bpp_multiplier) / BX_VGA_THIS vbe.virtual_xres) / Y_TILESIZE;
    x_tileno = ((offset / BX_VGA_THIS vbe.bpp_multiplier) % BX_VGA_THIS vbe.virtual_xres) / X_TILESIZE;

    if ((y_tileno < BX_VGA_THIS s.num_y_tiles) && (x_tileno < BX_VGA_THIS s.num_x_tiles))
    {
      BX_VGA_THIS s.vga_mem_updated = 1;
      SET_TILE_UPDATED (x_tileno, y_tileno, 1);
    }
  }
}

Bit32u bx_vga_c::vbe_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if BX_USE_VGA_SMF == 0
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;
  return class_ptr->vbe_read(address, io_len);
}

Bit32u bx_vga_c::vbe_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif  // BX_USE_VGA_SMF == 0
  Bit16u retval;

//  BX_INFO(("VBE_read %x (len %x)", address, io_len));

  if (address==VBE_DISPI_IOPORT_INDEX)
  {
    // index register
    return (Bit32u) BX_VGA_THIS vbe.curindex;
  }
  else
  {
    // data register read

    switch (BX_VGA_THIS vbe.curindex)
    {
      case VBE_DISPI_INDEX_ID: // Display Interface ID check
        return BX_VGA_THIS vbe.cur_dispi;

      case VBE_DISPI_INDEX_XRES: // x resolution
        if (BX_VGA_THIS vbe.get_capabilities) {
          return BX_VGA_THIS vbe.max_xres;
        } else {
          return BX_VGA_THIS vbe.xres;
        }

      case VBE_DISPI_INDEX_YRES: // y resolution
        if (BX_VGA_THIS vbe.get_capabilities) {
          return BX_VGA_THIS vbe.max_yres;
        } else {
          return BX_VGA_THIS vbe.yres;
        }

      case VBE_DISPI_INDEX_BPP: // bpp
        if (BX_VGA_THIS vbe.get_capabilities) {
          return BX_VGA_THIS vbe.max_bpp;
        } else {
          return BX_VGA_THIS vbe.bpp;
        }

      case VBE_DISPI_INDEX_ENABLE: // vbe enabled
        retval = BX_VGA_THIS vbe.enabled;
        if (BX_VGA_THIS vbe.get_capabilities)
          retval |= VBE_DISPI_GETCAPS;
        if (BX_VGA_THIS vbe.dac_8bit)
          retval |= VBE_DISPI_8BIT_DAC;
        return retval;

      case VBE_DISPI_INDEX_BANK: // current bank
        return BX_VGA_THIS vbe.bank;

      case VBE_DISPI_INDEX_X_OFFSET:
        return BX_VGA_THIS vbe.offset_x;

      case VBE_DISPI_INDEX_Y_OFFSET:
        return BX_VGA_THIS vbe.offset_y;

      case VBE_DISPI_INDEX_VIRT_WIDTH:
        return BX_VGA_THIS vbe.virtual_xres;

      case VBE_DISPI_INDEX_VIRT_HEIGHT:
        return BX_VGA_THIS vbe.virtual_yres;

      case VBE_DISPI_INDEX_VIDEO_MEMORY_64K:
        return (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB >> 6);

      default:
        BX_PANIC(("VBE unknown data read index 0x%x",BX_VGA_THIS vbe.curindex));
        break;
    }
  }
  BX_PANIC(("VBE_read shouldn't reach this"));
  return 0; /* keep compiler happy */
}

void bx_vga_c::vbe_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if BX_USE_VGA_SMF == 0
  bx_vga_c *class_ptr = (bx_vga_c *) this_ptr;
  class_ptr->vbe_write(address, value, io_len);
}

Bit32u bx_vga_c::vbe_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif
  bx_bool new_vbe_8bit_dac;
  bx_bool needs_update = 0;
  unsigned i;

//  BX_INFO(("VBE_write %x = %x (len %x)", address, value, io_len));

  switch(address)
  {
    // index register
    case VBE_DISPI_IOPORT_INDEX:

      BX_VGA_THIS vbe.curindex = (Bit16u) value;
      break;

    // data register
    // FIXME: maybe do some 'sanity' checks on received data?
    case VBE_DISPI_IOPORT_DATA:
      switch (BX_VGA_THIS vbe.curindex)
      {
        case VBE_DISPI_INDEX_ID: // Display Interface ID check
        {
          if ((value == VBE_DISPI_ID0) ||
              (value == VBE_DISPI_ID1) ||
              (value == VBE_DISPI_ID2) ||
              (value == VBE_DISPI_ID3) ||
              (value == VBE_DISPI_ID4) ||
              (value == VBE_DISPI_ID5))
          {
            // allow backwards compatible with previous dispi bioses
            BX_VGA_THIS vbe.cur_dispi=value;
          }
          else
          {
            BX_PANIC(("VBE unknown Display Interface %x", value));
          }

          // make sure we don't flood the logfile
          static int count=0;
          if (count < 100)
          {
            count++;
            BX_INFO(("VBE known Display Interface %x", value));
          }
        } break;

        case VBE_DISPI_INDEX_XRES: // set xres
        {
          // check that we don't set xres during vbe enabled
          if (!BX_VGA_THIS vbe.enabled)
          {
            // check for within max xres range
            if (value <= VBE_DISPI_MAX_XRES)
            {
              BX_VGA_THIS vbe.xres=(Bit16u) value;
              BX_INFO(("VBE set xres (%d)", value));
            }
            else
            {
              BX_INFO(("VBE set xres more then max xres (%d)", value));
            }
          }
          else
          {
            BX_ERROR(("VBE set xres during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_YRES: // set yres
        {
          // check that we don't set yres during vbe enabled
          if (!BX_VGA_THIS vbe.enabled)
          {
            // check for within max yres range
            if (value <= VBE_DISPI_MAX_YRES)
            {
              BX_VGA_THIS vbe.yres=(Bit16u) value;
              BX_INFO(("VBE set yres (%d)", value));
            }
            else
            {
              BX_INFO(("VBE set yres more then max yres (%d)", value));
            }
          }
          else
          {
            BX_ERROR(("VBE set yres during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_BPP: // set bpp
        {
          // check that we don't set bpp during vbe enabled
          if (!BX_VGA_THIS vbe.enabled)
          {
            // for backward compatiblity
            if (value == 0) value = VBE_DISPI_BPP_8;
            // check for correct bpp range
            if ((value == VBE_DISPI_BPP_4) || (value == VBE_DISPI_BPP_8) || (value == VBE_DISPI_BPP_15) ||
                (value == VBE_DISPI_BPP_16) || (value == VBE_DISPI_BPP_24) || (value == VBE_DISPI_BPP_32))
            {
              BX_VGA_THIS vbe.bpp=(Bit16u) value;
              BX_INFO(("VBE set bpp (%d)", value));
            }
            else
            {
              BX_ERROR(("VBE set bpp with unknown bpp (%d)", value));
            }
          }
          else
          {
            BX_ERROR(("VBE set bpp during vbe enabled!"));
          }
        } break;

        case VBE_DISPI_INDEX_BANK: // set bank
        {
          value=value & 0xff; // FIXME lobyte = vbe bank A?
          unsigned divider = (BX_VGA_THIS vbe.bpp!=VBE_DISPI_BPP_4)?64:256;
          // check for max bank nr
          if (value < (VBE_DISPI_TOTAL_VIDEO_MEMORY_KB / divider))
          {
            if (!BX_VGA_THIS vbe.lfb_enabled)
            {
              BX_DEBUG(("VBE set bank to %d", value));
              BX_VGA_THIS vbe.bank = value;
              BX_VGA_THIS s.plane_offset = (BX_VGA_THIS vbe.bank << 16);
            }
            else
            {
              BX_ERROR(("VBE set bank in LFB mode ignored"));
            }
          }
          else
          {
            BX_ERROR(("VBE set invalid bank (%d)", value));
          }
        } break;

        case VBE_DISPI_INDEX_ENABLE: // enable video
        {
          if ((value & VBE_DISPI_ENABLED) && !BX_VGA_THIS vbe.enabled)
          {
            unsigned depth=0;

            // setup virtual resolution to be the same as current reso
            BX_VGA_THIS vbe.virtual_yres=BX_VGA_THIS vbe.yres;
            BX_VGA_THIS vbe.virtual_xres=BX_VGA_THIS vbe.xres;

            // reset offset
            BX_VGA_THIS vbe.offset_x=0;
            BX_VGA_THIS vbe.offset_y=0;
            BX_VGA_THIS vbe.virtual_start=0;

            switch((BX_VGA_THIS vbe.bpp))
            {
              // Default pixel sizes
              case VBE_DISPI_BPP_8:
                BX_VGA_THIS vbe.bpp_multiplier = 1;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres;
                depth=8;
                break;

              case VBE_DISPI_BPP_4:
                BX_VGA_THIS vbe.bpp_multiplier = 1;
                BX_VGA_THIS s.line_offset = (BX_VGA_THIS vbe.virtual_xres >> 3);
                depth=4;
                break;

              case VBE_DISPI_BPP_15:
                BX_VGA_THIS vbe.bpp_multiplier = 2;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres * 2;
                depth=15;
                break;

              case VBE_DISPI_BPP_16:
                BX_VGA_THIS vbe.bpp_multiplier = 2;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres * 2;
                depth=16;
                break;

              case VBE_DISPI_BPP_24:
                BX_VGA_THIS vbe.bpp_multiplier = 3;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres * 3;
                depth=24;
                break;

              case VBE_DISPI_BPP_32:
                BX_VGA_THIS vbe.bpp_multiplier = 4;
                BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres << 2;
                depth=32;
                break;
            }
            BX_VGA_THIS vbe.visible_screen_size = BX_VGA_THIS s.line_offset * BX_VGA_THIS vbe.yres;

            BX_INFO(("VBE enabling x %d, y %d, bpp %d, %u bytes visible", BX_VGA_THIS vbe.xres, BX_VGA_THIS vbe.yres, BX_VGA_THIS vbe.bpp, BX_VGA_THIS vbe.visible_screen_size));

            if (depth > 4) {
              BX_VGA_THIS vbe.lfb_enabled = (bx_bool)((value & VBE_DISPI_LFB_ENABLED) != 0);
              if ((value & VBE_DISPI_NOCLEARMEM) == 0) {
                memset(BX_VGA_THIS s.memory, 0, BX_VGA_THIS vbe.visible_screen_size);
              }
              bx_gui->dimension_update(BX_VGA_THIS vbe.xres, BX_VGA_THIS vbe.yres, 0, 0, depth);
              BX_VGA_THIS s.last_bpp = depth;
            } else {
              BX_VGA_THIS s.plane_shift = VBE_DISPI_4BPP_PLANE_SHIFT;
              BX_VGA_THIS s.plane_offset = (BX_VGA_THIS vbe.bank << 16);
            }
          } else if (((value & VBE_DISPI_ENABLED) == 0) && BX_VGA_THIS vbe.enabled) {
            BX_INFO(("VBE disabling"));
            BX_VGA_THIS vbe.lfb_enabled = 0;
            BX_VGA_THIS s.plane_shift = 16;
            BX_VGA_THIS s.plane_offset = 0;
          }
          BX_VGA_THIS vbe.enabled = (bx_bool)((value & VBE_DISPI_ENABLED) != 0);
          BX_VGA_THIS vbe.get_capabilities = (bx_bool)((value & VBE_DISPI_GETCAPS) != 0);
          new_vbe_8bit_dac = (bx_bool)((value & VBE_DISPI_8BIT_DAC) != 0);
          if (new_vbe_8bit_dac != BX_VGA_THIS vbe.dac_8bit) {
            if (new_vbe_8bit_dac) {
              for (i=0; i<256; i++) {
                BX_VGA_THIS s.pel.data[i].red <<= 2;
                BX_VGA_THIS s.pel.data[i].green <<= 2;
                BX_VGA_THIS s.pel.data[i].blue <<= 2;
              }
              BX_INFO(("DAC in 8 bit mode"));
            } else {
              for (i=0; i<256; i++) {
                BX_VGA_THIS s.pel.data[i].red >>= 2;
                BX_VGA_THIS s.pel.data[i].green >>= 2;
                BX_VGA_THIS s.pel.data[i].blue >>= 2;
              }
              BX_INFO(("DAC in standard mode"));
            }
            BX_VGA_THIS vbe.dac_8bit = new_vbe_8bit_dac;
            BX_VGA_THIS s.dac_shift = new_vbe_8bit_dac ? 0 : 2;
            needs_update = 1;
          }
        } break;

        case VBE_DISPI_INDEX_X_OFFSET:
        {
          BX_DEBUG(("VBE offset x %d", value));
          BX_VGA_THIS vbe.offset_x=(Bit16u)value;

          BX_VGA_THIS vbe.virtual_start = BX_VGA_THIS vbe.offset_y * BX_VGA_THIS s.line_offset;
          if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
            BX_VGA_THIS vbe.virtual_start += (BX_VGA_THIS vbe.offset_x * BX_VGA_THIS vbe.bpp_multiplier);
          } else {
            BX_VGA_THIS vbe.virtual_start += (BX_VGA_THIS vbe.offset_x >> 3);
          }
          needs_update = 1;
        } break;

        case VBE_DISPI_INDEX_Y_OFFSET:
        {
          BX_DEBUG(("VBE offset y %d", value));

          Bit32u new_screen_start = value * BX_VGA_THIS s.line_offset;
          if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
            if ((new_screen_start + BX_VGA_THIS vbe.visible_screen_size) > VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES)
            {
              BX_PANIC(("VBE offset y %d out of bounds", value));
              break;
            }
            new_screen_start += (BX_VGA_THIS vbe.offset_x * BX_VGA_THIS vbe.bpp_multiplier);
          } else {
            if ((new_screen_start + BX_VGA_THIS vbe.visible_screen_size) > (VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES / 4))
            {
              BX_PANIC(("VBE offset y %d out of bounds", value));
              break;
            }
            new_screen_start += (BX_VGA_THIS vbe.offset_x >> 3);
          }
          BX_VGA_THIS vbe.virtual_start = new_screen_start;
          BX_VGA_THIS vbe.offset_y = (Bit16u)value;
          needs_update = 1;
        } break;

        case VBE_DISPI_INDEX_VIRT_WIDTH:
        {
          BX_INFO(("VBE requested virtual width %d", value));

          // calculate virtual width & height dimensions
          // req:
          //   virt_width > xres
          //   virt_height >=yres
          //   virt_width*virt_height < MAX_VIDEO_MEMORY

          // basicly 2 situations

          // situation 1:
          //   MAX_VIDEO_MEMORY / virt_width >= yres
          //        adjust result height
          //   else
          //        adjust result width based upon virt_height=yres
          Bit16u new_width=value;
          Bit16u new_height;
          if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
            new_height=(VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES / BX_VGA_THIS vbe.bpp_multiplier) / new_width;
          } else {
            new_height=(VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES * 2) / new_width;
          }
          if (new_height >=BX_VGA_THIS vbe.yres)
          {
            // we have a decent virtual width & new_height
            BX_INFO(("VBE decent virtual height %d",new_height));
          }
          else
          {
            // no decent virtual height: adjust width & height
            new_height=BX_VGA_THIS vbe.yres;
            if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
              new_width=(VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES / BX_VGA_THIS vbe.bpp_multiplier) / new_height;
            } else {
              new_width=(VBE_DISPI_TOTAL_VIDEO_MEMORY_BYTES * 2) / new_height;
            }

            BX_INFO(("VBE recalc virtual width %d height %d",new_width, new_height));
          }

          BX_VGA_THIS vbe.virtual_xres=new_width;
          BX_VGA_THIS vbe.virtual_yres=new_height;
          if (BX_VGA_THIS vbe.bpp != VBE_DISPI_BPP_4) {
            BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres * BX_VGA_THIS vbe.bpp_multiplier;
          } else {
            BX_VGA_THIS s.line_offset = BX_VGA_THIS vbe.virtual_xres >> 3;
          }
          BX_VGA_THIS vbe.visible_screen_size = BX_VGA_THIS s.line_offset * BX_VGA_THIS vbe.yres;

        } break;
        case VBE_DISPI_INDEX_VIRT_HEIGHT:
          BX_ERROR(("VBE: write to virtual height register ignored"));
          break;
        default:
          BX_ERROR(("VBE: write unsupported register at index 0x%x",BX_VGA_THIS vbe.curindex));
          break;
      }
      if (needs_update) {
        BX_VGA_THIS s.vga_mem_updated = 1;
        for (unsigned xti = 0; xti < BX_VGA_THIS s.num_x_tiles; xti++) {
          for (unsigned yti = 0; yti < BX_VGA_THIS s.num_y_tiles; yti++) {
            SET_TILE_UPDATED (xti, yti, 1);
          }
        }
      }
      break;

  } // end switch address
}

#if BX_SUPPORT_PCI
// pci configuration space read callback handler
Bit32u bx_vga_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_VGA_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%08x", address, value));

  return value;
}


// static pci configuration space write callback handler
void bx_vga_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  bx_bool baseaddr_change = 0, romaddr_change = 0;

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));

  if ((address >= 0x14) && (address < 0x30))
    return;

  if (address == 0x30) {
    value = value & 0xfffffc01;
    romaddr_change = 1;
  }

  for (unsigned i = 0; i < io_len; i++) {
    unsigned write_addr = address + i;
    Bit8u old_value = BX_VGA_THIS pci_conf[write_addr];
    Bit8u new_value = (Bit8u)(value & 0xff);
    switch (write_addr) {
      case 0x04: // disallowing write to command
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      case 0x10: // base address #0
        new_value = (new_value & 0xf0) | (old_value & 0x0f);
      case 0x11: case 0x12: case 0x13:
        if (BX_VGA_THIS vbe_present) {
          baseaddr_change |= (old_value != new_value);
        } else {
          break;
        }
      default:
        BX_VGA_THIS pci_conf[write_addr] = new_value;
    }
    value >>= 8;
  }

  if (baseaddr_change) {
    if (BX_VGA_THIS vbe_set_base_addr(&BX_VGA_THIS pci_base_address[0],
                                      &BX_VGA_THIS pci_conf[0x10])) {
      BX_INFO(("new base address: 0x%08x", BX_VGA_THIS pci_base_address[0]));
    }
  }
  if (romaddr_change) {
    if (DEV_pci_set_base_mem(this, mem_read_handler, mem_write_handler,
                             &BX_VGA_THIS pci_rom_address,
                             &BX_VGA_THIS pci_conf[0x30],
                             BX_VGA_THIS pci_rom_size)) {
      BX_INFO(("new ROM address: 0x%08x", BX_VGA_THIS pci_rom_address));
    }
  }
}
#endif

#if BX_DEBUGGER
void bx_vga_c::debug_dump(int argc, char **argv)
{
  if (BX_VGA_THIS vbe.enabled) {
    dbg_printf("Bochs VGA/VBE adapter\n\n");
    dbg_printf("current mode : %u x %u x %u\n", BX_VGA_THIS vbe.xres,
               BX_VGA_THIS vbe.yres, BX_VGA_THIS vbe.bpp);
  } else {
    bx_vgacore_c::debug_dump();
  }
  if (argc > 0) {
    dbg_printf("\nAdditional options not supported\n");
  }
}
#endif
