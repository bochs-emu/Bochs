/////////////////////////////////////////////////////////////////////////
// $Id: pci_ide.cc,v 1.6 2004-07-11 20:38:48 vruppert Exp $
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
#if BX_PCI_SUPPORT

#define LOG_THIS thePciIdeController->

bx_pci_ide_c *thePciIdeController = NULL;

const Bit8u bmide_iomask[16] = {1, 0, 1, 0, 4, 0, 0, 0, 1, 0, 1, 0, 4, 0, 0, 0};

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
}

bx_pci_ide_c::~bx_pci_ide_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pci_ide_c::init(void)
{
  // called once when bochs initializes

  Bit8u devfunc = BX_PCI_DEVICE(1,1);
  DEV_register_pci_handlers(this, pci_read_handler, pci_write_handler,
                            &devfunc, BX_PLUGIN_PCI_IDE, "PIIX3 PCI IDE controller");

  for (unsigned i=0; i<256; i++)
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
  // This should be done by the PCI BIOS
  WriteHostDWordToLittleEndian(&BX_PIDE_THIS s.pci_conf[0x20], 0x0000);
  DEV_pci_set_base_io(this, read_handler, write_handler,
                      &BX_PIDE_THIS s.bmide_addr, &BX_PIDE_THIS s.pci_conf[0x20],
                      16, &bmide_iomask[0], "PIIX3 PCI IDE controller");
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
  Bit8u offset;

  offset = address - BX_PIDE_THIS s.bmide_addr;
  BX_INFO(("BM-IDE read register 0x%08x len %d", offset, io_len));
  return BX_PIDE_THIS s.bmide_regs[offset];
/*  switch (address) {
    }

  return(0xffffffff);*/
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
  Bit8u offset;

  offset = address - BX_PIDE_THIS s.bmide_addr;
  BX_INFO(("BM-IDE write register 0x%08x len %d", offset, io_len));
  BX_PIDE_THIS s.bmide_regs[offset] = (Bit8u)value;
/*  switch (address) {
    }*/
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
    }
  else
    return(0xffffffff);
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
  bx_bool bmide_change = 0;

  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      oldval = BX_PIDE_THIS s.pci_conf[address+i];
      value8 = (value >> (i*8)) & 0xFF;
      switch (address+i) {
        case 0x05:
        case 0x06:
        case 0x22:
        case 0x23:
          break;
        case 0x04:
          BX_PIDE_THIS s.pci_conf[address+i] = value8 & 0x05;
          break;
        case 0x20:
        case 0x21:
          bmide_change |= (value8 != oldval);
        default:
          BX_PIDE_THIS s.pci_conf[address+i] = value8;
          BX_DEBUG(("PIIX3 PCI IDE write register 0x%02x value 0x%02x", address,
                    value8));
      }
    }
    if (bmide_change) {
      DEV_pci_set_base_io(BX_PIDE_THIS_PTR, read_handler, write_handler,
                          &BX_PIDE_THIS s.bmide_addr, &BX_PIDE_THIS s.pci_conf[0x20],
                          16, &bmide_iomask[0], "PIIX3 PCI IDE controller");
      BX_INFO(("new BM-IDE address: 0x%04x", BX_PIDE_THIS s.bmide_addr));
    }
  }
}

#endif /* BX_PCI_SUPPORT */
