/*  
 *  PCIDEV: PCI host device mapping
 *  Copyright (C) 2003 - Frank Cornelis
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#if BX_USE_PCIDEV_SMF
#  define BX_PCIDEV_THIS thePciDevAdapter->
#  define BX_PCIDEV_THIS_ thePciDevAdapter
#else
#  define BX_PCIDEV_THIS this->
#  define BX_PCIDEV_THIS_ this
#endif

#define PCIDEV_IRQ 11

  struct region_struct {
	unsigned long config_value;
	unsigned long start; // can change
	unsigned long end; // can change
	unsigned long host_start; // never changes!!!
	class bx_pcidev_c *pcidev;
  };

class bx_pcidev_c : public bx_devmodel_c
{
public:
  bx_pcidev_c(void);
  ~bx_pcidev_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);
  
  int pcidev_fd; // to access the pcidev

  // resource mapping
  struct region_struct regions[6];
  int irq;

private:

  static Bit32u pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len);
  static void   pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len);
  
  static Bit32u read_handler(void *param, Bit32u address, unsigned io_len);
  static void write_handler(void *param, Bit32u address, Bit32u value, unsigned io_len);

#if !BX_USE_PCIDEV_SMF
  Bit32u pci_read(Bit8u address, unsigned io_len);
  void   pci_write(Bit8u address, Bit32u value, unsigned io_len);
  
  Bit32u read(void *param, Bit32u address, unsigned io_len);
  void write(void *param, Bit32u address, Bit32u value, unsigned io_len);
#endif
};
