/////////////////////////////////////////////////////////////////////////
// $Id: acpi.h,v 1.2 2006-09-26 18:43:42 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2006  Volker Ruppert
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

#ifndef BX_IODEV_ACPI_H
#define BX_IODEV_ACPI_H

#if BX_USE_ACPI_SMF
#  define BX_ACPI_THIS theACPIController->
#  define BX_ACPI_THIS_PTR theACPIController
#else
#  define BX_ACPI_THIS this->
#  define BX_ACPI_THIS_PTR this
#endif

class bx_acpi_ctrl_c : public bx_devmodel_c, public bx_pci_device_stub_c {
public:
  bx_acpi_ctrl_c();
  virtual ~bx_acpi_ctrl_c();
  virtual void init(void);
  virtual void reset(unsigned type);
#if BX_SUPPORT_SAVE_RESTORE
  virtual void register_state(void);
  virtual void after_restore_state(void);
#endif

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_ACPI_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif

private:
  static void set_irq_level(bx_bool level);
  struct {
    Bit8u pci_conf[256];
    Bit8u devfunc;
    Bit32u pm_base;
    Bit32u sm_base;
    Bit16u pmsts;
    Bit16u pmen;
    Bit16u pmcntrl;
  } s;
};

#endif
