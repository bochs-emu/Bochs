/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2013  The Bochs Project
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

#ifndef BX_IODEV_PCI_BRIDGE_H
#define BX_IODEV_PCI_BRIDGE_H

#if BX_USE_PCI_SMF
#  define BX_PCI_SMF  static
#  define BX_PCI_THIS thePciBridge->
#else
#  define BX_PCI_SMF
#  define BX_PCI_THIS this->
#endif

#define BX_PCI_DEVICE(device, function) ((device)<<3 | (function))

#define BX_PCI_INTA 1
#define BX_PCI_INTB 2
#define BX_PCI_INTC 3
#define BX_PCI_INTD 4

class bx_pci_bridge_c : public bx_devmodel_c, public bx_pci_device_stub_c {
public:
  bx_pci_bridge_c();
  virtual ~bx_pci_bridge_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

private:
  void smram_control(Bit8u value);

  unsigned chipset;
  Bit8u DRBA[8];
  Bit8u dram_detect;
};
#endif
