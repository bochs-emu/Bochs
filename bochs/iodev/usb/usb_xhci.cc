/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2010-2023  Benjamin D Lunt (fys [at] fysnet [dot] net)
//                2011-2023  The Bochs Project
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

// USB xHCI adapter

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

/* Notes by Ben Lunt:
 *  This emulates a NEC/Renesas uPD720202 2-port (2 socket, 4-port register sets)
 *   Root Hub xHCI Host Controller.
 *  Many, many thanks to Renesas for their work and effort in helping my research.
 *
 *  I have tested in with my own tests and WinXP Home Edition SP3 and Windows 7 Ultimate.
 *
 *  Assuming a two-socket, four-port root hub:
 *   Use port1 and port2 to emulate a Super-speed device, and use port3 and port4
 *    to emulate Low-, Full-, or High-speed devices.
 *  (Other settings, use the first half as Super-speed, and the last half as the other speeds)
 *
 *  Examples:
 *   usb_xhci: enabled=1, port3=mouse, options3="speed:low"
 *   usb_xhci: enabled=1, port3=disk, options3="speed:full, path:usbdisk.img"
 *   usb_xhci: enabled=1, port3=disk, options3="speed:high, path:usbdisk.img"
 *   usb_xhci: enabled=1, port1=disk, options1="speed:super, path:usbdisk.img"
 *   usb_xhci: enabled=1, port1=cdrom, options1="speed:super, path:cdrom.iso"
 *
 *  Currently, only a USB MSD Thumb Drive device ("=disk") and a USB MSD CDROM ("=cdrom")
 *   can be emulated as a Super-speed device.
 *  All other emulated devices must be Low-, Full-, or High-speed devices.
 *
 * This code is 32- and 64-bit addressing aware (uses: bx_phy_address & ADDR_CAP_64)
 *
 * This emulation is coded with the new registers of xHCI version 1.10.  However,
 *  at this time, nothing more has been done to emulate 1.10 features.
 * This emulation is set with VERSION_MAJOR and VERSION_MINOR as 0x01 and 0x00
 *  respectively (version 1.00).
 *
 * This emulation seems to work as expected.  It may not be completly accorate, though
 *  it is my intention to make it as close as possible.  If you find an error or would
 *  like to add to the emulation, please let me know at fys [at] fysnet [dot] net.
 *
 */

#include "iodev.h"

#if BX_SUPPORT_PCI && BX_SUPPORT_USB_XHCI

#include "pci.h"
#include "usb_common.h"
#include "usb_xhci.h"

#define LOG_THIS theUSB_XHCI->

bx_usb_xhci_c *theUSB_XHCI = NULL;

#define PROTOCOL_UBS3_OFFSET  (0x510 - 0x500)  // offset to the USB3 protocol struct
#define PROTOCOL_UBS2_OFFSET  (0x524 - 0x500)  // offset to the USB2 protocol struct
static Bit8u ext_caps[EXT_CAPS_SIZE] = {
  /* 0x500 */ 0x01,                    // Legacy Support
              0x04,                    // next = 0x04 * 4 = 0x510
              0x00, 0x00,              // Semaphores
              0x00, 0x00, 0x00, 0xE0,  // SMI on Bar, SMI on Command, SMI on Semaphore
              0x00, 0x00, 0x00, 0x00,  // reserved
              0x00, 0x00, 0x00, 0x00,  // filler

              // this block is modified by init()
  /* 0x510 */ 0x02,                    // Port Protocol
              0x05,                    // next = 0x05 * 4 = 0x524
              0x00, 0x03,              // Version 3.0
              0x55, 0x53, 0x42, 0x20,  // "USB "
              0x01,                    // 1-based starting index
              0x02,                    // count of 2 port registers starting at base above
              0x00, 0x00,              // flags
              0x00, 0x00, 0x00, 0x00,  // 8 bytes of filler (unnecessary to this emulation
              0x00, 0x00, 0x00, 0x00,  //   but it exists on the uPD720202 hardware)

              // this block is modified by init()
  /* 0x524 */ 0x02,                    // Port Protocol
              0x07,                    // next = 0x07 * 4 = 0x540
              0x00, 0x02,              // Version 2.0
              0x55, 0x53, 0x42, 0x20,  // "USB "
              0x03,                    // 1-based starting index
              0x02,                    // count of 2 port registers starting at base above
              0x00, 0x00,              // flags
              0x00, 0x00, 0x00, 0x00,  // 16 bytes of filler (unnecessary to this emulation
              0x00, 0x00, 0x00, 0x00,  //    but it exists on the uPD720202 hardware)
              0x00, 0x00, 0x00, 0x00,  //
              0x00, 0x00, 0x00, 0x00,  //

  /* The following two items are not needed or used by this emulation, but they
   *  are present in the uPD720202 controller.  I leave them here to closely emulate
   *  the said controller, and with intentions of some day adding the debug support.
   * If you wish to comment them out, please be sure to remember to change the
   *    0x07,            // (next = 0x07 * 4 = 0x540)
   *  above to a
   *    0x00,            // no more items
   */

  /* 0x540 */ 0xC0,                    // Vendor Defined
              0x04,                    // next = 0x04 * 4 = 0x550
              0x00, 0x00,              //
              0x00, 0x00, 0x00, 0x00,  //
              0x00, 0x00, 0x00, 0x00,  //
              0x00, 0x00, 0x00, 0x00,  //

  /* 0x550 */ 0x0A,                    // Debug Registers
              0x00,                    // no more
              0x00, 0x00,              //
              0x00, 0x00, 0x00, 0x00,  // Doorbell Target
              0x00, 0x00, 0x00, 0x00,  // Event Ring Table Size
              0x00, 0x00, 0x00, 0x00,  // Event Ring Table Base Address
              0x00, 0x00, 0x00, 0x00,  //   (64-bit)
              0x00, 0x00, 0x00, 0x00,  // Dequeue Pointer
              0x00, 0x00, 0x00, 0x00,  //   (64-bit)
              0x00, 0x00, 0x00, 0x00,  // Max Burst Size, Dev Address, etc.
              0x00, 0x00, 0x00, 0x00,  // Debug Port Number
              0x00, 0x00, 0x00, 0x00,  // Port Status
              0x00, 0x00, 0x00, 0x00,  // Context Pointer
              0x00, 0x00, 0x00, 0x00,  //   (64-bit)
              0x00, 0x00, 0x00, 0x00,  // DbC Protocol, Vendor ID
              0x00, 0x00, 0x00, 0x00,  // Product ID, Device Revision
              0x00, 0x00, 0x00, 0x00,  // filler
              0x00, 0x00, 0x00, 0x00   // filler
};

// builtin configuration handling functions
Bit32s usb_xhci_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "usb_xhci")) {
    bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_XHCI);
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "enabled=", 8)) {
        SIM->get_param_bool(BXPN_XHCI_ENABLED)->set(atol(&params[i][8]));
      } else if (!strncmp(params[i], "model=", 6)) {
        if (!strcmp(&params[i][6], "uPD720202"))
          SIM->get_param_enum(BXPN_XHCI_MODEL)->set(XHCI_HC_uPD720202);
        else if (!strcmp(&params[i][6], "uPD720201"))
          SIM->get_param_enum(BXPN_XHCI_MODEL)->set(XHCI_HC_uPD720201);
        else
          BX_PANIC(("%s: unknown parameter '%s' for usb_xhci: model=", context, &params[i][6]));
      } else if (!strncmp(params[i], "n_ports=", 8)) {
        int max_ports = (int) strtol(&params[i][8], NULL, 10);
        if ((max_ports >= 2) && (max_ports <= USB_XHCI_PORTS_MAX) && !(max_ports & 1))
          SIM->get_param_num(BXPN_XHCI_N_PORTS)->set(max_ports);
        else
          BX_PANIC(("%s: n_ports= must be at least 2, no more than %d, and an even number.", context, USB_XHCI_PORTS_MAX));
      } else if (!strncmp(params[i], "port", 4) || !strncmp(params[i], "options", 7)) {
        if (SIM->parse_usb_port_params(context, params[i], USB_XHCI_PORTS_MAX, base) < 0) {
          return -1;
        }
      } else {
        BX_ERROR(("%s: unknown parameter '%s' for usb_xhci ignored.", context, params[i]));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

Bit32s usb_xhci_options_save(FILE *fp)
{
  bx_list_c *base = (bx_list_c*) SIM->get_param(BXPN_USB_XHCI);
  SIM->write_usb_options(fp, USB_XHCI_PORTS_MAX, base);
  return 0;
}

// device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(usb_xhci)
{
  if (mode == PLUGIN_INIT) {
    theUSB_XHCI = new bx_usb_xhci_c();
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theUSB_XHCI, BX_PLUGIN_USB_XHCI);
    // add new configuration parameter for the config interface
    SIM->init_usb_options("xHCI", "xhci", USB_XHCI_PORTS_MAX);
    // register add-on option for bochsrc and command line
    SIM->register_addon_option("usb_xhci", usb_xhci_options_parser, usb_xhci_options_save);
  } else if (mode == PLUGIN_FINI) {
    SIM->unregister_addon_option("usb_xhci");
    bx_list_c *menu = (bx_list_c *) SIM->get_param("ports.usb");
    delete theUSB_XHCI;
    menu->remove("xhci");
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_OPTIONAL;
  } else if (mode == PLUGIN_FLAGS) {
    return PLUGFLAG_PCI;
  }
  return 0; // Success
}

// the device object

bx_usb_xhci_c::bx_usb_xhci_c()
{
  put("usb_xhci", "XHCI");
  memset((void*)&hub, 0, sizeof(bx_usb_xhci_t));
  rt_conf_id = -1;
  xhci_timer_index = BX_NULL_TIMER_HANDLE;
}

bx_usb_xhci_c::~bx_usb_xhci_c()
{
  char pname[32];

  SIM->unregister_runtime_config_handler(rt_conf_id);

  for (int i=0; i<USB_XHCI_PORTS_MAX; i++) {
    sprintf(pname, "port%d.device", i+1);
    SIM->get_param_enum(pname, SIM->get_param(BXPN_USB_XHCI))->set_handler(NULL);
    sprintf(pname, "port%d.options", i+1);
    SIM->get_param_string(pname, SIM->get_param(BXPN_USB_XHCI))->set_enable_handler(NULL);
    sprintf(pname, "port%d.over_current", i+1);
    SIM->get_param_bool(pname, SIM->get_param(BXPN_USB_XHCI))->set_handler(NULL);
    remove_device(i);
  }

  SIM->get_bochs_root()->remove("usb_xhci");
  bx_list_c *usb_rt = (bx_list_c *) SIM->get_param(BXPN_MENU_RUNTIME_USB);
  usb_rt->remove("xhci");
  BX_DEBUG(("Exit"));
}

void bx_usb_xhci_c::init(void)
{
  unsigned i, j;
  char pname[6];
  Bit8u *p;
  bx_list_c *xhci, *port;
  bx_param_enum_c *device;
  bx_param_string_c *options;
  bx_param_bool_c *over_current;
  struct XHCI_PROTOCOL *protocol;

  /*  If you wish to set DEBUG=report in the code, instead of
   *  in the configuration, simply uncomment this line.  I use
   *  it when I am working on this emulation.
   */
  //LOG_THIS setonoff(LOGLEV_DEBUG, ACT_REPORT);

  // Read in values from config interface
  xhci = (bx_list_c *) SIM->get_param(BXPN_USB_XHCI);
  // Check if the device is disabled or not configured
  if (!SIM->get_param_bool("enabled", xhci)->get()) {
    BX_INFO(("USB xHCI disabled"));
    // mark unused plugin for removal
    ((bx_param_bool_c *)((bx_list_c *) SIM->get_param(BXPN_PLUGIN_CTRL))->get_by_name("usb_xhci"))->set(0);
    return;
  }

  BX_XHCI_THIS xhci_timer_index =
      DEV_register_timer(this, xhci_timer_handler, 1024, 1, 1, "xhci_timer");

  BX_XHCI_THIS devfunc = 0x00;
  DEV_register_pci_handlers(this, &BX_XHCI_THIS devfunc, BX_PLUGIN_USB_XHCI,
                            "Experimental USB xHCI");

  // get the desired host controller type
  BX_XHCI_THIS hub.HostController = SIM->get_param_enum(BXPN_XHCI_MODEL)->get();
  if (BX_XHCI_THIS hub.HostController == XHCI_HC_uPD720202) {
    BX_INFO(("xHCI Host Controller: uPD720202"));
    BX_XHCI_THIS hub.n_ports = USB_XHCI_PORTS; // we hard code to USB_XHCI_PORTS for this host controller (default)
    // 0x1912 = vendor (Renesas)
    // 0x0015 = device (0x0015 = uPD720202)
    // revision number (0x02 = uPD720202)
    init_pci_conf(0x1912, 0x0015, 0x02, 0x0C0330, 0x00, BX_PCI_INTD);
  } else if (BX_XHCI_THIS hub.HostController == XHCI_HC_uPD720201) {
    BX_INFO(("xHCI Host Controller: uPD720201"));
    BX_XHCI_THIS hub.n_ports = 8; // we hard code to 8 for this host controller
    // 0x1912 = vendor (Renesas)
    // 0x0014 = device (0x0014 = uPD720201)
    // revision number (0x03 = uPD720201)
    init_pci_conf(0x1912, 0x0014, 0x03, 0x0C0330, 0x00, BX_PCI_INTD);
  } else {
    BX_PANIC(("Unknown xHCI Host Controller specified..."));
    return;
  }

  // set the number of ports used
  // if n_ports is not given in the Bochsrc.txt file, the number of ports
  //  is defaulted to the controller type above
  Bit32s n_ports = SIM->get_param_num(BXPN_XHCI_N_PORTS)->get();
  if (n_ports > -1) BX_XHCI_THIS hub.n_ports = n_ports;
  if ((BX_XHCI_THIS hub.n_ports < 2) || (BX_XHCI_THIS hub.n_ports > USB_XHCI_PORTS_MAX) || (BX_XHCI_THIS hub.n_ports & 1)) {
    BX_PANIC(("n_ports (%d) must be at least 2, not more than %d, and must be an even number.", BX_XHCI_THIS hub.n_ports, USB_XHCI_PORTS_MAX));
    return;
  }

  BX_XHCI_THIS init_bar_mem(0, IO_SPACE_SIZE, read_handler, write_handler);

  // initialize capability registers
  BX_XHCI_THIS hub.cap_regs.HcCapLength  = (VERSION_MAJOR << 24) | (VERSION_MINOR << 16) | OPS_REGS_OFFSET;
  BX_XHCI_THIS hub.cap_regs.HcSParams1   = (BX_XHCI_THIS hub.n_ports << 24) | (INTERRUPTERS << 8) | MAX_SLOTS;
  BX_XHCI_THIS hub.cap_regs.HcSParams2   =
    // MAX_SCRATCH_PADS 4:0 in bits 31:27 (v1.00) and bits 9:5 in bits 21:25 (v1.01+)
    ((MAX_SCRATCH_PADS >> 5) << 21) | ((MAX_SCRATCH_PADS & 0x1F) << 27) |
    ((SCATCH_PAD_RESTORE == 1) << 26) | (MAX_SEG_TBL_SZ_EXP << 4) | ISO_SECH_THRESHOLD;
  BX_XHCI_THIS hub.cap_regs.HcSParams3   = (U2_DEVICE_EXIT_LAT << 16) | U1_DEVICE_EXIT_LAT;
  BX_XHCI_THIS hub.cap_regs.HcCParams1   =
    ((EXT_CAPS_OFFSET >> 2) << 16) | (MAX_PSA_SIZE << 12) | (SEC_DOMAIN_BAND << 9) | (PARSE_ALL_EVENT << 8) |
    (LIGHT_HC_RESET << 5) |
    (PORT_INDICATORS << 4) | (PORT_POWER_CTRL << 3) | ((CONTEXT_SIZE >> 6) << 2) |
    (BW_NEGOTIATION << 1) | ADDR_CAP_64;
  BX_XHCI_THIS hub.cap_regs.DBOFF        = DOORBELL_OFFSET;  // at offset DOORBELL_OFFSET from base
  BX_XHCI_THIS hub.cap_regs.RTSOFF       = RUNTIME_OFFSET;   // at offset RUNTIME_OFFSET from base
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_XHCI_THIS hub.cap_regs.HcCParams2   =
    (CONFIG_INFO_CAP << 5) | (LARGE_ESIT_PAYLOAD_CAP << 4) | (COMPLNC_TRANS_CAP << 3) |
    (FORCE_SAVE_CONTEXT_CAP << 2) | (CONFIG_EP_CMND_CAP << 1) | U3_ENTRY_CAP;
#endif

  // initialize runtime configuration
  bx_list_c *usb_rt = (bx_list_c *) SIM->get_param(BXPN_MENU_RUNTIME_USB);
  bx_list_c *xhci_rt = new bx_list_c(usb_rt, "xhci", "xHCI Runtime Options");
  xhci_rt->set_options(xhci_rt->SHOW_PARENT | xhci_rt->USE_BOX_TITLE);
  for (i=0; i<BX_XHCI_THIS hub.n_ports; i++) {
    sprintf(pname, "port%d", i+1);
    port = (bx_list_c *) SIM->get_param(pname, xhci);
    xhci_rt->add(port);
    device = (bx_param_enum_c*)port->get_by_name("device");
    device->set_handler(usb_param_handler);
    options = (bx_param_string_c*)port->get_by_name("options");
    options->set_enable_handler(usb_param_enable_handler);
    over_current = (bx_param_bool_c*)port->get_by_name("over_current");
    over_current->set_handler(usb_param_oc_handler);
    BX_XHCI_THIS hub.usb_port[i].device = NULL;
    BX_XHCI_THIS hub.usb_port[i].portsc.ccs = 0;
    BX_XHCI_THIS hub.usb_port[i].portsc.csc = 0;
  }

  // register handler for correct device connect handling after runtime config
  BX_XHCI_THIS rt_conf_id = SIM->register_runtime_config_handler(BX_XHCI_THIS_PTR, runtime_config_handler);
  BX_XHCI_THIS device_change = 0;
  BX_XHCI_THIS packets = NULL;

  // The remaining is the code to adjust all items that depend
  //  on the count of port registers. This needs to be done at run-time
  //  so that we can adjust the count without hard-coding anything.
  // We divide the sets evenly and place the USB3 register sets 
  //  before the USB2 register sets.
  
  // initialize the allowed speeds for each port
  // initialize the paired port number for each port
  // TODO: port_speed_allowed is currently unused
  for (i=0; i<(BX_XHCI_THIS hub.n_ports / 2); i++) {
    //BX_XHCI_THIS hub.port_speed_allowed[i] = USB3;
    BX_XHCI_THIS hub.usb_port[i].is_usb3 = 1;
    BX_XHCI_THIS hub.paired_portnum[i] = i + (BX_XHCI_THIS hub.n_ports / 2); // zero based
  }
  for (; i<BX_XHCI_THIS hub.n_ports; i++) {
    //BX_XHCI_THIS hub.port_speed_allowed[i] = USB2;
    BX_XHCI_THIS hub.usb_port[i].is_usb3 = 0;
    BX_XHCI_THIS hub.paired_portnum[i] = i - (BX_XHCI_THIS hub.n_ports / 2); // zero based
  }

  // Bandwidth of each port of each speed.
  // See Section 6.2.6 of xHCI version 1.00
  // example: four port (two socket):
  //                  rsvd, port1, port2, port3, port4  (padding to 8 bytes)
  //  Full Speed:   { 0x00,     0,     0,    90,    90,  0, 0, 0 },
  //   Low Speed:   { 0x00,     0,     0,    90,    90,  0, 0, 0 },
  //  High Speed:   { 0x00,     0,     0,    80,    80,  0, 0, 0 },
  // Super Speed:   { 0x00,    90,    90,     0,     0,  0, 0, 0 }
  // (These numbers are for a two socket, four port hc.)
  // (They will need to be different for a different combination)
  p = BX_XHCI_THIS hub.port_band_width;
  for (i=0; i<4; i++) { // 4 speeds
    *p++ = 0; // first entry is reserved
    for (j=0; j<BX_XHCI_THIS hub.n_ports; j++) {
      switch (i) {
        case 0: // Full Speed
        case 1: // Low Speed
          *p++ = (j < (BX_XHCI_THIS hub.n_ports / 2)) ? 0 : 90;
          break;
        case 2: // High Speed
          *p++ = (j < (BX_XHCI_THIS hub.n_ports / 2)) ? 0 : 80;
          break;
        case 3: // Super Speed
          *p++ = (j < (BX_XHCI_THIS hub.n_ports / 2)) ? 90 : 0;
          break;
      }
    }
    // pad to an 8-byte boundary
    while ((j & 7) < 7) {
      *p++ = 0;
      j++;
    }
  }

  // adjust the Extended Caps Protocol fields
  // first the USB3 (first half ports)
  protocol = (struct XHCI_PROTOCOL *) &ext_caps[PROTOCOL_UBS3_OFFSET];
  protocol->start_index = 1; // 1 based starting index
  protocol->count = BX_XHCI_THIS hub.n_ports / 2;
  // then the USB2 (second half ports)
  protocol = (struct XHCI_PROTOCOL *) &ext_caps[PROTOCOL_UBS2_OFFSET];
  protocol->start_index = (BX_XHCI_THIS hub.n_ports / 2) + 1; // 1 based starting index
  protocol->count = BX_XHCI_THIS hub.n_ports / 2;

  // done initializing
  BX_INFO(("USB xHCI initialized"));
}

void bx_usb_xhci_c::reset(unsigned type)
{
  unsigned i;

  if (type == BX_RESET_HARDWARE) {
    static const struct reset_vals_t {
      unsigned      addr;
      unsigned char val;
    } reset_vals[] = {
      { 0x04, 0x06 }, { 0x05, 0x01 }, // command_io
      { 0x06, 0x10 }, { 0x07, 0x00 }, // status (has caps list)
      { 0x0C, 0x10 },                 // cache line size
      { 0x0D, 0x00 },                 // bus latency
      { 0x0F, 0x00 },                 // BIST is not supported

      // address space 0x10 - 0x17
      { 0x10, 0x04 }, { 0x11, 0x00 }, // 64-bit wide and anywhere in the 64-bit address space
      { 0x12, 0x50 }, { 0x13, 0xF0 }, //
      { 0x14, 0x00 }, { 0x15, 0x00 }, //
      { 0x16, 0x00 }, { 0x17, 0x00 }, //

      // addresses 0x18 - 0x28 are reserved

      { 0x2C, 0xFF }, { 0x2D, 0xFF }, // subsystem vendor ID
      { 0x2E, 0xFF }, { 0x2F, 0xFF }, // subsystem ID

      { 0x34, 0x50 },                 // offset of capabilities list within configuration space

      { 0x3C, 0x0A },                 // IRQ
      { 0x3E, 0x00 },                 // minimum time bus master needs PCI bus ownership, in 250ns units
      { 0x3F, 0x00 },                 // maximum latency, in 250ns units (bus masters only) (read-only)

      // capabilities list:
      { 0x50, 0x01 },                 // PCI Power Management

//      { 0x51, 0x70 },                 //  Pointer to next item (0x70 -> MSI stuff)
      { 0x51, 0x00 },                 //  Pointer to next item (0x00 = no more)

      { 0x52, 0xC3 }, { 0x53, 0xC9 }, //  Capabilities:  version = 1.2, Aux Current = 375mA,
      { 0x54, 0x08 }, { 0x55, 0x00 }, //        Status:  Power State = D0, Bit 3 = no soft reset
      { 0x56, 0x00 }, { 0x57, 0x00 }, //

      { 0x60, 0x30 },                 // Supports USB Spec version 3.0
      { 0x61, 0x20 },                 // Frame List Adjustment

      // Firmware version
      { 0x6C, 0x09 },                 // ????? This byte not documented in specification ??
      { 0x6D, 0x18 }, { 0x6E, 0x20 }, // low = 0x18, high = 0x20
      { 0x6F, 0x00 },                 //

      /* The following items are used with PCIe and are not wanted or needed within
       * this emulation.  However, the controller contains them within its PCI(e)
       * configuration space.  Therefore, I leave them here (though commented out)
       * for that sake.  This is for the benefit of the reader, along as myself.
       * If you uncomment this section, remember to swap the commented lines above.
       *  { 0x51, 0x70 },  and  { 0x51, 0x00 },

      // MSI
      { 0x70, 0x05 },                 // MSI
      { 0x71, 0x90 },                 //  Pointer to next item (0x00 = no more)
      { 0x72, 0x86 }, { 0x73, 0x00 }, //
      { 0x74, 0x00 }, { 0x75, 0x00 }, //  0x74 - 0x87 are zero's

      // MSI-x
      { 0x90, 0x11 },                 // MSI-x
      { 0x91, 0xA0 },                 //  Pointer to next item (0x00 = no more)
      { 0x92, 0x07 }, { 0x93, 0x00 }, //
      { 0x94, 0x00 }, { 0x95, 0x10 }, //
      { 0x96, 0x00 }, { 0x97, 0x00 }, //
      { 0x98, 0x80 }, { 0x99, 0x10 }, //
      { 0x9A, 0x00 }, { 0x9B, 0x00 }, //

      // PCIe caps register1
      { 0xA0, 0x10 },                 // PCIe caps register
      { 0xA1, 0x00 },                 //  Pointer to next item (0x00 = no more)
      { 0xA2, 0x02 }, { 0xA3, 0x00 }, //
      { 0xA4, 0xC0 }, { 0xA5, 0x8F }, //
      { 0xA6, 0x00 }, { 0xA7, 0x00 }, //
      { 0xA8, 0x10 }, { 0xA9, 0x28 }, //
      { 0xAA, 0x10 }, { 0xAB, 0x00 }, //
      { 0xAC, 0x12 }, { 0xAD, 0xEC }, //
      { 0xAE, 0x07 }, { 0xAF, 0x00 }, //
      { 0xB0, 0x40 }, { 0xB1, 0x00 }, //
      { 0xB2, 0x11 }, { 0xB3, 0x10 }, //

      // PCIe caps register2
      { 0xC4, 0x10 }, { 0xC5, 0x08 }, //
      { 0xC6, 0x00 }, { 0xC7, 0x00 }, //
      { 0xC8, 0x00 }, { 0xC9, 0x00 }, //
      { 0xCA, 0x00 }, { 0xCB, 0x00 }, //
      { 0xCC, 0x00 }, { 0xCD, 0x00 }, //
      { 0xCE, 0x00 }, { 0xCF, 0x00 }, //
      { 0xD0, 0x00 }, { 0xD1, 0x00 }, //
      { 0xD2, 0x01 }, { 0xD3, 0x10 }, //

      // Phy 0
      { 0xDC, 0x00 }, { 0xDD, 0x00 }, //
      { 0xDE, 0x00 }, { 0xDF, 0x00 }, //

      // Phy 1
      { 0xE0, 0x00 }, { 0xE1, 0x00 }, //
      { 0xE2, 0x00 }, { 0xE3, 0x00 }, //

      // Phy 2 (battery charging control)
      { 0xE4, 0x00 }, { 0xE5, 0x00 }, //
      { 0xE6, 0x03 }, { 0xE7, 0x00 }, //

      // HCConfiguration Register
      { 0xE8, 0x00 }, { 0xE9, 0x00 }, //
      { 0xEA, 0x01 }, { 0xEB, 0x05 }, //

      // External ROM Information Register
      { 0xEC, 0x7F }, { 0xED, 0x20 }, //
      { 0xEE, 0x9D }, { 0xEF, 0x01 }, //

      // External ROM Information Register
      { 0xF0, 0x00 }, { 0xF1, 0x00 }, //
      { 0xF2, 0x00 }, { 0xF3, 0x00 }, //

      // Firmware Download Control and Status
      { 0xF4, 0x00 }, { 0xF5, 0x00 }, //
      { 0xF6, 0x00 }, { 0xF7, 0x80 }, //

      // Data 0
      { 0xF8, 0x00 }, { 0xF9, 0x00 }, //
      { 0xFA, 0x00 }, { 0xFB, 0x00 }, //

      // Data 1
      { 0xFC, 0x00 }, { 0xFD, 0x00 }, //
      { 0xFE, 0x00 }, { 0xFF, 0x00 }, //

      * Offsets 0x100 -> 0x157 are used by controller,
      *  but Bochs only supports up to 256 (PCI)
      */
    };

    for (i = 0; i < sizeof(reset_vals) / sizeof(*reset_vals); i++) {
        BX_XHCI_THIS pci_conf[reset_vals[i].addr] = reset_vals[i].val;
    }
  }

  BX_XHCI_THIS reset_hc();
}

void bx_usb_xhci_c::reset_hc()
{
  unsigned int i;
  char pname[6];

  // Command
  BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1 = 0;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_XHCI_THIS hub.op_regs.HcCommand.cme    = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.spe   = 0;
#endif
  BX_XHCI_THIS hub.op_regs.HcCommand.eu3s   = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.ewe    = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.crs    = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.css    = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.lhcrst = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP0 = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.hsee   = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.inte   = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.hcrst  = 0;
  BX_XHCI_THIS hub.op_regs.HcCommand.rs     = 0;

  // Status
  BX_XHCI_THIS hub.op_regs.HcStatus.RsvdZ1  = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.hce     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.cnr     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.sre     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.rss     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.sss     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.RsvdZ0  = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.pcd     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.eint    = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.hse     = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.RsvdZ2  = 0;
  BX_XHCI_THIS hub.op_regs.HcStatus.hch     = 1;

  // Page Size
  BX_XHCI_THIS hub.op_regs.HcPageSize.pagesize = XHCI_PAGE_SIZE;

  // Device Notification Control Register
  BX_XHCI_THIS hub.op_regs.HcNotification.RsvdP = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n15   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n14   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n13   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n12   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n11   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n10   = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n9    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n8    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n7    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n6    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n5    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n4    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n3    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n2    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n1    = 0;
  BX_XHCI_THIS hub.op_regs.HcNotification.n0    = 0;

  // Command Ring Control Register
  BX_XHCI_THIS hub.op_regs.HcCrcr.crc     = 0;
  BX_XHCI_THIS hub.op_regs.HcCrcr.RsvdP   = 0;
  BX_XHCI_THIS hub.op_regs.HcCrcr.crr     = 0;
  BX_XHCI_THIS hub.op_regs.HcCrcr.ca      = 0;
  BX_XHCI_THIS hub.op_regs.HcCrcr.cs      = 0;
  BX_XHCI_THIS hub.op_regs.HcCrcr.rcs     = 0;

  // DCBAAP
  BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap = 0;

  // Config
  BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP      = 0;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_XHCI_THIS hub.op_regs.HcConfig.u3e = 0;
  BX_XHCI_THIS hub.op_regs.HcConfig.cie = 0;
#endif
  BX_XHCI_THIS hub.op_regs.HcConfig.MaxSlotsEn = 0;

  // Ports[x]
  for (i=0; i<BX_XHCI_THIS hub.n_ports; i++) {
    reset_port(i);
    if (BX_XHCI_THIS hub.usb_port[i].device == NULL) {
      sprintf(pname, "port%d", i+1);
      init_device(i, (bx_list_c *) SIM->get_param(pname, SIM->get_param(BXPN_USB_XHCI)));
    } else {
      set_connect_status(i, 1);
    }
  }

  // Extended Caps
  for (i=0; i<EXT_CAPS_SIZE; i++)
    BX_XHCI_THIS hub.extended_caps[i] = ext_caps[i];

  // interrupters[x]
  BX_XHCI_THIS hub.runtime_regs.mfindex.RsvdP = 0;
  BX_XHCI_THIS hub.runtime_regs.mfindex.index = 0;
  for (i=0; i<INTERRUPTERS; i++) {
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.RsvdP = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ie = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ip = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodc = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodi = 4000;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.RsvdP = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.erstabsize = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].RsvdP = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.RsvdP = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.ehb = 0;
    BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.desi = 0;
  }

  // reset our slot contexts
  for (i=0; i<MAX_SLOTS; i++)
    memset(&BX_XHCI_THIS hub.slots[i], 0, sizeof(struct HC_SLOT_CONTEXT));

  while (BX_XHCI_THIS packets != NULL) {
    usb_cancel_packet(&BX_XHCI_THIS packets->packet);
    remove_async_packet(&BX_XHCI_THIS packets, BX_XHCI_THIS packets);
  }
}

void bx_usb_xhci_c::reset_port(int p)
{
  BX_XHCI_THIS hub.usb_port[p].portsc.wpr = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.dr  = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.woe = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.wde = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.wce = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.cas = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.cec = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.plc = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.prc = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.occ = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.wrc = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.pec = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.csc = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.lws = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.pic = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.speed = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.pp  = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.pls = PLS_U0;  // always ready to go (for our sake anyway)
  BX_XHCI_THIS hub.usb_port[p].portsc.pr  = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.oca = 0;
  BX_XHCI_THIS hub.usb_port[p].portsc.ped = 0;

  if (BX_XHCI_THIS hub.usb_port[p].is_usb3) {
    BX_XHCI_THIS hub.usb_port[p].usb3.portpmsc.RsvdP = 0;
    BX_XHCI_THIS hub.usb_port[p].usb3.portpmsc.fla = 0;
    BX_XHCI_THIS hub.usb_port[p].usb3.portpmsc.u2timeout = 0;
    BX_XHCI_THIS hub.usb_port[p].usb3.portpmsc.u1timeout = 0;
    BX_XHCI_THIS hub.usb_port[p].usb3.portli.lec = 0;
  } else {
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.tmode = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.RsvdP = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.hle = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.l1dslot = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.hird = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.rwe = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portpmsc.l1s = 0;
    BX_XHCI_THIS hub.usb_port[p].usb2.portli.RsvdP = 0;
  }

  BX_XHCI_THIS hub.usb_port[p].has_been_reset = 0;
  BX_XHCI_THIS hub.usb_port[p].psceg = 0;
}

void bx_usb_xhci_c::reset_port_usb3(int port, const int reset_type)
{
  BX_INFO(("Reset port #%d, type=%d", port + 1, reset_type));
  BX_XHCI_THIS hub.usb_port[port].portsc.pr = 0;
  BX_XHCI_THIS hub.usb_port[port].has_been_reset = 1;
  if (BX_XHCI_THIS hub.usb_port[port].portsc.ccs) {
    BX_XHCI_THIS hub.usb_port[port].portsc.prc = 1;
    BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_U0;
    BX_XHCI_THIS hub.usb_port[port].portsc.ped = 1;
    if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
      BX_XHCI_THIS hub.usb_port[port].device->usb_send_msg(USB_MSG_RESET);
      if (BX_XHCI_THIS hub.usb_port[port].is_usb3 && (reset_type == WARM_RESET))
        BX_XHCI_THIS hub.usb_port[port].portsc.wrc = 1;
    }
  } else {
    BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_RXDETECT;
    BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
    BX_XHCI_THIS hub.usb_port[port].portsc.speed = 0;
  }
}

/* This is the Save/Restore part of the controller.  The host will issue a save state
 *  (via a bit in the command register) before it powers down the controller.  The controller
 *  will use the scratch pad buffers (or its own memory if HCSPARAMS2:ScratchPadRestore == 0)
 *  to save its internal state before power shutdown (hibernation).  If the SPR bit is set,
 *  the controller is to use the scratch pad buffers and the host is to make sure these
 *  buffers are maintained during hibernation. (The host may use paging to page to disk
 *  before hibernation as long as the memory is updated from the paged to disk before
 *  the restore is performed.)
 *
 * Since a Bochs power hibernation does not touch our current state, there would be no
 *  need to save the controller state.  However, the host may (just might) check to see
 *  if the scratch pad buffers have been modified (due to paging and/or protection), so
 *  we need to write something to these buffers.
 *
 * We will simply save 'BX_XHCI_THIS hub' to the scratch pad buffers.  However, since we
 *  use bx_bool's for bits, which are 32-bits for every single bit of the controller,
 *  'hub' is way too big to fit in the scratch pad buffers.  Therefore, we will only
 *  save up to PAGESIZE * BUFFER_COUNT.
 *
 * We will also use a simple crc check and store this as the last dword in each buffer.
 *  This way, when we restore, we can check to see if we restored the correct data and
 *  set the restore error bit accordingly.
 *
 * Please note that we must not assume that the scratch pad buffers will be in
 *  consecutive order, one right after the other, in memory.
 *
 * Callee checks to be sure the Halted bit is set first.
 * We return 0 == no error, 1 = error
 */
bool bx_usb_xhci_c::save_hc_state(void)
{
  int i, j;
  Bit64u addr;
  Bit32u *ptr = (Bit32u *) &BX_XHCI_THIS hub;
  Bit64u temp[2 | MAX_SCRATCH_PADS];  // at least the larger of 2 or MAX_SCRATCH_PADS
  Bit32u crc;

  // do we use the scratch pad buffers to save the controller state?
#if (SCATCH_PAD_RESTORE == 1)
  // get pointer to scratch pad buffers from DCBAAP
  DEV_MEM_READ_PHYSICAL((bx_phy_address) BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap, 8, (Bit8u*)&addr);

  // get MAX_SCRATCH_PADS worth of pointers
  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    DEV_MEM_READ_PHYSICAL((bx_phy_address) (addr + i * 8), 8, (Bit8u*)&temp[i]);
  }

  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    crc = 0;
    for (j=0; j<((XHCI_PAGE_SIZE << 12) >> 2) - 1; j++)
      crc += ptr[j];
    addr = temp[i];
    DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address)  addr, (XHCI_PAGE_SIZE << 12) - sizeof(Bit32u), (Bit8u*)ptr);
    DEV_MEM_WRITE_PHYSICAL((bx_phy_address) (addr + ((XHCI_PAGE_SIZE << 12) - sizeof(Bit32u))), sizeof(Bit32u), (Bit8u *) &crc);
    ptr += (((XHCI_PAGE_SIZE << 12) >> 2) - 1);
  }
#else
  // Use controller's internal memory to save the state
  // Since saving to the internal memory will have no effect on the host,
  //  since we don't destroy BX_XHCI_THIS hub on hibernation, we don't need
  //  to do anything here.
#endif

  // return no error
  return 0;
}

/*
 * See comments at save_hc_state above
 */
bool bx_usb_xhci_c::restore_hc_state(void)
{
  int i, j;
  Bit64u addr;
  Bit64u temp[2 | MAX_SCRATCH_PADS];  // at least the larger of 2 or MAX_SCRATCH_PADS
  Bit32u temp_buffer[(XHCI_PAGE_SIZE << 12) >> 2];
  Bit32u crc;

  // if we are to use the scratch pad buffers to restore the controller state
#if (SCATCH_PAD_RESTORE == 1)
  // get pointer to scratch pad buffers from DCBAAP
  DEV_MEM_READ_PHYSICAL((bx_phy_address) BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap, 8, (Bit8u*)&addr);

  // get MAX_SCRATCH_PADS worth of pointers
  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) (addr + i * 8), 8, (Bit8u*)&temp[i]);
  }

  // we read it in to a temp buffer just to check the crc.
  for (i=0; i<MAX_SCRATCH_PADS; i++) {
    addr = temp[i];
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr, (XHCI_PAGE_SIZE << 12), (Bit8u*)temp_buffer);
    crc = 0;
    for (j=0; j<((XHCI_PAGE_SIZE << 12) >> 2) - 1; j++)
      crc += temp_buffer[j];
    if (crc != temp_buffer[((XHCI_PAGE_SIZE << 12) >> 2) - 1])
      return 1;  // error
  }
#else
  // Use controller's internal memory to save the state
  // Since saving to the internal memory will have no effect on the host,
  //  since we don't destroy BX_XHCI_THIS hub on hibernation, we don't need
  //  to do anything here.
#endif

  // return no error
  return 0;
}

void bx_usb_xhci_c::register_state(void)
{
  unsigned i, j, k;
  char tmpname[16];
  bx_list_c *hub, *port, *reg, *reg_grp, *reg_grp1;
  bx_list_c *list1, *list2, *list3, *item, *entries, *entry, *entry1;

  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "usb_xhci", "USB xHCI State");
  hub = new bx_list_c(list, "hub");
  reg_grp = new bx_list_c(hub, "op_regs");
  reg = new bx_list_c(reg_grp, "HcCommand");
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BXRS_PARAM_BOOL(reg, cme, BX_XHCI_THIS hub.op_regs.HcCommand.cme);
  BXRS_PARAM_BOOL(reg, spe, BX_XHCI_THIS hub.op_regs.HcCommand.spe);
#endif
  BXRS_PARAM_BOOL(reg, eu3s, BX_XHCI_THIS hub.op_regs.HcCommand.eu3s);
  BXRS_PARAM_BOOL(reg, ewe, BX_XHCI_THIS hub.op_regs.HcCommand.ewe);
  BXRS_PARAM_BOOL(reg, crs, BX_XHCI_THIS hub.op_regs.HcCommand.crs);
  BXRS_PARAM_BOOL(reg, css, BX_XHCI_THIS hub.op_regs.HcCommand.css);
  BXRS_PARAM_BOOL(reg, lhcrst, BX_XHCI_THIS hub.op_regs.HcCommand.lhcrst);
  BXRS_PARAM_BOOL(reg, hsee, BX_XHCI_THIS hub.op_regs.HcCommand.hsee);
  BXRS_PARAM_BOOL(reg, inte, BX_XHCI_THIS hub.op_regs.HcCommand.inte);
  BXRS_PARAM_BOOL(reg, hcrst, BX_XHCI_THIS hub.op_regs.HcCommand.hcrst);
  BXRS_PARAM_BOOL(reg, rs, BX_XHCI_THIS hub.op_regs.HcCommand.rs);
  reg = new bx_list_c(reg_grp, "HcStatus");
  BXRS_PARAM_BOOL(reg, hce, BX_XHCI_THIS hub.op_regs.HcStatus.hce);
  BXRS_PARAM_BOOL(reg, cnr, BX_XHCI_THIS hub.op_regs.HcStatus.cnr);
  BXRS_PARAM_BOOL(reg, sre, BX_XHCI_THIS hub.op_regs.HcStatus.sre);
  BXRS_PARAM_BOOL(reg, rss, BX_XHCI_THIS hub.op_regs.HcStatus.rss);
  BXRS_PARAM_BOOL(reg, sss, BX_XHCI_THIS hub.op_regs.HcStatus.sss);
  BXRS_PARAM_BOOL(reg, pcd, BX_XHCI_THIS hub.op_regs.HcStatus.pcd);
  BXRS_PARAM_BOOL(reg, eint, BX_XHCI_THIS hub.op_regs.HcStatus.eint);
  BXRS_PARAM_BOOL(reg, hse, BX_XHCI_THIS hub.op_regs.HcStatus.hse);
  BXRS_PARAM_BOOL(reg, hch, BX_XHCI_THIS hub.op_regs.HcStatus.hch);
  BXRS_HEX_PARAM_FIELD(reg_grp, HcPageSize, BX_XHCI_THIS hub.op_regs.HcPageSize.pagesize);
  reg = new bx_list_c(reg_grp, "HcNotification");
  BXRS_PARAM_BOOL(reg, n15, BX_XHCI_THIS hub.op_regs.HcNotification.n15);
  BXRS_PARAM_BOOL(reg, n14, BX_XHCI_THIS hub.op_regs.HcNotification.n14);
  BXRS_PARAM_BOOL(reg, n13, BX_XHCI_THIS hub.op_regs.HcNotification.n13);
  BXRS_PARAM_BOOL(reg, n12, BX_XHCI_THIS hub.op_regs.HcNotification.n12);
  BXRS_PARAM_BOOL(reg, n11, BX_XHCI_THIS hub.op_regs.HcNotification.n11);
  BXRS_PARAM_BOOL(reg, n10, BX_XHCI_THIS hub.op_regs.HcNotification.n10);
  BXRS_PARAM_BOOL(reg, n9, BX_XHCI_THIS hub.op_regs.HcNotification.n9);
  BXRS_PARAM_BOOL(reg, n8, BX_XHCI_THIS hub.op_regs.HcNotification.n8);
  BXRS_PARAM_BOOL(reg, n7, BX_XHCI_THIS hub.op_regs.HcNotification.n7);
  BXRS_PARAM_BOOL(reg, n6, BX_XHCI_THIS hub.op_regs.HcNotification.n6);
  BXRS_PARAM_BOOL(reg, n5, BX_XHCI_THIS hub.op_regs.HcNotification.n5);
  BXRS_PARAM_BOOL(reg, n4, BX_XHCI_THIS hub.op_regs.HcNotification.n4);
  BXRS_PARAM_BOOL(reg, n3, BX_XHCI_THIS hub.op_regs.HcNotification.n3);
  BXRS_PARAM_BOOL(reg, n2, BX_XHCI_THIS hub.op_regs.HcNotification.n2);
  BXRS_PARAM_BOOL(reg, n1, BX_XHCI_THIS hub.op_regs.HcNotification.n1);
  BXRS_PARAM_BOOL(reg, n0, BX_XHCI_THIS hub.op_regs.HcNotification.n0);
  reg = new bx_list_c(reg_grp, "HcCrcr");
  BXRS_HEX_PARAM_FIELD(reg, crc, BX_XHCI_THIS hub.op_regs.HcCrcr.crc);
  BXRS_PARAM_BOOL(reg, crr, BX_XHCI_THIS hub.op_regs.HcCrcr.crr);
  BXRS_PARAM_BOOL(reg, ca, BX_XHCI_THIS hub.op_regs.HcCrcr.ca);
  BXRS_PARAM_BOOL(reg, cs, BX_XHCI_THIS hub.op_regs.HcCrcr.cs);
  BXRS_PARAM_BOOL(reg, rcs, BX_XHCI_THIS hub.op_regs.HcCrcr.rcs);
  BXRS_HEX_PARAM_FIELD(reg_grp, HcDCBAAP, BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap);
  BXRS_HEX_PARAM_FIELD(reg_grp, HcConfig_MaxSlotsEn, BX_XHCI_THIS hub.op_regs.HcConfig.MaxSlotsEn);
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BXRS_PARAM_BOOL(reg_grp, HcConfig_u3e, BX_XHCI_THIS hub.op_regs.HcConfig.u3e);
  BXRS_PARAM_BOOL(reg_grp, HcConfig_cie, BX_XHCI_THIS hub.op_regs.HcConfig.cie);
#endif

  for (i = 0; i < BX_XHCI_THIS hub.n_ports; i++) {
    sprintf(tmpname, "port%d", i+1);
    port = new bx_list_c(hub, tmpname);
    BXRS_PARAM_BOOL(port, has_been_reset, BX_XHCI_THIS hub.usb_port[i].has_been_reset);
    BXRS_HEX_PARAM_FIELD(port, psceg, BX_XHCI_THIS hub.usb_port[i].psceg);
    reg = new bx_list_c(port, "portsc");
    BXRS_PARAM_BOOL(reg, wpr, BX_XHCI_THIS hub.usb_port[i].portsc.wpr);
    BXRS_PARAM_BOOL(reg, dr, BX_XHCI_THIS hub.usb_port[i].portsc.dr);
    BXRS_PARAM_BOOL(reg, woe, BX_XHCI_THIS hub.usb_port[i].portsc.woe);
    BXRS_PARAM_BOOL(reg, wde, BX_XHCI_THIS hub.usb_port[i].portsc.wde);
    BXRS_PARAM_BOOL(reg, wce, BX_XHCI_THIS hub.usb_port[i].portsc.wce);
    BXRS_PARAM_BOOL(reg, cas, BX_XHCI_THIS hub.usb_port[i].portsc.cas);
    BXRS_PARAM_BOOL(reg, cec, BX_XHCI_THIS hub.usb_port[i].portsc.cec);
    BXRS_PARAM_BOOL(reg, plc, BX_XHCI_THIS hub.usb_port[i].portsc.plc);
    BXRS_PARAM_BOOL(reg, prc, BX_XHCI_THIS hub.usb_port[i].portsc.prc);
    BXRS_PARAM_BOOL(reg, occ, BX_XHCI_THIS hub.usb_port[i].portsc.occ);
    BXRS_PARAM_BOOL(reg, wrc, BX_XHCI_THIS hub.usb_port[i].portsc.wrc);
    BXRS_PARAM_BOOL(reg, pec, BX_XHCI_THIS hub.usb_port[i].portsc.pec);
    BXRS_PARAM_BOOL(reg, csc, BX_XHCI_THIS hub.usb_port[i].portsc.csc);
    BXRS_PARAM_BOOL(reg, lws, BX_XHCI_THIS hub.usb_port[i].portsc.lws);
    BXRS_HEX_PARAM_FIELD(reg, pic, BX_XHCI_THIS hub.usb_port[i].portsc.pic);
    BXRS_DEC_PARAM_FIELD(reg, speed, BX_XHCI_THIS hub.usb_port[i].portsc.speed);
    BXRS_PARAM_BOOL(reg, pp, BX_XHCI_THIS hub.usb_port[i].portsc.pp);
    BXRS_HEX_PARAM_FIELD(reg, pls, BX_XHCI_THIS hub.usb_port[i].portsc.pls);
    BXRS_PARAM_BOOL(reg, pr, BX_XHCI_THIS hub.usb_port[i].portsc.pr);
    BXRS_PARAM_BOOL(reg, oca, BX_XHCI_THIS hub.usb_port[i].portsc.oca);
    BXRS_PARAM_BOOL(reg, ped, BX_XHCI_THIS hub.usb_port[i].portsc.ped);
    BXRS_PARAM_BOOL(reg, ccs, BX_XHCI_THIS hub.usb_port[i].portsc.ccs);

    reg = new bx_list_c(port, "portpmsc");
    if (BX_XHCI_THIS hub.usb_port[i].is_usb3) {
      BXRS_PARAM_BOOL(reg, fla, BX_XHCI_THIS hub.usb_port[i].usb3.portpmsc.fla);
      BXRS_HEX_PARAM_FIELD(reg, u2timeout, BX_XHCI_THIS hub.usb_port[i].usb3.portpmsc.u2timeout);
      BXRS_HEX_PARAM_FIELD(reg, u1timeout, BX_XHCI_THIS hub.usb_port[i].usb3.portpmsc.u1timeout);
      BXRS_HEX_PARAM_FIELD(port, portli_lec, BX_XHCI_THIS hub.usb_port[i].usb3.portli.lec);
    } else {
      BXRS_HEX_PARAM_FIELD(reg, tmode, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.tmode);
      BXRS_PARAM_BOOL(reg, hle, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.hle);
      BXRS_HEX_PARAM_FIELD(reg, l1dslot, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.l1dslot);
      BXRS_HEX_PARAM_FIELD(reg, hird, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.hird);
      BXRS_PARAM_BOOL(reg, rwe, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.rwe);
      BXRS_HEX_PARAM_FIELD(reg, l1s, BX_XHCI_THIS hub.usb_port[i].usb2.portpmsc.l1s);
    }

    reg = new bx_list_c(port, "porthlpmc");
    BXRS_HEX_PARAM_FIELD(reg, hirdm, BX_XHCI_THIS hub.usb_port[i].porthlpmc.hirdm);
    BXRS_HEX_PARAM_FIELD(reg, l1timeout, BX_XHCI_THIS hub.usb_port[i].porthlpmc.l1timeout);
    BXRS_HEX_PARAM_FIELD(reg, hirdd, BX_XHCI_THIS hub.usb_port[i].porthlpmc.hirdd);
    // empty list for USB device state
    new bx_list_c(port, "device");
  }

  new bx_shadow_data_c(hub, "extended_caps", BX_XHCI_THIS hub.extended_caps, EXT_CAPS_SIZE, 1);

  reg_grp = new bx_list_c(hub, "runtime_regs");
  BXRS_HEX_PARAM_FIELD(reg_grp, mfindex, BX_XHCI_THIS hub.runtime_regs.mfindex.index);
  for (i = 0; i < INTERRUPTERS; i++) {
    sprintf(tmpname, "interrupter%d", i+1);
    reg_grp1 = new bx_list_c(reg_grp, tmpname);
    reg = new bx_list_c(reg_grp1, "iman");
    BXRS_PARAM_BOOL(reg, ie, BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ie);
    BXRS_PARAM_BOOL(reg, ip, BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ip);
    reg = new bx_list_c(reg_grp1, "imod");
    BXRS_HEX_PARAM_FIELD(reg, imodc, BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodc);
    BXRS_HEX_PARAM_FIELD(reg, imodi, BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodi);
    BXRS_HEX_PARAM_FIELD(reg_grp1, erstabsize, BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.erstabsize);
    BXRS_HEX_PARAM_FIELD(reg_grp1, erstabadd, BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd);
    reg = new bx_list_c(reg_grp1, "erdp");
    BXRS_HEX_PARAM_FIELD(reg, eventadd, BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd);
    BXRS_PARAM_BOOL(reg, ehb, BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.ehb);
    BXRS_HEX_PARAM_FIELD(reg, desi, BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.desi);
  }

  list1 = new bx_list_c(hub, "slots");
  for (i = 1; i < MAX_SLOTS; i++) {
    sprintf(tmpname, "slot%d", i);
    item = new bx_list_c(list1, tmpname);
    BXRS_PARAM_BOOL(item, enabled, BX_XHCI_THIS hub.slots[i].enabled);
    list2 = new bx_list_c(item, "slot_context");
    BXRS_DEC_PARAM_FIELD(list2, entries, BX_XHCI_THIS hub.slots[i].slot_context.entries);
    BXRS_PARAM_BOOL(list2, hub, BX_XHCI_THIS hub.slots[i].slot_context.hub);
    BXRS_PARAM_BOOL(list2, mtt, BX_XHCI_THIS hub.slots[i].slot_context.mtt);
    BXRS_DEC_PARAM_FIELD(list2, speed, BX_XHCI_THIS hub.slots[i].slot_context.speed);
    BXRS_DEC_PARAM_FIELD(list2, route_string, BX_XHCI_THIS hub.slots[i].slot_context.route_string);
    BXRS_DEC_PARAM_FIELD(list2, num_ports, BX_XHCI_THIS hub.slots[i].slot_context.num_ports);
    BXRS_DEC_PARAM_FIELD(list2, rh_port_num, BX_XHCI_THIS hub.slots[i].slot_context.rh_port_num);
    BXRS_DEC_PARAM_FIELD(list2, max_exit_latency, BX_XHCI_THIS hub.slots[i].slot_context.max_exit_latency);
    BXRS_DEC_PARAM_FIELD(list2, int_target, BX_XHCI_THIS hub.slots[i].slot_context.int_target);
    BXRS_DEC_PARAM_FIELD(list2, ttt, BX_XHCI_THIS hub.slots[i].slot_context.ttt);
    BXRS_DEC_PARAM_FIELD(list2, tt_port_num, BX_XHCI_THIS hub.slots[i].slot_context.tt_port_num);
    BXRS_DEC_PARAM_FIELD(list2, tt_hub_slot_id, BX_XHCI_THIS hub.slots[i].slot_context.tt_hub_slot_id);
    BXRS_DEC_PARAM_FIELD(list2, slot_state, BX_XHCI_THIS hub.slots[i].slot_context.slot_state);
    BXRS_DEC_PARAM_FIELD(list2, device_address, BX_XHCI_THIS hub.slots[i].slot_context.device_address);
    entries = new bx_list_c(item, "ep_context");
    for (j = 0; j < 32; j++) {
      sprintf(tmpname, "%d", j);
      entry = new bx_list_c(entries, tmpname);
      list2 = new bx_list_c(entry, "ep_context");
      BXRS_DEC_PARAM_FIELD(list2, interval, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.interval);
      BXRS_PARAM_BOOL(list2, lsa, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.lsa);
      BXRS_DEC_PARAM_FIELD(list2, max_pstreams, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.max_pstreams);
      BXRS_DEC_PARAM_FIELD(list2, mult, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.mult);
      BXRS_DEC_PARAM_FIELD(list2, ep_state, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.ep_state);
      BXRS_DEC_PARAM_FIELD(list2, max_packet_size, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.max_packet_size);
      BXRS_DEC_PARAM_FIELD(list2, max_burst_size, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.max_burst_size);
      BXRS_PARAM_BOOL(list2, hid, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.hid);
      BXRS_DEC_PARAM_FIELD(list2, ep_type, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.ep_type);
      BXRS_DEC_PARAM_FIELD(list2, cerr, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.cerr);
      BXRS_HEX_PARAM_FIELD(list2, tr_dequeue_pointer, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.tr_dequeue_pointer);
      BXRS_PARAM_BOOL(list2, dcs, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.dcs);
      BXRS_DEC_PARAM_FIELD(list2, max_esit_payload, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.max_esit_payload);
      BXRS_DEC_PARAM_FIELD(list2, average_trb_len, BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.average_trb_len);
      BXRS_HEX_PARAM_FIELD(entry, edtla, BX_XHCI_THIS hub.slots[i].ep_context[j].edtla);
      BXRS_HEX_PARAM_FIELD(entry, enqueue_pointer, BX_XHCI_THIS hub.slots[i].ep_context[j].enqueue_pointer);
      BXRS_PARAM_BOOL(entry, rcs, BX_XHCI_THIS hub.slots[i].ep_context[j].rcs);
      BXRS_PARAM_BOOL(entry, retry, BX_XHCI_THIS hub.slots[i].ep_context[j].retry);
      BXRS_DEC_PARAM_FIELD(entry, retry_counter, BX_XHCI_THIS hub.slots[i].ep_context[j].retry_counter);
      for (k = 0; k < MAX_PSA_SIZE_NUM; k++) {
        sprintf(tmpname, "%d", k);
        entry1 = new bx_list_c(entry, tmpname);
        list3 = new bx_list_c(entry1, "ep_stream");
        BXRS_HEX_PARAM_FIELD(list3, tr_dequeue_pointer, BX_XHCI_THIS hub.slots[i].ep_context[j].stream[k].tr_dequeue_pointer);
        BXRS_PARAM_BOOL(list3, dcs, BX_XHCI_THIS hub.slots[i].ep_context[j].stream[k].dcs);
      }
    }
  }

  list1 = new bx_list_c(hub, "ring_members");
  list2 = new bx_list_c(list1, "command_ring");
  BXRS_HEX_PARAM_FIELD(list2, dq_pointer, BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer);
  BXRS_PARAM_BOOL(list2, rcs, BX_XHCI_THIS hub.ring_members.command_ring.rcs);
  list2 = new bx_list_c(list1, "event_rings");
  for (i = 0; i < INTERRUPTERS; i++) {
    sprintf(tmpname, "ring%d", i);
    item = new bx_list_c(list2, tmpname);
    BXRS_PARAM_BOOL(item, rcs, BX_XHCI_THIS hub.ring_members.event_rings[i].rcs);
    BXRS_HEX_PARAM_FIELD(item, trb_count, BX_XHCI_THIS hub.ring_members.event_rings[i].trb_count);
    BXRS_HEX_PARAM_FIELD(item, count, BX_XHCI_THIS hub.ring_members.event_rings[i].count);
    BXRS_HEX_PARAM_FIELD(item, cur_trb, BX_XHCI_THIS hub.ring_members.event_rings[i].cur_trb);
    entries = new bx_list_c(item, "entries");
    for (j = 0; j < (1<<MAX_SEG_TBL_SZ_EXP); j++) {
      sprintf(tmpname, "entry%d", j);
      entry = new bx_list_c(entries, tmpname);
      BXRS_HEX_PARAM_FIELD(entry, addr, BX_XHCI_THIS hub.ring_members.event_rings[i].entrys[j].addr);
      BXRS_HEX_PARAM_FIELD(entry, size, BX_XHCI_THIS hub.ring_members.event_rings[i].entrys[j].size);
    }
  }

  register_pci_state(hub);
}

void bx_usb_xhci_c::after_restore_state(void)
{
  bx_pci_device_c::after_restore_pci_state(NULL);
  for (unsigned int j=0; j<BX_XHCI_THIS hub.n_ports; j++) {
    if (BX_XHCI_THIS hub.usb_port[j].device != NULL) {
      BX_XHCI_THIS hub.usb_port[j].device->after_restore_state();
    }
  }
}

int xhci_event_handler(int event, void *ptr, void *dev, int port);

void bx_usb_xhci_c::init_device(Bit8u port, bx_list_c *portconf)
{
  char pname[BX_PATHNAME_LEN];

  if (DEV_usb_init_device(portconf, BX_XHCI_THIS_PTR, &BX_XHCI_THIS hub.usb_port[port].device, xhci_event_handler, port)) {
    if (set_connect_status(port, 1)) {
      portconf->get_by_name("options")->set_enabled(0);
      sprintf(pname, "usb_xhci.hub.port%d.device", port+1);
      bx_list_c *sr_list = (bx_list_c *) SIM->get_param(pname, SIM->get_bochs_root());
      BX_XHCI_THIS hub.usb_port[port].device->register_state(sr_list);
    } else {
      ((bx_param_enum_c*)portconf->get_by_name("device"))->set_by_name("none");
      ((bx_param_string_c*)portconf->get_by_name("options"))->set("none");
      ((bx_param_bool_c*)portconf->get_by_name("over_current"))->set(0);
      set_connect_status(port, 0);
    }
  }
}

void bx_usb_xhci_c::remove_device(Bit8u port)
{
  if (BX_XHCI_THIS hub.usb_port[port].device != NULL) {
    delete BX_XHCI_THIS hub.usb_port[port].device;
    BX_XHCI_THIS hub.usb_port[port].device = NULL;
  }
}

void bx_usb_xhci_c::update_irq(unsigned interrupter)
{
  bool level = 0;

  if ((BX_XHCI_THIS hub.op_regs.HcCommand.inte) &&
      (BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].iman.ie)) {
    level = 1;
    BX_DEBUG(("Interrupt Fired."));
  }
  DEV_pci_set_irq(BX_XHCI_THIS devfunc, BX_XHCI_THIS pci_conf[0x3d], level);
}

bool bx_usb_xhci_c::read_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u val = 0, val_hi = 0;
  int i, speed = 0;

  const Bit32u offset = (Bit32u) (addr - BX_XHCI_THIS pci_bar[0].addr);

  // Even though the controller allows reads other than 32-bits & on odd boundaries,
  //  we are going to ASSUME dword reads and writes unless specified below

  // RO Capability Registers
  if (offset < OPS_REGS_OFFSET) {
    switch (offset) {
      // Capability Registers
      case 0x00: // CapLength / version
        val = BX_XHCI_THIS hub.cap_regs.HcCapLength;
        break;
      case 0x01:
        val = BX_XHCI_THIS hub.cap_regs.HcCapLength >> 8;
        break;
      case 0x02:
        val = BX_XHCI_THIS hub.cap_regs.HcCapLength >> 16;
        break;
      case 0x04: // HCSPARAMS1
        val = BX_XHCI_THIS hub.cap_regs.HcSParams1;
        break;
      case 0x08: // HCSPARAMS2
        val = BX_XHCI_THIS hub.cap_regs.HcSParams2;
        break;
      case 0x0C: // HCSPARAMS3
        val = BX_XHCI_THIS hub.cap_regs.HcSParams3;
        break;
      case 0x10: // HCCPARAMS1
        val = BX_XHCI_THIS hub.cap_regs.HcCParams1;
        break;
      case 0x14: // DBOFF
        val = BX_XHCI_THIS hub.cap_regs.DBOFF;
        break;
      case 0x18: // RTSOFF
        val = BX_XHCI_THIS hub.cap_regs.RTSOFF;
        break;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
      case 0x1C: // HCCPARAMS2
        val = BX_XHCI_THIS hub.cap_regs.HcCParams2;
        break;
#else
      case 0x1C: // reserved
        val = 0;
        break;
#endif
    }
  } else

  // Operational Registers
  if ((offset >= OPS_REGS_OFFSET) && (offset < (OPS_REGS_OFFSET + 0x40))) {
    switch (offset - OPS_REGS_OFFSET) {
      case 0x00: // Command
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
        val =   (BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1       << 14)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.cme      ? 1 << 13 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.spe      ? 1 << 12 : 0)
#else
        val =   (BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1       << 12)
#endif
              | (BX_XHCI_THIS hub.op_regs.HcCommand.eu3s     ? 1 << 11 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.ewe      ? 1 << 10 : 0)
              | 0   // controller restore state always read as zero
              | 0   // controller save state always read as zero
              | (BX_XHCI_THIS hub.op_regs.HcCommand.lhcrst   ? 1 <<  7 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP0       <<  4)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.hsee     ? 1 <<  3 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.inte     ? 1 <<  2 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.hcrst    ? 1 <<  1 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcCommand.rs       ? 1 <<  0 : 0);
        break;
      case 0x04: // Status
        val =   (BX_XHCI_THIS hub.op_regs.HcStatus.hce       ? 1 << 12 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.cnr       ? 1 << 11 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.sre       ? 1 << 10 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.rss       ? 1 <<  9 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.sss       ? 1 <<  8 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.pcd       ? 1 <<  4 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.eint      ? 1 <<  3 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.hse       ? 1 <<  2 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcStatus.hch       ? 1 <<  0 : 0);
        break;
      case 0x08: // Page Size
        val =    BX_XHCI_THIS hub.op_regs.HcPageSize.pagesize;
        break;
      case 0x0C: // Reserved and Zero'd
      case 0x10: // Reserved and Zero'd
        val = 0;
        break;
      case 0x14: // Device Notification Control Register
        val =   (BX_XHCI_THIS hub.op_regs.HcNotification.RsvdP   << 16)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n15 ? 1 << 15 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n14 ? 1 << 14 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n13 ? 1 << 13 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n12 ? 1 << 12 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n11 ? 1 << 11 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n10 ? 1 << 10 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n9  ? 1 <<  9 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n8  ? 1 <<  8 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n7  ? 1 <<  7 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n6  ? 1 <<  6 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n5  ? 1 <<  5 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n4  ? 1 <<  4 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n3  ? 1 <<  3 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n2  ? 1 <<  2 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n1  ? 1 <<  1 : 0)
              | (BX_XHCI_THIS hub.op_regs.HcNotification.n0  ? 1 <<  0 : 0);
        break;
      case 0x18: // Command Ring Control Register (Lo) (most fields always returns zero when read)
        val =   (BX_XHCI_THIS hub.op_regs.HcCrcr.RsvdP           <<  4)
              | (BX_XHCI_THIS hub.op_regs.HcCrcr.crr         ? 1 <<  3 : 0);
        break;
      case 0x1C: // Command Ring Control Register (Hi) (always returns zero when read)
        val = 0;
        break;
      case 0x20: // Reserved and Zero'd
      case 0x24: // Reserved and Zero'd
      case 0x28: // Reserved and Zero'd
      case 0x2C: // Reserved and Zero'd
        val = 0;
        break;
      case 0x30: // DCBAAP (Lo)
        val = (Bit32u) ((Bit32u) BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap & ~0x3F);
        if (len == 8)
#if ADDR_CAP_64
          val_hi = (Bit32u) (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap >> 32);
#else
          val_hi = 0;
#endif  // ADDR_CAP_64
        break;
      case 0x34: // DCBAAP (Hi)
#if ADDR_CAP_64
        val = (Bit32u) (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap >> 32);
#else
        val = 0;
#endif  // ADDR_CAP_64
        break;
      case 0x38: // Config
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
        val =   (BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP         << 10)
              | (BX_XHCI_THIS hub.op_regs.HcConfig.cie           <<  8)
              | (BX_XHCI_THIS hub.op_regs.HcConfig.u3e           <<  9)
#else
        val =   (BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP         <<  8)
#endif
              | (BX_XHCI_THIS hub.op_regs.HcConfig.MaxSlotsEn    <<  0);
        break;
    }
  } else

  // Register Port Sets
  if ((offset >= PORT_SET_OFFSET) && (offset < (PORT_SET_OFFSET + (BX_XHCI_THIS hub.n_ports * 16)))) {
    unsigned port = (((offset - PORT_SET_OFFSET) >> 4) & 0x3F); // calculate port number
    if (BX_XHCI_THIS hub.usb_port[port].portsc.pp) {
      // the speed field is only valid for USB3 before a port reset.  If a reset has not
      //  taken place after the port is powered, the USB2 ports don't show a valid speed field.
      if (BX_XHCI_THIS hub.usb_port[port].portsc.ccs &&
         (BX_XHCI_THIS hub.usb_port[port].is_usb3 || BX_XHCI_THIS hub.usb_port[port].has_been_reset))
           speed = BX_XHCI_THIS hub.usb_port[port].portsc.speed;
      switch (offset & 0x0000000F) {
        case 0x00:
          val =  0 /* BX_XHCI_THIS hub.usb_port[port].portsc.wpr == 0 when read */
                | (BX_XHCI_THIS hub.usb_port[port].portsc.dr      ? 1 <<  30 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.woe     ? 1 <<  27 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.wde     ? 1 <<  26 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.wce     ? 1 <<  25 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.cas     ? 1 <<  24 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.cec     ? 1 <<  23 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.plc     ? 1 <<  22 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.prc     ? 1 <<  21 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.occ     ? 1 <<  20 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.wrc     ? 1 <<  19 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.pec     ? 1 <<  18 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.csc     ? 1 <<  17 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.pic         <<  14)
                | (                                       speed       <<  10)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.pp      ? 1 <<   9 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.pls         <<   5)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.pr      ? 1 <<   4 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.oca     ? 1 <<   3 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.ped     ? 1 <<   1 : 0)
                | (BX_XHCI_THIS hub.usb_port[port].portsc.ccs     ? 1 <<   0 : 0);
          break;
        case 0x04:
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            val =    (BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.RsvdP         << 17)
                   | (BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.fla       ? 1 << 16 : 0)
                   | (BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u2timeout     <<  8)
                   | (BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u1timeout     <<  0);
          } else {
            val =    (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.tmode         << 28)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP         << 17)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.hle       ? 1 << 16 : 0)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.l1dslot       <<  8)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.hird          <<  4)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.rwe       ? 1 <<  3 : 0)
                   | (BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.l1s           <<  0);
          }
          break;
        case 0x08:
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            val =    (BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP           << 16)
                   | (BX_XHCI_THIS hub.usb_port[port].usb3.portli.lec             <<  0);
          } else {
            val =     BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP;
          }
          break;
        case 0x0C:
#if ((VERSION_MAJOR < 1) || ((VERSION_MAJOR == 1) && (VERSION_MINOR == 0)))
          BX_ERROR(("Read from Reserved Register in Port Register Set %d", port));
#else
          val =    (BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP             << 14)
                 | (BX_XHCI_THIS hub.usb_port[port].porthlpmc.hirdd             << 10)
                 | (BX_XHCI_THIS hub.usb_port[port].porthlpmc.l1timeout         <<  2)
                 | (BX_XHCI_THIS hub.usb_port[port].porthlpmc.hirdm             <<  0);
#endif
          break;
      }
    } else val = 0;
  } else

  // Extended Capabilities
  if ((offset >= EXT_CAPS_OFFSET) && (offset < (EXT_CAPS_OFFSET + EXT_CAPS_SIZE))) {
    unsigned caps_offset = (offset - EXT_CAPS_OFFSET);
    switch (len) {
      case 1:
        val = BX_XHCI_THIS hub.extended_caps[caps_offset];
        break;
      case 2:
        val = BX_XHCI_THIS hub.extended_caps[caps_offset] |
              (BX_XHCI_THIS hub.extended_caps[caps_offset + 1] << 8);
        break;
      case 8:
        val_hi = (BX_XHCI_THIS hub.extended_caps[caps_offset + 4] |
                  (BX_XHCI_THIS hub.extended_caps[caps_offset + 5] << 8) |
                  (BX_XHCI_THIS hub.extended_caps[caps_offset + 6] << 16) |
                  (BX_XHCI_THIS hub.extended_caps[caps_offset + 7] << 24));
      case 4:
        val = (BX_XHCI_THIS hub.extended_caps[caps_offset] |
               (BX_XHCI_THIS hub.extended_caps[caps_offset + 1] << 8) |
               (BX_XHCI_THIS hub.extended_caps[caps_offset + 2] << 16) |
               (BX_XHCI_THIS hub.extended_caps[caps_offset + 3] << 24));
        break;
    }
  } else

  // Host Controller Runtime Registers
  if ((offset >= RUNTIME_OFFSET) && (offset < (RUNTIME_OFFSET + 32 + (INTERRUPTERS * 32)))) {
    if (offset == RUNTIME_OFFSET) {
      val =   (BX_XHCI_THIS hub.runtime_regs.mfindex.RsvdP << 14)
            | (BX_XHCI_THIS hub.runtime_regs.mfindex.index <<  0);
    } else if (offset < (RUNTIME_OFFSET + 32)) {
      val = 0;
    } else {
      unsigned rt_offset = (offset - RUNTIME_OFFSET - 32);
      i = (rt_offset >> 5);  // interrupter offset
      switch (rt_offset & 0x1F) {
        case 0x00:
          val =   (BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.RsvdP      << 2)
            | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ie     ? 1 << 1 : 0)
            | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ip     ? 1 << 0 : 0);
          break;
        case 0x04:
          val =   (BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodc << 16)
                | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodi << 0);
          break;
        case 0x08:
          val =   (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.RsvdP << 16)
                | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.erstabsize << 0);
          break;
        case 0x0C:
          val = BX_XHCI_THIS hub.runtime_regs.interrupter[i].RsvdP;
          break;
        case 0x10:
          val =   ((Bit32u) BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd & ~0x3F)
                | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.RsvdP  << 0);
          if (len == 8)
#if ADDR_CAP_64
            val_hi = (Bit32u) (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd >> 32);
#else
            val_hi = 0;
#endif  // ADDR_CAP_64
          break;
        case 0x14:
#if ADDR_CAP_64
          val = (Bit32u) (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd >> 32);
#else
          val = 0;
#endif  // ADDR_CAP_64
          break;
        case 0x18:
          val = (Bit32u) ((Bit32u) BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd & ~0x0F)
                | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.ehb        ? 1 << 3 : 0)
                | (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.desi           << 0);
          if (len == 8)
#if ADDR_CAP_64
            val_hi = (Bit32u) (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd >> 32);
#else
            val_hi = 0;
#endif  // ADDR_CAP_64
          break;
        case 0x1C:
#if ADDR_CAP_64
          val = (Bit32u) (BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd >> 32);
#else
          val = 0;
#endif  // ADDR_CAP_64
          break;
      }
    }
  } else

  // Doorbell Registers (return zero when read)
  if ((offset >= DOORBELL_OFFSET) && (offset < (DOORBELL_OFFSET + 4 + (INTERRUPTERS * 4)))) {
    val = 0;
  } else {
#if BX_PHY_ADDRESS_LONG
    BX_ERROR(("register read from unknown offset 0x%08X:  0x%08X%08X (len=%d)", offset, (Bit32u) val_hi, (Bit32u) val, len));
#else
    BX_DEBUG(("register read from unknown offset 0x%08X:  0x%08X (len=%d)", offset, (Bit32u) val, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%d)", len));
#endif
  }
  switch (len) {
    case 1:
      val &= 0xFF;
      *((Bit8u *) data) = (Bit8u) val;
      break;
    case 2:
      val &= 0xFFFF;
      *((Bit16u *) data) = (Bit16u) val;
      break;
    case 8:
      *((Bit32u *) ((Bit8u *) data + 4)) = val_hi;
    case 4:
      *((Bit32u *) data) = val;
      break;
  }

  // don't populate the log file if reading from interrupter's IMAN and only INT_ENABLE is set.
  // (This only works with the first interrupter)
  if ((offset != 0x620) || (val != 0x02)) {
#if BX_PHY_ADDRESS_LONG
    // this populates the log file, so we comment it out for now.
    //BX_DEBUG(("register read from offset 0x%04X:  0x%08X%08X (len=%d)", offset, (Bit32u) val_hi, (Bit32u) val, len));
#else
    BX_DEBUG(("register read from offset 0x%04X:  0x%08X (len=%d)", offset, (Bit32u) val, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%d)", len));
#endif
  }
  return 1;
}

bool bx_usb_xhci_c::write_handler(bx_phy_address addr, unsigned len, void *data, void *param)
{
  Bit32u value = *((Bit32u *) data);
  Bit32u value_hi = *((Bit32u *) ((Bit8u *) data + 4));
  const Bit32u offset = (Bit32u) (addr - BX_XHCI_THIS pci_bar[0].addr);
  Bit32u temp;
  int i;

  // modify val and val_hi per len of data to write
  switch (len) {
    case 1:
      value &= 0xFF;
    case 2:
      value &= 0xFFFF;
    case 4:
      value_hi = 0;
      break;
  }

#if BX_PHY_ADDRESS_LONG
    BX_DEBUG(("register write to  offset 0x%04X:  0x%08X%08X (len=%d)", offset, value_hi, value, len));
#else
    BX_DEBUG(("register write to  offset 0x%04X:  0x%08X (len=%d)", offset, value, len));
    if (len > 4)
      BX_DEBUG(("Ben: In 32-bit mode, len > 4! (len=%d)", len));
#endif

  // Even though the controller allows reads other than 32-bits & on odd boundaries,
  //  we are going to ASSUME dword reads and writes unless specified below

  // RO Capability Registers
  if (offset < OPS_REGS_OFFSET) {
    switch (offset) {
      // Capability Registers
      case 0x00: // CapLength / version
      case 0x04: // HCSPARAMS1
      case 0x08: // HCSPARAMS2
      case 0x0C: // HCSPARAMS3
      case 0x10: // HCCPARAMS1
      case 0x14: // DBOFF
      case 0x18: // TRSOFF
      case 0x1C: // reserved/HCCPARAMS2
        BX_ERROR(("Write to Read Only Host Capability Register (0x%08X)", offset));
        break;
    }
  } else

  // Operational Registers
  if ((offset >= OPS_REGS_OFFSET) && (offset < (OPS_REGS_OFFSET + 0x40))) {
    switch (offset - OPS_REGS_OFFSET) {
      case 0x00: // Command
        temp = BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
        BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1 = (value >> 14);
        if (temp != BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1)
          BX_ERROR(("bits 31:14 in command register were not preserved"));
        BX_XHCI_THIS hub.op_regs.HcCommand.cme    = (value & (1 << 13)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCommand.spe    = (value & (1 << 12)) ? 1 : 0;
#else
        BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1 = (value >> 12);
        if (temp != BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP1)
          BX_ERROR(("bits 31:12 in command register were not preserved"));
#endif
        BX_XHCI_THIS hub.op_regs.HcCommand.eu3s   = (value & (1 << 11)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCommand.ewe    = (value & (1 << 10)) ? 1 : 0;
        if (value & (1 <<  9)) {
          if (BX_XHCI_THIS hub.op_regs.HcStatus.hch == 1) {
            // show that we are restoring the state
            //  (redundant since emulated host can't see it set until we are done here)
            BX_XHCI_THIS hub.op_regs.HcStatus.rss = 1;
            BX_XHCI_THIS hub.op_regs.HcStatus.sre = BX_XHCI_THIS restore_hc_state();
            // show that the restoring is done. (again, redundant, but we do it anyway)
            BX_XHCI_THIS hub.op_regs.HcStatus.rss = 0;
          } else
            BX_ERROR(("Restore State when controller not in Halted state."));
        }
        if (value & (1 <<  8)) {
          if (BX_XHCI_THIS hub.op_regs.HcStatus.hch == 1) {
            // show that we are saving the state
            //  (redundant since emulated host can't see it set until we are done here)
            BX_XHCI_THIS hub.op_regs.HcStatus.sss = 1;
            BX_XHCI_THIS hub.op_regs.HcStatus.sre = BX_XHCI_THIS save_hc_state();
            // show that the saving is done. (again, redundant, but we do it anyway)
            BX_XHCI_THIS hub.op_regs.HcStatus.sss = 0;
          } else
            BX_ERROR(("Save State when controller not in Halted state."));
        }
#if LIGHT_HC_RESET
        BX_XHCI_THIS hub.op_regs.HcCommand.lhcrst = (value & (1 <<  7)) ? 1 : 0;
        // TODO: Do light reset
#endif // LIGHT_HC_RESET
        temp = BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP0;
        BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP0 = (value & (7 <<  4)) >> 4;
        if (temp != BX_XHCI_THIS hub.op_regs.HcCommand.RsvdP0)
          BX_ERROR(("bits 6:4 in Command Register were not preserved"));
        BX_XHCI_THIS hub.op_regs.HcCommand.hsee   = (value & (1 <<  3)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCommand.inte   = (value & (1 <<  2)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCommand.hcrst  = (value & (1 <<  1)) ? 1 : 0;
        if (BX_XHCI_THIS hub.op_regs.HcCommand.hcrst) {
          BX_XHCI_THIS reset_hc();
          BX_XHCI_THIS hub.op_regs.HcCommand.hcrst = 0;
          // the controller will send a reset to all USB 3.0 ports,
          //  enabling the port if a device is attached.
          for (unsigned int port=0; port<BX_XHCI_THIS hub.n_ports; port++) {
            if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
              reset_port_usb3(port, HOT_RESET);
            }
          }
        }

        // if run/stop bit cleared, stop command ring
        BX_XHCI_THIS hub.op_regs.HcCommand.rs     = (value & (1 <<  0)) ? 1 : 0;
        if (BX_XHCI_THIS hub.op_regs.HcCommand.rs == 0) {
          BX_XHCI_THIS hub.op_regs.HcCrcr.crr = 0;
          BX_XHCI_THIS hub.op_regs.HcStatus.hch = 1;  // set the Halted Bit
        } else
          BX_XHCI_THIS hub.op_regs.HcStatus.hch = 0;  // clear the Halted Bit
        break;

      case 0x04: // Status
        if ((value & 0xFFFFE0E2) != 0)
          BX_ERROR(("RsvdZ bits in HcStatus not written as zeros"));
        if (value & ((1<<12) | (1<<11) | (1<<9) | (1<<8) | (1<<0)))
          BX_ERROR(("Write to one or more Read-only bits in Status Register"));
        BX_XHCI_THIS hub.op_regs.HcStatus.sre     = (value & (1 << 10)) ? 0 : BX_XHCI_THIS hub.op_regs.HcStatus.sre;
        BX_XHCI_THIS hub.op_regs.HcStatus.pcd     = (value & (1 <<  4)) ? 0 : BX_XHCI_THIS hub.op_regs.HcStatus.pcd;
        BX_XHCI_THIS hub.op_regs.HcStatus.eint    = (value & (1 <<  3)) ? 0 : BX_XHCI_THIS hub.op_regs.HcStatus.eint;
        BX_XHCI_THIS hub.op_regs.HcStatus.hse     = (value & (1 <<  2)) ? 0 : BX_XHCI_THIS hub.op_regs.HcStatus.hse;
        if (value & (1 << 3))  // acknowledging the interrupt
          DEV_pci_set_irq(BX_XHCI_THIS devfunc, BX_XHCI_THIS pci_conf[0x3d], 0);
        break;

      case 0x08: // Page Size
        BX_ERROR(("Write to one or more Read-only bits in Page Size Register"));
        break;

      case 0x0C: // Reserved and Zero'd
      case 0x10: // Reserved and Zero'd
        if (value != 0)
          BX_ERROR(("Write non-zero to RsvdZ Register (offset = 0x%08X  value = 0x%08X)", offset, value));
        break;

      case 0x14: // Device Notification Control Register
        temp = BX_XHCI_THIS hub.op_regs.HcNotification.RsvdP;
        BX_XHCI_THIS hub.op_regs.HcNotification.RsvdP = value >> 16;
        if (temp != BX_XHCI_THIS hub.op_regs.HcNotification.RsvdP)
          BX_ERROR(("bits 31:16 in DNCTRL Register were not preserved"));
        BX_XHCI_THIS hub.op_regs.HcNotification.n15 = (value & (1 << 15)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n14 = (value & (1 << 14)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n13 = (value & (1 << 13)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n12 = (value & (1 << 12)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n11 = (value & (1 << 11)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n10 = (value & (1 << 10)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n9  = (value & (1 <<  9)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n8  = (value & (1 <<  8)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n7  = (value & (1 <<  7)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n6  = (value & (1 <<  6)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n5  = (value & (1 <<  5)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n4  = (value & (1 <<  4)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n3  = (value & (1 <<  3)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n2  = (value & (1 <<  2)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n1  = (value & (1 <<  1)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcNotification.n0  = (value & (1 <<  0)) ? 1 : 0;
        break;

      case 0x18: // Command Ring Control Register (Lo)
        if (value & (1<<3))
          BX_ERROR(("Write to one or more Read-only bits in CRCR Register"));
        temp = BX_XHCI_THIS hub.op_regs.HcCrcr.RsvdP;
        BX_XHCI_THIS hub.op_regs.HcCrcr.RsvdP = (value & (2<<4)) >> 4;
        if (temp != BX_XHCI_THIS hub.op_regs.HcCrcr.RsvdP)
          BX_ERROR(("bits 5:4 in CRCR Register were not preserved"));
        BX_XHCI_THIS hub.op_regs.HcCrcr.ca    = (value & (1 <<  2)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCrcr.cs    = (value & (1 <<  1)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcCrcr.rcs   = (value & (1 <<  0)) ? 1 : 0;
        // save the new ring pointer
#if ADDR_CAP_64
        if (len == 8) {
          BX_XHCI_THIS hub.op_regs.HcCrcr.crc = (Bit64u) (((Bit64u) value_hi << 32) | (value & ~0x3F));
        } else {
          BX_XHCI_THIS hub.op_regs.HcCrcr.crc &= (Bit64u) ~0xFFFFFFFF;
          BX_XHCI_THIS hub.op_regs.HcCrcr.crc |= (Bit64u) (value & ~0x3F);
        }
#else
        BX_XHCI_THIS hub.op_regs.HcCrcr.crc = (Bit64u) (value & ~0x3F);
#endif
        BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = BX_XHCI_THIS hub.op_regs.HcCrcr.crc;
        BX_XHCI_THIS hub.ring_members.command_ring.rcs = BX_XHCI_THIS hub.op_regs.HcCrcr.rcs;
        // if command stop or abort, stop command ring
        if (BX_XHCI_THIS hub.op_regs.HcCrcr.ca || BX_XHCI_THIS hub.op_regs.HcCrcr.cs)
          BX_XHCI_THIS hub.op_regs.HcCrcr.crr = 0;
        break;

      case 0x1C: // Command Ring Control Register (Hi)
#if ADDR_CAP_64
        BX_XHCI_THIS hub.op_regs.HcCrcr.crc &= (Bit64u) 0xFFFFFFFF;
        BX_XHCI_THIS hub.op_regs.HcCrcr.crc |= (Bit64u) ((Bit64u) value << 32);
        BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = BX_XHCI_THIS hub.op_regs.HcCrcr.crc;
#endif
        break;

      case 0x20: // Reserved and Zero'd
      case 0x24: // Reserved and Zero'd
      case 0x28: // Reserved and Zero'd
      case 0x2C: // Reserved and Zero'd
        if (value != 0)
          BX_ERROR(("Write non-zero to RsvdZ Register (offset = 0x%08X  value = 0x%08X)", offset, value));
        break;

      case 0x30: // DCBAAP (Lo)
        if (value & 0x3F)
          BX_ERROR(("Write non-zero to RsvdZ member of DCBAAP Register"));
#if ADDR_CAP_64
        if (len == 8)
          BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap = (Bit64u) (((Bit64u) value_hi << 32) | (value & ~0x3F));
        else {
          BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap &= (Bit64u) ~0xFFFFFFFF;
          BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap |= (Bit64u) (value & ~0x3F);
        }
#else
        BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap = (Bit64u) (value & ~0x3F);
#endif
        break;

      case 0x34: // DCBAAP (Hi)
#if ADDR_CAP_64
        BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap &= (Bit64u) 0xFFFFFFFF;
        BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap |= (Bit64u) ((Bit64u) value << 32);
#endif
        break;

      case 0x38: // Config
        temp = BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP;
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
        BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP = (value >> 10);
        if (temp != BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP)
          BX_ERROR(("bits 31:10 in Config Register were not preserved"));
        BX_XHCI_THIS hub.op_regs.HcConfig.cie =  (value & (1 << 9)) ? 1 : 0;
        BX_XHCI_THIS hub.op_regs.HcConfig.u3e =  (value & (1 << 8)) ? 1 : 0;
#else
        BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP = (value >> 8);
        if (temp != BX_XHCI_THIS hub.op_regs.HcConfig.RsvdP)
          BX_ERROR(("bits 31:8 in Config Register were not preserved"));
#endif
        BX_XHCI_THIS hub.op_regs.HcConfig.MaxSlotsEn = (value & 0xFF);
        break;
    }
  } else

  // Register Port Sets
  if ((offset >= PORT_SET_OFFSET) && (offset < (PORT_SET_OFFSET + (BX_XHCI_THIS hub.n_ports * 16)))) {
    unsigned port = (((offset - PORT_SET_OFFSET) >> 4) & 0x3F); // calculate port number
    switch (offset & 0x0000000F) {
      case 0x00:
        if (value & (1<<9)) {  // port power
          if (value & ((1<<30) | (1<<24) | (1<<3) | (1<<0)))
            BX_DEBUG(("Write to one or more Read-only bits in PORTSC[%d] Register (0x%08X)", port, value));
          if (value & ((3<<28) | (1<<2)))
            BX_ERROR(("Write non-zero to a RsvdZ member of PORTSC[%d] Register", port));
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            BX_XHCI_THIS hub.usb_port[port].portsc.wpr = (value & (1 << 31)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].portsc.cec = (value & (1 << 23)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].portsc.wrc = (value & (1 << 19)) ? 0 : BX_XHCI_THIS hub.usb_port[port].portsc.wrc;
            if (value & (1<<18))  // For a USB3 protocol port, this bit shall never be set to 1, however doesn't hurt to write to it anyway
              BX_DEBUG(("Write to USB3 port: bit 18"));
          } else {
            BX_XHCI_THIS hub.usb_port[port].portsc.pec = (value & (1 << 18)) ? 0 : BX_XHCI_THIS hub.usb_port[port].portsc.pec;
            if (value & ((1<<31) | (1<<23) | (1<<19)))
              BX_ERROR(("Write to USB2 port: RsvdZ bit"));
          }
          // The WC1 bits must be before anything that will change these bits below.
          if (value & (1 << 22)) BX_XHCI_THIS hub.usb_port[port].portsc.plc = 0;
          if (value & (1 << 21)) BX_XHCI_THIS hub.usb_port[port].portsc.prc = 0;
          if (value & (1 << 20)) BX_XHCI_THIS hub.usb_port[port].portsc.occ = 0;
          if (value & (1 << 17)) BX_XHCI_THIS hub.usb_port[port].portsc.csc = 0;
          if (value & (1 <<  1)) BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
          BX_XHCI_THIS hub.usb_port[port].portsc.woe   = (value & (1 << 27)) ? 1 : 0;
          BX_XHCI_THIS hub.usb_port[port].portsc.wde   = (value & (1 << 26)) ? 1 : 0;
          BX_XHCI_THIS hub.usb_port[port].portsc.wce   = (value & (1 << 25)) ? 1 : 0;
          BX_XHCI_THIS hub.usb_port[port].portsc.pic   = (value & (0x3 << 14)) >> 14;
          // if transition from non-powered to powered
          if (BX_XHCI_THIS hub.usb_port[port].portsc.pp == 0) {
            // a "has been reset" is false.
            BX_XHCI_THIS hub.usb_port[port].has_been_reset = 0;
          }
          BX_XHCI_THIS hub.usb_port[port].portsc.pp = 1;
          if (value & (1<<16)) {  // LWS
            switch ((value & (0xF << 5)) >> 5) {
              case 0:
                BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_U0;
                break;
              case 2:  // USB2 only
                BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_U2;
                break;
              case 3:
                BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_U3_SUSPENDED;
                break;
              case 5:  // USB3 only
                if (BX_XHCI_THIS hub.usb_port[port].portsc.pls == PLS_DISABLED) {
                  BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_RXDETECT;
                  BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
                  BX_XHCI_THIS hub.usb_port[port].portsc.pec = 1;
                }
                break;
              case 15:  // USB2 only
                if (BX_XHCI_THIS hub.usb_port[port].portsc.pls == PLS_U3_SUSPENDED) {
                  // port should transition to the U3Exit state...
                }
                break;
            }
          }
          // if port reset bit is set, reset the port, then enable the port (if ccs == 1).
          if (((value & (1 << 31)) && BX_XHCI_THIS hub.usb_port[port].is_usb3) ||
               (value & (1 << 4))) {
            reset_port_usb3(port, (value & (1 << 4)) ? HOT_RESET : WARM_RESET);
          }
        } else
          BX_XHCI_THIS hub.usb_port[port].portsc.pp = 0;
        break;
      case 0x04:
        if (BX_XHCI_THIS hub.usb_port[port].portsc.pp) {
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            temp = BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.RsvdP = value >> 17;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.RsvdP)
              BX_ERROR(("bits 31:17 in PORTPMSC[%d] Register were not preserved", port));
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.fla   = (value & (1 << 16)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u2timeout = (value & (0xFF << 8)) >> 8;
            BX_XHCI_THIS hub.usb_port[port].usb3.portpmsc.u1timeout = (value & (0xFF << 0)) >> 0;
          } else {
            if (value & (7<<0))
              BX_ERROR(("Write to one or more Read-only bits in PORTPMSC[%d] Register", port));
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.tmode     = (value & (0xF << 28)) >> 28;
            temp = BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP     = (value & (0x1FFF << 15)) >> 15;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.RsvdP)
              BX_ERROR(("bits 27:15 in PORTPMSC[%d] Register were not preserved", port));
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.hle       = (value & (1 << 16)) ? 1 : 0;
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.l1dslot   = (value & (0xFF <<  8)) >> 8;
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.hird      = (value & (0xF  <<  4)) >> 4;
            BX_XHCI_THIS hub.usb_port[port].usb2.portpmsc.rwe       = (value & (1 <<  3)) ? 1 : 0;
          }
        }
        break;
      case 0x08:
        if (BX_XHCI_THIS hub.usb_port[port].portsc.pp) {
          if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
            if (value & (0xFFFF<<0))
              BX_ERROR(("Write to one or more Read-only bits in PORTLI[%d] Register", port));
            temp = BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP = value >> 16;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb3.portli.RsvdP)
              BX_ERROR(("bits 31:16 in PORTLI[%d] Register were not preserved", port));
          } else {
            temp = BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP;
            BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP = value;
            if (temp != BX_XHCI_THIS hub.usb_port[port].usb2.portli.RsvdP)
              BX_ERROR(("bits 31:0 in PORTLI[%d] Register were not preserved", port));
          }
        }
        break;
      case 0x0C:
#if ((VERSION_MAJOR < 1) || ((VERSION_MAJOR == 1) && (VERSION_MINOR == 0)))
        BX_ERROR(("Write to Reserved Register in Port Register Set %d", port));
#else
        temp = BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP;
        BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP = (value >> 14);
        if (temp != BX_XHCI_THIS hub.usb_port[port].porthlpmc.RsvdP)
          BX_ERROR(("bits 31:14 in PORTHLPMC[%d] Register were not preserved", port));
        BX_XHCI_THIS hub.usb_port[port].porthlpmc.hirdd = ((value & (0x0F << 10)) >> 10);
        BX_XHCI_THIS hub.usb_port[port].porthlpmc.l1timeout = ((value & (0xFF << 2)) >> 2);
        BX_XHCI_THIS hub.usb_port[port].porthlpmc.hirdm = ((value & (0x0F << 0)) >> 0);
#endif
        break;
    }
  } else

  // Extended Capabilities
  if ((offset >= EXT_CAPS_OFFSET) && (offset < (EXT_CAPS_OFFSET + EXT_CAPS_SIZE))) {
    unsigned caps_offset = (offset - EXT_CAPS_OFFSET);
    Bit64u qword = (((Bit64u) value_hi << 32) | value);
    while (len) {
      *(Bit8u *) &BX_XHCI_THIS hub.extended_caps[caps_offset] = (Bit8u) (qword & 0xFF);
      switch (caps_offset) {
        case 3:
          if (qword & 1)  // clear the BIOS owner bit
            BX_XHCI_THIS hub.extended_caps[2] &= ~(1<<0);
          break;
        default:
          ;
      }
      len--;
      caps_offset++;
      qword >>= 8;
    }
  } else

  // Host Controller Runtime Registers
  if ((offset >= RUNTIME_OFFSET) && (offset < (RUNTIME_OFFSET + 32 + (INTERRUPTERS * 32)))) {
    if (offset == RUNTIME_OFFSET) {
      BX_ERROR(("Write to MFINDEX register"));
    } else if (offset < (RUNTIME_OFFSET + 32)) {
      BX_ERROR(("Write to Reserved Register in HC Runtime Register set"));
    } else {
      unsigned rt_offset = (offset - RUNTIME_OFFSET - 32);
      i = (rt_offset >> 5);  // interrupter offset
      switch (rt_offset & 0x1F) {
        case 0x00:
          temp = BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.RsvdP;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.RsvdP = (value >> 2);
          if (temp != BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.RsvdP)
            BX_ERROR(("bits 31:2 in IMAN Register were not preserved"));
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ie    = ((value & (1 << 1)) == (1 << 1));
          if (value & (1 << 0))
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].iman.ip  = 0;
          break;
        case 0x04:
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodc = (value >> 16);
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].imod.imodi = (value & 0xFFFF);
          break;
        case 0x08:
          temp = BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.RsvdP;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.RsvdP = (value >> 16);
          if (temp != BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.RsvdP)
            BX_ERROR(("bits 31:16 in ERSTSZ Register were not preserved"));
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstsz.erstabsize = (value & 0xFFFF);
          break;
        case 0x0C:
          temp = BX_XHCI_THIS hub.runtime_regs.interrupter[i].RsvdP;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].RsvdP = value;
          if (temp != BX_XHCI_THIS hub.runtime_regs.interrupter[i].RsvdP)
            BX_ERROR(("bits 31:0 in RsvdP (0x0C) Register were not preserved"));
          break;
        case 0x10:
          temp = BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.RsvdP;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.RsvdP = (value & ERSTABADD_MASK);
          if (temp != BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.RsvdP)
            BX_ERROR(("RsvdP bits in ERSTBA Register were not preserved"));
#if ADDR_CAP_64
          if (len == 8) {
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd = (((Bit64u) value_hi << 32) | (value & ~ERSTABADD_MASK));
            init_event_ring(i); // initialize event ring members
          } else {
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd &= (Bit64u) ~0xFFFFFFFF;
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd |= (Bit64u) (value & ~ERSTABADD_MASK);
          }
#else
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd = (Bit64u) (value & ~ERSTABADD_MASK);
          init_event_ring(i); // initialize event ring members
#endif
          break;
        case 0x14:
#if ADDR_CAP_64
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd &= (Bit64u) 0xFFFFFFFF;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erstba.erstabadd |= ((Bit64u) value << 32);
          init_event_ring(i); // initialize event ring members
#endif
          break;
        case 0x18:
          if (value & (1 << 3))
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.ehb      = 0;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.desi       = (value & 0x07);
#if ADDR_CAP_64
          if (len == 8)
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd = (Bit64u) (((Bit64u) value_hi << 32) | (value & ~0x0F));
          else {
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd &= (Bit64u) ~0xFFFFFFFF;
            BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd |= (Bit64u) (value & ~0x0F);
          }
#else
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd == (Bit64u) (value & ~0x0F);
#endif
          break;
        case 0x1C:
#if ADDR_CAP_64
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd &= (Bit64u) 0xFFFFFFFF;
          BX_XHCI_THIS hub.runtime_regs.interrupter[i].erdp.eventadd |= ((Bit64u) value << 32);
#endif
          break;
      }
    }
  } else

  // Doorbell Registers
  if ((offset >= DOORBELL_OFFSET) && (offset < (DOORBELL_OFFSET + 4 + (INTERRUPTERS * 4)))) {
    if (value & (0xFF << 8))
      BX_ERROR(("RsvdZ field of Doorbell written as non zero."));
    unsigned doorbell = ((offset - DOORBELL_OFFSET) >> 2);
    if (doorbell == 0) { // Command Doorbell
      BX_DEBUG(("Command Doorbell Rang"));
      if (value & (0xFFFF << 16))
        BX_ERROR(("DB Stream ID not zero when Command Doorbell rung"));
      if ((value & 0xFF) != 0)
        BX_ERROR(("Command Doorbell rang with non zero value: 0x%02X", (value & 0xFF)));
      // if the run/stop bit is set, the command ring is running
      if (BX_XHCI_THIS hub.op_regs.HcCommand.rs)
        BX_XHCI_THIS hub.op_regs.HcCrcr.crr = 1;
      process_command_ring();
    } else {
      // doorbell = slot to use (1 based)
      // (value & 0xFF) = ep (1 = control, 2 = ep1 out, 3 = ep1 in, etc);
      int ep = (value & 0xFF);
      BX_DEBUG(("Rang Doorbell:  slot = %d  ep = %d (%s)", doorbell, ep, (ep & 1) ? "IN" : "OUT"));
      if (ep > 31) {
        BX_ERROR(("Doorbell rang with EP > 31  (ep = %d)", ep));
      } else {
#if MAX_PSA_SIZE > 0
        // is this endpoint using streams?
        if (BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.max_pstreams > 0) {   // specifying streams
          // the ep must be a super-speed bulk endpoint
          if ((BX_XHCI_THIS hub.slots[doorbell].slot_context.speed == XHCI_SPEED_SUPER) &&   // is a super-speed device
             ((BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.ep_type == 2) ||   // is a bulk out
              (BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.ep_type == 6))) {  //   or bulk in
            // if the lsa bit is set, use primary streams only
            if (BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.lsa == 1) {
              unsigned primary_id = PSA_PRIMARY_MASK(value, BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.max_pstreams);
              if ((primary_id > 0) && (primary_id < MAX_PSA_SIZE_NUM) &&
                  (primary_id < PSA_MAX_SIZE_NUM(BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.max_pstreams)) &&
                   BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].stream[primary_id].valid == 1) {
                BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].stream[primary_id].tr_dequeue_pointer =
                  BX_XHCI_THIS process_transfer_ring(doorbell, ep, BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].stream[primary_id].tr_dequeue_pointer, 
                                                                  &BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].stream[primary_id].dcs, primary_id);
              } else {
                BX_ERROR(("Stream Context with bad Primary Stream ID (%d)", primary_id));
                // no TRB is associated, so can't include a TRB address. Does this make the EventTRB invalid?
                //write_event_TRB(0, 0, TRB_SET_COMP_CODE(INVALID_STREAM_ID), TRB_SET_SLOT(doorbell) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT), 1);
                //BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.ep_state = EP_STATE_HALTED; // call a function to do this correctly.
              }
            } else {
  #if NO_SSD_SUPPORT
              BX_ERROR(("The EP's context lsa bit is zero. Secondary Streams are not supported."));
  #else
              // secondary streams are used
              unsigned primary_id = PSA_PRIMARY_MASK(value, BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.max_pstreams);
              unsigned secondary_id = PSA_SECONDARY_MASK(value, BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].ep_context.max_pstreams);
              BX_ERROR(("The EP's context lsa bit is set. Secondary Streams are not yet supported."));
              // TODO:
  #endif
            }
          } else {
            BX_ERROR(("EP:MaxPStream > 0 on a non-Bulk SS device endpoint"));
          }
        } else {
#endif
          // standard trb ring
          BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].enqueue_pointer =
            BX_XHCI_THIS process_transfer_ring(doorbell, ep, BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].enqueue_pointer, 
                                                            &BX_XHCI_THIS hub.slots[doorbell].ep_context[ep].rcs, 0);
#if MAX_PSA_SIZE > 0
        }
#endif
      }
    }
  } else
    BX_ERROR(("register write to unknown offset 0x%08X:  0x%08X%08X (len=%d)", offset, (Bit32u) value_hi, (Bit32u) value, len));

  return 1;
}

void bx_usb_xhci_c::get_stream_info(struct STREAM_CONTEXT *context, const Bit64u address, const int index) {
  struct STREAM_CONTEXT stream_context;
  Bit8u buffer[16];

  // the first stream context is reserved
  if ((index > 0) && (index < MAX_PSA_SIZE_NUM)) {
    // read in the stream context
    DEV_MEM_READ_PHYSICAL((bx_phy_address) address + (index * 16), 16, buffer);
    copy_stream_from_buffer(&stream_context, buffer);
    if ((stream_context.sct == 1) && (stream_context.tr_dequeue_pointer != 0)) {
      context->valid = 1; // mark that this is a valid stream context
      context->tr_dequeue_pointer = stream_context.tr_dequeue_pointer;
      context->dcs = stream_context.dcs;
      context->sct = stream_context.sct;
    } else {
#if NO_SSD_SUPPORT
      context->valid = 0;
      BX_DEBUG(("Stream Context index %d with SCT != 1 (%d)", index, stream_context.sct));
#else
      BX_DEBUG(("Stream Context index %d with SCT == 0 (%d)...Secondary Streams not yet supported.", index));
#endif
    }
  }
}

void bx_usb_xhci_c::put_stream_info(struct STREAM_CONTEXT *context, const Bit64u address, const int index) {
  Bit8u buffer[16];

  // the first stream context is reserved
  if ((index > 0) && (index < MAX_PSA_SIZE_NUM)) {
    copy_stream_to_buffer(buffer, context);
    // write the stream context
    DEV_MEM_WRITE_PHYSICAL((bx_phy_address) address + (index * 16), 16, buffer);
  }
}

int xhci_event_handler(int event, void *ptr, void *dev, int port)
{
  if (dev != NULL) {
    return ((bx_usb_xhci_c*)dev)->event_handler(event, ptr, port);
  }
  return -1;
}

int bx_usb_xhci_c::event_handler(int event, void *ptr, int port)
{
  int slot, ep, ret = 0;
  USBAsync *p;

  switch (event) {
    // packet events start here
    case USB_EVENT_ASYNC:
      BX_DEBUG(("Experimental async packet completion"));
      p = container_of_usb_packet(ptr);
      p->done = 1;
      slot = (p->slot_ep >> 8);
      ep = (p->slot_ep & 0xff);
      if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams > 0) {   // specifying streams
        BX_DEBUG(("Event Handler: USB_EVENT_ASYNC: slot %d, ep %d, stream ID %d", slot, ep, p->packet.strm_pid));
        BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[p->packet.strm_pid].tr_dequeue_pointer =
          BX_XHCI_THIS process_transfer_ring(slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[p->packet.strm_pid].tr_dequeue_pointer, 
                                                      &BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[p->packet.strm_pid].dcs, p->packet.strm_pid);
      } else {
        BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer =
          BX_XHCI_THIS process_transfer_ring(slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer,
                                                      &BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs, 0);
      }
      break;
    case USB_EVENT_WAKEUP:
      if (BX_XHCI_THIS hub.usb_port[port].portsc.pls != PLS_U3_SUSPENDED) {
        break;
      }
      BX_XHCI_THIS hub.usb_port[port].portsc.pls = PLS_RESUME;
      if (!BX_XHCI_THIS hub.usb_port[port].portsc.plc) {
        BX_XHCI_THIS hub.usb_port[port].portsc.plc = 1;
        if (BX_XHCI_THIS hub.op_regs.HcStatus.hch) {
          break;
        }
        write_event_TRB(0, ((port + 1) << 24), TRB_SET_COMP_CODE(1), TRB_SET_TYPE(PORT_STATUS_CHANGE), 1);
      }
      break;
      
    // host controller events start here
    case USB_EVENT_CHECK_SPEED:
      // all super-speed device must be on the first half port register sets,
      //  while all non-super-speed device must be on the second half.
      if (ptr != NULL) {
        usb_device_c *usb_device = (usb_device_c *) ptr;
        if ((usb_device->get_speed() == USB_SPEED_SUPER) &&
            (BX_XHCI_THIS hub.usb_port[port].is_usb3 != 1)) {
          ret = 0; // given speed is not allowed on this port
          break;
        }
        if ((usb_device->get_speed() != USB_SPEED_SUPER) &&
            (BX_XHCI_THIS hub.usb_port[port].is_usb3 != 0)) {
          ret = 0; // given speed is not allowed on this port
          break;
        }
        ret = 1; // given speed is allowed on this port
      }
      break;
    default:
      BX_ERROR(("unknown/unsupported event (id=%d) on port #%d", event, port+1));
      ret = -1; // unknown event, event not handled
  }

  return ret;
}

// This function checks and processes all enqueued TRB's in the EP's transfer ring
Bit64u bx_usb_xhci_c::process_transfer_ring(const int slot, const int ep, Bit64u ring_addr, bool *rcs, const int primary_sid)
{
  struct TRB trb;
  Bit64u address = 0, org_addr;
  int int_target/*, td_size*/;
  Bit32u transfer_length;
  int ret = 0, len;
  int port_num = BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num;
  USBAsync *p;
  Bit8u cur_direction = (ep & 1) ? USB_TOKEN_IN : USB_TOKEN_OUT; // for NORMAL without SETUP
  bool is_transfer_trb, is_immed_data, ioc, spd_occurred = 0;
  bool first_event_trb_encountered = 0;
  Bit32u bytes_not_transferred = 0;
  int comp_code = 0;
  Bit8u immed_data[8];

  // this assumes that we are starting at the first of the TD when this function is called.
  // this is usually the case, and rarely isn't.
  int trb_count = 0;
  BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;
  BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry = 0;

  // if the ep is disabled, return an error event trb.
  if ((BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_DISABLED_ENABLED)
    || (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_DISABLED)) {
    org_addr = ring_addr;
    write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(EP_NOT_ENABLED),
      TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT), 1);
    return ring_addr;
  }

  // if the ep is in the halted or error state, ignore the doorbell ring.
  if ((BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_HALTED)
    || (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR))
    return ring_addr;

  // if the ep_context::type::direction field is not correct for the ep type of this ep, then ignore the doorbell
  if (ep >= 2) {
    static int endpoint_dir[8] = { -1, EP_DIR_OUT, EP_DIR_OUT, EP_DIR_OUT, -1, EP_DIR_IN, EP_DIR_IN, EP_DIR_IN };
    int ep_type = (ep & 1) ? EP_DIR_IN : EP_DIR_OUT;
    if (endpoint_dir[BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type] != ep_type) {
      BX_ERROR(("Endpoint_context::Endpoint_type::direction is not correct for this endpoint number.  Ignoring doorbell ring."));
      return ring_addr;
    }
  }

  // If an endpoint is in the Stopped state when the doorbell is rung, it will transition to the Running state (page 126)
  // The output Context (*slot_addr) should be updated before any other transfer events are made
  if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_STOPPED) {
    BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state = EP_STATE_RUNNING;
    update_ep_context(slot, ep);
  }

  // read in the TRB
  read_TRB((bx_phy_address) ring_addr, &trb);
  while ((trb.command & 1) == *rcs) {
    org_addr = ring_addr;
    BX_DEBUG(("Found TRB: address = 0x" FORMATADDRESS " 0x" FMT_ADDRX64 " 0x%08X 0x%08X  %d (SPD occurred = %d)",
      (bx_phy_address) org_addr, trb.parameter, trb.status, trb.command, *rcs, spd_occurred));
    trb_count++;
    // these are used in some/most items.
    // If not used, won't hurt to extract bad data.
    int_target = TRB_GET_TARGET(trb.status);
//  td_size = TRB_GET_TDSIZE(trb.status);
    transfer_length = TRB_GET_TX_LEN(trb.status);
    is_transfer_trb = 0;  // assume not a transfer
    ioc = TRB_IOC(trb.command);

    // if a SPD occurred, we only process the LINK and EVENT TRB's in this TD, until either one of these two or end of TD
    if (!spd_occurred ||
       (spd_occurred && ((TRB_GET_TYPE(trb.command) == LINK) || (TRB_GET_TYPE(trb.command) == EVENT_DATA)))) {
      // is the data in trb.parameter? (Immediate data?)
      is_immed_data = TRB_IS_IMMED_DATA(trb.command);
      if (is_immed_data)
        DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) org_addr, 8, immed_data); // No byte-swapping here
      else
        address = trb.parameter;

      switch (TRB_GET_TYPE(trb.command)) {
        // is a LINK trb.
        case LINK:
          if (ioc)
            write_event_TRB(int_target, org_addr, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_TYPE(LINK), 1);
          if (TRB_TOGGLE(trb.command))
            (*rcs) ^= 1;
          ring_addr = trb.parameter & (Bit64u) ~0xF;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d): LINK TRB:  New dq_pointer = 0x" FMT_ADDRX64 " (%d)",
            (bx_phy_address) org_addr, slot, ep, ring_addr, *rcs));
#if ((VERSION_MAJOR == 0) && (VERSION_MINOR == 0x95))
          // https://patchwork.kernel.org/patch/51191/
          if (!TRB_CHAIN(trb.command))
            BX_DEBUG(("Chain Bit in Link TRB not set."));
#endif
          read_TRB((bx_phy_address) ring_addr, &trb);
          continue;

        // Setup Stage TRB
        case SETUP_STAGE:
          cur_direction = USB_TOKEN_SETUP;
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d) (len = %d): Found SETUP TRB",
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Data Stage TRB
        case DATA_STAGE:
          cur_direction = TRB_GET_DIR(trb.command);
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d) (len = %d): Found DATA STAGE TRB",
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Status Stage TRB
        case STATUS_STAGE:
          cur_direction = TRB_GET_DIR(trb.command);
          is_transfer_trb = 1;
          transfer_length = 0;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d): Found STATUS STAGE TRB",
            (bx_phy_address) org_addr, slot, ep));
          break;

        // Normal TRB
        case NORMAL:
          is_transfer_trb = 1;
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d) (len = %d): Found NORMAL TRB",
            (bx_phy_address) org_addr, slot, ep, transfer_length));
          break;

        // Event TRB
        // xHCI version 1.10 (Nov 2017), page 184
        case EVENT_DATA:
          if (!spd_occurred || (spd_occurred && !first_event_trb_encountered)) {
            comp_code = (spd_occurred) ? SHORT_PACKET : TRB_SUCCESS;
            write_event_TRB(int_target, trb.parameter,
              TRB_SET_COMP_CODE(comp_code) | (BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla & 0x00FFFFFF),
              TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT) | (1<<2),
              ioc);
          }

// in version 1.00+, we need to check the PARSE_ALL_EVENT bit
// to see if we parse all of them
#if (((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x00)) && PARSE_ALL_EVENT)
          first_event_trb_encountered = 0;
#else
          if (spd_occurred)
            first_event_trb_encountered = 1;
#endif

          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d) (trnsfrd = %d): Found EVENT_DATA TRB: (returning %d)",
            (bx_phy_address) org_addr, slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla, comp_code));
          break;

        // No Op (transfer ring) TRB (Sect: 6.4.1.4)
        case NO_OP:
          BX_DEBUG(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d): Found No Op TRB",
            (bx_phy_address) org_addr, slot, ep));
          cur_direction = 0;
          is_transfer_trb = 1;
          transfer_length = 0;
          break;

        // unknown TRB type
        default:
          BX_ERROR(("0x" FORMATADDRESS ": Transfer Ring (slot = %d) (ep = %d): Unknown TRB found.",
            (bx_phy_address) org_addr, slot, ep));
          BX_ERROR(("Unknown trb type found: %d  (0x" FMT_ADDRX64 " 0x%08X 0x%08X)", TRB_GET_TYPE(trb.command),
            trb.parameter, trb.status, trb.command));
      }

      // is there a transfer to be done?
      if (is_transfer_trb) {
        p = find_async_packet(&BX_XHCI_THIS packets, ring_addr);
        bool completion = (p != NULL);
        if (completion && !p->done) {
          return ring_addr;
        }
        comp_code = TRB_SUCCESS;  // assume good trans event
        if (completion) {
          ret = p->packet.len;
          len = ret;
        } else {
          p = create_async_packet(&BX_XHCI_THIS packets, org_addr, transfer_length);
          p->packet.pid = cur_direction;
          p->packet.devaddr = BX_XHCI_THIS hub.slots[slot].slot_context.device_address;
          p->packet.devep = (ep >> 1);
          p->packet.speed = broadcast_speed(slot);
#if HANDLE_TOGGLE_CONTROL
          p->packet.toggle = -1; // the xHCI handles the data toggle bit for USB2 devices
#endif
          p->packet.complete_cb = xhci_event_handler;
          p->packet.complete_dev = BX_XHCI_THIS_PTR;
          p->packet.strm_pid = primary_sid;
          p->slot_ep = (slot << 8) | ep;
          switch (cur_direction) {
            case USB_TOKEN_OUT:
            case USB_TOKEN_SETUP:
              if (is_immed_data)
                memcpy(p->packet.data, immed_data, transfer_length);
              else if (transfer_length > 0)
                DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) address, transfer_length, p->packet.data);
              // The XHCI should block all SET_ADDRESS SETUP TOKEN's
              if ((cur_direction == USB_TOKEN_SETUP)   &&
                  (p->packet.data[0] == 0) &&  // Request type
                  (p->packet.data[1] == 5)) {  // SET_ADDRESS
                len = 0;
                comp_code = TRB_ERROR;
                BX_ERROR(("SETUP_TOKEN: System Software should not send SET_ADDRESS command on the xHCI."));
              } else {
                ret = BX_XHCI_THIS broadcast_packet(&p->packet, port_num - 1);
                len = transfer_length;
                BX_DEBUG(("OUT: Transferred %d bytes (ret = %d)", len, ret));
              }
              break;
            case USB_TOKEN_IN:
              ret = BX_XHCI_THIS broadcast_packet(&p->packet, port_num - 1);
              break;
          }
          if (ret == USB_RET_ASYNC) {
            BX_DEBUG(("Async packet deferred"));
            break;
          }
        }
        if (cur_direction == USB_TOKEN_IN) {
          if (ret >= 0) {
            len = ret;
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla += len;
            if (len > 0)
              DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) address, len, p->packet.data);
            BX_DEBUG(("IN: Transferred %d bytes, requested %d bytes", len, transfer_length));
            if (len < (int) transfer_length) {
              bytes_not_transferred = transfer_length - len;
              spd_occurred = 1;
            } else
              bytes_not_transferred = 0;
          } else {
            switch (ret) {
              case USB_RET_STALL:
                comp_code = STALL_ERROR;
                break;
              case USB_RET_BABBLE:
                comp_code = BABBLE_DETECTION;
                break;
              default:
                comp_code = TRANSACTION_ERROR;
            }
            len = 0;
          }
        }
        remove_async_packet(&BX_XHCI_THIS packets, p);

        if (ret == USB_RET_NAK) {
          // If we are a high-speed device, and a SETUP packet, we need to
          // return a comp_code of TRANSACTION_ERROR instead of breaking out.
          if ((cur_direction == USB_TOKEN_SETUP) &&
              (BX_XHCI_THIS hub.slots[slot].slot_context.speed == XHCI_SPEED_HIGH))
            comp_code = TRANSACTION_ERROR;
          else {
            // if the device NAK'ed, we retry with given interval
            BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry = 1;
            int interval = 125 * (1 << BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.interval);
            if (interval < 1000) {
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry_counter = 1;
            } else {
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry_counter = interval / 1000;
            }
            break;
          }
        }

        // 4.10.1 paragraph 4
        // 4.10.1.1
        if (ioc) {
          if ((comp_code == TRB_SUCCESS) && spd_occurred && TRB_SPD(trb.command)) {
            comp_code = SHORT_PACKET;
            BX_DEBUG(("Sending Short Packet Detect Event TRB (%d)", bytes_not_transferred));
          }
          // create a Event TRB
          write_event_TRB(int_target, org_addr, TRB_SET_COMP_CODE(comp_code) | bytes_not_transferred,
            TRB_SET_SLOT(slot) | TRB_SET_EP(ep) | TRB_SET_TYPE(TRANS_EVENT), 1);
        }
      }
    }

    // if the Chain bit is clear, then end of TD
    if ((trb.command & (1<<4)) == 0) {
      spd_occurred = 0;
      BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;
    }

    // advance the Dequeue pointer and continue;
    ring_addr += 16;
    read_TRB((bx_phy_address) ring_addr, &trb);
  }

  BX_DEBUG(("Process Transfer Ring: Processed %d TRB's", trb_count));
  if (trb_count == 0)
    BX_ERROR(("Process Transfer Ring: Doorbell rang, but no TRB's were enqueued in the ring."));

  // return the new address
  return ring_addr;
}

// This function call starts at the current position in the Command Ring,
//  processes that command, then moves to the next one, until CCS != SCS;
//    BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = current position in ring
//    BX_XHCI_THIS hub.op_regs.HcCrcr.rcs  = current ring cycle state
//    BX_XHCI_THIS hub.op_regs.HcCrcr.crr  = command ring running
void bx_usb_xhci_c::process_command_ring(void)
{
  struct TRB trb;
  unsigned i, j;
  int slot, ep, comp_code = 0, new_addr = 0, bsr = 0, trb_command;
  Bit32u a_flags, d_flags;
  Bit64u org_addr, temp_addr;
  bool temp_dcs;
  Bit8u buffer[CONTEXT_SIZE + (32 * CONTEXT_SIZE)];
  struct SLOT_CONTEXT slot_context;
  struct EP_CONTEXT   ep_context;

  if (!BX_XHCI_THIS hub.op_regs.HcCrcr.crr)
    return;

  // read in the TRB
  read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
  BX_DEBUG(("Dump command trb: %d  (0x" FMT_ADDRX64 " 0x%08X 0x%08X) (%d)", TRB_GET_TYPE(trb.command),
    trb.parameter, trb.status, trb.command, BX_XHCI_THIS hub.ring_members.command_ring.rcs));
  while ((trb.command & 1) == BX_XHCI_THIS hub.ring_members.command_ring.rcs) {
    org_addr = BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer;
    trb_command = TRB_GET_TYPE(trb.command);
    switch (trb_command) {
      // is a LINK trb.
      case LINK:
        // Chain bit and Interrupter Target fields are ignored in Command Rings (Page 370)
        BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer = trb.parameter & (Bit64u) ~0xF;
        if (TRB_IOC(trb.command))
          write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_SLOT(0) | TRB_SET_TYPE(LINK), 1);
        if (TRB_TOGGLE(trb.command))
          BX_XHCI_THIS hub.ring_members.command_ring.rcs ^= 1;
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found LINK TRB:  New dq_pointer = 0x" FORMATADDRESS " (%d)",
          (bx_phy_address) org_addr, (bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer,
          BX_XHCI_THIS hub.ring_members.command_ring.rcs));
        read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
        continue;

      // NEC: Get Firmware version
      case NEC_TRB_TYPE_GET_FW:
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(1) | 0x3021, TRB_SET_SLOT(0) | TRB_SET_TYPE(NEC_TRB_TYPE_CMD_COMP), 1);
        BX_DEBUG(("NEC GET Firmware Version TRB found.  Returning 0x3021"));
        break;

      /* NEC: Get Verification
       * The NEC/Renesas driver sends a vendor specific TRB to the controller to
       *  verify that the controller is indeed a NEC/Renesas controller.
       * A 64-bit value is sent in the TRB.Param field and a 32-bit value is returned.
       *  (A 16-bit value in the high-order of TRB.Command and a 16-bit value in
       *   the low-order of TRB.Status)
       * I do not have permission from NEC/Renesas to show the code that calculates and
       *  returns the correct verification values.  Therefore, this simply returns.  
       * As long as you are not using a compatible NEC/Renesas driver, everything 
       *  should be fine.
       */
      case NEC_TRB_TYPE_GET_UN:
        BX_DEBUG(("NEC GET Verification TRB found."));
        break;
        
      /* Bochs Dump Controller command:
       * This command simply dumps the controller's contents to the debug file.
       * This is useful for displaying the controller before and then after a given command.
       * This uses the 8-byte Parameter member of the TRB to pass arguments:
       *   bits 4:0 - zero based highest slot number to dump (1,2,3,...31)
       *   bits 8:5 - zero based highest endpoint to dump (1,2,3,...15)
       * This command sends a Command Completion on the Event Ring.
       */
      case BX_TRB_TYPE_DUMP:
        BX_INFO(("BX_TRB_TYPE_DUMP TRB found.  Dumping the core."));
        dump_xhci_core((trb.parameter >> 0) & 0x1F, (trb.parameter >> 5) & 0xF);
        write_event_TRB(0, 0x0, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_SLOT(0) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        break;

      case ENABLE_SLOT:
        comp_code = NO_SLOTS_ERROR;  // assume no slots
        slot = 0;
        for (i=1; i<MAX_SLOTS; i++) {  // slots are one based
          if (BX_XHCI_THIS hub.slots[i].enabled == 0) {
            memset(&BX_XHCI_THIS hub.slots[i], 0, sizeof(struct HC_SLOT_CONTEXT));
            BX_XHCI_THIS hub.slots[i].slot_context.slot_state = SLOT_STATE_DISABLED_ENABLED;
            BX_XHCI_THIS hub.slots[i].enabled = 1;
            slot = i;
            for (j=1; j<32; j++)
              BX_XHCI_THIS hub.slots[i].ep_context[j].ep_context.ep_state = EP_STATE_DISABLED;
            comp_code = TRB_SUCCESS;
            break;
          }
        }
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Enable Slot TRB (slot = %d) (returning %d)",
          (bx_phy_address) org_addr, slot, comp_code));
        break;

      case DISABLE_SLOT:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        if (BX_XHCI_THIS hub.slots[slot].enabled) {
          BX_XHCI_THIS hub.slots[slot].enabled = 0;
          BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = 0; // disabled
          update_slot_context(slot);
          for (i=1; i<32; i++) {
            BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
            update_ep_context(slot, i);
          }
          comp_code = TRB_SUCCESS;
        } else
          comp_code = SLOT_NOT_ENABLED;

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Disable Slot TRB (slot = %d) (returning %d)",
          (bx_phy_address) org_addr, slot, comp_code));
        break;

      case ADDRESS_DEVICE:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
          get_dwords((bx_phy_address) trb.parameter, (Bit32u *) buffer, (CONTEXT_SIZE + (CONTEXT_SIZE * 2)) >> 2);
          bsr = ((trb.command & (1<<9)) == (1<<9));
          d_flags = * (Bit32u *) &buffer[0];
          a_flags = * (Bit32u *) &buffer[4];
          if (((d_flags & 0x03) == 0) && ((a_flags & 0x03) == 3)) {
            if ((d_flags & ~0x03) || (a_flags & ~0x03)) {
              BX_ERROR(("D2->D31 and A2->A31 must be zero..."));
            }
            // Use temporary slot and ep context incase there is an error we don't modify the main contexts
            copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
            copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + CONTEXT_SIZE]);
            // check that the Input contexts are valid
            if ((validate_slot_context(&slot_context, trb_command, slot) == TRB_SUCCESS) && 
                (validate_ep_context(&ep_context, trb_command, 0, slot_context.rh_port_num - 1, 1) == TRB_SUCCESS)) {
              if (bsr == 1) { // BSR flag set
                if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_DISABLED_ENABLED) {
                  slot_context.slot_state = SLOT_STATE_DEFAULT;
                  slot_context.device_address = 0;
                  ep_context.ep_state = EP_STATE_RUNNING;
                  comp_code = TRB_SUCCESS;
                } else
                  comp_code = CONTEXT_STATE_ERROR;
              } else { // BSR flag is clear
                if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state <= SLOT_STATE_DEFAULT) {
                  int port_num = slot_context.rh_port_num - 1;  // slot:port_num is 1 based
                  new_addr = create_unique_address(slot);
                  if (send_set_address(new_addr, port_num, slot) == 0) {
                    slot_context.slot_state = SLOT_STATE_ADDRESSED;
                    slot_context.device_address = new_addr;
                    ep_context.ep_state = EP_STATE_RUNNING;
                    comp_code = TRB_SUCCESS;
                  } else
                    comp_code = TRANSACTION_ERROR;
                } else
                  comp_code = CONTEXT_STATE_ERROR;
              }
            } else // validate contexts
              comp_code = CONTEXT_STATE_ERROR;
          } else
            comp_code = CONTEXT_STATE_ERROR;  // A0 and A1 not set correctly
        } else
          comp_code = SLOT_NOT_ENABLED;

        // if successful, copy to the buffer allocated for this slot
        if (comp_code == TRB_SUCCESS) {
          memcpy(&BX_XHCI_THIS hub.slots[slot].slot_context, &slot_context, sizeof(struct SLOT_CONTEXT));
          memcpy(&BX_XHCI_THIS hub.slots[slot].ep_context[1].ep_context, &ep_context, sizeof(struct EP_CONTEXT));
          // initialize our internal enqueue pointer
          BX_XHCI_THIS hub.slots[slot].ep_context[1].enqueue_pointer = ep_context.tr_dequeue_pointer;
          BX_XHCI_THIS hub.slots[slot].ep_context[1].rcs = ep_context.dcs;
          update_slot_context(slot);
          update_ep_context(slot, 1);
        }

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        //BX_INFO(("ADDRESS_DEVICE TRB: 0x" FMT_ADDRX64 "  0x%08X 0x%08X", trb.parameter, trb.status, trb.command));
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: SetAddress TRB (bsr = %d) (addr = %d) (slot = %d) (returning %d)",
          (bx_phy_address) org_addr, bsr, new_addr, slot, comp_code));
        break;

      case EVALUATE_CONTEXT:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
          get_dwords((bx_phy_address) trb.parameter, (Bit32u *) buffer, (CONTEXT_SIZE + (CONTEXT_SIZE * 32)) >> 2);
          a_flags = * (Bit32u *) &buffer[4];
          // only the Slot context and EP0 (control EP) contexts are evaluated. Section 6.2.3.3, p326
          // If the slot is not addressed or configured, then return error
          // XHCI specs 1.0, page 102 says DEFAULT or higher, while page 321 states higher than DEFAULT!!!
          //  (specs 1.1 removed the DEFAULT word and require it to be greater than the DEFAULT state)
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
          if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state > SLOT_STATE_DEFAULT) {
#else
          if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state >= SLOT_STATE_DEFAULT) {
#endif
            comp_code = TRB_SUCCESS;  // assume good completion
            if (a_flags & (1<<0)) {
              copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
              comp_code = validate_slot_context(&slot_context, trb_command, slot);
            }
            if (comp_code == TRB_SUCCESS) {
              if (a_flags & (1<<1)) {
                copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + CONTEXT_SIZE]);
                comp_code = validate_ep_context(&ep_context, trb_command, a_flags, BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num - 1, 1);
              }
            }
          } else {
            comp_code = CONTEXT_STATE_ERROR;
            BX_DEBUG(("Evaluate Context with an illegal slot state"));
          }

          // if all were good, go ahead and update our contexts
          if (comp_code == TRB_SUCCESS) {
            if (a_flags & (1<<0)) { // slot context
              // See section 6.2.2.3 for what fields will be updated
              copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);
              // only the Interrupter Target and Max Exit Latency are updated by this command
              BX_XHCI_THIS hub.slots[slot].slot_context.int_target = slot_context.int_target;
              BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency = slot_context.max_exit_latency;
              update_slot_context(slot); // update the DCBAAP slot
            }
            if (a_flags & (1<<1)) { // control ep context
              // See section 6.2.3.3 for what fields will be updated
              copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + CONTEXT_SIZE]);
              BX_XHCI_THIS hub.slots[slot].ep_context[1].ep_context.max_packet_size = ep_context.max_packet_size;
              update_ep_context(slot, 1);
            }
          }
        } else
          comp_code = SLOT_NOT_ENABLED;

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Evaluate TRB (slot = %d) (d_flags = 0x%08X) (a_flags = 0x%08X) (returning %d)",
          (bx_phy_address) org_addr, slot, d_flags, a_flags, comp_code));
        
        break;

      case CONFIG_EP: {
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        bool dc = TRB_DC(trb.command);
        if (BX_XHCI_THIS hub.slots[slot].enabled) {
          get_dwords((bx_phy_address) trb.parameter, (Bit32u *) buffer, (CONTEXT_SIZE + (CONTEXT_SIZE * 32)) >> 2);
          d_flags = * (Bit32u *) &buffer[0];
          a_flags = * (Bit32u *) &buffer[4];
          copy_slot_from_buffer(&slot_context, &buffer[CONTEXT_SIZE]);  // so we get entry_count

          for (i=2; i<32; i++) {
            if (dc || (d_flags & (1<<i))) {
              BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
              // TODO: Bandwidth stuff
              update_ep_context(slot, i);
            }
          }
          if (dc) {
            BX_XHCI_THIS hub.slots[slot].slot_context.entries = 1;
            BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_ADDRESSED;
            update_slot_context(slot);
          } else
            
          if (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state >= SLOT_STATE_ADDRESSED) {
            comp_code = TRB_SUCCESS;  // assume good completion
            if ((d_flags & 3) > 0)    // the D0 (slot) and D1 (ep0) bits must be cleared
              comp_code = PARAMETER_ERROR;
            if ((a_flags & 3) == 1) { // the A0 (slot) bit must be set, and A1 (ep0) must be cleared
              comp_code = validate_slot_context(&slot_context, trb_command, slot);
            } else {
              comp_code = PARAMETER_ERROR;
            }
            
            // Check all the input context entries with an a_flag == 1
            for (i=2; i<32; i++) {
              if (a_flags & (1<<i)) {
                copy_ep_from_buffer(&ep_context, &buffer[CONTEXT_SIZE + (CONTEXT_SIZE * i)]);
                if (i > (int) slot_context.entries) {
                  comp_code = PARAMETER_ERROR;
                  break;  // no need to check the rest
                }
                comp_code = validate_ep_context(&ep_context, trb_command, a_flags, BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num - 1, i);
                if (comp_code != TRB_SUCCESS)
                  break;  // no need to check the rest
              }
            }
            
            // if all were good, go ahead and update our contexts
            if (comp_code == TRB_SUCCESS) {
              // first update the slot context
              // See section 6.2.2.2 for what fields will be updated
              BX_XHCI_THIS hub.slots[slot].slot_context.entries = slot_context.entries;
              BX_XHCI_THIS hub.slots[slot].slot_context.hub = slot_context.hub;
              BX_XHCI_THIS hub.slots[slot].slot_context.ttt = slot_context.ttt;
              BX_XHCI_THIS hub.slots[slot].slot_context.mtt = slot_context.mtt;
              BX_XHCI_THIS hub.slots[slot].slot_context.num_ports = slot_context.num_ports;
              
              // now the endpoints
              for (i=2; i<32; i++) {
                if (d_flags & (1<<i)) {
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
                  // TODO: Bandwidth stuff
                  update_ep_context(slot, i);
                }
                if (a_flags & (1<<i)) {
                  // xhci 1.0: section 4.6.6, page 97
                  // All fields of the Input Endpoint Context data structure in the Configure Endpoint Context
                  //  are copied to the Output Endpoint Context fields in the Device Context.
                  copy_ep_from_buffer(&BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context, &buffer[CONTEXT_SIZE + (i * CONTEXT_SIZE)]);
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_RUNNING;
                  // initialize our internal enqueue pointer
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].enqueue_pointer =
                    BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.tr_dequeue_pointer;
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].rcs = BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.dcs;
                  BX_XHCI_THIS hub.slots[slot].ep_context[i].edtla = 0;
                  // once the ep is configured, the Stream Context belongs to us
                  if (BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_pstreams > 0) {
                    for (j=1; j<PSA_MAX_SIZE_NUM(BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.max_pstreams) && (j < MAX_PSA_SIZE_NUM); j++) {
                      get_stream_info(&BX_XHCI_THIS hub.slots[slot].ep_context[i].stream[j],
                                       BX_XHCI_THIS hub.slots[slot].ep_context[i].enqueue_pointer, j);
                    }
                  }
                  // TODO: Bandwidth stuff
                  update_ep_context(slot, i);
                }
              }

              // if all EP's are in the disabled state, then set to slot_state = ADDRESSED, else slot_state = CONFIGURED
              BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_ADDRESSED;
              for (i=2; i<32; i++) {
                if (BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state == EP_STATE_RUNNING) {
                  BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_CONFIGURED;
                  break;
                }
              }
              
              // now update the slot
              update_slot_context(slot);
            }
          } else
            comp_code = CONTEXT_STATE_ERROR;
        } else
          comp_code = SLOT_NOT_ENABLED;

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Config_EP TRB (slot = %d) (dc = %d) (d_flags = 0x%08X) (a_flags = 0x%08X) (returning %d)",
          (bx_phy_address) org_addr, slot, dc, d_flags, a_flags, comp_code));
      }
      break;

      case SET_TR_DEQUEUE:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        ep = TRB_GET_EP(trb.command);
        if (BX_XHCI_THIS hub.slots[slot].enabled == 1) {
          if ((BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_STOPPED) ||
              (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR)) {
            // is this endpoint using streams?
            if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams > 0) {   // specifying streams
              unsigned stream_id = TRB_GET_STREAM(trb.status);
              if ((stream_id > 0) && (stream_id < MAX_PSA_SIZE_NUM) &&
                  (stream_id < PSA_MAX_SIZE_NUM(BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams)) &&
                   BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[stream_id].valid == 1) {
                BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[stream_id].tr_dequeue_pointer =
                  temp_addr = (trb.parameter & (Bit64u) ~0xF);
                BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[stream_id].dcs =
                  temp_dcs = (trb.parameter & 1) > 0;
                update_ep_context(slot, ep);
                comp_code = TRB_SUCCESS;
              } else {
                comp_code = INVALID_STREAM_ID;
              }
            } else {
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer =
                BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer = 
                temp_addr = (trb.parameter & (Bit64u) ~0xF);
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs =
                BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs = 
                temp_dcs = (trb.parameter & 1) > 0;
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].edtla = 0;
              update_ep_context(slot, ep);
              comp_code = TRB_SUCCESS;
            }
          } else
            comp_code = CONTEXT_STATE_ERROR;
        } else
          comp_code = SLOT_NOT_ENABLED;

        // if the state is in error, move it to stopped
        if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR)
          BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state = EP_STATE_STOPPED;

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Set_tr_Dequeue TRB (slot = %d) (ep = %d) (returning %d)",
          (bx_phy_address) org_addr, slot, ep, comp_code));
        if (comp_code == TRB_SUCCESS)
          BX_DEBUG(("  New address: 0x" FORMATADDRESS " state = %d", temp_addr, temp_dcs));
        break;

//      case RESET_EP:
          // May only be sent to EP's in the HALTED state. (page 105)
//        BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_STOPPED;
//        break;

      case STOP_EP:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        ep = TRB_GET_EP(trb.command);
        // A Stop Endpoint Command received while an endpoint is in the Error state shall have no effect and shall
        // generate a Command Completion Event with the Completion Code set to Context State Error.
        if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state == EP_STATE_ERROR)
          comp_code = CONTEXT_STATE_ERROR;
        else {
          BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state = EP_STATE_STOPPED;
          update_ep_context(slot, ep);
          comp_code = TRB_SUCCESS;
        }

        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Stop EP TRB (slot = %d) (ep = %d) (sp = %d) (returning 1)",
          (bx_phy_address) org_addr, slot, ep, ((trb.command & (1<<23)) == (1<<23))));
        break;

      // Get Port Bandwidth (manditory on versions 0.95, 0.96, and 1.00. Optional on later versions)
      case GET_PORT_BAND:
        {
          unsigned hub_id = TRB_GET_SLOT(trb.command);
          unsigned band_speed = ((trb.command & (0x0F << 16)) >> 16) - 1;
          unsigned offset = band_speed * (((1 + BX_XHCI_THIS hub.n_ports) + 7) & ~7);
          if (hub_id == 0) { // root hub
            if (band_speed < 4) {
              DEV_MEM_WRITE_PHYSICAL_DMA((bx_phy_address) trb.parameter, 1 + BX_XHCI_THIS hub.n_ports, &BX_XHCI_THIS hub.port_band_width[offset]);
              comp_code = TRB_SUCCESS;
            } else {
              comp_code = TRB_ERROR;
              BX_ERROR(("Get Port Bandwidth with unknown speed of %d", band_speed + 1));
            }
          } else {
#if SEC_DOMAIN_BAND
            // TODO: External HUB support
            comp_code = TRB_ERROR;
            BX_ERROR(("Get Secondary Port Bandwidth not implemented yet."));
#else
            write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(TRB_ERROR), TRB_SET_SLOT(0) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
#endif
          }

          write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(0) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
          BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: GetPortBandwidth TRB (speed = %d) (hub_id = %d) (returning %d)",
            (bx_phy_address) org_addr, band_speed, hub_id, comp_code));
        }
        break;

      case RESET_DEVICE:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        if ((BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_ADDRESSED) ||
            (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state == SLOT_STATE_CONFIGURED)) {
          BX_XHCI_THIS hub.slots[slot].slot_context.slot_state = SLOT_STATE_DEFAULT;
          BX_XHCI_THIS hub.slots[slot].slot_context.entries = 1;
          BX_XHCI_THIS hub.slots[slot].slot_context.device_address = 0;
          update_slot_context(slot);
          for (i=2; i<32; i++) {
            BX_XHCI_THIS hub.slots[slot].ep_context[i].ep_context.ep_state = EP_STATE_DISABLED;
            update_ep_context(slot, i);
          }
          comp_code = TRB_SUCCESS;
        } else {
          comp_code = CONTEXT_STATE_ERROR;
        }
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(comp_code), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring: Found Reset Device TRB (slot = %d) (returning %d)",
          (bx_phy_address) org_addr, slot, comp_code));
        break;

      // No Op (command ring) TRB (Sect: 6.4.3.1)
      case NO_OP_CMD:
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        ep = TRB_GET_EP(trb.command);
        BX_DEBUG(("0x" FORMATADDRESS ": Command Ring (slot = %d) (ep = %d): Found No Op TRB",
          (bx_phy_address) org_addr, slot, ep));
        write_event_TRB(0, org_addr, TRB_SET_COMP_CODE(TRB_SUCCESS), TRB_SET_SLOT(0) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
        break;

      // unknown TRB type
      default:
        BX_ERROR(("0x" FORMATADDRESS ": Command Ring: Unknown TRB found.", (bx_phy_address) org_addr));
        BX_ERROR(("Unknown trb type found: %d  (0x" FMT_ADDRX64 " 0x%08X 0x%08X)", TRB_GET_TYPE(trb.command),
          trb.parameter, trb.status, trb.command));
        slot = TRB_GET_SLOT(trb.command);  // slots are 1 based
        write_event_TRB(0, 0x0, TRB_SET_COMP_CODE(TRB_ERROR), TRB_SET_SLOT(slot) | TRB_SET_TYPE(COMMAND_COMPLETION), 1);
    }

    // advance the Dequeue pointer and continue;
    BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer += 16;
    read_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.command_ring.dq_pointer, &trb);
  }
}

void bx_usb_xhci_c::init_event_ring(const unsigned interrupter)
{
  const Bit64u addr = BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstba.erstabadd;
  Bit32u val32;
  Bit64u val64;

  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].rcs = 1;
  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count = 0;
  DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) addr, sizeof(BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys),
    (Bit8u *) BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys);
  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb =
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys[0].addr;
  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].trb_count =
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys[0].size;

  // dump the event segment table
  BX_DEBUG(("Interrupter %02i: Event Ring Table (at 0x" FMT_ADDRX64 ") has %d entries:", interrupter,
    addr, BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstsz.erstabsize));
  for (int i=0; i<BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstsz.erstabsize; i++) {
    DEV_MEM_READ_PHYSICAL((bx_phy_address) addr + (i * 16),     8, (Bit8u*)&val64);
    DEV_MEM_READ_PHYSICAL((bx_phy_address) addr + (i * 16) + 8, 4, (Bit8u*)&val32);
    BX_DEBUG((" %02i:  address = 0x" FMT_ADDRX64 "  Count = %d", i, val64, val32 & 0x0000FFFF));
  }
}

void bx_usb_xhci_c::write_event_TRB(const unsigned interrupter, const Bit64u parameter, const Bit32u status,
                                    const Bit32u command, const bool fire_int)
{
  // write the TRB
  write_TRB((bx_phy_address) BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb, parameter, status,
    command | (Bit32u) BX_XHCI_THIS hub.ring_members.event_rings[interrupter].rcs); // set the cycle bit
  
  BX_DEBUG(("Write Event TRB: table index: %d, trb index: %d",
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count,
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys[BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count].size - 
      BX_XHCI_THIS hub.ring_members.event_rings[interrupter].trb_count));
  BX_DEBUG(("Write Event TRB: address = 0x" FORMATADDRESS " 0x" FMT_ADDRX64 " 0x%08X 0x%08X  (type = %d)",
    (bx_phy_address) BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb,
    parameter, status, command, TRB_GET_TYPE(command)));

  // calculate position for next event TRB
  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb += 16;
  BX_XHCI_THIS hub.ring_members.event_rings[interrupter].trb_count--;

  if (BX_XHCI_THIS hub.ring_members.event_rings[interrupter].trb_count == 0) {
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count++;
    if (BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count ==
        BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erstsz.erstabsize) {
      BX_XHCI_THIS hub.ring_members.event_rings[interrupter].rcs ^= 1;
      BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count = 0;
    }
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].cur_trb =
      BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys[BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count].addr;
    BX_XHCI_THIS hub.ring_members.event_rings[interrupter].trb_count =
      BX_XHCI_THIS hub.ring_members.event_rings[interrupter].entrys[BX_XHCI_THIS hub.ring_members.event_rings[interrupter].count].size;
  }

  // if caller wants us to fire and interrupt, do so
  if (fire_int) {
    BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].iman.ip = 1;
    BX_XHCI_THIS hub.runtime_regs.interrupter[interrupter].erdp.ehb = 1; // set event handler busy
    BX_XHCI_THIS hub.op_regs.HcStatus.eint = 1;
    update_irq(interrupter);
  }
}

void bx_usb_xhci_c::read_TRB(bx_phy_address addr, struct TRB *trb)
{
  DEV_MEM_READ_PHYSICAL(addr,      8, (Bit8u*)&trb->parameter);
  DEV_MEM_READ_PHYSICAL(addr +  8, 4, (Bit8u*)&trb->status);
  DEV_MEM_READ_PHYSICAL(addr + 12, 4, (Bit8u*)&trb->command);
}

void bx_usb_xhci_c::write_TRB(bx_phy_address addr, const Bit64u parameter, const Bit32u status, const Bit32u command)
{
  DEV_MEM_WRITE_PHYSICAL(addr     , 8, (Bit8u*)&parameter);
  DEV_MEM_WRITE_PHYSICAL(addr +  8, 4, (Bit8u*)&status);
  DEV_MEM_WRITE_PHYSICAL(addr + 12, 4, (Bit8u*)&command);
}

void bx_usb_xhci_c::update_slot_context(const int slot)
{
  Bit32u buffer[16];

  memset(buffer, 0, 16 * sizeof(Bit32u));
  copy_slot_to_buffer(buffer, slot);
  Bit64u slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (slot * sizeof(Bit64u)));
  DEV_MEM_READ_PHYSICAL((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  put_dwords((bx_phy_address) slot_addr, buffer, CONTEXT_SIZE >> 2);
}

void bx_usb_xhci_c::update_ep_context(const int slot, const int ep)
{
  Bit32u buffer[16];
  unsigned i;

  memset(buffer, 0, 16 * sizeof(Bit32u));
  copy_ep_to_buffer(buffer, slot, ep);
  Bit64u slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (slot * sizeof(Bit64u)));
  DEV_MEM_READ_PHYSICAL((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  put_dwords((bx_phy_address) (slot_addr + (ep * CONTEXT_SIZE)), buffer, CONTEXT_SIZE >> 2);

#if MAX_PSA_SIZE > 0
  // do we need to update the stream context?
  if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams > 0) {
    for (i=1; i<PSA_MAX_SIZE_NUM(BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams) && (i < MAX_PSA_SIZE_NUM); i++) {
      if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer != 0) {
        put_stream_info(&BX_XHCI_THIS hub.slots[slot].ep_context[ep].stream[i],
                         BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer, i);
      }
    }
  }
#endif
}

void bx_usb_xhci_c::dump_slot_context(const Bit32u *context, const int slot)
{
  BX_INFO((" -=-=-=-=-=-=-=- Slot Context -=-=-=-=-=-=-=-"));
  BX_INFO((" Context Entries: %d (%d)", (context[0] & (0x1F<<27)) >> 27, BX_XHCI_THIS hub.slots[slot].slot_context.entries));
  BX_INFO(("             Hub: %d (%d)", (context[0] & (1   <<26)) >> 26, BX_XHCI_THIS hub.slots[slot].slot_context.hub));
  BX_INFO(("             MTT: %d (%d)", (context[0] & (1   <<25)) >> 25, BX_XHCI_THIS hub.slots[slot].slot_context.mtt));
  BX_INFO(("       ReservedZ: %02X",    (context[0] & (1   <<24)) >> 24));
  BX_INFO(("           Speed: %d (%d)", (context[0] & (0x0F<<20)) >> 20, BX_XHCI_THIS hub.slots[slot].slot_context.speed));
  BX_INFO(("    Route String: %05X (%05X)", (context[0] & 0xFFFFF)    >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.route_string));
  BX_INFO(("       Num Ports: %d (%d)", (context[1] & (0xFF<<24)) >> 24, BX_XHCI_THIS hub.slots[slot].slot_context.num_ports));
  BX_INFO(("     RH Port Num: %d (%d)", (context[1] & (0xFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num));
  BX_INFO(("Max Exit Latency: %d (%d)", (context[1] & 0xFFFF)     >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency));
  BX_INFO(("      Int Target: %d (%d)", (context[2] & (0x3F<<22)) >> 22, BX_XHCI_THIS hub.slots[slot].slot_context.int_target));
  BX_INFO(("       ReservedZ: %02X",    (context[2] & (0x0F<<18)) >> 18));
  BX_INFO(("             TTT: %d (%d)", (context[2] & (0x03<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].slot_context.ttt));
  BX_INFO(("     TT Port Num: %d (%d)", (context[2] & (0xFF<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].slot_context.tt_port_num));
  BX_INFO(("     TT Hub Slot: %d (%d)", (context[2] & 0xFF)       >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.tt_hub_slot_id));
  BX_INFO(("      Slot State: %d (%d)", (context[3] & (0x1F<<27)) >> 27, BX_XHCI_THIS hub.slots[slot].slot_context.slot_state));
  BX_INFO(("       ReservedZ: %06X",    (context[3] & (0x7FFFF<<8)) >> 8));
  BX_INFO(("     Dev Address: %d (%d)", (context[3] & 0xFF)       >>  0, BX_XHCI_THIS hub.slots[slot].slot_context.device_address));
  BX_INFO(("       ReservedZ: %08X",     context[4]));
  BX_INFO(("       ReservedZ: %08X",     context[5]));
  BX_INFO(("       ReservedZ: %08X",     context[6]));
  BX_INFO(("       ReservedZ: %08X",     context[7]));
#if (CONTEXT_SIZE == 64)
  BX_INFO(("       ReservedZ: %08x",  context[8]));
  BX_INFO(("       ReservedZ: %08x",  context[9]));
  BX_INFO(("       ReservedZ: %08x",  context[10]));
  BX_INFO(("       ReservedZ: %08x",  context[11]));
  BX_INFO(("       ReservedZ: %08x",  context[12]));
  BX_INFO(("       ReservedZ: %08x",  context[13]));
  BX_INFO(("       ReservedZ: %08x",  context[14]));
  BX_INFO(("       ReservedZ: %08x",  context[15]));
#endif
}

void bx_usb_xhci_c::dump_ep_context(const Bit32u *context, const int slot, const int ep)
{
  BX_INFO((" -=-=-=-=-=-=-=-=- EP Context -=-=-=-=-=-=-=-"));
  BX_INFO(("       ReservedZ: %02x",    (context[0] & (0xFF<<24)) >> 24));
  BX_INFO(("        Interval: %d (%d)", (context[0] & (0x0F<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.interval));
  BX_INFO(("             LSA: %d (%d)", (context[0] & (1   <<15)) >> 15, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.lsa));
  BX_INFO(("     MaxPStreams: %d (%d)", (context[0] & (0x1F<<10)) >> 10, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams));
  BX_INFO(("            Mult: %d (%d)", (context[0] & (0x03<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.mult));
  BX_INFO(("       ReservedZ: %02x",    (context[0] & (0x1F<< 3)) >>  3));
  BX_INFO(("        EP State: %d (%d)", (context[0] & 0x7)        >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state));
  BX_INFO((" Max Packet Size: %d (%d)", (context[1] & (0xFFFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_packet_size));
  BX_INFO(("  Max Burst Size: %d (%d)", (context[1] & (0xFF<< 8)) >>  8, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_burst_size));
  BX_INFO(("             HID: %d (%d)", (context[1] & (1   << 7)) >>  7, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.hid));
  BX_INFO(("       ReservedZ: %01x",    (context[1] & (1   << 6)) >>  6));
  BX_INFO(("         EP Type: %d (%d)", (context[1] & (0x07<< 3)) >>  3, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type));
  BX_INFO(("            CErr: %d (%d)", (context[1] & (0x03<< 1)) >>  1, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.cerr));
  BX_INFO(("       ReservedZ: %01x",    (context[1] & (1   << 0)) >>  0));
  BX_INFO(("  TR Dequeue Ptr: " FMT_ADDRX64 " (" FMT_ADDRX64 ")", (* (Bit64u *) &context[2] & (Bit64u) ~0x0F), BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer));
  BX_INFO(("       ReservedZ: %01x",    (context[2] & (0x07  << 1)) >>  1));
  BX_INFO(("             DCS: %d (%d)", (context[2] & (1     << 0)) >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs));
  BX_INFO(("Avg ESIT Payload: %d (%d)", (context[4] & (0xFFFF<<16)) >> 16, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_esit_payload));
  BX_INFO(("  Avg TRB Length: %d (%d)", (context[4] & (0xFFFF<< 0)) >>  0, BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.average_trb_len));
  BX_INFO(("       ReservedZ: %08x",     context[5]));
  BX_INFO(("       ReservedZ: %08x",     context[6]));
  BX_INFO(("       ReservedZ: %08x",     context[7]));
#if (CONTEXT_SIZE == 64)
  BX_INFO(("       ReservedZ: %08x",  context[8]));
  BX_INFO(("       ReservedZ: %08x",  context[9]));
  BX_INFO(("       ReservedZ: %08x",  context[10]));
  BX_INFO(("       ReservedZ: %08x",  context[11]));
  BX_INFO(("       ReservedZ: %08x",  context[12]));
  BX_INFO(("       ReservedZ: %08x",  context[13]));
  BX_INFO(("       ReservedZ: %08x",  context[14]));
  BX_INFO(("       ReservedZ: %08x",  context[15]));
#endif
}

void bx_usb_xhci_c::dump_stream_context(const Bit32u *context)
{
  BX_INFO((" -=-=-=-=-=-=-=- Stream Context -=-=-=-=-=-=-"));
  BX_INFO(("  TR Dequeue Ptr: " FMT_ADDRX64 " (" FMT_ADDRX64 ")", (* (Bit64u *) &context[0] & (Bit64u) ~0x0F)));
  BX_INFO(("    Content Type: %01x",    (context[0] & (0x07  << 1)) >>  1));
  BX_INFO(("             DCS: %d (%d)", (context[0] & (1     << 0)) >>  0));
}

void bx_usb_xhci_c::copy_slot_from_buffer(struct SLOT_CONTEXT *slot_context, const Bit8u *buffer)
{
  Bit32u *buffer32 = (Bit32u *) buffer;

  slot_context->entries =          (buffer32[0] >> 27);
  slot_context->hub =              (buffer32[0] & (1<<26)) ? 1 : 0;
  slot_context->mtt =              (buffer32[0] & (1<<25)) ? 1 : 0;
  slot_context->speed =            (buffer32[0] & (0x0F<<20)) >> 20;
  slot_context->route_string =     (buffer32[0] & 0x000FFFFF);
  slot_context->num_ports =        (buffer32[1] >> 24);
  slot_context->rh_port_num =      (buffer32[1] & (0xFF<<16)) >> 16;
  slot_context->max_exit_latency = (buffer32[1] & 0xFFFF);
  slot_context->int_target =       (buffer32[2] >> 22);
  slot_context->ttt =              (buffer32[2] & (0x3<<16)) >> 16;
  slot_context->tt_port_num =      (buffer32[2] & (0xFF<<8)) >> 8;
  slot_context->tt_hub_slot_id =   (buffer32[2] & 0xFF);
  slot_context->slot_state =       (buffer32[3] >> 27);
  slot_context->device_address =   (buffer32[3] & 0xFF);
}

void bx_usb_xhci_c::copy_ep_from_buffer(struct EP_CONTEXT *ep_context, const Bit8u *buffer)
{
  Bit32u *buffer32 = (Bit32u *) buffer;

  ep_context->interval =           (buffer32[0] & (0xFF<<16)) >> 16;
  ep_context->lsa =                (buffer32[0] & (1<<15)) ? 1 : 0;
  ep_context->max_pstreams =       (buffer32[0] & (0x1F<<10)) >> 10;
  ep_context->mult =               (buffer32[0] & (0x03<<8)) >> 8;
  ep_context->ep_state =           (buffer32[0] & 0x07);
  ep_context->max_packet_size =    (buffer32[1] >> 16);
  ep_context->max_burst_size =     (buffer32[1] & (0xFF<<8)) >> 8;
  ep_context->hid =                (buffer32[1] & (1<<7)) ? 1 : 0;
  ep_context->ep_type =            (buffer32[1] & (0x07<<3)) >> 3;
  ep_context->cerr =               (buffer32[1] & (0x03<<1)) >> 1;
  ep_context->tr_dequeue_pointer = (buffer32[2] & (Bit64u) ~0xF) | ((Bit64u) buffer32[3] << 32);
  ep_context->dcs =                (buffer32[2] & (1<<0));
  ep_context->max_esit_payload =   (buffer32[4] >> 16);
  ep_context->average_trb_len =    (buffer32[4] & 0xFFFF);
}

void bx_usb_xhci_c::copy_slot_to_buffer(Bit32u *buffer32, const int slot)
{
  buffer32[0] = (BX_XHCI_THIS hub.slots[slot].slot_context.entries << 27) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.hub << 26) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.mtt << 25) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.speed << 20) |
                 BX_XHCI_THIS hub.slots[slot].slot_context.route_string;
  buffer32[1] = (BX_XHCI_THIS hub.slots[slot].slot_context.num_ports << 24) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.rh_port_num << 16) |
                 BX_XHCI_THIS hub.slots[slot].slot_context.max_exit_latency;
  buffer32[2] = (BX_XHCI_THIS hub.slots[slot].slot_context.int_target << 22) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.ttt << 16) |
                (BX_XHCI_THIS hub.slots[slot].slot_context.tt_port_num << 8) |
                 BX_XHCI_THIS hub.slots[slot].slot_context.tt_hub_slot_id;
  buffer32[3] = (BX_XHCI_THIS hub.slots[slot].slot_context.slot_state << 27) |
                 BX_XHCI_THIS hub.slots[slot].slot_context.device_address;
}

void bx_usb_xhci_c::copy_ep_to_buffer(Bit32u *buffer32, const int slot, const int ep)
{
  buffer32[0] = (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.interval << 16) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.lsa << 15) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams << 10) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.mult << 8) |
                 BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_state;
  buffer32[1] = (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_packet_size << 16) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_burst_size << 8) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.hid << 7) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.ep_type << 3) |
                (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.cerr << 1);
  buffer32[2] = ((Bit32u)BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer) |
                 (Bit32u)BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.dcs;
  buffer32[3] =  (Bit32u)(BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.tr_dequeue_pointer >> 32);
  buffer32[4] = (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_esit_payload << 16) |
                 BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.average_trb_len;
}

void bx_usb_xhci_c::copy_stream_from_buffer(struct STREAM_CONTEXT *context, const Bit8u *buffer)
{
  Bit32u *buffer32 = (Bit32u *) buffer;

  context->tr_dequeue_pointer = (buffer32[0] & (Bit64u) ~0xF) | ((Bit64u) buffer32[1] << 32);
  context->dcs =                (buffer32[0] & (1<<0));
  context->sct =               ((buffer32[0] & (7<<1)) >> 1);
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  context->stopped_EDTLA =      (buffer32[2] & 0x00FFFFFF);
#endif
}

void bx_usb_xhci_c::copy_stream_to_buffer(Bit8u *buffer, const struct STREAM_CONTEXT *context)
{
  Bit32u *buffer32 = (Bit32u *) buffer;

  buffer32[0] = (Bit32u) context->tr_dequeue_pointer | (((Bit32u) context->sct) << 1) | (Bit32u) context->dcs;
  buffer32[1] = (Bit32u)(context->tr_dequeue_pointer >> 32);
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  buffer[2] = context->stopped_EDTLA;
#endif
}

// Validate a slot context
// specs 1.0: sect 6.2.2.2 (p321):
// specs 1.0: sect 6.2.2.3 (p321):
//  "A 'valid' Input Slot Context for an Evaluate Context Command requires the Interrupter Target and 
//   Max Exit Latency fields to be initialized."
int bx_usb_xhci_c::validate_slot_context(const struct SLOT_CONTEXT *slot_context, const int trb_command, const int slot)
{
  int ret = TRB_SUCCESS;
  unsigned MaxIntrs;
  //int speed = -1;
  //int port_num = slot_context->rh_port_num - 1;
  
  //if ((port_num >= 0) && (BX_XHCI_THIS hub.usb_port[port_num].device != NULL)) {
  //  speed = BX_XHCI_THIS hub.usb_port[port_num].device->get_speed();
  //} else {
  //  BX_ERROR(("Validate Slot Context: Invalid port_num (%d) sent.", port_num));
  //  return PARAMETER_ERROR;
  //}
  
  switch (trb_command) {
    case ADDRESS_DEVICE:
    case EVALUATE_CONTEXT:
      // valid values for the Interrupt Target are 0 to MaxIntrs-1, inclusive.
      MaxIntrs = (BX_XHCI_THIS hub.cap_regs.HcSParams1 & (0x7FF << 8)) >> 8;
      if (slot_context->int_target > MaxIntrs)
        ret = PARAMETER_ERROR;
      
      // all high-speed and lower devices must have a Max Exit Latency value of zero
      // (this will fail because 'rh_port_num' hasn't been initialized yet, so 'speed' will be -1)
      //if ((slot_context->max_exit_latency > 0) && (speed < USB_SPEED_SUPER))
      //  ret = PARAMETER_ERROR;
      
      if (ret != TRB_SUCCESS) {
        BX_ERROR(("Validate Slot Context: int_target = %d (0 -> %d), slot_context->max_exit_latency = %d", 
          slot_context->int_target, MaxIntrs, slot_context->max_exit_latency));
      }
      break;
      
    case CONFIG_EP:
      // The config command needs to check the Context Entries field
      if (slot_context->entries < BX_XHCI_THIS hub.slots[slot].slot_context.entries)
        ret = PARAMETER_ERROR;
      
      // if 'hub' is 1 and device is a high-speed device, then the 'tt think time', 'multi-tt',
      //  and 'number of ports' fields must be initialized.
      // (this will fail because 'rh_port_num' hasn't been initialized yet, so 'speed' will be -1)
      //if (slot_context->hub) {
      //  if ((speed == USB_SPEED_HIGH) && (slot_context->ttt == 0) || (slot_context->mtt == 0)) {
      //    ret = PARAMETER_ERROR;
      //  }
      //} else {
      //  if (slot_context->num_ports != 0) {
      //    ret = PARAMETER_ERROR;
      //  }
      //}
      
      if (ret != TRB_SUCCESS) {
        BX_ERROR(("Validate Slot Context: entry count = %d (%d), hub = %d", 
          slot_context->entries, BX_XHCI_THIS hub.slots[slot].slot_context.entries,
          slot_context->hub));
      }
      break;
  }

  return ret;
}

// xHCI, section 6.2.3.2, p326
// The controller can return a valid EP Context even though the MPS is less than
//  the specified amount.  Win7 uses this to detect the actual EP0's max packet size...
//  For example, the descriptor might return 64, but the device really only handles
//   8-byte control transfers.
int bx_usb_xhci_c::validate_ep_context(const struct EP_CONTEXT *ep_context, const int trb_command, const Bit32u a_flags, int port_num, int ep_num)
{
  int ret = TRB_SUCCESS;
  unsigned int mps = 0;
  int speed = -1;

  // get the device's speed
  if ((port_num >= 0) && (BX_XHCI_THIS hub.usb_port[port_num].device != NULL)) {
    speed = BX_XHCI_THIS hub.usb_port[port_num].device->get_speed();
  } else {
    BX_ERROR(("Validate EP Context: Invalid port_num (%d) sent.", port_num));
    return PARAMETER_ERROR;
  }

  // get the default max packet size for the device
  switch (speed) {
    case USB_SPEED_LOW:
      mps = 8;
      break;
    case USB_SPEED_FULL:
    case USB_SPEED_HIGH:
      mps = 64;
      break;
    case USB_SPEED_SUPER:
      mps = 512;
      break;
  }

  switch (trb_command) {
    case ADDRESS_DEVICE:
      if (ep_num == 1) {
        // 1) the EP Type field = Control,
        if (ep_context->ep_type != 4)
          ret = PARAMETER_ERROR;
        
        // 2) the values of the Max Packet Size, Max Burst Size, and the Interval are considered
        //    within range for endpoint type and the speed of the device,
        if (ep_context->max_packet_size != mps)
          ret = PARAMETER_ERROR;
        if (ep_context->interval > 15) // The legal range of 0 to 15 for all (non full/low-speed interrupt) endpoint types
          ret = PARAMETER_ERROR;
        
        // 3) the TR Dequeue Pointer field points to a valid Transfer Ring,
        if (ep_context->tr_dequeue_pointer == 0)
          ret = PARAMETER_ERROR;
        
        // 4) the DCS field = 1,
        if (ep_context->dcs != 1)
          ret = PARAMETER_ERROR;
        
        // 5) the MaxPStreams field = 0, and
        if (ep_context->max_pstreams != 0)
          ret = PARAMETER_ERROR;
        
        // 6) all other fields are within the valid range of values.
        
        // The Max Burst Size, and EP State values shall be cleared to 0.
        if ((ep_context->max_burst_size != 0) || (ep_context->ep_state != 0))
          ret = PARAMETER_ERROR;
      }
      break;

    // section 6.2.3.3, p326, for an Evaluate Context Command, only the Slot and EP0 are evaluated.
    case EVALUATE_CONTEXT:
      if ((ep_num == 1) && (a_flags & 2)) {
        
        // check the max packet size field
        if (ep_context->max_packet_size != BX_XHCI_THIS hub.usb_port[port_num].device->get_mps(ep_num / 2))
          ret = PARAMETER_ERROR;
        
      }
      break;
      
    case CONFIG_EP:
      if ((ep_num > 1) && (a_flags & (1 << ep_num))) {
        // 1) the values of the Max Packet Size, Max Burst Size, and the Interval are considered within
        //    range for endpoint type and the speed of the device,
        if (ep_context->max_packet_size != BX_XHCI_THIS hub.usb_port[port_num].device->get_mps(ep_num / 2))
          ret = PARAMETER_ERROR;
        if (ep_context->max_burst_size != BX_XHCI_THIS hub.usb_port[port_num].device->get_max_burst_size(ep_num / 2))
          ret = PARAMETER_ERROR;
        if (ep_context->interval > 15) // The legal range of 0 to 15 for all (non full/low-speed interrupt) endpoint types
          ret = PARAMETER_ERROR;
        
        // 2) if MaxPStreams > 0, then the TR Dequeue Pointer field points to an array of valid 
        //    Stream Contexts, or if MaxPStreams = 0, then the TR Dequeue Pointer field points 
        //    to a Transfer Ring, 
        if (ep_context->tr_dequeue_pointer == 0)
          ret = PARAMETER_ERROR;
        
        // 3) the EP State field = Disabled, and
        if (ep_context->ep_state != 0)
          ret = PARAMETER_ERROR;
        
        // 4) all other fields are within their valid range of values.
        
      }
      break;
      
    default:
      BX_ERROR(("Error: Unknown command on Evaluate Context: %d", trb_command));
  }

  if (ret != TRB_SUCCESS) {
    BX_ERROR(("Validate Endpoint Context returned PARAMETER_ERROR."));
  }

  return ret;
}

// The Specs say that the address is only unique to the RH Port Number
//  For now, we simply return the slot number (+1);
int bx_usb_xhci_c::create_unique_address(const int slot)
{
  return (slot + 1);  // Windows may need the first one to be 2 (though it shouldn't know the difference for xHCI)
}

int bx_usb_xhci_c::send_set_address(const int addr, const int port_num, const int slot)
{
  int ret;
  USBPacket packet;
  static Bit8u setup_address[8] = { 0, 0x05, 0, 0, 0, 0, 0 };

  setup_address[2] = addr & 0xff;
  setup_address[3] = addr >> 8;

  packet.pid = USB_TOKEN_SETUP;
  packet.devep = 0;
  packet.speed = broadcast_speed(slot);
#if HANDLE_TOGGLE_CONTROL
  packet.toggle = -1; // the xHCI handles the data toggle bit for USB2 devices
#endif
  packet.devaddr = 0;  // default address
  packet.len = 8;
  packet.data = setup_address;
  packet.complete_cb = NULL;
  packet.complete_dev = BX_XHCI_THIS_PTR;
  ret = BX_XHCI_THIS broadcast_packet(&packet, port_num);
  if (ret == 0) {
    packet.pid = USB_TOKEN_IN;
    packet.len = 0;
    ret = BX_XHCI_THIS broadcast_packet(&packet, port_num);
  }
  return ret;
}

int bx_usb_xhci_c::broadcast_speed(const int slot)
{
  int ret = -1;
  
  switch (BX_XHCI_THIS hub.slots[slot].slot_context.speed) {
    case 1:
      ret = USB_SPEED_FULL;
      break;
    case 2:
      ret = USB_SPEED_LOW;
      break;
    case 3:
      ret = USB_SPEED_HIGH;
      break;
    case 4: case 5:
    case 6: case 7:
      ret = USB_SPEED_SUPER;
      break;
    default:
      BX_ERROR(("Invalid speed (%d) specified in Speed field of the Slot Context.", BX_XHCI_THIS hub.slots[slot].slot_context.speed));
  }
  
  return ret;
}

int bx_usb_xhci_c::broadcast_packet(USBPacket *p, const int port)
{
  int ret = USB_RET_NODEV;

  if (BX_XHCI_THIS hub.usb_port[port].device != NULL)
    ret = BX_XHCI_THIS hub.usb_port[port].device->handle_packet(p);

  return ret;
}

void bx_usb_xhci_c::xhci_timer_handler(void *this_ptr)
{
  bx_usb_xhci_c *class_ptr = (bx_usb_xhci_c *) this_ptr;
  class_ptr->xhci_timer();
}

// return the status of all the PSCEG (Port Status Change Event Generation) bits
Bit8u bx_usb_xhci_c::get_psceg(const int port)
{
  Bit8u ret;

  ret  = (BX_XHCI_THIS hub.usb_port[port].portsc.csc) ? PSCEG_CSC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.pec) ? PSCEG_PEC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.wrc) ? PSCEG_WRC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.occ) ? PSCEG_OCC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.prc) ? PSCEG_PRC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.plc) ? PSCEG_PLC : 0;
  ret |= (BX_XHCI_THIS hub.usb_port[port].portsc.cec) ? PSCEG_CEC : 0;

  return ret;
}

void bx_usb_xhci_c::xhci_timer(void)
{
  int slot, ep;
  unsigned int port;
  Bit8u new_psceg;

  if (BX_XHCI_THIS hub.op_regs.HcStatus.hch)
    return;

  /* Per section 4.19.3 of the xHCI 1.0 specs, we need to present
    *  a "Port Status Change Event".  
    * Also, we should only present this event once if any other bits 
    *  change, only presenting it again when all change bits are written 
    *  back to zero, and a change bit goes from 0 to 1.
    */
  for (port=0; port<BX_XHCI_THIS hub.n_ports; port++) {
    new_psceg = get_psceg(port);
    // if any bit has transitioned from 0 to 1 (in any port),
    //  set the pcd bit in the host status register.
    if ((new_psceg & BX_XHCI_THIS hub.usb_port[port].psceg) > 0)
      BX_XHCI_THIS hub.op_regs.HcStatus.pcd = 1;
    // clear any bits that have been cleared by the guest,
    //  if we are now zero *and* there are any new bits set, send a Change Event.
    BX_XHCI_THIS hub.usb_port[port].psceg &= new_psceg;
    if ((BX_XHCI_THIS hub.usb_port[port].psceg == 0) && (new_psceg != 0)) {
      BX_DEBUG(("Port #%d Status Change Event: (%2Xh)", port + 1, new_psceg));
      write_event_TRB(0, ((port + 1) << 24), TRB_SET_COMP_CODE(1), TRB_SET_TYPE(PORT_STATUS_CHANGE), 1);
    }
    BX_XHCI_THIS hub.usb_port[port].psceg |= new_psceg;
  }
  
  for (slot=1; slot<MAX_SLOTS; slot++) {
    if (BX_XHCI_THIS hub.slots[slot].enabled) {
      for (ep=1; ep<32; ep++) {
        if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry) {
          if (--BX_XHCI_THIS hub.slots[slot].ep_context[ep].retry_counter <= 0) {
            if (BX_XHCI_THIS hub.slots[slot].ep_context[ep].ep_context.max_pstreams > 0) {   // specifying streams
              //
              // ben: TODO:
              //
              BX_ERROR(("Retry on a streamed endpoint."));
              
            } else {
              BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer =
                BX_XHCI_THIS process_transfer_ring(slot, ep, BX_XHCI_THIS hub.slots[slot].ep_context[ep].enqueue_pointer,
                                                            &BX_XHCI_THIS hub.slots[slot].ep_context[ep].rcs, 0);
            }
          }
        }
      }
    }
  }
}

void bx_usb_xhci_c::runtime_config_handler(void *this_ptr)
{
  bx_usb_xhci_c *class_ptr = (bx_usb_xhci_c *) this_ptr;
  class_ptr->runtime_config();
}

void bx_usb_xhci_c::runtime_config(void)
{
  unsigned int i;
  char pname[6];

  for (i = 0; i < BX_XHCI_THIS hub.n_ports; i++) {
    // device change support
    if ((BX_XHCI_THIS device_change & (1 << i)) != 0) {
      if (!BX_XHCI_THIS hub.usb_port[i].portsc.ccs) {
        sprintf(pname, "port%d", i + 1);
        init_device(i, (bx_list_c *) SIM->get_param(pname, SIM->get_param(BXPN_USB_XHCI)));
      } else {
        set_connect_status(i, 0);
      }
      BX_XHCI_THIS device_change &= ~(1 << i);
    }
    // forward to connected device
    if (BX_XHCI_THIS hub.usb_port[i].device != NULL) {
      BX_XHCI_THIS hub.usb_port[i].device->runtime_config();
    }
  }
}

// pci configuration space write callback handler
void bx_usb_xhci_c::pci_write_handler(Bit8u address, Bit32u value, unsigned io_len)
{
  if (((address >= 0x14) && (address <= 0x34)))
    return;

  BX_DEBUG_PCI_WRITE(address, value, io_len);
  for (unsigned i=0; i<io_len; i++) {
    Bit8u value8 = (value >> (i*8)) & 0xFF;
//  Bit8u oldval = BX_XHCI_THIS pci_conf[address+i];
    switch (address + i) {
      case 0x04:
        value8 &= 0x06; // (bit 0 is read only for this card) (we don't allow port IO)
        BX_XHCI_THIS pci_conf[address+i] = value8;
        break;
      case 0x3d: //
      case 0x3e: //
      case 0x3f: //
      case 0x05: // disallowing write to command hi-byte
      case 0x06: // disallowing write to status lo-byte (is that expected?)
        break;
      case 0x54:
        if ((((value8 & 0x03) == 0x03) && ((BX_XHCI_THIS pci_conf[address+i] & 0x03) == 0x00)) &&
          (BX_XHCI_THIS hub.op_regs.HcCommand.rs || !BX_XHCI_THIS hub.op_regs.HcStatus.hch))
            BX_ERROR(("Power Transition from D0 to D3 with Run bit set and/or Halt bit clear"));
        BX_XHCI_THIS pci_conf[address+i] = value8;
        break;
      case 0x55:
        BX_XHCI_THIS pci_conf[address+i] = value8;
        if (value8 & 0x80)  // if we write a one to bit 7 (15 of word register at 0x54), clear bit 7.
          BX_XHCI_THIS pci_conf[address+i] &= 0x7F;
        break;
      default:
        BX_XHCI_THIS pci_conf[address+i] = value8;
    }
  }
}

bool bx_usb_xhci_c::set_connect_status(Bit8u port, bool connected)
{
  const bool ccs_org = BX_XHCI_THIS hub.usb_port[port].portsc.ccs;
  const bool ped_org = BX_XHCI_THIS hub.usb_port[port].portsc.ped;
  int otherportnum = BX_XHCI_THIS hub.paired_portnum[port];

  usb_device_c *device = BX_XHCI_THIS hub.usb_port[port].device;
  if (device != NULL) {
    if (connected) {
      // make sure the user has not tried to put a device on a paired port number
      // (this is invalid in all but external USB3 hubs)
      if (BX_XHCI_THIS hub.usb_port[otherportnum].portsc.ccs) {
        BX_PANIC(("Port #%d: Paired port number #%d already in use.", port + 1, otherportnum + 1));
        return 0;
      }
      if ((device->get_speed() == USB_SPEED_SUPER) &&
          !BX_XHCI_THIS hub.usb_port[port].is_usb3) {
        BX_PANIC(("Super-speed device not supported on USB2 port."));
        return 0;
      }
      if ((device->get_speed() < USB_SPEED_SUPER) &&
          BX_XHCI_THIS hub.usb_port[port].is_usb3) {
        BX_PANIC(("Non super-speed device not supported on USB3 port."));
        return 0;
      }
      if (BX_XHCI_THIS hub.usb_port[port].is_usb3) {
        if (!device->set_speed(USB_SPEED_SUPER)) {
          BX_PANIC(("Only super-speed devices supported on USB3 port."));
          return 0;
        }
      }
      switch (device->get_speed()) {
        case USB_SPEED_LOW:
          BX_XHCI_THIS hub.usb_port[port].portsc.speed = 2;
          break;
        case USB_SPEED_FULL:
          BX_XHCI_THIS hub.usb_port[port].portsc.speed = 1;
          break;
        case USB_SPEED_HIGH:
          BX_XHCI_THIS hub.usb_port[port].portsc.speed = 3;
          break;
        case USB_SPEED_SUPER:
          BX_XHCI_THIS hub.usb_port[port].portsc.speed = 4;
          break;
        default:
          BX_PANIC(("USB device returned invalid speed value"));
          return 0;
      }
      BX_XHCI_THIS hub.usb_port[port].portsc.ccs = 1;
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
        BX_XHCI_THIS hub.usb_port[port].portsc.ccs = 0;
        BX_XHCI_THIS hub.usb_port[port].portsc.ped = 0;
        BX_XHCI_THIS hub.usb_port[port].portsc.speed = 0;
        remove_device(port);
    }
    // did we change?
    if (ccs_org != BX_XHCI_THIS hub.usb_port[port].portsc.ccs)
      BX_XHCI_THIS hub.usb_port[port].portsc.csc = 1;
    if (ped_org != BX_XHCI_THIS hub.usb_port[port].portsc.ped)
      BX_XHCI_THIS hub.usb_port[port].portsc.pec = 1;
  }
  return connected;
}

// USB runtime parameter handler
Bit64s bx_usb_xhci_c::usb_param_handler(bx_param_c *param, bool set, Bit64s val)
{
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    bool empty = (val == 0);
    if ((portnum >= 0) && (portnum < (int) BX_XHCI_THIS hub.n_ports)) {
      if (empty && BX_XHCI_THIS hub.usb_port[portnum].portsc.ccs) {
        BX_XHCI_THIS device_change |= (1 << portnum);
      } else if (!empty && !BX_XHCI_THIS hub.usb_port[portnum].portsc.ccs) {
        BX_XHCI_THIS device_change |= (1 << portnum);
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
Bit64s bx_usb_xhci_c::usb_param_oc_handler(bx_param_c *param, bool set, Bit64s val)
{
  int portnum;

  if (set) {
    portnum = atoi((param->get_parent())->get_name()+4) - 1;
    if ((portnum >= 0) && (portnum < (int) BX_XHCI_THIS hub.n_ports)) {
      if (val) {
        if (BX_XHCI_THIS hub.usb_port[portnum].portsc.ccs) {
          BX_XHCI_THIS hub.usb_port[portnum].portsc.occ = 1;
          BX_XHCI_THIS hub.usb_port[portnum].portsc.oca = 1;
          BX_DEBUG(("Over-current signaled on port #%d.", portnum + 1));
          write_event_TRB(0, ((portnum + 1) << 24), TRB_SET_COMP_CODE(1), TRB_SET_TYPE(PORT_STATUS_CHANGE), 1);
        }
      }
    }
  }

  return 0; // clear the indicator for next time
}

// USB runtime parameter enable handler
bool bx_usb_xhci_c::usb_param_enable_handler(bx_param_c *param, bool en)
{
  int portnum = atoi((param->get_parent())->get_name()+4) - 1;
  if (en && (BX_XHCI_THIS hub.usb_port[portnum].device != NULL)) {
    en = 0;
  }
  
  return en;
}

void bx_usb_xhci_c::dump_xhci_core(const unsigned int slots, const unsigned int eps)
{
  bx_phy_address addr = BX_XHCI_THIS pci_bar[0].addr;
  Bit32u dword;
  Bit64u qword, slot_addr;
  unsigned int p, i;
  Bit8u buffer[4096];

  // dump the caps registers
  BX_INFO((" CAPLENGTH: 0x%02X", BX_XHCI_THIS hub.cap_regs.HcCapLength & 0xFF));
  BX_INFO(("HC VERSION: %X.%02X", ((BX_XHCI_THIS hub.cap_regs.HcCapLength & 0xFF000000) >> 24),
    ((BX_XHCI_THIS hub.cap_regs.HcCapLength & 0x00FF0000) >> 16)));
  BX_INFO(("HCSPARAMS1: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams1));
  BX_INFO(("HCSPARAMS2: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams2));
  BX_INFO(("HCSPARAMS3: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcSParams3));
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_INFO(("HCCPARAMS1: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams1));
#else
  BX_INFO(("HCCPARAMS: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams1));
#endif
  BX_INFO(("     DBOFF: 0x%08X", BX_XHCI_THIS hub.cap_regs.DBOFF));
  BX_INFO(("    RTSOFF: 0x%08X", BX_XHCI_THIS hub.cap_regs.RTSOFF));
#if ((VERSION_MAJOR == 1) && (VERSION_MINOR >= 0x10))
  BX_INFO(("HCCPARAMS2: 0x%08X", BX_XHCI_THIS hub.cap_regs.HcCParams2));
#endif

  // dump the operational registers
  BX_XHCI_THIS read_handler(addr + 0x20, 4, &dword, NULL);
  BX_INFO((" USB_COMMAND: 0x%08X", dword));
  BX_XHCI_THIS read_handler(addr + 0x24, 4, &dword, NULL);
  BX_INFO(("  USB_STATUS: 0x%08X", dword));
  BX_XHCI_THIS read_handler(addr + 0x28, 4, &dword, NULL);
  BX_INFO(("   PAGE_SIZE: 0x%08X", dword));
  BX_XHCI_THIS read_handler(addr + 0x34, 4, &dword, NULL);
  BX_INFO(("      DNCTRL: 0x%08X", dword));
  BX_XHCI_THIS read_handler(addr + 0x38, 8, &qword, NULL);
  BX_INFO(("        CRCR: 0x" FMT_ADDRX64, qword));
  BX_XHCI_THIS read_handler(addr + 0x50, 8, &qword, NULL);
  BX_INFO(("      DCBAAP: 0x" FMT_ADDRX64, qword));
  BX_XHCI_THIS read_handler(addr + 0x58, 4, &dword, NULL);
  BX_INFO(("      CONFIG: 0x%08X", dword));

  for (i=0, p=0; i<BX_XHCI_THIS hub.n_ports; i++, p+=16) {
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 0), 4, &dword, NULL);
    BX_INFO(("    Port %d: 0x%08X", i, dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 4), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 8), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
    BX_XHCI_THIS read_handler(addr + 0x420 + (p + 12), 4, &dword, NULL);
    BX_INFO(("            0x%08X", dword));
  }

  slot_addr = BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap;
  DEV_MEM_READ_PHYSICAL((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
  BX_INFO((" SCRATCH PADS:  0x" FMT_ADDRX64, slot_addr));
  for (i=1; i<slots+1; i++) {
    slot_addr = (BX_XHCI_THIS hub.op_regs.HcDCBAAP.dcbaap + (i * sizeof(Bit64u)));
    DEV_MEM_READ_PHYSICAL((bx_phy_address) slot_addr, sizeof(Bit64u), (Bit8u *) &slot_addr);
    DEV_MEM_READ_PHYSICAL_DMA((bx_phy_address) slot_addr, 2048, buffer);
    dump_slot_context((Bit32u *) &buffer[0], i);
    for (p=1; p<eps+1; p++)
      dump_ep_context((Bit32u *) &buffer[p * CONTEXT_SIZE], i, p);
  }
}
#endif // BX_SUPPORT_PCI && BX_SUPPORT_USB_XHCI
