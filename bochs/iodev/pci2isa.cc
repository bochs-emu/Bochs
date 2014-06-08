/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002-2014  The Bochs Project
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

// PCI-to-ISA bridge
// i430FX - PIIX
// i440FX - PIIX3

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI

#include "pci.h"
#include "pci2isa.h"

#define LOG_THIS thePci2IsaBridge->

bx_piix3_c *thePci2IsaBridge = NULL;

int CDECL libpci2isa_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  if (type == PLUGTYPE_CORE) {
    thePci2IsaBridge = new bx_piix3_c();
    bx_devices.pluginPci2IsaBridge = thePci2IsaBridge;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, thePci2IsaBridge, BX_PLUGIN_PCI2ISA);
    return 0; // Success
  } else {
    return -1;
  }
}

void CDECL libpci2isa_LTX_plugin_fini(void)
{
  delete thePci2IsaBridge;
}

bx_piix3_c::bx_piix3_c()
{
  put("pci2isa", "P2ISA");
}

bx_piix3_c::~bx_piix3_c()
{
  SIM->get_bochs_root()->remove("pci2isa");
  BX_DEBUG(("Exit"));
}

void bx_piix3_c::init(void)
{
  unsigned i;
  // called once when bochs initializes

  Bit8u devfunc = BX_PCI_DEVICE(1,0);
  DEV_register_pci_handlers(this, &devfunc, BX_PLUGIN_PCI2ISA,
      "PIIX3 PCI-to-ISA bridge");
  BX_P2I_THIS s.chipset = SIM->get_param_enum(BXPN_PCI_CHIPSET)->get();

  DEV_register_iowrite_handler(this, write_handler, 0x00B2, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x00B3, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x04D0, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x04D1, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_iowrite_handler(this, write_handler, 0x0CF9, "PIIX3 PCI-to-ISA bridge", 1);

  DEV_register_ioread_handler(this, read_handler, 0x00B2, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_ioread_handler(this, read_handler, 0x00B3, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_ioread_handler(this, read_handler, 0x04D0, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_ioread_handler(this, read_handler, 0x04D1, "PIIX3 PCI-to-ISA bridge", 1);
  DEV_register_ioread_handler(this, read_handler, 0x0CF9, "PIIX3 PCI-to-ISA bridge", 1);

  for (i=0; i<16; i++)
    BX_P2I_THIS s.irq_registry[i] = 0x0;
  for (i=0; i<16; i++)
    BX_P2I_THIS s.irq_level[i] = 0x0;
  // initialize readonly registers
  if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
    init_pci_conf(0x8086, 0x7000, 0x00, 0x060100, 0x80);
  } else {
    init_pci_conf(0x8086, 0x122e, 0x01, 0x060100, 0x80);
  }
  BX_P2I_THIS pci_conf[0x04] = 0x07;
  // irq routing registers
  BX_P2I_THIS pci_conf[0x60] = 0x80;
  BX_P2I_THIS pci_conf[0x61] = 0x80;
  BX_P2I_THIS pci_conf[0x62] = 0x80;
  BX_P2I_THIS pci_conf[0x63] = 0x80;
#if BX_DEBUGGER
  // register device for the 'info device' command (calls debug_dump())
  bx_dbg_register_debug_info("pci2isa", this);
#endif
}

void bx_piix3_c::reset(unsigned type)
{
  BX_P2I_THIS pci_conf[0x05] = 0x00;
  BX_P2I_THIS pci_conf[0x06] = 0x00;
  BX_P2I_THIS pci_conf[0x07] = 0x02;
  BX_P2I_THIS pci_conf[0x4c] = 0x4d;
  BX_P2I_THIS pci_conf[0x4e] = 0x03;
  BX_P2I_THIS pci_conf[0x4f] = 0x00;
  BX_P2I_THIS pci_conf[0x69] = 0x02;
  BX_P2I_THIS pci_conf[0x70] = 0x80;
  BX_P2I_THIS pci_conf[0x76] = 0x0c;
  BX_P2I_THIS pci_conf[0x77] = 0x0c;
  BX_P2I_THIS pci_conf[0x78] = 0x02;
  BX_P2I_THIS pci_conf[0x79] = 0x00;
  BX_P2I_THIS pci_conf[0x80] = 0x00;
  BX_P2I_THIS pci_conf[0x82] = 0x00;
  BX_P2I_THIS pci_conf[0xa0] = 0x08;
  BX_P2I_THIS pci_conf[0xa2] = 0x00;
  BX_P2I_THIS pci_conf[0xa3] = 0x00;
  BX_P2I_THIS pci_conf[0xa4] = 0x00;
  BX_P2I_THIS pci_conf[0xa5] = 0x00;
  BX_P2I_THIS pci_conf[0xa6] = 0x00;
  BX_P2I_THIS pci_conf[0xa7] = 0x00;
  BX_P2I_THIS pci_conf[0xa8] = 0x0f;
  BX_P2I_THIS pci_conf[0xaa] = 0x00;
  BX_P2I_THIS pci_conf[0xab] = 0x00;
  BX_P2I_THIS pci_conf[0xac] = 0x00;
  BX_P2I_THIS pci_conf[0xae] = 0x00;

  for (unsigned i = 0; i < 4; i++) {
    pci_set_irq(0x08, i+1, 0);
    pci_unregister_irq(i, 0x80);
  }

  BX_P2I_THIS s.elcr1 = 0x00;
  BX_P2I_THIS s.elcr2 = 0x00;
  BX_P2I_THIS s.pci_reset = 0x00;
  BX_P2I_THIS s.apms = 0x00;
  BX_P2I_THIS s.apmc = 0x00;
}

void bx_piix3_c::register_state(void)
{
  unsigned i;
  char name[6];

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "pci2isa", "PCI-to-ISA Bridge State");

  register_pci_state(list);

  BXRS_HEX_PARAM_FIELD(list, elcr1, BX_P2I_THIS s.elcr1);
  BXRS_HEX_PARAM_FIELD(list, elcr2, BX_P2I_THIS s.elcr2);
  BXRS_HEX_PARAM_FIELD(list, apmc, BX_P2I_THIS s.apmc);
  BXRS_HEX_PARAM_FIELD(list, apms, BX_P2I_THIS s.apms);
  BXRS_HEX_PARAM_FIELD(list, pci_reset, BX_P2I_THIS s.pci_reset);

  bx_list_c *irqr = new bx_list_c(list, "irq_registry");
  for (i=0; i<16; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(irqr, name, &BX_P2I_THIS s.irq_registry[i]);
  }
  bx_list_c *irql = new bx_list_c(list, "irq_level");
  for (i=0; i<16; i++) {
    sprintf(name, "%d", i);
    new bx_shadow_num_c(irql, name, &BX_P2I_THIS s.irq_level[i]);
  }
}

void bx_piix3_c::after_restore_state(void)
{
  for (unsigned i=0; i<16; i++) {
    if (BX_P2I_THIS s.irq_registry[i]) {
      DEV_register_irq(i, "PIIX3 IRQ routing");
    }
  }
}

void bx_piix3_c::pci_register_irq(unsigned pirq, Bit8u irq)
{
  if ((irq < 16) && (((1 << irq) & 0xdef8) > 0)) {
    if (BX_P2I_THIS pci_conf[0x60 + pirq] < 16) {
      pci_unregister_irq(pirq, irq);
    }
    BX_P2I_THIS pci_conf[0x60 + pirq] = irq;
    if (!BX_P2I_THIS s.irq_registry[irq]) {
      DEV_register_irq(irq, "PIIX3 IRQ routing");
    }
    BX_P2I_THIS s.irq_registry[irq] |= (1 << pirq);
  }
}

void bx_piix3_c::pci_unregister_irq(unsigned pirq, Bit8u irq)
{
  Bit8u oldirq =  BX_P2I_THIS pci_conf[0x60 + pirq];
  if (oldirq < 16) {
    BX_P2I_THIS s.irq_registry[oldirq] &= ~(1 << pirq);
    if (!BX_P2I_THIS s.irq_registry[oldirq]) {
      BX_P2I_THIS pci_set_irq(0x08, pirq+1, 0);
      DEV_unregister_irq(oldirq, "PIIX3 IRQ routing");
    }
    BX_P2I_THIS pci_conf[0x60 + pirq] = irq;
  }
}

void bx_piix3_c::pci_set_irq(Bit8u devfunc, unsigned line, bx_bool level)
{
  Bit8u pirq = ((devfunc >> 3) + line - 2) & 0x03;
#if BX_SUPPORT_APIC
  // forward this function call to the ioapic too
  if (DEV_ioapic_present()) {
    DEV_ioapic_set_irq_level(pirq + 16, level);
  }
#endif
  Bit8u irq = BX_P2I_THIS pci_conf[0x60 + pirq];
  if ((irq < 16) && (((1 << irq) & 0xdef8) > 0)) {
    if (level == 1) {
      if (!BX_P2I_THIS s.irq_level[irq]) {
        DEV_pic_raise_irq(irq);
        BX_DEBUG(("PIRQ%c -> IRQ %d = 1", pirq+65, irq));
      }
      BX_P2I_THIS s.irq_level[irq] |= (1 << (devfunc >> 3));
    } else {
      BX_P2I_THIS s.irq_level[irq] &= ~(1 << (devfunc >> 3));
      if (!BX_P2I_THIS s.irq_level[irq]) {
        DEV_pic_lower_irq(irq);
        BX_DEBUG(("PIRQ%c -> IRQ %d = 0", pirq+65, irq));
      }
    }
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_piix3_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_piix3_c *class_ptr = (bx_piix3_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_piix3_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF

  switch (address) {
    case 0x00b2:
      return(BX_P2I_THIS s.apmc);

    case 0x00b3:
      return(BX_P2I_THIS s.apms);

    case 0x04d0:
      return(BX_P2I_THIS s.elcr1);

    case 0x04d1:
      return(BX_P2I_THIS s.elcr2);

    case 0x0cf9:
      return(BX_P2I_THIS s.pci_reset);
  }

  return(0xffffffff);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_piix3_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_P2I_SMF
  bx_piix3_c *class_ptr = (bx_piix3_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_piix3_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_P2I_SMF

  switch (address) {
    case 0x00b2:
#if BX_SUPPORT_PCI
      DEV_acpi_generate_smi((Bit8u)value);
#else
      BX_ERROR(("write %08x: APM command register not supported without ACPI", value));
#endif
      BX_P2I_THIS s.apmc = value & 0xff;
      break;
    case 0x00b3:
      BX_P2I_THIS s.apms = value & 0xff;
      break;
    case 0x04d0:
      value &= 0xf8;
      if (value != BX_P2I_THIS s.elcr1) {
        BX_P2I_THIS s.elcr1 = value;
        BX_INFO(("write: ELCR1 = 0x%02x", BX_P2I_THIS s.elcr1));
        DEV_pic_set_mode(1, BX_P2I_THIS s.elcr1); // master PIC
      }
      break;
    case 0x04d1:
      value &= 0xde;
      if (value != BX_P2I_THIS s.elcr2) {
        BX_P2I_THIS s.elcr2 = value;
        BX_INFO(("write: ELCR2 = 0x%02x", BX_P2I_THIS s.elcr2));
        DEV_pic_set_mode(0, BX_P2I_THIS s.elcr2); // slave PIC
      }
      break;
    case 0x0cf9:
      BX_INFO(("write: CPU reset register = 0x%02x", value));
      BX_P2I_THIS s.pci_reset = value & 0x02;
      if (value & 0x04) {
        if (BX_P2I_THIS s.pci_reset) {
          bx_pc_system.Reset(BX_RESET_HARDWARE);
        } else {
          bx_pc_system.Reset(BX_RESET_SOFTWARE);
        }
      }
      break;
  }
}

// pci configuration space read callback handler
Bit32u bx_piix3_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_P2I_THIS pci_conf[address+i] << (i*8));
  }
  BX_DEBUG(("PIIX3 PCI-to-ISA read  register 0x%02x value 0x%08x", address, value));
  return value;
}

// pci configuration space write callback handler
void bx_piix3_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;

  if ((address >= 0x10) && (address < 0x34))
    return;
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_P2I_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        BX_P2I_THIS pci_conf[address+i] = (value8 & 0x08) | 0x07;
        break;
      case 0x05:
        if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
          BX_P2I_THIS pci_conf[address+i] = (value8 & 0x01);
        }
        break;
      case 0x06:
        break;
      case 0x07:
        if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
          value8 &= 0x78;
        } else {
          value8 &= 0x38;
        }
        BX_P2I_THIS pci_conf[address+i] = (oldval & ~value8) | 0x02;
        break;
      case 0x4e:
        if ((value & 0x04) != (oldval & 0x04)) {
          DEV_mem_set_bios_write((value8 & 0x04) != 0);
        }
        BX_P2I_THIS pci_conf[address+i] = value8;
        break;
      case 0x4f:
        if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
          BX_P2I_THIS pci_conf[address+i] = (value8 & 0x01);
#if BX_SUPPORT_APIC
          if (DEV_ioapic_present()) {
            DEV_ioapic_set_enabled(value8 & 0x01, (BX_P2I_THIS pci_conf[0x80] & 0x3f) << 10);
          }
#endif
        }
        break;
      case 0x60:
      case 0x61:
      case 0x62:
      case 0x63:
        value8 &= 0x8f;
        if (value8 != oldval) {
          if (value8 >= 0x80) {
            pci_unregister_irq((address+i) & 0x03, value8);
          } else {
            pci_register_irq((address+i) & 0x03, value8);
          }
          BX_INFO(("PCI IRQ routing: PIRQ%c# set to 0x%02x", address+i-31,
                   value8));
        }
        break;
      case 0x6a:
        if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
          // TODO: bit #4: enable / disable USB function at boot time
          BX_P2I_THIS pci_conf[address+i] = (value8 & 0xd7);
        }
        break;
      case 0x80:
        if (BX_P2I_THIS s.chipset == BX_PCI_CHIPSET_I440FX) {
          BX_P2I_THIS pci_conf[address+i] = (value8 & 0x7f);
#if BX_SUPPORT_APIC
          if (DEV_ioapic_present()) {
            DEV_ioapic_set_enabled(BX_P2I_THIS pci_conf[0x4f] & 0x01, (value8 & 0x3f) << 10);
          }
#endif
        }
        break;
      default:
        BX_P2I_THIS pci_conf[address+i] = value8;
        BX_DEBUG(("PIIX3 PCI-to-ISA write register 0x%02x value 0x%02x", address+i,
                  value8));
    }
  }
}

#if BX_DEBUGGER
void bx_piix3_c::debug_dump(int argc, char **argv)
{
  int arg, i, j, r;

  dbg_printf("PIIX3 ISA bridge\n\n");
  dbg_printf("ELCR1 = 0x%02x\n", BX_P2I_THIS s.elcr1);
  dbg_printf("ELCR2 = 0x%02x\n", BX_P2I_THIS s.elcr2);
  if (argc == 0) {
    for (i = 0; i < 4; i++) {
      dbg_printf("PIRQ%c# = 0x%02x", i + 65, BX_P2I_THIS pci_conf[0x60 + i]);
      Bit8u irq = BX_P2I_THIS pci_conf[0x60 + i];
      if (irq < 16) {
        dbg_printf(" (level=%d)\n", BX_P2I_THIS s.irq_level[irq] > 0);
      } else {
        dbg_printf("\n");
      }
    }
    dbg_printf("\nSupported options:\n");
    dbg_printf("info device 'pci2isa' 'dump=full' - show PCI config space\n");
  } else {
    for (arg = 0; arg < argc; arg++) {
      if (!strcmp(argv[arg], "dump=full")) {
        dbg_printf("\nPCI config space\n\n");
        r = 0;
        for (i=0; i<16; i++) {
          dbg_printf("%04x ", r);
          for (j=0; j<16; j++) {
            dbg_printf(" %02x", BX_P2I_THIS pci_conf[r++]);
          }
          dbg_printf("\n");
        }
      } else {
        dbg_printf("\nUnknown option: '%s'\n", argv[arg]);
      }
    }
  }
}
#endif

#endif /* BX_SUPPORT_PCI */
