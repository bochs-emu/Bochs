/////////////////////////////////////////////////////////////////////////
// $Id: pci.cc,v 1.16 2002-08-05 17:43:25 vruppert Exp $
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
  unsigned i;

  put("PCI");
  settype(PCILOG);

  BX_PCI_THIS num_pci_handles = 0;

  /* set unused elements to appropriate values */
  for (i=0; i < BX_MAX_PCI_DEVICES; i++) {
    BX_PCI_THIS pci_handler[i].read  = NULL;
    BX_PCI_THIS pci_handler[i].write = NULL;
    }

  for (i=0; i < 0x100; i++) {
    BX_PCI_THIS pci_handler_id[i] = BX_MAX_PCI_DEVICES;  // not assigned
  }

  for (i=0; i<0x40000; i++) {
    BX_PCI_THIS s.i440fx.shadow[i] = 0xff;
  }
}

bx_pci_c::~bx_pci_c(void)
{
  // nothing for now
  BX_DEBUG(("Exit."));
}


  void
bx_pci_c::init(bx_devices_c *d)
{
  // called once when bochs initializes

  BX_PCI_THIS devices = d;

  if (bx_options.Oi440FXSupport->get ()) {
    d->register_io_read_handler(this, read_handler, 0x0CF8, "i440FX");
    for (unsigned i=0x0CFC; i<=0x0CFF; i++) {
      d->register_io_read_handler(this, read_handler, i, "i440FX");
      }

    d->register_io_write_handler(this, write_handler, 0x0CF8, "i440FX");
    for (unsigned i=0x0CFC; i<=0x0CFF; i++) {
      d->register_io_write_handler(this, write_handler, i, "i440FX");
      }

    BX_PCI_THIS register_pci_handlers(this, pci_read_handler, pci_write_handler,
                                      0x00, "440FX Host bridge");

    for (unsigned i=0; i<256; i++)
      BX_PCI_THIS s.i440fx.array[i] = 0x0;
    // readonly registers
    BX_PCI_THIS s.i440fx.array[0x00] = 0x86;
    BX_PCI_THIS s.i440fx.array[0x01] = 0x80;
    BX_PCI_THIS s.i440fx.array[0x02] = 0x37;
    BX_PCI_THIS s.i440fx.array[0x03] = 0x12;
    BX_PCI_THIS s.i440fx.array[0x0b] = 0x06;
  }
}

  void
bx_pci_c::reset(void)
{
  BX_PCI_THIS s.i440fx.confAddr = 0;
  BX_PCI_THIS s.i440fx.confData = 0;

  BX_PCI_THIS s.i440fx.array[0x04] = 0x06;
  BX_PCI_THIS s.i440fx.array[0x05] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x06] = 0x80;
  BX_PCI_THIS s.i440fx.array[0x07] = 0x02;
  BX_PCI_THIS s.i440fx.array[0x0d] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x0f] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x50] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x51] = 0x01;
  BX_PCI_THIS s.i440fx.array[0x52] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x53] = 0x80;
  BX_PCI_THIS s.i440fx.array[0x54] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x55] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x56] = 0x00;
  BX_PCI_THIS s.i440fx.array[0x57] = 0x01;
  BX_PCI_THIS s.i440fx.array[0x58] = 0x10;
  for (unsigned i=0x59; i<0x60; i++)
    BX_PCI_THIS s.i440fx.array[i] = 0x00;
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
    case 0x0CF8:
      {
      if (io_len == 4) {
        return BX_PCI_THIS s.i440fx.confAddr;
        }
      else {
        return 0xFFFF;
        }
      }
      break;
    case 0x0CFC:
    case 0x0CFD:
    case 0x0CFE:
    case 0x0CFF:
      {
      Bit32u handle, retval;
      Bit8u devfunc, regnum;

      if ((BX_PCI_THIS s.i440fx.confAddr & 0x80FF0000) == 0x80000000) {
        devfunc = (BX_PCI_THIS s.i440fx.confAddr >> 8) & 0xff;
        regnum = (BX_PCI_THIS s.i440fx.confAddr & 0xfc) + (address & 0x03);
        handle = BX_PCI_THIS pci_handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES))
          retval = (* BX_PCI_THIS pci_handler[handle].read)
                     (BX_PCI_THIS pci_handler[handle].this_ptr, regnum, io_len);
        else
          retval = 0xFFFFFFFF;
        }
      else
        retval = 0xFFFFFFFF;
      BX_PCI_THIS s.i440fx.confData = retval;
      return retval;
      }
    }

  BX_PANIC(("unsupported IO read to port 0x%x",
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
      {
      // confAddr accepts a dword value only
      if (io_len == 4) {
        BX_PCI_THIS s.i440fx.confAddr = value;
        if ((value & 0x80FFFF00) == 0x80000000) {
          BX_DEBUG(("440FX PMC register 0x%02x selected", value & 0xfc));
          }
        else if ((value & 0x80000000) == 0x80000000) {
          BX_DEBUG(("440FX request for bus 0x%02x device 0x%02x function 0x%02x",
                    (value >> 16) & 0xFF, (value >> 11) & 0x1F, (value >> 8) & 0x07));
          }
        }
      }
      break;

    case 0xCFC:
    case 0xCFD:
    case 0xCFE:
    case 0xCFF:
      {
      Bit32u handle;
      Bit8u devfunc, regnum;

      if ((BX_PCI_THIS s.i440fx.confAddr & 0x80FF0000) == 0x80000000) {
        devfunc = (BX_PCI_THIS s.i440fx.confAddr >> 8) & 0xff;
        regnum = (BX_PCI_THIS s.i440fx.confAddr & 0xfc) + (address & 0x03);
        handle = BX_PCI_THIS pci_handler_id[devfunc];
        if ((io_len <= 4) && (handle < BX_MAX_PCI_DEVICES)) {
          if (((regnum>=4) && (regnum<=7)) || (regnum==12) || (regnum==13) || (regnum>14)) {
            (* BX_PCI_THIS pci_handler[handle].write)
               (BX_PCI_THIS pci_handler[handle].this_ptr, regnum, value, io_len);
            BX_PCI_THIS s.i440fx.confData = value << (8 * (address & 0x03));
            }
          else
            BX_DEBUG(("read only register, write ignored"));
          }
        }
      }
      break;

    default:
      BX_PANIC(("IO write to port 0x%x", (unsigned) address));
    }
}


  // static pci configuration space read callback handler
  // redirects to non-static class handler to avoid virtual functions

  Bit32u
bx_pci_c::pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_c *class_ptr = (bx_pci_c *) this_ptr;

  return( class_ptr->pci_read(address, io_len) );
}


  Bit32u
bx_pci_c::pci_read(Bit8u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  Bit32u val440fx = 0;

  if (io_len <= 4) {
    memcpy(&val440fx, &BX_PCI_THIS s.i440fx.array[address], io_len);
    BX_DEBUG(("440FX PMC read register 0x%02x value 0x%08x", address, val440fx));
    return val440fx;
    }
  else
    return(0xffffffff);
}


  // static pci configuration space write callback handler
  // redirects to non-static class handler to avoid virtual functions

  void
bx_pci_c::pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_PCI_SMF
  bx_pci_c *class_ptr = (bx_pci_c *) this_ptr;

  class_ptr->pci_write(address, value, io_len);
}

  void
bx_pci_c::pci_write(Bit8u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_PCI_SMF

  Bit8u value8;

  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value8 = (value >> (i*8)) & 0xFF;
      switch (address+i) {
        case 0x06:
        case 0x0c:
          break;
        default:
          BX_PCI_THIS s.i440fx.array[address+i] = value8;
          BX_DEBUG(("440FX PMC write register 0x%02x value 0x%02x", address,
                    value8));
        }
      }
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
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5A] & 0x3));
      case 0xC4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5A] >> 4) & 0x3));
      case 0xC8:
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5B]  & 0x3));
      case 0xCC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5B] >> 4) & 0x3));


      case 0xD0:
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5C] & 0x3));
      case 0xD4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 4) & 0x3));
      case 0xD8:
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5D] & 0x3));
      case 0xDC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5D] >> 4) & 0x3));

      case 0xE0:
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5E] & 0x3));
      case 0xE4:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5E] >> 4) & 0x3));
      case 0xE8:
           return (mapRead ( BX_PCI_THIS s.i440fx.array[0x5F] & 0x3));
      case 0xEC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x5F] >> 4) & 0x3));

      case 0xF0:
      case 0xF4:
      case 0xF8:
      case 0xFC:
           return (mapRead ( (BX_PCI_THIS s.i440fx.array[0x59] >> 4) & 0x3));

      default:
           BX_PANIC(("rd_memType () Error: Memory Type not known !"));
           return(0); // keep compiler happy
           break;
   }

}

  Bit32u
bx_pci_c::wr_memType (Bit32u addr)
{
   switch ((addr & 0xFC000) >> 12) {
      case 0xC0:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5A] & 0x3));
      case 0xC4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5A] >> 4) & 0x3));
      case 0xC8:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5B] & 0x3));
      case 0xCC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5B] >> 4) & 0x3));


      case 0xD0:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5C] & 0x3));
      case 0xD4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5C] >> 4) & 0x3));
      case 0xD8:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5D] & 0x3));
      case 0xDC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5D] >> 4) & 0x3));

      case 0xE0:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5E] & 0x3));
      case 0xE4:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5E] >> 4) & 0x3));
      case 0xE8:
           return (mapWrite ( BX_PCI_THIS s.i440fx.array[0x5F] & 0x3));
      case 0xEC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x5F] >> 4) & 0x3));

      case 0xF0:
      case 0xF4:
      case 0xF8:
      case 0xFC:
           return (mapWrite ( (BX_PCI_THIS s.i440fx.array[0x59] >> 4) & 0x3));

      default:
           BX_PANIC(("wr_memType () Error: Memory Type not known !"));
           return(0); // keep compiler happy
           break;
   }
}

  void
bx_pci_c::print_i440fx_state()
{
  int  i;

  BX_DEBUG(( "i440fxConfAddr:0x%08x", BX_PCI_THIS s.i440fx.confAddr ));
  BX_DEBUG(( "i440fxConfData:0x%08x", BX_PCI_THIS s.i440fx.confData ));

#ifdef DUMP_FULL_I440FX
  for (i=0; i<256; i++) {
    BX_DEBUG(( "i440fxArray%02x:0x%02x", i, BX_PCI_THIS s.i440fx.array[i] ));
    }
#else /* DUMP_FULL_I440FX */
  for (i=0x59; i<0x60; i++) {
    BX_DEBUG(( "i440fxArray%02x:0x%02x", i, BX_PCI_THIS s.i440fx.array[i] ));
    }
#endif /* DUMP_FULL_I440FX */
}

  Bit8u*
bx_pci_c::i440fx_fetch_ptr(Bit32u addr)
{
  if (bx_options.Oi440FXSupport->get ()) {
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


  Boolean
bx_pci_c::register_pci_handlers( void *this_ptr, bx_pci_read_handler_t f1,
                                 bx_pci_write_handler_t f2, Bit8u devfunc,
                                 const char *name)
{
  unsigned handle;

  /* first check if device/function is available */
  if (BX_PCI_THIS pci_handler_id[devfunc] == BX_MAX_PCI_DEVICES) {
    if (BX_PCI_THIS num_pci_handles >= BX_MAX_PCI_DEVICES) {
      BX_INFO(("too many PCI devices installed."));
      BX_PANIC(("  try increasing BX_MAX_PCI_DEVICES"));
      return false;
      }
    handle = BX_PCI_THIS num_pci_handles++;
    BX_PCI_THIS pci_handler[handle].read  = f1;
    BX_PCI_THIS pci_handler[handle].write = f2;
    BX_PCI_THIS pci_handler[handle].this_ptr = this_ptr;
    BX_PCI_THIS pci_handler_id[devfunc] = handle;
    BX_INFO(("%s present at device %d, function %d", name, devfunc >> 3,
             devfunc & 0x07));
    return true; // device/function mapped successfully
    }
  else {
    return false; // device/function not available, return false.
    }
}
