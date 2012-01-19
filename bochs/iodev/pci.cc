/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2009  The Bochs Project
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
// i440FX Support - PMC/DBX
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI

#include "pci.h"

#define LOG_THIS thePciBridge->

bx_pci_bridge_c *thePciBridge = NULL;

int libpci_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    thePciBridge = new bx_pci_bridge_c();
    bx_devices.pluginPciBridge = thePciBridge;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciBridge, BX_PLUGIN_PCI);
    return 0; // Success
  } else {
    return -1;
  }
}

void libpci_LTX_plugin_fini(void)
{
  delete thePciBridge;
}

bx_pci_bridge_c::bx_pci_bridge_c()
{
  put("PCI");
}

bx_pci_bridge_c::~bx_pci_bridge_c()
{
  debug_dump();
  BX_DEBUG(("Exit"));
}

void bx_pci_bridge_c::init(void)
{
  // called once when bochs initializes
  unsigned i;
  BX_PCI_THIS num_pci_handlers = 0;

  /* set unused elements to appropriate values */
  for (i=0; i < BX_MAX_PCI_DEVICES; i++) {
    BX_PCI_THIS pci_handler[i].handler = NULL;
  }

  for (i=0; i < 0x100; i++) {
    BX_PCI_THIS pci_handler_id[i] = BX_MAX_PCI_DEVICES;  // not assigned
  }

  for (i=0; i < BX_N_PCI_SLOTS; i++) {
    BX_PCI_THIS slot_used[i] = 0;  // no device connected
  }
  BX_PCI_THIS slots_checked = 0;

  // confAddr accepts dword i/o only
  DEV_register_ioread_handler(this, read_handler, 0x0CF8, "i440FX", 4);
  DEV_register_iowrite_handler(this, write_handler, 0x0CF8, "i440FX", 4);

  for (i=0x0CFC; i<=0x0CFF; i++) {
    DEV_register_ioread_handler(this, read_handler, i, "i440FX", 7);
    DEV_register_iowrite_handler(this, write_handler, i, "i440FX", 7);
  }

  Bit8u devfunc = BX_PCI_DEVICE(0,0);
  DEV_register_pci_handlers(this, &devfunc, BX_PLUGIN_PCI, "440FX Host bridge");

  for (i=0; i<256; i++)
    BX_PCI_THIS pci_conf[i] = 0x0;
  // readonly registers
  BX_PCI_THIS pci_conf[0x00] = 0x86;
  BX_PCI_THIS pci_conf[0x01] = 0x80;
  BX_PCI_THIS pci_conf[0x02] = 0x37;
  BX_PCI_THIS pci_conf[0x03] = 0x12;
  BX_PCI_THIS pci_conf[0x0b] = 0x06;
}

  void
bx_pci_bridge_c::reset(unsigned type)
{
  unsigned i;
  char devname[80];
  char *device;

  if (!BX_PCI_THIS slots_checked) {
    for (i=0; i<BX_N_PCI_SLOTS; i++) {
      sprintf(devname, "pci.slot.%d", i+1);
      device = SIM->get_param_string(devname)->getptr();
      if ((strlen(device) > 0) && !BX_PCI_THIS slot_used[i]) {
        BX_PANIC(("Unknown plugin '%s' at PCI slot #%d", device, i+1));
      }
    }
    BX_PCI_THIS slots_checked = 1;
  }

  BX_PCI_THIS confAddr = 0;
  BX_PCI_THIS confData = 0;

  BX_PCI_THIS pci_conf[0x04] = 0x06;
  BX_PCI_THIS pci_conf[0x05] = 0x00;
  BX_PCI_THIS pci_conf[0x06] = 0x80;
  BX_PCI_THIS pci_conf[0x07] = 0x02;
  BX_PCI_THIS pci_conf[0x0d] = 0x00;
  BX_PCI_THIS pci_conf[0x0f] = 0x00;
  BX_PCI_THIS pci_conf[0x50] = 0x00;
  BX_PCI_THIS pci_conf[0x51] = 0x01;
  BX_PCI_THIS pci_conf[0x52] = 0x00;
  BX_PCI_THIS pci_conf[0x53] = 0x80;
  BX_PCI_THIS pci_conf[0x54] = 0x00;
  BX_PCI_THIS pci_conf[0x55] = 0x00;
  BX_PCI_THIS pci_conf[0x56] = 0x00;
  BX_PCI_THIS pci_conf[0x57] = 0x01;
  BX_PCI_THIS pci_conf[0x58] = 0x10;
  for (i=0x59; i<0x60; i++)
    BX_PCI_THIS pci_conf[i] = 0x00;
  BX_PCI_THIS pci_conf[0x72] = 0x02;
}

void bx_pci_bridge_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pci_bridge", "PCI Bridge State", 3);
  BXRS_HEX_PARAM_FIELD(list, confAddr, BX_PCI_THIS confAddr);
  BXRS_HEX_PARAM_FIELD(list, confData, BX_PCI_THIS confData);
  register_pci_state(list);
}

void bx_pci_bridge_c::after_restore_state(void)
{
  BX_PCI_THIS smram_control(BX_PCI_THIS pci_conf[0x72]);
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_pci_bridge_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_bridge_c *class_ptr = (bx_pci_bridge_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_pci_bridge_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  switch (address) {
    case 0x0CF8:
      return BX_PCI_THIS confAddr;
    case 0x0CFC:
    case 0x0CFD:
    case 0x0CFE:
    case 0x0CFF:
    {
      Bit32u handle, retval;
      Bit8u devfunc, regnum;

      if ((BX_PCI_THIS confAddr & 0x80FF0000) == 0x80000000) {
        devfunc = (BX_PCI_THIS confAddr >> 8) & 0xff;
        regnum = (BX_PCI_THIS confAddr & 0xfc) + (address & 0x03);
        handle = BX_PCI_THIS pci_handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES))
          retval = BX_PCI_THIS pci_handler[handle].handler->pci_read_handler(regnum, io_len);
        else
          retval = 0xFFFFFFFF;
      }
      else
        retval = 0xFFFFFFFF;
      BX_PCI_THIS confData = retval;
      return retval;
    }
  }

  BX_PANIC(("unsupported IO read to port 0x%x", (unsigned) address));
  return(0xffffffff);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_pci_bridge_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_bridge_c *class_ptr = (bx_pci_bridge_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_pci_bridge_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  switch (address) {
    case 0xCF8:
      BX_PCI_THIS confAddr = value;
      if ((value & 0x80FFFF00) == 0x80000000) {
        BX_DEBUG(("440FX PMC register 0x%02x selected", value & 0xfc));
      } else if ((value & 0x80000000) == 0x80000000) {
        BX_DEBUG(("440FX request for bus 0x%02x device 0x%02x function 0x%02x",
                  (value >> 16) & 0xFF, (value >> 11) & 0x1F, (value >> 8) & 0x07));
      }
      break;

    case 0xCFC:
    case 0xCFD:
    case 0xCFE:
    case 0xCFF:
      if ((BX_PCI_THIS confAddr & 0x80FF0000) == 0x80000000) {
        Bit8u devfunc = (BX_PCI_THIS confAddr >> 8) & 0xff;
        Bit8u regnum = (BX_PCI_THIS confAddr & 0xfc) + (address & 0x03);
        Bit32u handle = BX_PCI_THIS pci_handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES)) {
          if (((regnum>=4) && (regnum<=7)) || (regnum==12) || (regnum==13) || (regnum>14)) {
            BX_PCI_THIS pci_handler[handle].handler->pci_write_handler(regnum, value, io_len);
            BX_PCI_THIS confData = value << (8 * (address & 0x03));
          }
          else
            BX_DEBUG(("read only register, write ignored"));
        }
      }
      break;

    default:
      BX_PANIC(("IO write to port 0x%x", (unsigned) address));
  }
}

// pci configuration space read callback handler
Bit32u bx_pci_bridge_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_PCI_THIS pci_conf[address+i] << (i*8));
  }
  BX_DEBUG(("440FX PMC read  register 0x%02x value 0x%08x", address, value));
  return value;
}

// pci configuration space write callback handler
void bx_pci_bridge_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  unsigned area;

  if ((address >= 0x10) && (address < 0x34))
    return;
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_PCI_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        BX_PCI_THIS pci_conf[address+i] = (value8 & 0x40) | 0x06;
        break;
      case 0x06:
      case 0x0c:
        break;
      case 0x59:
      case 0x5A:
      case 0x5B:
      case 0x5C:
      case 0x5D:
      case 0x5E:
      case 0x5F:
        if (value != oldval) {
          BX_PCI_THIS pci_conf[address+i] = value8;
          if ((address+i) == 0x59) {
            area = BX_MEM_AREA_F0000;
            DEV_mem_set_memory_type(area, 0, (value >> 4) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value >> 5) & 0x1);
          } else {
            area = ((address+i) - 0x5a) << 1;
            DEV_mem_set_memory_type(area, 0, (value >> 0) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value >> 1) & 0x1);
            area++;
            DEV_mem_set_memory_type(area, 0, (value >> 4) & 0x1);
            DEV_mem_set_memory_type(area, 1, (value >> 5) & 0x1);
          }
          BX_INFO(("440FX PMC write to PAM register %x (TLB Flush)", address+i));
          bx_pc_system.MemoryMappingChanged();
        }
        break;
      case 0x72:
        smram_control(value);  // SMRAM conrol register
        break;
      default:
        BX_PCI_THIS pci_conf[address+i] = value8;
        BX_DEBUG(("440FX PMC write register 0x%02x value 0x%02x", address+i, value8));
    }
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

void bx_pci_bridge_c::debug_dump()
{
  int i;

  BX_DEBUG(("i440fxConfAddr:0x%08x", BX_PCI_THIS confAddr));
  BX_DEBUG(("i440fxConfData:0x%08x", BX_PCI_THIS confData));

#ifdef DUMP_FULL_I440FX
  for (i=0; i<256; i++) {
    BX_DEBUG(("i440fxArray%02x:0x%02x", i, BX_PCI_THIS pci_conf[i]));
  }
#else /* DUMP_FULL_I440FX */
  for (i=0x59; i<0x60; i++) {
    BX_DEBUG(("i440fxArray%02x:0x%02x", i, BX_PCI_THIS pci_conf[i]));
  }
#endif /* DUMP_FULL_I440FX */
}

bx_bool bx_pci_bridge_c::register_pci_handlers(bx_pci_device_stub_c *dev,
                                        Bit8u *devfunc, const char *name,
                                        const char *descr)
{
  unsigned i, handle;
  char devname[80];
  char *device;

  if (strcmp(name, "pci") && strcmp(name, "pci2isa") && strcmp(name, "pci_ide")
      && (*devfunc == 0x00)) {
    for (i = 0; i < BX_N_PCI_SLOTS; i++) {
      sprintf(devname, "pci.slot.%d", i+1);
      device = SIM->get_param_string(devname)->getptr();
      if ((strlen(device) > 0) && (!strcmp(name, device))) {
        *devfunc = (i + 2) << 3;
        BX_PCI_THIS slot_used[i] = 1;
        BX_INFO(("PCI slot #%d used by plugin '%s'", i+1, name));
        break;
      }
    }
    if (*devfunc == 0x00) {
      BX_ERROR(("Plugin '%s' not connected to a PCI slot", name));
    }
  }
  /* check if device/function is available */
  if (BX_PCI_THIS pci_handler_id[*devfunc] == BX_MAX_PCI_DEVICES) {
    if (BX_PCI_THIS num_pci_handlers >= BX_MAX_PCI_DEVICES) {
      BX_INFO(("too many PCI devices installed."));
      BX_PANIC(("  try increasing BX_MAX_PCI_DEVICES"));
      return false;
    }
    handle = BX_PCI_THIS num_pci_handlers++;
    BX_PCI_THIS pci_handler[handle].handler = dev;
    BX_PCI_THIS pci_handler_id[*devfunc] = handle;
    BX_INFO(("%s present at device %d, function %d", descr, *devfunc >> 3,
             *devfunc & 0x07));
    return true; // device/function mapped successfully
  }
  else {
    return false; // device/function not available, return false.
  }
}

bx_bool bx_pci_bridge_c::is_pci_device(const char *name)
{
  unsigned i;
  char devname[80];
  char *device;

  for (i = 0; i < BX_N_PCI_SLOTS; i++) {
    sprintf(devname, "pci.slot.%d", i+1);
    device = SIM->get_param_string(devname)->getptr();
    if ((strlen(device) > 0) && (!strcmp(name, device))) {
      return 1;
    }
  }
  return 0;
}

bx_bool bx_pci_bridge_c::pci_set_base_mem(void *this_ptr, memory_handler_t f1, memory_handler_t f2,
                           Bit32u *addr, Bit8u *pci_conf, unsigned size)
{
  Bit32u newbase;

  Bit32u oldbase = *addr;
  Bit32u mask = ~(size - 1);
  Bit8u pci_flags = pci_conf[0x00] & 0x0f;
  if ((pci_flags & 0x06) > 0) {
    BX_PANIC(("PCI base memory flag 0x%02x not supported", pci_flags));
    return 0;
  }
  pci_conf[0x00] &= (mask & 0xf0);
  pci_conf[0x01] &= (mask >> 8) & 0xff;
  pci_conf[0x02] &= (mask >> 16) & 0xff;
  pci_conf[0x03] &= (mask >> 24) & 0xff;
  ReadHostDWordFromLittleEndian(pci_conf, newbase);
  pci_conf[0x00] |= pci_flags;
  if (newbase != mask && newbase != oldbase) { // skip PCI probe
    if (oldbase > 0) {
      DEV_unregister_memory_handlers(this_ptr, oldbase, oldbase + size - 1);
    }
    if (newbase > 0) {
      DEV_register_memory_handlers(this_ptr, f1, f2, newbase, newbase + size - 1);
    }
    *addr = newbase;
    return 1;
  }
  return 0;
}

bx_bool bx_pci_bridge_c::pci_set_base_io(void *this_ptr, bx_read_handler_t f1, bx_write_handler_t f2,
                          Bit32u *addr, Bit8u *pci_conf, unsigned size,
                          const Bit8u *iomask, const char *name)
{
  unsigned i;
  Bit32u newbase;

  Bit32u oldbase = *addr;
  Bit16u mask = ~(size - 1);
  Bit8u pci_flags = pci_conf[0x00] & 0x03;
  pci_conf[0x00] &= (mask & 0xfc);
  pci_conf[0x01] &= (mask >> 8);
  ReadHostDWordFromLittleEndian(pci_conf, newbase);
  pci_conf[0x00] |= pci_flags;
  if (((newbase & 0xfffc) != mask) && (newbase != oldbase)) { // skip PCI probe
    if (oldbase > 0) {
      for (i=0; i<size; i++) {
        if (iomask[i] > 0) {
          DEV_unregister_ioread_handler(this_ptr, f1, oldbase + i, iomask[i]);
          DEV_unregister_iowrite_handler(this_ptr, f2, oldbase + i, iomask[i]);
        }
      }
    }
    if (newbase > 0) {
      for (i=0; i<size; i++) {
        if (iomask[i] > 0) {
          DEV_register_ioread_handler(this_ptr, f1, newbase + i, name, iomask[i]);
          DEV_register_iowrite_handler(this_ptr, f2, newbase + i, name, iomask[i]);
        }
      }
    }
    *addr = newbase;
    return 1;
  }
  return 0;
}

#endif /* BX_SUPPORT_PCI */
