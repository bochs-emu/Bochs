/////////////////////////////////////////////////////////////////////////
// $Id: usb_ohci.cc,v 1.1 2009-01-19 17:18:57 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009  Benjamin D Lunt (fys at frontiernet net)
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// Experimental USB OHCI adapter

// Notes: See usb_common.cc

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI

#include "pci.h"
#include "usb_common.h"
#include "usb_hid.h"
#include "usb_msd.h"
#include "usb_ohci.h"

#define NUM_DOWNSTREAM_PORTS 1   // for the sake of this code, at least 1 and no more than 4

#define LOG_THIS theUSB_OHCI->

bx_usb_ohci_c* theUSB_OHCI = NULL;

int libusb_ohci_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSB_OHCI = new bx_usb_ohci_c();
  bx_devices.pluginUSB_OHCI = theUSB_OHCI;
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_OHCI, BX_PLUGIN_USB_OHCI);
  return 0; // Success
}

void libusb_ohci_LTX_plugin_fini(void)
{
  delete theUSB_OHCI;
}

bx_usb_ohci_c::bx_usb_ohci_c()
{
  put("OHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_ohci_t));
  device_buffer = NULL;
}

bx_usb_ohci_c::~bx_usb_ohci_c()
{
  if (BX_OHCI_THIS device_buffer != NULL)
    delete [] BX_OHCI_THIS device_buffer;

  for (int j=0; j<NUM_DOWNSTREAM_PORTS; j++) {
    if (BX_OHCI_THIS hub.usb_port[j].device != NULL) {
      delete BX_OHCI_THIS hub.usb_port[j].device;
    }
  }

  SIM->get_param_string(BXPN_OHCI_PORT1)->set_handler(NULL);
  SIM->get_param_string(BXPN_OHCI_PORT2)->set_handler(NULL);

  BX_DEBUG(("Exit"));
}

void bx_usb_ohci_c::init(void)
{
  BX_OHCI_THIS device_buffer = new Bit8u[65536];

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_OHCI_THIS hub.timer_index =
                   bx_pc_system.register_timer(this, usb_timer_handler, 1000, 1,1, "ohci.timer");

  BX_OHCI_THIS hub.devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_OHCI_THIS hub.devfunc, BX_PLUGIN_USB_OHCI,
                            "Experimental USB OHCI");

  for (unsigned i=0; i<256; i++) {
    BX_OHCI_THIS hub.pci_conf[i] = 0x0;
  }

  BX_OHCI_THIS hub.base_addr = 0x0;

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_OHCI_THIS hub.statusbar_id[0] = bx_gui->register_statusitem("OHCI");

  SIM->get_param_string(BXPN_OHCI_PORT1)->set_handler(usb_param_handler);
  SIM->get_param_string(BXPN_OHCI_PORT1)->set_runtime_param(1);
  SIM->get_param_string(BXPN_OHCI_PORT2)->set_handler(usb_param_handler);
  SIM->get_param_string(BXPN_OHCI_PORT2)->set_runtime_param(1);

  //HACK: Turn on debug messages from the start
  //BX_OHCI_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

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
      { 0x00, 0xC1 }, { 0x01, 0x11 }, // 0x11C1 = vendor
      { 0x02, 0x03 }, { 0x03, 0x58 }, // 0x5803 = device
      { 0x04, 0x06 }, { 0x05, 0x00 }, // command_io
      { 0x06, 0x10 }, { 0x07, 0x02 }, // status (bit 4 = 1, has capabilities list.)
      { 0x08, 0x11 },                 // revision number
      { 0x09, 0x10 },                 // interface
      { 0x0a, 0x03 },                 // class_sub  USB Host Controller
      { 0x0b, 0x0c },                 // class_base Serial Bus Controller
      { 0x0D, 0x40 },                 // bus latency
      //{ 0x0e, 0x80 },                 // header_type_generic (multi-function device)
      { 0x0e, 0x00 },                 // header_type_generic

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
        BX_OHCI_THIS hub.pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  // reset locals
  BX_OHCI_THIS busy = 0;
  BX_OHCI_THIS global_reset = 0;

  // HcRevision
  BX_OHCI_THIS hub.op_regs.HcRevision.reserved =   0x000001;
  BX_OHCI_THIS hub.op_regs.HcRevision.rev      =       0x10;

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
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.zero     = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.oc       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.reserved = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rhsc     = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.fno      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.ue       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rd       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.sf       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.wdh      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptStatus.so       = 0;

  // HcInterruptEnable
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.reserved = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc     = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so       = 0;

  // HcInterruptDisable
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.mie      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.oc       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.reserved = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.rhsc     = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.fno      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.ue       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.rd       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.sf       = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.wdh      = 0;
  BX_OHCI_THIS hub.op_regs.HcInterruptDisable.so       = 0;

  // HcHCCA
  BX_OHCI_THIS hub.op_regs.HcHCCA                      = 0x00000000;

  // HcPeriodCurrentED
  BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED.pced   = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED.zero   = 0;

  // HcControlHeadED
  BX_OHCI_THIS hub.op_regs.HcControlHeadED.ched     = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcControlHeadED.zero     = 0;

  // HcControlCurrentED
  BX_OHCI_THIS hub.op_regs.HcControlCurrentED.cced  = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcControlCurrentED.zero  = 0;

  // HcBulkHeadED
  BX_OHCI_THIS hub.op_regs.HcBulkHeadED.bhed     = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcBulkHeadED.zero     = 0;

  // HcBulkCurrentED
  BX_OHCI_THIS hub.op_regs.HcBulkCurrentED.bced  = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcBulkCurrentED.zero  = 0;

  // HcDoneHead
  BX_OHCI_THIS hub.op_regs.HcDoneHead.dh    = 0x00000000;
  BX_OHCI_THIS hub.op_regs.HcDoneHead.zero  = 0;

  // HcFmInterval
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fit      =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps    =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.reserved =      0;
  BX_OHCI_THIS hub.op_regs.HcFmInterval.fi       = 0x2EDF;

  // HcFmRemaining
  BX_OHCI_THIS hub.op_regs.HcFmRemaining.frt      =      0;
  BX_OHCI_THIS hub.op_regs.HcFmRemaining.reserved =      0;
  BX_OHCI_THIS hub.op_regs.HcFmRemaining.fr       = 0x0000;

  // HcFmNumber
  BX_OHCI_THIS hub.op_regs.HcFmNumber.reserved    =      0;
  BX_OHCI_THIS hub.op_regs.HcFmNumber.fn          =      0;

  // HcPeriodicStart
  BX_OHCI_THIS hub.op_regs.HcPeriodicStart.reserved  =      0;
  BX_OHCI_THIS hub.op_regs.HcPeriodicStart.ps        =      0;

  // HcLSThreshold
  BX_OHCI_THIS hub.op_regs.HcLSThreshold.reserved    =      0;
  BX_OHCI_THIS hub.op_regs.HcLSThreshold.lst         = 0x0628;

  // HcRhDescriptorA
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt   = 0x10;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.reserved =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp     =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm     =    1;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.dt       =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps      =    0;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm      =    1;
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp      =    NUM_DOWNSTREAM_PORTS;

  // HcRhDescriptorB
  BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm     = 0x0002;
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
  for (i=0; i<1003; i++) {
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].reserved0 = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].prsc      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].ocic      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].pssc      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].pesc      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].csc       = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].reserved1 = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].lsda      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].pps       = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].reserved2 = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].prs       = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].poci      = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].pss       = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].pes       = 0;
    BX_OHCI_THIS hub.op_regs.HcRhPortStatus[i].ccs       = 0;
  }

  BX_OHCI_THIS mousedev = NULL;
  BX_OHCI_THIS keybdev = NULL;

  init_device(0, SIM->get_param_string(BXPN_OHCI_PORT1)->getptr());
  init_device(1, SIM->get_param_string(BXPN_OHCI_PORT2)->getptr());
}

void bx_usb_ohci_c::register_state(void)
{
//  unsigned i, j;
//  char hubnum[8], portnum[8];
//  bx_list_c *hub, *usb_cmd, *usb_st, *usb_en, *port;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "usb_ohci", "USB OHCI State");
/*
  sprintf(hubnum, "hub%d", i+1);
  hub = new bx_list_c(list, hubnum, USB_NUM_PORTS + 7);
  usb_cmd = new bx_list_c(hub, "usb_command", 8);
  new bx_shadow_bool_c(usb_cmd, "max_packet_size", &BX_OHCI_THIS hub.usb_command.max_packet_size);
  new bx_shadow_bool_c(usb_cmd, "configured", &BX_OHCI_THIS hub.usb_command.configured);
  new bx_shadow_bool_c(usb_cmd, "debug", &BX_OHCI_THIS hub.usb_command.debug);
  new bx_shadow_bool_c(usb_cmd, "resume", &BX_OHCI_THIS hub.usb_command.resume);
  new bx_shadow_bool_c(usb_cmd, "suspend", &BX_OHCI_THIS hub.usb_command.suspend);
  new bx_shadow_bool_c(usb_cmd, "reset", &BX_OHCI_THIS hub.usb_command.reset);
  new bx_shadow_bool_c(usb_cmd, "host_reset", &BX_OHCI_THIS hub.usb_command.host_reset);
  new bx_shadow_bool_c(usb_cmd, "schedule", &BX_OHCI_THIS hub.usb_command.schedule);
  usb_st = new bx_list_c(hub, "usb_status", 6);
  new bx_shadow_bool_c(usb_st, "host_halted", &BX_OHCI_THIS hub.usb_status.host_halted);
  new bx_shadow_bool_c(usb_st, "host_error", &BX_OHCI_THIS hub.usb_status.host_error);
  new bx_shadow_bool_c(usb_st, "pci_error", &BX_OHCI_THIS hub.usb_status.pci_error);
  new bx_shadow_bool_c(usb_st, "resume", &BX_OHCI_THIS hub.usb_status.resume);
  new bx_shadow_bool_c(usb_st, "error_interrupt", &BX_OHCI_THIS hub.usb_status.error_interrupt);
  new bx_shadow_bool_c(usb_st, "interrupt", &BX_OHCI_THIS hub.usb_status.interrupt);
  usb_en = new bx_list_c(hub, "usb_enable", 4);
  new bx_shadow_bool_c(usb_en, "short_packet", &BX_OHCI_THIS hub.usb_enable.short_packet);
  new bx_shadow_bool_c(usb_en, "on_complete", &BX_OHCI_THIS hub.usb_enable.on_complete);
  new bx_shadow_bool_c(usb_en, "resume", &BX_OHCI_THIS hub.usb_enable.resume);
  new bx_shadow_bool_c(usb_en, "timeout_crc", &BX_OHCI_THIS hub.usb_enable.timeout_crc);
  new bx_shadow_num_c(hub, "frame_num", &BX_OHCI_THIS hub.usb_frame_num.frame_num, BASE_HEX);
  new bx_shadow_num_c(hub, "frame_base", &BX_OHCI_THIS hub.usb_frame_base.frame_base, BASE_HEX);
  new bx_shadow_num_c(hub, "sof_timing", &BX_OHCI_THIS hub.usb_sof.sof_timing, BASE_HEX);
  for (j=0; j<USB_NUM_PORTS; j++) {
    sprintf(portnum, "port%d", j+1);
    port = new bx_list_c(hub, portnum, 11);
    new bx_shadow_bool_c(port, "suspend", &BX_OHCI_THIS hub.usb_port[j].suspend);
    new bx_shadow_bool_c(port, "reset", &BX_OHCI_THIS hub.usb_port[j].reset);
    new bx_shadow_bool_c(port, "low_speed", &BX_OHCI_THIS hub.usb_port[j].low_speed);
    new bx_shadow_bool_c(port, "resume", &BX_OHCI_THIS hub.usb_port[j].resume);
    new bx_shadow_bool_c(port, "line_dminus", &BX_OHCI_THIS hub.usb_port[j].line_dminus);
    new bx_shadow_bool_c(port, "line_dplus", &BX_OHCI_THIS hub.usb_port[j].line_dplus);
    new bx_shadow_bool_c(port, "able_changed", &BX_OHCI_THIS hub.usb_port[j].able_changed);
    new bx_shadow_bool_c(port, "enabled", &BX_OHCI_THIS hub.usb_port[j].enabled);
    new bx_shadow_bool_c(port, "connect_changed", &BX_OHCI_THIS hub.usb_port[j].connect_changed);
    new bx_shadow_bool_c(port, "status", &BX_OHCI_THIS hub.usb_port[j].status);
    // empty list for USB device state
    new bx_list_c(port, "device", 20);
  }
  register_pci_state(hub, BX_OHCI_THIS hub.pci_conf);
*/
  new bx_shadow_bool_c(list, "busy", &BX_OHCI_THIS busy);
  new bx_shadow_num_c(list, "global_reset", &BX_OHCI_THIS global_reset);
}

void bx_usb_ohci_c::after_restore_state(void)
{
  if (DEV_pci_set_base_mem(BX_OHCI_THIS_PTR, read_handler, write_handler,
                         &BX_OHCI_THIS hub.base_addr,
                         &BX_OHCI_THIS hub.pci_conf[0x10],
                         4096))  {
     BX_INFO(("new base address: 0x%04x", BX_OHCI_THIS hub.base_addr));
  }
  for (int j=0; j<NUM_DOWNSTREAM_PORTS; j++) {
    if (BX_OHCI_THIS hub.usb_port[j].device != NULL) {
      BX_OHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_usb_ohci_c::init_device(Bit8u port, const char *devname)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  char pname[BX_PATHNAME_LEN];

  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (!strcmp(devname, "mouse")) {
    type = USB_DEV_TYPE_MOUSE;
    BX_OHCI_THIS hub.usb_port[port].device = new usb_hid_device_c(type);
    if (BX_OHCI_THIS mousedev == NULL) {
      BX_OHCI_THIS mousedev = (usb_hid_device_c*)BX_OHCI_THIS hub.usb_port[port].device;
    }
  } else if (!strcmp(devname, "tablet")) {
    type = USB_DEV_TYPE_TABLET;
    BX_OHCI_THIS hub.usb_port[port].device = new usb_hid_device_c(type);
    if (BX_OHCI_THIS mousedev == NULL) {
      BX_OHCI_THIS mousedev = (usb_hid_device_c*)BX_OHCI_THIS hub.usb_port[port].device;
    }
  } else if (!strcmp(devname, "keypad")) {
    type = USB_DEV_TYPE_KEYPAD;
    BX_OHCI_THIS hub.usb_port[port].device = new usb_hid_device_c(type);
    if (BX_OHCI_THIS keybdev == NULL) {
      BX_OHCI_THIS keybdev = (usb_hid_device_c*)BX_OHCI_THIS hub.usb_port[port].device;
    }
  } else if (!strncmp(devname, "disk", 4)) {
    if ((strlen(devname) > 5) && (devname[4] == ':')) {
      type = USB_DEV_TYPE_DISK;
      BX_OHCI_THIS hub.usb_port[port].device = new usb_msd_device_c();
    } else {
      BX_PANIC(("USB device 'disk' needs a filename separated with a colon"));
      return;
    }
  } else {
    BX_PANIC(("unknown USB device: %s", devname));
    return;
  }
  sprintf(pname, "usb.ohci.port%d.device", port+1);
  bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
  BX_OHCI_THIS hub.usb_port[port].device->register_state(devlist);
  usb_set_connect_status(port, type, 1);
}

void bx_usb_ohci_c::set_irq_level(bx_bool level)
{
  DEV_pci_set_irq(BX_OHCI_THIS hub.devfunc, BX_OHCI_THIS hub.pci_conf[0x3d], level);
}

bx_bool bx_usb_ohci_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param) {

  Bit32u val = 0x0;
  int p;

  if (len != 4) {
    BX_INFO(("Read at 0x%08X with len != 4 (%i)", addr, len));
    return 1;
  }

  Bit32u  offset = addr - BX_OHCI_THIS hub.base_addr;
  switch (offset) {
    case 0x00: // HcRevision
      val =   BX_OHCI_THIS hub.op_regs.HcRevision.reserved << 8
            | BX_OHCI_THIS hub.op_regs.HcRevision.rev;
      break;

    case 0x04: // HcControl
      val =   BX_OHCI_THIS hub.op_regs.HcControl.reserved << 11
            | BX_OHCI_THIS hub.op_regs.HcControl.rwe      << 10
            | BX_OHCI_THIS hub.op_regs.HcControl.rwc      << 9
            | BX_OHCI_THIS hub.op_regs.HcControl.ir       << 8
            | BX_OHCI_THIS hub.op_regs.HcControl.hcfs     << 6
            | BX_OHCI_THIS hub.op_regs.HcControl.ble      << 5
            | BX_OHCI_THIS hub.op_regs.HcControl.cle      << 4
            | BX_OHCI_THIS hub.op_regs.HcControl.ie       << 3
            | BX_OHCI_THIS hub.op_regs.HcControl.ple      << 2
            | BX_OHCI_THIS hub.op_regs.HcControl.cbsr     << 0;
      break;

    case 0x08: // HcCommandStatus
      val =   BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved0 << 18
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.soc       << 16
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.reserved1 << 4
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr       << 3
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf       << 2
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf       << 1
            | BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr       << 0;
      break;

    case 0x0C: // HcInterruptStatus
      val =   BX_OHCI_THIS hub.op_regs.HcInterruptStatus.zero     << 31
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.oc       << 30
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.reserved << 7
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rhsc     << 6
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.fno      << 5
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.ue       << 4
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rd       << 3
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.sf       << 2
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.wdh      << 1
            | BX_OHCI_THIS hub.op_regs.HcInterruptStatus.so       << 0;
      break;

    case 0x10: // HcInterruptEnable
    case 0x14: // HcInterruptDisable (reading this one returns that one)
      val =   BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie      << 31
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc       << 30
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.reserved << 7
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc     << 6
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno      << 5
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue       << 4
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd       << 3
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf       << 2
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh      << 1
            | BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so       << 0;
      break;

    case 0x18: // HcHCCA
      val =   BX_OHCI_THIS hub.op_regs.HcHCCA;
      break;

    case 0x1C: // HcPeriodCurrentED
      val =   BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED.pced << 4
            | BX_OHCI_THIS hub.op_regs.HcPeriodCurrentED.zero << 0;
      break;

    case 0x20: // HcControlHeadED
      val =   BX_OHCI_THIS hub.op_regs.HcControlHeadED.ched << 4
            | BX_OHCI_THIS hub.op_regs.HcControlHeadED.zero << 0;
      break;

    case 0x24: // HcControlCurrentED
      val =   BX_OHCI_THIS hub.op_regs.HcControlCurrentED.cced << 4
            | BX_OHCI_THIS hub.op_regs.HcControlCurrentED.zero << 0;
      break;

    case 0x28: // HcBulkHeadED
      val =   BX_OHCI_THIS hub.op_regs.HcBulkHeadED.bhed << 4
            | BX_OHCI_THIS hub.op_regs.HcBulkHeadED.zero << 0;
      break;

    case 0x2C: // HcBulkCurrentED
      val =   BX_OHCI_THIS hub.op_regs.HcBulkCurrentED.bced << 4
            | BX_OHCI_THIS hub.op_regs.HcBulkCurrentED.zero << 0;
      break;

    case 0x30: // HcDoneHead
      val =   BX_OHCI_THIS hub.op_regs.HcDoneHead.dh   << 4
            | BX_OHCI_THIS hub.op_regs.HcDoneHead.zero << 0;
      break;

    case 0x34: // HcFmInterval
      val =   BX_OHCI_THIS hub.op_regs.HcFmInterval.fit      << 31
            | BX_OHCI_THIS hub.op_regs.HcFmInterval.fsmps    << 16
            | BX_OHCI_THIS hub.op_regs.HcFmInterval.reserved << 14
            | BX_OHCI_THIS hub.op_regs.HcFmInterval.fi       << 0;
      break;

    case 0x38: // HcFmRemaining
      val =   BX_OHCI_THIS hub.op_regs.HcFmRemaining.frt      << 31
            | BX_OHCI_THIS hub.op_regs.HcFmRemaining.reserved << 14
            | BX_OHCI_THIS hub.op_regs.HcFmRemaining.fr       << 0;
      break;

    case 0x3C: // HcFmNumber
      val =   BX_OHCI_THIS hub.op_regs.HcFmNumber.reserved << 16
            | BX_OHCI_THIS hub.op_regs.HcFmNumber.fn       << 0;
      break;

    case 0x40: // HcPeriodicStart
      val =   BX_OHCI_THIS hub.op_regs.HcPeriodicStart.reserved << 14
            | BX_OHCI_THIS hub.op_regs.HcPeriodicStart.ps       << 0;
      break;

    case 0x44: // HcLSThreshold
      val =   BX_OHCI_THIS hub.op_regs.HcLSThreshold.reserved << 12
            | BX_OHCI_THIS hub.op_regs.HcLSThreshold.lst      << 0;
      break;

    case 0x48: // HcRhDescriptorA
      val =   BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.potpgt   << 24
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.reserved << 13
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp     << 12
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ocpm     << 11
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.dt       << 10
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nps      <<  9
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm      <<  8
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.ndp      <<  0;
      break;

    case 0x4C: // HcRhDescriptorB
      val =   BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm << 16
            | BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr   << 0;
      break;

    case 0x50: // HcRhStatus
      val =   BX_OHCI_THIS hub.op_regs.HcRhStatus.crwe      << 31
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved0 << 18
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic      << 17
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.lpsc      << 16
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe      << 15
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.reserved1 <<  2
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.oci       <<  1
            | BX_OHCI_THIS hub.op_regs.HcRhStatus.lps       <<  0;
      break;

    case 0x60: // HcRhPortStatus[3]
#if (NUM_DOWNSTREAM_PORTS < 4)
      val = 0;
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (NUM_DOWNSTREAM_PORTS < 3)
      val = 0;
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (NUM_DOWNSTREAM_PORTS < 2)
      val = 0;
      break;
#endif
    case 0x54: // HcRhPortStatus[0]
      p = (offset - 0x54) >> 2;
      val =   BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].reserved0 << 21
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].prsc      << 20
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ocic      << 19
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pssc      << 18
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pesc      << 17
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].csc       << 16
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].reserved1 << 10
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].lsda      <<  9
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps       <<  8
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].reserved2 <<  5
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].prs       <<  4
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].poci      <<  3
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pss       <<  2
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pes       <<  1
            | BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ccs       <<  0;
      break;

    default:
      BX_ERROR(("unsupported read from address=0x%08X!", addr));
      break;
  }

  BX_INFO(("register read from address 0x%04X:  0x%08X (len = %2i)", (unsigned) addr, (Bit32u) val, len));
  *((Bit32u *) data) = val;

  return 1;
}

bx_bool bx_usb_ohci_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param) {

  Bit32u value = *((Bit32u *) data);
  BX_INFO(("register write to  address 0x%04X:  0x%08X (len = %2i)", (unsigned) addr, (unsigned) value, len));
  int p;

  if (len != 4) {
    BX_INFO(("Write at 0x%08X with len != 4 (%i)", addr, len));
    return 1;
  }

  Bit32u  offset = addr - BX_OHCI_THIS hub.base_addr;
  switch (offset) {
    case 0x00: // HcRevision
      BX_INFO(("Write to HcRevision ignored"));
      break;

    case 0x04: // HcControl
      if (value & 0xFFFFF800)
        BX_ERROR(("Write to reserved field in HcControl"));
      BX_OHCI_THIS hub.op_regs.HcControl.rwe      = (value & (1<<10)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.rwc      = (value & (1<< 9)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ir       = (value & (1<< 8)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.hcfs     = (value & (3<< 6)) >>  6;
      BX_OHCI_THIS hub.op_regs.HcControl.ble      = (value & (1<< 5)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.cle      = (value & (1<< 4)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ie       = (value & (1<< 3)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.ple      = (value & (1<< 2)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcControl.cbsr     = (value & (3<< 0)) >>  0;
      break;

    case 0x08: // HcCommandStatus
      if (value & 0xFFFCFFF0)
        BX_ERROR(("Write to a reserved field in HcCommandStatus"));
      if (value & (3<<16))
        BX_ERROR(("Write to R/O field: HcCommandStatus.soc"));
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.ocr    = (value & (1<< 3)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.blf    = (value & (1<< 2)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.clf    = (value & (1<< 1)) ? 1 : 0;
      BX_OHCI_THIS hub.op_regs.HcCommandStatus.hcr    = (value & (1<< 0)) ? 1 : 0;
      break;

    case 0x0C: // HcInterruptStatus /// all are WC  
      if (value & 0xBFFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptStatus"));
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.oc       = (value & (1<<30)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.oc;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rhsc     = (value & (1<< 6)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rhsc;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.fno      = (value & (1<< 5)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.fno;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.ue       = (value & (1<< 4)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.ue;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rd       = (value & (1<< 3)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.rd;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.sf       = (value & (1<< 2)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.sf;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.wdh      = (value & (1<< 1)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.wdh;
      BX_OHCI_THIS hub.op_regs.HcInterruptStatus.so       = (value & (1<< 0)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptStatus.so;
      break;

    case 0x10: // HcInterruptEnable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptEnable"));
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie      = (value & (1<<31)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc       = (value & (1<<30)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc     = (value & (1<< 6)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno      = (value & (1<< 5)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue       = (value & (1<< 4)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd       = (value & (1<< 3)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf       = (value & (1<< 2)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh      = (value & (1<< 1)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so       = (value & (1<< 0)) ? 1 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so;
      break;

    case 0x14: // HcInterruptDisable
      if (value & 0x3FFFFF80)
        BX_ERROR(("Write to a reserved field in HcInterruptDisable"));
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie      = (value & (1<<31)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.mie;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc       = (value & (1<<30)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.oc;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc     = (value & (1<< 6)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rhsc;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno      = (value & (1<< 5)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.fno;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue       = (value & (1<< 4)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.ue;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd       = (value & (1<< 3)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.rd;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf       = (value & (1<< 2)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.sf;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh      = (value & (1<< 1)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.wdh;
      BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so       = (value & (1<< 0)) ? 0 : BX_OHCI_THIS hub.op_regs.HcInterruptEnable.so;
      break;

    case 0x18: // HcHCCA
      if (value & 0x000000FF)
        BX_ERROR(("Write to lower byte of HcHCCA non zero."));
      BX_OHCI_THIS hub.op_regs.HcHCCA = value & 0xFFFFFF00;
      break;

    case 0x1C: // HcPeriodCurrentED
      BX_ERROR(("Write to HcPeriodCurrentED not allowed."));
      break;

    case 0x20: // HcControlHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlHeadED non zero."));
      BX_OHCI_THIS hub.op_regs.HcControlHeadED.ched = value & 0xFFFFFFF0;
      break;

    case 0x24: // HcControlCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcControlCurrentED non zero."));
      BX_OHCI_THIS hub.op_regs.HcControlCurrentED.cced = value & 0xFFFFFFF0;
      break;

    case 0x28: // HcBulkHeadED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkHeadED non zero."));
      BX_OHCI_THIS hub.op_regs.HcBulkHeadED.bhed = value & 0xFFFFFFF0;
      break;

    case 0x2C: // HcBulkCurrentED
      if (value & 0x0000000F)
        BX_ERROR(("Write to lower nibble of HcBulkCurrentED non zero."));
      BX_OHCI_THIS hub.op_regs.HcBulkCurrentED.bced = value & 0xFFFFFFF0;
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
      BX_OHCI_THIS hub.op_regs.HcPeriodicStart.ps       = (value & 0x00003FFF);
      break;

    case 0x44: // HcLSThreshold
      if (value & 0xFFFFF000)
        BX_ERROR(("Write to a reserved field in HcLSThreshold."));
      BX_OHCI_THIS hub.op_regs.HcLSThreshold.lst        = (value & 0x00000FFF);
      // docs say that we can't change this value, but also say that it is R/W ????????
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
      break;

    case 0x4C: // HcRhDescriptorB
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm = (value & 0xFFFF0000) >> 16;
      BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.dr   = (value & 0x0000FFFF) >>  0;
      break;

    case 0x50: // HcRhStatus
      if (value & 0x7FFC7FFC)
        BX_ERROR(("Write to a reserved field in HcRhStatus."));
      if (value & (1<<1))
        BX_ERROR(("Write to HcRhStatus.oci not allowed."));
      // which one of these two takes presidence?  
      BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe      = (1<<31) ? 0 : BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe;
      BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe      = (1<<15) ? 1 : BX_OHCI_THIS hub.op_regs.HcRhStatus.drwe;

      BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic      = (1<<17) ? 0 : BX_OHCI_THIS hub.op_regs.HcRhStatus.ocic;
      if (value & (1<<16)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<NUM_DOWNSTREAM_PORTS; p++)
            BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 1;
        } else {
          for (p=0; p<NUM_DOWNSTREAM_PORTS; p++)
            if (!(BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)))
              BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 1;
        }
      }
      if (value & (1<<0)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.psm == 0) {
          for (p=0; p<NUM_DOWNSTREAM_PORTS; p++)
            BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 0;
        } else {
          for (p=0; p<NUM_DOWNSTREAM_PORTS; p++)
            if (!(BX_OHCI_THIS hub.op_regs.HcRhDescriptorB.ppcm & (1<<p)))
              BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 0;
        }
      }
      break;

    case 0x60: // HcRhPortStatus[3]
#if (NUM_DOWNSTREAM_PORTS < 4)
      break;
#endif
    case 0x5C: // HcRhPortStatus[2]
#if (NUM_DOWNSTREAM_PORTS < 3)
      break;
#endif
    case 0x58: // HcRhPortStatus[1]
#if (NUM_DOWNSTREAM_PORTS < 2)
      break;
#endif
    case 0x54: // HcRhPortStatus[0]
      p = (offset - 0x54) >> 2;
      if (value & 0xFFE0FCE0)
        BX_ERROR(("Write to a reserved field in HcRhPortStatus[p]."));
      if (value & (1<<0))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pes = 0;
      if (value & (1<<1)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ccs)
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].csc = 1;
        else
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pes = 1;
      }
      if (value & (1<<2)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ccs)
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].csc = 1;
        else
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pss = 1;
      }
//      if (value & (1<<3))
//        if (BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pss)
//          ; // do a resume (or test this in the timer code and do the resume there)
      if (value & (1<<4)) {
        if (BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ccs)
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].csc = 1;
        else
          BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].prs = 1;
      }
      if (value & (1<<8))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 1;
      if (value & (1<<9))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pps = 0;
      if (value & (1<<16))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].csc = (value & ((1<<4) | (1<<1) | (1<<2))) ? 1 : 0;
      if (value & (1<<17))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pesc = 0;
      if (value & (1<<18))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].pssc = 0;
      if (value & (1<<19))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].ocic = 0;
      if (value & (1<<20))
        BX_OHCI_THIS hub.op_regs.HcRhPortStatus[p].prsc = 0;
      break;

    default:
      BX_ERROR(("unsupported write to address=0x%08X, val = 0x%08X!", addr, value));
      break;
  }

  return 1;
}

void bx_usb_ohci_c::usb_timer_handler(void *this_ptr)
{
  bx_usb_ohci_c *class_ptr = (bx_usb_ohci_c *) this_ptr;
  class_ptr->usb_timer();
}

// Called once every 1ms
#define USB_STACK_SIZE  256
void bx_usb_ohci_c::usb_timer(void)
{
//  int i;
/*


    if (BX_OHCI_THIS hub.op_regs.HcRhPortStatus[0].prs == 1) {
      // reset the port, then
      BX_OHCI_THIS hub.op_regs.HcRhPortStatus[0].prsc = 1;
      BX_OHCI_THIS hub.op_regs.HcRhPortStatus[0].pssc = 1;
    }




  // If the "global reset" bit was set by software
  if (BX_OHCI_THIS global_reset) {
    for (i=0; i<NUM_DOWNSTREAM_PORTS; i++) {
      BX_OHCI_THIS hub.usb_port[i].able_changed = 0;
      BX_OHCI_THIS hub.usb_port[i].connect_changed = 0;
      BX_OHCI_THIS hub.usb_port[i].enabled = 0;
      BX_OHCI_THIS hub.usb_port[i].line_dminus = 0;
      BX_OHCI_THIS hub.usb_port[i].line_dplus = 0;
      BX_OHCI_THIS hub.usb_port[i].low_speed = 0;
      BX_OHCI_THIS hub.usb_port[i].reset = 0;
      BX_OHCI_THIS hub.usb_port[i].resume = 0;
      BX_OHCI_THIS hub.usb_port[i].status = 0;
      BX_OHCI_THIS hub.usb_port[i].suspend = 0;
    }
    return;
  }

  // If command.schedule = 1, then run schedule
  //  *** This assumes that we can complete the frame within the 1ms time allowed ***
  // Actually, not complete, but reach the end of the frame.  This means that there may still
  //  be TDs and QHs that were BREADTH defined and will be executed on the next cycle/iteration.

  if (BX_OHCI_THIS busy) {
    BX_PANIC(("Did not complete last frame before the 1ms was over. Starting next frame."));
    BX_OHCI_THIS busy = 0;
  }
  if (BX_OHCI_THIS hub.usb_command.schedule) {
    BX_OHCI_THIS busy = 1;
    bx_bool fire_int = 0;
    set_irq_level(0);  // make sure it is low
    bx_bool interrupt = 0, shortpacket = 0, stalled = 0;
    struct TD td;
    struct HCSTACK stack[USB_STACK_SIZE+1];  // queue stack for this item only
    Bit32s stk = 0;
    Bit32u item, address, lastvertaddr = 0, queue_num = 0;
    Bit32u frame, frm_addr = BX_OHCI_THIS hub.usb_frame_base.frame_base +
                                (BX_OHCI_THIS hub.usb_frame_num.frame_num << 2);
    DEV_MEM_READ_PHYSICAL(frm_addr, 4, (Bit8u*) &frame);
    if ((frame & 1) == 0) {
      stack[stk].next = (frame & ~0xF);
      stack[stk].d = 0;
      stack[stk].q = (frame & 0x0002) ? 1 : 0;
      stack[stk].t = 0;
      while (stk > -1) {

        // Linux seems to just loop a few queues together and wait for the 1ms to end.
        // We will just count the stack and exit when we get to a good point to stop.
        if (stk >= USB_STACK_SIZE) break;

        // check to make sure we are not done before continue-ing on
        if ((stack[stk].d == HC_VERT) && stack[stk].t) { stk--; continue; }
        if ((stack[stk].d == HC_HORZ) && stack[stk].t) break;
        if (stack[stk].q) { // is a queue
          address = stack[stk].next;
          lastvertaddr = address + 4;
          // get HORZ slot
          stk++;
          DEV_MEM_READ_PHYSICAL(address, 4, (Bit8u*) &item);
          stack[stk].next = item & ~0xF;
          stack[stk].d = HC_HORZ;
          stack[stk].q = (item & 0x0002) ? 1 : 0;
          stack[stk].t = (item & 0x0001) ? 1 : 0;
          // get VERT slot
          stk++;
          DEV_MEM_READ_PHYSICAL(lastvertaddr, 4, (Bit8u*) &item);
          stack[stk].next = item & ~0xF;
          stack[stk].d = HC_VERT;
          stack[stk].q = (item & 0x0002) ? 1 : 0;
          stack[stk].t = (item & 0x0001) ? 1 : 0;
          BX_DEBUG(("Queue %3i: 0x%08X %i %i  0x%08X %i %i", queue_num,
            stack[stk-1].next, stack[stk-1].q, stack[stk-1].t,
            stack[stk].next, stack[stk].q, stack[stk].t));
          queue_num++;
        } else {  // else is a TD
          address = stack[stk].next;
          DEV_MEM_READ_PHYSICAL(address,    4, (Bit8u*) &td.dword0);
          DEV_MEM_READ_PHYSICAL(address+4,  4, (Bit8u*) &td.dword1);
          DEV_MEM_READ_PHYSICAL(address+8,  4, (Bit8u*) &td.dword2);
          DEV_MEM_READ_PHYSICAL(address+12, 4, (Bit8u*) &td.dword3);
          bx_bool spd = (td.dword1 & (1<<29)) ? 1 : 0;
          stack[stk].next = td.dword0 & ~0xF;
          bx_bool depthbreadth = (td.dword0 & 0x0004) ? 1 : 0;     // 1 = depth first, 0 = breadth first
          stack[stk].q = (td.dword0 & 0x0002) ? 1 : 0;
          stack[stk].t = (td.dword0 & 0x0001) ? 1 : 0;
          if (td.dword1 & (1<<24)) interrupt = 1;
          if (td.dword1 & (1<<23)) {  // is it an active TD
            BX_DEBUG(("Frame: %04i (0x%04X)", BX_OHCI_THIS hub.usb_frame_num.frame_num, BX_OHCI_THIS hub.usb_frame_num.frame_num));
            if (BX_OHCI_THIS DoTransfer(address, queue_num, &td)) {
              // issue short packet?
              Bit16u r_actlen = (((td.dword1 & 0x7FF)+1) & 0x7FF);
              Bit16u r_maxlen = (((td.dword2>>21)+1) & 0x7FF);
              BX_DEBUG((" r_actlen = 0x%04X r_maxlen = 0x%04X", r_actlen, r_maxlen));
              if (((td.dword2 & 0xFF) == USB_TOKEN_IN) && spd && stk && (r_actlen < r_maxlen) && ((td.dword1 & 0x00FF0000) == 0)) {
                shortpacket = 1;
                td.dword1 |= (1<<29);
              }
              if (td.dword1 & (1<<22)) stalled = 1;

              DEV_MEM_WRITE_PHYSICAL(address+4, 4, (Bit8u*) &td.dword1);  // write back the status
              // copy pointer for next queue item, in to vert queue head
              if ((stk > 0) && !shortpacket && (stack[stk].d == HC_VERT))
                DEV_MEM_WRITE_PHYSICAL(lastvertaddr, 4, (Bit8u*) &td.dword0);
            }
          }

          if (stk > 0) {
            // if last TD in HORZ queue pointer, then we are done.
            if (stack[stk].t && (stack[stk].d == HC_HORZ)) break;
            // if Breadth first or last item in queue, move to next queue.
            if (!depthbreadth || stack[stk].t) {
              if (stack[stk].d == HC_HORZ) queue_num--;  // <-- really, this should never happen until we
              stk--;                                     //           support bandwidth reclamation...
            }
            if (stk < 1) break;
          } else {
            if (stack[stk].t) break;
          }

        }
      }

      // set the status register bit:0 to 1 if SPD is enabled
      // and if interrupts not masked via interrupt register, raise irq interrupt.
      if (shortpacket && BX_OHCI_THIS hub.usb_enable.short_packet) {
        fire_int = 1;
        BX_DEBUG((" [SPD] We want it to fire here (Frame: %04i)", BX_OHCI_THIS hub.usb_frame_num.frame_num));
      }

      // if one of the TD's in this frame had the ioc bit set, we need to
      //   raise an interrupt, if interrupts are not masked via interrupt register.
      //   always set the status register if IOC.
      if (interrupt && BX_OHCI_THIS hub.usb_enable.on_complete) {
        fire_int = 1;
        BX_DEBUG((" [IOC] We want it to fire here (Frame: %04i)", BX_OHCI_THIS hub.usb_frame_num.frame_num));
      }

      if (stalled && BX_OHCI_THIS hub.usb_enable.timeout_crc) {
        fire_int = 1;
        BX_DEBUG((" [stalled] We want it to fire here (Frame: %04i)", BX_OHCI_THIS hub.usb_frame_num.frame_num));
      }
    }

    // The Frame Number Register is incremented every 1ms
    BX_OHCI_THIS hub.usb_frame_num.frame_num++;
    BX_OHCI_THIS hub.usb_frame_num.frame_num &= (1024-1);

    // if we needed to fire an interrupt now, lets do it *after* we increment the frame_num register
    if (fire_int) {
      BX_OHCI_THIS hub.usb_status.interrupt = 1;
      BX_OHCI_THIS hub.usb_status.error_interrupt = stalled;
      set_irq_level(1);
    }

    BX_OHCI_THIS busy = 0;  // ready to do next frame item
  }  // end run schedule

  // if host turned off the schedule, set the halted bit in the status register
  // Note: Can not use an else from the if() above since the host can changed this bit
  //  while we are processing a frame.
  if (BX_OHCI_THIS hub.usb_command.schedule == 0)
    BX_OHCI_THIS hub.usb_status.host_halted = 1;

  // TODO:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
*/
}

bx_bool bx_usb_ohci_c::DoTransfer(Bit32u address, Bit32u queue_num, struct TD *td)
{
/*
  int i, len = 0, ret = 0;
  usb_device_c *dev = NULL;

  Bit16u maxlen = (td->dword2 >> 21);
  Bit8u  addr   = (td->dword2 >> 8) & 0x7F;
  Bit8u  endpt  = (td->dword2 >> 15) & 0x0F;
  Bit8u  pid    =  td->dword2 & 0xFF;

  BX_DEBUG(("QH%03i:TD found at address: 0x%08X", queue_num, address));
  BX_DEBUG(("  %08X   %08X   %08X   %08X", td->dword0, td->dword1, td->dword2, td->dword3));

  // check TD to make sure it is valid
  // A max length 0x500 to 0x77E is illegal
  if (((td->dword2 >> 21) >= 0x500) && ((td->dword2 >> 21) != 0x7FF)) {
    BX_ERROR(("error at 11111111111"));
    return 1;  // error = consistency check failure
  }

  //if (td->dword0 & 0x8) return 1; // error = reserved bits in dword0 set
  // other error checks here

  // find device address
  bx_bool at_least_one = 0;
  for (i=0; i<USB_NUM_PORTS; i++) {
    if (BX_OHCI_THIS hub.usb_port[i].device != NULL) {
      if (BX_OHCI_THIS hub.usb_port[i].device->get_connected()) {
        at_least_one = 1;
        if (BX_OHCI_THIS hub.usb_port[i].device->get_address() == addr) {
          dev = BX_OHCI_THIS hub.usb_port[i].device;
          break;
        }
      }
    }
  }
  if (!at_least_one) {
    BX_OHCI_THIS set_status(td, 1, 0, 0, 0, (pid==USB_TOKEN_SETUP)?1:0, 0, 0x007); // an 8 byte packet was received, but stalled
    return 1;
  }
  if (dev == NULL) {
    if ((pid == USB_TOKEN_OUT) && (maxlen == 0x7FF) && (addr == 0)) {
      // This is the "keep awake" packet that Windows sends once a schedule cycle.
      // For now, let it pass through to the code below.
    } else {
      BX_PANIC(("Device not found for addr: %i", addr));
      BX_OHCI_THIS set_status(td, 1, 0, 0, 0, (pid==USB_TOKEN_SETUP)?1:0, 0, 0x007); // an 8 byte packet was received, but stalled
      return 1;  // device not found
    }
  }

  // the device should remain in a stall state until the next setup packet is recieved
  // For some reason, this doesn't work yet.
  //if (dev && dev->in_stall && (pid != USB_TOKEN_SETUP))
  //  return FALSE;

  maxlen++;
  maxlen &= 0x7FF;

  if (dev != NULL) {
    BX_OHCI_THIS usb_packet.pid = pid;
    BX_OHCI_THIS usb_packet.devaddr = addr;
    BX_OHCI_THIS usb_packet.devep = endpt;
    BX_OHCI_THIS usb_packet.data = device_buffer;
    BX_OHCI_THIS usb_packet.len = maxlen;
    switch (pid) {
      case USB_TOKEN_OUT:
      case USB_TOKEN_SETUP:
        if (maxlen > 0) {
          DEV_MEM_READ_PHYSICAL_BLOCK(td->dword3, maxlen, device_buffer);
        }
        ret = dev->handle_packet(&BX_OHCI_THIS usb_packet);
        len = maxlen;
        break;
      case USB_TOKEN_IN:
        ret = dev->handle_packet(&BX_OHCI_THIS usb_packet);
        if (ret >= 0) {
          len = ret;
          if (len > maxlen) {
            len = maxlen;
            ret = USB_RET_BABBLE;
          }
          if (len > 0) {
            DEV_MEM_WRITE_PHYSICAL_BLOCK(td->dword3, len, device_buffer);
          }
        } else {
          len = 0;
        }
        break;
      default:
        BX_OHCI_THIS hub.usb_status.host_error = 1;
        BX_OHCI_THIS set_irq_level(1);
    }
    if (ret >= 0) {
      BX_OHCI_THIS set_status(td, 0, 0, 0, 0, 0, 0, len-1);
    } else {
      BX_OHCI_THIS set_status(td, 1, 0, 0, 0, 0, 0, 0x007); // stalled
    }
    return 1;
  }
*/
  return 0;
}

// If the request fails, set the stall bit ????
void bx_usb_ohci_c::set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
                             bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len)
{
/*
  // clear out the bits we can modify and/or want zero
  td->dword1 &= 0xDF00F800;

  // now set the bits according to the passed param's
  td->dword1 |= stalled           ? (1<<22) : 0; // stalled
  td->dword1 |= data_buffer_error ? (1<<21) : 0; // data buffer error
  td->dword1 |= babble            ? (1<<20) : 0; // babble
  td->dword1 |= nak               ? (1<<19) : 0; // nak
  td->dword1 |= crc_time_out      ? (1<<18) : 0; // crc/timeout
  td->dword1 |= bitstuff_error    ? (1<<17) : 0; // bitstuff error
  td->dword1 |= (act_len & 0x7FF);               // actual length
  if (stalled || data_buffer_error || babble || nak || crc_time_out || bitstuff_error)
    td->dword1 &= ~((1<<28) | (1<<27));  // clear the c_err field in there was an error
*/
}

// pci configuration space read callback handler
Bit32u bx_usb_ohci_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  if (io_len > 4 || io_len == 0) {
    BX_ERROR(("Experimental USB OHCI read register 0x%02x, len=%u !",
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
    case 0x20: pszName = "(base address)    "; break;
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
    value |= (BX_OHCI_THIS hub.pci_conf[address+i] << (i*8));
    sprintf(szTmp2, "%02x", (BX_OHCI_THIS hub.pci_conf[address+i]));
    strrev(szTmp2);
    strcat(szTmp, szTmp2);
  }
  strrev(szTmp);
  BX_DEBUG(("USB OHCI read  register 0x%02x %svalue 0x%s", address, pszName, szTmp));
  return value;
}


// pci configuration space write callback handler
void bx_usb_ohci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;
  
  if (((address >= 0x14) && (address <= 0x34)))
    return;
  
  // This odd code is to display only what bytes actually were written.
  char szTmp[9];
  char szTmp2[3];
  szTmp[0] = '\0';
  szTmp2[0] = '\0';
  if (io_len <= 4) {
    for (unsigned i=0; i<io_len; i++) {
      value8 = (value >> (i*8)) & 0xFF;
      oldval = BX_OHCI_THIS hub.pci_conf[address+i];
      switch (address+i) {
        case 0x04:
          value8 &= 0x06; // (bit 0 is read only for this card) (we don't allow port IO)
          BX_OHCI_THIS hub.pci_conf[address+i] = value8;
          sprintf(szTmp2, "%02x", value8);
          break;
        case 0x3d: //
        case 0x3e: //
        case 0x3f: //
        case 0x05: // disallowing write to command hi-byte
        case 0x06: // disallowing write to status lo-byte (is that expected?)
          strcpy(szTmp2, "..");
          break;
        case 0x3c:
          if (value8 != oldval) {
            BX_INFO(("new irq line = %d", value8));
            BX_OHCI_THIS hub.pci_conf[address+i] = value8;
          }
          sprintf(szTmp2, "%02x", value8);
          break;
        case 0x10:  // low 12 bits of BAR are R/O
          value8 = 0x00;
        case 0x11:  // low 12 bits of BAR are R/O
          value8 &= 0xF0;
        case 0x12:
        case 0x13:
          baseaddr_change |= (value8 != oldval);
        default:
          BX_OHCI_THIS hub.pci_conf[address+i] = value8;
          sprintf(szTmp2, "%02x", value8);
      }
      strrev(szTmp2);
      strcat(szTmp, szTmp2);
    }
    if (baseaddr_change) {
      if (DEV_pci_set_base_mem(BX_OHCI_THIS_PTR, read_handler, write_handler,
                       &BX_OHCI_THIS hub.base_addr,
                       &BX_OHCI_THIS hub.pci_conf[0x10],
                       4096)) {
         BX_INFO(("new base address: 0x%04x", BX_OHCI_THIS hub.base_addr));
      }
    }
  }
  strrev(szTmp);
  BX_DEBUG(("USB OHCI write register 0x%02x                   value 0x%s", address, szTmp));
}

bx_bool bx_usb_ohci_c::usb_mouse_enabled_changed(bx_bool enabled)
{
  if (BX_OHCI_THIS mousedev != NULL) {
    if (enabled) mousedev->handle_reset();
    return 1;
  }
  return 0;
}

void bx_usb_ohci_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
//  char pname[BX_PATHNAME_LEN];
//  char fname[BX_PATHNAME_LEN];

/*
  if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
    if (BX_OHCI_THIS hub.usb_port[port].device->get_type() == type) {
      if (connected) {
        if (!BX_OHCI_THIS hub.usb_port[port].device->get_connected()) {
          BX_OHCI_THIS hub.usb_port[port].low_speed =
            (BX_OHCI_THIS hub.usb_port[port].device->get_speed() == USB_SPEED_LOW);
        }
        if (BX_OHCI_THIS hub.usb_port[port].low_speed) {
          BX_OHCI_THIS hub.usb_port[port].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_OHCI_THIS hub.usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        } else {
          BX_OHCI_THIS hub.usb_port[port].line_dminus = 0;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_OHCI_THIS hub.usb_port[port].line_dplus = 1;   //  dminus=0 & dplus=1 = high speed (at idle time)
        }
        BX_OHCI_THIS hub.usb_port[port].status = 1;       //
        BX_OHCI_THIS hub.usb_port[port].connect_changed = 1;
        BX_OHCI_THIS hub.usb_port[port].able_changed = 1;

        // if in suspend state, signal resume
        if (BX_OHCI_THIS hub.usb_command.suspend) {
          BX_OHCI_THIS hub.usb_port[port].resume = 1;
          BX_OHCI_THIS hub.usb_status.resume = 1;
          if (BX_OHCI_THIS hub.usb_enable.resume) {
            BX_OHCI_THIS hub.usb_status.interrupt = 1;
            set_irq_level(1);
          }
        }

        if ((type == USB_DEV_TYPE_DISK) &&
            (!BX_OHCI_THIS hub.usb_port[port].device->get_connected())) {
          if (port == 0) {
            strcpy(pname, BXPN_OHCI_PORT1);
          } else {
            strcpy(pname, BXPN_OHCI_PORT2);
          }
          strcpy(fname, SIM->get_param_string(pname)->getptr() + 5);
          if (!((usb_msd_device_t*)BX_OHCI_THIS hub.usb_port[port].device)->init(fname)) {
            usb_set_connect_status(port, USB_DEV_TYPE_DISK, 0);
          } else {
            BX_INFO(("HD on USB port #%d: '%s'", port+1, fname));
          }
        }
      } else {
        BX_OHCI_THIS hub.usb_port[port].status = 0;
        BX_OHCI_THIS hub.usb_port[port].connect_changed = 1;
        BX_OHCI_THIS hub.usb_port[port].enabled = 0;
        BX_OHCI_THIS hub.usb_port[port].able_changed = 1;
        BX_OHCI_THIS hub.usb_port[port].low_speed = 0;
        BX_OHCI_THIS hub.usb_port[port].line_dminus = 0;  //  dminus=1 & dplus=0 = low speed  (at idle time)
        BX_OHCI_THIS hub.usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        if ((type == USB_DEV_TYPE_MOUSE) ||
            (type == USB_DEV_TYPE_TABLET)) {
          if (BX_OHCI_THIS hub.usb_port[port].device == BX_OHCI_THIS mousedev) {
            BX_OHCI_THIS mousedev = NULL;
          }
        } else if (type == USB_DEV_TYPE_KEYPAD) {
          if (BX_OHCI_THIS hub.usb_port[port].device == BX_OHCI_THIS keybdev) {
            BX_OHCI_THIS keybdev = NULL;
          }
        }
        if (BX_OHCI_THIS hub.usb_port[port].device != NULL) {
          delete BX_OHCI_THIS hub.usb_port[port].device;
          BX_OHCI_THIS hub.usb_port[port].device = NULL;
          sprintf(pname, "usb.ohci.port%d.device", port+1);
          bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
          devlist->clear();
        }
      }
    }
  }
*/
}

bx_bool bx_usb_ohci_c::usb_mouse_enq(int delta_x, int delta_y, int delta_z, unsigned button_state)
{
  if (BX_OHCI_THIS mousedev != NULL) {
    mousedev->mouse_enq(delta_x, delta_y, delta_z, button_state);
    return 1;
  }
  return 0;
}

bx_bool bx_usb_ohci_c::usb_key_enq(Bit8u *scan_code)
{
  if (BX_OHCI_THIS keybdev != NULL) {
    return keybdev->key_enq(scan_code);
  }
  return 0;
}

// Send an internal message to a USB device
void bx_usb_ohci_c::usb_send_msg(usb_device_c *dev, int msg)
{
    USBPacket p;
    memset(&p, 0, sizeof(p));
    p.pid = msg;
    dev->handle_packet(&p);
}

// USB runtime parameter handler
const char *bx_usb_ohci_c::usb_param_handler(bx_param_string_c *param, int set,
                                           const char *oldval, const char *val, int maxlen)
{
/*
  usbdev_type type = USB_DEV_TYPE_NONE;

  // handler for USB runtime parameters
  if (set) {
    char pname[BX_PATHNAME_LEN];
    param->get_param_path(pname, BX_PATHNAME_LEN);
    if (!strcmp(pname, BXPN_OHCI_PORT1)) {
      BX_INFO(("USB port #1 experimental device change"));
      if (!strcmp(val, "none") && BX_OHCI_THIS hub.usb_port[0].status) {
        if (BX_OHCI_THIS hub.usb_port[0].device != NULL) {
          type = BX_OHCI_THIS hub.usb_port[0].device->get_type();
        }
        usb_set_connect_status(0, type, 0);
      } else if (strcmp(val, "none") && !BX_OHCI_THIS hub.usb_port[0].status) {
        init_device(0, val);
      }
    } else if (!strcmp(pname, BXPN_OHCI_PORT2)) {
      BX_INFO(("USB port #2 experimental device change"));
      if (!strcmp(val, "none") && BX_OHCI_THIS hub.usb_port[1].status) {
        if (BX_OHCI_THIS hub.usb_port[1].device != NULL) {
          type = BX_OHCI_THIS hub.usb_port[1].device->get_type();
        }
        usb_set_connect_status(1, type, 0);
      } else if (strcmp(val, "none") && !BX_OHCI_THIS hub.usb_port[1].status) {
        init_device(1, val);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", pname));
    }
  }
*/
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_OHCI
