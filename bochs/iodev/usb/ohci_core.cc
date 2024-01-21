/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2023  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

// USB OHCI adapter

// Notes: See usb_ohci.cc

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI

#include "pci.h"
#include "usb_common.h"
#include "ohci_core.h"

#define LOG_THIS 

const char *usb_ohci_port_name[] = {
  "HCRevision        ",
  "HCControl         ",
  "HCCommandStatus   ",
  "HCInterruptStatus ",
  "HCInterruptEnable ",
  "HCInterruptDisable",
  "HCHCCA            ",
  "HCPeriodCurrentED ",
  "HCControlHeadED   ",
  "HCControlCurrentED",
  "HCBulkHeadED      ",
  "HCBulkCurrentED   ",
  "HCDoneHead        ",
  "HCFmInterval      ",
  "HCFmRemaining     ",
  "HCFmNumber        ",
  "HCPeriodicStart   ",
  "HCLSThreshold     ",
  "HCRhDescriptorA   ",
  "HCRhDescriptorB   ",
  "HCRhStatus        ",
  "HCRhPortStatus0   ",
  "HCRhPortStatus1   ",
  "HCRhPortStatus2   ",
  "HCRhPortStatus3   ",
  "  **unknown**     "
};

// the device object

bx_ohci_core_c::bx_ohci_core_c()
{
  put("ohci_core", "OHCIC");
  memset((void*)&hub, 0, sizeof(bx_ohci_core_t));
  hub.timer_index = BX_NULL_TIMER_HANDLE;
}

bx_ohci_core_c::~bx_ohci_core_c()
{
  BX_DEBUG(("Exit"));
}

void bx_ohci_core_c::init_ohci(Bit8u devfunc, Bit16u venid, Bit16u devid, Bit8u rev, Bit8u headt, Bit8u intp)
{
  // Call our frame timer routine every 1mS (1,000uS)
  // Continuous and active
  hub.timer_index =
    DEV_register_timer(this, ohci_timer_handler, 1000, 1,1, "ohci.frame_timer");

  hub.devfunc = devfunc;
  DEV_register_pci_handlers(this, &hub.devfunc, BX_PLUGIN_USB_OHCI, "USB OHCI");

  // initialize readonly registers
  init_pci_conf(venid, devid, rev, 0x0C0310, headt, intp);

  init_bar_mem(0, 4096, read_handler, write_handler);
  hub.ohci_done_count = 7;
  hub.use_control_head = 0;
  hub.use_bulk_head = 0;
  hub.sof_time = 0;

  packets = NULL;
}

void bx_ohci_core_c::reset_ohci(unsigned type)
{
  unsigned i;

  if (type == BX_RESET_HARDWARE) {
    static const struct reset_vals_t {
      unsigned      addr;
      unsigned char val;
    } reset_vals[] = {
      { 0x04, 0x06 }, { 0x05, 0x00 }, // command_io
      { 0x06, 0x10 }, { 0x07, 0x02 }, // status (bit 4 = 1, has capabilities list.)
      { 0x0d, 0x40 },                 // bus latency

      // address space 0x10 - 0x13
      { 0x10, 0x00 }, { 0x11, 0x50 }, //
      { 0x12, 0x00 }, { 0x13, 0xE1 }, //

      { 0x2C, 0xC1 }, { 0x2D, 0x11 }, // subsystem vendor ID
      { 0x2E, 0x03 }, { 0x2F, 0x58 }, // subsystem ID

      { 0x34, 0x50 },                 // offset of capabilities list within configuration space

      { 0x3c, 0x0B },                 // IRQ
      { 0x3E, 0x03 },                 // minimum time bus master needs PCI bus ownership, in 250ns units
      { 0x3F, 0x56 },                 // maximum latency, in 250ns units (bus masters only) (read-only)

      // capabilities list:
      { 0x50, 0x01 },                 //
      { 0x51, 0x00 },                 //
      { 0x52, 0x02 },                 //
      { 0x53, 0x76 },                 //
      { 0x54, 0x00 },                 //
      { 0x55, 0x20 },                 //
      { 0x56, 0x00 },                 //
      { 0x57, 0x1F },                 //
    };
    for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
      pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  reset_hc();
}

void bx_ohci_core_c::reset_hc()
{
  // reset locals
  hub.ohci_done_count = 7;

  // HcRevision
  hub.op_regs.HcRevision         = 0x0110;

  // HcControl
  hub.op_regs.HcControl.reserved  =          0;
  hub.op_regs.HcControl.rwe       =          0;
  hub.op_regs.HcControl.rwc       =          0;
  hub.op_regs.HcControl.ir        =          0;
  hub.op_regs.HcControl.hcfs      =          OHCI_USB_RESET;
  hub.op_regs.HcControl.ble       =          0;
  hub.op_regs.HcControl.cle       =          0;
  hub.op_regs.HcControl.ie        =          0;
  hub.op_regs.HcControl.ple       =          0;
  hub.op_regs.HcControl.cbsr      =          0;

  // HcCommandStatus
  hub.op_regs.HcCommandStatus.reserved0 = 0x000000;
  hub.op_regs.HcCommandStatus.soc       =        0;
  hub.op_regs.HcCommandStatus.reserved1 = 0x000000;
  hub.op_regs.HcCommandStatus.ocr       =        0;
  hub.op_regs.HcCommandStatus.blf       =        0;
  hub.op_regs.HcCommandStatus.clf       =        0;
  hub.op_regs.HcCommandStatus.hcr       =        0;

  // HcInterruptStatus
  hub.op_regs.HcInterruptStatus  = 0x00000000;

  // HcInterruptEnable
  hub.op_regs.HcInterruptEnable  = OHCI_INTR_MIE;

  // HcHCCA
  hub.op_regs.HcHCCA             = 0x00000000;

  // HcPeriodCurrentED
  hub.op_regs.HcPeriodCurrentED  = 0x00000000;

  // HcControlHeadED
  hub.op_regs.HcControlHeadED    = 0x00000000;

  // HcControlCurrentED
  hub.op_regs.HcControlCurrentED = 0x00000000;

  // HcBulkHeadED
  hub.op_regs.HcBulkHeadED       = 0x00000000;

  // HcBulkCurrentED
  hub.op_regs.HcBulkCurrentED    = 0x00000000;

  // HcDoneHead
  hub.op_regs.HcDoneHead         = 0x00000000;

  // HcFmInterval
  hub.op_regs.HcFmInterval.fit      =      0;
  hub.op_regs.HcFmInterval.fsmps    =      0;
  hub.op_regs.HcFmInterval.reserved =      0;
  hub.op_regs.HcFmInterval.fi       = 0x2EDF;

  // HcFmRemaining
  hub.op_regs.HcFmRemainingToggle   =      0;

  // HcFmNumber
  hub.op_regs.HcFmNumber         = 0x00000000;

  // HcPeriodicStart
  hub.op_regs.HcPeriodicStart    = 0x00000000;

  // HcLSThreshold
  hub.op_regs.HcLSThreshold      = 0x0628;

  // HcRhDescriptorA
  hub.op_regs.HcRhDescriptorA.potpgt   = 0x10;
  hub.op_regs.HcRhDescriptorA.reserved =    0;
  hub.op_regs.HcRhDescriptorA.nocp     =    0;
  hub.op_regs.HcRhDescriptorA.ocpm     =    1;
  hub.op_regs.HcRhDescriptorA.dt       =    0;
  hub.op_regs.HcRhDescriptorA.nps      =    0;
  hub.op_regs.HcRhDescriptorA.psm      =    1;
  hub.op_regs.HcRhDescriptorA.ndp      =    USB_OHCI_PORTS;

  // HcRhDescriptorB
  hub.op_regs.HcRhDescriptorB.ppcm     = ((1 << USB_OHCI_PORTS) - 1) << 1;
  hub.op_regs.HcRhDescriptorB.dr       = 0x0000;

  // HcRhStatus
  hub.op_regs.HcRhStatus.crwe      = 0;
  hub.op_regs.HcRhStatus.reserved0 = 0;
  hub.op_regs.HcRhStatus.ocic      = 0;
  hub.op_regs.HcRhStatus.lpsc      = 0;
  hub.op_regs.HcRhStatus.drwe      = 0;
  hub.op_regs.HcRhStatus.reserved1 = 0;
  hub.op_regs.HcRhStatus.oci       = 0;
  hub.op_regs.HcRhStatus.lps       = 0;

  // HcRhPortStatus[x]
  for (int i=0; i<USB_OHCI_PORTS; i++) {
    reset_port(i);
    if (hub.usb_port[i].device != NULL) {
      set_connect_status(i, 1);
    }
  }
  
  while (packets != NULL) {
    usb_cancel_packet(&packets->packet);
    remove_async_packet(&packets, packets);
  }
}

void bx_ohci_core_c::reset_port(int p)
{
  hub.usb_port[p].HcRhPortStatus.reserved0 = 0;
  hub.usb_port[p].HcRhPortStatus.prsc      = 0;
  hub.usb_port[p].HcRhPortStatus.ocic      = 0;
  hub.usb_port[p].HcRhPortStatus.pssc      = 0;
  hub.usb_port[p].HcRhPortStatus.pesc      = 0;
  hub.usb_port[p].HcRhPortStatus.reserved1 = 0;
  hub.usb_port[p].HcRhPortStatus.lsda      = 0;
  hub.usb_port[p].HcRhPortStatus.pps       = 0;
  hub.usb_port[p].HcRhPortStatus.reserved2 = 0;
  hub.usb_port[p].HcRhPortStatus.prs       = 0;
  hub.usb_port[p].HcRhPortStatus.poci      = 0;
  hub.usb_port[p].HcRhPortStatus.pss       = 0;
  hub.usb_port[p].HcRhPortStatus.pes       = 0;
}

void bx_ohci_core_c::ohci_register_state(bx_list_c *parent)
{
  unsigned i;
  char portnum[8];
  bx_list_c *hub1, *port, *reg;

  bx_list_c *list = new bx_list_c(parent, "usb_ohci", "USB OHCI State");
  hub1 = new bx_list_c(list, "hub");
  reg = new bx_list_c(hub1, "HcControl");
  BXRS_PARAM_BOOL(reg, rwe, hub.op_regs.HcControl.rwe);
  BXRS_PARAM_BOOL(reg, rwc, hub.op_regs.HcControl.rwc);
  BXRS_PARAM_BOOL(reg, ir, hub.op_regs.HcControl.ir);
  BXRS_HEX_PARAM_FIELD(reg, hcfs, hub.op_regs.HcControl.hcfs);
  BXRS_PARAM_BOOL(reg, ble, hub.op_regs.HcControl.ble);
  BXRS_PARAM_BOOL(reg, cle, hub.op_regs.HcControl.cle);
  BXRS_PARAM_BOOL(reg, ie, hub.op_regs.HcControl.ie);
  BXRS_PARAM_BOOL(reg, ple, hub.op_regs.HcControl.ple);
  BXRS_HEX_PARAM_FIELD(reg, cbsr, hub.op_regs.HcControl.cbsr);
  reg = new bx_list_c(hub1, "HcCommandStatus");
  BXRS_HEX_PARAM_FIELD(reg, soc, hub.op_regs.HcCommandStatus.soc);
  BXRS_PARAM_BOOL(reg, ocr, hub.op_regs.HcCommandStatus.ocr);
  BXRS_PARAM_BOOL(reg, blf, hub.op_regs.HcCommandStatus.blf);
  BXRS_PARAM_BOOL(reg, clf, hub.op_regs.HcCommandStatus.clf);
  BXRS_PARAM_BOOL(reg, hcr, hub.op_regs.HcCommandStatus.hcr);
  BXRS_HEX_PARAM_FIELD(hub1, HcInterruptStatus, hub.op_regs.HcInterruptStatus);
  BXRS_HEX_PARAM_FIELD(hub1, HcInterruptEnable, hub.op_regs.HcInterruptEnable);
  BXRS_HEX_PARAM_FIELD(hub1, HcHCCA, hub.op_regs.HcHCCA);
  BXRS_HEX_PARAM_FIELD(hub1, HcPeriodCurrentED, hub.op_regs.HcPeriodCurrentED);
  BXRS_HEX_PARAM_FIELD(hub1, HcControlHeadED, hub.op_regs.HcControlHeadED);
  BXRS_HEX_PARAM_FIELD(hub1, HcControlCurrentED, hub.op_regs.HcControlCurrentED);
  BXRS_HEX_PARAM_FIELD(hub1, HcBulkHeadED, hub.op_regs.HcBulkHeadED);
  BXRS_HEX_PARAM_FIELD(hub1, HcBulkCurrentED, hub.op_regs.HcBulkCurrentED);
  BXRS_HEX_PARAM_FIELD(hub1, HcDoneHead, hub.op_regs.HcDoneHead);
  reg = new bx_list_c(hub1, "HcFmInterval");
  BXRS_PARAM_BOOL(reg, fit, hub.op_regs.HcFmInterval.fit);
  BXRS_HEX_PARAM_FIELD(reg, fsmps, hub.op_regs.HcFmInterval.fsmps);
  BXRS_HEX_PARAM_FIELD(reg, fi, hub.op_regs.HcFmInterval.fi);
  BXRS_PARAM_BOOL(hub1, HcFmRemainingToggle, hub.op_regs.HcFmRemainingToggle);
  BXRS_HEX_PARAM_FIELD(hub1, HcFmNumber, hub.op_regs.HcFmNumber);
  BXRS_HEX_PARAM_FIELD(hub1, HcPeriodicStart, hub.op_regs.HcPeriodicStart);
  reg = new bx_list_c(hub1, "HcRhDescriptorA");
  BXRS_HEX_PARAM_FIELD(reg, potpgt, hub.op_regs.HcRhDescriptorA.potpgt);
  BXRS_PARAM_BOOL(reg, nocp, hub.op_regs.HcRhDescriptorA.nocp);
  BXRS_PARAM_BOOL(reg, ocpm, hub.op_regs.HcRhDescriptorA.ocpm);
  BXRS_PARAM_BOOL(reg, nps, hub.op_regs.HcRhDescriptorA.nps);
  BXRS_PARAM_BOOL(reg, psm, hub.op_regs.HcRhDescriptorA.psm);
  reg = new bx_list_c(hub1, "HcRhDescriptorB");
  BXRS_HEX_PARAM_FIELD(reg, ppcm, hub.op_regs.HcRhDescriptorB.ppcm);
  BXRS_HEX_PARAM_FIELD(reg, dr, hub.op_regs.HcRhDescriptorB.dr);
  reg = new bx_list_c(hub1, "HcRhStatus");
  BXRS_PARAM_BOOL(reg, crwe, hub.op_regs.HcRhStatus.crwe);
  BXRS_PARAM_BOOL(reg, ocic, hub.op_regs.HcRhStatus.ocic);
  BXRS_PARAM_BOOL(reg, lpsc, hub.op_regs.HcRhStatus.lpsc);
  BXRS_PARAM_BOOL(reg, drwe, hub.op_regs.HcRhStatus.drwe);
  BXRS_PARAM_BOOL(reg, oci, hub.op_regs.HcRhStatus.oci);
  BXRS_PARAM_BOOL(reg, lps, hub.op_regs.HcRhStatus.lps);
  for (i=0; i<USB_OHCI_PORTS; i++) {
    sprintf(portnum, "port%d", i+1);
    port = new bx_list_c(hub1, portnum);
    reg = new bx_list_c(port, "HcRhPortStatus");
    BXRS_PARAM_BOOL(reg, prsc, hub.usb_port[i].HcRhPortStatus.prsc);
    BXRS_PARAM_BOOL(reg, ocic, hub.usb_port[i].HcRhPortStatus.ocic);
    BXRS_PARAM_BOOL(reg, pssc, hub.usb_port[i].HcRhPortStatus.pssc);
    BXRS_PARAM_BOOL(reg, pesc, hub.usb_port[i].HcRhPortStatus.pesc);
    BXRS_PARAM_BOOL(reg, csc, hub.usb_port[i].HcRhPortStatus.csc);
    BXRS_PARAM_BOOL(reg, lsda, hub.usb_port[i].HcRhPortStatus.lsda);
    BXRS_PARAM_BOOL(reg, pps, hub.usb_port[i].HcRhPortStatus.pps);
    BXRS_PARAM_BOOL(reg, prs, hub.usb_port[i].HcRhPortStatus.prs);
    BXRS_PARAM_BOOL(reg, poci, hub.usb_port[i].HcRhPortStatus.poci);
    BXRS_PARAM_BOOL(reg, pss, hub.usb_port[i].HcRhPortStatus.pss);
    BXRS_PARAM_BOOL(reg, pes, hub.usb_port[i].HcRhPortStatus.pes);
    BXRS_PARAM_BOOL(reg, ccs, hub.usb_port[i].HcRhPortStatus.ccs);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  BXRS_DEC_PARAM_FIELD(hub1, ohci_done_count, hub.ohci_done_count);
  BXRS_PARAM_BOOL(hub1, use_control_head, hub.use_control_head);
  BXRS_PARAM_BOOL(hub1, use_bulk_head, hub.use_bulk_head);
  BXRS_DEC_PARAM_FIELD(hub1, sof_time, hub.sof_time);
  // TODO: handle async packets
  register_pci_state(hub1);
}

void bx_ohci_core_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(NULL);
  for (int j=0; j<USB_OHCI_PORTS; j++) {
    if (hub.usb_port[j].device != NULL) {
      hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_ohci_core_c::update_irq()
{
  bool level = 0;

  if ((hub.op_regs.HcInterruptEnable & OHCI_INTR_MIE) &&
      (hub.op_regs.HcInterruptStatus & hub.op_regs.HcInterruptEnable)) {
      level = 1;
      BX_DEBUG(("Interrupt Fired."));
  }
  DEV_pci_set_irq(hub.devfunc, pci_conf[0x3d], level);
}

void bx_ohci_core_c::set_interrupt(Bit32u value)
{
  hub.op_regs.HcInterruptStatus |= value;
  update_irq();
}

bool bx_ohci_core_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_ohci_core_c *class_ptr = (bx_ohci_core_c *) param;
  return class_ptr->mem_read(addr, len, data);
}

bool bx_ohci_core_c::mem_read(bx_phy_address addr, unsigned len, void *data)
{
  Bit32u val = 0x0;
  int p = 0;

  if (len != 4) {
    BX_INFO(("Read at 0x%08X with len != 4 (%d)", (Bit32u)addr, len));
    return 1;
  }
  if (addr & 3) {
    BX_INFO(("Misaligned read at 0x%08X", (Bit32u)addr));
    return 1;
  }

  Bit32u  offset = (Bit32u)(addr - pci_bar[0].addr);
  switch (offset) {
    case 0x00: // HcRevision
      val = hub.op_regs.HcRevision;
      break;

    case 0x04: // HcControl
      val =   (hub.op_regs.HcControl.reserved     << 11)
            | (hub.op_regs.HcControl.rwe      ? 1 << 10 : 0)
            | (hub.op_regs.HcControl.rwc      ? 1 << 9 : 0)
            | (hub.op_regs.HcControl.ir       ? 1 << 8 : 0)
            | (hub.op_regs.HcControl.hcfs         << 6)
            | (hub.op_regs.HcControl.ble      ? 1 << 5 : 0)
            | (hub.op_regs.HcControl.cle      ? 1 << 4 : 0)
            | (hub.op_regs.HcControl.ie       ? 1 << 3 : 0)
            | (hub.op_regs.HcControl.ple      ? 1 << 2 : 0)
            | (hub.op_regs.HcControl.cbsr         << 0);
      break;

    case 0x08: // HcCommandStatus
      val =   (hub.op_regs.HcCommandStatus.reserved0     << 18)
            | (hub.op_regs.HcCommandStatus.soc           << 16)
            | (hub.op_regs.HcCommandStatus.reserved1     << 4)
            | (hub.op_regs.HcCommandStatus.ocr       ? 1 << 3 : 0)
            | (hub.op_regs.HcCommandStatus.blf       ? 1 << 2 : 0)
            | (hub.op_regs.HcCommandStatus.clf       ? 1 << 1 : 0)
            | (hub.op_regs.HcCommandStatus.hcr       ? 1 << 0 : 0);
      break;

    case 0x0C: // HcInterruptStatus
      val = hub.op_regs.HcInterruptStatus;
      break;

    case 0x10: // HcInterruptEnable
    case 0x14: // HcInterruptDisable (reading this one returns that one)
      val = hub.op_regs.HcInterruptEnable;
      break;

    case 0x18: // HcHCCA
      val = hub.op_regs.HcHCCA;
      break;

    case 0x1C: // HcPeriodCurrentED
      val = hub.op_regs.HcPeriodCurrentED;
      break;

    case 0x20: // HcControlHeadED
      val = hub.op_regs.HcControlHeadED;
      break;

    case 0x24: // HcControlCurrentED
      val = hub.op_regs.HcControlCurrentED;
      break;

    case 0x28: // HcBulkHeadED
      val = hub.op_regs.HcBulkHeadED;
      break;

    case 0x2C: // HcBulkCurrentED
      val = hub.op_regs.HcBulkCurrentED;
      break;

    case 0x30: // HcDoneHead
      val = hub.op_regs.HcDoneHead;
      break;

    case 0x34: // HcFmInterval
      val =   (hub.op_regs.HcFmInterval.fit      ? 1 << 31 : 0)
            | (hub.op_regs.HcFmInterval.fsmps        << 16)
            | (hub.op_regs.HcFmInterval.reserved     << 14)
            | (hub.op_regs.HcFmInterval.fi           << 0);
      break;

    case 0x38: // HcFmRemaining
      val = get_frame_remaining();
      break;

    case 0x3C: // HcFmNumber
      val = hub.op_regs.HcFmNumber;
      break;

    case 0x40: // HcPeriodicStart
      val = hub.op_regs.HcPeriodicStart;
      break;

    case 0x44: // HcLSThreshold
      val = hub.op_regs.HcLSThreshold;
      break;

    case 0x48: // HcRhDescriptorA
      val =   (hub.op_regs.HcRhDescriptorA.potpgt       << 24)
            | (hub.op_regs.HcRhDescriptorA.reserved     << 13)
            | (hub.op_regs.HcRhDescriptorA.nocp     ? 1 << 12 : 0)
            | (hub.op_regs.HcRhDescriptorA.ocpm     ? 1 << 11 : 0)
            | 0 //hub.op_regs.HcRhDescriptorA.dt       << 10
            | (hub.op_regs.HcRhDescriptorA.nps      ? 1 <<  9 : 0)
            | (hub.op_regs.HcRhDescriptorA.psm      ? 1 <<  8 : 0)
            | (hub.op_regs.HcRhDescriptorA.ndp          <<  0);
      break;

    case 0x4C: // HcRhDescriptorB
      val =   (hub.op_regs.HcRhDescriptorB.ppcm << 16)
            | (hub.op_regs.HcRhDescriptorB.dr   << 0);
      break;

    case 0x50: // HcRhStatus
      val =   (hub.op_regs.HcRhStatus.crwe      ? 1 << 31 : 0)
            | (hub.op_regs.HcRhStatus.reserved0     << 18)
            | (hub.op_regs.HcRhStatus.ocic      ? 1 << 17 : 0)
            | 0 //hub.op_regs.HcRhStatus.lpsc      << 16
            | (hub.op_regs.HcRhStatus.drwe      ? 1 << 15 : 0)
            | (hub.op_regs.HcRhStatus.reserved1     <<  2)
            | (hub.op_regs.HcRhStatus.oci       ? 1 <<  1 : 0)
            | (hub.op_regs.HcRhStatus.lps       ? 1 <<  0 : 0);
      break;

    case 0x60: // HcRhPortStatus[3]
#if (USB_OHCI_PORTS < 4)
      val = 0;
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (USB_OHCI_PORTS < 3)
      val = 0;
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (USB_OHCI_PORTS < 2)
      val = 0;
      break;
#endif
    case 0x54: // HcRhPortStatus[0]
      p = (offset - 0x54) >> 2;
      val =   (hub.usb_port[p].HcRhPortStatus.reserved0  << 21)
            | (hub.usb_port[p].HcRhPortStatus.prsc      ? (1 << 20) : 0)
            | (hub.usb_port[p].HcRhPortStatus.ocic      ? (1 << 19) : 0)
            | (hub.usb_port[p].HcRhPortStatus.pssc      ? (1 << 18) : 0)
            | (hub.usb_port[p].HcRhPortStatus.pesc      ? (1 << 17) : 0)
            | (hub.usb_port[p].HcRhPortStatus.csc       ? (1 << 16) : 0)
            | (hub.usb_port[p].HcRhPortStatus.reserved1     << 10)
            | (hub.usb_port[p].HcRhPortStatus.lsda      ? (1 <<  9) : 0)
            | (hub.usb_port[p].HcRhPortStatus.pps       ? (1 <<  8) : 0)
            | (hub.usb_port[p].HcRhPortStatus.reserved2     <<  5)
            | (hub.usb_port[p].HcRhPortStatus.prs       ? (1 <<  4) : 0)
            | (hub.usb_port[p].HcRhPortStatus.poci      ? (1 <<  3) : 0)
            | (hub.usb_port[p].HcRhPortStatus.pss       ? (1 <<  2) : 0)
            | (hub.usb_port[p].HcRhPortStatus.pes       ? (1 <<  1) : 0)
            | (hub.usb_port[p].HcRhPortStatus.ccs       ? (1 <<  0) : 0);
      break;

    default:
      BX_ERROR(("unsupported read from address=0x%08X!", (Bit32u)addr));
      break;
  }

  int name = offset >> 2;
  if (name > (0x60 >> 2))
    name = 25;
  //BX_INFO(("register read from address 0x%04X (%s):  0x%08X (len=%d)", (unsigned) addr, usb_ohci_port_name[name], (Bit32u) val, len));
  *((Bit32u *) data) = val;

  return 1;
}

bool bx_ohci_core_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  bx_ohci_core_c *class_ptr = (bx_ohci_core_c *) param;
  return class_ptr->mem_write(addr, len, data);
}

bool bx_ohci_core_c::mem_write(bx_phy_address addr, unsigned len, void *data)
{
  Bit32u value = *((Bit32u *) data);
  Bit32u offset = (Bit32u)addr - pci_bar[0].addr;
  int p, org_state;

  int name = offset >> 2;
  if (name > (0x60 >> 2))
    name = 25;
  //BX_INFO(("register write to  address 0x%04X (%s):  0x%08X (len=%d)", (unsigned) addr, usb_ohci_port_name[name], (unsigned) value, len));

  if (len != 4) {
    BX_INFO(("Write at 0x%08X with len != 4 (%d)", (Bit32u)addr, len));
    return 1;
  }
  if (addr & 3) {
    BX_INFO(("Misaligned write at 0x%08X", (Bit32u)addr));
    return 1;
  }

  switch (offset) {
    case 0x00: // HcRevision
      BX_ERROR(("Write to HcRevision ignored"));
      break;

    case 0x04: // HcControl
      if (value & 0xFFFFF800)
        BX_ERROR(("Write to reserved field in HcControl"));
      org_state = hub.op_regs.HcControl.hcfs;
      hub.op_regs.HcControl.rwe      = (value & (1<<10)) ? 1 : 0;
      hub.op_regs.HcControl.rwc      = (value & (1<< 9)) ? 1 : 0;
      hub.op_regs.HcControl.ir       = (value & (1<< 8)) ? 1 : 0;
      hub.op_regs.HcControl.hcfs     = (value & (3<< 6)) >>  6;
      hub.op_regs.HcControl.ble      = (value & (1<< 5)) ? 1 : 0;
      hub.op_regs.HcControl.cle      = (value & (1<< 4)) ? 1 : 0;
      hub.op_regs.HcControl.ie       = (value & (1<< 3)) ? 1 : 0;
      hub.op_regs.HcControl.ple      = (value & (1<< 2)) ? 1 : 0;
      hub.op_regs.HcControl.cbsr     = (value & (3<< 0)) >>  0;
      if (hub.op_regs.HcControl.hcfs == OHCI_USB_OPERATIONAL) {
        hub.op_regs.HcFmRemainingToggle = 0;
        if (org_state != OHCI_USB_OPERATIONAL)
          hub.use_control_head = hub.use_bulk_head = 1;
      }
      break;

    case 0x08: // HcCommandStatus
      if (value & 0xFFFCFFF0)
        BX_ERROR(("Write to a reserved field in HcCommandStatus"));
      if (value & (3<<16))
        BX_ERROR(("Write to R/O field: HcCommandStatus.soc"));
      if (value & (1<< 3)) hub.op_regs.HcCommandStatus.ocr = 1;
      if (value & (1<< 2)) hub.op_regs.HcCommandStatus.blf = 1;
      if (value & (1<< 1)) hub.op_regs.HcCommandStatus.clf = 1;
      if (value & (1<< 0)) {
        hub.op_regs.HcCommandStatus.hcr = 1;
        reset_hc();
        hub.op_regs.HcControl.hcfs = OHCI_USB_SUSPEND;
        for (unsigned i=0; i<USB_OHCI_PORTS; i++)
          if (hub.usb_port[i].HcRhPortStatus.ccs && (hub.usb_port[i].device != NULL))
            hub.usb_port[i].device->usb_send_msg(USB_MSG_RESET);
      }
      break;

    case 0x0C: // HcInterruptStatus /// all are WC
      if (value & 0xBFFFFF80)
        BX_DEBUG(("Write to a reserved field in HcInterruptStatus"));
      hub.op_regs.HcInterruptStatus &= ~value;
      update_irq();
      break;

    case 0x10: // HcInterruptEnable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptEnable"));
      hub.op_regs.HcInterruptEnable |= (value & 0xC000007F);
      update_irq();
      break;

    case 0x14: // HcInterruptDisable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptDisable"));
      hub.op_regs.HcInterruptEnable &= ~value;
      update_irq();
      break;

    case 0x18: // HcHCCA
      // the HCD can write 0xFFFFFFFF to this register to see what the alignement is
      //  by reading back the amount and seeing how many lower bits are clear.
      if ((value & 0x000000FF) && (value != 0xFFFFFFFF))
        BX_ERROR(("Write to lower byte of HcHCCA non zero."));
      hub.op_regs.HcHCCA = (value & 0xFFFFFF00);
      break;

    case 0x1C: // HcPeriodCurrentED
      BX_ERROR(("Write to HcPeriodCurrentED not allowed."));
      break;

    case 0x20: // HcControlHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlHeadED non zero."));
      hub.op_regs.HcControlHeadED = (value & 0xFFFFFFF0);
      break;

    case 0x24: // HcControlCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlCurrentED non zero."));
      hub.op_regs.HcControlCurrentED = (value & 0xFFFFFFF0);
      break;

    case 0x28: // HcBulkHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkHeadED non zero."));
      hub.op_regs.HcBulkHeadED = (value & 0xFFFFFFF0);
      break;

    case 0x2C: // HcBulkCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkCurrentED non zero."));
      hub.op_regs.HcBulkCurrentED = (value & 0xFFFFFFF0);
      break;

    case 0x30: // HcDoneHead
      BX_ERROR(("Write to HcDoneHead not allowed."));
      break;

    case 0x34: // HcFmInterval
      if (value & 0x0000C000)
        BX_ERROR(("Write to a reserved field in HcFmInterval."));
      hub.op_regs.HcFmInterval.fit      = (value & (1<<31)) ? 1 : 0;
      hub.op_regs.HcFmInterval.fsmps    = (value & 0x7FFF0000) >> 16;
      hub.op_regs.HcFmInterval.fi       = (value & 0x00003FFF) >> 0;
      break;

    case 0x38: // HcFmRemaining
      BX_ERROR(("Write to HcFmRemaining not allowed."));
      break;

    case 0x3C: // HcFmNumber
      BX_ERROR(("Write to HcFmNumber not allowed."));
      break;

    case 0x40: // HcPeriodicStart
      if (value & 0xFFFFC000)
        BX_ERROR(("Write to a reserved field in HcPeriodicStart."));
      hub.op_regs.HcPeriodicStart = (value & 0x00003FFF);
      break;

    case 0x44: // HcLSThreshold
      hub.op_regs.HcLSThreshold = (value & 0x00000FFF);
      break;

    case 0x48: // HcRhDescriptorA
      if (value & 0x00FFE000)
        BX_ERROR(("Write to a reserved field in HcRhDescriptorA."));
      if ((value & 0x000000FF) != hub.op_regs.HcRhDescriptorA.ndp)
        BX_ERROR(("Write to HcRhDescriptorA.ndp not allowed."));
      if (value & (1<<10))
        BX_ERROR(("Write to HcRhDescriptorA.dt not allowed."));
      hub.op_regs.HcRhDescriptorA.potpgt   = (value & 0xFF000000) >> 24;
      hub.op_regs.HcRhDescriptorA.nocp     = (value & (1<<12)) ? 1 : 0;
      hub.op_regs.HcRhDescriptorA.ocpm     = (value & (1<<11)) ? 1 : 0;
      hub.op_regs.HcRhDescriptorA.nps      = (value & (1<< 9)) ? 1 : 0;
      hub.op_regs.HcRhDescriptorA.psm      = (value & (1<< 8)) ? 1 : 0;
      if (hub.op_regs.HcRhDescriptorA.psm == 0) {

        BX_INFO(("Ben: hub.op_regs.HcRhDescriptorA.psm == 0"));
        // all ports have power, etc.
        // hub.usb_port[p].HcRhPortStatus.pps = 1
        //  Call a routine to set each ports dword (LS, Connected, etc.)

      } else {

        BX_INFO(("Ben: hub.op_regs.HcRhDescriptorA.psm == 1"));
        // only ports with bit set in rhstatus have power, etc.
        //  Call a routine to set each ports dword (LS, Connected, etc.)

      }
      break;

    case 0x4C: // HcRhDescriptorB
      hub.op_regs.HcRhDescriptorB.ppcm = (value & 0xFFFF0000) >> 16;
      hub.op_regs.HcRhDescriptorB.dr   = (value & 0x0000FFFF) >>  0;
      break;

    case 0x50: { // HcRhStatus
      if (value & 0x7FFC7FFC)
        BX_ERROR(("Write to a reserved field in HcRhStatus."));
      if (value & (1<<1))
        BX_ERROR(("Write to HcRhStatus.oci not allowed."));
      // which one of these two takes presidence?
      if (value & (1<<31)) hub.op_regs.HcRhStatus.drwe = 0;
      if (value & (1<<15)) hub.op_regs.HcRhStatus.drwe = 1;

      if (value & (1<<17)) hub.op_regs.HcRhStatus.ocic = 1;
      if (value & (1<<16)) {
        if (hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<USB_OHCI_PORTS; p++)
            hub.usb_port[p].HcRhPortStatus.pps = 1;
        } else {
          for (p=0; p<USB_OHCI_PORTS; p++)
            if ((hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)) == 0)
              hub.usb_port[p].HcRhPortStatus.pps = 1;
        }
      }
      if (value & (1<<0)) {
        if (hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<USB_OHCI_PORTS; p++)
            hub.usb_port[p].HcRhPortStatus.pps = 0;
        } else {
          for (p=0; p<USB_OHCI_PORTS; p++)
            if (!(hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)))
              hub.usb_port[p].HcRhPortStatus.pps = 0;
        }
      }
      break;
    }

    case 0x60: // HcRhPortStatus[3]
#if (USB_OHCI_PORTS < 4)
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (USB_OHCI_PORTS < 3)
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (USB_OHCI_PORTS < 2)
      break;
#endif
    case 0x54: { // HcRhPortStatus[0]
      p = (offset - 0x54) >> 2;
      if (value & 0xFFE0FCE0)
        BX_ERROR(("Write to a reserved field in usb_port[%d].HcRhPortStatus", p));
      if (value & (1<<0))
        hub.usb_port[p].HcRhPortStatus.pes = 0;
      if (value & (1<<1)) {
        if (hub.usb_port[p].HcRhPortStatus.ccs == 0)
          hub.usb_port[p].HcRhPortStatus.csc = 1;
        else {
          hub.usb_port[p].HcRhPortStatus.pes = 1;
        }
      }
      if (value & (1<<2)) {
        if (hub.usb_port[p].HcRhPortStatus.ccs == 0)
          hub.usb_port[p].HcRhPortStatus.csc = 1;
        else
          hub.usb_port[p].HcRhPortStatus.pss = 1;
      }
//      if (value & (1<<3))
//        if (hub.usb_port[p].HcRhPortStatus.pss)
//          ; // do a resume (or test this in the timer code and do the resume there)
      if (value & (1<<4)) {
        if (hub.usb_port[p].HcRhPortStatus.ccs == 0)
          hub.usb_port[p].HcRhPortStatus.csc = 1;
        else {
          reset_port(p);
          hub.usb_port[p].HcRhPortStatus.pps = 1;
          hub.usb_port[p].HcRhPortStatus.pes = 1;
          hub.usb_port[p].HcRhPortStatus.prsc = 1;
          // are we are currently connected/disconnected
          if (hub.usb_port[p].device != NULL) {
            hub.usb_port[p].HcRhPortStatus.lsda =
              (hub.usb_port[p].device->get_speed() == USB_SPEED_LOW);
            set_connect_status(p, 1);
            hub.usb_port[p].device->usb_send_msg(USB_MSG_RESET);
          }
          set_interrupt(OHCI_INTR_RHSC);
        }
      }
      if (value & (1<<8))
        hub.usb_port[p].HcRhPortStatus.pps = 1;
      if (value & (1<<9))
        hub.usb_port[p].HcRhPortStatus.pps = 0;
      if (value & (1<<16))
        hub.usb_port[p].HcRhPortStatus.csc = (value & ((1<<4) | (1<<1) | (1<<2))) ? 1 : 0;
      if (value & (1<<17))
        hub.usb_port[p].HcRhPortStatus.pesc = 0;
      if (value & (1<<18))
        hub.usb_port[p].HcRhPortStatus.pssc = 0;
      if (value & (1<<19))
        hub.usb_port[p].HcRhPortStatus.ocic = 0;
      if (value & (1<<20))
        hub.usb_port[p].HcRhPortStatus.prsc = 0;
      break;
    }

    default:
      BX_ERROR(("unsupported write to address=0x%08X, val = 0x%08X!", (Bit32u)addr, value));
      break;
  }

  return 1;
}

Bit32u bx_ohci_core_c::get_frame_remaining(void)
{
  Bit16u bit_time, fr;

  bit_time = (Bit16u)((bx_pc_system.time_usec() - hub.sof_time) * 12);
  if ((hub.op_regs.HcControl.hcfs != OHCI_USB_OPERATIONAL) ||
      (bit_time > hub.op_regs.HcFmInterval.fi)) {
    fr = 0;
  } else {
    fr = hub.op_regs.HcFmInterval.fi - bit_time;
  }
  return (hub.op_regs.HcFmRemainingToggle << 31) | fr;
}

void bx_ohci_core_c::ohci_timer_handler(void *this_ptr)
{
  bx_ohci_core_c *class_ptr = (bx_ohci_core_c *) this_ptr;
  class_ptr->ohci_timer();
}

// Called once every 1mS
void bx_ohci_core_c::ohci_timer(void)
{
  struct OHCI_ED cur_ed;
  Bit32u address, ed_address;
  Bit16u zero = 0;

  if (hub.op_regs.HcControl.hcfs == OHCI_USB_OPERATIONAL) {
    // set remaining to the interval amount.
    hub.op_regs.HcFmRemainingToggle = hub.op_regs.HcFmInterval.fit;
    hub.sof_time = bx_pc_system.time_usec();

    // The Frame Number Register is incremented
    //  every time bit 15 is changed (at 0x8000 or 0x0000), fno is fired.
    hub.op_regs.HcFmNumber++;
    hub.op_regs.HcFmNumber &= 0xffff;
    DEV_MEM_WRITE_PHYSICAL(hub.op_regs.HcHCCA + 0x80, 2, (Bit8u *) &hub.op_regs.HcFmNumber);
    DEV_MEM_WRITE_PHYSICAL(hub.op_regs.HcHCCA + 0x82, 2, (Bit8u *) &zero);
    if ((hub.op_regs.HcFmNumber == 0x8000) || (hub.op_regs.HcFmNumber == 0x0000)) {
      set_interrupt(OHCI_INTR_FNO);
    }

    //
    set_interrupt(OHCI_INTR_SF);

    // if interrupt delay (done_count) == 0, and status.wdh == 0, then update the donehead fields.
    //BX_DEBUG(("done_count = %d, status.wdh = %d", hub.ohci_done_count,
    //          ((hub.op_regs.HcInterruptStatus & OHCI_INTR_WD) > 0)));
    if ((hub.ohci_done_count == 0) && ((hub.op_regs.HcInterruptStatus & OHCI_INTR_WD) == 0)) {
      Bit32u temp = hub.op_regs.HcDoneHead;
      if (hub.op_regs.HcInterruptStatus & hub.op_regs.HcInterruptEnable)
        temp |= 1;
      BX_DEBUG(("Updating the hcca.DoneHead field to 0x%08X and setting the wdh flag", temp));
      DEV_MEM_WRITE_PHYSICAL(hub.op_regs.HcHCCA + 0x84, 4, (Bit8u *) &temp);
      hub.op_regs.HcDoneHead = 0;
      hub.ohci_done_count = 7;
      set_interrupt(OHCI_INTR_WD);
    }

    // if (6 >= done_count > 0) then decrement done_count
    if ((hub.ohci_done_count != 7) && (hub.ohci_done_count > 0))
      hub.ohci_done_count--;

    process_lists();

    // do the ED's in the interrupt table
    if (hub.op_regs.HcControl.ple) {
      address = hub.op_regs.HcHCCA + ((hub.op_regs.HcFmNumber & 0x1F) * 4);
      DEV_MEM_READ_PHYSICAL(address, 4, (Bit8u*) &ed_address);
      while (ed_address) {
        DEV_MEM_READ_PHYSICAL(ed_address,      4, (Bit8u*) &cur_ed.dword0);
        DEV_MEM_READ_PHYSICAL(ed_address +  4, 4, (Bit8u*) &cur_ed.dword1);
        DEV_MEM_READ_PHYSICAL(ed_address +  8, 4, (Bit8u*) &cur_ed.dword2);
        DEV_MEM_READ_PHYSICAL(ed_address + 12, 4, (Bit8u*) &cur_ed.dword3);
        process_ed(&cur_ed, ed_address);
        ed_address = ED_GET_NEXTED(&cur_ed);
      }
    }

  }  // end run schedule
}

void bx_ohci_core_c::process_lists(void)
{
  struct OHCI_ED cur_ed;

  // TODO:  Rather than just comparing .fr to < 8000 here, and < 4000 below, see the statement on 
  //   page 45 of the OHCI specs:
  // "When a new frame starts, the Host Controller processes control and bulk Endpoint Descriptors until 
  //  the Remaining field of the HcFmRemaining register is less than or equal to the Start field of the 
  //  HcPeriodicStart register"

  // if the control list is enabled *and* the control list filled bit is set, do a control list ED
  if (hub.op_regs.HcControl.cle) {
    if (hub.use_control_head) {
      hub.op_regs.HcControlCurrentED = 0;
      hub.use_control_head = 0;
    }
    if (!hub.op_regs.HcControlCurrentED && hub.op_regs.HcCommandStatus.clf) {
      hub.op_regs.HcControlCurrentED = hub.op_regs.HcControlHeadED;
      hub.op_regs.HcCommandStatus.clf = 0;
    }
    while (hub.op_regs.HcControlCurrentED) {
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcControlCurrentED,      4, (Bit8u*) &cur_ed.dword0);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcControlCurrentED +  4, 4, (Bit8u*) &cur_ed.dword1);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcControlCurrentED +  8, 4, (Bit8u*) &cur_ed.dword2);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcControlCurrentED + 12, 4, (Bit8u*) &cur_ed.dword3);
      process_ed(&cur_ed, hub.op_regs.HcControlCurrentED);
      hub.op_regs.HcControlCurrentED = ED_GET_NEXTED(&cur_ed);
      if (get_frame_remaining() < 8000)
        break;
    }
  }

  // if the bulk list is enabled *and* the bulk list filled bit is set, do a bulk list ED
  if (hub.op_regs.HcControl.ble) {
    if (hub.use_bulk_head) {
      hub.op_regs.HcBulkCurrentED = 0;
      hub.use_bulk_head = 0;
    }
    if (!hub.op_regs.HcBulkCurrentED && hub.op_regs.HcCommandStatus.blf) {
      hub.op_regs.HcBulkCurrentED = hub.op_regs.HcBulkHeadED;
      hub.op_regs.HcCommandStatus.blf = 0;
    }
    while (hub.op_regs.HcBulkCurrentED) {
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcBulkCurrentED,      4, (Bit8u*) &cur_ed.dword0);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcBulkCurrentED +  4, 4, (Bit8u*) &cur_ed.dword1);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcBulkCurrentED +  8, 4, (Bit8u*) &cur_ed.dword2);
      DEV_MEM_READ_PHYSICAL(hub.op_regs.HcBulkCurrentED + 12, 4, (Bit8u*) &cur_ed.dword3);
      if (process_ed(&cur_ed, hub.op_regs.HcBulkCurrentED)) {
        hub.op_regs.HcCommandStatus.blf = 1;
      }
      hub.op_regs.HcBulkCurrentED = ED_GET_NEXTED(&cur_ed);
      if (get_frame_remaining() < 4000)
        break;
    }
  }
}

bool bx_ohci_core_c::process_ed(struct OHCI_ED *ed, const Bit32u ed_address)
{
  struct OHCI_TD cur_td;
  int toggle;
  bool ret = 0;

  if (!ED_GET_H(ed) && !ED_GET_K(ed) && (ED_GET_HEADP(ed) != ED_GET_TAILP(ed))) {
    // if the isochronous is enabled and ed is a isochronous, do TD
    if (ED_GET_F(ed)) {
      if (hub.op_regs.HcControl.ie) {
        // load and do a isochronous TD list
        BX_DEBUG(("Found a valid ED that points to an isochronous TD"));
        // we currently ignore ISO TD's
      }
    } else {
      BX_DEBUG(("Found a valid ED that points to an control/bulk/int TD"));
      ret = 1;
      while (!ED_GET_H(ed) && (ED_GET_HEADP(ed) != ED_GET_TAILP(ed))) {
        toggle = ED_GET_C(ed);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed),      4, (Bit8u*) &cur_td.dword0);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) +  4, 4, (Bit8u*) &cur_td.dword1);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) +  8, 4, (Bit8u*) &cur_td.dword2);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) + 12, 4, (Bit8u*) &cur_td.dword3);
        BX_DEBUG(("Head: 0x%08X  Tail: 0x%08X  Next: 0x%08X", ED_GET_HEADP(ed), ED_GET_TAILP(ed), TD_GET_NEXTTD(&cur_td)));
        if (TD_GET_T(&cur_td) & 2)
          toggle = TD_GET_T(&cur_td) & 1;
        int td_ret = process_td(&cur_td, ed, toggle);
        if (td_ret == 0) {
          // USB_RET_ASYNC or already processed TD, so done with ED (for now)
          break;
        } else if (td_ret > 0) {
          // Processed TD with no error
          const Bit32u temp = ED_GET_HEADP(ed);
          if (TD_GET_CC(&cur_td) < NotAccessed) {
            ED_SET_HEADP(ed, TD_GET_NEXTTD(&cur_td));
            TD_SET_NEXTTD(&cur_td, hub.op_regs.HcDoneHead);
            hub.op_regs.HcDoneHead = temp;
            if (TD_GET_DI(&cur_td) < hub.ohci_done_count)
              hub.ohci_done_count = TD_GET_DI(&cur_td);
          }
          ED_SET_C(ed, toggle ^ 1);
          DEV_MEM_WRITE_PHYSICAL(temp,      4, (Bit8u*) &cur_td.dword0);
          DEV_MEM_WRITE_PHYSICAL(temp +  4, 4, (Bit8u*) &cur_td.dword1);
          DEV_MEM_WRITE_PHYSICAL(temp +  8, 4, (Bit8u*) &cur_td.dword2);
        } else {
          // Processed TD with error, advance the toggle anyway
          ED_SET_C(ed, toggle ^ 1);
          break;
        }
      }
    }
    DEV_MEM_WRITE_PHYSICAL(ed_address +  8, 4, (Bit8u*) &ed->dword2);
  }
  return ret;
}

int ohci_event_handler(int event, void *ptr, void *dev, int port)
{
  if (dev != NULL) {
    return ((bx_ohci_core_c *) dev)->event_handler(event, ptr, port);
  }
  return -1;
}

int bx_ohci_core_c::event_handler(int event, void *ptr, int port)
{
  Bit32u intr = 0;
  int ret = 0;
  USBAsync *p;

  switch (event) {
    // packet events start here
    case USB_EVENT_ASYNC:
      BX_DEBUG(("Async packet completion"));
      p = container_of_usb_packet(ptr);
      p->done = 1;
      process_lists();
      break;
    case USB_EVENT_WAKEUP:
      if (hub.usb_port[port].HcRhPortStatus.pss) {
        hub.usb_port[port].HcRhPortStatus.pss = 0;
        hub.usb_port[port].HcRhPortStatus.pssc = 1;
        intr = OHCI_INTR_RHSC;
      }
      if (hub.op_regs.HcControl.hcfs == OHCI_USB_SUSPEND) {
        hub.op_regs.HcControl.hcfs = OHCI_USB_RESUME;
        intr = OHCI_INTR_RD;
      }
      set_interrupt(intr);
      break;

    // host controller events start here
    case USB_EVENT_DEFAULT_SPEED:
      // return default speed for specified port number
      return USB_SPEED_FULL;
      
    case USB_EVENT_CHECK_SPEED:
      if (ptr != NULL) {
        usb_device_c *usb_device = (usb_device_c *) ptr;
        if (usb_device->get_speed() <= USB_SPEED_FULL)
          ret = 1;
      }
      break;
    default:
      BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
      ret = -1; // unknown event, event not handled
  }

  return ret;
}

int bx_ohci_core_c::process_td(struct OHCI_TD *td, struct OHCI_ED *ed, int toggle)
{
  unsigned pid = 0, len = 0, len1, len2;
  int ret2 = 1;
  int ilen, ret = 0;
  Bit32u addr;
  Bit16u maxlen = 0;
  USBAsync *p;
  bool completion;

  addr = ED_GET_HEADP(ed);
  p = find_async_packet(&packets, addr);
  completion = (p != NULL);
  if (completion && !p->done) {
    return 0;
  }

  // The td->cc field should be 111x if it hasn't been processed yet.
  if (TD_GET_CC(td) < NotAccessed) {
    BX_ERROR(("Found TD with CC value not 111x"));
    return 0;
  }

  if (ED_GET_D(ed) == 1)
    pid = USB_TOKEN_OUT;
  else if (ED_GET_D(ed) == 2)
    pid = USB_TOKEN_IN;
  else {
    if (TD_GET_DP(td) == 0)
      pid = USB_TOKEN_SETUP;
    else if (TD_GET_DP(td) == 1)
      pid = USB_TOKEN_OUT;
    else if (TD_GET_DP(td) == 2)
      pid = USB_TOKEN_IN;
  }

  // calculate the length of the packet
  if (TD_GET_CBP(td) && TD_GET_BE(td)) {
    if ((TD_GET_CBP(td) & 0xFFFFF000) != (TD_GET_BE(td) & 0xFFFFF000))
      len = (TD_GET_BE(td) & 0xFFF) + 0x1001 - (TD_GET_CBP(td) & 0xFFF);
    else {
      ilen = ((int) TD_GET_BE(td) - TD_GET_CBP(td)) + 1;
      if (ilen < 0)
        len = 0x1001 + len;
      else
        len = (unsigned) ilen;
    }
  } else
    len = 0;

  if (completion) {
    ret = p->packet.len;
  } else {
    switch (pid) {
      case USB_TOKEN_SETUP:
      case USB_TOKEN_OUT:
        maxlen = (len <= ED_GET_MPS(ed)) ? len : ED_GET_MPS(ed); // limit the data length the the max packet size
        break;
      case USB_TOKEN_IN:
        maxlen = len;
        break;
    }
    p = create_async_packet(&packets, addr, maxlen);
    p->packet.pid = pid;
    p->packet.devaddr = ED_GET_FA(ed);
    p->packet.devep = ED_GET_EN(ed);
    p->packet.speed = ED_GET_S(ed) ? USB_SPEED_LOW : USB_SPEED_FULL;
#if HANDLE_TOGGLE_CONTROL
    p->packet.toggle = toggle;
#endif
    p->packet.complete_cb = ohci_event_handler;
    p->packet.complete_dev = this;

    BX_DEBUG(("    pid = %s  addr = %d  endpnt = %d  len = %d  mps = %d s = %d (td->cbp = 0x%08X, td->be = 0x%08X)",
      (pid == USB_TOKEN_IN)? "IN" : (pid == USB_TOKEN_OUT) ? "OUT" : (pid == USB_TOKEN_SETUP) ? "SETUP" : "UNKNOWN",
      ED_GET_FA(ed), ED_GET_EN(ed), maxlen, ED_GET_MPS(ed), ED_GET_S(ed), TD_GET_CBP(td), TD_GET_BE(td)));
    BX_DEBUG(("    td->t = %d  ed->c = %d  td->di = %d  td->r = %d", TD_GET_T(td), ED_GET_C(ed), TD_GET_DI(td), TD_GET_R(td)));

    switch (pid) {
      case USB_TOKEN_SETUP:
        if (maxlen > 0)
          DEV_MEM_READ_PHYSICAL_DMA(TD_GET_CBP(td), maxlen, p->packet.data);
        // TODO: This is a hack.  dev->handle_packet() should return the amount of bytes
        //  it received, not the amount it anticipates on receiving/sending in the next packet.
        if ((ret = broadcast_packet(&p->packet)) >= 0)
          ret = 8;
        break;
      case USB_TOKEN_OUT:
        if (maxlen > 0)
          DEV_MEM_READ_PHYSICAL_DMA(TD_GET_CBP(td), maxlen, p->packet.data);
        ret = broadcast_packet(&p->packet);
        break;
      case USB_TOKEN_IN:
        ret = broadcast_packet(&p->packet);
        break;
      default:
        TD_SET_CC(td, UnexpectedPID);
        TD_SET_EC(td, 3);
        return 1;
    }

    if (ret == USB_RET_ASYNC) {
      BX_DEBUG(("Async packet deferred"));
      return 0;
    }
  }
  
  if ((ret > 0) && (pid == USB_TOKEN_IN)) {
    if (((TD_GET_CBP(td) & 0xfff) + ret) > 0x1000) {
      len1 = 0x1000 - (TD_GET_CBP(td) & 0xfff);
      len2 = ret - len1;
      DEV_MEM_WRITE_PHYSICAL_DMA(TD_GET_CBP(td), len1, p->packet.data);
      DEV_MEM_WRITE_PHYSICAL_DMA((TD_GET_BE(td) & ~0xfff), len2, p->packet.data + len1);
    } else {
      DEV_MEM_WRITE_PHYSICAL_DMA(TD_GET_CBP(td), ret, p->packet.data);
    }
  }
  
  if ((ret == (int) len) || 
     ((pid == USB_TOKEN_IN) && (ret >= 0) && TD_GET_R(td)) || 
     ((pid == USB_TOKEN_OUT) && (ret >= 0) && (ret <= (int) ED_GET_MPS(ed)))) {
    if (ret == (int) len)
      TD_SET_CBP(td, 0);
    else {
      if (((TD_GET_CBP(td) & 0xfff) + ret) >= 0x1000) {
        TD_SET_CBP(td, (TD_GET_CBP(td) + ret) & 0x0FFF);
        TD_SET_CBP(td, TD_GET_CBP(td) | (TD_GET_BE(td) & ~0x0FFF));
      } else {
        TD_SET_CBP(td, TD_GET_CBP(td) + ret);
      }
    }
    if ((pid != USB_TOKEN_OUT) || (ret == (int) len)) {
      TD_SET_CC(td, NoError);
      TD_SET_EC(td, 0);
    }
  } else {
    if (ret >= 0) {
      TD_SET_CC(td, DataUnderrun);
      if (((TD_GET_CBP(td) & 0xfff) + ret) >= 0x1000) {
        TD_SET_CBP(td, (TD_GET_CBP(td) + ret) & 0x0FFF);
        TD_SET_CBP(td, TD_GET_CBP(td) | (TD_GET_BE(td) & ~0x0FFF));
      } else {
        TD_SET_CBP(td, TD_GET_CBP(td) + ret);
      }
      if (!TD_GET_R(td))
        ED_SET_H(ed, 1);
    } else {
      switch (ret) {
        case USB_RET_NODEV:  // (-1)
          TD_SET_CC(td, DeviceNotResponding);
          break;
        case USB_RET_NAK:    // (-2)
          break;
        case USB_RET_STALL:  // (-3)
          TD_SET_CC(td, Stall);
          break;
        case USB_RET_BABBLE:  // (-4)
          TD_SET_CC(td, BufferOverrun);
          break;
        default:
          BX_ERROR(("Unknown error returned: %d", ret));
          break;
      }
      ret2 = ret;
    }
    if (ret != USB_RET_NAK) {
      TD_SET_EC(td, 3);
      ED_SET_H(ed, 1);
    }
  }

  BX_DEBUG((" td->cbp = 0x%08X   ret = %d  len = %d  td->cc = %d   td->ec = %d  ed->h = %d", TD_GET_CBP(td), ret, maxlen, TD_GET_CC(td), TD_GET_EC(td), ED_GET_H(ed)));
  BX_DEBUG(("    td->t = %d  ed->c = %d", TD_GET_T(td), ED_GET_C(ed)));
  remove_async_packet(&packets, p);

  return ret2;
}

int bx_ohci_core_c::broadcast_packet(USBPacket *p)
{
  int ret = USB_RET_NODEV;
  for (int i = 0; i < USB_OHCI_PORTS && ret == USB_RET_NODEV; i++) {
    if ((hub.usb_port[i].device != NULL) &&
        (hub.usb_port[i].HcRhPortStatus.ccs)) {
      ret = hub.usb_port[i].device->handle_packet(p);
    }
  }
  return ret;
}

// pci configuration space write callback handler
void bx_ohci_core_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  if (((address >= 0x14) && (address <= 0x34)))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    Bit8u value8 = (value >> (i*8)) & 0xFF;
//  Bit8u oldval = pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x06; // (bit 0 is read only for this card) (we don't allow port IO)
        pci_conf[address+i] = value8;
        break;
      case 0x3d: //
      case 0x3e: //
      case 0x3f: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      default:
        pci_conf[address+i] = value8;
    }
  }
}

bool bx_ohci_core_c::set_connect_status(Bit8u port, bool connected)
{
  const bool ccs_org = hub.usb_port[port].HcRhPortStatus.ccs;
  const bool pes_org = hub.usb_port[port].HcRhPortStatus.pes;

  usb_device_c *device = hub.usb_port[port].device;
  if (device != NULL) {
    if (connected) {
      switch (device->get_speed()) {
        case USB_SPEED_LOW:
          hub.usb_port[port].HcRhPortStatus.lsda = 1;
          break;
        case USB_SPEED_FULL:
          hub.usb_port[port].HcRhPortStatus.lsda = 0;
          break;
        case USB_SPEED_HIGH:
        case USB_SPEED_SUPER:
          BX_ERROR(("HC ignores device with unsupported speed"));
          return 0;
        default:
          BX_PANIC(("USB device returned invalid speed value"));
          return 0;
      }
      hub.usb_port[port].HcRhPortStatus.ccs = 1;
      if (!device->get_connected()) {
        if (!device->init()) {
          BX_ERROR(("port #%d: connect failed", port+1));
          return 0;
        } else {
          BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
        }
      }
    } else { // not connected
      BX_INFO(("port #%d: device disconnect", port+1));
      hub.usb_port[port].HcRhPortStatus.ccs = 0;
      hub.usb_port[port].HcRhPortStatus.pes = 0;
      hub.usb_port[port].HcRhPortStatus.lsda = 0;
    }
    hub.usb_port[port].HcRhPortStatus.csc |= (ccs_org != hub.usb_port[port].HcRhPortStatus.ccs);
    hub.usb_port[port].HcRhPortStatus.pesc |= (pes_org != hub.usb_port[port].HcRhPortStatus.pes);

    // we changed the value of the port, so show it
    set_interrupt(OHCI_INTR_RHSC);
  }
  return connected;
}

void bx_ohci_core_c::set_port_device(int port, usb_device_c *dev)
{
  usb_device_c *olddev = hub.usb_port[port].device;
  if ((dev != NULL) && (olddev == NULL)) {
    // make sure we are calling the correct handler for the device
    dev->set_event_handler(this, ohci_event_handler, port);
    hub.usb_port[port].device = dev;
    set_connect_status(port, 1);
  } else if ((dev == NULL) && (olddev != NULL)) {
    set_connect_status(port, 0);
    hub.usb_port[port].device = dev;
  }
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI
