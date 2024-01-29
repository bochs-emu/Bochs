/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2017-2024  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

/*
 *  Portion of this software comes with the following license
 */

/***************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

// 3dfx Voodoo Banshee / Voodoo3 emulation (partly based on a patch for DOSBox)

// TODO:
// - 2D host-to-screen stretching support
// - 2D screen-to-screen pattern stretching support
// - 2D chromaKey support
// - 2D reversible line drawing
// - pixel format conversion not supported in all cases
// - full AGP support

// FIXME:
// - Display errors in 16 bpp mode after leaving 3D mode
// - Display errors in 16 bpp mode with debug messages turned on (timing issue)
// - Bochs crashes on Windows host (MSVC in some cases, MSYS2 64-bit build also
//   reported, but not yet reproduced)

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_VOODOO

#include "pci.h"
#include "vgacore.h"
#include "ddc.h"
#include "voodoo.h"
#include "virt_timer.h"
#include "bxthread.h"
#define BX_USE_TERNARY_ROP
#include "bitblt.h"

#define LOG_THIS theVoodooDevice->

const Bit8u banshee_iomask[256] = {4,0,0,0,7,1,3,1,7,1,3,1,7,1,3,1,7,1,3,1,
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

const Bit8u pxconv_table[16] = {0x3a,0x02,0x00,0x38,0x38,0x38,0x00,0x00,
                                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#include "voodoo_types.h"
#include "voodoo_data.h"
#include "voodoo_main.h"

// Extern and forward declarations
extern voodoo_state *v;
extern BX_MUTEX(cmdfifo_mutex);
extern BX_MUTEX(render_mutex);
Bit32u register_r(Bit32u offset);
void register_w(Bit32u offset, Bit32u data, bool log);
void register_w_common(Bit32u offset, Bit32u data);
Bit32u lfb_r(Bit32u offset);
Bit32u lfb_w(Bit32u offset, Bit32u data, Bit32u mem_mask);
Bit32s texture_w(Bit32u offset, Bit32u data);
void cmdfifo_w(cmdfifo_info *f, Bit32u fbi_offset, Bit32u data);

// the class bx_banshee_c

bx_banshee_c::bx_banshee_c() : bx_voodoo_base_c()
{
}

bx_banshee_c::~bx_banshee_c()
{
  SIM->get_bochs_root()->remove("voodoo");
}

void bx_banshee_c::init_model(void)
{
  static char model[40];
  const char *vgabios_signature;

  if (theVoodooVga == NULL) {
    BX_PANIC(("Voodoo Banshee with VGA disabled not supported yet"));
  }
  is_agp = SIM->is_agp_device(BX_PLUGIN_VOODOO);
  if (s.model == VOODOO_BANSHEE) {
    if (!is_agp) {
      strcpy(model, "Experimental 3dfx Voodoo Banshee PCI");
    } else {
      strcpy(model, "Experimental 3dfx Voodoo Banshee AGP");
    }
    DEV_register_pci_handlers2(this, &s.devfunc, BX_PLUGIN_VOODOO, model, is_agp);
    init_pci_conf(0x121a, 0x0003, 0x01, 0x030000, 0x00, BX_PCI_INTA);
  } else if (s.model == VOODOO_3) {
    if (!is_agp) {
      strcpy(model, "Experimental 3dfx Voodoo 3 PCI");
    } else {
      strcpy(model, "Experimental 3dfx Voodoo 3 AGP");
    }
    DEV_register_pci_handlers2(this, &s.devfunc, BX_PLUGIN_VOODOO, model, is_agp);
    init_pci_conf(0x121a, 0x0005, 0x01, 0x030000, 0x00, BX_PCI_INTA);
  } else {
    BX_PANIC(("Unknown Voodoo Banshee compatible model"));
  }
  pci_conf[0x14] = 0x08;
  init_bar_mem(0, 0x2000000, mem_read_handler, mem_write_handler);
  init_bar_mem(1, 0x2000000, mem_read_handler, mem_write_handler);
  init_bar_io(2, 256, read_handler, write_handler, banshee_iomask);
  pci_rom_address = 0;
  pci_rom_read_handler = mem_read_handler;
  load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());
  // Zero out Banshee i/o registers and init strapInfo
  memset(v->banshee.io, 0, sizeof(v->banshee.io));
  v->banshee.io[io_strapInfo] = 0x00000060;
  if (is_agp) {
    v->banshee.io[io_strapInfo] |= 0x0000000c;
  }
  if (pci_rom_size > 0x8000) {
    v->banshee.io[io_strapInfo] |= 0x00000002;
  }
  // Modify device IDs in LGPL'd VGABIOS if required
  vgabios_signature = (const char*)(pci_rom + 0x22);
  if (!strncmp(vgabios_signature, "Bochs VGABios (PCI)", 19)) {
    Bit16u pcir = pci_rom[0x18] | (pci_rom[0x19] << 8); // offset to PCIR data
    Bit16u pci_vid = pci_rom[pcir + 4] | (pci_rom[pcir + 5] << 8);
    if (pci_vid == 0x121a) {
      if (s.model == VOODOO_BANSHEE) {
        pci_rom[0x7ffa] = is_agp ? 0x03:0x04;
      } else {
        pci_rom[pcir + 6] = 0x05;
        pci_rom[0x7ffa] = is_agp ? 0x52:0x36;
      }
      Bit8u checksum = 0;
      for (int i = 0; i < 0x7fff; i++) {
        checksum += pci_rom[i];
      }
      pci_rom[0x7fff] = -checksum;
    }
  }
}

void bx_banshee_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals2[] = {
    { 0x04, 0x00 }, { 0x05, 0x00 }, // command io / memory
    { 0x06, 0x10 }, { 0x07, 0x00 }, // status
    // capabilities pointer 0x34 - 0x37
    { 0x34, 0x60 }, { 0x35, 0x00 },
    { 0x36, 0x00 }, { 0x37, 0x00 },
    { 0x3c, 0x00 },                 // IRQ
    // ACPI capabilities ID 0x60 - 0x63
    { 0x60, 0x01 }, { 0x61, 0x00 },
    { 0x62, 0x21 }, { 0x63, 0x00 },
    // ACPI control/status 0x64 - 0x67
    { 0x64, 0x00 }, { 0x65, 0x00 },
    { 0x66, 0x00 }, { 0x67, 0x00 },
  };
  for (i = 0; i < sizeof(reset_vals2) / sizeof(*reset_vals2); ++i) {
    pci_conf[reset_vals2[i].addr] = reset_vals2[i].val;
  }
  // AGP reported by PCI status, new capabilities and strapInfo
  if (is_agp) {
    pci_conf[0x06] |= 0x20;
    pci_conf[0x34] = 0x54;
    pci_conf[0x54] = 0x02;
    pci_conf[0x55] = 0x60;
    pci_conf[0x56] = 0x10;
    pci_conf[0x57] = 0x00;
    if (s.model == VOODOO_3) {
      pci_conf[0x58] = 0x23;
    } else {
      pci_conf[0x58] = 0x21;
    }
    pci_conf[0x59] = 0x02;
    pci_conf[0x5b] = 0x07;
  }
  for (i = 0; i < 4; i++) {
    pci_conf[0x2c + i] = pci_rom[(pci_rom_size - 8) + i];
  }
  v->banshee.io[io_pciInit0] = 0x01800040;
  v->banshee.io[io_sipMonitor] = 0x40000000;
  v->banshee.io[io_lfbMemoryConfig] = 0x000a2200;
  v->banshee.io[io_miscInit1] = ((v->banshee.io[io_strapInfo] & 0x1f) << 24);
  v->banshee.io[io_dramInit0] = 0x00579d29 | ((v->banshee.io[io_strapInfo] & 0x60) << 21);
  v->banshee.io[io_dramInit1] = 0x00f02200;
  v->banshee.io[io_tmuGbeInit] = 0x00000bfb;
  v->vidclk = 14318180;
  if (theVoodooVga != NULL) {
    theVoodooVga->banshee_set_vclk3((Bit32u)v->vidclk);
  }

  // Deassert IRQ
  set_irq_level(0);
}

void bx_banshee_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "voodoo", "Voodoo Banshee State");
  voodoo_register_state(list);
  bx_list_c *banshee = new bx_list_c(list, "banshee", "Banshee State");
  new bx_shadow_data_c(banshee, "io", (Bit8u*)v->banshee.io, 256, 1);
  new bx_shadow_data_c(banshee, "agp", (Bit8u*)v->banshee.agp, 0x80, 1);
  new bx_shadow_data_c(banshee, "crtc", (Bit8u*)v->banshee.crtc, 0x27, 1);
  new bx_shadow_num_c(banshee, "disp_bpp", &v->banshee.disp_bpp);
  BXRS_PARAM_BOOL(banshee, half_mode, v->banshee.half_mode);
  BXRS_PARAM_BOOL(banshee, double_width, v->banshee.double_width);
  BXRS_PARAM_BOOL(banshee, dac_8bit, v->banshee.dac_8bit);
  BXRS_PARAM_BOOL(banshee, desktop_tiled, v->banshee.desktop_tiled);
  BXRS_PARAM_BOOL(banshee, overlay_tiled, v->banshee.overlay_tiled);
  BXRS_PARAM_BOOL(banshee, hwcursor_enabled, v->banshee.hwcursor.enabled);
  BXRS_PARAM_BOOL(banshee, hwcursor_mode, v->banshee.hwcursor.mode);
  new bx_shadow_num_c(banshee, "hwcursor_addr", &v->banshee.hwcursor.addr, BASE_HEX);
  new bx_shadow_num_c(banshee, "hwcursor_x", &v->banshee.hwcursor.x, BASE_HEX);
  new bx_shadow_num_c(banshee, "hwcursor_y", &v->banshee.hwcursor.y, BASE_HEX);
  new bx_shadow_num_c(banshee, "hwcursor_color0", &v->banshee.hwcursor.color[0], BASE_HEX);
  new bx_shadow_num_c(banshee, "hwcursor_color1", &v->banshee.hwcursor.color[1], BASE_HEX);
  new bx_shadow_data_c(banshee, "blt_reg", (Bit8u*)v->banshee.blt.reg, 0x80, 1);
  new bx_shadow_data_c(banshee, "blt_cpat", (Bit8u*)v->banshee.blt.cpat, 0x100, 1);
  BXRS_PARAM_BOOL(banshee, blt_busy, v->banshee.blt.busy);
  new bx_shadow_num_c(banshee, "blt_cmd", &v->banshee.blt.cmd);
  BXRS_PARAM_BOOL(banshee, blt_immed, v->banshee.blt.immed);
  BXRS_PARAM_BOOL(banshee, blt_x_dir, v->banshee.blt.x_dir);
  BXRS_PARAM_BOOL(banshee, blt_y_dir, v->banshee.blt.y_dir);
  BXRS_PARAM_BOOL(banshee, blt_transp, v->banshee.blt.transp);
  new bx_shadow_num_c(banshee, "blt_patsx", &v->banshee.blt.patsx);
  new bx_shadow_num_c(banshee, "blt_patsy", &v->banshee.blt.patsy);
  BXRS_PARAM_BOOL(banshee, blt_clip_sel, v->banshee.blt.clip_sel);
  new bx_shadow_num_c(banshee, "blt_rop0", &v->banshee.blt.rop[0]);
  new bx_shadow_num_c(banshee, "blt_rop1", &v->banshee.blt.rop[1]);
  new bx_shadow_num_c(banshee, "blt_rop2", &v->banshee.blt.rop[2]);
  new bx_shadow_num_c(banshee, "blt_rop3", &v->banshee.blt.rop[3]);
  new bx_shadow_num_c(banshee, "blt_src_base", &v->banshee.blt.src_base, BASE_HEX);
  BXRS_PARAM_BOOL(banshee, blt_src_tiled, v->banshee.blt.src_tiled);
  new bx_shadow_num_c(banshee, "blt_src_fmt", &v->banshee.blt.src_fmt);
  new bx_shadow_num_c(banshee, "blt_src_pitch", &v->banshee.blt.src_pitch);
  new bx_shadow_num_c(banshee, "blt_src_swizzle", &v->banshee.blt.src_swizzle);
  new bx_shadow_num_c(banshee, "blt_src_x", &v->banshee.blt.src_x);
  new bx_shadow_num_c(banshee, "blt_src_y", &v->banshee.blt.src_y);
  new bx_shadow_num_c(banshee, "blt_src_w", &v->banshee.blt.src_w);
  new bx_shadow_num_c(banshee, "blt_src_h", &v->banshee.blt.src_h);
  new bx_shadow_num_c(banshee, "blt_dst_base", &v->banshee.blt.dst_base, BASE_HEX);
  BXRS_PARAM_BOOL(banshee, blt_dst_tiled, v->banshee.blt.dst_tiled);
  new bx_shadow_num_c(banshee, "blt_dst_fmt", &v->banshee.blt.dst_fmt);
  new bx_shadow_num_c(banshee, "blt_dst_pitch", &v->banshee.blt.dst_pitch);
  new bx_shadow_num_c(banshee, "blt_dst_x", &v->banshee.blt.dst_x);
  new bx_shadow_num_c(banshee, "blt_dst_y", &v->banshee.blt.dst_y);
  new bx_shadow_num_c(banshee, "blt_dst_w", &v->banshee.blt.dst_w);
  new bx_shadow_num_c(banshee, "blt_dst_h", &v->banshee.blt.dst_h);
  new bx_shadow_num_c(banshee, "blt_fgcolor", (Bit32u*)&v->banshee.blt.fgcolor, BASE_HEX);
  new bx_shadow_num_c(banshee, "blt_bgcolor", (Bit32u*)&v->banshee.blt.bgcolor, BASE_HEX);
  new bx_shadow_num_c(banshee, "blt_clipx0_0", &v->banshee.blt.clipx0[0]);
  new bx_shadow_num_c(banshee, "blt_clipx0_1", &v->banshee.blt.clipx0[1]);
  new bx_shadow_num_c(banshee, "blt_clipy0_0", &v->banshee.blt.clipy0[0]);
  new bx_shadow_num_c(banshee, "blt_clipy0_1", &v->banshee.blt.clipy0[1]);
  new bx_shadow_num_c(banshee, "blt_clipx1_0", &v->banshee.blt.clipx1[0]);
  new bx_shadow_num_c(banshee, "blt_clipx1_1", &v->banshee.blt.clipx1[1]);
  new bx_shadow_num_c(banshee, "blt_clipy1_0", &v->banshee.blt.clipy1[0]);
  new bx_shadow_num_c(banshee, "blt_clipy1_1", &v->banshee.blt.clipy1[1]);
  new bx_shadow_num_c(banshee, "blt_h2s_pitch", &v->banshee.blt.h2s_pitch);
  new bx_shadow_num_c(banshee, "blt_h2s_pxstart", &v->banshee.blt.h2s_pxstart);
}

void bx_banshee_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(mem_read_handler);
  if ((v->banshee.io[io_vidProcCfg] & 0x01) && (theVoodooVga != NULL)) {
    v->fbi.clut_dirty = 1;
    theVoodooVga->banshee_update_mode();
  }
  start_fifo_thread();
}

bool bx_banshee_c::update_timing(void)
{
  float hfreq;
  bx_crtc_params_t crtcp;
  Bit32u vclock = 0;

  BX_VVGA_THIS get_crtc_params(&crtcp, &vclock);
  hfreq = vclock / (float)(crtcp.htotal * 8);
  v->vertfreq = hfreq / (float)crtcp.vtotal;
  s.vdraw.vtotal_usec = (unsigned)(1000000.0 / v->vertfreq);
  s.vdraw.width = v->fbi.width;
  s.vdraw.height = v->fbi.height;
  vertical_timer_handler(this);
  bx_virt_timer.activate_timer(s.vertical_timer_id, (Bit32u)s.vdraw.vtotal_usec, 1);
  if (BX_VVGA_THIS get_update_mode()) {
    BX_VVGA_THIS set_update_timer(0);
  }
  return 1;
}

void bx_banshee_c::set_tile_updated(unsigned xti, unsigned yti, bool flag)
{
  SET_TILE_UPDATED(, xti, yti, flag);
}

void bx_banshee_c::draw_hwcursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  unsigned cx, cy, cw, ch, px, py, w, h, x, y;
  Bit8u *cpat0, *cpat1, *tile_ptr, *tile_ptr2, *vid_ptr;
  Bit8u ccode, pbits, pval0, pval1;
  Bit32u colour = 0, start;
  Bit16u index, pitch;
  Bit16u hwcx = v->banshee.hwcursor.x;
  Bit16u hwcy = v->banshee.hwcursor.y;
  Bit8u hwcw = 63;
  int i;

  if (v->banshee.double_width) {
    hwcx <<= 1;
    hwcw <<= 1;
  }
  if ((xc <= hwcx) && ((int)(xc + X_TILESIZE) > (hwcx - hwcw)) &&
      (yc <= hwcy) && ((int)(yc + Y_TILESIZE) > (hwcy - 63))) {
    if ((v->banshee.io[io_vidProcCfg] & 0x181) == 0x81) {
      start = v->banshee.io[io_vidDesktopStartAddr];
      pitch = v->banshee.io[io_vidDesktopOverlayStride] & 0x7fff;
    } else {
      start = v->fbi.rgboffs[0];
      pitch = (v->banshee.io[io_vidDesktopOverlayStride] >> 16) & 0x7fff;
    }
    Bit8u *disp_ptr = &v->fbi.ram[start & v->fbi.mask];
    if (v->banshee.desktop_tiled) {
      pitch *= 128;
    }
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);

    if ((hwcx - hwcw) < (int)xc) {
      cx = xc;
      if ((hwcx - xc + 1) > w) {
        cw = w;
      } else {
        cw = hwcx - xc + 1;
      }
      px = hwcw - (hwcx - xc);
    } else {
      cx = hwcx - hwcw;
      cw = w - (hwcx - hwcw - xc);
      px = 0;
    }
    if ((hwcy - 63) < (int)yc) {
      cy = yc;
      if ((hwcy - yc + 1) > h) {
        ch = h;
      } else {
        ch = hwcy - yc + 1;
      }
      py = 63 - (hwcy - yc);
    } else {
      cy = hwcy - 63;
      ch = h - (hwcy - 63 - yc);
      py = 0;
    }
    tile_ptr += ((cy - yc) * info->pitch);
    tile_ptr += ((cx - xc) * (info->bpp >> 3));
    cpat0 = &v->fbi.ram[v->banshee.hwcursor.addr] + (py * 16);
    for (y = cy; y < (cy + ch); y++) {
      if (v->banshee.double_width) {
        cpat1 = cpat0 + (px >> 4);
        pbits = 8 - ((px >> 1) & 7);
      } else {
        cpat1 = cpat0 + (px >> 3);
        pbits = 8 - (px & 7);
      }
      tile_ptr2 = tile_ptr;
      for (x = cx; x < (cx + cw); x++) {
        pval0 = (*cpat1 >> (pbits - 1)) & 1;
        pval1 = (*(cpat1 + 8) >> (pbits - 1)) & 1;
        ccode = pval0 + (pval1 << 1) + (v->banshee.hwcursor.mode << 2);
        if ((ccode == 0) || (ccode == 5)) {
          colour = v->banshee.hwcursor.color[0];
        } else if ((ccode == 2) || (ccode == 7)) {
          colour = v->banshee.hwcursor.color[1];
        } else {
          if (v->banshee.half_mode) {
            if (v->banshee.double_width) {
              vid_ptr = disp_ptr + (y >> 1) * pitch + (x >> 1) * (v->banshee.disp_bpp >> 3);
            } else {
              vid_ptr = disp_ptr + (y >> 1) * pitch + x * (v->banshee.disp_bpp >> 3);
            }
          } else {
            vid_ptr = disp_ptr + y * pitch + x * (v->banshee.disp_bpp >> 3);
          }
          switch (v->banshee.disp_bpp) {
            case 8:
              if (info->is_indexed) {
                colour = *vid_ptr;
              } else {
                colour = v->fbi.clut[*vid_ptr];
              }
              break;
            case 16:
              index = *(vid_ptr++);
              index |= *(vid_ptr++) << 8;
              colour = v->fbi.pen[index];
              break;
            case 24:
            case 32:
              colour = *vid_ptr;
              colour |= (*(vid_ptr + 1)) << 8;
              colour |= (*(vid_ptr + 2)) << 16;
              break;
          }
          if (ccode == 3) colour ^= 0xffffff;
        }
        if (!info->is_indexed) {
          colour = MAKE_COLOUR(
            colour, 24, info->red_shift, info->red_mask,
            colour, 16, info->green_shift, info->green_mask,
            colour, 8, info->blue_shift, info->blue_mask);
          if (info->is_little_endian) {
            for (i=0; i<info->bpp; i+=8) {
              *(tile_ptr2++) = (Bit8u)(colour >> i);
            }
          } else {
            for (i=info->bpp-8; i>-8; i-=8) {
              *(tile_ptr2++) = (Bit8u)(colour >> i);
            }
          }
        } else {
          *(tile_ptr2++) = (Bit8u)colour;
        }
        if (!v->banshee.double_width || (x & 1)) {
          if (--pbits == 0) {
            cpat1++;
            pbits = 8;
          }
        }
      }
      cpat0 += 16;
      tile_ptr += info->pitch;
    }
  }
}

Bit32u bx_banshee_c::get_retrace(bool hv)
{
  return theVoodooVga->get_retrace();
}

void bx_banshee_c::reg_write(Bit32u reg, Bit32u value)
{
  if ((reg >> 11) & 1) {
    blt_reg_write(reg & 0xff, value);
  } else {
    register_w(reg, value, 1);
  }
}

// pci configuration space write handler
void bx_banshee_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;

  if ((address >= 0x1c) && (address < 0x2c))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x23;
        break;
      case 0x2c:
      case 0x2d:
      case 0x2e:
      case 0x2f:
        if ((v->banshee.io[io_miscInit1] & 0x08) == 0) {
          value8 = oldval;
        }
        break;
      case 0x06:
      case 0x07:
        value8 = oldval;
        break;
      default:
        if (address >= 0x54) {
          value8 = oldval;
        }
    }
    pci_conf[address+i] = value8;
  }
}

Bit32u bx_banshee_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  bx_banshee_c *class_ptr = (bx_banshee_c*)this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_banshee_c::read(Bit32u address, unsigned io_len)
{
  static Bit8u lastreg = 0xff;
  Bit32u result;

  Bit8u offset = (Bit8u)(address & 0xff);
  Bit8u reg = (offset >> 2);
  switch (reg) {
    case io_status:
      result = register_r(0);
      break;

    case io_dacData:
      result = v->fbi.clut[v->banshee.io[io_dacAddr] & 0x1ff] = v->banshee.io[reg];
      break;

    case io_vgab0:  case io_vgab4:  case io_vgab8:  case io_vgabc:
    case io_vgac0:  case io_vgac4:  case io_vgac8:  case io_vgacc:
    case io_vgad0:  case io_vgad4:  case io_vgad8:  case io_vgadc:
      result = 0;
      // i/o read only, not memory mapped
      if ((theVoodooVga != NULL) && (address & 0xff00)) {
        for (unsigned i=0; i<io_len; i++) {
          result |= (theVoodooVga->banshee_vga_read_handler(theVoodooVga, 0x300+offset+i, 1) << (i*8));
        }
      }
      break;

    case io_vidSerialParallelPort:
      result = v->banshee.io[reg] & 0xf387ffff;
      if ((v->banshee.io[reg] >> 18) & 1) {
        result |= ((Bit32u)ddc.read() << 19);
      } else {
        result |= 0x00780000;
      }
      if ((v->banshee.io[reg] >> 23) & 1) {
        result |= ((v->banshee.io[reg] & 0x03000000) << 2);
      } else {
        result |= 0x0f000000;
      }
      break;

    default:
      result = v->banshee.io[reg];
      break;
  }
  if ((reg < io_vgab0) || (reg > io_vgadc)) {
    if ((offset & 3) != 0) {
      result >>= ((offset & 3) * 8);
    }
  }
  if ((reg != io_status) || (lastreg != io_status)) {
    BX_DEBUG(("banshee read from offset 0x%02x (%s) result = 0x%08x", offset,
              banshee_io_reg_name[reg], result));
  }
  lastreg = reg;
  return result;
}

void bx_banshee_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  bx_banshee_c *class_ptr = (bx_banshee_c*)this_ptr;
  return class_ptr->write(address, value, io_len);
}

void bx_banshee_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
  Bit8u offset = (Bit8u)(address & 0xff);
  Bit8u reg = (offset>>2), dac_idx, k, m, n, shift;
  Bit32u old = v->banshee.io[reg], mask;
  bool prev_hwce = v->banshee.hwcursor.enabled;
  Bit16u prev_hwcx = v->banshee.hwcursor.x;
  Bit16u prev_hwcy = v->banshee.hwcursor.y;
  Bit16u x0, y0;
  bool mode_change = false;

  BX_DEBUG(("banshee write to offset 0x%02x: value = 0x%08x len=%d (%s)", offset, value,
            io_len, banshee_io_reg_name[reg]));
  if ((reg < io_vgab0) || (reg > io_vgadc)) {
    if (io_len == 1) {
      shift = ((offset & 3) * 8);
      mask = ~(0xff << shift);
      value = (old & mask) | (value << shift);
    } else if (io_len == 2) {
      shift = ((offset & 2) * 8);
      mask = ~(0xffff << shift);
      value = (old & mask) | (value << shift);
    }
  }
  switch (reg) {
    case io_lfbMemoryConfig:
      v->banshee.io[reg] = value;
      v->fbi.lfb_base = (value & 0x1fff) << 12;
      v->fbi.lfb_stride = ((value >> 13) & 7) + 10;
      break;

    case io_miscInit0:
      v->banshee.io[reg] = value;
      v->fbi.yorigin = (value >> 18) & 0xfff;
      break;

    case io_miscInit1:
      v->banshee.io[reg] = value & 0xffffff;
      v->banshee.io[reg] |= ((v->banshee.io[io_strapInfo] & 0x1f) << 24);
      break;

    case io_vgaInit0:
      v->banshee.io[reg] = value;
      if (theVoodooVga != NULL) {
        theVoodooVga->banshee_set_dac_mode((v->banshee.io[reg] & 0x04) != 0);
      }
      break;

    case io_dramCommand:
      blt_reg_write(0x1c, value);
      break;

    case io_dramData:
      blt_reg_write(0x19, value);
      break;

    case io_strapInfo:
      break;

    case io_pllCtrl0:
      if (value != v->banshee.io[reg]) {
        v->banshee.io[reg] = value;
        k = (Bit8u)(value & 0x03);
        m = (Bit8u)((value >> 2) & 0x3f);
        n = (Bit8u)((value >> 8) & 0xff);
        v->vidclk = (float)(14318180.0f * ((double)n + 2.0f) / ((double)m + 2.0f) / (double)(1 << k));
        BX_INFO(("Setting VCLK #3 (pllCtrl0) = %.3f MHz", v->vidclk / 1000000.0f));
        if (theVoodooVga != NULL) {
          theVoodooVga->banshee_set_vclk3((Bit32u)v->vidclk);
        }
      }
      break;

    case io_dacData:
      v->banshee.io[reg] = value;
      if (v->banshee.io[reg] != v->fbi.clut[v->banshee.io[io_dacAddr] & 0x1ff]) {
        v->fbi.clut[v->banshee.io[io_dacAddr] & 0x1ff] = v->banshee.io[reg];
        v->fbi.clut_dirty = 1;
        if (v->banshee.io[io_dacAddr] <= 0xff) {
          dac_idx = v->banshee.io[io_dacAddr] & 0xff;
          bx_gui->palette_change_common(dac_idx, (v->fbi.clut[dac_idx] >> 16) & 0xff,
                                        (v->fbi.clut[dac_idx] >> 8) & 0xff,
                                        v->fbi.clut[dac_idx] & 0xff);
        }
      }
      break;

    case io_vidProcCfg:
      v->banshee.io[reg] = value;
      if ((v->banshee.io[reg] ^ old) & 0x3d80)
        v->fbi.clut_dirty = 1;
      if ((v->banshee.io[reg] & 0x01) && ((old & 0x01) == 0x00)) {
        if (theVoodooVga != NULL) {
          theVoodooVga->banshee_update_mode();
        }
        mode_change = true;
      } else if (!(v->banshee.io[reg] & 0x01) && ((old & 0x01) == 0x01)) {
        bx_virt_timer.deactivate_timer(s.vertical_timer_id);
        v->vtimer_running = 0;
      }
      if ((v->banshee.io[reg] & 0x01) && ((v->banshee.io[reg] & 0x180) != (old & 0x180))) {
        mode_change = true;
      }
      if (mode_change) {
        if ((v->banshee.io[reg] & 0x180) == 0x080) {
          BX_INFO(("2D desktop mode enabled"));
        } else if ((v->banshee.io[reg] & 0x180) == 0x100) {
          BX_INFO(("3D overlay mode enabled"));
          v->vtimer_running = 1;
        }
      }
      v->banshee.hwcursor.enabled = ((v->banshee.io[reg] >> 27) & 1);
      v->banshee.hwcursor.mode = ((v->banshee.io[reg] >> 1) & 1);
      if (v->banshee.hwcursor.enabled != prev_hwce) {
        x0 = (v->banshee.hwcursor.x < 63) ? 0 : (v->banshee.hwcursor.x - 63);
        y0 = (v->banshee.hwcursor.y < 63) ? 0 : (v->banshee.hwcursor.y - 63);
        if (v->banshee.double_width) {
          theVoodooVga->redraw_area(x0 << 1, y0, 64, 64);
        } else {
          theVoodooVga->redraw_area(x0, y0, 64, 64);
        }
      }
      if ((v->banshee.io[reg] >> 2) & 1) {
        BX_ERROR(("vidProcCfg: overlay stereo mode not supported yet"));
      }
      if ((v->banshee.io[reg] >> 5) & 1) {
        BX_ERROR(("vidProcCfg: chromaKey mode not supported yet"));
      }
      if ((v->banshee.io[reg] >> 16) & 3) {
        BX_ERROR(("vidProcCfg: overlay filter mode %d not supported yet", (v->banshee.io[reg] >> 16) & 3));
      }
      v->banshee.desktop_tiled = ((v->banshee.io[reg] >> 24) & 1);
      v->banshee.overlay_tiled = ((v->banshee.io[reg] >> 25) & 1);
      break;

    case io_hwCurPatAddr:
      v->banshee.io[reg] = value;
      v->banshee.hwcursor.addr = v->banshee.io[reg] & 0xffffff;
      if (v->banshee.hwcursor.enabled && (value != old)) {
        x0 = (v->banshee.hwcursor.x < 63) ? 0 : (v->banshee.hwcursor.x - 63);
        y0 = (v->banshee.hwcursor.y < 63) ? 0 : (v->banshee.hwcursor.y - 63);
        if (v->banshee.double_width) {
          theVoodooVga->redraw_area(x0 << 1, y0, 64, 64);
        } else {
          theVoodooVga->redraw_area(x0, y0, 64, 64);
        }
      }
      break;

    case io_hwCurLoc:
      v->banshee.io[reg] = value;
      v->banshee.hwcursor.x = v->banshee.io[reg] & 0x7ff;
      v->banshee.hwcursor.y = (v->banshee.io[reg] >> 16) & 0x7ff;
      if (v->banshee.hwcursor.enabled && (value != old)) {
        x0 = (prev_hwcx < 63) ? 0 : (prev_hwcx - 63);
        y0 = (prev_hwcy < 63) ? 0 : (prev_hwcy - 63);
        if (v->banshee.double_width) {
          theVoodooVga->redraw_area(x0 << 1, y0, 64, 64);
        } else {
          theVoodooVga->redraw_area(x0, y0, 64, 64);
        }
        x0 = (v->banshee.hwcursor.x < 63) ? 0 : (v->banshee.hwcursor.x - 63);
        y0 = (v->banshee.hwcursor.y < 63) ? 0 : (v->banshee.hwcursor.y - 63);
        if (v->banshee.double_width) {
          theVoodooVga->redraw_area(x0 << 1, y0, 64, 64);
        } else {
          theVoodooVga->redraw_area(x0, y0, 64, 64);
        }
      }
      break;

    case io_hwCurC0:
      v->banshee.io[reg] = value;
      v->banshee.hwcursor.color[0] = v->banshee.io[reg] & 0xffffff;
      break;

    case io_hwCurC1:
      v->banshee.io[reg] = value;
      v->banshee.hwcursor.color[1] = v->banshee.io[reg] & 0xffffff;
      break;

    case io_vidSerialParallelPort:
      v->banshee.io[reg] = value;
      if ((v->banshee.io[reg] >> 18) & 1) {
        ddc.write((v->banshee.io[reg] >> 19) & 1, (v->banshee.io[reg] >> 20) & 1);
      }
      break;

    case io_vidScreenSize:
      v->banshee.io[reg] = value;
      v->fbi.width = (value & 0xfff);
      v->fbi.height = (value >> 12) & 0xfff;
      break;

    case io_vgab0:  case io_vgab4:  case io_vgab8:  case io_vgabc:
    case io_vgac0:  case io_vgac4:  case io_vgac8:  case io_vgacc:
    case io_vgad0:  case io_vgad4:  case io_vgad8:  case io_vgadc:
      // i/o write only, not memory mapped
      if ((theVoodooVga != NULL) && (address & 0xff00)) {
        for (unsigned i=0; i<io_len; i++) {
          Bit8u value8 = (value >> (i*8)) & 0xff;
          theVoodooVga->banshee_vga_write_handler(theVoodooVga, 0x300+offset+i, value8, 1);
        }
      }
      break;

    case io_vidDesktopStartAddr:
    case io_vidDesktopOverlayStride:
      if ((v->banshee.io[io_vidProcCfg] & 0x01) && (v->banshee.io[reg] != value)) {
        v->fbi.video_changed = 1;
      }
      v->banshee.io[reg] = value;
      break;

    default:
      v->banshee.io[reg] = value;
      break;
  }
}

bool bx_banshee_c::mem_read_handler(bx_phy_address addr, unsigned len,
                                    void *data, void *param)
{
  bx_banshee_c *class_ptr = (bx_banshee_c*)param;
  class_ptr->mem_read(addr, len, data);
  return 1;
}

bool bx_banshee_c::mem_write_handler(bx_phy_address addr, unsigned len,
                                     void *data, void *param)
{
  bx_banshee_c *class_ptr = (bx_banshee_c*)param;
  if (len == 16) {
    Bit64u *data64 = (Bit64u*)data;
#ifdef BX_LITTLE_ENDIAN
    class_ptr->mem_write(addr, 8, &data64[0]);
    class_ptr->mem_write(addr + 8, 8, &data64[1]);
#else
    class_ptr->mem_write(addr, 8, &data64[1]);
    class_ptr->mem_write(addr + 8, 8, &data64[0]);
#endif
  } else {
    class_ptr->mem_write(addr, len, data);
  }
  return 1;
}

void bx_banshee_c::mem_read(bx_phy_address addr, unsigned len, void *data)
{
  Bit64u value = BX_MAX_BIT64U;
  Bit32u offset = (addr & 0x1ffffff);
  Bit32u pitch = v->banshee.io[io_vidDesktopOverlayStride] & 0x7fff;
  unsigned i, x, y;

  if (pci_rom_size > 0) {
    Bit32u mask = (pci_rom_size - 1);
    if (((Bit32u)addr & ~mask) == pci_rom_address) {
#ifdef BX_LITTLE_ENDIAN
      Bit8u *data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
      Bit8u *data_ptr = (Bit8u *) data + (len - 1);
#endif
      for (unsigned i = 0; i < len; i++) {
        if (pci_conf[0x30] & 0x01) {
          *data_ptr = pci_rom[(addr & mask) + i];
        } else {
          *data_ptr = 0xff;
        }
        addr++;
#ifdef BX_LITTLE_ENDIAN
        data_ptr++;
#else // BX_BIG_ENDIAN
        data_ptr--;
#endif
      }
      return;
    }
  }
  if ((addr & ~0x1ffffff) == pci_bar[0].addr) {
    if (offset < 0x80000) {
      value = read(offset, len);
    } else if (offset < 0x100000) {
      value = agp_reg_read((offset >> 2) & 0x7f);
    } else if (offset < 0x200000) {
      value = blt_reg_read((offset >> 2) & 0x7f);
    } else if (offset < 0x600000) {
      value = register_r((offset - 0x200000) >> 2);
    } else if (offset < 0xc00000) {
      BX_DEBUG(("reserved read from offset 0x%08x", offset));
    } else if (offset < 0x1000000) {
      BX_ERROR(("TODO: YUV planar space read from offset 0x%08x", offset));
    } else {
      Bit8u temp = v->fbi.lfb_stride;
      v->fbi.lfb_stride = 11;
      value = lfb_r((offset & v->fbi.mask) >> 2);
      v->fbi.lfb_stride = temp;
    }
  } else if ((addr & ~0x1ffffff) == pci_bar[1].addr) {
    if (offset >= v->fbi.lfb_base) {
      offset -= v->fbi.lfb_base;
      pitch *= 128;
      x = (offset << 0) & ((1 << v->fbi.lfb_stride) - 1);
      y = (offset >> v->fbi.lfb_stride) & 0x1fff;
      offset = (v->fbi.lfb_base + y * pitch + x) & v->fbi.mask;
    } else {
      offset &= v->fbi.mask;
    }
    value = 0;
    for (i = 0; i < len; i++) {
      value |= ((Bit64u)v->fbi.ram[offset + i] << (i*8));
    }
  }
  switch (len) {
    case 1:
      *((Bit8u*)data) = (Bit8u)value;
      break;
    case 2:
      *((Bit16u*)data) = (Bit16u)value;
      break;
    case 4:
      *((Bit32u*)data) = (Bit32u)value;
      break;
    case 8:
      *((Bit64u*)data) = value;
      break;
    default:
     BX_ERROR(("bx_banshee_c::mem_read unsupported length %d", len));
  }
}

void bx_banshee_c::mem_write(bx_phy_address addr, unsigned len, void *data)
{
  Bit32u offset = (addr & 0x1ffffff);
  Bit64u value = 0;
  Bit32u mask = 0xffffffff;

#ifdef BX_LITTLE_ENDIAN
  Bit8u *data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  Bit8u *data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    value |= ((Bit64u)*data_ptr << (i * 8));
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  if ((addr & ~0x1ffffff) == pci_bar[0].addr) {
    if (offset < 0x80000) {
      write(offset, value, len);
    } else if (offset < 0x100000) {
      agp_reg_write((offset >> 2) & 0x7f, value);
    } else if (offset < 0x200000) {
      blt_reg_write((offset >> 2) & 0x7f, value);
    } else if (offset < 0x600000) {
      register_w_common((offset - 0x200000) >> 2, value);
    } else if (offset < 0x800000) {
      texture_w((offset & 0x1fffff) >> 2, value);
    } else if ((offset < 0xa00000) && (s.model == VOODOO_3)) {
      texture_w((1 << 19) | ((offset & 0x1fffff) >> 2), value);
    } else if (offset < 0xc00000) {
      BX_DEBUG(("reserved write to offset 0x%08x", offset));
    } else if (offset < 0x1000000) {
      BX_ERROR(("TODO: YUV planar space write to offset 0x%08x", offset));
    } else {
      Bit8u temp = v->fbi.lfb_stride;
      v->fbi.lfb_stride = 11;
      if (len == 2) {
        if ((offset & 3) == 0) {
          mask = 0x0000ffff;
        } else {
          mask = 0xffff0000;
        }
      }
      lfb_w((offset & v->fbi.mask) >> 2, value, mask);
      v->fbi.lfb_stride = temp;
    }
  } else if ((addr & ~0x1ffffff) == pci_bar[1].addr) {
    if (v->fbi.cmdfifo[0].enabled && (offset >= v->fbi.cmdfifo[0].base) &&
        (offset < v->fbi.cmdfifo[0].end)) {
      if (len == 4) {
        cmdfifo_w(&v->fbi.cmdfifo[0], offset, value);
      } else if (len == 8) {
        cmdfifo_w(&v->fbi.cmdfifo[0], offset, value);
        cmdfifo_w(&v->fbi.cmdfifo[0], offset + 4, value >> 32);
      } else {
        BX_ERROR(("CMDFIFO #0 write with len = %d redirected to LFB", len));
        mem_write_linear(offset, value, len);
      }
    } else if (v->fbi.cmdfifo[1].enabled && (offset >= v->fbi.cmdfifo[1].base) &&
               (offset < v->fbi.cmdfifo[1].end)) {
      if (len == 4) {
        cmdfifo_w(&v->fbi.cmdfifo[1], offset, value);
      } else if (len == 8) {
        cmdfifo_w(&v->fbi.cmdfifo[1], offset, value);
        cmdfifo_w(&v->fbi.cmdfifo[1], offset + 4, value >> 32);
      } else  {
        BX_ERROR(("CMDFIFO #1 write with len = %d redirected to LFB", len));
        mem_write_linear(offset, value, len);
      }
    } else {
      mem_write_linear(offset, value, len);
    }
  }
}

void bx_banshee_c::mem_write_linear(Bit32u offset, Bit64u value, unsigned len)
{
  Bit8u value8;
  Bit32u start = v->banshee.io[io_vidDesktopStartAddr] & v->fbi.mask;
  Bit32u pitch = v->banshee.io[io_vidDesktopOverlayStride] & 0x7fff;
  unsigned i, w, x, y;

  if (offset >= v->fbi.lfb_base) {
    offset -= v->fbi.lfb_base;
    pitch *= 128;
    x = (offset << 0) & ((1 << v->fbi.lfb_stride) - 1);
    y = (offset >> v->fbi.lfb_stride) & 0x1fff;
    offset = (v->fbi.lfb_base + y * pitch + x) & v->fbi.mask;
  } else {
    offset &= v->fbi.mask;
  }
  BX_LOCK(render_mutex);
  for (i = 0; i < len; i++) {
    value8 = (value >> (i * 8)) & 0xff;
    v->fbi.ram[offset + i] = value8;
  }
  if ((offset >= start) && (pitch > 0)) {
    offset -= start;
    x = (offset % pitch) / (v->banshee.disp_bpp >> 3);
    y = offset / pitch;
    w = len / (v->banshee.disp_bpp >> 3);
    if (v->banshee.half_mode) {
      y <<= 1;
    }
    if (v->banshee.double_width) {
      x <<= 1;
      w <<= 1;
    }
    if (w == 0) w = 1;
    theVoodooVga->redraw_area(x, y, w, 1);
  }
  BX_UNLOCK(render_mutex);
}

Bit32u bx_banshee_c::agp_reg_read(Bit8u reg)
{
  Bit32u result = 0;
  Bit8u fifo_idx = (reg >= cmdBaseAddr1);

  switch (reg) {
    case cmdBaseAddr0:
    case cmdBaseAddr1:
      result = (v->fbi.cmdfifo[fifo_idx].base >> 12);
      break;
    case cmdBump0:
    case cmdBump1:
      break;
    case cmdRdPtrL0:
    case cmdRdPtrL1:
      result = v->fbi.cmdfifo[fifo_idx].rdptr;
      break;
    case cmdFifoDepth0:
    case cmdFifoDepth1:
      result = v->fbi.cmdfifo[fifo_idx].depth;
      break;
    case cmdHoleCnt0:
    case cmdHoleCnt1:
      result = v->fbi.cmdfifo[fifo_idx].holes;
      break;
    case cmdStatus0:
    case cmdStatus1:
      BX_ERROR(("cmdStatus%d not implemented yet", fifo_idx));
    default:
      result = v->banshee.agp[reg];
  }
  BX_DEBUG(("AGP read register 0x%03x (%s) result = 0x%08x", reg<<2,
            banshee_agp_reg_name[reg], result));
  return result;
}

void bx_banshee_c::agp_reg_write(Bit8u reg, Bit32u value)
{
  Bit8u fifo_idx = (reg >= cmdBaseAddr1);

  BX_DEBUG(("AGP write register 0x%03x (%s) value = 0x%08x", reg<<2,
            banshee_agp_reg_name[reg], value));
  switch (reg) {
    case cmdBaseAddr0:
    case cmdBaseAddr1:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[fifo_idx].base = (value << 12);
      if (fifo_idx == 0) {
        v->fbi.cmdfifo[0].end = v->fbi.cmdfifo[0].base +
          (((v->banshee.agp[cmdBaseSize0] & 0xff) + 1) << 12);
      } else {
        v->fbi.cmdfifo[1].end = v->fbi.cmdfifo[1].base +
          (((v->banshee.agp[cmdBaseSize1] & 0xff) + 1) << 12);
      }
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdBaseSize0:
    case cmdBaseSize1:
      BX_LOCK(cmdfifo_mutex);
      if (fifo_idx == 0) {
        v->fbi.cmdfifo[0].end = v->fbi.cmdfifo[0].base + (((value & 0xff) + 1) << 12);
      } else {
        v->fbi.cmdfifo[1].end = v->fbi.cmdfifo[1].base + (((value & 0xff) + 1) << 12);
      }
      v->fbi.cmdfifo[fifo_idx].count_holes = (((value >> 10) & 1) == 0);
      if ((value >> 9) & 1) {
        BX_ERROR(("CMDFIFO in AGP memory not supported yet"));
      }
      if (v->fbi.cmdfifo[fifo_idx].enabled != ((value >> 8) & 1)) {
        v->fbi.cmdfifo[fifo_idx].enabled = ((value >> 8) & 1);
        BX_INFO(("CMDFIFO #%d now %sabled", fifo_idx,
                 v->fbi.cmdfifo[fifo_idx].enabled ? "en" : "dis"));
      }
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdBump0:
    case cmdBump1:
      if (value > 0) {
        BX_ERROR(("cmdBump%d not implemented (value = 0x%04x)", fifo_idx, (Bit16u)value));
      }
      break;
    case cmdRdPtrL0:
    case cmdRdPtrL1:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[fifo_idx].rdptr = value;
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdRdPtrH0:
    case cmdRdPtrH1:
      if (value > 0) {
        BX_ERROR(("cmdRdPtrH%d not supported yet", fifo_idx));
      }
      break;
    case cmdAMin0:
    case cmdAMin1:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[fifo_idx].amin = value;
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdAMax0:
    case cmdAMax1:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[fifo_idx].amax = value;
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdFifoDepth0:
    case cmdFifoDepth1:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[fifo_idx].depth = value & 0xfffff;
      BX_UNLOCK(cmdfifo_mutex);
      break;
    case cmdHoleCnt0:
    case cmdHoleCnt1:
      if (value > 0) {
        BX_ERROR(("cmdHoleCnt%d not supported yet", fifo_idx));
      }
      break;
  }
  v->banshee.agp[reg] = value;
}

#define BLT v->banshee.blt

Bit32u bx_banshee_c::blt_reg_read(Bit8u reg)
{
  Bit32u result = 0;

  switch (reg) {
    case blt_status:
      result = register_r(0);
      break;
    case blt_intrCtrl:
      result = register_r(1);
      break;
    default:
      if (reg < 0x20) {
        result = BLT.reg[reg];
      }
  }
  if (reg < 0x20) {
    BX_DEBUG(("2D read register 0x%03x (%s) result = 0x%08x", reg<<2,
              banshee_blt_reg_name[reg], result));
  }
  return result;
}

void bx_banshee_c::blt_reg_write(Bit8u reg, Bit32u value)
{
  Bit8u old_cmd;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;

  if (reg < 0x20) {
    BLT.reg[reg] = value;
    BX_DEBUG(("2D write register 0x%03x (%s) value = 0x%08x", reg<<2,
              banshee_blt_reg_name[reg], value));
  }
  switch (reg) {
    case blt_intrCtrl:
      register_w_common(1, value);
      break;
    case blt_clip0Min:
      BLT.clipx0[0] = BLT.reg[reg] & 0xfff;
      BLT.clipy0[0] = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_clip0Max:
      BLT.clipx1[0] = BLT.reg[reg] & 0xfff;
      BLT.clipy1[0] = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_dstBaseAddr:
      BLT.dst_base = BLT.reg[reg] & v->fbi.mask;
      BLT.dst_tiled = BLT.reg[reg] >> 31;
      if (BLT.dst_tiled) {
        BLT.dst_pitch = (BLT.reg[blt_dstFormat] & 0x7f) << 7;
      } else {
        BLT.dst_pitch = BLT.reg[blt_dstFormat] & 0x3fff;
      }
      break;
    case blt_dstFormat:
      BLT.dst_fmt = (BLT.reg[reg] >> 16) & 0x07;
      if (BLT.dst_tiled) {
        BLT.dst_pitch = (BLT.reg[reg] & 0x7f) << 7;
      } else {
        BLT.dst_pitch = BLT.reg[reg] & 0x3fff;
      }
      break;
    case blt_srcBaseAddr:
      BLT.src_base = BLT.reg[reg] & v->fbi.mask;
      BLT.src_tiled = BLT.reg[reg] >> 31;
      if (BLT.src_tiled) {
        BLT.src_pitch = (BLT.reg[blt_srcFormat] & 0x7f) << 7;
      } else {
        BLT.src_pitch = BLT.reg[blt_srcFormat] & 0x3fff;
      }
      break;
    case blt_srcFormat:
      BLT.src_fmt = (BLT.reg[reg] >> 16) & 0x0f;
      BLT.src_swizzle = (BLT.reg[reg] >> 20) & 0x03;
      if (BLT.src_tiled) {
        BLT.src_pitch = (BLT.reg[reg] & 0x7f) << 7;
      } else {
        BLT.src_pitch = BLT.reg[reg] & 0x3fff;
      }
      break;
    case blt_pattern0Alias:
      BLT.cpat[0][0] = value & 0xff;
      BLT.cpat[0][1] = (value >> 8) & 0xff;
      BLT.cpat[0][2] = (value >> 16) & 0xff;
      BLT.cpat[0][3] = (value >> 24) & 0xff;
      break;
    case blt_pattern1Alias:
      BLT.cpat[1][0] = value & 0xff;
      BLT.cpat[1][1] = (value >> 8) & 0xff;
      BLT.cpat[1][2] = (value >> 16) & 0xff;
      BLT.cpat[1][3] = (value >> 24) & 0xff;
      break;
    case blt_clip1Min:
      BLT.clipx0[1] = BLT.reg[reg] & 0xfff;
      BLT.clipy0[1] = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_clip1Max:
      BLT.clipx1[1] = BLT.reg[reg] & 0xfff;
      BLT.clipy1[1] = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_srcSize:
      BLT.src_w = BLT.reg[reg] & 0x1fff;
      BLT.src_h = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_srcXY:
      BLT.src_x = BLT.reg[reg] & 0x1fff;
      BLT.src_y = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_colorBack:
      BLT.bgcolor[0] = BLT.reg[reg] & 0xff;
      BLT.bgcolor[1] = (BLT.reg[reg] >> 8) & 0xff;
      BLT.bgcolor[2] = (BLT.reg[reg] >> 16) & 0xff;
      BLT.bgcolor[3] = (BLT.reg[reg] >> 24) & 0xff;
      break;
    case blt_colorFore:
      BLT.fgcolor[0] = BLT.reg[reg] & 0xff;
      BLT.fgcolor[1] = (BLT.reg[reg] >> 8) & 0xff;
      BLT.fgcolor[2] = (BLT.reg[reg] >> 16) & 0xff;
      BLT.fgcolor[3] = (BLT.reg[reg] >> 24) & 0xff;
      break;
    case blt_dstSize:
      BLT.dst_w = BLT.reg[reg] & 0x1fff;
      BLT.dst_h = (BLT.reg[reg] >> 16) & 0x1fff;
      break;
    case blt_dstXY:
      if ((BLT.reg[reg] >> 15) & 1) {
        BLT.dst_x = (Bit16s)(BLT.reg[reg] & 0xffff);
      } else {
        BLT.dst_x = BLT.reg[reg] & 0x1fff;
      }
      if (BLT.reg[reg] >> 31) {
        BLT.dst_y = (Bit16s)(BLT.reg[reg] >> 16);
      } else {
        BLT.dst_y = (BLT.reg[reg] >> 16) & 0x1fff;
      }
      break;
    case blt_command:
      old_cmd = BLT.cmd;
      BLT.cmd = (value & 0x0f);
      BLT.immed = (value >> 8) & 1;
      BLT.x_dir = (value >> 14) & 1;
      BLT.y_dir = (value >> 15) & 1;
      BLT.transp = (value >> 16) & 1;
      BLT.patsx = (value >> 17) & 7;
      BLT.patsy = (value >> 20) & 7;
      BLT.clip_sel = (value >> 23) & 1;
      BLT.rop[0] = (value >> 24);
      BLT.rop[1] = BLT.reg[blt_rop] & 0xff;
      BLT.rop[2] = (BLT.reg[blt_rop] >> 8) & 0xff;
      BLT.rop[3] = (BLT.reg[blt_rop] >> 16) & 0xff;
      BLT.pattern_blt = (BLT.rop_flags[BLT.rop[0]] & BX_ROP_PATTERN);
      if (colorkey_en & 1) {
        BLT.pattern_blt |= (BLT.rop_flags[BLT.rop[2]] & BX_ROP_PATTERN);
      }
      if (colorkey_en & 2) {
        BLT.pattern_blt |= (BLT.rop_flags[BLT.rop[1]] & BX_ROP_PATTERN);
      }
      if (colorkey_en == 3) {
        BLT.pattern_blt |= (BLT.rop_flags[BLT.rop[3]] & BX_ROP_PATTERN);
      }
      if (BLT.x_dir) {
        BLT.rop_fn[0] = BLT.rop_handler[1][BLT.rop[0]];
        BLT.rop_fn[1] = BLT.rop_handler[1][BLT.rop[1]];
        BLT.rop_fn[2] = BLT.rop_handler[1][BLT.rop[2]];
        BLT.rop_fn[3] = BLT.rop_handler[1][BLT.rop[3]];
      } else {
        BLT.rop_fn[0] = BLT.rop_handler[0][BLT.rop[0]];
        BLT.rop_fn[1] = BLT.rop_handler[0][BLT.rop[1]];
        BLT.rop_fn[2] = BLT.rop_handler[0][BLT.rop[2]];
        BLT.rop_fn[3] = BLT.rop_handler[0][BLT.rop[3]];
      }
      if (BLT.lamem != NULL) {
        BX_ERROR(("Writing new command while another one is still pending"));
        delete [] BLT.lamem;
        BLT.lamem = NULL;
      }
      if (old_cmd == 8) {
        blt_polygon_fill(1);
      }
      if (BLT.cmd == 8) {
        BLT.pgn_init = 0;
      }
      if (BLT.immed) {
        blt_execute();
      } else {
        blt_launch_area_setup();
      }
      break;
    case blt_commandExtra:
      if ((value & 0x04) != 0) {
        BX_ERROR(("wait for vsync not supported yet"));
      }
      break;
    default:
      if ((reg >= 0x20) && (reg < 0x40)) {
        blt_launch_area_write(value);
      } else if ((reg >= 0x40) && (reg < 0x80)) {
        reg -= 0x40;
        BX_DEBUG(("colorPattern write reg 0x%02x: value = 0x%08x", reg, value));
        BLT.cpat[reg][0] = value & 0xff;
        BLT.cpat[reg][1] = (value >> 8) & 0xff;
        BLT.cpat[reg][2] = (value >> 16) & 0xff;
        BLT.cpat[reg][3] = (value >> 24) & 0xff;
      }
  }
}

void bx_banshee_c::blt_launch_area_setup()
{
  Bit32u pbytes;
  Bit8u pxpack, pxsize = 0, pxstart;

  BLT.lacnt = 0;
  BLT.laidx = 0;
  switch (BLT.cmd) {
    case 1:
    case 2:
    case 5:
    case 6:
    case 7:
    case 8:
      BLT.lacnt = 1;
      break;
    case 3:
    case 4:
      pxpack = (BLT.reg[blt_srcFormat] >> 22) & 3;
      if (BLT.src_fmt == 0) {
        BLT.h2s_pxstart = BLT.reg[blt_srcXY] & 0x1f;
        pbytes = (BLT.dst_w + BLT.h2s_pxstart + 7) >> 3;
      } else {
        BLT.h2s_pxstart = BLT.reg[blt_srcXY] & 0x03;
        if (BLT.src_fmt == 1) {
          pxsize = 1;
        } else if ((BLT.src_fmt >= 3) && (BLT.src_fmt <= 5))  {
          pxsize = BLT.src_fmt - 1;
        } else {
          BX_ERROR(("Source format %d not handled yet", BLT.src_fmt));
        }
        pbytes = BLT.dst_w * pxsize + BLT.h2s_pxstart;
      }
      switch (pxpack) {
        case 1:
          BLT.h2s_pitch = pbytes;
          break;
        case 2:
          BLT.h2s_pitch = (pbytes + 1) & ~1;
          break;
        case 3:
          BLT.h2s_pitch = (pbytes + 3) & ~3;
          break;
        default:
          BLT.h2s_pitch = (pbytes + 3) & ~3;
          pbytes = 0;
          pxstart = BLT.h2s_pxstart;
          if (BLT.src_fmt == 0) {
            for (int i = 0; i < BLT.dst_h; i++) {
              pbytes += ((((BLT.dst_w + pxstart + 7) >> 3) + 3) & ~3);
              pxstart += (Bit8u)(BLT.reg[blt_srcFormat] << 3);
              pxstart &= 0x1f;
            }
          } else {
            for (int i = 0; i < BLT.dst_h; i++) {
              pbytes += ((BLT.dst_w * pxsize + pxstart + 3) & ~3);
              pxstart += (Bit8u)BLT.reg[blt_srcFormat];
              pxstart &= 0x03;
            }
          }
          BLT.lacnt = pbytes >> 2;
      }
      if (pxpack != 0) {
        BLT.lacnt = (BLT.h2s_pitch * BLT.dst_h + 3) >> 2;
      }
      BLT.lamem = new Bit8u[BLT.lacnt * 4];
      break;
    default:
      BX_ERROR(("launchArea setup: command %d not handled yet", BLT.cmd));
  }
}

void bx_banshee_c::blt_launch_area_write(Bit32u value)
{
  if (BLT.lacnt > 0) {
    BX_DEBUG(("launchArea write: value = 0x%08x", value));
    if (BLT.lamem != NULL) {
      if (BLT.src_swizzle & 1) {
        value = bx_bswap32(value);
      }
      if (BLT.src_swizzle & 2) {
        value = (value >> 16) | (value << 16);
      }
      BLT.lamem[BLT.laidx++] = (value & 0xff);
      BLT.lamem[BLT.laidx++] = ((value >> 8) & 0xff);
      BLT.lamem[BLT.laidx++] = ((value >> 16) & 0xff);
      BLT.lamem[BLT.laidx++] = ((value >> 24) & 0xff);
    } else if ((BLT.cmd == 1) || (BLT.cmd == 2)) {
      BLT.reg[blt_srcXY] = value;
      BLT.src_x = value & 0x1fff;
      BLT.src_y = (value >> 16) & 0x1fff;
    } if ((BLT.cmd >= 5) && (BLT.cmd <= 7)) {
      BLT.reg[blt_dstXY] = value;
      if ((value >> 15) & 1) {
        BLT.dst_x = (Bit16s)(value & 0xffff);
      } else {
        BLT.dst_x = value & 0x1fff;
      }
      if (value >> 31) {
        BLT.dst_y = (Bit16s)(value >> 16);
      } else {
        BLT.dst_y = (value >> 16) & 0x1fff;
      }
    } if (BLT.cmd == 8) {
      BLT.pgn_val = value;
    }
    if (--BLT.lacnt == 0) {
      blt_execute();
    }
  } else {
    BX_ERROR(("launchArea write: ignoring extra data"));
  }
}

void bx_banshee_c::blt_execute()
{
  Bit16u x, y;

  switch (BLT.cmd) {
    case 0: // NOP
      break;
    case 1:
      BLT.busy = 1;
      if (BLT.pattern_blt) {
        blt_screen_to_screen_pattern();
      } else {
        blt_screen_to_screen();
      }
      if (!BLT.immed) {
        BLT.lacnt = 1;
      }
      break;
    case 2:
      if (BLT.pattern_blt) {
        BX_ERROR(("TODO: 2D Screen to screen stretch pattern blt"));
      } else {
        BLT.busy = 1;
        blt_screen_to_screen_stretch();
      }
      break;
    case 3:
    case 4:
      if (!BLT.immed) {
        if (BLT.cmd == 3) {
          BLT.busy = 1;
          if (BLT.pattern_blt) {
            blt_host_to_screen_pattern();
          } else {
            blt_host_to_screen();
          }
        } else {
          BX_ERROR(("TODO: 2D Host to screen stretch blt"));
        }
        delete [] BLT.lamem;
        BLT.lamem = NULL;
      } else {
        BX_ERROR(("Host to screen blt: immediate execution not supported"));
      }
      break;
    case 5:
      BLT.busy = 1;
      if (BLT.pattern_blt) {
        if ((BLT.reg[blt_command] >> 13) & 1) {
          blt_pattern_fill_mono();
        } else {
          blt_pattern_fill_color();
        }
      } else {
        blt_rectangle_fill();
      }
      if (!BLT.immed) {
        BLT.lacnt = 1;
      }
      break;
    case 6:
    case 7:
      BLT.busy = 1;
      blt_line(BLT.cmd == 7);
      if (!BLT.immed) {
        BLT.lacnt = 1;
      }
      break;
    case 8:
      if (!BLT.immed) {
        if (!BLT.pgn_init) {
          BLT.pgn_l0x = BLT.pgn_l1x = BLT.src_x;
          BLT.pgn_l0y = BLT.pgn_l1y = BLT.src_y;
          BLT.pgn_r0x = BLT.pgn_r1x = BLT.dst_x;
          BLT.pgn_r0y = BLT.pgn_r1y = BLT.dst_y;
          BLT.pgn_init = 1;
        }
        x = (Bit16u)BLT.pgn_val;
        y = (Bit16u)(BLT.pgn_val >> 16);
        if (BLT.pgn_r1y >= BLT.pgn_l1y) {
          BLT.pgn_l1x = x;
          BLT.pgn_l1y = y;
          if (y == BLT.pgn_l0y) {
            BLT.pgn_l0x = x;
          }
        } else {
          BLT.pgn_r1x = x;
          BLT.pgn_r1y = y;
          if (y == BLT.pgn_r0y) {
            BLT.pgn_r0x = x;
          }
        }
        blt_polygon_fill(0);
      } else {
        BLT.reg[blt_dstXY] = BLT.reg[blt_srcXY];
        BLT.immed = 0;
      }
      BLT.lacnt = 1;
      break;
    case 13:
      BX_ERROR(("TODO: 2D Write Sgram Mode register"));
      break;
    case 14:
      BX_ERROR(("TODO: 2D Write Sgram Mask register"));
      break;
    case 15:
      BX_ERROR(("TODO: 2D Write Sgram Color register"));
      break;
    default:
      BX_ERROR(("Unknown BitBlt command"));
  }
}

void bx_banshee_c::blt_complete()
{
  Bit32u vstart = v->banshee.io[io_vidDesktopStartAddr] & v->fbi.mask;
  Bit16u vpitch = v->banshee.io[io_vidDesktopOverlayStride] & 0x7fff;
  Bit8u vpxsize = (v->banshee.disp_bpp >> 3);
  Bit32u dstart = BLT.dst_base;
  Bit16u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit32u cmd = BLT.reg[blt_command];
  bool xinc = (cmd >> 10) & 1;
  bool yinc = (cmd >> 11) & 1;
  int x, y, w, h;

  if ((v->banshee.io[io_vidProcCfg] & 0x101) == 0x101) {
    if (v->banshee.overlay_tiled) {
      vpitch *= 128;
    }
  } else {
    if (v->banshee.desktop_tiled) {
      vpitch *= 128;
    }
  }
  if ((dstart == vstart) && (dpitch == vpitch) && (dpxsize == vpxsize)) {
    if (BLT.cmd < 6) {
      if (BLT.x_dir) {
        x = BLT.dst_x + 1 - BLT.dst_w;
      } else {
        x = BLT.dst_x;
      }
      if (BLT.y_dir) {
        y = BLT.dst_y + 1 - BLT.dst_h;
      } else {
        y = BLT.dst_y;
      }
      w = BLT.dst_w;
      h = BLT.dst_h;
    } else {
      if (BLT.src_x < BLT.dst_x) {
        x = BLT.src_x;
        w = BLT.dst_x - BLT.src_x + 1;
      } else {
        x = BLT.dst_x;
        w = BLT.src_x - BLT.dst_x + 1;
      }
      if (BLT.src_y < BLT.dst_y) {
        y = BLT.src_y;
        h = BLT.dst_y - BLT.src_y + 1;
      } else {
        y = BLT.dst_y;
        h = BLT.src_y - BLT.dst_y + 1;
      }
    }
    if (v->banshee.half_mode) {
      y <<= 1;
      h <<= 1;
    }
    if (v->banshee.double_width) {
      x <<= 1;
      w <<= 1;
    }
    if ((v->banshee.io[io_vidProcCfg] & 0x101) == 0x101) {
      v->fbi.video_changed = 1;
    } else {
      theVoodooVga->redraw_area(x, y, w, h);
    }
  }
  if (xinc) {
    BLT.dst_x += BLT.dst_w;
    BLT.reg[blt_dstXY] &= ~0xffff;
    BLT.reg[blt_dstXY] |= BLT.dst_x;
  }
  if (yinc) {
    BLT.dst_y += BLT.dst_h;
    BLT.reg[blt_dstXY] &= 0xffff;
    BLT.reg[blt_dstXY] |= (BLT.dst_y << 16);
  }
  BLT.busy = 0;
}

bool bx_banshee_c::blt_apply_clipwindow(int *x0, int *y0, int *x1, int *y1, int *w, int *h)
{
  int cx0, cx1, cy0, cy1, xd, yd;

  cx0 = BLT.clipx0[BLT.clip_sel];
  cy0 = BLT.clipy0[BLT.clip_sel];
  cx1 = BLT.clipx1[BLT.clip_sel];
  cy1 = BLT.clipy1[BLT.clip_sel];
  if (BLT.x_dir) {
    xd = *x1 - cx1 + 1;
    if (xd > 0) {
      *w -= xd;
      *x1 = cx1 - 1;
      if (x0 != NULL) *x0 -= xd;
    }
    xd = cx0 - (*x1 - *w + 1);
    if (xd > 0) {
      *w -= xd;
    }
  } else {
    xd = cx0 - *x1;
    if (xd > 0) {
      *w -= xd;
      *x1 = cx0;
      if (x0 != NULL) *x0 += xd;
    }
    xd = *x1 + *w - cx1;
    if (xd > 0) {
      *w -= xd;
    }
  }
  if (BLT.y_dir) {
    yd = *y1 - cy1 + 1;
    if (yd > 0) {
      *h -= yd;
      *y1 = cy1 - 1;
      if (y0 != NULL) *y0 -= xd;
    }
    yd = cy0 - (*y1 - *h + 1);
    if (yd > 0) {
      *h -= xd;
    }
  } else {
    yd = cy0 - *y1;
    if (yd > 0) {
      *h -= yd;
      *y1 = cy0;
      if (y0 != NULL) *y0 += yd;
    }
    yd = *y1 + *h - cy1;
    if (yd > 0) {
      *h -= yd;
    }
  }
  return ((*w > 0) && (*h > 0));
}

bool bx_banshee_c::blt_clip_check(int x, int y)
{
  if ((x >= BLT.clipx0[BLT.clip_sel]) && (x < BLT.clipx1[BLT.clip_sel]) &&
      (y >= BLT.clipy0[BLT.clip_sel]) && (y < BLT.clipy1[BLT.clip_sel])) {
    return true;
  }
  return false;
}

Bit8u bx_banshee_c::blt_colorkey_check(Bit8u *ptr, Bit8u pxsize, bool dst)
{
  Bit8u pass = 0;
  Bit32u color, cmin, cmax;
  Bit8u r, g, b, rmin, rmax, gmin, gmax, bmin, bmax;

  if (!dst) {
    cmin = BLT.reg[blt_srcColorkeyMin];
    cmax = BLT.reg[blt_srcColorkeyMax];
  } else {
    cmin = BLT.reg[blt_dstColorkeyMin];
    cmax = BLT.reg[blt_dstColorkeyMax];
  }
  if (pxsize == 1) {
    pass = ((*ptr >= (Bit8u)cmin) && (*ptr <= (Bit8u)cmax));
  } else {
    if (pxsize == 2) {
      color = *ptr;
      color |= *(ptr + 1) << 8;
      r = (color >> 11);
      g = (color >> 5) & 0x3f;
      b = color & 0x1f;
      rmin = (cmin >> 11) & 0x1f;
      rmax = (cmax >> 11) & 0x1f;
      gmin = (cmin >> 5) & 0x3f;
      gmax = (cmax >> 5) & 0x3f;
      bmin = cmin & 0x1f;
      bmax = cmax & 0x1f;
    } else {
      color = *ptr;
      color |= *(ptr + 1) << 8;
      color |= *(ptr + 2) << 16;
      r = (color >> 16);
      g = (color >> 8) & 0xff;
      b = color & 0xff;
      rmin = (cmin >> 16) & 0xff;
      rmax = (cmax >> 16) & 0xff;
      gmin = (cmin >> 8) & 0xff;
      gmax = (cmax >> 8) & 0xff;
      bmin = cmin & 0xff;
      bmax = cmax & 0xff;
    }
    pass = ((r >= rmin) && (r <= rmax) && (g >= gmin) && (g <= gmax) &&
            (b >= bmin) && (b <= bmax));
  }
  if (!dst) pass <<= 1;
  return pass;
}

void bx_banshee_c::blt_rectangle_fill()
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *dst_ptr, *dst_ptr1;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  int dx, dy, w, h, x, y;

  dx = BLT.dst_x;
  dy = BLT.dst_y;
  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Rectangle fill: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if (!blt_apply_clipwindow(NULL, NULL, &dx, &dy, &w, &h)) {
    BLT.busy = 0;
    return;
  }
  BX_LOCK(render_mutex);
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + dx * dpxsize];
  for (y = 0; y < h; y++) {
    dst_ptr1 = dst_ptr;
    for (x = 0; x < w; x++) {
      if (colorkey_en & 2) {
        rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
      }
      BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
      dst_ptr1 += dpxsize;
    }
    dst_ptr += dpitch;
  }
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_pattern_fill_mono()
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *pat_ptr = &BLT.cpat[0][0];
  Bit8u *dst_ptr, *dst_ptr1, *pat_ptr1;
  bool patrow0 = (BLT.reg[blt_commandExtra] & 0x08) > 0;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  Bit8u *color;
  int dx, dy, w, h, x, y;
  Bit8u mask;
  bool set;

  dx = BLT.dst_x;
  dy = BLT.dst_y;
  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Pattern fill mono: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if (!blt_apply_clipwindow(NULL, NULL, &dx, &dy, &w, &h)) {
    BLT.busy = 0;
    return;
  }
  BX_LOCK(render_mutex);
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + dx * dpxsize];
  for (y = dy; y < (dy + h); y++) {
    dst_ptr1 = dst_ptr;
    if (!patrow0) {
      pat_ptr1 = pat_ptr + ((y + BLT.patsy) & 7);
    } else {
      pat_ptr1 = pat_ptr;
    }
    for (x = dx; x < (dx + w); x++) {
      mask = 0x80 >> ((x + BLT.patsx) & 7);
      set = (*pat_ptr1 & mask) > 0;
      if (set) {
        color = &BLT.fgcolor[0];
      } else {
        color = &BLT.bgcolor[0];
      }
      if ((set) || !BLT.transp) {
        if (colorkey_en & 2) {
          rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
        }
        BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
      }
      dst_ptr1 += dpxsize;
    }
    dst_ptr += dpitch;
  }
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_pattern_fill_color()
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *pat_ptr = &BLT.cpat[0][0];
  Bit8u *dst_ptr, *dst_ptr1, *pat_ptr1, *pat_ptr2;
  bool patrow0 = (BLT.reg[blt_commandExtra] & 0x08) > 0;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  int dx, dy, w, h, x, y;

  dx = BLT.dst_x;
  dy = BLT.dst_y;
  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Pattern fill color: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if (!blt_apply_clipwindow(NULL, NULL, &dx, &dy, &w, &h)) {
    BLT.busy = 0;
    return;
  }
  BX_LOCK(render_mutex);
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + dx * dpxsize];
  for (y = dy; y < (dy + h); y++) {
    dst_ptr1 = dst_ptr;
    if (!patrow0) {
      pat_ptr1 = pat_ptr + ((y + BLT.patsy) & 7) * dpxsize * 8;
    } else {
      pat_ptr1 = pat_ptr;
    }
    for (x = dx; x < (dx + w); x++) {
      pat_ptr2 = pat_ptr1 + ((x + BLT.patsx) & 7) * dpxsize;
      if (colorkey_en & 2) {
        rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
      }
      BLT.rop_fn[rop](dst_ptr1, pat_ptr2, dpitch, dpxsize, dpxsize, 1);
      dst_ptr1 += dpxsize;
    }
    dst_ptr += dpitch;
  }
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_screen_to_screen()
{
  Bit8u *src_ptr = &v->fbi.ram[BLT.src_base];
  Bit8u *dst_ptr, *dst_ptr1, *src_ptr1;
  Bit8u pxpack = (BLT.reg[blt_srcFormat] >> 22) & 3;
  int dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *color;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  int spitch;
  int dpitch = BLT.dst_pitch;
  int bkw_adj = 0;
  int ncols, nrows, dx, dy, sx, sy, w, h;
  Bit8u smask, rop = 0;
  bool set;

  sx = BLT.src_x;
  sy = BLT.src_y;
  dx = BLT.dst_x;
  dy = BLT.dst_y;
  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Screen to screen blt: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if ((BLT.src_fmt != 0) && (BLT.dst_fmt != BLT.src_fmt)) {
    BX_ERROR(("Pixel format conversion not supported yet"));
  }
  if (!blt_apply_clipwindow(&sx, &sy, &dx, &dy, &w, &h)) {
    BLT.busy = 0;
    return;
  }
  BX_LOCK(render_mutex);
  if ((BLT.src_fmt == 0) && (pxpack == 1)) {
    spitch = (BLT.dst_w + 7) / 8;
  } else {
    spitch = BLT.src_pitch;
  }
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + dx * dpxsize];
  if (BLT.x_dir) {
    bkw_adj = dpxsize - 1;
    dpxsize *= -1;
  }
  if (BLT.y_dir) {
    spitch *= -1;
    dpitch *= -1;
  }
  if ((BLT.src_fmt == 0) && (pxpack == 1)) {
    src_ptr += (sy * abs(spitch) + sx / 8);
    nrows = h;
    do {
      src_ptr1 = src_ptr;
      dst_ptr1 = dst_ptr;
      smask = 0x80 >> (sx & 7);
      ncols = w;
      do {
        set = (*src_ptr1 & smask) > 0;
        if (set) {
          color = &BLT.fgcolor[0];
        } else {
          color = &BLT.bgcolor[0];
        }
        if (set || !BLT.transp) {
          if (colorkey_en & 2) {
            rop = blt_colorkey_check(dst_ptr1, abs(dpxsize), 1);
          }
          BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, abs(dpxsize), 1);
        }
        smask >>= 1;
        if (smask == 0) {
          src_ptr1++;
          smask = 0x80;
        }
        dst_ptr1 += dpxsize;
      } while (--ncols);
      src_ptr += spitch;
      dst_ptr += dpitch;
    } while (--nrows);
  } else if (colorkey_en > 0) {
    src_ptr += (sy * abs(spitch) + sx * abs(dpxsize));
    nrows = h;
    do {
      src_ptr1 = src_ptr;
      dst_ptr1 = dst_ptr;
      ncols = w;
      do {
        if (colorkey_en & 1) {
          rop = blt_colorkey_check(src_ptr1, abs(dpxsize), 0);
        }
        if (colorkey_en & 2) {
          rop |= blt_colorkey_check(dst_ptr1, abs(dpxsize), 1);
        }
        BLT.rop_fn[rop](dst_ptr1 + bkw_adj, src_ptr1 + bkw_adj, dpitch, spitch, abs(dpxsize), 1);
        src_ptr1 += dpxsize;
        dst_ptr1 += dpxsize;
      } while (--ncols);
      src_ptr += spitch;
      dst_ptr += dpitch;
    } while (--nrows);
  } else {
    src_ptr += (sy * abs(spitch) + sx * abs(dpxsize) + bkw_adj);
    BLT.rop_fn[0](dst_ptr + bkw_adj, src_ptr, dpitch, spitch, w * abs(dpxsize), h);
  }
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_screen_to_screen_pattern()
{
  Bit8u *dst_ptr, *dst_ptr1, *src_ptr, *src_ptr1;
  Bit8u *pat_ptr = &BLT.cpat[0][0], *pat_ptr1, *pat_ptr2 = NULL;
  int dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  int spitch = BLT.src_pitch;
  int dpitch = BLT.dst_pitch;
  bool patmono = (BLT.reg[blt_command] >> 13) & 1;
  bool patrow0 = (BLT.reg[blt_commandExtra] & 0x08) > 0;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u *patcolor;
  int ncols, nrows, dx, dy, x, sx, sy, w, h;
  Bit8u pmask = 0, rop = 0, patline;
  bool set;

  sx = BLT.src_x;
  sy = BLT.src_y;
  dx = BLT.dst_x;
  dy = BLT.dst_y;
  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Screen to screen pattern blt: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if (BLT.dst_fmt != BLT.src_fmt) {
    BX_ERROR(("Pixel format conversion not supported yet"));
  }
  if (!blt_apply_clipwindow(&sx, &sy, &dx, &dy, &w, &h)) {
    BLT.busy = 0;
    return;
  }
  BX_LOCK(render_mutex);
  src_ptr = &v->fbi.ram[BLT.src_base + sy * spitch + sx * dpxsize];
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + dx * dpxsize];
  if (BLT.x_dir) {
    dpxsize *= -1;
  }
  if (BLT.y_dir) {
    spitch *= -1;
    dpitch *= -1;
  }
  nrows = h;
  do {
    src_ptr1 = src_ptr;
    dst_ptr1 = dst_ptr;
    if (!patrow0) {
      patline = (dy + BLT.patsy) & 7;
      if (patmono) {
        pat_ptr1 = pat_ptr + patline;
      } else {
        pat_ptr1 = pat_ptr + patline * dpxsize * 8;
      }
    } else {
      pat_ptr1 = pat_ptr;
    }
    x = dx;
    ncols = w;
    do {
      if (patmono) {
        pmask = 0x80 >> ((x + BLT.patsx) & 7);
        set = (*pat_ptr & pmask) > 0;
        if (set) {
          patcolor = &BLT.fgcolor[0];
        } else {
          patcolor = &BLT.bgcolor[0];
        }
        if (set || !BLT.transp) {
          if (colorkey_en & 2) {
            rop = blt_colorkey_check(dst_ptr1, abs(dpxsize), 1);
          }
          bx_ternary_rop(BLT.rop[rop], dst_ptr1, src_ptr1, patcolor, abs(dpxsize));
        }
      } else {
        pat_ptr2 = pat_ptr1 + ((x + BLT.patsx) & 7) * dpxsize;
        patcolor = pat_ptr2;
        if (colorkey_en & 1) {
          rop = blt_colorkey_check(src_ptr1, abs(dpxsize), 0);
        }
        if (colorkey_en & 2) {
          rop |= blt_colorkey_check(dst_ptr1, abs(dpxsize), 1);
        }
        bx_ternary_rop(BLT.rop[rop], dst_ptr1, src_ptr1, patcolor, abs(dpxsize));
      }
      src_ptr1 += dpxsize;
      dst_ptr1 += dpxsize;
      x++;
    } while (--ncols);
    src_ptr += spitch;
    dst_ptr += dpitch;
    if (BLT.y_dir) {
      dy--;
    } else {
      dy++;
    }
  } while (--nrows);
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_screen_to_screen_stretch()
{
  Bit8u *dst_ptr, *dst_ptr1, *src_ptr, *src_ptr1;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  int spitch = BLT.src_pitch;
  int dpitch = BLT.dst_pitch;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  int nrows, stepy;
  int dx, dy, x2, x3, y2, y3, w0, h0, w1, h1;
  double fx, fy;

  w0 = BLT.src_w;
  h0 = BLT.src_h;
  w1 = BLT.dst_w;
  h1 = BLT.dst_h;
  BX_DEBUG(("Screen to screen stretch blt: : %d x %d -> %d x %d  ROP0 %02X",
            w0, h0, w1, h1, BLT.rop[0]));
  if (BLT.dst_fmt != BLT.src_fmt) {
    BX_ERROR(("Pixel format conversion not supported yet"));
  }
  BX_LOCK(render_mutex);
  dy = BLT.dst_y;
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + BLT.dst_x * dpxsize];
  src_ptr = &v->fbi.ram[BLT.src_base + BLT.src_y * spitch + BLT.src_x * dpxsize];
  if (BLT.y_dir) {
    spitch *= -1;
    dpitch *= -1;
    stepy = -1;
  } else {
    stepy = 1;
  }
  fx = (double)w1 / (double)w0;
  fy = (double)h1 / (double)h0;
  y2 = 0;
  nrows = h1;
  do {
    dst_ptr1 = dst_ptr;
    x2 = 0;
    for (dx = BLT.dst_x; dx < (BLT.dst_x + w1); dx++) {
      if (blt_clip_check(dx, dy)) {
        x3 = (int)((double)x2 / fx + 0.49f);
        y3 = (int)((double)y2 / fy + 0.49f);
        src_ptr1 = src_ptr + (y3 * spitch + x3 * dpxsize);
        if (colorkey_en & 1) {
          rop = blt_colorkey_check(src_ptr1, dpxsize, 0);
        }
        if (colorkey_en & 2) {
          rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
        }
        BLT.rop_fn[rop](dst_ptr1, src_ptr1, dpitch, dpxsize, dpxsize, 1);
      }
      dst_ptr1 += dpxsize;
      x2++;
    }
    dst_ptr += dpitch;
    dy += stepy;
    y2++;
  } while (--nrows);
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_host_to_screen()
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  Bit8u *src_ptr = &BLT.lamem[0];
  Bit8u *dst_ptr, *dst_ptr1, *src_ptr1;
  Bit16u spitch = BLT.h2s_pitch;
  Bit8u srcfmt = BLT.src_fmt;
  Bit8u pxpack = (BLT.reg[blt_srcFormat] >> 22) & 3;
  Bit8u spxsize = 0, r = 0, g = 0, b = 0;
  Bit8u scolor[4];
  Bit8u *color;
  int nrows, x, y, w, h, xs;
  Bit8u smask = 0;
  bool set;

  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Host to screen blt: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if ((pxconv_table[srcfmt] & (1 << BLT.dst_fmt)) == 0) {
    BX_ERROR(("Pixel format conversion not supported"));
  }
  BX_LOCK(render_mutex);
  if (srcfmt > 0) {
    if (srcfmt == 1) {
      spxsize = 1;
    } else if ((srcfmt >= 3) && (srcfmt <= 5)) {
      spxsize = srcfmt - 1;
    } else {
      spxsize = 4;
    }
  }
  y = BLT.dst_y;
  xs = BLT.h2s_pxstart;
  dst_ptr = &v->fbi.ram[BLT.dst_base + y * dpitch + BLT.dst_x * dpxsize];
  nrows = h;
  do {
    if (srcfmt == 0) {
      src_ptr1 = src_ptr + (xs >> 3);
      smask = 0x80 >> (xs & 7);
    } else {
      src_ptr1 = src_ptr + xs;
    }
    dst_ptr1 = dst_ptr;
    for (x = BLT.dst_x; x < (BLT.dst_x + w); x++) {
      if (blt_clip_check(x, y)) {
        if (srcfmt == 0) {
          set = (*src_ptr1 & smask) > 0;
          if (set) {
            color = &BLT.fgcolor[0];
          } else {
            color = &BLT.bgcolor[0];
          }
          if (set || !BLT.transp) {
            if (colorkey_en & 2) {
              rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
            }
            BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
          }
        } else {
          if (colorkey_en & 1) {
            rop = blt_colorkey_check(src_ptr1, spxsize, 0);
          }
          if (BLT.dst_fmt != srcfmt) {
            if ((srcfmt == 4) || (srcfmt == 5)) {
              b = src_ptr1[0];
              g = src_ptr1[1];
              r = src_ptr1[2];
            } else if (srcfmt == 3) {
              b = src_ptr1[0] << 3;
              g = ((src_ptr1[1] & 0x07) << 5) | ((src_ptr1[0] & 0xe0) >> 3);
              r = src_ptr1[1] & 0xf8;
            }
            if (dpxsize == 2) {
              scolor[0] = (b >> 3) | ((g & 0x1c) << 3);
              scolor[1] = (g >> 5) | (r & 0xf8);
              if (colorkey_en & 2) {
                rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
              }
              BLT.rop_fn[rop](dst_ptr1, scolor, dpitch, dpxsize, dpxsize, 1);
            } else if ((dpxsize == 3) || (dpxsize == 4)) {
              scolor[0] = b;
              scolor[1] = g;
              scolor[2] = r;
              scolor[3] = 0;
              if (colorkey_en & 2) {
                rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
              }
              BLT.rop_fn[rop](dst_ptr1, scolor, dpitch, dpxsize, dpxsize, 1);
            }
          } else {
            if (colorkey_en & 2) {
              rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
            }
            BLT.rop_fn[rop](dst_ptr1, src_ptr1, dpitch, dpxsize, dpxsize, 1);
          }
        }
      }
      if (srcfmt == 0) {
        smask >>= 1;
        if (smask == 0) {
          src_ptr1++;
          smask = 0x80;
        }
      } else {
        src_ptr1 += spxsize;
      }
      dst_ptr1 += dpxsize;
    }
    src_ptr += spitch;
    if (pxpack == 0) {
      if (srcfmt == 0) {
        xs += (Bit8u)(BLT.reg[blt_srcFormat] << 3);
        xs &= 0x1f;
        spitch = ((((w + xs + 7) >> 3) + 3) & ~3);
      } else {
        xs += (Bit8u)BLT.reg[blt_srcFormat];
        xs &= 0x03;
        spitch = ((w * spxsize + xs + 3) & ~3);
      }
    }
    if (BLT.y_dir) {
      dst_ptr -= dpitch;
      y--;
    } else {
      dst_ptr += dpitch;
      y++;
    }
  } while (--nrows);
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_host_to_screen_pattern()
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *src_ptr = &BLT.lamem[0];
  Bit8u *dst_ptr, *dst_ptr1, *src_ptr1, *pat_ptr1, *pat_ptr2;
  Bit8u *pat_ptr = &BLT.cpat[0][0];
  Bit16u spitch = BLT.h2s_pitch;
  Bit8u srcfmt = BLT.src_fmt;
  Bit8u pxpack = (BLT.reg[blt_srcFormat] >> 22) & 3;
  bool patmono = (BLT.reg[blt_command] >> 13) & 1;
  bool patrow0 = (BLT.reg[blt_commandExtra] & 0x08) > 0;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u spxsize = 0;
  Bit8u *srccolor, *patcolor;
  int nrows, dx, dy, w, h, xs;
  Bit8u smask = 0, pmask = 0, rop = 0, patline;
  bool set;

  w = BLT.dst_w;
  h = BLT.dst_h;
  BX_DEBUG(("Host to screen pattern blt: %d x %d  ROP0 %02X", w, h, BLT.rop[0]));
  if ((srcfmt != 0) && (BLT.dst_fmt != srcfmt)) {
    BX_ERROR(("Pixel format conversion not supported yet"));
  }
  BX_LOCK(render_mutex);
  if (srcfmt > 0) {
    if (srcfmt == 1) {
      spxsize = 1;
    } else if ((srcfmt >= 3) && (srcfmt <= 5)) {
      spxsize = srcfmt - 1;
    } else {
      spxsize = 4;
    }
  }
  dy = BLT.dst_y;
  xs = BLT.h2s_pxstart;
  dst_ptr = &v->fbi.ram[BLT.dst_base + dy * dpitch + BLT.dst_x * dpxsize];
  nrows = h;
  do {
    if (srcfmt == 0) {
      src_ptr1 = src_ptr + (xs >> 3);
      smask = 0x80 >> (xs & 7);
    } else {
      src_ptr1 = src_ptr + xs;
    }
    if (!patrow0) {
      patline = (dy + BLT.patsy) & 7;
      if (patmono) {
        pat_ptr1 = pat_ptr + patline;
      } else {
        pat_ptr1 = pat_ptr + patline * dpxsize * 8;
      }
    } else {
      pat_ptr1 = pat_ptr;
    }
    dst_ptr1 = dst_ptr;
    for (dx = BLT.dst_x; dx < (BLT.dst_x + w); dx++) {
      if (blt_clip_check(dx, dy)) {
        if (srcfmt == 0) {
          set = (*src_ptr1 & smask) > 0;
          if (set) {
            srccolor = &BLT.fgcolor[0];
          } else {
            srccolor = &BLT.bgcolor[0];
          }
        } else {
          srccolor = src_ptr1;
        }
        if (patmono) {
          pmask = 0x80 >> ((dx + BLT.patsx) & 7);
          set = (*pat_ptr1 & pmask) > 0;
          if (set) {
            patcolor = &BLT.fgcolor[0];
          } else {
            patcolor = &BLT.bgcolor[0];
          }
          if (set || !BLT.transp) {
            if (colorkey_en & 1) {
              rop = blt_colorkey_check(srccolor, dpxsize, 0);
            }
            if (colorkey_en & 2) {
              rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
            }
            bx_ternary_rop(BLT.rop[rop], dst_ptr1, srccolor, patcolor, dpxsize);
          }
        } else {
          pat_ptr2 = pat_ptr1 + ((dx + BLT.patsx) & 7) * dpxsize;
          patcolor = pat_ptr2;
          if (colorkey_en & 1) {
            rop = blt_colorkey_check(srccolor, dpxsize, 0);
          }
          if (colorkey_en & 2) {
            rop |= blt_colorkey_check(dst_ptr1, dpxsize, 1);
          }
          bx_ternary_rop(BLT.rop[rop], dst_ptr1, srccolor, patcolor, dpxsize);
        }
      }
      if (srcfmt == 0) {
        smask >>= 1;
        if (smask == 0) {
          src_ptr1++;
          smask = 0x80;
        }
      } else {
        src_ptr1 += spxsize;
      }
      dst_ptr1 += dpxsize;
    }
    src_ptr += spitch;
    if (pxpack == 0) {
      if (srcfmt == 0) {
        xs += (Bit8u)(BLT.reg[blt_srcFormat] << 3);
        xs &= 0x1f;
        spitch = ((((w + xs + 7) >> 3) + 3) & ~3);
      } else {
        xs += (Bit8u)BLT.reg[blt_srcFormat];
        xs &= 0x03;
        spitch = ((w * spxsize + xs + 3) & ~3);
      }
    }
    if (BLT.y_dir) {
      dst_ptr -= dpitch;
      dy--;
    } else {
      dst_ptr += dpitch;
      dy++;
    }
  } while (--nrows);
  blt_complete();
  BX_UNLOCK(render_mutex);
}

void bx_banshee_c::blt_line(bool pline)
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0;
  Bit8u *dst_ptr = &v->fbi.ram[BLT.dst_base];
  Bit8u *dst_ptr1;
  int i, deltax, deltay, numpixels, d, dinc1, dinc2;
  int x, xinc1, xinc2, y, yinc1, yinc2;
  int x0, y0, x1, y1;
  bool lstipple = ((BLT.reg[blt_command] >> 12) & 1);
  Bit8u lpattern = BLT.reg[blt_lineStipple];
  Bit8u lrepeat = (BLT.reg[blt_lineStyle] & 0xff);
  Bit8u lpat_max = ((BLT.reg[blt_lineStyle] >> 8) & 0x1f);
  Bit8u lrep_cnt = lrepeat - ((BLT.reg[blt_lineStyle] >> 16) & 0xff);
  Bit8u lpat_idx = ((BLT.reg[blt_lineStyle] >> 24) & 0x1f);

  BX_LOCK(render_mutex);
  x0 = BLT.src_x;
  y0 = BLT.src_y;
  x1 = BLT.dst_x;
  y1 = BLT.dst_y;
  if (pline) {
    BX_DEBUG(("Polyline: %d/%d  -> %d/%d  ROP0 %02X", x0, y0, x1, y1, BLT.rop[0]));
  } else {
    BX_DEBUG(("Line: %d/%d  -> %d/%d  ROP0 %02X", x0, y0, x1, y1, BLT.rop[0]));
  }
  deltax = abs(x1 - x0);
  deltay = abs(y1 - y0);
  if (deltax >= deltay) {
    numpixels = deltax + 1;
    d = (deltay << 1) - deltax;
    dinc1 = deltay << 1;
    dinc2 = (deltay - deltax) << 1;
    xinc1 = 1;
    xinc2 = 1;
    yinc1 = 0;
    yinc2 = 1;
  } else {
    numpixels = deltay + 1;
    d = (deltax << 1) - deltay;
    dinc1 = deltax << 1;
    dinc2 = (deltax - deltay) << 1;
    xinc1 = 0;
    xinc2 = 1;
    yinc1 = 1;
    yinc2 = 1;
  }

  if (x0 > x1) {
    xinc1 = -xinc1;
    xinc2 = -xinc2;
  }
  if (y0 > y1) {
    yinc1 = -yinc1;
    yinc2 = -yinc2;
  }
  x = x0;
  y = y0;

  for (i = 0; i < (numpixels - 1); i++) {
    if (blt_clip_check(x, y)) {
      dst_ptr1 = dst_ptr + y * dpitch + x * dpxsize;
      if (colorkey_en & 2) {
        rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
      }
      if (!lstipple) {
        BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
      } else {
        if ((lpattern & (1 << lpat_idx)) != 0) {
          BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
        } else if (!BLT.transp) {
          BLT.rop_fn[rop](dst_ptr1, BLT.bgcolor, dpitch, dpxsize, dpxsize, 1);
        }
      }
    }
    if (lrep_cnt == 0) {
      if (++lpat_idx > lpat_max) {
        lpat_idx = 0;
      }
      lrep_cnt = lrepeat;
    } else {
      lrep_cnt--;
    }
    if (d < 0) {
      d = d + dinc1;
      x = x + xinc1;
      y = y + yinc1;
    } else {
      d = d + dinc2;
      x = x + xinc2;
      y = y + yinc2;
    }
  }

  if (!pline) {
    dst_ptr1 = dst_ptr + y1 * dpitch + x1 * dpxsize;
    if (colorkey_en & 2) {
      rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
    }
    BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
  }
  blt_complete();
  BLT.reg[blt_srcXY] = BLT.reg[blt_dstXY];
  BLT.src_x = BLT.dst_x;
  BLT.src_y = BLT.dst_y;
  BX_UNLOCK(render_mutex);
}

int calc_line_xpos(int x1, int y1, int x2, int y2, int yc, bool r)
{
  int i, deltax, deltay, numpixels,
      d, dinc1, dinc2,
      x, xinc1, xinc2,
      y, yinc1, yinc2;
  int xl = -1, xr = -1;

  if (x1 == x2) {
    xl = xr = x1;
  } else {
    deltax = abs(x2 - x1);
    deltay = abs(y2 - y1);
    if (deltax >= deltay) {
      numpixels = deltax + 1;
      d = (deltay << 1) - deltax;
      dinc1 = deltay << 1;
      dinc2 = (deltay - deltax) << 1;
      xinc1 = 1;
      xinc2 = 1;
      yinc1 = 0;
      yinc2 = 1;
    } else {
      numpixels = deltay + 1;
      d = (deltax << 1) - deltay;
      dinc1 = deltax << 1;
      dinc2 = (deltax - deltay) << 1;
      xinc1 = 0;
      xinc2 = 1;
      yinc1 = 1;
      yinc2 = 1;
    }

    if (x1 > x2) {
      xinc1 = - xinc1;
      xinc2 = - xinc2;
    }
    if (y1 > y2) {
      yinc1 = - yinc1;
      yinc2 = - yinc2;
    }
    x = x1;
    y = y1;

    for (i = 0; i < numpixels; i++) {
      if (y == yc) {
        if (xl == -1) {
          xl = xr = x;
        } else {
          if (x < xl) xl = x;
          if (x > xr) xr = x;
        }
      }
      if (d < 0) {
        d = d + dinc1;
        x = x + xinc1;
        y = y + yinc1;
      } else {
        d = d + dinc2;
        x = x + xinc2;
        y = y + yinc2;
      }
    }
  }
  return (r ? xr : xl);
}

void bx_banshee_c::blt_polygon_fill(bool force)
{
  Bit32u dpitch = BLT.dst_pitch;
  Bit8u dpxsize = (BLT.dst_fmt > 1) ? (BLT.dst_fmt - 1) : 1;
  Bit8u *dst_ptr = &v->fbi.ram[BLT.dst_base];
  Bit8u *pat_ptr = &BLT.cpat[0][0];
  Bit8u *dst_ptr1, *pat_ptr1 = NULL;
  bool patmono = (BLT.reg[blt_command] >> 13) & 1;
  bool patrow0 = (BLT.reg[blt_commandExtra] & 0x08) > 0;
  bool set;
  Bit8u colorkey_en = BLT.reg[blt_commandExtra] & 3;
  Bit8u rop = 0, mask, patline;
  Bit8u *color;
  Bit16u x, y, x0, x1, y0, y1;

  if (force) {
    if (BLT.pgn_l1y == BLT.pgn_r1y) {
      return;
    } else if (BLT.pgn_l1y < BLT.pgn_r1y) {
      BLT.pgn_l1x = BLT.pgn_r1x;
      BLT.pgn_l1y = BLT.pgn_r1y;
    } else {
      BLT.pgn_r1x = BLT.pgn_l1x;
      BLT.pgn_r1y = BLT.pgn_l1y;
    }
  }
  if ((BLT.pgn_l1y > BLT.pgn_l0y) && (BLT.pgn_r1y > BLT.pgn_r0y)) {
    BLT.busy = 1;
    BX_LOCK(render_mutex);
    y0 = BLT.pgn_l0y;
    if (BLT.pgn_l1y < BLT.pgn_r1y) {
      y1 = BLT.pgn_l1y;
    } else {
      y1 = BLT.pgn_r1y;
    }
    for (y = y0; y < y1; y++) {
      x0 = calc_line_xpos(BLT.pgn_l0x, BLT.pgn_l0y, BLT.pgn_l1x, BLT.pgn_l1y, y, 0);
      if (y <= BLT.pgn_r0y) {
        x1 = calc_line_xpos(BLT.pgn_l0x, BLT.pgn_l0y, BLT.pgn_r0x, BLT.pgn_r0y, y, 1);
      } else {
        x1 = calc_line_xpos(BLT.pgn_r0x, BLT.pgn_r0y, BLT.pgn_r1x, BLT.pgn_r1y, y, 1);
      }
      if (BLT.pattern_blt) {
        if (!patrow0) {
          patline = (y + BLT.patsy) & 7;
          if (patmono) {
            pat_ptr1 = pat_ptr + patline;
          } else {
            pat_ptr1 = pat_ptr + patline * dpxsize * 8;
          }
        } else {
          pat_ptr1 = pat_ptr;
        }
      }
      dst_ptr1 = dst_ptr + y * dpitch + x0 * dpxsize;
      if (blt_clip_check(x0, y)) {
        if (colorkey_en & 2) {
          rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
        }
        if (BLT.pattern_blt) {
          if (patmono) {
            mask = 0x80 >> ((x0 + BLT.patsx) & 7);
            set = (*pat_ptr1 & mask) > 0;
            if (set) {
              color = &BLT.fgcolor[0];
            } else {
              color = &BLT.bgcolor[0];
            }
            if ((set) || !BLT.transp) {
              BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
            }
          } else {
            color = (pat_ptr1 + ((x0 + BLT.patsx) & 7) * dpxsize);
            BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
          }
        } else {
          BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
        }
      }
      dst_ptr1 += dpxsize;
      for (x = x0 + 1; x < x1; x++) {
        if (blt_clip_check(x, y)) {
          if (colorkey_en & 2) {
            rop = blt_colorkey_check(dst_ptr1, dpxsize, 1);
          }
          if (BLT.pattern_blt) {
            if (patmono) {
              mask = 0x80 >> ((x + BLT.patsx) & 7);
              set = (*pat_ptr1 & mask) > 0;
              if (set) {
                color = &BLT.fgcolor[0];
              } else {
                color = &BLT.bgcolor[0];
              }
              if ((set) || !BLT.transp) {
                BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
              }
            } else {
              color = (pat_ptr1 + ((x + BLT.patsx) & 7) * dpxsize);
              BLT.rop_fn[rop](dst_ptr1, color, dpitch, dpxsize, dpxsize, 1);
            }
          } else {
            BLT.rop_fn[rop](dst_ptr1, BLT.fgcolor, dpitch, dpxsize, dpxsize, 1);
          }
        }
        dst_ptr1 += dpxsize;
      }
    }
    BX_DEBUG(("Polygon fill: L0=(%d,%d) L1=(%d,%d) R0=(%d,%d) R1=(%d,%d) ROP0 %02X",
              BLT.pgn_l0x, BLT.pgn_l0y, BLT.pgn_l1x, BLT.pgn_l1y,
              BLT.pgn_r0x, BLT.pgn_r0y, BLT.pgn_r1x, BLT.pgn_r1y, BLT.rop[0]));
    if (y1 == BLT.pgn_l1y) {
      BLT.pgn_l0x = BLT.pgn_l1x;
      BLT.pgn_l0y = BLT.pgn_l1y;
    }
    if (y1 == BLT.pgn_r1y) {
      BLT.pgn_r0x = BLT.pgn_r1x;
      BLT.pgn_r0y = BLT.pgn_r1y;
    }
    blt_complete();
    BX_UNLOCK(render_mutex);
  }
}

#if BX_DEBUGGER
void bx_banshee_c::debug_dump(int argc, char **argv)
{
  bool is_agp = SIM->is_agp_device(BX_PLUGIN_VOODOO);
  if (v->banshee.io[io_vidProcCfg] & 0x01) {
    if (s.model == VOODOO_BANSHEE) {
      dbg_printf("Voodoo Banshee %s adapter\n\n", is_agp ? "AGP":"PCI");
    } else {
      dbg_printf("Voodoo3 %s adapter\n\n", is_agp ? "AGP":"PCI");
    }
    dbg_printf("current mode : %u x %u x %u ", v->fbi.width,
               v->fbi.height, v->banshee.disp_bpp);
    if ((v->banshee.io[io_vidProcCfg] & 0x180) == 0x080) {
      dbg_printf("(2D desktop mode)\n");
    } else if ((v->banshee.io[io_vidProcCfg] & 0x180) == 0x100) {
      dbg_printf("(3D overlay mode)\n");
    } else {
      dbg_printf("\n");
    }
    if (argc > 0) {
      dbg_printf("\nAdditional options not supported\n");
    }
  } else {
    theVoodooVga->debug_dump(argc, argv);
  }
}
#endif

#undef BLT

#undef LOG_THIS
#define LOG_THIS theVoodooVga->

bx_voodoo_vga_c::bx_voodoo_vga_c() : bx_vgacore_c()
{
  put("VVGA");
}

bx_voodoo_vga_c::~bx_voodoo_vga_c()
{
  s.memory = NULL;
}

bool bx_voodoo_vga_c::init_vga_extension(void)
{
  bool ret = 0;

  Bit8u model = (Bit8u)SIM->get_param_enum("model", (bx_list_c*)SIM->get_param(BXPN_VOODOO))->get();
  if (model < VOODOO_BANSHEE) {
    theVoodooDevice = new bx_voodoo_1_2_c();
    theVoodooDevice->init();
    init_iohandlers(read_handler, write_handler);
  } else {
    theVoodooDevice = new bx_banshee_c();
    theVoodooDevice->init();
    BX_VVGA_THIS s.memory = v->fbi.ram;
    BX_VVGA_THIS s.memsize = v->fbi.mask + 1;
    init_iohandlers(banshee_vga_read_handler, banshee_vga_write_handler);
    DEV_register_iowrite_handler(this, banshee_vga_write_handler, 0x0102, "banshee", 1);
    DEV_register_iowrite_handler(this, banshee_vga_write_handler, 0x46e8, "banshee", 1);
    BX_VVGA_THIS s.max_xres = 1920;
    BX_VVGA_THIS s.max_yres = 1440;
    v->banshee.disp_bpp = 8;
    BX_VVGA_THIS s.vclk[0] = 25175000;
    BX_VVGA_THIS s.vclk[1] = 28322000;
    BX_VVGA_THIS s.vclk[2] = 50000000;
    BX_VVGA_THIS s.vclk[3] = 25175000;
    BX_VVGA_THIS pci_enabled = 1;
    ret = 1;
  }
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("voodoo", theVoodooDevice);
#endif
  return ret;
}

void bx_voodoo_vga_c::reset(unsigned type)
{
  theVoodooDevice->reset(type);
}

void bx_voodoo_vga_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "voodoo_vga", "Voodoo VGA State");
  BX_VVGA_THIS vgacore_register_state(list);
  theVoodooDevice->register_state();
}

void bx_voodoo_vga_c::after_restore_state(void)
{
  bx_vgacore_c::after_restore_state();
  theVoodooDevice->after_restore_state();
}

void bx_voodoo_vga_c::redraw_area(unsigned x0, unsigned y0, unsigned width,
                                  unsigned height)
{
  if (v->banshee.io[io_vidProcCfg] & 0x01) {
    theVoodooDevice->redraw_area(x0, y0, width, height);
  } else {
    bx_vgacore_c::redraw_area(x0, y0, width, height);
  }
}

void bx_voodoo_vga_c::banshee_update_mode(void)
{
  Bit8u format = (v->banshee.io[io_vidProcCfg] >> 18) & 0x07;

  if (format < 4) {
    v->banshee.disp_bpp = (format + 1) << 3;
  } else {
    BX_ERROR(("Ignoring reserved pixel format"));
    return;
  }
  v->banshee.half_mode = (v->banshee.io[io_vidProcCfg] >> 4) & 1;
  if (v->banshee.half_mode && (v->fbi.height > v->fbi.width)) {
    v->fbi.width <<= 1;
    v->banshee.double_width = true;
  } else {
    v->banshee.double_width = false;
  }
  theVoodooDevice->update_timing();
  BX_INFO(("switched to %d x %d x %d @ %d Hz", v->fbi.width, v->fbi.height,
           v->banshee.disp_bpp, (unsigned)v->vertfreq));
  bx_gui->dimension_update(v->fbi.width, v->fbi.height, 0, 0, v->banshee.disp_bpp);
  bx_virt_timer.deactivate_timer(vga_vtimer_id);
  // compatibilty settings for VGA core
  BX_VVGA_THIS s.last_xres = v->fbi.width;
  BX_VVGA_THIS s.last_yres = v->fbi.height;
  BX_VVGA_THIS s.last_bpp = v->banshee.disp_bpp;
  BX_VVGA_THIS s.last_fh = 0;
}

void bx_voodoo_vga_c::banshee_set_dac_mode(bool mode)
{
  unsigned i;

  if (mode != v->banshee.dac_8bit) {
    if (mode) {
      for (i=0; i<256; i++) {
        s.pel.data[i].red <<= 2;
        s.pel.data[i].green <<= 2;
        s.pel.data[i].blue <<= 2;
      }
      BX_INFO(("DAC in 8 bit mode"));
    } else {
      for (i=0; i<256; i++) {
        s.pel.data[i].red >>= 2;
        s.pel.data[i].green >>= 2;
        s.pel.data[i].blue >>= 2;
      }
      BX_INFO(("DAC in standard mode"));
    }
    v->banshee.dac_8bit = mode;
    s.dac_shift = mode ? 0 : 2;
  }
}

void bx_voodoo_vga_c::banshee_set_vclk3(Bit32u value)
{
  BX_VVGA_THIS s.vclk[3] = value;
  if (BX_VVGA_THIS s.misc_output.clock_select == 3) {
    calculate_retrace_timing();
  }
}

Bit32u bx_voodoo_vga_c::get_retrace()
{
  Bit32u retval = 1;
  Bit64u display_usec =
    bx_virt_timer.time_usec(BX_VVGA_THIS vsync_realtime) % BX_VVGA_THIS s.vtotal_usec;
  if ((display_usec >= BX_VVGA_THIS s.vrstart_usec) &&
      (display_usec <= BX_VVGA_THIS s.vrend_usec)) {
    retval = 0;
  }
  return retval;
}

void bx_voodoo_vga_c::get_crtc_params(bx_crtc_params_t *crtcp, Bit32u *vclock)
{
  *vclock = BX_VVGA_THIS s.vclk[BX_VVGA_THIS s.misc_output.clock_select];
  crtcp->htotal = BX_VVGA_THIS s.CRTC.reg[0] + ((v->banshee.crtc[0x1a] & 0x01) << 8) + 5;
  crtcp->vtotal = BX_VVGA_THIS s.CRTC.reg[6] + ((BX_VVGA_THIS s.CRTC.reg[7] & 0x01) << 8) +
                  ((BX_VVGA_THIS s.CRTC.reg[7] & 0x20) << 4) +
                  ((v->banshee.crtc[0x1b] & 0x01) << 10) + 2;
  crtcp->vbstart = BX_VVGA_THIS s.CRTC.reg[21] +
                   ((BX_VVGA_THIS s.CRTC.reg[7] & 0x08) << 5) +
                   ((BX_VVGA_THIS s.CRTC.reg[9] & 0x20) << 4) +
                   ((v->banshee.crtc[0x1b] & 0x10) << 6);
  crtcp->vrstart = BX_VVGA_THIS s.CRTC.reg[16] +
                   ((BX_VVGA_THIS s.CRTC.reg[7] & 0x04) << 6) +
                   ((BX_VVGA_THIS s.CRTC.reg[7] & 0x80) << 2) +
                   ((v->banshee.crtc[0x1b] & 0x40) << 4);
}

void bx_voodoo_vga_c::update(void)
{
  if (v->banshee.io[io_vidProcCfg] & 0x01) {
    theVoodooDevice->update();
  } else if (!((v->banshee.io[io_vgaInit0] >> 12) & 1)) {
    BX_VVGA_THIS bx_vgacore_c::update();
  }
}

Bit32u bx_voodoo_vga_c::banshee_vga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
  UNUSED(this_ptr);
  Bit32u value;

  if ((io_len == 2) && ((address & 1) == 0)) {
    value = banshee_vga_read_handler(theVoodooVga,address,1);
    value |= (banshee_vga_read_handler(theVoodooVga,address+1,1) << 8);
    return value;
  }

  if (BX_VVGA_THIS s.misc_output.color_emulation && (address == 0x03b5))
    return 0xff;
  if (!BX_VVGA_THIS s.misc_output.color_emulation && (address == 0x03d5))
    return 0xff;

  switch (address) {
    case 0x03b5:
    case 0x03d5:
      if (BX_VVGA_THIS s.CRTC.address > 0x18) {
        if (BX_VVGA_THIS s.CRTC.address == 0x22) {
          value = bx_vgacore_c::read_handler(BX_VVGA_THIS_PTR, address, io_len);
        } else if (BX_VVGA_THIS s.CRTC.address <= 0x26) {
          if ((v->banshee.io[io_vgaInit0] & 0x440) == 0x040) {
            value = v->banshee.crtc[BX_VVGA_THIS s.CRTC.address];
            BX_DEBUG(("read from banshee CRTC address 0x%02x value 0x%02x",
                      BX_VVGA_THIS s.CRTC.address, value));
          } else {
            value = 0xff;
          }
        } else {
          value = 0xff;
        }
        break;
      }
    default:
      value = bx_vgacore_c::read_handler(BX_VVGA_THIS_PTR, address, io_len);
  }
  return value;

}

void bx_voodoo_vga_c::banshee_vga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
  UNUSED(this_ptr);
  Bit8u value8;

  if ((io_len == 2) && ((address & 1) == 0)) {
    banshee_vga_write_handler(theVoodooVga,address,value & 0xff,1);
    banshee_vga_write_handler(theVoodooVga,address+1,value >> 8,1);
    return;
  }

  if (BX_VVGA_THIS s.misc_output.color_emulation && (address == 0x03b5))
    return;
  if (!BX_VVGA_THIS s.misc_output.color_emulation && (address == 0x03d5))
    return;

  switch (address) {
    case 0x0102:
    case 0x46e8:
      // TODO
      return;

    case 0x03c9:
      value8 = (Bit8u)value;
      if (!v->banshee.dac_8bit) value8 <<= 2;
      switch (BX_VVGA_THIS s.pel.write_data_cycle) {
        case 0:
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] &= 0x00ffff;
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] |= (value8 << 16);
          break;
        case 1:
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] &= 0xff00ff;
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] |= (value8 << 8);
          break;
        case 2:
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] &= 0xffff00;
          v->fbi.clut[BX_VVGA_THIS s.pel.write_data_register] |= value8;
          break;
      }
      break;

    case 0x03b5:
    case 0x03d5:
      if (BX_VVGA_THIS s.CRTC.address > 0x18) {
        if (BX_VVGA_THIS s.CRTC.address <= 0x26) {
          if ((v->banshee.io[io_vgaInit0] & 0x440) == 0x040) {
            BX_DEBUG(("write to banshee CRTC address 0x%02x value 0x%02x",
                      BX_VVGA_THIS s.CRTC.address, value));
            v->banshee.crtc[BX_VVGA_THIS s.CRTC.address] = (Bit8u)value;
          }
        }
        return;
      }
      break;
  }
  bx_vgacore_c::write_handler(theVoodooVga, address, value, io_len);
}

Bit8u bx_voodoo_vga_c::mem_read(bx_phy_address addr)
{
  Bit32u offset = ((v->banshee.io[io_vgaInit1] & 0xffc00) << 5) + (addr & 0x1ffff);
  bool chain4 = ((v->banshee.io[io_vgaInit1] >> 20) & 1);

  if (chain4) {
    return v->fbi.ram[offset & v->fbi.mask];
  } else {
    return bx_vgacore_c::mem_read(addr);
  }
}

void bx_voodoo_vga_c::mem_write(bx_phy_address addr, Bit8u value)
{
  bool chain4 = ((v->banshee.io[io_vgaInit1] >> 20) & 1);
  Bit32u offset, start, end, pitch;
  unsigned xti, yti;

  if (chain4) {
    offset = (((v->banshee.io[io_vgaInit1] & 0x3ff) << 15) + (addr & 0x1ffff)) & v->fbi.mask;
    v->fbi.ram[offset] = value;
    start = v->banshee.io[io_vidDesktopStartAddr] & v->fbi.mask;
    pitch = v->banshee.io[io_vidDesktopOverlayStride] & 0x7fff;
    end = start + pitch * v->fbi.height;
    if ((offset >= start) && (offset < end)) {
      offset -= start;
      if (v->banshee.half_mode) {
        yti = (offset / pitch) / (Y_TILESIZE / 2);
      } else {
        yti = (offset / pitch) / Y_TILESIZE;
      }
      xti = ((offset % pitch) / (v->banshee.disp_bpp >> 3)) / X_TILESIZE;
      theVoodooDevice->set_tile_updated(xti, yti, 1);
    }
  } else {
    bx_vgacore_c::mem_write(addr, value);
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_VOODOO
