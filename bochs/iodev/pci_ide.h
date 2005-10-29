/////////////////////////////////////////////////////////////////////////
// $Id: pci_ide.h,v 1.7 2005-10-29 12:35:01 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004  MandrakeSoft S.A.
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


#if BX_USE_PIDE_SMF
#  define BX_PIDE_SMF  static
#  define BX_PIDE_THIS thePciIdeController->
#  define BX_PIDE_THIS_PTR thePciIdeController
#else
#  define BX_PIDE_SMF
#  define BX_PIDE_THIS this->
#  define BX_PIDE_THIS_PTR this
#endif


class bx_pci_ide_c : public bx_pci_ide_stub_c {

public:
  bx_pci_ide_c(void);
  ~bx_pci_ide_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);
  virtual bx_bool bmdma_present(void);
  virtual void   bmdma_set_irq(Bit8u channel);

  static void timer_handler(void *);
  BX_PIDE_SMF void timer(void);

private:

  struct {
    Bit8u pci_conf[256];
    Bit32u bmdma_addr;
    struct {
      bx_bool cmd_ssbm;
      bx_bool cmd_rwcon;
      Bit8u  status;
      Bit32u dtpr;
      Bit32u prd_current;
      int timer_index;
      Bit8u *buffer;
      Bit8u *buffer_top;
      Bit8u *buffer_idx;
    } bmdma[2];
  } s;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
  static Bit32u pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len);
  static void   pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len);
#if !BX_USE_PIDE_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
  Bit32u pci_read(Bit8u address, unsigned io_len);
  void   pci_write(Bit8u address, Bit32u value, unsigned io_len);
#endif
  };
