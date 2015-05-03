/////////////////////////////////////////////////////////////////////////
// $Id$
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

#ifndef BX_IODEV_ACPI_H
#define BX_IODEV_ACPI_H

#if BX_USE_ACPI_SMF
#  define BX_ACPI_SMF static
#  define BX_ACPI_THIS theACPIController->
#  define BX_ACPI_THIS_PTR theACPIController
#else
#  define BX_ACPI_SMF
#  define BX_ACPI_THIS this->
#  define BX_ACPI_THIS_PTR this
#endif

class bx_acpi_ctrl_c : public bx_acpi_ctrl_stub_c {
public:
  bx_acpi_ctrl_c();
  virtual ~bx_acpi_ctrl_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void generate_smi(Bit8u value);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  virtual Bit32u pci_read_handler(Bit8u address, unsigned io_len);
  virtual void   pci_write_handler(Bit8u address, Bit32u value, unsigned io_len);

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);
#if !BX_USE_ACPI_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
  BX_ACPI_SMF void timer(void);

private:
  BX_ACPI_SMF void set_irq_level(bx_bool level);
  BX_ACPI_SMF Bit32u get_pmtmr(void);
  BX_ACPI_SMF Bit16u get_pmsts(void);
  BX_ACPI_SMF void pm_update_sci(void);
  static void timer_handler(void *);

  struct {
    Bit8u devfunc;
    Bit32u pm_base;
    Bit32u sm_base;
    Bit16u pmsts;
    Bit16u pmen;
    Bit16u pmcntrl;
    Bit64u tmr_overflow_time;
    int timer_index;
    struct {
      Bit8u stat;
      Bit8u ctl;
      Bit8u cmd;
      Bit8u addr;
      Bit8u data0;
      Bit8u data1;
      Bit8u index;
      Bit8u data[32];
    } smbus;
  } s;
};

#endif
