/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2015  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2018  The Bochs Project
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

// USB UHCI adapter
// PIIX3/PIIX4 function 2

// Notes by Ben Lunt (see uhci_core.cc)

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI

#include "pci.h"
#include "usb_common.h"
#include "uhci_core.h"
#include "usb_uhci.h"

#define LOG_THIS theUSB_UHCI->

bx_usb_uhci_c* theUSB_UHCI = NULL;

// builtin configuration handling functions

Bit32s usb_uhci_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "usb_uhci")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_UHCI);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_UHCI_ENABLED)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "port", 4)) {
        if (SIM->parse_usb_port_params(context, 0, params[i], USB_UHCI_PORTS, base) < 0) {
          return -1;
        }
      } else if (!strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, 1, params[i], USB_UHCI_PORTS, base) < 0) {
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
  SIM->write_usb_options(fp, USB_UHCI_PORTS, base);
  return 0;
}

// device plugin entry points

int CDECL libusb_uhci_LTX_plugin_init(plugin_t *plugin, plugintype_t type)
{
  theUSB_UHCI = new bx_usb_uhci_c();
  BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_UHCI, BX_PLUGIN_USB_UHCI);
  // add new configuration parameter for the config interface
  SIM->init_usb_options("UHCI", "uhci", USB_UHCI_PORTS);
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
  rt_conf_id = -1;
}

bx_usb_uhci_c::~bx_usb_uhci_c()
{
  char pname[16];

  SIM->unregister_runtime_config_handler(rt_conf_id);

  for (int i=0; i<USB_UHCI_PORTS; i++) {
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
  Bit8u devfunc;
  Bit16u devid;

  // Read in values from config interface
  uhci = (bx_list_c*) SIM->get_param(BXPN_USB_UHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", uhci)->get()) {
    BX_INFO(("USB UHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c*)((bx_list_c*)SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_uhci"))->set(0);
    return;
  }

  if (SIM->get_param_enum(BXPN_PCI_CHIPSET)->get() == BX_PCI_CHIPSET_I440FX) {
    devfunc = BX_PCI_DEVICE(1, 2);
    devid = 0x7020;
  } else if (SIM->get_param_enum(BXPN_PCI_CHIPSET)->get() == BX_PCI_CHIPSET_I440BX) {
    devfunc = BX_PCI_DEVICE(7, 2);
    devid = 0x7112;
  } else {
    devfunc = 0x00;
    devid = 0x7020;
  }
  BX_UHCI_THIS init_uhci(devfunc, devid, 0x00, BX_PCI_INTD);

  bx_list_c *usb_rt = (bx_list_c*)SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *uhci_rt = new bx_list_c(usb_rt, "uhci", "UHCI Runtime Options");
  uhci_rt->set_options(uhci_rt->SHOW_PARENT);
  for (i=0; i<USB_UHCI_PORTS; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c*)SIM->get_param(pname, uhci);
    uhci_rt->add(port);
    device = (bx_param_string_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
  }

  // register handler for correct device connect handling after runtime config
  BX_UHCI_THIS rt_conf_id = SIM->register_runtime_config_handler(BX_UHCI_THIS_PTR, runtime_config_handler);
  BX_UHCI_THIS device_change = 0;

  BX_INFO(("USB UHCI initialized"));
}

void bx_usb_uhci_c::reset(unsigned type)
{
  unsigned i;
  char pname[6];

  BX_UHCI_THIS reset_uhci(type);
  for (i=0; i<USB_UHCI_PORTS; i++) {
    if (BX_UHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_UHCI)));
    }
  }
}

void bx_usb_uhci_c::register_state()
{
  bx_uhci_core_c::register_state(SIM->get_bochs_root());
}

void bx_usb_uhci_c::after_restore_state()
{
  bx_uhci_core_c::after_restore_state();
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
    set_connect_status(port, type, 1);
  }
}

void bx_usb_uhci_c::remove_device(Bit8u port)
{
  if (BX_UHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_UHCI_THIS hub.usb_port[port].device;
    BX_UHCI_THIS hub.usb_port[port].device = NULL;
  }
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
  usbdev_type type = USB_DEV_TYPE_NONE;

  for (i = 0; i < USB_UHCI_PORTS; i++) {
    // device change support
    if ((BX_UHCI_THIS device_change & (1 << i)) != 0) {
      if (!BX_UHCI_THIS hub.usb_port[i].status) {
        BX_INFO(("USB port #%d: device connect", i+1));
        sprintf(pname, "port%d", i + 1);
        init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_UHCI)));
      } else {
        BX_INFO(("USB port #%d: device disconnect", i+1));
        if (BX_UHCI_THIS hub.usb_port[i].device != NULL) {
          type = BX_UHCI_THIS hub.usb_port[i].device->get_type();
        }
        set_connect_status(i, type, 0);
        remove_device(i);
      }
      BX_UHCI_THIS device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_UHCI_THIS hub.usb_port[i].device != NULL) {
      BX_UHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// USB runtime parameter handler
const char *bx_usb_uhci_c::usb_param_handler(bx_param_string_c *param, int set,
                                           const char *oldval, const char *val, int maxlen)
{
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bx_bool empty = ((strlen(val) == 0) || (!strcmp(val, "none")));
    if ((portnum >= 0) && (portnum < USB_UHCI_PORTS)) {
      if (empty && BX_UHCI_THIS hub.usb_port[portnum].status) {
        BX_UHCI_THIS device_change |= (1 << portnum);
      } else if (!empty && !BX_UHCI_THIS hub.usb_port[portnum].status) {
        BX_UHCI_THIS device_change |= (1 << portnum);
      }
    } else {
      BX_PANIC(("usb_param_handler called with unexpected parameter '%s'", param->get_name()));
    }
  }
  return val;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_UHCI
