/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2018  The Bochs Project
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

// PCI host bridge support
// i430FX - TSC/TDP
// i440FX - PMC/DBX
// i440BX - Host bridge

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI

#include "pci.h"

#define LOG_THIS thePciBridge->

const char csname[3][20] = {"i430FX TSC", "i440FX PMC", "i440BX Host bridge"};

bx_pci_bridge_c *thePciBridge = NULL;

int CDECL libpci_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
{
  if (type == PLUGTYPE_CORE) {
    thePciBridge = new bx_pci_bridge_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciBridge, BX_PLUGIN_PCI);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libpci_LTX_plugin_fini(void)
{
  delete thePciBridge;
}

bx_pci_bridge_c::bx_pci_bridge_c()
{
  put("PCI");
  vbridge = NULL;
}

bx_pci_bridge_c::~bx_pci_bridge_c()
{
  if (vbridge != NULL) {
    delete vbridge;
  }
  SIM->get_bochs_root()->remove("pci_bridge");
  BX_DEBUG(("Exit"));
}

void bx_pci_bridge_c::init(void)
{
  // called once when bochs initializes
  unsigned i;
  Bit32u ramsize;

  Bit8u devfunc = BX_PCI_DEVICE(0, 0);
  BX_PCI_THIS chipset = SIM->get_param_enum(BXPN_PCI_CHIPSET)->get();
  DEV_register_pci_handlers(this, &devfunc, BX_PLUGIN_PCI, csname[BX_PCI_THIS chipset]);

  // initialize readonly registers
  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
    init_pci_conf(0x8086, 0x0122, 0x02, 0x060000, 0x00, 0);
  } else if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
    init_pci_conf(0x8086, 0x7190, 0x02, 0x060000, 0x00, 0);
    BX_PCI_THIS pci_conf[0x10] = 0x08;
    init_bar_mem(0, 0xf0000000, agp_ap_read_handler, agp_ap_write_handler);
    BX_PCI_THIS pci_conf[0x06] = 0x10;
    BX_PCI_THIS pci_conf[0x34] = 0xa0;
    BX_PCI_THIS pci_conf[0xa0] = 0x02;
    BX_PCI_THIS pci_conf[0xa2] = 0x10;
    BX_PCI_THIS pci_conf[0xa4] = 0x03;
    BX_PCI_THIS pci_conf[0xa5] = 0x02;
    BX_PCI_THIS pci_conf[0xa7] = 0x1f;
    BX_PCI_THIS pci_conf[0xf3] = 0xf8;
    BX_PCI_THIS pci_conf[0xf8] = 0x20;
    BX_PCI_THIS pci_conf[0xf9] = 0x0f;
    BX_PCI_THIS vbridge = new bx_pci_vbridge_c();
    BX_PCI_THIS vbridge->init();
  } else { // i440FX
    init_pci_conf(0x8086, 0x1237, 0x00, 0x060000, 0x00, 0);
  }

  // DRAM module setup
  for (i = 0; i < 8; i++)
    BX_PCI_THIS DRBA[i] = 0x0;
  ramsize = SIM->get_param_num(BXPN_MEM_SIZE)->get();
  if ((ramsize & 0x07) != 0) {
    ramsize = (ramsize & ~0x07) + 8;
  }
  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
    if (ramsize > 128) ramsize = 128;
    if (ramsize == 8) {
      for (i = 0; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x02;
      }
    } else if (ramsize == 16) {
      BX_PCI_THIS DRBA[0] = 0x02;
      for (i = 1; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x04;
      }
    } else if (ramsize == 24) {
      BX_PCI_THIS DRBA[0] = 0x02;
      BX_PCI_THIS DRBA[1] = 0x04;
      for (i = 2; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x06;
      }
    } else if (ramsize == 32) {
      BX_PCI_THIS DRBA[0] = 0x04;
      for (i = 1; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x08;
      }
    } else if (ramsize <= 48) {
      BX_PCI_THIS DRBA[0] = 0x04;
      BX_PCI_THIS DRBA[1] = 0x08;
      for (i = 2; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x0c;
      }
    } else if (ramsize <= 64) {
      BX_PCI_THIS DRBA[0] = 0x08;
      for (i = 1; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x10;
      }
    } else if (ramsize <= 96) {
      BX_PCI_THIS DRBA[0] = 0x04;
      BX_PCI_THIS DRBA[1] = 0x08;
      BX_PCI_THIS DRBA[2] = 0x10;
      BX_PCI_THIS DRBA[3] = 0x18;
      BX_PCI_THIS DRBA[4] = 0x18;
    } else if (ramsize <= 128) {
      BX_PCI_THIS DRBA[0] = 0x10;
      for (i = 1; i < 5; i++) {
        BX_PCI_THIS DRBA[i] = 0x20;
      }
    }
  } else { // i440FX
    const Bit8u type[3] = {128, 32, 8};
    if (ramsize > 1024) ramsize = 1024;
    Bit8u drbval = 0;
    unsigned row = 0;
    unsigned ti = 0;
    while ((ramsize > 0) && (row < 8) && (ti < 3)) {
      unsigned mc = ramsize / type[ti];
      ramsize = ramsize % type[ti];
      for (i = 0; i < mc; i++) {
        drbval += (type[ti] >> 3);
        BX_PCI_THIS DRBA[row++] = drbval;
        if (row == 8) break;
      }
      ti++;
    }
    while (row < 8) {
      BX_PCI_THIS DRBA[row++] = drbval;
    }
  }
  for (i = 0; i < 8; i++)
    BX_PCI_THIS pci_conf[0x60 + i] = BX_PCI_THIS DRBA[i];
  dram_detect = 0;

#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("pci", this);
#endif
}

  void
bx_pci_bridge_c::reset(unsigned type)
{
  unsigned i;

  BX_PCI_THIS pci_conf[0x04] = 0x06;
  BX_PCI_THIS pci_conf[0x05] = 0x00;
  BX_PCI_THIS pci_conf[0x07] = 0x02;
  BX_PCI_THIS pci_conf[0x0d] = 0x00;
  BX_PCI_THIS pci_conf[0x0f] = 0x00;
  BX_PCI_THIS pci_conf[0x50] = 0x00;
  BX_PCI_THIS pci_conf[0x52] = 0x00;
  BX_PCI_THIS pci_conf[0x53] = 0x80;
  BX_PCI_THIS pci_conf[0x54] = 0x00;
  BX_PCI_THIS pci_conf[0x55] = 0x00;
  BX_PCI_THIS pci_conf[0x56] = 0x00;
  BX_PCI_THIS pci_conf[0x57] = 0x01;
  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
    BX_PCI_THIS pci_conf[0x06] = 0x00;
    BX_PCI_THIS pci_conf[0x58] = 0x00;
  } else if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
    BX_PCI_THIS vbridge->reset(type);
  } else { // i440FX
    BX_PCI_THIS pci_conf[0x06] = 0x80;
    BX_PCI_THIS pci_conf[0x51] = 0x01;
    BX_PCI_THIS pci_conf[0x58] = 0x10;
    BX_PCI_THIS pci_conf[0xb4] = 0x00;
    BX_PCI_THIS pci_conf[0xb9] = 0x00;
    BX_PCI_THIS pci_conf[0xba] = 0x00;
    BX_PCI_THIS pci_conf[0xbb] = 0x00;
    BX_PCI_THIS gart_base = 0;
  }
  for (i=0x59; i<0x60; i++)
    BX_PCI_THIS pci_conf[i] = 0x00;
  for (i = 0; i <= BX_MEM_AREA_F0000; i++) {
    DEV_mem_set_memory_type(i, 0, 0);
    DEV_mem_set_memory_type(i, 1, 0);
  }
  BX_PCI_THIS pci_conf[0x72] = 0x02;
}

void bx_pci_bridge_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pci_bridge", "PCI Bridge State");
  register_pci_state(list);
  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
    BX_PCI_THIS vbridge->register_state();
  }
}

void bx_pci_bridge_c::after_restore_state(void)
{
  BX_PCI_THIS smram_control(BX_PCI_THIS pci_conf[0x72]);
  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
    BX_PCI_THIS vbridge->after_restore_state();
  }
}

// pci configuration space write callback handler
void bx_pci_bridge_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  unsigned area;
  Bit8u drba_reg, old_dram_detect;
  bx_bool drba_changed;
  bx_bool attbase_changed = 0;
  Bit32u apsize;

  old_dram_detect = BX_PCI_THIS dram_detect;
  if ((address >= 0x10) && (address < 0x34))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_PCI_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x02) | 0x04;
        } else {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x40) | 0x06;
        }
        break;
      case 0x05:
        if (BX_PCI_THIS chipset != BX_PCI_CHIPSET_I430FX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x01);
        }
        break;
      case 0x07:
        if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
          value8 &= 0x30;
        } else if (BX_PCI_THIS chipset != BX_PCI_CHIPSET_I440BX) {
          value8 = (BX_PCI_THIS pci_conf[0x07] & ~value8) | 0x02;
        } else {
          value8 &= 0xf9;
        }
        BX_PCI_THIS pci_conf[address+i] &= ~value8;
        break;
      case 0x0d:
        BX_PCI_THIS pci_conf[address+i] = (value8 & 0xf8);
        break;
      case 0x06:
      case 0x0c:
      case 0x0f:
        break;
      case 0x50:
        if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0xef);
        } else if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0xec);
        } else {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x70);
        }
        break;
      case 0x51:
        if (BX_PCI_THIS chipset != BX_PCI_CHIPSET_I430FX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x80) | 0x01;
        } else if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
          BX_PCI_THIS pci_conf[address+i] = (value8 & 0x8f);
        }
        break;
      case 0x59:
      case 0x5A:
      case 0x5B:
      case 0x5C:
      case 0x5D:
      case 0x5E:
      case 0x5F:
        if (value8 != oldval) {
          BX_PCI_THIS pci_conf[address+i] = value8;
          if ((address+i) == 0x59) {
            area = BX_MEM_AREA_F0000;
            DEV_mem_set_memory_type(area, 0, (value8 >> 4) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value8 >> 5) & 0x1);
          } else {
            area = ((address+i) - 0x5a) << 1;
            DEV_mem_set_memory_type(area, 0, (value8 >> 0) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value8 >> 1) & 0x1);
            area++;
            DEV_mem_set_memory_type(area, 0, (value8 >> 4) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value8 >> 5) & 0x1);
          }
          BX_INFO(("%s write to PAM register %x (TLB Flush)", csname[BX_PCI_THIS chipset], address+i));
          bx_pc_system.MemoryMappingChanged();
        }
        break;
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x66:
      case 0x67:
        BX_PCI_THIS pci_conf[address+i] = value8;
        drba_reg = (address + i) & 0x07;
        drba_changed = (BX_PCI_THIS pci_conf[0x60 + drba_reg] != BX_PCI_THIS DRBA[drba_reg]);
        if (drba_changed) {
          BX_PCI_THIS dram_detect |= (1 << drba_reg);
        } else if (!drba_changed && dram_detect) {
          BX_PCI_THIS dram_detect &= ~(1 << drba_reg);
        }
        break;
      case 0x72:
        smram_control(value8); // SMRAM control register
        break;
      case 0xb4:
        if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
          BX_PCI_THIS pci_conf[address+i] = value8 & 0x3f;
          switch (BX_PCI_THIS pci_conf[0xb4]) {
            case 0x00:
              apsize = (1 << 28);
              break;
            case 0x20:
              apsize = (1 << 27);
              break;
            case 0x30:
              apsize = (1 << 26);
              break;
            case 0x38:
              apsize = (1 << 25);
              break;
            case 0x3c:
              apsize = (1 << 24);
              break;
            case 0x3e:
              apsize = (1 << 23);
              break;
            case 0x3f:
              apsize = (1 << 22);
              break;
            default:
              BX_ERROR(("Invalid AGP aperture size mask"));
              apsize = 0;
          }
          BX_INFO(("AGP aperture size set to %d MB", apsize >> 20));
          pci_bar[0].size = apsize;
        }
        break;
      case 0xb8:
        break;
      case 0xb9:
        value8 &= 0xf0;
      case 0xba:
      case 0xbb:
        if ((BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) &&
            (value8 != oldval)) {
          BX_PCI_THIS pci_conf[address+i] = value8;
          attbase_changed |= 1;
        }
        break;
      case 0xf0:
        if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
          BX_PCI_THIS pci_conf[address+i] = value8 & 0xc0;
        }
        break;
      default:
        BX_PCI_THIS pci_conf[address+i] = value8;
        BX_DEBUG(("%s write register 0x%02x value 0x%02x", csname[BX_PCI_THIS chipset], address+i, value8));
    }
  }
  if ((BX_PCI_THIS dram_detect > 0) && (old_dram_detect == 0)) {
    // TODO
    BX_ERROR(("FIXME: DRAM module detection"));
  } else if ((BX_PCI_THIS dram_detect == 0) && (old_dram_detect > 0)) {
    // TODO
    BX_INFO(("normal memory access mode"));
  }
  if (attbase_changed) {
    BX_PCI_THIS gart_base = ((BX_PCI_THIS pci_conf[0xbb] << 24) |
                             (BX_PCI_THIS pci_conf[0xba] << 16) |
                             (BX_PCI_THIS pci_conf[0xb9] << 8));
    BX_INFO(("New GART base address = 0x%08x", BX_PCI_THIS gart_base));
  }
}

bx_bool bx_pci_bridge_c::agp_ap_read_handler(bx_phy_address addr, unsigned len,
                                             void *data, void *param)
{
  bx_pci_bridge_c *class_ptr = (bx_pci_bridge_c*)param;
  Bit32u value = class_ptr->agp_aperture_read(addr, len, 0);
  switch (len) {
    case 1:
      value &= 0xFF;
      *((Bit8u *) data) = (Bit8u) value;
      break;
    case 2:
      value &= 0xFFFF;
      *((Bit16u *) data) = (Bit16u) value;
      break;
    case 4:
      *((Bit32u *) data) = value;
      break;
  }
  return 1;
}

Bit32u bx_pci_bridge_c::agp_aperture_read(bx_phy_address addr, unsigned len,
                                          bx_bool agp)
{
  if (BX_PCI_THIS pci_conf[0x51] & 0x02) {
    Bit32u offset = (Bit32u)(addr - pci_bar[0].addr);
    Bit32u gart_index = (Bit32u)(offset >> 12);
    Bit32u page_offset = (Bit32u)(offset & 0xfff);
    Bit32u gart_addr = BX_PCI_THIS gart_base + (gart_index << 2);
    Bit32u page_addr;
    DEV_MEM_READ_PHYSICAL(gart_addr, 4, (Bit8u*)&page_addr);
    BX_INFO(("TODO: AGP aperture read: page address = 0x%08x / offset = 0x%04x",
             page_addr, (Bit16u)page_offset));
    // TODO
  }
  return 0;
}

bx_bool bx_pci_bridge_c::agp_ap_write_handler(bx_phy_address addr, unsigned len,
                                              void *data, void *param)
{
  bx_pci_bridge_c *class_ptr = (bx_pci_bridge_c*)param;
  Bit32u value = *(Bit32u*)data;
  class_ptr->agp_aperture_write(addr, value, len, 0);
  return 1;
}

void bx_pci_bridge_c::agp_aperture_write(bx_phy_address addr, Bit32u value,
                                         unsigned len, bx_bool agp)
{
  if (BX_PCI_THIS pci_conf[0x51] & 0x02) {
    Bit32u offset = (Bit32u)(addr - pci_bar[0].addr);
    Bit32u gart_index = (Bit32u)(offset >> 12);
    Bit32u page_offset = (Bit32u)(offset & 0xfff);
    Bit32u gart_addr = BX_PCI_THIS gart_base + (gart_index << 2);
    Bit32u page_addr;
    DEV_MEM_READ_PHYSICAL(gart_addr, 4, (Bit8u*)&page_addr);
    BX_INFO(("TODO: AGP aperture write: page address = 0x%08x / offset = 0x%04x",
             page_addr, (Bit16u)page_offset));
    // TODO
  }
}

void bx_pci_bridge_c::smram_control(Bit8u value8)
{
  //
  // From i440FX chipset manual:
  //
  // [7:7] Reserved.
  // [6:6] SMM Space Open (DOPEN), when DOPEN=1 and DLCK=0, SMM space DRAM
  //       became visible even CPU not indicte SMM mode access. This is
  //       indended to help BIOS to initialize SMM space.
  // [5:5] SMM Space Closed (DCLS), when DCLS=1, SMM space is not accessible
  //       for data references, even if CPU indicates SMM mode access. Code
  //       references may still access SMM space DRAM.
  // [4:4] SMM Space Locked (DLCK), when DLCK=1, DOPEN is set to 0 and
  //       both DLCK and DOPEN became R/O. DLCK can only be cleared by
  //       a power-on reset.
  // [3:3] SMRAM Enable (SMRAME)
  // [2:0] SMM space base segment, program the location of SMM space
  //       reserved.
  //

  // SMRAM space access cycles:

  // | SMRAME | DLCK | DCLS | DOPEN | CPU_SMM |    | Code | Data |
  // ------------------------------------------    ---------------
  // |    0   |  X   |  X   |   X   |    X    | -> |  PCI |  PCI |
  // |    1   |  0   |  X   |   0   |    0    | -> |  PCI |  PCI |
  // |    1   |  0   |  0   |   0   |    1    | -> | DRAM | DRAM |
  // |    1   |  0   |  0   |   1   |    X    | -> | DRAM | DRAM |
  // |    1   |  1   |  0   |   X   |    1    | -> | DRAM | DRAM |
  // |    1   |  0   |  1   |   0   |    1    | -> | DRAM |  PCI |
  // |    1   |  0   |  1   |   1   |    X    | -> | ---- | ---- |
  // |    1   |  1   |  X   |   X   |    0    | -> |  PCI |  PCI |
  // |    1   |  1   |  1   |   X   |    1    | -> | DRAM |  PCI |
  // ------------------------------------------    ---------------

  value8 = (value8 & 0x78) | 0x2; // ignore reserved bits

  if (BX_PCI_THIS pci_conf[0x72] & 0x10)
  {
    value8 &= 0xbf; // set DOPEN=0, DLCK=1
    value8 |= 0x10;
  }

  if ((value8 & 0x08) == 0) {
    bx_devices.mem->disable_smram();
  }
  else {
    bx_bool DOPEN = (value8 & 0x40) > 0, DCLS = (value8 & 0x20) > 0;
    if(DOPEN && DCLS) BX_PANIC(("SMRAM control: DOPEN not mutually exclusive with DCLS !"));
    bx_devices.mem->enable_smram(DOPEN, DCLS);
  }

  BX_INFO(("setting SMRAM control register to 0x%02x", value8));
  BX_PCI_THIS pci_conf[0x72] = value8;
}

#if BX_DEBUGGER
void bx_pci_bridge_c::debug_dump(int argc, char **argv)
{
  int arg, i, j, r;

  if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I430FX) {
    dbg_printf("i430FX TSC/TDP");
  } else if (BX_PCI_THIS chipset == BX_PCI_CHIPSET_I440BX) {
    dbg_printf("i440BX Host bridge");
  } else {
    dbg_printf("i440FX PMC/DBX");
  }
  dbg_printf("\n\nconfAddr = 0x%08x\n\n", DEV_pci_get_confAddr());

  if (argc == 0) {
    for (i = 0x59; i < 0x60; i++) {
      dbg_printf("PAM reg 0x%02x = 0x%02x\n", i, BX_PCI_THIS pci_conf[i]);
    }
    dbg_printf("SMRAM control = 0x%02x\n", BX_PCI_THIS pci_conf[0x72]);
    dbg_printf("\nSupported options:\n");
    dbg_printf("info device 'pci' 'dump=full' - show PCI config space\n");
  } else {
    for (arg = 0; arg < argc; arg++) {
      if (!strcmp(argv[arg], "dump=full")) {
        dbg_printf("\nPCI config space\n\n");
        r = 0;
        for (i=0; i<16; i++) {
          dbg_printf("%04x ", r);
          for (j=0; j<16; j++) {
            dbg_printf(" %02x", BX_PCI_THIS pci_conf[r++]);
          }
          dbg_printf("\n");
        }
      } else {
        dbg_printf("\nUnknown option: '%s'\n", argv[arg]);
      }
    }
  }
}
#endif

// i440BX PCI-to-AGP bridge

#undef LOG_THIS
#define LOG_THIS

bx_pci_vbridge_c::bx_pci_vbridge_c()
{
  put("PCIAGP");
}

bx_pci_vbridge_c::~bx_pci_vbridge_c()
{
  SIM->get_bochs_root()->remove("pci_vbridge");
  BX_DEBUG(("Exit"));
}

void bx_pci_vbridge_c::init(void)
{
  Bit8u devfunc = BX_PCI_DEVICE(1, 0);
  DEV_register_pci_handlers(this, &devfunc, BX_PLUGIN_PCI, "i440BX PCI-to-AGP bridge");
  init_pci_conf(0x8086, 0x7191, 0x02, 0x060400, 0x01, 0);
  pci_conf[0x06] = 0x20;
  pci_conf[0x07] = 0x02;
  pci_conf[0x1e] = 0xa0;
}

void bx_pci_vbridge_c::reset(unsigned type)
{
  pci_conf[0x04] = 0x00;
  pci_conf[0x05] = 0x00;
  pci_conf[0x1c] = 0xf0;
  pci_conf[0x1f] = 0x02;
  pci_conf[0x20] = 0xf0;
  pci_conf[0x21] = 0xff;
  pci_conf[0x22] = 0x00;
  pci_conf[0x23] = 0x00;
  pci_conf[0x24] = 0xf0;
  pci_conf[0x25] = 0xff;
  pci_conf[0x26] = 0x00;
  pci_conf[0x27] = 0x00;
  pci_conf[0x3e] = 0x80;
}

void bx_pci_vbridge_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pci_vbridge", "PCI/AGP Bridge State");
  register_pci_state(list);
}

void bx_pci_vbridge_c::after_restore_state(void)
{
  // TODO
}

// pci configuration space write callback handler
void bx_pci_vbridge_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  BX_DEBUG_PCI_WRITE(address, value, io_len);

  for (unsigned i=0; i<io_len; i++) {
    Bit8u value8 = (value >> (i*8)) & 0xff;
    Bit8u oldval = pci_conf[address+i];
    switch (address+i) {
      case 0x04: // PCICMD1
        value8 &= 0x3f;
        break;
      case 0x05:
        value8 &= 0x01;
        break;
      case 0x0d: // MLT1
      case 0x1b: // SMLT
        value8 &= 0xf8;
        break;
      case 0x1c: // IOBASE
      case 0x1d: // IOLIMIT
      case 0x20: // MBASE lo
      case 0x22: // MLIMIT lo
      case 0x24: // PMBASE lo
      case 0x26: // PMLIMIT lo
        value8 &= 0xf0;
        break;
      case 0x1f: // SSTS hi
        value8 = (pci_conf[0x1f] & ~value8) | 0x02;
        break;
      case 0x3e: // BCTRL
        value8 = (value8 & 0xc1) | 0x80;
        break;
      case 0x19: // SBUSN - all bits r/w
      case 0x1a: // SUBUSN
      case 0x21: // MBASE hi
      case 0x23: // MLIMIT hi
      case 0x25: // PMBASE hi
      case 0x27: // PMLIMIT hi
        break;
      case 0x06: // PCISTS1 - all bits r/o
      case 0x07:
      case 0x18: // PBUSN
      case 0x1e: // SSTS lo
      default:
        value8 = oldval;
    }
    pci_conf[address+i] = value8;
  }
}
#endif /* BX_SUPPORT_PCI */
