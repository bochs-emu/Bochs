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

// Notes: See usb_uhci.cc

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE


#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI

#include "pci.h"
#include "usb_common.h"
#include "usb_ohci.h"

#define LOG_THIS theUSB_OHCI->

bx_usb_ohci_c* theUSB_OHCI = NULL;

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

// builtin configuration handling functions

Bit32s usb_ohci_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "usb_ohci")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_OHCI);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_OHCI_ENABLED)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "port", 4) || !strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, params[i], USB_OHCI_PORTS, base) < 0) {
          return -1;
        }
      } else {
        BX_ERROR(("%s: unknown parameter '%s' for usb_ohci ignored.", context, params[i]));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s usb_ohci_options_save(FILE *fp)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_OHCI);
  SIM->write_usb_options(fp, USB_OHCI_PORTS, base);
  return 0;
}

// device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_ohci)
{
  if (mode == PLUGIN_INIT) {
    theUSB_OHCI = new bx_usb_ohci_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_OHCI, BX_PLUGIN_USB_OHCI);
    // add new configuration parameter for the config interface
    SIM->init_usb_options("OHCI", "ohci", USB_OHCI_PORTS);
    // register add-on option for bochsrc and command line
    SIM->register_addon_option("usb_ohci", usb_ohci_options_parser, usb_ohci_options_save);
  } else if (mode == PLUGIN_FINI) {
    SIM->unregister_addon_option("usb_ohci");
    bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.usb");
    delete theUSB_OHCI;
    menu->remove("ohci");
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_OPTIONAL;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

// the device object

bx_usb_ohci_c::bx_usb_ohci_c()
{
  put("usb_ohci", "OHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_ohci_t));
  hub.frame_timer_index = BX_NULL_TIMER_HANDLE;
  hub.rt_conf_id = -1;
}

bx_usb_ohci_c::~bx_usb_ohci_c()
{
  char pname[32];

  SIM->unregister_runtime_config_handler(hub.rt_conf_id);

  for (int i=0; i<USB_OHCI_PORTS; i++) {
    sprintf(pname, "port%d.device", i+1);
    SIM->get_param_enum(pname, SIM->get_param(BXPN_USB_OHCI))->set_handler(NULL);
    sprintf(pname, "port%d.options", i+1);
    SIM->get_param_string(pname, SIM->get_param(BXPN_USB_OHCI))->set_enable_handler(NULL);
    sprintf(pname, "port%d.over_current", i+1);
    SIM->get_param_bool(pname, SIM->get_param(BXPN_USB_OHCI))->set_handler(NULL);
    remove_device(i);
  }

  SIM->get_bochs_root()->remove("usb_ohci");
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove("ohci");
  BX_DEBUG(("Exit"));
}

void bx_usb_ohci_c::init(void)
{
  unsigned i;
  char pname[6];
  bx_list_c *ohci, *port;
  bx_param_enum_c *device;
  bx_param_string_c *options;
  bx_param_bool_c *over_current;

  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);
  
  // Read in values from config interface
  ohci = (bx_list_c*) SIM->get_param(BXPN_USB_OHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", ohci)->get()) {
    BX_INFO(("USB OHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c *)((bx_list_c *) SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_ohci"))->set(0);
    return;
  }

  // Call our frame timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_OHCI_THIS hub.frame_timer_index =
                   DEV_register_timer(this, usb_frame_handler, 1000, 1,1, "ohci.frame_timer");

  BX_OHCI_THIS hub.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_OHCI_THIS hub.devfunc, BX_PLUGIN_USB_OHCI,
                            "USB OHCI");

  // initialize readonly registers
  init_pci_conf(0x11c1, 0x5803, 0x11, 0x0c0310, 0x00, BX_PCI_INTD);

  BX_OHCI_THIS init_bar_mem(0, 4096, read_handler, write_handler);
  BX_OHCI_THIS hub.ohci_done_count = 7;
  BX_OHCI_THIS hub.use_control_head = 0;
  BX_OHCI_THIS hub.use_bulk_head = 0;
  BX_OHCI_THIS hub.sof_time = 0;

  bx_list_c *usb_rt = (bx_list_c *) SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *ohci_rt = new bx_list_c(usb_rt, "ohci", "OHCI Runtime Options");
  ohci_rt->set_options(ohci_rt->SHOW_PARENT);
  for (i=0; i<USB_OHCI_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c*)SIM->get_param(pname, ohci);
    ohci_rt->add(port);
    device = (bx_param_enum_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
    options = (bx_param_string_c*)port->get_by_name("options");
    options->set_enable_handler(usb_param_enable_handler);
    over_current = (bx_param_bool_c*)port->get_by_name("over_current");
    over_current->set_handler(usb_param_oc_handler);
    BX_OHCI_THIS hub.usb_port[i].device = NULL;
    BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs = 0;
    BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.csc = 0;
  }

  // register handler for correct device connect handling after runtime config
  BX_OHCI_THIS hub.rt_conf_id = SIM->register_runtime_config_handler(BX_OHCI_THIS_PTR, runtime_config_handler);
  BX_OHCI_THIS hub.device_change = 0;
  BX_OHCI_THIS packets = NULL;

  BX_INFO(("USB OHCI initialized"));
}

void bx_usb_ohci_c::reset(unsigned type)
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
        BX_OHCI_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  BX_OHCI_THIS reset_hc();
}

void bx_usb_ohci_c::reset_hc()
{
  int i;
  char pname[6];

  // reset locals
  BX_OHCI_THIS hub.ohci_done_count = 7;

  // HcRevision
  BX_OHCI_THIS hub.op_regs.HcRevision         = 0x0110;

  // HcControl
  BX_OHCI_THIS hub.op_regs.HcControl.reserved  =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.rwe       =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.rwc       =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.ir        =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.hcfs      =          OHCI_USB_RESET;
  BX_OHCI_THIS hub.op_regs.HcControl.ble       =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.cle       =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.ie        =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.ple       =          0;
  BX_OHCI_THIS hub.op_regs.HcControl.cbsr      =          0;

  // HcCommandStatus
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved0 = 0x000000;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.soc       =        0;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved1 = 0x000000;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr       =        0;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf       =        0;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf       =        0;
  BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr       =        0;

  // HcInterruptStatus
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus  = 0x00000000;

  // HcInterruptEnable
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable  = OHCI_INTR_MIE;

  // HcHCCA
  BX_OHCI_THIS hub.op_regs.HcHCCA             = 0x00000000;

  // HcPeriodCurrentED
  BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED  = 0x00000000;

  // HcControlHeadED
  BX_OHCI_THIS hub.op_regs.HcControlHeadED    = 0x00000000;

  // HcControlCurrentED
  BX_OHCI_THIS hub.op_regs.HcControlCurrentED = 0x00000000;

  // HcBulkHeadED
  BX_OHCI_THIS hub.op_regs.HcBulkHeadED       = 0x00000000;

  // HcBulkCurrentED
  BX_OHCI_THIS hub.op_regs.HcBulkCurrentED    = 0x00000000;

  // HcDoneHead
  BX_OHCI_THIS hub.op_regs.HcDoneHead         = 0x00000000;

  // HcFmInterval
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fit      =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps    =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.reserved =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fi       = 0x2EDF;

  // HcFmRemaining
  BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle   =      0;

  // HcFmNumber
  BX_OHCI_THIS hub.op_regs.HcFmNumber         = 0x00000000;

  // HcPeriodicStart
  BX_OHCI_THIS hub.op_regs.HcPeriodicStart    = 0x00000000;

  // HcLSThreshold
  BX_OHCI_THIS hub.op_regs.HcLSThreshold      = 0x0628;

  // HcRhDescriptorA
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt   = 0x10;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.reserved =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp     =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm     =    1;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.dt       =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps      =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm      =    1;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp      =    USB_OHCI_PORTS;

  // HcRhDescriptorB
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm     = ((1 << USB_OHCI_PORTS) - 1) << 1;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr       = 0x0000;

  // HcRhStatus
  BX_OHCI_THIS hub.op_regs.HcRhStatus.crwe      = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved0 = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic      = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.lpsc      = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe      = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved1 = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.oci       = 0;
  BX_OHCI_THIS hub.op_regs.HcRhStatus.lps       = 0;

  // HcRhPortStatus[x]
  for (i=0; i<USB_OHCI_PORTS; i++) {
    reset_port(i);
    if (BX_OHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
    } else {
      set_connect_status(i, 1);
    }
  }

  while (BX_OHCI_THIS packets != NULL) {
    usb_cancel_packet(&BX_OHCI_THIS packets->packet);
    remove_async_packet(&BX_OHCI_THIS packets, BX_OHCI_THIS packets);
  }
}

void bx_usb_ohci_c::reset_port(int p)
{
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved0 = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prsc      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ocic      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pssc      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pesc      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved1 = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.lsda      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps       = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved2 = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prs       = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.poci      = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pss       = 0;
  BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pes       = 0;
}

void bx_usb_ohci_c::register_state(void)
{
  unsigned i;
  char portnum[8];
  bx_list_c *hub, *port, *reg;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "usb_ohci", "USB OHCI State");
  hub = new bx_list_c(list, "hub");
  reg = new bx_list_c(hub, "HcControl");
  BXRS_PARAM_BOOL(reg, rwe, BX_OHCI_THIS hub.op_regs.HcControl.rwe);
  BXRS_PARAM_BOOL(reg, rwc, BX_OHCI_THIS hub.op_regs.HcControl.rwc);
  BXRS_PARAM_BOOL(reg, ir, BX_OHCI_THIS hub.op_regs.HcControl.ir);
  BXRS_HEX_PARAM_FIELD(reg, hcfs, BX_OHCI_THIS hub.op_regs.HcControl.hcfs);
  BXRS_PARAM_BOOL(reg, ble, BX_OHCI_THIS hub.op_regs.HcControl.ble);
  BXRS_PARAM_BOOL(reg, cle, BX_OHCI_THIS hub.op_regs.HcControl.cle);
  BXRS_PARAM_BOOL(reg, ie, BX_OHCI_THIS hub.op_regs.HcControl.ie);
  BXRS_PARAM_BOOL(reg, ple, BX_OHCI_THIS hub.op_regs.HcControl.ple);
  BXRS_HEX_PARAM_FIELD(reg, cbsr, BX_OHCI_THIS hub.op_regs.HcControl.cbsr);
  reg = new bx_list_c(hub, "HcCommandStatus");
  BXRS_HEX_PARAM_FIELD(reg, soc, BX_OHCI_THIS hub.op_regs.HcCommandStatus.soc);
  BXRS_PARAM_BOOL(reg, ocr, BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr);
  BXRS_PARAM_BOOL(reg, blf, BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf);
  BXRS_PARAM_BOOL(reg, clf, BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf);
  BXRS_PARAM_BOOL(reg, hcr, BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr);
  BXRS_HEX_PARAM_FIELD(hub, HcInterruptStatus, BX_OHCI_THIS hub.op_regs.HcInterruptStatus);
  BXRS_HEX_PARAM_FIELD(hub, HcInterruptEnable, BX_OHCI_THIS hub.op_regs.HcInterruptEnable);
  BXRS_HEX_PARAM_FIELD(hub, HcHCCA, BX_OHCI_THIS hub.op_regs.HcHCCA);
  BXRS_HEX_PARAM_FIELD(hub, HcPeriodCurrentED, BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED);
  BXRS_HEX_PARAM_FIELD(hub, HcControlHeadED, BX_OHCI_THIS hub.op_regs.HcControlHeadED);
  BXRS_HEX_PARAM_FIELD(hub, HcControlCurrentED, BX_OHCI_THIS hub.op_regs.HcControlCurrentED);
  BXRS_HEX_PARAM_FIELD(hub, HcBulkHeadED, BX_OHCI_THIS hub.op_regs.HcBulkHeadED);
  BXRS_HEX_PARAM_FIELD(hub, HcBulkCurrentED, BX_OHCI_THIS hub.op_regs.HcBulkCurrentED);
  BXRS_HEX_PARAM_FIELD(hub, HcDoneHead, BX_OHCI_THIS hub.op_regs.HcDoneHead);
  reg = new bx_list_c(hub, "HcFmInterval");
  BXRS_PARAM_BOOL(reg, fit, BX_OHCI_THIS hub.op_regs.HcFmInterval.fit);
  BXRS_HEX_PARAM_FIELD(reg, fsmps, BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps);
  BXRS_HEX_PARAM_FIELD(reg, fi, BX_OHCI_THIS hub.op_regs.HcFmInterval.fi);
  BXRS_PARAM_BOOL(hub, HcFmRemainingToggle, BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle);
  BXRS_HEX_PARAM_FIELD(hub, HcFmNumber, BX_OHCI_THIS hub.op_regs.HcFmNumber);
  BXRS_HEX_PARAM_FIELD(hub, HcPeriodicStart, BX_OHCI_THIS hub.op_regs.HcPeriodicStart);
  reg = new bx_list_c(hub, "HcRhDescriptorA");
  BXRS_HEX_PARAM_FIELD(reg, potpgt, BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt);
  BXRS_PARAM_BOOL(reg, nocp, BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp);
  BXRS_PARAM_BOOL(reg, ocpm, BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm);
  BXRS_PARAM_BOOL(reg, nps, BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps);
  BXRS_PARAM_BOOL(reg, psm, BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm);
  reg = new bx_list_c(hub, "HcRhDescriptorB");
  BXRS_HEX_PARAM_FIELD(reg, ppcm, BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm);
  BXRS_HEX_PARAM_FIELD(reg, dr, BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr);
  reg = new bx_list_c(hub, "HcRhStatus");
  BXRS_PARAM_BOOL(reg, crwe, BX_OHCI_THIS hub.op_regs.HcRhStatus.crwe);
  BXRS_PARAM_BOOL(reg, ocic, BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic);
  BXRS_PARAM_BOOL(reg, lpsc, BX_OHCI_THIS hub.op_regs.HcRhStatus.lpsc);
  BXRS_PARAM_BOOL(reg, drwe, BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe);
  BXRS_PARAM_BOOL(reg, oci, BX_OHCI_THIS hub.op_regs.HcRhStatus.oci);
  BXRS_PARAM_BOOL(reg, lps, BX_OHCI_THIS hub.op_regs.HcRhStatus.lps);
  for (i=0; i<USB_OHCI_PORTS; i++) {
    sprintf(portnum, "port%d", i+1);
    port = new bx_list_c(hub, portnum);
    reg = new bx_list_c(port, "HcRhPortStatus");
    BXRS_PARAM_BOOL(reg, prsc, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.prsc);
    BXRS_PARAM_BOOL(reg, ocic, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ocic);
    BXRS_PARAM_BOOL(reg, pssc, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pssc);
    BXRS_PARAM_BOOL(reg, pesc, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pesc);
    BXRS_PARAM_BOOL(reg, csc, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.csc);
    BXRS_PARAM_BOOL(reg, lsda, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.lsda);
    BXRS_PARAM_BOOL(reg, pps, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pps);
    BXRS_PARAM_BOOL(reg, prs, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.prs);
    BXRS_PARAM_BOOL(reg, poci, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.poci);
    BXRS_PARAM_BOOL(reg, pss, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pss);
    BXRS_PARAM_BOOL(reg, pes, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pes);
    BXRS_PARAM_BOOL(reg, ccs, BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  BXRS_DEC_PARAM_FIELD(hub, ohci_done_count, BX_OHCI_THIS hub.ohci_done_count);
  BXRS_PARAM_BOOL(hub, use_control_head, BX_OHCI_THIS hub.use_control_head);
  BXRS_PARAM_BOOL(hub, use_bulk_head, BX_OHCI_THIS hub.use_bulk_head);
  BXRS_DEC_PARAM_FIELD(hub, sof_time, BX_OHCI_THIS hub.sof_time);
  // TODO: handle async packets
  register_pci_state(hub);
}

void bx_usb_ohci_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(NULL);
  for (int j=0; j<USB_OHCI_PORTS; j++) {
    if (BX_OHCI_THIS hub.usb_port[j].device != NULL) {
      BX_OHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

int ohci_event_handler(int event, void *ptr, void *dev, int port);

void bx_usb_ohci_c::init_device(Bit8u port, bx_list_c *portconf)
{
  char pname[BX_PATHNAME_LEN];

  if (DEV_usb_init_device(portconf, BX_OHCI_THIS_PTR, &BX_OHCI_THIS hub.usb_port[port].device, ohci_event_handler, port)) {
    if (set_connect_status(port, 1)) {
      portconf->get_by_name("options")->set_enabled(0);
      sprintf(pname, "usb_ohci.hub.port%d.device", port+1);
      bx_list_c *sr_list = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
      BX_OHCI_THIS hub.usb_port[port].device->register_state(sr_list);
    } else {
      ((bx_param_enum_c*)portconf->get_by_name("device"))->set_by_name("none");
      ((bx_param_string_c*)portconf->get_by_name("options"))->set("none");
      ((bx_param_bool_c*)portconf->get_by_name("over_current"))->set(0);
      set_connect_status(port, 0);
    }
  }
}

void bx_usb_ohci_c::remove_device(Bit8u port)
{
  if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_OHCI_THIS hub.usb_port[port].device;
    BX_OHCI_THIS hub.usb_port[port].device = NULL;
  }
}

void bx_usb_ohci_c::update_irq()
{
  bool level = 0;

  if ((BX_OHCI_THIS hub.op_regs.HcInterruptEnable & OHCI_INTR_MIE) &&
      (BX_OHCI_THIS hub.op_regs.HcInterruptStatus & BX_OHCI_THIS hub.op_regs.HcInterruptEnable)) {
      level = 1;
      BX_DEBUG(("Interrupt Fired."));
  }
  DEV_pci_set_irq(BX_OHCI_THIS hub.devfunc, BX_OHCI_THIS pci_conf[0x3d], level);
}

void bx_usb_ohci_c::set_interrupt(Bit32u value)
{
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus |= value;
  update_irq();
}

bool bx_usb_ohci_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
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

  Bit32u  offset = (Bit32u)(addr - BX_OHCI_THIS pci_bar[0].addr);
  switch (offset) {
    case 0x00: // HcRevision
      val = BX_OHCI_THIS hub.op_regs.HcRevision;
      break;

    case 0x04: // HcControl
      val =   (BX_OHCI_THIS hub.op_regs.HcControl.reserved     << 11)
            | (BX_OHCI_THIS hub.op_regs.HcControl.rwe      ? 1 << 10 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.rwc      ? 1 << 9 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.ir       ? 1 << 8 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.hcfs         << 6)
            | (BX_OHCI_THIS hub.op_regs.HcControl.ble      ? 1 << 5 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.cle      ? 1 << 4 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.ie       ? 1 << 3 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.ple      ? 1 << 2 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcControl.cbsr         << 0);
      break;

    case 0x08: // HcCommandStatus
      val =   (BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved0     << 18)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.soc           << 16)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved1     << 4)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr       ? 1 << 3 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf       ? 1 << 2 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf       ? 1 << 1 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr       ? 1 << 0 : 0);
      break;

    case 0x0C: // HcInterruptStatus
      val = BX_OHCI_THIS hub.op_regs.HcInterruptStatus;
      break;

    case 0x10: // HcInterruptEnable
    case 0x14: // HcInterruptDisable (reading this one returns that one)
      val = BX_OHCI_THIS hub.op_regs.HcInterruptEnable;
      break;

    case 0x18: // HcHCCA
      val = BX_OHCI_THIS hub.op_regs.HcHCCA;
      break;

    case 0x1C: // HcPeriodCurrentED
      val = BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED;
      break;

    case 0x20: // HcControlHeadED
      val = BX_OHCI_THIS hub.op_regs.HcControlHeadED;
      break;

    case 0x24: // HcControlCurrentED
      val = BX_OHCI_THIS hub.op_regs.HcControlCurrentED;
      break;

    case 0x28: // HcBulkHeadED
      val = BX_OHCI_THIS hub.op_regs.HcBulkHeadED;
      break;

    case 0x2C: // HcBulkCurrentED
      val = BX_OHCI_THIS hub.op_regs.HcBulkCurrentED;
      break;

    case 0x30: // HcDoneHead
      val = BX_OHCI_THIS hub.op_regs.HcDoneHead;
      break;

    case 0x34: // HcFmInterval
      val =   (BX_OHCI_THIS hub.op_regs.HcFmInterval.fit      ? 1 << 31 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps        << 16)
            | (BX_OHCI_THIS hub.op_regs.HcFmInterval.reserved     << 14)
            | (BX_OHCI_THIS hub.op_regs.HcFmInterval.fi           << 0);
      break;

    case 0x38: // HcFmRemaining
      val = get_frame_remaining();
      break;

    case 0x3C: // HcFmNumber
      val = BX_OHCI_THIS hub.op_regs.HcFmNumber;
      break;

    case 0x40: // HcPeriodicStart
      val = BX_OHCI_THIS hub.op_regs.HcPeriodicStart;
      break;

    case 0x44: // HcLSThreshold
      val = BX_OHCI_THIS hub.op_regs.HcLSThreshold;
      break;

    case 0x48: // HcRhDescriptorA
      val =   (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt       << 24)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.reserved     << 13)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp     ? 1 << 12 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm     ? 1 << 11 : 0)
            | 0 //BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.dt       << 10
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps      ? 1 <<  9 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm      ? 1 <<  8 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp          <<  0);
      break;

    case 0x4C: // HcRhDescriptorB
      val =   (BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm << 16)
            | (BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr   << 0);
      break;

    case 0x50: // HcRhStatus
      val =   (BX_OHCI_THIS hub.op_regs.HcRhStatus.crwe      ? 1 << 31 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved0     << 18)
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic      ? 1 << 17 : 0)
            | 0 //BX_OHCI_THIS hub.op_regs.HcRhStatus.lpsc      << 16
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe      ? 1 << 15 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved1     <<  2)
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.oci       ? 1 <<  1 : 0)
            | (BX_OHCI_THIS hub.op_regs.HcRhStatus.lps       ? 1 <<  0 : 0);
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
      val =   (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved0  << 21)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prsc      ? (1 << 20) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ocic      ? (1 << 19) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pssc      ? (1 << 18) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pesc      ? (1 << 17) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.csc       ? (1 << 16) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved1     << 10)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.lsda      ? (1 <<  9) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps       ? (1 <<  8) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.reserved2     <<  5)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prs       ? (1 <<  4) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.poci      ? (1 <<  3) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pss       ? (1 <<  2) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pes       ? (1 <<  1) : 0)
            | (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ccs       ? (1 <<  0) : 0);
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

bool bx_usb_ohci_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u value = *((Bit32u *) data);
  Bit32u  offset = (Bit32u)addr - BX_OHCI_THIS pci_bar[0].addr;
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
      org_state = BX_OHCI_THIS hub.op_regs.HcControl.hcfs;
      BX_OHCI_THIS hub.op_regs.HcControl.rwe      = (value & (1<<10)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.rwc      = (value & (1<< 9)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ir       = (value & (1<< 8)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.hcfs     = (value & (3<< 6)) >>  6;
      BX_OHCI_THIS hub.op_regs.HcControl.ble      = (value & (1<< 5)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.cle      = (value & (1<< 4)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ie       = (value & (1<< 3)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ple      = (value & (1<< 2)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.cbsr     = (value & (3<< 0)) >>  0;
      if (BX_OHCI_THIS hub.op_regs.HcControl.hcfs == OHCI_USB_OPERATIONAL) {
        BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle = 0;
        if (org_state != OHCI_USB_OPERATIONAL)
          BX_OHCI_THIS hub.use_control_head = BX_OHCI_THIS hub.use_bulk_head = 1;
      }
      break;

    case 0x08: // HcCommandStatus
      if (value & 0xFFFCFFF0)
        BX_ERROR(("Write to a reserved field in HcCommandStatus"));
      if (value & (3<<16))
        BX_ERROR(("Write to R/O field: HcCommandStatus.soc"));
      if (value & (1<< 3)) BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr = 1;
      if (value & (1<< 2)) BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf = 1;
      if (value & (1<< 1)) BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf = 1;
      if (value & (1<< 0)) {
        BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr = 1;
        BX_OHCI_THIS reset_hc();
        BX_OHCI_THIS hub.op_regs.HcControl.hcfs = OHCI_USB_SUSPEND;
        for (unsigned i=0; i<USB_OHCI_PORTS; i++)
          if (BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs && (BX_OHCI_THIS hub.usb_port[i].device != NULL))
            BX_OHCI_THIS hub.usb_port[i].device->usb_send_msg(USB_MSG_RESET);
      }
      break;

    case 0x0C: // HcInterruptStatus /// all are WC
      if (value & 0xBFFFFF80)
        BX_DEBUG(("Write to a reserved field in HcInterruptStatus"));
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus &= ~value;
      update_irq();
      break;

    case 0x10: // HcInterruptEnable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptEnable"));
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable |= (value & 0xC000007F);
      update_irq();
      break;

    case 0x14: // HcInterruptDisable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptDisable"));
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable &= ~value;
      update_irq();
      break;

    case 0x18: // HcHCCA
      // the HCD can write 0xFFFFFFFF to this register to see what the alignement is
      //  by reading back the amount and seeing how many lower bits are clear.
      if ((value & 0x000000FF) && (value != 0xFFFFFFFF))
        BX_ERROR(("Write to lower byte of HcHCCA non zero."));
      BX_OHCI_THIS hub.op_regs.HcHCCA = (value & 0xFFFFFF00);
      break;

    case 0x1C: // HcPeriodCurrentED
      BX_ERROR(("Write to HcPeriodCurrentED not allowed."));
      break;

    case 0x20: // HcControlHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlHeadED non zero."));
      BX_OHCI_THIS hub.op_regs.HcControlHeadED = (value & 0xFFFFFFF0);
      break;

    case 0x24: // HcControlCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlCurrentED non zero."));
      BX_OHCI_THIS hub.op_regs.HcControlCurrentED = (value & 0xFFFFFFF0);
      break;

    case 0x28: // HcBulkHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkHeadED non zero."));
      BX_OHCI_THIS hub.op_regs.HcBulkHeadED = (value & 0xFFFFFFF0);
      break;

    case 0x2C: // HcBulkCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkCurrentED non zero."));
      BX_OHCI_THIS hub.op_regs.HcBulkCurrentED = (value & 0xFFFFFFF0);
      break;

    case 0x30: // HcDoneHead
      BX_ERROR(("Write to HcDoneHead not allowed."));
      break;

    case 0x34: // HcFmInterval
      if (value & 0x0000C000)
        BX_ERROR(("Write to a reserved field in HcFmInterval."));
      BX_OHCI_THIS hub.op_regs.HcFmInterval.fit      = (value & (1<<31)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps    = (value & 0x7FFF0000) >> 16;
      BX_OHCI_THIS hub.op_regs.HcFmInterval.fi       = (value & 0x00003FFF) >> 0;
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
      BX_OHCI_THIS hub.op_regs.HcPeriodicStart = (value & 0x00003FFF);
      break;

    case 0x44: // HcLSThreshold
      BX_OHCI_THIS hub.op_regs.HcLSThreshold = (value & 0x00000FFF);
      break;

    case 0x48: // HcRhDescriptorA
      if (value & 0x00FFE000)
        BX_ERROR(("Write to a reserved field in HcRhDescriptorA."));
      if ((value & 0x000000FF) != BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp)
        BX_ERROR(("Write to HcRhDescriptorA.ndp not allowed."));
      if (value & (1<<10))
        BX_ERROR(("Write to HcRhDescriptorA.dt not allowed."));
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt   = (value & 0xFF000000) >> 24;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp     = (value & (1<<12)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm     = (value & (1<<11)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps      = (value & (1<< 9)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm      = (value & (1<< 8)) ? 1 : 0;
      if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {

        BX_INFO(("Ben: BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0"));
        // all ports have power, etc.
        // BX_USB_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1
        //  Call a routine to set each ports dword (LS, Connected, etc.)

      } else {

        BX_INFO(("Ben: BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 1"));
        // only ports with bit set in rhstatus have power, etc.
        //  Call a routine to set each ports dword (LS, Connected, etc.)

      }
      break;

    case 0x4C: // HcRhDescriptorB
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm = (value & 0xFFFF0000) >> 16;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr   = (value & 0x0000FFFF) >>  0;
      break;

    case 0x50: { // HcRhStatus
      if (value & 0x7FFC7FFC)
        BX_ERROR(("Write to a reserved field in HcRhStatus."));
      if (value & (1<<1))
        BX_ERROR(("Write to HcRhStatus.oci not allowed."));
      // which one of these two takes presidence?
      if (value & (1<<31)) BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe = 0;
      if (value & (1<<15)) BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe = 1;

      if (value & (1<<17)) BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic = 1;
      if (value & (1<<16)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<USB_OHCI_PORTS; p++)
            BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
        } else {
          for (p=0; p<USB_OHCI_PORTS; p++)
            if ((BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)) == 0)
              BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
        }
      }
      if (value & (1<<0)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<USB_OHCI_PORTS; p++)
            BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 0;
        } else {
          for (p=0; p<USB_OHCI_PORTS; p++)
            if (!(BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)))
              BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 0;
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
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pes = 0;
      if (value & (1<<1)) {
        if (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ccs == 0)
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.csc = 1;
        else
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pes = 1;
      }
      if (value & (1<<2)) {
        if (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ccs == 0)
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.csc = 1;
        else
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pss = 1;
      }
//      if (value & (1<<3))
//        if (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pss)
//          ; // do a resume (or test this in the timer code and do the resume there)
      if (value & (1<<4)) {
        if (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ccs == 0)
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.csc = 1;
        else {
          reset_port(p);
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pes = 1;
          BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prsc = 1;
          // are we are currently connected/disconnected
          if (BX_OHCI_THIS hub.usb_port[p].device != NULL) {
            BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.lsda =
              (BX_OHCI_THIS hub.usb_port[p].device->get_speed() == USB_SPEED_LOW);
            set_connect_status(p, 1);
            BX_OHCI_THIS hub.usb_port[p].device->usb_send_msg(USB_MSG_RESET);
          }
          set_interrupt(OHCI_INTR_RHSC);
        }
      }
      if (value & (1<<8))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
      if (value & (1<<9))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 0;
      if (value & (1<<16))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.csc = (value & ((1<<4) | (1<<1) | (1<<2))) ? 1 : 0;
      if (value & (1<<17))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pesc = 0;
      if (value & (1<<18))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pssc = 0;
      if (value & (1<<19))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.ocic = 0;
      if (value & (1<<20))
        BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.prsc = 0;
      break;
    }

    default:
      BX_ERROR(("unsupported write to address=0x%08X, val = 0x%08X!", (Bit32u)addr, value));
      break;
  }

  return 1;
}

Bit32u bx_usb_ohci_c::get_frame_remaining(void)
{
  Bit16u bit_time, fr;

  bit_time = (Bit16u)((bx_pc_system.time_usec() - BX_OHCI_THIS hub.sof_time) * 12);
  if ((BX_OHCI_THIS hub.op_regs.HcControl.hcfs != OHCI_USB_OPERATIONAL) ||
      (bit_time > BX_OHCI_THIS hub.op_regs.HcFmInterval.fi)) {
    fr = 0;
  } else {
    fr = BX_OHCI_THIS hub.op_regs.HcFmInterval.fi - bit_time;
  }
  return (BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle << 31) | fr;
}

void bx_usb_ohci_c::usb_frame_handler(void *this_ptr)
{
  bx_usb_ohci_c *class_ptr = (bx_usb_ohci_c *) this_ptr;
  class_ptr->usb_frame_timer();
}

// Called once every 1mS
void bx_usb_ohci_c::usb_frame_timer(void)
{
  struct OHCI_ED cur_ed;
  Bit32u address, ed_address;
  Bit16u zero = 0;

  if (BX_OHCI_THIS hub.op_regs.HcControl.hcfs == OHCI_USB_OPERATIONAL) {
    // set remaining to the interval amount.
    BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle = BX_OHCI_THIS hub.op_regs.HcFmInterval.fit;
    BX_OHCI_THIS hub.sof_time = bx_pc_system.time_usec();

    // The Frame Number Register is incremented
    //  every time bit 15 is changed (at 0x8000 or 0x0000), fno is fired.
    BX_OHCI_THIS hub.op_regs.HcFmNumber++;
    BX_OHCI_THIS hub.op_regs.HcFmNumber &= 0xffff;
    DEV_MEM_WRITE_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcHCCA + 0x80, 2, (Bit8u *) &BX_OHCI_THIS hub.op_regs.HcFmNumber);
    DEV_MEM_WRITE_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcHCCA + 0x82, 2, (Bit8u *) &zero);
    if ((BX_OHCI_THIS hub.op_regs.HcFmNumber == 0x8000) || (BX_OHCI_THIS hub.op_regs.HcFmNumber == 0x0000)) {
      set_interrupt(OHCI_INTR_FNO);
    }

    //
    set_interrupt(OHCI_INTR_SF);

    // if interrupt delay (done_count) == 0, and status.wdh == 0, then update the donehead fields.
    //BX_DEBUG(("done_count = %d, status.wdh = %d", BX_OHCI_THIS hub.ohci_done_count,
    //          ((BX_OHCI_THIS hub.op_regs.HcInterruptStatus & OHCI_INTR_WD) > 0)));
    if ((BX_OHCI_THIS hub.ohci_done_count == 0) && ((BX_OHCI_THIS hub.op_regs.HcInterruptStatus & OHCI_INTR_WD) == 0)) {
      Bit32u temp = BX_OHCI_THIS hub.op_regs.HcDoneHead;
      if (BX_OHCI_THIS hub.op_regs.HcInterruptStatus & BX_OHCI_THIS hub.op_regs.HcInterruptEnable)
        temp |= 1;
      BX_DEBUG(("Updating the hcca.DoneHead field to 0x%08X and setting the wdh flag", temp));
      DEV_MEM_WRITE_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcHCCA + 0x84, 4, (Bit8u *) &temp);
      BX_OHCI_THIS hub.op_regs.HcDoneHead = 0;
      BX_OHCI_THIS hub.ohci_done_count = 7;
      set_interrupt(OHCI_INTR_WD);
    }

    // if (6 >= done_count > 0) then decrement done_count
    if ((BX_OHCI_THIS hub.ohci_done_count != 7) && (BX_OHCI_THIS hub.ohci_done_count > 0))
      BX_OHCI_THIS hub.ohci_done_count--;

    BX_OHCI_THIS process_lists();

    // do the ED's in the interrupt table
    if (BX_OHCI_THIS hub.op_regs.HcControl.ple) {
      address = BX_OHCI_THIS hub.op_regs.HcHCCA + ((BX_OHCI_THIS hub.op_regs.HcFmNumber & 0x1F) * 4);
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

void bx_usb_ohci_c::process_lists(void)
{
  struct OHCI_ED cur_ed;

  // TODO:  Rather than just comparing .fr to < 8000 here, and < 4000 below, see the statement on 
  //   page 45 of the OHCI specs:
  // "When a new frame starts, the Host Controller processes control and bulk Endpoint Descriptors until 
  //  the Remaining field of the HcFmRemaining register is less than or equal to the Start field of the 
  //  HcPeriodicStart register"

  // if the control list is enabled *and* the control list filled bit is set, do a control list ED
  if (BX_OHCI_THIS hub.op_regs.HcControl.cle) {
    if (BX_OHCI_THIS hub.use_control_head) {
      BX_OHCI_THIS hub.op_regs.HcControlCurrentED = 0;
      BX_OHCI_THIS hub.use_control_head = 0;
    }
    if (!BX_OHCI_THIS hub.op_regs.HcControlCurrentED && BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf) {
      BX_OHCI_THIS hub.op_regs.HcControlCurrentED = BX_OHCI_THIS hub.op_regs.HcControlHeadED;
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf = 0;
    }
    while (BX_OHCI_THIS hub.op_regs.HcControlCurrentED) {
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcControlCurrentED,      4, (Bit8u*) &cur_ed.dword0);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcControlCurrentED +  4, 4, (Bit8u*) &cur_ed.dword1);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcControlCurrentED +  8, 4, (Bit8u*) &cur_ed.dword2);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcControlCurrentED + 12, 4, (Bit8u*) &cur_ed.dword3);
      process_ed(&cur_ed, BX_OHCI_THIS hub.op_regs.HcControlCurrentED);
      BX_OHCI_THIS hub.op_regs.HcControlCurrentED = ED_GET_NEXTED(&cur_ed);
      if (get_frame_remaining() < 8000)
        break;
    }
  }

  // if the bulk list is enabled *and* the bulk list filled bit is set, do a bulk list ED
  if (BX_OHCI_THIS hub.op_regs.HcControl.ble) {
    if (BX_OHCI_THIS hub.use_bulk_head) {
      BX_OHCI_THIS hub.op_regs.HcBulkCurrentED = 0;
      BX_OHCI_THIS hub.use_bulk_head = 0;
    }
    if (!BX_OHCI_THIS hub.op_regs.HcBulkCurrentED && BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf) {
      BX_OHCI_THIS hub.op_regs.HcBulkCurrentED = BX_OHCI_THIS hub.op_regs.HcBulkHeadED;
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf = 0;
    }
    while (BX_OHCI_THIS hub.op_regs.HcBulkCurrentED) {
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcBulkCurrentED,      4, (Bit8u*) &cur_ed.dword0);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcBulkCurrentED +  4, 4, (Bit8u*) &cur_ed.dword1);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcBulkCurrentED +  8, 4, (Bit8u*) &cur_ed.dword2);
      DEV_MEM_READ_PHYSICAL(BX_OHCI_THIS hub.op_regs.HcBulkCurrentED + 12, 4, (Bit8u*) &cur_ed.dword3);
      if (process_ed(&cur_ed, BX_OHCI_THIS hub.op_regs.HcBulkCurrentED)) {
        BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf = 1;
      }
      BX_OHCI_THIS hub.op_regs.HcBulkCurrentED = ED_GET_NEXTED(&cur_ed);
      if (get_frame_remaining() < 4000)
        break;
    }
  }
}

bool bx_usb_ohci_c::process_ed(struct OHCI_ED *ed, const Bit32u ed_address)
{
  struct OHCI_TD cur_td;
  bool ret = 0;

  if (!ED_GET_H(ed) && !ED_GET_K(ed) && (ED_GET_HEADP(ed) != ED_GET_TAILP(ed))) {
    // if the isochronous is enabled and ed is a isochronous, do TD
    if (ED_GET_F(ed)) {
      if (BX_OHCI_THIS hub.op_regs.HcControl.ie) {
        // load and do a isochronous TD list
        BX_DEBUG(("Found a valid ED that points to an isochronous TD"));
        // we currently ignore ISO TD's
      }
    } else {
      BX_DEBUG(("Found a valid ED that points to an control/bulk/int TD"));
      ret = 1;
      while (ED_GET_HEADP(ed) != ED_GET_TAILP(ed)) {
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed),      4, (Bit8u*) &cur_td.dword0);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) +  4, 4, (Bit8u*) &cur_td.dword1);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) +  8, 4, (Bit8u*) &cur_td.dword2);
        DEV_MEM_READ_PHYSICAL(ED_GET_HEADP(ed) + 12, 4, (Bit8u*) &cur_td.dword3);
        BX_DEBUG(("Head: 0x%08X  Tail: 0x%08X  Next: 0x%08X", ED_GET_HEADP(ed), ED_GET_TAILP(ed), TD_GET_NEXTTD(&cur_td)));
        if (process_td(&cur_td, ed)) {
          const Bit32u temp = ED_GET_HEADP(ed);
          if (TD_GET_CC(&cur_td) < NotAccessed) {
            ED_SET_HEADP(ed, TD_GET_NEXTTD(&cur_td));
            TD_SET_NEXTTD(&cur_td, BX_OHCI_THIS hub.op_regs.HcDoneHead);
            BX_OHCI_THIS hub.op_regs.HcDoneHead = temp;
            if (TD_GET_DI(&cur_td) < BX_OHCI_THIS hub.ohci_done_count)
              BX_OHCI_THIS hub.ohci_done_count = TD_GET_DI(&cur_td);
          }
          DEV_MEM_WRITE_PHYSICAL(temp,      4, (Bit8u*) &cur_td.dword0);
          DEV_MEM_WRITE_PHYSICAL(temp +  4, 4, (Bit8u*) &cur_td.dword1);
          DEV_MEM_WRITE_PHYSICAL(temp +  8, 4, (Bit8u*) &cur_td.dword2);
        } else
          break;
      }
    }
    DEV_MEM_WRITE_PHYSICAL(ed_address +  8, 4, (Bit8u*) &ed->dword2);
  }
  return ret;
}

int ohci_event_handler(int event, void *ptr, void *dev, int port)
{
  if (dev != NULL) {
    return ((bx_usb_ohci_c *) dev)->event_handler(event, ptr, port);
  }
  return -1;
}

int bx_usb_ohci_c::event_handler(int event, void *ptr, int port)
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
      BX_OHCI_THIS process_lists();
      break;
    case USB_EVENT_WAKEUP:
      if (BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pss) {
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pss = 0;
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pssc = 1;
        intr = OHCI_INTR_RHSC;
      }
      if (BX_OHCI_THIS hub.op_regs.HcControl.hcfs == OHCI_USB_SUSPEND) {
        BX_OHCI_THIS hub.op_regs.HcControl.hcfs = OHCI_USB_RESUME;
        intr = OHCI_INTR_RD;
      }
      set_interrupt(intr);
      break;

    // host controller events start here
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

bool bx_usb_ohci_c::process_td(struct OHCI_TD *td, struct OHCI_ED *ed)
{
  unsigned pid = 0, len = 0, len1, len2;
  bool ret2 = 1;
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
    p->packet.toggle = (TD_GET_T(td) & 2) ? ((TD_GET_T(td) & 1) > 0) : (ED_GET_C(ed) > 0);
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
        if ((ret = BX_OHCI_THIS broadcast_packet(&p->packet)) >= 0)
          ret = 8;
        break;
      case USB_TOKEN_OUT:
        if (maxlen > 0)
          DEV_MEM_READ_PHYSICAL_DMA(TD_GET_CBP(td), maxlen, p->packet.data);
        ret = BX_OHCI_THIS broadcast_packet(&p->packet);
        break;
      case USB_TOKEN_IN:
        ret = BX_OHCI_THIS broadcast_packet(&p->packet);
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
    if (TD_GET_T(td) & 2) {
      TD_SET_T(td, TD_GET_T(td) ^ 1);
      ED_SET_C(ed, (TD_GET_T(td) & 1));
    } else
      ED_SET_C(ed, (ED_GET_C(ed) ^ 1));
    if ((pid != USB_TOKEN_OUT) || (ret == (int) len)) {
      TD_SET_CC(td, NoError);
      TD_SET_EC(td, 0);
    }
  } else {
    if (ret >= 0)
      TD_SET_CC(td, DataUnderrun);
    else {
      switch (ret) {
        case USB_RET_NODEV:  // (-1)
          TD_SET_CC(td, DeviceNotResponding);
          break;
        case USB_RET_NAK:    // (-2)
          ret2 = 0;
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

int bx_usb_ohci_c::broadcast_packet(USBPacket *p)
{
  int i, ret;

  ret = USB_RET_NODEV;
  for (i = 0; i < USB_OHCI_PORTS && ret == USB_RET_NODEV; i++) {
    if ((BX_OHCI_THIS hub.usb_port[i].device != NULL) &&
        (BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs)) {
      ret = BX_OHCI_THIS hub.usb_port[i].device->handle_packet(p);
    }
  }
  return ret;
}

void bx_usb_ohci_c::runtime_config_handler(void *this_ptr)
{
  if (this_ptr != NULL) {
    bx_usb_ohci_c *class_ptr = (bx_usb_ohci_c *) this_ptr;
    class_ptr->runtime_config();
  }
}

void bx_usb_ohci_c::runtime_config(void)
{
  int i;
  char pname[6];

  for (i = 0; i < USB_OHCI_PORTS; i++) {
    // device change support
    if ((BX_OHCI_THIS hub.device_change & (1 << i)) != 0) {
      if (!BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs) {
        sprintf(pname, "port%d", i + 1);
        init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
      } else {
        set_connect_status(i, 0);
      }
      BX_OHCI_THIS hub.device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_OHCI_THIS hub.usb_port[i].device != NULL) {
      BX_OHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// pci configuration space write callback handler
void bx_usb_ohci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8;

  if (((address >= 0x14) && (address <= 0x34)))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
//  Bit8u oldval = BX_OHCI_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x06; // (bit 0 is read only for this card) (we don't allow port IO)
        BX_OHCI_THIS pci_conf[address+i] = value8;
        break;
      case 0x3d: //
      case 0x3e: //
      case 0x3f: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      default:
        BX_OHCI_THIS pci_conf[address+i] = value8;
    }
  }
}

bool bx_usb_ohci_c::set_connect_status(Bit8u port, bool connected)
{
  const bool ccs_org = BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs;
  const bool pes_org = BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes;

  usb_device_c *device = BX_OHCI_THIS hub.usb_port[port].device;
  if (device != NULL) {
    if (connected) {
      switch (device->get_speed()) {
        case USB_SPEED_LOW:
          BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.lsda = 1;
          break;
        case USB_SPEED_FULL:
          BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.lsda = 0;
          break;
        case USB_SPEED_HIGH:
        case USB_SPEED_SUPER:
          BX_PANIC(("HC supports 'low' or 'full' speed devices only."));
          return 0;
        default:
          BX_PANIC(("USB device returned invalid speed value"));
          return 0;
      }
      BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs = 1;
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
      BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs = 0;
      BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes = 0;
      BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.lsda = 0;
      remove_device(port);
    }
    BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.csc |= (ccs_org != BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs);
    BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pesc |= (pes_org != BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes);

    // we changed the value of the port, so show it
    set_interrupt(OHCI_INTR_RHSC);
  }
  return connected;
}

// USB runtime parameter handler
Bit64s bx_usb_ohci_c::usb_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  if (set) {
    int portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bool empty = (val == 0);
    if ((portnum >= 0) && (portnum < USB_OHCI_PORTS)) {
      if (empty && BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_OHCI_THIS hub.device_change |= (1 << portnum);
      } else if (!empty && !BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_OHCI_THIS hub.device_change |= (1 << portnum);
      } else if (val != ((bx_param_enum_c*)param)->get()) {
        BX_ERROR(("usb_param_handler(): port #%d already in use", portnum+1));
        val = ((bx_param_enum_c*)param)->get();
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

// USB runtime parameter handler: over-current
Bit64s bx_usb_ohci_c::usb_param_oc_handler(bx_param_c *param, bool set, Bit64s val)
{
  int portnum;

  if (set && val) {
    if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp == 0) {
      portnum = atoi((param->get_parent())->get_name()+4) - 1;
      if ((portnum >= 0) && (portnum < USB_OHCI_PORTS)) {
        if (BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
          // is over current reported on a per-port basis?
          if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm == 1) {
            BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ocic = 1;
            BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.poci = 1;
            BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.pes = 0;
            BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.pesc = 1;
            BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.pps = 0;
            BX_DEBUG(("Over-current signaled on port #%d.", portnum + 1));
          // else over current is reported globally
          } else {
            BX_OHCI_THIS hub.op_regs.HcRhStatus.oci = 1;
            BX_DEBUG(("Global over-current signaled."));
          }
          set_interrupt(OHCI_INTR_RHSC);
        }
      } else {
        BX_ERROR(("Over-current: Bad portnum given: %d", portnum + 1));
      }
    } else {
      BX_DEBUG(("Over-current signaled with NOCP set."));
    }
  }

  return 0; // clear the indicator for next time
}

// USB runtime parameter enable handler
bool bx_usb_ohci_c::usb_param_enable_handler(bx_param_c *param, bool en)
{
  int portnum = atoi((param->get_parent())->get_name() + 4) - 1;
  if (en && (BX_OHCI_THIS hub.usb_port[portnum].device != NULL)) {
    en = 0;
  }
  return en;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI
