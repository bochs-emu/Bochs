//  Copyright (C) 2001  MandrakeSoft S.A.
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
#define LOG_THIS bx_pci.



//
// i440FX Support
//


bx_pci_c bx_pci;
#if BX_USE_PCI_SMF
#define this (&bx_pci)
#endif


bx_pci_c::bx_pci_c(void)
{
	setprefix("PCI");
	settype(PCILOG);
}

bx_pci_c::~bx_pci_c(void)
{
	BX_INFO(("Exit."));
}


  void
bx_pci_c::init(bx_devices_c *d)
{
  // called once when bochs initializes

  BX_PCI_THIS devices = d;

  for (unsigned i=0x0CFC; i<=0x0CFF; i++) {
    d->register_io_read_handler(this, read_handler, i, "i440FX");
    }

  d->register_io_write_handler(this, write_handler, 0x0CF8, "i440FX");
  for (unsigned i=0x0CFC; i<=0x0CFF; i++) {
    d->register_io_write_handler(this, write_handler, i, "i440FX");
    }

  // should this go into ::reset() ???
  if (bx_options.i440FXSupport) {
    for (unsigned i=0; i<256; i++)
      BX_PCI_THIS s.i440fx.array[i] = 0x0;
    }
}

  void
bx_pci_c::reset(void)
{
  // upon RESET
}



  // static IO port read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_c *class_ptr = (bx_pci_c *) this_ptr;

  return( class_ptr->read(address, io_len) );
}


  Bit32u
bx_pci_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  switch (address) {
    case 0x0CFC:
    case 0x0CFD:
    case 0x0CFE:
    case 0x0CFF:
      {
      Bit32u idx440fx, val440fx, retMask;
      idx440fx = BX_PCI_THIS s.i440fx.confAddr & 0x00FC;
      val440fx = (BX_PCI_THIS s.i440fx.array[idx440fx] >> ((address & 0x3)*8) );

      switch (io_len) {
        case 1:
          retMask = 0xFF; break;
        case 2:
          retMask = 0xFFFF; break;
        case 4:
          retMask = 0xFFFFFFFF; break;
        default:
          retMask = 0xFFFFFFFF; break;
        }
      BX_INFO(("440FX IO read from port: %04x, len: %02x, data: %04x",
                address, io_len, (val440fx & retMask)));
      return (val440fx & retMask);
      }
    }

  BX_PANIC(("pci: unsupported IO read to port 0x%x",
           (unsigned) address));
  return(0xffffffff);
}


  // static IO port write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pci_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_c *class_ptr = (bx_pci_c *) this_ptr;

  class_ptr->write(address, value, io_len);
}

  void
bx_pci_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  switch (address) {
    case 0xCF8:
      BX_PCI_THIS s.i440fx.confAddr = value;
      BX_INFO(("440FX IO write to port %04x of %04x, len %02x ",
                address, value, io_len));
      break;

    case 0xCFC:
      {
      Bit32u dMask, idx;

      switch (io_len) {
          case 1:
            dMask = 0xFF; break;
          case 2:
            dMask = 0xFFFF; break;
          case 4:
            dMask = 0xFFFFFFFF; break;
          default:
            dMask = 0x0; break;
        }
      if (BX_PCI_THIS s.i440fx.confAddr & 0x80000000) {
        idx = (BX_PCI_THIS s.i440fx.confAddr & 0xFC);
        BX_PCI_THIS s.i440fx.array[idx] = (BX_PCI_THIS s.i440fx.array[idx] & ~dMask) | (value & dMask);
        BX_INFO(("440FX IO write to port %04x of %04x, len %02x ",
                  address, value, io_len));
        }
     }
     break;

    case 0xCFD:
      {
      Bit32u dMask, idx;

      switch (io_len) {
        case 1:
          dMask = 0xFF00; break;
        case 2:
          dMask = 0xFFFF00; break;
        default:
          dMask = 0x0; break;
        }
      if (BX_PCI_THIS s.i440fx.confAddr & 0x80000000) {
        idx = (BX_PCI_THIS s.i440fx.confAddr & 0xFC);
        BX_PCI_THIS s.i440fx.array[idx] = (BX_PCI_THIS s.i440fx.array[idx] & ~dMask) | ((value << 8) & dMask);
        BX_INFO(("440FX IO write to port %04x of %04x, len %02x ",
                  address, value, io_len));
        }
     }
     break;

    case 0xCFE:
      {
      Bit32u dMask, idx;

      switch (io_len) {
        case 1:
          dMask = 0xFF0000; break;
        case 2:
          dMask = 0xFFFF0000; break;
        default:
          dMask = 0x0; break;
        }
      if (BX_PCI_THIS s.i440fx.confAddr & 0x80000000) {
        idx = (BX_PCI_THIS s.i440fx.confAddr & 0xFC);
        BX_PCI_THIS s.i440fx.array[idx] = (BX_PCI_THIS s.i440fx.array[idx] & ~dMask) | ((value << 16) & dMask);
        BX_INFO(("440FX IO write to port %04x of %04x, len %02x ",
                  address, value, io_len));
        }
     }
     break;

    case 0xCFF:
      {
      Bit32u dMask, idx;

      switch (io_len) {
        case 1:
          dMask = 0xFF000000; break;
        default:
          dMask = 0x0; break;
        }
      if (BX_PCI_THIS s.i440fx.confAddr & 0x80000000) {
        idx = (BX_PCI_THIS s.i440fx.confAddr & 0xFC);
        BX_PCI_THIS s.i440fx.array[idx] = (BX_PCI_THIS s.i440fx.array[idx] & ~dMask) | ((value << 24) & dMask);
        BX_INFO(("440FX IO write to port %04x of %04x, len %02x ",
                  address, value, io_len));
        }
     }
     break;

    default:
      BX_PANIC(("pci: IO write to port 0x%x", (unsigned) address));
    }
}


  Bit32u
bx_pci_c::mapRead (Bit32u val)
{
   switch (val) {
     case 0x0:
     case 0x2:
          return (1);    // (0) Goto ROM

     case 0x1:
     case 0x3:
          return (0);    // (1) Goto Shadow
   }
   return (2);
}

  Bit32u
bx_pci_c::mapWrite (Bit32u val)
{
   switch (val) {
     case 0x0:
     case 0x1:
          return (1);    // (0) Goto ROM

     case 0x2:
     case 0x3:
          return (0);    // (1) Goto Shadow
   }
   return (2);
}

  Bit32u
bx_pci_c::rd_memType (Bit32u addr)
{
   switch ((addr & 0xFC000) >> 12) {
      case 0xC0:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x58] >> 16) & 0x3));
      case 0xC4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x58] >> 20) & 0x3));
      case 0xC8:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x58] >> 24) & 0x3));
      case 0xCC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x58] >> 28) & 0x3));


      case 0xD0:
           return (mapRead (BX_PCI_THIS s.i440fx.array[0x5C] & 0x3));
      case 0xD4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 4) & 0x3));
      case 0xD8:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 8) & 0x3));
      case 0xDC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 12) & 0x3));

      case 0xE0:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 16) & 0x3));
      case 0xE4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 20) & 0x3));
      case 0xE8:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 24) & 0x3));
      case 0xEC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 28) & 0x3));

      case 0xF0:
      case 0xF4:
      case 0xF8:
      case 0xFC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x58] >> 12) & 0x3));

      default:
           BX_PANIC(("wr_memType () Error: Memory Type not known !"));
           return(0); // keep compiler happy
           break;
   }

}

  Bit32u
bx_pci_c::wr_memType (Bit32u addr)
{
   switch ((addr & 0xFC000) >> 12) {
      case 0xC0:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x58] >> 16) & 0x3));
      case 0xC4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x58] >> 20) & 0x3));
      case 0xC8:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x58] >> 24) & 0x3));
      case 0xCC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x58] >> 28) & 0x3));


      case 0xD0:
           return (mapWrite (BX_PCI_THIS s.i440fx.array[0x5C] & 0x3));
      case 0xD4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 4) & 0x3));
      case 0xD8:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 8) & 0x3));
      case 0xDC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 12) & 0x3));

      case 0xE0:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 16) & 0x3));
      case 0xE4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 20) & 0x3));
      case 0xE8:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 24) & 0x3));
      case 0xEC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 28) & 0x3));

      case 0xF0:
      case 0xF4:
      case 0xF8:
      case 0xFC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x58] >> 12) & 0x3));

      default:
           BX_PANIC(("rd_memType () Error: Memory Type not known !"));
           return(0); // keep compiler happy
           break;
   }
}

  void
bx_pci_c::print_i440fx_state()
{
#ifdef DUMP_FULL_I440FX
  int  i;
#endif /* DUMP_FULL_I440FX */

  BX_INFO(( "i440fxConfAddr:0x%x", BX_PCI_THIS s.i440fx.confAddr ));
  BX_INFO(( "i440fxConfData:0x%x", BX_PCI_THIS s.i440fx.confData ));

#ifdef DUMP_FULL_I440FX
  for (i=0; i<256; i++) {
    BX_INFO(( "i440fxArray%02x:0x%x", i, BX_PCI_THIS s.i440fx.array[i] ));
    }
#else /* DUMP_FULL_I440FX */
  BX_INFO(( "i440fxArray58:0x%x", BX_PCI_THIS s.i440fx.array[0x58] ));
  BX_INFO(( "i440fxArray5c:0x%x", BX_PCI_THIS s.i440fx.array[0x5c] ));
#endif /* DUMP_FULL_I440FX */
}

  Bit8u*
bx_pci_c::i440fx_fetch_ptr(Bit32u addr)
{
  if (bx_options.i440FXSupport) {
    switch (rd_memType (addr)) {
      case 0x0:   // Read from ShadowRAM
        return (&BX_PCI_THIS devices->mem->vector[addr]);

      case 0x1:   // Read from ROM
        return (&BX_PCI_THIS s.i440fx.shadow[(addr - 0xc0000)]);
      default:
        BX_PANIC(("i440fx_fetch_ptr(): default case"));
        return(0);
      }
    }
  else
    return (&BX_PCI_THIS devices->mem->vector[addr]);
}
