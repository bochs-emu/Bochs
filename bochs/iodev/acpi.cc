/////////////////////////////////////////////////////////////////////////
// $Id: acpi.cc,v 1.4 2006-10-01 19:51:49 vruppert Exp $
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

//
// PIIX4 ACPI support
//


// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE 
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "cpu/cpu.h"
#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_ACPI

#define LOG_THIS theACPIController->

bx_acpi_ctrl_c* theACPIController = NULL;

// FIXME
const Bit8u acpi_pm_iomask[64] = {2, 0, 2, 0, 2, 0, 0, 0, 4, 0, 0, 0, 7, 7, 7, 7,
                                  7, 7, 7, 7, 1, 1, 0, 0, 7, 7, 0, 0, 7, 7, 7, 7,
                                  7, 7, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7,
                                  1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};
const Bit8u acpi_sm_iomask[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 2, 0, 0, 0};

#define PM_FREQ 3579545

#define RTC_EN (1 << 10)
#define PWRBTN_EN (1 << 8)
#define GBL_EN (1 << 5)
#define TMROF_EN (1 << 0)

#define SCI_EN (1 << 0)

#define SUS_EN (1 << 13)

int libacpi_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theACPIController = new bx_acpi_ctrl_c();
  bx_devices.pluginACPIController = theACPIController;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theACPIController, BX_PLUGIN_ACPI);
  return 0; // Success
}

void libacpi_LTX_plugin_fini(void)
{
  delete theACPIController;
}

/* ported from QEMU: compute with 96 bit intermediate result: (a*b)/c */
Bit64u muldiv64(Bit64u a, Bit32u b, Bit32u c)
{
    union {
        Bit64u ll;
        struct {
#ifdef WORDS_BIGENDIAN
            Bit32u high, low;
#else
            Bit32u low, high;
#endif            
        } l;
    } u, res;
    Bit64u rl, rh;

    u.ll = a;
    rl = (Bit64u)u.l.low * (Bit64u)b;
    rh = (Bit64u)u.l.high * (Bit64u)b;
    rh += (rl >> 32);
    res.l.high = rh / c;
    res.l.low = (((rh % c) << 32) + (rl & 0xffffffff)) / c;
    return res.ll;
}

bx_acpi_ctrl_c::bx_acpi_ctrl_c()
{
  put("ACPI");
  settype(ACPILOG);
  s.timer_index = BX_NULL_TIMER_HANDLE;
}

bx_acpi_ctrl_c::~bx_acpi_ctrl_c()
{
  BX_DEBUG(("Exit"));
}

void bx_acpi_ctrl_c::init(void)
{
  // called once when bochs initializes

  unsigned i;

  BX_ACPI_THIS s.devfunc = BX_PCI_DEVICE(1, 3);
  DEV_register_pci_handlers(this, &BX_ACPI_THIS s.devfunc, BX_PLUGIN_ACPI,
                            "ACPI Controller");

  if (BX_ACPI_THIS s.timer_index == BX_NULL_TIMER_HANDLE) {
    BX_ACPI_THIS s.timer_index =
      bx_pc_system.register_timer(this, timer_handler, 1000, 0, 0, "ACPI");
  }

  for (i=0; i<256; i++) {
    BX_ACPI_THIS s.pci_conf[i] = 0x0;
  }
  BX_ACPI_THIS s.pm_base = 0x0;
  BX_ACPI_THIS s.sm_base = 0x0;

  // readonly registers
  static const struct init_vals_t {
    unsigned      addr;
    unsigned char val;
  } init_vals[] = {
    { 0x00, 0x86 }, { 0x01, 0x80 },
    { 0x02, 0x13 }, { 0x03, 0x71 },
    { 0x0a, 0x80 },                 // other bridge device
    { 0x0b, 0x06 },                 // bridge device
    { 0x0e, 0x00 },                 // header type
    { 0x3d, BX_PCI_INTA }           // interrupt pin #1
  };
  for (i = 0; i < sizeof(init_vals) / sizeof(*init_vals); ++i) {
    BX_ACPI_THIS s.pci_conf[init_vals[i].addr] = init_vals[i].val;
  }
}

void bx_acpi_ctrl_c::reset(unsigned type)
{
  static const struct reset_vals_t {
    unsigned      addr;
    unsigned char val;
  } reset_vals[] = {
      { 0x04, 0x00 }, { 0x05, 0x00 },  // command_io + command_mem
      { 0x06, 0x80 }, { 0x07, 0x02 },  // status_devsel_medium
      { 0x3c, 0x00 },                  // IRQ
      // PM base 0x40 - 0x43
      { 0x40, 0x01 }, { 0x41, 0x00 },
      { 0x42, 0x00 }, { 0x43, 0x00 },
      // device resources
      { 0x5f, 0x90 }, { 0x63, 0x60 },
      { 0x67, 0x98 },
      // SM base 0x90 - 0x93
      { 0x90, 0x01 }, { 0x91, 0x00 },
      { 0x92, 0x00 }, { 0x93, 0x00 }
  };
  for (unsigned i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
    BX_ACPI_THIS s.pci_conf[reset_vals[i].addr] = reset_vals[i].val;
  }

  BX_ACPI_THIS s.pmsts = 0;
  BX_ACPI_THIS s.pmen = 0;
  BX_ACPI_THIS s.pmcntrl = 0;
  BX_ACPI_THIS s.tmr_overflow_time = 0xffffff;
}

#if BX_SUPPORT_SAVE_RESTORE
void bx_acpi_ctrl_c::register_state(void)
{
  unsigned i;
  char name[6];

  bx_list_c *list = new bx_list_c(SIM->get_sr_root(), "acpi", "ACPI Controller State");
  BXRS_HEX_PARAM_FIELD(list, pmsts, BX_ACPI_THIS s.pmsts);
  BXRS_HEX_PARAM_FIELD(list, pmen, BX_ACPI_THIS s.pmen);
  BXRS_HEX_PARAM_FIELD(list, pmcntrl, BX_ACPI_THIS s.pmcntrl);
  BXRS_HEX_PARAM_FIELD(list, tmr_overflow_time, BX_ACPI_THIS s.tmr_overflow_time);
  bx_list_c *pci_conf = new bx_list_c(list, "pci_conf", 256);
  for (i=0; i<256; i++) {
    sprintf(name, "0x%02x", i);
    new bx_shadow_num_c(pci_conf, name, &BX_ACPI_THIS s.pci_conf[i], BASE_HEX);
  }
}

void bx_acpi_ctrl_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_ACPI_THIS_PTR, read_handler, write_handler,
                         &BX_ACPI_THIS s.pm_base,
                         &BX_ACPI_THIS s.pci_conf[0x40],
                         64, &acpi_pm_iomask[0], "ACPI PM base"))
  {
     BX_INFO(("new PM base address: 0x%04x", BX_ACPI_THIS s.pm_base));
  }
  if (DEV_pci_set_base_io(BX_ACPI_THIS_PTR, read_handler, write_handler,
                         &BX_ACPI_THIS s.sm_base,
                         &BX_ACPI_THIS s.pci_conf[0x90],
                         16, &acpi_sm_iomask[0], "ACPI SM base"))
  {
     BX_INFO(("new SM base address: 0x%04x", BX_ACPI_THIS s.sm_base));
  }
}
#endif

void bx_acpi_ctrl_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_ACPI_THIS s.devfunc, BX_ACPI_THIS s.pci_conf[0x3d], level);
}

Bit32u bx_acpi_ctrl_c::get_pmtmr(void)
{
  Bit64u value = muldiv64(bx_pc_system.time_usec(), PM_FREQ, 1000000);
  return (Bit32u)(value & 0xffffff);
}

Bit16u bx_acpi_ctrl_c::get_pmsts(void)
{
  Bit16u pmsts = BX_ACPI_THIS s.pmsts;
  Bit64u value = muldiv64(bx_pc_system.time_usec(), PM_FREQ, 1000000);
  if (value >= BX_ACPI_THIS s.tmr_overflow_time)
    BX_ACPI_THIS s.pmsts |= TMROF_EN;
  return pmsts;
}

void bx_acpi_ctrl_c::pm_update_sci(void)
{
  Bit16u pmsts = get_pmsts();
  bx_bool sci_level = (((pmsts & BX_ACPI_THIS s.pmen) & 
                      (RTC_EN | PWRBTN_EN | GBL_EN | TMROF_EN)) != 0);
  BX_ACPI_THIS set_irq_level(sci_level);
  // schedule a timer interruption if needed
  if ((BX_ACPI_THIS s.pmen & TMROF_EN) && !(pmsts & TMROF_EN)) {
    Bit64u expire_time = muldiv64(BX_ACPI_THIS s.tmr_overflow_time, 1000000, PM_FREQ);
      bx_pc_system.activate_timer(BX_ACPI_THIS s.timer_index, expire_time, 0);
    } else {
      bx_pc_system.deactivate_timer(BX_ACPI_THIS s.timer_index);
    }
}

void bx_acpi_ctrl_c::generate_smi(void)
{
  if (BX_ACPI_THIS s.pci_conf[0x5b] & 0x02) {
    BX_CPU(0)->deliver_SMI();
  }
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_acpi_ctrl_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_ACPI_SMF
  bx_acpi_ctrl_c *class_ptr = (bx_acpi_ctrl_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_acpi_ctrl_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_ACPI_SMF
  Bit8u reg = address & 0x3f;
  Bit32u value = 0xffffffff;

  if ((address & 0xffc0) == BX_ACPI_THIS s.pm_base) {
    switch (reg) {
      case 0x00:
        value = BX_ACPI_THIS get_pmsts();
        break;
      case 0x02:
        value = BX_ACPI_THIS s.pmen;
        break;
      case 0x04:
        value = BX_ACPI_THIS s.pmcntrl;
        break;
      case 0x08:
        value = BX_ACPI_THIS get_pmtmr();
        break;
      default:
        BX_INFO(("ACPI read from PM register 0x%02x not implemented yet", reg));
    }
    BX_DEBUG(("ACPI read from PM register 0x%02x returns 0x%08x", reg, value));
  } else {
    BX_INFO(("ACPI read from SM register 0x%02x not implemented yet", reg));
  }
  return value;
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_acpi_ctrl_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_ACPI_SMF
  bx_acpi_ctrl_c *class_ptr = (bx_acpi_ctrl_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_acpi_ctrl_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_ACPI_SMF
  Bit8u reg = address & 0x3f;

  if ((address & 0xffc0) == BX_ACPI_THIS s.pm_base) {
    BX_DEBUG(("ACPI write to PM register 0x%02x, value = 0x%04x", reg, value));
    switch (reg) {
      case 0x00:
        {
          Bit16u pmsts = BX_ACPI_THIS get_pmsts();
          if (pmsts & value & TMROF_EN) {
            // if TMRSTS is reset, then compute the new overflow time
            Bit64u d = muldiv64(bx_pc_system.time_usec(), PM_FREQ, 1000000);
            BX_ACPI_THIS s.tmr_overflow_time = (d + 0x800000LL) & ~0x7fffffLL;
          }
          BX_ACPI_THIS s.pmsts &= ~value;
          BX_ACPI_THIS pm_update_sci();
        }
        break;
      case 0x02:
        BX_ACPI_THIS s.pmen = value;
        BX_ACPI_THIS pm_update_sci();
        break;
      case 0x04:
        {
          BX_ACPI_THIS s.pmcntrl = value & ~(SUS_EN);
          if (value & SUS_EN) {
            // change suspend type
            Bit16u sus_typ = (value >> 10) & 3;
            switch (sus_typ) {
              case 0: // soft power off
                bx_user_quit = 1;
                LOG_THIS setonoff(LOGLEV_PANIC, ACT_FATAL);
                BX_PANIC(("ACPI control: soft power off"));
                break;
              default:
                break;
            }
          }
        }
        break;
      default:
        BX_INFO(("ACPI write to PM register 0x%02x not implemented yet", reg));
    }
  } else {
    BX_INFO(("ACPI write to SM register 0x%02x not implemented yet", reg));
  }
}

void bx_acpi_ctrl_c::timer_handler(void *this_ptr)
{
  bx_acpi_ctrl_c *class_ptr = (bx_acpi_ctrl_c *) this_ptr;
  class_ptr->timer();
}

void bx_acpi_ctrl_c::timer()
{
  BX_ACPI_THIS pm_update_sci();
}

// pci configuration space read callback handler
Bit32u bx_acpi_ctrl_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  if (io_len > 4 || io_len == 0) {
    BX_DEBUG(("ACPI controller read register 0x%02x, len=%u !",
             (unsigned) address, (unsigned) io_len));
    return 0xffffffff;
  }

  const char* pszName = "                  ";
  switch (address) {
    case 0x00: if (io_len == 2) {
                 pszName = "(vendor id)       ";
               } else if (io_len == 4) {
                 pszName = "(vendor + device) ";
               }
      break;
    case 0x04: if (io_len == 2) {
                 pszName = "(command)         ";
               } else if (io_len == 4) {
                 pszName = "(command+status)  ";
               }
      break;
    case 0x08: if (io_len == 1) {
                 pszName = "(revision id)     ";
               } else if (io_len == 4) {
                 pszName = "(rev.+class code) ";
               }
      break;
    case 0x0c: pszName = "(cache line size) "; break;
    case 0x28: pszName = "(cardbus cis)     "; break;
    case 0x2c: pszName = "(subsys. vendor+) "; break;
    case 0x30: pszName = "(rom base)        "; break;
    case 0x3c: pszName = "(interrupt line+) "; break;
    case 0x3d: pszName = "(interrupt pin)   "; break;
  }

  // This odd code is to display only what bytes actually were read.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_ACPI_THIS s.pci_conf[address+i] << (i*8));

    sprintf(szTmp2, "%02x", (BX_ACPI_THIS s.pci_conf[address+i]));
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("ACPI controller  read register 0x%02x %svalue 0x%s",
            address, pszName, szTmp));
  return value;
}


// static pci configuration space write callback handler
void bx_acpi_ctrl_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool pm_base_change = 0, sm_base_change = 0;

  if ((address >= 0x10) && (address < 0x34))
    return;
  // This odd code is to display only what bytes actually were written.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_ACPI_THIS s.pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 = (value8 & 0xfe) | (value & 0x01);
        goto set_value;
        break;
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        strcpy(szTmp2, "..");
        break;
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
        }
        goto set_value;
        break;
      case 0x40:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x41:
      case 0x42:
      case 0x43:
        pm_base_change |= (value8 != oldval);
        goto set_value;
        break;
      case 0x90:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x91:
      case 0x92:
      case 0x93:
        sm_base_change |= (value8 != oldval);
      default:
set_value:
        BX_ACPI_THIS s.pci_conf[address+i] = value8;
        sprintf(szTmp2, "%02x", value8);
    }
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  if (pm_base_change) {
    if (DEV_pci_set_base_io(BX_ACPI_THIS_PTR, read_handler, write_handler,
                            &BX_ACPI_THIS s.pm_base,
                            &BX_ACPI_THIS s.pci_conf[0x40],
                            64, &acpi_pm_iomask[0], "ACPI PM base"))
    {
       BX_INFO(("new PM base address: 0x%04x", BX_ACPI_THIS s.pm_base));
    }
  }
  if (sm_base_change) {
    if (DEV_pci_set_base_io(BX_ACPI_THIS_PTR, read_handler, write_handler,
                            &BX_ACPI_THIS s.sm_base,
                            &BX_ACPI_THIS s.pci_conf[0x90],
                            16, &acpi_sm_iomask[0], "ACPI SM base"))
    {
       BX_INFO(("new SM base address: 0x%04x", BX_ACPI_THIS s.sm_base));
    }
  }
  strrev(szTmp);
  BX_DEBUG(("ACPI controller write register 0x%02x value 0x%s", address, szTmp));
}

#endif // BX_SUPPORT_PCI
