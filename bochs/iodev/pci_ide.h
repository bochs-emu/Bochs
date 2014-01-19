/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2014  The Bochs Project
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

#ifndef BX_IODEV_PCIIDE_H
#define BX_IODEV_PCIIDE_H

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
  bx_pci_ide_c();
  virtual ~bx_pci_ide_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual bx_bool bmdma_present(void);
  virtual void bmdma_start_transfer(Bit8u channel);
  virtual void bmdma_set_irq(Bit8u channel);
  virtual void register_state(void);
  virtual void after_restore_state(void);
  static Bit64s param_save_handler(void *devptr, bx_param_c *param);
  static void param_restore_handler(void *devptr, bx_param_c *param, Bit64s val);
#if !BX_USE_PIDE_SMF
  Bit64s param_save(bx_param_c *param);
  void param_restore(bx_param_c *param, Bit64s val);
#endif

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static void timer_handler(void *);
  BX_PIDE_SMF void timer(void);

private:

  struct {
    unsigned chipset;
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
      bx_bool data_ready;
    } bmdma[2];
  } s;

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_PIDE_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#endif
