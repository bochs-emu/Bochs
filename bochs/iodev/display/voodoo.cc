/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2012-2014  The Bochs Project
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

// 3dfx Voodoo Graphics (SST-1/2) emulation (based on a patch for DOSBox)

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_VOODOO

#include "pci.h"
#include "vgacore.h"
#include "voodoo.h"
#include "virt_timer.h"

#define LOG_THIS theVoodooDevice->

bx_voodoo_c* theVoodooDevice = NULL;

#include "voodoo_types.h"
#include "voodoo_data.h"
#include "voodoo_main.h"
voodoo_state *v;
#include "voodoo_func.h"

// builtin configuration handling functions

void voodoo_init_options(void)
{
  static const char *voodoo_model_list[] = {
    "voodoo1",
    "voodoo2",
    NULL
  };

  bx_param_c *display = SIM->get_param("display");
  bx_list_c *menu = new bx_list_c(display, "voodoo", "Voodoo Graphics");
  menu->set_options(menu->SHOW_PARENT);
  bx_param_bool_c *enabled = new bx_param_bool_c(menu,
    "enabled",
    "Enable Voodoo Graphics emulation",
    "Enables the 3dfx Voodoo Graphics emulation",
    1);
  new bx_param_enum_c(menu,
    "model",
    "Voodoo model",
    "Selects the Voodoo model to emulate.",
    voodoo_model_list,
    VOODOO_1, VOODOO_1);
  enabled->set_dependent_list(menu->clone());
}

Bit32s voodoo_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "voodoo")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_VOODOO);
    for (int i = 1; i < num_params; i++) {
      if (SIM->parse_param_from_list(context, params[i], base) < 0) {
        BX_ERROR(("%s: unknown parameter for voodoo ignored.", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s voodoo_options_save(FILE *fp)
{
  return SIM->write_param_list(fp, (bx_list_c*) SIM->get_param(BXPN_VOODOO), NULL, 0);
}

// device plugin entry points

int CDECL libvoodoo_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theVoodooDevice = new bx_voodoo_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theVoodooDevice, BX_PLUGIN_VOODOO);
  // add new configuration parameter for the config interface
  voodoo_init_options();
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("voodoo", voodoo_options_parser, voodoo_options_save);
  return 0; // Success
}

void CDECL libvoodoo_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("voodoo");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("display");
  menu->remove("voodoo");
  delete theVoodooDevice;
}

// the device object

bx_voodoo_c::bx_voodoo_c()
{
  put("VOODOO");
  s.mode_change_timer_id = BX_NULL_TIMER_HANDLE;
  s.update_timer_id = BX_NULL_TIMER_HANDLE;
  v = NULL;
}

bx_voodoo_c::~bx_voodoo_c()
{
  if (v != NULL) {
    free(v->fbi.ram);
    free(v->tmu[0].ram);
    free(v->tmu[1].ram);
    delete v;
  }

  BX_DEBUG(("Exit"));
}

void bx_voodoo_c::init(void)
{
  // Read in values from config interface
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_VOODOO);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", base)->get()) {
    BX_INFO(("Voodoo disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("voodoo"))->set(0);
    return;
  }
  BX_VOODOO_THIS s.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_VOODOO_THIS s.devfunc, BX_PLUGIN_VOODOO,
                            "Experimental 3dfx Voodoo Graphics (SST-1/2)");

  if (BX_VOODOO_THIS s.mode_change_timer_id == BX_NULL_TIMER_HANDLE) {
    BX_VOODOO_THIS s.mode_change_timer_id = bx_virt_timer.register_timer(this, mode_change_timer_handler,
       1000, 0, 0, 0, "voodoo_mode_change");
  }
  if (BX_VOODOO_THIS s.update_timer_id == BX_NULL_TIMER_HANDLE) {
    BX_VOODOO_THIS s.update_timer_id = bx_virt_timer.register_timer(this, update_timer_handler,
       50000, 1, 0, 1, "voodoo_update");
  }
  BX_VOODOO_THIS s.vdraw.clock_enabled = 1;
  BX_VOODOO_THIS s.vdraw.output_on = 0;
  BX_VOODOO_THIS s.vdraw.override_on = 0;
  BX_VOODOO_THIS s.vdraw.screen_update_pending = 0;

  v = new voodoo_state;
  Bit8u model = (Bit8u)SIM->get_param_enum("model", base)->get();
  if (model == VOODOO_2) {
    init_pci_conf(0x121a, 0x0002, 0x02, 0x038000, 0x00);
    BX_VOODOO_THIS pci_conf[0x10] = 0x08;
  } else {
    init_pci_conf(0x121a, 0x0001, 0x02, 0x000000, 0x00);
  }
  BX_VOODOO_THIS pci_conf[0x3d] = BX_PCI_INTA;
  BX_VOODOO_THIS pci_base_address[0] = 0;

  voodoo_init(model);

  BX_INFO(("3dfx Voodoo Graphics adapter (model=%s) initialized",
           SIM->get_param_enum("model", base)->get_selected()));
}

void bx_voodoo_c::reset(unsigned type)
{
  unsigned i;

  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
    { 0x04, 0x00 }, { 0x05, 0x00 }, // command io / memory
    { 0x06, 0x00 }, { 0x07, 0x00 }, // status
    // address space 0x10 - 0x13
    { 0x11, 0x00 },
    { 0x12, 0x00 }, { 0x13, 0x00 },
    { 0x3c, 0x00 },                 // IRQ
    // initEnable
    { 0x40, 0x00 }, { 0x41, 0x00 },
    { 0x42, 0x00 }, { 0x43, 0x00 },
    // busSnoop0
    { 0x44, 0x00 }, { 0x45, 0x00 },
    { 0x46, 0x00 }, { 0x47, 0x00 },
    // busSnoop1
    { 0x48, 0x00 }, { 0x49, 0x00 },
    { 0x4a, 0x00 }, { 0x4b, 0x00 },

  };
  for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      BX_VOODOO_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }
  v->pci.init_enable = 0x00;

  // Deassert IRQ
  set_irq_level(0);
}

void bx_voodoo_c::register_state(void)
{
  char name[8];
  int i, j, k;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "voodoo", "Voodoo State");
  bx_list_c *vstate = new bx_list_c(list, "vstate", "Voodoo Device State");
  new bx_shadow_data_c(vstate, "reg", (Bit8u*)v->reg, sizeof(v->reg));
  new bx_shadow_num_c(vstate, "alt_regmap", &v->alt_regmap);
  new bx_shadow_num_c(vstate, "pci_init_enable", &v->pci.init_enable, BASE_HEX);
  bx_list_c *dac = new bx_list_c(vstate, "dac", "DAC");
  for (i = 0; i < 8; i++) {
    sprintf(name, "reg%d", i);
    new bx_shadow_num_c(dac, name, &v->dac.reg[i], BASE_HEX);
  }
  new bx_shadow_num_c(dac, "read_result", &v->dac.read_result, BASE_HEX);
  bx_list_c *fbi = new bx_list_c(vstate, "fbi", "framebuffer");
  new bx_shadow_data_c(fbi, "ram", v->fbi.ram, (4 << 20));
  new bx_shadow_num_c(fbi, "rgboffs0", &v->fbi.rgboffs[0], BASE_HEX);
  new bx_shadow_num_c(fbi, "rgboffs1", &v->fbi.rgboffs[1], BASE_HEX);
  new bx_shadow_num_c(fbi, "rgboffs2", &v->fbi.rgboffs[2], BASE_HEX);
  new bx_shadow_num_c(fbi, "auxoffs", &v->fbi.auxoffs, BASE_HEX);
  new bx_shadow_num_c(fbi, "frontbuf", &v->fbi.frontbuf);
  new bx_shadow_num_c(fbi, "backbuf", &v->fbi.backbuf);
  new bx_shadow_num_c(fbi, "swaps_pending", &v->fbi.swaps_pending);
  new bx_shadow_num_c(fbi, "yorigin", &v->fbi.yorigin);
  new bx_shadow_num_c(fbi, "width", &v->fbi.width);
  new bx_shadow_num_c(fbi, "height", &v->fbi.height);
  new bx_shadow_num_c(fbi, "rowpixels", &v->fbi.rowpixels);
  new bx_shadow_num_c(fbi, "vblank", &v->fbi.vblank);
  new bx_shadow_num_c(fbi, "vblank_count", &v->fbi.vblank_count);
  new bx_shadow_bool_c(fbi, "vblank_swap_pending", &v->fbi.vblank_swap_pending);
  new bx_shadow_num_c(fbi, "vblank_swap", &v->fbi.vblank_swap);
  new bx_shadow_num_c(fbi, "vblank_dont_swap", &v->fbi.vblank_dont_swap);
  new bx_shadow_bool_c(fbi, "cheating_allowed", &v->fbi.cheating_allowed);
  new bx_shadow_num_c(fbi, "sign", &v->fbi.sign);
  new bx_shadow_num_c(fbi, "ax", &v->fbi.ax);
  new bx_shadow_num_c(fbi, "ay", &v->fbi.ay);
  new bx_shadow_num_c(fbi, "bx", &v->fbi.bx);
  new bx_shadow_num_c(fbi, "by", &v->fbi.by);
  new bx_shadow_num_c(fbi, "cx", &v->fbi.cx);
  new bx_shadow_num_c(fbi, "cy", &v->fbi.cy);
  new bx_shadow_num_c(fbi, "startr", &v->fbi.startr);
  new bx_shadow_num_c(fbi, "startg", &v->fbi.startg);
  new bx_shadow_num_c(fbi, "startb", &v->fbi.startb);
  new bx_shadow_num_c(fbi, "starta", &v->fbi.starta);
  new bx_shadow_num_c(fbi, "startz", &v->fbi.startz);
  new bx_shadow_num_c(fbi, "startw", &v->fbi.startw);
  new bx_shadow_num_c(fbi, "drdx", &v->fbi.drdx);
  new bx_shadow_num_c(fbi, "dgdx", &v->fbi.dgdx);
  new bx_shadow_num_c(fbi, "dbdx", &v->fbi.dbdx);
  new bx_shadow_num_c(fbi, "dadx", &v->fbi.dadx);
  new bx_shadow_num_c(fbi, "dzdx", &v->fbi.dzdx);
  new bx_shadow_num_c(fbi, "dwdx", &v->fbi.dwdx);
  new bx_shadow_num_c(fbi, "drdy", &v->fbi.drdy);
  new bx_shadow_num_c(fbi, "dgdy", &v->fbi.dgdy);
  new bx_shadow_num_c(fbi, "dbdy", &v->fbi.dbdy);
  new bx_shadow_num_c(fbi, "dady", &v->fbi.dady);
  new bx_shadow_num_c(fbi, "dzdy", &v->fbi.dzdy);
  new bx_shadow_num_c(fbi, "dwdy", &v->fbi.dwdy);
  bx_list_c *fogblend = new bx_list_c(fbi, "fogblend", "");
  for (i = 0; i < 64; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(fogblend, name, &v->fbi.fogblend[i]);
  }
  bx_list_c *fogdelta = new bx_list_c(fbi, "fogdelta", "");
  for (i = 0; i < 64; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(fogdelta, name, &v->fbi.fogdelta[i]);
  }
  new bx_shadow_data_c(fbi, "clut", (Bit8u*)v->fbi.clut, sizeof(v->fbi.clut));
  new bx_shadow_bool_c(fbi, "clut_dirty", &v->fbi.clut_dirty);
  bx_list_c *tmu = new bx_list_c(vstate, "tmu", "textures");
  for (i = 0; i < MAX_TMU; i++) {
    sprintf(name, "%d", i);
    bx_list_c *num = new bx_list_c(tmu, name, "");
    new bx_shadow_data_c(num, "ram", v->tmu[i].ram, (4 << 20));
    new bx_shadow_bool_c(num, "regdirty", &v->tmu[i].regdirty);
    new bx_shadow_num_c(num, "starts", &v->tmu[i].starts);
    new bx_shadow_num_c(num, "startt", &v->tmu[i].startt);
    new bx_shadow_num_c(num, "startw", &v->tmu[i].startw);
    new bx_shadow_num_c(num, "dsdx", &v->tmu[i].dsdx);
    new bx_shadow_num_c(num, "dtdx", &v->tmu[i].dtdx);
    new bx_shadow_num_c(num, "dwdx", &v->tmu[i].dwdx);
    new bx_shadow_num_c(num, "dsdy", &v->tmu[i].dsdy);
    new bx_shadow_num_c(num, "dtdy", &v->tmu[i].dtdy);
    new bx_shadow_num_c(num, "dwdy", &v->tmu[i].dwdy);
    new bx_shadow_num_c(num, "lodmin", &v->tmu[i].lodmin);
    new bx_shadow_num_c(num, "lodmax", &v->tmu[i].lodmax);
    new bx_shadow_num_c(num, "lodbias", &v->tmu[i].lodbias);
    new bx_shadow_num_c(num, "lodmask", &v->tmu[i].lodmask);
    bx_list_c *lodoffs = new bx_list_c(num, "lodoffset", "");
    for (j = 0; j < 9; j++) {
      sprintf(name, "%d", j);
      new bx_shadow_num_c(lodoffs, name, &v->tmu[i].lodoffset[j]);
    }
    new bx_shadow_num_c(num, "detailmax", &v->tmu[i].detailmax);
    new bx_shadow_num_c(num, "detailbias", &v->tmu[i].detailbias);
    new bx_shadow_num_c(num, "wmask", &v->tmu[i].wmask);
    new bx_shadow_num_c(num, "hmask", &v->tmu[i].hmask);
    bx_list_c *ncc = new bx_list_c(num, "ncc", "");
    for (j = 0; j < 2; j++) {
      sprintf(name, "%d", j);
      bx_list_c *ncct = new bx_list_c(ncc, name, "");
      new bx_shadow_bool_c(ncct, "dirty", &v->tmu[i].ncc[j].dirty);
      bx_list_c *ir = new bx_list_c(ncct, "ir", "");
      bx_list_c *ig = new bx_list_c(ncct, "ig", "");
      bx_list_c *ib = new bx_list_c(ncct, "ib", "");
      bx_list_c *qr = new bx_list_c(ncct, "qr", "");
      bx_list_c *qg = new bx_list_c(ncct, "qg", "");
      bx_list_c *qb = new bx_list_c(ncct, "qb", "");
      for (k = 0; k < 4; k++) {
        sprintf(name, "%d", k);
        new bx_shadow_num_c(ir, name, &v->tmu[i].ncc[j].ir[k]);
        new bx_shadow_num_c(ig, name, &v->tmu[i].ncc[j].ig[k]);
        new bx_shadow_num_c(ib, name, &v->tmu[i].ncc[j].ib[k]);
        new bx_shadow_num_c(qr, name, &v->tmu[i].ncc[j].qr[k]);
        new bx_shadow_num_c(qg, name, &v->tmu[i].ncc[j].qg[k]);
        new bx_shadow_num_c(qb, name, &v->tmu[i].ncc[j].qb[k]);
      }
      bx_list_c *y = new bx_list_c(ncct, "y", "");
      for (k = 0; k < 16; k++) {
        sprintf(name, "%d", k);
        new bx_shadow_num_c(y, name, &v->tmu[i].ncc[j].y[k]);
      }
      new bx_shadow_data_c(ncct, "texel", (Bit8u*)v->tmu[i].ncc[j].texel, 256 * sizeof(rgb_t));
    }
    new bx_shadow_data_c(num, "palette", (Bit8u*)v->tmu[i].palette, 256 * sizeof(rgb_t));
    new bx_shadow_data_c(num, "palettea", (Bit8u*)v->tmu[i].palettea, 256 * sizeof(rgb_t));
  }
  new bx_shadow_num_c(vstate, "send_config", &v->send_config);
  bx_list_c *vdraw = new bx_list_c(list, "vdraw", "Voodoo Draw State");
  new bx_shadow_bool_c(vdraw, "clock_enabled", &BX_VOODOO_THIS s.vdraw.clock_enabled);
  new bx_shadow_bool_c(vdraw, "output_on", &BX_VOODOO_THIS s.vdraw.output_on);
  new bx_shadow_bool_c(vdraw, "override_on", &BX_VOODOO_THIS s.vdraw.override_on);

  register_pci_state(list);
}

void bx_voodoo_c::after_restore_state(void)
{
  if (DEV_pci_set_base_mem(BX_VOODOO_THIS_PTR, mem_read_handler, mem_write_handler,
                           &BX_VOODOO_THIS pci_base_address[0],
                           &BX_VOODOO_THIS pci_conf[0x10],
                           0x1000000)) {
    BX_INFO(("new mem base address: 0x%08x", BX_VOODOO_THIS pci_base_address[0]));
  }
  // force update
  v->fbi.video_changed = 1;
  BX_VOODOO_THIS s.vdraw.override_on = !BX_VOODOO_THIS s.vdraw.override_on;
  BX_VOODOO_THIS s.vdraw.frame_start = bx_pc_system.time_usec();
  mode_change_timer_handler(NULL);
}

bx_bool bx_voodoo_c::mem_read_handler(bx_phy_address addr, unsigned len,
                                      void *data, void *param)
{
  Bit32u *data_ptr = (Bit32u*)data;

  *data_ptr = voodoo_r((addr>>2) & 0x3FFFFF);
  return 1;
}

bx_bool bx_voodoo_c::mem_write_handler(bx_phy_address addr, unsigned len,
                                       void *data, void *param)
{
  Bit32u val = *(Bit32u*)data;

  if (len == 4) {
    voodoo_w((addr>>2) & 0x3FFFFF, val, 0xffffffff);
  } else if (len == 2) {
    if (addr & 3) {
      voodoo_w((addr>>2) & 0x3FFFFF, val<<16, 0xffff0000);
    } else {
      voodoo_w((addr>>2) & 0x3FFFFF, val, 0x0000ffff);
    }
  }
  return 1;
}

void bx_voodoo_c::mode_change_timer_handler(void *this_ptr)
{
  UNUSED(this_ptr);
  BX_VOODOO_THIS s.vdraw.screen_update_pending = 0;

  if ((!BX_VOODOO_THIS s.vdraw.clock_enabled || !BX_VOODOO_THIS s.vdraw.output_on) && BX_VOODOO_THIS s.vdraw.override_on) {
    // switching off
    bx_virt_timer.deactivate_timer(BX_VOODOO_THIS s.update_timer_id);
    DEV_vga_set_override(0, NULL);
    BX_VOODOO_THIS s.vdraw.override_on = 0;
    BX_VOODOO_THIS s.vdraw.width = 0;
    BX_VOODOO_THIS s.vdraw.height = 0;
  }

  if ((BX_VOODOO_THIS s.vdraw.clock_enabled && BX_VOODOO_THIS s.vdraw.output_on) && !BX_VOODOO_THIS s.vdraw.override_on) {
    // switching on
    if (!BX_VOODOO_THIS update_timing())
      return;
    DEV_vga_set_override(1, BX_VOODOO_THIS_PTR);
    BX_VOODOO_THIS s.vdraw.override_on = 1;
  }
}

bx_bool bx_voodoo_c::update_timing(void)
{
  if (!BX_VOODOO_THIS s.vdraw.clock_enabled || !BX_VOODOO_THIS s.vdraw.output_on)
    return 0;
  if ((v->reg[hSync].u == 0) || (v->reg[vSync].u == 0))
    return 0;
  int htotal = ((v->reg[hSync].u >> 16) & 0x3ff) + 1 + (v->reg[hSync].u & 0xff) + 1;
  int vtotal = ((v->reg[vSync].u >> 16) & 0xfff) + (v->reg[vSync].u & 0xfff);
  int vsync = ((v->reg[vSync].u >> 16) & 0xfff);
  double hfreq = (double)(v->dac.clk0_freq * 1000) / htotal;
  if (((v->reg[fbiInit1].u >> 20) & 3) == 1) { // VCLK div 2
    hfreq /= 2;
  }
  double vfreq = hfreq / (double)vtotal;
  BX_VOODOO_THIS s.vdraw.vtotal_usec = (unsigned)(1000000.0 / vfreq);
  BX_VOODOO_THIS s.vdraw.htotal_usec = (unsigned)(1000000.0 / hfreq);
  BX_VOODOO_THIS s.vdraw.vsync_usec = vsync * BX_VOODOO_THIS s.vdraw.htotal_usec;
  if ((BX_VOODOO_THIS s.vdraw.width != v->fbi.width) ||
      (BX_VOODOO_THIS s.vdraw.height != v->fbi.height)) {
    BX_VOODOO_THIS s.vdraw.width = v->fbi.width;
    BX_VOODOO_THIS s.vdraw.height = v->fbi.height;
    bx_gui->dimension_update(v->fbi.width, v->fbi.height, 0, 0, 16);
    update_timer_handler(NULL);
  }
  BX_INFO(("Voodoo output %dx%d@%uHz", v->fbi.width, v->fbi.height, (unsigned)vfreq));
  bx_virt_timer.activate_timer(BX_VOODOO_THIS s.update_timer_id, (Bit32u)BX_VOODOO_THIS s.vdraw.vtotal_usec, 1);
  return 1;
}

void bx_voodoo_c::refresh_display(void *this_ptr, bx_bool redraw)
{
  if (redraw) {
    redraw_area(0, 0, v->fbi.width, v->fbi.height);
  }
  update_timer_handler(this_ptr);
}

void bx_voodoo_c::update_timer_handler(void *this_ptr)
{
  UNUSED(this_ptr);

  update();
  bx_gui->flush();
}

void bx_voodoo_c::update(void)
{
  unsigned pitch;
  unsigned xc, yc, xti, yti;
  unsigned r, c, w, h;
  int i;
  unsigned long colour;
  Bit8u * vid_ptr, * vid_ptr2;
  Bit8u * tile_ptr, * tile_ptr2;
  bx_svga_tileinfo_t info;

  BX_VOODOO_THIS s.vdraw.frame_start = bx_pc_system.time_usec();

  if (v->fbi.vblank_swap_pending) {
    swap_buffers(v);
  }

  rectangle re;
  re.min_x = re.min_y = 0;
  re.max_x = v->fbi.width;
  re.max_y = v->fbi.height;
  if (!voodoo_update(&re))
    return;

  Bit8u *disp_ptr = (Bit8u*)(v->fbi.ram + v->fbi.rgboffs[v->fbi.frontbuf]);
  pitch = v->fbi.rowpixels * 2;

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < BX_VOODOO_THIS s.vdraw.height; yc++) {
          memcpy(tile_ptr, vid_ptr, info.pitch);
          vid_ptr += pitch;
          tile_ptr += info.pitch;
        }
      }
    } else if (info.is_indexed) {
      BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays"));
    } else {
      for (yc=0, yti = 0; yc<BX_VOODOO_THIS s.vdraw.height; yc+=Y_TILESIZE, yti++) {
        for (xc=0, xti = 0; xc<BX_VOODOO_THIS s.vdraw.width; xc+=X_TILESIZE, xti++) {
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
        }
      }
    }
  } else {
    BX_PANIC(("cannot get svga tile info"));
  }
}

void bx_voodoo_c::redraw_area(unsigned x0, unsigned y0, unsigned width,
                      unsigned height)
{
  // TODO: implement tile-based update mechanism
  v->fbi.video_changed = 1;
}

Bit16u bx_voodoo_c::get_retrace(void)
{
  Bit64u time_in_frame = bx_pc_system.time_usec()  - BX_VOODOO_THIS s.vdraw.frame_start;
  if (time_in_frame > BX_VOODOO_THIS s.vdraw.vsync_usec) {
    return 0;
  } else {
    return (Bit16u)((BX_VOODOO_THIS s.vdraw.vsync_usec - time_in_frame) / BX_VOODOO_THIS s.vdraw.htotal_usec + 1);
  }
}

void bx_voodoo_c::output_enable(bx_bool enabled)
{
  if (BX_VOODOO_THIS s.vdraw.output_on != enabled) {
    BX_VOODOO_THIS s.vdraw.output_on = enabled;
    update_screen_start();
  }
}

void bx_voodoo_c::update_screen_start(void)
{
  if (!BX_VOODOO_THIS s.vdraw.screen_update_pending) {
    BX_VOODOO_THIS s.vdraw.screen_update_pending = 1;
    bx_virt_timer.activate_timer(BX_VOODOO_THIS s.mode_change_timer_id, 1000, 0);
  }
}

void bx_voodoo_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_VOODOO_THIS s.devfunc, BX_VOODOO_THIS pci_conf[0x3d], level);
}


// pci configuration space read callback handler
Bit32u bx_voodoo_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_VOODOO_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%08x", address, value));

  return value;
}


// pci configuration space write callback handler
void bx_voodoo_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if ((address >= 0x14) && (address < 0x34))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_VOODOO_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x02;
        break;
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
        }
        break;
      case 0x10:
        value8 = (value8 & 0xf0) | (oldval & 0x0f);
      case 0x11:
      case 0x12:
      case 0x13:
        baseaddr_change |= (value8 != oldval);
        break;
      case 0x40:
      case 0x41:
      case 0x42:
      case 0x43:
        v->pci.init_enable &= ~(0xff << (i*8));
        v->pci.init_enable |= (value8 << (i*8));
        break;
      case 0xc0:
        BX_VOODOO_THIS s.vdraw.clock_enabled = 1;
        update_screen_start();
        break;
      case 0xe0:
        BX_VOODOO_THIS s.vdraw.clock_enabled = 0;
        update_screen_start();
        break;
      default:
        value8 = oldval;
    }
    BX_VOODOO_THIS pci_conf[address+i] = value8;
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_mem(BX_VOODOO_THIS_PTR, mem_read_handler, mem_write_handler,
                             &BX_VOODOO_THIS pci_base_address[0],
                             &BX_VOODOO_THIS pci_conf[0x10],
                             0x1000000)) {
      BX_INFO(("new mem base address: 0x%08x", BX_VOODOO_THIS pci_base_address[0]));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_VOODOO
