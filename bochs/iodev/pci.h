/////////////////////////////////////////////////////////////////////////
// $Id: pci.h,v 1.8 2002-08-27 19:54:46 bdenney Exp $
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


#if BX_PCI_SUPPORT

#define BX_MAX_PCI_DEVICES 20

typedef Bit32u (*bx_pci_read_handler_t)(void *, Bit8u, unsigned);
typedef void   (*bx_pci_write_handler_t)(void *, Bit8u, Bit32u, unsigned);

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
  Bit8u  pci_conf[256];
  Bit8u  shadow[4*16*4096];     // 256k of memory
  } bx_def440fx_t;



class bx_pci_c : public logfunctions {

public:
  bx_pci_c(void);
  ~bx_pci_c(void);
  BX_PCI_SMF void   init(bx_devices_c *);
  BX_PCI_SMF void   reset(unsigned type);
  Boolean register_pci_handlers(void *this_ptr, bx_pci_read_handler_t f1,
                                bx_pci_write_handler_t f2, Bit8u devfunc,
                                const char *name);
  BX_PCI_SMF void   print_i440fx_state( );
  BX_PCI_SMF Bit32u rd_memType (Bit32u addr);
  BX_PCI_SMF Bit32u wr_memType (Bit32u addr);
  BX_PCI_SMF Bit8u* i440fx_fetch_ptr(Bit32u addr);
  BX_PCI_SMF int    load_ROM(int fd, Bit32u offset, Bit32u size);
  BX_PCI_SMF Bit8u  mem_read(Bit32u offset);

private:
  bx_devices_c *devices;

  Bit8u pci_handler_id[0x100];  // 256 devices/functions
  struct {
    bx_pci_read_handler_t  read;
    bx_pci_write_handler_t write;
    void             *this_ptr;
    } pci_handler[BX_MAX_PCI_DEVICES];
  unsigned num_pci_handles;

  struct {
    bx_def440fx_t i440fx;
    } s;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static Bit32u pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len);
  static void   pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCI_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
  Bit32u pci_read(Bit8u address, unsigned io_len);
  void   pci_write(Bit8u address, Bit32u value, unsigned io_len);
#endif
  BX_PCI_SMF Bit32u mapRead (Bit32u val);
  BX_PCI_SMF Bit32u mapWrite (Bit32u val);
  };

#if BX_USE_PCI_SMF
extern bx_pci_c bx_pci;
#endif

#endif  // #if BX_PCI_SUPPORT
