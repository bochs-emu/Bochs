/////////////////////////////////////////////////////////////////////////
// $Id: pci2isa.cc,v 1.6 2002-11-14 20:02:58 vruppert Exp $
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
// i440FX Support - PCI-to-ISA bridge (PIIX3)
//

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#define LOG_THIS thePci2IsaBridge->

bx_pci2isa_c *thePci2IsaBridge = NULL;

  int
libpci2isa_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  thePci2IsaBridge = new bx_pci2isa_c ();
  bx_devices.pluginPci2IsaBridge = thePci2IsaBridge;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePci2IsaBridge, BX_PLUGIN_PCI2ISA);
  return(0); // Success
}

  void
libpci2isa_LTX_plugin_fini(void)
{
}

bx_pci2isa_c::bx_pci2isa_c(void)
{
  put("P2I");
  settype(PCI2ISALOG);
}

bx_pci2isa_c::~bx_pci2isa_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pci2isa_c::init(void)
{
  // called once when bochs initializes

  DEV_register_pci_handlers(this, pci_read_handler, pci_write_handler,
                            0x08, "PIIX3 PCI-to-ISA bridge");

  DEV_register_iowrite_handler(this, write_handler, 0x00B2, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x00B3, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x00F0, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x04D0, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x04D1, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_iowrite_handler(this, write_handler, 0x0CF9, "PIIX3 PCI-to-ISA bridge", 7);

  DEV_register_ioread_handler(this, read_handler, 0x00B2, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_ioread_handler(this, read_handler, 0x00B3, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_ioread_handler(this, read_handler, 0x04D0, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_ioread_handler(this, read_handler, 0x04D1, "PIIX3 PCI-to-ISA bridge", 7);
  DEV_register_ioread_handler(this, read_handler, 0x0CF9, "PIIX3 PCI-to-ISA bridge", 7);

  for (unsigned i=0; i<256; i++)
    BX_P2I_THIS s.pci_conf[i] = 0x0;
  // readonly registers
  BX_P2I_THIS s.pci_conf[0x00] = 0x86;
  BX_P2I_THIS s.pci_conf[0x01] = 0x80;
  BX_P2I_THIS s.pci_conf[0x02] = 0x00;
  BX_P2I_THIS s.pci_conf[0x03] = 0x70;
  BX_P2I_THIS s.pci_conf[0x0a] = 0x01;
  BX_P2I_THIS s.pci_conf[0x0b] = 0x06;
  BX_P2I_THIS s.pci_conf[0x0e] = 0x80;
}

  void
bx_pci2isa_c::reset(unsigned type)
{
  BX_P2I_THIS s.pci_conf[0x04] = 0x07;
  BX_P2I_THIS s.pci_conf[0x05] = 0x00;
  BX_P2I_THIS s.pci_conf[0x06] = 0x00;
  BX_P2I_THIS s.pci_conf[0x07] = 0x02;
  BX_P2I_THIS s.pci_conf[0x4c] = 0x4d;
  BX_P2I_THIS s.pci_conf[0x4e] = 0x03;
  BX_P2I_THIS s.pci_conf[0x4f] = 0x00;
  BX_P2I_THIS s.pci_conf[0x60] = 0x80;
  BX_P2I_THIS s.pci_conf[0x69] = 0x02;
  BX_P2I_THIS s.pci_conf[0x70] = 0x80;
  BX_P2I_THIS s.pci_conf[0x76] = 0x0c;
  BX_P2I_THIS s.pci_conf[0x77] = 0x0c;
  BX_P2I_THIS s.pci_conf[0x78] = 0x02;
  BX_P2I_THIS s.pci_conf[0x79] = 0x00;
  BX_P2I_THIS s.pci_conf[0x80] = 0x00;
  BX_P2I_THIS s.pci_conf[0x82] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa0] = 0x08;
  BX_P2I_THIS s.pci_conf[0xa0] = 0x08;
  BX_P2I_THIS s.pci_conf[0xa2] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa3] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa4] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa5] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa6] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa7] = 0x00;
  BX_P2I_THIS s.pci_conf[0xa8] = 0x0f;
  BX_P2I_THIS s.pci_conf[0xaa] = 0x00;
  BX_P2I_THIS s.pci_conf[0xab] = 0x00;
  BX_P2I_THIS s.pci_conf[0xac] = 0x00;
  BX_P2I_THIS s.pci_conf[0xae] = 0x00;

  BX_P2I_THIS s.elcr1 = 0x00;
  BX_P2I_THIS s.elcr2 = 0x00;
}



  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci2isa_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_pci2isa_c *class_ptr = (bx_pci2isa_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pci2isa_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF
  if (io_len > 1)
    BX_PANIC(("io read from port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  switch (address) {
    case 0x00b2:
      BX_ERROR(("read: APM command register not supported yet"));
      break;
    case 0x00b3:
      BX_ERROR(("read: APM status register not supported yet"));
      break;
    case 0x04d0:
      return(BX_P2I_THIS s.elcr1);
      break;
    case 0x04d1:
      return(BX_P2I_THIS s.elcr2);
      break;
    case 0x0cf9:
      BX_ERROR(("read: CPU reset register not supported yet"));
      break;
    }

  return(0xffffffff);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pci2isa_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_pci2isa_c *class_ptr = (bx_pci2isa_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pci2isa_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF
  if (io_len > 1)
    BX_PANIC(("io write to port %04x, len=%u", (unsigned) address,
             (unsigned) io_len));

  switch (address) {
    case 0x00b2:
      BX_ERROR(("write: APM command register not supported yet"));
      break;
    case 0x00b3:
      BX_ERROR(("write: APM status register not supported yet"));
      break;
    case 0x00f0:
      BX_ERROR(("write: FPU error register not supported yet"));
      break;
    case 0x04d0:
      BX_P2I_THIS s.elcr1 = (value & 0xf8);
      BX_ERROR(("write: ELCR1 changes have no effect yet"));
      break;
    case 0x04d1:
      BX_P2I_THIS s.elcr2 = (value & 0xde);
      BX_ERROR(("write: ELCR2 changes have no effect yet"));
      break;
    case 0x0cf9:
      BX_ERROR(("write: CPU reset register not supported yet"));
      break;
    }
}


  // static pci configuration space read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci2isa_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_pci2isa_c *class_ptr = (bx_pci2isa_c *) this_ptr;

  return( class_ptr->pci_read(address, io_len) );
}


  Bit32u
bx_pci2isa_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF

  Bit32u value = 0;

  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value |= (BX_P2I_THIS s.pci_conf[address+i] << (i*8));
    }
    BX_DEBUG(("PIIX3 PCI-to-ISA read register 0x%02x value 0x%08x", address, value));
    return value;
    }
  else
    return(0xffffffff);
}


  // static pci configuration space write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pci2isa_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_pci2isa_c *class_ptr = (bx_pci2isa_c *) this_ptr;

  class_ptr->pci_write(address, value, io_len);
}

  void
bx_pci2isa_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF

  Bit8u value8;

  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value8 = (value >> (i*8)) & 0xFF;
      switch (address+i) {
        case 0x06:
          break;
        default:
          BX_P2I_THIS s.pci_conf[address+i] = value8;
          BX_DEBUG(("PIIX3 PCI-to-ISA write register 0x%02x value 0x%02x", address,
                    value8));
        }
      }
    }
}
