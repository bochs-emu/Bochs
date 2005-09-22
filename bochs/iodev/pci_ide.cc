/////////////////////////////////////////////////////////////////////////
// $Id: pci_ide.cc,v 1.13 2005-09-22 21:12:26 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

//
// i440FX Support - PCI IDE controller (PIIX3)
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI

#define LOG_THIS thePciIdeController->

bx_pci_ide_c *thePciIdeController = NULL;

const Bit8u bmdma_iomask[16] = {1, 0, 1, 0, 4, 0, 0, 0, 1, 0, 1, 0, 4, 0, 0, 0};

  int
libpci_ide_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePciIdeController = new bx_pci_ide_c ();
  bx_devices.pluginPciIdeController = thePciIdeController;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePciIdeController, BX_PLUGIN_PCI_IDE);
  return(0); // Success
}

  void
libpci_ide_LTX_plugin_fini(void)
{
}

bx_pci_ide_c::bx_pci_ide_c(void)
{
  put("PIDE");
  settype(PCIIDELOG);
  s.bmdma[0].timer_index = BX_NULL_TIMER_HANDLE;
  s.bmdma[1].timer_index = BX_NULL_TIMER_HANDLE;
}

bx_pci_ide_c::~bx_pci_ide_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pci_ide_c::init(void)
{
  unsigned i;

  Bit8u devfunc = BX_PCI_DEVICE(1,1);
  DEV_register_pci_handlers(this, pci_read_handler, pci_write_handler,
                            &devfunc, BX_PLUGIN_PCI_IDE, "PIIX3 PCI IDE controller");

  // register BM-DMA timer
  for (i=0; i<2; i++) {
    if (BX_PIDE_THIS s.bmdma[i].timer_index == BX_NULL_TIMER_HANDLE) {
      BX_PIDE_THIS s.bmdma[i].timer_index =
        DEV_register_timer(this, timer_handler, 1000, 0,0, "PIIX3 BM-DMA timer");
    }
  }

  for (i=0; i<256; i++)
    BX_PIDE_THIS s.pci_conf[i] = 0x0;
  // readonly registers
  BX_PIDE_THIS s.pci_conf[0x00] = 0x86;
  BX_PIDE_THIS s.pci_conf[0x01] = 0x80;
  BX_PIDE_THIS s.pci_conf[0x02] = 0x10;
  BX_PIDE_THIS s.pci_conf[0x03] = 0x70;
  BX_PIDE_THIS s.pci_conf[0x09] = 0x80;
  BX_PIDE_THIS s.pci_conf[0x0a] = 0x01;
  BX_PIDE_THIS s.pci_conf[0x0b] = 0x01;
  BX_PIDE_THIS s.pci_conf[0x0e] = 0x00;
  BX_PIDE_THIS s.pci_conf[0x20] = 0x01;
  BX_PIDE_THIS s.bmdma_addr = 0;
}

  void
bx_pci_ide_c::reset(unsigned type)
{
  BX_PIDE_THIS s.pci_conf[0x04] = 0x01;
  BX_PIDE_THIS s.pci_conf[0x05] = 0x00;
  BX_PIDE_THIS s.pci_conf[0x06] = 0x80;
  BX_PIDE_THIS s.pci_conf[0x07] = 0x02;
  if (bx_options.ata[0].Opresent->get ()) {
    BX_PIDE_THIS s.pci_conf[0x40] = 0x00;
    BX_PIDE_THIS s.pci_conf[0x41] = 0x80;
  }
  if (bx_options.ata[1].Opresent->get ()) {
    BX_PIDE_THIS s.pci_conf[0x42] = 0x00;
    BX_PIDE_THIS s.pci_conf[0x43] = 0x80;
  }
  BX_PIDE_THIS s.pci_conf[0x44] = 0x00;
  for (unsigned i=0; i<2; i++) {
    BX_PIDE_THIS s.bmdma[i].cmd_ssbm = 0;
    BX_PIDE_THIS s.bmdma[i].cmd_rwcon = 0;
    BX_PIDE_THIS s.bmdma[i].status = 0;
    BX_PIDE_THIS s.bmdma[i].dtpr = 0;
    BX_PIDE_THIS s.bmdma[i].prd_current = 0;
  }
}

  bx_bool
bx_pci_ide_c::bmdma_present(void)
{
//  return (BX_PIDE_THIS s.bmdma_addr > 0);
  return 0; // For now
}

  void
bx_pci_ide_c::bmdma_set_irq(Bit8u channel)
{
  if (channel < 2) {
    BX_PIDE_THIS s.bmdma[channel].status |= 0x04;
  }
}

  void
bx_pci_ide_c::timer_handler(void *this_ptr)
{
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  class_ptr->timer();
}

  void
bx_pci_ide_c::timer()
{
  int timer_id, count;
  Bit8u channel;
  Bit32u size;
  Bit8u buffer[0x10000];
  Bit8u *bufptr;
  struct {
    Bit32u addr;
    Bit32u size;
  } prd;

  timer_id = bx_pc_system.triggeredTimerID();
  if (timer_id == BX_PIDE_THIS s.bmdma[0].timer_index) {
    channel = 0;
  } else {
    channel = 1;
  }
  if (((BX_PIDE_THIS s.bmdma[channel].status & 0x01) == 0) ||
      (BX_PIDE_THIS s.bmdma[channel].prd_current == 0)) {
    return;
  }
  BX_MEM_READ_PHYSICAL(BX_PIDE_THIS s.bmdma[channel].prd_current, 8, (Bit8u *)&prd);
  size = prd.size & 0xfffe;
  if (size == 0) {
    size = 0x10000;
  }
  count = size;
  bufptr = &buffer[0];
  if (BX_PIDE_THIS s.bmdma[channel].cmd_rwcon) {
    BX_INFO(("READ DMA to addr=0x%08x, size=0x%08x", prd.addr, size));
    do {
      if (DEV_hd_bmdma_read_sector(channel, bufptr)) {
        bufptr += 512;
        count -= 512;
      } else {
        break;
      }
    } while (count > 0);
    if (count > 0) {
      BX_PIDE_THIS s.bmdma[channel].status &= ~0x01;
      BX_PIDE_THIS s.bmdma[channel].status |= 0x06;
      return;
    } else {
      BX_MEM_WRITE_PHYSICAL(prd.addr, size, &buffer);
    }
  } else {
    BX_INFO(("WRITE DMA from addr=0x%08x, size=0x%08x", prd.addr, size));
    BX_MEM_READ_PHYSICAL(prd.addr, size, &buffer);
    do {
      if (DEV_hd_bmdma_write_sector(channel, bufptr)) {
        bufptr += 512;
        count -= 512;
      } else {
        break;
      }
    } while (count > 0);
    if (count > 0) {
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
    BX_PIDE_THIS s.bmdma[channel].prd_current += 8;
    bx_pc_system.activate_timer( BX_PIDE_THIS s.bmdma[channel].timer_index, 1000, 0 );
  }
}


  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci_ide_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pci_ide_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF
  Bit8u offset, channel;
  Bit32u value = 0xffffffff;

  offset = address - BX_PIDE_THIS s.bmdma_addr;
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

  void
bx_pci_ide_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pci_ide_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF
  Bit8u offset, channel;

  offset = address - BX_PIDE_THIS s.bmdma_addr;
  channel = (offset >> 3);
  offset &= 0x07;
  switch (offset) {
    case 0x00:
      BX_INFO(("BM-DMA write command register, channel %d, value = 0x%02x", channel, value));
      BX_PIDE_THIS s.bmdma[channel].cmd_rwcon = (value >> 3) & 1;
      if ((value & 0x01) && !BX_PIDE_THIS s.bmdma[channel].cmd_ssbm) {
        BX_PIDE_THIS s.bmdma[channel].cmd_ssbm = 1;
        BX_PIDE_THIS s.bmdma[channel].status |= 0x01;
        BX_PIDE_THIS s.bmdma[channel].prd_current = BX_PIDE_THIS s.bmdma[channel].dtpr;
        bx_pc_system.activate_timer( BX_PIDE_THIS s.bmdma[channel].timer_index, 1000, 0 );
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
      BX_INFO(("BM-DMA write DTP register, channel %d, value = 0x%04x", channel, value));
      break;
  }
}


  // static pci configuration space read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci_ide_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  return( class_ptr->pci_read(address, io_len) );
}


  Bit32u
bx_pci_ide_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF

  Bit32u value = 0;

  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value |= (BX_PIDE_THIS s.pci_conf[address+i] << (i*8));
    }
    BX_DEBUG(("PIIX3 PCI IDE read register 0x%02x value 0x%08x", address, value));
    return value;
  } else {
    return(0xffffffff);
  }
}


  // static pci configuration space write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pci_ide_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PIDE_SMF
  bx_pci_ide_c *class_ptr = (bx_pci_ide_c *) this_ptr;

  class_ptr->pci_write(address, value, io_len);
}

  void
bx_pci_ide_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PIDE_SMF

  Bit8u value8, oldval;
  bx_bool bmdma_change = 0;

  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x21) && (address < 0x40)))
    return;
  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      oldval = BX_PIDE_THIS s.pci_conf[address+i];
      value8 = (value >> (i*8)) & 0xFF;
      switch (address+i) {
        case 0x05:
        case 0x06:
          break;
        case 0x04:
          BX_PIDE_THIS s.pci_conf[address+i] = value8 & 0x05;
          break;
        case 0x20:
          value8 = (value8 & 0xfc) | 0x01;
        case 0x21:
          bmdma_change |= (value8 != oldval);
        default:
          BX_PIDE_THIS s.pci_conf[address+i] = value8;
          BX_DEBUG(("PIIX3 PCI IDE write register 0x%02x value 0x%02x", address+i,
                    value8));
      }
    }
    if (bmdma_change) {
      DEV_pci_set_base_io(BX_PIDE_THIS_PTR, read_handler, write_handler,
                          &BX_PIDE_THIS s.bmdma_addr, &BX_PIDE_THIS s.pci_conf[0x20],
                          16, &bmdma_iomask[0], "PIIX3 PCI IDE controller");
      BX_INFO(("new BM-DMA address: 0x%04x", BX_PIDE_THIS s.bmdma_addr));
    }
  }
}

#endif /* BX_SUPPORT_PCI */
