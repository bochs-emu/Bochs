/////////////////////////////////////////////////////////////////////////
// $Id: pci2isa.cc,v 1.1 2002-09-16 19:18:58 vruppert Exp $
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




#include "bochs.h"
#define LOG_THIS bx_pci2isa.



//
// i440FX Support - PCI-to-ISA bridge (PIIX3)
//


bx_pci2isa_c bx_pci2isa;
#if BX_USE_P2I_SMF
#define this (&bx_pci2isa)
#endif


bx_pci2isa_c::bx_pci2isa_c(void)
{
  put("PCI");
  settype(PCILOG);
}

bx_pci2isa_c::~bx_pci2isa_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pci2isa_c::init(bx_devices_c *d)
{
  // called once when bochs initializes

  BX_P2I_THIS devices = d;

  if (bx_options.Oi440FXSupport->get ()) {
    BX_REGISTER_PCI_HANDLERS(this, pci_read_handler, pci_write_handler,
                             0x08, "440FX PCI-to-ISA bridge");

    for (unsigned i=0; i<256; i++)
      BX_P2I_THIS pci_conf[i] = 0x0;
    // readonly registers
    BX_P2I_THIS pci_conf[0x00] = 0x86;
    BX_P2I_THIS pci_conf[0x01] = 0x80;
    BX_P2I_THIS pci_conf[0x02] = 0x00;
    BX_P2I_THIS pci_conf[0x03] = 0x70;
    BX_P2I_THIS pci_conf[0x0a] = 0x01;
    BX_P2I_THIS pci_conf[0x0b] = 0x06;
    BX_P2I_THIS pci_conf[0x0e] = 0x80;
  }
}

  void
bx_pci2isa_c::reset(unsigned type)
{
  BX_P2I_THIS pci_conf[0x04] = 0x07;
  BX_P2I_THIS pci_conf[0x05] = 0x00;
  BX_P2I_THIS pci_conf[0x06] = 0x00;
  BX_P2I_THIS pci_conf[0x07] = 0x02;
  BX_P2I_THIS pci_conf[0x4c] = 0x4d;
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
    memcpy(&value, &BX_P2I_THIS pci_conf[address], io_len);
    BX_DEBUG(("440FX PCI-to-ISA read register 0x%02x value 0x%08x", address, value));
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
          BX_P2I_THIS pci_conf[address+i] = value8;
          BX_DEBUG(("440FX PCI-to-ISA write register 0x%02x value 0x%02x", address,
                    value8));
        }
      }
    }
}
