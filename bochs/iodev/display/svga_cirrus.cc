/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004 Makoto Suzuki (suzu)
//                     Volker Ruppert (vruppert)
//                     Robin Kay (komadori)
//  Copyright (C) 2004-2024  The Bochs Project
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

// limited PCI/ISA CLGD5446 support for Bochs
//
// there are still many unimplemented features:
//
// - BLT mode extension support is not complete (bit 3..4)
// - 4bpp modes
//
// some codes are copied from vga.cc and modified.
// some codes are ported from the cirrus emulation in qemu
//   (http://savannah.nongnu.org/projects/qemu).

#define BX_PLUGGABLE

#include "iodev.h"
#include "vgacore.h"
#define BX_USE_BINARY_ROP
#include "bitblt.h"
#include "ddc.h"
#include "svga_cirrus.h"
#include "virt_timer.h"

#if BX_SUPPORT_CLGD54XX

#define LOG_THIS BX_CIRRUS_THIS

#if BX_USE_CIRRUS_SMF
#define VGA_READ(addr,len)       bx_vgacore_c::read_handler(theSvga,addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write_handler(theSvga,addr,val,len)
#define SVGA_READ(addr,len)      svga_read_handler(theSvga,addr,len)
#define SVGA_WRITE(addr,val,len) svga_write_handler(theSvga,addr,val,len)
#else
#define VGA_READ(addr,len)       bx_vgacore_c::read(addr,len)
#define VGA_WRITE(addr,val,len)  bx_vgacore_c::write(addr,val,len)
#define SVGA_READ(addr,len)      svga_read(addr,len)
#define SVGA_WRITE(addr,val,len) svga_write(addr,val,len)
#endif // BX_USE_CIRRUS_SMF

#define ID_CLGD5428  (0x26<<2)
#define ID_CLGD5430  (0x28<<2)
#define ID_CLGD5434  (0x2A<<2)
#define ID_CLGD5446  (0x2E<<2)

// sequencer 0x07
#define CIRRUS_SR7_BPP_VGA            0x00
#define CIRRUS_SR7_BPP_SVGA           0x01
#define CIRRUS_SR7_BPP_MASK           0x0e
#define CIRRUS_SR7_BPP_8              0x00
#define CIRRUS_SR7_BPP_16_DOUBLEVCLK  0x02
#define CIRRUS_SR7_BPP_24             0x04
#define CIRRUS_SR7_BPP_16             0x06
#define CIRRUS_SR7_BPP_32             0x08
#define CIRRUS_SR7_ISAADDR_MASK       0xe0

// sequencer 0x0f
#define CIRRUS_MEMSIZE_512k        0x08
#define CIRRUS_MEMSIZE_1M          0x10
#define CIRRUS_MEMSIZE_2M          0x18
#define CIRRUS_MEMFLAGS_BANKSWITCH 0x80 // bank switching is enabled.

// sequencer 0x12
#define CIRRUS_CURSOR_SHOW         0x01
#define CIRRUS_CURSOR_HIDDENPEL    0x02
#define CIRRUS_CURSOR_LARGE        0x04 // 64x64 if set, 32x32 if clear

// sequencer 0x17
#define CIRRUS_BUSTYPE_VLBFAST   0x10
#define CIRRUS_BUSTYPE_PCI       0x20
#define CIRRUS_BUSTYPE_VLBSLOW   0x30
#define CIRRUS_BUSTYPE_ISA       0x38
#define CIRRUS_MMIO_ENABLE       0x04
#define CIRRUS_MMIO_USE_PCIADDR  0x40  // 0xb8000 if cleared.
#define CIRRUS_MEMSIZEEXT_DOUBLE 0x80

// control 0x0b
#define CIRRUS_BANKING_DUAL             0x01
#define CIRRUS_BANKING_GRANULARITY_16K  0x20  // set:16k, clear:4k

// control 0x30
#define CIRRUS_BLTMODE_BACKWARDS        0x01
#define CIRRUS_BLTMODE_MEMSYSDEST       0x02
#define CIRRUS_BLTMODE_MEMSYSSRC        0x04
#define CIRRUS_BLTMODE_TRANSPARENTCOMP  0x08
#define CIRRUS_BLTMODE_PATTERNCOPY      0x40
#define CIRRUS_BLTMODE_COLOREXPAND      0x80
#define CIRRUS_BLTMODE_PIXELWIDTHMASK   0x30
#define CIRRUS_BLTMODE_PIXELWIDTH8      0x00
#define CIRRUS_BLTMODE_PIXELWIDTH16     0x10
#define CIRRUS_BLTMODE_PIXELWIDTH24     0x20
#define CIRRUS_BLTMODE_PIXELWIDTH32     0x30

// control 0x31
#define CIRRUS_BLT_BUSY                 0x01
#define CIRRUS_BLT_START                0x02
#define CIRRUS_BLT_RESET                0x04
#define CIRRUS_BLT_FIFOUSED             0x10
#define CIRRUS_BLT_AUTOSTART            0x80

// control 0x32
#define CIRRUS_ROP_0                    0x00
#define CIRRUS_ROP_SRC_AND_DST          0x05
#define CIRRUS_ROP_NOP                  0x06
#define CIRRUS_ROP_SRC_AND_NOTDST       0x09
#define CIRRUS_ROP_NOTDST               0x0b
#define CIRRUS_ROP_SRC                  0x0d
#define CIRRUS_ROP_1                    0x0e
#define CIRRUS_ROP_NOTSRC_AND_DST       0x50
#define CIRRUS_ROP_SRC_XOR_DST          0x59
#define CIRRUS_ROP_SRC_OR_DST           0x6d
#define CIRRUS_ROP_NOTSRC_AND_NOTDST    0x90
#define CIRRUS_ROP_SRC_NOTXOR_DST       0x95
#define CIRRUS_ROP_SRC_OR_NOTDST        0xad
#define CIRRUS_ROP_NOTSRC               0xd0
#define CIRRUS_ROP_NOTSRC_OR_DST        0xd6
#define CIRRUS_ROP_NOTSRC_OR_NOTDST     0xda

// control 0x33
#define CIRRUS_BLTMODEEXT_SYNCDISPSWITCH   0x10 // unimplemented
#define CIRRUS_BLTMODEEXT_BKGNDONLYCLIP    0x08 // unimplemented
#define CIRRUS_BLTMODEEXT_SOLIDFILL        0x04
#define CIRRUS_BLTMODEEXT_COLOREXPINV      0x02
#define CIRRUS_BLTMODEEXT_DWORDGRANULARITY 0x01

#define CLGD543x_MMIO_BLTBGCOLOR    0x00 // dword
#define CLGD543x_MMIO_BLTFGCOLOR    0x04 // dword
#define CLGD543x_MMIO_BLTWIDTH      0x08 // word
#define CLGD543x_MMIO_BLTHEIGHT     0x0a // word
#define CLGD543x_MMIO_BLTDESTPITCH  0x0c // word
#define CLGD543x_MMIO_BLTSRCPITCH   0x0e // word
#define CLGD543x_MMIO_BLTDESTADDR   0x10 // dword
#define CLGD543x_MMIO_BLTSRCADDR    0x14 // dword
#define CLGD543x_MMIO_BLTWRITEMASK  0x17 // byte
#define CLGD543x_MMIO_BLTMODE       0x18 // byte
#define CLGD543x_MMIO_BLTROP        0x1a // byte
#define CLGD543x_MMIO_BLTMODEEXT    0x1b // byte
#define CLGD543x_MMIO_BLTTRANSPARENTCOLOR 0x1c // word?
#define CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK 0x20 // word?
#define CLGD543x_MMIO_BLTSTATUS     0x40 // byte

// PCI 0x00: vendor, 0x02: device
#define PCI_VENDOR_CIRRUS             0x1013
#define PCI_DEVICE_CLGD5430           0x00a0 // CLGD5430 or CLGD5440
#define PCI_DEVICE_CLGD5434           0x00a8
#define PCI_DEVICE_CLGD5436           0x00ac
#define PCI_DEVICE_CLGD5446           0x00b8
#define PCI_DEVICE_CLGD5462           0x00d0
#define PCI_DEVICE_CLGD5465           0x00d6
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
#define CIRRUS_PNPMEM_SIZE          CIRRUS_VIDEO_MEMORY_BYTES
#define CIRRUS_PNPMMIO_SIZE         0x1000

static bx_svga_cirrus_c *theSvga = NULL;

PLUGIN_ENTRY_FOR_MODULE(svga_cirrus)
{
  if (mode == PLUGIN_INIT) {
    theSvga = new bx_svga_cirrus_c();
    bx_devices.pluginVgaDevice = theSvga;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theSvga, BX_PLUGIN_CIRRUS);
  } else if (mode == PLUGIN_FINI) {
    delete theSvga;
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_VGA;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

bx_svga_cirrus_c::bx_svga_cirrus_c() : bx_vgacore_c()
{
  // nothing else to do
}

bx_svga_cirrus_c::~bx_svga_cirrus_c()
{
  SIM->get_bochs_root()->remove("svga_cirrus");
  BX_DEBUG(("Exit"));
}

bool bx_svga_cirrus_c::init_vga_extension(void)
{
  BX_CIRRUS_THIS put("CIRRUS");
  // initialize SVGA stuffs.
  BX_CIRRUS_THIS bx_vgacore_c::init_iohandlers(svga_read_handler, svga_write_handler);
  BX_CIRRUS_THIS pci_enabled = SIM->is_pci_device("cirrus");
  BX_CIRRUS_THIS svga_init_members();
#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled)
  {
    BX_CIRRUS_THIS svga_init_pcihandlers();
    BX_INFO(("CL-GD5446 PCI initialized"));
  }
  else
#endif
  {
    BX_INFO(("CL-GD5430 ISA initialized"));
  }
  BX_CIRRUS_THIS s.max_xres = 1600;
  BX_CIRRUS_THIS s.max_yres = 1200;
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("cirrus", this);
#endif
  return 1;
}

void bx_svga_cirrus_c::svga_init_members()
{
  unsigned i;

  // clear all registers.
  BX_CIRRUS_THIS sequencer.index = CIRRUS_SEQENCER_MAX + 1;
  for (i = 0; i <= CIRRUS_SEQENCER_MAX; i++)
    BX_CIRRUS_THIS sequencer.reg[i] = 0x00;
  BX_CIRRUS_THIS control.index = CIRRUS_CONTROL_MAX + 1;
  for (i = 0; i <= CIRRUS_CONTROL_MAX; i++)
    BX_CIRRUS_THIS control.reg[i] = 0x00;
  BX_CIRRUS_THIS control.shadow_reg0 = 0x00;
  BX_CIRRUS_THIS control.shadow_reg1 = 0x00;
  BX_CIRRUS_THIS crtc.index = CIRRUS_CRTC_MAX + 1;
  for (i = 0; i <= CIRRUS_CRTC_MAX; i++)
    BX_CIRRUS_THIS crtc.reg[i] = 0x00;
  BX_CIRRUS_THIS hidden_dac.lockindex = 0;
  BX_CIRRUS_THIS hidden_dac.data = 0x00;

  BX_CIRRUS_THIS svga_unlock_special = 0;
  BX_CIRRUS_THIS svga_needs_update_tile = 1;
  BX_CIRRUS_THIS svga_needs_update_dispentire = 1;
  BX_CIRRUS_THIS svga_needs_update_mode = 0;

  BX_CIRRUS_THIS svga_xres = 640;
  BX_CIRRUS_THIS svga_yres = 480;
  BX_CIRRUS_THIS svga_bpp = 8;
  BX_CIRRUS_THIS svga_pitch = 640;
  BX_CIRRUS_THIS bank_base[0] = 0;
  BX_CIRRUS_THIS bank_base[1] = 0;
  BX_CIRRUS_THIS bank_limit[0] = 0;
  BX_CIRRUS_THIS bank_limit[1] = 0;

  svga_reset_bitblt();

  BX_CIRRUS_THIS hw_cursor.x = 0;
  BX_CIRRUS_THIS hw_cursor.y = 0;
  BX_CIRRUS_THIS hw_cursor.size = 0;

  // memory allocation.
  if (BX_CIRRUS_THIS s.memory == NULL)
    BX_CIRRUS_THIS s.memory = new Bit8u[CIRRUS_VIDEO_MEMORY_BYTES];

  // set some registers.

  BX_CIRRUS_THIS sequencer.reg[0x06] = 0x0f;
  BX_CIRRUS_THIS sequencer.reg[0x07] = 0x00; // 0xf0:linearbase(0x00 if disabled)
#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled) {
    BX_CIRRUS_THIS svga_unlock_special = 1;
    BX_CIRRUS_THIS crtc.reg[0x27] = ID_CLGD5446;
    BX_CIRRUS_THIS sequencer.reg[0x1F] = 0x2d; // MemClock
    BX_CIRRUS_THIS control.reg[0x18] = 0x0f;
    BX_CIRRUS_THIS sequencer.reg[0x0F] = 0x98;
    BX_CIRRUS_THIS sequencer.reg[0x17] = CIRRUS_BUSTYPE_PCI;
    BX_CIRRUS_THIS sequencer.reg[0x15] = 0x04; // memory size 4MB
    BX_CIRRUS_THIS s.memsize = (4 << 20);
  } else
#endif
  {
    BX_CIRRUS_THIS crtc.reg[0x27] = ID_CLGD5430;
    BX_CIRRUS_THIS sequencer.reg[0x1F] = 0x22; // MemClock
    BX_CIRRUS_THIS sequencer.reg[0x0F] = CIRRUS_MEMSIZE_2M;
    BX_CIRRUS_THIS sequencer.reg[0x17] = CIRRUS_BUSTYPE_ISA;
    BX_CIRRUS_THIS sequencer.reg[0x15] = 0x03; // memory size 2MB
    BX_CIRRUS_THIS s.memsize = (2 << 20);
  }

  BX_CIRRUS_THIS hidden_dac.lockindex = 5;
  BX_CIRRUS_THIS hidden_dac.data = 0;

  memset(BX_CIRRUS_THIS s.memory, 0xff, CIRRUS_VIDEO_MEMORY_BYTES);
  BX_CIRRUS_THIS disp_ptr = BX_CIRRUS_THIS s.memory;
  BX_CIRRUS_THIS memsize_mask = BX_CIRRUS_THIS s.memsize - 1;

  // VCLK defaults after reset
  BX_CIRRUS_THIS sequencer.reg[0x0b] = 0x66;
  BX_CIRRUS_THIS sequencer.reg[0x0c] = 0x5b;
  BX_CIRRUS_THIS sequencer.reg[0x0d] = 0x45;
  BX_CIRRUS_THIS sequencer.reg[0x0e] = 0x7e;
  BX_CIRRUS_THIS sequencer.reg[0x1b] = 0x3b;
  BX_CIRRUS_THIS sequencer.reg[0x1c] = 0x2f;
  BX_CIRRUS_THIS sequencer.reg[0x1d] = 0x30;
  BX_CIRRUS_THIS sequencer.reg[0x1e] = 0x33;
  BX_CIRRUS_THIS s.vclk[0] = 25180000;
  BX_CIRRUS_THIS s.vclk[1] = 28325000;
  BX_CIRRUS_THIS s.vclk[2] = 41165000;
  BX_CIRRUS_THIS s.vclk[3] = 36082000;
}

void bx_svga_cirrus_c::reset(unsigned type)
{
  // reset VGA stuffs.
  BX_CIRRUS_THIS bx_vgacore_c::reset(type);
  // reset SVGA stuffs.
  BX_CIRRUS_THIS svga_init_members();
}

void bx_svga_cirrus_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "svga_cirrus", "Cirrus SVGA State");
  BX_CIRRUS_THIS vgacore_register_state(list);
  bx_list_c *crtc = new bx_list_c(list, "crtc");
  new bx_shadow_num_c(crtc, "index", &BX_CIRRUS_THIS crtc.index, BASE_HEX);
  new bx_shadow_data_c(crtc, "reg", BX_CIRRUS_THIS crtc.reg, CIRRUS_CRTC_MAX + 1, 1);
  bx_list_c *sequ = new bx_list_c(list, "sequencer");
  new bx_shadow_num_c(sequ, "index", &BX_CIRRUS_THIS sequencer.index, BASE_HEX);
  new bx_shadow_data_c(sequ, "reg", BX_CIRRUS_THIS sequencer.reg, CIRRUS_SEQENCER_MAX + 1, 1);
  bx_list_c *ctrl = new bx_list_c(list, "control");
  new bx_shadow_num_c(ctrl, "index", &BX_CIRRUS_THIS control.index, BASE_HEX);
  new bx_shadow_data_c(ctrl, "reg", BX_CIRRUS_THIS control.reg, CIRRUS_CONTROL_MAX + 1, 1);
  new bx_shadow_num_c(ctrl, "shadow_reg0", &BX_CIRRUS_THIS control.shadow_reg0, BASE_HEX);
  new bx_shadow_num_c(ctrl, "shadow_reg1", &BX_CIRRUS_THIS control.shadow_reg1, BASE_HEX);
  bx_list_c *hdac = new bx_list_c(list, "hidden_dac");
  new bx_shadow_num_c(hdac, "lockindex", &BX_CIRRUS_THIS hidden_dac.lockindex, BASE_HEX);
  new bx_shadow_num_c(hdac, "data", &BX_CIRRUS_THIS hidden_dac.data, BASE_HEX);
  new bx_shadow_data_c(hdac, "palette", BX_CIRRUS_THIS hidden_dac.palette, 48, 1);
  BXRS_PARAM_BOOL(list, svga_unlock_special, BX_CIRRUS_THIS svga_unlock_special);
  new bx_shadow_num_c(list, "svga_xres", &BX_CIRRUS_THIS svga_xres);
  new bx_shadow_num_c(list, "svga_yres", &BX_CIRRUS_THIS svga_yres);
  new bx_shadow_num_c(list, "svga_pitch", &BX_CIRRUS_THIS svga_pitch);
  new bx_shadow_num_c(list, "svga_bpp", &BX_CIRRUS_THIS svga_bpp);
  new bx_shadow_num_c(list, "svga_dispbpp", &BX_CIRRUS_THIS svga_dispbpp);
  new bx_shadow_num_c(list, "bank_base0", &BX_CIRRUS_THIS bank_base[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_base1", &BX_CIRRUS_THIS bank_base[1], BASE_HEX);
  new bx_shadow_num_c(list, "bank_limit0", &BX_CIRRUS_THIS bank_limit[0], BASE_HEX);
  new bx_shadow_num_c(list, "bank_limit1", &BX_CIRRUS_THIS bank_limit[1], BASE_HEX);
  bx_list_c *cursor = new bx_list_c(list, "hw_cursor");
  new bx_shadow_num_c(cursor, "x", &BX_CIRRUS_THIS hw_cursor.x, BASE_HEX);
  new bx_shadow_num_c(cursor, "y", &BX_CIRRUS_THIS hw_cursor.y, BASE_HEX);
  new bx_shadow_num_c(cursor, "size", &BX_CIRRUS_THIS hw_cursor.size, BASE_HEX);
#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled) {
    register_pci_state(list);
  }
#endif
}

void bx_svga_cirrus_c::after_restore_state(void)
{
#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled) {
    bx_pci_device_c::after_restore_pci_state(cirrus_mem_read_handler);
  }
#endif
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
    BX_CIRRUS_THIS bx_vgacore_c::after_restore_state();
  } else {
    for (unsigned i=0; i<256; i++) {
      bx_gui->palette_change_common(i, BX_CIRRUS_THIS s.pel.data[i].red<<2,
                                    BX_CIRRUS_THIS s.pel.data[i].green<<2,
                                    BX_CIRRUS_THIS s.pel.data[i].blue<<2);
    }
    BX_CIRRUS_THIS svga_needs_update_mode = 1;
    BX_CIRRUS_THIS update();
  }
}

void bx_svga_cirrus_c::redraw_area(unsigned x0, unsigned y0, unsigned width,
                                   unsigned height)
{
  unsigned xti, yti, xt0, xt1, yt0, yt1;

  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
    BX_CIRRUS_THIS bx_vgacore_c::redraw_area(x0,y0,width,height);
    return;
  }

  if (BX_CIRRUS_THIS svga_needs_update_mode) {
    return;
  }

  BX_CIRRUS_THIS svga_needs_update_tile = 1;

  xt0 = x0 / X_TILESIZE;
  yt0 = y0 / Y_TILESIZE;
  if (x0 < BX_CIRRUS_THIS svga_xres) {
    xt1 = (x0 + width  - 1) / X_TILESIZE;
  } else {
    xt1 = (BX_CIRRUS_THIS svga_xres - 1) / X_TILESIZE;
  }
  if (y0 < BX_CIRRUS_THIS svga_yres) {
    yt1 = (y0 + height - 1) / Y_TILESIZE;
  } else {
    yt1 = (BX_CIRRUS_THIS svga_yres - 1) / Y_TILESIZE;
  }
  if ((x0 + width) > svga_xres) {
    BX_CIRRUS_THIS redraw_area(0, y0 + 1, width, height);
  }
  for (yti=yt0; yti<=yt1; yti++) {
    for (xti=xt0; xti<=xt1; xti++) {
      SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 1);
    }
  }
}

void bx_svga_cirrus_c::mem_write_mode4and5_8bpp(Bit8u mode, Bit32u offset, Bit8u value)
{
  Bit8u val = value;
  Bit8u *dst;

  dst = BX_CIRRUS_THIS s.memory + offset;
  for (int x = 0; x < 8; x++) {
    if (val & 0x80) {
      *dst = BX_CIRRUS_THIS control.shadow_reg1;
    } else if (mode == 5) {
      *dst = BX_CIRRUS_THIS control.shadow_reg0;
    }
    val <<= 1;
    dst++;
  }
}

void bx_svga_cirrus_c::mem_write_mode4and5_16bpp(Bit8u mode, Bit32u offset, Bit8u value)
{
  Bit8u val = value;
  Bit8u *dst;

  dst = BX_CIRRUS_THIS s.memory + offset;
  for (int x = 0; x < 8; x++) {
    if (val & 0x80) {
      *dst = BX_CIRRUS_THIS control.shadow_reg1;
      *(dst + 1) = BX_CIRRUS_THIS control.reg[0x11];
    } else if (mode == 5) {
      *dst = BX_CIRRUS_THIS control.shadow_reg0;
      *(dst + 1) = BX_CIRRUS_THIS control.reg[0x10];
    }
    val <<= 1;
    dst += 2;
  }
}

#if BX_SUPPORT_PCI
bool bx_svga_cirrus_c::cirrus_mem_read_handler(bx_phy_address addr, unsigned len,
                                        void *data, void *param)
{
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    *data_ptr = BX_CIRRUS_THIS mem_read(addr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}
#endif

Bit8u bx_svga_cirrus_c::mem_read(bx_phy_address addr)
{
#if BX_SUPPORT_PCI
  if ((BX_CIRRUS_THIS pci_enabled) && (BX_CIRRUS_THIS pci_rom_size > 0)) {
    Bit32u mask = (BX_CIRRUS_THIS pci_rom_size - 1);
    if (((Bit32u)addr & ~mask) == BX_CIRRUS_THIS pci_rom_address) {
      if (BX_CIRRUS_THIS pci_conf[0x30] & 0x01) {
        return BX_CIRRUS_THIS pci_rom[addr & mask];
      } else {
        return 0xff;
      }
    }
  }
#endif

#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled) {
    if ((addr >= BX_CIRRUS_THIS pci_bar[0].addr) &&
        (addr < (BX_CIRRUS_THIS pci_bar[0].addr + CIRRUS_PNPMEM_SIZE))) {
      Bit8u *ptr;

      if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
        return 0xff;
      }

      Bit32u offset = addr & BX_CIRRUS_THIS memsize_mask;
      if ((offset >= (BX_CIRRUS_THIS s.memsize - 256)) &&
          ((BX_CIRRUS_THIS sequencer.reg[0x17] & 0x44) == 0x44)) {
        return svga_mmio_blt_read(offset & 0xff);
      }

      // video-to-cpu BLT
      if (BX_CIRRUS_THIS bitblt.memdst_needed != 0) {
        ptr = BX_CIRRUS_THIS bitblt.memdst_ptr;
        if (ptr != BX_CIRRUS_THIS bitblt.memdst_endptr) {
          BX_CIRRUS_THIS bitblt.memdst_ptr ++;
          return *ptr;
        }
        if (!svga_asyncbitblt_next()) {
          ptr = BX_CIRRUS_THIS bitblt.memdst_ptr;
          BX_CIRRUS_THIS bitblt.memdst_ptr ++;
          return *ptr;
        }
      }

      ptr = BX_CIRRUS_THIS s.memory;
      if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) == 0x14) {
        offset <<= 4;
      } else if (BX_CIRRUS_THIS control.reg[0x0b] & 0x02) {
        offset <<= 3;
      }
      offset &= BX_CIRRUS_THIS memsize_mask;
      return *(ptr + offset);
    } else if ((addr >= BX_CIRRUS_THIS pci_bar[1].addr) &&
      (addr < (BX_CIRRUS_THIS pci_bar[1].addr + CIRRUS_PNPMMIO_SIZE))) {

      Bit32u offset = addr & (CIRRUS_PNPMMIO_SIZE - 1);
      if (offset >= 0x100) {
        return svga_mmio_blt_read(offset - 0x100);
      } else {
        return svga_mmio_vga_read(offset);
      }
    }
  }
#endif // BX_SUPPORT_PCI
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
    return BX_CIRRUS_THIS bx_vgacore_c::mem_read(addr);
  }

  if (addr >= 0xA0000 && addr <= 0xAFFFF)
  {
    Bit32u bank;
    Bit32u offset;
    Bit8u *ptr;

    // video-to-cpu BLT
    if (BX_CIRRUS_THIS bitblt.memdst_needed != 0) {
      ptr = BX_CIRRUS_THIS bitblt.memdst_ptr;
      if (ptr != BX_CIRRUS_THIS bitblt.memdst_endptr) {
        BX_CIRRUS_THIS bitblt.memdst_ptr ++;
        return *ptr;
      }
      if (!svga_asyncbitblt_next()) {
        ptr = BX_CIRRUS_THIS bitblt.memdst_ptr;
        BX_CIRRUS_THIS bitblt.memdst_ptr ++;
        return *ptr;
      }
    }

    offset = addr & 0xffff;
    bank = (offset >> 15);
    offset &= 0x7fff;
    if (offset < bank_limit[bank]) {
      offset += bank_base[bank];
      if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) == 0x14) {
        offset <<= 4;
      } else if (BX_CIRRUS_THIS control.reg[0x0b] & 0x02) {
        offset <<= 3;
      }
      offset &= BX_CIRRUS_THIS memsize_mask;
      return *(BX_CIRRUS_THIS s.memory + offset);
    }
    else {
      return 0xff;
    }
  }
  else if (addr >= 0xB8000 && addr <= 0xB8100) {
    // memory-mapped I/O.
    Bit32u offset = (Bit32u) (addr - 0xb8000);
    if ((BX_CIRRUS_THIS sequencer.reg[0x17] & 0x44) == 0x04)
      return svga_mmio_blt_read(offset);
  }
  else {
    BX_DEBUG(("mem_read 0x%08x", (Bit32u)addr));
  }

  return 0xff;
}

#if BX_SUPPORT_PCI
bool bx_svga_cirrus_c::cirrus_mem_write_handler(bx_phy_address addr, unsigned len,
                                         void *data, void *param)
{
  Bit8u *data_ptr;
#ifdef BX_LITTLE_ENDIAN
  data_ptr = (Bit8u *) data;
#else // BX_BIG_ENDIAN
  data_ptr = (Bit8u *) data + (len - 1);
#endif
  for (unsigned i = 0; i < len; i++) {
    BX_CIRRUS_THIS mem_write(addr, *data_ptr);
    addr++;
#ifdef BX_LITTLE_ENDIAN
    data_ptr++;
#else // BX_BIG_ENDIAN
    data_ptr--;
#endif
  }
  return 1;
}
#endif

void bx_svga_cirrus_c::mem_write(bx_phy_address addr, Bit8u value)
{
#if BX_SUPPORT_PCI
  if (BX_CIRRUS_THIS pci_enabled) {
    if ((addr >= BX_CIRRUS_THIS pci_bar[0].addr) &&
        (addr < (BX_CIRRUS_THIS pci_bar[0].addr + CIRRUS_PNPMEM_SIZE))) {

      if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
        return;
      }

      Bit32u offset = addr & BX_CIRRUS_THIS memsize_mask;
      if ((offset >= (BX_CIRRUS_THIS s.memsize - 256)) &&
          ((BX_CIRRUS_THIS sequencer.reg[0x17] & 0x44) == 0x44)) {
        svga_mmio_blt_write(addr & 0xff, value);
        return;
      }

      // cpu-to-video BLT
      if (BX_CIRRUS_THIS bitblt.memsrc_needed > 0) {
        *(BX_CIRRUS_THIS bitblt.memsrc_ptr)++ = (value);
        if (BX_CIRRUS_THIS bitblt.memsrc_ptr >= BX_CIRRUS_THIS bitblt.memsrc_endptr) {
          svga_asyncbitblt_next();
        }
        return;
      }

      // BX_DEBUG(("write offset 0x%08x,value 0x%02x",offset,value));
      if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) == 0x14) {
        offset <<= 4;
      } else if (BX_CIRRUS_THIS control.reg[0x0b] & 0x02) {
        offset <<= 3;
      }
      offset &= BX_CIRRUS_THIS memsize_mask;
      Bit8u mode = BX_CIRRUS_THIS control.reg[0x05] & 0x07;
      if ((mode < 4) || (mode > 5) || ((BX_CIRRUS_THIS control.reg[0x0b] & 0x4) == 0)) {
        *(BX_CIRRUS_THIS s.memory + offset) = value;
      } else {
        if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) != 0x14) {
          mem_write_mode4and5_8bpp(mode, offset, value);
        } else {
          mem_write_mode4and5_16bpp(mode, offset, value);
        }
      }
      BX_CIRRUS_THIS svga_needs_update_tile = 1;
      SET_TILE_UPDATED(BX_CIRRUS_THIS, ((offset % BX_CIRRUS_THIS svga_pitch) / (BX_CIRRUS_THIS svga_bpp / 8)) / X_TILESIZE,
                       (offset / BX_CIRRUS_THIS svga_pitch) / Y_TILESIZE, 1);
      return;
    } else if ((addr >= BX_CIRRUS_THIS pci_bar[1].addr) &&
               (addr < (BX_CIRRUS_THIS pci_bar[1].addr + CIRRUS_PNPMMIO_SIZE))) {
      // memory-mapped I/O.

      // BX_DEBUG(("write mmio 0x%08x",addr));
      Bit32u offset = addr & (CIRRUS_PNPMMIO_SIZE - 1);
      if (offset >= 0x100) {
        svga_mmio_blt_write(offset - 0x100, value);
      } else {
        svga_mmio_vga_write(offset,value);
      }
      return;
    }
  }
#endif // BX_SUPPORT_PCI
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
    BX_CIRRUS_THIS bx_vgacore_c::mem_write(addr,value);
    return;
  }

  if (addr >= 0xA0000 && addr <= 0xAFFFF) {
    Bit32u bank, offset;
    Bit8u mode;

    // cpu-to-video BLT
    if (BX_CIRRUS_THIS bitblt.memsrc_needed > 0) {
      *(BX_CIRRUS_THIS bitblt.memsrc_ptr)++ = (value);
      if (BX_CIRRUS_THIS bitblt.memsrc_ptr >= BX_CIRRUS_THIS bitblt.memsrc_endptr) {
        svga_asyncbitblt_next();
      }
      return;
    }

    offset = addr & 0xffff;
    bank = (offset >> 15);
    offset &= 0x7fff;
    if (offset < bank_limit[bank]) {
      offset += bank_base[bank];
      if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) == 0x14) {
        offset <<= 4;
      } else if (BX_CIRRUS_THIS control.reg[0x0b] & 0x02) {
        offset <<= 3;
      }
      offset &= BX_CIRRUS_THIS memsize_mask;
      mode = BX_CIRRUS_THIS control.reg[0x05] & 0x07;
      if ((mode < 4) || (mode > 5) || ((BX_CIRRUS_THIS control.reg[0x0b] & 0x4) == 0)) {
        *(BX_CIRRUS_THIS s.memory + offset) = value;
      } else {
        if ((BX_CIRRUS_THIS control.reg[0x0b] & 0x14) != 0x14) {
          mem_write_mode4and5_8bpp(mode, offset, value);
        } else {
          mem_write_mode4and5_16bpp(mode, offset, value);
        }
      }
      BX_CIRRUS_THIS svga_needs_update_tile = 1;
      SET_TILE_UPDATED(BX_CIRRUS_THIS, ((offset % BX_CIRRUS_THIS svga_pitch) / (BX_CIRRUS_THIS svga_bpp / 8)) / X_TILESIZE,
                       (offset / BX_CIRRUS_THIS svga_pitch) / Y_TILESIZE, 1);
    }
  } else if (addr >= 0xB8000 && addr < 0xB8100) {
    // memory-mapped I/O.
    Bit32u offset = (Bit32u) (addr - 0xb8000);
    if ((BX_CIRRUS_THIS sequencer.reg[0x17] & 0x44) == 0x04) {
      svga_mmio_blt_write(offset & 0xff, value);
    }
  }
  else {
    BX_DEBUG(("mem_write 0x%08x, value 0x%02x", (Bit32u)addr, value));
  }
}

void bx_svga_cirrus_c::get_text_snapshot(Bit8u **text_snapshot,
                                    unsigned *txHeight, unsigned *txWidth)
{
  BX_CIRRUS_THIS bx_vgacore_c::get_text_snapshot(text_snapshot,txHeight,txWidth);
}

Bit32u bx_svga_cirrus_c::svga_read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_CIRRUS_SMF
  bx_svga_cirrus_c *class_ptr = (bx_svga_cirrus_c *) this_ptr;

  return class_ptr->svga_read(address, io_len);
}

Bit32u bx_svga_cirrus_c::svga_read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_CIRRUS_SMF

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
      return BX_CIRRUS_THIS crtc.index;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_CIRRUS_THIS is_unlocked())
        return BX_CIRRUS_THIS svga_read_crtc(address,BX_CIRRUS_THIS crtc.index);
      break;

    case 0x03c4: /* VGA: Sequencer Index Register */
      if (BX_CIRRUS_THIS is_unlocked()) {
        Bit32u value = BX_CIRRUS_THIS sequencer.index;
        if ((value & 0x1e) == 0x10) { /* SR10-F0, SR11-F1 */
          if (value & 1)
            value = ((BX_CIRRUS_THIS hw_cursor.y & 7) << 5) | 0x11;
          else
            value = ((BX_CIRRUS_THIS hw_cursor.x & 7) << 5) | 0x10;
        }
        return value;
      }
      return BX_CIRRUS_THIS sequencer.index;
    case 0x03c5: /* VGA: Sequencer Registers */
      if ((BX_CIRRUS_THIS sequencer.index == 0x06) ||
          (BX_CIRRUS_THIS is_unlocked())) {
        return BX_CIRRUS_THIS svga_read_sequencer(address,BX_CIRRUS_THIS sequencer.index);
      }
      break;

    case 0x03c6: /* Hidden DAC */
      if (BX_CIRRUS_THIS is_unlocked()) {
        if ((++BX_CIRRUS_THIS hidden_dac.lockindex) == 5) {
          BX_CIRRUS_THIS hidden_dac.lockindex = 0;
          return BX_CIRRUS_THIS hidden_dac.data;
        }
      }
      break;
    case 0x03c8: /* PEL write address */
      BX_CIRRUS_THIS hidden_dac.lockindex = 0;
      break;
    case 0x03c9: /* PEL Data Register, hiddem pel colors 00..0F */
      if (BX_CIRRUS_THIS sequencer.reg[0x12] & CIRRUS_CURSOR_HIDDENPEL) {
        Bit8u index = (BX_CIRRUS_THIS s.pel.read_data_register & 0x0f) * 3 +
                      BX_CIRRUS_THIS s.pel.read_data_cycle;
        Bit8u retval = BX_CIRRUS_THIS hidden_dac.palette[index];
        BX_CIRRUS_THIS s.pel.read_data_cycle ++;
        if (BX_CIRRUS_THIS s.pel.read_data_cycle >= 3) {
          BX_CIRRUS_THIS s.pel.read_data_cycle = 0;
          BX_CIRRUS_THIS s.pel.read_data_register++;
        }
        return retval;
      }
      break;
    case 0x03ce: /* VGA: Graphics Controller Index Register */
      return BX_CIRRUS_THIS control.index;
    case 0x03cf: /* VGA: Graphics Controller Registers */
      if (BX_CIRRUS_THIS is_unlocked())
        return BX_CIRRUS_THIS svga_read_control(address,BX_CIRRUS_THIS control.index);
      break;

    default:
      break;
  }

  return VGA_READ(address,io_len);
}

void bx_svga_cirrus_c::svga_write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_CIRRUS_SMF
  bx_svga_cirrus_c *class_ptr = (bx_svga_cirrus_c *) this_ptr;
  class_ptr->svga_write(address, value, io_len);
}

void bx_svga_cirrus_c::svga_write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_CIRRUS_SMF

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
      BX_CIRRUS_THIS crtc.index = value & 0x3f;
      break;
    case 0x03b5: /* VGA: CRTC Registers (monochrome emulation modes) */
    case 0x03d5: /* VGA: CRTC Registers (color emulation modes) */
      if (BX_CIRRUS_THIS is_unlocked()) {
        BX_CIRRUS_THIS svga_write_crtc(address,BX_CIRRUS_THIS crtc.index,value);
        return;
      }
      break;

    case 0x03c4: /* VGA: Sequencer Index Register */
      BX_CIRRUS_THIS sequencer.index = value;
      break;
    case 0x03c5: /* VGA: Sequencer Registers */
      if ((BX_CIRRUS_THIS sequencer.index == 0x06) ||
          (BX_CIRRUS_THIS is_unlocked())) {
        BX_CIRRUS_THIS svga_write_sequencer(address,BX_CIRRUS_THIS sequencer.index,value);
        return;
      }
      break;
    case 0x03c6: /* Hidden DAC */
      if (BX_CIRRUS_THIS is_unlocked()) {
        if (BX_CIRRUS_THIS hidden_dac.lockindex == 4) {
          BX_CIRRUS_THIS hidden_dac.data = value;
        }
        BX_CIRRUS_THIS hidden_dac.lockindex = 0;
        return;
      }
      break;
    case 0x03c9: /* PEL Data Register, hidden pel colors 00..0F */
      BX_CIRRUS_THIS svga_needs_update_dispentire = 1;

      if (BX_CIRRUS_THIS sequencer.reg[0x12] & CIRRUS_CURSOR_HIDDENPEL) {
        Bit8u index = (BX_CIRRUS_THIS s.pel.write_data_register & 0x0f) * 3 +
                      BX_CIRRUS_THIS s.pel.write_data_cycle;
        BX_CIRRUS_THIS hidden_dac.palette[index] = value;
        BX_CIRRUS_THIS s.pel.write_data_cycle ++;
        if (BX_CIRRUS_THIS s.pel.write_data_cycle >= 3) {
          BX_CIRRUS_THIS s.pel.write_data_cycle = 0;
          BX_CIRRUS_THIS s.pel.write_data_register++;
        }
        return;
      }
      break;
    case 0x03ce: /* VGA: Graphics Controller Index Register */
      BX_CIRRUS_THIS control.index = value;
      break;
    case 0x03cf: /* VGA: Graphics Controller Registers */
      if (BX_CIRRUS_THIS is_unlocked()) {
        BX_CIRRUS_THIS svga_write_control(address,BX_CIRRUS_THIS control.index,value);
        return;
      }
      break;
    default:
      break;
  }

  VGA_WRITE(address,value,io_len);
}

void bx_svga_cirrus_c::svga_modeupdate(void)
{
  Bit32u iTopOffset, iWidth, iHeight, vclock = 0;
  Bit8u iBpp, iDispBpp;
  bx_crtc_params_t crtcp;
  float hfreq, vfreq;

  iTopOffset = (BX_CIRRUS_THIS crtc.reg[0x0c] << 8)
       + BX_CIRRUS_THIS crtc.reg[0x0d]
       + ((BX_CIRRUS_THIS crtc.reg[0x1b] & 0x01) << 16)
       + ((BX_CIRRUS_THIS crtc.reg[0x1b] & 0x0c) << 15)
       + ((BX_CIRRUS_THIS crtc.reg[0x1d] & 0x80) << 12);
  iTopOffset <<= 2;

  iHeight = 1 + BX_CIRRUS_THIS crtc.reg[0x12]
       + ((BX_CIRRUS_THIS crtc.reg[0x07] & 0x02) << 7)
       + ((BX_CIRRUS_THIS crtc.reg[0x07] & 0x40) << 3);
  if (BX_CIRRUS_THIS s.ext_y_dblsize) {
    iHeight <<= 1; // interlaced mode
  }
  iWidth = (BX_CIRRUS_THIS crtc.reg[0x01] + 1) * 8;
  iBpp = 8;
  iDispBpp = 4;
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x1) == CIRRUS_SR7_BPP_SVGA) {
    switch (BX_CIRRUS_THIS sequencer.reg[0x07] & CIRRUS_SR7_BPP_MASK) {
    case CIRRUS_SR7_BPP_8:
      iBpp = 8;
      iDispBpp = 8;
      break;
    case CIRRUS_SR7_BPP_16_DOUBLEVCLK:
    case CIRRUS_SR7_BPP_16:
      iBpp = 16;
      iDispBpp = (BX_CIRRUS_THIS hidden_dac.data & 0x1) ? 16 : 15;
      break;
    case CIRRUS_SR7_BPP_24:
      iBpp = 24;
      iDispBpp = 24;
      break;
    case CIRRUS_SR7_BPP_32:
      iBpp = 32;
      iDispBpp = 32;
      break;
    default:
      BX_PANIC(("unknown bpp - seqencer.reg[0x07] = %02x",BX_CIRRUS_THIS sequencer.reg[0x07]));
      break;
    }
  }
  BX_CIRRUS_THIS get_crtc_params(&crtcp, &vclock);
  hfreq = vclock / (float)(crtcp.htotal * 8);
  vfreq = hfreq / (float)crtcp.vtotal;
  if ((iWidth != BX_CIRRUS_THIS svga_xres) || (iHeight != BX_CIRRUS_THIS svga_yres)
      || (iDispBpp != BX_CIRRUS_THIS svga_dispbpp)) {
    if (!BX_CIRRUS_THIS s.ext_y_dblsize) {
      BX_INFO(("switched to %u x %u x %u @ %.1f Hz", iWidth, iHeight, iDispBpp,
               vfreq));
    } else {
      BX_INFO(("switched to %u x %u x %u @ %.1f Hz (interlaced)", iWidth,
               iHeight, iDispBpp, vfreq / 2.0f));
    }
  }
  BX_CIRRUS_THIS svga_xres = iWidth;
  BX_CIRRUS_THIS svga_yres = iHeight;
  BX_CIRRUS_THIS svga_bpp = iBpp;
  BX_CIRRUS_THIS svga_dispbpp = iDispBpp;
  BX_CIRRUS_THIS disp_ptr = BX_CIRRUS_THIS s.memory + iTopOffset;
  // compatibilty settings for VGA core
  BX_CIRRUS_THIS s.last_xres = iWidth;
  BX_CIRRUS_THIS s.last_yres = iHeight;
  BX_CIRRUS_THIS s.last_bpp = iDispBpp;
  BX_CIRRUS_THIS s.last_fh = 0;
}

void bx_svga_cirrus_c::draw_hardware_cursor(unsigned xc, unsigned yc, bx_svga_tileinfo_t *info)
{
  if (BX_CIRRUS_THIS hw_cursor.size &&
      (xc < (unsigned)(BX_CIRRUS_THIS hw_cursor.x+BX_CIRRUS_THIS hw_cursor.size)) &&
      (xc+X_TILESIZE > BX_CIRRUS_THIS hw_cursor.x) &&
      (yc < (unsigned)(BX_CIRRUS_THIS hw_cursor.y+BX_CIRRUS_THIS hw_cursor.size)) &&
      (yc+Y_TILESIZE > BX_CIRRUS_THIS hw_cursor.y)) {
    int i;
    unsigned w, h, pitch, cx, cy, cx0, cy0, cx1, cy1;

    Bit8u * tile_ptr, * tile_ptr2;
    Bit8u * plane0_ptr, *plane0_ptr2;
    Bit8u * plane1_ptr, *plane1_ptr2;
    unsigned long fgcol, bgcol;
    Bit64u plane0, plane1;

    cx0 = BX_CIRRUS_THIS hw_cursor.x > xc ? BX_CIRRUS_THIS hw_cursor.x : xc;
    cy0 = BX_CIRRUS_THIS hw_cursor.y > yc ? BX_CIRRUS_THIS hw_cursor.y : yc;
    cx1 = (unsigned)(BX_CIRRUS_THIS hw_cursor.x+BX_CIRRUS_THIS hw_cursor.size) < xc+X_TILESIZE ? BX_CIRRUS_THIS hw_cursor.x+BX_CIRRUS_THIS hw_cursor.size : xc+X_TILESIZE;
    cy1 = (unsigned)(BX_CIRRUS_THIS hw_cursor.y+BX_CIRRUS_THIS hw_cursor.size) < yc+Y_TILESIZE ? BX_CIRRUS_THIS hw_cursor.y+BX_CIRRUS_THIS hw_cursor.size : yc+Y_TILESIZE;

    if (info->bpp == 15) info->bpp = 16;
    tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h) +
               info->pitch * (cy0 - yc) + (info->bpp / 8) * (cx0 - xc);
    plane0_ptr = BX_CIRRUS_THIS s.memory + BX_CIRRUS_THIS s.memsize - 16384;

    switch (BX_CIRRUS_THIS hw_cursor.size) {
      case 32:
        plane0_ptr += (BX_CIRRUS_THIS sequencer.reg[0x13] & 0x3f) * 256;
        plane1_ptr = plane0_ptr + 128;
        pitch = 4;
        break;

      case 64:
        plane0_ptr += (BX_CIRRUS_THIS sequencer.reg[0x13] & 0x3c) * 256;
        plane1_ptr = plane0_ptr + 8;
        pitch = 16;
        break;

      default:
        BX_ERROR(("unsupported hardware cursor size"));
        return;
        break;
    }

    if (!info->is_indexed) {
      fgcol = MAKE_COLOUR(
        BX_CIRRUS_THIS hidden_dac.palette[45], 6, info->red_shift, info->red_mask,
        BX_CIRRUS_THIS hidden_dac.palette[46], 6, info->green_shift, info->green_mask,
        BX_CIRRUS_THIS hidden_dac.palette[47], 6, info->blue_shift, info->blue_mask);
      bgcol = MAKE_COLOUR(
        BX_CIRRUS_THIS hidden_dac.palette[0], 6, info->red_shift, info->red_mask,
        BX_CIRRUS_THIS hidden_dac.palette[1], 6, info->green_shift, info->green_mask,
        BX_CIRRUS_THIS hidden_dac.palette[2], 6, info->blue_shift, info->blue_mask);
    } else {
      // FIXME: this is a hack that works in Windows guests
      // TODO: compare hidden DAC entries with DAC entries to find nearest match
      fgcol = 0xff;
      bgcol = 0x00;
    }

    plane0_ptr += pitch * (cy0 - BX_CIRRUS_THIS hw_cursor.y);
    plane1_ptr += pitch * (cy0 - BX_CIRRUS_THIS hw_cursor.y);
    for (cy=cy0; cy<cy1; cy++) {
      tile_ptr2 = tile_ptr + (info->bpp/8) * (cx1 - cx0) - 1;
      plane0_ptr2 = plane0_ptr;
      plane1_ptr2 = plane1_ptr;
      plane0 = plane1 = 0;
      for (i=0; i<BX_CIRRUS_THIS hw_cursor.size; i+=8) {
        plane0 = (plane0 << 8) | *(plane0_ptr2++);
        plane1 = (plane1 << 8) | *(plane1_ptr2++);
      }
      plane0 >>= BX_CIRRUS_THIS hw_cursor.x+BX_CIRRUS_THIS hw_cursor.size - cx1;
      plane1 >>= BX_CIRRUS_THIS hw_cursor.x+BX_CIRRUS_THIS hw_cursor.size - cx1;
      for (cx=cx0; cx<cx1; cx++) {
        if (plane0 & 1) {
          if (plane1 & 1) {
            if (info->is_little_endian) {
              for (i=info->bpp-8; i>-8; i-=8) {
                *(tile_ptr2--) = (Bit8u)(fgcol >> i);
              }
            }
            else {
              for (i=0; i<info->bpp; i+=8) {
                *(tile_ptr2--) = (Bit8u)(fgcol >> i);
              }
            }
          }
          else {
            for (i=0; i<info->bpp; i+=8) {
              *(tile_ptr2--) ^= 0xff;
            }
          }
        }
        else {
          if (plane1 & 1) {
            if (info->is_little_endian) {
              for (i=info->bpp-8; i>-8; i-=8) {
                *(tile_ptr2--) = (Bit8u)(bgcol >> i);
              }
            }
            else {
              for (i=0; i<info->bpp; i+=8) {
                *(tile_ptr2--) = (Bit8u)(bgcol >> i);
              }
            }
          }
          else {
            tile_ptr2 -= (info->bpp/8);
          }
        }
        plane0 >>= 1;
        plane1 >>= 1;
      }
      tile_ptr += info->pitch;
      plane0_ptr += pitch;
      plane1_ptr += pitch;
    }
  }
}

void bx_svga_cirrus_c::update(void)
{
  unsigned width, height, pitch;

  /* skip screen update when the sequencer is in reset mode or video is disabled */
  if (! BX_CIRRUS_THIS s.sequencer.reset1 ||
      ! BX_CIRRUS_THIS s.sequencer.reset2 ||
      ! BX_CIRRUS_THIS s.attribute_ctrl.video_enabled) {
    return;
  }

  if (BX_CIRRUS_THIS svga_needs_update_mode) {
    BX_CIRRUS_THIS s.ext_y_dblsize = (BX_CIRRUS_THIS crtc.reg[0x1a] & 0x01) > 0;
  }
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_VGA) {
    if (BX_CIRRUS_THIS svga_needs_update_mode) {
      BX_CIRRUS_THIS s.vga_mem_updated = 1;
      BX_CIRRUS_THIS svga_needs_update_mode = 0;
    }
    BX_CIRRUS_THIS bx_vgacore_c::update();
    return;
  } else {
    if (BX_CIRRUS_THIS svga_needs_update_mode) {
      svga_modeupdate();
    }
  }

  width  = BX_CIRRUS_THIS svga_xres;
  height = BX_CIRRUS_THIS svga_yres;
  pitch = BX_CIRRUS_THIS svga_pitch;

  if (BX_CIRRUS_THIS svga_needs_update_mode) {
    width  = BX_CIRRUS_THIS svga_xres;
    height = BX_CIRRUS_THIS svga_yres;
    bx_gui->dimension_update(width, height, 0, 0, BX_CIRRUS_THIS svga_dispbpp);
    BX_CIRRUS_THIS s.last_bpp = BX_CIRRUS_THIS svga_dispbpp;
    BX_CIRRUS_THIS svga_needs_update_mode = 0;
    BX_CIRRUS_THIS svga_needs_update_dispentire = 1;
  }

  if (BX_CIRRUS_THIS svga_needs_update_dispentire) {
    BX_CIRRUS_THIS redraw_area(0,0,width,height);
    BX_CIRRUS_THIS svga_needs_update_dispentire = 0;
  }

  if (!BX_CIRRUS_THIS svga_needs_update_tile) {
    return;
  }
  BX_CIRRUS_THIS svga_needs_update_tile = 0;

  unsigned xc, yc, xti, yti;
  unsigned r, c, w, h;
  int i;
  Bit8u red, green, blue;
  Bit32u colour;
  Bit8u * vid_ptr, * vid_ptr2;
  Bit8u * tile_ptr, * tile_ptr2;
  bx_svga_tileinfo_t info;

  if (bx_gui->graphics_tile_info_common(&info)) {
    if (info.snapshot_mode) {
      vid_ptr = BX_CIRRUS_THIS disp_ptr;
      tile_ptr = bx_gui->get_snapshot_buffer();
      if (tile_ptr != NULL) {
        for (yc = 0; yc < height; yc++) {
          memcpy(tile_ptr, vid_ptr, info.pitch);
          vid_ptr += pitch;
          tile_ptr += info.pitch;
        }
      }
    } else if (info.is_indexed) {
      switch (BX_CIRRUS_THIS svga_dispbpp) {
        case 4:
        case 15:
        case 16:
        case 24:
        case 32:
          BX_ERROR(("current guest pixel format is unsupported on indexed colour host displays, svga_dispbpp=%d",
            BX_CIRRUS_THIS svga_dispbpp));
          break;
        case 8:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + xc);
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = 0;
                    for (i=0; i<(int)BX_CIRRUS_THIS svga_bpp; i+=8) {
                      colour |= *(vid_ptr2++) << i;
                    }
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  vid_ptr  += pitch;
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
              }
            }
          }
          break;
      }
    }
    else {
      switch (BX_CIRRUS_THIS svga_dispbpp) {
        case 4:
          BX_ERROR(("cannot draw 4bpp SVGA"));
          break;
        case 8:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                if (!BX_CIRRUS_THIS s.y_doublescan) {
                  vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + xc);
                } else {
                  vid_ptr = BX_CIRRUS_THIS disp_ptr + ((yc >> 1) * pitch + xc);
                }
                tile_ptr = bx_gui->graphics_tile_get(xc, yc, &w, &h);
                for (r=0; r<h; r++) {
                  vid_ptr2  = vid_ptr;
                  tile_ptr2 = tile_ptr;
                  for (c=0; c<w; c++) {
                    colour = *(vid_ptr2++);
                    colour = MAKE_COLOUR(
                      BX_CIRRUS_THIS s.pel.data[colour].red, 6, info.red_shift, info.red_mask,
                      BX_CIRRUS_THIS s.pel.data[colour].green, 6, info.green_shift, info.green_mask,
                      BX_CIRRUS_THIS s.pel.data[colour].blue, 6, info.blue_shift, info.blue_mask);
                    if (info.is_little_endian) {
                      for (i=0; i<info.bpp; i+=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  if (!BX_CIRRUS_THIS s.y_doublescan || (r & 1)) {
                    vid_ptr += pitch;
                  }
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 15:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + (xc<<1));
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
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  vid_ptr  += pitch;
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 16:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + (xc<<1));
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
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  vid_ptr  += pitch;
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 24:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + 3*xc);
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
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  vid_ptr  += pitch;
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
              }
            }
          }
          break;
        case 32:
          for (yc=0, yti = 0; yc<height; yc+=Y_TILESIZE, yti++) {
            for (xc=0, xti = 0; xc<width; xc+=X_TILESIZE, xti++) {
              if (GET_TILE_UPDATED (xti, yti)) {
                vid_ptr = BX_CIRRUS_THIS disp_ptr + (yc * pitch + (xc<<2));
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
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                    else {
                      for (i=info.bpp-8; i>-8; i-=8) {
                        *(tile_ptr2++) = colour >> i;
                      }
                    }
                  }
                  vid_ptr  += pitch;
                  tile_ptr += info.pitch;
                }
                draw_hardware_cursor(xc, yc, &info);
                bx_gui->graphics_tile_update_in_place(xc, yc, w, h);
                SET_TILE_UPDATED(BX_CIRRUS_THIS, xti, yti, 0);
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

void bx_svga_cirrus_c::update_bank_ptr(Bit8u bank_index)
{
  unsigned offset;
  unsigned limit;

  if (BX_CIRRUS_THIS banking_is_dual())
    offset = BX_CIRRUS_THIS control.reg[0x09 + bank_index];
  else
    offset = BX_CIRRUS_THIS control.reg[0x09];

  if (BX_CIRRUS_THIS banking_granularity_is_16k())
    offset <<= 14;
  else
    offset <<= 12;

  if (BX_CIRRUS_THIS s.memsize <= offset) {
    limit = 0;
    BX_ERROR(("bank offset %08x is invalid",offset));
  } else {
    limit = BX_CIRRUS_THIS s.memsize - offset;
  }

  if (!BX_CIRRUS_THIS banking_is_dual() && (bank_index != 0)) {
    if (limit > 0x8000) {
      offset += 0x8000;
      limit -= 0x8000;
    } else {
      limit = 0;
    }
  }

  if (limit > 0) {
    BX_CIRRUS_THIS bank_base[bank_index] = offset;
    BX_CIRRUS_THIS bank_limit[bank_index] = limit;
    if ((BX_CIRRUS_THIS crtc.reg[0x1b] & 0x02) != 0) {
      BX_CIRRUS_THIS s.ext_offset = BX_CIRRUS_THIS bank_base[0];
    }
  } else {
    BX_CIRRUS_THIS bank_base[bank_index] = 0;
    BX_CIRRUS_THIS bank_limit[bank_index] = 0;
  }
}

Bit8u bx_svga_cirrus_c::svga_read_crtc(Bit32u address, unsigned index)
{
  switch (index) {
    case 0x00: // VGA
    case 0x01: // VGA
    case 0x02: // VGA
    case 0x03: // VGA
    case 0x04: // VGA
    case 0x05: // VGA
    case 0x06: // VGA
    case 0x07: // VGA
    case 0x08: // VGA
    case 0x09: // VGA
    case 0x0a: // VGA
    case 0x0b: // VGA
    case 0x0c: // VGA
    case 0x0d: // VGA
    case 0x0e: // VGA
    case 0x0f: // VGA
    case 0x10: // VGA
    case 0x11: // VGA
    case 0x12: // VGA
    case 0x13: // VGA
    case 0x14: // VGA
    case 0x15: // VGA
    case 0x16: // VGA
    case 0x17: // VGA
    case 0x18: // VGA
      break;
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x24:
    case 0x25:
    case 0x27:
      break;
    case 0x22:
      return VGA_READ(address,1);
      break;
    case 0x26:
      return (BX_CIRRUS_THIS s.attribute_ctrl.address & 0x3f);
    default:
      BX_DEBUG(("CRTC index 0x%02x is unknown(read)", index));
      break;
  }

  if (index <= VGA_CRTC_MAX) {
    return VGA_READ(address,1);
  }

  if (index <= CIRRUS_CRTC_MAX) {
    return BX_CIRRUS_THIS crtc.reg[index];
  }

  return 0xff;
}

void bx_svga_cirrus_c::svga_write_crtc(Bit32u address, unsigned index, Bit8u value)
{
  BX_DEBUG(("crtc: index 0x%02x write 0x%02x", index, (unsigned)value));

  bool update_pitch = 0;

  switch (index) {
    case 0x00: // VGA
    case 0x02: // VGA
    case 0x03: // VGA
    case 0x04: // VGA
    case 0x05: // VGA
    case 0x06: // VGA
    case 0x08: // VGA
    case 0x0a: // VGA
    case 0x0b: // VGA
    case 0x0e: // VGA
    case 0x0f: // VGA
    case 0x10: // VGA
    case 0x11: // VGA
    case 0x14: // VGA
    case 0x15: // VGA
    case 0x16: // VGA
    case 0x17: // VGA
    case 0x18: // VGA
      break;

    case 0x01: // VGA
    case 0x07: // VGA
    case 0x09: // VGA
    case 0x0c: // VGA (display offset 0x00ff00)
    case 0x0d: // VGA (display offset 0x0000ff)
    case 0x12: // VGA
    case 0x1A: // 0x01: interlaced video mode
    case 0x1D: // 0x80: offset 0x080000 (>=CLGD5434)
      BX_CIRRUS_THIS svga_needs_update_mode = 1;
      break;

    case 0x13: // VGA
    case 0x1B: // 0x01: offset 0x010000, 0x0c: offset 0x060000
      update_pitch = 1;
      break;

    case 0x19:
    case 0x1C:
      break;

    default:
      BX_DEBUG(("CRTC index 0x%02x is unknown(write 0x%02x)", index, (unsigned)value));
      return;
  }

  if (index <= CIRRUS_CRTC_MAX) {
    BX_CIRRUS_THIS crtc.reg[index] = value;
  }
  if (index <= VGA_CRTC_MAX) {
    VGA_WRITE(address,value,1);
  }

  if (update_pitch) {
    if ((BX_CIRRUS_THIS crtc.reg[0x1b] & 0x02) != 0) {
      if (!BX_CIRRUS_THIS s.sequencer.chain_four) {
        BX_CIRRUS_THIS s.plane_shift = 19;
      }
      BX_CIRRUS_THIS s.ext_offset = BX_CIRRUS_THIS bank_base[0];
    } else {
      BX_CIRRUS_THIS s.plane_shift = 16;
      BX_CIRRUS_THIS s.ext_offset = 0;
    }
    BX_CIRRUS_THIS svga_pitch = (BX_CIRRUS_THIS crtc.reg[0x13] << 3) | ((BX_CIRRUS_THIS crtc.reg[0x1b] & 0x10) << 7);
    BX_CIRRUS_THIS svga_needs_update_mode = 1;
  }
}

Bit8u bx_svga_cirrus_c::svga_read_sequencer(Bit32u address, unsigned index)
{
  Bit8u value;

  switch (index) {
    case 0x00:  // VGA
    case 0x01:  // VGA
    case 0x02:  // VGA
    case 0x03:  // VGA
    case 0x04:  // VGA
      break;
    case 0x6: // cirrus unlock extensions
    case 0x7: // cirrus extended sequencer mode
    case 0xf: // cirrus dram control
    case 0x12: // graphics cursor attribute
    case 0x13: // graphics cursor pattern address offset
    case 0x17: // configuration readback & extended control
      break;
    case 0x10: // cursor xpos << 5 (index & 0x3f)
    case 0x30:
    case 0x50:
    case 0x70:
    case 0x90:
    case 0xb0:
    case 0xd0:
    case 0xf0:
      return BX_CIRRUS_THIS sequencer.reg[0x10];
    case 0x11: // cursor ypos << 5 (index & 0x3f)
    case 0x31:
    case 0x51:
    case 0x71:
    case 0x91:
    case 0xb1:
    case 0xd1:
    case 0xf1:
      return BX_CIRRUS_THIS sequencer.reg[0x11];
    case 0x08: // DDC / EEPROM
      if ((BX_CIRRUS_THIS sequencer.reg[0x08] & 0x40) != 0) {
        value = BX_CIRRUS_THIS ddc.read();
        value = (value & 0x07) | ((value & 0x08) << 4);
        return (BX_CIRRUS_THIS sequencer.reg[0x08] & 0x40) | value;
      }
      break;
    default:
      BX_DEBUG(("sequencer index 0x%02x is unknown(read)", index));
      break;
  }

  if (index <= VGA_SEQENCER_MAX) {
    return VGA_READ(address,1);
  }

  if (index <= CIRRUS_SEQENCER_MAX) {
    return BX_CIRRUS_THIS sequencer.reg[index];
  }

  return 0xff;
}

void bx_svga_cirrus_c::svga_write_sequencer(Bit32u address, unsigned index, Bit8u value)
{
  BX_DEBUG(("sequencer: index 0x%02x write 0x%02x", index, (unsigned)value));

  bool update_cursor = 0;
  Bit16u x, y, size;
  Bit8u i, n, d, p;
  float mclk;

  x = BX_CIRRUS_THIS hw_cursor.x;
  y = BX_CIRRUS_THIS hw_cursor.y;
  size = BX_CIRRUS_THIS hw_cursor.size;

  switch (index) {
    case 0x00:  // VGA
    case 0x02:  // VGA
    case 0x03:  // VGA
      break;
    case 0x01:  // VGA
    case 0x04:  // VGA
      BX_CIRRUS_THIS svga_needs_update_mode = 1;
      break;
    case 0x6: // cirrus unlock extensions
      value &= 0x17;
      if (value == 0x12) {
        BX_CIRRUS_THIS svga_unlock_special = 1;
        BX_CIRRUS_THIS sequencer.reg[0x6] = 0x12;
      } else {
#if BX_SUPPORT_PCI
        BX_CIRRUS_THIS svga_unlock_special = 0;
#else
        if (!BX_CIRRUS_THIS pci_enabled) {
          BX_CIRRUS_THIS svga_unlock_special = 0;
        }
#endif
        BX_CIRRUS_THIS sequencer.reg[0x6] = 0x0f;
      }
      return;
    case 0x7: // cirrus extended sequencer mode
      if (value != BX_CIRRUS_THIS sequencer.reg[0x7]) {
        BX_CIRRUS_THIS svga_needs_update_mode = 1;
      }
      break;
    case 0x08: // DDC / EEPROM
      if ((value & 0x40) != 0) {
        BX_CIRRUS_THIS ddc.write(value & 1, (value >> 1) & 1);
      }
      break;
    case 0x09:
    case 0x0a: // cirrus scratch reg 1
      break;
    case 0x0b: // VCLK stuff
    case 0x0c:
    case 0x0d:
    case 0x0e:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
      if (value != BX_CIRRUS_THIS sequencer.reg[index]) {
        BX_CIRRUS_THIS sequencer.reg[index] = value;
        i = (index & 0x0f) - 11;
        n = BX_CIRRUS_THIS sequencer.reg[0x0b + i];
        d = BX_CIRRUS_THIS sequencer.reg[0x1b + i] >> 1;
        p = BX_CIRRUS_THIS sequencer.reg[0x1b + i] & 1;
        if (d > 0) {
          if (!p) {
            BX_CIRRUS_THIS s.vclk[i] = (Bit32u)(14318180.0f * ((double)n / (double)d));
          } else {
            BX_CIRRUS_THIS s.vclk[i] = (Bit32u)(14318180.0f * ((double)n / (double)(d << 1)));
          }
          BX_DEBUG(("VCLK%d = %.3f MHz", i, (double)BX_CIRRUS_THIS s.vclk[i] / 1000000.0f));
        }
      }
      break;
    case 0x1f:
      if (value != BX_CIRRUS_THIS sequencer.reg[index]) {
        if ((value & 0x40) != 0) {
          BX_ERROR(("SR1F: Using MCLK as VCLK not implemented yet"));
        }
        mclk = (float)(value & 0x3f) * 14318180.0f / 8.0f;
        BX_DEBUG(("SR1F: MCLK = %.3f MHz (unused)", mclk / 1000000.0f));
      }
      break;
    case 0x0f:
      return;
    case 0x10: // cursor xpos << 5 (index & 0x3f)
    case 0x30:
    case 0x50:
    case 0x70:
    case 0x90:
    case 0xb0:
    case 0xd0:
    case 0xf0:
      BX_CIRRUS_THIS sequencer.reg[0x10] = value;
      x = BX_CIRRUS_THIS hw_cursor.x;
      BX_CIRRUS_THIS hw_cursor.x = (value << 3) | (index >> 5);
      update_cursor = 1;
      break;
    case 0x11: // cursor ypos << 5 (index & 0x3f)
    case 0x31:
    case 0x51:
    case 0x71:
    case 0x91:
    case 0xb1:
    case 0xd1:
    case 0xf1:
      BX_CIRRUS_THIS sequencer.reg[0x11] = value;
      y = BX_CIRRUS_THIS hw_cursor.y;
      BX_CIRRUS_THIS hw_cursor.y = (value << 3) | (index >> 5);
      update_cursor = 1;
      break;
    case 0x12:
      size = BX_CIRRUS_THIS hw_cursor.size;
      if (value & CIRRUS_CURSOR_SHOW) {
        if (value & CIRRUS_CURSOR_LARGE) {
          BX_CIRRUS_THIS hw_cursor.size = 64;
        }
        else {
         BX_CIRRUS_THIS hw_cursor.size = 32;
        }
      }
      else {
        BX_CIRRUS_THIS hw_cursor.size = 0;
      }
      update_cursor = 1;
      break;
    case 0x13:
      update_cursor = 1;
      break;
    case 0x17:
      value = (BX_CIRRUS_THIS sequencer.reg[0x17] & 0x38) | (value & 0xc7);
      break;
    default:
      BX_DEBUG(("sequencer index 0x%02x is unknown(write 0x%02x)", index, (unsigned)value));
      break;
  }

  if (update_cursor) {
    BX_CIRRUS_THIS vga_redraw_area(x, y, size, size);
    BX_CIRRUS_THIS vga_redraw_area(BX_CIRRUS_THIS hw_cursor.x, BX_CIRRUS_THIS hw_cursor.y, BX_CIRRUS_THIS hw_cursor.size, BX_CIRRUS_THIS hw_cursor.size);
  }

  if (index <= CIRRUS_SEQENCER_MAX) {
    BX_CIRRUS_THIS sequencer.reg[index] = value;
  }
  if (index <= VGA_SEQENCER_MAX) {
    VGA_WRITE(address,value,1);
  }
}

Bit8u bx_svga_cirrus_c::svga_read_control(Bit32u address, unsigned index)
{
  switch (index) {
    case 0x00:  // VGA
      return BX_CIRRUS_THIS control.shadow_reg0;
    case 0x01:  // VGA
      return BX_CIRRUS_THIS control.shadow_reg1;
    case 0x05:  // VGA
      return BX_CIRRUS_THIS control.reg[index];
    case 0x02:  // VGA
    case 0x03:  // VGA
    case 0x04:  // VGA
    case 0x06:  // VGA
    case 0x07:  // VGA
    case 0x08:  // VGA
      break;
    case 0x09: // bank offset #0
    case 0x0A: // bank offset #1
    case 0x0B:
      break;

    case 0x10:  // BGCOLOR 0x0000ff00
    case 0x11:  // FGCOLOR 0x0000ff00
    case 0x12:  // BGCOLOR 0x00ff0000
    case 0x13:  // FGCOLOR 0x00ff0000
    case 0x14:  // BGCOLOR 0xff000000
    case 0x15:  // FGCOLOR 0xff000000
      break;

    case 0x20: // BLT WIDTH 0x0000ff
    case 0x21: // BLT WIDTH 0x001f00
    case 0x22: // BLT HEIGHT 0x0000ff
    case 0x23: // BLT HEIGHT 0x001f00
    case 0x24: // BLT DEST PITCH 0x0000ff
    case 0x25: // BLT DEST PITCH 0x001f00
    case 0x26: // BLT SRC PITCH 0x0000ff
    case 0x27: // BLT SRC PITCH 0x001f00
    case 0x28: // BLT DEST ADDR 0x0000ff
    case 0x29: // BLT DEST ADDR 0x00ff00
    case 0x2a: // BLT DEST ADDR 0x3f0000
    case 0x2c: // BLT SRC ADDR 0x0000ff
    case 0x2d: // BLT SRC ADDR 0x00ff00
    case 0x2e: // BLT SRC ADDR 0x3f0000
    case 0x2f: // BLT WRITE MASK
    case 0x30: // BLT MODE
    case 0x31: // BLT STATUS
    case 0x32: // RASTER OP
    case 0x33: // BLT MODE EXTENSION
    case 0x34: // BLT TRANSPARENT COLOR 0x00ff
    case 0x35: // BLT TRANSPARENT COLOR 0xff00
    case 0x38: // BLT TRANSPARENT COLOR MASK 0x00ff
    case 0x39: // BLT TRANSPARENT COLOR MASK 0xff00
      break;

    default:
      BX_DEBUG(("control index 0x%02x is unknown(read)", index));
      break;
  }

  if (index <= VGA_CONTROL_MAX) {
    return VGA_READ(address,1);
  }

  if (index <= CIRRUS_CONTROL_MAX) {
    return BX_CIRRUS_THIS control.reg[index];
  }

  return 0xff;
}

void bx_svga_cirrus_c::svga_write_control(Bit32u address, unsigned index, Bit8u value)
{
  Bit8u old_value = BX_CIRRUS_THIS control.reg[index];

  BX_DEBUG(("control: index 0x%02x write 0x%02x", index, (unsigned)value));

  switch (index) {
    case 0x00:  // VGA
      BX_CIRRUS_THIS control.shadow_reg0 = value;
      break;
    case 0x01:  // VGA
      BX_CIRRUS_THIS control.shadow_reg1 = value;
      break;
    case 0x02:  // VGA
    case 0x03:  // VGA
    case 0x04:  // VGA
    case 0x07:  // VGA
    case 0x08:  // VGA
      break;
    case 0x05:  // VGA
    case 0x06:  // VGA
      BX_CIRRUS_THIS svga_needs_update_mode = 1;
      break;
    case 0x09: // bank offset #0
    case 0x0A: // bank offset #1
    case 0x0B:
      BX_CIRRUS_THIS control.reg[index] = value;
      update_bank_ptr(0);
      update_bank_ptr(1);
      break;

    case 0x10:  // BGCOLOR 0x0000ff00
    case 0x11:  // FGCOLOR 0x0000ff00
    case 0x12:  // BGCOLOR 0x00ff0000
    case 0x13:  // FGCOLOR 0x00ff0000
    case 0x14:  // BGCOLOR 0xff000000
    case 0x15:  // FGCOLOR 0xff000000
      break;

    case 0x20: // BLT WIDTH 0x0000ff
      break;
    case 0x21: // BLT WIDTH 0x001f00
      value &= 0x1f;
      break;
    case 0x22: // BLT HEIGHT 0x0000ff
      break;
    case 0x23: // BLT HEIGHT 0x001f00
      value &= 0x1f;
      break;
    case 0x24: // BLT DEST PITCH 0x0000ff
      break;
    case 0x25: // BLT DEST PITCH 0x001f00
      value &= 0x1f;
      break;
    case 0x26: // BLT SRC PITCH 0x0000ff
      break;
    case 0x27: // BLT SRC PITCH 0x001f00
      value &= 0x1f;
      break;
    case 0x28: // BLT DEST ADDR 0x0000ff
      break;
    case 0x29: // BLT DEST ADDR 0x00ff00
      break;
    case 0x2a: // BLT DEST ADDR 0x3f0000
      BX_CIRRUS_THIS control.reg[index] = value & 0x3f;
      if (BX_CIRRUS_THIS control.reg[0x31] & CIRRUS_BLT_AUTOSTART) {
        svga_bitblt();
      }
      return;
    case 0x2b: // BLT DEST ADDR (unused bits)
      break;
    case 0x2c: // BLT SRC ADDR 0x0000ff
      break;
    case 0x2d: // BLT SRC ADDR 0x00ff00
      break;
    case 0x2e: // BLT SRC ADDR 0x3f0000
      value &= 0x3f;
      break;
    case 0x2f: // BLT WRITE MASK
      break;
    case 0x30: // BLT MODE
      break;
    case 0x31: // BLT STATUS/START
      BX_CIRRUS_THIS control.reg[0x31] = value;
      if (((old_value & CIRRUS_BLT_RESET) != 0) &&
          ((value & CIRRUS_BLT_RESET) == 0)) {
        svga_reset_bitblt();
      }
      else if (((old_value & CIRRUS_BLT_START) == 0) &&
          ((value & CIRRUS_BLT_START) != 0)) {
        BX_CIRRUS_THIS control.reg[0x31] |= CIRRUS_BLT_BUSY;
        svga_bitblt();
      }
      return;
    case 0x32: // RASTER OP
      break;
    case 0x33: // BLT MODE EXTENSION
#if BX_SUPPORT_PCI
      if (BX_CIRRUS_THIS pci_enabled) {
        if (((value ^ old_value) & 0x18) && (value & 0x18)) {
          BX_ERROR(("BLT MODE EXTENSION support is not complete (value = 0x%02x)", value & 0x18));
        }
      }
      else
#endif
      {
        BX_DEBUG(("BLT MODE EXTENSION not available"));
        return;
      }
      break;
    case 0x34: // BLT TRANSPARENT COLOR 0x00ff
    case 0x35: // BLT TRANSPARENT COLOR 0xff00
      break;
    case 0x38: // BLT TRANSPARENT COLOR MASK 0x00ff
    case 0x39: // BLT TRANSPARENT COLOR MASK 0xff00
    default:
      BX_DEBUG(("control index 0x%02x is unknown (write 0x%02x)", index, (unsigned)value));
      break;
    }

  if (index <= CIRRUS_CONTROL_MAX) {
    BX_CIRRUS_THIS control.reg[index] = value;
  }
  if (index <= VGA_CONTROL_MAX) {
    VGA_WRITE(address,value,1);
  }
}

Bit8u bx_svga_cirrus_c::svga_mmio_vga_read(Bit32u address)
{
  Bit8u value = 0xff;

  BX_DEBUG(("MMIO vga read - address 0x%04x, value 0x%02x",address,value));

#if BX_USE_CIRRUS_SMF
  value = (Bit8u)svga_read_handler(theSvga,0x3c0+address,1);
#else // BX_USE_CIRRUS_SMF
  value = (Bit8u)svga_read(0x3c0+address,1);
#endif // BX_USE_CIRRUS_SMF

  return value;
}

void bx_svga_cirrus_c::svga_mmio_vga_write(Bit32u address,Bit8u value)
{

  BX_DEBUG(("MMIO vga write - address 0x%04x, value 0x%02x",address,value));

#if BX_USE_CIRRUS_SMF
  svga_write_handler(theSvga,0x3c0+address,value,1);
#else // BX_USE_CIRRUS_SMF
  svga_write(0x3c0+address,value,1);
#endif // BX_USE_CIRRUS_SMF

}

Bit8u bx_svga_cirrus_c::svga_mmio_blt_read(Bit32u address)
{
  Bit8u value = 0xff;

  switch (address) {
    case (CLGD543x_MMIO_BLTBGCOLOR+0):
      value = BX_CIRRUS_THIS control.shadow_reg0;
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+1):
      value = svga_read_control(0x3cf,0x10);
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+2):
      value = svga_read_control(0x3cf,0x12);
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+3):
      value = svga_read_control(0x3cf,0x14);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+0):
      value = BX_CIRRUS_THIS control.shadow_reg1;
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+1):
      value = svga_read_control(0x3cf,0x11);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+2):
      value = svga_read_control(0x3cf,0x13);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+3):
      value = svga_read_control(0x3cf,0x15);
      break;
    case (CLGD543x_MMIO_BLTWIDTH+0):
      value = svga_read_control(0x3cf,0x20);
      break;
    case (CLGD543x_MMIO_BLTWIDTH+1):
      value = svga_read_control(0x3cf,0x21);
      break;
    case (CLGD543x_MMIO_BLTHEIGHT+0):
      value = svga_read_control(0x3cf,0x22);
      break;
    case (CLGD543x_MMIO_BLTHEIGHT+1):
      value = svga_read_control(0x3cf,0x23);
      break;
    case (CLGD543x_MMIO_BLTDESTPITCH+0):
      value = svga_read_control(0x3cf,0x24);
      break;
    case (CLGD543x_MMIO_BLTDESTPITCH+1):
      value = svga_read_control(0x3cf,0x25);
      break;
    case (CLGD543x_MMIO_BLTSRCPITCH+0):
      value = svga_read_control(0x3cf,0x26);
      break;
    case (CLGD543x_MMIO_BLTSRCPITCH+1):
      value = svga_read_control(0x3cf,0x27);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+0):
      value = svga_read_control(0x3cf,0x28);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+1):
      value = svga_read_control(0x3cf,0x29);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+2):
      value = svga_read_control(0x3cf,0x2a);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+3):
      value = svga_read_control(0x3cf,0x2b);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+0):
      value = svga_read_control(0x3cf,0x2c);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+1):
      value = svga_read_control(0x3cf,0x2d);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+2):
      value = svga_read_control(0x3cf,0x2e);
      break;
    case CLGD543x_MMIO_BLTWRITEMASK:
      value = svga_read_control(0x3cf,0x2f);
      break;
    case CLGD543x_MMIO_BLTMODE:
      value = svga_read_control(0x3cf,0x30);
      break;
    case CLGD543x_MMIO_BLTROP:
      value = svga_read_control(0x3cf,0x32);
      break;
    case CLGD543x_MMIO_BLTMODEEXT:
      value = svga_read_control(0x3cf,0x33);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+0):
      value = svga_read_control(0x3cf,0x34);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+1):
      value = svga_read_control(0x3cf,0x35);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+2):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLOR"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+3):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLOR"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+0):
      value = svga_read_control(0x3cf,0x38);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+1):
      value = svga_read_control(0x3cf,0x39);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+2):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+3):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK"));
      break;
    case CLGD543x_MMIO_BLTSTATUS:
      value = svga_read_control(0x3cf,0x31);
      break;
    default:
      BX_ERROR(("MMIO blt read - address 0x%04x",address));
      break;
    }

  BX_DEBUG(("MMIO blt read - address 0x%04x, value 0x%02x",address,value));

  return value;
}

void bx_svga_cirrus_c::svga_mmio_blt_write(Bit32u address,Bit8u value)
{
  BX_DEBUG(("MMIO blt write - address 0x%04x, value 0x%02x",address,value));

  switch (address) {
    case (CLGD543x_MMIO_BLTBGCOLOR+0):
      BX_CIRRUS_THIS control.shadow_reg0 = value;
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+1):
      svga_write_control(0x3cf,0x10,value);
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+2):
      svga_write_control(0x3cf,0x12,value);
      break;
    case (CLGD543x_MMIO_BLTBGCOLOR+3):
      svga_write_control(0x3cf,0x14,value);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+0):
      BX_CIRRUS_THIS control.shadow_reg1 = value;
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+1):
      svga_write_control(0x3cf,0x11,value);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+2):
      svga_write_control(0x3cf,0x13,value);
      break;
    case (CLGD543x_MMIO_BLTFGCOLOR+3):
      svga_write_control(0x3cf,0x15,value);
      break;
    case (CLGD543x_MMIO_BLTWIDTH+0):
      svga_write_control(0x3cf,0x20,value);
      break;
    case (CLGD543x_MMIO_BLTWIDTH+1):
      svga_write_control(0x3cf,0x21,value);
      break;
    case (CLGD543x_MMIO_BLTHEIGHT+0):
      svga_write_control(0x3cf,0x22,value);
      break;
    case (CLGD543x_MMIO_BLTHEIGHT+1):
      svga_write_control(0x3cf,0x23,value);
      break;
    case (CLGD543x_MMIO_BLTDESTPITCH+0):
      svga_write_control(0x3cf,0x24,value);
      break;
    case (CLGD543x_MMIO_BLTDESTPITCH+1):
      svga_write_control(0x3cf,0x25,value);
      break;
    case (CLGD543x_MMIO_BLTSRCPITCH+0):
      svga_write_control(0x3cf,0x26,value);
      break;
    case (CLGD543x_MMIO_BLTSRCPITCH+1):
      svga_write_control(0x3cf,0x27,value);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+0):
      svga_write_control(0x3cf,0x28,value);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+1):
      svga_write_control(0x3cf,0x29,value);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+2):
      svga_write_control(0x3cf,0x2a,value);
      break;
    case (CLGD543x_MMIO_BLTDESTADDR+3):
      svga_write_control(0x3cf,0x2b,value);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+0):
      svga_write_control(0x3cf,0x2c,value);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+1):
      svga_write_control(0x3cf,0x2d,value);
      break;
    case (CLGD543x_MMIO_BLTSRCADDR+2):
      svga_write_control(0x3cf,0x2e,value);
      break;
    case CLGD543x_MMIO_BLTWRITEMASK:
      svga_write_control(0x3cf,0x2f,value);
      break;
    case CLGD543x_MMIO_BLTMODE:
      svga_write_control(0x3cf,0x30,value);
      break;
    case CLGD543x_MMIO_BLTMODE+1:
      // unused ??? - ignored for now
      break;
    case CLGD543x_MMIO_BLTROP:
      svga_write_control(0x3cf,0x32,value);
      break;
    case CLGD543x_MMIO_BLTMODEEXT:
      svga_write_control(0x3cf,0x33,value);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+0):
      svga_write_control(0x3cf,0x34,value);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+1):
      svga_write_control(0x3cf,0x35,value);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+2):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLOR"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLOR+3):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLOR"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+0):
      svga_write_control(0x3cf,0x38,value);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+1):
      svga_write_control(0x3cf,0x39,value);
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+2):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK"));
      break;
    case (CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK+3):
      BX_ERROR(("CLGD543x_MMIO_BLTTRANSPARENTCOLORMASK"));
      break;
    case CLGD543x_MMIO_BLTSTATUS:
      svga_write_control(0x3cf,0x31,value);
      break;
    default:
      BX_ERROR(("MMIO blt write - address 0x%04x, value 0x%02x",address,value));
      break;
    }
}


/////////////////////////////////////////////////////////////////////////
//
// PCI support
//
/////////////////////////////////////////////////////////////////////////

#if BX_SUPPORT_PCI

void bx_svga_cirrus_c::svga_init_pcihandlers(void)
{
  Bit8u devfunc = 0x00;
  DEV_register_pci_handlers(BX_CIRRUS_THIS_PTR,
     &devfunc, "cirrus", "SVGA Cirrus PCI");

  // initialize readonly registers
  BX_CIRRUS_THIS init_pci_conf(PCI_VENDOR_CIRRUS, PCI_DEVICE_CLGD5446, 0x00,
    (PCI_CLASS_BASE_DISPLAY << 16) | (PCI_CLASS_SUB_VGA << 8),
    PCI_CLASS_HEADERTYPE_00h, 0);
  BX_CIRRUS_THIS pci_conf[0x04] = (PCI_COMMAND_IOACCESS | PCI_COMMAND_MEMACCESS);

  BX_CIRRUS_THIS pci_conf[0x10] =
    (PCI_MAP_MEM | PCI_MAP_MEMFLAGS_32BIT | PCI_MAP_MEMFLAGS_CACHEABLE);
  BX_CIRRUS_THIS pci_conf[0x14] =
    (PCI_MAP_MEM | PCI_MAP_MEMFLAGS_32BIT);

  BX_CIRRUS_THIS init_bar_mem(0, 0x2000000, cirrus_mem_read_handler,
                              cirrus_mem_write_handler);
  BX_CIRRUS_THIS init_bar_mem(1, CIRRUS_PNPMMIO_SIZE, cirrus_mem_read_handler,
                              cirrus_mem_write_handler);
  BX_CIRRUS_THIS pci_rom_address = 0;
  BX_CIRRUS_THIS pci_rom_read_handler = cirrus_mem_read_handler;
  BX_CIRRUS_THIS load_pci_rom(SIM->get_param_string(BXPN_VGA_ROM_PATH)->getptr());
}

void bx_svga_cirrus_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  unsigned i;
  unsigned write_addr;
  Bit8u new_value, old_value;

  if ((address > 0x17) && (address < 0x30))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (i = 0; i < io_len; i++) {
    write_addr = address + i;
    old_value = BX_CIRRUS_THIS pci_conf[write_addr];
    new_value = (Bit8u)(value & 0xff);
    switch (write_addr) {
      case 0x04: // command bit0-7
        new_value &= PCI_COMMAND_IOACCESS | PCI_COMMAND_MEMACCESS;
        new_value |= old_value & ~(PCI_COMMAND_IOACCESS | PCI_COMMAND_MEMACCESS);
        break;
      case 0x05: // command bit8-15
        new_value = old_value;
        break;
      case 0x06: // status bit0-7
        new_value = old_value & (~new_value);
        break;
      case 0x07: // status bit8-15
        new_value = old_value & (~new_value);
        break;

      // read-only.
      case 0x00: case 0x01: // vendor
      case 0x02: case 0x03: // device
      case 0x08: // revision
      case 0x09: case 0x0a: case 0x0b: // class
      case 0x0e: // header type
      case 0x0f: // built-in self test(unimplemented)
        new_value = old_value;
        break;
      default:
        break;
    }
    BX_CIRRUS_THIS pci_conf[write_addr] = new_value;
    value >>= 8;
  }
}

#endif // BX_SUPPORT_PCI

/////////////////////////////////////////////////////////////////////////
//
// Bitblt.
//
/////////////////////////////////////////////////////////////////////////

void bx_svga_cirrus_c::svga_reset_bitblt(void)
{
  BX_CIRRUS_THIS control.reg[0x31] &= ~(CIRRUS_BLT_START|CIRRUS_BLT_BUSY|CIRRUS_BLT_FIFOUSED);
  BX_CIRRUS_THIS bitblt.rop_handler = NULL;
  BX_CIRRUS_THIS bitblt.src = NULL;
  BX_CIRRUS_THIS bitblt.dst = NULL;
  BX_CIRRUS_THIS bitblt.memsrc_ptr = NULL;
  BX_CIRRUS_THIS bitblt.memsrc_endptr = NULL;
  BX_CIRRUS_THIS bitblt.memsrc_needed = 0;
  BX_CIRRUS_THIS bitblt.memdst_ptr = NULL;
  BX_CIRRUS_THIS bitblt.memdst_endptr = NULL;
  BX_CIRRUS_THIS bitblt.memdst_needed = 0;
}

void bx_svga_cirrus_c::svga_bitblt()
{
  Bit16u tmp16;
  Bit32u tmp32;
  Bit32u dstaddr;
  Bit32u srcaddr;
  Bit32u offset;
  Bit32u offset2;
  Bit8u *cregs = BX_CIRRUS_THIS control.reg;

  tmp16 = ReadHostWordFromLittleEndian((Bit16u*) &cregs[0x20]);
  BX_CIRRUS_THIS bitblt.bltwidth = ((int)(tmp16 & 0x1fff)) + 1;
  tmp16 = ReadHostWordFromLittleEndian((Bit16u*) &cregs[0x22]);
  BX_CIRRUS_THIS bitblt.bltheight = ((int)(tmp16 & 0x07ff)) + 1;
  tmp16 = ReadHostWordFromLittleEndian((Bit16u*) &cregs[0x24]);
  BX_CIRRUS_THIS bitblt.dstpitch = (int)(tmp16 & 0x1fff);
  tmp16 = ReadHostWordFromLittleEndian((Bit16u*) &cregs[0x26]);
  BX_CIRRUS_THIS bitblt.srcpitch = (int)(tmp16 & 0x1fff);
  tmp32 = ReadHostDWordFromLittleEndian((Bit32u*) &cregs[0x28]);
  dstaddr = tmp32 & BX_CIRRUS_THIS memsize_mask;
  tmp32 = ReadHostDWordFromLittleEndian((Bit32u*) &cregs[0x2c]);
  srcaddr = tmp32 & BX_CIRRUS_THIS memsize_mask;
  BX_CIRRUS_THIS bitblt.srcaddr = srcaddr;
  BX_CIRRUS_THIS bitblt.dstaddr = dstaddr;
  BX_CIRRUS_THIS bitblt.bltmode = BX_CIRRUS_THIS control.reg[0x30];
  BX_CIRRUS_THIS bitblt.bltmodeext = BX_CIRRUS_THIS control.reg[0x33];
  BX_CIRRUS_THIS bitblt.bltrop = BX_CIRRUS_THIS control.reg[0x32];
  offset = dstaddr - (Bit32u)(BX_CIRRUS_THIS disp_ptr - BX_CIRRUS_THIS s.memory);
  BX_CIRRUS_THIS redraw.x = (offset % BX_CIRRUS_THIS bitblt.dstpitch) / (BX_CIRRUS_THIS svga_bpp >> 3);
  BX_CIRRUS_THIS redraw.y = offset / BX_CIRRUS_THIS bitblt.dstpitch;
  BX_CIRRUS_THIS redraw.w = BX_CIRRUS_THIS bitblt.bltwidth / (BX_CIRRUS_THIS svga_bpp >> 3);
  BX_CIRRUS_THIS redraw.h = BX_CIRRUS_THIS bitblt.bltheight;

  BX_DEBUG(("BLT: src:0x%08x,dst 0x%08x,block %ux%u,mode 0x%02x,ROP 0x%02x",
    (unsigned)srcaddr,(unsigned)dstaddr,
    (unsigned)BX_CIRRUS_THIS bitblt.bltwidth,(unsigned)BX_CIRRUS_THIS bitblt.bltheight,
    (unsigned)BX_CIRRUS_THIS bitblt.bltmode,(unsigned)BX_CIRRUS_THIS bitblt.bltrop));
  BX_DEBUG(("BLT: srcpitch:0x%08x,dstpitch 0x%08x,modeext 0x%02x,writemask 0x%02x",
    (unsigned)BX_CIRRUS_THIS bitblt.srcpitch,
    (unsigned)BX_CIRRUS_THIS bitblt.dstpitch,
    (unsigned)BX_CIRRUS_THIS bitblt.bltmodeext,
    BX_CIRRUS_THIS control.reg[0x2f]));

  switch (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_PIXELWIDTHMASK) {
    case CIRRUS_BLTMODE_PIXELWIDTH8:
      BX_CIRRUS_THIS bitblt.pixelwidth = 1;
      break;
    case CIRRUS_BLTMODE_PIXELWIDTH16:
      BX_CIRRUS_THIS bitblt.pixelwidth = 2;
      break;
    case CIRRUS_BLTMODE_PIXELWIDTH24:
      BX_CIRRUS_THIS bitblt.pixelwidth = 3;
      break;
    case CIRRUS_BLTMODE_PIXELWIDTH32:
      BX_CIRRUS_THIS bitblt.pixelwidth = 4;
      break;
    default:
      BX_PANIC(("unknown pixel width"));
      goto ignoreblt;
  }

  BX_CIRRUS_THIS bitblt.bltmode &= ~CIRRUS_BLTMODE_PIXELWIDTHMASK;

  if ((BX_CIRRUS_THIS bitblt.bltmode & (CIRRUS_BLTMODE_MEMSYSSRC|CIRRUS_BLTMODE_MEMSYSDEST))
                                   == (CIRRUS_BLTMODE_MEMSYSSRC|CIRRUS_BLTMODE_MEMSYSDEST)) {
    BX_ERROR(("BLT: memory-to-memory copy is requested, ROP %02x",
      (unsigned)BX_CIRRUS_THIS bitblt.bltrop));
    goto ignoreblt;
  }

  if ((BX_CIRRUS_THIS bitblt.bltmodeext & CIRRUS_BLTMODEEXT_SOLIDFILL) &&
      (BX_CIRRUS_THIS bitblt.bltmode & (CIRRUS_BLTMODE_MEMSYSDEST |
                             CIRRUS_BLTMODE_TRANSPARENTCOMP |
                             CIRRUS_BLTMODE_PATTERNCOPY |
                             CIRRUS_BLTMODE_COLOREXPAND)) ==
      (CIRRUS_BLTMODE_PATTERNCOPY | CIRRUS_BLTMODE_COLOREXPAND)) {
    BX_CIRRUS_THIS bitblt.rop_handler = svga_get_fwd_rop_handler(BX_CIRRUS_THIS bitblt.bltrop);
    BX_CIRRUS_THIS bitblt.dst = BX_CIRRUS_THIS s.memory + dstaddr;
    svga_solidfill();
  } else {
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_BACKWARDS) {
      BX_CIRRUS_THIS bitblt.dstpitch = -BX_CIRRUS_THIS bitblt.dstpitch;
      BX_CIRRUS_THIS bitblt.srcpitch = -BX_CIRRUS_THIS bitblt.srcpitch;
      BX_CIRRUS_THIS bitblt.rop_handler = svga_get_bkwd_rop_handler(BX_CIRRUS_THIS bitblt.bltrop);
      if (BX_CIRRUS_THIS redraw.w > BX_CIRRUS_THIS redraw.x) {
        BX_CIRRUS_THIS redraw.w = BX_CIRRUS_THIS redraw.x;
        BX_CIRRUS_THIS redraw.x = 0;
      } else {
        BX_CIRRUS_THIS redraw.x -= BX_CIRRUS_THIS redraw.w;
      }
      if (BX_CIRRUS_THIS redraw.h > BX_CIRRUS_THIS redraw.y) {
        BX_CIRRUS_THIS redraw.h = BX_CIRRUS_THIS redraw.y;
        BX_CIRRUS_THIS redraw.y = 0;
      } else {
        BX_CIRRUS_THIS redraw.y -= BX_CIRRUS_THIS redraw.h;
      }
    } else {
      BX_CIRRUS_THIS bitblt.rop_handler = svga_get_fwd_rop_handler(BX_CIRRUS_THIS bitblt.bltrop);
      offset2 = offset + (BX_CIRRUS_THIS redraw.h * BX_CIRRUS_THIS bitblt.dstpitch);
      offset2 &= BX_CIRRUS_THIS memsize_mask;
      if (offset2 < offset) {
        BX_DEBUG(("Address wrap detected"));
        BX_CIRRUS_THIS redraw.x = 0;
        BX_CIRRUS_THIS redraw.y = 0;
      }
    }

    BX_DEBUG(("BLT redraw: x = %d, y = %d, w = %d, h = %d", BX_CIRRUS_THIS redraw.x,
      BX_CIRRUS_THIS redraw.y, BX_CIRRUS_THIS redraw.w, BX_CIRRUS_THIS redraw.h));

    // setup bitblt engine.
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_MEMSYSSRC) {
      svga_setup_bitblt_cputovideo(dstaddr,srcaddr);
    }
    else if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_MEMSYSDEST) {
      svga_setup_bitblt_videotocpu(dstaddr,srcaddr);
    }
    else {
      svga_setup_bitblt_videotovideo(dstaddr,srcaddr);
    }
    return;
  }

ignoreblt:
  svga_reset_bitblt();
}

void bx_svga_cirrus_c::svga_setup_bitblt_cputovideo(Bit32u dstaddr,Bit32u srcaddr)
{
  Bit16u w;

  BX_CIRRUS_THIS bitblt.bltmode &= ~CIRRUS_BLTMODE_MEMSYSSRC;

  BX_CIRRUS_THIS bitblt.dst = BX_CIRRUS_THIS s.memory + dstaddr;
  BX_CIRRUS_THIS bitblt.src = NULL;

  BX_CIRRUS_THIS bitblt.memsrc_ptr = &BX_CIRRUS_THIS bitblt.memsrc[0];
  BX_CIRRUS_THIS bitblt.memsrc_endptr = &BX_CIRRUS_THIS bitblt.memsrc[0];

  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_PATTERNCOPY) {
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_COLOREXPAND) {
      BX_CIRRUS_THIS bitblt.srcpitch = 8;
    } else {
      BX_CIRRUS_THIS bitblt.srcpitch = 8 * 8 * BX_CIRRUS_THIS bitblt.pixelwidth;
    }
    BX_CIRRUS_THIS bitblt.memsrc_needed = BX_CIRRUS_THIS bitblt.srcpitch;
    BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_patterncopy_memsrc_static;
  } else {
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_COLOREXPAND) {
      w = BX_CIRRUS_THIS bitblt.bltwidth / BX_CIRRUS_THIS bitblt.pixelwidth;
      if (BX_CIRRUS_THIS bitblt.bltmodeext & CIRRUS_BLTMODEEXT_DWORDGRANULARITY) {
        BX_CIRRUS_THIS bitblt.srcpitch = ((w + 31) & ~0x1f) >> 3;
      } else {
        BX_CIRRUS_THIS bitblt.srcpitch = (w + 7) >> 3;
      }
      if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_TRANSPARENTCOMP) {
        BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_colorexpand_transp_memsrc_static;
      } else {
        BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_simplebitblt_memsrc_static;
      }
    } else {
      BX_CIRRUS_THIS bitblt.srcpitch = (BX_CIRRUS_THIS bitblt.bltwidth + 3) & (~3);
      BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_simplebitblt_memsrc_static;
    }
    BX_CIRRUS_THIS bitblt.memsrc_needed =
        BX_CIRRUS_THIS bitblt.srcpitch * BX_CIRRUS_THIS bitblt.bltheight;
  }
  BX_CIRRUS_THIS bitblt.memsrc_endptr += BX_CIRRUS_THIS bitblt.srcpitch;
}

void bx_svga_cirrus_c::svga_setup_bitblt_videotocpu(Bit32u dstaddr,Bit32u srcaddr)
{
  BX_ERROR(("BLT: MEMSYSDEST is not implemented"));

  BX_CIRRUS_THIS bitblt.bltmode &= ~CIRRUS_BLTMODE_MEMSYSDEST;

#if 0
  BX_CIRRUS_THIS bitblt.dst = NULL;
  BX_CIRRUS_THIS bitblt.src = BX_CIRRUS_THIS s.memory + srcaddr;

  BX_CIRRUS_THIS bitblt.memdst_ptr = &BX_CIRRUS_THIS bitblt.memdst[0];
  BX_CIRRUS_THIS bitblt.memdst_endptr = &BX_CIRRUS_THIS bitblt.memdst[0];

  BX_CIRRUS_THIS bitblt.memdst_needed =
    BX_CIRRUS_THIS bitblt.bltwidth * BX_CIRRUS_THIS bitblt.bltheight;
  BX_CIRRUS_THIS bitblt.memdst_needed = (BX_CIRRUS_THIS bitblt.memdst_needed + 3) & (~3);

  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_PATTERNCOPY) {
    BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_patterncopy_memdst_static;
  }
  else {
    BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_simplebitblt_memdst_static;
  }
#endif
}

void bx_svga_cirrus_c::svga_setup_bitblt_videotovideo(Bit32u dstaddr,Bit32u srcaddr)
{
  BX_CIRRUS_THIS bitblt.dst = BX_CIRRUS_THIS s.memory + dstaddr;

  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_PATTERNCOPY) {
    BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_patterncopy_static;
    BX_CIRRUS_THIS bitblt.src = BX_CIRRUS_THIS s.memory + (srcaddr & ~0x07);
  } else {
    BX_CIRRUS_THIS bitblt.bitblt_ptr = svga_simplebitblt_static;
    BX_CIRRUS_THIS bitblt.src = BX_CIRRUS_THIS s.memory + srcaddr;
  }

  (*BX_CIRRUS_THIS bitblt.bitblt_ptr)();
  svga_reset_bitblt();
  BX_CIRRUS_THIS redraw_area(BX_CIRRUS_THIS redraw.x, BX_CIRRUS_THIS redraw.y,
                             BX_CIRRUS_THIS redraw.w, BX_CIRRUS_THIS redraw.h);
}

void bx_svga_cirrus_c::svga_colorexpand(Bit8u *dst,const Bit8u *src,int count,int pixelwidth)
{
  BX_DEBUG(("svga_cirrus: COLOR EXPAND"));

  switch (pixelwidth) {
    case 1:
      svga_colorexpand_8(dst,src,count);
      break;
    case 2:
      svga_colorexpand_16(dst,src,count);
      break;
    case 3:
      svga_colorexpand_24(dst,src,count);
      break;
    case 4:
      svga_colorexpand_32(dst,src,count);
      break;
    default:
      BX_PANIC(("COLOREXPAND: unknown pixelwidth %u",(unsigned)pixelwidth));
      break;
  }
}

#if !BX_USE_CIRRUS_SMF
void bx_svga_cirrus_c::svga_colorexpand_8_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_colorexpand_8(dst,src,count);
}

void bx_svga_cirrus_c::svga_colorexpand_16_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_colorexpand_16(dst,src,count);
}

void bx_svga_cirrus_c::svga_colorexpand_24_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_colorexpand_24(dst,src,count);
}

void bx_svga_cirrus_c::svga_colorexpand_32_static(void *this_ptr,Bit8u *dst,const Bit8u *src,int count)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_colorexpand_32(dst,src,count);
}

#endif // BX_USE_CIRRUS_SMF

void bx_svga_cirrus_c::svga_colorexpand_8(Bit8u *dst,const Bit8u *src,int count)
{
  Bit8u colors[2];
  unsigned bits;
  unsigned bitmask;

  colors[0] = BX_CIRRUS_THIS control.shadow_reg0;
  colors[1] = BX_CIRRUS_THIS control.shadow_reg1;

  bitmask = 0x80;
  bits = *src++;
  for (int x = 0; x < count; x++) {
    if ((bitmask & 0xff) == 0) {
      bitmask = 0x80;
      bits = *src++;
    }
    *dst++ = colors[!!(bits & bitmask)];
    bitmask >>= 1;
  }
}

void bx_svga_cirrus_c::svga_colorexpand_16(Bit8u *dst,const Bit8u *src,int count)
{
  Bit8u colors[2][2];
  unsigned bits;
  unsigned bitmask;
  unsigned index;

  colors[0][0] = BX_CIRRUS_THIS control.shadow_reg0;
  colors[0][1] = BX_CIRRUS_THIS control.reg[0x10];
  colors[1][0] = BX_CIRRUS_THIS control.shadow_reg1;
  colors[1][1] = BX_CIRRUS_THIS control.reg[0x11];

  bitmask = 0x80;
  bits = *src++;
  for (int x = 0; x < count; x++) {
    if ((bitmask & 0xff) == 0) {
      bitmask = 0x80;
      bits = *src++;
    }
    index = !!(bits & bitmask);
    *dst++ = colors[index][0];
    *dst++ = colors[index][1];
    bitmask >>= 1;
  }
}

void bx_svga_cirrus_c::svga_colorexpand_24(Bit8u *dst,const Bit8u *src,int count)
{
  Bit8u colors[2][3];
  unsigned bits;
  unsigned bitmask;
  unsigned index;

  colors[0][0] = BX_CIRRUS_THIS control.shadow_reg0;
  colors[0][1] = BX_CIRRUS_THIS control.reg[0x10];
  colors[0][2] = BX_CIRRUS_THIS control.reg[0x12];
  colors[1][0] = BX_CIRRUS_THIS control.shadow_reg1;
  colors[1][1] = BX_CIRRUS_THIS control.reg[0x11];
  colors[1][2] = BX_CIRRUS_THIS control.reg[0x13];

  bitmask = 0x80;
  bits = *src++;
  for (int x = 0; x < count; x++) {
    if ((bitmask & 0xff) == 0) {
      bitmask = 0x80;
      bits = *src++;
    }
    index = !!(bits & bitmask);
    *dst++ = colors[index][0];
    *dst++ = colors[index][1];
    *dst++ = colors[index][2];
    bitmask >>= 1;
  }
}

void bx_svga_cirrus_c::svga_colorexpand_32(Bit8u *dst,const Bit8u *src,int count)
{
  Bit8u colors[2][4];
  unsigned bits;
  unsigned bitmask;
  unsigned index;

  colors[0][0] = BX_CIRRUS_THIS control.shadow_reg0;
  colors[0][1] = BX_CIRRUS_THIS control.reg[0x10];
  colors[0][2] = BX_CIRRUS_THIS control.reg[0x12];
  colors[0][3] = BX_CIRRUS_THIS control.reg[0x14];
  colors[1][0] = BX_CIRRUS_THIS control.shadow_reg1;
  colors[1][1] = BX_CIRRUS_THIS control.reg[0x11];
  colors[1][2] = BX_CIRRUS_THIS control.reg[0x13];
  colors[1][3] = BX_CIRRUS_THIS control.reg[0x15];

  bitmask = 0x80;
  bits = *src++;
  for (int x = 0; x < count; x++) {
    if ((bitmask & 0xff) == 0) {
      bitmask = 0x80;
      bits = *src++;
    }
    index = !!(bits & bitmask);
    *dst++ = colors[index][0];
    *dst++ = colors[index][1];
    *dst++ = colors[index][2];
    *dst++ = colors[index][3];
    bitmask >>= 1;
  }
}

#if !BX_USE_CIRRUS_SMF
void bx_svga_cirrus_c::svga_patterncopy_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_patterncopy();
}

void bx_svga_cirrus_c::svga_simplebitblt_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_simplebitblt();
}

void bx_svga_cirrus_c::svga_solidfill_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_solidfill();
}

void bx_svga_cirrus_c::svga_patterncopy_memsrc_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_patterncopy_memsrc();
}

void bx_svga_cirrus_c::svga_simplebitblt_memsrc_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_simplebitblt_memsrc();
}

void bx_svga_cirrus_c::svga_colorexpand_transp_memsrc_static(void *this_ptr)
{
  ((bx_svga_cirrus_c *)this_ptr)->svga_colorexpand_transp_memsrc();
}

#endif // !BX_USE_CIRRUS_SMF

void bx_svga_cirrus_c::svga_patterncopy()
{
  Bit8u color[4];
  Bit8u work_colorexp[256];
  Bit8u *src, *dst;
  Bit8u *srcc, *src2;
  Bit32u dstaddr;
  int x, y, pattern_x, pattern_y, srcskipleft;
  int patternbytes = 8 * BX_CIRRUS_THIS bitblt.pixelwidth;
  int pattern_pitch = patternbytes;
  int bltbytes = BX_CIRRUS_THIS bitblt.bltwidth;
  unsigned bits, bits_xor, bitmask;

  if (BX_CIRRUS_THIS bitblt.pixelwidth == 3) {
    pattern_x = BX_CIRRUS_THIS control.reg[0x2f] & 0x1f;
    srcskipleft = pattern_x / 3;
  } else {
    srcskipleft = BX_CIRRUS_THIS control.reg[0x2f] & 0x07;
    pattern_x = srcskipleft * BX_CIRRUS_THIS bitblt.pixelwidth;
  }
  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_COLOREXPAND) {
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_TRANSPARENTCOMP) {
      color[0] = BX_CIRRUS_THIS control.shadow_reg1;
      color[1] = BX_CIRRUS_THIS control.reg[0x11];
      color[2] = BX_CIRRUS_THIS control.reg[0x13];
      color[3] = BX_CIRRUS_THIS control.reg[0x15];
      if (BX_CIRRUS_THIS bitblt.bltmodeext & CIRRUS_BLTMODEEXT_COLOREXPINV) {
        bits_xor = 0xff;
      } else {
        bits_xor = 0x00;
      }

      pattern_y = BX_CIRRUS_THIS bitblt.srcaddr & 0x07;
      for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
        dstaddr = (BX_CIRRUS_THIS bitblt.dstaddr + pattern_x) & BX_CIRRUS_THIS memsize_mask;
        bitmask = 0x80 >> srcskipleft;
        bits = BX_CIRRUS_THIS bitblt.src[pattern_y] ^ bits_xor;
        for (x = pattern_x; x < BX_CIRRUS_THIS bitblt.bltwidth; x+=BX_CIRRUS_THIS bitblt.pixelwidth) {
          if ((bitmask & 0xff) == 0) {
            bitmask = 0x80;
            bits = BX_CIRRUS_THIS bitblt.src[pattern_y] ^ bits_xor;
          }
          dst = BX_CIRRUS_THIS s.memory + dstaddr;
          if (bits & bitmask) {
            (*BX_CIRRUS_THIS bitblt.rop_handler)(
              dst, &color[0], 0, 0, BX_CIRRUS_THIS bitblt.pixelwidth, 1);
          }
          dstaddr = (dstaddr + BX_CIRRUS_THIS bitblt.pixelwidth) & BX_CIRRUS_THIS memsize_mask;
          bitmask >>= 1;
        }
        pattern_y = (pattern_y + 1) & 7;
        BX_CIRRUS_THIS bitblt.dstaddr += BX_CIRRUS_THIS bitblt.dstpitch;
      }
      return;
    } else {
      svga_colorexpand(work_colorexp,BX_CIRRUS_THIS bitblt.src,8*8,BX_CIRRUS_THIS bitblt.pixelwidth);
      BX_CIRRUS_THIS bitblt.src = work_colorexp;
      BX_CIRRUS_THIS bitblt.bltmode &= ~CIRRUS_BLTMODE_COLOREXPAND;
    }
  } else {
    if (BX_CIRRUS_THIS bitblt.pixelwidth == 3) {
      pattern_pitch = 32;
    }
  }
  if (BX_CIRRUS_THIS bitblt.bltmode & ~CIRRUS_BLTMODE_PATTERNCOPY) {
    BX_ERROR(("PATTERNCOPY: unknown bltmode %02x",BX_CIRRUS_THIS bitblt.bltmode));
    return;
  }

  BX_DEBUG(("svga_cirrus: PATTERN COPY"));
  pattern_y = BX_CIRRUS_THIS bitblt.srcaddr & 0x07;
  src = (Bit8u *)BX_CIRRUS_THIS bitblt.src;
  for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
    srcc = src + pattern_y * pattern_pitch;
    dstaddr = (BX_CIRRUS_THIS bitblt.dstaddr + pattern_x) & BX_CIRRUS_THIS memsize_mask;
    for (x = pattern_x; x < bltbytes; x += BX_CIRRUS_THIS bitblt.pixelwidth) {
      src2 = srcc + (x % patternbytes);
      dst = BX_CIRRUS_THIS s.memory + dstaddr;
      (*BX_CIRRUS_THIS bitblt.rop_handler)(
        dst, src2, 0, 0, BX_CIRRUS_THIS bitblt.pixelwidth, 1);
      dstaddr = (dstaddr + BX_CIRRUS_THIS bitblt.pixelwidth) & BX_CIRRUS_THIS memsize_mask;
    }
    pattern_y = (pattern_y + 1) & 7;
    BX_CIRRUS_THIS bitblt.dstaddr += BX_CIRRUS_THIS bitblt.dstpitch;
  }
}

void bx_svga_cirrus_c::svga_simplebitblt()
{
  Bit8u color[4];
  Bit8u work_colorexp[2048];
  Bit16u w, x, y, pxcolor, trcolor;
  Bit8u *src, *dst;
  unsigned bits, bits_xor, bitmask;
  int pattern_x, srcskipleft;

  if (BX_CIRRUS_THIS bitblt.pixelwidth == 3) {
    pattern_x = BX_CIRRUS_THIS control.reg[0x2f] & 0x1f;
    srcskipleft = pattern_x / 3;
  } else {
    srcskipleft = BX_CIRRUS_THIS control.reg[0x2f] & 0x07;
    pattern_x = srcskipleft * BX_CIRRUS_THIS bitblt.pixelwidth;
  }
  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_COLOREXPAND) {
    if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_TRANSPARENTCOMP) {
      color[0] = BX_CIRRUS_THIS control.shadow_reg1;
      color[1] = BX_CIRRUS_THIS control.reg[0x11];
      color[2] = BX_CIRRUS_THIS control.reg[0x13];
      color[3] = BX_CIRRUS_THIS control.reg[0x15];
      if (BX_CIRRUS_THIS bitblt.bltmodeext & CIRRUS_BLTMODEEXT_COLOREXPINV) {
        bits_xor = 0xff;
      } else {
        bits_xor = 0x00;
      }

      for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
        dst = BX_CIRRUS_THIS bitblt.dst + pattern_x;
        bitmask = 0x80 >> srcskipleft;
        bits = *BX_CIRRUS_THIS bitblt.src++ ^ bits_xor;
        for (x = pattern_x; x < BX_CIRRUS_THIS bitblt.bltwidth; x+=BX_CIRRUS_THIS bitblt.pixelwidth) {
          if ((bitmask & 0xff) == 0) {
            bitmask = 0x80;
            bits = *BX_CIRRUS_THIS bitblt.src++ ^ bits_xor;
          }
          if (bits & bitmask) {
            (*BX_CIRRUS_THIS bitblt.rop_handler)(
              dst, &color[0], 0, 0, BX_CIRRUS_THIS bitblt.pixelwidth, 1);
          }
          dst += BX_CIRRUS_THIS bitblt.pixelwidth;
          bitmask >>= 1;
        }
        BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
      }
      return;
    } else {
      w = BX_CIRRUS_THIS bitblt.bltwidth / BX_CIRRUS_THIS bitblt.pixelwidth;
      for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
        svga_colorexpand(work_colorexp,BX_CIRRUS_THIS bitblt.src, w,
                         BX_CIRRUS_THIS bitblt.pixelwidth);
        dst = BX_CIRRUS_THIS bitblt.dst + pattern_x;
        (*BX_CIRRUS_THIS bitblt.rop_handler)(
          dst, work_colorexp + pattern_x, 0, 0,
          BX_CIRRUS_THIS bitblt.bltwidth - pattern_x, 1);
        BX_CIRRUS_THIS bitblt.src += ((w + 7) >> 3);
        BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
      }
      return;
    }
  } else if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_TRANSPARENTCOMP) {
    if (BX_CIRRUS_THIS bitblt.pixelwidth == 1) {
      trcolor = BX_CIRRUS_THIS control.reg[0x34];
      for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
        src = (Bit8u*)BX_CIRRUS_THIS bitblt.src;
        dst = BX_CIRRUS_THIS bitblt.dst;
        for (x = 0; x < BX_CIRRUS_THIS bitblt.bltwidth; x++) {
          if (*src != trcolor) {
            (*BX_CIRRUS_THIS bitblt.rop_handler)(dst, src, 0, 0, 1, 1);
          }
          src++;
          dst++;
        }
        BX_CIRRUS_THIS bitblt.src += BX_CIRRUS_THIS bitblt.srcpitch;
        BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
      }
      return;
    } else if (BX_CIRRUS_THIS bitblt.pixelwidth == 2) {
      trcolor = BX_CIRRUS_THIS control.reg[0x34] | (BX_CIRRUS_THIS control.reg[0x35] << 8);
      for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
        src = (Bit8u*)BX_CIRRUS_THIS bitblt.src;
        dst = BX_CIRRUS_THIS bitblt.dst;
        for (x = 0; x < BX_CIRRUS_THIS bitblt.bltwidth; x+=2) {
          pxcolor = src[0] | (src[1] << 8);
          if (pxcolor != trcolor) {
            (*BX_CIRRUS_THIS bitblt.rop_handler)(dst, src, 0, 0, 2, 1);
          }
          src += 2;
          dst += 2;
        }
        BX_CIRRUS_THIS bitblt.src += BX_CIRRUS_THIS bitblt.srcpitch;
        BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
      }
      return;
    } else {
      BX_ERROR(("SIMPLE BLT: bltmode TRANSPARENTCOMP: depth > 16 bpp unsupported"));
      return;
    }
  } else if (BX_CIRRUS_THIS bitblt.bltmode & ~CIRRUS_BLTMODE_BACKWARDS) {
    BX_ERROR(("SIMPLE BLT: unknown bltmode %02x",BX_CIRRUS_THIS bitblt.bltmode));
    return;
  }

  BX_DEBUG(("svga_cirrus: BITBLT"));
  (*BX_CIRRUS_THIS bitblt.rop_handler)(
    BX_CIRRUS_THIS bitblt.dst, BX_CIRRUS_THIS bitblt.src,
    BX_CIRRUS_THIS bitblt.dstpitch, BX_CIRRUS_THIS bitblt.srcpitch,
    BX_CIRRUS_THIS bitblt.bltwidth, BX_CIRRUS_THIS bitblt.bltheight);
}

void bx_svga_cirrus_c::svga_solidfill()
{
  Bit8u color[4];
  int x, y;
  Bit8u *dst;

  BX_DEBUG(("BLT: SOLIDFILL"));

  color[0] = BX_CIRRUS_THIS control.shadow_reg1;
  color[1] = BX_CIRRUS_THIS control.reg[0x11];
  color[2] = BX_CIRRUS_THIS control.reg[0x13];
  color[3] = BX_CIRRUS_THIS control.reg[0x15];

  for (y = 0; y < BX_CIRRUS_THIS bitblt.bltheight; y++) {
    dst = BX_CIRRUS_THIS bitblt.dst;
    for (x = 0; x < BX_CIRRUS_THIS bitblt.bltwidth; x+=BX_CIRRUS_THIS bitblt.pixelwidth) {
      (*BX_CIRRUS_THIS bitblt.rop_handler)(
        dst, &color[0], 0, 0, BX_CIRRUS_THIS bitblt.pixelwidth, 1);
        dst += BX_CIRRUS_THIS bitblt.pixelwidth;
    }
    BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
  }
  BX_CIRRUS_THIS redraw_area(BX_CIRRUS_THIS redraw.x, BX_CIRRUS_THIS redraw.y,
                             BX_CIRRUS_THIS redraw.w, BX_CIRRUS_THIS redraw.h);
}

void bx_svga_cirrus_c::svga_patterncopy_memsrc()
{
  BX_INFO(("svga_patterncopy_memsrc() - not tested"));

  BX_CIRRUS_THIS bitblt.src = &BX_CIRRUS_THIS bitblt.memsrc[0];
  svga_patterncopy();
  BX_CIRRUS_THIS bitblt.memsrc_needed = 0;
  BX_CIRRUS_THIS redraw_area(BX_CIRRUS_THIS redraw.x, BX_CIRRUS_THIS redraw.y,
                             BX_CIRRUS_THIS redraw.w, BX_CIRRUS_THIS redraw.h);
}

void bx_svga_cirrus_c::svga_simplebitblt_memsrc()
{
  Bit8u *srcptr = &BX_CIRRUS_THIS bitblt.memsrc[0];
  Bit8u work_colorexp[2048];
  Bit16u w;
  int pattern_x;
  int byteofs = (BX_CIRRUS_THIS control.reg[0x2f] >> 5) & 0x03;

  BX_DEBUG(("svga_cirrus: BLT, cpu-to-video"));

  if (BX_CIRRUS_THIS bitblt.bltmode & CIRRUS_BLTMODE_COLOREXPAND) {
    if (BX_CIRRUS_THIS bitblt.pixelwidth == 3) {
      pattern_x = (BX_CIRRUS_THIS control.reg[0x2f] & 0x1f) + byteofs * 24;
    }
    else {
      pattern_x = ((BX_CIRRUS_THIS control.reg[0x2f] & 0x07) + byteofs * 8) * BX_CIRRUS_THIS bitblt.pixelwidth;
    }
    if (BX_CIRRUS_THIS bitblt.bltmode & ~CIRRUS_BLTMODE_COLOREXPAND) {
      BX_ERROR(("cpu-to-video BLT: unknown bltmode %02x",BX_CIRRUS_THIS bitblt.bltmode));
      return;
    }

    w = BX_CIRRUS_THIS bitblt.bltwidth / BX_CIRRUS_THIS bitblt.pixelwidth;
    svga_colorexpand(work_colorexp,srcptr,w,BX_CIRRUS_THIS bitblt.pixelwidth);
    (*BX_CIRRUS_THIS bitblt.rop_handler)(
        BX_CIRRUS_THIS bitblt.dst + pattern_x, work_colorexp + pattern_x, 0, 0,
        BX_CIRRUS_THIS bitblt.bltwidth - pattern_x, 1);
  } else {
    if (BX_CIRRUS_THIS bitblt.bltmode != 0) {
      BX_ERROR(("cpu-to-video BLT: unknown bltmode %02x",BX_CIRRUS_THIS bitblt.bltmode));
      return;
    }
    (*BX_CIRRUS_THIS bitblt.rop_handler)(
        BX_CIRRUS_THIS bitblt.dst + byteofs, srcptr + byteofs, 0, 0,
        BX_CIRRUS_THIS bitblt.bltwidth - byteofs, 1);
  }
}

void bx_svga_cirrus_c::svga_colorexpand_transp_memsrc()
{
  Bit8u *src = &BX_CIRRUS_THIS bitblt.memsrc[0];
  Bit8u color[4];
  int x, pattern_x, srcskipleft;
  Bit32u dstaddr;
  Bit8u *dst;
  unsigned bits, bits_xor, bitmask;
  int byteofs;

  BX_DEBUG(("BLT, cpu-to-video, transparent"));

  byteofs = (BX_CIRRUS_THIS control.reg[0x2f] >> 5) & 3;
  src += byteofs;
  byteofs <<= 3;
  if (BX_CIRRUS_THIS bitblt.pixelwidth == 3) {
    pattern_x = BX_CIRRUS_THIS control.reg[0x2f] & 0x1f;
    srcskipleft = pattern_x / 3;
    pattern_x += byteofs * 3;
  } else {
    srcskipleft = BX_CIRRUS_THIS control.reg[0x2f] & 0x07;
    pattern_x = (srcskipleft + byteofs) * BX_CIRRUS_THIS bitblt.pixelwidth;
  }
  color[0] = BX_CIRRUS_THIS control.shadow_reg1;
  color[1] = BX_CIRRUS_THIS control.reg[0x11];
  color[2] = BX_CIRRUS_THIS control.reg[0x13];
  color[3] = BX_CIRRUS_THIS control.reg[0x15];
  if (BX_CIRRUS_THIS bitblt.bltmodeext & CIRRUS_BLTMODEEXT_COLOREXPINV) {
    bits_xor = 0xff;
  } else {
    bits_xor = 0x00;
  }

  dstaddr = (BX_CIRRUS_THIS bitblt.dstaddr + pattern_x) & BX_CIRRUS_THIS memsize_mask;
  bitmask = 0x80 >> srcskipleft;
  bits = *src++ ^ bits_xor;
  for (x = pattern_x; x < BX_CIRRUS_THIS bitblt.bltwidth; x+=BX_CIRRUS_THIS bitblt.pixelwidth) {
    if ((bitmask & 0xff) == 0) {
      bitmask = 0x80;
      bits = *src++ ^ bits_xor;
    }
    if (bits & bitmask) {
      dst = BX_CIRRUS_THIS s.memory + dstaddr;
      (*BX_CIRRUS_THIS bitblt.rop_handler)(
          dst, &color[0], 0, 0, BX_CIRRUS_THIS bitblt.pixelwidth, 1);
    }
    dstaddr += BX_CIRRUS_THIS bitblt.pixelwidth;
    bitmask >>= 1;
  }
}

  bool // 1 if finished, 0 otherwise
bx_svga_cirrus_c::svga_asyncbitblt_next()
{
  int count;
  int avail;

  if (BX_CIRRUS_THIS bitblt.bitblt_ptr == NULL) {
    BX_PANIC(("svga_asyncbitblt_next: unexpected call"));
    goto cleanup;
  }

  if (BX_CIRRUS_THIS bitblt.memdst_needed > 0) {
    BX_CIRRUS_THIS bitblt.memdst_needed -= (int)(BX_CIRRUS_THIS bitblt.memdst_ptr - &BX_CIRRUS_THIS bitblt.memdst[0]);
    avail = BX_MIN(CIRRUS_BLT_CACHESIZE, BX_CIRRUS_THIS bitblt.memdst_needed);
    BX_CIRRUS_THIS bitblt.memdst_ptr = &BX_CIRRUS_THIS bitblt.memdst[0];
    BX_CIRRUS_THIS bitblt.memdst_endptr = &BX_CIRRUS_THIS bitblt.memdst[avail];

    if (BX_CIRRUS_THIS bitblt.memsrc_needed <= 0 &&
        BX_CIRRUS_THIS bitblt.memdst_needed <= 0) {
      goto cleanup;
    }
  }

  (*BX_CIRRUS_THIS bitblt.bitblt_ptr)();

  if (BX_CIRRUS_THIS bitblt.memsrc_needed > 0) {
    BX_CIRRUS_THIS bitblt.dst += BX_CIRRUS_THIS bitblt.dstpitch;
    BX_CIRRUS_THIS bitblt.dstaddr += BX_CIRRUS_THIS bitblt.dstpitch;
    BX_CIRRUS_THIS bitblt.memsrc_needed -= BX_CIRRUS_THIS bitblt.srcpitch;
    if (BX_CIRRUS_THIS bitblt.memsrc_needed <= 0) {
      BX_CIRRUS_THIS redraw_area(BX_CIRRUS_THIS redraw.x, BX_CIRRUS_THIS redraw.y,
                                 BX_CIRRUS_THIS redraw.w, BX_CIRRUS_THIS redraw.h);
      if (BX_CIRRUS_THIS bitblt.memdst_needed <= 0) {
        goto cleanup;
      }
    } else {
      count = (int)(BX_CIRRUS_THIS bitblt.memsrc_endptr - BX_CIRRUS_THIS bitblt.memsrc_ptr);
      memmove(&BX_CIRRUS_THIS bitblt.memsrc[0], BX_CIRRUS_THIS bitblt.memsrc_ptr, count);
      BX_CIRRUS_THIS bitblt.memsrc_ptr = &BX_CIRRUS_THIS bitblt.memsrc[count];
    }
  }

  return 0;

cleanup:
  svga_reset_bitblt();
  return 1;
}

/////////////////////////////////////////////////////////////////////////
//
// Raster operations.
//
/////////////////////////////////////////////////////////////////////////

bx_bitblt_rop_t bx_svga_cirrus_c::svga_get_fwd_rop_handler(Bit8u rop)
{
  bx_bitblt_rop_t rop_handler = bitblt_rop_fwd_nop;

  switch (rop)
  {
    case CIRRUS_ROP_0:
      rop_handler = bitblt_rop_fwd_0;
      break;
    case CIRRUS_ROP_SRC_AND_DST:
      rop_handler = bitblt_rop_fwd_src_and_dst;
      break;
    case CIRRUS_ROP_NOP:
      rop_handler = bitblt_rop_fwd_nop;
      break;
    case CIRRUS_ROP_SRC_AND_NOTDST:
      rop_handler = bitblt_rop_fwd_src_and_notdst;
      break;
    case CIRRUS_ROP_NOTDST:
      rop_handler = bitblt_rop_fwd_notdst;
      break;
    case CIRRUS_ROP_SRC:
      rop_handler = bitblt_rop_fwd_src;
      break;
    case CIRRUS_ROP_1:
      rop_handler = bitblt_rop_fwd_1;
      break;
    case CIRRUS_ROP_NOTSRC_AND_DST:
      rop_handler = bitblt_rop_fwd_notsrc_and_dst;
      break;
    case CIRRUS_ROP_SRC_XOR_DST:
      rop_handler = bitblt_rop_fwd_src_xor_dst;
      break;
    case CIRRUS_ROP_SRC_OR_DST:
      rop_handler = bitblt_rop_fwd_src_or_dst;
      break;
    case CIRRUS_ROP_NOTSRC_OR_NOTDST:
      rop_handler = bitblt_rop_fwd_notsrc_or_notdst;
      break;
    case CIRRUS_ROP_SRC_NOTXOR_DST:
      rop_handler = bitblt_rop_fwd_src_notxor_dst;
      break;
    case CIRRUS_ROP_SRC_OR_NOTDST:
      rop_handler = bitblt_rop_fwd_src_or_notdst;
      break;
    case CIRRUS_ROP_NOTSRC:
      rop_handler = bitblt_rop_fwd_notsrc;
      break;
    case CIRRUS_ROP_NOTSRC_OR_DST:
      rop_handler = bitblt_rop_fwd_notsrc_or_dst;
      break;
    case CIRRUS_ROP_NOTSRC_AND_NOTDST:
      rop_handler = bitblt_rop_fwd_notsrc_and_notdst;
      break;
    default:
      BX_ERROR(("unknown ROP %02x",rop));
      break;
  }

  return rop_handler;
}

bx_bitblt_rop_t bx_svga_cirrus_c::svga_get_bkwd_rop_handler(Bit8u rop)
{
  bx_bitblt_rop_t rop_handler = bitblt_rop_bkwd_nop;

  switch (rop)
  {
    case CIRRUS_ROP_0:
      rop_handler = bitblt_rop_bkwd_0;
      break;
    case CIRRUS_ROP_SRC_AND_DST:
      rop_handler = bitblt_rop_bkwd_src_and_dst;
      break;
    case CIRRUS_ROP_NOP:
      rop_handler = bitblt_rop_bkwd_nop;
      break;
    case CIRRUS_ROP_SRC_AND_NOTDST:
      rop_handler = bitblt_rop_bkwd_src_and_notdst;
      break;
    case CIRRUS_ROP_NOTDST:
      rop_handler = bitblt_rop_bkwd_notdst;
      break;
    case CIRRUS_ROP_SRC:
      rop_handler = bitblt_rop_bkwd_src;
      break;
    case CIRRUS_ROP_1:
      rop_handler = bitblt_rop_bkwd_1;
      break;
    case CIRRUS_ROP_NOTSRC_AND_DST:
      rop_handler = bitblt_rop_bkwd_notsrc_and_dst;
      break;
    case CIRRUS_ROP_SRC_XOR_DST:
      rop_handler = bitblt_rop_bkwd_src_xor_dst;
      break;
    case CIRRUS_ROP_SRC_OR_DST:
      rop_handler = bitblt_rop_bkwd_src_or_dst;
      break;
    case CIRRUS_ROP_NOTSRC_OR_NOTDST:
      rop_handler = bitblt_rop_bkwd_notsrc_or_notdst;
      break;
    case CIRRUS_ROP_SRC_NOTXOR_DST:
      rop_handler = bitblt_rop_bkwd_src_notxor_dst;
      break;
    case CIRRUS_ROP_SRC_OR_NOTDST:
      rop_handler = bitblt_rop_bkwd_src_or_notdst;
      break;
    case CIRRUS_ROP_NOTSRC:
      rop_handler = bitblt_rop_bkwd_notsrc;
      break;
    case CIRRUS_ROP_NOTSRC_OR_DST:
      rop_handler = bitblt_rop_bkwd_notsrc_or_dst;
      break;
    case CIRRUS_ROP_NOTSRC_AND_NOTDST:
      rop_handler = bitblt_rop_bkwd_notsrc_and_notdst;
      break;
    default:
      BX_ERROR(("unknown ROP %02x",rop));
      break;
  }

  return rop_handler;
}

#if BX_DEBUGGER
void bx_svga_cirrus_c::debug_dump(int argc, char **argv)
{
  if ((BX_CIRRUS_THIS sequencer.reg[0x07] & 0x01) == CIRRUS_SR7_BPP_SVGA) {
#if BX_SUPPORT_PCI
    if (BX_CIRRUS_THIS pci_enabled)
    {
      dbg_printf("CL-GD5446 PCI\n\n");
    }
    else
#endif
    {
      dbg_printf("CL-GD5430 ISA\n\n");
    }
    dbg_printf("current mode: %u x %u x %u\n", BX_CIRRUS_THIS svga_xres,
               BX_CIRRUS_THIS svga_yres, BX_CIRRUS_THIS svga_dispbpp);
    if (argc > 0) {
      dbg_printf("\nAdditional options not supported\n");
    }
  } else {
    bx_vgacore_c::debug_dump(argc, argv);
  }
}
#endif

#endif // BX_SUPPORT_CLGD54XX
