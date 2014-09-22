/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009       Benjamin D Lunt (fys at frontiernet net)
//                2009-2014  The Bochs Project
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

// Experimental USB OHCI adapter

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
      } else if (!strncmp(params[i], "port", 4)) {
        if (SIM->parse_usb_port_params(context, 0, params[i], BX_N_USB_OHCI_PORTS, base) < 0) {
          return -1;
        }
      } else if (!strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, 1, params[i], BX_N_USB_OHCI_PORTS, base) < 0) {
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
  SIM->write_usb_options(fp, BX_N_USB_OHCI_PORTS, base);
  return 0;
}

// device plugin entry points

int CDECL libusb_ohci_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSB_OHCI = new bx_usb_ohci_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_OHCI, BX_PLUGIN_USB_OHCI);
  // add new configuration parameter for the config interface
  SIM->init_usb_options("OHCI", "ohci", BX_N_USB_OHCI_PORTS);
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("usb_ohci", usb_ohci_options_parser, usb_ohci_options_save);
  return 0; // Success
}

void CDECL libusb_ohci_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("usb_ohci");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.usb");
  delete theUSB_OHCI;
  menu->remove("ohci");
}

// the device object

bx_usb_ohci_c::bx_usb_ohci_c()
{
  put("usb_ohci", "OHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_ohci_t));
  device_buffer = NULL;
  hub.frame_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_usb_ohci_c::~bx_usb_ohci_c()
{
  char pname[16];

  if (BX_OHCI_THIS device_buffer != NULL)
    delete [] BX_OHCI_THIS device_buffer;

  for (int i=0; i<BX_N_USB_OHCI_PORTS; i++) {
    sprintf(pname, "port%d.device", i+1);
    SIM->get_param_string(pname, SIM->get_param(BXPN_USB_OHCI))->set_handler(NULL);
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
  bx_param_string_c *device;

  // Read in values from config interface
  ohci = (bx_list_c*) SIM->get_param(BXPN_USB_OHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", ohci)->get()) {
    BX_INFO(("USB OHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_ohci"))->set(0);
    return;
  }

  BX_OHCI_THIS device_buffer = new Bit8u[65536];

  // Call our frame timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_OHCI_THIS hub.frame_timer_index =
                   bx_pc_system.register_timer(this, usb_frame_handler, 1000, 1,1, "ohci.frame_timer");

  BX_OHCI_THIS hub.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_OHCI_THIS hub.devfunc, BX_PLUGIN_USB_OHCI,
                            "Experimental USB OHCI");

  // initialize readonly registers
  init_pci_conf(0x11c1, 0x5803, 0x11, 0x0c0310, 0x00);

  BX_OHCI_THIS pci_base_address[0] = 0x0;
  BX_OHCI_THIS hub.ohci_done_count = 7;
  BX_OHCI_THIS hub.use_control_head = 0;
  BX_OHCI_THIS hub.use_bulk_head = 0;
  BX_OHCI_THIS hub.sof_time = 0;

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_OHCI_THIS hub.statusbar_id = bx_gui->register_statusitem("OHCI", 1);

  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *ohci_rt = new bx_list_c(usb_rt, "ohci", "OHCI Runtime Options");
  ohci_rt->set_options(ohci_rt->SHOW_PARENT);
  for (i=0; i<BX_N_USB_OHCI_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c*)SIM->get_param(pname, ohci);
    ohci_rt->add(port);
    device = (bx_param_string_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
    BX_OHCI_THIS hub.usb_port[i].device = NULL;
    BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs = 0;
    BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.csc = 0;
  }

  // register handler for correct device connect handling after runtime config
  SIM->register_runtime_config_handler(BX_OHCI_THIS_PTR, runtime_config_handler);
  BX_OHCI_THIS hub.device_change = 0;

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
      { 0x3d, BX_PCI_INTD },          // INT
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
  BX_OHCI_THIS hub.op_regs.HcControl.hcfs      =          0;
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
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp      =    BX_N_USB_OHCI_PORTS;

  // HcRhDescriptorB
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm     = ((1 << BX_N_USB_OHCI_PORTS) - 1) << 1;
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
  for (i=0; i<BX_N_USB_OHCI_PORTS; i++) {
    reset_port(i);
    if (BX_OHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
    } else {
      usb_set_connect_status(i, BX_OHCI_THIS hub.usb_port[i].device->get_type(), 1);
    }
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
  new bx_shadow_bool_c(reg, "rwe", &BX_OHCI_THIS hub.op_regs.HcControl.rwe);
  new bx_shadow_bool_c(reg, "rwc", &BX_OHCI_THIS hub.op_regs.HcControl.rwc);
  new bx_shadow_bool_c(reg, "ir", &BX_OHCI_THIS hub.op_regs.HcControl.ir);
  new bx_shadow_num_c(reg, "hcfs", &BX_OHCI_THIS hub.op_regs.HcControl.hcfs);
  new bx_shadow_bool_c(reg, "ble", &BX_OHCI_THIS hub.op_regs.HcControl.ble);
  new bx_shadow_bool_c(reg, "cle", &BX_OHCI_THIS hub.op_regs.HcControl.cle);
  new bx_shadow_bool_c(reg, "ie", &BX_OHCI_THIS hub.op_regs.HcControl.ie);
  new bx_shadow_bool_c(reg, "ple", &BX_OHCI_THIS hub.op_regs.HcControl.ple);
  new bx_shadow_num_c(reg, "cbsr", &BX_OHCI_THIS hub.op_regs.HcControl.cbsr);
  reg = new bx_list_c(hub, "HcCommandStatus");
  new bx_shadow_num_c(reg, "soc", &BX_OHCI_THIS hub.op_regs.HcCommandStatus.soc);
  new bx_shadow_bool_c(reg, "ocr", &BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr);
  new bx_shadow_bool_c(reg, "blf", &BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf);
  new bx_shadow_bool_c(reg, "clf", &BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf);
  new bx_shadow_bool_c(reg, "hcr", &BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr);
  new bx_shadow_num_c(hub, "HcInterruptStatus", &BX_OHCI_THIS hub.op_regs.HcInterruptStatus, BASE_HEX);
  new bx_shadow_num_c(hub, "HcInterruptEnable", &BX_OHCI_THIS hub.op_regs.HcInterruptEnable, BASE_HEX);
  new bx_shadow_num_c(hub, "HcHCCA", &BX_OHCI_THIS hub.op_regs.HcHCCA, BASE_HEX);
  new bx_shadow_num_c(hub, "HcPeriodCurrentED", &BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED, BASE_HEX);
  new bx_shadow_num_c(hub, "HcControlHeadED", &BX_OHCI_THIS hub.op_regs.HcControlHeadED, BASE_HEX);
  new bx_shadow_num_c(hub, "HcControlCurrentED", &BX_OHCI_THIS hub.op_regs.HcControlCurrentED, BASE_HEX);
  new bx_shadow_num_c(hub, "HcBulkHeadED", &BX_OHCI_THIS hub.op_regs.HcBulkHeadED, BASE_HEX);
  new bx_shadow_num_c(hub, "HcBulkCurrentED", &BX_OHCI_THIS hub.op_regs.HcBulkCurrentED, BASE_HEX);
  new bx_shadow_num_c(hub, "HcDoneHead", &BX_OHCI_THIS hub.op_regs.HcDoneHead, BASE_HEX);
  reg = new bx_list_c(hub, "HcFmInterval");
  new bx_shadow_bool_c(reg, "fit", &BX_OHCI_THIS hub.op_regs.HcFmInterval.fit);
  new bx_shadow_num_c(reg, "fsmps", &BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps);
  new bx_shadow_num_c(reg, "fi", &BX_OHCI_THIS hub.op_regs.HcFmInterval.fi, BASE_HEX);
  new bx_shadow_bool_c(hub, "HcFmRemainingToggle", &BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle);
  new bx_shadow_num_c(hub, "HcFmNumber", &BX_OHCI_THIS hub.op_regs.HcFmNumber, BASE_HEX);
  new bx_shadow_num_c(hub, "HcPeriodicStart", &BX_OHCI_THIS hub.op_regs.HcPeriodicStart, BASE_HEX);
  reg = new bx_list_c(hub, "HcRhDescriptorA");
  new bx_shadow_num_c(reg, "potpgt", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt, BASE_HEX);
  new bx_shadow_bool_c(reg, "nocp", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp);
  new bx_shadow_bool_c(reg, "ocpm", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm);
  new bx_shadow_bool_c(reg, "dt", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.dt);
  new bx_shadow_bool_c(reg, "nps", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps);
  new bx_shadow_bool_c(reg, "psm", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm);
  new bx_shadow_num_c(reg, "ndp", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp);
  reg = new bx_list_c(hub, "HcRhDescriptorB");
  new bx_shadow_num_c(reg, "ppcm", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm, BASE_HEX);
  new bx_shadow_num_c(reg, "dr", &BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr, BASE_HEX);
  reg = new bx_list_c(hub, "HcRhStatus");
  new bx_shadow_bool_c(reg, "crwe", &BX_OHCI_THIS hub.op_regs.HcRhStatus.crwe);
  new bx_shadow_bool_c(reg, "ocic", &BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic);
  new bx_shadow_bool_c(reg, "lpsc", &BX_OHCI_THIS hub.op_regs.HcRhStatus.lpsc);
  new bx_shadow_bool_c(reg, "drwe", &BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe);
  new bx_shadow_bool_c(reg, "oci", &BX_OHCI_THIS hub.op_regs.HcRhStatus.oci);
  new bx_shadow_bool_c(reg, "lps", &BX_OHCI_THIS hub.op_regs.HcRhStatus.lps);
  for (i=0; i<BX_N_USB_OHCI_PORTS; i++) {
    sprintf(portnum, "port%d", i+1);
    port = new bx_list_c(hub, portnum);
    reg = new bx_list_c(port, "HcRhPortStatus");
    new bx_shadow_bool_c(reg, "prsc", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.prsc);
    new bx_shadow_bool_c(reg, "ocic", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ocic);
    new bx_shadow_bool_c(reg, "pssc", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pssc);
    new bx_shadow_bool_c(reg, "pesc", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pesc);
    new bx_shadow_bool_c(reg, "csc", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.csc);
    new bx_shadow_bool_c(reg, "lsda", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.lsda);
    new bx_shadow_bool_c(reg, "pps", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pps);
    new bx_shadow_bool_c(reg, "prs", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.prs);
    new bx_shadow_bool_c(reg, "poci", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.poci);
    new bx_shadow_bool_c(reg, "pss", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pss);
    new bx_shadow_bool_c(reg, "pes", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.pes);
    new bx_shadow_bool_c(reg, "ccs", &BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  new bx_shadow_num_c(hub, "ohci_done_count", &BX_OHCI_THIS hub.ohci_done_count, BASE_DEC);
  new bx_shadow_bool_c(hub, "use_control_head", &BX_OHCI_THIS hub.use_control_head);
  new bx_shadow_bool_c(hub, "use_bulk_head", &BX_OHCI_THIS hub.use_bulk_head);
  new bx_shadow_num_c(hub, "sof_time", &BX_OHCI_THIS hub.sof_time);
  register_pci_state(hub);
}

void bx_usb_ohci_c::after_restore_state(void)
{
  if (DEV_pci_set_base_mem(BX_OHCI_THIS_PTR, read_handler, write_handler,
                         &BX_OHCI_THIS pci_base_address[0],
                         &BX_OHCI_THIS pci_conf[0x10],
                         4096))  {
     BX_INFO(("new base address: 0x%04x", BX_OHCI_THIS pci_base_address[0]));
  }
  for (int j=0; j<BX_N_USB_OHCI_PORTS; j++) {
    if (BX_OHCI_THIS hub.usb_port[j].device != NULL) {
      BX_OHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_usb_ohci_c::init_device(Bit8u port, bx_list_c *portconf)
{
  usbdev_type type;
  char pname[BX_PATHNAME_LEN];
  const char *devname = NULL;

  devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr();
  if (devname == NULL) return;
  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
    BX_ERROR(("init_device(): port%d already in use", port+1));
    return;
  }
  sprintf(pname, "usb_ohci.hub.port%d.device", port+1);
  bx_list_c *sr_list = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
  type = DEV_usb_init_device(portconf, BX_OHCI_THIS_PTR, &BX_OHCI_THIS hub.usb_port[port].device, sr_list);
  if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
    usb_set_connect_status(port, type, 1);
  }
}

void bx_usb_ohci_c::remove_device(Bit8u port)
{
  char pname[BX_PATHNAME_LEN];

  if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_OHCI_THIS hub.usb_port[port].device;
    BX_OHCI_THIS hub.usb_port[port].device = NULL;
    sprintf(pname, "usb_ohci.hub.port%d.device", port+1);
    bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
    devlist->clear();
  }
}

void bx_usb_ohci_c::update_irq()
{
  bx_bool level = 0;

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

bx_bool bx_usb_ohci_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u val = 0x0;
  int p = 0;

  if (len != 4) {
    BX_INFO(("Read at 0x%08X with len != 4 (%i)", (Bit32u)addr, len));
    return 1;
  }
  if (addr & 3) {
    BX_INFO(("Misaligned read at 0x%08X", (Bit32u)addr));
    return 1;
  }

  Bit32u  offset = (Bit32u)(addr - BX_OHCI_THIS pci_base_address[0]);
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
#if (BX_N_USB_OHCI_PORTS < 4)
      val = 0;
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (BX_N_USB_OHCI_PORTS < 3)
      val = 0;
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (BX_N_USB_OHCI_PORTS < 2)
      val = 0;
      break;
#endif
    case 0x54: // HcRhPortStatus[0]
      p = (offset - 0x54) >> 2;
      if (BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps == 1) {
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
      } else
        val = 0;
      break;

    default:
      BX_ERROR(("unsupported read from address=0x%08X!", (Bit32u)addr));
      break;
  }

  int name = offset >> 2;
  if (name > (0x60 >> 2))
    name = 25;
  //BX_INFO(("register read from address 0x%04X (%s):  0x%08X (len=%i)", (unsigned) addr, usb_ohci_port_name[name], (Bit32u) val, len));
  *((Bit32u *) data) = val;

  return 1;
}

bx_bool bx_usb_ohci_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u value = *((Bit32u *) data);
  Bit32u  offset = (Bit32u)addr - BX_OHCI_THIS pci_base_address[0];
  int p, org_state;

  int name = offset >> 2;
  if (name > (0x60 >> 2))
    name = 25;
  //BX_INFO(("register write to  address 0x%04X (%s):  0x%08X (len=%i)", (unsigned) addr, usb_ohci_port_name[name], (unsigned) value, len));

  if (len != 4) {
    BX_INFO(("Write at 0x%08X with len != 4 (%i)", (Bit32u)addr, len));
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
      if (BX_OHCI_THIS hub.op_regs.HcControl.hcfs == 0x02) {
        BX_OHCI_THIS hub.op_regs.HcFmRemainingToggle = 0;
        if (org_state != 2)
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
        BX_OHCI_THIS hub.op_regs.HcControl.hcfs = 3;      // suspend state
        for (unsigned i=0; i<BX_N_USB_OHCI_PORTS; i++)
          if (BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs && (BX_OHCI_THIS hub.usb_port[i].device != NULL))
            DEV_usb_send_msg(BX_OHCI_THIS hub.usb_port[i].device, USB_MSG_RESET);
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
      BX_ERROR(("Write to HcLSThreshold not allowed."));
      break;

    case 0x48: // HcRhDescriptorA
      if (value & 0x00FFE000)
        BX_ERROR(("Write to a reserved field in HcRhDescriptorA."));
      if (value & 0x000000FF)
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
          for (p=0; p<BX_N_USB_OHCI_PORTS; p++)
            BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
        } else {
          for (p=0; p<BX_N_USB_OHCI_PORTS; p++)
            if ((BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)) == 0)
              BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 1;
        }
      }
      if (value & (1<<0)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<BX_N_USB_OHCI_PORTS; p++)
            BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 0;
        } else {
          for (p=0; p<BX_N_USB_OHCI_PORTS; p++)
            if (!(BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)))
              BX_OHCI_THIS hub.usb_port[p].HcRhPortStatus.pps = 0;
        }
      }
      break;
    }

    case 0x60: // HcRhPortStatus[3]
#if (BX_N_USB_OHCI_PORTS < 4)
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (BX_N_USB_OHCI_PORTS < 3)
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (BX_N_USB_OHCI_PORTS < 2)
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
            usb_set_connect_status(p, BX_OHCI_THIS hub.usb_port[p].device->get_type(), 1);
            DEV_usb_send_msg(BX_OHCI_THIS hub.usb_port[p].device, USB_MSG_RESET);
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
  if ((BX_OHCI_THIS hub.op_regs.HcControl.hcfs != 2) ||
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

  if (BX_OHCI_THIS hub.op_regs.HcControl.hcfs == 2) {
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
    BX_DEBUG(("done_count = %i, status.wdh = %i", BX_OHCI_THIS hub.ohci_done_count,
              ((BX_OHCI_THIS hub.op_regs.HcInterruptStatus & OHCI_INTR_WD) > 0)));
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

    // TODO:  Rather than just comparing .fr to <8000 here, and <4000 below, see the highlighted
    //   statement on page 45.

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
          goto do_bulk_eds;
      }
    }

do_bulk_eds:
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
        process_ed(&cur_ed, BX_OHCI_THIS hub.op_regs.HcBulkCurrentED);
        BX_OHCI_THIS hub.op_regs.HcBulkCurrentED = ED_GET_NEXTED(&cur_ed);
        if (get_frame_remaining() < 4000)
          goto do_iso_eds;
      }
    }

do_iso_eds:
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

void bx_usb_ohci_c::process_ed(struct OHCI_ED *ed, const Bit32u ed_address)
{
  struct OHCI_TD cur_td;

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
}

bx_bool bx_usb_ohci_c::process_td(struct OHCI_TD *td, struct OHCI_ED *ed)
{
  unsigned pid = 0, len = 0, len1, len2;
  int r, ret = 0;
  char buf_str[1025], temp_str[17];

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
      len = (TD_GET_BE(td) - TD_GET_CBP(td)) + 1;
      if (len < 0) len = 0x1001 + len;
    }
  } else
    len = 0;

  BX_OHCI_THIS usb_packet.pid = pid;
  BX_OHCI_THIS usb_packet.devaddr = ED_GET_FA(ed);
  BX_OHCI_THIS usb_packet.devep = ED_GET_EN(ed);
  BX_OHCI_THIS usb_packet.data = BX_OHCI_THIS device_buffer;
  switch (pid) {
    case USB_TOKEN_SETUP:
    case USB_TOKEN_OUT:
      BX_OHCI_THIS usb_packet.len = (len <= ED_GET_MPS(ed)) ? len : ED_GET_MPS(ed);
      break;
    case USB_TOKEN_IN:
      BX_OHCI_THIS usb_packet.len = len;
      break;
  }

  BX_DEBUG(("    pid = %s  addr = %i   endpnt = %i    len = %i  mps = %i (td->cbp = 0x%08X, td->be = 0x%08X)", 
    (pid == USB_TOKEN_IN)? "IN" : (pid == USB_TOKEN_OUT) ? "OUT" : (pid == USB_TOKEN_SETUP) ? "SETUP" : "UNKNOWN", 
    ED_GET_FA(ed), ED_GET_EN(ed), len, ED_GET_MPS(ed), TD_GET_CBP(td), TD_GET_BE(td)));
  BX_DEBUG(("    td->t = %i  ed->c = %i  td->di = %i  td->r = %i", TD_GET_T(td), ED_GET_C(ed), TD_GET_DI(td), TD_GET_R(td)));

  /* set status bar conditions for device */
  if ((len > 0) && (BX_OHCI_THIS hub.statusbar_id >= 0)) {
    if (pid == USB_TOKEN_IN)
      bx_gui->statusbar_setitem(BX_OHCI_THIS hub.statusbar_id, 1);     // read
    else
      bx_gui->statusbar_setitem(BX_OHCI_THIS hub.statusbar_id, 1, 1);  // write
  }

  switch (pid) {
    case USB_TOKEN_SETUP:
      if (len > 0)
        DEV_MEM_READ_PHYSICAL_DMA(TD_GET_CBP(td), len, device_buffer);
      // TODO: This is a hack.  dev->handle_packet() should return the amount of bytes
      //  it received, not the amount it anticipates on receiving/sending in the next packet.
      if ((ret = BX_OHCI_THIS broadcast_packet(&BX_OHCI_THIS usb_packet)) >= 0)
        ret = 8;
      break;
    case USB_TOKEN_OUT:
      if (len > 0)
        DEV_MEM_READ_PHYSICAL_DMA(TD_GET_CBP(td), len, device_buffer);
      ret = BX_OHCI_THIS broadcast_packet(&BX_OHCI_THIS usb_packet);
      break;
    case USB_TOKEN_IN:
      ret = BX_OHCI_THIS broadcast_packet(&BX_OHCI_THIS usb_packet);
      if (ret > 0) {
        if (((TD_GET_CBP(td) & 0xfff) + ret) > 0x1000) {
          len1 = 0x1000 - (TD_GET_CBP(td) & 0xfff);
          len2 = ret - len1;
          DEV_MEM_WRITE_PHYSICAL_DMA(TD_GET_CBP(td), len1, device_buffer);
          DEV_MEM_WRITE_PHYSICAL_DMA((TD_GET_BE(td) & ~0xfff), len2, device_buffer+len1);
        } else {
          DEV_MEM_WRITE_PHYSICAL_DMA(TD_GET_CBP(td), ret, device_buffer);
        }
      } else
        ret = 0;
      break;
    default:
      TD_SET_CC(td, UnexpectedPID);
      TD_SET_EC(td, 3);
      return 1;
  }

  // print the buffer used, to the log file
  if (ret > 0) {
    BX_DEBUG(("buffer dump (%i bytes)", ret));
    buf_str[0] = 0;
    for (r=0; r<ret; r++) {
      sprintf(temp_str, "%02X ", device_buffer[r]);
      strcat(buf_str, temp_str);
      if ((r % 16) == 15) {
        BX_DEBUG(("%s", buf_str));
        buf_str[0] = 0;
      }
    }
    if (strlen(buf_str) > 0) BX_DEBUG(("%s", buf_str));
  }

  if ((ret == (int)len) || ((pid == USB_TOKEN_IN) && (ret >= 0) &&
      TD_GET_R(td)) || ((pid == USB_TOKEN_OUT) && (ret >= 0) &&
      (ret <= (int) ED_GET_MPS(ed)))) {
    if (ret == (int)len)
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
    if ((pid != USB_TOKEN_OUT) || (ret == (int)len)) {
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
          TD_SET_CC(td, Stall);
          break;
        case USB_RET_STALL:  // (-3)
          TD_SET_CC(td, Stall);
          break;
        case USB_RET_BABBLE:  // (-4)
          TD_SET_CC(td, BufferOverrun);
          break;
        case USB_RET_ASYNC:  // (-5)
          TD_SET_CC(td, BufferOverrun);
          break;
        default:
          BX_ERROR(("Unknown error returned: %i", ret));
          break;
      }
    }
    TD_SET_EC(td, 3);
    ED_SET_H(ed, 1);
  }

  BX_DEBUG((" td->cbp = 0x%08X   ret = %i  len = %i  td->cc = %i   td->ec = %i  ed->h = %i", TD_GET_CBP(td), ret, len, TD_GET_CC(td), TD_GET_EC(td), ED_GET_H(ed)));
  BX_DEBUG(("    td->t = %i  ed->c = %i", TD_GET_T(td), ED_GET_C(ed)));

  return 1;
}

int bx_usb_ohci_c::broadcast_packet(USBPacket *p)
{
  int i, ret;

  ret = USB_RET_NODEV;
  for (i = 0; i < BX_N_USB_OHCI_PORTS && ret == USB_RET_NODEV; i++) {
    if ((BX_OHCI_THIS hub.usb_port[i].device != NULL) &&
        (BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs)) {
      ret = BX_OHCI_THIS hub.usb_port[i].device->handle_packet(p);
    }
  }
  return ret;
}

void bx_usb_ohci_c::runtime_config_handler(void *this_ptr)
{
  bx_usb_ohci_c *class_ptr = (bx_usb_ohci_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_usb_ohci_c::runtime_config(void)
{
  int i;
  char pname[6];

  for (i = 0; i < BX_N_USB_OHCI_PORTS; i++) {
    // device change support
    if ((BX_OHCI_THIS hub.device_change & (1 << i)) != 0) {
      BX_INFO(("USB port #%d: device connect", i+1));
      sprintf(pname, "port%d", i + 1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
      BX_OHCI_THIS hub.device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_OHCI_THIS hub.usb_port[i].device != NULL) {
      BX_OHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// pci configuration space read callback handler
Bit32u bx_usb_ohci_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_OHCI_THIS pci_conf[address+i] << (i*8));
  }

  if (io_len == 1)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("read  PCI register 0x%02x value 0x%08x", address, value));

  return value;
}


// pci configuration space write callback handler
void bx_usb_ohci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if (((address >= 0x14) && (address <= 0x34)))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_OHCI_THIS pci_conf[address+i];
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
      case 0x3c:
        if (value8 != oldval) {
          BX_INFO(("new irq line = %d", value8));
          BX_OHCI_THIS pci_conf[address+i] = value8;
        }
        break;
      case 0x10:  // low 12 bits of BAR are R/O
        value8 = 0x00;
      case 0x11:  // low 12 bits of BAR are R/O
        value8 &= 0xF0;
      case 0x12:
      case 0x13:
        baseaddr_change |= (value8 != oldval);
      default:
        BX_OHCI_THIS pci_conf[address+i] = value8;
    }
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_mem(BX_OHCI_THIS_PTR, read_handler, write_handler,
                             &BX_OHCI_THIS pci_base_address[0],
                             &BX_OHCI_THIS pci_conf[0x10],
                             4096)) {
      BX_INFO(("new base address: 0x%04x", BX_OHCI_THIS pci_base_address[0]));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

void bx_usb_ohci_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  const bx_bool ccs_org = BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs;
  const bx_bool pes_org = BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes;

  usb_device_c *device = BX_OHCI_THIS hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
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
            device->set_speed(USB_SPEED_FULL);
            break;
          default:
            BX_ERROR(("device->get_speed() returned invalid speed value"));
        }
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs = 1;
        if (!device->get_connected()) {
          if (!device->init()) {
            usb_set_connect_status(port, type, 0);
            BX_ERROR(("port #%d: connect failed", port+1));
          } else {
            BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
          }
        }
      } else { // not connected
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs = 0;
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes = 0;
        BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.lsda = 0;
        remove_device(port);
      }
    }
    BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.csc |= (ccs_org != BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.ccs);
    BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pesc |= (pes_org != BX_OHCI_THIS hub.usb_port[port].HcRhPortStatus.pes);

    // we changed the value of the port, so show it
    set_interrupt(OHCI_INTR_RHSC);
  }
}

// USB runtime parameter handler
const char *bx_usb_ohci_c::usb_param_handler(bx_param_string_c *param, int set,
                                           const char *oldval, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
    if ((portnum >= 0) && (portnum < BX_N_USB_OHCI_PORTS)) {
      if (empty && BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_INFO(("USB port #%d: device disconnect", portnum+1));
        if (BX_OHCI_THIS hub.usb_port[portnum].device != NULL) {
          type = BX_OHCI_THIS hub.usb_port[portnum].device->get_type();
        }
        usb_set_connect_status(portnum, type, 0);
      } else if (!empty && !BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_OHCI_THIS hub.device_change |= (1 << portnum);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI
