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

// Experimental USB UHCI adapter
// PIIX3/PIIX4 function 2

/* Notes by Ben Lunt:
   - My purpose of coding this emulation was/is to learn about the USB.
     It has been a challenge, but I have learned a lot.
   - 31 July 2006:
     I now have a Beagle USB Protocol Analyzer from Total Phase for my research.
     (http://www.totalphase.com/products/beagle-usb12/)
     With this device, I plan on doing a lot of research and development to get this
     code to a state where it is actually very useful.  I plan on adding support
     of many "plug-in" type modules so that you can simply add a plug-in for your
     specific device without having to modify the root code.
     I hope to have some working code to upload to the CVS as soon as possible.
     Thanks to Total Phase for their help in my research and the development of
     this project.
  */

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI

#include "pci.h"
#include "usb_common.h"
#include "usb_uhci.h"

#define LOG_THIS theUSB_UHCI->

bx_usb_uhci_c* theUSB_UHCI = NULL;

const Bit8u uhci_iomask[32] = {2, 1, 2, 1, 2, 1, 2, 0, 4, 0, 0, 0, 1, 0, 0, 0,
                              3, 1, 3, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// builtin configuration handling functions

Bit32s usb_uhci_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "usb_uhci")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_UHCI);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_UHCI_ENABLED)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "port", 4)) {
        if (SIM->parse_usb_port_params(context, 0, params[i], BX_N_USB_UHCI_PORTS, base) < 0) {
          return -1;
        }
      } else if (!strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, 1, params[i], BX_N_USB_UHCI_PORTS, base) < 0) {
          return -1;
        }
      } else {
        BX_ERROR(("%s: unknown parameter '%s' for usb_uhci ignored.", context, params[i]));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s usb_uhci_options_save(FILE *fp)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_UHCI);
  SIM->write_usb_options(fp, BX_N_USB_UHCI_PORTS, base);
  return 0;
}

// device plugin entry points

int CDECL libusb_uhci_LTX_plugin_init(plugin_t *plugin, plugintype_t type, int argc, char *argv[])
{
  theUSB_UHCI = new bx_usb_uhci_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_UHCI, BX_PLUGIN_USB_UHCI);
  // add new configuration parameter for the config interface
  SIM->init_usb_options("UHCI", "uhci", BX_N_USB_UHCI_PORTS);
  // register add-on option for bochsrc and command line
  SIM->register_addon_option("usb_uhci", usb_uhci_options_parser, usb_uhci_options_save);
  return 0; // Success
}

void CDECL libusb_uhci_LTX_plugin_fini(void)
{
  SIM->unregister_addon_option("usb_uhci");
  bx_list_c *menu = (bx_list_c*)SIM->get_param("ports.usb");
  delete theUSB_UHCI;
  menu->remove("uhci");
}

// the device object

bx_usb_uhci_c::bx_usb_uhci_c()
{
  put("usb_uhci", "UHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_uhci_t));
  device_buffer = NULL;
  hub.timer_index = BX_NULL_TIMER_HANDLE;
}

bx_usb_uhci_c::~bx_usb_uhci_c()
{
  char pname[16];

  if (BX_UHCI_THIS device_buffer != NULL)
    delete [] BX_UHCI_THIS device_buffer;

  for (int i=0; i<BX_N_USB_UHCI_PORTS; i++) {
    sprintf(pname, "port%d.device", i+1);
    SIM->get_param_string(pname, SIM->get_param(BXPN_USB_UHCI))->set_handler(NULL);
    remove_device(i);
  }

  SIM->get_bochs_root()->remove("usb_uhci");
  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove("uhci");
  BX_DEBUG(("Exit"));
}

void bx_usb_uhci_c::init(void)
{
  unsigned i;
  char pname[6];
  bx_list_c *uhci, *port;
  bx_param_string_c *device;

  // Read in values from config interface
  uhci = (bx_list_c*) SIM->get_param(BXPN_USB_UHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", uhci)->get()) {
    BX_INFO(("USB UHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_uhci"))->set(0);
    return;
  }

  BX_UHCI_THIS device_buffer = new Bit8u[65536];

  // Call our timer routine every 1mS (1,000uS)
  // Continuous and active
  BX_UHCI_THIS hub.timer_index =
                   bx_pc_system.register_timer(this, usb_timer_handler, 1000, 1,1, "usb.timer");

  if (SIM->get_param_enum(BXPN_PCI_CHIPSET)->get() == BX_PCI_CHIPSET_I440FX) {
    BX_UHCI_THIS hub.devfunc = BX_PCI_DEVICE(1,2);
  } else {
    BX_UHCI_THIS hub.devfunc = 0x00;
  }
  DEV_register_pci_handlers(this, &BX_UHCI_THIS hub.devfunc, BX_PLUGIN_USB_UHCI,
                            "Experimental USB UHCI");

  // initialize readonly registers
  init_pci_conf(0x8086, 0x7020, 0x01, 0x0c0300, 0x00);
  BX_UHCI_THIS pci_conf[0x3d] = BX_PCI_INTD;

  BX_UHCI_THIS pci_base_address[4] = 0x0;

  //FIXME: for now, we want a status bar // hub zero, port zero
  BX_UHCI_THIS hub.statusbar_id = bx_gui->register_statusitem("UHCI", 1);

  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *uhci_rt = new bx_list_c(usb_rt, "uhci", "UHCI Runtime Options");
  uhci_rt->set_options(uhci_rt->SHOW_PARENT);
  for (i=0; i<BX_N_USB_UHCI_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c*)SIM->get_param(pname, uhci);
    uhci_rt->add(port);
    device = (bx_param_string_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
    BX_UHCI_THIS hub.usb_port[i].device = NULL;
  }

  // register handler for correct device connect handling after runtime config
  SIM->register_runtime_config_handler(BX_UHCI_THIS_PTR, runtime_config_handler);
  BX_UHCI_THIS hub.device_change = 0;

  BX_INFO(("USB UHCI initialized"));
}

void bx_usb_uhci_c::reset(unsigned type)
{
  unsigned i, j;
  char pname[6];

  if (type == BX_RESET_HARDWARE) {
    static const struct reset_vals_t {
      unsigned      addr;
      unsigned char val;
    } reset_vals[] = {
      { 0x04, 0x05 }, { 0x05, 0x00 }, // command_io
      { 0x06, 0x80 }, { 0x07, 0x02 }, // status
      { 0x0d, 0x20 },                 // bus latency
      // address space 0x20 - 0x23
      { 0x20, 0x01 }, { 0x21, 0x00 },
      { 0x22, 0x00 }, { 0x23, 0x00 },
      { 0x3c, 0x00 },                 // IRQ
      { 0x60, 0x10 },                 // USB revision 1.0
      { 0x6a, 0x01 },                 // USB clock
      { 0xc1, 0x20 }                  // PIRQ enable

    };
    for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); ++i) {
        BX_UHCI_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  // reset locals
  BX_UHCI_THIS busy = 0;
  BX_UHCI_THIS global_reset = 0;

  // Put the USB registers into their RESET state
  BX_UHCI_THIS hub.usb_command.max_packet_size = 0;
  BX_UHCI_THIS hub.usb_command.configured = 0;
  BX_UHCI_THIS hub.usb_command.debug = 0;
  BX_UHCI_THIS hub.usb_command.resume = 0;
  BX_UHCI_THIS hub.usb_command.suspend = 0;
  BX_UHCI_THIS hub.usb_command.reset = 0;
  BX_UHCI_THIS hub.usb_command.host_reset = 0;
  BX_UHCI_THIS hub.usb_command.schedule = 0;
  BX_UHCI_THIS hub.usb_status.error_interrupt = 0;
  BX_UHCI_THIS hub.usb_status.host_error = 0;
  BX_UHCI_THIS hub.usb_status.host_halted = 0;
  BX_UHCI_THIS hub.usb_status.interrupt = 0;
  BX_UHCI_THIS hub.usb_status.status2 = 0;
  BX_UHCI_THIS hub.usb_status.pci_error = 0;
  BX_UHCI_THIS hub.usb_status.resume = 0;
  BX_UHCI_THIS hub.usb_enable.short_packet = 0;
  BX_UHCI_THIS hub.usb_enable.on_complete = 0;
  BX_UHCI_THIS hub.usb_enable.resume = 0;
  BX_UHCI_THIS hub.usb_enable.timeout_crc = 0;
  BX_UHCI_THIS hub.usb_frame_num.frame_num = 0x0000;
  BX_UHCI_THIS hub.usb_frame_base.frame_base = 0x00000000;
  BX_UHCI_THIS hub.usb_sof.sof_timing = 0x40;
  for (j=0; j<BX_N_USB_UHCI_PORTS; j++) {
    BX_UHCI_THIS hub.usb_port[j].connect_changed = 0;
    BX_UHCI_THIS hub.usb_port[j].line_dminus = 0;
    BX_UHCI_THIS hub.usb_port[j].line_dplus = 0;
    BX_UHCI_THIS hub.usb_port[j].low_speed = 0;
    BX_UHCI_THIS hub.usb_port[j].reset = 0;
    BX_UHCI_THIS hub.usb_port[j].resume = 0;
    BX_UHCI_THIS hub.usb_port[j].suspend = 0;
    BX_UHCI_THIS hub.usb_port[j].enabled = 0;
    BX_UHCI_THIS hub.usb_port[j].able_changed = 0;
    BX_UHCI_THIS hub.usb_port[j].status = 0;
    if (BX_UHCI_THIS hub.usb_port[j].device == NULL) {
      sprintf(pname, "port%d", j+1);
      init_device(j, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_UHCI)));
    } else {
      usb_set_connect_status(j, BX_UHCI_THIS hub.usb_port[j].device->get_type(), 1);
    }
  }

}

void bx_usb_uhci_c::register_state(void)
{
  unsigned j;
  char portnum[8];
  bx_list_c *hub, *usb_cmd, *usb_st, *usb_en, *port;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "usb_uhci", "USB UHCI State");
  hub = new bx_list_c(list, "hub");
  usb_cmd = new bx_list_c(hub, "usb_command");
  new bx_shadow_bool_c(usb_cmd, "max_packet_size", &BX_UHCI_THIS hub.usb_command.max_packet_size);
  new bx_shadow_bool_c(usb_cmd, "configured", &BX_UHCI_THIS hub.usb_command.configured);
  new bx_shadow_bool_c(usb_cmd, "debug", &BX_UHCI_THIS hub.usb_command.debug);
  new bx_shadow_bool_c(usb_cmd, "resume", &BX_UHCI_THIS hub.usb_command.resume);
  new bx_shadow_bool_c(usb_cmd, "suspend", &BX_UHCI_THIS hub.usb_command.suspend);
  new bx_shadow_bool_c(usb_cmd, "reset", &BX_UHCI_THIS hub.usb_command.reset);
  new bx_shadow_bool_c(usb_cmd, "host_reset", &BX_UHCI_THIS hub.usb_command.host_reset);
  new bx_shadow_bool_c(usb_cmd, "schedule", &BX_UHCI_THIS hub.usb_command.schedule);
  usb_st = new bx_list_c(hub, "usb_status");
  new bx_shadow_bool_c(usb_st, "host_halted", &BX_UHCI_THIS hub.usb_status.host_halted);
  new bx_shadow_bool_c(usb_st, "host_error", &BX_UHCI_THIS hub.usb_status.host_error);
  new bx_shadow_bool_c(usb_st, "pci_error", &BX_UHCI_THIS hub.usb_status.pci_error);
  new bx_shadow_bool_c(usb_st, "resume", &BX_UHCI_THIS hub.usb_status.resume);
  new bx_shadow_bool_c(usb_st, "error_interrupt", &BX_UHCI_THIS hub.usb_status.error_interrupt);
  new bx_shadow_bool_c(usb_st, "interrupt", &BX_UHCI_THIS hub.usb_status.interrupt);
  new bx_shadow_num_c(usb_st, "status2", &BX_UHCI_THIS hub.usb_status.status2, BASE_HEX);
  usb_en = new bx_list_c(hub, "usb_enable");
  new bx_shadow_bool_c(usb_en, "short_packet", &BX_UHCI_THIS hub.usb_enable.short_packet);
  new bx_shadow_bool_c(usb_en, "on_complete", &BX_UHCI_THIS hub.usb_enable.on_complete);
  new bx_shadow_bool_c(usb_en, "resume", &BX_UHCI_THIS hub.usb_enable.resume);
  new bx_shadow_bool_c(usb_en, "timeout_crc", &BX_UHCI_THIS hub.usb_enable.timeout_crc);
  new bx_shadow_num_c(hub, "frame_num", &BX_UHCI_THIS hub.usb_frame_num.frame_num, BASE_HEX);
  new bx_shadow_num_c(hub, "frame_base", &BX_UHCI_THIS hub.usb_frame_base.frame_base, BASE_HEX);
  new bx_shadow_num_c(hub, "sof_timing", &BX_UHCI_THIS hub.usb_sof.sof_timing, BASE_HEX);
  for (j=0; j<BX_N_USB_UHCI_PORTS; j++) {
    sprintf(portnum, "port%d", j+1);
    port = new bx_list_c(hub, portnum);
    new bx_shadow_bool_c(port, "suspend", &BX_UHCI_THIS hub.usb_port[j].suspend);
    new bx_shadow_bool_c(port, "reset", &BX_UHCI_THIS hub.usb_port[j].reset);
    new bx_shadow_bool_c(port, "low_speed", &BX_UHCI_THIS hub.usb_port[j].low_speed);
    new bx_shadow_bool_c(port, "resume", &BX_UHCI_THIS hub.usb_port[j].resume);
    new bx_shadow_bool_c(port, "line_dminus", &BX_UHCI_THIS hub.usb_port[j].line_dminus);
    new bx_shadow_bool_c(port, "line_dplus", &BX_UHCI_THIS hub.usb_port[j].line_dplus);
    new bx_shadow_bool_c(port, "able_changed", &BX_UHCI_THIS hub.usb_port[j].able_changed);
    new bx_shadow_bool_c(port, "enabled", &BX_UHCI_THIS hub.usb_port[j].enabled);
    new bx_shadow_bool_c(port, "connect_changed", &BX_UHCI_THIS hub.usb_port[j].connect_changed);
    new bx_shadow_bool_c(port, "status", &BX_UHCI_THIS hub.usb_port[j].status);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }
  register_pci_state(hub);

  new bx_shadow_bool_c(list, "busy", &BX_UHCI_THIS busy);
  new bx_shadow_num_c(list, "global_reset", &BX_UHCI_THIS global_reset);
}

void bx_usb_uhci_c::after_restore_state(void)
{
  if (DEV_pci_set_base_io(BX_UHCI_THIS_PTR, read_handler, write_handler,
                         &BX_UHCI_THIS pci_base_address[4],
                         &BX_UHCI_THIS pci_conf[0x20],
                         32, &uhci_iomask[0], "USB UHCI Hub"))
  {
     BX_INFO(("new base address: 0x%04x", BX_UHCI_THIS pci_base_address[4]));
  }
  for (int j=0; j<BX_N_USB_UHCI_PORTS; j++) {
    if (BX_UHCI_THIS hub.usb_port[j].device != NULL) {
      BX_UHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

void bx_usb_uhci_c::init_device(Bit8u port, bx_list_c *portconf)
{
  usbdev_type type;
  char pname[BX_PATHNAME_LEN];
  const char *devname = NULL;

  devname = ((bx_param_string_c*)portconf->get_by_name("device"))->getptr();
  if (devname == NULL) return;
  if (!strlen(devname) || !strcmp(devname, "none")) return;

  if (BX_UHCI_THIS hub.usb_port[port].device != NULL) {
    BX_ERROR(("init_device(): port%d already in use", port+1));
    return;
  }
  sprintf(pname, "usb_uhci.hub.port%d.device", port+1);
  bx_list_c *sr_list = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
  type = DEV_usb_init_device(portconf, BX_UHCI_THIS_PTR, &BX_UHCI_THIS hub.usb_port[port].device, sr_list);
  if (BX_UHCI_THIS hub.usb_port[port].device != NULL) {
    usb_set_connect_status(port, type, 1);
  }
}

void bx_usb_uhci_c::remove_device(Bit8u port)
{
  char pname[BX_PATHNAME_LEN];

  if (BX_UHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_UHCI_THIS hub.usb_port[port].device;
    BX_UHCI_THIS hub.usb_port[port].device = NULL;
    sprintf(pname, "usb_uhci.hub.port%d.device", port+1);
    bx_list_c *devlist = (bx_list_c*)SIM->get_param(pname, SIM->get_bochs_root());
    devlist->clear();
  }
}

void bx_usb_uhci_c::update_irq()
{
  bx_bool level;

  if (((BX_UHCI_THIS hub.usb_status.status2 & 1) && (BX_UHCI_THIS hub.usb_enable.on_complete)) ||
      ((BX_UHCI_THIS hub.usb_status.status2 & 2) && (BX_UHCI_THIS hub.usb_enable.short_packet)) ||
      ((BX_UHCI_THIS hub.usb_status.error_interrupt) && (BX_UHCI_THIS hub.usb_enable.timeout_crc)) ||
      ((BX_UHCI_THIS hub.usb_status.resume) && (BX_UHCI_THIS hub.usb_enable.resume)) ||
      (BX_UHCI_THIS hub.usb_status.pci_error) ||
      (BX_UHCI_THIS hub.usb_status.host_error)) {
    level = 1;
  } else {
    level = 0;
  }
  DEV_pci_set_irq(BX_UHCI_THIS hub.devfunc, BX_UHCI_THIS pci_conf[0x3d], level);
}

// static IO port read callback handler
// redirects to non-static class handler to avoid virtual functions

Bit32u bx_usb_uhci_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_USB_UHCI_SMF
  bx_usb_uhci_c *class_ptr = (bx_usb_uhci_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_usb_uhci_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_USB_UHCI_SMF
  Bit32u val = 0x0;
  Bit8u  offset,port;

  offset = address - BX_UHCI_THIS pci_base_address[4];

  switch (offset) {
    case 0x00: // command register (16-bit)
      val =   BX_UHCI_THIS hub.usb_command.max_packet_size << 7
            | BX_UHCI_THIS hub.usb_command.configured << 6
            | BX_UHCI_THIS hub.usb_command.debug << 5
            | BX_UHCI_THIS hub.usb_command.resume << 4
            | BX_UHCI_THIS hub.usb_command.suspend << 3
            | BX_UHCI_THIS hub.usb_command.reset << 2
            | BX_UHCI_THIS hub.usb_command.host_reset << 1
            | BX_UHCI_THIS hub.usb_command.schedule;
      break;

    case 0x02: // status register (16-bit)
      val = BX_UHCI_THIS hub.usb_status.host_halted << 5
            | BX_UHCI_THIS hub.usb_status.host_error << 4
            | BX_UHCI_THIS hub.usb_status.pci_error << 3
            | BX_UHCI_THIS hub.usb_status.resume << 2
            | BX_UHCI_THIS hub.usb_status.error_interrupt << 1
            | BX_UHCI_THIS hub.usb_status.interrupt;
      break;

    case 0x04: // interrupt enable register (16-bit)
      val = BX_UHCI_THIS hub.usb_enable.short_packet << 3
            | BX_UHCI_THIS hub.usb_enable.on_complete << 2
            | BX_UHCI_THIS hub.usb_enable.resume << 1
            | BX_UHCI_THIS hub.usb_enable.timeout_crc;
      break;

    case 0x06: // frame number register (16-bit)
      val = BX_UHCI_THIS hub.usb_frame_num.frame_num;
      break;

    case 0x08: // frame base register (32-bit)
      val = BX_UHCI_THIS hub.usb_frame_base.frame_base;
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      val = BX_UHCI_THIS hub.usb_sof.sof_timing;
      break;

    case 0x14: // port #3 non existant, but linux systems check it to see if there are more than 2
      BX_ERROR(("read from non existant offset 0x14 (port #3)"));
      val = 0xFF7F;
      break;

    case 0x10: // port #1
    case 0x11:
    case 0x12: // port #2
    case 0x13:
      port = (offset & 0x0F) >> 1;
      if (port < BX_N_USB_UHCI_PORTS) {
        val = BX_UHCI_THIS hub.usb_port[port].suspend << 12
              |                                       1 << 10  // some Root Hubs have bit 10 set ?????
              | BX_UHCI_THIS hub.usb_port[port].reset << 9
              | BX_UHCI_THIS hub.usb_port[port].low_speed << 8
              | 1 << 7
              | BX_UHCI_THIS hub.usb_port[port].resume << 6
              | BX_UHCI_THIS hub.usb_port[port].line_dminus << 5
              | BX_UHCI_THIS hub.usb_port[port].line_dplus << 4
              | BX_UHCI_THIS hub.usb_port[port].able_changed << 3
              | BX_UHCI_THIS hub.usb_port[port].enabled << 2
              | BX_UHCI_THIS hub.usb_port[port].connect_changed << 1
              | BX_UHCI_THIS hub.usb_port[port].status;
        if (offset & 1) val >>= 8;
        break;
      } // else fall through to default
    default:
      val = 0xFF7F; // keep compiler happy
      BX_ERROR(("unsupported io read from address=0x%04x!", (unsigned) address));
      break;
  }

  BX_DEBUG(("register read from address 0x%04X:  0x%08X (%2i bits)", (unsigned) address, (Bit32u) val, io_len * 8));

  return(val);
}

// static IO port write callback handler
// redirects to non-static class handler to avoid virtual functions

void bx_usb_uhci_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_USB_UHCI_SMF
  bx_usb_uhci_c *class_ptr = (bx_usb_uhci_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_usb_uhci_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif // !BX_USE_USB_UHCI_SMF
  Bit8u  offset,port;

  BX_DEBUG(("register write to  address 0x%04X:  0x%08X (%2i bits)", (unsigned) address, (unsigned) value, io_len * 8));

  offset = address - BX_UHCI_THIS pci_base_address[4];

  switch (offset) {
    case 0x00: // command register (16-bit) (R/W)
      if (value & 0xFF00)
        BX_DEBUG(("write to command register with bits 15:8 not zero: 0x%04x", value));

      BX_UHCI_THIS hub.usb_command.max_packet_size = (value & 0x80) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.configured = (value & 0x40) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.debug = (value & 0x20) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.resume = (value & 0x10) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.suspend = (value & 0x08) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.reset = (value & 0x04) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.host_reset = (value & 0x02) ? 1: 0;
      BX_UHCI_THIS hub.usb_command.schedule = (value & 0x01) ? 1: 0;

      // HCRESET
      if (BX_UHCI_THIS hub.usb_command.host_reset) {
        BX_UHCI_THIS reset(0);
        for (unsigned i=0; i<BX_N_USB_UHCI_PORTS; i++) {
          if (BX_UHCI_THIS hub.usb_port[i].status) {
            if (BX_UHCI_THIS hub.usb_port[i].device != NULL) {
              DEV_usb_send_msg(BX_UHCI_THIS hub.usb_port[i].device, USB_MSG_RESET);
            }
            BX_UHCI_THIS hub.usb_port[i].connect_changed = 1;
            if (BX_UHCI_THIS hub.usb_port[i].enabled) {
              BX_UHCI_THIS hub.usb_port[i].able_changed = 1;
              BX_UHCI_THIS hub.usb_port[i].enabled = 0;
            }
          }
        }
      }

      // If software set the GRESET bit, we need to send the reset to all USB.
      // The software should guarentee that the reset is for at least 10ms.
      // We hold the reset until software resets this bit
      if (BX_UHCI_THIS hub.usb_command.reset) {
        BX_UHCI_THIS global_reset = 1;
        BX_DEBUG(("Global Reset"));
      } else {
        // if software cleared the reset, then we need to reset the usb registers.
        if (BX_UHCI_THIS global_reset) {
          BX_UHCI_THIS global_reset = 0;
          unsigned int running = BX_UHCI_THIS hub.usb_command.schedule;
          BX_UHCI_THIS reset(0);
          BX_UHCI_THIS hub.usb_status.host_halted = (running) ? 1 : 0;
        }
      }

      // If Run/Stop, identify in log
      if (BX_UHCI_THIS hub.usb_command.schedule) {
        BX_UHCI_THIS hub.usb_status.host_halted = 0;
        BX_DEBUG(("Schedule bit set in Command register"));
      } else {
        BX_UHCI_THIS hub.usb_status.host_halted = 1;
        BX_DEBUG(("Schedule bit clear in Command register"));
      }

      // If Debug mode set, panic.  Not implemented
      if (BX_UHCI_THIS hub.usb_command.debug)
        BX_PANIC(("Software set DEBUG bit in Command register. Not implemented"));

      break;

    case 0x02: // status register (16-bit) (R/WC)
      if (value & 0xFFC0)
        BX_DEBUG(("write to status register with bits 15:6 not zero: 0x%04x", value));

      // host_halted, even though not specified in the specs, is read only
      //BX_UHCI_THIS hub.usb_status.host_halted = (value & 0x20) ? 0: BX_UHCI_THIS hub.usb_status.host_halted;
      BX_UHCI_THIS hub.usb_status.host_error = (value & 0x10) ? 0: BX_UHCI_THIS hub.usb_status.host_error;
      BX_UHCI_THIS hub.usb_status.pci_error = (value & 0x08) ? 0: BX_UHCI_THIS hub.usb_status.pci_error;
      BX_UHCI_THIS hub.usb_status.resume = (value & 0x04) ? 0: BX_UHCI_THIS hub.usb_status.resume;
      BX_UHCI_THIS hub.usb_status.error_interrupt = (value & 0x02) ? 0: BX_UHCI_THIS hub.usb_status.error_interrupt;
      BX_UHCI_THIS hub.usb_status.interrupt = (value & 0x01) ? 0: BX_UHCI_THIS hub.usb_status.interrupt;
      if (value & 0x01) {
        BX_UHCI_THIS hub.usb_status.status2 = 0;
      }
      update_irq();
      break;

    case 0x04: // interrupt enable register (16-bit)
      if (value & 0xFFF0)
        BX_DEBUG(("write to interrupt enable register with bits 15:4 not zero: 0x%04x", value));

      BX_UHCI_THIS hub.usb_enable.short_packet  = (value & 0x08) ? 1: 0;
      BX_UHCI_THIS hub.usb_enable.on_complete  = (value & 0x04) ? 1: 0;
      BX_UHCI_THIS hub.usb_enable.resume  = (value & 0x02) ? 1: 0;
      BX_UHCI_THIS hub.usb_enable.timeout_crc = (value & 0x01) ? 1: 0;

      if (value & 0x08) {
        BX_DEBUG(("Host set Enable Interrupt on Short Packet"));
      }
      if (value & 0x04) {
        BX_DEBUG(("Host set Enable Interrupt on Complete"));
      }
      if (value & 0x02) {
        BX_DEBUG(("Host set Enable Interrupt on Resume"));
      }
      update_irq();
      break;

    case 0x06: // frame number register (16-bit)
      if (value & 0xF800)
        BX_DEBUG(("write to frame number register with bits 15:11 not zero: 0x%04x", value));

      if (BX_UHCI_THIS hub.usb_status.host_halted)
        BX_UHCI_THIS hub.usb_frame_num.frame_num = (value & 0x07FF);
      else
        // ignored by the hardward, but lets report it anyway
        BX_DEBUG(("write to frame number register with STATUS.HALTED == 0"));
      break;

    case 0x08: // frame base register (32-bit)
      if (value & 0xFFF)
        BX_DEBUG(("write to frame base register with bits 11:0 not zero: 0x%08x", value));

      BX_UHCI_THIS hub.usb_frame_base.frame_base = (value & ~0xfff);
      break;

    case 0x0C: // start of Frame Modify register (8-bit)
      if (value & 0x80)
        BX_DEBUG(("write to SOF Modify register with bit 7 not zero: 0x%04x", value));

       BX_UHCI_THIS hub.usb_sof.sof_timing = value;
       break;

    case 0x14: // port #3 non existant, but linux systems check it to see if there are more than 2
      BX_ERROR(("write to non existant offset 0x14 (port #3)"));
      break;

    case 0x10: // port #1
    case 0x12: // port #2
      port = (offset & 0x0F) >> 1;
      if ((port < BX_N_USB_UHCI_PORTS) && (io_len == 2)) {
        // If the ports reset bit is set, don't allow any writes unless the new write will clear the reset bit
        if (BX_UHCI_THIS hub.usb_port[port].reset & (value & (1<<9)))
          break;
        if (value & ((1<<5) | (1<<4) | (1<<0)))
          BX_DEBUG(("write to one or more read-only bits in port #%d register: 0x%04x", port+1, value));
        if (!(value & (1<<7)))
          BX_DEBUG(("write to port #%d register bit 7 = 0", port+1));
        if (value & (1<<8))
          BX_DEBUG(("write to bit 8 in port #%d register ignored", port+1));
        if ((value & (1<<12)) && BX_UHCI_THIS hub.usb_command.suspend)
          BX_DEBUG(("write to port #%d register bit 12 when in Global-Suspend", port+1));

        BX_UHCI_THIS hub.usb_port[port].suspend = (value & (1<<12)) ? 1 : 0;
        BX_UHCI_THIS hub.usb_port[port].reset = (value & (1<<9)) ? 1 : 0;
        BX_UHCI_THIS hub.usb_port[port].resume = (value & (1<<6)) ? 1 : 0;
        if (!BX_UHCI_THIS hub.usb_port[port].enabled && (value & (1<<2)))
          BX_UHCI_THIS hub.usb_port[port].able_changed = 0;
        else
          if ((value & (1<<3)) != 0) BX_UHCI_THIS hub.usb_port[port].able_changed = 0;
        BX_UHCI_THIS hub.usb_port[port].enabled = (value & (1<<2)) ? 1 : 0;
        if ((value & (1<<1)) != 0) BX_UHCI_THIS hub.usb_port[port].connect_changed = 0;

        // if port reset, reset function(s)
        //TODO: only reset items on the downstream...
        // for now, reset the one and only
        // TODO: descriptors, etc....
        if (BX_UHCI_THIS hub.usb_port[port].reset) {
          BX_UHCI_THIS hub.usb_port[port].suspend = 0;
          BX_UHCI_THIS hub.usb_port[port].resume = 0;
          BX_UHCI_THIS hub.usb_port[port].enabled = 0;
          // are we are currently connected/disconnected
          if (BX_UHCI_THIS hub.usb_port[port].status) {
            if (BX_UHCI_THIS hub.usb_port[port].device != NULL) {
              BX_UHCI_THIS hub.usb_port[port].low_speed =
                (BX_UHCI_THIS hub.usb_port[port].device->get_speed() == USB_SPEED_LOW);
              usb_set_connect_status(port, BX_UHCI_THIS hub.usb_port[port].device->get_type(), 1);
              DEV_usb_send_msg(BX_UHCI_THIS hub.usb_port[port].device, USB_MSG_RESET);
            }
          }
          BX_INFO(("Port%d: Reset", port+1));
        }
        break;
      }
      // else fall through to default
    default:
      BX_ERROR(("unsupported io write to address=0x%04x!", (unsigned) address));
      break;
  }
}

void bx_usb_uhci_c::usb_timer_handler(void *this_ptr)
{
  bx_usb_uhci_c *class_ptr = (bx_usb_uhci_c *) this_ptr;
  class_ptr->usb_timer();
}

// Called once every 1ms
#define USB_STACK_SIZE  256
void bx_usb_uhci_c::usb_timer(void)
{
  int i;

  // If the "global reset" bit was set by software
  if (BX_UHCI_THIS global_reset) {
    for (i=0; i<BX_N_USB_UHCI_PORTS; i++) {
      BX_UHCI_THIS hub.usb_port[i].able_changed = 0;
      BX_UHCI_THIS hub.usb_port[i].connect_changed = 0;
      BX_UHCI_THIS hub.usb_port[i].enabled = 0;
      BX_UHCI_THIS hub.usb_port[i].line_dminus = 0;
      BX_UHCI_THIS hub.usb_port[i].line_dplus = 0;
      BX_UHCI_THIS hub.usb_port[i].low_speed = 0;
      BX_UHCI_THIS hub.usb_port[i].reset = 0;
      BX_UHCI_THIS hub.usb_port[i].resume = 0;
      BX_UHCI_THIS hub.usb_port[i].status = 0;
      BX_UHCI_THIS hub.usb_port[i].suspend = 0;
    }
    return;
  }

  // If command.schedule = 1, then run schedule
  //  *** This assumes that we can complete the frame within the 1ms time allowed ***
  // Actually, not complete, but reach the end of the frame.  This means that there may still
  //  be TDs and QHs that were BREADTH defined and will be executed on the next cycle/iteration.

  if (BX_UHCI_THIS busy) {
    BX_PANIC(("Did not complete last frame before the 1ms was over. Starting next frame."));
    BX_UHCI_THIS busy = 0;
  }
  if (BX_UHCI_THIS hub.usb_command.schedule) {
    BX_UHCI_THIS busy = 1;
    bx_bool interrupt = 0, shortpacket = 0, stalled = 0;
    struct TD td;
    struct HCSTACK stack[USB_STACK_SIZE+1];  // queue stack for this item only
    Bit32s stk = 0;
    Bit32u item, address, lastvertaddr = 0, queue_num = 0;
    Bit32u frame, frm_addr = BX_UHCI_THIS hub.usb_frame_base.frame_base +
                                (BX_UHCI_THIS hub.usb_frame_num.frame_num << 2);
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
            BX_DEBUG(("Frame: %04i (0x%04X)", BX_UHCI_THIS hub.usb_frame_num.frame_num, BX_UHCI_THIS hub.usb_frame_num.frame_num));
            if (BX_UHCI_THIS DoTransfer(address, queue_num, &td)) {
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
              if (shortpacket) {
                td.dword0 |= 1;
                stack[stk].t = 1;
              }
              // copy pointer for next queue item, in to vert queue head
              if ((stk > 0) && (stack[stk].d == HC_VERT) && !shortpacket)
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
      if (shortpacket) BX_UHCI_THIS hub.usb_status.status2 |= 2;
      if (shortpacket && BX_UHCI_THIS hub.usb_enable.short_packet) {
        BX_DEBUG((" [SPD] We want it to fire here (Frame: %04i)", BX_UHCI_THIS hub.usb_frame_num.frame_num));
      }

      // if one of the TD's in this frame had the ioc bit set, we need to
      //   raise an interrupt, if interrupts are not masked via interrupt register.
      //   always set the status register if IOC.
      BX_UHCI_THIS hub.usb_status.status2 |= interrupt;
      if (interrupt && BX_UHCI_THIS hub.usb_enable.on_complete) {
        BX_DEBUG((" [IOC] We want it to fire here (Frame: %04i)", BX_UHCI_THIS hub.usb_frame_num.frame_num));
      }

      BX_UHCI_THIS hub.usb_status.error_interrupt |= stalled;
      if (stalled && BX_UHCI_THIS hub.usb_enable.timeout_crc) {
        BX_DEBUG((" [stalled] We want it to fire here (Frame: %04i)", BX_UHCI_THIS hub.usb_frame_num.frame_num));
      }
    }

    // The Frame Number Register is incremented every 1ms
    BX_UHCI_THIS hub.usb_frame_num.frame_num++;
    BX_UHCI_THIS hub.usb_frame_num.frame_num &= (1024-1);

    // The status.interrupt bit should be set regardless of the enable bits if a IOC or SPD is found
    if (interrupt || shortpacket) {
      BX_UHCI_THIS hub.usb_status.interrupt = 1;
    }
    // if we needed to fire an interrupt now, lets do it *after* we increment the frame_num register
    update_irq();

    BX_UHCI_THIS busy = 0;  // ready to do next frame item
  }  // end run schedule

  // if host turned off the schedule, set the halted bit in the status register
  // Note: Can not use an else from the if() above since the host can changed this bit
  //  while we are processing a frame.
  if (BX_UHCI_THIS hub.usb_command.schedule == 0)
    BX_UHCI_THIS hub.usb_status.host_halted = 1;

  // TODO:
  //  If in Global_Suspend mode and any of usb_port[i] bits 6,3, or 1 are set,
  //    we need to issue a Global_Resume (set the global resume bit).
  //    However, since we don't do anything, let's not.
}

bx_bool bx_usb_uhci_c::DoTransfer(Bit32u address, Bit32u queue_num, struct TD *td) {

  int len = 0, ret = 0;

  Bit16u maxlen = (td->dword2 >> 21);
  Bit8u  addr   = (td->dword2 >> 8) & 0x7F;
  Bit8u  endpt  = (td->dword2 >> 15) & 0x0F;
  Bit8u  pid    =  td->dword2 & 0xFF;

  BX_DEBUG(("QH%03i:TD found at address: 0x%08X", queue_num, address));
  BX_DEBUG(("  %08X   %08X   %08X   %08X", td->dword0, td->dword1, td->dword2, td->dword3));

  // check TD to make sure it is valid
  // A max length 0x500 to 0x77E is illegal
  if ((maxlen >= 0x500) && (maxlen != 0x7FF)) {
    BX_ERROR(("invalid max. length value 0x%04x", maxlen ));
    return 0;  // error = consistency check failure
  }

  // if (td->dword0 & 0x8) return 1; // error = reserved bit in dword0 set
  // other error checks here

  // the device should remain in a stall state until the next setup packet is recieved
  // For some reason, this doesn't work yet.
  //if (dev && dev->in_stall && (pid != USB_TOKEN_SETUP))
  //  return FALSE;

  maxlen++;
  maxlen &= 0x7FF;

  /* set status bar conditions for device */
  if ((maxlen > 0) && (BX_UHCI_THIS hub.statusbar_id >= 0)) {
    if (pid == USB_TOKEN_IN)
      bx_gui->statusbar_setitem(BX_UHCI_THIS hub.statusbar_id, 1);     // read
    else
      bx_gui->statusbar_setitem(BX_UHCI_THIS hub.statusbar_id, 1, 1);  // write
  }

  BX_UHCI_THIS usb_packet.pid = pid;
  BX_UHCI_THIS usb_packet.devaddr = addr;
  BX_UHCI_THIS usb_packet.devep = endpt;
  BX_UHCI_THIS usb_packet.data = device_buffer;
  BX_UHCI_THIS usb_packet.len = maxlen;
  switch (pid) {
    case USB_TOKEN_OUT:
    case USB_TOKEN_SETUP:
      if (maxlen > 0) {
        DEV_MEM_READ_PHYSICAL_DMA(td->dword3, maxlen, device_buffer);
      }
      ret = BX_UHCI_THIS broadcast_packet(&BX_UHCI_THIS usb_packet);
      len = maxlen;
      break;
    case USB_TOKEN_IN:
      ret = BX_UHCI_THIS broadcast_packet(&BX_UHCI_THIS usb_packet);
      if (ret >= 0) {
        len = ret;
        if (len > maxlen) {
          len = maxlen;
          ret = USB_RET_BABBLE;
        }
        if (len > 0) {
          DEV_MEM_WRITE_PHYSICAL_DMA(td->dword3, len, device_buffer);
        }
      } else {
        len = 0;
      }
      break;
    default:
      BX_UHCI_THIS hub.usb_status.host_error = 1;
      update_irq();
      return 0;
  }
  if (ret >= 0) {
    BX_UHCI_THIS set_status(td, 0, 0, 0, 0, 0, 0, len-1);
  } else {
    BX_UHCI_THIS set_status(td, 1, 0, 0, 0, 0, 0, 0x007); // stalled
  }
  return 1;
}

int bx_usb_uhci_c::broadcast_packet(USBPacket *p)
{
  int i, ret;

  ret = USB_RET_NODEV;
  for (i = 0; i < BX_N_USB_UHCI_PORTS && ret == USB_RET_NODEV; i++) {
    if ((BX_UHCI_THIS hub.usb_port[i].device != NULL) &&
        (BX_UHCI_THIS hub.usb_port[i].enabled)) {
      ret = BX_UHCI_THIS hub.usb_port[i].device->handle_packet(p);
    }
  }
  return ret;
}

// If the request fails, set the stall bit ????
void bx_usb_uhci_c::set_status(struct TD *td, bx_bool stalled, bx_bool data_buffer_error, bx_bool babble,
                             bx_bool nak, bx_bool crc_time_out, bx_bool bitstuff_error, Bit16u act_len)
{
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
}

void bx_usb_uhci_c::runtime_config_handler(void *this_ptr)
{
  bx_usb_uhci_c *class_ptr = (bx_usb_uhci_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_usb_uhci_c::runtime_config(void)
{
  int i;
  char pname[6];

  for (i = 0; i < BX_N_USB_UHCI_PORTS; i++) {
    // device change support
    if ((BX_UHCI_THIS hub.device_change & (1 << i)) != 0) {
      BX_INFO(("USB port #%d: device connect", i+1));
      sprintf(pname, "port%d", i + 1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_UHCI)));
      BX_UHCI_THIS hub.device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_UHCI_THIS hub.usb_port[i].device != NULL) {
      BX_UHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// pci configuration space read callback handler
Bit32u bx_usb_uhci_c::pci_read_handler(Bit8u address, unsigned io_len)
{
  Bit32u value = 0;

  for (unsigned i=0; i<io_len; i++) {
    value |= (BX_UHCI_THIS pci_conf[address+i] << (i*8));
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
void bx_usb_uhci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  Bit8u value8, oldval;
  bx_bool baseaddr_change = 0;

  if (((address >= 0x10) && (address < 0x20)) ||
      ((address > 0x23) && (address < 0x34)))
    return;

  for (unsigned i=0; i<io_len; i++) {
    value8 = (value >> (i*8)) & 0xFF;
    oldval = BX_UHCI_THIS pci_conf[address+i];
    switch (address+i) {
      case 0x04:
        value8 &= 0x05;
        BX_UHCI_THIS pci_conf[address+i] = value8;
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
          BX_UHCI_THIS pci_conf[address+i] = value8;
        }
        break;
      case 0x20:
        value8 = (value8 & 0xfc) | 0x01;
      case 0x21:
      case 0x22:
      case 0x23:
        baseaddr_change |= (value8 != oldval);
      default:
        BX_UHCI_THIS pci_conf[address+i] = value8;
    }
  }
  if (baseaddr_change) {
    if (DEV_pci_set_base_io(BX_UHCI_THIS_PTR, read_handler, write_handler,
                            &BX_UHCI_THIS pci_base_address[4],
                            &BX_UHCI_THIS pci_conf[0x20],
                            32, &uhci_iomask[0], "USB UHCI Hub")) {
      BX_INFO(("new base address: 0x%04x", BX_UHCI_THIS pci_base_address[4]));
    }
  }

  if (io_len == 1)
    BX_DEBUG(("write PCI register 0x%02x value 0x%02x", address, value));
  else if (io_len == 2)
    BX_DEBUG(("write PCI register 0x%02x value 0x%04x", address, value));
  else if (io_len == 4)
    BX_DEBUG(("write PCI register 0x%02x value 0x%08x", address, value));
}

const char *usb_speed[4] = {
  "low",
  "full",
  "high",
  "super"
};

void bx_usb_uhci_c::usb_set_connect_status(Bit8u port, int type, bx_bool connected)
{
  usb_device_c *device = BX_UHCI_THIS hub.usb_port[port].device;
  if (device != NULL) {
    if (device->get_type() == type) {
      if (connected) {
        BX_INFO(("port #%d: speed = %s", port+1, usb_speed[device->get_speed()]));
        switch (device->get_speed()) {
          case USB_SPEED_LOW:
            BX_UHCI_THIS hub.usb_port[port].low_speed = 1;
            break;
          case USB_SPEED_FULL:
            BX_UHCI_THIS hub.usb_port[port].low_speed = 0;
            break;
          case USB_SPEED_HIGH:
          case USB_SPEED_SUPER:
            BX_PANIC(("HC supports 'low' or 'full' speed devices only."));
            device->set_speed(USB_SPEED_FULL);
            break;
          default:
            BX_ERROR(("device->get_speed() returned invalid speed value"));
        }
        if (BX_UHCI_THIS hub.usb_port[port].low_speed) {
          BX_UHCI_THIS hub.usb_port[port].line_dminus = 1;  //  dminus=1 & dplus=0 = low speed  (at idle time)
          BX_UHCI_THIS hub.usb_port[port].line_dplus = 0;   //  dminus=0 & dplus=1 = high speed (at idle time)
        } else {
          BX_UHCI_THIS hub.usb_port[port].line_dminus = 0;
          BX_UHCI_THIS hub.usb_port[port].line_dplus = 1;
        }
        BX_UHCI_THIS hub.usb_port[port].status = 1;
        BX_UHCI_THIS hub.usb_port[port].connect_changed = 1;

        // if in suspend state, signal resume
        if (BX_UHCI_THIS hub.usb_command.suspend) {
          BX_UHCI_THIS hub.usb_port[port].resume = 1;
          BX_UHCI_THIS hub.usb_status.resume = 1;
          if (BX_UHCI_THIS hub.usb_enable.resume) {
            BX_UHCI_THIS hub.usb_status.interrupt = 1;
          }
          update_irq();
        }

        if (!device->get_connected()) {
          if (!device->init()) {
            usb_set_connect_status(port, type, 0);
            BX_ERROR(("port #%d: connect failed", port+1));
          } else {
            BX_INFO(("port #%d: connect: %s", port+1, device->get_info()));
          }
        }
      } else {
        BX_UHCI_THIS hub.usb_port[port].status = 0;
        BX_UHCI_THIS hub.usb_port[port].connect_changed = 1;
        if (BX_UHCI_THIS hub.usb_port[port].enabled) {
          BX_UHCI_THIS hub.usb_port[port].able_changed = 1;
          BX_UHCI_THIS hub.usb_port[port].enabled = 0;
        }
        BX_UHCI_THIS hub.usb_port[port].low_speed = 0;
        BX_UHCI_THIS hub.usb_port[port].line_dminus = 0;
        BX_UHCI_THIS hub.usb_port[port].line_dplus = 0;
        remove_device(port);
      }
    }
  }
}

// USB runtime parameter handler
const char *bx_usb_uhci_c::usb_param_handler(bx_param_string_c *param, int set,
                                           const char *oldval, const char *val, int maxlen)
{
  usbdev_type type = USB_DEV_TYPE_NONE;
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
    if ((portnum >= 0) && (portnum < BX_N_USB_UHCI_PORTS)) {
      if (empty && BX_UHCI_THIS hub.usb_port[portnum].status) {
        BX_INFO(("USB port #%d: device disconnect", portnum+1));
        if (BX_UHCI_THIS hub.usb_port[portnum].device != NULL) {
          type = BX_UHCI_THIS hub.usb_port[portnum].device->get_type();
        }
        usb_set_connect_status(portnum, type, 0);
      } else if (!empty && !BX_UHCI_THIS hub.usb_port[portnum].status) {
        BX_UHCI_THIS hub.device_change |= (1 << portnum);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI
