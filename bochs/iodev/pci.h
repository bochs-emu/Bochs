/////////////////////////////////////////////////////////////////////////
// $Id: pci.h,v 1.13 2002-11-09 20:51:40 vruppert Exp $
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


#define BX_MAX_PCI_DEVICES 20

typedef Bit32u (*bx_pci_read_handler_t)(void *, Bit8u, unsigned);
typedef void   (*bx_pci_write_handler_t)(void *, Bit8u, Bit32u, unsigned);

#if BX_USE_PCI_SMF
#  define BX_PCI_SMF  static
#  define BX_PCI_THIS thePciBridge->
#else
#  define BX_PCI_SMF
#  define BX_PCI_THIS this->
#endif


typedef struct {
  Bit32u confAddr;
  Bit32u confData;
  Bit8u  pci_conf[256];
  } bx_def440fx_t;



class bx_pci_c : public bx_pci_stub_c {

public:
  bx_pci_c(void);
  ~bx_pci_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);
  virtual bx_bool register_pci_handlers(void *this_ptr,
                                        bx_pci_read_handler_t f1,
                                        bx_pci_write_handler_t f2,
                                        Bit8u devfunc, const char *name);
  virtual void   print_i440fx_state(void);
  virtual Bit8u rd_memType (Bit32u addr);
  virtual Bit8u wr_memType (Bit32u addr);

private:
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
  };
