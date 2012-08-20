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

#ifndef BX_IODEV_PCI_BRIDGE_H
#define BX_IODEV_PCI_BRIDGE_H

#define BX_MAX_PCI_DEVICES 20

#define BX_PCI_DEVICE(device, function) ((device)<<3 | (function))

#if BX_USE_PCI_SMF
#  define BX_PCI_SMF  static
#  define BX_PCI_THIS thePciBridge->
#else
#  define BX_PCI_SMF
#  define BX_PCI_THIS this->
#endif

#define BX_PCI_INTA 1
#define BX_PCI_INTB 2
#define BX_PCI_INTC 3
#define BX_PCI_INTD 4

class bx_pci_device_stub_c;

class bx_pci_bridge_c : public bx_pci_bridge_stub_c {
public:
  bx_pci_bridge_c();
  virtual ~bx_pci_bridge_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  virtual bx_bool register_pci_handlers(bx_pci_device_stub_c *device,
                                        Bit8u *devfunc, const char *name,
                                        const char *descr);
  virtual bx_bool is_pci_device(const char *name);
  virtual bx_bool pci_set_base_mem(void *this_ptr, memory_handler_t f1,
                                   memory_handler_t f2, Bit32u *addr,
                                   Bit8u *pci_conf, unsigned size);
  virtual bx_bool pci_set_base_io(void *this_ptr, bx_read_handler_t f1,
                                  bx_write_handler_t f2, Bit32u *addr,
                                  Bit8u *pci_conf, unsigned size,
                                  const Bit8u *iomask, const char *name);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);
#if BX_DEBUGGER
  virtual void debug_dump(int argc, char **argv);
#endif

private:
  Bit8u pci_handler_id[0x100];  // 256 devices/functions
  struct {
    bx_pci_device_stub_c *handler;
  } pci_handler[BX_MAX_PCI_DEVICES];
  unsigned num_pci_handlers;

  bx_bool slot_used[BX_N_PCI_SLOTS];
  bx_bool slots_checked;

  Bit32u confAddr;
  Bit32u confData;

  void smram_control(Bit8u value);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCI_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
