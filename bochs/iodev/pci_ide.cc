/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2014  The Bochs Project
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

// PCI IDE controller
// i430FX - PIIX
// i440FX - PIIX3

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI

#include "pci.h"
#include "pci_ide.h"

#define LOG_THIS thePciIdeController->

bx_pci_ide_c *thePciIdeController = NULL;

const Bit8u bmdma_iomask[16] = {1, 0, 1, 0, 4, 0, 0, 0, 1, 0, 1, 0, 4, 0, 0, 0};

int CDECL libpci_ide_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePciIdeController = new bx_pci_ide_c();
  bx_devices.pluginPciIdeController = thePciIdeController;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciIdeController, BX_PLUGIN_PCI_IDE);
  return(0); // Success
}

void CDECL libpci_ide_LTX_plugin_fini(void)
{
  delete thePciIdeController;
}

bx_pci_ide_c::bx_pci_ide_c()
{
  put("pci_ide", "PIDE");
  s.bmdma[0].timer_index = BX_NULL_TIMER_HANDLE;
  s.bmdma[1].timer_index = BX_NULL_TIMER_HANDLE;
  s.bmdma[0].buffer = NULL;
  s.bmdma[1].buffer = NULL;
}

bx_pci_ide_c::~bx_pci_ide_c()
{
  if (s.bmdma[0].buffer != NULL) {
    delete [] s.bmdma[0].buffer;
  }
  if (s.bmdma[1].buffer != NULL) {
    delete [] s.bmdma[1].buffer;
  }
  SIM->get_bochs_root()->remove("pci_ide");
  BX_DEBUG(("Exit"));
}

void bx_pci_ide_c::init(void)
{
  unsigned i;

  Bit8u devfunc = BX_PCI_DEVICE(1,1);
  DEV_register_pci_handlers(this, &devfunc,
      BX_PLUGIN_PCI_IDE, "PIIX3 PCI IDE controller");

  // register BM-DMA timer
  for (i=0; i<2; i++) {
    if (BX_PIDE_THIS s.bmdma[i].timer_index == BX_NULL_TIMER_HANDLE) {
      BX_PIDE_THIS s.bmdma[i].timer_index =
        DEV_register_timer(this, timer_handler, 1000, 0,0, "PIIX3 BM-DMA timer");
        bx_pc_system.setTimerParam(BX_PIDE_THIS s.bmdma[i].timer_index, i);
    }
  }

  BX_PIDE_THIS s.bmdma[0].buffer = new Bit8u[0x20000];
  BX_PIDE_THIS s.bmdma[1].buffer = new Bit8u[0x20000];

  BX_PIDE_THIS s.chipset = SIM->get_param_enum(BXPN_PCI_CHIPSET)->get();
  // initialize readonly registers
  if (BX_PIDE_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
    init_pci_conf(0x8086, 0x7010, 0x00, 0x010180, 0x00);
  } else {
    init_pci_conf(0x8086, 0x1230, 0x00, 0x010180, 0x00);
  }
  BX_PIDE_THIS pci_conf[0x20] = 0x01;
  BX_PIDE_THIS pci_base_address[4] = 0;
}

void bx_pci_ide_c::reset(unsigned type)
{
  BX_PIDE_THIS pci_conf[0x04] = 0x01;
  BX_PIDE_THIS pci_conf[0x06] = 0x80;
  BX_PIDE_THIS pci_conf[0x07] = 0x02;
  if (SIM->get_param_bool(BXPN_ATA0_ENABLED)->get()) {
    BX_PIDE_THIS pci_conf[0x40] = 0x00;
    BX_PIDE_THIS pci_conf[0x41] = 0x80;
  }
  if (SIM->get_param_bool(BXPN_ATA1_ENABLED)->get()) {
    BX_PIDE_THIS pci_conf[0x42] = 0x00;
    BX_PIDE_THIS pci_conf[0x43] = 0x80;
  }
  BX_PIDE_THIS pci_conf[0x44] = 0x00;
  for (unsigned i=0; i<2; i++) {
    BX_PIDE_THIS s.bmdma[i].cmd_ssbm = 0;
    BX_PIDE_THIS s.bmdma[i].cmd_rwcon = 0;
    BX_PIDE_THIS s.bmdma[i].status = 0;
    BX_PIDE_THIS s.bmdma[i].dtpr = 0;
    BX_PIDE_THIS s.bmdma[i].prd_current = 0;
    BX_PIDE_THIS s.bmdma[i].buffer_top = BX_PIDE_THIS s.bmdma[i].buffer;
    BX_PIDE_THIS s.bmdma[i].buffer_idx = BX_PIDE_THIS s.bmdma[i].buffer;
    BX_PIDE_THIS s.bmdma[i].data_ready = 0;
  }
}

// save/restore code begin
void bx_pci_ide_c::register_state(void)
{
  char name[6];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pci_ide", "PCI IDE Controller State");

  register_pci_state(list);

  new bx_shadow_data_c(list, "buffer0", BX_PIDE_THIS s.bmdma[0].buffer, 0x20000);
  new bx_shadow_data_c(list, "buffer1", BX_PIDE_THIS s.bmdma[1].buffer, 0x20000);

  for (unsigned i=0; i<2; i++) {
    sprintf(name, "%d", i);
    bx_list_c *ctrl = new bx_list_c(list, name);
    BXRS_PARAM_BOOL(ctrl, cmd_ssbm, BX_PIDE_THIS s.bmdma[i].cmd_ssbm);
    BXRS_PARAM_BOOL(ctrl, cmd_rwcon, BX_PIDE_THIS s.bmdma[i].cmd_rwcon);
    BXRS_HEX_PARAM_FIELD(ctrl, status, BX_PIDE_THIS s.bmdma[i].status);
    BXRS_HEX_PARAM_FIELD(ctrl, dtpr, BX_PIDE_THIS s.bmdma[i].dtpr);
    BXRS_HEX_PARAM_FIELD(ctrl, prd_current, BX_PIDE_THIS s.bmdma[i].prd_current);
    BXRS_PARAM_SPECIAL32(ctrl, buffer_top,
       BX_PIDE_THIS param_save_handler, BX_PIDE_THIS param_restore_handler);
    BXRS_PARAM_SPECIAL32(ctrl, buffer_idx,
       BX_PIDE_THIS param_save_handler, BX_PIDE_THIS param_restore_handler);
    BXRS_PARAM_BOOL(ctrl, data_ready, BX_PIDE_THIS s.bmdma[i].data_ready);
  }
}

void bx_pci_ide_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_PIDE_THIS_PTR, read_handler, write_handler,
                          &BX_PIDE_THIS pci_base_address[4], &BX_PIDE_THIS pci_conf[0x20],
                          16, &bmdma_iomask[0], "PIIX3 PCI IDE controller"))
  {
    BX_INFO(("new BM-DMA address: 0x%04x", BX_PIDE_THIS pci_base_address[4]));
  }
}

Bit64s bx_pci_ide_c::param_save_handler(void *devptr, bx_param_c *param)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) devptr;
  return class_ptr->param_save(param, val);
}

Bit64s bx_pci_ide_c::param_save(bx_param_c *param)
{
#else
  UNUSED(devptr);
#endif // !BX_USE_PIDE_SMF
  int chan = atoi(param->get_parent()->get_name());
  Bit64s val = 0;
  if (!strcmp(param->get_name(), "buffer_top")) {
    val = (Bit32u)(BX_PIDE_THIS s.bmdma[chan].buffer_top - BX_PIDE_THIS s.bmdma[chan].buffer);
  } else if (!strcmp(param->get_name(), "buffer_idx")) {
    val = (Bit32u)(BX_PIDE_THIS s.bmdma[chan].buffer_idx - BX_PIDE_THIS s.bmdma[chan].buffer);
  }
  return val;
}

void bx_pci_ide_c::param_restore_handler(void *devptr, bx_param_c *param, Bit64s val)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) devptr;
  class_ptr->param_restore(param, val);
}

void bx_pci_ide_c::param_restore(bx_param_c *param, Bit64s val)
{
#else
  UNUSED(devptr);
#endif // !BX_USE_PIDE_SMF
  int chan = atoi(param->get_parent()->get_name());
  if (!strcmp(param->get_name(), "buffer_top")) {
    BX_PIDE_THIS s.bmdma[chan].buffer_top = BX_PIDE_THIS s.bmdma[chan].buffer + val;
  } else if (!strcmp(param->get_name(), "buffer_idx")) {
    BX_PIDE_THIS s.bmdma[chan].buffer_idx = BX_PIDE_THIS s.bmdma[chan].buffer + val;
  }
}
// save/restore code end

bx_bool bx_pci_ide_c::bmdma_present(void)
{
  return (BX_PIDE_THIS pci_base_address[4] > 0);
}

void bx_pci_ide_c::bmdma_start_transfer(Bit8u channel)
{
  if (channel < 2) {
    BX_PIDE_THIS s.bmdma[channel].data_ready = 1;
  }
}

void bx_pci_ide_c::bmdma_set_irq(Bit8u channel)
{
  if (channel < 2) {
    BX_PIDE_THIS s.bmdma[channel].status |= 0x04;
  }
}

void bx_pci_ide_c::timer_handler(void *this_ptr)
{
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;
  class_ptr->timer();
}

void bx_pci_ide_c::timer()
{
  int count;
  Bit32u size, sector_size;
  struct {
    Bit32u addr;
    Bit32u size;
  } prd;

  Bit8u channel = bx_pc_system.triggeredTimerParam();
  if (((BX_PIDE_THIS s.bmdma[channel].status & 0x01) == 0) ||
      (BX_PIDE_THIS s.bmdma[channel].prd_current == 0)) {
    return;
  }
  if (BX_PIDE_THIS s.bmdma[channel].cmd_rwcon &&
      !BX_PIDE_THIS s.bmdma[channel].data_ready) {
    bx_pc_system.activate_timer(BX_PIDE_THIS s.bmdma[channel].timer_index, 1000, 0);
    return;
  }
  DEV_MEM_READ_PHYSICAL(BX_PIDE_THIS s.bmdma[channel].prd_current, 4, (Bit8u *)&prd.addr);
  DEV_MEM_READ_PHYSICAL(BX_PIDE_THIS s.bmdma[channel].prd_current+4, 4, (Bit8u *)&prd.size);
  size = prd.size & 0xfffe;
  if (size == 0) {
    size = 0x10000;
  }
  if (BX_PIDE_THIS s.bmdma[channel].cmd_rwcon) {
    BX_DEBUG(("READ DMA to addr=0x%08x, size=0x%08x", prd.addr, size));
    count = size - (BX_PIDE_THIS s.bmdma[channel].buffer_top - BX_PIDE_THIS s.bmdma[channel].buffer_idx);
    while (count > 0) {
      sector_size = count;
      if (DEV_hd_bmdma_read_sector(channel, BX_PIDE_THIS s.bmdma[channel].buffer_top, &sector_size)) {
        BX_PIDE_THIS s.bmdma[channel].buffer_top += sector_size;
        count -= sector_size;
      } else {
        break;
      }
    };
    if (count > 0) {
      BX_PIDE_THIS s.bmdma[channel].status &= ~0x01;
      BX_PIDE_THIS s.bmdma[channel].status |= 0x06;
      return;
    } else {
      DEV_MEM_WRITE_PHYSICAL_DMA(prd.addr, size, BX_PIDE_THIS s.bmdma[channel].buffer_idx);
      BX_PIDE_THIS s.bmdma[channel].buffer_idx += size;
    }
  } else {
    BX_DEBUG(("WRITE DMA from addr=0x%08x, size=0x%08x", prd.addr, size));
    DEV_MEM_READ_PHYSICAL_DMA(prd.addr, size, BX_PIDE_THIS s.bmdma[channel].buffer_top);
    BX_PIDE_THIS s.bmdma[channel].buffer_top += size;
    count = BX_PIDE_THIS s.bmdma[channel].buffer_top - BX_PIDE_THIS s.bmdma[channel].buffer_idx;
    while (count > 511) {
      if (DEV_hd_bmdma_write_sector(channel, BX_PIDE_THIS s.bmdma[channel].buffer_idx)) {
        BX_PIDE_THIS s.bmdma[channel].buffer_idx += 512;
        count -= 512;
      } else {
        break;
      }
    };
    if (count > 511) {
      BX_PIDE_THIS s.bmdma[channel].status &= ~0x01;
      BX_PIDE_THIS s.bmdma[channel].status |= 0x06;
      return;
    }
  }
  if (prd.size & 0x80000000) {
    BX_PIDE_THIS s.bmdma[channel].status &= ~0x01;
    BX_PIDE_THIS s.bmdma[channel].status |= 0x04;
    BX_PIDE_THIS s.bmdma[channel].prd_current = 0;
    DEV_hd_bmdma_complete(channel);
  } else {
    // To avoid buffer overflow reset buffer pointers and copy data if necessary
    count = BX_PIDE_THIS s.bmdma[channel].buffer_top - BX_PIDE_THIS s.bmdma[channel].buffer_idx;
    if (count > 0) {
      memcpy(BX_PIDE_THIS s.bmdma[channel].buffer, BX_PIDE_THIS s.bmdma[channel].buffer_idx, count);
    }
    BX_PIDE_THIS s.bmdma[channel].buffer_top = BX_PIDE_THIS s.bmdma[channel].buffer + count;
    BX_PIDE_THIS s.bmdma[channel].buffer_idx = BX_PIDE_THIS s.bmdma[channel].buffer;
    // Prepare for next PRD
    BX_PIDE_THIS s.bmdma[channel].prd_current += 8;
    DEV_MEM_READ_PHYSICAL(BX_PIDE_THIS s.bmdma[channel].prd_current, 4, (Bit8u *)&prd.addr);
    DEV_MEM_READ_PHYSICAL(BX_PIDE_THIS s.bmdma[channel].prd_current+4, 4, (Bit8u *)&prd.size);
    size = prd.size & 0xfffe;
    if (size == 0) {
      size = 0x10000;
    }
    bx_pc_system.activate_timer(BX_PIDE_THIS s.bmdma[channel].timer_index, (size >> 4) | 0x10, 0);
  }
}


// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_pci_ide_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_pci_ide_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF
  Bit8u offset, channel;
  Bit32u value = 0xffffffff;

  offset = address - BX_PIDE_THIS pci_base_address[4];
  channel = (offset >> 3);
  offset &= 0x07;
  switch (offset) {
    case 0x00:
      value = BX_PIDE_THIS s.bmdma[channel].cmd_ssbm |
              (BX_PIDE_THIS s.bmdma[channel].cmd_rwcon << 3);
      BX_DEBUG(("BM-DMA read command register, channel %d, value = 0x%02x", channel, value));
      break;
    case 0x02:
      value = BX_PIDE_THIS s.bmdma[channel].status;
      BX_DEBUG(("BM-DMA read status register, channel %d, value = 0x%02x", channel, value));
      break;
    case 0x04:
      value = BX_PIDE_THIS s.bmdma[channel].dtpr;
      BX_DEBUG(("BM-DMA read DTP register, channel %d, value = 0x%04x", channel, value));
      break;
  }

  return value;
}


// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_pci_ide_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

void bx_pci_ide_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF
  Bit8u offset, channel;

  offset = address - BX_PIDE_THIS pci_base_address[4];
  channel = (offset >> 3);
  offset &= 0x07;
  switch (offset) {
    case 0x00:
      BX_DEBUG(("BM-DMA write command register, channel %d, value = 0x%02x", channel, value));
      BX_PIDE_THIS s.bmdma[channel].cmd_rwcon = (value >> 3) & 1;
      if ((value & 0x01) && !BX_PIDE_THIS s.bmdma[channel].cmd_ssbm) {
        BX_PIDE_THIS s.bmdma[channel].cmd_ssbm = 1;
        BX_PIDE_THIS s.bmdma[channel].status |= 0x01;
        BX_PIDE_THIS s.bmdma[channel].prd_current = BX_PIDE_THIS s.bmdma[channel].dtpr;
        BX_PIDE_THIS s.bmdma[channel].buffer_top = BX_PIDE_THIS s.bmdma[channel].buffer;
        BX_PIDE_THIS s.bmdma[channel].buffer_idx = BX_PIDE_THIS s.bmdma[channel].buffer;
        BX_PIDE_THIS s.bmdma[channel].data_ready = 0;
        bx_pc_system.activate_timer(BX_PIDE_THIS s.bmdma[channel].timer_index, 1000, 0);
      } else if (!(value & 0x01) && BX_PIDE_THIS s.bmdma[channel].cmd_ssbm) {
        BX_PIDE_THIS s.bmdma[channel].cmd_ssbm = 0;
        BX_PIDE_THIS s.bmdma[channel].status &= ~0x01;
      }
      break;
    case 0x02:
      BX_PIDE_THIS s.bmdma[channel].status = (value & 0x60)
        | (BX_PIDE_THIS s.bmdma[channel].status & 0x01)
        | (BX_PIDE_THIS s.bmdma[channel].status & (~value & 0x06));
      BX_DEBUG(("BM-DMA write status register, channel %d, value = 0x%02x", channel, value));
      break;
    case 0x04:
      BX_PIDE_THIS s.bmdma[channel].dtpr = value & 0xfffffffc;
      BX_DEBUG(("BM-DMA write DTP register, channel %d, value = 0x%04x", channel, value));
      break;
  }
}


// pci configuration space read callback handler
Bit32u bx_pci_ide_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_PIDE_THIS pci_conf[address+i] << (i*8));
  }
  BX_DEBUG(("PIIX3 PCI IDE read  register 0x%02x value 0x%08x", address, value));
  return value;
}

// pci configuration space write callback handler
void bx_pci_ide_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool bmdma_change = 0;

  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x23) && (address < 0x40)))
    return;
  for (unsigned i=0; i<io_len; i++) {
    oldval = BX_PIDE_THIS pci_conf[address+i];
    value8 = (value >> (i*8)) & 0xFF;
    switch (address+i) {
      case 0x05:
      case 0x06:
        break;
      case 0x04:
        BX_PIDE_THIS pci_conf[address+i] = value8 & 0x05;
        break;
      case 0x20:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x21:
      case 0x22:
      case 0x23:
        bmdma_change |= (value8 != oldval);
      default:
        BX_PIDE_THIS pci_conf[address+i] = value8;
        BX_DEBUG(("PIIX3 PCI IDE write register 0x%02x value 0x%02x", address+i,
                  value8));
    }
  }
  if (bmdma_change) {
    if (DEV_pci_set_base_io(BX_PIDE_THIS_PTR, read_handler, write_handler,
                            &BX_PIDE_THIS pci_base_address[4], &BX_PIDE_THIS pci_conf[0x20],
                            16, &bmdma_iomask[0], "PIIX3 PCI IDE controller")) {
      BX_INFO(("new BM-DMA address: 0x%04x", BX_PIDE_THIS pci_base_address[4]));
    }
  }
}

#endif /* BX_SUPPORT_PCI */
