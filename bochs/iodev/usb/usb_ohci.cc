/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2009-2024  The Bochs Project
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
#include "usb_ohci.h"

#define LOG_THIS theUSB_OHCI->

bx_usb_ohci_c* theUSB_OHCI = NULL;

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
    SIM->init_usb_options("OHCI", "ohci", USB_OHCI_PORTS, 0);
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
  device_change = 0;
  rt_conf_id = -1;
}

bx_usb_ohci_c::~bx_usb_ohci_c()
{
  char pname[32];

  SIM->unregister_runtime_config_handler(rt_conf_id);

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
  BX_OHCI_THIS init_ohci(0x00, 0x11c1, 0x5803, 0x11, 0, BX_PCI_INTD);

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
  BX_OHCI_THIS rt_conf_id = SIM->register_runtime_config_handler(BX_OHCI_THIS_PTR, runtime_config_handler);
  BX_OHCI_THIS device_change = 0;

#if BX_USB_DEBUGGER
  if (SIM->get_param_enum(BXPN_USB_DEBUG_TYPE)->get() == USB_DEBUG_OHCI) {
    SIM->register_usb_debug_type(USB_DEBUG_OHCI);
  }
#endif

  BX_INFO(("USB OHCI initialized"));
}

void bx_usb_ohci_c::reset(unsigned type)
{
  unsigned i;
  char pname[6];

  BX_OHCI_THIS reset_ohci(type);
  for (i=0; i<USB_OHCI_PORTS; i++) {
    if (BX_OHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c *) SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
    }
  }
}

void bx_usb_ohci_c::register_state()
{
  BX_OHCI_THIS ohci_register_state(SIM->get_bochs_root());
}

void bx_usb_ohci_c::after_restore_state()
{
  bx_ohci_core_c::after_restore_state();
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

void bx_usb_ohci_c::runtime_config_handler(void *this_ptr)
{
  if (this_ptr != NULL) {
    bx_usb_ohci_c *class_ptr = (bx_usb_ohci_c *) this_ptr;
    class_ptr->runtime_config();
  }
}

void bx_usb_ohci_c::runtime_config(void)
{
  char pname[6];

  for (int i = 0; i < USB_OHCI_PORTS; i++) {
    // device change support
    if ((BX_OHCI_THIS device_change & (1 << i)) != 0) {
      if (!BX_OHCI_THIS hub.usb_port[i].HcRhPortStatus.ccs) {
        sprintf(pname, "port%d", i + 1);
        init_device(i, (bx_list_c*)SIM->get_param(pname, SIM->get_param(BXPN_USB_OHCI)));
      } else {
        set_connect_status(i, 0);
        remove_device(i);
      }
      BX_OHCI_THIS device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_OHCI_THIS hub.usb_port[i].device != NULL) {
      BX_OHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// USB runtime parameter handler
Bit64s bx_usb_ohci_c::usb_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  if (set) {
    int portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bool empty = (val == 0);
    if ((portnum >= 0) && (portnum < USB_OHCI_PORTS)) {
      if (empty && BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_OHCI_THIS device_change |= (1 << portnum);
      } else if (!empty && !BX_OHCI_THIS hub.usb_port[portnum].HcRhPortStatus.ccs) {
        BX_OHCI_THIS device_change |= (1 << portnum);
      } else if (val != ((bx_param_enum_c *) param)->get()) {
        BX_ERROR(("usb_param_handler(): port #%d already in use", portnum+1));
        val = ((bx_param_enum_c *) param)->get();
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
  if (set && val) {
    if (BX_OHCI_THIS hub.op_regs.HcRhDescriptorA.nocp == 0) {
      int portnum = atoi((param->get_parent())->get_name()+4) - 1;
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
          BX_OHCI_THIS set_interrupt(OHCI_INTR_RHSC);
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
