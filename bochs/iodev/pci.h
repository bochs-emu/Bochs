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


#if BX_PCI_SUPPORT

#if BX_USE_PCI_SMF
#  define BX_PCI_SMF  static
#  define BX_PCI_THIS bx_pci.
#else
#  define BX_PCI_SMF
#  define BX_PCI_THIS this->
#endif


typedef struct {
  Bit32u confAddr;
  Bit32u confData;
  Bit32u array[256];
  Bit8u  shadow[4*16*4096];     // 256k of memory
  } bx_def440fx_t;



class bx_pci_c {

public:
  bx_pci_c(void);
  ~bx_pci_c(void);
  BX_PCI_SMF void   init(bx_devices_c *);
  BX_PCI_SMF void   reset(void);
  BX_PCI_SMF void   print_i440fx_state( FILE *fd );
  BX_PCI_SMF Bit32u rd_memType (Bit32u addr);
  BX_PCI_SMF Bit32u wr_memType (Bit32u addr);
  BX_PCI_SMF Bit8u* i440fx_fetch_ptr(Bit32u addr);

  struct {
    bx_def440fx_t i440fx;
    } s;

private:
  bx_devices_c *devices;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCI_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_PCI_SMF Bit32u mapRead (Bit32u val);
  BX_PCI_SMF Bit32u mapWrite (Bit32u val);
  };

#if BX_USE_PCI_SMF
extern bx_pci_c bx_pci;
#endif

#endif  // #if BX_PCI_SUPPORT
